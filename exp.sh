#!/bin/bash
exe='./evac';
dir='./experiments';

pedestrians=500;
smoke=3;
simulations=1;
delay=0;
opt="-t $delay -p $pedestrians -s $smoke -r $simulations";

cases=(
    #'D1.bmp'
    #'D2.bmp'
    #'D3.bmp'
    #'D4.bmp'
    'D5.bmp'
    #'D5-3.bmp'
    #'D5-4.bmp'
    #'E1.bmp'
    #'E2.bmp'
    #'E3.bmp'
    #'E4.bmp'
    'E5.bmp'
);

for (( i=0; i<${#cases[@]}; i++ ));
do
	echo "*********************************************************";
	echo ${cases[$i]} :;
	eval $exe $dir/${cases[$i]} $opt;
done
