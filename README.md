# DNS 中继调试模式说明

## 调试模式设置

在调试模式的设置中，我们首先用三位二进制数来判断接下来各部分是否有输入：
- 如果对应二进制数为 `1`，则表明对应部分有输入
- 如果为 `0`，则表明为默认值

### 输入参数说明

1. **第一个输入** - 调试级别判断：
   - 无输入时级别为 `0`：无输出
   - 输入为 `-d` 时级别为 `1`：输出时间坐标、序号、客户端IP地址、查询的域名
   - 输入为 `-dd` 时级别为 `2`：输出详细调试信息

2. **第二位输入**：将 DNS 服务器的 IP 值改为相应 IP 值

3. **第三位输入**：将域名-IP 对照表的路径修改为读入的路径

### 示例

1. `dnsrelay 100 -d`  
   表示：
   - 调试信息级别为 `1`
   - DNS 服务器 IP
   - IP-域名对照表的路径均为默认值

2. `dnsrelay 111 -d 10.3.9.45 C:/ComputerNetwork/DNSTable.txt`  
   表示：
   - 调试信息级别为 `1`
   - 指定的 DNS 服务器 IP 地址为 `10.3.9.45`
   - 域名-IP 对照表文件路径为 `C:/ComputerNetwork/DNSTable.txt`

## 数据结构定义

### 域名和IP组成的二元组

```c
typedef struct DNSRelay {
    char* IP;          // IP  
    char* domin;       // 域名  
    int length;        // 表的长度  
} DNSRelay;

```

### ID转换表

```c
typedef struct IDTransform {
    unsigned short ID;      // ID   
    SOCKADDR_IN client;     // 发送该请求的客户端      
    bool ifDone;            // 该请求是否完成    
    time_t received_time;   // 接收请求到的时间   
} IDTransformTable;

```

### DNS报头

```c
typedef struct HEADER {     
    unsigned id : 16;     /* query identification number */  
    unsigned rd : 1;      /* recursion desired */  
    unsigned tc : 1;      /* truncated message */  
    unsigned aa : 1;      /* authoritive answer */  
    unsigned opcode : 4;  /* purpose of message */  
    unsigned qr : 1;      /* response flag */  
    unsigned rcode : 4;   /* response code */  
    unsigned cd : 1;      /* checking disabled by resolver */  
    unsigned ad : 1;      /* authentic data from named */  
    unsigned z: 1;        /* unused bits, must be ZERO */  
    unsigned ra: 1;       /* recursion available */  
    unsigned short qdcount;  /* number of question entries */  
    unsigned short ancount;  /* number of answer entries */   
    unsigned short nscount;  /* number of authority entries */  
    unsigned short arcount;  /* number of resource entries */  
} DNSHead, *DNShead;

```

### Cache的节点

```c
typedef struct CacheNode {     
    unsigned long ip;    //IP地址  
    char* DomainName;   //域名
    time_t releaseTime;  //释放时间  
    struct CacheNode* next; //后继  
    struct CacheNode* prior; //前驱  
}CacheNode;  

``` 
