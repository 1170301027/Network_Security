
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

//链路层数据包格式 14字节
typedef struct {
    u_char DestMac[6];
    u_char SrcMac[6];
    u_char Etype[2];
}ETHHEADER;
//IP头 20字节
typedef struct {
    int header_len:4; 	//头长
    int version:4;  	//版本
    u_char tos:8; 	//服务类型
    int total_len:16; 	//总长
    int ident:16;	//标识
    int flags:16;	//标志位
    u_char ttl:8;	//生存时间
    u_char proto:8;	//协议
    int checksum:16;	//校验和
    u_char sourceIP[4];	//源ip
    u_char destIP[4];	//目的ip
}IPHEADER;
//TCP头,20字节
// typedef struct {
//     u_int16 sport;     //源端口
//     u_int16 dport;     //目的端口
//     u_int32 seq;       //序列号
//     u_int32 ack;       //确认序号
//     u_int8  head_len;  //头长度
//     u_int8  flags;     //保留和标记位
//     u_int16 wind_size; //窗口大小
//     u_int16 check_sum; //校验和
//     u_int16 urgent_p;  //紧急指针
// }TCPHEADER;
//协议映射表
char *Proto[]={
    "Reserved","ICMP","IGMP","GGP","IP","ST","TCP"
};
char * file; //file Name
//十六进制转换为ASCII码值
unsigned char HexToAsc(unsigned char aChar){
    if((aChar>=0x30)&&(aChar<=0x39))
         aChar -= 0x30; 
    else if((aChar>=0x41)&&(aChar<=0x46))//大写字母
         aChar -= 0x37;
    else if((aChar>=0x61)&&(aChar<=0x66))//小写字母
        aChar -= 0x57;
    else if(aChar==0x0a)
        aChar = '\n';
    else if(aChar==0x2e)
        aChar = '.';
    
    return aChar;
}
//写入文件
void writeTofile(char* file,char* data)
{
	FILE *fp;
	if((fp = fopen(file,"a+")) == NULL)
	{
		fp = fopen(file,"w");
	}
	fprintf(fp,"\nRecieve Datagram:\n");
	// fprintf(fp,"Source IP: %s\n",datagram->sourceIP);
	// fprintf(fp,"Target IP: %s\n",datagram->targetIP);
	// fprintf(fp,"Source Port: %d\n",datagram->sourcePort);
	// fprintf(fp,"Target Port: %d\n",datagram->targetPort);
	fprintf(fp,"%s\n",data);
	fclose(fp);
	return ;
}
//回调函数
void callback(u_char* user,const struct pcap_pkthdr* header,const u_char* pkt_data)
{
    ETHHEADER *eth_header=(ETHHEADER*)pkt_data;
    printf("---------------Begin Analysis-----------------\n");
    printf("----------------------------------------------\n");
    printf("Packet length: %d \n",header->len);
    //解析数据包IP头部
    if(header->len>=14){
        IPHEADER *ip_header=(IPHEADER*)(pkt_data+14);
        //解析协议类型
        char strType[100];
        if(ip_header->proto>7)
            strcpy(strType,"IP/UNKNWN");
        else
            strcpy(strType,Proto[ip_header->proto]);

        printf("Source MAC : %02X-%02X-%02X-%02X-%02X-%02X==>",eth_header->SrcMac[0],eth_header->SrcMac[1],eth_header->SrcMac[2],eth_header->SrcMac[3],eth_header->SrcMac[4],eth_header->SrcMac[5]);
        printf("Dest   MAC : %02X-%02X-%02X-%02X-%02X-%02X\n",eth_header->DestMac[0],eth_header->DestMac[1],eth_header->DestMac[2],eth_header->DestMac[3],eth_header->DestMac[4],eth_header->DestMac[5]);

        printf("Source IP : %d.%d.%d.%d==>",ip_header->sourceIP[0],ip_header->sourceIP[1],ip_header->sourceIP[2],ip_header->sourceIP[3]);
        printf("Dest   IP : %d.%d.%d.%d\n",ip_header->destIP[0],ip_header->destIP[1],ip_header->destIP[2],ip_header->destIP[3]);

        printf("Protocol : %s\n",strType);

        //显示数据帧内容
        int i;
        for(i=14+20+20; i<(int)header->len; ++i)  { //数据报十六进制显示
            printf(" %02x", pkt_data[i]);
            if( (i + 1) % 16 == 0 )
                printf("\n");
        }
        //记录数据报内容
        char buf[100];
        int record = 0;
        for(i=14+20+20; i<(int)header->len; ++i)  { //数据报数据部分ASCII码显示
            char temp = (char)pkt_data[i];
            buf[record++] = temp;
                printf(" %c", (char)pkt_data[i]);
                if( (i + 1) % 16 == 0 )
                    printf("\n");
        }
        buf[record] = '\0';
        char* file = "record.txt"; //写入文件文件名
        writeTofile(file,buf);
        printf("\n\n");
    }
}

#define MaxLength 100

typedef struct Datagram
{
	int sourcePort;
	int targetPort;
	char sourceIP[32];
	char targetIP[32];
	char data[MaxLength];
}*datagram;

int main(int argc, char **argv)
{
    char *device="eth33";//linux下的默认网卡
    char errbuf[1024];
    pcap_t *phandle;

    bpf_u_int32 ipaddress,ipmask;
    struct bpf_program fcode;
    int datalink;
    //查找网络设备
    if((device=pcap_lookupdev(errbuf))==NULL){
        perror(errbuf);
        return 1;
    }
    else
        printf("device: %s\n",device);
    //打开设备
    phandle=pcap_open_live(device,200,0,500,errbuf);

    if(phandle==NULL){
        perror(errbuf);
        return 1;
    }
    //获得网络设备的网络号和掩码
    if(pcap_lookupnet(device,&ipaddress,&ipmask,errbuf)==-1){
        perror(errbuf);
        return 1;
    }
    else{
        char ip[INET_ADDRSTRLEN],mask[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET,&ipaddress,ip,sizeof(ip))==NULL)
            perror("inet_ntop error");
        else if(inet_ntop(AF_INET,&ipmask,mask,sizeof(mask))==NULL)
            perror("inet_ntop error");
        printf("IP address: %s, Network Mask: %s\n",ip,mask);
    }

    int flag=1;
    while(flag){
        //input the design filter
        printf("Input packet Filter: ");
        char filterString[1024];
        scanf("%s",filterString);
        //编译
        if(pcap_compile(phandle,&fcode,filterString,0,ipmask)==-1)
            fprintf(stderr,"pcap_compile: %s,please input again....\n",pcap_geterr(phandle));
        else
            flag=0;
    }
    printf("input the name of record file: ");
    scanf("%s",file);
    //设置网络过滤器
    if(pcap_setfilter(phandle,&fcode)==-1){
        fprintf(stderr,"pcap_setfilter: %s\n",pcap_geterr(phandle));
        return 1;
    }
    //link
    if((datalink=pcap_datalink(phandle))==-1){
        fprintf(stderr,"pcap_datalink: %s\n",pcap_geterr(phandle));
        return 1;
    }

    printf("datalink= %d\n",datalink);
    //循环补包
    pcap_loop(phandle,-1,callback,NULL);

    return 0;
}
