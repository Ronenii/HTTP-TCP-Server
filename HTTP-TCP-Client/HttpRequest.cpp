#include "HttpRequest.h"




void HttpRequest::doGet()
{
	throw std::exception();
}

void HttpRequest::doHead(ServerSocket::SocketState& socket)
{
	int fileSize = 0;
	char sendBuff[bufferSize];
	std::string message, fileSizeString;
	const std::string filePath = "C:\\temp\\indexen.html";
	std::ifstream File;
	socket.prevActivity = time(0);
	
	File.open(filePath);
	if (!File) {
		message = httpMessageStart(HttpStatus::eCode::not_found);
		fileSize = 0;
	}
	else {
		message = httpMessageStart(HttpStatus::eCode::ok);
		File.seekg(0, std::ios::end);
		fileSize = File.tellg();
	}

	std::string lastModifiedTime = getLastModifiedTime(filePath);

	message += "\r\nContent-type: text/html";
	message += "\r\nDate:";
	message += lastModifiedTime;
	message += "\r\nContent-length: ";
	fileSizeString = std::to_string(fileSize);
	message += fileSizeString;
	message += "\r\n\r\n";
	strcpy(sendBuff, message.c_str());
	File.close();
}

std::string HttpRequest::httpMessageStart(HttpStatus::eCode code)
{
	return httpVer + " " + std::to_string(static_cast<int>(code)) + " " + HttpStatus::reasonPhrase(code);
}

std::string getLastModifiedTime(const std::string& filePath) {
	struct stat result;
	std::string lastModifiedTime;
	if(stat(filePath.c_str(), &result) != 0)
	{
		const time_t timeModified = result.st_mtime;
		const tm* gmt = gmtime(&timeModified);
		std::stringstream buffer;
		buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
		lastModifiedTime = buffer.str();

	}
	return lastModifiedTime;
}

void HttpRequest::doPut()
{
	throw std::exception();
}

void HttpRequest::doDelete()
{
	throw std::exception();
}

void HttpRequest::doTrace()
{
	throw std::exception();
}

void HttpRequest::doOptions()
{
	throw std::exception();
}
