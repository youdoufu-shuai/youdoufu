target_os="linux"
target_processor="x86_64"

mkdir -p build
rm -r build/*

# cmake
cmake -B build -S .
cmake --build build

# copy libs to 3dlib-export directory
if (( $? == 0 )); then
    echo "build done, run ......"

   ./build/bin/3dlib-demo-pc
fi
