#!/bin/bash
#
# File : build.sh
#
# init created: 2016-07-04
# last updated: 2016-07-04
#
########################################################################
_file=$(readlink -f $0)
_cdir=$(dirname $_file)
_name=$(basename $_file)

project_home=$(dirname $_cdir)

echo "**** project home: "$project_home

echo "**** build and start fastsync-server:"

cd $project_home && make clean && make && sudo make install && /usr/local/fastsync/fastsync-server