CC := gcc

LIB := -fopenmp



example7_14_1: example7_14_1.c
  
	${CC} -o  example7_14_1.exe  example7_14_1.c  ${LIB}


example7_14_2: example7_14_2.c
  
	${CC} -o  example7_14_2.exe  example7_14_2.c  ${LIB}


example7_14_3: example7_14_3.c
  
	${CC} -o  example7_14_3.exe  example7_14_3.c  ${LIB}

GE_OPENMP: GE_OPENMP.c
  
	${CC} -o  GE_OPENMP.exe  GE_OPENMP.c  ${LIB}

all: example7_14_1 example7_14_2 example7_14_3 GE_OPENMP

clean:
	rm -f *.exe
