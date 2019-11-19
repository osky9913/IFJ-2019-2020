testcase=test3
program=../bin/test_scanner_id
input=../inputs/example3.src;
touch temp.txt;
touch temp2.txt;
touch ../results/${testcase}.txt;
output=temp.txt;
output2=temp2.txt;
result=../results/${testcase}.txt;
valgrind ./${program} ${input} >${output} 2>&1;
./${program} ${input} >${output2};
grep "ERROR*" "$output">${result};
grep "total heap usage" "$output">>${result};
diff ${output2} "../correct_results/${testcase}_correct.txt" >> ${result};
mv ${output2} ../results/${testcase}_valgrind.txt;
rm ${output};