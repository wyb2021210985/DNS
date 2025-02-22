#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>
#include <time.h>
#include <process.h>
#include "struct.h"
#include "function.h"
#include "cache.h"
#include <stdint.h>
#include"debug.h"
#pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll
#pragma warning(disable:4996)
#define SERVER_DNS_IP "10.3.9.44"
#define PATH "F:\\学习\\大二下\\计网课设\\DNS\\dnsrelay.txt"
struct DNSRelay DNSTable[1500];
int socketfd;
struct sockaddr_in tempAddr;
SOCKADDR_IN AnyAddr, serverAddr;
IDTransformTable IDTable[1 << 16];
char tempBuff[256];
extern int cachesize;
SYSTEMTIME TimeOfSys;
int Day, Hour, Minute, Second, Milliseconds;
int Seq ;
int main( ){
    char input[100]; // 假设输入的字符串不超过100个字符
    char* argv[5]; // 存储分隔后的字符串
    int count = 0; // 字符串计数器
    int argc=0;
rd:
    printf("请输入一行字符：");
    fgets(input, sizeof(input), stdin); // 从标准输入读取一行字符
    printf("%s \n", input);
    if (input[strlen(input) - 1]=='\n') {
        input[strlen(input) - 1] = '\0';
    }
    // 使用 strtok 函数分割字符串
    char* token = strtok(input, " ");
    while (token != NULL && count < 5) {
        argv[count] = token;
        count++;
        token = strtok(NULL, " ");
    }
    GetLocalTime(&TimeOfSys);
    Day = TimeOfSys.wDay;
    Hour = TimeOfSys.wHour;
    Minute = TimeOfSys.wMinute;
    Milliseconds = TimeOfSys.wMilliseconds;
    char tablePath[512];
    char server_DNS[64];
    int level = 0;
    strcpy(tablePath, PATH);
    strcpy(server_DNS, SERVER_DNS_IP);
    //调试级别0
    char sign[4];
    int temp = 2;
    for (int i = 0; i < 3; i++) {
        sign[i] = argv[1][i];
        if (sign[i] == '1') {
            if (temp >= count) {
                printf("指令格式错误,请重新输入\n");
                goto rd;
            }
            if (i == 0) {
                if (strcmp("-d", argv[temp]) == 0) {
                    level = 1;
                }
                else if (strcmp("-dd", argv[temp]) == 0) {
                    level = 2;
                }
            }
            else if (i == 1) {
                strcpy(server_DNS, argv[temp]);
            }
            else if (i == 2) {
                strcpy(tablePath, argv[temp]);
            }
            temp++;
        }
    }
    initSocket(server_DNS);
    initDNSTable(tablePath);
    printf("TablePath:%s\n", tablePath);
    printf("server_DNS:%s\n", server_DNS);
    printf("Debug_Level:%d\n", level);
    CacheNode* CacheHead=(CacheNode*)malloc(sizeof(CacheNode));
    CacheHead->next = CacheHead;
    CacheHead->prior = CacheHead;
    while (1){
        char receiveBuff[1024];
        memset(receiveBuff, 0, 1024);
        int length = sizeof(tempAddr);
        int byteNum;
        byteNum = recvfrom(socketfd, receiveBuff, sizeof(receiveBuff), 0, (SOCKADDR*)&tempAddr, &length);
        if (byteNum == -1){
            continue;
        }
        else if (byteNum == 0){
            break;
        }
        else{
            RR* rr=(RR*)malloc(sizeof(RR));
            //查询报文
            printMessage(level, tempAddr, receiveBuff);
            if (level == 2) {
                printf("CacheSize:%d\n", cachesize);
                Debug_Cache(CacheHead);
            }
            DNSHead* header = (DNSHead*)receiveBuff;
            if (header->qr == 0)
            {   
                if (level > 0) {
                    printf("收到查询报文\n");
                }
                char* tempBuff = getDomainName(receiveBuff);
                if (FindIPInCache(tempBuff, rr, CacheHead)) {
                    if (level > 0) {
                        printf("在Cache中找到该域名\n");
                    }
                    send_respond_to_client(receiveBuff, byteNum, rr);
                }
                else {
                    int exist;
                    exist = isExist(tempBuff, byteNum);
                    if (exist == NOTFOUND){
                        if (level > 0) {
                            printf("数据库中未找到域名，转至外部DNS\n");
                        }
                        send_to_server(receiveBuff, byteNum,header);
                    }
                    else{
                        rr->RDATA = inet_addr(DNSTable[exist].IP);
                        if (level > 0) {
                            printf("数据库中找到该域名\n");
                        }
                        send_respond_to_client(receiveBuff, byteNum, rr);
                        addCache(rr,CacheHead,DNSTable[exist].domin);
                    }
                }
            }
            //应答报文
            
            else{
                if (level > 0) {
                    printf("收到应答报文 \n");
                }
                int isSend = send_to_client_from_DNS(receiveBuff, byteNum,header);
                if (isSend == -1){
                    if (level > 0) {
                        printf("发送失败\n");
                    }
                }
                else if (isSend == 0){
                    break;
                }
                else{
                    char* tempBuff = getDomainName(receiveBuff);
                    char* pos = receiveBuff + sizeof(DNSHead) + strlen(tempBuff) + 1 + 1;
                    pos = pos + 4;
                    int offset = 0;
                    memcpy(&(rr->NAME),pos + offset, sizeof(unsigned short));
                    offset = offset + sizeof(unsigned short);
                    memcpy(&(rr->TYPE),pos + offset, sizeof(unsigned short));
                    offset = offset + sizeof(unsigned short);
                    memcpy(&(rr->CLASS), pos + offset, sizeof(unsigned short));
                    offset = offset + sizeof(unsigned short);
                    memcpy(&(rr->TTL), pos + offset, sizeof(unsigned long));
                    offset = offset + sizeof(unsigned long);
                    memcpy(&(rr->RDLENGTH), pos + offset, sizeof(unsigned short));
                    offset = offset + sizeof(unsigned short);
                    memcpy(&(rr->RDATA), pos + offset, sizeof(unsigned long));
                    offset = offset + sizeof(unsigned long);
                    addCache(rr, CacheHead, tempBuff);
                    if (level > 0) {
                        printf("转发成功\n");
                    }
                }
            }
            free(rr);
            if (level > 0) {
                printf("\n");
            }
        }
    }
    destroyCache(CacheHead);
    closesocket(socketfd);
    WSACleanup();
}




