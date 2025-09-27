static const char help[] = "Parallel reordering algorithms and parallel direct solvers.\n";

#include <petsc.h>
#include "solver_utils.h"

//========================================================
int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    Mat     A;
    Vec     b, x;
    MatType mat_type = MATMPIAIJ;
    VecType vec_type = VECMPI;

    // logging setup
    PetscCall( PetscLogDefaultBegin() );

    PetscLogStage stage_reorder, stage_factor, stage_solve;
    PetscCall( PetscLogStageRegister("Reorder", &stage_reorder) );
    PetscCall( PetscLogStageRegister("Factorization", &stage_factor) );
    PetscCall( PetscLogStageRegister("Solve", &stage_solve) );

    const char *input_mat_file = "../matrices/bin/08_s_gyro_k.bin";
    const char *input_rhs_file = ""; // "../matrices/bin/09_u_powersim_b.bin";

    // prepare the system matrix and RHS vector
    PetscCall( load_matrix(input_mat_file, &A, mat_type) );

    if ( input_rhs_file && input_rhs_file[0] ) {
        PetscCall( load_vector(input_rhs_file, &b, vec_type) );
    } else {
        PetscCall( generate_rhs(A, &b, vec_type) );
    }

    // show matrix info (rank 0 only)
    PetscCall( matrix_info(A) );

    // solve the system
    PetscCall( solve_system(
        A, b, &x,
        MATORDERINGNATURAL,
        PCLU,
        MATSOLVERSUPERLU_DIST,
        stage_reorder, stage_factor, stage_solve
    ));



    // validate solution: compute ||Ax - b||_2
Vec       r;
PetscReal norm;

PetscCall( VecDuplicate(b, &r) );           // r = Ax - b
PetscCall( MatMult(A, x, r) );              // r = A * x
PetscCall( VecAXPY(r, -1.0, b) );           // r = r - b
PetscCall( VecNorm(r, NORM_2, &norm) );     // norm = ||r||_2

PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Residual norm ||Ax - b||_2 = %.6e\n", norm) );

PetscCall( VecDestroy(&r) );



PetscInt rstart, rend, loc_rows, loc_nnz = 0;
PetscMPIInt rank;
PetscInt ncols;
const PetscInt *cols;
const PetscScalar *vals;

PetscCall(MatGetOwnershipRange(A, &rstart, &rend));

for (PetscInt i = rstart; i < rend; i++) {
    PetscCall(MatGetRow(A, i, &ncols, &cols, &vals));
    loc_nnz += ncols;
    PetscCall(MatRestoreRow(A, i, &ncols, &cols, &vals));
}

PetscCallMPI(MPI_Comm_rank(PETSC_COMM_WORLD, &rank));
loc_rows = rend - rstart;

PetscSynchronizedPrintf(PETSC_COMM_WORLD,
    "[%d] owns rows %d to %d (total %d rows), local nnz = %d\n",
    rank, rstart, rend-1, loc_rows, loc_nnz);
PetscSynchronizedFlush(PETSC_COMM_WORLD, PETSC_STDOUT);




    // cleanup
    VecDestroy(&x);
    VecDestroy(&b);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
