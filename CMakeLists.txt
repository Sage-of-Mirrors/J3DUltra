cmake_minimum_required(VERSION 3.12)
project(j3dultra)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set( GLM_TEST_ENABLE      OFF CACHE INTERNAL "" )
set( BUILD_SHARED_LIBS    OFF CACHE INTERNAL "" )
set( BUILD_STATIC_LIBS    OFF CACHE INTERNAL "" )

add_subdirectory(lib/magic_enum)
add_subdirectory(lib/glm)
add_subdirectory(lib/libflipper)

file(GLOB J3DULTRA_SRC
    # J3DUltra
    "src/*.cpp"
    "include/*.hpp"
    "src/J3D/*.cpp"
    "src/J3D/Animation/*.cpp"
    "include/J3D/*.hpp"
    "include/GX/*.hpp"
    "include/J3D/Animation/*.hpp"
    
    # bStream
    "lib/bStream/*.cpp"
    
    # glad
    "lib/glad/src/*.c"
)

add_library(j3dultra ${J3DULTRA_SRC})
target_include_directories(j3dultra PUBLIC include lib/bStream lib/glad/include lib/libflipper/include lib/libflipper/include/geometry lib/magic_enum/include)
target_link_libraries(j3dultra PUBLIC magic_enum glm libflipper)
