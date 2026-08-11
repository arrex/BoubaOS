clean:
    rm -rf build

build:
    mkdir -p build && cmake -B build -S . && cmake --build build

test:
    ./run_tests.sh

run:
    just clean
    just build
    just test
    ./build/mysh
