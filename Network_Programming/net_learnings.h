#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>

void learnings()
{
	printf("NETWORK PROGRAMMING WOO!!!\n");
	
	//used to prep the socket address structures for subsequent use
	//also used for host name lookups and serivce name lookups
	//contains a linked list
	//can use ipv4, ipv6 or either, so can be ip version-agnostic
	//passed to the function getaddrinfo
	addrinfo addressInfo;
	
	//Holds the socket address info of many types of sockets
	//can use IPv4 or IPv6
	sockaddr socketAddr;
	
	//A parallel struct to sockaddr used for IPv4.
	//Can be cast to a sockaddr.
	sockaddr_in socketAddress_in;
	
	//A parallel struct to sockaddr used for IPv6.
	//Can be cast to a sockaddr.
	sockaddr_in6 socketAddress_in6;
	
	//Internet address for IPv4
	in_addr inAddr;
	
	//Internet address for IPv6
	in6_addr in6Addr;
	
	//This struct is large enough to hold both IPv4 and IPv6 structures.
	//Used when you don't know if u need an IPv4 or IPv6 address.
	//Check which family it belongs to (IPv4 or IPv6) then cast to the type u need
	sockaddr_storage sockAddrStorage;
	
	//converts a string(ip address string) to its number (byte) representation
	//pton means presentation to network
	//returns -1 if error, 0 if the address is messed up, so check that its > 0 before continuing.
	//inet_pton()
	
	//converts the number (byte) presentation of the ip address to a string
	//ntop means network to presentation
	//inet_ntop()
	
	//Identifies an interet host and a service
	//A service is http/https or a port number
	//sets up structs to use in other functions
	//Returns 0 if successful, non-zero if error.
	//Returns a pointer to a linked list of addrinfos
	//Call gai_strerror to print error code
	//call freeaddrinfo to free the linked list
	//getaddrinfo()
	
	//creates an endpoint for communication and
	//returns a file descriptor that refers to that endpoint
	//returns a vaild file descriptor (>= 0) if success, -1 if error
	//feed info from getaddrinfo() to this function
	//socket()
	
	//attaches (binds) a socket (file descriptor) to a port number.
	//Can give your process (app) any port number, which is a 16-bit number above 1024
	//1024 below is reserved on the OS unless you're a superuser
	//returns -1 on error
	//Use setsocketopt to use a port thats already being used.
	//Only need to use bind if you expect to recieve packets, so the OS knows which process (app)
	//to send the packet to.
	//bind()
	
	//connects you to an ip on a specific port.
	//connects you to an process (app).
	//returns -1 on error.
	//connect()
	
	//marks the socket as a passive socket
	//a passive socket is a socket that will be used to accept incoming connections
	//incoming connections are put into a queue
	//returns -1 on error
	//listen()
	
	//Gets the top connection request in the listening socket queue.
	//Returns a new socket (file descriptor) (connected socket).
	//The returned socket is the one used to communicate
	//accept()
	
	//Sends data to a socket
	//Returns the number of bytes actually sent
	//If the number bytes sent doesn't match the number of bytes of the message then it's
	//up to you to send the remaning bytes
	//this is like writing to a file
	//send()
	
	//Recieves data from a socket
	//Retruns the number of bytes actually read from the buffer or -1 if error.
	//Can also return 0. This means the connection was closed.
	//this is like reading from file
	//recv()
	
	//Same as send but you need to pass in the socket address to send the data to.
	//Only used if you don't connect, so for datagram sockets
	//Returns the number of bytes actually sent
	//If the number bytes sent doesn't match the number of bytes of the message then it's
	//up to you to send the remaning bytes
	//sendto()
	
	//Same as recv but you need to pass in the socket address to recieve the data from.
	//Only used if you don't connect, so for datagram sockets
	//recvfrom()

	//used to close a connection
	//can't do anymore read or writes on the socket
	//frees a socket descriptor
	//close()
	
	//allows you to choose how to cutoff communcation
	//doesn't free a socket descriptor need to call close().
	//returns 0 on success, -1 on error
	//shutdown()
	
	//tells you who is at the other end of a connected stream socket.
	//returns -1 on error
	//getpeername()
	
	//returns the name of the computer that your program is running on.
	//can be used by getaddrinfo() to determine the IP address of your local machine.
	//gethostname()
	
	/*int status = 0;
	addrinfo hints;
	addrinfo* serverInfo;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if((status = getaddrinfo(nullptr, "3490", &hints, &serverInfo)) != 0)
	{
		printf("gai error %s\n", gai_strerror(status));
		exit(1);
	}
	
	addrinfo* pInfo = serverInfo;
	sockaddr_in* ipv4;
	sockaddr_in6* ipv6;
	void* addr;
	char ipStr[INET6_ADDRSTRLEN];
	while(pInfo != nullptr)
	{
		if(pInfo->ai_family == AF_INET)
		{
			ipv4 = (sockaddr_in*)pInfo->ai_addr;
			addr = &(ipv4->sin_addr);
		}
		else if(pInfo->ai_family == AF_INET6)
		{
			ipv6 = (sockaddr_in6*)pInfo->ai_addr;
			addr = &(ipv6->sin6_addr);
		}
		
		//pInfo->ai_family
		inet_ntop(pInfo->ai_family, addr, ipStr, INET6_ADDRSTRLEN);
		if(ipStr[0] == 0)
		{
			perror("inet_ntop");
			exit(1);
		}
		printf("%s\n", ipStr);
		pInfo = pInfo->ai_next;
	}
	
	freeaddrinfo(serverInfo);*/
}