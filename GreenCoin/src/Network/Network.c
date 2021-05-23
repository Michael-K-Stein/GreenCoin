
#include "Network.h"

char P2P_CHANNEL_CONNECTION_INIT_MESSAGE[5] = "Ahoy!";
char CONNECTED_TEST_MESSAGE[10] = "GreenCoin";
char CONNECTED_TEST_MESSAGE_RESPONSE[4] = "MOON";

char TRANSACTION_BROADCAST_MAGIC[4] = { "GCT3" };
char BLOCK_BROADCAST_MAGIC[4] = { "GCB3" };

char BLOCK_REQUEST_MAGIC[4] = { "GCBR" };

char BLOCKCHAIN_QUERY_MAGIC[4] = { "GCBL" };

unsigned int GENERAL_SERVER_PORT = 22110;

unsigned char LOCALHOST_IP[4] = { 127, 0, 0, 1 };

char Network_Nodes_List_File_Path[256] = "Nodes.GCNL";

struct Main_Server_Thread_Params {
	WSADATA * ptr_WSA_Data;
	SOCKET * ptr_Sending_Socket;
	unsigned char * server_addr;
	int ret;
};

struct P2P_Thread_Params {
	WSADATA * ptr_WSA_Data;
	SOCKET * ptr_Sending_Socket;
	SOCKET * client_socket;
};

struct Node_Peer {
	unsigned char ip[4];
	SOCKET * socket;
	Node_Peer * next_node;
};
Node_Peer * Node_List = NULL;

void Copy_Node_To_List(SOCKET * socket, unsigned char * ip) {
	Node_Peer * ptr = Node_List;

	if (ptr->socket == NULL) {
		ptr->socket = (SOCKET *)malloc(sizeof(SOCKET));
		memcpy(ptr->socket, socket, sizeof(SOCKET));
		memcpy(ptr->ip, ip, 4);
	}
	else {
		while (ptr->next_node != NULL) { ptr = ptr->next_node; }

		ptr->next_node = (Node_Peer*)calloc(1, sizeof(Node_Peer));

		Node_Peer * new_node = ptr->next_node;
		new_node->next_node = NULL;
		new_node->socket = (SOCKET *)malloc(sizeof(SOCKET));
		memcpy(new_node->socket, socket, sizeof(SOCKET));
		memcpy(new_node->ip, ip, 4);
	}
}

void Copy_Socket_To_List(SOCKET * socket) {
	unsigned char tmp_ip[4] = { 0 };
	SOCKADDR_IN addr;
	int addr_len = sizeof(addr);
	getpeername(*socket, &addr, &addr_len);

	memcpy(tmp_ip, &addr.sin_addr.S_un, sizeof(tmp_ip));

	Copy_Node_To_List(socket, tmp_ip);
}

error_t Network_Init(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket) {
	WSADATA		WSA_Data;
	SOCKET		Sending_Socket;

	// Server/Reciever address
	SOCKADDR_IN		Server_Addr, This_Sender_Info;

	WSAStartup(MAKEWORD(2, 2), &WSA_Data);
	printf("Client: Winsock DLL status is %s.\n", WSA_Data.szSystemStatus);

	// Create socket to make a client connection
	Sending_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Sending_Socket == INVALID_SOCKET) {
		printf("Client: socket() failed! Error code: %ld\n", WSAGetLastError());

		// Do the clean up
		WSACleanup();

		// Exit with error
		return ERROR_NETWORK_FAILED;
	}
	else {
		printf("Client: socket() successful.\n");
	}

	memcpy(ptr_WSA_Data, &WSA_Data, sizeof(WSADATA));
	memcpy(ptr_Sending_Socket, &Sending_Socket, sizeof(SOCKET));

	Node_List = (Node_Peer*)malloc(sizeof(Node_Peer));
	Node_List->next_node = NULL;
	Node_List->socket = NULL;

	Network_Locate_Nodes(ptr_WSA_Data, ptr_Sending_Socket);

	return ERROR_NETWORK_NONE;
}    

