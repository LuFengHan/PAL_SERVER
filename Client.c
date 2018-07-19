//#include "stdafx.h"
#include "WINSOCK2.H"
#include "STDIO.H"
#include "Winsock2.h"

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#pragma  comment(lib,"ws2_32.lib")



int main(int argc, char* argv[])
{
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data;
    SOCKET sclient;
    struct sockaddr_in serAddr;
    char * sendData = "你好，TCP服务端，我是客户端!\n";
    char recData[255];
    char keyboardData[1024];
    int ret;

    if(WSAStartup(sockVersion, &data) != 0)
    {
        return 0;
    }

    sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
        return 0;
    }

    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if (connect(sclient, (struct sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        printf("connect error !");
        closesocket(sclient);
        return 0;
    }

    while(1)
    {
        printf("发送消息，请从键盘输入要发送的内容:");
        scanf("%s", keyboardData);

        send(sclient, keyboardData, strlen(keyboardData), 0);

        //send(sclient, sendData, strlen(sendData), 0);

        printf("接收消息中:");
        ret = recv(sclient, recData, 255, 0);
        if(ret > 0)
        {
            recData[ret] = 0x00;
            printf(recData);
            return 0;
        }

    }


    #if 0
    send(sclient, sendData, strlen(sendData), 0);

    ret = recv(sclient, recData, 255, 0);
    if(ret > 0)
    {
        recData[ret] = 0x00;
        printf(recData);
    }
    #endif

    closesocket(sclient);
    WSACleanup();
    return 0;
}