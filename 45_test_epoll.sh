#!/bin/bash

# 配置目标服务器的 IP 和 端口
SERVER_IP="127.0.0.1"
SERVER_PORT=8888

# 接收脚本的第一个参数作为客户端数量，如果不传则默认为 10
NUM_CLIENTS=${1:-10}

echo "=== 开始向 $SERVER_IP:$SERVER_PORT 发起 $NUM_CLIENTS 个并发请求 ==="

# 循环启动 nc 客户端
for (( i=1; i<=NUM_CLIENTS; i++ ))
do
    # 构造要发送的具体内容，实现数字递增
    MSG="input number $i"
    
    # 将消息通过管道传给 nc，并使用 & 将其放入后台并发执行
    # 注意：标准输入 EOF 后，nc 会将数据发送给服务端。
    echo "$MSG" | nc $SERVER_IP $SERVER_PORT &
    
    # 打印本地执行日志
    echo "[本地] 启动客户端 $i，发送: $MSG"
    
    # (可选) 如果你发现并发太快导致你的系统抛出 "Too many open files" 错误，
    # 或者想更清晰地观察服务端的逐个响应，可以取消下方 sleep 的注释。
    # sleep 0.1 
done

# wait 命令会阻塞当前脚本，直到所有后台任务（通过 & 启动的进程）执行完毕
wait

echo "=== 所有客户端并发请求已下发完毕 ==="