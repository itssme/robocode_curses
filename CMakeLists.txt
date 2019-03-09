cmake_minimum_required(VERSION 3.9)

project(robocode)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lm -DASIO_STANDALONE -lncurses  -ltinfo -pthread")

function(PROTOBUF_GENERATE_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() called without any proto files")
        return()
    endif()

    if(PROTOBUF_GENERATE_CPP_APPEND_PATH)
        # Create an include path for each file specified
        foreach(FIL ${ARGN})
            get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
            get_filename_component(ABS_PATH ${ABS_FIL} PATH)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    else()
        set(_protobuf_include_path)
    endif()

    if(DEFINED PROTOBUF_IMPORT_DIRS)
        foreach(DIR ${PROTOBUF_IMPORT_DIRS})
            get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        get_filename_component(FIL_DIR ${ABS_FIL} DIRECTORY)

        list(APPEND _protobuf_include_path -I ${FIL_DIR})
        list(APPEND ${SRCS} "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.cc")
        list(APPEND ${HDRS} "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.h")

        add_custom_command(
                OUTPUT "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.cc"
                "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.h"
                COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
                ARGS --cpp_out  ${CMAKE_BINARY_DIR} ${_protobuf_include_path} ${ABS_FIL}
                DEPENDS ${ABS_FIL} ${PROTOBUF_PROTOC_EXECUTABLE}
                COMMENT "Running C++ protocol buffer compiler on ${FIL}"
                VERBATIM )
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


##############################
# GRPC Generate Function
# grpc cpp

function(GRPC_GENERATE_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: GRPC_GENERATE_CPP() called without any proto files")
        return()
    endif()

    if(GRPC_GENERATE_CPP_APPEND_PATH)
        # Create an include path for each file specified
        foreach(FIL ${ARGN})
            get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
            get_filename_component(ABS_PATH ${ABS_FIL} PATH)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    else()
        set(_protobuf_include_path)
    endif()

    if(DEFINED PROTOBUF_IMPORT_DIRS)
        foreach(DIR ${PROTOBUF_IMPORT_DIRS})
            get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        get_filename_component(FIL_DIR ${ABS_FIL} DIRECTORY)

        list(APPEND _protobuf_include_path -I ${FIL_DIR})
        list(APPEND ${SRCS} "${CMAKE_BINARY_DIR}/${FIL_WE}.grpc.pb.cc")
        list(APPEND ${SRCS} "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.cc")
        list(APPEND ${HDRS} "${CMAKE_BINARY_DIR}/${FIL_WE}.grpc.pb.h")
        list(APPEND ${HDRS} "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.h")
        add_custom_command(
                OUTPUT "${CMAKE_BINARY_DIR}/${FIL_WE}.grpc.pb.cc"
                "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.cc"
                "${CMAKE_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
                "${CMAKE_BINARY_DIR}/${FIL_WE}.pb.h"
                COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
                ARGS --cpp_out ${CMAKE_BINARY_DIR}
                --grpc_out  ${CMAKE_BINARY_DIR}
                ${_protobuf_include_path}
                --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
                ${ABS_FIL}
                DEPENDS ${ABS_FIL} ${PROTOBUF_PROTOC_EXECUTABLE}
                COMMENT "Running C++ protocol buffer compiler on ${FIL} using gRPC plugin"
                VERBATIM )
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_PROTOBUF_ARCH_DIR x64/)
    set(_GRPC_ARCH_DIR x64/)
endif()

# Internal function: search for normal library as well as a debug one
#    if the debug one is specified also include debug/optimized keywords
#    in *_LIBRARIES variable
function(_protobuf_find_libraries name filename)
    find_library(${name}_LIBRARY
            NAMES ${filename}
            PATHS ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/${_PROTOBUF_ARCH_DIR}Release)
    mark_as_advanced(${name}_LIBRARY)

    find_library(${name}_LIBRARY_DEBUG
            NAMES ${filename}
            PATHS ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/${_PROTOBUF_ARCH_DIR}Debug)
    mark_as_advanced(${name}_LIBRARY_DEBUG)

    if(NOT ${name}_LIBRARY_DEBUG)
        # There is no debug library
        set(${name}_LIBRARY_DEBUG ${${name}_LIBRARY} PARENT_SCOPE)
        set(${name}_LIBRARIES     ${${name}_LIBRARY} PARENT_SCOPE)
    else()
        # There IS a debug library
        set(${name}_LIBRARIES
                optimized ${${name}_LIBRARY}
                debug     ${${name}_LIBRARY_DEBUG}
                PARENT_SCOPE
                )
    endif()
endfunction()

# Internal function: search for normal library
function(_grpc_find_libraries name filename)
    find_library(${name}_LIBRARY
            NAMES ${filename}
            PATHS ${GRPC_SRC_ROOT_FOLDER}/vsprojects/${_GRPC_ARCH_DIR}Release)
    mark_as_advanced(${name}_LIBRARY)

    find_library(${name}_LIBRARY_DEBUG
            NAMES ${filename}
            PATHS ${GRPC_SRC_ROOT_FOLDER}/vsprojects/${_GRPC_ARCH_DIR}Debug)
    mark_as_advanced(${name}_LIBRARY_DEBUG)

    if(NOT ${name}_LIBRARY_DEBUG)
        # There is no debug library
        set(${name}_LIBRARY_DEBUG ${${name}_LIBRARY} PARENT_SCOPE)
    else()
        # There IS a debug library
        set(${name}_LIBRARIES
                optimized ${${name}_LIBRARY}
                debug     ${${name}_LIBRARY_DEBUG}
                PARENT_SCOPE
                )
    endif()
endfunction()

# Internal function: find threads library
function(_protobuf_find_threads)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    find_package(Threads)
    if(Threads_FOUND)
        list(APPEND PROTOBUF_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
        set(PROTOBUF_LIBRARIES "${PROTOBUF_LIBRARIES}" PARENT_SCOPE)
    endif()
endfunction()

# Internal function: find dl library
function(_grpc_find_dl)
    list(APPEND GRPC_LIBRARIES "-ldl")
    set(GRPC_LIBRARIES ${GRPC_LIBRARIES} PARENT_SCOPE)
endfunction()

#
# Main.
#

# By default have PROTOBUF_GENERATE_CPP macro pass -I to protoc
# for each directory where a proto file is referenced.
if(NOT DEFINED PROTOBUF_GENERATE_CPP_APPEND_PATH)
    set(PROTOBUF_GENERATE_CPP_APPEND_PATH TRUE)
endif()


# Google's provided vcproj files generate libraries with a "lib"
# prefix on Windows
if(MSVC)
    set(PROTOBUF_ORIG_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES}")
    set(CMAKE_FIND_LIBRARY_PREFIXES "lib" "")

    find_path(PROTOBUF_SRC_ROOT_FOLDER protobuf.pc.in)
    find_path(GRPC_SRC_ROOT_FOLDER grpc.def)
endif()

# The Protobuf library
_protobuf_find_libraries(PROTOBUF protobuf)
#DOC "The Google Protocol Buffers RELEASE Library"

_protobuf_find_libraries(PROTOBUF_LITE protobuf-lite)

# The Protobuf Protoc Library
_protobuf_find_libraries(PROTOBUF_PROTOC protoc)

# Restore original find library prefixes
if(MSVC)
    set(CMAKE_FIND_LIBRARY_PREFIXES "${PROTOBUF_ORIG_FIND_LIBRARY_PREFIXES}")
endif()

_grpc_find_libraries(GRPC grpc)
_grpc_find_libraries(GRPC++ grpc++)
_grpc_find_libraries(GRPC++_REFLECTION grpc++_reflection)

if(UNIX)
    _protobuf_find_threads()
    _grpc_find_dl()
endif()

set(GRPC_LIBRARIES ${GRPC_LIBRARY})
list(APPEND GRPC_LIBRARIES ${GRPC++_LIBRARY} ${GRPC++_REFLECTION_LIBRARY})
list(APPEND GRPC_LIBRARIES ${PROTOBUF_LIBRARIES})

# Find the include directory
find_path(PROTOBUF_INCLUDE_DIR
        google/protobuf/service.h
        PATHS ${PROTOBUF_SRC_ROOT_FOLDER}/src
        )
mark_as_advanced(PROTOBUF_INCLUDE_DIR)

find_path(GRPC_INCLUDE_DIR
        grpc/grpc.h
        PATHS ${GRPC_SRC_ROOT_FOLDER}/include
        )
mark_as_advanced(GRPC_INCLUDE_DIR)

# Find the protoc Executable
find_program(PROTOBUF_PROTOC_EXECUTABLE
        NAMES protoc
        DOC "The Google Protocol Buffers Compiler"
        PATHS
        ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/${_PROTOBUF_ARCH_DIR}Release
        ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/${_PROTOBUF_ARCH_DIR}Debug
        )
mark_as_advanced(PROTOBUF_PROTOC_EXECUTABLE)

find_program(GRPC_CPP_PLUGIN
        NAMES grpc_cpp_plugin
        DOC "The Google RPC C++ Plugin for Protoc"
        PATHS
        ${GRPC_SRC_ROOT_FOLDER}/vsprojects/${_GRPC_ARCH_DIR}Release
        ${GRPC_SRC_ROOT_FOLDER}/vsprojects/${_GRPC_ARCH_DIR}Debug
        )
mark_as_advanced(GRPC_CPP_PLUGIN)

find_program(GRPC_PYTHON_PLUGIN
        NAMES grpc_python_plugin
        DOC "The Google RPC Python Plugin for Protoc"
        PATHS
        ${GRPC_SRC_ROOT_FOLDER}/vsprojects/${_GRPC_ARCH_DIR}Release
        ${GRPC_SRC_ROOT_FOLDER}/vsprojects/${_GRPC_ARCH_DIR}Debug
        )
mark_as_advanced(GRPC_PYTHON_PLUGIN)

find_package(PackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GRPC DEFAULT_MSG
        GRPC_LIBRARY GRPC_INCLUDE_DIR PROTOBUF_INCLUDE_DIR
        GRPC_PYTHON_PLUGIN GRPC_CPP_PLUGIN)

if(PROTOBUF_FOUND)
    set(PROTOBUF_INCLUDE_DIRS ${PROTOBUF_INCLUDE_DIR})
endif()
if(GRPC_FOUND)
    set(GRPC_INCLUDE_DIRS ${GRPC_INCLUDE_DIR})
endif()

include_directories(include)
include_directories(include/streaming)

find_package(Protobuf REQUIRED)
#find_package(GRPC REQUIRED)  // won't be found even if grpc is correctly installed ¯\_(ツ)_/¯

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${GRPC_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# SPDLOG
include_directories("/home/joel/Documents/school/nvs_5te_klasse/inclue_libs/spdlog/include")

# JSON
include_directories("/home/joel/Documents/school/nvs_5te_klasse/inclue_libs/json")

# CLIPP
include_directories("/home/joel/Documents/school/nvs_5te_klasse/inclue_libs/clipp/include")

# ASIO
include_directories("/home/joel/Documents/school/nvs_5te_klasse/inclue_libs/asio/asio/include")

# TODO: set env variables
#include_directories($ENV{ASIO_INCLUDE_PATH})
#include_directories($ENV{SPDLOG_INCLUDE_PATH})
#include_directories($ENV{CLIPP_INCLUDE_PATH})
#include_directories($ENV{JSON_INCLUDE_PATH})

file(GLOB protos src/proto/*.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${protos})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${protos})

file(GLOB sources include/*.h include/streaming/*.h src/communication/*.cpp src/communication/streaming/*.cpp src/game_objects/*.cpp src/*.cpp)
#message(${sources})
add_executable(robocode ${sources} ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
target_link_libraries(robocode ${PROTOBUF_LIBRARY} stdc++fs)
target_link_libraries(robocode ${GRPC_LIBRARIES} stdc++fs)