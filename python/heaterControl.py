# =============================================
# File: heaterControl.py
# Author: Benny Saxen
# Date: 2019-03-14
# Description: heater control algorithm
# 90 degrees <=> 1152/4 steps = 288
# =============================================
import math
import urllib
import urllib2
import time
import datetime
from iotLib import *

#=====================================================
class HeaterControl:
   bias    = 0.0
   need    = 1

   temperature_indoor    = 999
   temperature_outdoor   = 999

   temperature_indoor_prev    = 999
   temperature_outdoor_prev   = 999

   timeout_temperature_indoor    = 60
   timeout_temperature_outdoor   = 60
#=====================================================
def heater_model(co,dy,hc):
    mintemp = float(co.mintemp)
    maxtemp = float(co.maxtemp)
    minheat = float(co.minheat)
    maxheat = float(co.maxheat)
    x_0 = float(co.x_0)
    y_0 = float(co.y_0)

    y = 999

    coeff1 = (maxheat - y_0)/(mintemp - x_0)
    mconst1 = y_0 - coeff1*x_0
    coeff2 = (y_0 - minheat)/(x_0 - maxtemp)
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

    old_data = 0

    print ndi

    hc.timeout_temperature_indoor -= 1
    hc.timeout_temperature_outdoor -= 1

    if hc.timeout_temperature_indoor < 1:
	message = "Old data - temperature_indoor " + str(hc.timeout_temperature_indoor)
	old_data= 1
        lib_publishMyLog(co, message )

    if hc.timeout_temperature_outdoor < 1:
	message = "Old data - temperature_outdoor " + str(hc.timeout_temperature_outdoor)
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
            if dy.mystop == 0:
                dy.mystate = STATE_ON
                message = 'STATE_ON'
                lib_publishMyLog(co, message )

        if dy.mystate == STATE_ON:
            hc.need = 1
            if hc.temperature_indoor > 20:
		hc.need = 0
            if hc.temperature_indoor < hc.temperature_outdoor:
		hc.need = 0

            temp = hc.temperature_outdoor

            if temp > maxtemp:
                temp = maxtemp
            if temp < mintemp:
                temp = mintemp

            if temp < x_0:
                y = coeff1*temp + mconst1
            else:
                y = coeff2*temp + mconst2

            y = y +hc.bias
            if dy.mystop == 1:
                y = 999
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

    print payload
    msg = lib_publishMyDynamic(co,dy,payload)

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

    return

#===================================================
# Setup
#===================================================
hc = HeaterControl()

confile = "heatercontrol.conf"
lib_readConfiguration(confile,co)
lib_publishMyStatic(co)

dy.mymode = MODE_OFFLINE
dy.mystate = STATE_OFF
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,md)

    url = 'http://' + co.ds_uri[0] + '/' + co.ds_topic[0] + '/payload.json'
    print url
    hc.temperature_indoor_prev = hc.temperature_indoor
    hc.temperature_indoor = lib_readJsonPayload(url,'temp1')
    print hc.temperature_indoor
    diff  = float(hc.temperature_indoor) - float(hc.temperature_indoor_prev)
    if abs(diff) > 10 and hc.temperature_indoor_prev != 999:
      message = 'Temperature indoor error: cur=' + str(hc.temperature_indoor) + ' prev=' + str(hc.temperature_indoor_prev)
    	#publishLog(co, message )
    	hc.temperature_indoor = hc.temperature_indoor_prev
    	dy.myerrors += 1
    hc.timeout_temperature_indoor = 60

    url = 'http://' + co.ds_uri[1] + '/' + co.ds_topic[1] + '/payload.json'
    print url
    hc.temperature_outdoor_prev = hc.temperature_outdoor
    hc.temperature_outdoor = lib_readJsonPayload(url,'temp1')
    print hc.temperature_outdoor
    diff  = float(hc.temperature_outdoor) - float(hc.temperature_outdoor_prev)
    if abs(diff) > 10 and hc.temperature_outdoor_prev != 999:
      message = 'Temperature outdoor error: cur=' + str(hc.temperature_outdoor) + ' prev=' + str(hc.temperature_outdoor_prev)
    	#gowPublishLog(co, message )
    	hc.temperature_outdoor = hc.temperature_outdoor_prev
    	dy.myerrors += 1
    hc.timeout_temperature_outdoor = 60

    heater_model(co,md,hc)
    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    time.sleep(float(co.myperiod))

#===================================================
# End of file
#===================================================
