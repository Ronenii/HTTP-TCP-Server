#include "HttpResponse.h"

HttpResponse * buildHttpResponse(const std::string httpVer, HttpStatus::eCode code, std::string contentType,
	std::string body)
{
	HttpResponse * ret = (HttpResponse*)malloc(sizeof(HttpResponse));

	ret->statusPhrase = getStatusPhrase(httpVer, code);
	ret->contentType = contentType;
	ret->contentLength = body.length();
	ret->body = body;

	return ret;
}


