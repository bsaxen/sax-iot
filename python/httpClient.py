# =============================================
# File: httpClient.py
# Author: Benny Saxen
# Date: 2019-03-18
# Description:
# =============================================
from iotLib import *
#===================================================
# Setup
#===================================================
confile = "httpclient.conf"
version = 1
lib_setup(co,confile,version)
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,dy)

    msg = lib_publishMyDynamic(co,dy)

    payload = '{"test":"10043","test2": "453"}'
    print payload
    msg = lib_publishMyPayload(co,payload)

    lib_commonAction(co,msg)

    message = 'counter:' + str(dy.mycounter)
    lib_publishMyLog(co, message)

    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
