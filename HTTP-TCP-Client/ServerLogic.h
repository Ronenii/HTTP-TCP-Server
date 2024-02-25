#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include "HttpRequest.h"
#include "HttpStatus.h"
#include "ServerSocket.h"

#define OK 200
#define CREATED 201
#define NO_CONTENT 204
#define NOT_OK 400
#define NOT_FOUND 404
#define FAILED 412

constexpr int PORT = 8080;
constexpr int MAX_SOCKETS = 60;
constexpr int BUFFSIZE = 1024;

using namespace std;

struct SocketState {
	SOCKET					id;
	enum eSocketStatus		recv;
	enum eSocketStatus		send;
	enum eRequestType		httpReq;
	char					buffer[BUFFSIZE];
	time_t					prevActivity;
	int						socketDataLen;
};

/*-----------------------------------------ENUM--=-----------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
enum eSocketStatus { EMPTY, LISTEN, RECEIVE, IDLE, SEND };
/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/





/*------------------------------------FUNCTIONS-------------------------------------------------*/\
/*----------------------------------------------------------------------------------------------*/
bool addSocket(SOCKET id, enum eSocketStatus what, SocketState* sockets, int& socketsCount);
void removeSocket(ServerSocket::SocketState &socket, int& socketsCount, int index);
void acceptConnection(int index, SocketState* sockets, int& socketsCount);
void rcvMessage(int index, SocketState* sockets, int& socketsCount);
bool sendMessage(int index, SocketState* sockets);
int put(char* filename, ServerSocket::SocketState &socket);
string get_field_value(const string& request, const string& field);
string GetQuery(const string& request, const string& param);
template <typename TP>
time_t parse_to_time_t(TP tp);
void setSocketStateDet(ServerSocket::SocketState &socketState, HttpRequest::eRequestType type, int bufferIndex);
std::string getCurrentTime();
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
