setup:
    git config core.hooksPath .githooks

clean:
    rm -f *.log
    rm -rf build

build FRAME_STORE_SIZE="600" VAR_STORE_SIZE="10":
    just clean
    mkdir -p build && cmake -B build -DFRAME_STORE_SIZE={{FRAME_STORE_SIZE}} -DVAR_STORE_SIZE={{VAR_STORE_SIZE}} -S . -DCMAKE_BUILD_TYPE=Debug && cmake --build build

test:
    ./run_tests.sh

run FRAME_STORE_SIZE="600" VAR_STORE_SIZE="10":
    just clean
    just build {{FRAME_STORE_SIZE}} {{VAR_STORE_SIZE}}
    just test
    ./build/mysh

lint:
    find . -name "*.c" -o -name "*.h" | xargs clang-format -i
