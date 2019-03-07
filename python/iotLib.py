# =============================================
# File: iotLib.py
# Author: Benny Saxen
# Date: 2019-03-07
# Description: IoT python library
# =============================================
import MySQLdb
import urllib
import urllib2
import time
import datetime
import random
import string
import json

STATE_OFF     = 1
STATE_ON      = 2
MODE_OFFLINE  = 1
MODE_ONLINE   = 2

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

class Configuration:
    myid         = '1234'
    mytitle      = 'title'
    mytags       = 'tag'
    mydesc       = 'some'
    mydomain     = 'gow.simuino.com'
    myserver     = 'gowServer.php'
    myplatform   = 'python'
    myperiod     = 10
    myfeedback   = 1
    mywrap       = 999999
    mytopic      = "benny/saxen/0"

    # Trigger feedback
    fb_feedback    = []
    fb_domain      = []
    fb_device      = []
    nfb = 0
    # Heater algorithm
    mintemp = 0.0
    maxtemp = 0.0
    minheat = 0.0
    maxheat = 0.0
    x_0     = 0.0
    y_0     = 0.0
    relax   = 4.0
    minsmoke = 0.0
    minsteps  = 0
    maxsteps  = 0
    defsteps  = 0
    maxenergy = 0

    # database access
    dbhost     = '192.168.1.85'
    dbname     = 'gow'
    dbuser     = 'myuser'
    dbpassword = 'mypswd'

    # datastreams subscriptions
    ds_domain = []
    ds_device = []
    ds_param  = []
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
ds = Datastreams()
dy = ModuleDynamic()

#===================================================
def lib_increaseMyCounter(co,dy):
#===================================================
    dy.mycounter += 1
    if dy.mycounter > co.mywrap:
        dy.mycounter = 1
#===================================================
def lib_publishMyStatic(co):
#===================================================

    domain = co.mydomain
    server = co.myserver
    co.mycounter = 0
    data = {}
    # meta data
    data['do']       = 'stat'
    data['id']       = co.myid
    data['desc']     = co.mydesc
    data['tags']     = co.mytags
    data['topic']    = co.mytopic
    data['wrap']     = co.mywrap
    data['period']   = co.myperiod
    data['platform'] = 'python'
    data['domain']      = co.mydomain
    data['ssid']     = 'nowifi'
    data['feedback']   = co.myfeedback

    values = urllib.urlencode(data)
    req = 'http://' + domain + '/' + server + '?' + values
    print req
    try:
        response = urllib2.urlopen(req)
        the_page = response.read()
        print 'Message to ' + co.mytopic + ': ' + the_page
        #evaluateAction(the_page)
    except urllib2.URLError as e:
        print e.reason
#===================================================
def lib_publishMyDynamic(co,dy,payload):
#===================================================
    msg = '-'
    domain = co.mydomain
    server = co.myserver
    data = {}
    # meta data
    data['do']       = 'dyn'
    data['id']       = co.myid
    data['topic']    = co.mytopic
    data['counter']  = dy.mycounter
    data['rssi']     = 0
    data['dev_ts']   = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    data['fail']     = 0
    data['payload']  = 0
    values = urllib.urlencode(data)
    req = 'http://' + domain + '/' + server + '?' + values
    print req
    try:
        response = urllib2.urlopen(req)
        msg = response.read()
        print 'Message to ' + co.mytopic + ': ' + msg
    except urllib2.URLError as e:
        print e.reason

    return msg
#===================================================
def lib_publishMyPayload(co,dy,payload):
#===================================================
    msg = '-'
    domain = co.mydomain
    server = co.myserver
    data = {}
    # meta data
    data['do']       = 'dyn'
    data['id']       = co.myid
    data['topic']    = co.mytopic
    data['counter']  = dy.mycounter
    data['rssi']     = 0
    data['dev_ts']   = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    data['fail']     = 0
    data['payload']    = payload

    values = urllib.urlencode(data)
    req = 'http://' + domain + '/' + server + '?' + values
    print req
    try:
        response = urllib2.urlopen(req)
        msg = response.read()
        print 'Message to ' + co.mytopic + ': ' + msg
    except urllib2.URLError as e:
        print e.reason

    return msg
#===================================================
def lib_publishMyLog(co, message ):
#===================================================
    msg = '-'
    domain = co.mydomain
    server = co.myserver
    data = {}

    data['do']       = 'log'
    data['id']       = co.myid
    data['topic']    = co.mytopic
    data['dev_ts']   = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    data['log']      = message

    values = urllib.urlencode(data)
    req = 'http://' + domain + '/' + server + '?' + values
    print req
    try:
        response = urllib2.urlopen(req)
        msg = response.read()
        print 'Message to ' + co.mytopic + ': ' + msg
        #evaluateAction(the_page)
    except urllib2.URLError as e:
        print e.reason

    return msg
