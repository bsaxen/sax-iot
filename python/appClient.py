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
lib_matrix(co)
#
#       app1  app2    app3
# app1   -    temp1   temp1
# app2
# app3 
#===================================================
# Loop
#===================================================
while True:
    
    lib_increaseMyCounter(co,dy)
    
    payload = '{"channel":"1","test": "123"}'
    msg = lib_publishMyChannel(co,dy,1,payload)
    
    payload = '{"channel":"2","test": "321"}'
    msg = lib_publishMyChannel(co,dy,2,payload)
    
    
    
    message = 'counter:' + str(dy.mycounter)
    lib_publishMyLog(co, message)
    time.sleep(float(co.myperiod))
    
#===================================================
# End of file
#===================================================
