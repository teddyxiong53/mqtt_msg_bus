.PHONY: all homeapp audioservice clean
all: homeapp audioservice

CFLAGS = -I $(HOME)/include
LDFLAGS = -L $(HOME)/lib -lmosquitto -lpthread


audioservice:
	gcc -c audioservice.c $(CFLAGS) -o audioservice.o
	gcc audioservice.o  $(LDFLAGS) -o audioservice


homeapp:
	gcc -c homeapp.c $(CFLAGS) -o homeapp.o
	gcc homeapp.o  $(LDFLAGS) -o homeapp
clean:
	rm -rf *.o homeapp audioservice 