unsigned long Network_Node_Addr_Format(unsigned char * node_addr) {
	return (unsigned long)(node_addr[0] | node_addr[1] << 8 | node_addr[2] << 16 | node_addr[3] << 24);
}

error_t Network_Send_Message_Await_Return(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * node_addr, char * message, int message_len, char ** output, int * output_len) {
	// Server/Reciever address
	SOCKADDR_IN		Server_Addr, This_Sender_Info;

	// Server/Reciever port to connect to
	unsigned int	Port = 10087;
	int Return_Code;

	int Bytes_Sent, nlen;

	// IPv4
	Server_Addr.sin_family = AF_INET;

	// Port no.
	Server_Addr.sin_port = htons(Port);

	// The IP address
	Server_Addr.sin_addr.s_addr = Network_Node_Addr_Format(node_addr);//inet_addr("127.0.0.1");

	Return_Code = connect(*ptr_Sending_Socket, (SOCKADDR*)&Server_Addr, sizeof(Server_Addr));
	if (Return_Code != 0) {
		printf("Client: connect() failed! Error code: %ld\n", WSAGetLastError());
		// Exit with error
		return -1;
	}
	else {
		printf("Client: connect() is OK, got connected...\n");
		printf("Client: Ready for sending and/or receiving data...\n");
	}


	// At this point you can start sending or receiving data on
	// the socket SendingSocket.

	// Some info on the receiver side...
	int ilen = sizeof(Server_Addr);
	getsockname(*ptr_Sending_Socket, (SOCKADDR *)&Server_Addr, &ilen);
	printf("Client: Receiver IP(s) used: %s\n", inet_ntoa(Server_Addr.sin_addr));
	//printf("Client: Receiver IP(s) used: %s\n", inet_ntop(Server_Addr.sin_addr));

	printf("Client: Receiver port used: %d\n", htons(Server_Addr.sin_port));
	// Sends some data to server/receiver...
	Bytes_Sent = send(*ptr_Sending_Socket, message, message_len, 0);

	if (Bytes_Sent == SOCKET_ERROR) {
		printf("Client: send() error %ld.\n", WSAGetLastError());
	}
	else {
		printf("Client: send() is OK - bytes sent: %ld\n", Bytes_Sent);
		// Some info on this sender side...
		// Allocate the required resources
		memset(&This_Sender_Info, 0, sizeof(This_Sender_Info));
		nlen = sizeof(This_Sender_Info);

		getsockname(*ptr_Sending_Socket, (SOCKADDR *)&This_Sender_Info, &nlen);
		//printf("Client: Sender IP(s) used: %s\n", inet_ntop(This_Sender_Info.sin_addr));
		printf("Client: Sender IP(s) used: %s\n", inet_ntoa(This_Sender_Info.sin_addr));
		printf("Client: Sender port used: %d\n", htons(This_Sender_Info.sin_port));
		printf("Client: Those bytes represent: \"%s\"\n", message);

		char buffer[1024] = { 0 };
		int Bytes_Recieved = 0;
		while (!Bytes_Recieved) {
			Bytes_Recieved = recv(*ptr_Sending_Socket, buffer, sizeof(buffer), 0);
		}
		printf("Recieved %d bytes: '%s'\n", Bytes_Recieved, buffer);
	}
}

error_t Network_Query_Node_Nodes_List(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * node_addr) {
	
}

error_t Network_Locate_Nodes(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket) {
	char * buffer;
	int size = Load_File(Network_Nodes_List_File_Path, &buffer);

	if (size > 0) {
		// For each entry
		int ind = 0;
		// 4 bytes for address, 1 byte for status
		unsigned char addr_bytes[5];
		while (ind < size) {
			memcpy(addr_bytes, buffer + ind, 5); ind += 5;
			if (addr_bytes[4] == 0xff) {
				// A node with status 0xff is extremely weak, and therefore will not be checked.
			}
			else {

				Network_TCP_Connect(ptr_WSA_Data, ptr_Sending_Socket, addr_bytes);

			}
		}
	}

	if (size > 0) {
		free(buffer);
	}
}

