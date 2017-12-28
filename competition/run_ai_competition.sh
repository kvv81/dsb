#!/bin/bash

EXECUTABLE=../dsb
MATCHES=100000
ALGOS=algos.lst
PLACEMENTS=placements.lst
TMP=/tmp/dsb.$$

if [ ! -x $EXECUTABLE ] ; then
	echo "Cannot find DSB executable in the parent directory - you need to build it at the first !"
	exit 1
fi

function print_line()
{
	echo "======================================================================================================="
}

print_line

declare -a pl
i=0
while read p; do
	pl[$i]=$(echo "$p" | cut -d '|' -f 2)
	placement_name=$(echo "$p" | cut -d '|' -f 1)
	i=$((i + 1))
	echo "PLACEMENT #$i: $placement_name"
done < $PLACEMENTS

num_placement=${#pl[*]}
echo "Number of placements: $num_placement"

print_line

declare -a al
i=0
while read a; do
	al[$i]=$(echo "$a" | cut -d '|' -f 2)
	algo_name=$(echo "$a" | cut -d '|' -f 1)
	i=$((i + 1))
	echo "ALGO #$i: $algo_name"
done < $ALGOS

num_algos=${#al[*]}
echo "Number of algos: $num_algos"

print_line

declare -a placement_worst_case
declare -a placement_worst_case_algo_index
declare -a algo_worst_case
declare -a algo_worst_case_placement_index

for (( i=0; i < $num_placement; i++ ))
do
	placement_worst_case[$i]=1000
done


for (( j=0; j < $num_algos; j++ ))
do
	algo_worst_case[$j]=0
done

for (( i=0; i < $num_placement; i++ ))
do
	for (( j=0; j < $num_algos; j++ ))
	do
		echo -n "Placement #$((i+1)) VS algo #$((j+1)):      "
		/usr/bin/time -p $EXECUTABLE -v none -n $MATCHES ${pl[$i]} ${al[$j]} >$TMP 2>&1
		output=$(grep average $TMP | head -n1)
		tm=$(grep real $TMP | cut -d ' ' -f 2)
		echo -e "$output, \telapsed_time=${tm}s"
		cnt=$(cat $TMP | grep average | head -n1 | egrep -o 'average=.*$' | cut -d = -f 2)

		is_less=$(echo $cnt '<' ${placement_worst_case[$i]} | bc -l)
		if [ $is_less -eq 1 ] ; then
			placement_worst_case[$i]=$cnt
			placement_worst_case_algo_index[$i]=$j
		fi

		is_greater=$(echo $cnt '>' ${algo_worst_case[$j]} | bc -l)
		if [ $is_greater -eq 1 ] ; then
			algo_worst_case[$j]=$cnt
			algo_worst_case_placement_index[$j]=$i
		fi
	done
done

print_line

for (( i=0; i < $num_placement; i++ ))
do
	echo "Placement #$((i+1)) has worst-case score ${placement_worst_case[$i]}"
done

winner_placement_index=0
for (( i=1; i < $num_placement; i++ ))
do
	is_greater=$(echo ${placement_worst_case[$i]} '>' ${placement_worst_case[$winner_placement_index]} | bc -l)
	if [ $is_greater -eq 1 ] ; then
		winner_placement_index=$i
	fi
done

winner_placement_name=$(head -n $((winner_placement_index+1)) $PLACEMENTS | tail -n1 | cut -d '|' -f 1)
echo "Placement-winner is #$((winner_placement_index+1)) (${winner_placement_name})- it has maximal worst-case score ${placement_worst_case[$winner_placement_index]}"

print_line

for (( j=0; j < $num_algos; j++ ))
do
	echo "Algo #$((j+1)) has worst-case score ${algo_worst_case[$j]}"
done

winner_algo_index=0
for (( j=1; j < $num_algos; j++ ))
do
	is_less=$(echo ${algo_worst_case[$j]} '<' ${algo_worst_case[$winner_algo_index]} | bc -l)
	if [ $is_less -eq 1 ] ; then
		winner_algo_index=$i
	fi
done

winner_algo_name=$(head -n $((winner_algo_index+1)) $ALGOS | tail -n1 | cut -d '|' -f 1)
echo "Algo-winner is #$((winner_algo_index+1)) (${winner_algo_name}) - it has minimal worst-case score ${algo_worst_case[$winner_algo_index]}"

#rm -f $TMP
