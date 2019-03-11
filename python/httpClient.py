# =============================================
# File: httpClient.py
# Author: Benny Saxen
# Date: 2019-03-11
# Description:
version = 1
# =============================================
from iotLib import *
#===================================================
# Setup
#===================================================
confile = "httpclient.conf"
lib_readConfiguration(confile,co)
lib_publishMyStatic(co)
co.mysw = version
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,dy)

    msg = lib_publishMyDynamic(co,dy)

    payload = '{"test":"10043","test2": "453"}'
    print payload
    msg = lib_publishMyPayload(co,dy,payload)

    lib_common_action(co,msg)

    message = 'counter:' + str(dy.mycounter)
    lib_publishMyLog(co, message)

    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
