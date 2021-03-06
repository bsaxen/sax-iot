#!/usr/bin/python
#=============================================
# File.......: mysql.py
# Date.......: 2019-05-19
# Author.....: Benny Saxen
# Description:
#=============================================
# Libraries
#=============================================
from iotLib import *
from mysqlLib import *
#=============================================
# Configuration
#=============================================
schedule    = []
work        = []
running     = []
description = []
#=============================================
# setup
#=============================================
confile = 'mysql.conf'
version = 1
lib_setup(co,confile,version)

print "Number of datastreams: " + str(co.nds)
message = 'mysql_started_streams_' + str(co.nds)
msg = lib_publishMyLog(co,message)

max_period = 0
for num in range(0,co.nds):
    print num
    domain = co.ds_domain[num]
    device = co.ds_device[num]
    source = co.ds_source[num]
    param  = co.ds_db_par[num]
    roger = 1
    
    error = lib_readStaticParam(co,domain,device,'period')
    if error == 0:
        period = int(co.myresult)
        schedule.append(period)
        work.append(period)
        print "period="+str(period)
    else:
        roger = 0
    
    error = lib_readStaticParam(co,domain,device,'desc')
    if error == 0:
        desc = co.myresult
        description.append(desc)
    else:
        roger = 0

    error = float(lib_readDynamicParam(co,domain,device,'counter'))
    if error == 0:
        counter = int(co.myresult)
        running.append(counter)
        print counter
    else:
        roger = 0

    if source == 'dynamic':
        error = lib_readDynamicParam(co,domain,device,param)
    if source == 'payload':
        error = lib_readPayloadParam(co,domain,device,param)
        
    if error == 0:
        x = float(co.myresult)
        print x
    else:
        roger = 0
        
    if roger == 1:
        if co.ds_db_table[num] == 'auto':
            table = desc
        else:
            table = co.ds_db_table[num]
        
        lib_mysqlInsert(co,1,table,'value',x)
    else:
        print "Error during setup reading data: " + str(num)
        message = str(counter) + "_ERROR_SETUP_" + description[num]
        msg = lib_publishMyLog(co,message)
#=============================================
# loop
#=============================================
now = datetime.datetime.now()#.strftime("%Y-%m-%d %H:%M:%S")
time.sleep(3)
total_duration = 0
while True:
    dy.mycounter += 1
    if dy.mycounter > co.mywrap:
        dy.mycounter = 1
        
    if dy.mycounter%60 == 0:
        lib_publishMyDynamic(co,dy)
        
    then = now
    now = datetime.datetime.now()#.strftime("%Y-%m-%d %H:%M:%S")
    #print now
    duration = now - then                         # For build-in functions
    duration_in_s = duration.total_seconds()
    #print duration_in_s
    total_duration += duration_in_s
    #print total_duration
    #print "sleep " + str(1)
    time.sleep(1)

    for num in range(0,co.nds):
        counter = 1234
        domain = co.ds_domain[num]
        device = co.ds_device[num]
        source = co.ds_source[num]
        param  = co.ds_db_par[num]

        work[num] -= 1.0
        #print str(num) + " " + str(work[num])
        if work[num] == 0:
            work[num] = schedule[num]
            
            roger = 1
            
            error = lib_readDynamicParam(co,domain,device,'counter')
            if error == 0:
                counter = int(co.myresult)
                #print counter
            else:
                roger = 0
                
            
            if roger == 1:
                delta_counter = counter - running[num]
                #print str(counter) + " " + str(running[num]) + " " + param
                ok = 0
                if delta_counter == 1:
                    #print "Correct data: " + str(delta_counter) + " " + description[num] + " " + param
                    ok = 1
                if delta_counter > 3:
                    print "**** Missing data: " + str(delta_counter) + " " + description[num] + " " + param
                    message = str(counter) + "_Missing_data_" + description[num] + " " + str(delta_counter)
                    msg = lib_publishMyLog(co,message)
                    ok = 1
                if delta_counter == 0:
                    print "==== No update of data: " + str(delta_counter) + " " + description[num] + " " + param
                if delta_counter < 0:
                    print "Wrap around of data: " + str(delta_counter)
                    message = str(counter) + "_Wrap_data_" + description[num]
                    msg = lib_publishMyLog(co,message)
                    ok = 1
                if ok == 1:
                    running[num] = counter
                    if source == 'dynamic':
                        error  = lib_readDynamicParam(co,domain,device,param)
                    else:
                        error  = lib_readPayloadParam(co,domain,device,param)
                    if error == 0:
                        x = float(co.myresult)
                        #print x
                        if co.ds_db_table[num] == 'auto':
                            table = description[num]
                        else:
                            table = co.ds_db_table[num]
                    
                        lib_mysqlInsert(co,0,table,'value',x)
                    else:
                        print "Error reading data during loop " + str(num) + " " + description[num]
                        message = str(counter) + "_ERROR_LOOP_" + description[num]
                        msg = lib_publishMyLog(co,message)
            else:
                print "Error roger in loop " + str(num) + " " + description[num]
                message = str(counter) + "_ERROR_ROGER_LOOP" + description[num]
                msg = lib_publishMyLog(co,message)
#===================================================
# End of file
#===================================================
