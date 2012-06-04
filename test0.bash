#!/bin/bash
i=1;
while [ $i -le 1048576 ] ; do
	if [ $i == 1 ]
	then
	BASE_ER=$(./id3 um $1 $1 $2 $i | grep "Error rate before pruning:")
	BASE_LINES=$(./id3 um $1 $1 $2 0 | wc -l)
	printf "%s " $BASE_ER
	printf "\n"
	fi
	ER=$(./id3 um $1 $1 $2 $i | grep "Error rate after pruning:")
	LINES=$(./id3 um $1 $1 $2 $i | wc -l)
	printf "%s " $ER
	DIFF=$[ BASE_LINES-LINES ]
	printf "  Cut nodes: %d\n" $DIFF
	i=$[i*2]
done
