#include <stdio.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MYPORT "3490"
#define BACKLOG 10

void sigChildHandler(int unused)
{
	int saved_errno = errno;
	
	//waits for any child process to finish, doesn't block once finished.
	while(waitpid(-1, nullptr, WNOHANG) > 0);
	
	errno = saved_errno;
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
	int socketfd;
	int yes = 1;
	while(pAddrInfos != nullptr)
	{
		socketfd = socket(pAddrInfos->ai_family, pAddrInfos->ai_socktype, pAddrInfos->ai_protocol);
		if(socketfd == -1)
		{
			perror("server: socket");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		//allow port number reuse
		int sockOptError = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(sockOptError == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		
		int bindError = bind(socketfd, pAddrInfos->ai_addr, pAddrInfos->ai_addrlen);
		if(bindError == -1)
		{
			close(socketfd);
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
	int listenError = listen(socketfd, BACKLOG);
	if(listenError == -1)
	{
		perror("listen");
		exit(1);
	}
	
	struct sigaction sa;
	sa.sa_handler = sigChildHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART; //restart function if interrupted.
	int sigActionError = sigaction(SIGCHLD, &sa, nullptr);
	if(sigActionError == -1)
	{
		perror("sigaction");
		exit(1);
	}
	
	printf("Waiting for connections...\n");
	
	socklen_t sockAddrStorageSize = sizeof(sockaddr_storage);
	int newfd;
	sockaddr_storage theirAddr;
	void* addr;
	char ipAddrStr[INET6_ADDRSTRLEN];
	while(true)
	{
		newfd = accept(socketfd, (sockaddr*)&theirAddr, &sockAddrStorageSize);
		if(newfd == -1)
		{
			perror("accept");
			continue;
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
		
		pid_t pid = fork();
		if(pid == 0)
		{
			//this is the child process
			
			close(socketfd); //child doesn't need the listener
			
			int sendError = send(newfd, "Hello, World!\n", 13, 0);
			if(sendError == -1)
			{
				perror("send");
			}
			close(newfd);
			exit(0);
			
		}
		else if(pid < 0)
		{
			//error
			perror("fork");
			close(socketfd);
			close(newfd);
			exit(1);
		}
		else
		{
			//this is the parent
			close(newfd); //parent doesn't need the connected socket
		}
	}
	
	close(socketfd);
	
	return 0;
}
