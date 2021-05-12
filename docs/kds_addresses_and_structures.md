# KDS Device details 

OSAL library provides an interface to talk to certain devices exposed from RTOS. One of them is `/dev/kds` and sample code for interacting with it can be found in `https://github.com/ea/bosch_headunit_root/blob/main/scripts/rtos_iosc_testing.c`

This document will aim to enumerate all KDS associated addresses and structures of returned data:

- 0xD59 VehicleInformation
- 0xD61 CameraSystem
- 0xDFE BTName
- 0xD53 SystemConfig2FormerAudio
- 0xD60 ECORoute
- 0xD30-0xD38 SDSECNR (what could this acronym mean?)
- 0xD52 SystemConfiguration
- 0xD40-0xD48 PhoneECNR
- 0xD5D FMAMTunerParameter
- 0xD5F AudioParamterSet
- 0xD5B , 0xD5C - DABParameterSet
- 0xD5E PartsNumber 

Neighbour @raburton has helpfully started to document the structures in the associated issue https://github.com/ea/bosch_headunit_root/issues/13#issuecomment-838215185 
Copying it here for posterity:

**Note:** these structs are for documentation purposes - the order of bit field storage is not defined in the C standard and is implementation specific. The structs are written as you would read a dump of the data from kds, i.e. the first byte in the structure is the first byte in the data, the first bit in a bit field (as documented below) is the left most bit in the byte containing it.
Your compiler may store bit fields in reverse order, i.e. first bit field below is actually the right most bit in the byte. If this is the case you will need to reverse the order of the any bit fields, within each byte, to use these structs. Also note that your structs will need to be packed with #pragma pack(1).


```
// code: 0x0d60, len: 0x04
struct ECORoute {
	uint8  FuelType : 1;          // byte 0
	uint8  DragCoefficient : 7;
	uint8  TransmissionType : 2;  // byte 1
	uint8  unused : 6;
	uint16 crc;                   // byte 2-3
};
```
Example 20c078ea\
FuelType = 0 (from an unleaded car)\
DragCoefficient = 20\
TransmissionType = 3 (from a manual 6 speed car)\
crc = 0xea78

---

```
// code: 0x0d70, len: 0x18
struct BTName {
	uint8  Name[22];
	uint16 crc;
};
```
Example 4d59434152000000 0000000000000000 0000000000004651\
Name = "MYCAR" (null padded, max length 21 if must be null terminated or 22 if not)\
crc = 0x5146

---

```
// code: 0x0d5e, len: 0x1b
struct PartsNumber {
	uint8  PartNumber[5];
	uint8  ConfigurationHash[20];
	uint16 crc;
};
```
Example 335a4c3742...... ................ ................ ..b0e9\
PartNumber = "3ZL7B" (appears fixed length, not null terminated)\
ConfigurationHash = (hidden, in case unique)\
crc = 0xe9b0

---

```
// code: 0x0d61, len: 0x04
struct CameraSystem {
	uint8  CameraSystem : 3;        // byte 0
	uint8  AnticipatoryLine : 1;
	uint8  unused : 4;
	uint8  Guideline;               // byte 1
	uint16 crc;                     // byte 2-3
};
```
Example 80008b83\
CameraSystem = 4 (from car with birds eye view, 4 camera system)\
AnticipatoryLine = 0\
Guideline = 0\
crc = 0x838b

---

```
// code: 0x0d59, len: 0x0a
struct VehicleInformation {
	uint8  unknown1;                        // byte 0
	uint16 VehicleType;                     // byte 1-2
	uint8  CANGeneration1 : 1;              // byte 3
	uint8  Region : 4;
	uint8  SteeringWheel : 3;
	uint8  SteeringPosition : 1;            // byte 4
	uint8  unknown2 : 1;
	uint8  ShowClock : 1;
	uint8  MeterClockSynchronisation : 1;
	uint8  PlatformType : 1;
	uint8  DistanceUnitsSupported : 2;
	uint8  unknown3 : 1;
	uint8  OpeningAnimation : 2;            // byte 5
	uint8  unknown4 : 6;
	uint8  OffRoadInformation : 1;          // byte 6
	uint8  unknown5 : 1;
	uint8  VehicleConfiguration : 1;
	uint8  HEV1 : 1;
	uint8  AntiTheft : 1;
	uint8  DrivingType : 2;
	uint8  JudgementForOP : 1;
	uint8  VoiceRecognition : 1;            // byte 7
	uint8  unknown6 : 1;
	uint8  CAN : 1;
	uint8  AUXKind1 : 2;
	uint8  ISA : 1;
	uint8  unknown7 : 2;
	uint16 crc;                             // byte 8-9
};
enum Regions {USA = 1, CAN, EUR, PRC, GCC, RUS, ASR, ARG, BRA, SAF, MEX, THI, TKY};
```
Example 00124c19bc400928 8c5d\
...\
Region = 3 (from UK car)\
...\
crc = 0x5d8c

---

```
// code: 0x0106, len: 0x07
uint8 DeviceSerialNumber[7];
```

---

```
// code: 0x1216, len: 0x10
uint8 CryptedFileAesKey[16];
```
Used for .ntq alert files.

---

