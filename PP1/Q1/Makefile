CC := gcc

LIB := -pthread



test: test.c
  
	${CC} -o  test.exe  test.c  ${LIB}


runtest: test.exe
	./test.exe

all: test runtest
