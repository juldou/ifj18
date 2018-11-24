#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    travis_build=0
else
    travis_build=$1
fi

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

for file in ./programs/gen_tests_programs/in/*; do
    echo "----------------------TEST GEN PROGRAMS $file----------------------"
    ./ifj18 < ${file} > temp_out
    if [ $travis_build -eq 1 ]; then
        docker run -ti -v $PWD:/test ubuntu:16.04 bash -c "cd /test/; ./ic18int temp_out" > interpret_out
    else
        ./ic18int temp_out > interpret_out
    fi

    filename=${file##*/}
    diff interpret_out ./programs/gen_tests_programs/out/"${filename%.*}.ifj"
    if [ $? -eq 0 ]; then
        echo "TEST PASSED"
    else

        echo "TEST FAILED"
        let tests_failed+=1
    fi
    let cntr+=1
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