#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    travis_build=0
else
    travis_build=$1
fi

make clean && make

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

tests_failed=0
#TODO matov dojebany scanner (escape sequence) sample3.rb

echo -e "${NC}------------------STARTING TESTS----------------------"
echo ""

for file in ./programs/return_value_2/*; do
    echo -e "${NC}----------------------TEST $file----------------------"
    ./ifj18 < ${file} > temp_out
    if [ $? -eq 2 ]; then
        echo -e "${GREEN} TEST PASSED"
    else

        echo -e "${RED} TEST FAILED"
        let tests_failed+=1
    fi
done

for file in ./programs/return_value_3/*; do
    echo -e "${NC}----------------------TEST $file----------------------"
    ./ifj18 < ${file} > temp_out
    if [ $? -eq 3 ]; then
        echo -e "${GREEN} TEST PASSED"
    else

        echo -e "${RED} TEST FAILED"
        let tests_failed+=1
    fi
done

for file in ./programs/return_value_4/*; do
    echo -e "${NC}----------------------TEST $file----------------------"
    ./ifj18 < ${file} > temp_out
    if [[ ${travis_build} -eq 1 ]]; then
        docker run -ti -v $PWD:/test ubuntu:16.04 bash -c "cd /test/; ./ic18int temp_out"
    else
        ./ic18int temp_out > interpret_out
    fi

    if [[ $? -eq 4 ]]; then
        echo -e "${GREEN} TEST PASSED"
    else

        echo -e "${RED} TEST FAILED"
        let tests_failed+=1
    fi
done

for file in ./programs/return_value_5/*; do
    echo -e "${NC}----------------------TEST $file----------------------"
    ./ifj18 < ${file} > temp_out
    if [ $? -eq 5 ]; then
        echo -e "${GREEN} TEST PASSED"
    else

        echo -e "${RED} TEST FAILED"
        let tests_failed+=1
    fi
done

# TODO ked to mato spravi
#for file in ./programs/return_value_1/*; do
#    echo -e "${NC}----------------------TEST $file----------------------"
#    ./ifj18 < ${file} > temp_out
#    if [ $? -eq 1 ]; then
#        echo -e "${GREEN} TEST PASSED"
#    else
#
#        echo -e "${RED} TEST FAILED"
#        let tests_failed+=1
#    fi
#done

for file in ./programs/valid_programs/*; do
    echo -e "${NC}----------------------TEST $file----------------------"
    ./ifj18 < ${file} > temp_out
    if [ $? -eq 0 ]; then
        echo -e "${GREEN} TEST PASSED"
    else

        echo -e "${RED} TEST FAILED"
        let tests_failed+=1
    fi
done

for file in ./programs/gen_tests_programs/in/*; do
    echo -e "${NC}----------------------TEST GEN PROGRAMS $file----------------------"
    ./ifj18 < ${file} > temp_out
    if [ $travis_build -eq 1 ]; then
        docker run -ti -v $PWD:/test ubuntu:16.04 bash -c "cd /test/; ./ic18int temp_out" > interpret_out
    else
        ./ic18int temp_out > interpret_out
    fi

    filename=${file##*/}
    diff interpret_out ./programs/gen_tests_programs/out/"${filename%.*}.ifj" > temp_out
    if [ $? -eq 0 ]; then
        echo -e "${GREEN} TEST PASSED"
    else

        echo -e "${RED} TEST FAILED"
        let tests_failed+=1
    fi
done

echo ""
echo -e "${NC}--------------------SUMMARY-----------------------"
echo ""
if [ $tests_failed -gt 0 ]; then
    echo -e "${RED}                 TESTS FAILED: "$tests_failed""
    echo ""
    echo -e "${NC}--------------------------------------------------"
    exit 1
else
    echo -e "${GREEN}                 TESTS PASSED                    "
    echo ""
    echo -e "${NC}--------------------------------------------------"
    exit 0
fi