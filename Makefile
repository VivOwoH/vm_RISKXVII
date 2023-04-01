TARGET = vm_riskxvii
TEST_TARGET = autotest.sh

CC = gcc

CFLAGS     = -c -Wall -Wvla -Werror -std=c11
OPT_FLAGS = -Os -s -fno-ident -fno-exceptions -fno-asynchronous-unwind-tables
LIB_FLAGS = -lm
SRC        = vm_riskxvii.c instructions.c func_err.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ) $(OPT_FLAGS) $(LIB_FLAGS)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(OPT_FLAGS) $(LIB_FLAGS) $<

run:
	./$(TARGET)

tests:
	echo "additional c file for tests":

# compile rule different in tests
run_tests:
	./$(TEST_TARGET)

clean:
	rm -f *.o *.obj *.gcno *.gcov *.gcda $(TARGET)