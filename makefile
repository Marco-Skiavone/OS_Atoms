CC = gcc
# flags defined by the assignment:
CFLAGS = -Wvla -Wextra -Werror
# var is the param to use 
var = 1
# can add other libraries below
OBJS = ./build/*lib.o
SOURCES = ./src/*.c
# application names
TARGET = ./bin/application.out
ATOM = ./bin/atom.out
ACT = ./bin/activator.out
PW_SUP = ./bin/power_supplier.out

$(OBJS): $(SOURCES)
	gcc $(CFLAGS) $(SOURCES) -c
	mv ./*.o ./build

all: $(OBJS)
	gcc $(OBJS) ./build/master.o -o $(TARGET)
	gcc $(OBJS) ./build/atom.o -o $(ATOM)
	gcc $(OBJS) ./build/activator.o -o $(ACTIVATOR) -lm
	gcc $(OBJS) ./build/power_supplier.o -o $(PW_SUP)

run: all
	./$(TARGET) $(var)

clear: ipcrm
	rm -f ./bin/*.out ./build/*.o ./--library=m

ipcrm:
	ipcrm --all
