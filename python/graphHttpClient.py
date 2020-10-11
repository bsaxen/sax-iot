# =============================================
# File.......: graphHttpClient.py
# Author.....: Benny Saxen
# Date.......: 2020-10-10
# Description:
# =============================================
import urllib.request
import urllib.parse
import time

g_tot = 0
g_counter = 1
g_url = ''

#===================================================
# Configuration
#===================================================
c_id     =   8001
c_server =   'http://localhost:7777/graphGateway.php'
c_period =   10
c_max_counter = 999999
g_url   +=   c_server + '?id='+str(c_id)+'&counter='+str(g_counter)+'&period='+str(c_period)
#===================================================
def setParameter(par,value):
#===================================================
    global g_url,g_tot

    if g_tot < par:
        g_tot = par

    g_url += '&p'+str(par)+'='+value
    return
#===================================================
def sendRequest():
#===================================================
    global g_url,g_tot,g_counter
    global c_server,c_id,c_period
    g_url += '&tot='+str(g_tot)
    print(g_url)
    f = urllib.request.urlopen(g_url)
    response = f.read().decode('utf-8')
    print(response)
    g_url = c_server + '?id='+str(c_id)+'&counter='+str(g_counter)+'&period='+str(c_period)
    return
#===================================================
# Setup
#===================================================

#===================================================
# Loop
#===================================================
while True:
    g_counter += 1
    if g_counter > c_max_counter:
        g_counter = 1
    setParameter(1,str(10))
    setParameter(2,str(12))
    sendRequest()
    time.sleep(c_period)
#===================================================
# End of file
#===================================================
