# =============================================
# File: httpClient.py
# Author: Benny Saxen
# Date: 2019-03-07
# Description:
# =============================================
from iotLib import *
#===================================================
# Setup
#===================================================
confile = "httpclient.conf"
lib_readConfiguration(confile,co)
lib_publishMyStatic(co)
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,dy)
    payload = '{}'
    msg = lib_publishMyDynamic(co,dy,payload)
    lib_common_action(co,msg)
    message = 'counter:' + str(dy.mycounter)
    lib_publishMyLog(co, message)
    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
