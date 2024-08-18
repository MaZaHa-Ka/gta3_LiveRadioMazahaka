#pragma once

#include "Windows.h"
#include <string>
#include <vector>

// bass
#include "../Bass/bass.h"
#pragma comment(lib, "bass.lib")

#define LOG_AUIDIO_LOGIC_
#define USE_3D_BASS false // TODO
#define NAMESPACE_AUDIO
//#define INLINE_FUNCS // видел оишбки с неразрешенным символом

#ifdef NAMESPACE_AUDIO
namespace BassAudio
{
#endif

//----------------------------------------------------FILE---TOOLS-------
bool
AU_FileExists(std::string &filePath);
std::vector<std::string>
AU_FileReadAllLines(std::string filePath);
std::string
AU_FileReadAllText(std::string filePath);
std::vector<std::string>
LoadAllFilesFromFolder(std::string folderPath, std::string ext = "ogg");
//---------------------------------------------------!FILE---TOOLS------------------------

//--------------------------SOUND---PLAY-------------
extern std::string gsCurrDir; // a\b\ 
extern std::vector<std::string> gvAudioPaths; // ~3sec
extern HSAMPLE gStream;       // текущий поток
extern bool gStreamNeed;      // play/pause true, release false

bool
IsBASSInited();
bool
Init_SoundSystem(bool use3d = USE_3D_BASS);
void
StopStream(HSTREAM& Stream = gStream);
bool
IsSomethingPlaying(HSTREAM &Stream = gStream, bool checkpause = true);
bool
isTrackEnded(HSTREAM& Stream = gStream);
DWORD
GetStreamStatus(HSTREAM& Stream = gStream);
QWORD
GetStreamPosition(HSTREAM& Stream = gStream);
void
SetStreamPosition(HSTREAM& Stream = gStream, QWORD position = 0);
//void
//PauseStream(HSTREAM& Stream = gStream);
//void
//ResumeStream(HSTREAM& Stream = gStream);
void
SwitchStreamPauseMode(HSTREAM& Stream = gStream, bool bPause = true);
void
SetPlaySyncPercentVolume(int percent_volume, HSTREAM& Stream = gStream);
void
PlaySync(std::string sPath, int percent_vol = 20, HSTREAM& Stream = gStream);
void
PlayAsync(std::string sPath, int percent_vol = 20);
void
Play(std::string sPath, int percent_vol = 20, bool bSync = true, HSTREAM& Stream = gStream);
bool
Init_SoundSystem_WithPath(std::string sSoundDir = "");

//----test block
void
ApplyMyEchoEffect(HSTREAM& stream);
void
ApplyMyParamEqEffect(HSTREAM& stream);
//--------!test

//---live radio streaming block
extern HSTREAM gHLiveStream;
extern bool gbIsPlayingLiveStream;
extern int giLiveStreamPercentVolume;

void
SetLiveRadioPercentVolume(int percent_volume);

#ifdef INLINE_FUNCS
inline
#endif
int
getLiveRadioPercentVolume();

#ifdef INLINE_FUNCS
inline
#endif
bool
IsLiveRadioAudible(bool check_mute = false);

#ifdef INLINE_FUNCS
inline
#endif
void
StopLiveRadio();

bool
SetLiveRadioStation(const char *url, int percent_volume = -1);

// extern volatile bool isSLRSARunning; // не нужны они глобально
// extern HANDLE hSLRSAEvent;
void
SetLiveRadioStationAsync(const char *url, int percent_volume = -1);
void
FreeSoundSystem();

#ifdef NAMESPACE_AUDIO
}
#endif