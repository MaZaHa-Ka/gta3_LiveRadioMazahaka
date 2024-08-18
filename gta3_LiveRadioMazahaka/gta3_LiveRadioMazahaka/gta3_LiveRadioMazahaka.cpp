#pragma once
#include "Windows.h"
#include <iostream>
#include <chrono>
#include <string>


#pragma warning(disable: 4244)	// int to float
#pragma warning(disable: 4800)	// int to bool
#pragma warning(disable: 4838)  // narrowing conversion
#pragma warning(disable: 4996)  // POSIX names

#pragma warning(disable: 4018)  // size_t 2 int
#define null NULL

#include "CMenuManager.h"
#include "CTimer.h"
#include "CCheat.h"
#include "GameVersion.h"

// game
#include "CPlayerPed.h"
#include "CMenuManager.h"
#include "CStats.h"
#include "CCamera.h"

#include "CMenuManager.h"
#include "CTimer.h"
#include "CStreaming.h"
#include "CPopulation.h"
#include "CWorld.h"
#include "CVehicle.h"
#include "CPager.h" // ? not used?
#include "CUserDisplay.h"
#include "CFileLoader.h"
#include "CSurfaceTable.h"
#include "CPed.h"
#include "CLines.h"
#include "CWeaponInfo.h"
#include "CTheZones.h"
#include "CCollision.h"
#include "CModelInfo.h"
#include "CGarages.h"
#include "CPedType.h"
#include "CFileMgr.h"
#include "CEntity.h"
#include "CTxdStore.h"
#include "RenderWare.h"
#include "RenderBuffer.h"

#include "Callbacks.hpp"


// common.h
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Min(a,b) ((a) < (b) ? (a) : (b))

// get mask for int & BIT(5)
#define BIT(num) (1<<(num))


#include "plugin.h"
using namespace plugin;

std::vector<HANDLE> g_threads;



//------------------------------------------------------------------
HANDLE InitConsole() // with proto
{
	AllocConsole();

	//SetConsoleOutputCP(866);
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);


	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);

	return hConsole;
}

//void LeaveConsole(HANDLE hConsole = nullptr)
void LeaveConsole(HANDLE hConsole) // with proto
{
	if (hConsole != nullptr) { SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); } // Reset to default color
	FreeConsole();
}



void MboxSTD(std::string msg, std::string title)
{
	MessageBoxA(HWND_DESKTOP, msg.c_str(), title.c_str(), MB_SYSTEMMODAL | MB_ICONWARNING);
}
void EXIT_F()
{
	ExitProcess(EXIT_FAILURE);
}

void DebugHandler()
{
	while (true)
	{
		Sleep(90);
		if ((GetAsyncKeyState(VK_F8) & 0x8000))
		{
		}
	}
}

DWORD CALLBACK DebugEntry(LPVOID)
{
	DebugHandler();
	return TRUE;
}

void StartThread()
{
	HANDLE threadHandle = CreateThread(NULL, 0, DebugEntry, NULL, 0, NULL);
	g_threads.push_back(threadHandle);
}

void StopAllThreads()
{
	for (HANDLE handle : g_threads)
	{
		TerminateThread(handle, 0);
		CloseHandle(handle);
	}
}


bool Check100Version()
{
	bool flag = IsGameVersion10en();
	if (!flag) { MboxSTD("This mod only 4 gta3 version 1.0!", "GTA3 Mod"); } // mbox
	return flag;
}







void OnInject()
{
	if (!Check100Version()) { EXIT_F(); return; } // return ??
	//InitConsole();
	//StartThread();
	CB::InitCBPatch();
	LiveRadioPlayer::InitLiveRadios("DATA\\RADIOS.M3U");
	//std::cout << "sz: " << LiveRadioPlayer::live_radios.size() << "\n";
}

void OnStart() // once
{

}

void OnEachStartNewGame()
{

}

void OnEachReStartGame() // not hooked!
{

}


void drawHudEvent()
{
	//CB::OnDraw();
}


void shutdownRwEvent()
{
	//StopAllThreads();
	//FreeBass(); // lock game TODO
}

bool bIsInitedAudio = false;
void drawMenuEvent()
{
	if (!bIsInitedAudio)
	{
		//AusioUtils::Init_SoundSystem("MaxPayne1_Resources\\Audio");
		bIsInitedAudio = true;
	}
}



bool initRwEventFIX = false;
class gta3_LiveRadioMazahaka {
public:
	gta3_LiveRadioMazahaka() {
		// Добавьте здесь код инициализации плагина
		OnInject();

		Events::initGameEvent += [] { // инит, первый лоад сейва или каждый new game
			if (!initRwEventFIX)  // adapter to initRwEvent
			{
				initRwEventFIX = true;
				//---1st init
				//StartEntryThread();
				OnStart();
			}

		};

		Events::menuDrawingEvent += [] { drawMenuEvent(); };
		Events::drawHudEvent += [] { drawHudEvent(); }; // онли 1.0
		//drawHudEventBefore += [] { drawHudEvent(); };
		//Events::shutdownRwEvent += [] {};
	}
	~gta3_LiveRadioMazahaka() { shutdownRwEvent(); }

} _gta3_LiveRadioMazahaka;