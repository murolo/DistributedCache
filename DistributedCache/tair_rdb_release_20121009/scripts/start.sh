#!/bin/bash



if (( $# != 1 )); then
    echo "Usage $0 [dataserver|configserver]"
    exit 1
fi


this_file_path=$( dirname $0 )
echo "This file path [${this_file_path}]"


server_name=$1

case "${server_name}" in                 
    "dataserver" )
        tair_start_action="start_ds"
        tair_bin="tair_server"
        ;;                           
    "configserver" )                 
        tair_start_action="start_cs"
        tair_bin="tair_cfg_svr"
        ;;                           
    * )                              
        echo "There is no this server [${server_name}]"    
        exit -1                      
        ;;                           
esac  

echo "=====> Start server ..."
ret=$( cd ${this_file_path} && sh ./tair.sh ${tair_start_action} )
sleep 3
if (( 0 != ret )); then
    echo "Start ${server_name} FAILED" 
    echo "EXIT"
    exit 1
fi

echo "=====> Check new pid file ..."
new_bin_pid=$( cat ${this_file_path}/logs/server.pid )
new_pid_file=/home/s/var/proc/agile_${server_name}.${new_bin_pid}.pid
if [ -f "$new_pid_file" ] ; then
    echo "pid file ${new_pid_file} is OK"
else
    echo "pid file ${new_pid_file} does not exist"
fi

echo "Start ${server_name} SUCCESS" 
echo "COMPLETED"
