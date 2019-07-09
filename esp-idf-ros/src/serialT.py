#!/usr/bin/env python
# license removed for brevity

'''
@zafersn93
'''
import rospy
from std_msgs.msg import String
from geometry_msgs.msg  import Twist
from sensor_msgs.msg import Joy

import serial

class ESP_IDF_TEST(object):
	def __init__(self):
		self.blink=rospy.Subscriber("/turtle1/cmd_vel", Twist, self.blink_callback)
		self.ser = serial.Serial('/dev/ttyUSB1', 115200)
		self.vel_msg=Twist()		   


	def blink_callback (self, msg):
		self.vel_msg.linear.x=msg.linear.x
		self.vel_msg.linear.y=msg.linear.y
		self.vel_msg.linear.z=msg.linear.z

		self.vel_msg.angular.x = msg.angular.x
        	self.vel_msg.angular.y = msg.angular.y
	        self.vel_msg.angular.z = msg.angular.z
		
		if msg.linear.x == 2:
			self.yonVer(1)
		elif msg.linear.x == -2:
			self.yonVer(2)
		
		if msg.angular.z == 2:
			self.yonVer(3)
		elif msg.angular.z == -2:
			self.yonVer(4)

	
	def yonVer(self,Lnr):
		mData=str(int(Lnr))+"\0"	
		rospy.loginfo(rospy.get_caller_id() + "I heard %s", mData)
		self.ser.write(str(mData))
		

if __name__ == '__main__':
    try:
	rospy.init_node("esp_idf_test")
	r=rospy.Rate(50)
	lr=ESP_IDF_TEST()
	isOp=lr.ser.isOpen()
	if isOp:
		print("serialOpen")

	while not rospy.is_shutdown():
		r.sleep()
        
    except rospy.ROSInterruptException:
        pass 


