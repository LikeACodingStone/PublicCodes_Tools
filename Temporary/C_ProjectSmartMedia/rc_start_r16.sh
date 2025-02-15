sleep 50
PROCESS=`ps -ef|grep playdemo|grep -v grep|grep -v PPID|awk '{ print $2}'`
for i in $PROCESS
do
  echo "Kill the $1 process [ $i ]"
  kill -9 $i
done
sleep 25

amixer cset numid=12,iface=MIXER,name='Lineout volume control' 60
sleep 5

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/mnt/ZHOU/lib
sleep 5
monitor_num=`ps -ef | grep "monitor_process" | grep -v grep | wc -l`
if [ $monitor_num -eq 0 ]
then 
    ./monitor_process &> /dev/null
    ./overDect.sh &	
fi


