

#ifndef DNS_Work_STRUCT_H
#define DNS_Work_STRUCT_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>

#define NOTFOUND -1
#define CONTINUE  -2
#define BREAK  -3

typedef struct DNSRelay{
    char* IP;
    char* domin;
    int length;
}DNSRelay;


typedef struct IDTransform{
    unsigned short ID;
    SOCKADDR_IN client;
    bool ifDone;
    time_t received_time;
}IDTransformTable;
typedef struct RR {         
    unsigned short NAME;
    unsigned short TYPE;
    unsigned short CLASS;
    unsigned long TTL;
    unsigned short RDLENGTH;
    unsigned long RDATA;
}RR;
typedef struct HEADER {
    unsigned id : 16;    /* query identification number */
    unsigned rd : 1;     /* recursion desired */
    unsigned tc : 1;     /* truncated message */
    unsigned aa : 1;     /* authoritive answer */
    unsigned opcode : 4; /* purpose of message */
    unsigned qr : 1;     /* response flag */
    unsigned rcode : 4;  /* response code */
    unsigned cd : 1;     /* checking disabled by resolver */
    unsigned ad : 1;     /* authentic data from named */
    unsigned z: 1;      /* unused bits, must be ZERO */
    unsigned ra: 1;     /* recursion available */
    unsigned short qdcount;       /* number of question entries */
    unsigned short ancount;       /* number of answer entries */
    unsigned short nscount;       /* number of authority entries */
    unsigned short arcount;       /* number of resource entries */
}DNSHead,*DNShead;


#endif //DNS_Work_STRUCT_H
