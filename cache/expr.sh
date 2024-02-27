#!/bin/bash

for ((i=100;i<=100;i+=100))
do
	cli="g++ main.c murmur3.c -o ./runtime/main -O2 -std=c++2a -Dn_topk=\($i\)"
	echo $cli
	eval $cli

	cli="./runtime/main > ./runtime/result_$i.txt &"
	echo $cli
	eval $cli
done

