static const char help[] = "Parallel reordering algorithms and parallel direct solvers.\n";

#include <petsc.h>

/**
 * @brief Loads the system matrix from an input .bin file.
 * 
 * @param filename The input file.
 * @return Mat 
 */
Mat load_matrix( const char *filename )
{
    Mat         A;
    PetscViewer viewer;

    // open PETSc binary file for reading
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_READ, &viewer) );

    // create and load the matrix
    PetscCall( MatCreate(PETSC_COMM_WORLD, &A) );
    PetscCall( MatSetFromOptions(A) );
    PetscCall( MatLoad(A, viewer) );

    // cleanup
    PetscCall( PetscViewerDestroy(&viewer) );

    return A;
}

/**
 * @brief Loads the RHS vector from an input .bin file.
 * 
 * @param filename The input file.
 * @return Vec 
 */
Vec load_vector( const char *filename )
{
    Vec         v;
    PetscViewer viewer;

    // open PETSc binary file for reading
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_READ, &viewer) );

    // create and load the matrix
    PetscCall( VecCreate(PETSC_COMM_WORLD, &v) );
    PetscCall( VecSetFromOptions(v) );
    PetscCall( VecLoad(v, viewer) );

    // cleanup
    PetscCall( PetscViewerDestroy(&viewer) );

    return v;
}

void apply_reordering(Mat A) 
{
    // TODO: Implement matrix reordering using MatGetOrdering, etc.
}

Vec solve_system(Mat A) 
{
    Vec x;
    // TODO: Create RHS, set up KSP, solve the system, return solution vector
    return x;
}

void save_results(Vec x, const char *output_filename) 
{
    // TODO: Save solution vector to file using PetscViewer
}

//========================================================
int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    const char *input_mat_file = "../matrices/bin/01_s_bcsstk03.bin";
    const char *input_vec_file = "../matrices/bin/09_u_powersim_b.bin";

    Mat A = load_matrix(input_mat_file);
    Vec v = load_vector(input_vec_file);

    // check if loaded correctly
    PetscInt m, n;
    MatInfo info;

    PetscCall( MatGetSize(A, &m, &n) );
    PetscCall( MatGetInfo(A, MAT_GLOBAL_SUM, &info) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Matrix size: %d x %d\n", m, n) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Number of nonzeros: %.0f\n", info.nz_allocated) );

    PetscInt vec_size;
    PetscCall( VecGetSize(v, &vec_size) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Vec size: %" PetscInt_FMT "\n", vec_size) );

    PetscInt nprint = 3;
    PetscInt idx[3] = {0, 1, 2};
    PetscScalar vals[3];

    PetscCall( VecGetValues(v, nprint, idx, vals) );
    for ( PetscInt i = 0; i < nprint; i++ ) {
        PetscCall( PetscPrintf(PETSC_COMM_WORLD,
            "v[%" PetscInt_FMT "] = %g\n", idx[i] + 1, (double)PetscRealPart(vals[i])) );
    }

    VecDestroy(&v);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
