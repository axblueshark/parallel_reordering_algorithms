static const char help[] = "Parallel reordering algorithms and parallel direct solvers.\n";

#include <petsc.h>
#include "solver_utils.h"

//========================================================
int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    Mat             A, A_perm, A_in = NULL;
    Vec             b, b_perm, b_in = NULL, x;
    IS              rperm;

    char            input_mat_file[PETSC_MAX_PATH_LEN] = "";
    char            input_rhs_file[PETSC_MAX_PATH_LEN] = "";

    char            ordering_type[64] = MATORDERINGNATURAL;
    char            pc_type[64]       = PCLU;
    char            solver_type[64]   = MATSOLVERMUMPS;

    char            export_file[PETSC_MAX_PATH_LEN] = "";

    PetscBool       show_mat_info = PETSC_FALSE,
                    get_solution_norm = PETSC_FALSE,
                    export_mat = PETSC_FALSE,
                    own_reordering = PETSC_FALSE;

    PetscLogStage   stage_reorder, stage_factor, stage_solve;

    // logging setup
    PetscCall( PetscLogDefaultBegin() );

    // stages
    PetscCall( PetscLogStageRegister("Reorder", &stage_reorder) );
    PetscCall( PetscLogStageRegister("Factorization", &stage_factor) );
    PetscCall( PetscLogStageRegister("Solve", &stage_solve) );

    // options
    PetscOptionsBegin( PETSC_COMM_WORLD, NULL, "Experiment options", "" );

    PetscCall( PetscOptionsString(
        "-mat_file", "System matrix (.bin)", "", 
        input_mat_file, input_mat_file, 
        sizeof(input_mat_file), NULL) 
    );
    
    PetscCall( PetscOptionsString(
        "-rhs_file", "RHS vector (.bin)", "", 
        input_rhs_file, input_rhs_file, 
        sizeof(input_rhs_file), NULL)
    );

    PetscCall( PetscOptionsString(
        "-mat_ordering_type", "Matrix reordering type", MATORDERINGNATURAL, 
        ordering_type, ordering_type, 
        sizeof(ordering_type), NULL)
    );

    PetscCall( PetscOptionsString(
        "-pc_type", "Preconditioner type", PCLU, 
        pc_type, pc_type, 
        sizeof(pc_type), NULL)
    );

    PetscCall( PetscOptionsString(
        "-mat_solver_type", "Direct solver package", MATSOLVERMUMPS,
        solver_type, solver_type, 
        sizeof(solver_type), NULL)
    );

    PetscCall( PetscOptionsBool(
        "-show_mat_info", "Show matrix info", NULL, 
        show_mat_info, &show_mat_info, NULL)
    );

    PetscCall( PetscOptionsBool(
        "-get_solution_norm", "Compute residual norm", NULL, 
        get_solution_norm, &get_solution_norm, NULL)
    );

    PetscCall( PetscOptionsBool(
        "-export_mat", "Export permuted matrix", NULL, 
        export_mat, &export_mat, NULL)
    );

    PetscCall( PetscOptionsString(
        "-export_file", "File to save permuted matrix", "", 
        export_file, export_file, 
        sizeof(export_file), NULL)
    );

    PetscCall( PetscOptionsBool(
        "-own_reordering", "Use custom reorder() routine (default: false)", PETSC_FALSE, 
        own_reordering, &own_reordering, NULL)
    );

    PetscOptionsEnd();

    // require input
    PetscCheck( input_mat_file[0] != '\0', 
        PETSC_COMM_WORLD, PETSC_ERR_USER,
        "Missing -mat_file <path>" );

    // load system & RHS vector
    PetscCall( load_matrix(input_mat_file, &A) );

    if ( input_rhs_file[0] ) {
        PetscCall( load_vector(input_rhs_file, &b) );
    } else {
        PetscCall( generate_rhs(A, &b) );
    }

    // show matrix info
    if ( show_mat_info ) {
        PetscCall( matrix_info(A) );
    }

    // reordering
    if ( own_reordering ) {
        PetscCall( PetscLogStagePush(stage_reorder) );
        PetscCall( reorder(A, b, ordering_type, &A_perm, &b_perm, &rperm) );
        PetscCall( PetscLogStagePop() );

        A_in = A_perm;
        b_in = b_perm;
    } else {
        // skip own reordering function, let solver handle it
        A_in = A;
        b_in = b;
    }

    // solve
    PetscCall( solve_system(A_in, b_in, &x,
                           pc_type,
                           own_reordering,
                           solver_type,
                           stage_factor, stage_solve) );

    // unpermute if our reorder() was used
    if ( own_reordering ) {
        PetscCall( VecPermute(x, rperm, PETSC_TRUE) );
    }

    // check solution norm
    if ( get_solution_norm ) {
        PetscReal norm;
        PetscCall( compute_solution_norm(A, b, x, &norm));
        PetscCall( PetscPrintf(PETSC_COMM_WORLD, "||Ax - b||_2 = %.6e\n", norm) );
    }

    // export permuted matrix
    if ( export_mat && export_file[0] && own_reordering ) {
        PetscCall( save_matrix(A_perm, export_file) );
    }

    // cleanup
    VecDestroy(&x);
    VecDestroy(&b);
    MatDestroy(&A);
    if ( own_reordering ) {
        MatDestroy(&A_perm);
        VecDestroy(&b_perm);
        ISDestroy(&rperm);
    }

    PetscFinalize();
    return 0;
}
