#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>

enum Family
{
	FAMILY_IPV4,
	FAMILY_IPV6,
	FAMILY_UNSPEC
};

enum SocketType
{
	SOCKET_TYPE_STREAM,
	SOCKET_TYPE_DATAGRAM
};

enum Protocol
{
	PROTOCOL_TCP,
	PROTOCOL_UDP
};

enum AddressInfoFlags
{
	ADDRESS_INFO_FLAG_NONE = 0x0,
	ADDRESS_INFO_FLAG_PASSIVE = 0x1, //returned address are suitable for binding a socket to accept connections
	ADDRESS_INFO_FLAG_NUMERIC_HOST = 0x2, //name has to be a numeric address
	ADDRESS_INFO_FLAG_V4MAPPED = 0x4 //if Family is set to FAMILY_IPV6, then IPV4 address will be returned as IPV4-mapped IPV6 addresses
};

#define ADDRESS_STRLEN INET6_ADDRSTRLEN

struct AddressInfo
{
	Family family;
	SocketType type;
	Protocol prot;
	int flags;
	const char* name; //can be a host name (ex. example.com) or an IPV4 or IPV6 address
	const char* port; //port number
};

struct Addresses
{
	addrinfo* pAddrInfo;
};

//Stores a list of address in pAddresses. pInfo specifies the type of socket address to store.
//Returns 0 if successful, -1 otherwise
int GetAddresses(const AddressInfo* pInfo, Addresses* pAddresses);

//Frees address
void FreeAddresses(Addresses* pAddresses);

//Prints the list of ip addresses
void PrintAddresses(const Addresses* pAddresses);

//Stores the first ip address of the list in buffer
void GetAddress(const Addresses* pAddress, char* buffer);

struct Socket
{
	int socketfd;
};

//Creates a socket from the first address we can create one from.
//Returns the address that the socket was created from in pOutAddress
//Returns 0 if successful, -1 otherwise
int CreateSocket(Socket* pSocket, const Addresses* pAddresses, Addresses* pOutAddress);

//Closes a socket
void CloseSocket(Socket* pSocket);

//Sets a socket to blocking
int SetToBlock(Socket* pSocket);

//Sets a socket to non -blocking
int SetToNonBlock(Socket* pSocket);

//Connects a socket to a server
//The address should be from the pOutAddress in CreateSocket()
int Connect(Socket* pSocket, const Addresses* pAddress);

//Associates a socket with an ip address and port number
//Returns 0 if successful, -1 otherwise
int Bind(Socket* pSocket, const Addresses* pAddress);

//Tells a socket to listen for incoming connections. 
//Size is the number pending connections that can be held before news ones get rejected
//Returns 0 if successful, -1 otherwise
int Listen(Socket* pSocket, uint32_t size);

//Accepts an incoming connection on a listening socket.
//Returns 0 if successful, -1 otherwise
int Accept(Socket* pListeningSocket, Socket* pConnectionSocket);

//Read incoming data
//Returns the number of bytes read
//If the remote side closed connection will return 0. Use this to determine if a connection was closed.
//Returns -1 if error
int Recieve(const Socket* pSocket, void* buffer, uint32_t numBytesToRead);

//Send data
//Returns the number of bytes sent.
//Returns -1 if error
int Send(const Socket* pSocket, void* buffer, uint32_t numBytesToSend);

enum Events
{
	//Raised when there is data to read from the socket.
	EVENT_READ = 0x1,
	
	//Rasied when there data can be sent without being blocked
	//if the socket is set to non-blocking, this will always be rasied if level triggered
	EVENT_WRITE = 0x2,
	
	//Sets the event to be edge triggered.
	//For edge triggered events will only be recieved if when the state of the file descriptor changes.
	//For level triggered events will continue to be raised until the file descriptor is no longer in a ready state.
	//Default is level triggered if this event is not specified.
	EVENT_EDGE_TRIGGERED = 0x4
};

//WRAPPER FOR EPOLL
struct SocketEventInfo
{
	int socketfd;
	uint32_t events;
};

struct SocketEvent
{
	int epollFd;
	uint32_t event;
};

//Returns 0 if successful, -1 otherwise
int CreateSocketEvent(const SocketEventInfo* pInfo, SocketEvent* pEvent);

//Returns 0 if successful, -1 otherwise
int ModifySocketEvent(const SocketEventInfo* pInfo, SocketEvent* pEvent);

void CloseSocketEvent(SocketEvent* pEvent);

//Blocks until an event is raised. The event that happens is store in pEvent->event
//Returns 0 if successful, -1 otherwise
int WaitForEvent(SocketEvent* pEvent);


#endif