/*
Winodws系统内存读写插件
用于搜索和修改进程内存数据
使用Visual Studio编译开发
       部分编译器不支持
           开源   By-XE
----------功能介绍----------
1.搜索所有内存

2.搜索模块内存

3.数据特征码偏移

4.偏移修改数据

5.获取基址

6.获取指针

----------函数介绍----------

SetProcess("进程名")//设置要修改的进程

MemorySearchAll_DWORD(数值);//搜索全部内存DWORD类型数据(部分值搜索不到)

MemorySearchAll_FLOAT(数值);//搜索全部内存FLOAT类型数据(部分值搜索不到)

MemorySearch_DWORD(数值, "模块名");//搜索模块中的DWORD类型数据

MemorySearch_FLOAT(数值, "模块名");//搜索模块中的FLOAT类型数据

MemoryOffset_DWORD(特征码,偏移量);//偏移链表中所有数据DWORD类型的特征码

MemoryOffset_FLOAT(特征码,偏移量);//偏移链表中所有数据FLOAT类型的特征码

MemoryWrite_DWORD(修改数值,偏移量);//修改链表中所有偏移后的数据DWORD类型

MemoryWrite_FLOAT(修改数值,偏移量);//修改链表中所有偏移后的数据FLOAT类型

ReadAddress_DWORD(地址);//获取地址中的值DWORD类型

ReadAddress_FLOAT(地址);//获取地址中的值FLOAT类型

WriteAddress_DWORD(修改数值, 地址);//修改地址中的值DWORD类型

WriteAddress_FLOAT(修改数值, 地址);//修改地址中的值FLOAT类型

GetModuleBase("模块名");//获取模块的基址

lsp64(地址);//获取地址中的64位指针

lsp32(地址);//获取地址中的32位指针

GetAddr(n);获取链表第n个数据的地址

PrintRes()//打印所有结果地址

ClearRes();//清楚当前搜索结果链表;

*/
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>


#pragma GCC optimize("Ofast",3,"inline")


struct RES
{
    uint64_t addr;
    struct RES* next;
};


typedef struct RES RES;
typedef struct RES * RESS;


HANDLE handle;
uint32_t pid;
RESS Res;
uint32_t ResCount;



