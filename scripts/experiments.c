static const char help[] = "Parallel reordering algorithms and parallel direct solvers.\n";

#include <petsc.h>
#include "solver_utils.h"

//========================================================
int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    Mat     A, A_perm;
    Vec     b, b_perm, x;
    IS      rperm;
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

    // show matrix info
    // PetscCall( matrix_info(A) );


    // reordering
    PetscCall( PetscLogStagePush(stage_reorder) );
    PetscCall( reorder(A, b, MATORDERINGRCM, &A_perm, &b_perm, &rperm) );
    PetscCall( PetscLogStagePop() );

    // solve the system
    PetscCall( solve_system(
        A_perm, b_perm, &x,
        PCLU,
        MATSOLVERSUPERLU_DIST,
        stage_factor, stage_solve
    ));

    // "unpermute" the solution to get the original one
    PetscCall( VecPermute(x, rperm, PETSC_TRUE) );

    // validate solution
    PetscReal norm;
    PetscCall( compute_solution_norm(A, b, x, &norm) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "||Ax - b||_2 = %.6e\n", norm) );



    // export permuted matrix for plotting
    // PetscCall( save_matrix(A_perm, "../matrices/bin/permuted/08_s_gyro_k_AMD.bin") );

    // cleanup
    VecDestroy(&x);
    VecDestroy(&b);
    MatDestroy(&A);
    ISDestroy(&rperm);

    PetscFinalize();
    return 0;
}
