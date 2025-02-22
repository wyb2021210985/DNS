#include"cache.h"
int cachesize = 0;
void addCache(RR* rr,CacheNode*CacheHead,char*domainName){
	CacheNode* added = (CacheNode*)malloc(sizeof(CacheNode));
	added->DomainName = (char*)malloc(sizeof(char)*256);
	strcpy(added->DomainName ,domainName);
	added->ip = rr->RDATA;
	added->releaseTime = time(NULL) + ntohl(rr->TTL);
	added->next = CacheHead->next;
	added->prior= CacheHead;
	CacheHead->next->prior = added;
	CacheHead->next = added;
	cachesize++;
	CacheNode* temp=CacheHead->prior;
	while (cachesize > MAX_CACHE_SIZE&&temp!=CacheHead) {
		CacheNode* oldTemp = temp;
		temp = temp->prior;
		deleteNode(oldTemp);
		cachesize--;
	}
}
void destroyCache(CacheNode* CacheHead) {
	if (CacheHead) {
		CacheNode* temp = CacheHead->next;
		while (temp != CacheHead) {
			CacheNode* oldTemp = temp;
			temp = temp->next;
			free(oldTemp);
		}
		free(CacheHead);
	}
}
void deleteNode(CacheNode* node) {
	if (node) {
		node->next->prior = node->prior;
		node->prior->next = node->next;
		free(node);
	}
}
int FindIPInCache(char* domainName, RR* rr,CacheNode*CacheHead) {
	int result = 0;
	CacheNode* temp=CacheHead->next;
	while(temp!=CacheHead) {
		CacheNode* Next=temp->next;
		if (strcmp(temp->DomainName, domainName) == 0) {
			rr->RDATA = temp->ip;
			result = 1;
		}
		if (temp->releaseTime <= time(NULL)) {
			deleteNode(temp);
			cachesize--;
		}
		temp = Next;
	}
	return result;
}
