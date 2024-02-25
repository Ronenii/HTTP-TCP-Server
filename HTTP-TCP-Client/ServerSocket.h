#pragma once
#ifndef SERVERSOCKET
#define SERVERSOCKET

#include "HttpRequest.h"
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