#pragma once
#include "Windows.h"
#include <iostream>
#include <chrono>
#include <string>

#include "plugin.h"
#include "CFont.h"
#include "CDraw.h"
#include "CText.h"
#include "Default.h"
#include "LiveRadio/LiveRadio.h" // tmp inc
using namespace plugin;

//#define ASPECT_RATIO_SCALE

float FindAspectRatio(void)
{
#ifndef ASPECT_RATIO_SCALE
	if (FrontEndMenuManager.m_nPrefsUseWideScreen)
		return 16.0f / 9.0f;
	else
		return 4.0f / 3.0f;
#else
	switch (FrontEndMenuManager.m_nPrefsUseWideScreen) {
	case AR_AUTO:
		return SCREEN_WIDTH / SCREEN_HEIGHT;
	default:
	case AR_4_3:
		return 4.0f / 3.0f;
	case AR_5_4:
		return 5.0f / 4.0f;
	case AR_16_10:
		return 16.0f / 10.0f;
	case AR_16_9:
		return 16.0f / 9.0f;
	case AR_21_9:
		return 21.0f / 9.0f;
	};
#endif
}

#define DEFAULT_SCREEN_WIDTH  (640)
#define DEFAULT_SCREEN_HEIGHT (480)
#define DEFAULT_ASPECT_RATIO (4.0f/3.0f)
#define DEFAULT_VIEWWINDOW (0.7f)
#define SCREEN_ASPECT_RATIO (FindAspectRatio())

#define SCREEN_STRETCH_X(a)   ((a) * (float) SCREEN_WIDTH / DEFAULT_SCREEN_WIDTH)
#define SCREEN_STRETCH_Y(a)   ((a) * (float) SCREEN_HEIGHT / DEFAULT_SCREEN_HEIGHT)

#define SCREEN_SCALE_AR(a) ((a) * DEFAULT_ASPECT_RATIO / SCREEN_ASPECT_RATIO)

#define SCREEN_SCALE_X(a) SCREEN_SCALE_AR(SCREEN_STRETCH_X(a))
#define SCREEN_SCALE_Y(a) SCREEN_STRETCH_Y(a)



namespace CB
{
	void* AudioManager = (void*)0x880FC0;
	void* MusicManager = (void*)0x8F3964;
	void* SampleManager = (void*)0x7341E0;

	auto SampleManager_StartStreamedFile = (bool(__thiscall*)(void* pthis, char nFile, unsigned int nPos, char nStream))0x567D80; // nStream = 0
	auto SampleManager_StopStreamedFile = (void(__thiscall*)(void* pthis, char nStream))0x5680E0; // nStream = 0
	auto MusicManager_ChangeRadioChannel = (bool(__thiscall*)(void* pthis))0x57E130;
	auto MusicManager_ServiceTrack = (void(__thiscall*)(void* pthis))0x57E100;
	auto AudioManager_PlayOneShot = (void(__thiscall*)(void* pthis, int32_t index, uint16_t sound, float vol))0x57A500;

	// MusicManager.cpp
	uintptr_t& pCurrentStation = *(uintptr_t*)0x650B9C;
	char& cDisplay = *(char*)0x650BA1;
	int32_t& gNumRetunePresses = *(int32_t*)0x650B80;
	int32_t& gRetuneCounter = *(int32_t*)0x650B84;
	char& bHasStarted = *(char*)0x650B7C;
#define SOUND_FRONTEND_RADIO_CHANGE 159 // eSound soundlist.h

	/*struct OldDefaultRadioData
	{
		bool sucess;
		int iOldRadioStation;
		int iOldRadioPos;
	};*/

	//// hardcode ucs-2
	//typedef uint16_t wchar;
	//void CB_AsciiToUnicode(const char* src, wchar* dst)
	//{
	//	while ((*dst++ = (unsigned char)*src++) != '\0');
	//}

