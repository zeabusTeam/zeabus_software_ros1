from manage_log import log

if __name__=='__main__':
	a = log("zeabus_planner" , "log" , "test")
	
	print("line 6")

	a.write("Hello GAP" , True , 1)
