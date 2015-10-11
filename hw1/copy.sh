#!/bin/bash

counter=1
pref='^c'
suff='\.sh$'

for file in *
do
        if [[  $file =~ $pref  ]] && !([[  $file =~ $suff  ]])
	then
		cp -f "$file" "crishna$counter"
		(( counter++ )) 
	fi
done