const char * convertToChar(const wchar_t * wstr)
{
    static char buffer[256];
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, buffer, sizeof(buffer), NULL, NULL);
    return buffer;
}
void hidecmd()
{
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
    return;
}
uint32_t GetPID(const char * processName)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, & entry))
    {
        do
        {
            if (_stricmp(convertToChar(entry.szExeFile), processName) == 0)
            {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return 0;
}
void SetProcess(const char * processName)
{
    pid = GetPID(processName);
    if (pid == 0)
    {
        printf("获取pid失败");
        exit(1);
    }
    handle = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ, FALSE, pid);
    return;
}
void MemorySearchAll_DWORD(int32_t value)
{
    uint64_t StartAddr = 0;
    uint64_t EndAddr = 0x4ffff000;
    uint64_t c = EndAddr / 4096;
    int32_t buff[1024] = {0};
    uint32_t iCount = 0;
    RESS pHand, pNew, pEnd;
    pHand = pEnd = (RESS) malloc(sizeof(RES));
    for (int j = 0; j < c; j++)
    {
        if (ReadProcessMemory(handle, (LPVOID)(StartAddr + j * 4096), & buff, 0x1000, NULL))
        {
            for (int i = 0; i < 1024; i++)
            {
                if (buff[i] == value)
                {
                    iCount++;
                    pNew = (RESS)malloc(sizeof(RES));
                    pNew -> addr = StartAddr + j * 4096 + i * 4;
                    //pNew->next=NULL;//链表尾为空
                    pEnd -> next = pNew;
                    pEnd = pNew;
                }
            }
        }
    }
    //pNew->next=NULL;//链表尾为空
    ResCount = iCount;
    Res = pHand;
    return;
}
void MemorySearchAll_FLOAT(float value)
{
    uint64_t StartAddr = 0;
    uint64_t EndAddr = 0x4ffff000;
    uint64_t c = EndAddr / 4096;
    float buff[1024] ={0};
    uint32_t iCount = 0;
    RESS pHand, pNew, pEnd;
    pHand = pEnd = (RESS) malloc(sizeof(RES));
    for (int j = 0; j < c; j++)
    {
        if (ReadProcessMemory(handle, (LPVOID)(StartAddr + j * 4096), & buff, 0x1000, NULL))
        {
            for (int i = 0; i < 1024; i++)
            {
                if (buff[i] == value)
                {
                    iCount++;
                    pNew = (RESS) malloc(sizeof(RES));
                    pNew -> addr = StartAddr + j * 4096 + i * 4;
                    //pNew->next=NULL;//链表尾为空
                    pEnd -> next = pNew;
                    pEnd = pNew;
                }
            }
        }
    }
    //pNew->next=NULL;//链表尾为空
    ResCount = iCount;
    Res = pHand;
    return;
}
void MemorySearch_DWORD(int32_t value, const char* ModuleName)
{
    uint64_t StartAddr;
    uint64_t EndAddr;
    uint64_t c;
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (Module32First(snapshot, &entry))
    {
        do
        {
            if (_stricmp(convertToChar(entry.szModule), ModuleName) == 0)
            {
                StartAddr = (uint64_t)entry.modBaseAddr;
                EndAddr = StartAddr + (uint64_t)entry.modBaseSize;
                c = (uint64_t)entry.modBaseSize / 4096;
                break;
            }
        } while (Module32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    int32_t buff[1024] = { 0 };
    uint32_t iCount = 0;
    RESS pHand, pNew, pEnd;
    pHand = pEnd = (RESS)malloc(sizeof(RES));
    for (int j = 0; j < c; j++)
    {
        ReadProcessMemory(handle, (LPVOID)(StartAddr + j * 4096), &buff, 0x1000, NULL);
        for (int i = 0; i < 1024; i++)
        {
            if (buff[i] == value)
            {
                iCount++;
                pNew = (RESS)malloc(sizeof(RES));
                pNew->addr = StartAddr + j * 4096 + i * 4;
                //pNew->next=NULL;//链表尾为空
                pEnd->next = pNew;
                pEnd = pNew;
            }
        }
    }
    //pNew->next=NULL;//链表尾为空
    ResCount = iCount;
    Res = pHand;
    return;
}
void MemorySearch_FLOAT(float value, const char * ModuleName)
{
    uint64_t StartAddr;
    uint64_t EndAddr;
    uint64_t c;
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (Module32First(snapshot, & entry))
    {
        do 
        {
            if (_stricmp(convertToChar(entry.szModule), ModuleName) == 0)
            {
                StartAddr = (uint64_t)entry.modBaseAddr;
                EndAddr = StartAddr + (uint64_t)entry.modBaseSize;
                c = (uint64_t)entry.modBaseSize / 4096;
                break;
            }
        } while (Module32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    float buff[1024] = {0};
    uint32_t iCount = 0;
    RESS pHand, pNew, pEnd;
    pHand = pEnd = (RESS) malloc(sizeof(RES));
    for (int j = 0; j < c; j++)
    {
        ReadProcessMemory(handle, (LPVOID)(StartAddr + j * 4096), & buff, 0x1000, NULL);
        for (int i = 0; i < 1024; i++)
        {
            if (buff[i] == value)
            {
                iCount++;
                pNew = (RESS)malloc(sizeof(RES));
                pNew -> addr = StartAddr + j * 4096 + i * 4;
                //pNew->next=NULL;//链表尾为空
                pEnd -> next = pNew;
                pEnd = pNew;
            }
        }
    }
    //pNew->next=NULL;//链表尾为空
    ResCount = iCount;
    Res = pHand;
    return;
}
void ClearRes()
{
    RESS pBuff = Res->next;
    RESS pTemp;
    for (uint32_t i = 0; i < ResCount; i++)
    {
        pTemp = pBuff;
        pBuff = pBuff->next;
        free(pTemp);
    }
    free(Res);
    return;
}
void MemoryOffset_DWORD(int32_t value, int32_t offset)
{
    RESS pBuff = Res;
    RESS pHand, pNew, pEnd;
    pHand = pEnd = (RESS)malloc(sizeof(RES));
    uint32_t iCount = 0;
    int32_t buf;
    for (uint32_t i = 0; i < ResCount; i++)
    {
        pBuff = pBuff -> next;
        ReadProcessMemory(handle, (LPVOID)(pBuff -> addr + offset), & buf, 4, NULL);
        if (buf == value)
        {
            iCount++;
            pNew = (RESS)malloc(sizeof(RES));
            pNew -> addr = pBuff -> addr;
            //pNew->next=NULL;//链表尾为空
            pEnd -> next = pNew;
            pEnd = pNew;
        }
    }
    //pNew->next=NULL;//链表尾为空
    ClearRes();
    ResCount = iCount;
    Res = pHand;
    return;
}
void MemoryOffset_FLOAT(float value, int32_t offset)
{
    RESS pBuff = Res;
    RESS pHand, pNew, pEnd;
    pHand = pEnd = (RESS)malloc(sizeof(RES));
    uint32_t iCount = 0;
    float buf;
    for (uint32_t i = 0; i < ResCount; i++)
    {
        pBuff = pBuff -> next;
        ReadProcessMemory(handle, (LPVOID)(pBuff -> addr + offset), & buf, 4, NULL);
        if (buf == value)
        {
            iCount++;
            pNew = (RESS)malloc(sizeof(RES));
            pNew -> addr = pBuff -> addr;
            //pNew->next=NULL;//链表尾为空
            pEnd -> next = pNew;
            pEnd = pNew;
        }
    }
    //pNew->next=NULL;//链表尾为空
    ClearRes();
    ResCount = iCount;
    Res = pHand;
    return;
}
void MemoryWrite_DWORD(int32_t value, int32_t offset)
{
    RESS pBuff = Res;
    for (uint32_t i = 0; i < ResCount; i++)
    {
        pBuff = pBuff->next;
        WriteProcessMemory(handle, (LPVOID)(pBuff->addr + offset), &value, 4, NULL);
    }
    return;
}

void MemoryWrite_FLOAT(float value, int32_t offset)
{
    RESS pBuff = Res;
    for (uint32_t i = 0; i < ResCount; i++)
    {
        pBuff = pBuff->next;
        WriteProcessMemory(handle, (LPVOID)(pBuff->addr + offset), &value, 4, NULL);
    }
    return;
}
void PrintRes()
{
    RESS pBuff = Res;
    for (uint32_t i = 0; i < ResCount; i++)
    {
        pBuff = pBuff -> next;
        printf("%llx\n", i, pBuff -> addr);
    }
    return;
}
int32_t ReadAddress_DWORD(uint64_t addr)
{
    int32_t value;
    ReadProcessMemory(handle, (LPCVOID) addr, & value, 4, NULL);
    return value;
}
int32_t ReadAddress_FLOAT(uint64_t addr)
{
    float value;
    ReadProcessMemory(handle, (LPCVOID) addr, & value, 4, NULL);
    return value;
}
void WriteAddress_DWORD(int32_t value, uint64_t addr)
{
    WriteProcessMemory(handle, (LPVOID)addr, &value, 4, NULL);
}
void WriteAddress_FLOAT(float value, uint64_t addr)
{
    WriteProcessMemory(handle, (LPVOID)addr, &value, 4, NULL);
}
uint64_t GetModuleBase(const char * ModuleName)
{
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (Module32First(snapshot, & entry))
    {
        do {
            if (_stricmp(convertToChar(entry.szModule), ModuleName) == 0)
            {
                CloseHandle(snapshot);
                return (uint64_t)entry.modBaseAddr;
            }
        } while (Module32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return 0;
}
uint64_t GetAddr(uint32_t number)
{
    RESS pBuff = Res;
    for (uint32_t i = 0; i < number; i++) pBuff = pBuff -> next;
    return pBuff->addr;
}
uint64_t lsp64(uint64_t addr)
{
    uint64_t point;
    ReadProcessMemory(handle, (LPCVOID)addr, &point, 8, NULL);
    return point;
}
uint32_t lsp32(uint64_t addr)
{
    uint32_t point;
    ReadProcessMemory(handle, (LPCVOID) addr, & point, 4, NULL);
    return point;
}