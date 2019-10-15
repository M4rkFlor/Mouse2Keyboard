#pragma once


#ifdef MOUSEHOOKDLL_EXPORTS
#define MOUSEHOOKDLL_API __declspec(dllexport)
#else
#define MOUSEHOOKDLL_API __declspec(dllimport)
#endif
enum Cardinals { NONE, UP, UPLEFT, LEFT, DOWNLEFT, DOWN, DOWNRIGHT, RIGHT, UPRIGHT};
enum KEYS { RELOAD, JUMP, CROUCH, WALK };
static WORD keyCodes[4] = { 0x13, 0x39, 0x2A, 0x1D }; //r,space,shift,ctrl
struct moveData{
	Cardinals cDir;//not used can delete
	WORD ScanCode;
	WORD ScanCode2;
	bool diagnol;
};
static moveData moveCodes[9] = { {NONE,0,0}, {UP,0x11,0}, {UPLEFT,0x11,0x1E,true}, {LEFT,0x1E,0},
{DOWNLEFT,0x1E,0x1F,true}, {DOWN,0x1F,0}, {DOWNRIGHT,0x1F,0x20,true}, {RIGHT,0x20,0}, {UPRIGHT,0x20,0x11,true} };
static UINT const WM_HOOK = WM_APP + 1;
static HWND hwndServer = NULL;
static HINSTANCE instanceHandle = NULL;
static HHOOK hookHandle = NULL;
static POINT prevPos;
static Cardinals currentDirection = NONE;
const unsigned short CALIBRATION_SIZE = 7;//being odd increases accuracy
static char XYcycleIndex = 0;
static short previousXs[CALIBRATION_SIZE]; // I hope u don't have this many
static short previousYs[CALIBRATION_SIZE];
static LONG cursorX;
static LONG cursorY;
extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
extern "C" MOUSEHOOKDLL_API BOOL InstallHook(HWND hwndParent);
extern "C" MOUSEHOOKDLL_API BOOL UninstallHook();
extern "C" MOUSEHOOKDLL_API void Move(Cardinals dir);
extern "C" MOUSEHOOKDLL_API void ReleaseKey_Board_Dir();
extern "C" MOUSEHOOKDLL_API void PressKey(KEYS action);
extern "C" MOUSEHOOKDLL_API void ReleaseKey(KEYS action);