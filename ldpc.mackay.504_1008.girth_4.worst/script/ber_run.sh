#!/bin/bash

a=$2
b=$3
for (( i=$a; i<=$b; i++ ))
do
	../ldpc_decoder 100 $i 0 > ../BER.log/$1/$1.snr_$i.txt &
done
