static const char help[] = "Parallel reordering algorithms and parallel direct solvers.\n";

#include <petsc.h>
#include "solver_utils.h"

//========================================================
int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    // logging setup
    PetscCall( PetscLogDefaultBegin() );

    PetscLogStage stageReorder, stageFactor, stageSolve;
    PetscCall( PetscLogStageRegister("Reorder", &stageReorder) );
    PetscCall( PetscLogStageRegister("Factorization", &stageFactor) );
    PetscCall( PetscLogStageRegister("Solve", &stageSolve) );

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
    MatType type;

    PetscCall( MatGetSize(A, &m, &n) );
    PetscCall( MatGetType(A, &type) );

    PetscCall( matrix_info(A) );

    // solve the system
    PetscCall( solve_system( 
        A, b, &x,
        MATORDERINGRCM, 
        PCLU, 
        MATSOLVERSUPERLU_DIST,
        stageReorder, stageFactor, stageSolve
        )
    );

    // cleanup
    VecDestroy(&x);
    VecDestroy(&b);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
