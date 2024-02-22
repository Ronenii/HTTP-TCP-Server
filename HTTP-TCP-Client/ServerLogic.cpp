#include "ServerLogic.h"

#include <unordered_map>


void rcvMessage(int index, ServerSocket::SocketState* sockets, int& socketsCount)
{
	SOCKET msgSocket = sockets[index].id;

	int len = sockets[index].socketDataLen;
	int bytesRecv = recv(msgSocket, &sockets[index].buffer[len], sizeof(sockets[index].buffer) - len, 0);

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "HTTP Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(sockets[index], socketsCount, index);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);
		removeSocket(sockets[index], socketsCount, index);
		return;
	}
	else
	{
		sockets[index].buffer[len + bytesRecv] = '\0';
		cout << "HTTP Server: Recieved: " << bytesRecv << " bytes of \"" << &sockets[index].buffer[len] << " \message.\n\n";
		sockets[index].socketDataLen += bytesRecv;

		if (sockets[index].socketDataLen > 0)
		{
			string buffer = sockets[index].buffer;
			sockets[index].send = ServerSocket::eSocketStatus::SEND;

			switch (buffer)
			{
			case "GET":
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::get, 5);
				break;
			case "HEAD":
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::head, 6);
				break;
			case "PUT":
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::put, 0);
				break;
			case "DELETE":
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::del, 0);
				break;
			case "TRACE":
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::trace, 5);
				break;
			case "OPTIONS":
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::options, 9);
				break;
			case "POST":
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::post, 6);
				break;
			default:
				setSocketStateDet(sockets[index], HttpRequest::eRequestType::not_allowed, 0);
				break;
			}
		}
	}
}


void setSocketStateDet(ServerSocket::SocketState &socketState, HttpRequest::eRequestType type, int bufferIndex)
{
	socketState.httpReq = type;

	if(type != HttpRequest::eRequestType::put && type != HttpRequest::eRequestType::del && type != HttpRequest::eRequestType::not_allowed)
	{
		strcpy(socketState.buffer, &socketState.buffer[bufferIndex]);
		socketState.socketDataLen = strlen(socketState.buffer);
		socketState.buffer[socketState.socketDataLen] = NULL;
	}
}


void acceptConnection(int index, SocketState* sockets, int& socketsCount)
{
	SOCKET id = sockets[index].id;
	sockets[index].prevActivity = time(0);
	struct sockaddr_in from;
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket) {
		cout << "HTTP Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << "HTTP Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;
	unsigned long flag = 1;
	if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0) {
		cout << "HTTP Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
	}
	if (addSocket(msgSocket, RECEIVE, sockets, socketsCount) == false) {
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
}
bool addSocket(SOCKET id, enum eSocketStatus what, SocketState* sockets, int& socketsCount)
{
	for (int i = 0; i < MAX_SOCKETS; i++) {
		if (sockets[i].recv == EMPTY) {
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = IDLE;
			sockets[i].prevActivity = time(0);
			sockets[i].socketDataLen = 0;
			socketsCount++;
			return true;
		}
	}
	return false;
}
void removeSocket(ServerSocket::SocketState &socket, int& socketsCount, int index)
{
	socket.recv = ServerSocket::eSocketStatus::EMPTY;
	socket.send = ServerSocket::eSocketStatus::EMPTY;
	socket.prevActivity = 0;
	socketsCount--;
	cout << "The socket number " << index << " has been removed\n" << endl;
}

bool sendMessage(int index, ServerSocket::SocketState* sockets)
{
	int bytesSent = 0, buffLen = 0;
	char sendBuff[BUFFSIZE];
	string message;
	SOCKET msgSocket = sockets[index].id;
	sockets[index].prevActivity = time(0);

	switch (sockets[index].httpReq)
	{
	case HttpRequest::eRequestType::head:
	{
		message = HttpRequest::doHead(sockets[index], buffLen);
		break;
	}
	case HttpRequest::eRequestType::get:
	{
		message = HttpRequest::doGet(sockets[index], buffLen);
		break;
	}

	case HttpRequest::eRequestType::put:
	{
		message = HttpRequest::doPut(sockets[index], buffLen);
		break;
	}
	case HttpRequest::eRequestType::del:
	{
		message = HttpRequest::doDelete(sockets[index], buffLen);
		break;
	}
	case HttpRequest::eRequestType::trace:
	{
		message = HttpRequest::doTrace(sockets[index], buffLen);
		break;
	}

	case HttpRequest::eRequestType::options: 
	{
		message = HttpRequest::doOptions(sockets[index], buffLen);
		break;
	}

	case HttpRequest::eRequestType::post:
	{
		message = HttpRequest::doPost(sockets[index], buffLen);
		break;
	}
	case HttpRequest::eRequestType::not_allowed:
		message = HttpRequest::doNotAllowed(sockets[index], buffLen);
	}

	strcpy(sendBuff, message.c_str());
	bytesSent = send(msgSocket, sendBuff, buffLen, 0);
	memset(sockets[index].buffer, 0, BUFFSIZE);
	sockets[index].socketDataLen = 0;
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "HTTP Server: Error at send(): " << WSAGetLastError() << endl;
		return false;
	}
	sockets[index].send = ServerSocket::eSocketStatus::IDLE;
	return true;
}

int put(char* filename, ServerSocket::SocketState &socket)
{
	int buffLen = 0;
	int CODE = OK, content_len = 0;
	string str_buffer = { socket.buffer };
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