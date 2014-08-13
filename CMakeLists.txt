cmake_minimum_required (VERSION 2.6)
project (Gyro)

set (EXECUTABLE_OUTPUT_PATH bin)
set (IP "192.168.112.112")
set (CROSS_COMPILE_FLAGS "-m32 -march=i586 --sysroot=/opt/clanton-tiny/1.4.2/sysroots/i586-poky-linux-uclibc")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CROSS_COMPILE_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CROSS_COMPILE_FLAGS} -Wall -std=c++11")

include_directories (include)

set (
  BINARY_NAME
  gyro
)

set (
  HEADERS
  include/main.hh
)

set (
  SOURCES
  src/main.cc
)

add_executable (
  ${BINARY_NAME}
  ${SOURCES}
  ${HEADERS}
)

target_link_libraries (
  ${BINARY_NAME}
  pthread
)

# Create install rule
install (TARGETS ${BINARY_NAME} RUNTIME DESTINATION .)

# Deploy the binary on the board
add_custom_target (
  deploy
  COMMAND
  scp ${BINARY_NAME} root@${IP}:
  DEPENDS
  ${BINARY_NAME}
  WORKING_DIRECTORY
  ${EXECUTABLE_OUTPUT_PATH}
)

# Deploy the binary and execute it on the board
add_custom_target (
  run
  COMMAND
  ssh root@${IP} './${BINARY_NAME}'
  DEPENDS
  deploy
  WORKING_DIRECTORY
  ${EXECUTABLE_OUTPUT_PATH}
)
