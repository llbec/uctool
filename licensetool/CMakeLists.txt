message("") # empty line
message("--   ********* LicenseTool *********")
message("--   target LicenseTool")

###################################### Libs ######################################
message("") # empty line
message("------------------------------- Libs -------------------------------")
execute_process(COMMAND mysql_config --libs_r OUTPUT_VARIABLE MYSQL_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --include OUTPUT_VARIABLE MYSQL_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
message("-- MySQL include: ${MYSQL_INCLUDE}")
message("-- MySQL library: ${MYSQL_LIB}")
if(NOT MYSQL_LIB)
  message(FATAL_ERROR "MySQL library not found!")
endif()
if(NOT MYSQL_INCLUDE)
  message(FATAL_ERROR "MySQL header not found!")
endif()

include_directories(${CHAIN_SRC_ROOT}/src ${PROJECT_SOURCE_DIR}/common)
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
target_link_libraries(licensetool utcenter ${IMPORTLIB})

message("--   ********* LicenseTool End *********")
message("") # empty line 