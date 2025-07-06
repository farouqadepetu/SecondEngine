#include <stdio.h>
#include <cstdlib>

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <syscall.h>
#include <semaphore.h>
#include <string.h>

//replace?
#include <queue>

#define MYPORT "3490"
#define BACKLOG 10
#define MAX_DATA_SIZE 100 //max number of bytes we can recieve at once
#define MAX_EVENTS 5

pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueCond = PTHREAD_COND_INITIALIZER;
std::queue<pthread_t> tids;

struct HandleConnectionData
{
	int listenerSocket;
	int connectionSocket;
};

void ThreadExit()
{
	pthread_mutex_lock(&queueMutex);
	tids.push(pthread_self());
	pthread_cond_signal(&queueCond);
	pthread_mutex_unlock(&queueMutex);
	pthread_exit(nullptr);
}

void* HandleConnection(void* ptr)
{
	HandleConnectionData* data;
	data = (HandleConnectionData*)ptr;
	
	//sect the connection socket to non-blocking
	int fnctlError = fcntl(data->connectionSocket, F_SETFL, O_NONBLOCK);
	if(fnctlError == -1)
	{
		perror("fcntl");
		exit(1);
	}
	
	int epoll_fd = epoll_create1(0); //create epoll instance
	if(epoll_fd == -1)
	{
		perror("epoll_create1");
		pthread_exit(nullptr);
	}
	
	epoll_event connectedPoll;
	connectedPoll.data.fd = data->connectionSocket;
	connectedPoll.events = EPOLLOUT | EPOLLET;
	
	int epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, data->connectionSocket, &connectedPoll);
	if(epollCtlError == -1)
	{
		perror("epoll_ctl");
		pthread_exit(nullptr);
	}
	
	epoll_event events[MAX_EVENTS];

	while(true)
	{
		//block until an event happens
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if(event_count == -1)
		{
			perror("event_count");
			pthread_exit(nullptr);
		}
		
		printf("server:thread an event happened! Checking..\n");
		for(int i = 0; i < event_count; ++i)
		{
			printf("event = %d\n", events[i].events);
			
			if(events[i].data.fd == data->listenerSocket)
				continue;
				
			if(events[i].events & EPOLLOUT)
			{
				char msg[] = "Hello World!";
				uint32_t numU = 349;
				int numI = -5039292;
				float numF = 50.983f;
				double numD = 987.3212;
				
				int sendError = send(data->connectionSocket, msg, sizeof(msg), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				uint32_t dataU = htonl(numU);
				sendError = send(data->connectionSocket, &dataU, sizeof(uint32_t), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				int dataI = htonl(numI);
				sendError = send(data->connectionSocket, &dataI, sizeof(int), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				uint32_t tempF;
				memcpy(&tempF, &numF, sizeof(float));
				uint32_t dataF = htonl(tempF);
				sendError = send(data->connectionSocket, &dataF, sizeof(uint32_t), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				uint64_t tempD;
				memcpy(&tempD, &numD, sizeof(double));
				uint64_t dataD = htole64(tempD);
				sendError = send(data->connectionSocket, &dataD, sizeof(uint64_t), 0);
				if(sendError == -1)
				{
					perror("send");
				}
				
				printf("server:thread sent data\n");
				
				printf("server:thread waiting for ack\n");
				
				connectedPoll.data.fd = data->connectionSocket;
				connectedPoll.events = EPOLLIN;
				
				int epollCtlError = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->connectionSocket, &connectedPoll);
				if(epollCtlError == -1)
				{
					perror("epoll_ctl");
					pthread_exit(nullptr);
				}
			}
			else if(events[i].events & EPOLLIN)
			{
				char buf[MAX_DATA_SIZE];
				int numbytes = recv(data->connectionSocket, buf, 4, 0);
				if(numbytes == -1)
				{
					//error
					perror("recv");
					pthread_exit(nullptr);
				}
				else if(numbytes == 0)
				{
					//client closed connection
					printf("server:thread client closed connection, exiting\n");
					close(data->connectionSocket);
					close(epoll_fd);
					ThreadExit();
					pthread_exit(nullptr);
				}
				else
				{
					//data was recieved
					buf[numbytes] = '\0';
					printf("Recieved %s\n", buf);
				}
			}
		}
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

void* ThreadJoin(void* ptr)
{
	while(true)
	{
		pthread_mutex_lock(&queueMutex);
		while(tids.empty())
		{
			pthread_cond_wait(&queueCond, &queueMutex);
		}
		pthread_t tid = tids.front();
		tids.pop();
		printf("Joining thread\n");
		int joinError = pthread_join(tid, nullptr);
		if(joinError != 0)
		{
			perror("pthread_join");
		}
		printf("Thread joined\n");
		pthread_mutex_unlock(&queueMutex);
	}
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
	
	pthread_mutexattr_t attr;
	int result = pthread_mutexattr_init(&attr);
	result |= pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	result |= pthread_mutex_init(&queueMutex, &attr);
	result |= pthread_mutexattr_destroy(&attr);
	if(result != 0)
	{
		printf("Error initializing mutex. Exiting program!\n");
		exit(1);
	}
	
	result = pthread_cond_init(&queueCond, nullptr);
	if(result != 0)
	{
		printf("Error initializing condition variable. Exiting program!\n");
		exit(1);
	}
	
	
	pthread_t joinThread;
	int pthread_error = pthread_create(&joinThread, nullptr, ThreadJoin, nullptr);
	if(pthread_error != 0)
	{
		perror("pthread_create");
		exit(1);
	}
	printf("server: join thread created\n");
	
	printf("Waiting for connections...\n");
	
	epoll_event events[MAX_EVENTS];
	while(true)
	{
		//block until an event happens
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if(event_count == -1)
		{
			perror("event_wait");
			if(errno == EINTR)
				continue;
				
			exit(1);
		}
		
		printf("server: an event happened. Checking...\n");
		
		for(int i = 0; i < event_count; ++i)
		{
			printf("server: event = %d\n", events[i].events);
			if(events[i].data.fd == listenerSocket)
			{
				//accept new connection and send data
				int connectionSocket = NewConnection(&listenerSocket);
				if(connectionSocket == -1)
				{
					exit(1);
				}
				
				pthread_t tid;
				HandleConnectionData data;
				data.listenerSocket = listenerSocket;
				data.connectionSocket = connectionSocket;
				pthread_error = pthread_create(&tid, nullptr, HandleConnection, (void*)&data);
				if(pthread_error != 0)
				{
					perror("pthread_create");
					exit(1);
				}
				printf("server: connection thread created\n");
			}
		}
	}
	
	pthread_cond_destroy(&queueCond);
	pthread_mutex_destroy(&queueMutex);
	close(epoll_fd);
	close(listenerSocket);
	
	return 0;
}
