CC := mpicc

mpi_point: mpi_point.c
  
	${CC} -o  mpi_point.exe  mpi_point.c


mpi_collective: mpi_collective.c
  
	${CC} -o  mpi_collective.exe  mpi_collective.c


all: mpi_point mpi_collective

clean:
	rm -f *.exe

