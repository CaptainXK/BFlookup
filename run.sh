#! /bin/bash
if [ $# -lt 2 ]
then
	echo "parameter number error!\n"
	exit 1
fi

g++ *.c -o test -g3
chmod 777 test
./test $1 $2