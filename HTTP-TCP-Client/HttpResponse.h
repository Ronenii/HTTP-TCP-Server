#pragma once
#ifndef HTTPRESPONSE
#define HTTPRESPONSE

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include "HttpStatus.h"
#include "ServerLogic.h"

namespace HttpResponse
{
	typedef struct httpResponse
	{
		std::string statusPhrase;
		std::string contentType;
		int contentLength;
		std::string Date;
		std::string body;
	}Response;

	Response* buildHttpResponse(const std::string httpVer, HttpStatus::eCode code, std::string contentType, std::string body);
	std::string extractHeaders(const std::string& httpResponseString);

	inline std::string getStatusPhrase(const std::string httpVer, HttpStatus::eCode code) { return httpVer + " " + std::to_string(static_cast<int>(code)) + " " + HttpStatus::reasonPhrase(code); }


	inline std::string httpResponseToString(const Response httpResponse) {
		return httpResponse.statusPhrase + "\r\nDate: " + httpResponse.Date + "\r\nContent-Type: " + httpResponse.contentType
			+ "\r\nContent-Length: " + std::to_string(httpResponse.contentLength) + "\r\n\r\n" + httpResponse.body;
	}

	inline std::string httpResponseToStringNoContent(const Response httpResponse) {
		return httpResponse.statusPhrase + "\r\nDate: " + httpResponse.Date + "\r\n\r\n";
	}
}


#endif 
