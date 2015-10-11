#!/bin/bash

pref='^a'
suff='\.sh$'

for file in *
do
	if [[  $file =~ $pref  ]] && !([[  $file =~ $suff  ]])
	then
		rm "$file"
	fi
done
