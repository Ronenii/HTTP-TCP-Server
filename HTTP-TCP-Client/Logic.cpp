#include "Logic.h"
#include <sstream>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <iomanip>

void rcvMessage(int index, SocketState* sockets, int& socketsCount)
{
	SOCKET msgSocket = sockets[index].id;

	int len = sockets[index].socketDataLen;
	int bytesRecv = recv(msgSocket, &sockets[index].buffer[len], sizeof(sockets[index].buffer) - len, 0);

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "HTTP Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(index, sockets, socketsCount);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(index, sockets, socketsCount);
		return;
	}
	else
	{
		sockets[index].buffer[len + bytesRecv] = '\0';
		cout << "HTTP Server: Recieved: " << bytesRecv << " bytes of \"" << &sockets[index].buffer[len] << " \message.\n\n";
		sockets[index].socketDataLen += bytesRecv;


		if (sockets[index].socketDataLen > 0)
		{

			sockets[index].send = SocketStatus::SEND;

			if (strncmp(sockets[index].buffer, "GET", 3) == 0)
			{
				sockets[index].httpReq = RequestType::GET;
				strcpy(sockets[index].buffer, &sockets[index].buffer[5]);
				sockets[index].socketDataLen = strlen(sockets[index].buffer);
				sockets[index].buffer[sockets[index].socketDataLen] = NULL;
				return;
			}
			if (strncmp(sockets[index].buffer, "HEAD", 4) == 0)
			{
				sockets[index].httpReq = RequestType::HEAD;
				strcpy(sockets[index].buffer, &sockets[index].buffer[6]);
				sockets[index].socketDataLen = strlen(sockets[index].buffer);
				sockets[index].buffer[sockets[index].socketDataLen] = NULL;
				return;
			}
			if (strncmp(sockets[index].buffer, "PUT", 3) == 0)
			{
				sockets[index].httpReq = RequestType::PUT;
				return;
			}
			if (strncmp(sockets[index].buffer, "DELETE", 6) == 0)
			{
				sockets[index].httpReq = RequestType::DELETE1;
				return;
			}
			if (strncmp(sockets[index].buffer, "TRACE", 5) == 0)
			{
				sockets[index].httpReq = RequestType::TRACE;
				strcpy(sockets[index].buffer, &sockets[index].buffer[5]);
				sockets[index].socketDataLen = strlen(sockets[index].buffer);
				sockets[index].buffer[sockets[index].socketDataLen] = NULL;
				return;
			}
			if (strncmp(sockets[index].buffer, "OPTIONS", 7) == 0)
			{
				sockets[index].httpReq = RequestType::OPTIONS;
				strcpy(sockets[index].buffer, &sockets[index].buffer[9]);
				sockets[index].socketDataLen = strlen(sockets[index].buffer);
				sockets[index].buffer[sockets[index].socketDataLen] = NULL;
				return;
			}
			if (strncmp(sockets[index].buffer, "POST", 4) == 0)
			{
				sockets[index].httpReq = RequestType::POST;
				strcpy(sockets[index].buffer, &sockets[index].buffer[6]);
				sockets[index].socketDataLen = strlen(sockets[index].buffer);
				sockets[index].buffer[sockets[index].socketDataLen] = NULL;
				return;
			}
			sockets[index].httpReq = RequestType::NOT_ALLOWED_REQ;
		}
	}
}



void acceptConnection(int index, SocketState* sockets, int& socketsCount)
{
	SOCKET id = sockets[index].id;
	sockets[index].prevActivity = time(0);
	struct sockaddr_in from;
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket){
		cout << "HTTP Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "HTTP Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;
	unsigned long flag = 1;
	if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0){
		cout << "HTTP Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
	}
	if (addSocket(msgSocket, SocketStatus::RECEIVE, sockets, socketsCount) == false){
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
}
bool addSocket(SOCKET id, SocketStatus what, SocketState* sockets, int& socketsCount)
{
	for (int i = 0; i < MAX_SOCKETS; i++) {
		if (sockets[i].recv == SocketStatus::EMPTY) {
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = SocketStatus::IDLE;
			sockets[i].prevActivity = time(0);
			sockets[i].socketDataLen = 0;
			socketsCount++;
			return true;
		}
	}
	return false;
}
void removeSocket(int index, SocketState* sockets, int& socketsCount)
{
	sockets[index].recv = SocketStatus::EMPTY;
	sockets[index].send = SocketStatus::EMPTY;
	sockets[index].prevActivity = 0;
	socketsCount--;
	cout << "The socket number " << index << " has been removed\n" << endl;
}

