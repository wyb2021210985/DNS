
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include <process.h>
#include "struct.h"
#include "function.h"
#include "cache.h"
#pragma  comment(lib, "Ws2_32.lib") //加载 ws2_32.dll
#define ANY_IP "0.0.0.0"   
#define SERVER_PORT 53
#define LOCAL_PORT 53
//#define Max_Request_Lasttime 60
extern struct DNSRelay DNSTable[1500];
extern int socketfd;
//extern int serverSocket;
extern struct sockaddr_in tempAddr;
extern SOCKADDR_IN AnyAddr, serverAddr;
extern IDTransformTable IDTable[1<<16];
extern int IDcount = 0;
extern char tempBuff[256];
char sendBuff[1024];



char* getDomainName(char* receiveBuff) {
    memset(tempBuff, 0, 256); //全用0初始化
    int j = 0, i = 12;
    int num;
    while (receiveBuff[i]!='\0'){
        num = receiveBuff[i];
        i++;
        for (int k = 0; k < num; k++)
        {
            tempBuff[j] = receiveBuff[i];
            j++, i++;
        }
        if (num > 0 && receiveBuff[i + 1] > 0)
        {
            tempBuff[j] = '.';
        }
        j++;
        
    }
    tempBuff[j] = '\0';

    return tempBuff;
}
void initSocket(char*serverIp){
    WSADATA wsaData;//声明于_wsadata.h
    WSAStartup(MAKEWORD(2, 2), &wsaData);//运用winsock版本为2.2

    //创建套接字
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&AnyAddr, 0, sizeof(AnyAddr));//置0
    AnyAddr.sin_family = AF_INET;//表示IPv4地址
    AnyAddr.sin_port = htons(LOCAL_PORT);  // 绑定DNS端口号53
    AnyAddr.sin_addr.s_addr = inet_addr(ANY_IP);  // 绑定本地地址

    //绑定serverAddr
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);  // 绑定DNS端口号53
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);


    //设置为非阻塞模式
    //ioctlsocket 当mode值为非零时，将套接字设置为非阻塞
    u_long mode = 0;
    ioctlsocket(socketfd, FIONBIO, &mode);
    //ioctlsocket(serverSocket, FIONBIO, &mode);


    //绑定本地服务器地址
    if (bind(socketfd, (SOCKADDR*)&AnyAddr, sizeof(AnyAddr)) < 0) {
        printf("ERROR: Could not bind: %s\n", strerror(errno));
        exit(-1);
    }
    else{
        printf("Bind success\n");
    }

}

void initDNSTable(char*filepath){
    FILE* file;
    int Length = 0, lineCount = 0;
    file = fopen(filepath, "r");
    char ch, * line[1500];
    //计算总长度
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            Length++;
        }
    }
    file = fopen(filepath, "r");
    for (int i = 0; i < Length; i++){
        line[i] = (char*)malloc(sizeof(char) * 200);
        fgets(line[i], 1500, file);
    }

    //存储倒DNSTable里
    while (lineCount < Length){
        char* tempIP = strtok(line[lineCount], " "); //按空格分割字符串
        // 存储第一个部分
        DNSTable[lineCount].IP = tempIP;
        // 继续获取下一个部分
        char* tempDomin = strtok(NULL, " ");
        DNSTable[lineCount].domin = tempDomin;
        DNSTable[lineCount].domin[strlen(tempDomin) - 1] = '\0';
        // 行数加一
        lineCount++;
    }
    DNSTable->length = Length;
    fclose(file);
}

int isExist(char*domainName, int receive){
    int exist = NOTFOUND;
    for (int i = 0; i < DNSTable->length; i++){
        
        if (strcmp(DNSTable[i].domin, domainName) == 0){
            exist = i;
            break;
        }

    }
    return exist;
}

int send_to_server(char* receiveBuff, int byteNum,DNSHead* head){
    IDTable[IDcount].ID = head->id;
    IDTable[IDcount].client = tempAddr;
    IDTable[IDcount].ifDone = FALSE;
    IDTable[IDcount].received_time = time(NULL);
    unsigned short newID = IDcount;
    IDcount++;
    memcpy(receiveBuff, &newID, 2);
    int isSend = sendto(socketfd, receiveBuff, byteNum, 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    for (int i = 0; i < IDcount; i++) {
        time_t Max_Request_Lasttime = 60;
        if (time(NULL)> IDTable[i].received_time+Max_Request_Lasttime) {
            IDTable[i].ifDone = TRUE;
        }
    }
    return isSend;
}



int send_to_client_from_DNS(char* receiveBuff, int byteNum,DNSHead*head){
    unsigned short* oldID=(unsigned short*)malloc(sizeof(unsigned short));
    *oldID = head->id;
    int isSend = -1;
    if (IDTable[*oldID].ifDone == FALSE) {
        unsigned short newID ;
        newID = IDTable[*oldID].ID;
        memcpy(receiveBuff, &newID, 2);
        IDTable[*oldID].ifDone = TRUE;
        tempAddr = IDTable[*oldID].client; 
        isSend = sendto(socketfd, receiveBuff, byteNum, 0, (SOCKADDR*)&tempAddr, sizeof(tempAddr));
    }
    else {
        isSend = -1;
    }
    free(oldID);
    return isSend;

}

int send_respond_to_client(char* receiveBuff, int byteNum, RR*rr){
    memset(sendBuff, 0, 1024);
    DNSHead* head = (DNSHead*)receiveBuff;
    if (rr->RDATA == 0) {
        head->rcode = 3;
        head->ancount = htons(0x0000);
    }
    else {
        head->rcode = 0;
        head->ancount = htons(0x0001);
    }
    head->qr = 1;
    int offset = 0;
    offset = offset + sizeof(DNSHead);
    char* temp = getDomainName(receiveBuff);
    offset = offset + strlen(temp) + 1 + 1 + 4;
    memcpy(sendBuff, receiveBuff, offset);
    int currentLength = constructDNS(rr, offset);
    //发送
    int isSend;
    isSend = sendto(socketfd, sendBuff, currentLength, 0, (SOCKADDR*)&tempAddr, sizeof(tempAddr));
    return isSend;
}

int constructDNS(RR*rr, int offset){
    int currentLength = offset;
    rr->NAME = htons(0xc00c); //域名指针（偏移量）
    memcpy(sendBuff+currentLength,&(rr->NAME),sizeof(unsigned short));
    currentLength = currentLength + sizeof(unsigned short);
    rr->TYPE= htons(0x0001); //类型
    memcpy(sendBuff + currentLength, &(rr->TYPE), sizeof(unsigned short));
    currentLength = currentLength + sizeof(unsigned short);
    rr->CLASS= htons(0x0001); //查询类
    memcpy(sendBuff + currentLength, &(rr->CLASS), sizeof(unsigned short));
    currentLength = currentLength + sizeof(unsigned short);
    //TTL四字节
    rr->TTL = htonl(0x7b); //生存时间
    memcpy(sendBuff + currentLength, &(rr->TTL), sizeof(unsigned long));
    currentLength = currentLength + sizeof(unsigned long);
    rr->RDLENGTH = htons(0x0004); //资源数据长度
    memcpy(sendBuff + currentLength, &(rr->RDLENGTH), sizeof(unsigned short));
    currentLength = currentLength + sizeof(unsigned short);
    memcpy(sendBuff + currentLength, &(rr->RDATA), sizeof(unsigned long));
    currentLength = currentLength + sizeof(unsigned long);
    return currentLength;
}


