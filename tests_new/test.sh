#!/bin/bash

nl="
"
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`


compileTests=$(cat "./tests.txt")
generateTests=$(cat "./tests_generate.txt")
iteration=0
failCnt=0
passCnt=0
failures=""

complier="../parser"
interpreter="../ic19int"
rm output_log.txt

for line in $compileTests
do
  if [ $iteration = 0 ];
  then
    testCase="$line"
    iteration=1

  else
    echo "Test case $testCase:" >>output_log.txt

    ${complier} <test_cases/${testCase} >/dev/null 2>>output_log.txt
    retvalue=$?

    if [ $line = $retvalue ];
    then
      echo "Test $testCase ${green}PASSED${reset} - returned $retvalue, expected $line"
      echo "Test $testCase PASSED - returned $retvalue, expected $line">>output_log.txt
      passCnt=$((passCnt+1))
    else
      echo "Test $testCase ${red}FAILED${reset} - returned $retvalue, expected $line"
      echo "Test $testCase FAILED - returned $retvalue, expected $line">>output_log.txt
      failCnt=$((failCnt+1))
      failures=${failures}$(echo "Test $testCase ${red}FAILED${reset} - returned $retvalue, expected $line")${nl}
    fi

    echo "===============================================">>output_log.txt
    echo " ">>output_log.txt
    iteration=0
  fi
done

echo "==================END OF STATIC ANALYSIS=====================${nl}"
echo "==================END OF STATIC ANALYSIS=====================${nl}"
echo "==================END OF STATIC ANALYSIS=====================${nl}"

for line in $generateTests
do
  if [ $iteration = 0 ];
  then
    testCase="$line"
    iteration=1

  else
    echo "Test case $testCase:" >>output_log.txt

    ${complier} <test_cases_generate/${testCase} >tmp.out 2>>output_log.txt
    ${interpreter} tmp.out
    retvalue=$?

    if [ $line = $retvalue ];
    then
      echo "Test $testCase ${green}PASSED${reset} - returned $retvalue, expected $line"
      echo "Test $testCase PASSED - returned $retvalue, expected $line">>output_log.txt
      passCnt=$((passCnt+1))
    else
      echo "Test $testCase ${red}FAILED${reset} - returned $retvalue, expected $line"
      echo "Test $testCase FAILED - returned $retvalue, expected $line">>output_log.txt
      failCnt=$((failCnt+1))
      failures=${failures}$(echo "Test $testCase ${red}FAILED${reset} - returned $retvalue, expected $line")${nl}

    fi

    echo "${nl}"
    echo "===============================================">>output_log.txt
    echo " ">>output_log.txt
    iteration=0
  fi
done

echo "==================FAILURES=====================${nl}"
echo "${failures}"
echo "==============================================="
echo "Done. ${green}$passCnt PASSED${reset}, ${red}$failCnt FAILED${reset}"
