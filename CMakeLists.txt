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
    "include/J3D/*.hpp"
    "include/J3D/Animation/*.hpp"
    "include/J3D/Data/*.hpp"
    "include/J3D/Geometry/*.hpp"
    "include/J3D/Material/*.hpp"
    "include/J3D/Picking/*.hpp"
    "include/J3D/Rendering/*.hpp"
    "include/J3D/Skeleton/*.hpp"
    "include/J3D/Texture/*.hpp"
    "include/J3D/Util/*.hpp"

    "src/J3D/*.cpp"
    "src/J3D/Animation/*.cpp"
    "src/J3D/Data/*.cpp"
    "src/J3D/Geometry/*.cpp"
    "src/J3D/Material/*.cpp"
    "src/J3D/Picking/*.cpp"
    "src/J3D/Rendering/*.cpp"
    "src/J3D/Skeleton/*.cpp"
    "src/J3D/Texture/*.cpp"
    "src/J3D/Util/*.cpp"

    "include/GX/*.hpp"
    
    # bStream
    "lib/bStream/*.cpp"
    
    # glad
    "lib/glad/src/*.c"
)

add_library(j3dultra ${J3DULTRA_SRC})
target_include_directories(j3dultra PUBLIC include lib/bStream lib/glad/include lib/libflipper/include lib/libflipper/include/geometry lib/magic_enum/include)
target_link_libraries(j3dultra PUBLIC magic_enum glm libflipper)
