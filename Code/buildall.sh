#!/bin/bash

argv=$1
rootdir=$(pwd)
ddirs=`find  ./ -name "Build"`

params=

case $argv in
"clean")
	echo "begin make clean..."
	params="clean";;
*)
	echo "begin make ..."
	params=;;
esac


for dir in $ddirs
do
	cd $rootdir/$dir
	make $params
done

echo "end make ..."

