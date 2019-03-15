# =============================================
# File: heaterTwin.py
# Author: Benny Saxen
# Date: 2019-03-15
# Description: heater control algorithm
# 90 degrees <=> 1152/4 steps = 288
#
# Configuration:
# 1 water_out
# 2 water_in
# 3 smoke
# 4 target
#
# c_data       iot.simuino.com A0_20_A6_10_3C_36 payload temp2
# c_data       iot.simuino.com A0_20_A6_10_3C_36 payload temp1
# c_data       iot.simuino.com A0_20_A6_10_3C_36 payload temp1
# c_data       iot.simuino.com A0_20_A6_10_3C_36 payload temp1
# =============================================
import math
import urllib
import urllib2
import time
import datetime
from iotLib import *

#=====================================================
class HeaterTwin:

   temperature_water_out_ix   = 0
   temperature_water_in_ix    = 1
   temperature_smoke_ix       = 2
   temperature_target_ix      = 3

   temperature_water_out    = 999
   temperature_water_in     = 999
   temperature_smoke        = 999
   temperature_target       = 999

   value         = []
   value_prev    = []
   value_timeout = []

   inertia  = 0
   warmcool = 0
   steps = 0

#=====================================================
def simulate(co,dy,ht):

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

    if ndi > 0:
        print ndi
    if ndi == 0:
        all_data_is_available = 1
    else:
        all_data_is_available = 0

    old_data = 0

    print ndi

    for x in range(co.ndata):
        ht.value_timeout[x] -= 1
        if ht.value_timeout[x] < 1:
            message = "Old data index=" + str(x)
            old_data= 1
            lib_publishMyLog(co, message )

    if dy.mymode == MODE_OFFLINE:
	if all_data_is_available == 1 and old_data == 0:
	    dy.mymode = MODE_ONLINE
        message = 'MODE_ONLINE'
        lib_publishMyLog(co, message )

    if dy.mymode == MODE_ONLINE:
        if old_data == 1:
            dy.mymode = MODE_OFFLINE
            message = 'MODE_OFFLINE'
            lib_publishMyLog(co, message )

        if dy.mystate == STATE_OFF:
            ht.warmcool -= 1
            if ht.warmcool < 0:
                ht.warmcool = 0
            if ht.temperature_smoke > co.minsmoke:
                dy.mystate = STATE_WARMING
                message = 'STATE_WARMING'
                lib_publishMyLog(co, message )

        if dy.mystate == STATE_ON:

            ht.steps = 10
#========================================================================
    payload  = '{\n'
    payload += '"mode" : "' + str(dy.mymode) + '",\n'
    payload += '"state" : "' + str(dy.mystate) + '",\n'
    payload += '"errors" : "' + str(dy.myerrors) + '",\n'
    payload += '"stop" : "' + str(dy.mystop) + '",\n'
    payload += '"inertia" : "' + str(ht.inertia) + '",\n'
    payload += '"warmcool" : "' + str(ht.warmcool) + '",\n'
    payload += '"steps" : "' + str(ht.steps) + '",\n'
    payload += '"temperature_water_out" : "' + str(ht.temperature_water_out) + '",\n'
    payload += '"temperature_water_in" : "' + str(ht.temperature_water_in) + '",\n'
    payload += '"temperature_smoke" : "' + str(ht.temperature_smoke) + '",\n'
    payload += '"temperature_target" : "' + str(ht.temperature_target) + '"\n'
    payload += '}\n'

    lib_publishMyPayload(co,dy,payload)
    msg = lib_publishMyDynamic(co,dy)

    if ":" in msg:
		p = msg.split(':')
		#print p[1]
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
			if q[0] == 'bias':
				ht.bias = float(q[1])
				message = 'Bias: ' + str(ht.bias)
				lib_publishMyLog(co, message )

    return
#=====================================================
def getLatestValue(co,dy,ht,ix):
    ht.value_prev[ix] = ht.value[ix]
    ht.value[ix] = lib_readData(co,ds,ix)
    diff  = float(ht.value[ix]) - float(ht.value_prev[ix])
    if abs(diff) > 10 and ht.value_prev[ix] != 999:
        message = 'value error: cur=' + str(ht.value[ix]) + ' prev=' + str(ht.value_prev[ix] + ' ix=' + str(ix))
        lib_publishMyLog(co, message)
        ht.value[ix] = ht.value_prev[ix]
        dy.myerrors += 1

    ht.value_timeout[ix] = 60

    return ht.value[ix]

#===================================================
# Setup
#===================================================
ht = HeaterTwin()

confile = "heatertwin.conf"
lib_readConfiguration(confile,co)
lib_publishMyStatic(co)

for x in range(co.ndata):
    ht.value.append(999)
    ht.value_prev.append(999)
    ht.value_timeout.append(60)

print "ndata=" + str(co.ndata)
dy.mymode  = MODE_OFFLINE
dy.mystate = STATE_OFF

ht.inertia = 0
ht.warmcool = int(co.warmcool)
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,dy)

    res = getLatestValue(co,ds,ht,ht.temperature_water_out_ix)
    print " water_out" + str(res)
    ht.temperature_water_out = res

    res = getLatestValue(co,ds,ht,ht.temperature_water_in_ix)
    print "water_in" + str(res)
    ht.temperature_water_in = res

    res = getLatestValue(co,ds,ht,ht.temperature_smoke_ix)
    print "smoke" + str(res)
    ht.temperature_smoke = res

    res = getLatestValue(co,ds,ht,ht.temperature_target_ix)
    print "target" + str(res)
    ht.temperature_target = res

    simulate(co,dy,ht)
    #print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    time.sleep(float(co.myperiod))

#===================================================
# End of file
#===================================================
