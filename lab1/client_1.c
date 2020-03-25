#include <stdio.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")
#define SourcePort 2000
#define TargetPort 5000 //大于 一千
#define SourceIP "192.168.1.8" //localhost
#define TargetIP "192.168.124.130" //服务器ip

#define MaxLength 100
//指定数据报格式
typedef struct Datagram
{
	int sourcePort;
	int targetPort;
	char sourceIP[32];
	char targetIP[32];
	char data[MaxLength];
}*datagram;

int main(int argc, char* argv[]) // argc是命令行总的参数个数
{

	WSADATA s; // 用来储存调用AfxSocketInit全局函数返回的Windows Sockets初始化信息
	SOCKET ClientSocket;

	struct sockaddr_in ClientAddr; // 一个sockaddr_in型的结构体对象
	int ret = 0;
	char SendBuffer[MAX_PATH];   // Windows的MAX_PATH默认是260
	char nouse;
	// 初始化Windows Socket
	// WSAStartup函数对Winsock服务的初始化
	if (WSAStartup(MAKEWORD(2, 2), &s) != 0) // 通过连接两个给定的无符号参数,首个参数为低字节
	{
		printf("Init Windows Socket Failed! Error: %d\n", GetLastError());
		nouse=getchar();
		return -1;
	}

	// 创建一个套接字
	// 如果这样一个套接字用connect()与一个指定端口连接
	// 则可用send()和recv()与该端口进行数据报的发送与接收
	// 当会话结束后，调用closesocket()
	ClientSocket = socket(AF_INET, // 只支持ARPA Internet地址格式
		SOCK_STREAM, // TCP
		IPPROTO_TCP); // 套接字所用的协议

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("Create Socket Failed! Error: %d\n", GetLastError());
		nouse=getchar();
		return -1;
	}

	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_addr.s_addr = inet_addr(TargetIP); // 定义IP地址
	ClientAddr.sin_port = htons(TargetPort); // 将主机的无符号短整形数转换成网络字节顺序
	memset(ClientAddr.sin_zero, 0, 8); // 函数通常为新申请的内存做初始化工作

	// 连接Socket
	ret = connect(ClientSocket,
		(struct sockaddr*) & ClientAddr,
		sizeof(ClientAddr));
		
	if (ret == SOCKET_ERROR)
	{
		printf("Socket Connect Failed! Error:%d\n", GetLastError());
		nouse=getchar();
		return -1;
	}
	else
	{
		printf("Socket Connect %s Succeed!",TargetIP);
	}

	int flag = 1;
	while (flag)
	{
		//生成数据报
		datagram datagram_Index = (datagram)malloc(sizeof(struct Datagram));
			
		//写入数据
		strcpy(datagram_Index->sourceIP,SourceIP);
		datagram_Index->sourcePort = SourcePort;
		strcpy(datagram_Index->targetIP,TargetIP);
		datagram_Index->targetPort = TargetPort;
		strcpy((datagram_Index->data),"what");//data

			
		sprintf(SendBuffer, "%d\n%d\n%s\n%s\n%s\n", datagram_Index->sourcePort, datagram_Index->targetPort, datagram_Index->sourceIP, datagram_Index->targetIP, datagram_Index->data);
		printf("\nDatagram:\n%s",SendBuffer);
		// 发送数据至服务器
		ret = send(ClientSocket,SendBuffer,(int)strlen(SendBuffer), 0);// 返回发送缓冲区数据长度


		if (ret == SOCKET_ERROR)
		{
			printf("Send Information Failed! Error:%d\n", GetLastError());
			nouse= getchar();
			break;
		}

		break;
	}

	// 关闭socket
	closesocket(ClientSocket);
	//if (SendBuffer[0] == 'q') // 设定输入第一个字符为q时退出
	//{
	//	printf("Quit!\n");
	//}

	WSACleanup();
	nouse = getchar();
	return 0;
}