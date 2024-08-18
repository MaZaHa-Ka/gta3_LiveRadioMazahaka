#pragma once
#include "BassAudio.h"
#include <iostream>
#include <fstream>
#include "common.h"
#include <mutex>
#include <thread>
//#include "../utils/SetCurrDir.h"

#ifdef NAMESPACE_AUDIO
namespace BassAudio {
#endif


//----------------------------------------------------FILE---TOOLS-------
bool
AU_FileExists(std::string &filePath)
{
	std::ifstream file(filePath);
	return file.good();
}

std::vector<std::string>
AU_FileReadAllLines(std::string filePath)
{
	// if (is_valid_utf8(filePath)) { filePath = utf8_to_cp1251(filePath); }

	std::vector<std::string> lines;
	std::ifstream file(filePath);

	if(!file.is_open()) { return lines; }

	std::string line;
	while(std::getline(file, line)) { lines.push_back(line); }

	file.close();
	return lines;
}

std::string
AU_FileReadAllText(std::string filePath)
{
	// if (is_valid_utf8(filePath)) { filePath = utf8_to_cp1251(filePath); }

	std::ifstream file(filePath);
	std::string content;

	if(!file.is_open()) {
		// std::cerr << "Error opening the file." << std::endl;
		return content; // Return an empty string in case of an error.
	}

	std::string line;
	while(std::getline(file, line)) {
		content += line + '\n'; // Concatenate the line and a newline character.
	}

	file.close();
	return content;
}

std::vector<std::string>
LoadAllFilesFromFolder(std::string folderPath, std::string ext)
{
	if(folderPath.back() != '\\' && folderPath.back() != '/') { folderPath += "\\"; }

	std::vector<std::string> oggFiles; // ������� ������ ��� �������� ����� � .ogg ������
	WIN32_FIND_DATA findFileData;
	// HANDLE hFind = FindFirstFile((folderPath + "*.ogg").c_str(), &findFileData);
	HANDLE hFind = FindFirstFile((folderPath + "*." + ext).c_str(), &findFileData);

	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			std::string filePath = folderPath + findFileData.cFileName;
			oggFiles.push_back(filePath);
			// std::cout << "ScanFile: " << filePath << "\n";
		} while(FindNextFile(hFind, &findFileData) != 0);

		FindClose(hFind);
	}

	return oggFiles;
}
//---------------------------------------------------!FILE---TOOLS------------------------

//--------------------------SOUND---PLAY-------------
std::string gsCurrDir = ""; // a\b\ 
std::vector<std::string> gvAudioPaths; // ~3sec

HSAMPLE gStream = 0;      // ������� �����
bool gStreamNeed = false; // play/pause true, release false

#ifdef INLINE_FUNCS
inline
#endif
bool
IsBASSInited()
{
	return !(BASS_GetDevice() == -1);
}

bool
Init_SoundSystem(bool use3d) // 3d todo funcs
{
	if(IsBASSInited()) { return true; }
	if((BASS_Init(-1, 44100, use3d ? BASS_DEVICE_3D : 0, 0, NULL) == FALSE)) { return false; }
	// if ((BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL) == FALSE)) { return false; }
	// if ((BASS_Init(-1, 44100, 0, 0, NULL) == FALSE)) { return false; }

	// if ((BASS_GetDevice() == -1) && (BASS_Init(-1, 44100, 0, 0, NULL) == FALSE)) { return false; }
	// if (BASS_Init(-1, 44100, 0, 0, NULL) == FALSE) { return false; }
	return true;
}

// ������� ��� ��������� �������� ������ � ������������ ��������
// todo list streams not clear async
void
StopStream(HSTREAM& Stream)
{
	if(Stream != 0) {
		BASS_ChannelStop(Stream);  // ��������� ���������������
		BASS_SampleFree(Stream);  // ������������ ��������
		Stream = 0;                // ����� �������� ������
	}
}

bool
IsSomethingPlaying(HSTREAM& Stream, bool checkpause)
{
	if(Stream == 0) return false;
	DWORD status = BASS_ChannelIsActive(Stream);
	return ((status == BASS_ACTIVE_PLAYING) || (checkpause && (status == BASS_ACTIVE_PAUSED)));
}

