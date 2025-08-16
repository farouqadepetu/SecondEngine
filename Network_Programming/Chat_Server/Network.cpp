#include "Network.h"
#include <stdio.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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
	
	int gaiError = getaddrinfo(pInfo->name, pInfo->port, &hints, &pAddresses->pAddrInfos);
	if(gaiError != 0)
	{
		return -1;
	}
	
	addrinfo* pAddrInfos = pAddresses->pAddrInfos;
	uint32_t i = 0;
	while(pAddrInfos != nullptr)
	{
		++i;
		pAddrInfos = pAddrInfos->ai_next;
	}
	pAddresses->numAddresses = i;
	
	return 0;
}

void FreeAddresses(Addresses* pAddresses)
{
	freeaddrinfo(pAddresses->pAddrInfos);
}

Family GetFamily(const Address* pAddress)
{
	if(pAddress->addressInfo.ai_family == AF_INET)
		return FAMILY_IPV4;
	else if(pAddress->addressInfo.ai_family == AF_INET6)
		return FAMILY_IPV6;
		
	return FAMILY_NONE;
}

SocketType GetSocketType(const Address* pAddress)
{
	if(pAddress->addressInfo.ai_socktype == SOCK_STREAM)
		return SOCKET_TYPE_STREAM;
	else if(pAddress->addressInfo.ai_socktype == SOCKET_TYPE_DATAGRAM)
		return SOCKET_TYPE_DATAGRAM;
		
	return SOCKET_TYPE_NONE;
}

Protocol GetProtocol(const Address* pAddress)
{
	if(pAddress->addressInfo.ai_protocol == IPPROTO_TCP)
		return PROTOCOL_TCP;
	else if(pAddress->addressInfo.ai_protocol == IPPROTO_TCP)
		return PROTOCOL_UDP;
		
	return PROTOCOL_NONE;
}

//Store the address at index in pAddress.
//If the index is out-of-bounds, pAddress = nullptr;
void GetAddress(const Addresses* pAddresses, Address* pAddress, uint32_t index)
{
	if (index > pAddresses->numAddresses - 1)
	{
		pAddress = nullptr;
		return;
	}
	
	addrinfo* pAddrInfos = pAddresses->pAddrInfos;
	uint32_t i = 0;
	while(i < index)
	{
		++i;
		pAddrInfos = pAddrInfos->ai_next;
	}
		
	pAddress->addressInfo = *pAddrInfos;
}

void GetAddress(const Address* pAddress, char* buffer)
{
	void* addr;
	sockaddr_in* ipv4Addr = nullptr;
	sockaddr_in6* ipv6Addr = nullptr;
	if(pAddress->addressInfo.ai_family == AF_INET)
	{
		ipv4Addr = (struct sockaddr_in*)pAddress->addressInfo.ai_addr;
		addr = &(ipv4Addr->sin_addr);
	}
	else if(pAddress->addressInfo.ai_family == AF_INET6)
	{
		ipv6Addr = (struct sockaddr_in6*)pAddress->addressInfo.ai_addr;
		addr = &(ipv6Addr->sin6_addr);
	}
	
	inet_ntop(pAddress->addressInfo.ai_family, addr, buffer, ADDRESS_STRLEN);
}

//Stores the name in buffer
void GetName(const Address* pAddress, char* buffer)
{
	buffer = pAddress->addressInfo.ai_canonname;
}

int CreateSocket(Socket* pSocket, const Address* pAddress)
{
	int socketfd = socket(pAddress->addressInfo.ai_family, pAddress->addressInfo.ai_socktype, 
	pAddress->addressInfo.ai_protocol);
	if(socketfd == -1)
	{
		return - 1;
	}
	
	pSocket->socketfd = socketfd;
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

int Connect(Socket* pSocket, const Address* pAddress)
{
	char ipAddrStr[ADDRESS_STRLEN];
	GetAddress(pAddress, ipAddrStr);
	
	printf("attempting connection to %s\n", ipAddrStr);
	
	int connectError = connect(pSocket->socketfd, pAddress->addressInfo.ai_addr, 
	pAddress->addressInfo.ai_addrlen);
	if(connectError == -1)
	{
		return -1;
	}
	
	printf("connection to %s successful\n", ipAddrStr);
	return 0;
}

int Bind(Socket* pSocket, const Address* pAddress)
{
	//allow port number reuse
	int yes = 1;
	int sockOptError = setsockopt(pSocket->socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if(sockOptError == -1)
	{
		return sockOptError;
	}
	
	int bindError = bind(pSocket->socketfd, pAddress->addressInfo.ai_addr, pAddress->addressInfo.ai_addrlen);
	
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

int RecieveFrom(const Socket* pSocket, void* buffer, uint32_t numBytesToRead, Address* pAddress)
{
	socklen_t addr_len = sizeof(sockaddr);
	sockaddr_storage their_address;
	int numBytesRead = recvfrom(pSocket->socketfd, buffer, numBytesToRead, 0, (sockaddr*)&their_address, 
	&addr_len);

	pAddress->addressInfo.ai_family = their_address.ss_family;
	memcpy(pAddress->addressInfo.ai_addr, &their_address, sizeof(sockaddr_storage));
	pAddress->addressInfo.ai_addrlen = addr_len;
	
	return numBytesRead;
}

int Send(const Socket* pSocket, void* buffer, uint32_t numBytesToSend)
{
	int numBytesSent = send(pSocket->socketfd, buffer, numBytesToSend, 0);
	
	return numBytesSent;
}

int SendTo(const Socket* pSocket, void* buffer, uint32_t numBytesToSend, const Address* pAddress)
{
	int numBytesSent = sendto(pSocket->socketfd, buffer, numBytesToSend, 0, pAddress->addressInfo.ai_addr, 
	pAddress->addressInfo.ai_addrlen);
	
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
	epollEvent.events = 0;
	
	if(pInfo->events & EVENT_RECIEVE)
		epollEvent.events |= EPOLLIN; // event is raised when their data to recv
	if(pInfo->events & EVENT_SEND)
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
	
	if(pInfo->events & EVENT_RECIEVE)
		epollEvent.events |= EPOLLIN; // event is raised when their data to recv
	if(pInfo->events & EVENT_SEND)
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
	
	pEvent->event = EVENT_NONE;
	
	if(rasiedEvent.events & EPOLLIN)
		pEvent->event |= EVENT_RECIEVE;
	if(rasiedEvent.events & EPOLLOUT)
		pEvent->event |= EVENT_SEND;
		
	return 0;
}
