cmake_minimum_required (VERSION 2.6)

project (ULORDTOOL)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

###################################### Options ######################################
message("") # empty line
message("------------------------------- Options -------------------------------")

#
# System Type:
#
MESSAGE(STATUS "Current platform: ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

#
# Build Type: -DCMAKE_BUILD_TYPE=Debug|Release
# Default: Release
#
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message("-- Build Type: ${CMAKE_BUILD_TYPE} (-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})")
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_CXX_FLAGS "-g -O0 -Wall")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  set(CMAKE_CXX_FLAGS "-g -O2 -Wall")
ELSE()
  set(CMAKE_CXX_FLAGS "-g -Wall")
ENDIF()

# compiler options
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER_ARG1 "-std=c++0x")

#
# The number of concurrent jobs when compiling a third-party library
# Example: -DJOBS=2
# Default: 1
#
if(JOBS)
  message("-- Concurrent Jobs: ${JOBS} (-DJOBS=${JOBS})")
  set(MAKE_JOBS "-j${JOBS}") 
else()
  message("-- Concurrent Jobs: 1 (-DJOBS=1)")
  set(MAKE_JOBS "") 
endif()

#
# Chain Type
#
set(CHAIN_TYPE "VDS")
set(CHAIN_NAME "vds Core")
IF(APPLE)
    add_definitions( "-DMAC_OSX" )
ELSE()
    #add_definitions("-rdynamic")
ENDIF()
add_definitions("-pthread")

message("-- Chain Type: ${CHAIN_NAME} (-DCHAIN_TYPE=${CHAIN_TYPE})")

# add a macro that needed by chain's source code
add_definitions(-DHAVE_CONFIG_H)

#
# Root Dir of Chain's Source Code: -DCHAIN_SRC_ROOT=<path>
#
if(NOT CHAIN_SRC_ROOT)
  message(FATAL_ERROR "Root dir of chain's source code not defined! (-DCHAIN_SRC_ROOT=<path>)")
endif()
message("-- Root Dir of ${CHAIN_NAME}: ${CHAIN_SRC_ROOT} (-DCHAIN_SRC_ROOT=${CHAIN_SRC_ROOT})")
if (NOT EXISTS "${CHAIN_SRC_ROOT}/src/version.h")
  message(FATAL_ERROR "The source code of ${CHAIN_NAME} not exists!")
endif()

###################################### Libs ######################################
message("") # empty line
message("------------------------------- Libs -------------------------------")

#
# chain's libs
#
set(BITCOIN_LIBRARIES ${BITCOIN_LIBRARIES}
    "${CHAIN_SRC_ROOT}/src/libbitcoin_common.a"
    "${CHAIN_SRC_ROOT}/src/libbitcoin_consensus.a"
    "${CHAIN_SRC_ROOT}/src/libbitcoin_util.a"
    "${CHAIN_SRC_ROOT}/src/crypto/libbitcoin_crypto_base.a"
    )

list(GET BITCOIN_LIBRARIES 0 BITCOIN_LIBRARIE)
if (NOT EXISTS ${BITCOIN_LIBRARIE})
  message(STATUS "building ${CHAIN_NAME}...")
  execute_process(WORKING_DIRECTORY "${CHAIN_SRC_ROOT}" COMMAND ./autogen.sh)
  execute_process(WORKING_DIRECTORY "${CHAIN_SRC_ROOT}" COMMAND ./configure --disable-tests --disable-bench)
  execute_process(WORKING_DIRECTORY "${CHAIN_SRC_ROOT}" COMMAND make ${MAKE_JOBS})
endif()

foreach(BITCOIN_LIBRARIE ${BITCOIN_LIBRARIES})
  if (NOT EXISTS ${BITCOIN_LIBRARIE})
    message(FATAL_ERROR "${BITCOIN_LIBRARIE} not exists!")
  endif()
endforeach()

#
# libsecp256k1
#
set(secp256k1_DIR "${CHAIN_SRC_ROOT}/src/secp256k1")
set(secp256k1_LIBRARIES "${secp256k1_DIR}/.libs/libsecp256k1.a")
if (NOT EXISTS ${secp256k1_LIBRARIES})
  message(STATUS "build secp256k1...")
  execute_process(WORKING_DIRECTORY "${secp256k1_DIR}" COMMAND ./autogen.sh)
  execute_process(WORKING_DIRECTORY "${secp256k1_DIR}" COMMAND ./configure --enable-module-recovery)
  execute_process(WORKING_DIRECTORY "${secp256k1_DIR}" COMMAND make ${MAKE_JOBS})
endif()
if (NOT EXISTS ${secp256k1_LIBRARIES})
  message(FATAL_ERROR "build secp256k1 failed!")
endif()