#===================================================
def lib_createMyUrl(co):
#===================================================
    url = 'http://' + co.mydomain + '/' + co.mytopic + '/'
    return url
#=====================================================
def lib_buildMyUrl(co,dynstat):
#===================================================
    url =  'http://' + co.mydomain + '/' + co.mytopic + '/' + dynstat +'.json'
    return url
#===================================================
def lib_createAnyUrl(domain,device):
#===================================================
    url = 'http://' + domain + '/' + device + '/'
    return url
#=====================================================
def lib_buildAnyUrl(domain,device,dynstat):
#===================================================
    url =  'http://' + domain + '/' + device + '/' + dynstat +'.json'
    return url
#=====================================================
def lib_init_history(fname):
    try:
        f = open(fname,'w')
        f.write(fname)
        f.write('\n')
        f.close()
    except:
        print "ERROR init file " + fname
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
        print "ERROR write to file " + fname
    return
#=====================================================
def lib_log(application,message):
    msg = application + " " + message
    lib_writeFile('gow.log',msg,1)
    return
#=====================================================
def lib_getDeviceStatus(ds,domain,device):
    url = lib_gowCreateAnyUrl(domain,device)
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
    print url
    response = urllib2.urlopen(url)
    the_page = response.read()
    the_page = the_page.replace('.reg','')
    the_page = the_page.replace('_','/')
    list = the_page.split(':')
    print co.domain
    print the_page
    return list
#=====================================================
def lib_apiSearchTopics(co,key):
    url = 'http://' + co.domain + '/' + co.myserver + '?do=search&search=' + key
    print url
    response = urllib2.urlopen(url)
    the_page = response.read()
    the_page = the_page.replace('.reg','')
    the_page = the_page.replace('_','/')
    list = the_page.split(':')
    print co.domain
    print the_page
    return list
#===================================================
def lib_common_action(co,feedback):
    action = ' '
    if ":" in feedback:
        p = feedback.split(':')
        q = p[1].split(",")
        m = len(q)
        if m == 1:
            if q[0] == 'test':
                print 'test'
                action = 'test'
            if q[0] == 'photo':
                print 'photo'
                action = 'photo'
        if m == 2:
            if q[0] == 'period':
                co.myperiod = q[1]
            if q[0] == 'feedback':
                co.myfeedback = q[1]
            if q[0] == 'mytopic':
                co.mytopic = q[1]
            if q[0] == 'desc':
                co.mydesc = q[1]
    return action
#===================================================
def lib_evaluateAction( action):
    print action
#===================================================
def lib_readConfiguration(confile,c1):
    print confile
    try:
        c1.nds = 0
        fh = open(confile, 'r')
        for line in fh:
            print 'z' + line
            if line[0] != '#':
                word = line.split()
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
                if word[0] == 'c_topic':
                    c1.mytopic            = word[1]
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
                if word[0] == 'c_relax':
                    c1.relax            = word[1]
                if word[0] == 'c_minsmoke':
                    c1.minsmoke         = word[1]
                if word[0] == 'c_minsteps':
                    c1.minsteps         = word[1]
                if word[0] == 'c_maxsteps':
                    c1.maxsteps         = word[1]
                if word[0] == 'c_defsteps':
                    c1.defsteps         = word[1]
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

                if word[0] == 'c_fb_vector':
                    c1.fb_domain.append(word[1])
                    c1.fb_device.append(word[2])
                    c1.fb_feedback.append(word[3])
                    c1.nfb += 1

                if word[0] == 'c_stream':
                    c1.ds_domain.append(word[1])
                    c1.ds_device.append(word[2])
                    c1.ds_db_table.append(word[3])
                    c1.ds_db_par.append(word[4])
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
                print line
        fh.close()
    except Exception as e:
        print(e)
        fh = open(confile, 'w')
        fh.write('c_title     title\n')
        fh.write('c_domain    gow.simuino.com\n')
        fh.write('c_tags      tag\n')
        fh.write('c_desc      some\n')
        fh.write('c_server    gowServer.php\n')
        fh.write('c_period    10\n')
        fh.write('c_platform  python\n')
        fh.write('c_wrap      999999\n')
        fh.write('c_topic     benny/saxen/0\n')
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
        fh.write('c_defsteps     30\n')
        fh.write('c_maxenergy    4.0\n')

        fh.write('c_fb_vector  domain device feedback\n')

        fh.write('c_dbhost       192.168.1.85\n')
        fh.write('c_dbname       gow\n')
        fh.write('c_dbuser       folke\n')
        fh.write('c_dbpassword   something\n')
        fh.write('c_stream       domain device table param\n')

        fh.write('c_image_user   folke\n')
        fh.write('c_image_url    gow.test.com\n')
        fh.write('c_image_path   images\n')
        fh.write('c_image_prefix some\n')
        fh.write('c_image_name   any\n')
        fh.close()
        print "Configuration file created: " + confile
        print "Edit your configuration and restart the application"
        exit()
    return
