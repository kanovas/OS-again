#!/bin/bash
rm -f result
# make Filedescriptor(FD) 6 a copy of stdout (FD 1)
exec 6>&1
# open file for writing
exec 1>result

grep Gnostic text

# close file
exec 1>&-
# make stdout a copy of FD 6 (reset stdout)
exec 1>&6
# close FD6
exec 6>&-
