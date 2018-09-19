import rospy, math

class PID:
	def __init__(self):
		self.Kp = 1
		self.ki = 1
		self.kd = 1
		self.sum_error = 0
		self.diff_error = 0
		self.diff_time = 0
		self.sum_diff_time = 0
		self.previous_error = 0
	
	def caculate(self, error):
		self.diff_time = rospy.Time.from_sec(self.this_time)
		self.sum_diff_time+=self.diff_time
		self.diff_error = error - self.previous_error
		self.sum_error+=error
# this is result
		result = error*self.Kp + self.sum_diff_time*self.sum_error*self.Ki + (self.diff_error/self.diff_time)*self.Kd
# after cal result
		self.this_time = rospy.get_time()
		self.previous_error = error
# when want to use time to limit (ttl)

		return result		
		

	def set_PID(self, Kp , Ki , Kd):
		self.Kp = Kp
		self.Ki = Ki
		self.Kd = Kd
		self.reset_PID()

	def reset_PID(self):
		self.diff_error = 0
		self.sum_diff_time = 0
		self.sum_error = 0
		self.previous_error = 0
		self.this_time = rospy.get_time()
