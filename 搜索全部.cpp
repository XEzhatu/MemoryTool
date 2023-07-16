#include "MemoryTool.h"

int main() {
	hidecmd();//隐藏cmd窗口
	SetProcess("game.exe");//设置进程为game.exe
	std::cout << "进程id: " << pid << std::endl;
	MemorySearchAll_DWORD(16384);//搜索进程中所有DWORD类型为16384的值
	printf("搜索到%d个值\n", ResCount);
	MemoryOffset_DWORD(100, 4);//保存所有链表中的偏移4后为100的值
	printf("偏移到%d个值\n", ResCount);
	MemoryOffset_DWORD(200, -4);//保存所有链表中的偏移-4后为200的值
	printf("偏移到%d个值\n", ResCount);
	MemoryOffset_DWORD(300, 0xE0);//保存所有链表中的偏移0xE0后为300的值
	printf("偏移到%d个值\n", ResCount);
	MemoryWrite_DWORD(16385,0);//链表中所有的值偏移0后修改为16385
	printf("修改了%d个值\n", ResCount);
	ClearRes();//清理链表
	return 0;
}