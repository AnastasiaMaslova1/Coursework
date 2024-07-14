#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 

#include<iostream>
#include<fstream>
#include<string>
#include<experimental/filesystem> 

#pragma warning(disable: 4996)
#pragma comment(lib, "ws2_32.lib")
#include<WinSock2.h>

using namespace std;

void send_file(SOCKET* sock, const string& file_name) {
	
	fstream file;
	file.open(file_name, ios_base::in | ios_base::binary);

	if (!file.is_open()) {
		cout << "Error opening file: " << file_name << endl;
		return;
	}
	else {
		int file_size = experimental::filesystem::file_size(file_name) + 1;

		char* bytes = new char[file_size];
		file.read(bytes, file_size);

		cout << "size: " << file_size << endl;
		cout << "name: " << file_name << endl;
		cout << "data: " << bytes << endl;

		if (send(*sock, to_string(file_size).c_str(), 16, 0) == SOCKET_ERROR ||
			send(*sock, file_name.c_str(), 32, 0) == SOCKET_ERROR ||
			send(*sock, bytes, file_size, 0) == SOCKET_ERROR) {
			cout << "Send failed: " << WSAGetLastError() << endl;
		}
		delete[] bytes;
	}
	file.close();
		
}

int main() {

	WORD dllVer = MAKEWORD(2, 1); 
	WSAData wsad; 

	if (WSAStartup(dllVer, &wsad) != 0) {
		cout << "WSAStartup failed\n";
		return 1;
	}

	SOCKADDR_IN addr_info;
	memset(&addr_info, 0, sizeof(SOCKADDR_IN));
	int size_addr = sizeof(addr_info);
	addr_info.sin_port = htons(4321);
	addr_info.sin_family = AF_INET;
	SOCKET s_listen = socket(AF_INET, SOCK_STREAM, 0);
	bind(s_listen, (sockaddr*)&addr_info, sizeof(addr_info));
	listen(s_listen, SOMAXCONN);
	SOCKET s_for_connect = accept(s_listen, (sockaddr*)&addr_info, &size_addr);



	SOCKADDR_IN addr_info;
	memset(&addr_info, 0, sizeof(SOCKADDR_IN));

	int size_addr = sizeof(addr_info);
	addr_info.sin_port = htons(4321);
	addr_info.sin_family = AF_INET;

	SOCKET s_listen = socket(AF_INET, SOCK_STREAM, 0);

	if (s_listen == INVALID_SOCKET) {
		cout << "Socket creation failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	if (bind(s_listen, (sockaddr*)&addr_info, sizeof(addr_info)) == SOCKET_ERROR) {
		cout << "Bind failed: " << WSAGetLastError() << endl;
		closesocket(s_listen);
		WSACleanup();
		return 1;
	}

	if (listen(s_listen, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed: " << WSAGetLastError() << endl;
		closesocket(s_listen);
		WSACleanup();
		return 1;
	}

	SOCKET s_for_connect = accept(s_listen, (sockaddr*)&addr_info, &size_addr);

	if (s_for_connect == INVALID_SOCKET) {
		cout << "Accept failed: " << WSAGetLastError() << endl;
		closesocket(s_listen);
		WSACleanup();
		return 1;
	}
		
	cout << "CONNECT\n";

	string path;
	cin >> path;
	send_file(&s_for_connect, path);

	closesocket(s_listen);
	closesocket(s_for_connect);
	WSACleanup();

	system("pause");

	return 0;
}





