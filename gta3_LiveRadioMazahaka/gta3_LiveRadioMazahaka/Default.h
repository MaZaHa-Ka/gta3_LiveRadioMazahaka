#pragma once
#include "CEntity.h"
#include "CVehicle.h"

// common.h
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Min(a,b) ((a) < (b) ? (a) : (b))

// get mask for int & BIT(0)
#define BIT(num) (1<<(num))

//enum eVehicleType { //#include "CVehicle.h"
//	VEHICLE_TYPE_CAR,
//	VEHICLE_TYPE_BOAT,
//	VEHICLE_TYPE_TRAIN,
//	VEHICLE_TYPE_HELI,
//	VEHICLE_TYPE_PLANE,
//	VEHICLE_TYPE_BIKE,
//	NUM_VEHICLE_TYPES
//};

enum PedState
{
	PED_NONE,
	PED_IDLE,
	PED_LOOK_ENTITY,
	PED_LOOK_HEADING,
	PED_WANDER_RANGE,
	PED_WANDER_PATH,
	PED_SEEK_POS,
	PED_SEEK_ENTITY,
	PED_FLEE_POS,
	PED_FLEE_ENTITY,
	PED_PURSUE,
	PED_FOLLOW_PATH,
	PED_SNIPER_MODE,
	PED_ROCKET_MODE,
	PED_DUMMY,
	PED_PAUSE,
	PED_ATTACK,
	PED_FIGHT,
	PED_FACE_PHONE,
	PED_MAKE_CALL,
	PED_CHAT,
	PED_MUG,
	PED_AIM_GUN,
	PED_AI_CONTROL,
	PED_SEEK_CAR,
	PED_SEEK_IN_BOAT,
	PED_FOLLOW_ROUTE,
	PED_CPR,
	PED_SOLICIT,
	PED_BUY_ICECREAM,
	PED_INVESTIGATE,
	PED_STEP_AWAY,
	PED_ON_FIRE,

	PED_UNKNOWN,	// Same with IDLE, but also infects up to 5 peds with same pedType and WANDER_PATH, so they become stone too. HANG_OUT in Fire_Head's idb

	PED_STATES_NO_AI,

	// One of these states isn't on PS2 - start
	PED_JUMP,
	PED_FALL,
	PED_GETUP,
	PED_STAGGER,
	PED_DIVE_AWAY,

	PED_STATES_NO_ST,
	PED_ENTER_TRAIN,
	PED_EXIT_TRAIN,
	PED_ARREST_PLAYER,
	// One of these states isn't on PS2 - end

	PED_DRIVING,
	PED_PASSENGER,
	PED_TAXI_PASSENGER,
	PED_OPEN_DOOR,
	PED_DIE,
	PED_DEAD,
	PED_CARJACK,
	PED_DRAG_FROM_CAR,
	PED_ENTER_CAR,
	PED_STEAL_CAR,
	PED_EXIT_CAR,
	PED_HANDS_UP,
	PED_ARRESTED,
};

