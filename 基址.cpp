#include "MemoryTool.h"

int main() {
	//hidecmd();//����cmd����
	SetProcess("game.exe");//���ý���Ϊgame.exe
	std::cout << "����id: " << pid << std::endl;
	uint64_t ModuleAddr = GetModuleBase("game.dll");
	printf("ģ���ַΪ%llx\n", ModuleAddr);
	uint64_t addr = lsp64(lsp64(ModuleAddr + 0x2D2749C) + 0x3e8) + 0x88;
	printf("�޸ĵ�ַΪ%llx\n", addr);
	WriteAddress_DWORD(-1, addr);
	return 0;
}