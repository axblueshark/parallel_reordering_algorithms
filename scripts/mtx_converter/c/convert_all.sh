MTX_DIR=../../matrices/mtx
AUX_DIR=$MTX_DIR/aux
BIN_DIR=../../matrices/bin
CONVERTER_MAT=./mtx_to_bin_mat
CONVERTER_VEC=./mtx_to_bin_vec

mkdir -p "$BIN_DIR"

# convert matrix files
for file in "$MTX_DIR"/*.mtx; do
    filename=$(basename "$file" .mtx)
    input="$file"
    output="$BIN_DIR/$filename.bin"

    # skip coordinate files
    if [[ "$filename" == *_coord* ]]; then
        echo "Skipping file: $file"
        continue
    fi

    echo "Converting matrix: $input → $output"
    $CONVERTER_MAT "$input" "$output"
done

# convert vector files in aux/
for file in "$AUX_DIR"/*.mtx; do
    filename=$(basename "$file" .mtx)
    input="$file"
    output="$BIN_DIR/$filename.bin"

    echo "Converting vector: $input → $output"
    $CONVERTER_VEC "$input" "$output"
done
