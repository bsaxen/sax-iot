# =============================================
# File:        camera.py
# Author:      Benny Saxen
# Date:        2019-03-27
# Description: application for running a picamera
# feedback: photo
# =============================================
import os
from iotLib import *
from picamera import PiCamera

#===================================================
def take_picture(co):
#===================================================
    camera = PiCamera()
    camera.resolution = (1280, 1024)
    camera.capture("temp.jpg")
    camera.close()
    time.sleep(1)

    #api_host = lib_gowCreateMyUrl(co)
    #headers = {'Content-Type' : 'image/jpeg'}
    #image_url = 'temp.jpeg'
	
    #img_file = urllib2.urlopen(image_url)
    #response = requests.post(api_host, data=img_file.read(), headers=headers, verify=False)
	
    # copy image to server
    os.system("scp {0} {1}@{2}{3}{4}{5}".format("temp.jpg",
                                              co.image_user,
                                              co.image_url,
                                              co.image_path,
                                              co.image_prefix,
                                              co.image_name))

#===================================================
# Setup
#===================================================
confile = "camera.conf"
version = 1
lib_setup(co,confile,version)
#===================================================
# Loop
#===================================================
while True:
    lib_increaseMyCounter(co,dy)

    msg = lib_publishMyDynamic(co,dy)
    action = lib_commonAction(co,msg)

    if action == 'photo':
        print 'take photo'
	#payload = '{"camera":"test"}'
	#msg = lib_publishMyPayload(co,dy,payload)
	message = 'Photo_taken_' + str(dy.mycounter)
	lib_publishMyLog(co, message)
        take_picture(co)
	
    print "sleep: " + str(co.myperiod) + " triggered: " + str(dy.mycounter)
    time.sleep(float(co.myperiod))
#===================================================
# End of file
#===================================================
