cmake_minimum_required(VERSION 3.10)
project(ImageProcessingAssignment CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(assignment1
    src/main.cpp
    src/Image.cpp
    src/stb_impl.cpp
)

target_include_directories(assignment1 PRIVATE external)

# Run the executable with the project folder as working directory so it finds Lenna.png.
set_target_properties(assignment1 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
