#include<iostream>
#include<fstream>

#pragma warning(disable:4996) 
#pragma comment(lib, "ws2_32.lib") 
#include<WinSock2.h> 

using namespace std;

void recv_file(SOCKET* sock) { 

	char file_size_str[16]; 
	char file_name[32]; 

	if (recv(*sock, file_size_str, 16, 0) == SOCKET_ERROR) {
		cout << "Receive file size failed: " << WSAGetLastError() << endl;
		return;
	}

	int file_size = atoi(file_size_str); 
	char* bytes = new char[file_size]; 

	if (recv(*sock, file_name, 32, 0) == SOCKET_ERROR) {
		cout << "Receive file name failed: " << WSAGetLastError() << endl;
		return;
	}

	fstream file;
	file.open(file_name, ios_base::out | ios_base::binary); 
	cout << "size: " << file_size << endl;
	cout << "name: " << file_name << endl;

	if (!file.is_open()) {
		cout << "Error opening file: " << file_name << endl;
		delete[] bytes;
		return;
	}
	else {

		if (recv(*sock, bytes, file_size, 0) == SOCKET_ERROR) {
			cout << "Receive file data failed: " << WSAGetLastError() << endl;
			delete[] bytes;
			file.close();
			return;
		}

		cout << "data: " << bytes << endl;

		file.write(bytes, file_size); 
		cout << "ok save\n";
	

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
	addr_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr_info.sin_port = htons(4321);
	addr_info.sin_family = AF_INET;
	SOCKET s_client = socket(AF_INET, SOCK_STREAM, 0);



	SOCKADDR_IN addr_info; 
	memset(&addr_info, 0, sizeof(SOCKADDR_IN)); 

	addr_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	addr_info.sin_port = htons(4321); 
	addr_info.sin_family = AF_INET; 

	SOCKET s_client = socket(AF_INET, SOCK_STREAM, 0); 

	if (s_client == INVALID_SOCKET) {
		cout << "Socket creation failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	if (connect(s_client, (sockaddr*)&addr_info, sizeof(addr_info)) == SOCKET_ERROR) {
		cout << "Connect failed: " << WSAGetLastError() << endl;
		closesocket(s_client);
		WSACleanup();
		return 1;
	}

	cout << "CONNECT\n";
	recv_file(&s_client);

	closesocket(s_client);
	WSACleanup();

	system("pause");

	return 0;
}
