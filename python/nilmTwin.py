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

missed = 0
kwh = 0

error = lib_readStaticParam(co,domain,device,'period')
if error == 0:
    period = int(co.myresult)
    print "period="+str(period)
 else:
    print "Not able to read period " + str(error)
    exit()
#===================================================
# Loop
#===================================================
while True:
    roger = 0
	
    lib_increaseMyCounter(co,dy)

    msg = lib_publishMyDynamic(co,dy)

    prev_counter = counter
    error = lib_readData(co,ds,0)
    if error == 0:
        counter = co.myresult
    else:
	print "Error reading counter"
    
    diff = counter  - prev_counter
    if diff == 1:
       error = lib_readData(co,ds,1)
       if error == 0:
          pulses = co.myresult
	  kwh += pulses
	  roger = 1
       else:
	   print "Error reading pulses"
    else diff > 1:
	print "Missed information: " + str(diff)
	missed += 1
    else:
	print "Information sync problem: " + str(diff)
    
	
    payload  = '{\n'
    payload += '"pulses": "'  + str(pulses)  + '",\n'
    payload += '"counter": "' + str(counter) + '",\n'
    payload += '"missed": "'  + str(missed)  + '",\n'
    payload += '"kwh": "'     + str(123)     + '"\n'
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
