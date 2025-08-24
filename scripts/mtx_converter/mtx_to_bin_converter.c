static const char help[] = "Simple converter from .mtx to .bin format.\n";

#include <petsc.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    FILE *f;
    int row_idx, col_idx, m, n, nnz;  // m, n, nnz - matrix dimensions and number of nonzeros, respectively
    double val;                       // value at (row_idx, col_idx)
    Mat A;                            // matrix to be constructed
    PetscViewer viewer;
    char line[512];

    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    // some arguments are missing
    if ( argc < 3 ) {
        PetscPrintf(PETSC_COMM_WORLD, "Some arguments are missing, expected format: %s input.mtx output.bin\n", argv[0]);
        PetscFinalize();
        return 1;
    }

    // open input .mtx file
    f = fopen( argv[1], "r" );
    if ( !f ) {
        PetscPrintf( PETSC_COMM_WORLD, "Could not open file: %s\n", argv[1] );
        PetscFinalize();
        return 1;
    }

    // skip comments (headers beginning with %)
    do {
        if ( !fgets(line, sizeof(line), f) ) {
            PetscPrintf( PETSC_COMM_WORLD, "Invalid matrix market file.\n" );
            fclose(f);
            PetscFinalize();
            return 1;
        }
    } while ( line[0] == '%' );

    // read matrix dimensions and number of nonzeros
    sscanf( line, "%d %d %d", &m, &n, &nnz );

    // empty matrix setup
    PetscCall( MatCreate(PETSC_COMM_WORLD, &A) );
    PetscCall( MatSetSizes(A, PETSC_DECIDE, PETSC_DECIDE, m, n) );
    PetscCall( MatSetFromOptions(A) );
    PetscCall( MatSetUp(A) );

    // check for symmetry from filename
    const char *input_filename = strrchr(argv[1], '/');
    input_filename = input_filename ? input_filename + 1 : argv[1];
    int symmetric = 0;
    
    if ( strstr(input_filename, "_s_") ) {
        symmetric = 1;

        PetscCall( MatSetOption(A, MAT_SYMMETRIC, PETSC_TRUE) );
        PetscPrintf(PETSC_COMM_WORLD, "Matrix marked as symmetric based on filename: %s\n", input_filename);
    }

    // read and insert entries
    for ( int i = 0; i < nnz; i++ ) {
        if ( fscanf(f, "%d %d %lf", &row_idx, &col_idx, &val) != 3 ) {
            PetscPrintf( PETSC_COMM_WORLD, "Failed to read value #%d\n", i + 1 );
            break;
        }
        PetscCall( MatSetValue(A, row_idx - 1, col_idx - 1, val, INSERT_VALUES) );

        // ensure symmetric values if matrix is symmetric
        if ( symmetric && row_idx != col_idx ) {
            PetscCall( MatSetValue(A, col_idx - 1, row_idx - 1, val, INSERT_VALUES) );
        }
    }

    fclose(f);
    PetscCall( MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY) );
    PetscCall( MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY) );

    // save to binary format
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, argv[2], FILE_MODE_WRITE, &viewer) );
    PetscCall( MatView(A, viewer) );
    PetscCall( PetscViewerDestroy(&viewer) );
    PetscCall( MatDestroy(&A) );

    PetscFinalize();
    return 0;
}