bool
isTrackEnded(HSTREAM& Stream)
{
	if(Stream == 0) { return true; }
	DWORD status = BASS_ChannelIsActive(Stream);
	QWORD trackPos = BASS_ChannelGetPosition(Stream, BASS_POS_BYTE);
	QWORD trackLen = BASS_ChannelGetLength(Stream, BASS_POS_BYTE);
	return (trackPos >= trackLen) && (status == BASS_ACTIVE_STOPPED);
}

DWORD
GetStreamStatus(HSTREAM& Stream)
{
	DWORD status = BASS_ChannelIsActive(Stream);
	return status;
}

QWORD
GetStreamPosition(HSTREAM& Stream)
{
	QWORD position = BASS_ChannelGetPosition(Stream, BASS_POS_BYTE);
	return position;
	//BASS_ChannelSetPosition(Stream, position, BASS_POS_BYTE);
}

void
SetStreamPosition(HSTREAM& Stream, QWORD position)
{
	//QWORD position = BASS_ChannelGetPosition(Stream, BASS_POS_BYTE);
	BASS_ChannelSetPosition(Stream, position, BASS_POS_BYTE);
}

void
PauseStream(HSTREAM& Stream)
{
	//if(!IsSomethingPlaying(Stream)) { return; }
	//if(!isTrackEnded(Stream)) { BASS_ChannelPlay(Stream, FALSE); } // ������� �������
	if(Stream != 0) {
		BASS_ChannelPause(Stream); // ����� ���������������
	}
}

void
ResumeStream(HSTREAM& Stream)
{
	//if(!IsSomethingPlaying(Stream)) { return; }
	if(!isTrackEnded(Stream)) { BASS_ChannelPlay(Stream, FALSE); } // ������� �������
	//if(Stream != 0) {
	//	BASS_ChannelPlay(Stream, FALSE); // ����������� ���������������, ���� BASS_ChannelIsActive() status BASS_ACTIVE_STOPPED ����� ������
	//}
}

void
SwitchStreamPauseMode(HSTREAM& Stream, bool bPause)
{
	if(bPause) { PauseStream(Stream); }
	else{ ResumeStream(Stream); }
}

void
SetPlaySyncPercentVolume(int percent_volume, HSTREAM& Stream)
{
	if(percent_volume < 0) { percent_volume = 0; }
	if(percent_volume > 100) { percent_volume = 100; }
	if(Stream) { BASS_ChannelSetAttribute(Stream, BASS_ATTRIB_VOL, percent_volume / 100.0f); }
}

void
PlaySync(std::string sPath, int percent_vol, HSTREAM& Stream)
{
	if((!IsBASSInited()) && (!Init_SoundSystem())) {
#ifdef LOG_AUIDIO_LOGIC
		std::cout << "! IsBASSInited && !Init_SoundSystem PlayAsync()"
		          << "\n";
#endif
		return;
	}

	//CFileMgr::SetDir("");
	//U_SetCurrentDirectory();

#ifdef LOG_AUIDIO_LOGIC
	std::cout << "Play(sPath) File Exists?: " << FileExists(sPath) << "\n";
#endif
	if(!AU_FileExists(sPath)) {
		std::cout << "!Play file"
		          << "\n";
		return;
	}

	StopStream(Stream); // ��������� �������� ������ ����� ���������������� ������

	// playing
	Stream = BASS_StreamCreateFile(FALSE, sPath.c_str(), 0, 0, BASS_STREAM_PRESCAN);           // �������� ������ ������
	if(Stream == 0) { return; }                                                                // error
	BASS_ChannelSetAttribute(Stream, BASS_ATTRIB_VOL, (float)((percent_vol % 101) / 100.0f));  // volume (vol 0-100)
	BASS_ChannelPlay(Stream, FALSE);                                                           // ������ ���������������
	// TODO List play with can removable?

	BASS_ChannelRemoveFX(Stream, -1); // ������� ��������
}

