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

/**
 * @brief Function to generate right hand vector if none is available from suite sparse collection.
 * 
 * @param A The system matrix.
 * @return Vec 
 */
Vec generate_rhs( Mat A )
{
    Vec      b, ones;
    PetscInt n;

    PetscCall( MatGetSize(A, NULL, &n) );

    // create a vector filled with ones
    PetscCall( VecCreate(PETSC_COMM_WORLD, &ones) );
    PetscCall( VecSetSizes(ones, PETSC_DECIDE, n) );
    PetscCall( VecSetFromOptions(ones) );
    PetscCall( VecSet(ones, 1.0) );
    PetscCall( VecAssemblyBegin(ones) );
    PetscCall( VecAssemblyEnd(ones) );

    // create b of size n and calculate b = A * ones
    PetscCall( VecDuplicate(ones, &b) );
    PetscCall( MatMult(A, ones, b) );

    PetscCall( VecDestroy(&ones) );
    return b;
}

/**
 * @brief Function to get a reordering of the system matrix.
 * 
 * @param A The system matrix.
 * @param ordering Type of the reordering.
 * @return Mat 
 */
Mat reorder( Mat A, MatOrderingType ordering ) 
{
    IS  rperm, cperm;
    Mat A_perm;

    // get the permutation indices
    PetscCall( MatGetOrdering(A, ordering, &rperm, &cperm) );

    // get the permuted matrix
    PetscCall( MatPermute(A, rperm, cperm, &A_perm) );

    // cleanup
    PetscCall( ISDestroy(&rperm) );
    PetscCall( ISDestroy(&cperm) );

    return A_perm;
}

Vec solve_system( Mat A, Vec b )
{
    
}


void save_results(Vec x, const char *output_filename) 
{
    // TODO: Save solution vector to file using PetscViewer
}

//========================================================
int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    const char *input_mat_file = "../matrices/bin/12_s_GAP-road.bin";
    //const char *input_vec_file = "../matrices/bin/09_u_powersim_b.bin";

    Mat A = load_matrix(input_mat_file);
    //Vec v = load_vector(input_vec_file);
    Vec b = generate_rhs(A);

    // check if loaded correctly
    PetscInt m, n;
    MatInfo info;

    PetscCall( MatGetSize(A, &m, &n) );
    PetscCall( MatGetInfo(A, MAT_GLOBAL_SUM, &info) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Matrix size: %d x %d\n", m, n) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Number of nonzeros: %.0f\n", info.nz_allocated) );

    //PetscInt vec_size;
    //PetscCall( VecGetSize(v, &vec_size) );
    //PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Vec size: %" PetscInt_FMT "\n", vec_size) );
//
    //PetscInt nprint = 3;
    //PetscInt idx[3] = {0, 1, 2};
    //PetscScalar vals[3];

    //PetscCall( VecGetValues(v, nprint, idx, vals) );
    //for ( PetscInt i = 0; i < nprint; i++ ) {
    //    PetscCall( PetscPrintf(PETSC_COMM_WORLD,
    //        "v[%" PetscInt_FMT "] = %g\n", idx[i] + 1, (double)PetscRealPart(vals[i])) );
    //}

    //Mat A_perm = reorder( A, MATORDERINGRCM );

    Vec x = solve_system(A, b);

    //VecDestroy(&v);
    VecDestroy(&b);
    VecDestroy(&x);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
