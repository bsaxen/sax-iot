# =============================================
# File: heaterTwin.py
# Author: Benny Saxen
# Date: 2019-04-15
# Description: heater control algorithm
# 90 degrees <=> 1152/4 steps = 288
#
# Configuration:
# --- read data ---
# 0 water_out
# 1 water_in
# 2 smoke
# 3 target
# 4 need to heat
# 5 stepper position
#
# --- create actions ---
# stepper
# =============================================
import math
import urllib
import urllib2
import time
import datetime
from iotLib import *
confile = "heatertwin.conf"
version = 1
#=====================================================
class HeaterTwin:

   temperature_water_out_ix   = 0
   temperature_water_in_ix    = 1
   temperature_smoke_ix       = 2
   temperature_target_ix      = 3
   need_ix                    = 4
   stepper_pos_ix             = 5

   temperature_water_out      = 999
   temperature_water_in       = 999
   temperature_smoke          = 999
   temperature_target         = 999
   need                       = 999
   stepper_pos                = 999

   value         = []
   value_prev    = []

   inertia     = 0
   warmcool    = 0
   steps       = 0
#=====================================================
def show_action_bit_info(a):
    message = ''
    c = a & 1
    if c == 1:
        message +=  "|inertia_active "
    c = a & 2
    if c == 2:
        message +=  "|heater_is_off "
    c = a & 4
    if c == 4:
        message +=  "|no_need"
    c = a & 8
    if c == 8:
        message +=  "|no_cooling_possible"
    c = a & 16
    if c == 16:
        message +=  "|below_min_steps"
    c = a & 32
    if c == 32:
        message +=  "|stepper_zero"
    c = a & 64
    if c == 64:
        message +=  "|energy_limit_reached"
    c = a & 128
    if c == 128:
        message +=  "|stepper_max"
    print message
    return message