#=============================================
def lib_getStaticDeviceJson(domain,device):
#=============================================
    url = lib_buildAnyUrl(domain,device,'static')
    r = urllib2.urlopen(url)
    j = json.load(r)
    return j
#=============================================
def lib_getDynamicDeviceJson(domain,device):
#=============================================
    url = lib_buildAnyUrl(domain,device,'dynamic')
    r = urllib2.urlopen(url)
    j = json.load(r)
    return j
#=============================================
def lib_getPayloadDeviceJson(domain,device):
#=============================================
    url = lib_buildAnyUrl(domain,device,'payload')
    r = urllib2.urlopen(url)
    j = json.load(r)
    return j

#=============================================
def lib_checkMessageAge(co,ds,domain,device):
#=============================================
    now = str(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
    ts = lib_readDynamicParam(co,ds,domain,device,'sys_ts')
    period = lib_readStaticParam(co,ds,domain,device,'period')
    xts1 = time.mktime(datetime.datetime.strptime(ts, "%Y-%m-%d %H:%M:%S").timetuple())
    xts2 = time.mktime(datetime.datetime.strptime(now, "%Y-%m-%d %H:%M:%S").timetuple())
    diff = xts2 - xts1
    print str(period) + " " + str(diff)
    old = 0
    if diff > period:
        old = 1

    return old
#=============================================
def lib_checkSequenceNumber(co,ds,domain,device):
#=============================================
    j = lib_getDynamicDeviceJson(domain,device)

    n = j['gow']['counter']
    m = ds.no[device]
    k = m + 1

    if n < m and n != 1: # Out of order
        message = "Sequence number out of order " + domain + ' ' + device + ' n='+n + ' old='+m
        lib_gowPublishMyLog(co, message)
        res = 2
    if n == k or n == 1: # Expected number
        ds.no[device] = n
        res = 0
    if n > k: # Missed number
        message = "Missed seuence number " + domain + ' ' + device + ' n='+n + ' old='+m
        lib_gowPublishMyLog(co, message)
        res = 1
    return res
#=============================================
def lib_readDynamicParam(co,ds,domain,device,par):
#=============================================
    j = lib_getDynamicDeviceJson(domain,device)
    #ok = lib_checkSequenceNumber(co,ds,domain,device)
    ok = 0
    x = 'void'
    if ok == 0:
        x = j['gow'][par]
    return x
#=============================================
def lib_readStaticParam(co,ds,domain,device,par):
#=============================================
    j = lib_getStaticDeviceJson(domain,device)
    #ok = lib_checkSequenceNumber(co,ds,domain,device)
    ok = 0
    x = 'void'
    if ok == 0:
        x = j['gow'][par]
    return x
#=============================================
def lib_readPayloadParam(co,ds,domain,device,par):
#=============================================
    j = lib_getPayloadDeviceJson(domain,device)
    #ok = lib_checkSequenceNumber(co,ds,domain,device)
    ok = 0
    x = 'void'
    if ok == 0:
        x = j['gow']['payload'][par]
    return x
#===================================================
def lib_placeOrder(domain, server, device, feedback):
#===================================================
    data = {}
    data['do']     = 'feedback'
    data['topic']  = device
    data['feedback']  = feedback
    data['tag']    = lib_generateRandomString()
    values = urllib.urlencode(data)
    req = 'http://' + domain + '/' + server + '?' + values
    print req
    try:
        response = urllib2.urlopen(req)
    except urllib2.URLError as e:
        print e.reason
#=============================================
def lib_mysqlInsert(c1,cr,xTable,xPar,xValue):
    db = MySQLdb.connect(host=c1.dbhost,user=c1.dbuser,db=c1.dbname)
    cursor = db.cursor()
    if cr == 1:
        sql = "CREATE TABLE IF NOT EXISTS " + xTable + " (id int(11) NOT NULL AUTO_INCREMENT,value float,ts timestamp, PRIMARY KEY (id))"
        cursor.execute(sql)
    sql = "INSERT INTO "+ xTable + " (`id`, " + xPar + ", `ts`) VALUES (NULL," + str(xValue) + ", CURRENT_TIMESTAMP)"
    cursor.execute(sql)
    db.commit()
    db.close()

#=============================================
def lib_generateRandomString():
#=============================================
   char_set = string.ascii_uppercase + string.digits
   return ''.join(random.sample(char_set*6, 6))
#===================================================
# End of file
#===================================================
