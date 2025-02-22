#ifndef DNS_Work_DEBUG_H
#define DNS_Work_DEBUG_H
#pragma once
#include<stdlib.h>
#include"struct.h"
#include"cache.h"
#include"function.h"
#include <WinSock2.h>
#include<WS2tcpip.h>
void Debug_DNSHead(DNSHead* a);
void Debug_Cache(CacheNode*CacheHead);
void printMessage( int level, SOCKADDR_IN clientAddr, char* recvBuf);//打印相关信息
#endif