error_t Network_TCP_Connect(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * node_addr) {
	SOCKADDR_IN my_info;
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_sock == INVALID_SOCKET) {
		printf("Client: socket() failed! Error code: %ld\n", WSAGetLastError());
		// Exit with error
		return ERROR_NETWORK_FAILED;
	}
	else {
		printf("Client: socket() successful.\n");
	}

	// IPv4
	my_info.sin_family = AF_INET;
	// Port no.
	my_info.sin_port = htons(GENERAL_SERVER_PORT);
	// The IP address
	my_info.sin_addr.s_addr = Network_Node_Addr_Format(node_addr);
	char * server_ip = inet_ntoa(my_info.sin_addr);

	int err = connect(my_sock, (SOCKADDR*)&my_info, sizeof(my_info));
	if (err != 0) { printf("Could not connect to server node %s!\n", server_ip); }
	else {
		Copy_Socket_To_List(&my_sock);
	}

	//free(server_ip);
}

DWORD WINAPI Network_Main_Server_Thread(/*Main_Server_Thread_Params*/ void * param) {
	//WSADATA * ptr_wsadata = ((Main_Server_Thread_Params*)params)->ptr_WSA_Data;
	int ret = Network_Main_Server(((Main_Server_Thread_Params*)param)->ptr_WSA_Data, ((Main_Server_Thread_Params*)param)->ptr_Sending_Socket, ((Main_Server_Thread_Params*)param)->server_addr);
	(((Main_Server_Thread_Params*)param)->ret) = ret;
	return 0;
}

