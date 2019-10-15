#include "WinMouseHookManager.h"

#include <Windows.h>

#include <iostream>
#pragma comment(lib, "MouseHookDLL.lib")
#include "MouseHookDLL.h"
WinMouseHookManager::WinMouseHookManager()
{
}

void WinMouseHookManager::ForceOnTop()
{
	HWND hwnd = GetActiveWindow();
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WinMouseHookManager::MakeMessage(const char * msg)
{
	HWND hwnd = GetActiveWindow();
	MessageBox(hwnd, msg, "ERROR", MB_OK);
}

void WinMouseHookManager::CreateHook()
{
	//Register MOUSE Hook
	HWND hwnd = GetActiveWindow();
	bool succses = InstallHook(hwnd);
	if (succses) {
		std::cout << "Sucsessfull hook INSTALL" << std::endl;
	}
	else {
		std::cout << "ERROR hook FAILED" << std::endl;
	}
}

void WinMouseHookManager::DestroyHook()
{
	UninstallHook();
}

void WinMouseHookManager::Move(Directions dir)
{
	::Move((Cardinals)dir);
}

void WinMouseHookManager::ReleaseKey_Board_Dir()
{
	::ReleaseKey_Board_Dir();
}

void WinMouseHookManager::PressKey(KEYS key)
{
	::PressKey((::KEYS)key);
}

void WinMouseHookManager::ReleaseKey(KEYS key)
{
	::ReleaseKey((::KEYS)key);
}

WinMouseHookManager::~WinMouseHookManager()
{
	UninstallHook();//probaly do not need this
}
