#!/bin/bash

if sudo -n echo "Start set up zeabus package"
then
	sudo python2.7 setup.py install
else 
	echo "Require password for sudo permission"
	sudo python2.7 setup.py install
fi

exit