#
# libunivalue
#
set(univalue_DIR "${CHAIN_SRC_ROOT}/src/univalue")
set(univalue_LIBRARIES "${univalue_DIR}/.libs/libunivalue.a")
if (NOT EXISTS ${univalue_LIBRARIES})
  message(STATUS "build univalue...")
  execute_process(WORKING_DIRECTORY "${univalue_DIR}" COMMAND ./autogen.sh)
  execute_process(WORKING_DIRECTORY "${univalue_DIR}" COMMAND ./configure --enable-module-recovery)
  execute_process(WORKING_DIRECTORY "${univalue_DIR}" COMMAND make ${MAKE_JOBS})
endif()
if (NOT EXISTS ${univalue_LIBRARIES})
  message(FATAL_ERROR "build univalue failed!")
endif()

#
# libleveldb
#
set(leveldb_DIR "${CHAIN_SRC_ROOT}/src/leveldb")
set(leveldb_LIBRARIES "${leveldb_DIR}/libleveldb.a" "${leveldb_DIR}/libmemenv.a")
foreach(LEVELDB_LIBRARIE ${leveldb_LIBRARIES})
  if (NOT EXISTS ${LEVELDB_LIBRARIE})
    message(FATAL_ERROR "${LEVELDB_LIBRARIE} not exists!")
  endif()
endforeach()

find_package(OpenSSL)
if(NOT OPENSSL_FOUND)
  message(FATAL_ERROR "OpenSSL not found!")
endif()

find_package(libzmq)
if(NOT LIBZMQ_FOUND)
  message(FATAL_ERROR "libzmq not found!")
endif()

find_package(Glog)
if(NOT GLOG_FOUND)
  message(FATAL_ERROR "Glog not found!")
endif()

find_package(LibEvent)
if(NOT LibEvent_FOUND)
  message(FATAL_ERROR "libevent2 not found!")
endif()

find_package(Boost 1.36.0 COMPONENTS log system filesystem program_options thread regex timer chrono serialization)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found!")
endif()

find_package(LibPthread)
if(NOT PTHREAD_FOUND)
  message(FATAL_ERROR "libpthread not found!")
endif()

find_package(BDB)
if(NOT BDB_FOUND)
  message(FATAL_ERROR "Berkeley DB not found!")
endif()

#MYSQL

execute_process(COMMAND mysql_config --libs_r OUTPUT_VARIABLE MYSQL_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --include OUTPUT_VARIABLE MYSQL_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
message("-- MySQL include: ${MYSQL_INCLUDE}")
message("-- MySQL library: ${MYSQL_LIB}")
if(NOT MYSQL_LIB)
  message(STATUS "MySQL library not found!")
endif()
if(NOT MYSQL_INCLUDE)
  message(STATUS "MySQL header not found!")
endif()

if(MYSQL_LIB AND MYSQL_INCLUDE)
set(MYSQL "use")
add_definitions("-DMYSQL_ENABLE")
endif()

message("") # add an empty line

###################################### Targets ######################################
message("") # empty line
message("------------------------------- Targets -------------------------------")

include_directories(common
                    ${CHAIN_SRC_ROOT}/src
                    ${CHAIN_SRC_ROOT}/src/config
                    ${CHAIN_SRC_ROOT}/src/wallet
                    ${CHAIN_SRC_ROOT}/src/crypto
                    ${CHAIN_SRC_ROOT}/src/hello
                    ${secp256k1_DIR}/include
                    ${univalue_DIR}/include
                    ${leveldb_DIR}/include
                    ${OPENSSL_INCLUDE_DIR}
                    ${Boost_INCLUDE_DIRS}
                    ${LIBEVENT_INCLUDE_DIR}
                    ${LIBZMQ_INCLUDE_DIR}
                    ${BDB_INCLUDE_DIR}
                    ${GLOG_INCLUDE_DIRS}
                    )
if(MYSQL)
include_directories(${MYSQL_INCLUDE})
endif()
set(THIRD_LIBRARIES ${BITCOIN_LIBRARIES}
                    ${OPENSSL_CRYPTO_LIBRARY}
                    ${OPENSSL_SSL_LIBRARY}
                    ${univalue_LIBRARIES}
                    ${leveldb_LIBRARIES}
                    ${Boost_LIBRARIES}
                    ${LIBZMQ_LIBRARIES}
                    ${BDB_LIBRARYS}
                    ${LIBEVENT_LIB}
                    ${LIBEVENT_PTHREADS_LIB}
                    ${secp256k1_LIBRARIES}
                    ${PTHREAD_LIBRARIES}
                    ${GLOG_LIBRARIES}
                    )
if(MYSQL)
list(APPEND THIRD_LIBRARIES ${MYSQL_LIB})
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

message("") # empty line
file(GLOB LIB_SOURCES common/*.cc)
file(GLOB LIB_HEAD common/*.h)
add_library(vdslib STATIC ${LIB_SOURCES})

file(GLOB_RECURSE VDS_SOURCES vdstool/*.cc)
add_executable(vdstool ${VDS_SOURCES})
target_link_libraries(vdstool vdslib ${THIRD_LIBRARIES})