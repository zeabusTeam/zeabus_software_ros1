<h1> Zeabus Extension </h1>
&ensp;In this file will tell you all function and how to use all that
<br/><br/>
<h2>1. zeabus_extension/convert_to_string.h </h2>
&ensp; This file use to convert any type to string<br/>
&ensp; You can edit by use overload function
&ensp;<h3>convert<h3/>
&ensp;&ensp;<li>std::string to_string( data) <br/>
&ensp;&ensp;&ensp;this use convert to type string <br/>
&ensp;&ensp;&ensp;first arguments is data <br/>
&ensp;&ensp;&ensp;&ensp;include type is posix_time::ptime, gregorian::data, int, float, char</li>
&ensp;&ensp;<li>std::string edit_space( data , key) receive two arguments<br/>
&ensp;&ensp;&ensp;first is string want to edit syntax {' ', ':', ',', '-', '.', '\_', ';'} <br/>
&ensp;&ensp;&ensp;second is string key want to replace on syntax</li>
&ensp;&ensp;<li>bool in_set( data ) reseivce char for check that char in syntax or not</li> 
<br/><br/>
<h2>2. zeabus_extension/time.h </h2>
&ensp; This file use library posix_time to tell you about time<br/>
&ensp;<h3>world_time::time<h3/>
&ensp:&ensp;This class will tell you about time accuracy in second<br/>
&ensp;&ensp;<li>time( print_test)</li>
&ensp;&ensp;&ensp;first arguments it for test don't worry about that<br/>
&ensp;&ensp;<li>std::string local_time()</li>
&ensp;&ensp;&ensp;this function will return string of time on your pc<br/>
&ensp;&ensp;<li>std::string unversal_time()</li>
&ensp;&ensp;&ensp;this function will return string of time UTC +0.0<br/>
&ensp;&ensp;<li>void get_time()</li>
&ensp;&ensp;&ensp;for save a new time or current time<br/>
&ensp;<h3>count_time::time</h3>
&ensp;&ensp;This class is original time class for accuracy in micro<br/>
&ensp;&ensp;<li>time ( print_test )</li>
&ensp;&ensp;&ensp; first arguments it for test don't worry about that<br/>
&ensp;&ensp;<li> void reset_time()</li>
&ensp;&ensp;&ensp; this function for reset time start to calculate time<br/>
&ensp;&ensp;<li> boost::posix_time::ptime now()</li>
&ensp;&ensp;&ensp;this function will return current local time<br/>
&ensp;&ensp;<li> void find_different()</li>
&ensp;&ensp;&ensp; this use with other class<br/>
&ensp;<h3>count_time::{ second, millisecond, microsecond, nanosecond}</h3>
&ensp;&ensp; this class have similar count_time::time but that tell about accuracy<br/>
&ensp;&ensp;<li> long different( reset)</li>
&ensp;&ensp;&ensp; this function will return about different after use reset_time()<br/>
&ensp;&ensp;&ensp; first is boolean if yes will reset_time after calculate (default = true)<br/>
&ensp;&ensp;<li> void sleep( data )</li>
&ensp;&ensp;&ensp; this function is use for sleep<br/>
