#!/bin/bash

fileText=$(cat "./tests.txt")
iteration=0

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

path="../parser"
rm output_log.txt

for line in $fileText
do
  if [ $iteration = 0 ];
  then
    testCase="$line"
    iteration=1

  else
    echo "Test case $testCase:" >>output_log.txt

    ${path} <test_cases/${testCase} >/dev/null 2>>output_log.txt
    retvalue=$?

    if [ $line = $retvalue ];
    then
      echo "Test $testCase ${green}PASSED${reset} - returned $retvalue, expected $line"
      echo "Test $testCase PASSED - returned $retvalue, expected $line">>output_log.txt
    else
      echo "Test $testCase ${red}FAILED${reset} - returned $retvalue, expected $line"
      echo "Test $testCase FAILED - returned $retvalue, expected $line">>output_log.txt
    fi

    echo "===============================================">>output_log.txt
    echo " ">>output_log.txt
    iteration=0
  fi
done

