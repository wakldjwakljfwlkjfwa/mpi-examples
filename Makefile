all: MPI1Proc4 ring MPI2Send16 MPI3Coll10 MPI4Type14 sem4 decart MPI5Comm14

MPI1Proc4: MPI1Proc4.c
	./mpicc ./MPI1Proc4.c -o MPI1Proc4

ring: ring.c
	./mpicc ./ring.c -o ring

MPI2Send16: MPI2Send16.c
	./mpicc ./MPI2Send16.c -o MPI2Send16

MPI3Coll10: MPI3Coll10.c
	./mpicc ./MPI3Coll10.c -o MPI3Coll10

MPI4Type14: MPI4Type14.c
	./mpicc ./MPI4Type14.c -o MPI4Type14

MPI5Comm14: MPI5Comm14.c
	./mpicc ./MPI5Comm14.c -o MPI5Comm14

mat2vec: mat2vec.c
	./mpicc ./mat2vec.c -o mat2vec

sem4: sem4.c
	./mpicc ./sem4.c -o sem4

decart: decart.c
	./mpicc ./decart.c -o decart

clean:
	rm MPI1Proc4 ring MPI2Send16 MPI3Coll10 MPI4Type14 sem4 decart MPI5Comm14
