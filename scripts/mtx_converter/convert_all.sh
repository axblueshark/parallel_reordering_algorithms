MTX_DIR=../../matrices/mtx
BIN_DIR=../../matrices/bin
CONVERTER=./mtx_to_bin_converter

# make sure output directory exists
mkdir -p "$BIN_DIR"  

for file in "$MTX_DIR"/*.mtx; do
    filename=$(basename "$file" .mtx)
    input="$file"
    output="$BIN_DIR/$filename.bin"
    
    # skip RHS or auxiliary files (_b, _t, _coord, etc.)
    if [[ "$filename" == *_b ]] || [[ "$filename" == *_t* ]] || [[ "$filename" == *_coord* ]]; then
        echo "Skipping file: $file"
        continue
    fi

    echo "Converting: $input → $output"
    $CONVERTER "$input" "$output"
done
