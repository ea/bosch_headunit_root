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