	// live radio need--------------------------
	// on tick
	// frontend on esc
	// frontend on sfx/music
	// retune
	// on taxi set cb OnPedSetInCarCBAfter CPed::PedSetInCarCB();
	// print radio station name cMusicManager::DisplayRadioStationName()


	/*OldDefaultRadioData MuteDefaultRadio();
	void UnmuteDefaultRadio(OldDefaultRadioData radio);*/

	// calls
	void __stdcall OnDraw() // ++
	{
		//std::cout << "OnDraw()" << "\n";
		LiveRadioPlayer::OnTick();
	}

	void __stdcall OnEscMenu(bool bIsLoad) // ++
	{
		//std::cout << "OnEscMenu()" << "\n";
		LiveRadioPlayer::OnFrontendChangeMode(bIsLoad);
	}

	void __stdcall OnFrontendChangeSFX(uint8_t volume) // ++
	{
		//std::cout << "OnFrontendChangeSFX()" << "\n";
		LiveRadioPlayer::OnFrontendChangeSFX(volume);
	}

	void __stdcall OnFrontendChangeMusic(uint8_t volume) // ++
	{
		//std::cout << "OnFrontendChangeMusic()" << "\n";
		LiveRadioPlayer::OnFrontendChangeMusic(volume);
	}


	void __stdcall OnRadioRetune(bool scrl_up_next) // ++
	{
		//std::cout << "OnRadioRetune()" << "\n";
		LiveRadioPlayer::SetRetune(scrl_up_next);
	}
	bool __stdcall AllowDefaultRadio() // ++
	{
		return !LiveRadioPlayer::bIsLiveRadioModeActive;
	}

	void __stdcall OnSetInCar(CPed* pPed) // ++
	{ // any ped sit
		//std::cout << "OnSetInCar()" << "\n";
		CPlayerPed* pPlayer = FindPlayerPed();
		if (pPlayer && pPlayer == pPed) { LiveRadioPlayer::OnSetInCar(pPed); }
	}

	void __stdcall OnSetOutCar(CPed* pPed) // ++
	{ // any ped out of car
		//std::cout << "OnSetOutCar()" << "\n";
		CPlayerPed* pPlayer = FindPlayerPed();
		if (pPlayer && pPlayer == pPed) { LiveRadioPlayer::OnSetOutCar(pPed); }
	}


	bool tmp = false;
	bool hold = false;
	//OldDefaultRadioData radio;
	//int old_radio = 0;
	void DrawRadioName(std::string name, CRGBA col, bool draw_black_bg, FONTFONT font);
	void OnPrintRadioStation()
	{
		LiveRadioPlayer::CurrentStationStates res = LiveRadioPlayer::GetLiveRadioState();
		if (res.need_print)
		{
			CRGBA col = res.bIslight_color ? res.light_col : res.dark_col;
			DrawRadioName(res.station_name, col, res.need_bg_name_black, res.font);
		}
		return;
		std::cout << "OnPrintRadioStation()" << "\n";
		CRGBA dark_col = CRGBA(102, 133, 143, 255);
		CRGBA light_col = CRGBA(147, 196, 211, 255);

		CRGBA orange = CRGBA(255, 108, 0, 255);
		CRGBA green = CRGBA(39, 152, 7, 255);
		bool press = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

		//if (GetAsyncKeyState(VK_SHIFT) & 0x8000) { DrawRadioName("RISE FM", green); }
		if (press && !hold) {
			tmp = !tmp;
			//if (tmp) { old_radio = MuteDefaultRadio(); }
			//else { UnmuteDefaultRadio(old_radio); }
			hold = true;
		}
		else if (!press)
		{
			hold = false;
		}
	}


	void __stdcall SetAudioGameHandlersPatch();
	void InitCBPatch()
	{
		CB::SetAudioGameHandlersPatch(); // frontend esc/sfx/music settings
	}





