#!/bin/bash
exe='./evac';
dir='./experiments';

pedestrians=400;
smoke=5;
simulations=5;
opt="-t 0 -p $pedestrians -s $smoke -r $simulations";

cases=(
	'D-1.bmp' 'D-2.bmp' 'D-3.bmp' 'D-4.bmp' 'D-5.bmp'
	#'E-1.bmp' 'E-2.bmp' 'E-3.bmp' 'E-4.bmp' 'E-5.bmp'
);

for (( i=0; i<${#cases[@]}; i++ ));
do
	echo "*********************************************************";
	echo ${cases[$i]} :;
	eval $exe $dir/${cases[$i]} $opt;
done