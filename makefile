CC = gcc
# flags defined by the assignment:
CFLAGS = -Wvla -Wextra -Werror
# var is the param to use 
var = 1
# can add other libraries below
OBJS = *lib.o
SOURCES = *.c
# application names
TARGET = application
ATOM = atom
ACT = activator
PW_SUP = power_supplier

$(OBJS): $(SOURCES)
	gcc $(CFLAGS) $(SOURCES) -c

all: $(OBJS)
	gcc $(OBJS) master.o -o $(TARGET) -lm
	gcc $(OBJS) atom.o -o $(ATOM) -lm
	gcc $(OBJS) activator.o -o $(ACTIVATOR) -lm
	gcc $(OBJS) power_supplier.o -o $(PW_SUP) -lm

run: all
	./$(TARGET) $(var)

clear: ipcrm
	rm -f *.o $(TARGET) $(ATOM) $(ACTIVATOR) $(PW_SUP)

ipcrm:
	ipcrm --all