	//------game handlers
	int GetGameVolumePercent(bool bSFX = false)
	{ // FrontEndMenuManager MAX VOL 128
#define FRONTEND_AUDIO_MAX 127
	//InitConsole();
	//std::cout << "FrontEndMenuManager.m_nPrefsSfxVolume: " << FrontEndMenuManager.m_nPrefsSfxVolume << "\n";
	//std::cout << "FrontEndMenuManager.m_nPrefsMusicVolume: " << FrontEndMenuManager.m_nPrefsMusicVolume << "\n\n";
	//return 20;
		int res = bSFX ? (FrontEndMenuManager.m_nPrefsSfxVolume * 100) / FRONTEND_AUDIO_MAX : (FrontEndMenuManager.m_nPrefsMusicVolume * 100) / FRONTEND_AUDIO_MAX;
		//std::cout << "bSFX:" << bSFX << " vol:" << res << "\n";
		return res;
#undef FRONTEND_AUDIO_MAX
	}




	// helper hooks/funcs
	/*OldDefaultRadioData MuteDefaultRadio()
	{
		CPlayerPed* pPlayer = FindPlayerPed();
		OldDefaultRadioData res = { 0 };
		if (pPlayer && pPlayer->m_bInVehicle && (!FrontEndMenuManager.m_bMenuActive)) {
			CVehicle* pVehicle = pPlayer->m_pVehicle;
			if (!pVehicle) { return res; }
			res.sucess = true;
			// COMMAND_SET_RADIO_CHANNEL
			// DMAudio.StopFrontEndTrack();
			// DMAudio.Terminate(); // мьютит всЄ
			// MusicManager.Terminate();
			res.iOldRadioStation = (*(cMusicManager*)MusicManager).m_nNextTrack;
			res.iOldRadioPos = (*(cMusicManager*)MusicManager).m_nRadioPosition;
			{ // MusicManager.Terminate(); мюьт без !init
				SampleManager_StopStreamedFile(SampleManager, 0);
				(*(cMusicManager*)MusicManager).m_nNextTrack = eStreamedSounds::NO_TRACK;
				(*(cMusicManager*)MusicManager).m_nPlayingTrack = eStreamedSounds::NO_TRACK;
			}
			pCurrentStation = (uintptr_t)TheText.Get("FEA_FMN");
			pVehicle->m_nRadioStation = (*(cMusicManager*)MusicManager).m_nRadioInCar = eRadioStation::RADIO_OFF; // затираем позицию
			// MusicManager.m_bPreviousPlayerInCar = true;
			// MusicManager.m_nRadioInCar = eStreamedSounds::NO_TRACK;
			cDisplay = 0; // frames to show

			// AudioManager.PlayOneShot(AudioManager.m_nFrontEndEntity, SOUND_FRONTEND_RADIO_CHANGE, 1.0f);
			// cMusicManager::ServiceGameMode() отвечает за переключалку
			// MusicManager.m_bSetNextStation = FALSE;
			// MusicManager.m_nLastTrackServiceTime = 0;
			// MusicManager.m_nResetTime = 0;
			// MusicManager.m_nTimer = 0;

			// gNumRetunePresses = 0; // ненужное
			// gRetuneCounter = 0;
			// bHasStarted = false;

			// MusicManager.ResetTimers(CTimer::GetTimeInMilliseconds());
			// DMAudio.SetRadioChannel(0, 0);
			// DMAudio.SetRadioChannel(eStreamedSounds::NO_TRACK, -1);
			// pVehicle->m_nRadioStation = eRadioStation::RADIO_OFF;
			// MusicManager.SetRadioInCar(eRadioStation::RADIO_OFF);
		}
		return res;
	}

	void UnmuteDefaultRadio(OldDefaultRadioData radio)
	{
		CPlayerPed* pPlayer = FindPlayerPed();
		if (pPlayer && pPlayer->m_bInVehicle && (!FrontEndMenuManager.m_bMenuActive)) {
			CVehicle* pVehicle = pPlayer->m_pVehicle;
			// if(!pVehicle) { return; }

			{ // MusicManager.Terminate(); мюьт без !init
				SampleManager_StartStreamedFile(SampleManager, radio.iOldRadioStation, radio.iOldRadioPos, 0); // todo restore pos normal
				(*(cMusicManager*)MusicManager).m_nNextTrack = radio.iOldRadioStation;
				(*(cMusicManager*)MusicManager).m_nPlayingTrack = radio.iOldRadioStation;
			}

			// MusicManager.m_nRadioInCar = LiveRadioPlayer::iOldRadioStation;
			// if(pVehicle) { pVehicle->m_nRadioStation = LiveRadioPlayer::iOldRadioStation; }
		}
	}*/

