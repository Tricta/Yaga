#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ -z "$1" ]; then
    echo "Usage: $(basename "$0") <architecture>"
    echo "- architectures:"
    echo "       - x86_64"
    echo "       - arm64"
    exit 1
fi

ARCH="$1"

# ========== HANDLE PARAMETER ==========
if [[ "$ARCH" == "arm64" ]]; then
    BASE_PATH_64BITS="$SCRIPT_DIR/Yaga/src/main/libs/arm64-v8a"
    BASE_PATH_32BITS="$SCRIPT_DIR/Yaga/src/main/libs/armeabi-v7a"
elif [[ "$ARCH" == "x86_64" ]]; then
    BASE_PATH_64BITS="$SCRIPT_DIR/Yaga/src/main/libs/x86_64"
    BASE_PATH_32BITS="$SCRIPT_DIR/Yaga/src/main/libs/x86"
else
    echo "Invalid architecture: $ARCH"
    exit 1
fi

# ========== BUILD .so ==========
echo "Running ndk-build in: $SCRIPT_DIR/Yaga/src/main"
pushd "$SCRIPT_DIR/Yaga/src/main" > /dev/null
ndk-build
popd > /dev/null

# ========== COPY .SO TO MODULE ==========
echo

SOURCELOADER="libyagaloader.so"
SOURCEOPERATOR="libyagaoperator.so"
DESTINATION_MODULE_PATH_64BITS="$SCRIPT_DIR/YagaMagiskModule/system/lib64"
DESTINATION_MODULE_PATH_32BITS="$SCRIPT_DIR/YagaMagiskModule/system/lib"

copy_if_exists() {
    local SOURCE_DIR="$1"
    local FILENAME="$2"
    local DEST_DIR="$3"

    echo "Copying $FILENAME to $DEST_DIR..."
    if [ -f "$SOURCE_DIR/$FILENAME" ]; then
        mkdir -p "$DEST_DIR"
        cp "$SOURCE_DIR/$FILENAME" "$DEST_DIR/"
    else
        echo "Source file $SOURCE_DIR/$FILENAME not found!"
    fi
}

copy_if_exists "$BASE_PATH_64BITS" "$SOURCELOADER" "$DESTINATION_MODULE_PATH_64BITS"
copy_if_exists "$BASE_PATH_32BITS" "$SOURCELOADER" "$DESTINATION_MODULE_PATH_32BITS"
copy_if_exists "$BASE_PATH_64BITS" "$SOURCEOPERATOR" "$DESTINATION_MODULE_PATH_64BITS"
copy_if_exists "$BASE_PATH_32BITS" "$SOURCEOPERATOR" "$DESTINATION_MODULE_PATH_32BITS"

# ========== ZIP MODULE ==========
echo
echo "Zipping Yaga Magisk Module..."

MODULE_DIR="$SCRIPT_DIR/YagaMagiskModule"
ZIP_PATH="$SCRIPT_DIR/YagaMagiskModule.zip"

rm -f "$ZIP_PATH"
cd "$MODULE_DIR"
zip -r "$ZIP_PATH" * > /dev/null
cd - > /dev/null

echo "Zip created at: $ZIP_PATH"