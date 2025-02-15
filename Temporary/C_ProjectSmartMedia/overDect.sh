#!/bin/sh  
while true
do 
	filename=mj.log  
	filesize=`ls -l $filename | awk '{ print $5 }'`  
	maxsize=$((1024 * 10))  
	if [ $filesize -gt $maxsize ]  
	then  
	    echo "" > $filename
	fi 
	sleep 2
done 
