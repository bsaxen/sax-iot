# =============================================
# File: iotLib.py
# Author: Benny Saxen
# Date: 2019-05-15
# Description: IoT python library
version = 2
# =============================================
import urllib
import urllib2
import time
import datetime
import random
import string
import json

STATE_OFF     = 1
STATE_ON      = 2
STATE_WARMING = 3
MODE_OFFLINE  = 1
MODE_ONLINE   = 2

CLOCKWISE        = 1
COUNTERCLOCKWISE = 2

DECREASE = 2
INCREASE = 1

class Datastreams:
    no        = []
    period    = []
    dev_ts    = []
    sys_ts    = []
    wifi_ss   = []
    value     = []

class ModuleDynamic:
    mystate   = 0
    mymode    = 0
    mycounter = 0
    mystop    = 0
    myerrors  = 0
    mydev_ts = ""
    mywifi_ss = 0
    myresult  = 0
    myuplink_time   = 0
    mydownlink_time = 0

class Configuration:
    myid         = '1234'
    mytitle      = 'x'
    mytags       = 'y'
    mydesc       = 'z'
    mydomain     = 'iot.simuino.com'
    myserver     = 'gateway.php'
    myplatform   = 'python'
    myperiod     = 10
    myfeedback   = 1
    mywrap       = 999999
    mysw         = 0
    mylibrary    = 0
    myresult     = 0

    # Data sources
    data_parameter = []
    data_domain    = []
    data_device    = []
    data_type      = [] # static, dynamic, payload
    ndata = 0

    # Data sources
    send_domain    = []
    send_device    = []
    nsend = 0

    # Heater algorithm
    mintemp = 0.0
    maxtemp = 0.0
    minheat = 0.0
    maxheat = 0.0
    x_0     = 0.0
    y_0     = 0.0

    # Heater Twin
    relax     = 4.0
    minsmoke  = 0.0
    minsteps  = 0
    maxsteps  = 0
    maxenergy = 0
    inertia   = 480
    warmcool  = 720

    # database access
    dbhost     = '192.168.1.85'
    dbname     = 'gow'
    dbuser     = 'myuser'
    dbpassword = 'mypswd'

    # datastreams subscriptions
    ds_domain = []
    ds_device = []
    ds_param  = []
    ds_source = []

    # Database tables and parameters
    ds_db_table  = []
    ds_db_par    = []
    nds = 0

    # Image
    image_user   = 'folke'
    image_url    = 'gow.test.com'
    image_path   = 'images_dir'
    image_prefix = 'some'
    image_name   = 'any'


co = Configuration()
co.mylibrary = version
ds = Datastreams()
dy = ModuleDynamic()


#===================================================
def lib_httpRequest(domain,server,values):
#===================================================
    req = 'http://' + domain + '/' + server + '?' + values
    try:
        #t1 = int(round(time.time() * 1000))     
        response = urllib2.urlopen(req)
        msg = response.read()
        #t2 = int(round(time.time() * 1000))
        #dy.myuplink_time = t2 - t1
        #print ('Message to ' + co.myid + ': ' + msg)
    except urllib2.URLError as e:
        print (e.reason)
        msg = '-'
        
    return msg
#===================================================
def lib_publishMyStatic(co):
#===================================================

    domain = co.mydomain
    server = co.myserver
    co.mycounter = 0
    data = {}
    # meta data
    data['do']       = 'static'
    data['id']       = co.myid
    payload  = '{'
    payload += '"title" : "'    + str(co.mytitle) + '",'
    payload += '"desc" : "'     + str(co.mydesc) + '",'
    payload += '"tags" : "'     + str(co.mytags) + '",'
    payload += '"feedback" : "' + str(co.myfeedback) + '",'
    payload += '"period" : "'   + str(co.myperiod) + '",'
    payload += '"wrap" : "'     + str(co.mywrap) + '",'
    payload += '"sw" : "'       + str(co.mysw) + '",'
    payload += '"library" : "'  + str(co.mylibrary) + '",'
    payload += '"platform" : "' + str(co.myplatform) + '"'
    payload += '}'
    data['json']  = payload

    values = urllib.urlencode(data)
    msg = lib_httpRequest(domain,server,values)
    
    return msg
