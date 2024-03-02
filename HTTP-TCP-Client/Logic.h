#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <time.h>
#include <string>
#include <iostream>

using namespace std;

constexpr int PORT = 8080;
constexpr int MAX_SOCKETS = 60;
constexpr int BUFFSIZE = 1024;



enum class SocketStatus { EMPTY, LISTEN, RECEIVE, IDLE, SEND };
enum class RequestType { GET = 1, HEAD, PUT, POST, DELETE1, TRACE, OPTIONS, NOT_ALLOWED_REQ };
enum class HttpStatus { OK = 200, CREATED = 201, NO_CONTENT = 204, NOT_OK = 400, NOT_FOUND = 404, FAILED = 412 };

struct SocketState {
	SOCKET					id;
	SocketStatus			recv;
	SocketStatus			send;
	RequestType				httpReq;
	char					buffer[BUFFSIZE];
	time_t					prevActivity;
	int						socketDataLen;
};


bool addSocket(SOCKET id, SocketStatus what, SocketState* sockets, int& socketsCount);
void removeSocket(int index, SocketState* sockets, int& socketsCount);
void acceptConnection(int index, SocketState* sockets, int& socketsCount);
void rcvMessage(int index, SocketState* sockets, int& socketsCount);
bool sendMessage(int index, SocketState* sockets);
string getFileContent(const string& fileAddress, int& fileSize);
string generateResponse(HttpStatus status, const string& content, int fileSize);
string buildFileAddress(char* buffer);
string handlePutRequest(int index, int& fileSize, SocketState* sockets);
string handleDeleteRequest(const char* buffer, int& fileSize);
string generateTraceResponse(const char* buffer, int& fileSize);
string generateOptionsResponse();
string handlePostRequest(const char* buffer);
string generateFormattedTime();
void clearSocketData(int index, SocketState* sockets);
int put(int index, char* filename, SocketState* sockets);
string get_field_value(const string& request, const string& field);
string GetQuery(const string& request, const string& param);
template <typename TP>
time_t parse_to_time_t(TP tp);
