#include "MemoryTool.h"

int main() {
	hidecmd();//����cmd����
	SetProcess("game.exe");//���ý���Ϊgame.exe
	std::cout << "����id: " << pid << std::endl;
	MemorySearchAll_DWORD(16384);//��������������DWORD����Ϊ16384��ֵ
	printf("������%d��ֵ\n", ResCount);
	MemoryOffset_DWORD(100, 4);//�������������е�ƫ��4��Ϊ100��ֵ
	printf("ƫ�Ƶ�%d��ֵ\n", ResCount);
	MemoryOffset_DWORD(200, -4);//�������������е�ƫ��-4��Ϊ200��ֵ
	printf("ƫ�Ƶ�%d��ֵ\n", ResCount);
	MemoryOffset_DWORD(300, 0xE0);//�������������е�ƫ��0xE0��Ϊ300��ֵ
	printf("ƫ�Ƶ�%d��ֵ\n", ResCount);
	MemoryWrite_DWORD(16385,0);//���������е�ֵƫ��0���޸�Ϊ16385
	printf("�޸���%d��ֵ\n", ResCount);
	ClearRes();//��������
	return 0;
}