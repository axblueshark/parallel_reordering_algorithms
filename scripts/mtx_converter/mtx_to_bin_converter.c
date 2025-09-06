static const char help[] = "Simple converter from .mtx to .bin format.\n";

#include <petsc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char **argv )
{
    FILE *file;
    PetscViewer viewer;
    char line[512];

    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    // missing arguments
    if ( argc < 3 ) {
        PetscPrintf( PETSC_COMM_WORLD, "Some args are missing. Expected format: %s input.mtx output.bin\n", argv[0] );
        PetscFinalize();
        return 1;
    }

    // open input file
    file = fopen( argv[1], "r" );
    if ( !file ) {
        PetscPrintf( PETSC_COMM_WORLD, "Could not open file: %s\n", argv[1] );
        PetscFinalize();
        return 1;
    }

    // detect file type from filename
    const char *input_filename = strrchr( argv[1], '/' );
    input_filename = input_filename ? input_filename + 1 : argv[1];
    PetscBool is_vector = PETSC_FALSE;
    if ( strstr(input_filename, "_b") || strstr(input_filename, "_t") ) {
        is_vector = PETSC_TRUE;
    }

    // skip comments (lines starting with %)
    do {
        if ( !fgets(line, sizeof(line), file) ) {
            PetscPrintf( PETSC_COMM_WORLD, "Invalid MatrixMarket file: %s\n", argv[1] );
            fclose( file );
            PetscFinalize();
            return 1;
        }
    } while ( line[0] == '%' );

    // handle _b, _t... files conversion
    if ( is_vector ) {
        PetscInt n, m;
        PetscReal val;

        sscanf( line, "%" PetscInt_FMT " %" PetscInt_FMT, &n, &m );

        Vec v;
        PetscCall( VecCreate(PETSC_COMM_WORLD, &v) );
        PetscCall( VecSetSizes(v, PETSC_DECIDE, n) );
        PetscCall( VecSetFromOptions(v) );

        for ( PetscInt i = 0; i < n; i++ ) {
            if ( fscanf(file, "%lf", &val) != 1 ) {
                PetscPrintf( PETSC_COMM_WORLD, "Failed to read vector entry #%" PetscInt_FMT "\n", i + 1 );
                break;
            }
            PetscCall( VecSetValue(v, i, val, INSERT_VALUES) );
        }

        fclose( file );
        PetscCall( VecAssemblyBegin(v) );
        PetscCall( VecAssemblyEnd(v) );

        PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, argv[2], FILE_MODE_WRITE, &viewer) );
        PetscCall( VecView(v, viewer) );
        PetscCall( PetscViewerDestroy(&viewer) );
        PetscCall( VecDestroy(&v) );
    }
    // handle matrix conversion
    else {
        PetscInt row_idx, col_idx, m, n, nnz;
        PetscReal val;
        Mat A;

        // read dimensions & nnz
        sscanf( line, "%" PetscInt_FMT " %" PetscInt_FMT " %" PetscInt_FMT,
                      &m, &n, &nnz );

        PetscCall( MatCreate(PETSC_COMM_WORLD, &A) );
        PetscCall( MatSetSizes(A, PETSC_DECIDE, PETSC_DECIDE, m, n) );
        PetscCall( MatSetFromOptions(A) );
        PetscCall( MatSetUp(A) );

        // detect symmetry from filename
        PetscBool symmetric = PETSC_FALSE;
        if ( strstr(input_filename, "_s_") ) {
            symmetric = PETSC_TRUE;
            PetscCall( MatSetOption(A, MAT_SYMMETRIC, PETSC_TRUE) );
        }

        // read entries
        for ( PetscInt i = 0; i < nnz; i++ ) {
            if ( fscanf(file, "%" PetscInt_FMT " %" PetscInt_FMT " %lf",
                              &row_idx, &col_idx, &val) != 3 ) {
                PetscPrintf( PETSC_COMM_WORLD, "Failed to read matrix entry #%" PetscInt_FMT "\n", i + 1 );
                break;
            }
            
            PetscCall( MatSetValue(A, row_idx - 1, col_idx - 1, val, INSERT_VALUES) );

            if ( symmetric && row_idx != col_idx ) {
                PetscCall( MatSetValue(A, col_idx - 1, row_idx - 1, val, INSERT_VALUES) );
            }
        }

        fclose( file );
        PetscCall( MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY) );
        PetscCall( MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY) );

        PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, argv[2],
                                        FILE_MODE_WRITE, &viewer) );
        PetscCall( MatView(A, viewer) );

        PetscCall( PetscViewerDestroy(&viewer) );
        PetscCall( MatDestroy(&A) );
    }

    PetscFinalize();
    return 0;
}
