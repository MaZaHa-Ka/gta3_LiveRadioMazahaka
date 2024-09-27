#pragma once
#include "LiveRadio.h"
#include "Windows.h"
#include <iostream>
//#include <fstream>
//#include <algorithm>
#include <vector>
//#include <string>

// MIT лицензия By MaZaHaKa
#pragma once
#pragma warning(disable : 4267) // stzet to int


#include "CMenuManager.h"
#include "CWorld.h"
#include "CCamera.h"
#include "CReplay.h"
#include "CHud.h"
#include "CGame.h"
#include "CCutsceneMgr.h"
#include "CUserDisplay.h"
#include "CText.h"


//----------ADD---UTILS----BLOCK
// moved from Callbacks
void* LR_AudioManager = (void*)0x880FC0;
void* LR_MusicManager = (void*)0x8F3964;
void* LR_SampleManager = (void*)0x7341E0;

auto LR_SampleManager_StartStreamedFile = (bool(__thiscall*)(void* pthis, char nFile, unsigned int nPos, char nStream))0x567D80; // nStream = 0
auto LR_SampleManager_StopStreamedFile = (void(__thiscall*)(void* pthis, char nStream))0x5680E0; // nStream = 0
auto LR_MusicManager_ChangeRadioChannel = (bool(__thiscall*)(void* pthis))0x57E130;
auto LR_MusicManager_ServiceTrack = (void(__thiscall*)(void* pthis))0x57E100;
auto LR_AudioManager_PlayOneShot = (void(__thiscall*)(void* pthis, int32_t index, uint16_t sound, float vol))0x57A500;

// MusicManager.cpp
uintptr_t& LR_pCurrentStation = *(uintptr_t*)0x650B9C;
char& LR_cDisplay = *(char*)0x650BA1;
int32_t& LR_gNumRetunePresses = *(int32_t*)0x650B80;
int32_t& LR_gRetuneCounter = *(int32_t*)0x650B84;
char& LR_bHasStarted = *(char*)0x650B7C;
#define LR_SOUND_FRONTEND_RADIO_CHANGE 159 // eSound soundlist.h

//int GetFrontendAudioPercentVolume(bool bSFX = false)
int LiveRadioPlayer::GetFrontendAudioPercentVolume(bool bSFX)
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

//// hardcode ucs-2
//typedef uint16_t wchar;
void
LR_AsciiToUnicode(const char* src, wchar_t* dst)
{
	while ((*dst++ = (unsigned char)*src++) != '\0');
}


wchar_t* CPagerMsgBuff = nullptr;
bool DisplayPagerMsg(std::string msg) // status is add
{
	//if (CUserDisplay::Pager.m_messages[0]) { return false; }
	int16_t& PagerOn = *(int16_t*)0x95CCA0; // CHud::Draw()
	//std::cout << "PG: " << PagerOn << "\n"; // 0 not draw. 1 display. 2 go back
	if (PagerOn != 0) { return false; }
	if (CPagerMsgBuff) { delete[] CPagerMsgBuff; CPagerMsgBuff = nullptr; }
	//size_t len = strlen(copyStr) + 1;
	size_t len = msg.length() + 1;
	CPagerMsgBuff = new wchar_t[len];
	mbstowcs(CPagerMsgBuff, msg.c_str(), len);
	CUserDisplay::Pager.AddMessage(CPagerMsgBuff, 140, 2, 0);
	return true;
}

void
LR_Message(const char* str)
{
	/*std::wstring wideStr = Utf8ToWide(str);
#ifdef MAZAHAKA_SILENT_CHUD_MESSAGE
	if(play_beep) { CHud::SetHelpMessage((wchar *)wideStr.c_str(), true); }
	else { CHud::SetHelpMessageNoSound((wchar *)wideStr.c_str(), true); }
#else
	CHud::SetHelpMessage((wchar *)wideStr.c_str(), true);
#endif*/

	wchar_t strW[200];
	// char strA[250];
	// sprintf(strA, "TTS DAT Reloaded!");
	LR_AsciiToUnicode(str, strW);
	CHud::SetHelpMessage(strW, true);
}

bool
IsPlayerAsPassangerNow()
{
	CPlayerPed* pPlayer = FindPlayerPed();
	if (!pPlayer) { return false; }
	if (pPlayer->m_bInVehicle && pPlayer->m_pVehicle->m_pDriver != pPlayer) { return true; } // passanger?
	return false;
}

bool
CheckWastedBustedPlayer() // return true if wasted/busted
{
	CPlayerInfo* pPlayerInfo = &CWorld::Players[CWorld::PlayerInFocus];
	if (!pPlayerInfo) { return false; }
	//CPlayerInfo::IsRestartingAfterDeath
	//CPlayerInfo::IsRestartingAfterArres
	return (pPlayerInfo->m_nPlayerState != eWastedBustedState::WBSTATE_PLAYING);
	//return ((pPlayerInfo->m_WBState == WBSTATE_WASTED) || (pPlayerInfo->m_WBState == WBSTATE_BUSTED));
}

