#!/bin/bash
# 在容器内启动 foxglove_bridge，供主机(WSL/Windows)上的 Foxglove 连接 ROS 2 话题。
#
# 用法：
#   .script/run_foxglove_bridge.sh                 # 默认端口 8766
#   .script/run_foxglove_bridge.sh 9000            # 指定端口
#   FOXGLOVE_PORT=9000 .script/run_foxglove_bridge.sh
#
# 端口说明（重要）：
#   本环境 WSL 使用 mirrored 网络模式，与 Windows 共享端口空间。
#   Foxglove 官方默认端口 8765 已被 Windows 上的 verge-mihomo.exe (Clash Verge) 占用，
#   因此这里默认改用 8766。若 8766 也被占用，换一个未被占用的端口即可。

set -eo pipefail

PORT="${1:-${FOXGLOVE_PORT:-8766}}"
WS="/workspaces/ros2-develop/ros2_ws"

# ROS 2 基础环境
# shellcheck disable=SC1091
source /opt/ros/jazzy/setup.bash

# 工作区 overlay（用户自己的包：pub_sub_pkg / signal_filter_pkg）
if [ -f "${WS}/install/setup.bash" ]; then
    # shellcheck disable=SC1091
    source "${WS}/install/setup.bash"
fi

# 端口占用预检查，提前给出可读的错误而不是 bind 失败
if command -v netstat >/dev/null 2>&1 && netstat -ltn 2>/dev/null | grep -q ":${PORT} "; then
    echo "错误：端口 ${PORT} 已被占用。请换一个端口，例如：$0 8767" >&2
    netstat -ltnp 2>/dev/null | grep ":${PORT} " >&2 || true
    exit 1
fi

echo "启动 foxglove_bridge (端口 ${PORT})..."
echo "主机 Foxglove 连接地址： ws://localhost:${PORT}"
echo "  - Foxglove 中选 'Open connection' -> 'Foxglove WebSocket' -> ws://localhost:${PORT}"
echo "  - 若连接被代理拦截，请在代理软件(Clash Verge)绕过列表中放行 localhost/127.0.0.1"
echo

exec ros2 run foxglove_bridge foxglove_bridge --ros-args -p port:="${PORT}"
