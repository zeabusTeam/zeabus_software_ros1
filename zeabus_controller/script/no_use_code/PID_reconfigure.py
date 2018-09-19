#!/usr/bin/env python

PACKAGE = 'zeabus_control'
import roslib;roslib.load_manifest(PACKAGE)
import rospy
import rospkg


from dynamic_reconfigure.server import Server
import dynamic_reconfigure.client
from zeabus_control.cfg import PIDConfig

import yaml
from SPID import SPID


AXIS = [ "X","Y","Z","r","p","y"]
TYPE = ["KP","KV"]


class PID_reconfigure():

    def __init__(self,filename,node,KP=[],KV=[]):

        self.KP = KP
        self.KV = KV


        self.file_name = filename


        self.data = self.load()
        self.srv = Server(PIDConfig, self.server_callback)

        print "START",node
        self.client = dynamic_reconfigure.client.Client(node, timeout=30, config_callback=self.client_callback)


        self.update(self.data)

        print "FINISH"


    def server_callback(self,data,level):


        cfg = dict(data)
        
        del data['groups']

        print "==="*10
        K_list = []
        for t in TYPE:
            K_list.append([])
            for a in AXIS:
                K_list[-1].append([])
                for k in "PID":
                    key = "%s_%s_%s"%(t,a,k)
                    #print key,data[key]
                    K_list[-1][-1].append(data[key])
                #print 

        KP , KV = K_list 

        for i in xrange(len(self.KP)):
            self.KP[i].set(*KP[i])

        for i in xrange(len(self.KV)):
            self.KV[i].set(*KV[i])
        

        print "SAVE : ",data['save']
        if data['save']:

            data['save']=False
            print "SAVED !!"
            with open(self.file_name, 'w') as f:
                yaml.dump(data,f, default_flow_style=True) 

        cfg['save']=False
        return cfg

    def client_callback(self,data):


        pass#print data

    def update(self,data):
        if data:
            self.client.update_configuration(data)


    def load(self):
        try:
            with open(self.file_name, 'r') as f:
                return yaml.load(f)

        except yaml.YAMLError as exc:
            return None
        except:
            return None


if __name__ == "__main__":


    node  = "PID"
    rospy.init_node(node, anonymous = False)
    file_name = rospkg.RosPack().get_path('zeabus_control')+"/param/PID.yaml"
    PID = PID_reconfigure(file_name,node,[SPID()])

    r = rospy.Rate(3)

 

    rospy.spin()