bool
CheckWindowStateIsOpenedMaZaHaKa()
{
#define PSGLOBAL(var) (((psGlobalType *)(RsGlobal.ps))->var)
	//HWND window = ((psGlobalType *)(RsGlobal.ps))->window; // PSGLOBAL(window)
	HWND window = PSGLOBAL(window);
	HWND activeWindow = GetForegroundWindow();

	//bool IsMinimized = (IsIconic(window) != 0); // no used here
	bool IsActive = (window == activeWindow);
	return IsActive;
}

bool
IsPlayerPlaying(bool check_fading = true, bool check_cutscene_borders = true, bool check_passanger = true, bool check_window = true)
{ // CPlayerInfo::MakePlayerSafe(bool toggle)
	enum eGameState
	{
		GS_START_UP = 0,
		GS_INIT_LOGO_MPEG,
		GS_LOGO_MPEG,
		GS_INIT_INTRO_MPEG,
		GS_INTRO_MPEG,
		GS_INIT_ONCE,
		GS_INIT_FRONTEND,
		GS_FRONTEND,
		GS_INIT_PLAYING_GAME,
		GS_PLAYING_GAME,
	};

	CPlayerInfo* pPlayerInfo = &CWorld::Players[CWorld::PlayerInFocus];
	CPlayerPed* pPlayer = FindPlayerPed();
	uint32_t& gGameState = *(uint32_t*)0x8F5838; // re3 win.h crossplatforn.h
	if (!pPlayer || !pPlayerInfo) { return false; }

	if (CCutsceneMgr::ms_running) { return false; }
	if (gGameState != eGameState::GS_PLAYING_GAME) { return false; }
	// if (IsPlayerOnMission()) { return false; } // desp rando wanted
//#ifdef MAZAHAKA_FIX_BACKGROUND_APP_NO_HOLD_MOUSE 
	if (check_window && (!CheckWindowStateIsOpenedMaZaHaKa())) { return false; }
//#endif

	// pad? check
	if (FrontEndMenuManager.m_bMenuActive) { return false; }
	if (FrontEndMenuManager.m_bSaveMenuActive) { return false; }
	if (check_fading && TheCamera.m_bFading) { return false; }
	if (FrontEndMenuManager.m_bGameNotLoaded) { return false; }
	if (CheckWastedBustedPlayer()) { return false; }
	// FindPlayerVehicle() return true if player once sit in his car!!
	if (pPlayer->m_bInVehicle && (pPlayer->m_pVehicle->m_nVehicleClass == eVehicleType::VEHICLE_TRAIN)) { return false; } // pPlayer->m_pVehicle TRUE AFTER SIT IN CAR

	if (CGame::playingIntro) { return false; }
	if (check_cutscene_borders && TheCamera.m_bWideScreenOn) { return false; } // на катсценах чёрныё полоски
	//if (FrontEndMenuManager.m_nPrefsUseWideScreen) { return false; } // в настройках widescreen по default off FrontEndMenuManager.m_PrefsUseWideScreen CDraw::FindAspectRatio()

	//if(pPlayer->InVehicle() && pPlayer->m_pMyVehicle->pDriver != pPlayer) { return false; } // passanger?
	if (check_passanger && IsPlayerAsPassangerNow()) { return false; } // passanger?
	if (CReplay::Mode != eRaplayMode::MODE_RECORD) { return false; }
	//if (CReplay::IsPlayingBack()) { return false; }

	bool defaultgamesafeped =
		(true
		&& pPlayer->m_pWanted->m_nWantedFlags & (1 << (1)) // m_bIgnoredByEveryone
		&& pPlayer->m_nFlags.bBulletProof
		&& pPlayer->m_nFlags.bFireProof
		&& pPlayer->m_nFlags.bCollisionProof
		&& pPlayer->m_nFlags.bMeleeProof
		&& pPlayer->m_nFlags.bOnlyDamagedByPlayer
		&& pPlayer->m_nFlags.bExplosionProof
		&& !(pPlayer->m_bCanBeDamaged)
		);
	if (defaultgamesafeped) { return false; }
	// on true safe
	// m_pPed->bBulletProof = true;
	// m_pPed->bFireProof = true;
	// m_pPed->bCollisionProof = true;
	// m_pPed->bMeleeProof = true;
	// m_pPed->bOnlyDamagedByPlayer = true;
	// m_pPed->bExplosionProof = true;
	// m_pPed->m_bCanBeDamaged = false;
	return true;
}
/*bool8 // not used
RadioPlayerInCar()
{
	CPlayerPed* pPlayer = FindPlayerPed();
	if (!FindPlayerVehicle() || !pPlayer) { return FALSE; }
	if(!pPlayer->m_bInVehicle) { return FALSE; }

	int32_t State = FindPlayerPed()->m_ePedState;

	if (State == PED_DRAG_FROM_CAR || State == PED_EXIT_CAR || State == PED_ARRESTED)
		return FALSE;

	if (!FindPlayerVehicle())
		return TRUE;

	if (FindPlayerVehicle()->m_nState == STATUS_WRECKED)
		return FALSE;

	switch (FindPlayerVehicle()->m_nModelIndex) {
	case MI_FIRETRUCK:
	case MI_AMBULAN:
	case MI_MRWHOOP:
	case MI_PREDATOR:
	case MI_TRAIN:
	case MI_SPEEDER:
	case MI_REEFER:
	case MI_GHOST: return FALSE;
	default: return TRUE;
	}
}*/