error_t Network_Main_Server(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * server_addr) {
	int iResult = 0;
	struct addrinfo * result = NULL;
	
	SOCKADDR_IN		Server_Addr;
	unsigned int	Port = GENERAL_SERVER_PORT;
	// IPv4
	Server_Addr.sin_family = AF_INET;
	// Port no.
	Server_Addr.sin_port = htons(Port);
	// The IP address
	Server_Addr.sin_addr.s_addr = Network_Node_Addr_Format(server_addr);//inet_addr("127.0.0.1");

	iResult = bind(*ptr_Sending_Socket, (SOCKADDR*)&Server_Addr, sizeof(Server_Addr));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	iResult = listen(*ptr_Sending_Socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		return 1;
	}

	while (1) {
		// Accept a client socket
		SOCKET ClientSocket = accept(*ptr_Sending_Socket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			//return 1;
		}

		unsigned char client_ip[4];
		SOCKADDR_IN addr;
		int addr_len = sizeof(addr);
		getpeername(ClientSocket, &addr, &addr_len);
		memcpy(client_ip, &addr.sin_addr.S_un, sizeof(client_ip));



		// Receive until the peer shuts down the connection
		char recvbuf[32768] = { 0 };
		int recvbuflen = sizeof(recvbuf);
		int iSendResult = 0;
		do {
			memset(recvbuf, 0, recvbuflen);
			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				printf("Bytes received: %d\n", iResult);
				printf("Recieved: '%s'\n", recvbuf);

				if (strcmp(recvbuf, P2P_CHANNEL_CONNECTION_INIT_MESSAGE) == 0) {
					// The client has sent the P2P_CHANNEL_CONNECTION_INIT_MESSAGE,
					//  meaning they want to begin communicating over P2P.
					P2P_Thread_Params param;
					param.ptr_WSA_Data = ptr_WSA_Data;
					param.ptr_Sending_Socket = ptr_Sending_Socket;
					param.client_socket = &ClientSocket;
					HANDLE thread = CreateThread(0,0,Network_P2P_Thread, &param,0,0);
				}
				else if (strcmp(recvbuf, CONNECTED_TEST_MESSAGE) == 0) {
					// The client has queried the server with the connection test message.
					// Return the protocol-wide response to confirm that the server is valid.
					iSendResult = send(ClientSocket, CONNECTED_TEST_MESSAGE_RESPONSE, sizeof(CONNECTED_TEST_MESSAGE_RESPONSE), 0);
					if (iSendResult == SOCKET_ERROR) {
						printf("Failed to respond %s to querying client! Error code: %d\n", CONNECTED_TEST_MESSAGE_RESPONSE, WSAGetLastError());
						closesocket(ClientSocket);
						return 1;
					}
				}
				else if (memcmp(recvbuf, TRANSACTION_BROADCAST_MAGIC, sizeof(TRANSACTION_BROADCAST_MAGIC)) == 0) {
					Network_Transaction_Recieved(ptr_WSA_Data, ptr_Sending_Socket, recvbuf + sizeof(TRANSACTION_BROADCAST_MAGIC), iResult - sizeof(TRANSACTION_BROADCAST_MAGIC));
					Print_Transaction(stderr, recvbuf + sizeof(TRANSACTION_BROADCAST_MAGIC));
				}
				else if (memcmp(recvbuf, BLOCK_BROADCAST_MAGIC, sizeof(BLOCK_BROADCAST_MAGIC)) == 0) {
					Network_Block_Recieved(ptr_WSA_Data, ptr_Sending_Socket, recvbuf + sizeof(BLOCK_BROADCAST_MAGIC), iResult - sizeof(BLOCK_BROADCAST_MAGIC));
					//Print_Block(stderr, recvbuf + sizeof(BLOCK_BROADCAST_MAGIC));
				}
				else if (memcmp(recvbuf, BLOCK_REQUEST_MAGIC, sizeof(BLOCK_REQUEST_MAGIC)) == 0) {
					Network_Block_Request(ptr_WSA_Data, ptr_Sending_Socket, recvbuf + sizeof(BLOCK_REQUEST_MAGIC), iResult - sizeof(BLOCK_REQUEST_MAGIC), client_ip);
				}
			}
			else if (iResult == 0) {
				printf("Connection closing...\n");
			}
			else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				//return 1;
			}

		} while (iResult > 0);

		// shutdown the connection since we're done
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			//return 1;
		}
	}

	return ERROR_NETWORK_NONE;
}
DWORD WINAPI Network_P2P_Thread(void * param) {
	return Network_P2P(((P2P_Thread_Params*)param)->ptr_WSA_Data, ((P2P_Thread_Params*)param)->ptr_Sending_Socket, ((P2P_Thread_Params*)param)->client_socket);
}
error_t Network_P2P(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, SOCKET * ptr_Peer_Socket) {
	SOCKADDR_IN client_info;
	int client_info_len = sizeof(client_info);
	getpeername(*ptr_Peer_Socket, &client_info, &client_info_len);

	char * peer_ip_address = inet_ntoa(client_info.sin_addr);

	printf("Client '%s' has requested to open a P2P channel!\n", peer_ip_address);

	SOCKADDR_IN my_info;
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (my_sock == INVALID_SOCKET) {
		printf("Client: socket() failed! Error code: %ld\n", WSAGetLastError());
		// Exit with error
		return ERROR_NETWORK_FAILED;
	}
	else {
		printf("Client: socket() successful.\n");
	}

	// IPv4
	my_info.sin_family = AF_INET;
	// Port no.
	my_info.sin_port = htons(GENERAL_SERVER_PORT);
	// The IP address
	memcpy(&my_info.sin_addr.s_addr, &client_info.sin_addr, sizeof(client_info.sin_addr));

	int err = connect(my_sock, (SOCKADDR*)&my_info, sizeof(my_info));
	if (err != 0) { printf("Could not connect to peer %s!\n", peer_ip_address); }
	else {
		Copy_Socket_To_List(&my_sock);
	}

	return ERROR_NONE;
}

