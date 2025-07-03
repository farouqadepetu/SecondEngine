#include <stdio.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MYPORT "3490"
#define BACKLOG 10
#define MAX_DATA_SIZE 100 //max number of bytes we can recieve at once
#define MAX_EVENTS 5

void SigChildHandler(int unused)
{
	int saved_errno = errno;
	
	//waits for any child process to finish, doesn't block once finished.
	while(waitpid(-1, nullptr, WNOHANG) > 0);
	
	errno = saved_errno;
}

void SendData(int* listenerSocket, int* connectionSocket, int* epollInstance)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		//this is the child process
		
		close(*listenerSocket); //child doesn't need the listener
		
		int sendError = send(*connectionSocket, "Hello, World!", 13, 0);
		if(sendError == -1)
		{
			perror("send");
		}
		
		printf("server: waiting for ack\n");
		
		epoll_event events[MAX_EVENTS];
	
		//block until an event happens
		int event_count = epoll_wait(*epollInstance, events, MAX_EVENTS, -1);
		if(event_count == -1)
		{
			perror("event_count");
			exit(1);
		}
		
		printf("server:child: an event happened! Checking..");
		
		while(true)
		{
			for(int i = 0; i < event_count; ++i)
			{
				//connection has been closed
				if(events[i].events & EPOLLHUP)
				{
					close(*connectionSocket);
					close(*epollInstance);
					exit(0);
				}
				else if(events[i].events & EPOLLIN)
				{
					char buf[MAX_DATA_SIZE];
					int numbytes = recv(*connectionSocket, buf, MAX_DATA_SIZE - 1, 0);
					if(numbytes == -1)
					{
						perror("recv");
						exit(1);
					}
					buf[numbytes] = '\0';
					printf("Recieved %s\n", buf);
				}
			}
		}
	}
	else if(pid < 0)
	{
		//error
		perror("fork");
		close(*listenerSocket);
		exit(1);
	}
	else
	{
		//this is the parent
		close(*connectionSocket); //parent doesn't need the connected socket
	}
}

int NewConnection(int* listenerSocket)
{
	socklen_t sockAddrStorageSize = sizeof(sockaddr_storage);
	sockaddr_storage theirAddr;
	void* addr;
	char ipAddrStr[INET6_ADDRSTRLEN];
	
	//got a new connection so accept and send data
	int connectionSocket = accept(*listenerSocket, (sockaddr*)&theirAddr, &sockAddrStorageSize);
	if(connectionSocket == -1)
	{
		perror("accept");
		return connectionSocket;
	}
	
	if(theirAddr.ss_family == AF_INET)
	{
		sockaddr_in* ipv4Addr = (struct sockaddr_in*)&theirAddr;
		addr = &(ipv4Addr->sin_addr);
	}
	else if(theirAddr.ss_family == AF_INET6)
	{
		sockaddr_in6* ipv6Addr = (struct sockaddr_in6*)&theirAddr;
		addr = &(ipv6Addr->sin6_addr);
	}
	
	inet_ntop(theirAddr.ss_family, addr,  ipAddrStr, INET6_ADDRSTRLEN);
	printf("server: got connection from %s\n", ipAddrStr);
	
	return connectionSocket;
}

int main(int argc, char **argv)
{
	//set up the type of addresses we want returned.
	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; //want to use in the bind function
	
	//get server info
	addrinfo* serverInfo; 
	int gaiError = getaddrinfo(nullptr, MYPORT, &hints, &serverInfo);
	if(gaiError != 0)
	{
		printf("getaddrinfo error: %s\n", gai_strerror(gaiError));
		exit(1);
	}
	
	//loop through the results and bind to the first we can
	addrinfo* pAddrInfos = serverInfo;
	int listenerSocket;
	int yes = 1;
	while(pAddrInfos != nullptr)
	{
		listenerSocket = socket(pAddrInfos->ai_family, pAddrInfos->ai_socktype, pAddrInfos->ai_protocol);
		if(listenerSocket == -1)
		{
			perror("server: socket");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		//allow port number reuse
		int sockOptError = setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(sockOptError == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		
		int bindError = bind(listenerSocket, pAddrInfos->ai_addr, pAddrInfos->ai_addrlen);
		if(bindError == -1)
		{
			close(listenerSocket);
			perror("server: bind");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		break;
	}
	
	//the struct is no longer needed
	freeaddrinfo(serverInfo);
	
	if(pAddrInfos == nullptr)
	{
		printf("server: failed to bind");
		exit(1);
	}
	
	//listen for incoming connections
	int listenError = listen(listenerSocket, BACKLOG);
	if(listenError == -1)
	{
		perror("listen");
		exit(1);
	}
	
	//set the listener socket to non-blocking, so the
	//the accept function doesn't block
	int fnctlError = fcntl(listenerSocket, F_SETFL, O_NONBLOCK);
	if(fnctlError == -1)
	{
		perror("fcntl");
		exit(1);
	}
	
	/*struct sigaction sa;
	sa.sa_handler = SigChildHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART; //restart function if interrupted.
	int sigActionError = sigaction(SIGCHLD, &sa, nullptr);
	if(sigActionError == -1)
	{
		perror("sigaction");
		exit(1);
	}*/
	
	int epoll_fd = epoll_create1(0); //create epoll instance
	if(epoll_fd == -1)
	{
		perror("epoll_create1");
		exit(1);
	}
	
	epoll_event listenerPoll;
	listenerPoll.data.fd = listenerSocket;
	listenerPoll.events = EPOLLIN; //an event is raised when their is a new connection to accept
	
	//add the listener socket to the epoll instance
	int epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenerSocket, &listenerPoll);
	if(epollCtlError == -1)
	{
		perror("epoll_ctl");
		exit(1);
	}
	
	printf("Waiting for connections...\n");
	
	epoll_event events[MAX_EVENTS];
	bool running = true;
	while(running)
	{
		//block until an event happens
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if(event_count == -1)
		{
			perror("event_count");
			exit(1);
		}
		
		printf("server: an event happened. Checking...\n");
		
		for(int i = 0; i < event_count; ++i)
		{
			if(events[i].data.fd == listenerSocket)
			{
				//accept new connection and send data
				int connectionSocket = NewConnection(&listenerSocket);
				if(connectionSocket == -1)
				{
					exit(1);
				}
				
				epoll_event connectedPoll;
				connectedPoll.data.fd = connectionSocket;
				
				//an event is raised when their is a new connection to accept or 
				//when there is data to read (recv)
				connectedPoll.events = EPOLLIN | EPOLLHUP;
				
				int epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connectionSocket, &connectedPoll);
				if(epollCtlError == -1)
				{
					perror("epoll_ctl");
					exit(1);
				}
				
				int fnctlError = fcntl(connectionSocket, F_SETFL, O_NONBLOCK);
				if(fnctlError == -1)
				{
					perror("fcntl");
					exit(1);
				}
				
				SendData(&listenerSocket, &connectionSocket, &epoll_fd);
			}
		}
		break;
	}
	
	close(epoll_fd);
	close(listenerSocket);
	
	return 0;
}
