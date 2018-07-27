message("") # empty line
message("--   ********* tools *********")
message("--   target tools")

message("--   include path ${PROJECT_SOURCE_DIR}/common")
include_directories(${CHAIN_SRC_ROOT}/src ${PROJECT_SOURCE_DIR}/common)
link_directories( ${PROJECT_BINARY_DIR}/lib )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(IMPORTLIB ${BITCOIN_LIBRARIES} ${THIRD_LIBRARIES}
	pthread
)
if(NOT APPLE)
  list(APPEND IMPORTLIB anl)
endif()
if(NOT CENTOS)
  list(APPEND IMPORTLIB miniupnpc)
endif()

#message("--      ${IMPORTLIB}")

file(GLOB TOOL_HEADERS "*.h" )
file(GLOB TOOL_SRC "*.cpp"  "*.cc")
add_executable(tools ${TOOL_SRC} ${TOOL_HEADERS})
target_link_libraries(tools ulordtool ${IMPORTLIB})

message("--   ********* tools end *********")
message("") # empty line 