```
// code: 0x0104, len: 0x08
uint8 TelematicsPassword[8];
```
Not seen yet for real. Password appears to be encrypted/hashed.

---

```
// code: 0x0d53, len: 0x03
struct SystemConfiguration2formerAudio {
	uint8  unknown : 4;
	uint8  AudioOutput : 4;
	uint16 crc;
}
```

Example 01f1e1\
AudioOutput = 1\
crc = 0xe1f1

---

```
// code: 0x0d52, len: 0x41
struct SystemConfiguration1 {
	uint8  Tag[8];                         // byte 0-7
	uint8  MajorVersion;                   // byte 8
	uint8  MinorVersion[2];                // byte 9-10
	uint8  Language;                       // byte 11
	uint8  KindOfDestinationEntry : 4;     // byte 12
	uint8  unknown2 : 4;
	uint8  UserKindOfPOIWarning : 1;       // byte 13
	uint8  MapZoom : 1;
	uint8  unknown3 : 4;
	uint8  SpeedLock : 1;
	uint8  Disclaimer : 1;
	uint8  unknown4 : 3;                   // byte 14
	uint8  HMIEV : 1;
	uint8  FrequencyPSDisplay : 1;
	uint8  ShowFMStationList : 1;
	uint8  ECOFeedback : 1;
	uint8  SoundOnEncoderPress : 1;
	uint8  unknown5;                       // byte 15
	uint8  unknown6 : 6;                   // byte 16
	uint8  VoiceRecognitionHandling : 1;
	uint8  GUIHandling : 1;
	uint8  unknown7;                       // byte 17
	uint8  ECOScoreParameter1[4];          // byte 18-21
	uint8  ECOScoreParameter2[4];          // byte 22-25
	uint8  ECOScoreParameter3[4];          // byte 26-29
	uint8  ECOScoreParameter4[4];          // byte 30-33
	uint8  unknown8[4];                    // byte 34-37
	uint8  Brightness;                     // byte 38
	uint8  unknown9 : 4;                   // byte 39
	uint8  SoundParameterSetSelection : 4;
	uint8  unknown10 : 6;                  // byte 40
	uint8  SmartphoneIntegration : 1;
	uint8  Pandora : 1;
	uint8  unknown11 : 7;                  // byte 41
	uint8  AutomaticPhoneBookTransfer : 1;
	uint8  unknown12 : 5;                  // byte 42
	uint8  MinimizeUnpavedRoads : 1;
	uint8  GetPositioningSystem : 2;       // *** see note ***
	uint8  unknown13 : 7;                  // byte 43
	uint8  Dab : 1;
	uint8  unknown14[19];                  // byte 44-62
	uint16 crc;                            // byte 63-64
}
```

Note: According to the getter method GetPositioningSystem is two bits, one of which overlaps MinimizeUnpavedRoads. Presumably this is a bug, and so I have moved it down one bit.

Example
```
2020202020202020 3220200000800600
0000000000c80000 00c8000000d20000
00c8000000003200 0200000100000000
0000000000000000 00000000000000e7
6a
```

---

```
// code: 0x0d5b & 0x0d5c, len: 0xfd (see notes)
struct DABParameterSet {
	uint8  Tag[8];
	uint8  MajorVersion;
	uint8  MinorVersion[2];
	uint8  unknown[240];
	uint16 crc;
}
```
Data structure spans two kds fields. Code actually only reads 0xe6 bytes from 0x0d5b. I assume the last two bytes are the crc for the block and are skipped.
It then appends 0x17 bytes from 0x0d5c, dropping multiple bytes from the end, including what is probably the crc for the second block in the last two bytes.
Oddly the code has a getter method for the crc that points 0xfb bytes into the reconstructed structure. This is the last two bytes of the truncated structure,
but doesn't appear to point to an actual crc in the example below.


Example:
```
Code: 0d5b, len: 00e8
4441425f45555231 3230001300030000  DAB_EUR120......
0001100313070700 0000000000000000  ................
0008040000000000 00000000037ed001  .............~..
7f7f7f0000000001 0000fe4e6f205365  ...........No.Se
7276696365000000 000000010000ff4e  rvice..........N
6f20456e73656d62 6c65000000000000  o.Ensemble......
0000000000000000 0000000c0c0c0c0c  ................
0c0c0c0c0c0c0c0c 0c0d0e1012141618  ................
1a1c1e1f1f1f1f1f 1f1f1f1212121213  ................
1313141414151515 1516161717171818  ................
1919191a1a1a1a1a 1a1a1a0710170a01  ................
0b2d141405011200 000000011002033c  .-.............<
0305040505040481 810201fffe000000  ................
0007020002040207 0000640064004605  ..........d.d.F.
000000000000f902                   ........

Code: 0d5c, len: 002f
0000000000000000 0000000000000000  ................
0000000000000000 0000000000000000  ................
0000000000000000 00000000005a19    .............Z.
```

Tag = "DAB_EUR1"\
MajorVersion = "2"\
MinorVersion = "0" (null padded)\
crc = 0x0000

---

```
// code: 0x0d30 - 0x0d38, len: 0x080d (see notes)
struct SDSECNR {
	uint8  Tag[8];
	uint8  MajorVersion;
	uint8  MinorVersion[2];
	uint8  unknown[2048];
	uint16 crc;
}
```

