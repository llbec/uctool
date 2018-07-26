message("") # empty line
message("--   target ulordcenter")

include_directories(${CHAIN_SRC_ROOT}/src
                    ${PROJECT_SOURCE_DIR}/common
                    )
link_directories( ${PROJECT_BINARY_DIR}/lib )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(IMPORTLIB
    ${THIRD_LIBRARIES}
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
target_link_libraries(ulordcenter utcenter ${IMPORTLIB})

message("--   ulordcenter end")
message("") # empty line 
