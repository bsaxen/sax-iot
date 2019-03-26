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
# Functions
#===================================================

#===================================================
def sendFeedback():
    global co
    lib_placeOrder(co.mydomain, co.myserver, co.device[0], co.feedback[0])

#===================================================
# Setup
#===================================================
schedule.every(10).seconds.do(sendFeedback)
#schedule.every(10).minutes.do(sendFeedback)
#schedule.every().hour.do(job)
#schedule.every().day.at("10:30").do(job)
#schedule.every().monday.do(job)
#schedule.every().wednesday.at("13:15").do(job)
confile = "gowtrigger.conf"
lib_readConfiguration(confile,co)
lib_gowPublishMyStatic(co)
#===================================================
# Loop
#===================================================
while True:
    lib_gowIncreaseMyCounter(co,dy)

    payload = '{}'
    msg = lib_gowPublishMyDynamic(co,dy,payload)
    lib_common_action(co,msg)

    message = 'counter:' + str(dy.mycounter)
    lib_gowPublishMyLog(co, message)

    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    schedule.run_pending()
    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