// and some hardcoded ones
// expand as needed
enum
{
	MI_PLAYER = 0,
	MI_COP,
	MI_SWAT,
	MI_FBI,
	MI_ARMY,
	MI_MEDIC,
	MI_FIREMAN,
	MI_MALE01,
	MI_TAXI_D,
	MI_PIMP,
	MI_GANG01,
	MI_GANG02,
	MI_GANG03,
	MI_GANG04,
	MI_GANG05,
	MI_GANG06,
	MI_GANG07,
	MI_GANG08,
	MI_GANG09,
	MI_GANG10,
	MI_GANG11,
	MI_GANG12,
	MI_GANG13,
	MI_GANG14,
	MI_CRIMINAL01,
	MI_CRIMINAL02,
	MI_SPECIAL01,
	MI_SPECIAL02,
	MI_SPECIAL03,
	MI_SPECIAL04,
	MI_MALE02,
	MI_MALE03,
	MI_FATMALE01,
	MI_FATMALE02,
	MI_FEMALE01,
	MI_FEMALE02,
	MI_FEMALE03,
	MI_FATFEMALE01,
	MI_FATFEMALE02,
	MI_PROSTITUTE,
	MI_PROSTITUTE2,
	MI_P_MAN1,
	MI_P_MAN2,
	MI_P_WOM1,
	MI_P_WOM2,
	MI_CT_MAN1,
	MI_CT_MAN2,
	MI_CT_WOM1,
	MI_CT_WOM2,
	MI_LI_MAN1,
	MI_LI_MAN2,
	MI_LI_WOM1,
	MI_LI_WOM2,
	MI_DOCKER1,
	MI_DOCKER2,
	MI_SCUM_MAN,
	MI_SCUM_WOM,
	MI_WORKER1,
	MI_WORKER2,
	MI_B_MAN1,
	MI_B_MAN2,
	MI_B_MAN3,
	MI_B_WOM1,
	MI_B_WOM2,
	MI_B_WOM3,
	MI_MOD_MAN,
	MI_MOD_WOM,
	MI_ST_MAN,
	MI_ST_WOM,
	MI_FAN_MAN1,
	MI_FAN_MAN2,
	MI_FAN_WOM,
	MI_HOS_MAN,
	MI_HOS_WOM,
	MI_CONST1,
	MI_CONST2,
	MI_SHOPPER1,
	MI_SHOPPER2,
	MI_SHOPPER3,
	MI_STUD_MAN,
	MI_STUD_WOM,
	MI_CAS_MAN,
	MI_CAS_WOM,
	MI_BUSKER1,
	MI_BUSKER2,
	MI_BUSKER3,
	MI_BUSKER4,
	// three more peds possible

	MI_LAST_PED = 89,
	MI_FIRST_VEHICLE,

	MI_LANDSTAL = MI_FIRST_VEHICLE,
	MI_IDAHO,
	MI_STINGER,
	MI_LINERUN,
	MI_PEREN,
	MI_SENTINEL,
	MI_PATRIOT,
	MI_FIRETRUCK,
	MI_TRASH,
	MI_STRETCH,
	MI_MANANA,
	MI_INFERNUS,
	MI_BLISTA,
	MI_PONY,
	MI_MULE,
	MI_CHEETAH,
	MI_AMBULAN,
	MI_FBICAR,
	MI_MOONBEAM,
	MI_ESPERANT,
	MI_TAXI,
	MI_KURUMA,
	MI_BOBCAT,
	MI_MRWHOOP,
	MI_BFINJECT,
	MI_CORPSE,
	MI_POLICE,
	MI_ENFORCER,
	MI_SECURICA,
	MI_BANSHEE,
	MI_PREDATOR,
	MI_BUS,
	MI_RHINO,
	MI_BARRACKS,
	MI_TRAIN,
	MI_CHOPPER,
	MI_DODO,
	MI_COACH,
	MI_CABBIE,
	MI_STALLION,
	MI_RUMPO,
	MI_RCBANDIT,
	MI_BELLYUP,
	MI_MRWONGS,
	MI_MAFIA,
	MI_YARDIE,
	MI_YAKUZA,
	MI_DIABLOS,
	MI_COLUMB,
	MI_HOODS,
	MI_AIRTRAIN,
	MI_DEADDODO,
	MI_SPEEDER,
	MI_REEFER,
	MI_PANLANT,
	MI_FLATBED,
	MI_YANKEE,
	MI_ESCAPE,
	MI_BORGNINE,
	MI_TOYZ,
	MI_GHOST,

	// leftovers on PC
	MI_MIAMI_RCBARON = 154,
	MI_MIAMI_RCRAIDER = 155,
	MI_MIAMI_SPARROW = 159,

	MI_GRENADE = 170,
	MI_AK47,
	MI_BASEBALL_BAT,
	MI_COLT,
	MI_MOLOTOV,
	MI_ROCKETLAUNCHER,
	MI_SHOTGUN,
	MI_SNIPER,
	MI_UZI,
	MI_MISSILE,
	MI_M16,
	MI_FLAMETHROWER,
	MI_BOMB,
	MI_FINGERS,

