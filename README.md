
在调试模式的设置中我们首先用三位二进制数来判断接下来各部分是否有输入，如果对应二进制数为1则表明对应部分有输入，反之则表明为默认值。  
第一个输入作为调试级别判断，当无输入时级别为0，此时无输出；输入为-d时级别为1，仅输出时间坐标，序号，客户端IP地址，查询的域名；  
输入为-dd时级别为2，输出详细调试信息。第二位读入后将DNS服务器的IP值改为相应IP值，第三位读入后将域名-IP对照表的路径修改为读入的路径。
例如输入dnsrelay 100 -d 表示调试信息级别为1，DNS服务器IP，IP——域名对照表的路径均为默认。   
dnsrelay 111 -d 10.3.9.45 C:/ComputerNetwork/DNSTable.txt 
表示调式信息级别为1，指定的DNS服务器IP地址为10.3.9.45，域名-IP对照表文件路径为C:/ComputerNetwork/DNSTable.txt  
数据结构:  
域名和IP组成的一个二元组：  
typedef struct DNSRelay  
{  
	char* IP;          //IP  
	char* domin;       //域名  
	int length;        //表的长度  
}DNSRelay;  
   
id转换表，用来记录一个中继请求 ：  
typedef struct IDTransform    
{
	unsigned short ID;    //ID   
	SOCKADDR_IN client;   //发送该请求的客户端      
	bool ifDone;         //该请求是否完成    
	ime_t received_time;   //接收请求到的时间   
}IDTransformTable;
资源记录：  
typedef struct RR {         
	unsigned short NAME;  
	unsigned short CLASS;  
	unsigned long TTL;  
	unsigned short RDLENGTH;  
	unsigned long RDATA;  
}RR;  
DNS报头：  
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
Cache的节点：    
typedef struct CacheNode {     
	unsigned long ip;    //IP地址  
	char* DomainName;   //域名
	time_t releaseTime;  //释放时间  
	struct CacheNode* next; //后继  
	struct CacheNode* prior; //前驱  
}CacheNode;  