bool sendMessage(int index, SocketState* sockets)
{
	int fileSize = 0;
	string message, fileSizeString, fileAddress, FileContent;

	time_t currentTime;
	time(&currentTime);

	SOCKET msgSocket = sockets[index].id;
	sockets[index].prevActivity = time(0);

	switch (sockets[index].httpReq)
	{
	case RequestType::HEAD:
		fileAddress = "C:\\temp\\indexen.html";
		FileContent = getFileContent(fileAddress, fileSize);
		message = generateResponse(HttpStatus::NO_CONTENT, "", fileSize);
		break;

	case RequestType::GET:
		fileAddress = buildFileAddress(sockets[index].buffer);
		FileContent = getFileContent(fileAddress, fileSize);
		message = generateResponse(HttpStatus::OK, FileContent, fileSize);
		break;

	case RequestType::PUT:
		message = handlePutRequest(index, fileSize, sockets);
		break;

	case RequestType::DELETE1:
		message = handleDeleteRequest(sockets[index].buffer, fileSize);
		break;

	case RequestType::TRACE:
		message = generateTraceResponse(sockets[index].buffer, fileSize);
		break;

	case RequestType::OPTIONS:
		message = generateOptionsResponse();
		break;

	case RequestType::POST:
		message = handlePostRequest(sockets[index].buffer);
		break;

	case RequestType::NOT_ALLOWED_REQ:
		message = generateResponse(HttpStatus::NOT_OK, "", 0);
		break;
	}

	int bytesSent = send(msgSocket, message.c_str(), message.size(), 0);

	clearSocketData(index, sockets);

	if (SOCKET_ERROR == bytesSent)
	{
		cout << "HTTP Server: Error at send(): " << WSAGetLastError() << endl;
		return false;
	}

	sockets[index].send = SocketStatus::IDLE;
	return true;
}

string getFileContent(const string& fileAddress, int& fileSize)
{
	ifstream File(fileAddress);
	stringstream contentStream;

	if (File.is_open())
	{
		string line;
		while (getline(File, line))
		{
			contentStream << line;
			fileSize += line.length();
		}
		File.close();
	}

	return contentStream.str();
}

string generateResponse(HttpStatus status, const string& content, int fileSize)
{
	string message = "HTTP/1.1 " + to_string(static_cast<int>(status));
	message += (status == HttpStatus::NOT_FOUND) ? " Not Found " : " OK ";
	message += "\r\nContent-type: text/html";
	message += "\r\nDate: " + generateFormattedTime();
	message += "\r\nContent-length: " + to_string(fileSize);
	message += "\r\n\r\n" + content;

	return message;
}

string buildFileAddress(char* buffer)
{
	string subBuff = strtok(buffer, " ");
	string fileAddress = "C:\\temp\\index";

	string langValue = GetQuery(subBuff.c_str(), "lang");

	if (langValue.empty())
	{
		fileAddress += "en";
	}
	else
	{
		fileAddress += langValue;
	}

	fileAddress += ".html";
	return fileAddress;
}

string handlePutRequest(int index, int& fileSize, SocketState* sockets)
{
	char fileName[BUFFSIZE];
	int res = put(index, fileName, sockets);

	HttpStatus httpStatus = static_cast<HttpStatus>(res);
	string message = "HTTP/1.1 " + to_string(static_cast<int>(httpStatus));
	message += (httpStatus == HttpStatus::FAILED) ? " Precondition failed " : " OK ";
	message += "\r\nDate: " + generateFormattedTime();
	message += "\r\nContent-length: " + to_string(fileSize);
	message += "\r\n\r\n";

	return message;
}

string handleDeleteRequest(const char* buffer, int& fileSize)
{
	string fileName = GetQuery(buffer, "fileName");
	fileName = "C:\\temp\\" + fileName + ".txt";

	HttpStatus httpStatus = (remove(fileName.c_str()) != 0) ? HttpStatus::NO_CONTENT : HttpStatus::OK;

	string message = "HTTP/1.1 " + to_string(static_cast<int>(httpStatus));
	message += (httpStatus == HttpStatus::NO_CONTENT) ? " File not found " : " OK DELETED ";
	message += "\r\nDate: " + generateFormattedTime();
	message += "\r\nContent-length: " + to_string(fileSize);
	message += "\r\n\r\n";

	return message;
}

string generateTraceResponse(const char* buffer, int& fileSize)
{
	fileSize = strlen("TRACE") + strlen(buffer);

	string message = "HTTP/1.1 " + to_string(static_cast<int>(HttpStatus::OK)) + " OK \r\nContent-type: message/http\r\nDate: ";
	message += generateFormattedTime();
	message += "\r\nContent-length: " + to_string(fileSize);
	message += "\r\n\r\nTRACE" + string(buffer);

	return message;
}

