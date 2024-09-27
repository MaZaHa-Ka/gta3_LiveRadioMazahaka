#pragma once
//#include "CFont.h" // Default.h enums
#include "Windows.h"
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>

#include "BassAudio.h"
#include "../Default.h"

#include "plugin.h"
using namespace plugin;

class M3UNodeMazahaka
{
public:
	std::string name;
	std::string url;
	bool is_live;
	M3UNodeMazahaka(const std::string &name, const std::string &url, const bool &is_live);
};

// need hooks:
//cMusicManager::ServiceGameMode()  retune
// game ontick
// fronted esc/sfx/music
// OnPedSetInCarCBAfter for turn on

// TODO IF MAZAHAKA_EXTERNAL_URL_RADIO in sit car no start radio!!!!
#define GTA3_RADIO_DARW_NAME_TIME 1.0f // 1.0f сек на серую и 1.0f сек на белую всего 2сек имя радио
#define VOLUME_RETUNE // todo
//#define NOT_ALLOW_SWITCH_TO_LR_IF_VOL_ZERO

class LiveRadioPlayer
{
public:
	struct CurrentStationStates { // TODO CHECK DRAW IF IN VEHICLE
		std::string str2print;
		bool bIslight_color;
		bool need_bg_name_black;
		bool need_print;
		//float time_buffer_draw_name = 0.0f;
		std::chrono::time_point<std::chrono::steady_clock> start_time;
		FONTFONT font; // FONT_HEADING orig, FONT_BANK work
		CRGBA dark_col;
		CRGBA light_col;
	};

	static std::string m_sRadioListPath;
	static char m_cKeySwitch;
	//---vol
#ifdef VOLUME_RETUNE
	static char m_cVolRetuneKeySwitch;
	static int m_iVolRetunePercent;
	static int m_iVolRetunePercentStep;
#endif
	//-----
	static bool m_bSwitcherHoldFlag;


	static bool bIsLRInitalize;
	static bool bIsLiveRadioModeActive;
	static bool bIsLiveRadioModeNeedPrintText;
	static CurrentStationStates LiveRadioState; // info for music manager draw name
	static int m_iCurrentRadioPos;
	static std::vector<M3UNodeMazahaka> live_radios;

	static int m_iNextRadioPos;
	static volatile bool isStationChanging;
	static std::thread radioSwitchThread;

	// stored
	static int m_iOldStreamedPlayingTrack;
	//static int m_iOldStreamedNextTrack;
	static int m_iOldRadioStation;
	//static int m_iOldRadioInCar; // 127
	//static int m_iOldRadioPos; // todo stored cvehicle pointer?


	// funcs
	static void RadioSwitchingThread();
	static void SwitchStation(int index);

	static void InitLiveRadios(std::string sM3UPath);
	static CurrentStationStates GetLiveRadioState(); // удобнее понять вызов ondraw
	static void SetRetune(bool scrl_up_next);
	static std::vector<M3UNodeMazahaka> LiveRadioParse(const std::string &filePath);
	static std::vector<M3UNodeMazahaka> GetFilteredStations(const std::vector<M3UNodeMazahaka> &stations, bool is_live);

	static void SwitchLiveRadio(bool mode, bool need_save_old_radio = true);
	static void OnTick();
	static void RestartDrawCurrentRadioStation(std::string new_str2print);
	static void ResetDefaultPrintRadioNameData(); // timers. pointers etc

	// default radio funcs
	// or on OnSetOutCar when bIsLiveRadioModeActive restore default radio
	static void TryMuteVehRadio(bool need_save_old_radio = true); // false when sit in car with radio off and live on. rewrite oldradio on radio off
	static void RestoreRadioInCar();
	static bool IsAllowedRetuneDefaultRadio();
	static void SetNewVol(int percent_vol, bool bSFX = false);

	// helpers
	static int GetFrontendAudioPercentVolume(bool bSFX = false);
	static void OnFrontendChangeMode(bool mode);
	static void OnFrontendChangeSFX(uint8_t volume); // nu
	static void OnFrontendChangeMusic(uint8_t volume);
	static void OnSetInCar(CPed* pPed);
	static void OnSetOutCar(CPed* pPed);

//private:
	static std::string trim(const std::string &str);
};