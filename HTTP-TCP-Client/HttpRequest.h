#pragma once
#ifndef HTTPREQUEST
#define HTTPREQUEST

#include <exception>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <format>
#include  "ServerSocket.h"
#include "HttpStatus.h"

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
	void doGet();
	void doHead(ServerSocket::SocketState &socket);
	void doPut();
	void doPost();
	void doDelete();
	void doTrace();
	void doOptions();
	std::string httpMessageStart(HttpStatus::eCode code);
	std::string getLastModifiedTime(const string& filePath);
}

#endif 