	void __stdcall drawHudEvent()
	{
		CB::OnDraw(); // only game
	}

	void __stdcall drawOnlyInMenuEvent()
	{
		CB::OnDraw(); // only menu
	}

	void __stdcall drawMenuEvent()
	{
		//CB::OnDraw(); // game+menu // crash?
	}

	void DrawRadioName(std::string name, CRGBA col, bool draw_black_bg, FONTFONT font)
	{
		// cMusicManager::ChangeRadioChannel()
		// CPed::SetRadioStation(void)
		// cMusicManager::DisplayRadioStationName() // lery FEA_FM0 :)

		wchar_t str[200]; // ++ MAZAHAKAFW
		char strA[250];
		//wchar *string = TheText.Get("FEA_FM0"); 

		// sprintf(strA, "Live Radio"); // dbg
		sprintf(strA, name.c_str());
		AsciiToUnicode(strA, str);
		// string = TheText.Get("FEA_FM8");
		// printf("%s\n", strA);

		// cMusicManager::DisplayRadioStationName() !!!
		CFont::SetJustifyOff();
		CFont::SetBackgroundOff();

		int& RsGlobalType_maximumHeight = *(int*)0x8F4370; // bss 1080 RsGlobal
		float& fHeightScale = *(float*)0x60D438; // 0.002232143 // 1/480 DEFAULT_SCREEN_HEIGHT?
		float& flt_60D43C = *(float*)0x60D43C; // 1.35 // hardcode
		float SetScale_height = RsGlobalType_maximumHeight * fHeightScale * flt_60D43C;

		int& RsGlobal_RsGlobalType_maximumWidth = *(int*)0x8F436C; // bss 1920 RsGlobal
		float& fWidthScale = *(float*)0x60D440; // 0.0015625 // 1/640 DEFAULT_SCREEN_WIDTH?
		float& flt_60D444 = *(float*)0x60D444; // 0.80000001 // hardcode
		float SetScale_width = RsGlobal_RsGlobalType_maximumWidth * fWidthScale * flt_60D444;

		CFont::SetScale(SetScale_width, SetScale_height);
		//CFont::SetScale(SCREEN_SCALE_X(0.8f), SCREEN_SCALE_Y(1.35f));
		CFont::SetPropOn();
		//CFont::SetFontStyle(FONT_BANK);
		//CFont::SetFontStyle(FONT_PAGER); // nw
		//CFont::SetFontStyle(FONT_HEADING); // orig
		CFont::SetFontStyle(font); // orig
		CFont::SetCentreOn();
		// Reminder: Game doesn't have "scaling" at all, it just stretches, and it's team's decision here to not let centered text occupy all the screen.
		//				Disable ASPECT_RATIO_SCALE and it'll go back to default behaviour; stretching.
		float& fDEFAULT_SCREEN_WIDTH = *(float*)0x60D448; // 640.0 // DEFAULT_SCREEN_WIDTH
		float SetCentreSize_size = RsGlobal_RsGlobalType_maximumWidth * fWidthScale * fDEFAULT_SCREEN_WIDTH; // SetScale_width
		CFont::SetCentreSize(SetCentreSize_size);
		//CFont::SetCentreSize(SCREEN_SCALE_X(DEFAULT_SCREEN_WIDTH));

		float& flt_60D44C = *(float*)0x60D44C; // 22.0 // hardcode
		float& flt_60D450 = *(float*)0x60D450; // 2.0 // black offset // hardcode
		if (draw_black_bg)
		{
			CFont::SetColor(CRGBA(0, 0, 0, 255)); //----“≈Ќ№
			float PrintString_black_y = RsGlobalType_maximumHeight * fHeightScale * flt_60D44C + flt_60D450;
			float PrintString_black_x = (RsGlobal_RsGlobalType_maximumWidth / 2) + flt_60D450;
			CFont::PrintString(PrintString_black_x, PrintString_black_y, str);
			//CFont::PrintString(SCREEN_WIDTH / 2 + 2.0f, SCREEN_SCALE_Y(22.0f) + 2.0f, str);
		}


		/*if (col) { CFont::SetColor(CRGBA(102, 133, 143, 255)); } // dark
		else { CFont::SetColor(CRGBA(147, 196, 211, 255)); } // light*/

		CFont::SetColor(col);

		// dbg
		//CFont::SetColor(CRGBA(255, 108, 0, 255)); // orig orange
		//CFont::SetColor(CRGBA(39, 152, 7, 255)); // green FW

		float PrintString_y = RsGlobalType_maximumHeight * fHeightScale * flt_60D44C;
		float PrintString_x = (RsGlobal_RsGlobalType_maximumWidth / 2.0f);
		CFont::PrintString(PrintString_x, PrintString_y, str);
		//CFont::PrintString(SCREEN_WIDTH / 2, SCREEN_SCALE_Y(22.0f), str);
		CFont::DrawFonts();
	}

