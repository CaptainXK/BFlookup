#! /bin/bash
if [ $# -lt 3 ]
then
	echo "parameter number error!\n"
	exit 1
fi
g++  -std=c++11 *.c -o test
chmod 777 test
./test $1 $2 $3