string generateOptionsResponse()
{
	string message = "HTTP/1.1 " + to_string(static_cast<int>(HttpStatus::NO_CONTENT)) + " No Content\r\nOptions: HEAD, GET, POST, PUT, TRACE, DELETE, OPTIONS\r\n";
	message += "Content-length: 0\r\n\r\n";

	return message;
}

string handlePostRequest(const char* buffer)
{
	string message = "HTTP/1.1 " + to_string(static_cast<int>(HttpStatus::OK)) + " OK \r\nDate:";
	message += generateFormattedTime();
	message += "\r\nContent-length: 0\r\n\r\n";

	string bodyMessage = get_field_value(string{ buffer }, string{ "body" });
	cout << "Message received: " << bodyMessage << "\n";

	return message;
}

string generateFormattedTime()
{
	time_t currentTime;
	time(&currentTime);
	tm* gmt = gmtime(&currentTime);
	stringstream buffer;
	buffer << put_time(gmt, "%A, %d %B %Y %H:%M");
	return buffer.str();
}

void clearSocketData(int index, SocketState* sockets)
{
	memset(sockets[index].buffer, 0, BUFFSIZE);
	sockets[index].socketDataLen = 0;
}

int put(int index, char* filename, SocketState* sockets)
{
	HttpStatus CODE = HttpStatus::OK;
	string str_buffer = { sockets[index].buffer };
	string value, file_name;
	value = get_field_value(str_buffer, "Content-Length");
	file_name = GetQuery(str_buffer, "fileName");
	value = get_field_value(str_buffer, "body");
	strcpy(filename, file_name.c_str());
	file_name = string{ filename };
	file_name = string{ "C:\\temp\\" } + file_name + string{ ".txt" };
	fstream outPutFile;
	if (file_name.find("error") == string::npos){
		try{
			outPutFile.open(file_name);
			if (!outPutFile.good())	{
				outPutFile.open(file_name.c_str(), ios::out);
				CODE = HttpStatus::CREATED;
			}
			if (!outPutFile.good()){
				cout << "HTTP Server: Error writing file to local storage: " << WSAGetLastError() << endl;
				CODE = HttpStatus::FAILED;
			}
			if (value.empty()){
				CODE = HttpStatus::NO_CONTENT;
			}
			else{
				outPutFile << value;
			}
		}
		catch (const exception&){
			outPutFile.close();
		}
	}
	else{
		cout << "HTTP Server: Error writing file to local storage: 'Error' name is not allowed" << endl;
		CODE = HttpStatus::FAILED;
	}
	return static_cast<int>(CODE);
}

template <typename TP>
time_t parse_to_time_t(TP tp)
{
	using namespace chrono;
	auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
		+ system_clock::now());
	return system_clock::to_time_t(sctp);
}

string get_field_value(const string& request, const string& field) {
	unordered_map<string, string> fields;
	string line, prev_line;
	stringstream request_stream(request);
	while (getline(request_stream, line)) {
		size_t sep = line.find(':');
		if (sep != string::npos) {
			string key = line.substr(0, sep);
			string value = line.substr(sep + 1);
			key = key.substr(key.find_first_not_of(" \t"));
			key = key.substr(0, key.find_last_not_of(" \t") + 1);
			value = value.substr(value.find_first_not_of(" \t"));
			value = value.substr(0, value.find_last_not_of(" \t") + 1);
			value.pop_back();
			fields[key] = value;
		}
		prev_line = line;
	}
	unordered_map<string, string>::iterator it = fields.find(field);
	if (it != fields.end()) {
		return it->second;
	}
	else {
		if (field.find("body") != string::npos)
		{
			string body = "";
			size_t body_start = request.find("\r\n\r\n");
			if (body_start != string::npos)
				body = request.substr(body_start + 4);
			return body;
		}
		return "";
	}
}
string GetQuery(const string& request, const string& param)
{
	string line, value = { "" };
	size_t ValueIndex, endIndex, paramIndex;
	stringstream request_stream(request);
	while (getline(request_stream, line)) {
		paramIndex = line.find(param);
		if (paramIndex != string::npos) {
			ValueIndex = line.find("=", paramIndex) + 1;
			endIndex = line.find(" ", ValueIndex);
			if (endIndex == string::npos) {
				endIndex = line.length();
			}
			value = line.substr(ValueIndex, endIndex - ValueIndex);
			break;
		}
	}
	return value;
}
