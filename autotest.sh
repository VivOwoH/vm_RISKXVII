gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm vm_riskxvii.c
gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm instructions.c
gcc --coverage -c -Wall -Wvla -Werror -O0 -g -std=c11 -lm vr_err.c

gcc --coverage -fsanitize=address -o vm_riskxvii vm_riskxvii.o instructions.o vr_err.o -lm

echo "add_2_numbers:"
./vm_riskxvii tests/add_2_numbers.mi < tests/test.in | diff -su - tests/test.out

gcov vm_riskxvii.c