	void* orig_printf_ptr = NULL;
	int __cdecl HKPRINTF(const char* str)
	{
		if (!strcmp(str, (char*)0x5F34A0)) // "LOAD sprite\n"
		{ // menu loaded once
			OnEscMenu(true);
		}
		else if (!strcmp(str, (char*)0x5F34D4)) // "REMOVE menu textures\n"
		{ // menu unloaded once
			OnEscMenu(false);
		}
		return orig_printf_ptr ? ((int(__cdecl*)(const char* str))orig_printf_ptr)(str) : printf(str);
	}

	void __stdcall HK_cAudioManager_SetEffectsMasterVolume(uint8_t volume)
	{
		OnFrontendChangeSFX(volume);
		((void(__stdcall*)(uint8_t volume))0x57A730)(volume); // cAudioManager::SetEffectsMasterVolume
	}

	void __stdcall HK_cAudioManager_SetMusicMasterVolume(uint8_t volume)
	{
		OnFrontendChangeMusic(volume);
		((void(__stdcall*)(uint8_t volume))0x57A750)(volume); // cAudioManager::SetMusicMasterVolume
	}


	inline bool GetMouseWheelUpJustDown(CPad* pad) { return (pad && (!!(pad->NewMouseControllerState.wheelUp && !pad->OldMouseControllerState.wheelUp))); }
	inline bool GetMouseWheelDownJustDown(CPad* pad) { return (pad && (!!(pad->NewMouseControllerState.wheelDown && !pad->OldMouseControllerState.wheelDown))); }
	void __stdcall HK_OnRadioRetuneHelper()
	{
//#define SOUND_FRONTEND_RADIO_CHANGE 159 // global define
		//((void(__thiscall*)(void* pthis, int32_t index, uint16_t sound, float vol))0x57A500)(AudioManager, );
		auto AudioManager_PlayOneShot = (void(__thiscall*)(void* pthis, int32_t index, uint16_t sound, float vol))0x57A500;
		int& AudioManager_m_nFrontEndEntity = *(int*)0x885734;
		int& gRetuneCounter = *(int*)0x650B84;
		int& gNumRetunePresses = *(int*)0x650B80;

		//bool IsUpMouse = CPad::GetPad(0)->ChangeStationJustDown();
		bool IsUpMouse = GetMouseWheelUpJustDown(CPad::GetPad(0));
		bool IsDownMouse = GetMouseWheelDownJustDown(CPad::GetPad(0));
		if (IsUpMouse)
		{
			if (CB::AllowDefaultRadio())
			{
				gRetuneCounter = 30;
				gNumRetunePresses++;
				AudioManager_PlayOneShot(AudioManager, AudioManager_m_nFrontEndEntity, SOUND_FRONTEND_RADIO_CHANGE, 1.0f);
			}
			CB::OnRadioRetune(true);
		}
		else if (IsDownMouse) // allow back radio station
		{
			if (CB::AllowDefaultRadio())
			{
				gRetuneCounter = 30;
				gNumRetunePresses--; // bugly back
				AudioManager_PlayOneShot(AudioManager, AudioManager_m_nFrontEndEntity, SOUND_FRONTEND_RADIO_CHANGE, 1.0f);
			}
			CB::OnRadioRetune(false);
		}

//#undef SOUND_FRONTEND_RADIO_CHANGE
	}

