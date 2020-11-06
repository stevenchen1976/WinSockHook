#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>
#include <windows.h>
#include "detours.h"
#include <iostream>

#pragma comment(lib,"Ws2_32.lib") 
#pragma comment( lib, "detours.lib" )
#pragma comment( lib, "detoured.lib" )
#pragma comment( lib, "Mswsock.lib" )
#pragma comment(lib, "detours.lib")


using namespace std;


int (WINAPI* Real_Send)(SOCKET s, const char* buf, int len, int flags) = send;
int (WINAPI* Real_Recv)(SOCKET s, char* buf, int len, int flags) = recv;
int (WINAPI* Real_RecvFrom)(SOCKET s, char* buf, int len, int flags, sockaddr* from, int* fromlen) = recvfrom;
int (WINAPI* Real_WSARecvEx)(SOCKET s, char* buf, int len, int* flags) = WSARecvEx;

int WINAPI Mine_Send(SOCKET s, const char* buf, int len, int flags);
int WINAPI Mine_Recv(SOCKET s, char* buf, int len, int flags);
int WINAPI Mine_RecvFrom(SOCKET s, char* buf, int len, int flags, sockaddr* from, int* fromlen);
int WINAPI Mine_WSARecvEx(SOCKET s, char* buf, int len, int* flags);

int WINAPI Mine_Send(SOCKET s, const char* buf, int len, int flags) {
    
    cout << "Packet Send: " << buf << endl;
    return Real_Send(s, buf, len, flags);
}

int WINAPI Mine_Recv(SOCKET s, char* buf, int len, int flags) {
    
    cout << "Packet Recv: " << buf << endl;
    return Real_Recv(s, buf, len, flags);
}

int WINAPI Mine_RecvFrom(SOCKET s, char* buf, int len, int flags, sockaddr* from, int* fromlen) {
    
    cout << "Packet RecvFrom: " << buf << endl;
    return Real_RecvFrom(s, buf, len, flags, from, fromlen);
}

int WINAPI Mine_WSARecvEx(SOCKET s, char* buf, int len, int* flags) {
    
    cout << "Packet RecvEx: " << buf << endl;
    return Real_WSARecvEx(s, buf, len, flags);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD dwReason, LPVOID) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen("CONOUT$", "w", stdout);


        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)Real_Send, Mine_Send);
        DetourAttach(&(PVOID&)Real_Recv, Mine_Recv);
        DetourAttach(&(PVOID&)Real_RecvFrom, Mine_RecvFrom);
        DetourAttach(&(PVOID&)Real_WSARecvEx, Mine_WSARecvEx);
        DetourTransactionCommit();

        break;

    case DLL_PROCESS_DETACH:

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)Real_Send, Mine_Send);
        DetourDetach(&(PVOID&)Real_Recv, Mine_Recv);
        DetourDetach(&(PVOID&)Real_RecvFrom, Mine_RecvFrom);
        DetourDetach(&(PVOID&)Real_WSARecvEx, Mine_WSARecvEx);
        DetourTransactionCommit();
    }
}