#=====================================================
def simulate(co,dy,ht):

    print ("In Mode: " + str(dy.mymode) + " State: " + str(dy.mystate))

    ndi = 0
    if ht.temperature_water_out == 999:
        message = "No data - temperature_water_out"
        lib_publishMyLog(co, message )
        ndi = ndi + 1
    if ht.temperature_water_in == 999:
        message = "No data - temperature_water_in"
        lib_publishMyLog(co, message )
        ndi = ndi + 1
    if ht.temperature_smoke == 999:
        message = "No data - temperature_smoke"
        lib_publishMyLog(co, message )
        ndi = ndi + 1
    if ht.temperature_target == 999:
        message = "No data - temperature_target"
        lib_publishMyLog(co, message )
        ndi = ndi + 1
    if ht.need == 999:
        message = "No data - need"
        lib_publishMyLog(co, message )
        ndi = ndi + 1
    if ht.stepper_pos == 999:
        message = "No data - stepper_pos"
        lib_publishMyLog(co, message )
        ndi = ndi + 1
	
    if ndi > 0:
        print ndi
    if ndi == 0:
        all_data_is_available = 1
    else:
        all_data_is_available = 0

    old_data = 0

    print ndi

    energy = float(ht.temperature_water_out) - float(ht.temperature_water_in)

    if dy.mymode == MODE_OFFLINE:
        if all_data_is_available == 1:
            dy.mymode = MODE_ONLINE
            message = 'MODE_ONLINE'
            lib_publishMyLog(co, message )

    if dy.mymode == MODE_ONLINE:
        if all_data_is_available == 0:
            dy.mymode = MODE_OFFLINE
            message = 'MODE_OFFLINE'
            lib_publishMyLog(co, message )

        if dy.mystate == STATE_OFF:
            ht.warmcool -= int(co.myperiod)
            if ht.warmcool < 0:
                ht.warmcool = 0
            if ht.temperature_smoke > float(co.minsmoke):
                dy.mystate = STATE_WARMING
                message = 'STATE_WARMING'
                lib_publishMyLog(co, message )

        if dy.mystate == STATE_WARMING:
            ht.warmcool += int(co.myperiod)
            if ht.warmcool > float(co.warmcool):
                ht.inertia = 0
                ht.warmcool = float(co.warmcool)
                dy.mystate = STATE_ON
                message = 'STATE_ON'
                lib_publishMyLog(co, message )
            if ht.temperature_smoke < float(co.minsmoke):
                action += 2
                dy.mystate = STATE_OFF
                ht.warmcool = 0
                message = 'STATE_OFF'
                lib_publishMyLog(co, message )

        action = 0
        if dy.mystate == STATE_ON:
            ht.inertia = int(ht.inertia) - int(co.myperiod)

            if ht.inertia < 0:
                ht.inertia = 0

            if ht.inertia > 0:
                action += 1

            if ht.temperature_smoke < float(co.minsmoke):
                action += 2
                dy.mystate = STATE_OFF
                ht.warmcool = 0
                message = 'STATE_OFF'
                lib_publishMyLog(co, message )

            tmp1 = float(ht.temperature_target)*float(co.relax)
            tmp2 = float(ht.temperature_water_out)*float(co.relax)
            tmp3 = tmp1 - tmp2
            ht.steps = round(tmp3)

            if ht.temperature_water_in > ht.temperature_water_out and ht.steps < 0:
                action += 8
            if abs(ht.steps) < int(co.minsteps):
                action += 16

            if energy > float(co.maxenergy) and int(ht.steps) > 0:
                action += 64

            if ht.steps < 0:
                direction = COUNTERCLOCKWISE
            if ht.steps > 0:
                direction = CLOCKWISE
		
            if direction == CLOCKWISE and ht.stepper_pos == 288:
                action += 32	
		
            if direction == COUNTERCLOCKWISE and ht.stepper_pos == 0:
                action += 128	
		
            if int(ht.need) == 0 and ht.steps > 0:
                action += 4
		
            if abs(ht.steps) > int(co.maxsteps):
                ht.steps = int(co.maxsteps)

            if action == 0 and dy.mystop == 0:
                dyn_inertia = int(60.0/float(ht.temperature_water_out))
                ht.inertia = int(int(co.inertia)/dyn_inertia)
                steps = abs(ht.steps)
                message = "Stepper_"+str(steps)+"_"+str(direction)
                lib_publishMyLog(co, message )
                #send message to stepper devices
                print "========= Stepper Move ====== " + str(direction) + ' steps=' + str(steps)
                message = "STEPPER"+','+str(direction)+','+str(steps)
		
                code = steps + direction*100
                lib_placeOrder(co.send_domain[0], co.myserver, co.send_device[0], code )
#========================================================================
    why = show_action_bit_info(action)
    payload  = '{\n'
    payload += '"mode"     : "' + str(dy.mymode) + '",\n'
    payload += '"state"    : "' + str(dy.mystate) + '",\n'
    payload += '"errors"   : "' + str(dy.myerrors) + '",\n'
    payload += '"stop"     : "' + str(dy.mystop) + '",\n'
    payload += '"inertia"  : "' + str(ht.inertia) + '",\n'
    payload += '"warmcool" : "' + str(ht.warmcool) + '",\n'
    payload += '"steps"    : "' + str(ht.steps) + '",\n'
    payload += '"action"   : "' + str(action) + '",\n'
    payload += '"why"      : "' + str(why) + '",\n'
    payload += '"energy"   : "' + str(energy) + '",\n'
    payload += '"need"     : "' + str(ht.need) + '",\n'
    payload += '"stepper_pos" : "' + str(ht.stepper_pos) + '",\n'
    payload += '"temperature_water_out" : "' + str(ht.temperature_water_out) + '",\n'
    payload += '"temperature_water_in"  : "' + str(ht.temperature_water_in) + '",\n'
    payload += '"temperature_smoke"     : "' + str(ht.temperature_smoke) + '",\n'
    payload += '"temperature_target"    : "' + str(ht.temperature_target) + '"\n'
    payload += '}\n'

    lib_publishMyPayload(co,dy,payload)
    msg = lib_publishMyDynamic(co,dy)

    if ":" in msg:
		p = msg.split(':')
		print msg
		q = p[1].split(",")
		m = len(q)
		if m == 1:
			if q[0] == 'stopcontrol':
				message = 'Stop control: '
				lib_publishMyLog(co, message )
				dy.mystop = 1
			if q[0] == 'startcontrol':
				message = 'Start control: '
				lib_publishMyLog(co, message )
				dy.mystop = 0
		if m == 2:
			if q[0] == 'period':
				co.myperiod = float(q[1])
				message = 'New Period: ' + str(co.myperiod)
				lib_publishMyLog(co, message )
    print ("Out Mode: " + str(dy.mymode) + " State: " + str(dy.mystate))
    return
