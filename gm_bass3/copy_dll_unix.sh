#! /bin/bash

if test -z "$1"; then
	echo "Give parameter 1!"
	exit 1
fi

if test -z "$1"; then
	echo "Give parameter 2!"
	exit 1
fi

if test "$1" = "$2"; then
	echo "Parameter 1 and parameter 2 can not be the same!"
	exit 2
fi

if ! test -e "$1"; then
	echo "File $1 doesn't exist!"
	exit 3
fi

while true; do
	cp $1 $2
	
	if test $? -ge 1; then
		echo "Couldn't copy $1 to $2, retrying..."
		sleep 0.5
		continue
	fi
	
	break
done

exit 0