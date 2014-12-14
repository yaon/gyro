cmake_minimum_required (VERSION 2.6)
project (Gyro)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (EXECUTABLE_OUTPUT_PATH bin)
set (IP "192.168.112.112")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++11")

include_directories (include)
include_directories (include/arduino_api)

set (
  BINARY_NAME
  gyro
)
set (
  TEST_ACCELERO
  test_accelero
)
set (
  TEST_MOTORS
  test_motors
)

# Arduino API
set (
  ARDUINO_HEADERS
  include/arduino_api/PID_v1.h
  include/arduino_api/WString.h
  include/arduino_api/WMath.h
  include/arduino_api/wiring_digital.h
  include/arduino_api/Wire.h
  include/arduino_api/WCharacter.h
  include/arduino_api/variant.h
  include/arduino_api/UtilTime.h
  include/arduino_api/Udp.h
  include/arduino_api/TTYUART.h
  include/arduino_api/Print.h
  include/arduino_api/trace.h
  include/arduino_api/sysfs.h
  include/arduino_api/Stream.h
  include/arduino_api/Server.h
  include/arduino_api/RingBuffer.h
  include/arduino_api/pins_arduino.h
  include/arduino_api/Printable.h
  include/arduino_api/OSAbstract.h
  include/arduino_api/Mux.h
  include/arduino_api/AdvancedIO.h
  include/arduino_api/AnalogIO.h
  include/arduino_api/Arduino.h
  include/arduino_api/binary.h
  include/arduino_api/BitsAndBytes.h
  include/arduino_api/fast_gpio_sc.h
  include/arduino_api/HardwareSerial.h
  include/arduino_api/Client.h
  include/arduino_api/i2c-dev.h
  include/arduino_api/i2c.h
  include/arduino_api/interrupt.h
  include/arduino_api/IPAddress.h
)

set (
  ARDUINO_SOURCES
  src/arduino_api/PID_v1.cpp
  src/arduino_api/fast_gpio_sc.c
  src/arduino_api/i2c.c
  src/arduino_api/interrupt.c
  src/arduino_api/IPAddress.cc
  src/arduino_api/mux.c
  src/arduino_api/Print.cc
  src/arduino_api/RingBuffer.cc
  src/arduino_api/Stream.cc
  src/arduino_api/sysfs.c
  src/arduino_api/trace.c
  src/arduino_api/TTYUART.cc
  src/arduino_api/UtilTime.cc
  src/arduino_api/variant.cc
  src/arduino_api/Wire.cc
  src/arduino_api/wiring_analog.c
  src/arduino_api/wiring_digital.c
  src/arduino_api/WMath.cc
  src/arduino_api/WString.cc
)

set (
  HEADERS
  include/main.hh
  include/accelero.hh
  include/motors.hh
)

set (
  SOURCES
  src/main.cc
  src/accelero.cc
  src/motors.cc
)

add_executable (
  ${BINARY_NAME}
  ${ARDUINO_SOURCES}
  ${ARDUINO_HEADERS}
  ${SOURCES}
  ${HEADERS}
)
target_link_libraries (
  ${BINARY_NAME}
  pthread
)

add_executable (
  ${TEST_ACCELERO}
  ${ARDUINO_SOURCES}
  ${ARDUINO_HEADERS}
  test/test_accelero.cc
  include/accelero.hh
  src/accelero.cc
)
target_link_libraries (
  ${TEST_ACCELERO}
  pthread
)

add_executable (
  ${TEST_MOTORS}
  ${ARDUINO_SOURCES}
  ${ARDUINO_HEADERS}
  test/test_motors.cc
  include/motors.hh
  src/motors.cc
)
target_link_libraries (
  ${TEST_MOTORS}
  pthread
)

# Create install rule
install (TARGETS ${BINARY_NAME} RUNTIME DESTINATION .)

# Deploy the binary on the board
add_custom_target (
  deploy
  COMMAND
  ssh root@${IP} 'rm -f ./gyro' &&
  scp ${BINARY_NAME} root@${IP}: &&
  scp ${TEST_ACCELERO} root@${IP}: &&
  scp ${TEST_MOTORS} root@${IP}:
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

# zip the source onto source.zip
add_custom_target (
  dist
  COMMAND
  cd ${EXECUTABLE_OUTPUT_PATH}/../.. &&
  zip source.zip AUTHORS README CMakeLists.txt configure ${SOURCES} ${HEADERS} test/test_accelero.cc test/test_motors.cc
)
