#!/usr/bin/env bash

make clean && make

tests_failed=0

echo "------------------STARTING TESTS----------------------"
echo ""

echo "----------------------TEST 1----------------------"
./ifj18 < ./programs/valid_programs/1.rb
if [ $? -eq 0 ]; then
    echo "TEST 1 PASSED"
else
    echo "TEST 1 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 2----------------------"
./ifj18 < ./programs/valid_programs/2.rb
if [ $? -eq 0 ]; then
    echo "TEST 2 PASSED"
else
    echo "TEST 2 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 3----------------------"
./ifj18 < ./programs/return_value_3/1.rb
if [ $? -eq 3 ]; then
    echo "TEST 3 PASSED"
else
    echo "TEST 3 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 4----------------------"
./ifj18 < ./programs/valid_programs/3.rb
if [ $? -eq 0 ]; then
    echo "TEST 4 PASSED"
else
    echo "TEST 4 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 5----------------------"
./ifj18 < ./programs/valid_programs/4.rb
if [ $? -eq 0 ]; then
    echo "TEST 5 PASSED"
else
    echo "TEST 5 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 6----------------------"
./ifj18 < ./programs/valid_programs/5.rb
if [ $? -eq 0 ]; then
    echo "TEST 6 PASSED"
else
    echo "TEST 6 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 7----------------------"
./ifj18 < ./programs/valid_programs/6.rb
if [ $? -eq 0 ]; then
    echo "TEST 7 PASSED"
else
    echo "TEST 7 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 8----------------------"
./ifj18 < ./programs/valid_programs/7.rb
if [ $? -eq 0 ]; then
    echo "TEST 8 PASSED"
else
    echo "TEST 8 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

# doesn't work yet
echo "----------------------TEST 9----------------------"
./ifj18 < ./programs/valid_programs/8.rb
if [ $? -eq 0 ]; then
    echo "TEST 9 PASSED"
else
    echo "TEST 9 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 10----------------------"
./ifj18 < ./programs/valid_programs/9.rb
if [ $? -eq 0 ]; then
    echo "TEST 10 PASSED"
else
    echo "TEST 10 FAILED"
    tests_failed=1
fi
echo "--------------------------------------------------"
echo "----------------------TEST 11----------------------"
./ifj18 < ./programs/valid_programs/10.rb
if [ $? -eq 0 ]; then
    echo "TEST 11 PASSED"
else
    echo "TEST 11 FAILED"
    tests_failed=1
fi

echo ""
echo "--------------------SUMMARY-----------------------"
echo ""
if [ $tests_failed -eq 1 ]; then
    echo "                  TESTS FAILED                    "
else
    echo "                  TESTS PASSED                    "
fi
echo ""
echo "--------------------------------------------------"

exit $tests_failed


