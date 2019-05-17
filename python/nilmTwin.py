# =============================================
# File: nilmTwin.py
# Author: Benny Saxen
# Date: 2019-05-17
# Description:
# Nilm Station 1
# data 0 = nilm period
# data 1 = nilm counter
# data 2 = nilm pulses
# Nilm Station 2
# data 3 = nilm period
# data 4 = nilm counter
# data 5 = nilm pulses
# =============================================
from iotLib import *
#===================================================
# Setup
#===================================================
confile = "nilmtwin.conf"
version = 1
lib_setup(co,confile,version)
if co.ndata != 6:
   print("Configuration missmatch  c_data " + str(co.ndata))
   exit()

missed1  = 0
wh1     = 0
counter1 = 0
pulses1  = 0

missed2  = 0
wh2     = 0
counter2 = 0
pulses2  = 0

hour = str(datetime.datetime.now().strftime("%H"))
prev_hour = hour

error = lib_readData(co,0)
if error == 0:
   period1 = int(co.myresult)
else:
   print("Error reading period1")
   exit()

error = lib_readData(co,3)
if error == 0:
   period2 = int(co.myresult)
else:
   print("Error reading period2")
   exit()
#===================================================
# Loop
#===================================================
while True:
    roger = 0

    prev_hour = hour
    hour = str(datetime.datetime.now().strftime("%H"))
    if prev_hour == '23' and hour == '00':
       wh1 = 0
       wh2 = 0
    
    lib_increaseMyCounter(co,dy)

    msg = lib_publishMyDynamic(co,dy)
   #================================================== 
    prev_counter1 = counter1
    error = lib_readData(co,1)
    if error == 0:
       counter1 = int(co.myresult)
    else:
       print("Error reading counter1")

    diff = int(counter1)  - int(prev_counter1)
    if diff == 1:
       error = lib_readData(co,2)
       if error == 0:
          pulses1 = int(co.myresult)
          wh1 += int(pulses1)
          lib_writeMemory('wh1.txt',str(wh1));
          roger = 1
       else:
          print("Error reading pulses1")
    elif diff > 1:
       wh1 = int(lib_readMemory('wh1.txt'));
       print ("Missed information1: " + str(diff))
       missed1 += 1
       message = 'Missed information1' + str(diff)
       lib_publishMyLog(co, message)
    else:
       print("Information sync problem1: " + str(diff))
   #================================================== 
    prev_counter2 = counter2
    error = lib_readData(co,4)
    if error == 0:
       counter2 = int(co.myresult)
    else:
       print("Error reading counter2")
    
    diff = int(counter2)  - int(prev_counter2)
    if diff == 1:
       error = lib_readData(co,5)
       if error == 0:
          pulses2 = int(co.myresult)
          wh2 += int(pulses2)
          lib_writeMemory('wh2.txt',str(wh2));
          roger = 1
       else:
          print("Error reading pulses2")
    elif diff > 1:
       wh2 = int(lib_readMemory('wh2.txt'));
       print ("Missed information2: " + str(diff))
       missed2 += 1
       message = 'Missed information2' + str(diff)
       lib_publishMyLog(co, message)
    else:
       print("Information sync problem2: " + str(diff))
   #================================================== 
    payload  = '{\n'
    payload += '"pulses1": "'  + str(pulses1)  + '",\n'
    payload += '"counter1": "' + str(counter1) + '",\n'
    payload += '"missed1": "'  + str(missed1)  + '",\n'
    payload += '"wh1": "'      + str(wh1)     + '",\n'
    payload += '"pulses2": "'  + str(pulses2)  + '",\n'
    payload += '"counter2": "' + str(counter2) + '",\n'
    payload += '"missed2": "'  + str(missed2)  + '",\n'
    payload += '"wh2": "'      + str(wh2)     + '"\n'
    payload += '}\n'

    msg = lib_publishMyPayload(co,payload)

    lib_commonAction(co,msg)

    #message = 'counter:' + str(dy.mycounter)
    #lib_publishMyLog(co, message)

    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