bool8
UsesPoliceRadio(CVehicle* veh) // false if default radio!!!
{
	if (!veh) { return false; }
	switch (veh->m_nModelIndex)
	{
	case MI_FBICAR:
	case MI_POLICE:
	case MI_ENFORCER:
	case MI_PREDATOR:
	case MI_RHINO:
	case MI_BARRACKS:
		return TRUE;
	}
	return FALSE;
}
//------------------------------







//----------------------LIVE RADIO [url node]
// Trim function to remove leading and trailing whitespace
M3UNodeMazahaka::M3UNodeMazahaka(const std::string &name, const std::string &url, const bool &is_live)
    : name(LiveRadioPlayer::trim(name)), url(LiveRadioPlayer::trim(url)), is_live(is_live)
{
}


//------------------------- LIVE RADIO PLAYER
// variables
std::string LiveRadioPlayer::m_sRadioListPath;
char LiveRadioPlayer::m_cKeySwitch = VK_F4;

#ifdef VOLUME_RETUNE
char LiveRadioPlayer::m_cVolRetuneKeySwitch = VK_SHIFT;
int LiveRadioPlayer::m_iVolRetunePercent = 0;
int LiveRadioPlayer::m_iVolRetunePercentStep = 0;
#endif

bool LiveRadioPlayer::m_bSwitcherHoldFlag;

bool LiveRadioPlayer::bIsLRInitalize = false;
bool LiveRadioPlayer::bIsLiveRadioModeActive = false;
// bool LiveRadioPlayer::bIsLiveRadioModeNeedPrintText = true;
// LiveRadioPlayer::CurrentStationStates LiveRadioPlayer::LiveRadioState = {"Live Radio", true, true, 0.0f, FONTFONT::FONT_HEADING};
LiveRadioPlayer::CurrentStationStates LiveRadioPlayer::LiveRadioState;
int LiveRadioPlayer::m_iCurrentRadioPos = 0;
std::vector<M3UNodeMazahaka> LiveRadioPlayer::live_radios;

int LiveRadioPlayer::m_iNextRadioPos;
volatile bool LiveRadioPlayer::isStationChanging;
std::thread LiveRadioPlayer::radioSwitchThread;

// for restore
int LiveRadioPlayer::m_iOldStreamedPlayingTrack = 0;
//int LiveRadioPlayer::m_iOldStreamedNextTrack = 0;
int LiveRadioPlayer::m_iOldRadioStation = 0;
//int LiveRadioPlayer::m_iOldRadioInCar = 0;
//int LiveRadioPlayer::m_iOldRadioPos = 0; // todo start from start

