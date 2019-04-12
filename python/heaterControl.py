# =============================================
# File: heaterControl.py
# Author: Benny Saxen
# Date: 2019-04-12
# Description: heater control algorithm
# 90 degrees <=> 1152/4 steps = 288
# Configuration:
#c_data         iot.simuino.com 60_01_94_17_88_05 payload temp2
#c_data         iot.simuino.com 60_01_94_17_88_05 payload temp1
# =============================================
import math
import urllib
import urllib2
import time
import datetime
from iotLib import *

#=====================================================
class HeaterControl:

    temperature_indoor_ix     = 0
    temperature_outdoor_ix    = 1

    value         = []
    value_prev    = []

    bias  = 0.0
    need  = 1

    temperature_indoor    = 999
    temperature_outdoor   = 999
#=====================================================
def control_algorithm(co,dy,hc):
    mintemp = float(co.mintemp)
    maxtemp = float(co.maxtemp)
    minheat = float(co.minheat)
    maxheat = float(co.maxheat)
    x_0 = float(co.x_0)
    y_0 = float(co.y_0)

    y = float(30)

    coeff1  = float((maxheat - y_0)/(mintemp - x_0))
    mconst1 = y_0 - coeff1*x_0
    coeff2  = (y_0 - minheat)/(x_0 - maxtemp)
    mconst2 = minheat - coeff2*maxtemp

    ndi = 0
    if hc.temperature_outdoor == 999:
        message = "No data - temperature_outdoor"
        lib_publishMyLog(co, message )
        ndi = ndi + 1
    if hc.temperature_indoor == 999:
        message = "No data - temperature_indoor"
        lib_publishMyLog(co, message )
        ndi = ndi + 1

    if ndi > 0:
        print ndi
    if ndi == 0:
        all_data_is_available = 1
    else:
        all_data_is_available = 0

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
            if dy.mystop == 0:
                dy.mystate = STATE_ON
                message = 'STATE_ON'
                lib_publishMyLog(co, message )

        if dy.mystate == STATE_ON:
		
            if dy.mystop == 1:
                dy.mystate = STATE_OFF
                message = 'STATE_OFF'
                lib_publishMyLog(co, message )
		
	    hc.need = 1
            if float(hc.temperature_indoor) > 20.0:
                hc.need = 0
            if float(hc.temperature_indoor) < float(hc.temperature_outdoor):
                hc.need = 0

            temp = float(hc.temperature_outdoor)

            if temp > maxtemp:
                temp = maxtemp
            if temp < mintemp:
                temp = mintemp

            if temp < x_0:
                y = coeff1*temp + mconst1
            else:
                y = coeff2*temp + mconst2

            y = y + hc.bias
#========================================================================
    payload  = '{\n'
    payload += '"mintemp" : "' + str(co.mintemp) + '",\n'
    payload += '"maxtemp" : "' + str(co.maxtemp) + '",\n'
    payload += '"minheat" : "' + str(co.minheat) + '",\n'
    payload += '"maxheat" : "' + str(co.maxheat) + '",\n'
    payload += '"x_0" : "' + str(co.x_0) + '",\n'
    payload += '"y_0" : "' + str(co.y_0) + '",\n'
    payload += '"need" : "' + str(hc.need) + '",\n'
    payload += '"target" : "' + str(y) + '",\n'
    payload += '"mode" : "' + str(dy.mymode) + '",\n'
    payload += '"state" : "' + str(dy.mystate) + '",\n'
    payload += '"errors" : "' + str(dy.myerrors) + '",\n'
    payload += '"stop" : "' + str(dy.mystop) + '",\n'
    payload += '"bias" : "' + str(hc.bias) + '",\n'
    payload += '"temperature_outdoor" : "' + str(hc.temperature_outdoor) + '",\n'
    payload += '"temperature_indoor" : "' + str(hc.temperature_indoor) + '"\n'
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
				hc.bias = float(q[1])
				message = 'Bias: ' + str(hc.bias)
				lib_publishMyLog(co, message )
			if q[0] == 'period':
				co.myperiod = float(q[1])
				message = 'New Period: ' + str(co.myperiod)
				lib_publishMyLog(co, message )
    return
#=====================================================
def getLatestValue(co,dy,hc,ix):
    hc.value_prev[ix] = hc.value[ix]
    error = lib_readData(co,ix)
    if error == 0:
        hc.value[ix] = co.myresult
        diff  = float(hc.value[ix]) - float(hc.value_prev[ix])
        if abs(diff) > 10 and hc.value_prev[ix] != 999:
            message = 'value error: cur=' + str(hc.value[ix]) + ' prev=' + str(hc.value_prev[ix] + ' ix=' + str(ix))
            lib_publishMyLog(co, message)
            hc.value[ix] = hc.value_prev[ix]
            dy.myerrors += 1
		
    return error
#===================================================
# Setup
#===================================================
hc = HeaterControl()

confile = "heatercontrol.conf"
version = 1
lib_setup(co,confile,version)

if co.ndata != 2:
    print "Configuration missing c_data"

if co.ndata  > 0:
    for x in range(co.ndata):
        hc.value.append(999)
        hc.value_prev.append(999)

print "ndata=" + str(co.ndata)
dy.mymode  = MODE_OFFLINE
dy.mystate = STATE_OFF
#===================================================
# Loop
#===================================================
while True:
    roger = 0
    lib_increaseMyCounter(co,dy)

    error = getLatestValue(co,dy,hc,hc.temperature_indoor_ix)
    if error == 0:
	hc.temperature_indoor = hc.value[hc.temperature_indoor_ix]
        print "temperature_indoor  " + str(hc.temperature_indoor)
    else:
	roger = 1

    error = getLatestValue(co,dy,hc,hc.temperature_outdoor_ix)
    if error == 0:
        hc.temperature_outdoor = hc.value[hc.temperature_outdoor_ix]
        print "temperature_outdoor " + str(hc.temperature_outdoor)
    else:
	roger = 1

    if roger == 0:
        control_algorithm(co,dy,hc)
    else:
        message = 'ROGER_LOOP_ERROR'
        lib_publishMyLog(co, message)	


    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
