cmake_minimum_required(VERSION 3.10)
project(BoubaOS C)

# Specify C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set build directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include/core
    ${PROJECT_SOURCE_DIR}/include/memory
    ${PROJECT_SOURCE_DIR}/include/scheduling
    ${PROJECT_SOURCE_DIR}/include/utils
    ${PROJECT_SOURCE_DIR}/include/commands
)

# Create executable
add_executable(mysh
    src/core/shell.c
    src/core/interpreter.c
    src/memory/shellmemory.c
    src/scheduling/pcb.c
    src/scheduling/ready_queue.c
    src/scheduling/scheduler.c
    src/utils/badcommand.c
    src/utils/utils.c
    src/commands/echo.c
    src/commands/exec.c
    src/commands/help.c
    src/commands/my_cd.c
    src/commands/my_fork.c
    src/commands/my_ls.c
    src/commands/my_mkdir.c
    src/commands/my_touch.c
    src/commands/print.c
    src/commands/quit.c
    src/commands/run.c
    src/commands/set.c
)

# Gather user-defined flags
set(FRAME_STORE_SIZE "600" CACHE STRING "Number of frames in the frame store")
set(VAR_STORE_SIZE "10" CACHE STRING
  "Number of variable entries in the variable memory table")

# Pass values to compilation flags
target_compile_definitions(mysh PRIVATE
  FRAME_STORE_SIZE=${FRAME_STORE_SIZE} VAR_STORE_SIZE=${VAR_STORE_SIZE})
