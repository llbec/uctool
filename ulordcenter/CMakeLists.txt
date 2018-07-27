message("") # empty line
message("--   target ulordcenter")

#message("PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
#message("PROJECT_BINARY_DIR= ${PROJECT_BINARY_DIR}")
#message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
set(MUDUO_LIB_INCLUDE ${CMAKE_SOURCE_DIR}/lib)

include_directories(${CHAIN_SRC_ROOT}/src ${PROJECT_SOURCE_DIR}/common ${CMAKE_SOURCE_DIR})
link_directories(${PROJECT_BINARY_DIR}/lib  /usr/local/lib ${MUDUO_LIB_INCLUDE})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(IMPORTLIB
    ${THIRD_LIBRARIES}
    boost_log_setup
    muduo_base_cpp11
    muduo_net_cpp11
    pthread
   )

if(NOT APPLE)
  list(APPEND IMPORTLIB anl)
endif()
if(NOT CENTOS)
  list(APPEND IMPORTLIB miniupnpc)
endif()

file(GLOB UCENTER_HEADERS "*.h" )
file(GLOB UCDENTER_SRC "*.cpp"  "*.cc")
add_executable(ulordcenter ${UCDENTER_SRC} ${UCENTER_HEADERS})
target_link_libraries(ulordcenter ulordtool ${IMPORTLIB})

message("--   ulordcenter end")
message("") # empty line 