#===================================================
def lib_publishMyDynamic(co,dy):
#===================================================
    msg = '-'
    domain = co.mydomain
    server = co.myserver
    data = {}
    # meta data
    data['do']       = 'dynamic'
    data['id']       = co.myid
    payload  = '{'
    payload += '"counter" : "'   + str(dy.mycounter)       + '",'
    #payload += '"uplink" : "'    + str(dy.myuplink_time)   + '",'
    #payload += '"downlink" : "'  + str(dy.mydownlink_time) + '",'
    payload += '"errors" : "'    + str(dy.myerrors)        + '"'
    payload += '}'
    data['json']     = payload

    values = urllib.urlencode(data)
    msg = lib_httpRequest(domain,server,values)
    
    return msg
#===================================================
def lib_publishMyPayload(co,payload):
#===================================================
    msg = '-'
    domain = co.mydomain
    server = co.myserver
    data = {}
    # meta data
    data['do']       = 'payload'
    data['id']       = co.myid
    data['json']     = payload

    values = urllib.urlencode(data)
    msg = lib_httpRequest(domain,server,values)
    
    return msg
#===================================================
def lib_publishMyLog(co,message):
#===================================================
    msg = '-'
    domain = co.mydomain
    server = co.myserver
    data = {}

    data['do']       = 'log'
    data['id']       = co.myid
    data['log']      = message

    values = urllib.urlencode(data)
    msg = lib_httpRequest(domain,server,values)
    
    return msg
#===================================================
def lib_createMyUrl(co):
#===================================================
    url = 'http://' + co.mydomain + '/devices/' + co.myid + '/'
    return url
#=====================================================
def lib_buildMyUrl(co,dynstat):
#===================================================
    url =  'http://' + co.mydomain + '/devices/' + co.myid + '/' + dynstat +'.json'
    return url
#===================================================
def lib_createAnyUrl(co,domain,device):
#===================================================
    co.myresult = 'http://' + domain + '/devices/' + device + '/'
    return
#=====================================================
def lib_buildAnyUrl(co,domain,device,dynstat):
#===================================================
    co.myresult =  'http://' + domain + '/devices/' + device + '/' + dynstat +'.json'
    return 
#=====================================================
def lib_buildLogUrl(domain,device):
#===================================================
    url =  'http://' + domain + '/devices/' + device + '/' + 'log.txt'
    return url
#=====================================================
def lib_writeMemory(fname,value):
    try:
        f = open(fname,'w')
        f.write(value)
        f.close()
    except:
        print ("ERROR write memory " + fname)
    return
#=====================================================
def lib_readMemory(fname):
    res = 0
    try:
        fh = open(fname,'r')
        for line in fh:
            res = line
        fh.close()
    except:
        print ("ERROR read memory " + fname)
    return res
#=====================================================
def lib_initFile(fname):
    try:
        f = open(fname,'w')
        f.write(fname)
        f.write('\n')
        f.close()
    except:
        print ("ERROR init file " + fname)
    return
#=====================================================
def lib_writeFile(fname,message,ts):
    try:
        f = open(fname,'a')
        if ts == 1:
            f.write(datetime.datetime.now().strftime("%y-%m-%d %H:%M:%S")+" ")
        f.write(message)
        f.write('\n')
        f.close()
    except:
        print ("ERROR write to file " + fname)
    return
#=====================================================
def lib_getDeviceStatus(ds,domain,device):
    url = lib_createAnyUrl(domain,device)
    nu = datetime.datetime.now()
    then = lib_consumeDatastream(ds,url,'sys_ts')
    period = lib_consumeDatastream(ds,url,'period')
    res = (nu-then).seconds
    if res <= period:
        status = 0
    else:
        status = res
    return status
#=====================================================
def lib_apiListDomainTopics(co):
    url = 'http://' + co.mydomain + '/' + co.myserver + '?do=list_topics'
    #print url
    response = urllib2.urlopen(url)
    the_page = response.read()
    the_page = the_page.replace('.reg','')
    the_page = the_page.replace('_','/')
    list = the_page.split(':')
    #print co.domain
    #print the_page
    return list