	void __cdecl HK_CPed_PedSetInCarCB(CAnimBlendAssociation* animAssoc, void* arg)
	{
		auto CPed_PedSetInCarCB = (void(__cdecl*)(CAnimBlendAssociation* animAssoc, void* arg))0x4CF220;
		CPed_PedSetInCarCB(animAssoc, arg);

		CPed* ped = (CPed*)arg;
		OnSetInCar(ped); // after priority
	}

	void __cdecl HK_CPed_PedSetOutCarCB(CAnimBlendAssociation* animAssoc, void* arg)
	{
		auto CPed_PedSetOutCarCB = (void(__cdecl*)(CAnimBlendAssociation* animAssoc, void* arg))0x4CE8F0;
		CPed_PedSetOutCarCB(animAssoc, arg);

		CPed* ped = (CPed*)arg;
		OnSetOutCar(ped); // after priority
	}

	void __fastcall HK_cMusicManager_DisplayRadioStationName(void* ecx_this, void* edx_nu)
	{
		auto cMusicManager_DisplayRadioStationName = (void(__thiscall*)(void* pthis))0x57E6D0;
		if (AllowDefaultRadio()) { cMusicManager_DisplayRadioStationName(ecx_this); }
		CB::OnPrintRadioStation();

	}

	void* CalcPointerOffset(void* op_addr, int offset) // call offset => pointer func // XMemory.h
	{
		return (void*)(((uintptr_t)op_addr) + 1 + sizeof(uintptr_t) + offset);
	}

	void __stdcall RenderMenus() // gta3 memes
	{
		//drawMenuEvent(); // before
		((void(__stdcall*)())0x48E450)(); // RenderMenus
		drawMenuEvent(); // after
	}

