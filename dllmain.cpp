#include "pch.h"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>
#include <windows.h>
#include "detours.h"
#include <iostream>
#include <winsock.h>

#pragma comment(lib,"Ws2_32.lib") 
#pragma comment( lib, "detours.lib" )

using namespace std;


int (WINAPI* Real_Send)(SOCKET s, const char* buf, int len, int flags) = send;
int (WINAPI* Real_Recv)(SOCKET s, char* buf, int len, int flags) = recv;

int WINAPI Mine_Send(SOCKET s, const char* buf, int len, int flags);
int WINAPI Mine_Recv(SOCKET s, char* buf, int len, int flags);

int WINAPI Mine_Send(SOCKET s, const char* buf, int len, int flags) {
    
    cout << "Packet Send: " << buf << endl;
    return Real_Send(s, buf, len, flags);
}

int WINAPI Mine_Recv(SOCKET s, char* buf, int len, int flags) {
    
    cout << "Packet Recv: " << buf << endl;
    return Real_Recv(s, buf, len, flags);
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
        DetourTransactionCommit();

        break;

    case DLL_PROCESS_DETACH:

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)Real_Send, Mine_Send);
        DetourDetach(&(PVOID&)Real_Recv, Mine_Recv);
        DetourTransactionCommit();
    }
}