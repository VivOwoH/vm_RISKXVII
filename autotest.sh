gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm vm_riskxvii.c
gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm instructions.c
gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm func_err.c

gcc --coverage -fsanitize=address -o vm_riskxvii vm_riskxvii.o instructions.o func_err.o -lm

# echo "[example(NOT MY TESTCASES)]print_h:"
# ./vm_riskxvii tests/example/printing_h.mi | diff -su - tests/example/print_h.out
# echo "[example]5_sum:"
# ./vm_riskxvii tests/example/5_sum.mi < tests/example/5_sum.in| diff -su - tests/example/5_sum.out
# echo "[example]add_2_numbers:"
# ./vm_riskxvii tests/example/add_2_numbers.mi < tests/example/add_2_numbers.in| diff -su - tests/example/add_2_numbers.out
# echo "[example]add_2_numbers_with_function:"
# ./vm_riskxvii tests/example/add_2_numbers_withfunc.mi < tests/example/add_2_numbers.in| diff -su - tests/example/add_2_numbers.out
# echo "[example]hello_world:"
# ./vm_riskxvii tests/example/hello_world.mi | diff -su - tests/example/hello_world.out
# echo "[example]shift:"
# ./vm_riskxvii tests/example/shift.mi < tests/example/shift.in| diff -su - tests/example/shift.out

echo "add_sub:"
./vm_riskxvii tests/images/add_sub.mi < tests/arithmatic/add_sub.in | diff -su - tests/arithmatic/add_sub.out
echo "bitwise:"
./vm_riskxvii tests/images/bitwise.mi < tests/arithmatic/bitwise.in | diff -su - tests/arithmatic/bitwise.out
echo "shift:"
./vm_riskxvii tests/images/shift2.mi < tests/shift/shift.in | diff -su - tests/shift/shift.out
echo "branch:"
./vm_riskxvii tests/images/branch.mi < tests/arithmatic/branch.in | diff -su - tests/arithmatic/branch.out

gcov vm_riskxvii.c
gcov instructions.c
gcov func_err.c
# lcov -c --directory . --output-file main_coverage.info
# genhtml main_coverage.info --output-directory coverage_report
