

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>

#else

#include <my_message.pb.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#include <google/protobuf/message_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#endif

#include <iostream>
#include <thread>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#include "EasyTcpClient.hpp"

union bl {
    unsigned char b[ sizeof( long )];
    long l;
};
void cmdThread(EasyTcpClient *client1)
{
    char cmdBuffer[128];
    while (true)
    {





        //3.
        cin >> cmdBuffer;
        //4.
        if (0 == strcmp(cmdBuffer, "exit"))
        {
            cout << " 输入 exit 即将退出" << endl;
            client1->closeSocket();
            break;

        }
        else if (0 == strcmp(cmdBuffer, "login"))
        {
            Login login;
            strcpy(login.username, "wu");
            strcpy(login.password, "123456");
            client1->sendData(&login);


        }
        else if (0 == strcmp(cmdBuffer, "logout"))
        {
            Logout logout;
            strcpy(logout.username, "wu");

            client1->sendData(&logout);
        }
        else
        {
            cout << "不支持的命令" << endl;
            continue;
        }
    }


}
void gen_random(char *s, const int len) {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}
void sendDataToServerTest(EasyTcpClient& client1){
    tutorial::Data data{};
    tutorial::Open* open=new tutorial::Open;
    time_t t;
    srand((unsigned) time(&t));
    int lenname= rand() % 500+1;
    char namearray[lenname];
    gen_random(namearray,lenname);
   // std::cout<<lenname<<std::endl;
    std::string name(namearray);
    open->set_name(name);
    open->set_email("13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com");
    tutorial::Data_DataType dataType= tutorial::Data_DataType_OPEN;
    data.set_data_type(dataType);
    data.set_allocated_opendata(open);

    int len = data.ByteSize();
    long totalLength=8+len;
    char buffer[len];
    data.SerializeToArray(buffer,len);


    char prebyte[8];
    int wei=56;
    for(int i=7;i>=0;i--)
    {
        long long temp=totalLength<<wei;//左移到56-64位，将比它高的位丢失
        prebyte[i]=temp>>56;//右移到1-8位，将比它低的位丢失
        wei-=8;
    }


    char buffer2[totalLength];
    buffer2[0]=prebyte[0];
    buffer2[1]=prebyte[1];
    buffer2[2]=prebyte[2];
    buffer2[3]=prebyte[3];
    buffer2[4]=prebyte[4];
    buffer2[5]=prebyte[5];
    buffer2[6]=prebyte[6];
    buffer2[7]=prebyte[7];
    // convert from an unsigned long int to a 4-byte array
    // buffer2[4] = (int)((totalLength >> 24) & 0xFF) ;
    //buffer2[5] = (int)((totalLength >> 16) & 0xFF) ;
    // buffer2[6] = (int)((totalLength >> 8) & 0XFF);
    // buffer2[7] = (int)((totalLength & 0XFF));
    for(int i=0;i<len;i++){
        buffer2[i+8]=buffer[i];
    }



    // google::protobuf::io::ArrayOutputStream arrayOut(buffer, len);
    //google::protobuf::io::CodedOutputStream codedOut(&arrayOut);
    // codedOut.WriteVarint32(data.ByteSize());
    // data.SerializeToCodedStream(&codedOut);
    client1.sendData2(buffer2,totalLength);

}
int main()
{
    EasyTcpClient client1;
    client1.connectServer("127.0.0.1",8888);

    // EasyTcpClient client2;
    //client2.connectServer("192.168.12.1",4568);

    // thread t1(cmdThread, &client1);
    //t1.detach();


    //  thread t2(cmdThread, &client2);
    //t2.detach();
    //Logout logout;
    // strcpy(logout.username, "wu");

 //   while(1==1){
        tutorial::Data data{};
        tutorial::Open* open=new tutorial::Open;
        time_t t;
        srand((unsigned) time(&t));
        int lenname= rand() % 500+1;
        char namearray[lenname];
        gen_random(namearray,lenname);
        std::cout<<lenname<<std::endl;
        std::string name(namearray);
        open->set_name(name);
        open->set_email("13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com13333@qq.com");
        tutorial::Data_DataType dataType= tutorial::Data_DataType_OPEN;
        data.set_data_type(dataType);
        data.set_allocated_opendata(open);

        int len = data.ByteSize();
        long totalLength=8+len;
        char buffer[len];
        data.SerializeToArray(buffer,len);


        char prebyte[8];
        int wei=56;
        for(int i=7;i>=0;i--)
        {
            long long temp=totalLength<<wei;//左移到56-64位，将比它高的位丢失
            prebyte[i]=temp>>56;//右移到1-8位，将比它低的位丢失
            wei-=8;
        }


        char buffer2[totalLength];
        buffer2[0]=prebyte[0];
        buffer2[1]=prebyte[1];
        buffer2[2]=prebyte[2];
        buffer2[3]=prebyte[3];
        buffer2[4]=prebyte[4];
        buffer2[5]=prebyte[5];
        buffer2[6]=prebyte[6];
        buffer2[7]=prebyte[7];
        // convert from an unsigned long int to a 4-byte array
        // buffer2[4] = (int)((totalLength >> 24) & 0xFF) ;
        //buffer2[5] = (int)((totalLength >> 16) & 0xFF) ;
        // buffer2[6] = (int)((totalLength >> 8) & 0XFF);
        // buffer2[7] = (int)((totalLength & 0XFF));
        for(int i=0;i<len;i++){
            buffer2[i+8]=buffer[i];
        }



        // google::protobuf::io::ArrayOutputStream arrayOut(buffer, len);
        //google::protobuf::io::CodedOutputStream codedOut(&arrayOut);
        // codedOut.WriteVarint32(data.ByteSize());
        // data.SerializeToCodedStream(&codedOut);
        client1.sendData2(buffer2,totalLength);
    //}


    while (client1.isRun())
    {
        sendDataToServerTest(client1);
        // client1.sendData(&logout);
        //client2.sendData(&logout);
        client1.onRun_protobuf();
        // std::cout<<"onRun()"<<std::endl;
        // client1.sendData(&logout);
        //client2.onRun();
    }

    client1.closeSocket();
    //client2.closeSocket();
    return 0;
}