#=====================================================
def lib_apiSearchTopics(co,key):
    url = 'http://' + co.domain + '/' + co.myserver + '?do=search&search=' + key
    #print url
    response = urllib2.urlopen(url)
    the_page = response.read()
    the_page = the_page.replace('.reg','')
    the_page = the_page.replace('_','/')
    list = the_page.split(':')
    #print co.domain
    #print the_page
    return list
#===================================================
def lib_commonAction(co,feedback):
    action = ' '
    if ":" in feedback:
        p = feedback.split(':')
        q = p[1].split(",")
        m = len(q)
        if m == 1:
            if q[0] == 'test':
                print ('test')
                action = 'test'
            if q[0] == 'photo':
                print ('photo')
                action = 'photo'
        if m == 2:
            if q[0] == 'period':
                co.myperiod = q[1]
            if q[0] == 'feedback':
                co.myfeedback = q[1]
            if q[0] == 'id':
                co.myid = q[1]
            if q[0] == 'desc':
                co.mydesc = q[1]
    return action
#===================================================
def lib_evaluateAction( action):
    print (action)
#===================================================
def lib_readConfiguration(confile,c1):
    #print confile
    try:
        c1.nds = 0
        fh = open(confile, 'r')
        for line in fh:
            #print 'z' + line
            if line[0] != '#':
                word = line.split()
                if word[0] == 'c_id':
                    c1.myid               = word[1]
                if word[0] == 'c_title':
                    c1.mytitle            = word[1]
                if word[0] == 'c_tags':
                    c1.mytags             = word[1]
                if word[0] == 'c_desc':
                    c1.mydesc             = word[1]
                if word[0] == 'c_domain':
                    c1.mydomain           = word[1]
                if word[0] == 'c_server':
                    c1.myserver           = word[1]
                if word[0] == 'c_period':
                    c1.myperiod           = word[1]
                if word[0] == 'c_platfrom':
                    c1.myplatform         = word[1]
                if word[0] == 'c_wrap':
                    c1.mywrap             = word[1]
                if word[0] == 'c_feedback':
                    c1.myfeedback         = word[1]

                # Heater algorithm
                if word[0] == 'c_mintemp':
                    c1.mintemp          = word[1]
                if word[0] == 'c_maxtemp':
                    c1.maxtemp          = word[1]
                if word[0] == 'c_minheat':
                    c1.minheat          = word[1]
                if word[0] == 'c_maxheat':
                    c1.maxheat          = word[1]
                if word[0] == 'c_x_0':
                    c1.x_0              = word[1]
                if word[0] == 'c_y_0':
                    c1.y_0              = word[1]

                # Heater Twin
                if word[0] == 'c_relax':
                    c1.relax            = word[1]
                if word[0] == 'c_minsmoke':
                    c1.minsmoke         = word[1]
                if word[0] == 'c_minsteps':
                    c1.minsteps         = word[1]
                if word[0] == 'c_maxsteps':
                    c1.maxsteps         = word[1]
                if word[0] == 'c_warmcool':
                    c1.warmcool         = word[1]
                if word[0] == 'c_inertia':
                    c1.inertia         = word[1]
                if word[0] == 'c_maxenergy':
                    c1.maxenergy        = word[1]

                # Database access
                if word[0] == 'c_dbhost':
                    c1.dbhost         = word[1]
                if word[0] == 'c_dbname':
                    c1.dbname         = word[1]
                if word[0] == 'c_dbuser':
                    c1.dbuser         = word[1]
                if word[0] == 'c_dbpassword':
                    c1.dbpassword      = word[1]

                if word[0] == 'c_data':
                    c1.data_domain.append(word[1])
                    c1.data_device.append(word[2])
                    c1.data_type.append(word[3])
                    c1.data_parameter.append(word[4])
                    c1.ndata += 1

                if word[0] == 'c_send':
                    c1.send_domain.append(word[1])
                    c1.send_device.append(word[2])
                    c1.nsend += 1

                if word[0] == 'c_stream':
                    c1.ds_domain.append(word[1])
                    c1.ds_device.append(word[2])
                    c1.ds_source.append(word[3])
                    c1.ds_db_table.append(word[4])
                    c1.ds_db_par.append(word[5])
                    c1.nds += 1

                # Image
                if word[0] == 'c_image_user':
                    c1.image_user      = word[1]
                if word[0] == 'c_image_user':
                    c1.image_url       = word[1]
                if word[0] == 'c_image_url':
                    c1.image_path      = word[1]
                if word[0] == 'c_image_path':
                    c1.image_path      = word[1]
                if word[0] == 'c_image_name':
                    c1.image_name      = word[1]
            else:
                print (line)
        fh.close()
    except Exception as e:
        print(e)
        fh = open(confile, 'w')
        fh.write('c_id        1234\n')
        fh.write('c_title     title\n')
        fh.write('c_domain    iot.simuino.com\n')
        fh.write('c_tags      tag\n')
        fh.write('c_desc      some\n')
        fh.write('c_server    gateway.php\n')
        fh.write('c_period    10\n')
        fh.write('c_platform  python\n')
        fh.write('c_wrap      999999\n')
        fh.write('c_feedback  1\n')

        fh.write('c_mintemp      -7\n')
        fh.write('c_maxtemp      15\n')
        fh.write('c_minheat      25\n')
        fh.write('c_maxheat      40\n')
        fh.write('c_x_0          0\n')
        fh.write('c_y_0          36\n')
        fh.write('c_relax        4.0\n')
        fh.write('c_minsmoke     27\n')
        fh.write('c_minsteps     5\n')
        fh.write('c_maxsteps     40\n')
        fh.write('c_warmcool     720\n')
        fh.write('c_inertia      480\n')
        fh.write('c_maxenergy    4.0\n')

        fh.write('#c_send         domain device\n')

        fh.write('#c_data         domain device type parameter\n')

        fh.write('c_dbhost       192.168.1.85\n')
        fh.write('c_dbname       gow\n')
        fh.write('c_dbuser       folke\n')
        fh.write('c_dbpassword   something\n')
        fh.write('#c_stream       domain device source table param\n')

        fh.write('c_image_user   folke\n')
        fh.write('c_image_url    gow.test.com\n')
        fh.write('c_image_path   images\n')
        fh.write('c_image_prefix some\n')
        fh.write('c_image_name   any\n')
        fh.close()
        print ("Configuration file created: " + confile)
        print ("Edit your configuration and restart the application")
        exit()
    return
