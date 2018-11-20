    #include "epoll_server.h"
    #include "common.h"

    Epoll_server::Epoll_server(int port):_port(port),  
        server_socket_fd(-1),  
        _epoll(0),  
        on(true)  
    {
    }
      
    Epoll_server::~Epoll_server()  
    {  
        if (isValid()) {  
            ::close(server_socket_fd);  
        }  
        delete _epoll;
    }
      
    inline  
    bool Epoll_server::isValid() const  
    {  
        return server_socket_fd > 0;  
    }  

    inline  
    void Epoll_server::poweroff()  
    {  
        on = false;  
    }  

    inline  
    bool Epoll_server::states() const  
    {  
        return on;  
    }  
      
    int Epoll_server::setSocketOpt(int socket_fd)
    {
        int yes;
        int reuseRes = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if(reuseRes < 0)
        {
            return -1;
        }
    }
      
    int Epoll_server::setNonblocking(int socket_fd)  
    {  
        int opts;  
        opts = fcntl(socket_fd, F_GETFL);  
        if (opts < 0)
        {
            return -1;  
        } else  
        {  
            opts = opts | O_NONBLOCK;  
            if (fcntl(socket_fd, F_SETFL, opts) < 0)
            {
                return -1;  
            }  
        }  
        return 0;  
    }

    void Epoll_server::doTask(const Task &t)
    {  
        std::list<FDtoIP>::iterator ite = fd_IP.begin();  
        std::list<FDtoIP>::iterator ite1 = fd_IP.end();  
        for (;ite != fd_IP.end();++ite)
        {
            if ((*ite).first == t.getS_fd())
            {
                memset(&m_event, '\0', sizeof(m_event));  
                m_event.events = EPOLLOUT | EPOLLET;  
                Task *c = new Task(t);  
                c->setS_fd((*ite).first);
                m_event.data.ptr = static_cast<void*>(c);  
                _epoll->mod((*ite).first, &m_event);  
            } else
            {
                ite1 = ite;  
            }  
        }  
        if (t.getFlag() == Task::DISCONNECT)
        {
            if (ite1 != fd_IP.end())
            {
                fd_IP.erase(ite1);  
            }  
      
        }  
      
    }  
    /** 
     * @brief Epoll_server::acceptSocketEpoll 有用户接入 
     * @return 
     */  
    int Epoll_server::acceptSocketEpoll()  
    {
        std::string log_str;
        socklen_t len = sizeof(struct sockaddr_in);  
        int connect_fd;  
        while ((connect_fd = ::accept(server_socket_fd,  
                                      (struct sockaddr*)(&client_addr), &len)) > 0)
        {
            if (setNonblocking(connect_fd) < 0)
            {
                ::close(connect_fd);  
                continue;  
            }
            log_str = "new accept :" + std::to_string(connect_fd);
            log_output(log_str);
            m_event.data.fd = connect_fd;  
            m_event.events = EPOLLIN | EPOLLET;  
            if (_epoll->add(connect_fd, &m_event) < 0)
            {
                ::close(connect_fd);  
                continue;  
            } else {  
      
                fd_IP.push_back(FDtoIP(connect_fd, client_addr.sin_addr));  
                Task t("come in", Task::CONNECT);  
                t.setIP(client_addr.sin_addr);  
                t.setS_fd(connect_fd);  
                //doTask(t);
                //printf("%s\n",t.getData().c_str());
            }
        }  
      
        if (connect_fd == -1 && errno != EAGAIN && errno != ECONNABORTED  
                && errno != EPROTO && errno !=EINTR)
        {
            return -1;
        }  

        return 0;
    }  

    int Epoll_server::readSocketEpoll(const epoll_event &ev)  
    {  
        int n = 0;  
        int nread = 0;  
        char buf[BUFSIZ]={'\0'};
        std::string log_str;
        while ((nread = ::read(ev.data.fd, buf + n, BUFSIZ-1)) > 0)
        {
            n += nread;  
        }  
        if (nread == -1 && errno != EAGAIN)
        {
            return -1;  
        }  

        std::list<FDtoIP>::iterator ite = fd_IP.begin();  
        for (;ite != fd_IP.end();++ite)
        {
            if ((*ite).first == ev.data.fd)
            {
                break;  
            }  
        }  
      
        if (nread == 0)
        {
            strcpy(buf, " disconet  left ");  
            Task t(buf,Task::DISCONNECT);  
            t.setIP(client_addr.sin_addr);  
            t.setS_fd((*ite).first);  
            doTask(t);
            //printf("%s[%d]\n",t.getData().c_str(),__LINE__);
            log_str = "fd[" + std::to_string(ev.data.fd) + "]:" + buf;
            log_output(log_str);
            close(ev.data.fd);  
        } else {  
            Task t(buf,Task::TALKING);  
            t.setIP(client_addr.sin_addr);  
            t.setS_fd((*ite).first);  
            doTask(t);
            //printf("%s\n",t.getData().c_str());
            log_str = "recv from fd[" + std::to_string(ev.data.fd) + "]:" + buf;
            log_output(log_str);
        }  
      
        //    Task *t = new Task(buf,Task::DISCONNECT);  
        //    t->setIP((*ite).second);  
        //    t->setS_fd((*ite).first);  

        // m_event.data.fd = ev.data.fd;  
        // m_event.events = ev.events;  
        return 0;//_epoll->mod(m_event.data.fd, &m_event);  
    }

    int Epoll_server::writeSocketEpoll(const epoll_event &ev)
    {
        std::string log_str;
        Task *t = static_cast<Task*>(ev.data.ptr);
        char buf[BUFSIZ]={'\0'};
        int data_size =  t->getMessage().size();
        memcpy(buf,t->getMessage().c_str(),t->getMessage().size());
        int fd = t->getS_fd();  
        int n = data_size;  
        delete t;
        int nwrite = write(fd, buf, n);
        if(nwrite == -1)
        {
            if(errno == EAGAIN)
            {
                std::cout << "write eagain!!!" << std::endl;
                return -1;
            }
        }
        //printf("sendto fd[%d]: %s[%s][%d]\n",fd,buf,__FUNCTION__,__LINE__);
        log_str = "sendto fd[" + std::to_string(fd) + "]:" + buf;
        log_output(log_str, true);
        /*
        while (n > 0)
        {
            //int nwrite = ::write(fd, buf + data_size - n, n);
            int nwrite = ::write(fd, buf , n);
            if (nwrite < 0)
            {
                if (nwrite == -1 && errno != EAGAIN)
                {
                    return -1;  
                }  
                break;  
            }  
            n -= nwrite;  
        }  
      */
        memset(&m_event, '\0', sizeof(m_event));  
        // m_event.events &= ~EPOLLOUT;  
        m_event.events = EPOLLIN | EPOLLET;  
        m_event.data.fd = fd;  
        if (_epoll->mod(fd, &m_event) < 0)
        {
            ::close(m_event.data.fd);  
            return -1;  
        }  
        return 0;  
    }  
      
    /** 
     * @brief Epoll_server::bind 
     * @return 
     */  
    int Epoll_server::bind()  
    {  
        server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);  
        if (server_socket_fd < 0)
        {
            return -1;  
        }
        setSocketOpt(server_socket_fd);
        setNonblocking(server_socket_fd);  
      
        _epoll = new Epoll();  
        if (_epoll->create() < 0)
        {
            return -1;  
        }  
        memset(&m_event, '\0', sizeof(m_event));  
        m_event.data.fd = server_socket_fd;  
        m_event.events = EPOLLIN | EPOLLET;  
        _epoll->add(server_socket_fd, &m_event);  
      
        memset(&server_addr, 0 ,sizeof(server_addr));  
        server_addr.sin_family = AF_INET;  
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
        server_addr.sin_port = htons(_port);  
      
        return ::bind(server_socket_fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr));
    }
      
    int Epoll_server::listen()  
    {
        std::string log_str;
        if (isValid())
        {
            if (::listen(server_socket_fd, BLOCKLOG) < 0)
            {
                std::cout << "listen failed" << std::endl;
                log_str = "listen failed :" + std::to_string(server_socket_fd);
                log_output(log_str);
                return -1;  
            } else {  
                while (on)
                {
                    int num = _epoll->wait();  
                    for (int i = 0;i < num;++i)
                    {
                        /** 
                         * 接受连接的连接，把她加入到epoll中 
                         */  
                        if ((*_epoll)[i].data.fd == server_socket_fd)
                        {
                            //std::cout << "new accept!" << std::endl;
                            //log_str = "new accept :" + std::to_string(server_socket_fd);
                            //log_output(log_str);

                            if (acceptSocketEpoll() < 0) {  
                                break;  
                            }  
                            continue;
                        }  
                        /** 
                         * EPOLLIN event 
                         */  
                        if ((*_epoll)[i].events & EPOLLIN)
                        {
                            if (readSocketEpoll((*_epoll)[i]) < 0)
                            {

                                break;  
                            }  
                            continue;
                        }
                        /** 
                         * EPOLLOUT event 
                         */  
                        if ((*_epoll)[i].events & EPOLLOUT)
                        {
                            if (writeSocketEpoll((*_epoll)[i]) < 0)
                            {

                                break;  
                            }
                        }  
                    }  
                }  
            }  
        }  
        return -1;  
    }  
