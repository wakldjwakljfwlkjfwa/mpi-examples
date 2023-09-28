all: MPI1Proc4 ring MPI2Send16 MPI3Coll10

MPI1Proc4: MPI1Proc4.c
	./mpicc ./MPI1Proc4.c -o MPI1Proc4

ring: ring.c
	./mpicc ./ring.c -o ring

MPI2Send16: MPI2Send16.c
	./mpicc ./MPI2Send16.c -o MPI2Send16

MPI3Coll10: MPI3Coll10.c
	./mpicc ./MPI3Coll10.c -o MPI3Coll10

mat2vec: mat2vec.c
	./mpicc ./mat2vec.c -o mat2vec

clean:
	rm MPI1Proc4 ring MPI2Send16
