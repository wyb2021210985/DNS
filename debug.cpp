#include"debug.h"
extern SYSTEMTIME TimeOfSys;
extern IDTransformTable IDTable[1 << 16];
extern struct DNSRelay DNSTable[1500];
extern int seq = 0;
void Debug_DNSHead(DNSHead* a) {
	printf("id:%d\n", a->id);
	printf("qr:%d\n", a->qr);
	printf("opcode:%d\n", a->opcode);
	printf("aa:%d\n", a->aa);
	printf("tc:%d\n", a->tc);
	printf("rd:%d\n", a->rd);
	printf("ra:%d\n", a->ra);
	printf("z:%d\n", a->z);
	printf("ad:%d\n", a->ad);
	printf("cd:%d\n", a->cd);
	printf("rcode:%d\n", a->rcode);
	printf("qdcount:%d\n", ntohs(a->qdcount));
    printf("ancount:%d\n", ntohs (a->ancount));
	printf("nscount:%d\n", ntohs(a->nscount));
	printf("arcount:%d\n",ntohs(a->arcount));	
}
void Debug_Cache(CacheNode* CacheHead) {
	if (CacheHead) {
		printf("CacheList:\n");
		for (CacheNode* temp = CacheHead->prior; temp != CacheHead; temp = temp->prior) {
			char IPString[16];
			inet_ntop(AF_INET, &(temp->ip), IPString, 16) ;
			printf("IP:%s--> <--", IPString);
			printf("DomainName:%s\n", temp->DomainName);
		}
	}
}
void printMessage(int level, SOCKADDR_IN clientAddr, char* recvBuf)
{
    //调试等级为0，无任何输出
    if (level == 0);
    //调试等级为1
    else if (level == 1) {
        GetLocalTime(&TimeOfSys);//获取系统时间

        /*
        int Btime, Ltime;
        Btime = ((((TimeOfSys.wDay - Day) * 24 + TimeOfSys.wHour - Hour) * 60 + TimeOfSys.wMinute) * 60) + TimeOfSys.wSecond - Second;
        Ltime = abs(TimeOfSys.wMilliseconds - Milliseconds);
        printf("% d. % d ", Btime, Ltime);
        */
        printf("time: % d: % d : % d\n", TimeOfSys.wHour, TimeOfSys.wMinute, TimeOfSys.wSecond);
        char* tempBuff = getDomainName(recvBuf);
        printf("\nThe requested domain name is: %s\n",tempBuff );
        char IPString[16];
        inet_ntop(AF_INET, &(clientAddr.sin_addr.s_addr), IPString, 16);
        printf("client IP:%s\n", IPString);
        printf("Seq:%d\n", seq);
        seq++;
    }
    //调试等级为2
    else if (level == 2) {
        GetLocalTime(&TimeOfSys);//获取系统时间
        printf("time: % d: % d : % d\n", TimeOfSys.wHour, TimeOfSys.wMinute, TimeOfSys.wSecond);

        char* tempBuff = getDomainName(recvBuf);
        printf("The requested domain name is: %s\n", tempBuff);
        char IPString[16];
        inet_ntop(AF_INET, &(clientAddr.sin_addr.s_addr), IPString, 16);
        printf("client IP:%s\n", IPString);
        printf("Seq:%d\n", seq);
        seq++;
        printf("DNSHeadByte:\n");
        for (int i = 0; i < sizeof(DNSHead); i++) {
            if (i % 8 == 0 && i != 0) {
                printf("\n");
            }           
            printf("0x%02x ", (unsigned char)recvBuf[i]);
           
        }
        printf("\n");
        DNSHead* head = (DNSHead*)recvBuf;
        Debug_DNSHead(head);
    }
}

