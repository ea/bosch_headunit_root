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


```
// code 0x0d60, len 0x04
struct ECORoute {
	uint8  FuelType : 1;          // byte 0
	uint8  DragCoefficient : 7;
	uint8  TransmissionType : 2;  // byte 1
	uint8  unused : 6;
	uint16 crc;                   // byte 2-3
};
```
Example 20c078ea
FuelType = 0 (from an unleaded car)
DragCoefficient = 20
TransmissionType = 3 (from a manual 6 speed car)
crc = 0xea78

---

```
// code: 0x0d70, len 0x18
struct BTName {
	uint8  Name[16];
	uint16 crc;
};
```
Example 4d59434152000000 0000000000000000 0000000000004651
Name = "MYCAR" (null padded, max len 15 null temrinated or 16 if not???)
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
Example 335a4c3742...... ................ ................ ..b0e9
PartNumber = "3ZL7B" (appears fixed length, not null terminated)
ConfigurationHash = (hidden, in case unique)
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
Example 80008b83
CameraSystem = 4 (from car with birds eye view, 4 camera system)
AnticipatoryLine = 0
Guideline = 0
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
Example 00124c19bc400928 8c5d
...
Region = 3 (from UK car)
...
crc = 0x5d8c
