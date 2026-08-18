setup:
    git config core.hooksPath .githooks

clean:
    rm -f *.log
    rm -rf build

build FRAME_SIZE="3" FRAME_STORE_SIZE="600" VAR_STORE_SIZE="10":
    just clean
    mkdir -p build && cmake -B build -DFRAME_SIZE={{FRAME_SIZE}} -DFRAME_STORE_SIZE={{FRAME_STORE_SIZE}} -DVAR_STORE_SIZE={{VAR_STORE_SIZE}} -S . -DCMAKE_BUILD_TYPE=Debug && cmake --build build

test:
    ./run_tests.sh

run FRAME_SIZE="3" FRAME_STORE_SIZE="600" VAR_STORE_SIZE="10":
    just clean
    just test
    just build {{FRAME_SIZE}} {{FRAME_STORE_SIZE}} {{VAR_STORE_SIZE}}
    ./build/mysh

lint:
    find . -name "*.c" -o -name "*.h" | xargs clang-format -i
