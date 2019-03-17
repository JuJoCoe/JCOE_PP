CC := gcc

LIB := -lpthread



example73: example73.c
  
	${CC} -o  example73.exe  example73.c  ${LIB}


example77: example77.c
	${CC} -o  example77.exe  example77.c  ${LIB}


gaussian_pthread: gaussian_pthread.c
	${CC} -o  gaussian_pthread.exe  gaussian_pthread.c  ${LIB}


all: example73 example77 gaussian_pthread

clean:
	rm -f *.exe
