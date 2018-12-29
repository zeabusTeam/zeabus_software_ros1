# Zeabus-Elec-ROS-Power_dist
ROS node for power distribution. The power distribution (PDist) is register to ROS as
a subscriber (listener) to the topic named "/zeabus/elec/power_switch" the incoming
message is a 8-bit switch status (0 = off, 1 = on). As we have 8 available switch, 
we use only 1 byte for each request.

Moreover, the PDist is a publisher to the topic named "/zeabus/elec/hw_error", which
transport error message from any hardware driver. Any subscriber to this channel will
get the message only there are some error occurred. The block diagram of the PDist
can be shown as:

<pre>
 +-------------------+
 |                   |&lt;------ /zeabus/elec/power_switch ----
 | Power Distributor |
 |                   |------- /zeabus/elec/hw_error -------&gt;
 +-------------------+
 </pre>
