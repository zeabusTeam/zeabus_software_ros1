file_name = 'test.yaml'
file_location = '~/Document/newcode/src/zeabus_controller/'
#file_location = '~/catkin_ws/src/src_code/zeabus_controller/src/'

class PID_constant_file:
	def __init__(self):
 		self.file = file_location+file_name

	def load_file(self):
		PID_file = open(self.file)
		file_variable = [] # 0-5 pP 6-11 iP 12-17 dP 18-23 pV 24-29 iV 30-35 dV
		count = 0
		amont = 0
		for line in PID_file:
			data = line.split(' ')
			if(amont == 0):
				file_variable.append([])
			file_variable[count].append(float(data[1]))
			amont+=1
			if (amont == 6):
				count+=1
				amont=0
		return file_variable[0], file_variable[1], file_variable[2], file_variable[3], file_variable[4], fild_variable[5]

#	def save_file(self, KPP, KIP, KDP, KPV, KIV, KDV):	

"""	 		
KPPx: 0
KPPy: 0.01
KPPz: 0.1
KPProll: 0.35
KPPpitch: 0.3
KPPyaw: 0.17
KIPx: 0
KIPy: 0
KIPz: 0
KIProll: 0
KIPpitch: 0
KIPyaw: 0
KDPx: 0
KDPy: 0
KDPz: 0
KDProll: 0.01
KDPpitch: 0.03
KDPyaw: 0
KPVx: 0.4
KPVy: 0.2
KPVz: 0
KPVroll: 0
KPVpitch: 0
KPVyaw: 0.2
KIVx: 0
KIVy: 0
KIVz: 0.1
KIVroll: 0
KIVpitch: 0
KIVyaw: 0
KDVx: 0
KDVy: 0
KDVz: 0
KDVroll: 0
KDVpitch: 0
KDVyaw: 0
"""
