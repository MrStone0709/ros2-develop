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
find_package(ament_index_cpp REQUIRED)

# Motor model library
add_library(motor_model
  src/motor_model.cpp
)

target_include_directories(motor_model PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

#PID library
add_library(PID
  src/PID.cpp
)

target_include_directories(PID PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

#csv reader library
add_library(csv_reader
  src/csv_reader.cpp
)

target_include_directories(csv_reader PUBLIC
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
  ament_index_cpp
)

#angle generator node
add_executable(angle_generator
  src/angle_generator.cpp
)

target_link_libraries(angle_generator
  csv_reader
)

ament_target_dependencies(angle_generator
  rclcpp
  std_msgs
  ament_index_cpp
)

# motor controller node
add_executable(motor_controller
  src/motor_controller.cpp
)

target_link_libraries(motor_controller
  PID
)

ament_target_dependencies(motor_controller
  rclcpp
  std_msgs
)

# Install header files
install(
  DIRECTORY include/
  DESTINATION include
)

# Install data files
install(
  DIRECTORY data/
  DESTINATION share/${PROJECT_NAME}/data
)

# Install executable
install(
  TARGETS
    motor_model
    PID
    csv_reader
    motor_simulator
    torque_generator
    angle_generator
    motor_controller
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()