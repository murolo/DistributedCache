#!/bin/bash

this_file_path=$( dirname $0 )
cd ${this_file_path}

base_dir=$1
server_name=$2
case "${server_name}" in
    "dataserver" ) 
        tair_bin="tair_server"
        ;;
    "configserver" )
        tair_bin="tair_cfg_svr"
        ;;
    * )
        echo "There is no this server [${server_name}]" 
        echo "===>DEPLOY FAILED"
        exit -1
        ;;
esac

echo "===> start to create install directory ..."
datetime=$(date +"%Y%m%d_%H%M%S")
tair_home_dir=${base_dir}/tair_rdb_${server_name}
install_dir=${tair_home_dir}/${datetime}
mkdir -p ${install_dir} 

if (( 0 != $? )); then
    echo "mkdir ${install_dir} failed"
    exit -1
fi

export TBLIB_ROOT=$(pwd)/../../tb-common-utils
echo "TBLIB_ROOT=${TBLIB_ROOT}"

echo "===> start to make install"
# back to agile/src/third_party/tair/tair_rdb_release_20121009
cd .. 
sh ./bootstrap.sh  
chmod u+x ./configure 
./configure && make && make install 

mv ~/tair_rdb_bin ${install_dir}

echo "===> start to create new run dir link ..."
run_dir_link=${tair_home_dir}/bin
if [ -L "${run_dir_link}" ] ; then
    rm -f ${run_dir_link}
fi
ln -s ${install_dir}/tair_rdb_bin ${run_dir_link}

echo "===>DEPLOY SUCCESS"
