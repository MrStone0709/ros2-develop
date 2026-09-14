cmake_minimum_required(VERSION 3.8)

project(signal_filter_pkg)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找 ROS 2 依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# =========================
# 信号发生器节点
# =========================
add_executable(signal_generator
  src/signal_generator.cpp
)

ament_target_dependencies(signal_generator
  rclcpp
  std_msgs
)

# =========================
# 信号滤波器节点
# =========================
#add_executable(signal_filter
#  src/signal_filter.cpp
#)

#ament_target_dependencies(signal_filter
#  rclcpp
#  std_msgs
#)

# =========================
# 安装可执行文件
# =========================
install(
  TARGETS
    signal_generator
    #signal_filter
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()