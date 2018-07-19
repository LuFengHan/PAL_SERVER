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
 ������һֱ����accept����״̬��
 ֱ���пͻ������ӣ�
 ���ͻ�������quit�󣬶Ͽ���ͻ��˵�����
 */

int main()
{
    //����socket�������ص��ļ�������
    int serverSocket;

    //���������׽���sockaddr_in�ṹ��������ֱ��ʾ�ͻ��˺ͷ�����
    struct sockaddr_in server_addr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);
    int client;
    char buffer[200];
    int iDataNum;

    //socket������ʧ�ܷ���-1
    //int socket(int domain, int type, int protocol);
    //��һ��������ʾʹ�õĵ�ַ���ͣ�һ�㶼��ipv4��AF_INET
    //�ڶ���������ʾ�׽������ͣ�tcp���������ӵ��ȶ����ݴ���SOCK_STREAM
    //��������������Ϊ0
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }

    //bzero(&server_addr, sizeof(server_addr));
    memset(&server_addr, 0, sizeof(server_addr));

    //��ʼ���������˵��׽��֣�����htons��htonl���˿ں͵�ַת�������ֽ���
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    //ip�����Ǳ���������ip��Ҳ�����ú�INADDR_ANY���棬����0.0.0.0���������е�ַ
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //����bind��accept֮��ĺ����������׽��ֲ���������Ҫǿ��ת����(struct sockaddr *)
    //bind�����������������˵��׽��ֵ��ļ���������
    if(bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    //���÷������ϵ�socketΪ����״̬
    if(listen(serverSocket, 5) < 0)
    {
        perror("listen");
        return 1;
    }

    while(1)
    {
        printf("�����˿�: %d\n", SERVER_PORT);
        //����accept�����󣬻��������״̬
        //accept����һ���׽��ֵ��ļ���������
        //�����������˱��������׽��ֵ��ļ�������: serverSocket��client��
        //serverSocket��Ȼ�����ڼ���״̬��client������պͷ�������
        //clientAddr��һ������������accept����ʱ�������ͻ��˵ĵ�ַ�Ͷ˿ں�
        //addr_len��һ������-����������������ǵ������ṩ�Ļ�������clientAddr�ĳ��ȣ��Ա��⻺���������
        //�������ǿͻ��˵�ַ�ṹ���ʵ�ʳ��ȡ�
        //������-1
        client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
        if(client < 0)
        {
            perror("accept");
            continue;
        }
        printf("�ȴ���Ϣ...\n");
        //inet_ntoa   ip��ַת���������������ֽ���IPת��Ϊ���ʮ����IP
        //���ʽ��char *inet_ntoa (struct in_addr);
        printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));
        printf("Port is %d\n", htons(clientAddr.sin_port));
        while(1)
        {
            printf("��ȡ��Ϣ:");
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

            printf("������Ϣ:");
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


//����socket�������ص��ļ�������
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

        /*��SOCKET�����2048���ֽ�*/
        iDataNum = recv(sClient, revData, 2048, 0);

        if(iDataNum < 0)
        {   /*! ���������� */
            //printf("recv null");
            continue;
        }

        printf("ThreadRcv, recv length = %d\r\n", iDataNum);

        /*! �����ַ��������� */
        revData[iDataNum] = '\0';
        if(iDataNum > 0)
        {
            revData[iDataNum] = 0x00;
            printf(&(revData[8]));
            printf("\r\n");
        }

        //��������
        //send(sClient, aucSendMsg, strlen(aucSendMsg), 0);
        //printf("SEND MSG. \r\n");
        //send(sClient, sendData, strlen(sendData), 0);

        if(strcmp(revData, "quit") == 0)
        {   /*! ��CLIENT����QUITE���˳����� */
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


    //��������
    send(sClient, (const char *)&stMsg, sizeof(OSA_MSG_COMMON_ST), 0);

}


/*
 ������һֱ����accept����״̬��
 ֱ���пͻ������ӣ�
 ���ͻ�������quit�󣬶Ͽ���ͻ��˵�����
 */
int main(int argc, char* argv[])
{

    //���������׽���sockaddr_in�ṹ��������ֱ��ʾ�ͻ��˺ͷ�����
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
    //��ʼ��WSA
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;

    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
        return 0;
    }

    //�����׽���
    //��һ��������ʾʹ�õĵ�ַ���ͣ�һ�㶼��ipv4��AF_INET
    //�ڶ���������ʾ�׽������ͣ�tcp���������ӵ��ȶ����ݴ���SOCK_STREAM
    //��������������Ϊ0
    slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //��IP�Ͷ˿�
    //��ʼ���������˵��׽��֣�����htons��htonl���˿ں͵�ַת�������ֽ���
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);

    //ip�����Ǳ���������ip��Ҳ�����ú�INADDR_ANY���棬����0.0.0.0���������е�ַ
    sin.sin_addr.S_un.S_addr = INADDR_ANY;

    //����bind��accept֮��ĺ����������׽��ֲ���������Ҫǿ��ת����(struct sockaddr *)
    //bind�����������������˵��׽��ֵ��ļ���������
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //���÷������ϵ�socketΪ����״̬
    //��ʼ����
    if(listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //ѭ����������
    nAddrlen = sizeof(remoteAddr);

    while(1)
    {
        printf("�ȴ�����...\n");

        //����accept�����󣬻��������״̬
        //accept����һ���׽��ֵ��ļ��������������������˱��������׽��ֵ��ļ���������
        //serverSocket��client��
        //serverSocket��Ȼ�����ڼ���״̬��client������պͷ�������
        //clientAddr��һ������������accept����ʱ�������ͻ��˵ĵ�ַ�Ͷ˿ں�
        //addr_len��һ������-����������������ǵ������ṩ�Ļ�������clientAddr�ĳ��ȣ��Ա��⻺���������
        //�������ǿͻ��˵�ַ�ṹ���ʵ�ʳ��ȡ�
        //������-1
        sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
        if(sClient == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        else
        {
            printf("���ܵ�һ�����ӣ�%s \r\n", inet_ntoa(remoteAddr.sin_addr));
            break;
        }
    }

    //inet_ntoa   ip��ַת���������������ֽ���IPת��Ϊ���ʮ����IP

    printf("������Ϣ:");
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

            /*! �Ӽ��̶�ȡҪ���͵�����*/
            scanf("%s", keyboardData);
            printf("\n");

            //��������

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
        //��������
        iDataNum = recv(sClient, revData, 255, 0);
        if(iDataNum > 0)
        {
            revData[iDataNum] = 0x00;
            printf(revData);
        }

        //��������
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
@PARAMETER[IN]  : sSocket:ͨ����SOCKET��������
                  ucBuf: ��Ŵ����͵��ֽ���
                  ulBufLen: ucBuf�е���Ч�ֽ���
*******************************************************************************
*/
#if 0
void PAL_SimuSocketSendOneString(SOCKET sSocket, UINT8 ucBuf[], UINT32 ulBufLen)
{

    /*! ���͵�����*/
    send(sSocket, ucBuf, ulBufLen, 0);
}
#endif
#endif
