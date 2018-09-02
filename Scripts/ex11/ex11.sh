#!/bin/bash
#sapir yami 316251818
number=0
if [ "$#" != 0 ] 
then
cd $1
fi
number=$(ls $1 | grep ".txt$" | wc -l)
echo Number of files in the directory that end with .txt is $number