#=============================================
def lib_getStaticDeviceJson(co,domain,device):
#=============================================
    co.myresult = 0
    lib_buildAnyUrl(co,domain,device,'static')
    error = 0
    try:
        r = urllib2.urlopen(co.myresult,timeout=5)
    except urllib2.URLError as e:
        print (e.reason)
        error = 1
        co.myresult = '{"error":"1234"}'

    if error == 0:
        try:
            co.myresult = json.load(r)
        except ValueError, e:
            error = 1
    else:
        print (error)
    return error
#=============================================
def lib_getDynamicDeviceJson(co,domain,device):
#=============================================
    co.myresult = 0
    lib_buildAnyUrl(co,domain,device,'dynamic')
    error = 0
    try:
        t1 = int(round(time.time() * 1000))
        r = urllib2.urlopen(co.myresult,timeout=5)
        t2 = int(round(time.time() * 1000))
        dy.mydownlink_time = t2 - t1
    except urllib2.URLError as e:
        print (e.reason)
        error = 1
        co.myresult = '{"error":"1234"}'

    if error == 0:
        try:
            co.myresult = json.load(r)
        except ValueError, e:
            error = 1
    else:
        print (error)
    return error
#=============================================
def lib_getPayloadDeviceJson(co,domain,device):
#=============================================
    co.myresult = 0
    lib_buildAnyUrl(co,domain,device,'payload')
    error = 0
    try:
        t1 = int(round(time.time() * 1000))
        r = urllib2.urlopen(co.myresult,timeout=5)
        t2 = int(round(time.time() * 1000))
        dy.mydownlink_time = t2 - t1
    except urllib2.URLError as e:
        print (e.reason)
        error = 1
        co.myresult = '{"error":"1234"}'

    if error == 0:
        try:
            co.myresult = json.load(r)
        except ValueError, e:
            error = 1
    else:
        print (error)
    return error

