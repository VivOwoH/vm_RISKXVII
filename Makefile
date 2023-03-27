TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -c -Os -s -fno-ident -fno-asynchronous-unwind-tables -Wall -Wvla -Werror -O0 -std=c11
SRC        = vm_riskxvii.c instructions.c vr.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(ASAN_FLAGS) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(ASAN_FLAGS) $<

run:
	./$(TARGET)

test:
	echo what are we testing?!

clean:
	rm -f *.o *.obj $(TARGET)
