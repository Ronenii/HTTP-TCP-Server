#pragma once
#ifndef SERVERSOCKET
#define SERVERSOCKET

#define _CRT_SECURE_NO_WARNINGS

#include "HttpRequest.h"  // Forward declaration instead of including the full header
#include <winsock2.h>
#include <ctime>

namespace ServerSocket
{
	constexpr int buffer = 1024;
	enum class eSocketStatus { EMPTY, LISTEN, RECEIVE, IDLE, SEND };

	struct SocketState {
		SOCKET							id;
		eSocketStatus					recv;
		eSocketStatus					send;
		HttpRequest::eRequestType		httpReq;
		char							buffer[buffer];
		time_t							prevActivity;
		int								dataLen;
	};
}
# endif 