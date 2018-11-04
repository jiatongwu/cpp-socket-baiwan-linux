
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
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR,
	CMD_NEW_USER_JOIN
};

struct DataHeader
{
	int dataLength;//????
	int cmd;//??
};
struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;

	}
	char username[32];
	char password[32];
};
struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};
struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;

	}
	char username[32];
};
struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
bool g_boolExit = true;
using namespace std;
void cmdThread(SOCKET _sock)
{
	char cmdBuffer[128];
	while (true)
	{

		//3.??????
		cin >> cmdBuffer;
		//4.??????
		if (0 == strcmp(cmdBuffer, "exit"))
		{
			cout << "????????? cmdThread??" << endl;
			g_boolExit = false;
			break;

		}
		else if (0 == strcmp(cmdBuffer, "login"))
		{
			Login login;
			strcpy(login.username, "wu");
			strcpy(login.password, "123456");
			//????????
			send(_sock, (const char*)&login, sizeof(Login), 0);
			//??????????
			//LoginResult loginResult;
			//recv(_sock, (char*)&loginResult, sizeof(LoginResult), 0);
			//cout << "loginResult:" << loginResult.result << endl;

		}
		else if (0 == strcmp(cmdBuffer, "logout"))
		{
			Logout logout;
			strcpy(logout.username, "wu");
			//????????
			send(_sock, (const char*)&logout, sizeof(Logout), 0);
			//??????????
			//LogoutResult logoutResult;
			//recv(_sock, (char*)&logoutResult, sizeof(LogoutResult), 0);
			//cout << "logoutResult:" << logoutResult.result << endl;
		}
		else
		{
			cout << "??????????????" << endl;
			continue;
		}
	}


}

int process(SOCKET _sock)
{
	char recvBuffer[4096];
	DataHeader* header;

	//5.???????????
	int nLen = recv(_sock, recvBuffer, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		cout << "????????" << endl;
		return -1;
	}
	header = (DataHeader*)recvBuffer;
	cout << "??????? cmd:" << header->cmd << " dataLength:" << header->dataLength << endl;
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* loginResult=NULL;
		recv(_sock, recvBuffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		loginResult = (LoginResult*)recvBuffer;
		cout << "???????logoutResult: " << loginResult ->result<< endl;

		break;
	}
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* logoutResult=NULL;
		recv(_sock, recvBuffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		logoutResult = (LogoutResult*)recvBuffer;
		cout << "?????logoutResult:" << logoutResult->result << endl;
		break;
	}
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* newUserJoin = NULL;
		recv(_sock, recvBuffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		newUserJoin = (NewUserJoin*)recvBuffer;
		cout << "?????NewUserJoin:" << newUserJoin->sock << endl;

		break;
	}
	case CMD_ERROR:
	{
		break;
	}
	default:
	{
		break;
	}
	}
	return 0;
}
int main()
{
#ifdef _WIN32
WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
  	WSAStartup(ver, &dat);
#endif

	///
	//1.?? socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		cout << "??socket??" << endl;
		return -1;
	}	else
	{
		cout << "??socket??" << endl;
	}

	//2.?????
	sockaddr_in _in;
	_in.sin_family = AF_INET;
	_in.sin_port = htons(4567);
#ifdef _WIN32
	_in.sin_addr.S_un.S_addr = inet_addr("192.168.12.1");
#else
	_in.sin_addr.s_addr=inet_addr("192.168.12.26");
#endif
	if (SOCKET_ERROR == connect(_sock, (sockaddr*)&_in, sizeof(_in)))
	{
		cout << "???????" << endl;
		return -1;
	}
	else
	{
		cout << "???????" << endl;
	}
	thread t1(cmdThread, _sock);
	t1.detach();
//	char cmdBuffer[128];
	while (g_boolExit)
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);

		FD_SET(_sock, &fdReads);
		timeval t = {1,0};
		int ret = select(_sock+1, &fdReads, NULL, NULL,&t);
		if (ret < 0)
		{
			cout<<"select return <0 ???? "<<endl;
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock,&fdReads);
			if (-1 == process(_sock))
			{
				cout<<"select??"<<endl;
				break;
			}
		}

		//cout << "??????????" << endl;


		/*
		Login login;
		strcpy(login.username,"wujiatong");
		strcpy(login.password,"password");
		send(_sock,(const char*)&login,sizeof(Login),0);
		*/
		//		Sleep(2000);
	}

	//4.?????socket
#ifdef _WIN32
		closesocket(_sock);




	WSACleanup();
#else
	close(_sock);
#endif
	cout<<"?????"<<endl;
	return 0;
}
