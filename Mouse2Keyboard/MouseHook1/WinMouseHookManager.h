#pragma once
class WinMouseHookManager
{

public:
	enum Directions { NONE, UP, UPLEFT, LEFT, DOWNLEFT, DOWN, DOWNRIGHT, RIGHT, UPRIGHT };
	enum KEYS { RELOAD, JUMP, CROUCH, WALK};
	WinMouseHookManager();
	static void ForceOnTop();
	static void MakeMessage(const char* msg);
	static void CreateHook();
	static void DestroyHook();
	static void Move(Directions dir);
	static void ReleaseKey_Board_Dir();
	static void PressKey(KEYS key);
	static void ReleaseKey(KEYS key);
	~WinMouseHookManager();
};