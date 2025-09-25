#include "solver_utils.h"

/**
 * @brief Function to solve the system of linear equations Ax = b.
 * 
 * @param A The system matrix.
 * @param b The right-hand side vector.
 * @param x The solution vector.
 * @param ordering_type The type of the reordering to be used.
 * @param pc_type The type of the preconditioner to be used.
 * @param mat_solver_type The solver type.
 * @return PetscErrorCode 
 */
PetscErrorCode solve_system( Mat A, Vec b, Vec *x,
                             MatOrderingType ordering_type,
                             PCType pc_type, MatSolverType mat_solver_type,
                             PetscLogStage stageReorder,
                             PetscLogStage stageFactor,
                             PetscLogStage stageSolve )
{    // variables setup
    KSP ksp;
    PC  pc;
    IS  rperm;
    Vec b_perm;
    Mat A_perm;

    PetscFunctionBeginUser;

    // reordering
    PetscCall( PetscLogStagePush(stageReorder) );
    PetscCall( reorder(A, b, ordering_type, &A_perm, &b_perm, &rperm) );
    PetscCall( PetscLogStagePop() );


    // solver setup
    PetscCall( KSPCreate(PETSC_COMM_WORLD, &ksp) );
    PetscCall( KSPSetOperators(ksp, A_perm, A_perm) );
    PetscCall( KSPSetType(ksp, KSPPREONLY) ); // to use direct solvers

    PetscCall( KSPGetPC(ksp, &pc) );
    PetscCall( PCSetType(pc, pc_type) );
    PetscCall( PCFactorSetMatSolverType(pc, mat_solver_type) );
    PetscCall( PCFactorSetUpMatSolverType(pc) );

    PetscCall( KSPSetFromOptions(ksp) );

    PetscCall( PetscLogStagePush(stageFactor) );
    PetscCall( KSPSetUp(ksp) ); // triggers symbolic+numeric factorization
    PetscCall( PetscLogStagePop() );

    // allocate space for solution vector
    PetscCall( VecDuplicate(b_perm, x) );

    // solve
    PetscCall( PetscLogStagePush(stageSolve) );
    PetscCall( KSPSolve(ksp, b_perm, *x) );
    PetscCall( PetscLogStagePop() );  

    // "unpermute" the solution to get the original one 
    PetscCall( VecPermute(*x, rperm, PETSC_TRUE) );

    // TODO: error? profiling? what do I know we will see

    // cleanup
    PetscCall( KSPDestroy(&ksp) );
    PetscCall( MatDestroy(&A_perm) );
    PetscCall( VecDestroy(&b_perm) );
    PetscCall( ISDestroy(&rperm) );

    PetscFunctionReturn( PETSC_SUCCESS );
}