	// install
	void __stdcall SetAudioGameHandlersPatch()
	{
#define CALL_BYTES_CNT 5
		//Events::drawHudEvent += [] { CB::OnDraw(); }; // онли 1.0 // DrawHud
		Events::drawHudEvent += [] { drawHudEvent(); }; // онли 1.0 // DrawHud
		Events::menuDrawingEvent += [] { drawOnlyInMenuEvent(); };
		//HookRenderMenus [black menu fix (png sprite blink)]
		plugin::patch::RedirectCall(0x48E6D0, RenderMenus); // todo call old pointer
		plugin::patch::RedirectCall(0x48E7B5, RenderMenus);


		// CMenuManager::SwitchMenuOnAndOff 0x488790 ??
		//unsigned char buff[CALL_BYTES_CNT];
		//memcpy(buff, (void*)0x47A3C7, CALL_BYTES_CNT);
		orig_printf_ptr = CalcPointerOffset((void*)0x47A3C7, plugin::patch::GetUInt(0x47A3C7 + 1));
		//std::cout << "orig_printf_ptr: 0x" << orig_printf_ptr << "\n";
		plugin::patch::RedirectCall(0x47A3C7, HKPRINTF); // "LOAD sprite\n"
		plugin::patch::RedirectCall(0x47A49D, HKPRINTF); // "REMOVE menu textures\n"

		// MENUACTION_SFXVOLUME MENUACTION_MUSICVOLUME CMenuManager::CheckSliderMovement
		// вызываютс€ очень странно, с 0 нажима€ 5ю будет все 5 вызов, а не последний 5й
		// также on reload game
		plugin::patch::RedirectCall(0x57C8AA, HK_cAudioManager_SetEffectsMasterVolume); // MENUACTION_SFXVOLUME void cDMAudio::SetEffectsMasterVolume(uint8 volume);
		plugin::patch::RedirectCall(0x57C8DA, HK_cAudioManager_SetMusicMasterVolume); // MENUACTION_MUSICVOLUME void cDMAudio::SetMusicMasterVolume(uint8 volume)

		// on returne
		plugin::patch::Nop(0x57D729, 0x57D774 - 0x57D729); // ServiceGameMode retune
		plugin::patch::Nop(0x57D6F6, 0x57D709 - 0x57D6F6); // ServiceGameMode retune CPad::ChangeStationJustDown() true
		plugin::patch::RedirectCall(0x57D729, HK_OnRadioRetuneHelper);

		// on taxi set cb OnPedSetInCarCBAfter CPed::PedSetInCarCB();
		plugin::patch::RedirectCall(0x4DED1F, HK_CPed_PedSetInCarCB);
		plugin::patch::RedirectCall(0x4DEE25, HK_CPed_PedSetInCarCB);
		plugin::patch::RedirectCall(0x4DEF9B, HK_CPed_PedSetInCarCB);
		plugin::patch::RedirectCall(0x4DF2D6, HK_CPed_PedSetInCarCB);
		plugin::patch::RedirectCall(0x4DF34C, HK_CPed_PedSetInCarCB);
		plugin::patch::SetUInt(0x4DF3CD + 1, (unsigned int)HK_CPed_PedSetInCarCB); // func is arg
		plugin::patch::SetUInt(0x4E0C6B + 1, (unsigned int)HK_CPed_PedSetInCarCB); // func is arg
		plugin::patch::SetUInt(0x61055C, (unsigned int)HK_CPed_PedSetInCarCB); // func is in arr

		// on taxi set cb OnPedSetOutCarCBAfter CPed::PedSetOutCarCB();
		plugin::patch::SetUInt(0x4A0003 + 1, (unsigned int)HK_CPed_PedSetOutCarCB); // func is arg
		plugin::patch::RedirectCall(0x4A0036, HK_CPed_PedSetOutCarCB);
		plugin::patch::RedirectCall(0x4A0184, HK_CPed_PedSetOutCarCB);
		plugin::patch::RedirectCall(0x4DF5EA, HK_CPed_PedSetOutCarCB);
		plugin::patch::SetUInt(0x4DF5EA + 1, (unsigned int)HK_CPed_PedSetOutCarCB); // func is arg
		plugin::patch::RedirectCall(0x4DF929, HK_CPed_PedSetOutCarCB);
		plugin::patch::RedirectCall(0x4E1516, HK_CPed_PedSetOutCarCB);
		plugin::patch::SetUInt(0x4E16C6 + 1, (unsigned int)HK_CPed_PedSetOutCarCB); // func is arg
		plugin::patch::SetUInt(0x610560, (unsigned int)HK_CPed_PedSetOutCarCB); // func is in arr

		// print radio station name cMusicManager::DisplayRadioStationName()
		plugin::patch::RedirectCall(0x48E400, HK_cMusicManager_DisplayRadioStationName);
	}

}

