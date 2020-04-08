#include <stdio.h>
#include <libnet.h>

int main(){
    libnet_t* init_libnet; // 接收初始化libnet的返回值
    const char* device = "eth33";
    char* err_buf;
    //源目的主机的信息配置
    unsigned char sourceMac[6] = {0x00,0x0C,0x29,0x7B,0x7B,0x80};
    unsigned char targetMac[6] = {0x00,0x50,0x56,0xE8,0x86,0x08};
    char* sourceIP = "192.168.124.130";
    char* targetIP = "192.168.124.2";
    //初始化libnet
    if((init_libnet = libnet_init(LIBNET_RAW4,device,err_buf)) == NULL){
        perror("init libnet error！\n");
        exit(0);
    }
    unsigned long netSourceIP = libnet_name2addr4(init_libnet,sourceIP,LIBNET_RESOLVE); //字符串类型的IP转化为顺序网络字节流
    unsigned long netTargetIP = libnet_name2addr4(init_libnet,targetIP,LIBNET_RESOLVE);
    
    int lensOfPackage = 0;//数据包长度
    char data[100]; //要发送的数据
    lensOfPackage = sprintf(data,"%s","Test libnet successfully!\0");
    libnet_ptag_t package;
    //构造udp数据报
    package = libnet_build_udp(8080,8080,8+lensOfPackage,0,(const u_int8_t*)data,lensOfPackage,init_libnet,0);
    int id = 12306;
    //构造IP数据包
    package = libnet_build_ipv4(20+8+lensOfPackage,0,id,0,10,17,0,netSourceIP,netTargetIP,NULL,0,init_libnet,0);
    //构造链路层数据帧
    package = libnet_build_ethernet((u_int8_t *)targetMac,(u_int8_t *)sourceMac,ETHERTYPE_IP,NULL,0,init_libnet,0);

    int res = 0;
    //发送数据包
    if((res = libnet_write(init_libnet))==-1){
        perror("libnet write error!\n");
        exit(0);
    }
    libnet_destroy(init_libnet); // 销毁资源

    printf("send udp successfully!\n");
    return 0;
}