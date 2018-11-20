#include "task.h"  
  
Task::Task(const std::string &message, TASKFLAG flag):  
    m_message(message),  
    m_flag(flag)  
{  
}  

const std::string& Task::getMessage() const  
{  
    return m_message;  
}  
  
Task::TASKFLAG Task::getFlag() const  
{  
    return m_flag;  
}  
  
void Task::setIP(in_addr _ip)  
{  
    ip = _ip;  
}  
  
int Task::getS_fd() const  
{  
    return s_fd;  
}  
  
void Task::setS_fd(int _fd)  
{  
    s_fd = _fd;  
}  
  
std::string Task::getData() const  
{  
    std::string re;  
    if (m_flag == CONNECT) {  
        //re = ::inet_ntoa(ip) + std::string("----->") + "CONNECT!    " + m_message;
        re = "fd[" + std::to_string(s_fd) + "]" + std::string("----->") + "CONNECT!    " + m_message;
    } else {  
        if (m_flag == DISCONNECT) {  
            //re = ::inet_ntoa(ip) + std::string("----->") + "DISCONNECT   " + m_message;;
            re = "fd["+std::to_string(s_fd) + "]" + std::string("----->") + "DISCONNECT   " + m_message;
        } else {  
            //re = ::inet_ntoa(ip) + std::string("----->Talk:") + m_message;
            re = "fd["+std::to_string(s_fd) + "]" + std::string("----->Talk:") + m_message;
        }  
    }  
    return re;  
}  