void
LiveRadioPlayer::InitLiveRadios(std::string sM3UPath)
{
	LiveRadioPlayer::m_sRadioListPath = sM3UPath;
	LiveRadioPlayer::m_cKeySwitch = VK_F4;

#ifdef VOLUME_RETUNE
	LiveRadioPlayer::m_cVolRetuneKeySwitch = VK_SHIFT;
	LiveRadioPlayer::m_iVolRetunePercent = GetFrontendAudioPercentVolume(false);
	LiveRadioPlayer::m_iVolRetunePercentStep = 10; // %
#endif

	LiveRadioPlayer::m_bSwitcherHoldFlag = false;

	// for restore
	LiveRadioPlayer::m_iOldStreamedPlayingTrack = 0; // -1?
	//--LiveRadioPlayer::m_iOldStreamedNextTrack = 0; // -1?
	LiveRadioPlayer::m_iOldRadioStation = 0; // -1?
	//--LiveRadioPlayer::m_iOldRadioInCar = 0; // -1?
	//LiveRadioPlayer::m_iOldRadioPos = 0; // todo start from start

	//LiveRadioPlayer::sRadioListPath = "DATA\\RADIOS.M3U";
	//LiveRadioPlayer::cKeySwitch = HotKeyStore_GetLiveRadioHotkey(); // init

	LiveRadioPlayer::live_radios = LiveRadioPlayer::LiveRadioParse(LiveRadioPlayer::m_sRadioListPath); // todo
	LiveRadioPlayer::live_radios = LiveRadioPlayer::GetFilteredStations(live_radios, true); // filter only url
	M3UNodeMazahaka radio_non_station = {"Live Radio Off", "", true};
	// Вставляем элемент в начало
	LiveRadioPlayer::live_radios.insert(LiveRadioPlayer::live_radios.begin(), radio_non_station);
	LiveRadioPlayer::bIsLRInitalize = true;
	// LiveRadioPlayer::LiveRadioState.need_print = true;
	LiveRadioPlayer::LiveRadioState.font = FONTFONT::FONT_HEADING;
	// CFont::SetFontStyle(FONT_BANK);
	// CFont::SetFontStyle(FONT_PAGER); // nw
	// CFont::SetFontStyle(FONT_HEADING); // orig

	LiveRadioPlayer::LiveRadioState.need_bg_name_black = true;
	LiveRadioPlayer::LiveRadioState.dark_col = CRGBA(102, 133, 143, 255);
	LiveRadioPlayer::LiveRadioState.light_col = CRGBA(147, 196, 211, 255);

	CRGBA dark_col = CRGBA(102, 133, 143, 255);
	CRGBA light_col = CRGBA(147, 196, 211, 255);
	CRGBA orange = CRGBA(255, 108, 0, 255);
	CRGBA green = CRGBA(39, 152, 7, 255);

	// test block
	/*int a = 200;
	LiveRadioPlayer::LiveRadioState.need_bg_name_black = false;
	LiveRadioPlayer::LiveRadioState.dark_col = CRGBA(102, 133, 143, a);
	LiveRadioPlayer::LiveRadioState.light_col = CRGBA(147, 196, 211, a);*/
	// !test block

	LiveRadioPlayer::m_iNextRadioPos = LiveRadioPlayer::m_iCurrentRadioPos = 1; // 0 radio off

	LiveRadioPlayer::radioSwitchThread = std::thread(&LiveRadioPlayer::RadioSwitchingThread);

	// dump
	/*std::cout << "radios: " << LiveRadioPlayer::live_radios.size() << "\n";
	for(size_t i = 0; i < LiveRadioPlayer::live_radios.size(); i++) {
	        std::cout << LiveRadioPlayer::live_radios[i].name << ": " << LiveRadioPlayer::live_radios[i].url << "\n";
	}*/
}

void
LiveRadioPlayer::RestartDrawCurrentRadioStation(std::string new_str2print)
{
	//if(!bIsLiveRadioModeActive) { return; }
	//LiveRadioState.station_name = live_radios[m_iCurrentRadioPos].name; // in callers
	LiveRadioState.str2print = new_str2print;
	LiveRadioState.bIslight_color = false;
	LiveRadioState.need_print = true;
	LiveRadioState.start_time = std::chrono::steady_clock::now();
}

void
LiveRadioPlayer::ResetDefaultPrintRadioNameData()
{
	LR_cDisplay = 0; // frames to show
	//LR_pCurrentStation = (uintptr_t)TheText.Get("FEA_FMN");
}



void
LiveRadioPlayer::SetRetune(bool scrl_up_next)
{// todo if shift ch volume + patch if shift on retune
#ifdef VOLUME_RETUNE
	bool bIsPressedVolumeHotkey = (GetAsyncKeyState(LiveRadioPlayer::m_cVolRetuneKeySwitch) & 0x8000);
	if (bIsPressedVolumeHotkey)
	{ // retune volume
		LiveRadioPlayer::ResetDefaultPrintRadioNameData();
		m_iVolRetunePercent = GetFrontendAudioPercentVolume(false);
		int k = m_iVolRetunePercentStep;
		m_iVolRetunePercent = ((m_iVolRetunePercent + k / 2) / k) * k;
		bool retune = false; // if over limit no need setnewvol. or duplicate setnewnol in retune block
		if (scrl_up_next && (m_iVolRetunePercent < 100)) { m_iVolRetunePercent = (m_iVolRetunePercent + m_iVolRetunePercentStep) > 100 ? 100 : m_iVolRetunePercent + m_iVolRetunePercentStep; retune = true; } // +
		else if((!scrl_up_next) && ((m_iVolRetunePercent > 0))) { m_iVolRetunePercent = (m_iVolRetunePercent - m_iVolRetunePercentStep) < 0 ? 0 : m_iVolRetunePercent - m_iVolRetunePercentStep; retune = true; } // -
		if (retune)
		{
			LiveRadioPlayer::SetNewVol(m_iVolRetunePercent, false);
			LR_AudioManager_PlayOneShot(LR_AudioManager, (*(cAudioManager*)LR_AudioManager).m_nFrontEndEntity, LR_SOUND_FRONTEND_RADIO_CHANGE, 1.0f); // щелчёк переключателя
		}
		return;
	}
	else if (!LiveRadioPlayer::bIsLiveRadioModeActive)
	{
		LiveRadioState.need_print = false;
	}
#endif

	// iа retune volume defined and activated we shouldn't be here
	if(!bIsLiveRadioModeActive) { return; }
	// printf("SetRetune: %d\n", scrl_up_next);

	if(scrl_up_next) {
		m_iCurrentRadioPos = (m_iCurrentRadioPos + 1) % live_radios.size();
	} else {
		m_iCurrentRadioPos = (m_iCurrentRadioPos - 1 + live_radios.size()) % live_radios.size();
	}

	// BassAudio::StopLiveRadio();
	// BassAudio::SetLiveRadioStationAsync(LiveRadioPlayer::live_radios[m_iCurrentRadioPos].url.c_str(), GetFrontendAudioPercentVolume(false));

	// Signal that a station change is needed
	isStationChanging = true;
	LiveRadioPlayer::RestartDrawCurrentRadioStation(live_radios[m_iCurrentRadioPos].name);

	LR_AudioManager_PlayOneShot(LR_AudioManager, (*(cAudioManager*)LR_AudioManager).m_nFrontEndEntity, LR_SOUND_FRONTEND_RADIO_CHANGE, 1.0f); // щелчёк переключателя
}

