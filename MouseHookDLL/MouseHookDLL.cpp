// MouseHookDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		instanceHandle = hModule;
		hookHandle = NULL;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//private functions
void ReleaseKey_Board_Dir() {
	if (currentDirection != NONE) {
		INPUT input;
		::ZeroMemory(&input, sizeof(INPUT));
		input.type = INPUT_KEYBOARD;
		input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		input.ki.wScan = moveCodes[currentDirection].ScanCode;
		//Release Currently pressed key/keys
		::SendInput(1, &input, sizeof(INPUT));
		if (moveCodes[currentDirection].diagnol) {
			input.ki.wScan = moveCodes[currentDirection].ScanCode2;
			::SendInput(1, &input, sizeof(INPUT));
		}
		currentDirection = NONE;
	}
}

void Move(Cardinals dir)
{
	if (currentDirection==dir) {
		return;// prevent saying move when already moving in the same dir
	}
	INPUT input[2] = { 0 };
	::ZeroMemory(&input, sizeof(INPUT));//quick way to set all members to zero
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.dwFlags = KEYEVENTF_SCANCODE;

	input[0].ki.wScan = moveCodes[dir].ScanCode; //list of scan codes: http://www.philipstorr.id.au/pcbook/book3/scancode.htm
	ReleaseKey_Board_Dir();

	currentDirection = dir;
	//Send the press
	if (moveCodes[dir].diagnol) {
		input[1].ki.wScan = moveCodes[dir].ScanCode2;
		::SendInput(2, input, sizeof(INPUT));
	}
	else {
		::SendInput(1, input, sizeof(INPUT));
	}
}

void PressKey(KEYS action)
{
	INPUT input = { 0 };
	::ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_KEYBOARD;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	input.ki.wScan = keyCodes[action];
	::SendInput(1, &input, sizeof(INPUT));
}
void ReleaseKey(KEYS action)
{
	INPUT input;
	::ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_KEYBOARD;
	input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	input.ki.wScan = keyCodes[action];
	::SendInput(1, &input, sizeof(INPUT));
}

//Mouse Hook procedure
static LRESULT CALLBACK LowLevelMouseProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	if (nCode >= 0)
	{
		POINT p;
		int deltaX = 0;
		int deltaY = 0;
		char Xsum = 0;
		char Ysum = 0;
		char i = 0;
		WORD Xbtn = 0;
		MSLLHOOKSTRUCT * info = (MSLLHOOKSTRUCT *)lParam;
		bool injectedKey = (info->flags & LLMHF_INJECTED);//bit mask
		if (injectedKey) {
			printf("Injected key detected \n");
			return CallNextHookEx(hookHandle, nCode, wParam, lParam); //only block the real mouse, not the emulated one.
		}
		switch (wParam)
		{
		case WM_MOUSEMOVE:
			if (GetCursorPos(&p)) 
			{
				cursorX = p.x;
				cursorY = p.y;
			}
			deltaX = info->pt.x - cursorX;
			deltaY = info->pt.y - cursorY;
			//printf("dX %d : dY %d\n", deltaX, deltaY);

			//cyclic way of storing recent deltas
			previousXs[XYcycleIndex] = deltaX;
			previousYs[XYcycleIndex] = deltaY;
			XYcycleIndex++;
			if (XYcycleIndex == CALIBRATION_SIZE) XYcycleIndex = 0;

			//average
			for (i = 0; i < CALIBRATION_SIZE;i++) {
				Xsum += previousXs[i];
				Ysum += previousYs[i];
			}
			//printf("Xsum %d : Ysum %d\n", Xsum, Ysum); //debug and think of a way for diagnols

			//select most likly direction
			if (abs(Xsum) > abs(Ysum)) Ysum = 0;
			else if (abs(Xsum) < abs(Ysum)) Xsum = 0;
			else printf("Equal \n");

			//rigt pos , up neg
			if (Xsum>0) {
				printf("Right \n");
				Move(RIGHT);
			}
			else if (Xsum<0) {
				printf("left \n");
				Move(LEFT);
			}
			else if (Ysum<0) {
				printf("up \n");
				Move(UP);
			}
			else if (Ysum>0){
				printf("down \n");
				Move(DOWN);
			}
			else {
				printf("Zero Delta \n"); //may occur in the first few movemnts due to sum being 0
			}
			break;
		case WM_LBUTTONDOWN:
			printf("Left Button Down \n");
			PressKey(JUMP);
			break;
		case WM_LBUTTONUP:
			printf("Left Button Up \n");
			ReleaseKey(JUMP);
			break; 
		case WM_MBUTTONDOWN:
			printf("Middle Button Down \n");
			PressKey(CROUCH);
			break;
		case WM_MBUTTONUP:
			printf("Middle Button Up \n");
			ReleaseKey(CROUCH);
			break;
		case WM_RBUTTONDOWN:
			printf("Right Button Down \n");
			PressKey(WALK);
			break;
		case WM_RBUTTONUP:
			printf("Right Button Up \n");
			ReleaseKey(WALK);
			break;
		case WM_XBUTTONDOWN:
			printf("X Button Down \n");
			Xbtn = HIWORD(info->mouseData);
			if (Xbtn & XBUTTON1) {
				//back btn
				ReleaseKey_Board_Dir();//clear direction key press
			}
			else if (Xbtn & XBUTTON2) {
				// frwd btn
				PressKey(RELOAD);
			}
			break;
		case WM_XBUTTONUP:
			printf("X Button Up \n");
			Xbtn = HIWORD(info->mouseData);
			if (Xbtn & XBUTTON2) {
				// frwd btn
				ReleaseKey(RELOAD);
			}
			break;
		}
	}
	else {
		return CallNextHookEx(hookHandle, nCode, wParam, lParam);
	}
	
	return 1;
}
////////////

BOOL InstallHook(HWND hwndParent) {
	//Check if already hooked
	if (hwndServer != NULL) {
		MessageBox(hwndParent, L"already hooked", L"ERROR", MB_OK);
		return FALSE;
	}
	if (instanceHandle==NULL) {
		MessageBox(hwndParent, L"instanceHandle==NULL", L"ERROR", MB_OK);
		return FALSE;
	}
	//Register MOUSE Hook
	hookHandle = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LowLevelMouseProc, instanceHandle, 0);
	if (hookHandle == NULL) {
		MessageBox(hwndParent, L"hookHandle == NULL", L"ERROR", MB_OK);
		return FALSE;
	}
	hwndServer = hwndParent;
	return TRUE;
}

BOOL UninstallHook() {
	//Check if already unhooked
	if (hookHandle == NULL) {
		return TRUE;
	}

	//If unhook attempt fails, check whether it is because of invalid handle (in that case continue)
	if (!UnhookWindowsHookEx(hookHandle)) {
		DWORD error = GetLastError();
		if (error != ERROR_INVALID_HOOK_HANDLE) {
			return FALSE;
		}
	}
	hwndServer = NULL;
	hookHandle = NULL;
	return TRUE;
}