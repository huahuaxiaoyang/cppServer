#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

void test()
{
	//1. 创建监听服务器的套接字
	int listenfd = ::socket(AF_INET, SOCK_STREAM, 0);	
	if(listenfd < 0) 
    {
		perror("socket");
		return;
	}
    
    //地址复用
    int opt = 1;
    int ret = setsockopt(listenfd,  SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(-1 == ret)
    {
        perror("setsockopt ip error");
        close(listenfd);
        return;
    }

    //端口重用
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if(-1 == ret)
    {
        perror("setsockopt port error");
        close(listenfd);
        return;
    }

	//网络地址需要采用网络字节序存储(大端模式)
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socklen_t length = sizeof(serveraddr);


	//2. 绑定服务器的网络地址
	if(::bind(listenfd, (struct sockaddr*)&serveraddr, length) < 0)
    {
		perror("bind");
		//文件描述符是比较稀缺的，所以不用的时候要回收
		close(listenfd);
		return;
	}

	//3. 让服务器开始监听
	// listenfd跟所有的新连接打交道
	if(::listen(listenfd, 128) < 0)
    {
		perror("listen");
		close(listenfd);
		return;
	} 
    cout << "server is listening..." << endl;

	// 创建epoll实例
	int efd = ::epoll_create1(0);// 红黑树 + 就绪链表

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = listenfd;
	//epoll要进行监听操作: 对listenfd的读事件进行监听
    //
	//Reactor: 注册读就绪事件
	ret = ::epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &ev);
	if(ret < 0) 
    {
		perror("epoll_ctl");
		close(listenfd);
		close(efd);
		return;
	}

	struct epoll_event *evtList = 
        (struct epoll_event *)malloc(1024 * sizeof(struct epoll_event));

	//事件循环
	while(1)
    {
		//Reactor: 事件分离器
		int nready = ::epoll_wait(efd, evtList, 1024, 3000);
		if(-1 == nready && errno == EINTR)
        {
			continue;
        }
		else if(-1 == nready) 
        {
			perror("epoll_wait");
			return;
		} 
        else if(0 == nready) 
        {
			printf(">> epoll_wait timeout!\n");
		} 
        else
        {
			//遍历struct epoll_event数组, 去check
			//每一个epoll_event到底发生了什么事件
			for(int idx = 0; idx < nready; ++idx)
            {
				// 必须要使用按位&操作来判断事件，不能使用==,&&
				if((evtList[idx].data.fd == listenfd)  &&
				   (evtList[idx].events & EPOLLIN)) 
				{   
					//意味着有新连接来了,所以要调用accept函数,获取新连接
					//写事件什么情况会触发? 只要内核发送缓冲区还有空间，就可以触发写事件
					int peerfd = ::accept(listenfd, nullptr, nullptr);	
					/* TcpConnection conn(peerfd); */

					//将新连接添加到epoll的监听实例中去
					struct epoll_event ev;
					ev.events = EPOLLIN | EPOLLOUT | EPOLLERR;
					ev.data.fd = peerfd;
					ret = ::epoll_ctl(efd, EPOLL_CTL_ADD, peerfd, &ev);
					if(ret < 0) 
                    {
						perror("epoll_ctl");
						continue;
					}

					//新连接到来之后的处理
					printf(">> conn has connected, fd: %d\n", peerfd);
					//记录日志, 使用Log4cpp完成
					//个性定制化 ==> 事件处理器
					/* onConnection();//考虑扩展性，挖一个坑 */
				} 
                else
                {
					// 已经建立好的连接发送数据过来了
					// 如果发生了读事件
					char buff[128] = {0};
					if(evtList[idx].events & EPOLLIN)
                    {
						int fd = evtList[idx].data.fd;
						ret = ::recv(fd, buff, sizeof(buff), 0);

						if(ret > 0) 
                        {
							printf(">>> recv msg %d bytes,content:%s\n",
									ret, buff);
							
							//1. 对应用层数据进行解析
							//2. 拿到最终要处理的数据之后，进行业务逻辑处理
							//(假设第2步执行的时间很长1S, 是否合适)
							//3. 得到要返回给客户端的数据之后，进行发送操作
							//

							ret = send(fd, buff, strlen(buff), 0);
							printf(">>> send %d bytes\n", ret);
							/* onMessage();//, 考虑扩展性，挖一个坑 */
						}
                        else if(ret == 0)
                        {
							printf("conn has closed!\n");
							
							//需要从epoll的监听实例删除掉, 因为连接断开，不需要再监听了
							struct epoll_event ev;
							ev.data.fd = fd;
							ret = ::epoll_ctl(efd, EPOLL_CTL_DEL, fd, &ev);
							if(ret < 0) 
                            {
								perror("epoll_ctl");
							}
							//记录日志, log4cpp
							/* onClose();//考虑扩展性，挖一个坑 */
						}// end of ret if
					}//end of event if
					//else if()  //处理其他事件的
				}
			}
		}
    }
	
	close(listenfd);// 关闭连接
}
 
int main(int argc, char *argv[])
{
	test();
	return 0;
}
