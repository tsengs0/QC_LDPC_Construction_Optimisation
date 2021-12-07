#!/bin/bash

a=$2
b=$3
entire_ber_filename=entire_ber.csv

if test -f ../BER.log/$1/$entire_ber_filename; then
	rm ../BER.log/$1/$entire_ber_filename
fi

for (( i=$a; i<=$b; i++ ))
do
	tail -n 1 ../BER.log/$1/$1_snr_$i.txt >> ../BER.log/$1/$entire_ber_filename
done
