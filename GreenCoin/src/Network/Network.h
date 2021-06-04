#pragma once

#ifndef __NETWORK_H
#define __NETWORK_H

#include "../General/error.h"
#include "../General/FileIO.h"

#include "../BlockChain/BlockChain.h"

#include <string.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
//#include <ws2tcpip.h>
#include <stdio.h>

#include "../General/Print/PrettyPrint.h"

/*
	=== Node Communications Protocol Manual ===

	1) All communication is on TCP.
	2) A connected node will always respond "MOON" {0x4d, 0x4f, 0x4f, 0x4e} to the message "GreenCoin" {0x47, 0x72, 0x65, 0x65, 0x6e, 0x43, 0x6f, 0x69, 0x6e}.
	3) A connected node will volunteer their Node_List as a response to "QueryNetworkNodes".
	4) Start a P2P channel by sending the string "Ahoy!" to a connected node on port 22110.
*/

char P2P_CHANNEL_CONNECTION_INIT_MESSAGE[5];
char CONNECTED_TEST_MESSAGE[10];
char CONNECTED_TEST_MESSAGE_RESPONSE[4];

char TRANSACTION_BROADCAST_MAGIC[4];
char BLOCK_BROADCAST_MAGIC[4];

char BLOCK_REQUEST_MAGIC[4];

char BLOCKCHAIN_QUERY_MAGIC[4];

unsigned int GENERAL_SERVER_PORT;// = 22110;

char Network_Nodes_List_File_Path[256];

unsigned char LOCALHOST_IP[4];

typedef struct Main_Server_Thread_Params Main_Server_Thread_Params;
typedef struct P2P_Thread_Params P2P_Thread_Params;

typedef struct Network_TCP_Connect_Thread_Params Network_TCP_Connect_Thread_Params;

typedef struct Node_Peer Node_Peer;
// A linked list of nodes holding pointers to the sockets for P2P connections
Node_Peer * Node_List;// = NULL;

void Copy_Node_To_List(SOCKET * socket, unsigned char * ip);
void Copy_Socket_To_List(SOCKET * socket);

error_t Network_Init(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket);

error_t Network_Print_IP();

unsigned long Network_Node_Addr_Format(unsigned char * node_addr);

error_t Network_Send_Message_Await_Return(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * node_addr, char * message, int message_len, char ** output, int * output_len);

error_t Network_Query_Node_Nodes_List(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * node_addr);

error_t Network_Locate_Nodes(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket);

error_t Network_TCP_Connect(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * node_addr);
DWORD WINAPI Network_TCP_Connect_Thread(void * params);

DWORD WINAPI Network_Main_Server_Thread(/*Main_Server_Thread_Params*/ void * param);
error_t Network_Main_Server(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, unsigned char * server_addr);

error_t Network_Add_New_Peer_To_Node_File(unsigned char * ip);
DWORD WINAPI Network_P2P_Thread(void * param);
error_t Network_P2P(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, SOCKET * ptr_Peer_Socket);

error_t Network_Broadcast_Transaction(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * transaction, int transaction_size);
error_t Network_Broadcast_Block(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * block, int block_size);

error_t Network_Transaction_Recieved(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * transaction, int transaction_size);
error_t Network_Block_Recieved(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * block, int block_size);

error_t Network_Block_Request(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * data, int data_size, unsigned char * client_ip);

error_t Network_Request_Block(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, uint64_t block_ind);

int Network_Send_To_Peer(WSADATA * ptr_WSA_Data, SOCKET * ptr_Sending_Socket, void * data, int data_size, unsigned char * client_ip);

void Network_CommandLine_Init(WSADATA ** wsadata, SOCKET ** socket);
HANDLE Network_CommandLine_Server(WSADATA * wsadata, SOCKET * socket);

error_t Network_Connect_To_Known_Peers();

void Recursive_Free(struct piece_t * ptr);
uint64_t Network_Request_Blockchain_Length(WSADATA * wsadata, SOCKET * socket);
void Network_Request_Blockchain_Length_Print(WSADATA * wsadata, SOCKET * socket);

error_t Network_CommandLine_Request_Blocks(WSADATA * wsadata, SOCKET * socket);

HANDLE Network_Demo(WSADATA * wsadata, SOCKET * socket);

#endif
