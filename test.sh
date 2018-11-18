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
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 2----------------------"
./ifj18 < ./programs/valid_programs/2.rb
if [ $? -eq 0 ]; then
    echo "TEST 2 PASSED"
else
    echo "TEST 2 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 3----------------------"
./ifj18 < ./programs/return_value_3/1.rb
if [ $? -eq 3 ]; then
    echo "TEST 3 PASSED"
else
    echo "TEST 3 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 4----------------------"
./ifj18 < ./programs/valid_programs/3.rb
if [ $? -eq 0 ]; then
    echo "TEST 4 PASSED"
else
    echo "TEST 4 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 5----------------------"
./ifj18 < ./programs/valid_programs/4.rb
if [ $? -eq 0 ]; then
    echo "TEST 5 PASSED"
else
    echo "TEST 5 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 6----------------------"
./ifj18 < ./programs/valid_programs/5.rb
if [ $? -eq 0 ]; then
    echo "TEST 6 PASSED"
else
    echo "TEST 6 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 7----------------------"
./ifj18 < ./programs/valid_programs/6.rb
if [ $? -eq 0 ]; then
    echo "TEST 7 PASSED"
else
    echo "TEST 7 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 8----------------------"
./ifj18 < ./programs/valid_programs/7.rb
if [ $? -eq 0 ]; then
    echo "TEST 8 PASSED"
else
    echo "TEST 8 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"


echo "----------------------TEST 9----------------------"
./ifj18 < ./programs/valid_programs/8.rb
if [ $? -eq 0 ]; then
    echo "TEST 9 PASSED"
else
    echo "TEST 9 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"

echo "----------------------TEST 10----------------------"
./ifj18 < ./programs/valid_programs/9.rb
if [ $? -eq 0 ]; then
    echo "TEST 10 PASSED"
else
    echo "TEST 10 FAILED"
    let tests_failed+=1
fi
echo "--------------------------------------------------"
echo "----------------------TEST 11----------------------"
./ifj18 < ./programs/valid_programs/10.rb
if [ $? -eq 0 ]; then
    echo "TEST 11 PASSED"
else
    echo "TEST 11 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 12----------------------"
./ifj18 < ./programs/return_value_5/1.rb
if [ $? -eq 5 ]; then
    echo "TEST 12 PASSED"
else
    echo "TEST 12 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 13----------------------"
./ifj18 < ./programs/return_value_5/2.rb
if [ $? -eq 5 ]; then
    echo "TEST 13 PASSED"
else
    echo "TEST 13 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 14----------------------"
./ifj18 < ./programs/return_value_5/3.rb
if [ $? -eq 5 ]; then
    echo "TEST 14 PASSED"
else
    echo "TEST 14 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 15----------------------"
./ifj18 < ./programs/return_value_2/1.rb
if [ $? -eq 201 ]; then
    echo "TEST 15 PASSED"
else
    echo "TEST 15 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 16----------------------"
./ifj18 < ./programs/valid_programs/12.rb
if [ $? -eq 0 ]; then
    echo "TEST 16 PASSED"
else
    echo "TEST 16 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 17----------------------"
./ifj18 < ./programs/valid_programs/11.rb
if [ $? -eq 0 ]; then
    echo "TEST 17 PASSED"
else
    echo "TEST 17 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 18----------------------"
./ifj18 < ./programs/return_value_5/4.rb
if [ $? -eq 5 ]; then
    echo "TEST 18 PASSED"
else
    echo "TEST 18 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 19----------------------"
./ifj18 < ./programs/return_value_5/5.rb
if [ $? -eq 5 ]; then
    echo "TEST 19 PASSED"
else
    echo "TEST 19 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 20----------------------"
./ifj18 < ./programs/return_value_6/1.rb
if [ $? -eq 6 ]; then
    echo "TEST 20 PASSED"
else
    echo "TEST 20 FAILED"
    let tests_failed+=1
fi

echo "----------------------TEST 21----------------------"
./ifj18 < ./programs/return_value_3/2.rb
if [ $? -eq 3 ]; then
    echo "TEST 21 PASSED"
else
    echo "TEST 21 FAILED"
    let tests_failed+=1
fi

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