/*
Winodwsϵͳ�ڴ��д���
�����������޸Ľ����ڴ�����
ʹ��Visual Studio���뿪��
       ���ֱ�������֧��
           ��Դ   By-XE
----------���ܽ���----------
1.���������ڴ�

2.����ģ���ڴ�

3.����������ƫ��

4.ƫ���޸�����

5.��ȡ��ַ

6.��ȡָ��

----------��������----------

SetProcess("������")//����Ҫ�޸ĵĽ���

MemorySearchAll_DWORD(��ֵ);//����ȫ���ڴ�DWORD��������(����ֵ��������)

MemorySearchAll_FLOAT(��ֵ);//����ȫ���ڴ�FLOAT��������(����ֵ��������)

MemorySearch_DWORD(��ֵ, "ģ����");//����ģ���е�DWORD��������

MemorySearch_FLOAT(��ֵ, "ģ����");//����ģ���е�FLOAT��������

MemoryOffset_DWORD(������,ƫ����);//ƫ����������������DWORD���͵�������

MemoryOffset_FLOAT(������,ƫ����);//ƫ����������������FLOAT���͵�������

MemoryWrite_DWORD(�޸���ֵ,ƫ����);//�޸�����������ƫ�ƺ������DWORD����

MemoryWrite_FLOAT(�޸���ֵ,ƫ����);//�޸�����������ƫ�ƺ������FLOAT����

ReadAddress_DWORD(��ַ);//��ȡ��ַ�е�ֵDWORD����

ReadAddress_FLOAT(��ַ);//��ȡ��ַ�е�ֵFLOAT����

WriteAddress_DWORD(�޸���ֵ, ��ַ);//�޸ĵ�ַ�е�ֵDWORD����

WriteAddress_FLOAT(�޸���ֵ, ��ַ);//�޸ĵ�ַ�е�ֵFLOAT����

GetModuleBase("ģ����");//��ȡģ��Ļ�ַ

lsp64(��ַ);//��ȡ��ַ�е�64λָ��

lsp32(��ַ);//��ȡ��ַ�е�32λָ��

GetAddr(n);��ȡ�����n�����ݵĵ�ַ

PrintRes()//��ӡ���н����ַ

ClearRes();//�����ǰ�����������;

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
        printf("��ȡpidʧ��");
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
                    //pNew->next=NULL;//����βΪ��
                    pEnd -> next = pNew;
                    pEnd = pNew;
                }
            }
        }
    }
    //pNew->next=NULL;//����βΪ��
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
                    //pNew->next=NULL;//����βΪ��
                    pEnd -> next = pNew;
                    pEnd = pNew;
                }
            }
        }
    }
    //pNew->next=NULL;//����βΪ��
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
                //pNew->next=NULL;//����βΪ��
                pEnd->next = pNew;
                pEnd = pNew;
            }
        }
    }
    //pNew->next=NULL;//����βΪ��
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
                //pNew->next=NULL;//����βΪ��
                pEnd -> next = pNew;
                pEnd = pNew;
            }
        }
    }
    //pNew->next=NULL;//����βΪ��
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
            //pNew->next=NULL;//����βΪ��
            pEnd -> next = pNew;
            pEnd = pNew;
        }
    }
    //pNew->next=NULL;//����βΪ��
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
            //pNew->next=NULL;//����βΪ��
            pEnd -> next = pNew;
            pEnd = pNew;
        }
    }
    //pNew->next=NULL;//����βΪ��
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