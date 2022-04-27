cmake_minimum_required(VERSION 3.20)
project(EssentialPatcher)

set(CMAKE_CXX_STANDARD 14)

add_library(EssentialPatcher SHARED
    dllmain.cpp
    functions.cpp
)

set_target_properties(EssentialPatcher PROPERTIES
    OUTPUT_NAME "EssentialPatcher"
    PREFIX ""
    SUFFIX ".dll"
)

find_package(JNI)
find_package(JVMTI)

target_include_directories(EssentialPatcher PUBLIC ${JNI_INCLUDE_DIRS} ${JVMTI_INCLUDE_DIRS})
target_link_libraries(EssentialPatcher ${JNI_LIBRARIES} ${JVMTI_LIBRARIES})