LiveRadioPlayer::CurrentStationStates
LiveRadioPlayer::GetLiveRadioState()
{ // ondraw
	// counter getter to disable draw if no retune
	//if(!bIsLiveRadioModeActive) { return LiveRadioState; }
	// printf("GetLiveRadioState ondraw()!\n");
	if(LiveRadioState.need_print) {
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration<float>(now - LiveRadioState.start_time).count();

		if(elapsed > GTA3_RADIO_DARW_NAME_TIME) {
			LiveRadioState.bIslight_color = true;
			// isStationChanging = true;
			// BassAudio::StopLiveRadio();
			// BassAudio::SetLiveRadioStationAsync(LiveRadioPlayer::live_radios[m_iCurrentRadioPos].url.c_str(),
			// GetFrontendAudioPercentVolume(false));
		}
		if(elapsed > GTA3_RADIO_DARW_NAME_TIME * 2) { LiveRadioState.need_print = false; }
	}

	return LiveRadioState;
}

void
LiveRadioPlayer::SwitchStation(int index) // todo sync
{
	if(index < 0) { return; }
	// Stop the current station
	BassAudio::StopLiveRadio();

	// Start the new station
	BassAudio::SetLiveRadioStation(LiveRadioPlayer::live_radios[index].url.c_str(), LiveRadioPlayer::GetFrontendAudioPercentVolume(false));

	// Update the current position
	// m_iNextRadioPos = index;
}

