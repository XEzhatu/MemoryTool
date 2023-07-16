#include "MemoryTool.h"

int main() {
	//hidecmd();//隐藏cmd窗口
	SetProcess("game.exe");//设置进程为game.exe
	std::cout << "进程id: " << pid << std::endl;
	uint64_t ModuleAddr = GetModuleBase("game.dll");
	printf("模块基址为%llx\n", ModuleAddr);
	uint64_t addr = lsp64(lsp64(ModuleAddr + 0x2D2749C) + 0x3e8) + 0x88;
	printf("修改地址为%llx\n", addr);
	WriteAddress_DWORD(-1, addr);
	return 0;
}