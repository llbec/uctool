message("") # empty line
message("--   target ulordcenter")

message("--    nuduo libs")
set(MUDUO_DIR ${CMAKE_SOURCE_DIR}/ulordcenter/3rdlibs)
set(MUDUO_LIB_DIR ${MUDUO_DIR}/build/release/lib)
set(MUDUO_LIBRARIES ${MUDUO_LIB_DIR}/libmuduo_net_cpp11.a ${MUDUO_LIB_DIR}/libmuduo_base_cpp11.a)
set(MUDUO_INCLUDE ${MUDUO_DIR}/muduo)
message("MUDUO_LIBRARIES = ${MUDUO_LIBRARIES}")

list(GET MUDUO_LIBRARIES 0 MUDUO_LIBRARIE)
if (NOT EXISTS ${MUDUO_LIBRARIE})
  message(STATUS "building muduo...")
  execute_process(WORKING_DIRECTORY "${MUDUO_INCLUDE}" COMMAND ./build.sh)
  execute_process(WORKING_DIRECTORY "${MUDUO_INCLUDE}" COMMAND ./build.sh)
endif()

foreach(MUDUO_LIBRARIE ${MUDUO_LIBRARIES})
  if (NOT EXISTS ${MUDUO_LIBRARIE})
    message(FATAL_ERROR "${MUDUO_LIBRARIE} not exists!")
  endif()
endforeach()

include_directories(${CHAIN_SRC_ROOT}/src ${PROJECT_SOURCE_DIR}/common ${MUDUO_INCLUDE})
link_directories(${PROJECT_BINARY_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(IMPORTLIB
    ${THIRD_LIBRARIES}
    ${MUDUO_LIBRARIES}
    pthread
   )

if(NOT APPLE)
  list(APPEND IMPORTLIB anl)
endif()
if(NOT CENTOS)
  list(APPEND IMPORTLIB miniupnpc)
endif()

file(GLOB UCENTER_HEADERS "ulordcenter.h" )
file(GLOB UCDENTER_SRC "ulordcenter.cc"  "ucentermain.cc")
add_executable(ulordcenter ${UCDENTER_SRC} ${UCENTER_HEADERS})
target_link_libraries(ulordcenter ulordtool ${IMPORTLIB})

message("--   ulordcenter end")
message("") # empty line 
