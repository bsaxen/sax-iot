# =============================================
# File: nilmTwin.py
# Author: Benny Saxen
# Date: 2019-04-12
# Description:
# =============================================
from iotLib import *
#===================================================
# Setup
#===================================================
confile = "nilmtwin.conf"
version = 1
lib_setup(co,confile,version)
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,dy)

    msg = lib_publishMyDynamic(co,dy)

    error = getLatestValue(co,ds,hc,hc.temperature_outdoor_ix)
    if error == 0:
        hc.temperature_outdoor = hc.value[hc.temperature_outdoor_ix]
        print "temperature_outdoor " + str(hc.temperature_outdoor)
    else:
	  roger = 1

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

    msg = lib_publishMyPayload(co,dy,payload)

    lib_commonAction(co,msg)

    message = 'counter:' + str(dy.mycounter)
    lib_publishMyLog(co, message)

    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