// todo global stream, semaphore (queue?), pause? free
void
PlayAsync(std::string sPath, int percent_vol)
{
	if((!IsBASSInited()) && (!Init_SoundSystem())) {
#ifdef LOG_AUIDIO_LOGIC
		std::cout << "! IsBASSInited && !Init_SoundSystem PlayAsync()"
		          << "\n";
#endif
		return;
	}

#ifdef LOG_AUIDIO_LOGIC
	std::cout << "Play(sPath) File Exists?: " << FileExists(sPath) << "\n";
#endif
	if(!AU_FileExists(sPath)) {
		std::cout << "!Play file"
		          << "\n";
		return;
	}

	// playing
	HSAMPLE stream = BASS_StreamCreateFile(FALSE, sPath.c_str(), 0, 0, BASS_STREAM_PRESCAN);  // stream
	if(stream == 0) { return; }                                                               // error
	BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, (float)((percent_vol % 101) / 100.0f)); // volume (vol 0-100)
	BASS_ChannelPlay(stream, FALSE);                                                          // ������ ���������������
	// TODO List play with can removable?

	BASS_ChannelRemoveFX(stream, -1); // ������� ��������
	BASS_SampleFree(stream);          // ������������ �������� ����� ���������������
}

void
Play(std::string sPath, int percent_vol, bool bSync, HSTREAM& Stream)
{
	if(bSync) {
		PlaySync(sPath, percent_vol, Stream);
	} else {
		PlayAsync(sPath, percent_vol);
	}
}

bool
Init_SoundSystem_WithPath(std::string sSoundDir)
{
	if(!Init_SoundSystem()) { return false; }
	// if ((BASS_GetDevice() == -1) && (BASS_Init(-1, 44100, 0, 0, NULL) == FALSE)) { return false; }
	// if (BASS_Init(-1, 44100, 0, 0, NULL) == FALSE) { return false; }
	if(sSoundDir != "") {
		gvAudioPaths = LoadAllFilesFromFolder(sSoundDir);
#ifdef LOG_AUIDIO_LOGIC
		std::cout << "gvAudioPaths.size(): " << gvAudioPaths.size() << "\n";
#endif
	} else {
#ifdef LOG_AUIDIO_LOGIC
		std::cout << "Init_SoundSystem() OK INIT"
		          << "\n";
#endif
	}
	return true;
}
//--------------------------SOUND---PLAY-------------


//----test block
//#include "bass_fx.hpp"
void
ApplyMyEchoEffect(HSTREAM& stream)
{
	if(!stream) { return; }

	HFX reverbFX = BASS_ChannelSetFX(stream, BASS_FX_DX8_REVERB, 0);
	if(reverbFX == 0) {
		std::cerr << "�� ������� ���������� ������ ������������: " << BASS_ErrorGetCode() << std::endl;
		return;
	}

	// ��������� ���������� ������������
	BASS_DX8_REVERB reverb;
	// reverb.fInGain = 0.5f;       // ������� �������
	// reverb.fReverbMix = 0.5f;    // ����������� ������������
	// reverb.fReverbTime = 1.0f;   // ����� ������������
	// reverb.fHighFreqRTRatio = 0.5f; // ��������������� �����������

	// reverb.fInGain = 0.8f;       // ������� �������
	// reverb.fReverbMix = 0.8f;    // ����������� ������������
	// reverb.fReverbTime = 0.8f;   // ����� ������������
	// reverb.fHighFreqRTRatio = 0.3f; // ��������������� �����������

	reverb.fInGain = 0.0f;    // ������� �������
	reverb.fReverbMix = 0.2f; // ����������� ������������
	// reverb.fReverbTime = 200.8f;   // ����� ������������
	// reverb.fHighFreqRTRatio = 200.0f; // ��������������� �����������

	// ���������� ����������
	if(!BASS_FXSetParameters(reverbFX, &reverb)) {
		// ��������� ������
		std::cerr << "�� ������� ��������� ��������� ������������: " << BASS_ErrorGetCode() << std::endl;
	}
}
void
ApplyMyParamEqEffect(HSTREAM& stream)
{
	if(!stream) { return; }

	HFX paramEqFX = BASS_ChannelSetFX(stream, BASS_FX_DX8_PARAMEQ, 0);
	if(paramEqFX == 0) {
		std::cerr << "�� ������� ���������� ��������������� ����������: " << BASS_ErrorGetCode() << std::endl;
		return;
	}

	BASS_DX8_PARAMEQ paramEq;
	paramEq.fCenter = 50.0f;   // ����������� �������
	paramEq.fBandwidth = 5.0f; // ������ �����������
	paramEq.fGain = 200.0f;    // ��������

	if(!BASS_FXSetParameters(paramEqFX, &paramEq)) {
		std::cerr << "�� ������� ��������� ��������� ���������������� �����������: " << BASS_ErrorGetCode() << std::endl;
	}
}