error_t Network_Broadcast_Transaction(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * transaction, int transaction_size) {
	char * message = malloc(transaction_size + 4);
	memcpy(message, "GCT3", 4);
	memcpy(message + 4, transaction, transaction_size);
	
	if (Node_List->socket == NULL) {
		printf("No nodes to broadcast to!\n");
		return ERROR_FAILED;
	}

	Node_Peer * ptr = Node_List;
	do {
		SOCKADDR_IN client_info;
		int client_info_len = sizeof(client_info);
		getpeername(*(ptr->socket), &client_info, &client_info_len);

		char * peer_ip_address = inet_ntoa(client_info.sin_addr);

		int res = send(*(ptr->socket), message, transaction_size + 4, 0);
		if (res != transaction_size + 4) {
			printf("Error broadcasting transaction to %s\n", peer_ip_address);
		}
		else {
			printf("Broadcasted the transaction to %s\n", peer_ip_address);
		}

		//free(peer_ip_address);
	} while (ptr->next_node != NULL);

	free(message);

	return ERROR_NONE;
}

error_t Network_Broadcast_Block(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * block, int block_size) {
	char * message = malloc(block_size + 4);
	memcpy(message, "GCB3", 4);
	memcpy(message + 4, block, block_size);

	if (Node_List->socket == NULL) {
		printf("No nodes to broadcast to!\n");
		free(message);
		return ERROR_FAILED;
	}

	Node_Peer * ptr = Node_List;
	do {
		SOCKADDR_IN client_info;
		int client_info_len = sizeof(client_info);
		getpeername(*(ptr->socket), &client_info, &client_info_len);

		char * peer_ip_address = inet_ntoa(client_info.sin_addr);

		int res = send(*(ptr->socket), message, block_size + 4, 0);
		if (res != block_size + 4) {
			printf("Error broadcasting transaction to %s\n", peer_ip_address);
		}
		else {
			printf("Broadcasted the transaction to %s\n", peer_ip_address);
		}
		ptr = ptr->next_node;
		//free(peer_ip_address);
	} while (ptr != NULL && ptr->next_node != NULL);

	free(message);

	return ERROR_NONE;
}

error_t Network_Transaction_Recieved(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * transaction, int transaction_size) {
	return Append_Transaction(ptr_WSA_Data, ptr_Sending_Socket, live_block, transaction);
}

error_t Network_Block_Recieved(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * block, int block_size) {
	return Verify_Block(ptr_WSA_Data, ptr_Sending_Socket, block, block_size);
}

error_t Network_Block_Request(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * data, int data_size, unsigned char * client_ip) {
	uint64_t block_ind = -1;
	if (data_size != sizeof(block_ind)) {
		return ERROR_FAILED;
	}

	memcpy(&block_ind, data, data_size);

	FILE * f;
	if (Open_Block_File(&f, block_ind) != ERROR_NONE) {
		return ERROR_FAILED;
	}

	_Block b;
	fseek(f, sizeof(BLOCK_BROADCAST_MAGIC), SEEK_SET);
	int read = fread(&b, sizeof(b), 1, f);
	if (read != 1) {
		return ERROR_FAILED;
	}

	char * message = malloc(sizeof(b) + 4);
	memcpy(message, "GCB3", 4);
	memcpy(message + 4, &b, sizeof(b));

	int res = Network_Send_To_Peer(ptr_WSA_Data, ptr_Sending_Socket, message, sizeof(b) + 4, client_ip);

	//int res = send(*ptr_Sending_Socket, message, sizeof(b) + 4, 0);
	if (res != sizeof(b) + 4) {
		free(message);
		return ERROR_FAILED;
	}

	free(message);
	return ERROR_NONE;
}

