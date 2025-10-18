#include "solver_utils.h"

/**
 * @brief Function to solve the system of linear equations Ax = b.
 * 
 * @param A The system matrix.
 * @param b The right-hand side vector.
 * @param x The solution vector.
 * @param pc_type The type of the preconditioner to be used.
 * @param mat_solver_type The solver type.
 * @param stage_factor The stage to measure factorization.
 * @param stage_solve THe stage to measure solve.
 * @return PetscErrorCode 
 */
PetscErrorCode solve_system( Mat A, Vec b, Vec *x,
                             PCType pc_type, 
                             PetscBool own_reordering,
                             MatSolverType mat_solver_type,
                             PetscLogStage stage_factor,
                             PetscLogStage stage_solve )
{
    KSP ksp;
    PC  pc;

    PetscFunctionBeginUser;

    // solver setup
    PetscCall( KSPCreate(PETSC_COMM_WORLD, &ksp) );
    PetscCall( KSPSetOperators(ksp, A, A) );
    PetscCall( KSPSetType(ksp, KSPPREONLY) ); // to use direct solvers

    PetscCall( KSPGetPC(ksp, &pc) );
    PetscCall( PCSetType(pc, pc_type) );
    PetscCall( PCFactorSetMatSolverType(pc, mat_solver_type) );
    PetscCall( KSPSetFromOptions(ksp) );

    if ( own_reordering ) {
        // solver does not reorder again
        PetscCall( PCFactorSetMatOrderingType(pc, MATORDERINGNATURAL) );
    }

    PetscCall( PCFactorSetUpMatSolverType(pc) );

    // factorization
    PetscCall( PetscLogStagePush(stage_factor) );
    PetscCall( KSPSetUp(ksp) ); // triggers symbolic+numeric factorization
    PetscCall( PetscLogStagePop() );

    // allocate space for solution vector
    PetscCall( VecDuplicate(b, x) );

    // solve
    PetscCall( PetscLogStagePush(stage_solve) );
    PetscCall( KSPSolve(ksp, b, *x) );
    PetscCall( PetscLogStagePop() );

    // cleanup
    PetscCall( KSPDestroy(&ksp) );

    PetscFunctionReturn( PETSC_SUCCESS );
}