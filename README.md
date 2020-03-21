# 简介

这是一个简单的网络库, 实现参考了 muduo, 即采用 Reactor 模式, 特点有 one loop per thread, I/O multiplexing(Epoll LT), rand robin 分配 connection.