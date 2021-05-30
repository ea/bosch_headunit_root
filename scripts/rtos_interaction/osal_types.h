#include <stdint.h>


typedef struct messageStruct {
    short unknown1;
    short queueNum;
    int numBytes;
    short unknown2;
    char unknown3;
    char msgType;
    short unknownWord4;
    short queueSubId;
    int time;
} MessageStruct ;

// code: 0x0d60, len: 0x04
struct ECORoute {
	uint8_t  FuelType : 1;          // byte 0
	uint8_t  DragCoefficient : 7;
	uint8_t  TransmissionType : 2;  // byte 1
	uint8_t  unused : 6;
	uint16_t crc;                   // byte 2-3
};

// code: 0x0d70, len: 0x18
struct BTName {
	uint8_t  Name[22];
	uint16_t crc;
};



// code: 0x0d5e, len: 0x1b
struct PartsNumber {
	uint8_t  PartNumber[5];
	uint8_t  ConfigurationHash[20];
	uint16_t crc;
};

// code: 0x0d61, len: 0x04
struct CameraSystem {
	uint8_t  CameraSystem : 3;        // byte 0
	uint8_t  AnticipatoryLine : 1;
	uint8_t  unused : 4;
	uint8_t  Guideline;               // byte 1
	uint16_t crc;                     // byte 2-3
};

// code: 0x0d59, len: 0x0a
struct VehicleInformation {
	uint8_t  unknown1;                        // byte 0
	uint16_t VehicleType;                     // byte 1-2
	uint8_t  CANGeneration1 : 1;              // byte 3
	uint8_t  Region : 4;
	uint8_t  SteeringWheel : 3;
	uint8_t  SteeringPosition : 1;            // byte 4
	uint8_t  unknown2 : 1;
	uint8_t  ShowClock : 1;
	uint8_t  MeterClockSynchronisation : 1;
	uint8_t  PlatformType : 1;
	uint8_t  DistanceUnitsSupported : 2;
	uint8_t  unknown3 : 1;
	uint8_t  OpeningAnimation : 2;            // byte 5
	uint8_t  unknown4 : 6;
	uint8_t  OffRoadInformation : 1;          // byte 6
	uint8_t  unknown5 : 1;
	uint8_t  VehicleConfiguration : 1;
	uint8_t  HEV1 : 1;
	uint8_t  AntiTheft : 1;
	uint8_t  DrivingType : 2;
	uint8_t  JudgementForOP : 1;
	uint8_t  VoiceRecognition : 1;            // byte 7
	uint8_t  unknown6 : 1;
	uint8_t  CAN : 1;
	uint8_t  AUXKind1 : 2;
	uint8_t  ISA : 1;
	uint8_t  unknown7 : 2;
	uint16_t crc;                             // byte 8-9
};
enum Regions {USA = 1, CAN, EUR, PRC, GCC, RUS, ASR, ARG, BRA, SAF, MEX, THI, TKY};

// code: 0x0106, len: 0x07
uint8_t DeviceSerialNumber[7];

// code: 0x1216, len: 0x10
uint8_t CryptedFileAesKey[16];

// code: 0x0104, len: 0x08
uint8_t TelematicsPassword[8];

// code: 0x0d53, len: 0x03
struct SystemConfiguration2formerAudio {
	uint8_t  unknown : 4;
	uint8_t  AudioOutput : 4;
	uint16_t crc;
};

// code: 0x0d52, len: 0x41
struct SystemConfiguration1 {
	uint8_t  Tag[8];                         // byte 0-7
	uint8_t  MajorVersion;                   // byte 8
	uint8_t  MinorVersion[2];                // byte 9-10
	uint8_t  Language;                       // byte 11
	uint8_t  KindOfDestinationEntry : 4;     // byte 12
	uint8_t  unknown2 : 4;
	uint8_t  UserKindOfPOIWarning : 1;       // byte 13
	uint8_t  MapZoom : 1;
	uint8_t  unknown3 : 4;
	uint8_t  SpeedLock : 1;
	uint8_t  Disclaimer : 1;
	uint8_t  unknown4 : 3;                   // byte 14
	uint8_t  HMIEV : 1;
	uint8_t  FrequencyPSDisplay : 1;
	uint8_t  ShowFMStationList : 1;
	uint8_t  ECOFeedback : 1;
	uint8_t  SoundOnEncoderPress : 1;
	uint8_t  unknown5;                       // byte 15
	uint8_t  unknown6 : 6;                   // byte 16
	uint8_t  VoiceRecognitionHandling : 1;
	uint8_t  GUIHandling : 1;
	uint8_t  unknown7;                       // byte 17
	uint8_t  ECOScoreParameter1[4];          // byte 18-21
	uint8_t  ECOScoreParameter2[4];          // byte 22-25
	uint8_t  ECOScoreParameter3[4];          // byte 26-29
	uint8_t  ECOScoreParameter4[4];          // byte 30-33
	uint8_t  unknown8[4];                    // byte 34-37
	uint8_t  Brightness;                     // byte 38
	uint8_t  unknown9 : 4;                   // byte 39
	uint8_t  SoundParameterSetSelection : 4;
	uint8_t  unknown10 : 6;                  // byte 40
	uint8_t  SmartphoneIntegration : 1;
	uint8_t  Pandora : 1;
	uint8_t  unknown11 : 7;                  // byte 41
	uint8_t  AutomaticPhoneBookTransfer : 1;
	uint8_t  unknown12 : 5;                  // byte 42
	uint8_t  MinimizeUnpavedRoads : 1;
	uint8_t  GetPositioningSystem : 2;       // *** see note ***
	uint8_t  unknown13 : 7;                  // byte 43
	uint8_t  Dab : 1;
	uint8_t  unknown14[19];                  // byte 44-62
	uint16_t crc;                            // byte 63-64
};


// code: 0x0d5b & 0x0d5c, len: 0xfd (see notes)
struct DABParameterSet {
	uint8_t  Tag[8];
	uint8_t  MajorVersion;
	uint8_t  MinorVersion[2];
	uint8_t  unknown[240];
	uint16_t crc;
};

// code: 0x0d30 - 0x0d38, len: 0x080d (see notes)
struct SDSECNR {
	uint8_t  Tag[8];
	uint8_t  MajorVersion;
	uint8_t  MinorVersion[2];
	uint8_t  unknown[2048];
	uint16_t crc;
};

