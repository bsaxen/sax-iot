# =============================================
# File: appClient.py
# Author: Benny Saxen
# Date: 2019-04-24
# Description:
# =============================================
from appLib import *
#===================================================
# Setup
#===================================================
confile = "appclient.conf"
version = 1
lib_setup(co,confile,version)
#===================================================
# Loop
#===================================================
while True:
    
    lib_increaseMyCounter(co,dy)
    payload = '{"test":"10043","test2": "453"}'
    msg = lib_publishMychannel(co,dy,1,payload)
    message = 'counter:' + str(dy.mycounter)
    lib_publishMyLog(co, message)
    time.sleep(float(co.myperiod))
    
#===================================================
# End of file
#===================================================
