#!/bin/sh
age=1
	#/home/avinashgs/foo/prex2/exer2/
if [ -f /tmp/avinashgs1/gbsd.log ]
then
rm /tmp/avinashgs1/gbsd.log
#echo "removed log file"
fi

while [ "$age" -gt 0 ]
do
pid=`ps -eaf| grep -i dmn |grep -v grep | tr -s ' '|cut -d " " -f2`
if [ -z $pid ]
then
#	echo "Deamon not running"
 age=0
 #exit function
 else 
 
 echo "process id is $pid and time(in secs) its running is $age" >> /tmp/avinashgs1/gbsd.log
  age=$(($age+1)) 
  sleep 1
 fi
 done
 
 
