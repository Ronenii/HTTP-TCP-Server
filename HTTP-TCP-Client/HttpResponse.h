#pragma once
#ifndef HTTPREQUEST
#define HTTPREQUEST
#include <string>
#include "HttpStatus.h"

typedef struct httpResponse
{
	std::string statusPhrase;
	std::string contentType;
	int contentLength;
	std::string Date;
	std::string body;
}HttpResponse;

inline std::string getStatusPhrase(const std::string httpVer, HttpStatus::eCode code) { return httpVer + " " + std::to_string(static_cast<int>(code)) + " " + HttpStatus::reasonPhrase(code); }

HttpResponse * buildHttpResponse(const std::string httpVer, HttpStatus::eCode code, std::string contentType, std::string body);

inline std::string httpResponseToString(const HttpResponse httpResponse) {
	return httpResponse.statusPhrase + "\nDate: " + httpResponse.Date + "\nContent-Type: " + httpResponse.contentType
		+ "\nContent-Length: " + std::to_string(httpResponse.contentLength) + "\n\n" + httpResponse.body;
}

#endif 
