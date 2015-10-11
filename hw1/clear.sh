#!/bin/bash

suff='\.sh$'
for file in *
do
	if !([[  $file =~ $suff  ]])
	then
		rm "$file"
	fi
done
