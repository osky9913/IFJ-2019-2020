#!/bin/bash
for i in "tests/bin/"*;do
	if [ $i == tests/bin/test_scanner ];then
		for j in "examples/"*.src; do
			valgrind ./$i $j;
		done
	fi
	valgrind ./$i;
done
