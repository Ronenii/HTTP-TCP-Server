#include "HttpRequest.h"
#include "ServerLogic.h"


std::string HttpRequest::doGet(ServerSocket::SocketState& socket, int& buffLen)
{
	char* subBuff;
	char readBuff[bufferSize];
	HttpStatus::eCode code;
	std::string message, fileAddress;
	std::ifstream File;
	socket.prevActivity = time(0);
	fileAddress = "C:\\temp\\index";
	std::string fileContent = "";
	std::string langValue = GetQuery(subBuff, "lang");

	if (langValue.empty())
	{
		fileAddress += "en";
	}
	else
	{
		fileAddress += langValue;
	}
	fileAddress.append(".html");
	File.open(fileAddress);
	if (!File)
	{
		File.open("C:\\temp\\error.html");
		code = HttpStatus::eCode::not_found;
	}
	else
	{
		code = HttpStatus::eCode::ok;
	}

	if (File)
	{
		while (File.getline(readBuff, BUFFSIZE))
		{
			fileContent += readBuff;
		}
	}
	File.close();

	HttpResponse::Response* response = HttpResponse::buildHttpResponse(httpVer, code, "text/html", fileContent);
	message = HttpResponse::httpResponseToString(*response);
	free(response);
	return message;
}

std::string HttpRequest::doHead(ServerSocket::SocketState& socket, int& buffLen)
{
	int fileSize = 0;
	std::string message, fileContent = "";;
	char readBuff[bufferSize];
	const std::string filePath = "C:\\temp\\indexen.html";
	std::ifstream File;
	HttpStatus::eCode code;
	socket.prevActivity = time(0);

	File.open(filePath);
	if (!File) {
		code = HttpStatus::eCode::not_found;
		File.open("C:\\temp\\error.html");
	}
	else {
		code = HttpStatus::eCode::ok;
		File.seekg(0, std::ios::end);
	}

	if (File)
	{
		while (File.getline(readBuff, BUFFSIZE))
		{
			fileContent += readBuff;
		}
	}

	File.close();

	HttpResponse::Response* response = HttpResponse::buildHttpResponse(httpVer, code, "text/html", fileContent);
	message = HttpResponse::httpResponseToString(*response);
	free(response);
	message = HttpResponse::extractHeaders(message);
	return message;
}

std::string HttpRequest::doPut(ServerSocket::SocketState& socket, int& buffLen)
{
	std::string message, body, fileSizeString;
	char fileName[bufferSize];
	int res = put(fileName, socket);
	HttpStatus::eCode code;

	switch (res)
	{
	case FAILED:
	{
		cout << "PUT " << fileName << "Failed";
		code = HttpStatus::eCode::precondition_failed;
		body = R"({"status": "failed", "message" : "Resource update failed"})";
		break;
	}
	case OK:
	{
		code = HttpStatus::eCode::ok;
		body = R"({"status": "success", "message" : "Resource updated successfully"})";
		break;
	}

	case CREATED:
	{
		code = HttpStatus::eCode::created;
		body = R"({"status": "success", "message" : "Resource updated successfully"})";
		break;
	}
	}

	HttpResponse::Response* response = HttpResponse::buildHttpResponse(httpVer, code, "application/json", body);
	message = HttpResponse::httpResponseToString(*response);
	free(response);
	message = HttpResponse::extractHeaders(message);

	return message;
}

std::string HttpRequest::doDelete(ServerSocket::SocketState& socket, int& buffLen)
{
	string fileName = GetQuery(socket.buffer, "fileName");
	std::string message, body;
	fileName = string{ "C:\\temp\\" } + fileName;
	fileName += string{ ".txt" };
	bool deleteSuccess = true;
	HttpStatus::eCode code;
	

	deleteSuccess = (remove(fileName.c_str()) != 0);
	if (!deleteSuccess)
	{
		code = HttpStatus::eCode::bad_request;
		body = R"({"error": "delete unsuccessful"})";
	}
	else
	{
		code = HttpStatus::eCode::no_content;
	}

	HttpResponse::Response* ret;
	if(!deleteSuccess)
	{
		ret = HttpResponse::buildHttpResponse(httpVer, code, "application/json", body);
		message = HttpResponse::httpResponseToString(*ret);
	}else
	{
		ret = HttpResponse::buildHttpResponse(httpVer, code, "", "");
		message = HttpResponse::httpResponseToStringNoContent(*ret);
	}

	free(ret);
	return message;
}

std::string HttpRequest::doTrace(ServerSocket::SocketState& socket, int& buffLen)
{
	int fileSize = 0;
	std::string message, fileSizeString;
	char sendBuff[bufferSize];
	time_t currentTime;
	time(&currentTime);

	fileSize = strlen("TRACE");
	fileSize += strlen(socket.buffer);
	message = httpMessageStart(HttpStatus::eCode::ok, " OK \r\nContent-type: message/http\r\nDate: ");
	message += ctime(&currentTime);
	message += "Content-length: ";
	fileSizeString = to_string(fileSize);
	message += fileSizeString;
	message += "\r\n\r\n";
	message += "TRACE";
	message += socket.buffer;
	buffLen = message.size();
	return message;
}

std::string HttpRequest::doOptions(ServerSocket::SocketState& socket, int& buffLen)
{
	std::string message;
	char sendBuff[bufferSize];

	message = httpMessageStart(HttpStatus::eCode::ok, " No Content\r\nOptions: HEAD, GET, POST, PUT, TRACE, DELETE, OPTIONS\r\n");
	message += "Content-length: 0\r\n\r\n";
	buffLen = message.size();
	return message;
}

std::string HttpRequest::doPost(ServerSocket::SocketState& socket, int& buffLen)
{
	std::string message;
	char sendBuff[bufferSize];
	time_t currentTime;
	time(&currentTime);

	message = httpMessageStart(HttpStatus::eCode::ok, " OK \r\nDate: ");
	message += ctime(&currentTime);
	message += "Content-length: 0\r\n\r\n";
	string bodyMessage = get_field_value(string{ socket.buffer }, string{ "body" });
	cout << "Message received: " << bodyMessage << "\n";
	buffLen = message.size();
	return message;
}

std::string HttpRequest::doNotAllowed(ServerSocket::SocketState& socket, int& buffLen)
{
	std::string message;
	char sendBuff[bufferSize];
	time_t currentTime;
	time(&currentTime);

	message = httpMessageStart(HttpStatus::eCode::bad_request, " BAD REQUEST \r\nDate:");
	message += ctime(&currentTime);
	message += "Content-length: 0\r\n\r\n";
	buffLen = message.size();
	return message;
}


std::string HttpRequest::httpMessageStart(HttpStatus::eCode code, std::string message)
{
	if (message.empty())
	{
		return httpVer + " " + std::to_string(static_cast<int>(code)) + " " + HttpStatus::reasonPhrase(code);
	}

	return httpVer + " " + std::to_string(static_cast<int>(code)) + " " + HttpStatus::reasonPhrase(code) + message;
}

std::string HttpRequest::getLastModifiedTime(const std::string& filePath) {
	struct stat result;
	std::string lastModifiedTime;
	if (stat(filePath.c_str(), &result) != 0)
	{
		const time_t timeModified = result.st_mtime;
		const tm* gmt = gmtime(&timeModified);
		std::stringstream buffer;
		buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M");
		lastModifiedTime = buffer.str();

	}
	return lastModifiedTime;
}

std::ifstream::pos_type HttpRequest::getFileSize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

