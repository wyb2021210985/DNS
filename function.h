#ifndef DNS_Work_FUNCTION_H
#define DNS_Work_FUNCTION_H

#pragma once

void initSocket(char*serverIP);//��ʼ��
void initDNSTable(char*filepath);//��ʼ��DNS
int isExist(char* domainName, int receive);//��ȡ�������ж������Ƿ����

//void DNSServer();//��ʼ����DNS������
//void construct();//����DNS��Ӧ����
int send_to_server(char* receiveBuff, int byteNum,DNSHead*head);
char* getDomainName(char* receiveBuff);
int constructDNS(RR*rr,int byteNum);
int send_to_client_from_DNS(char* receiveBuff, int byteNum,DNSHead*head);
int send_respond_to_client(char* receiveBuff, int byteNum, RR*rr);
//int recvMessage();
#endif


