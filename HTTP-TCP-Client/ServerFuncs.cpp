#include "ServerFuncs.h"
#include <sstream>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <__msvc_chrono.hpp>

bool receiveMessage(int index, SocketInfo* sockets, int& socketCount) {
	SOCKET currentSocket = sockets[index].socketId;

	int bytesReceived = recv(currentSocket, &sockets[index].buffer[sockets[index].bufferLength],
		sizeof(sockets[index].buffer) - sockets[index].bufferLength, 0);

	if (bytesReceived == SOCKET_ERROR) {
		std::cerr << "Error at recv(): " << WSAGetLastError() << std::endl;
		closesocket(currentSocket);
		removeSocket(index, sockets, socketCount); // Call the separate removeSocket function
		return false;
	}
	else if (bytesReceived == 0) {
		closesocket(currentSocket);
		removeSocket(index, sockets, socketCount);
		return false;
	}
	else {
		// Add null terminator to the received data
		sockets[index].buffer[sockets[index].bufferLength + bytesReceived] = '\0';

		std::cout << "Received: " << bytesReceived << " bytes of message: \""
			<< &sockets[index].buffer[sockets[index].bufferLength] << "\"" << std::endl;

		sockets[index].bufferLength += bytesReceived;

		if (sockets[index].bufferLength > 0) {
			sockets[index].sendStatus = SEND; // Set send status to indicate data is ready to send

			// Handle different HTTP request types
			if (strncmp(sockets[index].buffer, "GET", 3) == 0) {
				sockets[index].httpMethod = GET;
				// Use strncpy with the correct buffer size to prevent overflows
				strncpy(sockets[index].buffer, &sockets[index].buffer[5], sizeof(sockets[index].buffer) - 1);
				sockets[index].bufferLength = strlen(sockets[index].buffer);
				return true;
			}
			else if (strncmp(sockets[index].buffer, "HEAD", 4) == 0) {
				sockets[index].httpMethod = HEAD;
				strncpy(sockets[index].buffer, &sockets[index].buffer[6], sizeof(sockets[index].buffer) - 1);
				sockets[index].bufferLength = strlen(sockets[index].buffer);
				return true;
			}
			// ... (similar logic for other HTTP methods)
			else {
				sockets[index].httpMethod = NOT_ALLOWED;
				return true;
			}
		}
	}

	return true; // Indicate successful message processing
}



void acceptConnection(int listenerSocket, SocketInfo* sockets, int& socketCount) {
	struct sockaddr_in clientAddress;
	int addressLength = sizeof(clientAddress);

	SOCKET clientSocket = accept(listenerSocket, (struct sockaddr*)&clientAddress, &addressLength);
	if (INVALID_SOCKET == clientSocket) {
		std::cerr << "Error at accept(): " << WSAGetLastError() << std::endl;
		return;
	}

	char* clientIp = inet_ntoa(clientAddress.sin_addr);
	unsigned short clientPort = ntohs(clientAddress.sin_port);

	std::cout << "Client " << clientIp << ":" << clientPort << " is connected." << std::endl;

	unsigned long nonBlocking = 1;
	if (ioctlsocket(clientSocket, FIONBIO, &nonBlocking) != 0) {
		std::cerr << "Error at ioctlsocket(): " << WSAGetLastError() << std::endl;
	}

	if (!addSocket(clientSocket, RECEIVE, sockets, socketCount)) {
		std::cout << "\t\tToo many connections, dropped!" << std::endl;
		closesocket(clientSocket);
	}
}
bool addSocket(SOCKET socketId, eSocketStatus status, SocketInfo* sockets, int& socketCount) {
	for (int i = 0; i < MAX_SOCKETS; i++) {
		if (sockets[i].recvStatus == EMPTY) {
			sockets[i].socketId = socketId;
			sockets[i].recvStatus = status;
			sockets[i].sendStatus = IDLE;
			sockets[i].lastActivity = time(0);
			sockets[i].bufferLength = 0;
			socketCount++;
			return true;
		}
	}
	return false;
}
void removeSocket(int index, SocketInfo* sockets, int& socketCount) {
	sockets[index].recvStatus = EMPTY;
	sockets[index].sendStatus = EMPTY;
	sockets[index].lastActivity = 0;
	socketCount--;
	std::cout << "Socket " << index << " has been removed." << std::endl;
}


