#include "Network.h"
#include <stdio.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

int GetAddresses(const AddressInfo* pInfo, Addresses* pAddresses)
{
	//set up the type of addresses we want returned.
	addrinfo hints;
	
	if(pInfo->family == FAMILY_IPV4)
		hints.ai_family = AF_INET;
	else if(pInfo->family == FAMILY_IPV6)
		hints.ai_family = AF_INET6;
	else //FAMILY_UNSPEC
		hints.ai_family = AF_UNSPEC;
			
	if(pInfo->type == SOCKET_TYPE_STREAM)
		hints.ai_socktype = SOCK_STREAM;
	else //SOCKET_TYPE_DATAGRAM
		hints.ai_socktype = SOCK_DGRAM;
				
	if(pInfo->prot == PROTOCOL_TCP)
		hints.ai_protocol = IPPROTO_TCP;
	else //PROTOCOL_UDP
		hints.ai_protocol = IPPROTO_UDP;
	
	hints.ai_flags = 0;
	if(pInfo->flags & ADDRESS_INFO_FLAG_PASSIVE)
		hints.ai_flags |= AI_PASSIVE;
	if(pInfo->flags & ADDRESS_INFO_FLAG_NUMERIC_HOST)
		hints.ai_flags |= AI_NUMERICHOST;
	if(pInfo->flags & ADDRESS_INFO_FLAG_V4MAPPED)
		hints.ai_flags |= AI_V4MAPPED;
	
	int gaiError = getaddrinfo(pInfo->name, pInfo->port, &hints, &pAddresses->pAddrInfo);
	if(gaiError != 0)
	{
		return -1;
	}
	return 0;
}

void FreeAddresses(Addresses* pAddresses)
{
	freeaddrinfo(pAddresses->pAddrInfo);
}

void GetAddress(const Addresses* pAddresses, char* buffer)
{
	void* addr;
	sockaddr_in* ipv4Addr = nullptr;
	sockaddr_in6* ipv6Addr = nullptr;
	if(pAddresses->pAddrInfo->ai_family == AF_INET)
	{
		ipv4Addr = (struct sockaddr_in*)pAddresses->pAddrInfo->ai_addr;
		addr = &(ipv4Addr->sin_addr);
	}
	else if(pAddresses->pAddrInfo->ai_family == AF_INET6)
	{
		ipv6Addr = (struct sockaddr_in6*)pAddresses->pAddrInfo->ai_addr;
		addr = &(ipv6Addr->sin6_addr);
	}
	
	inet_ntop(pAddresses->pAddrInfo->ai_family, addr, buffer, INET6_ADDRSTRLEN);
}

void PrintAddresses(const Addresses* pAddresses)
{
	addrinfo* pAddrInfos = pAddresses->pAddrInfo;
	void* addr;
	char ipAddrStr[INET6_ADDRSTRLEN];
	uint32_t i = 0;
	sockaddr_in* ipv4Addr = nullptr;
	sockaddr_in6* ipv6Addr = nullptr;
	while(pAddrInfos != nullptr)
	{
		if(pAddrInfos->ai_family == AF_INET)
		{
			ipv4Addr = (struct sockaddr_in*)pAddrInfos->ai_addr;
			addr = &(ipv4Addr->sin_addr);
		}
		else if(pAddrInfos->ai_family == AF_INET6)
		{
			ipv6Addr = (struct sockaddr_in6*)pAddrInfos->ai_addr;
			addr = &(ipv6Addr->sin6_addr);
		}
		
		inet_ntop(pAddrInfos->ai_family, addr, ipAddrStr, INET6_ADDRSTRLEN);
		printf("IP %d = %s\n", i, ipAddrStr);
		++i;
		pAddrInfos = pAddrInfos->ai_next;
	}
}

int CreateSocket(Socket* pSocket, const Addresses* pAddresses, Addresses* pOutAddress)
{
	addrinfo* pAddrInfos = pAddresses->pAddrInfo;
	int socketfd;
	while(pAddrInfos != nullptr)
	{
		socketfd = socket(pAddrInfos->ai_family, pAddrInfos->ai_socktype, pAddrInfos->ai_protocol);
		if(socketfd == -1)
		{
			perror("socket error: ");
			pAddrInfos = pAddrInfos->ai_next;
			continue;
		}
		
		break;
	}
	
	if(pAddrInfos == nullptr)
	{
		return -1;
	}
	
	pSocket->socketfd = socketfd;
	pOutAddress->pAddrInfo = pAddrInfos;
	return 0;
}

void CloseSocket(Socket* pSocket)
{
	close(pSocket->socketfd);
}

int SetToBlock(Socket* pSocket)
{
	int flags = fcntl(pSocket->socketfd, F_GETFL, 0);
	if(flags == -1)
	{
		return -1;
	}
	flags &= ~O_NONBLOCK;
	int fnctlError = fcntl(pSocket->socketfd, F_SETFL, flags);
	if(fnctlError == -1)
	{
		return -1;
	}
	
	return 0;
}

