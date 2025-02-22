#ifndef DNS_Work_CACHE_H
#define DNS_Work_CACHE_H
#pragma once
#include<time.h>
#include<stdlib.h>
#include"struct.h"
#define MAX_CACHE_SIZE 32

typedef struct CacheNode {
	unsigned long ip;
	char* DomainName;
	time_t releaseTime;
	struct CacheNode* next;
	struct CacheNode* prior;
}CacheNode;
void addCache(RR* rr,CacheNode*CacheHead,char*domainName);
void destroyCache(CacheNode* CacheHead);
void deleteNode(CacheNode* node);
int FindIPInCache(char* domainName, RR* rr,CacheNode*CacheHead);
//Cache中每个节点的ip和rr中的IP都是网络字节顺序
#endif
