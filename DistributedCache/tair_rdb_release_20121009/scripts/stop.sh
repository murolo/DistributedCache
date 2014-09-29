#!/bin/bash

if (( $# != 1 )); then
    echo "Usage $0 [dataserver|configserver]"
    exit 1
fi
 
server_name=$1
 
case "${server_name}" in    
    "dataserver" )
        tair_stop_action="stop_ds"
        tair_bin="tair_server"
        ;;    
    "configserver" )    
        tair_stop_action="stop_cs"
        tair_bin="tair_cfg_svr"
        ;;    
    * )    
        echo "There is no this server [${server_name}]"    
        exit -1    
        ;;    
esac

this_file_path=$( dirname $0 )
echo "this file path is ${this_file_path}"

echo "Remove old pid file ..."
old_bin_pid=$( pidof ${tair_bin} )
old_pid_file="/home/s/var/proc/agile_${server_name}.${old_bin_pid}.pid"
if [ -f "${old_pid_file}" ]; then
    rm -f ${old_pid_file}
else 
    echo "pid file ${old_pid_file} does not exist"
fi

echo "Stop ${server_name} ..."
( cd ${this_file_path} && sh tair.sh ${tair_stop_action} )
sleep 3

new_bin_pid=$( pidof ${tair_bin} )
if [ -z "${new_bin_pid}" ]; then
    echo "STOP ${server_name} SUCCESS"
    echo "COMPLETED"
else
    echo "STOP ${server_name} FAILED"
    echo "EXIT"
fi