#=====================================================
def getLatestValue(co,dy,ht,ix):
    ht.value_prev[ix] = ht.value[ix]
    error = lib_readData(co,ix)
    if error == 0:
        ht.value[ix] = co.myresult
        diff  = float(ht.value[ix]) - float(ht.value_prev[ix])
        if abs(diff) > 10 and ht.value_prev[ix] != 999:
            message = 'value error: cur=' + str(ht.value[ix]) + ' prev=' + str(ht.value_prev[ix] + ' ix=' + str(ix))
            lib_publishMyLog(co, message)
            ht.value[ix] = ht.value_prev[ix]
            dy.myerrors += 1

    return error

#===================================================
# Setup
#===================================================
ht = HeaterTwin()
lib_setup(co,confile,version)

if co.ndata != 6:
    print "Configuration missing c_data"
    exit()
if co.nsend != 1:
    print "Configuration missing c_send"
    exit()

for x in range(co.ndata):
    ht.value.append(999)
    ht.value_prev.append(999)

print "ndata=" + str(co.ndata)
print "nsend=" + str(co.nsend)

dy.mymode  = MODE_OFFLINE
dy.mystate = STATE_OFF

ht.inertia = 0
ht.warmcool = int(co.warmcool)*int(co.myperiod)
#===================================================
# Loop
#===================================================
while True:
    roger = 0
    lib_increaseMyCounter(co,dy)

    error = getLatestValue(co,dy,ht,ht.temperature_water_out_ix)
    if error == 0:
	ht.temperature_water_out = float(ht.value[ht.temperature_water_out_ix])
	print "water_out " + str(ht.temperature_water_out)
    else:
	roger = 1

    error = getLatestValue(co,dy,ht,ht.temperature_water_in_ix)
    if error == 0:
        ht.temperature_water_in = float(ht.value[ht.temperature_water_in_ix])
        print "water_in  " + str(ht.temperature_water_in)
    else:
	roger = 1

    error = getLatestValue(co,dy,ht,ht.temperature_smoke_ix)
    if error == 0:
        ht.temperature_smoke = float(ht.value[ht.temperature_smoke_ix])
        print "smoke     " + str(ht.temperature_smoke)
    else:
	roger = 1

    error = getLatestValue(co,dy,ht,ht.temperature_target_ix)
    if error == 0:
        ht.temperature_target = float(ht.value[ht.temperature_target_ix])
        print "target    " + str(ht.temperature_target)
    else:
	roger = 1

    error = getLatestValue(co,dy,ht,ht.need_ix)
    if error == 0:
        ht.need = int(ht.value[ht.need_ix])
        print "need    " + str(ht.need)
    else:
	roger = 1
	
    error = getLatestValue(co,dy,ht,ht.stepper_pos_ix)
    if error == 0:
        ht.stepper_pos = int(ht.value[ht.stepper_pos_ix])
        print "stepper_pos    " + str(ht.stepper_pos)
    else:
	roger = 1
	
    if roger == 0:
        simulate(co,dy,ht)
    else:
        message = 'ROGER_LOOP_ERROR'
        lib_publishMyLog(co, message)	

    time.sleep(float(co.myperiod))

#===================================================
# End of file
#===================================================
