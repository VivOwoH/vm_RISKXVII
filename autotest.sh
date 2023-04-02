gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm vm_riskxvii.c
gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm instructions.c
gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm func_err.c

gcc --coverage -fsanitize=address -o vm_riskxvii vm_riskxvii.o instructions.o func_err.o -lm

echo "[sample(NOT MY TESTCASE)]print_h:"
./vm_riskxvii tests/images/printing_h.mi | diff -su - tests/print_h.out

gcov vm_riskxvii.c
# lcov -c --directory . --output-file main_coverage.info
# genhtml main_coverage.info --output-directory coverage_report
