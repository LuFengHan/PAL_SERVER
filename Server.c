#if 0

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

#include <string.h>


#pragma comment(lib,"ws2_32.lib")



#define SERVER_PORT 8888
typedef int socklen_t;


/*
 监听后，一直处于accept阻塞状态，
 直到有客户端连接，
 当客户端如数quit后，断开与客户端的连接
 */

int main()
{
    //调用socket函数返回的文件描述符
    int serverSocket;

    //声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器
    struct sockaddr_in server_addr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);
    int client;
    char buffer[200];
    int iDataNum;

    //socket函数，失败返回-1
    //int socket(int domain, int type, int protocol);
    //第一个参数表示使用的地址类型，一般都是ipv4，AF_INET
    //第二个参数表示套接字类型：tcp：面向连接的稳定数据传输SOCK_STREAM
    //第三个参数设置为0
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }

    //bzero(&server_addr, sizeof(server_addr));
    memset(&server_addr, 0, sizeof(server_addr));

    //初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    //ip可是是本服务器的ip，也可以用宏INADDR_ANY代替，代表0.0.0.0，表明所有地址
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
    //bind三个参数：服务器端的套接字的文件描述符，
    if(bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    //设置服务器上的socket为监听状态
    if(listen(serverSocket, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    while(1)
    {
        printf("监听端口: %d\n", SERVER_PORT);
        //调用accept函数后，会进入阻塞状态
        //accept返回一个套接字的文件描述符，
        //这样服务器端便有两个套接字的文件描述符: serverSocket和client。
        //serverSocket仍然继续在监听状态，client则负责接收和发送数据
        //clientAddr是一个传出参数，accept返回时，传出客户端的地址和端口号
        //addr_len是一个传入-传出参数，传入的是调用者提供的缓冲区的clientAddr的长度，以避免缓冲区溢出。
        //传出的是客户端地址结构体的实际长度。
        //出错返回-1
        client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
        if(client < 0)
        {
            perror("accept");
            continue;
        }
        printf("等待消息...\n");
        //inet_ntoa   ip地址转换函数，将网络字节序IP转换为点分十进制IP
        //表达式：char *inet_ntoa (struct in_addr);
        printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));
        printf("Port is %d\n", htons(clientAddr.sin_port));
        while(1)
        {
            printf("读取消息:");
            buffer[0] = '\0';
            iDataNum = recv(client, buffer, 1024, 0);
            if(iDataNum < 0)
            {
                perror("recv null");
                continue;
            }
            buffer[iDataNum] = '\0';
            if(strcmp(buffer, "quit") == 0)
                break;
            printf("%s\n", buffer);

            printf("发送消息:");
            scanf("%s", buffer);
            printf("\n");

            send(client, buffer, strlen(buffer), 0);

            if(strcmp(buffer, "quit") == 0)
                break;
        }
    }

    closesocket(serverSocket);
    return 0;
}

#else
//#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

#include <string.h>

#include "SerCliInterface.h"



#pragma comment(lib,"ws2_32.lib")


//调用socket函数返回的文件描述符
SOCKET slisten;
SOCKET sClient;

DWORD WINAPI ThreadRcv(LPVOID lpParameter)
{
    //int iDataNum;
    char * sendData = "HELLO, THIS COMES FROM SERVER11111111111111111111111111111111111111111111111111111111111111111111111111111.\n";

    int iDataNum;
    char revData[2048];
    while(1)
    {
        revData[0] = '\0';

        /*从SOCKET读最多2048个字节*/
        iDataNum = recv(sClient, revData, 2048, 0);

        if(iDataNum < 0)
        {   /*! 若发生错误 */
            //printf("recv null");
            continue;
        }

        printf("ThreadRcv, recv length = %d\r\n", iDataNum);

        /*! 增加字符串结束符 */
        revData[iDataNum] = '\0';
        if(iDataNum > 0)
        {
            revData[iDataNum] = 0x00;
            printf(&(revData[8]));
            printf("\r\n");
        }

        //发送数据
        //send(sClient, aucSendMsg, strlen(aucSendMsg), 0);
        //printf("SEND MSG. \r\n");
        //send(sClient, sendData, strlen(sendData), 0);

        if(strcmp(revData, "quit") == 0)
        {   /*! 若CLIENT发来QUITE，退出程序 */
            printf("%s\n", revData);
            break;
        }
    }

    return 0;
}

void sendOnePalMsgId(unsigned long  ulMsgType,
                           unsigned long  ulSn)
{

    /*The first 8 byte are MSG HEADER:
     * -------------------------------------------------------------------
     * bit1                     bit16   |   bit17                   bit32
     * -------------------------------------------------------------------
     *              CARD_ID             |       0X5555
     * -------------------------------------------------------------------
     *                              MSG ID
     * -------------------------------------------------------------------
     *                              MSG CONTENT
     *
     *
     * -------------------------------------------------------------------
     * */
    //OSA_MSG_HEADER_ST  stMsgHeader;
    OSA_MSG_COMMON_ST   stMsg;

    stMsg.ulMsgLen              = sizeof(OSA_MSG_COMMON_ST) - sizeof(stMsg.ulMsgLen);
    stMsg.stMsgHeader.usCardId  = 0x0F0F;
    stMsg.stMsgHeader.usDdrFlag = 0x5555;
    stMsg.stMsgHeader.ulMsgType = ulMsgType;
    stMsg.stMsgHeader.ulSn      = ulSn;
    stMsg.ulRsv                 = 0;


    //发送数据
    send(sClient, (const char *)&stMsg, sizeof(OSA_MSG_COMMON_ST), 0);

}


