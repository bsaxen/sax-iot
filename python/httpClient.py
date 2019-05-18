# =============================================
# File: httpClient.py
# Author: Benny Saxen
# Date: 2019-05-18
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
    lib_loop(co,dy)   
    payload = '{"test":"10043","test2": "453"}'
    print payload
    msg = lib_publishMyPayload(co,payload)
    lib_commonAction(co,msg)
#===================================================
# End of file
#===================================================