	MI_CUTOBJ01 = 185,
	MI_CUTOBJ02,
	MI_CUTOBJ03,
	MI_CUTOBJ04,
	MI_CUTOBJ05,

	MI_CAR_DOOR = 190,
	MI_CAR_BUMPER,
	MI_CAR_PANEL,
	MI_CAR_BONNET,
	MI_CAR_BOOT,
	MI_CAR_WHEEL,
	MI_BODYPARTA,
	MI_BODYPARTB,

	MI_AIRTRAIN_VLO = 198,
	MI_LOPOLYGUY,

	NUM_DEFAULT_MODELS,
};

enum eWastedBustedState
{
	WBSTATE_PLAYING,
	WBSTATE_WASTED,
	WBSTATE_BUSTED,
	WBSTATE_FAILED_CRITICAL_MISSION,
};


enum StreamFlags
{
	STREAMFLAGS_DONT_REMOVE = 0x01,
	STREAMFLAGS_SCRIPTOWNED = 0x02,
	STREAMFLAGS_DEPENDENCY = 0x04,	// Is this right?
	STREAMFLAGS_PRIORITY = 0x08,
	STREAMFLAGS_NOFADE = 0x10,

	STREAMFLAGS_CANT_REMOVE = STREAMFLAGS_DONT_REMOVE | STREAMFLAGS_SCRIPTOWNED,
	STREAMFLAGS_KEEP_IN_MEMORY = STREAMFLAGS_DONT_REMOVE | STREAMFLAGS_SCRIPTOWNED | STREAMFLAGS_DEPENDENCY,
};

enum eRaplayMode {
	MODE_RECORD = 0, // playing game
	MODE_PLAYBACK = 1
};

enum FONTFONT
{
	FONT_BANK,
	FONT_PAGER,
	FONT_HEADING,
#ifdef MORE_LANGUAGES
	FONT_JAPANESE,
#endif
	MAX_FONTS
};

enum eRadioStation
{
	HEAD_RADIO,
	DOUBLE_CLEF,
	JAH_RADIO,
	RISE_FM,
	LIPS_106,
	GAME_FM,
	MSX_FM,
	FLASHBACK,
	CHATTERBOX,
	USERTRACK,
	POLICE_RADIO = 10,
	NUM_RADIOS = 10,
	RADIO_OFF = 11,
};

enum eStreamedSounds
{
	STREAMED_SOUND_RADIO_HEAD,
	STREAMED_SOUND_RADIO_CLASSIC,
	STREAMED_SOUND_RADIO_KJAH,
	STREAMED_SOUND_RADIO_RISE,
	STREAMED_SOUND_RADIO_LIPS,
	STREAMED_SOUND_RADIO_GAME,
	STREAMED_SOUND_RADIO_MSX,
	STREAMED_SOUND_RADIO_FLASH,
	STREAMED_SOUND_RADIO_CHAT,
	STREAMED_SOUND_RADIO_MP3_PLAYER,
	STREAMED_SOUND_RADIO_POLICE,
	STREAMED_SOUND_CITY_AMBIENT,
	STREAMED_SOUND_WATER_AMBIENT,
	NO_TRACK = 197,
};

enum eAspectRatio
{
	// Make sure these work the same as FrontEndMenuManager.m_PrefsUseWideScreen
	// without widescreen support
	AR_AUTO,
	AR_4_3,
	AR_5_4,
	AR_16_10,
	AR_16_9,
	AR_21_9,

	AR_MAX,
};


struct tMP3Sample {
	unsigned int m_nLength;
	unsigned int m_nPosition;
	unsigned int m_nLastPosCheckTimer;
};

