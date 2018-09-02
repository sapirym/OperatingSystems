#!/bin/bash
#sapir yamin 316251818
if [ "$#" = 1 ]; then
	if [ -f $1 ];	then
		mkdir -p safe_rm_dir
		cp $1 safe_rm_dir
		rm $1
		echo "done!"		
	else
		echo "error: there is no such file"
	fi	
else
	echo error: only one argument is allowed
	
fi

