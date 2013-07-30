#!/bin/bash

for ((i=$1; i>0; i--))
do
$2/ya-task -i $3  -t $4 >> log.txt 
done

exit 0
