#include <stdio.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")
#define SourcePort 2000
#define TargetPort 5000 //���� һǧ
#define SourceIP "192.168.1.8" //localhost
#define TargetIP "192.168.124.130" //������ip

#define MaxLength 100
//ָ�����ݱ���ʽ
typedef struct Datagram
{
	int sourcePort;
	int targetPort;
	char sourceIP[32];
	char targetIP[32];
	char data[MaxLength];
}*datagram;

int main(int argc, char* argv[]) // argc���������ܵĲ�������
{

	WSADATA s; // �����������AfxSocketInitȫ�ֺ������ص�Windows Sockets��ʼ����Ϣ
	SOCKET ClientSocket;

	struct sockaddr_in ClientAddr; // һ��sockaddr_in�͵Ľṹ�����
	int ret = 0;
	char SendBuffer[MAX_PATH];   // Windows��MAX_PATHĬ����260
	char nouse;
	// ��ʼ��Windows Socket
	// WSAStartup������Winsock����ĳ�ʼ��
	if (WSAStartup(MAKEWORD(2, 2), &s) != 0) // ͨ�����������������޷��Ų���,�׸�����Ϊ���ֽ�
	{
		printf("Init Windows Socket Failed! Error: %d\n", GetLastError());
		nouse=getchar();
		return -1;
	}

	// ����һ���׽���
	// �������һ���׽�����connect()��һ��ָ���˿�����
	// �����send()��recv()��ö˿ڽ������ݱ��ķ��������
	// ���Ự�����󣬵���closesocket()
	ClientSocket = socket(AF_INET, // ֻ֧��ARPA Internet��ַ��ʽ
		SOCK_STREAM, // TCP
		IPPROTO_TCP); // �׽������õ�Э��

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("Create Socket Failed! Error: %d\n", GetLastError());
		nouse=getchar();
		return -1;
	}

	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_addr.s_addr = inet_addr(TargetIP); // ����IP��ַ
	ClientAddr.sin_port = htons(TargetPort); // ���������޷��Ŷ�������ת���������ֽ�˳��
	memset(ClientAddr.sin_zero, 0, 8); // ����ͨ��Ϊ��������ڴ�����ʼ������

	// ����Socket
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
		//�������ݱ�
		datagram datagram_Index = (datagram)malloc(sizeof(struct Datagram));
			
		//д������
		strcpy(datagram_Index->sourceIP,SourceIP);
		datagram_Index->sourcePort = SourcePort;
		strcpy(datagram_Index->targetIP,TargetIP);
		datagram_Index->targetPort = TargetPort;
		strcpy((datagram_Index->data),"what");//data

			
		sprintf(SendBuffer, "%d\n%d\n%s\n%s\n%s\n", datagram_Index->sourcePort, datagram_Index->targetPort, datagram_Index->sourceIP, datagram_Index->targetIP, datagram_Index->data);
		printf("\nDatagram:\n%s",SendBuffer);
		// ����������������
		ret = send(ClientSocket,SendBuffer,(int)strlen(SendBuffer), 0);// ���ط��ͻ��������ݳ���


		if (ret == SOCKET_ERROR)
		{
			printf("Send Information Failed! Error:%d\n", GetLastError());
			nouse= getchar();
			break;
		}

		break;
	}

	// �ر�socket
	closesocket(ClientSocket);
	//if (SendBuffer[0] == 'q') // �趨�����һ���ַ�Ϊqʱ�˳�
	//{
	//	printf("Quit!\n");
	//}

	WSACleanup();
	nouse = getchar();
	return 0;
}