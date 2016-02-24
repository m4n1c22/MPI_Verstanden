/**
  Defining a custom data type for 4*n X 4*n matrix which extends to only N diagonals of the matrix.
*/

#define DIM_N N
#define MAT_DIM 4*N

/** Defining using MPI_Type_create_indexed_block*/
MPI_Datatype first_diagonal,second_diagonal,third_diagonal,fourth_diagonal;

/** Defining Blocklength */

int blocklen = 1;

int displacements[DIM_N];

//initialize displacements[0,1....,i,...N-1] = {0,1*MAT_DIM+1,.,i*MAT_DIM+i...,(N-1)*MAT_DIM+(N-1)}

MPI_Type_create_indexed_block(DIM_N,blocklen,displacements,MPI_DOUBLE,&first_diagonal);
MPI_Type_commit(first_diagonal);

MPI_Type_create_indexed_block(1,blocklen,(N)*MAT_DIM+(N),first_diagonal,&second_diagonal);
MPI_Type_commit(second_diagonal);

MPI_Type_create_indexed_block(1,blocklen,(2*N)*MAT_DIM+(2*N),first_diagonal,&third_diagonal);
MPI_Type_commit(third_diagonal);

MPI_Type_create_indexed_block(1,blocklen,(3*N)*MAT_DIM+(3*N),first_diagonal,&fourth_diagonal);
MPI_Type_commit(fourth_diagonal);

....//sending diagonal info to 4 processes using each diagonal datatypes

MPI_Type_free(first_diagonal);
MPI_Type_free(second_diagonal);
MPI_Type_free(third_diagonal);
MPI_Type_free(fourth_diagonal);
