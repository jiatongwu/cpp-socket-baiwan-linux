
#ifndef _EasyTcpClient_hpp_
#define  _EasyTcpClient_hpp_
#define WIN32_LEAN_AND_MEAN
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
#include <windows.h>
#include <WinSock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif

#include <iostream>
#include <thread>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
class EasyTcpClient
{
private:
	SOCKET _sock;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	//?????
	virtual ~EasyTcpClient()
	{
		close();
	}
	//???socket
	void initSocket()
	{

#ifdef _WIN32
			WORD ver = MAKEWORD(2, 2);
			WSADATA dat;
			WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _sock)
		{
			close();
			cout<<"??????"<<endl;
		}
		///
		//1.?? socket
		_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == _sock)
		{
			cout << "??socket??" << endl;

		}
		else
		{
			cout << "??socket??" << endl;
		}



	}
	//??
	int connectServer(char* ip,unsigned short port)
	{
		int ret = -1;
		if (INVALID_SOCKET == _sock)
		{
			initSocket();
		}
		//2.?????
		sockaddr_in _in;
		_in.sin_family = AF_INET;
		_in.sin_port = htons(port);
#ifdef _WIN32
		_in.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_in.sin_addr.s_addr = inet_addr(ip);
#endif
		ret = connect(_sock, (sockaddr*)&_in, sizeof(_in));
		if (SOCKET_ERROR == ret)
		{
			cout << "???????" << endl;

		}
		else
		{
			cout << "???????" << endl;
		}

		return  ret;

	}
	//??socket
	void close()
	{
		if(INVALID_SOCKET!=_sock)
		{
			//4.?????socket
#ifdef _WIN32
			closesocket(_sock);
			WSACleanup();
#else
			close(_sock);
#endif
			cout << "?????" << endl;
		}

	}

private:

};
#endif
