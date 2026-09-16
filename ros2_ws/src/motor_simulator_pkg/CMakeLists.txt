cmake_minimum_required(VERSION 3.8)
project(motor_simulator_pkg)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# Motor model library
add_library(motor_model
  src/motor_model.cpp
)

target_include_directories(motor_model PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Motor simulator node
add_executable(motor_simulator
  src/motor_simulator.cpp
)

target_link_libraries(motor_simulator
  motor_model
)

ament_target_dependencies(motor_simulator
  rclcpp
  std_msgs
)

# torque generator node
add_executable(torque_generator
  src/torque_generator.cpp
)

ament_target_dependencies(torque_generator
  rclcpp
  std_msgs
)

# Install header files
install(
  DIRECTORY include/
  DESTINATION include
)

# Install executable
install(
  TARGETS
    motor_model
    motor_simulator
    torque_generator
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()