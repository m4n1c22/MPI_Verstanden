SHELL=/bin/bash
PROG = gameOfLife
SRC = $(PROG).c CGOLF.c
EXE = $(PROG).exe
MPICXX = mpicxx
MPICC = mpicc
MPIEXEC = mpiexec
CC = gcc
CXX = g++

NPROCS = 5



CFLAGS=-I./opt/include -w -std=c99
LDFLAGS=./opt/lib/libz.a

FLAGS_OPENMP = -fopenmp 
all: clean release 

debug: $(SRC)
	$(MPICC) -g $(CFLAGS) $(FLAGS_OPENMP) $(FLAGS_DEBUG) $(SRC) -DDEBUG -o $(EXE) -lm $(LDFLAGS)

release: tools $(PROG).exe

$(PROG).exe: $(SRC)
	$(MPICC) -g $(CFLAGS) $(FLAGS_OPENMP) $(FLAGS_DEBUG) $(SRC) -o $(EXE) -lm $(LDFLAGS)

tools: gameOfLife_decompress.c gameOfLife_compress.c CGOLF.c
	$(CC) $(CFLAGS) -g gameOfLife_decompress.c CGOLF.c -o gameOfLife_decompress.exe -lm $(LDFLAGS)
	$(CC) $(CFLAGS) -g gameOfLife_compress.c CGOLF.c -o gameOfLife_compress.exe -lm $(LDFLAGS)

partest:
	@echo "Usage: make run [NPROCS=<#procs>]"
	${MPIEXEC} -np $(NPROCS) ulimitWrapper $(EXE) 10 10 tiny.gol 1 1 1

test-small:
	${MPIEXEC} -np $(NPROCS) $(EXE) 40 12 40x12_gliderGund.cgolf 4 1 100

test-large:
	${MPIEXEC} -np $(NPROCS) $(EXE) 1024 1024 medium.gof 100 1 50

test: $(PROG).exe
	cp ./data/square_10x10.cgolf .
	./$(PROG).exe 10 10 square_10x10.cgolf 5 5 0 1 2 3 4
	@i=0; while [[ $$i -le 4 ]]; do \
		./gameOfLife_decompress.exe 10 10 square_10x10.$$i.cgolf square_10x10.$$i.golf y; \
		((i = i + 1)); \
	done
	./gameOfLife_decompress.exe 10 10 square_10x10.end.cgolf square_10x10.end.golf y

mpitest: $(PROG).exe
	cp ./data/square_10x10.cgolf .
	$(MPIEXEC) -np $(NPROCS) ./$(PROG).exe 10 10 square_10x10.cgolf 5 5 0 1 2 3 4


clean:
	@rm -f $(EXE) core* *.o *.mod a.out 
	@rm -f $(PROG).otf $(PROG).*.z
	@rm -f gameOfLife_decompress.exe gameOfLife_compress.exe