bool sendMessage(int index, SocketInfo* sockets) {
	int bytesSent = 0;
	char sendBuff[BUFFSIZE];

	SocketInfo& currentSocket = sockets[index];

	time_t currentTime;
	time(&currentTime);

	currentSocket.lastActivity = currentTime;

	switch (currentSocket.httpMethod) {
	case HEAD:
		if (!handleHeadRequest(index, sockets)) {
			return false;
		}
		break;
	case GET:
		if (!handleGetRequest(index, sockets)) {
			return false;
		}
		break;
	case PUT:
		if (!handlePutRequest(index, sockets)) {
			return false;
		}
		break;
	case DELETE1:
		if (!handleDeleteRequest(index, sockets)) {
			return false;
		}
		break;
	case TRACE:
		if (!handleTraceRequest(index, sockets)) {
			return false;
		}
		break;
	case OPTIONS:
		if (!handleOptionsRequest(index, sockets)) {
			return false;
		}
		break;
	case POST:
		if (!handlePostRequest(index, sockets)) {
			return false;
		}
		break;
	case NOT_ALLOWED:
		handleNotAllowedRequest(index, sockets);
		break;
	}

	bytesSent = send(currentSocket.socketId, sendBuff, strlen(sendBuff), 0);

	memset(currentSocket.buffer, 0, BUFFSIZE);
	currentSocket.bufferLength = 0;

	if (SOCKET_ERROR == bytesSent) {
		cout << "HTTP Server: Error at send(): " << WSAGetLastError() << endl;
		return false;
	}

	currentSocket.sendStatus = IDLE;
	return true;
}

string getCurrentTimeFormatted() {
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::tm* gmt = std::gmtime(&currentTime);
	std::stringstream buffer;
	buffer << std::put_time(gmt, "%A, %d %B %Y %H:%M:%S GMT");

	return buffer.str();
}

bool handleHeadRequest(int index, SocketInfo* sockets, char * sendBuff) {
	string fileAddress = "C:\\temp\\indexen.html";
	ifstream file(fileAddress);

	string responseMessage;
	if (!file.is_open()) {
		responseMessage = "HTTP/1.1 " + to_string(NOT_FOUND) + " Not Found\r\n";
	}
	else {
		responseMessage = "HTTP/1.1 " + to_string(OK) + " OK\r\n";
		file.seekg(0, ios::end);
		int fileSize = file.tellg();
		responseMessage += "Content-type: text/html\r\n";
		responseMessage += "Date: " + getCurrentTimeFormatted() + "\r\n";
		responseMessage += "Content-length: " + to_string(fileSize) + "\r\n\r\n";
	}

	// Copy the response message to the send buffer
	strcpy(sendBuff, responseMessage.c_str());

	return true;
}

int put(int index, char* filename, SocketState* sockets)
{
	int buffLen = 0;
	int CODE = OK, content_len = 0;
	string str_buffer = { sockets[index].buffer };
	string value, file_name;
	value = get_field_value(str_buffer, "Content-Length");
	content_len = stoi(value);
	file_name = GetQuery(str_buffer, "fileName");
	value = get_field_value(str_buffer, "body");
	strcpy(filename, file_name.c_str());
	file_name = string{ filename };
	file_name = string{ "C:\\temp\\" } + file_name + string{ ".txt" };
	fstream outPutFile;
	if (file_name.find("error") == string::npos) {
		try {
			outPutFile.open(file_name);
			if (!outPutFile.good()) {
				outPutFile.open(file_name.c_str(), ios::out);
				CODE = CREATED;
			}
			if (!outPutFile.good()) {
				cout << "HTTP Server: Error writing file to local storage: " << WSAGetLastError() << endl;
				CODE = FAILED;
			}
			if (value.empty()) {
				CODE = NO_CONTENT;
			}
			else {
				outPutFile << value;
			}
		}
		catch (const exception&) {
			outPutFile.close();
		}
	}
	else {
		cout << "HTTP Server: Error writing file to local storage: 'Error' name is not allowed" << endl;
		CODE = FAILED;
	}
	return CODE;
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