#=============================================
def lib_checkDeviceOnline(domain,device):
#=============================================
    now = str(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    ts = lib_readDynamicParam(domain,device,'sys_ts')
    period = lib_readStaticParam(domain,device,'period')
    xts1 = time.mktime(datetime.datetime.strptime(ts, "%Y-%m-%d %H:%M:%S").timetuple())
    xts2 = time.mktime(datetime.datetime.strptime(now, "%Y-%m-%d %H:%M:%S").timetuple())
    diff = xts2 - xts1
    print (str(period) + " " + str(diff))
    old = 0
    if diff > period:
        old = 1

    return old
#=============================================
def lib_checkSequenceNumber(co,ds,domain,device):
#=============================================
    n = lib_readDynamicParam(domain,device,'counter')
    m = ds.no[device]
    k = m + 1

    if n < m and n != 1: # Out of order
        message = "Sequence number out of order " + domain + ' ' + device + ' n='+n + ' old='+m
        lib_publishMyLog(co, message)
        res = 2
    if n == k or n == 1: # Expected number
        ds.no[device] = n
        res = 0
    if n > k: # Missed number
        message = "Missed seuence number " + domain + ' ' + device + ' n='+n + ' old='+m
        lib_publishMyLog(co, message)
        res = 1
    return res
#=============================================
def lib_readDynamicParam(co,domain,device,par):
#=============================================
    co.myresult = 0
    error = lib_getDynamicDeviceJson(co,domain,device)
    if error == 0:
        co.myresult = co.myresult['msg'][par]
    else:
        print (error)
    return error
#=============================================
def lib_readStaticParam(co,domain,device,par):
#=============================================
    co.myresult = 0
    error = lib_getStaticDeviceJson(co,domain,device)
    if error == 0:
        co.myresult = co.myresult['msg'][par]
    else:
        print (error)
    return error
#=============================================
def lib_readPayloadParam(co,domain,device,par):
#=============================================
    co.myresult = 0
    error = lib_getPayloadDeviceJson(co,domain,device)
    if error == 0:
        co.myresult = co.myresult['msg'][par]
    else:
        print (error)
    return error
#=============================================
def lib_searchLogKey(domain,device,par):
#=============================================
    x = 0
    url = lib_buildLogUrl(domain,device)
    r = urllib2.urlopen(url)
    return x
#=============================================
def lib_readData(co,index):
#=============================================
    index_error = 0
    co.myresult = 0
    if index >= co.ndata:
        print ("index too large")
        index_error = 1
    if index_error == 0:
        domain    = co.data_domain[index]
        device    = co.data_device[index]
        parameter = co.data_parameter[index]
        stdypa    = co.data_type[index]

        if stdypa == 'static':
          error = lib_readStaticParam(co,domain,device,parameter)
        if stdypa == 'dynamic':
          error = lib_readDynamicParam(co,domain,device,parameter)
        if stdypa == 'payload':
          error = lib_readPayloadParam(co,domain,device,parameter)
    else:
        error = 1001 
        print (error)       
    return error
#===================================================
def lib_placeOrder(domain, server, device, message):
#===================================================
    data = {}
    data['do']        = 'feedback'
    data['id']        = device
    data['msg']       = message
    data['tag']       = lib_generateRandomString()
    values = urllib.urlencode(data)
    req = 'http://' + domain + '/' + server + '?' + values
    print (req)
    try:
        response = urllib2.urlopen(req)
    except urllib2.URLError as e:
        print (e.reason)
#=============================================
def lib_generateRandomString():
#=============================================
   char_set = string.ascii_uppercase + string.digits
   return ''.join(random.sample(char_set*6, 6))
#===================================================
def lib_setup(co,confile,version):
#===================================================
    lib_readConfiguration(confile,co)
    lib_publishMyStatic(co)
    co.mysw = version
    message = '==== setup ===='
    lib_publishMyLog(co, message)
#===================================================
def lib_loop(co,dy):
#===================================================
    dy.mycounter += 1
    if dy.mycounter > co.mywrap:
        dy.mycounter = 1
    msg = lib_publishMyDynamic(co,dy)
    # TBD publish static
    time.sleep(float(co.myperiod))
    return msg
#===================================================
def lib_increaseMyCounter(co,dy):
#===================================================
    dy.mycounter += 1
    if dy.mycounter > co.mywrap:
        dy.mycounter = 1
    return
#===================================================
# End of file
#===================================================