class cMusicManager {
public:
	bool m_bIsInitialised;
	bool m_bDisabled;
	unsigned char m_nMusicMode;
	unsigned char m_nNextTrack;
	unsigned char m_nPlayingTrack;
	bool m_bFrontendTrackFinished;
	bool m_bPlayInFrontend;
	bool m_bSetNextStation;
	unsigned char m_nAnnouncement;
	bool m_bPreviousPlayerInCar;
	bool m_bPlayerInCar;
	bool m_bAnnouncementInProgress;
	tMP3Sample m_asMP3Samples[196];
	bool m_bResetTimers;
	unsigned int m_nResetTime;
	unsigned int m_nLastTrackServiceTime;
	unsigned int m_nTimer;
	bool m_bDoTrackService;
	bool m_bIgnoreTimeDelay;
	bool m_bDontServiceAmbienceTrack;
	bool m_bRadioSetByScript;
	unsigned char m_nRadioStation;
	int m_nRadioPosition;
	unsigned char m_nRadioInCar;
};

struct tSample {
	unsigned int nOffset;
	unsigned int nSize;
	unsigned int nFrequency;
	unsigned int nLoopStart;
	unsigned int nLoopEnd;
};

class cSampleManager {
public:
	unsigned char m_nEffectsVolume;
	unsigned char m_nMusicVolume;
	unsigned char m_nEffectsFadeVolume;
	unsigned char m_nMusicFadeVolume;
	bool m_nMonoMode;
	char unk;
	char m_szCDRomRootPath[80];
	bool8 m_bInitialised;
	unsigned char m_nNumberOfProviders;
	char* m_aAudioProviders[64];
	tSample m_aSamples[3032];
};

class tSound {
public:
	int32_t m_nEntityIndex;
	uint32_t m_nCounter;
	uint32_t m_nSampleIndex;
	uint8_t m_nBankIndex;
	bool m_bIs2D;
	uint32_t m_nPriority;
	uint32_t m_nFrequency;
	uint32_t m_nVolume;
	float m_fDistance;
	uint32_t m_nLoopCount;

	uint32_t m_nLoopStart;
	int32_t m_nLoopEnd;
	uint8_t m_nEmittingVolume;

	float m_fSpeedMultiplier;
	float m_MaxDistance;

	bool m_bStatic;
	CVector m_vecPos;
	bool m_bReverb;

	uint8_t m_nReflectionDelay;
	bool m_bReflections;

	uint8_t m_nPan;
	uint32_t m_nFramesToPlay;

	bool m_bIsBeingPlayed;
	bool m_bIsPlayingFinished;
	uint32_t m_nFinalPriority;
	int8_t m_nVolumeChange;
};


class cVehicleParams {
public:
	bool m_bDistanceCalculated;
	float m_fDistance;
	CVehicle* m_pVehicle;
	cTransmission* m_pTransmission;
	uint32_t m_nIndex;
	float m_fVelocityChange;
};

struct tVehicleSampleData {
	uint32_t m_nAccelerationSampleIndex;
	uint8_t m_nBank;
	uint32_t m_nHornSample;
	int32_t m_nHornFrequency;
	uint8_t m_nSirenOrAlarmSample;
	int32_t m_nSirenOrAlarmFrequency;
	uint8_t m_bDoorType;
};

enum eAudioType : unsigned int {
	AUDIOTYPE_PHYSICAL = 0,
	AUDIOTYPE_EXPLOSION = 1,
	AUDIOTYPE_FIRE = 2,
	AUDIOTYPE_WEATHER = 3,
	AUDIOTYPE_CRANE = 4,
	AUDIOTYPE_SCRIPTOBJECT = 5,
	AUDIOTYPE_BRIDGE = 6,
	AUDIOTYPE_COLLISION = 7,
	AUDIOTYPE_FRONTEND = 8,
	AUDIOTYPE_PROJECTILE = 9,
	AUDIOTYPE_GARAGE = 10,
	AUDIOTYPE_FIREHYDRANT = 11,
	AUDIOTYPE_WATERCANNON = 12,
	AUDIOTYPE_POLICERADIO = 13,
	TOTAL_AUDIO_TYPES = 14
};

