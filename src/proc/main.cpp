#include "epoll_server.h"
#include "common.h"
#include <iostream>
  
int main(int /*argc*/, char const **/*argv[]*/)  
{
    std::string log_str;
    //日志初始化
    log_init();
    //打印系统启动消息
    log_str = "epoll_server系统启动...版本号V1.0_20181120";
    log_output(log_str);

    std::cout << "epoll server start!" << std::endl;
    //Epoll_server s(18090);
    Epoll_server s(6066);
    if (s.bind() < 0) {
        std::cout << "bind failed" << std::endl;
        return -1;  
    }  
    return s.listen();  
} 