int SetToNonBlock(Socket* pSocket)
{
	int flags = fcntl(pSocket->socketfd, F_GETFL, 0);
	if(flags == -1)
	{
		return -1;
	}
	int fnctlError = fcntl(pSocket->socketfd, F_SETFL, flags | O_NONBLOCK);
	if(fnctlError == -1)
	{
		return -1;
	}
	return 0;
}

int Connect(Socket* pSocket, const Addresses* pAddress)
{
	char ipAddrStr[ADDRESS_STRLEN];
	GetAddress(pAddress, ipAddrStr);
	
	printf("attempting connection to %s\n", ipAddrStr);
	
	int connectError = connect(pSocket->socketfd, pAddress->pAddrInfo->ai_addr, pAddress->pAddrInfo->ai_addrlen);
	if(connectError == -1)
	{
		return -1;
	}
	
	printf("connection to %s successful\n", ipAddrStr);
	return 0;
}

int Bind(Socket* pSocket, const Addresses* pAddress)
{
	//allow port number reuse
	int yes = 1;
	int sockOptError = setsockopt(pSocket->socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(sockOptError == -1)
	{
		return sockOptError;
	}
	
	int bindError = bind(pSocket->socketfd, pAddress->pAddrInfo->ai_addr, pAddress->pAddrInfo->ai_addrlen);
	
	return bindError;
}

int Listen(Socket* pSocket, uint32_t size)
{
	int listenError = listen(pSocket->socketfd, size);
	
	return listenError;
}

int Accept(Socket* pListeningSocket, Socket* pConnectionSocket)
{
	socklen_t sockAddrStorageSize = sizeof(sockaddr_storage);
	sockaddr_storage theirAddr;
	void* addr;
	char ipAddrStr[ADDRESS_STRLEN];
	
	int connectionSocket = accept(pListeningSocket->socketfd, (sockaddr*)&theirAddr, &sockAddrStorageSize);
	if(connectionSocket == -1)
	{
		return -1;
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
	
	inet_ntop(theirAddr.ss_family, addr,  ipAddrStr, ADDRESS_STRLEN);
	printf("Accepted connection from %s\n", ipAddrStr);
	
	pConnectionSocket->socketfd = connectionSocket;
	return 0;
}

int Recieve(const Socket* pSocket, void* buffer, uint32_t numBytesToRead)
{
	int numBytesRead = recv(pSocket->socketfd, buffer, numBytesToRead, 0);
	
	return numBytesRead;
}

int Send(const Socket* pSocket, void* buffer, uint32_t numBytesToSend)
{
	int numBytesSent = send(pSocket->socketfd, buffer, numBytesToSend, 0);
	
	return numBytesSent;
}

int CreateSocketEvent(const SocketEventInfo* pInfo, SocketEvent* pEvent)
{
	//create epoll instance
	pEvent->epollFd = epoll_create1(0);
	if(pEvent->epollFd  == -1)
	{
		return -1;
	}
	
	epoll_event epollEvent;
	epollEvent.data.fd = pInfo->socketfd;
	
	if(pInfo->events & EVENT_READ)
		epollEvent.events |= EPOLLIN; // event is raised when their data to recv
	if(pInfo->events & EVENT_WRITE)
		epollEvent.events |= EPOLLOUT; //event is raised when you can write w.o being blocked
	if(pInfo->events & EVENT_EDGE_TRIGGERED)
		epollEvent.events |= EPOLLET;
		
	int epollCtlError = epoll_ctl(pEvent->epollFd, EPOLL_CTL_ADD, pInfo->socketfd, &epollEvent);
	if(epollCtlError == -1)
	{
		return -1;
	}
	
	return 0;
}

int ModifySocketEvent(const SocketEventInfo* pInfo, SocketEvent* pEvent)
{
	epoll_event epollEvent{};
	epollEvent.data.fd = pInfo->socketfd;
	
	if(pInfo->events & EVENT_READ)
		epollEvent.events |= EPOLLIN; // event is raised when their data to recv
	if(pInfo->events & EVENT_WRITE)
		epollEvent.events |= EPOLLOUT; //event is raised when you can write w.o being blocked
	if(pInfo->events & EVENT_EDGE_TRIGGERED)
		epollEvent.events |= EPOLLET;
		
	
	int epollCtlError = epoll_ctl(pEvent->epollFd, EPOLL_CTL_MOD, pInfo->socketfd, &epollEvent);
	if(epollCtlError == -1)
	{
		return -1;
	}
	return 0;
}

void CloseSocketEvent(SocketEvent* pEvent)
{
	close(pEvent->epollFd);
}

int WaitForEvent(SocketEvent* pEvent)
{
	epoll_event rasiedEvent;
	//block until an event happens
	int errorCheck = epoll_wait(pEvent->epollFd, &rasiedEvent, 1, -1);
	if(errorCheck == -1)
	{
		return -1;
	}
	
	if(rasiedEvent.events & EPOLLIN)
		pEvent->event = EVENT_READ;
	if(rasiedEvent.events & EPOLLOUT)
		pEvent->event = EVENT_WRITE;
		
	return 0;
}
