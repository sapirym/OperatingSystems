#!/bin/bash
# sapir yamin 316251818
if [ "$#" = 2 ]; then
	salary=0
	while read row
	do
		if echo $row | grep  "$1"; then
			arr=($row)			
			((salary = salary+${arr[2]}))
		fi
	done < "$2"
	echo Total balance: $salary
else
	echo "missing argument"
fi
