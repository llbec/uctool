message("") # empty line
message("--   target LicenseTool")

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

file(GLOB LICTOOL_HEADERS "*.h" )
file(GLOB LICTOOL_SRC "*.cpp"  "*.cc")
add_executable(licensetool ${LICTOOL_SRC} ${LICTOOL_HEADERS})
target_link_libraries(licensetool ulordtool ${IMPORTLIB})

message("--   LicenseTool end")
message("") # empty line 
