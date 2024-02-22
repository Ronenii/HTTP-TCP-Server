#pragma once
#ifndef HTTPREQUEST
#define HTTPREQUEST
#include  "ServerSocket.h"
#include "HttpStatus.h"
#include <exception>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <format>


namespace HttpRequest
{
	constexpr int bufferSize = 1024;
	const std::string httpVer = "HTTP/1.1";

	enum class eRequestType
	{
		get,
		head,
		put,
		post,
		del,
		trace,
		options,
		not_allowed,
	};


	// Placeholders
	std::string doGet(ServerSocket::SocketState& socket, int &buffLen);
	std::string doHead(ServerSocket::SocketState &socket, int& buffLen);
	std::string doPut(ServerSocket::SocketState& socket, int& buffLen);
	std::string doPost(ServerSocket::SocketState& socket, int& buffLen);
	std::string doDelete(ServerSocket::SocketState& socket, int& buffLen);
	std::string doTrace(ServerSocket::SocketState& socket, int& buffLen);
	std::string doOptions(ServerSocket::SocketState& socket, int& buffLen);
	std::string doNotAllowed(ServerSocket::SocketState& socket, int& buffLen);
	std::string httpMessageStart(HttpStatus::eCode code, std::string message);
	std::string getLastModifiedTime(const std::string& filePath);
}

#endif 