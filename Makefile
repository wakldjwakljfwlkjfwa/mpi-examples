all: MPI1Proc4 ring MPI2Send16

MPI1Proc4: MPI1Proc4.c
	./mpicc ./MPI1Proc4.c -o MPI1Proc4

ring: ring.c
	./mpicc ./ring.c -o ring

MPI2Send16: MPI2Send16.c
	./mpicc ./MPI2Send16.c -o MPI2Send16

clean:
	rm MPI1Proc4 ring MPI2Send16
