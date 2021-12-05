#!/bin/bash

a=$2
b=$3
for (( i=$a; i<=$b; i++ ))
do
	../ldpc_decoder 10 $i 0 > ../BER.log/$1/$1_snr_$i.txt &
done