error_t Network_Request_Block(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, uint64_t block_ind) {
	char * message = (char*)malloc(sizeof(BLOCK_REQUEST_MAGIC) + sizeof(block_ind));
	memcpy(message, BLOCK_REQUEST_MAGIC, sizeof(BLOCK_REQUEST_MAGIC));
	memcpy(message + sizeof(BLOCK_REQUEST_MAGIC), &block_ind, sizeof(block_ind));

	Node_Peer * node = Node_List;
	do {
		if (node->socket != NULL) {
			send(*(node->socket), message, sizeof(BLOCK_REQUEST_MAGIC) + sizeof(block_ind), 0);
			node = node->next_node;
		}
	} while (node != NULL && node->next_node != NULL);
}

int Network_Send_To_Peer(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * data, int data_size, unsigned char * client_ip) {
	Node_Peer * node = Node_List;
	int ires = 0;
	do {
		if (node->socket != NULL) {
			if (memcmp(node->ip, client_ip, 4) == 0) {
				// Found peer!
				ires = send(*(node->socket), data, data_size, 0);
				if (ires == data_size){
					return ires;
				}
			}
			node = node->next_node;
		}
	} while (node != NULL && node->next_node != NULL);
	return ires;
}

HANDLE Network_Demo(WSADATA * wsadata, SOCKET * socket) {

	wsadata = (WSADATA *)malloc(sizeof(WSADATA));
	socket = (SOCKET *)malloc(sizeof(SOCKET));

	char * message = "Test123";
	int message_len = strlen(message);

	char * output; int output_len;

	printf("Initializing server on ip: %hhu.%hhu.%hhu.%hhu\n", LOCALHOST_IP[0], LOCALHOST_IP[1], LOCALHOST_IP[2], LOCALHOST_IP[3]);

	Network_Init(wsadata, socket);

	//Network_Main_Server(&wsadata, &socket, node_addr);

	int a = 0;

	Main_Server_Thread_Params * param = malloc(sizeof(Main_Server_Thread_Params));
	param->ptr_WSA_Data = wsadata;
	param->ptr_Sending_Socket = socket;
	param->server_addr = LOCALHOST_IP;
	param->ret = a;

	DWORD thread_id;
	//HANDLE thread = CreateThread(NULL, 0, fun, NULL, 0, &thread_id);
	HANDLE thread = CreateThread(NULL, 0, Network_Main_Server_Thread, param, 0, &thread_id);

	char AHOY[5] = "Ahoy!";

	Node_Peer * node = Node_List;
	do {
		if (node->socket != NULL) {
			send(*(node->socket), AHOY, sizeof(AHOY), 0);
			node = node->next_node;
		}
	} while (node != NULL && node->next_node != NULL);

	/*node = Node_List;
	uint64_t block_chain_length = 0;
	while (block_chain_length == 0 && node->next_node != NULL && node->socket != NULL) {
		send(*(node->socket), BLOCKCHAIN_QUERY_MAGIC, sizeof(BLOCKCHAIN_QUERY_MAGIC), 0);
		uint64_t block_chain_length_tmp = 0;
		recv(*(node->socket), &block_chain_length_tmp, sizeof(block_chain_length_tmp), 0);
		block_chain_length = max(block_chain_length, block_chain_length_tmp);
	}*/

	uint64_t ind = 0;
	while (Block_Index_Exists(ind)) { ind++; }

	// Request future blocks
	while (ind == 0 || Block_Index_Exists(fmax(0, ind - 5))) {
		Network_Request_Block(wsadata, socket, ind++);
		Sleep(100);
	}

	/*if (thread) {
		// Optionally do stuff, such as wait on the thread.
		printf("Waiting...\n");
		while (params.ret == 0) {
			WaitForSingleObject(thread, 1000 * 20);
		}
		printf("Done waiting!\n");
	}*/
	
	//Network_Send_Message_Await_Return(&wsadata, &socket, node_addr, message, message_len, &output, &output_len);

	return thread;//Network_TCP_Handshake();
}