#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

constexpr int PORT = 8080;
constexpr int MAX_SOCKETS = 60;
constexpr int BUFFSIZE = 1024;

struct SocketInfo {
	// Use a descriptive and consistent naming convention (camelCase)
	SOCKET socketId;
	enum eSocketStatus recvStatus;
	enum eSocketStatus sendStatus;
	enum eHttpRequest httpMethod;
	char buffer[BUFFSIZE];
	time_t lastActivity;
	int bufferLength; // Use lowercase with 'th' for consistency
};

/*-----------------------------------------ENUM--=-----------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
enum eSocketStatus { EMPTY, LISTEN, RECEIVE, IDLE, SEND };
enum eHttpRequest { GET = 1, HEAD, PUT, POST, DELETE1, TRACE, OPTIONS, NOT_ALLOWED };
enum eHttpStatus {
	OK = 200,
	NOT_FOUND = 404,
	NO_CONTENT = 204,
	CREATED = 201,
	FAILED = 412,
	NOT_OK = 400
};
/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/





/*------------------------------------FUNCTIONS-------------------------------------------------*/\
/*----------------------------------------------------------------------------------------------*/
bool addSocket(SOCKET socketId, eSocketStatus status, SocketInfo* sockets, int& socketCount);
void removeSocket(int index, SocketInfo* sockets, int& socketCount);
bool sendMessage(int index, SocketInfo* sockets);
string getCurrentTimeFormatted();
void acceptConnection(int listenerSocket, SocketInfo* sockets, int& socketCount);
bool receiveMessage(int index, SocketInfo* sockets, int& socketCount);
bool sendMessage(int index, SocketState* sockets);
int put(int index, char* filename, SocketInfo* sockets);
string get_field_value(const string& request, const string& field);
string GetQuery(const string& request, const string& param);
template <typename TP>
time_t parse_to_time_t(TP tp);
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
