#!/bin/bash

cd $(dirname $0)

export TBLIB_ROOT=$(pwd)/../tb-common-utils

tmpfile=/tmp/tt.t.$(date +%s)
(cat ~/.bash_profile | grep -v "TBLIB_ROOT=" ; echo "export TBLIB_ROOT=${TBLIB_ROOT}")  > ${tmpfile}
mv -f ${tmpfile} ~/.bash_profile
source ~/.bash_profile

(cd $TBLIB_ROOT && sh ./build.sh)

(sh ./bootstrap.sh && ./configure && make)