//--------!test


//---live radio streaming bolck
HSTREAM gHLiveStream;
bool gbIsPlayingLiveStream = false;
int giLiveStreamPercentVolume = 20;
//static std::string gscurr_live_radio = "";

void
SetLiveRadioPercentVolume(int percent_volume)
{
	if(gHLiveStream) {
		if(percent_volume < 0) { percent_volume = 0; }
		if(percent_volume > 100) { percent_volume = 100; }
		giLiveStreamPercentVolume = percent_volume;
		BASS_ChannelSetAttribute(gHLiveStream, BASS_ATTRIB_VOL, giLiveStreamPercentVolume / 100.0f);
	}
}

#ifdef INLINE_FUNCS
inline
#endif
    int
    getLiveRadioPercentVolume()
{
	return giLiveStreamPercentVolume;
}

#ifdef INLINE_FUNCS
inline
#endif
    bool
    IsLiveRadioAudible(bool check_mute)
{
	if(check_mute && (!getLiveRadioPercentVolume())) { return false; }
	return gbIsPlayingLiveStream && BASS_ChannelIsActive(gHLiveStream) == BASS_ACTIVE_PLAYING;
}

#ifdef INLINE_FUNCS
inline
#endif
    void
    StopLiveRadio()
{
	if(gHLiveStream) {
		BASS_ChannelStop(gHLiveStream);
		BASS_StreamFree(gHLiveStream);
		gHLiveStream = NULL;
		//gscurr_live_radio = "";
		gbIsPlayingLiveStream = false;
	}
}

// ������� DSP
// void CALLBACK DSPPROC(HDSP handle, DWORD channel, void* buffer, DWORD length, void* user) {}

