#!/bin/bash
output=${1}
for i in "tests/bin/"*;do
	if [ $i == tests/bin/test_scanner ];then
		for j in "examples/"*; do
			valgrind ./$i $j >>${output} 2>&1;
		done
	fi
	if [ $i != tests/bin/placeholder.txt ];then	
		valgrind ./$i >>${output} 2>&1;
	fi
done

grep -q "ERRORS SUMMARY*" "$output"
