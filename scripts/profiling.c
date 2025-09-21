#include "solver_utils.h"

/**
 * @brief Measures the solution time.
 * 
 * @param ksp  KSP context.
 * @param time Time in seconds.
 * @return PetscErrorCode 
 */
PetscErrorCode measure_solve_time   ( KSP ksp, PetscLogDouble *time )
{

}

/**
 * @brief Measures the memory usage.
 * 
 * @param memory_usage The estimated memroy.
 * @return PetscErrorCode 
 */
PetscErrorCode measure_memory_usage ( PetscLogDouble *memory_usage )
{
    
}

/**
 * @brief Computes fill-in ratio: nnz(L+U) / nnz(A)
 * 
 * @param A          System matrix.
 * @param L 
 * @param fill_ratio 
 * @return PetscErrorCode 
 */
PetscErrorCode measure_fill_in      ( Mat A, Mat L, PetscReal *fill_ratio )
{
    
} 

/**
 * @brief Draws a sparsity structure of the system matrix.
 * 
 * @param A           System matrix.
 * @param output_file Output file to save the structure.
 * @return PetscErrorCode 
 */
PetscErrorCode draw_matrix_structure( Mat A, const char *output_file )
{
    PetscDraw   draw;
    PetscViewer viewer;

    PetscFunctionBeginUser;

    PetscCall( PetscOptionsSetValue(NULL, "-draw_save_single_file", "true") );

    PetscCall( PetscViewerDrawOpen(PETSC_COMM_WORLD, NULL, output_file,
                                   PETSC_DECIDE, PETSC_DECIDE, 
                                   800, 800, &viewer) );

    PetscCall( MatView(A, viewer) );

    PetscCall( PetscViewerDrawGetDraw(viewer, 0, &draw) );

    PetscCall( PetscDrawSetSave(draw, output_file) );
    PetscCall( PetscDrawSave(draw) );

    // cleanup
    PetscCall( PetscViewerDestroy(&viewer) );

    PetscFunctionReturn( PETSC_SUCCESS );
}

/**
 * @brief Save the obtained results to an output file.
 * 
 * @param x 
 * @param output_filename 
 * @return PetscErrorCode 
 */
PetscErrorCode save_results         ( Vec x, const char *output_filename)
{
    
}