#!/bin/bash

pref='^b'
suff='\.sh$'

counter=1

for file in *
do
	if [[  $file =~ $pref  ]] && !([[  $file =~ $suff  ]])
	then
		mv -f "$file" "bakhus$counter"
		(( counter++ )) 
	fi
done
