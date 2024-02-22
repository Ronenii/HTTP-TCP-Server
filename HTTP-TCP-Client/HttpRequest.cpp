#include "HttpRequest.h"
#include "ServerLogic.h"


std::string HttpRequest::doGet(ServerSocket::SocketState& socket, int& buffLen)
{
	int fileSize = 0;
	char* subBuff;
	char sendBuff[bufferSize], readBuff[bufferSize];
	std::string message, fileSizeString, fileAddress;
	std::ifstream File;
	socket.prevActivity = time(0);
	fileAddress = "C:\\temp\\index";
	std::string FileContent = "";
	subBuff = strtok(socket.buffer, " ");
	std::string langValue = GetQuery(subBuff, "lang");
	time_t currentTime;
	time(&currentTime);

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
		message = httpMessageStart(HttpStatus::eCode::not_found, nullptr);
		File.open("C:\\temp\\error.html");
	}
	else
	{
		message = httpMessageStart(HttpStatus::eCode::ok, nullptr);
	}

	if (File)
	{
		while (File.getline(readBuff, BUFFSIZE))
		{
			FileContent += readBuff;
			fileSize += strlen(readBuff);
		}
	}

	message += "\r\nContent-type: text/html";
	message += "\r\nDate:";
	message += ctime(&currentTime);
	message += "Content-length: ";
	fileSizeString = to_string(fileSize);
	message += fileSizeString;
	message += "\r\n\r\n";
	message += FileContent;
	buffLen = message.size();
	File.close();
	return message;
}

std::string HttpRequest::doHead(ServerSocket::SocketState& socket, int& buffLen)
{
	int fileSize = 0;
	char sendBuff[bufferSize];
	std::string message, fileSizeString;
	const std::string filePath = "C:\\temp\\indexen.html";
	std::ifstream File;
	socket.prevActivity = time(0);
	
	File.open(filePath);
	if (!File) {
		message = httpMessageStart(HttpStatus::eCode::not_found, nullptr);
		fileSize = 0;
	}
	else {
		message = httpMessageStart(HttpStatus::eCode::ok, nullptr);
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
	buffLen = message.size();
	File.close();
	return message;
}

std::string HttpRequest::doPut(ServerSocket::SocketState& socket, int& buffLen)
{
	std::string message, fileSizeString;
	int fileSize = 0;
	char fileName[bufferSize], sendBuff[bufferSize];
	int res = put(fileName, socket);
	time_t currentTime;
	time(&currentTime);

	switch (res)
	{
	case FAILED:
		{
			cout << "PUT " << fileName << "Failed";
			message = httpMessageStart(HttpStatus::eCode::precondition_failed, " Precondition failed \r\nDate: ");
			break;
		}
	case OK:
		{
			message = httpMessageStart(HttpStatus::eCode::ok, " OK \r\nDate: ");
			break;
		}

	case CREATED:
		{
			message = httpMessageStart(HttpStatus::eCode::created, " CREATED \r\nDate: ");
			break;
		}
	}

	message += ctime(&currentTime);
	message += "Content-length: ";
	fileSizeString = to_string(fileSize);
	message += fileSizeString;
	message += "\r\n\r\n";
	buffLen = message.size();
	return message;
}

std::string HttpRequest::doDelete(ServerSocket::SocketState& socket, int& buffLen)
{
	int fileSize = 0;
	string fileName = GetQuery(socket.buffer, "fileName");
	std::string message, fileSizeString;
	fileName = string{ "C:\\temp\\" } + fileName;
	fileName += string{ ".txt" };
	char sendBuff[bufferSize];
	time_t currentTime;
	time(&currentTime);

	if (remove(fileName.c_str()) != 0)
	{
		message = httpMessageStart(HttpStatus::eCode::no_content, " File not found \r\nDate: ");
	}
	else
	{
		message = httpMessageStart(HttpStatus::eCode::ok, " OK DELETED \r\nDate: ");
	}

	message += ctime(&currentTime);
	message += "Content-length: ";
	fileSizeString = to_string(fileSize);
	message += fileSizeString;
	message += "\r\n\r\n";
	buffLen = message.size();
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
	if(message.empty())
	{
		return httpVer + " " + std::to_string(static_cast<int>(code)) + " " + HttpStatus::reasonPhrase(code);
	}

	return httpVer + " " + std::to_string(static_cast<int>(code)) + " " + HttpStatus::reasonPhrase(code) + message;
}

std::string getLastModifiedTime(const std::string& filePath) {
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