class tAudioEntity {
public:
	eAudioType m_nType;
	void* m_pEntity;
	bool m_bIsUsed;
	bool m_bStatus;
	int16_t m_awAudioEvent[4];
	float m_afVolume[4];
	uint8_t m_AudioEvents;
};

class tPedComment {
public:
	uint32_t m_nSampleIndex;
	int32_t m_nEntityIndex;
	CVector m_vecPos;
	float m_fDistance;
	uint8_t m_nVolume;
	int8_t m_nLoadingTimeout;
};

class cPedComments {
public:
	tPedComment m_aPedCommentQueue[2][20];
	uint8_t m_aPedCommentOrderList[2][20];
	uint8_t m_nPedCommentCount[2];
	uint8_t m_nActiveQueue;
};

class cPoliceRadioQueue {
public:
	uint32_t m_aSamples[60];
	uint8_t m_nSamplesInQueue;
	uint8_t m_nAddOffset;
	uint8_t m_nRemoveOffset;
};


struct cAMCrime {
	int32_t type;
	CVector position;
	uint16_t timer;
};


class cAudioCollision {
public:
	CEntity* m_pEntity1;
	CEntity* m_pEntity2;
	uint8_t m_bSurface1;
	uint8_t m_bSurface2;
	float m_fIntensity1;
	float m_fIntensity2;
	CVector m_vecPosition;
	float m_fDistance;
	int32_t m_nBaseVolume;
};

class cAudioCollisionManager {
public:
	cAudioCollision m_asCollisions1[10];
	cAudioCollision m_asCollisions2[10];
	uint8_t m_bIndicesTable[10];
	uint8_t m_bCollisionsInQueue;
	cAudioCollision m_sQueue;
};

class cAudioManager {
public:
	bool m_bIsInitialised;
	bool m_bIsSurround; // unused until VC
	bool m_bReduceReleasingPriority;
	uint8_t m_nActiveSamples;
	bool m_bDoubleVolume; // unused
	bool m_bDynamicAcousticModelingStatus;
	float m_fSpeedOfSound;
	bool m_bTimerJustReset;
	uint32_t m_nTimer;
	tSound m_sQueueSample;
	uint8_t m_nActiveQueue;
	tSound m_aRequestedQueue[2][27];
	uint8_t m_aRequestedOrderList[2][27];
	uint8_t m_nRequestedCount[2];
	tSound m_asActiveSamples[27];
	tAudioEntity m_asAudioEntities[200];
	uint32_t m_aAudioEntityOrderList[200];
	uint32_t m_nAudioEntitiesCount;
	CVector m_avecReflectionsPos[5];
	float m_afReflectionsDistances[5];
	struct {
		int32_t m_anScriptObjectEntityIndices[40];
		int32_t m_nScriptObjectEntityTotal;
	} m_sAudioScriptObjectManager;
	cPedComments m_sPedComments;
	int32_t m_nFireAudioEntity;
	int32_t m_nWaterCannonEntity;
	int32_t m_nPoliceChannelEntity;
	cPoliceRadioQueue m_sPoliceRadioQueue;
	cAMCrime m_aCrimes[10];
	int32_t m_nFrontEndEntity;
	int32_t m_nCollisionEntity;
	cAudioCollisionManager m_sCollisionManager;
	int32_t m_nProjectileEntity;
	int32_t m_nBridgeEntity;

	// Mission audio stuff
	CVector m_vecMissionAudioPosition;
	bool m_bIsMissionAudio2D;
	uint32_t m_nMissionAudioSampleIndex;
	uint8_t m_nMissionAudioLoadingStatus;
	uint8_t m_nMissionAudioPlayStatus;
	bool m_bIsMissionAudioPlaying;
	int32_t m_nMissionAudioFramesToPlay;
	bool m_bIsMissionAudioAllowedToPlay;

	int32_t m_anRandomTable[5];
	uint8_t m_nTimeSpent;
	bool m_bIsPaused;
	bool m_bWasPaused;
	uint32_t m_FrameCounter;
};