/*
 监听后，一直处于accept阻塞状态，
 直到有客户端连接，
 当客户端如数quit后，断开与客户端的连接
 */
int main(int argc, char* argv[])
{

    //声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器
    struct sockaddr_in sin;
    struct sockaddr_in remoteAddr;
    int nAddrlen;
    unsigned long ulMsgId;
    unsigned long ulMsgSn;

    char keyboardData[1024];

            /*The first 8 byte are MSG HEADER:
         * -------------------------------------------------------------------
         * bit1                     bit16   |   bit17                   bit32
         * -------------------------------------------------------------------
         *              CARD_ID             |       0X5555
         * -------------------------------------------------------------------
         *                              MSG ID
         * -------------------------------------------------------------------
         *                              MSG CONTENT
         *
         * -------------------------------------------------------------------
         * */
         /*
        bytes[0] = 0;//CARD ID = 0
        bytes[1] = 0;//CARD ID = 0
        bytes[2] = 0X55;//isInDDRFlag = 0X5555
        bytes[3] = 0X55;//isInDDRFlag = 0X5555
        bytes[4] = 0x70;//0x00000070 is POWER_SWEEP_REQ
        bytes[5] = 0x00;
        bytes[6] = 0x00;
        bytes[7] = 0x00; */
    unsigned int  ulPowerSweepLen = 184;
    /*
    char aucSendMsg[1024] =  {
            00, 00, 55, 55, 70, 00, 00, 00,
            00, 00, 00, 00, 01, 00, 01, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
            01, 00, 04, 00, 00, 00};
    */

    char aucSendMsg[1024] =  {
            0x00, 0x00, 0x55, 0x55, 0x70, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x04, 0x00, 0x00, 0x00};

    //int iDataNum;
    char * sendData = "HELLO, THIS is not POWER SWEEP MSG.\n";

    HANDLE HOne;
    //初始化WSA
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;

    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
        return 0;
    }

    //创建套接字
    //第一个参数表示使用的地址类型，一般都是ipv4，AF_INET
    //第二个参数表示套接字类型：tcp：面向连接的稳定数据传输SOCK_STREAM
    //第三个参数设置为0
    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口
    //初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);

    //ip可是是本服务器的ip，也可以用宏INADDR_ANY代替，代表0.0.0.0，表明所有地址
    sin.sin_addr.S_un.S_addr = INADDR_ANY;

    //对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
    //bind三个参数：服务器端的套接字的文件描述符，
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //设置服务器上的socket为监听状态
    //开始监听
    if(listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //循环接收数据
    nAddrlen = sizeof(remoteAddr);

    while(1)
    {
        printf("等待连接...\n");

        //调用accept函数后，会进入阻塞状态
        //accept返回一个套接字的文件描述符，这样服务器端便有两个套接字的文件描述符，
        //serverSocket和client。
        //serverSocket仍然继续在监听状态，client则负责接收和发送数据
        //clientAddr是一个传出参数，accept返回时，传出客户端的地址和端口号
        //addr_len是一个传入-传出参数，传入的是调用者提供的缓冲区的clientAddr的长度，以避免缓冲区溢出。
        //传出的是客户端地址结构体的实际长度。
        //出错返回-1
        sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
        if(sClient == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        else
        {
            printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
            break;
        }
    }

    //inet_ntoa   ip地址转换函数，将网络字节序IP转换为点分十进制IP

    printf("接收消息:");
    HOne=CreateThread(NULL, 0, ThreadRcv, NULL, 0, NULL);

    ulMsgId = 1;
    ulMsgSn = 0;
    while (1)
    {
        #if 1
        while(1)
        {
            #if 1
            printf("PRESS WHAT YOU WANT TO SEND : NUMBERs or BYE");

            /*! 从键盘读取要发送的内容*/
            scanf("%s", keyboardData);
            printf("\n");

            //发送数据

            if(strcmp(keyboardData, "BYE") == 0)
            {
                printf("BYE.");
                break;
            }
            else
            {
                printf("SEND MSG id = %d, sn = %d. \r\n", ulMsgId, ulMsgSn);
                sendOnePalMsgId(ulMsgId, ulMsgSn++);
            }
            #endif
        }

        #else
        //接收数据
        iDataNum = recv(sClient, revData, 255, 0);
        if(iDataNum > 0)
        {
            revData[iDataNum] = 0x00;
            printf(revData);
        }

        //发送数据
        send(sClient, sendData, strlen(sendData), 0);
        closesocket(sClient);
        #endif
    }

    closesocket(slisten);
    WSACleanup();
    return 0;
}

/*!
*******************************************************************************
@FUNCTION       :
@BRIEF          :
@PARAMETER[IN]  : sSocket:通过此SOCKET发送数据
                  ucBuf: 存放待发送的字节流
                  ulBufLen: ucBuf中的有效字节数
*******************************************************************************
*/
#if 0
void PAL_SimuSocketSendOneString(SOCKET sSocket, UINT8 ucBuf[], UINT32 ulBufLen)
{

    /*! 发送的内容*/
    send(sSocket, ucBuf, ulBufLen, 0);
}
#endif
#endif
