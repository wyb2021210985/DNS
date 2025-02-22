#ifndef DNS_Work_FUNCTION_H
#define DNS_Work_FUNCTION_H

#pragma once

void initSocket(char*serverIP);//初始化
void initDNSTable(char*filepath);//初始化DNS
int isExist(char* domainName, int receive);//获取域名并判定域名是否存在

//void DNSServer();//开始运行DNS服务器
//void construct();//构建DNS相应报文
int send_to_server(char* receiveBuff, int byteNum,DNSHead*head);
char* getDomainName(char* receiveBuff);
int constructDNS(RR*rr,int byteNum);
int send_to_client_from_DNS(char* receiveBuff, int byteNum,DNSHead*head);
int send_respond_to_client(char* receiveBuff, int byteNum, RR*rr);
//int recvMessage();
#endif


