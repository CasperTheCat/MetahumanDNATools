cmake_minimum_required(VERSION 3.7)
project(MetahumanDNA)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set (ASSIMP_BUILD_TOOLS false)
set (ASSIMP_BUILD_ASSIMP_TOOLS false)
set (BUILD_SHARED_LIBS false)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wall -ggdb")

set(HEADERS
    src/Types.h
    src/Bones.h
)

set(SOURCE
    src/Source.cpp
    src/Bones.cpp
)

include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/extern
        ${CMAKE_SOURCE_DIR}/extern/glm
        ${CMAKE_SOURCE_DIR}/extern/assimp/include
)

add_subdirectory(${CMAKE_SOURCE_DIR}/extern/glm)
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/assimp)

add_executable(MetahumanDNA ${HEADERS} ${SOURCE})

target_link_libraries(MetahumanDNA
    assimp
    glm
    ${ASSIMP_LIBRARIES}
)
