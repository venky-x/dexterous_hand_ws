cmake_minimum_required(VERSION 3.8)
project(scs0009_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

# Build the official Feetech SDK
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../scservo_sdk_official/src
    ${CMAKE_CURRENT_BINARY_DIR}/scservo_sdk
)

# Our custom SCS0009 driver library
add_library(scs0009_driver_lib
    src/SCS0009Driver.cpp
)

target_include_directories(scs0009_driver_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../scservo_sdk_official/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(scs0009_driver_lib PRIVATE SCServo)

target_compile_features(scs0009_driver_lib PUBLIC cxx_std_11)

# Test executable
add_executable(scs0009_driver_test
    test/scs0009_driver_test.cpp
)

target_link_libraries(scs0009_driver_test
    scs0009_driver_lib
)

# Install targets
install(
    TARGETS scs0009_driver_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    TARGETS scs0009_driver_test
    DESTINATION lib/${PROJECT_NAME}
)

install(
    DIRECTORY include/
    DESTINATION include
)

ament_package()
