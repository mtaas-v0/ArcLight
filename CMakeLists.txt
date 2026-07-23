cmake_minimum_required(VERSION 3.15)
project("ArcLight" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE RelWithDebInfo)


# 1. Force static compilation for internal dependencies
set(BUILD_SHARED_LIBS OFF)

# 2. Force the MinGW compiler to bake runtimes into the final .exe
if(WIN32 AND NOT MSVC)
    add_link_options("-static-libgcc" "-static-libstdc++" "-static" "-Wl,--allow-multiple-definition")
    if(WIN32 AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "Windows ARM64 GCC detected: Enabling duplicate symbol linker workaround.")
        ## target_link_options(arclight PRIVATE )
    endif()
endif()


if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

set(ARCLIGHT_BACKEND "AUTO" CACHE STRING "Backend to build: AUTO, NEON, X86, or NONE")
set_property(CACHE ARCLIGHT_BACKEND PROPERTY STRINGS AUTO NEON X86 NONE)
string(TOUPPER "${ARCLIGHT_BACKEND}" ARCLIGHT_BACKEND)
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCLIGHT_SYSTEM_PROCESSOR)

set(NEON OFF CACHE BOOL "Enable NEON backend" FORCE)
set(X86 OFF CACHE BOOL "Enable x86 backend" FORCE)

if(ARCLIGHT_BACKEND STREQUAL "AUTO")
    if(ARCLIGHT_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|arm)")
        set(NEON ON CACHE BOOL "Enable NEON backend" FORCE)
        set(ARCLIGHT_SELECTED_BACKEND "NEON")
    elseif(ARCLIGHT_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|i[3-6]86)")
        set(X86 ON CACHE BOOL "Enable x86 backend" FORCE)
        set(ARCLIGHT_SELECTED_BACKEND "X86")
    else()
        set(ARCLIGHT_SELECTED_BACKEND "NONE")
        message(WARNING "Unknown processor '${CMAKE_SYSTEM_PROCESSOR}'; building without architecture-specific backend.")
    endif()
elseif(ARCLIGHT_BACKEND STREQUAL "NEON")
    set(NEON ON CACHE BOOL "Enable NEON backend" FORCE)
    set(ARCLIGHT_SELECTED_BACKEND "NEON")
elseif(ARCLIGHT_BACKEND STREQUAL "X86")
    set(X86 ON CACHE BOOL "Enable x86 backend" FORCE)
    set(ARCLIGHT_SELECTED_BACKEND "X86")
elseif(ARCLIGHT_BACKEND STREQUAL "NONE")
    set(ARCLIGHT_SELECTED_BACKEND "NONE")
else()
    message(FATAL_ERROR "Unsupported ARCLIGHT_BACKEND='${ARCLIGHT_BACKEND}'. Use AUTO, NEON, X86, or NONE.")
endif()

message(STATUS "ArcLight target OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "ArcLight backend: ${ARCLIGHT_SELECTED_BACKEND} (${CMAKE_SYSTEM_PROCESSOR})")

if (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Android")
    add_compile_definitions(_GNU_SOURCE)
endif()
if(WIN32)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
    if(MSVC)
        add_compile_options(/utf-8)
    endif()
endif()

if(NEON)
    message(STATUS "NEON enabled")
endif()
if(X86)
    message(STATUS "x86 enabled")
endif()
# add_subdirectory(nnml)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

add_subdirectory(nnml)

file(GLOB NNML_SRC
    *.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/*.cpp
)

add_library(arclight SHARED ${NNML_SRC})
set_target_properties(arclight PROPERTIES
    OUTPUT_NAME al
)
if(WIN32)
    set_target_properties(arclight PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
target_include_directories(arclight PUBLIC include models)
target_link_libraries(arclight PUBLIC nnml)


if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ipython/CMakeLists.txt")
    add_subdirectory(ipython)
endif()

# add_executable(test-gguf ${CMAKE_CURRENT_SOURCE_DIR}/test/test-gguf.cpp)
# target_link_libraries(test-gguf PRIVATE arclight)

# add_executable(test-model ${CMAKE_CURRENT_SOURCE_DIR}/test/test-model.cpp)
# target_link_libraries(test-model PRIVATE arclight)

# add_executable(test-tokenizer ${CMAKE_CURRENT_SOURCE_DIR}/test/test-tokenizer.cpp)
# target_link_libraries(test-tokenizer PRIVATE arclight)

# add_executable(test-onetoken ${CMAKE_CURRENT_SOURCE_DIR}/test/test-onetoken.cpp)
# target_link_libraries(test-onetoken PRIVATE arclight)

# add_executable(test-onetoken-tp ${CMAKE_CURRENT_SOURCE_DIR}/test/test-onetoken-tp.cpp)
# target_link_libraries(test-onetoken-tp PRIVATE arclight)

add_executable(al-gen ${CMAKE_CURRENT_SOURCE_DIR}/apps/al-gen.cpp)
target_link_libraries(al-gen PRIVATE arclight)

add_executable(al-chat ${CMAKE_CURRENT_SOURCE_DIR}/apps/al-chat.cpp)
target_link_libraries(al-chat PRIVATE arclight)

add_executable(al-ppl ${CMAKE_CURRENT_SOURCE_DIR}/apps/al-ppl.cpp)
target_link_libraries(al-ppl PRIVATE arclight)

# enable_testing()

# file(GLOB TEST_SOURCES test/*.cpp)
# foreach(TEST_SRC ${TEST_SOURCES})
#     get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
#     add_executable(${TEST_NAME} ${TEST_SRC})
#     target_link_libraries(${TEST_NAME} PRIVATE arclight)
#     add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
# endforeach()

message(STATUS "ArcLight build configured successfully.")
