#!/usr/bin/env bash

make clean && make

tests_failed=0
#TODO matov dojebany scanner (escape sequence) sample3.rb

echo "------------------STARTING TESTS----------------------"
echo ""

for file in ./programs/return_value_2/*; do
    echo "----------------------TEST $file----------------------"
    ./ifj18 < ${file}
    if [ $? -eq 201 ]; then
        echo "TEST PASSED"
    else

        echo "TEST FAILED"
        let tests_failed+=1
    fi
done

for file in ./programs/return_value_3/*; do
    echo "----------------------TEST $file----------------------"
    ./ifj18 < ${file}
    if [ $? -eq 3 ]; then
        echo "TEST PASSED"
    else

        echo "TEST FAILED"
        let tests_failed+=1
    fi
done

for file in ./programs/return_value_5/*; do
    echo "----------------------TEST $file----------------------"
    ./ifj18 < ${file}
    if [ $? -eq 5 ]; then
        echo "TEST PASSED"
    else

        echo "TEST FAILED"
        let tests_failed+=1
    fi
done

for file in ./programs/valid_programs/*; do
    echo "----------------------TEST $file----------------------"
    ./ifj18 < ${file}
    if [ $? -eq 0 ]; then
        echo "TEST PASSED"
    else

        echo "TEST FAILED"
        let tests_failed+=1
    fi
done


echo ""
echo "--------------------SUMMARY-----------------------"
echo ""
if [ $tests_failed -gt 0 ]; then
    echo "                 TESTS FAILED: "$tests_failed""
    echo ""
    echo "--------------------------------------------------"
    exit 1
else
    echo "                  TESTS PASSED                    "
    echo ""
    echo "--------------------------------------------------"
    exit 0
fi