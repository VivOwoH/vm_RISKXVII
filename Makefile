TARGET = vm_riskxvii
TEST_TARGET = autotest.sh

CC = gcc

CFLAGS     = -c -s -Wall -Wvla -Werror -Os -std=c11
OPT_FLAGS = -fno-ident -fno-exceptions -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -Wl,--gc-sections 
LIB_FLAGS = -lm
SRC        = vm_riskxvii.c instructions.c func_err.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ) $(LIB_FLAGS)

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