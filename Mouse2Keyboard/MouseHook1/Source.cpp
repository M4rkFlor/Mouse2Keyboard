#include <windows.h>
#include <WinUser.h>
#include "WinMouseHookManager.h"
#include <iostream>
using namespace std;

int main()
{
	int x = 0;
	
	WinMouseHookManager WMHM;
	WMHM.ForceOnTop();
	WMHM.CreateHook();
	MSG msg;
	while(x==0){
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			cout << "message recived" << endl;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	WMHM.DestroyHook();
	return 0;
}