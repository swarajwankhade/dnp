#!/bin/sh
for i in ` ipcs -m | awk '{print $2}'  `
do 
	ipcrm -m $i > /dev/null 2>&1
done
