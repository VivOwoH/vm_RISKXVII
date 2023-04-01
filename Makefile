TARGET = vm_riskxvii
TEST_TARGET = autotest.sh

CC = gcc

CFLAGS     = -c -Os -s -fno-ident -fno-exceptions -fno-asynchronous-unwind-tables -Wall -Wvla -Werror -O0 -std=c11
LIB_FLAGS = -lm
SRC        = vm_riskxvii.c instructions.c vr_err.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB_FLAGS)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(LIB_FLAGS) $<

run:
	./$(TARGET)

tests:
	echo "additional c file for tests":

run_tests:
	./$(TEST_TARGET)

clean:
	rm -f *.o *.obj *.gcno *.gcov *.gcda $(TARGET)