MTX_DIR=../../matrices/mtx
BIN_DIR=../../matrices/bin
CONVERTER=./mtx_to_bin_converter

# make sure output directory exists
mkdir -p "$BIN_DIR"  

for file in "$MTX_DIR"/*.mtx; do
    filename=$(basename "$file" .mtx)
    input="$file"
    output="$BIN_DIR/$filename.bin"
    
    # skip only auxiliary coordinate files
    if [[ "$filename" == *_coord* ]]; then
        echo "Skipping file: $file"
        continue
    fi

    echo "Converting: $input → $output"
    $CONVERTER "$input" "$output"
done
