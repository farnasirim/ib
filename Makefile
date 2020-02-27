.PHONY: hw clean all

all: clean hw

LIBS=-pthread -lrdmacm -libverbs -lmemcached

CC=gcc
C_FLAGS=-Wall -O2 # -Werror
C_SRCS=
C_OBJS=$(C_SRCS:.c=.o)

CXX=g++
CXX_FLAGS=-Wall -O2 # -Werror
CXX_SRCS=main.cc ib.cc
CXX_OBJS=$(CXX_SRCS:.cc=.o)

HEADER_FLAGS=-I.

.c.o:
	$(CC) $(C_FLAGS) $(HEADER_FLAGS) -c -o $@ $<

.cc.o:
	$(CXX) $(CXX_FLAGS) $(HEADER_FLAGS) -c -o $@ $<

hw: $(OBJS) $(CXX_OBJS)
	$(CXX) $(C_FLAGS) $(HEADER_FLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o hw