void
LiveRadioPlayer::RadioSwitchingThread()
{
	while(true) {
		if(isStationChanging || (m_iNextRadioPos != m_iCurrentRadioPos)) {
			m_iNextRadioPos = m_iCurrentRadioPos;
			LiveRadioPlayer::SwitchStation(m_iNextRadioPos);
			isStationChanging = false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

std::string
LiveRadioPlayer::trim(const std::string &str)
{
	const auto strBegin = str.find_first_not_of(" \t\n\r");
	if(strBegin == std::string::npos) return ""; // Empty string

	const auto strEnd = str.find_last_not_of(" \t\n\r");
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::vector<M3UNodeMazahaka>
LiveRadioPlayer::LiveRadioParse(const std::string &filePath)
{
	std::vector<M3UNodeMazahaka> radios;
	std::ifstream file(filePath);
	if(!file.is_open()) {
		//std::cout << "Error opening file: " << filePath << std::endl;
		std::cerr << "Error opening file: " << filePath << std::endl;
		return radios;
	}

	std::string line;
	std::string currentName;
	while(std::getline(file, line)) {
		// Remove any leading/trailing whitespace
		line.erase(0, line.find_first_not_of(" \t\n\r"));
		line.erase(line.find_last_not_of(" \t\n\r") + 1);

		if(line.empty() || line[0] == ';') {
			continue; // Skip empty lines and comments
		}

		if(line[0] == '#') {
			// Handle metadata lines, if needed (e.g., #EXTINF)
			if(line.find("#EXTINF:") == 0) {
				std::size_t commaPos = line.find(',', 8);
				if(commaPos != std::string::npos) {
					currentName = line.substr(commaPos + 1);
					currentName = trim(currentName); // Trim the name
				}
			}
		} else {
			// Assume non-comment lines are URLs
			if(!currentName.empty()) {
				bool isLive = (line.find("http") != std::string::npos);
				radios.emplace_back(currentName, line, isLive);
				currentName.clear();
			}
		}
	}

	return radios;
}

std::vector<M3UNodeMazahaka>
LiveRadioPlayer::GetFilteredStations(const std::vector<M3UNodeMazahaka> &stations, bool is_live)
{
	std::vector<M3UNodeMazahaka> filteredStations;
	for(const auto &station : stations) {
		if(station.is_live == is_live) { filteredStations.push_back(station); }
	}
	return filteredStations;
}

void
LiveRadioPlayer::SwitchLiveRadio(bool mode, bool need_save_old_radio) // OnOFF
{
	if(mode) {
		LiveRadioPlayer::TryMuteVehRadio(need_save_old_radio);
		LiveRadioPlayer::isStationChanging = true;
		// LiveRadioPlayer::bIsLiveRadioModeActive; // true
		// LiveRadioPlayer::SwitchStation(m_iCurrentRadioPos); // lock frame
		LiveRadioPlayer::RestartDrawCurrentRadioStation(live_radios[m_iCurrentRadioPos].name);
		LR_Message("Live Radio Mode");
	} else {
		BassAudio::StopLiveRadio();
		LiveRadioPlayer::isStationChanging = true;
		LiveRadioPlayer::RestoreRadioInCar();
		LR_Message("Default Radio Mode");
	}
}

void
LiveRadioPlayer::OnTick() // in game+menu(if menu mute just now loaded radio bug)
{
	CPlayerPed *pPlayer = FindPlayerPed();
	//if (!pPlayer) { return; }
	if (FrontEndMenuManager.m_bGameNotLoaded) { return; }
	bool ispress = (GetAsyncKeyState(LiveRadioPlayer::m_cKeySwitch) & 0x8000);
	bool is_player_inveh = (pPlayer && pPlayer->m_bInVehicle && (!UsesPoliceRadio(pPlayer->m_pVehicle)));
	bool allow_game_switch = IsPlayerPlaying(true, true, false, false) && is_player_inveh;
	bool is_frontend_vol = !!GetFrontendAudioPercentVolume(false);

	if (allow_game_switch) {
		if (ispress && !LiveRadioPlayer::m_bSwitcherHoldFlag) {
#ifdef NOT_ALLOW_SWITCH_TO_LR_IF_VOL_ZERO
			if (is_frontend_vol) {
#endif
				LiveRadioPlayer::bIsLiveRadioModeActive = !LiveRadioPlayer::bIsLiveRadioModeActive;
				if (LiveRadioPlayer::bIsLiveRadioModeActive) {
					SwitchLiveRadio(true);
				}
				else {
					SwitchLiveRadio(false);
				}
#ifdef NOT_ALLOW_SWITCH_TO_LR_IF_VOL_ZERO
			}
			else { // ! music
				LR_Message("Music volume in settings is 0. Live radio turned off.", true);
			}
#endif
			LiveRadioPlayer::m_bSwitcherHoldFlag = true;
		}
		else if (!ispress) {
			LiveRadioPlayer::m_bSwitcherHoldFlag = false;
		}
	}


	// if(!is_player_inveh)
	if(pPlayer && (pPlayer->m_eObjective == eObjective::OBJECTIVE_LEAVE_CAR) && LiveRadioPlayer::bIsLiveRadioModeActive) { // сброс liveradio
		BassAudio::StopLiveRadio();
		// LiveRadioPlayer::bIsLiveRadioModeActive = false;
		// LiveRadioPlayer::RestoreRadioInCar();
	}
	// if leave car and radio loading -> when not in car he play
	if((pPlayer && (!LiveRadioPlayer::bIsLiveRadioModeActive) && BassAudio::IsLiveRadioAudible()) || !is_player_inveh) { BassAudio::StopLiveRadio(); }
	// if loading radio and press esc after loading start play
	if(pPlayer && LiveRadioPlayer::bIsLiveRadioModeActive && (FrontEndMenuManager.m_bMenuActive || FrontEndMenuManager.m_bSaveMenuActive)) { BassAudio::SetLiveRadioPercentVolume(0); }
}

void
LiveRadioPlayer::TryMuteVehRadio(bool need_save_old_radio)
{
	CPlayerPed* pPlayer = FindPlayerPed();
	if (pPlayer && pPlayer->m_bInVehicle && (!FrontEndMenuManager.m_bMenuActive)) {
		CVehicle* pVehicle = pPlayer->m_pVehicle;
		if (!pVehicle) { return; }
		// COMMAND_SET_RADIO_CHANNEL
		// DMAudio.StopFrontEndTrack();
		// DMAudio.Terminate(); // мьютит всё
		// MusicManager.Terminate();
		if (need_save_old_radio)
		{
			LiveRadioPlayer::m_iOldStreamedPlayingTrack = (*(cMusicManager*)LR_MusicManager).m_nPlayingTrack;
			//LiveRadioPlayer::m_iOldStreamedNextTrack = (*(cMusicManager*)LR_MusicManager).m_nNextTrack;
			LiveRadioPlayer::m_iOldRadioStation = pVehicle->m_nRadioStation;
			//LiveRadioPlayer::m_iOldRadioInCar = (*(cMusicManager*)LR_MusicManager).m_nRadioInCar; // 127
			/*std::cout << "Stored:" << "\n";
			std::cout << "m_iOldStreamedPlayingTrack: " << LiveRadioPlayer::m_iOldStreamedPlayingTrack << "\n";
			//std::cout << "m_iOldStreamedNextTrack: " << LiveRadioPlayer::m_iOldStreamedNextTrack << "\n";
			std::cout << "m_iOldRadioStation: " << LiveRadioPlayer::m_iOldRadioStation << "\n";
			//std::cout << "m_iOldRadioInCar: " << LiveRadioPlayer::m_iOldRadioInCar << "\n\n";*/
		}

		(*(cMusicManager*)LR_MusicManager).m_nNextTrack = eStreamedSounds::NO_TRACK;
		//MusicManager.m_nNextTrack = eStreamedSounds::STREAMED_SOUND_RADIO_RISE;
		//MusicManager.ChangeRadioChannel();
		pVehicle->m_nRadioStation = (*(cMusicManager*)LR_MusicManager).m_nRadioInCar = eRadioStation::RADIO_OFF; // затираем
		if (LR_MusicManager_ChangeRadioChannel(LR_MusicManager)) { LR_MusicManager_ServiceTrack(LR_MusicManager); } // from cMusicManager::ServiceGameMode() // управление радио
		// do not show radio off
		LR_cDisplay = 0; // frames to show
		LR_pCurrentStation = (uintptr_t)TheText.Get("FEA_FMN");

		/*LiveRadioPlayer::iOldRadioStation = MusicManager.m_nNextTrack;
		LiveRadioPlayer::iOldRadioPos = MusicManager.m_nRadioPosition;
		{ // MusicManager.Terminate(); мюьт без !init
			SampleManager.StopStreamedFile();
			MusicManager.m_nNextTrack = eStreamedSounds::NO_TRACK;
			MusicManager.m_nPlayingTrack = eStreamedSounds::NO_TRACK;
		}
		pCurrentStation = TheText.Get("FEA_FMN");
		pVehicle->m_nRadioStation = MusicManager.m_nRadioInCar = eRadioStation::RADIO_OFF; // затираем позицию
		// MusicManager.m_bPreviousPlayerInCar = true;
		// MusicManager.m_nRadioInCar = eStreamedSounds::NO_TRACK;
		cDisplay = 0; // frames to show*/

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
}

void
LiveRadioPlayer::RestoreRadioInCar()
{
	//std::cout << "try restore radio: " << LiveRadioPlayer::m_iOldStreamedPlayingTrack << "\n";
	if (LiveRadioPlayer::m_iOldStreamedPlayingTrack < 0) { return; }
	CPlayerPed* pPlayer = FindPlayerPed();
	if (pPlayer && pPlayer->m_bInVehicle && (!FrontEndMenuManager.m_bMenuActive)) {
		CVehicle* pVehicle = pPlayer->m_pVehicle;
		// if(!pVehicle) { return; }

		/*{ // MusicManager.Terminate(); мюьт без !init
			SampleManager.StartStreamedFile(LiveRadioPlayer::iOldRadioStation, LiveRadioPlayer::iOldRadioPos); // todo restore pos normal
			MusicManager.m_nNextTrack = LiveRadioPlayer::iOldRadioStation;
			MusicManager.m_nPlayingTrack = LiveRadioPlayer::iOldRadioStation;
		}*/


		/*std::cout << "REstored:" << "\n";
		std::cout << "m_iOldStreamedPlayingTrack: " << LiveRadioPlayer::m_iOldStreamedPlayingTrack << "\n";
		std::cout << "m_iOldStreamedNextTrack: " << LiveRadioPlayer::m_iOldStreamedNextTrack << "\n";
		std::cout << "m_iOldRadioStation: " << LiveRadioPlayer::m_iOldRadioStation << "\n";
		//std::cout << "m_iOldRadioInCar: " << LiveRadioPlayer::m_iOldRadioInCar << "\n\n"; // 127*/

		//(*(cMusicManager*)LR_MusicManager).m_nPlayingTrack = LiveRadioPlayer::m_iOldStreamedPlayingTrack;
		//(*(cMusicManager*)LR_MusicManager).m_nNextTrack = LiveRadioPlayer::m_iOldStreamedNextTrack;
		(*(cMusicManager*)LR_MusicManager).m_nNextTrack = LiveRadioPlayer::m_iOldStreamedPlayingTrack;
		pVehicle->m_nRadioStation = LiveRadioPlayer::m_iOldRadioStation;
		//(*(cMusicManager*)LR_MusicManager).m_nRadioInCar = LiveRadioPlayer::m_iOldRadioInCar;

		if (LR_MusicManager_ChangeRadioChannel(LR_MusicManager)) { LR_MusicManager_ServiceTrack(LR_MusicManager); } // from cMusicManager::ServiceGameMode() // управление радио
		//cDisplay = 60; // need? 30 vanila

		// MusicManager.m_nRadioInCar = LiveRadioPlayer::iOldRadioStation;
		// if(pVehicle) { pVehicle->m_nRadioStation = LiveRadioPlayer::iOldRadioStation; }
	}
}


bool LiveRadioPlayer::IsAllowedRetuneDefaultRadio()
{ // todo shift with no retune vol stop print curr default radio station name!!
#ifdef VOLUME_RETUNE
	bool bIsPressedVolumeHotkey = (GetAsyncKeyState(LiveRadioPlayer::m_cVolRetuneKeySwitch) & 0x8000);
	return ((!LiveRadioPlayer::bIsLiveRadioModeActive) && !bIsPressedVolumeHotkey);
#else
	return !LiveRadioPlayer::bIsLiveRadioModeActive;
#endif
}

void LiveRadioPlayer::SetNewVol(int percent_vol, bool bSFX)
{
#define FRONTEND_AUDIO_MAX 127
	if (percent_vol < 0) { percent_vol = 0; }
	if (percent_vol > 100) { percent_vol = 100; }

	if (percent_vol < 0) { percent_vol = 0; }
	if (percent_vol > 100) { percent_vol = 100; }
	int volume = (percent_vol * FRONTEND_AUDIO_MAX) / 100;
	std::string vol_str = "VOLUME: " + std::to_string(percent_vol);// +"\0"; // 100

	if (bSFX)
	{ //?
		FrontEndMenuManager.m_nPrefsSfxVolume = volume;
		((void(__stdcall*)(uint8_t volume))0x57A730)(volume); // cAudioManager::SetEffectsMasterVolume
	}
	else
	{
		FrontEndMenuManager.m_nPrefsMusicVolume = volume;
		((void(__stdcall*)(uint8_t volume))0x57A750)(volume); // cAudioManager::SetMusicMasterVolume
		if (LiveRadioPlayer::bIsLiveRadioModeActive) { BassAudio::SetLiveRadioPercentVolume(percent_vol); }
		LiveRadioPlayer::RestartDrawCurrentRadioStation(vol_str);
	}
#undef FRONTEND_AUDIO_MAX
}






void LiveRadioPlayer::OnFrontendChangeMode(bool mode)
{
	if(mode) { // load menu
		//BassAudio::SetPlaySyncPercentVolume(0, BassAudio::gHLiveStream);
		BassAudio::SetLiveRadioPercentVolume(0);
	} else {
		//BassAudio::SetPlaySyncPercentVolume(LiveRadioPlayer::GetFrontendAudioPercentVolume(false), BassAudio::gHLiveStream);
		BassAudio::SetLiveRadioPercentVolume(LiveRadioPlayer::GetFrontendAudioPercentVolume(false));
	}
}

void LiveRadioPlayer::OnFrontendChangeSFX(uint8_t volume) // 0 -127 NU
{

}

void LiveRadioPlayer::OnFrontendChangeMusic(uint8_t volume) // 0 -127
{
	//BassAudio::SetPlaySyncPercentVolume(LiveRadioPlayer::GetFrontendAudioPercentVolume(false), BassAudio::gHLiveStream);
	BassAudio::SetLiveRadioPercentVolume(LiveRadioPlayer::GetFrontendAudioPercentVolume(false));
	if (LiveRadioPlayer::bIsLiveRadioModeActive && (FrontEndMenuManager.m_bMenuActive || FrontEndMenuManager.m_bSaveMenuActive))
	{ // SetPlaySyncPercentVolume
		//BassAudio::SetPlaySyncPercentVolume(0, BassAudio::gHLiveStream);
		BassAudio::SetLiveRadioPercentVolume(0);
	}
}

void LiveRadioPlayer::OnSetInCar(CPed *pPed) // pPed MUST BE PLAYER
{
	CPlayerPed *pPlayer = FindPlayerPed();
	// false try mute => no resave radiooff
	if (pPlayer && (pPlayer == pPed) && LiveRadioPlayer::bIsLiveRadioModeActive)
	{
		if (pPlayer->m_bInVehicle && (!UsesPoliceRadio(pPlayer->m_pVehicle))) { LiveRadioPlayer::SwitchLiveRadio(true, false); }
	}
}

void LiveRadioPlayer::OnSetOutCar(CPed* pPed) // nu
{
	//CPlayerPed* pPlayer = FindPlayerPed();
	// restore orig stantion for car?
}