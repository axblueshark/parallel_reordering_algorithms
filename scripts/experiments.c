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

    const char *input_mat_file = "../matrices/bin/08_s_gyro_k.bin";
    const char *input_rhs_file = ""; // "../matrices/bin/09_u_powersim_b.bin";

    // prepare the system matrix and RHS vector
    PetscCall( load_matrix(input_mat_file, &A, mat_type) );

    if ( input_rhs_file && input_rhs_file[0] ) {
        PetscCall( load_vector(input_rhs_file, &b, vec_type) );
    } else {
        PetscCall( generate_rhs(A, &b, vec_type) );
    }

    // check if loaded correctly
    PetscInt m, n;
    MatInfo info;

    PetscCall( MatGetSize(A, &m, &n) );
    PetscCall( MatGetInfo(A, MAT_GLOBAL_SUM, &info) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Matrix size: %d x %d\n", m, n) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Number of nonzeros: %.0f\n", info.nz_allocated) );

    // solve the system
    PetscCall( solve_system(
        A, b, &x,
        MATORDERINGND,
        PCLU,
        MATSOLVERMUMPS
        ) 
    );

    // cleanup
    VecDestroy(&x);
    VecDestroy(&b);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
