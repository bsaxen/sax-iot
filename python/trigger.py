# ==================================================
# File: trigger.py
# Author: Benny Saxen
# Date: 2019-03-26
# Description:
# Trigger send feedback
# Note: configured period multiples of schedule period
# ==================================================
import schedule
from iotLib import *
#===================================================
def sendFeedback():
    global co
    lib_placeOrder(co.mydomain, co.myserver, co.device[0], co.feedback[0])

#===================================================
# Setup
#===================================================
confile = "trigger.conf"
version = 1
lib_setup(co,confile,version)

schedule.every(10).seconds.do(sendFeedback)
#schedule.every(10).minutes.do(sendFeedback)
#schedule.every().hour.do(job)
#schedule.every().day.at("10:30").do(job)
#schedule.every().monday.do(job)
#schedule.every().wednesday.at("13:15").do(job)
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,dy)

    msg = lib_publishMyDynamic(co,dy)

    payload = '{"test":"10043","test2": "453"}'
    print payload
    msg = lib_publishMyPayload(co,dy,payload)

    lib_commonAction(co,msg)

    message = 'counter:' + str(dy.mycounter)
    lib_publishMyLog(co, message)

    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    schedule.run_pending()
    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