// ������� ��� ��������� ������� (URL ������)
bool
SetLiveRadioStation(const char *url, int percent_volume /*, bool tmptest*/) // SYNC!!!!! IF START ON FRAME => THIS FRAME IS BEEN LAG!!
{
	if((!IsBASSInited()) && (!Init_SoundSystem())) { return false; }
	StopLiveRadio();
	if(!url) { return false; } // stop and no start [0] \0
	if(gbIsPlayingLiveStream) { return false; } //?
	// if (gHLiveStream) { StopLiveRadio(); }
	// if (gHLiveStream) { BASS_StreamFree(gHLiveStream); }
	//gscurr_live_radio = url;
	gHLiveStream = BASS_StreamCreateURL(url, 0, BASS_STREAM_BLOCK | BASS_STREAM_AUTOFREE, 0, 0);
	if(!gHLiveStream) {
#ifdef LOG_AUIDIO_LOGIC
		std::cerr << "������ �������� ������: " << BASS_ErrorGetCode() << std::endl;
#endif
		return false;
	}
	/*if(strcmp(gscurr_live_radio.c_str(), url)) // async run
	{
		BASS_ChannelStop(gHLiveStream);
		BASS_StreamFree(gHLiveStream);
		return false;
	}*/

	// test reverb
	// HDSP hHDSP = BASS_ChannelSetDSP(gHLiveStream, DSPPROC, 0, 0);
	// BASS_ChannelRemoveDSP(gHLiveStream, hHDSP);
	// hHDSP = BASS_ChannelSetFX(gHLiveStream, BASS_FX_BFX_REVERB, 0);
	// BASS_ChannelRemoveFX(gHLiveStream, hHDSP);

	// ���������� ��������� ��������� ����� � 3D ������������
	// BASS_3DVECTOR pos = { 875.2f, -306.7f, 8.6f }; // ������� ��������� �����
	// BASS_Set3DFactors(1.0, 1.0, 1.0); // ��������� �������� ����������, ����-��� � �������
	// BASS_ChannelSet3DPosition(
	//	gHLiveStream,
	//	&pos,					   // ������� ��������� �����
	//	NULL,                     // ��� ������������� ������������� ������ ����������� ������
	//	NULL                      // ��� ������������� ������������� ������ ����������� �����
	//);
	// BASS_Apply3D(); // ��������� ���������

	// if (tmptest)
	//{
	//	//ApplyMyEchoEffect(gHLiveStream);
	//	//ApplyMyParamEqEffect(gHLiveStream);

	//	//ApplyChorusEffect(gHLiveStream);
	//	//ApplyCompressorEffect(gHLiveStream);
	//	//ApplyDistortionEffect(gHLiveStream);
	//	//ApplyEchoEffect(gHLiveStream);
	//	//ApplyFlangerEffect(gHLiveStream);
	//	//ApplyGargleEffect(gHLiveStream);
	//	//ApplyI3DL2ReverbEffect(gHLiveStream);
	//	//ApplyParamEqEffect(gHLiveStream);
	//	//ApplyReverbEffect(gHLiveStream);
	//	//ApplyVolumeEffect(gHLiveStream);
	//}

	// ApplyMyEchoEffect(gHLiveStream);

	SetLiveRadioPercentVolume((percent_volume < 0) ? giLiveStreamPercentVolume : percent_volume);
	BASS_ChannelPlay(gHLiveStream, FALSE);
	/*if(strcmp(gscurr_live_radio.c_str(), url)) // async run
	{
		BASS_ChannelStop(gHLiveStream);
		BASS_StreamFree(gHLiveStream);
		return false;
	}*/
	gbIsPlayingLiveStream = true;
	return true;
}
// async start
/*struct RadioStationArgs { const char* url; int percent_volume; }; // 4 CreateThread
DWORD CALLBACK SetLiveRadioStationEntry(LPVOID lpParameter)
{
        RadioStationArgs* args = static_cast<RadioStationArgs*>(lpParameter);
        if (args) { SetLiveRadioStation(args->url, args->percent_volume); delete args; }
        return TRUE;
}
void SetLiveRadioStationAsync(const char* url, int percent_volume = -1)
{
        RadioStationArgs* args = new RadioStationArgs{ url, percent_volume };
        HANDLE threadHandle = CreateThread(NULL, 0, SetLiveRadioStationEntry, args, 0, NULL);
}*/
/*void SetLiveRadioStationAsync(const char* url, int percent_volume = -1)
{
        struct RadioStationArgs { const char* url; int percent_volume; };
        auto SetLiveRadioStationEntry = [](LPVOID lpParameter) -> DWORD
        {
                RadioStationArgs* args = static_cast<RadioStationArgs*>(lpParameter);
                if (args) { SetLiveRadioStation(args->url, args->percent_volume); delete args; }
                return TRUE;
        };

        RadioStationArgs* args = new RadioStationArgs{ url, percent_volume };
        HANDLE hThread = CreateThread(NULL, 0, SetLiveRadioStationEntry, args, 0, NULL);
        if (hThread) { CloseHandle(hThread); }
}*/

volatile bool isSLRSARunning = false;
HANDLE hSLRSAEvent;
void
SetLiveRadioStationAsync(const char *url, int percent_volume)
{
	struct RadioStationArgs {
		const char *url;
		int percent_volume;
	};

	auto SetLiveRadioStationEntry = [](LPVOID lpParameter) -> DWORD {
		RadioStationArgs *args = static_cast<RadioStationArgs *>(lpParameter);
		if(args) {
			SetLiveRadioStation(args->url, args->percent_volume);
			delete args;
		}
		isSLRSARunning = false;
		SetEvent(hSLRSAEvent);
		return TRUE;
	};

	/*while(isSLRSARunning) {
	        WaitForSingleObject(hSLRSAEvent, INFINITE);
	        Sleep(0);
	}*/

	isSLRSARunning = true;
	hSLRSAEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	RadioStationArgs *args = new RadioStationArgs{url, percent_volume};
	HANDLE hThread = CreateThread(NULL, 0, SetLiveRadioStationEntry, args, 0, NULL);
	if(hThread) { CloseHandle(hThread); }
}

void
FreeSoundSystem() // dont work
{
	StopStream(); // ��������� � ������������ �������� ������
	StopLiveRadio();
	BASS_Free(); // ������������ ���� �������� ���������� BASS
}
//-------end live radio streaming block


#ifdef NAMESPACE_AUDIO
}
#endif