#include "HttpResponse.h"

HttpResponse::Response * buildHttpResponse(const std::string httpVer, HttpStatus::eCode code, std::string contentType,
                                           std::string body)
{
	HttpResponse::Response * ret = (HttpResponse::Response*)malloc(sizeof(HttpResponse::Response));

	ret->statusPhrase = HttpResponse::getStatusPhrase(httpVer, code);
	ret->contentType = contentType;
	ret->contentLength = body.length();
    ret->Date = getCurrentTime();
	ret->body = body;

	return ret;
}


std::string HttpResponse::extractHeaders(const std::string& httpResponseString)
{
    const std::string doubleCRLF = "\r\n\r\n"; // Marks the end of headers and start of the body
    std::string headers;

    // Find the position of double CRLF to separate headers and body
    size_t bodyStart = httpResponseString.find(doubleCRLF);

    if (bodyStart != std::string::npos) {
        // Extract headers substring
        headers = httpResponseString.substr(0, bodyStart + doubleCRLF.length());
    }
    else {
        // If no body is present, consider the whole string as headers
        headers = httpResponseString;
    }

    return headers;
}
