
void clearWarnings(char posX,char posY, char length);
void displayWarnings(Aircraft_t Aircraft, char posX,char posY);
void OSDWaitingForFix(Aircraft_t Aircraft);
void OSDPositionIsSet(int delay_millisec);
void OSDIntro(int delay_millisec);
void sendToOSDFlightData(Aircraft_t Aircraft);
void sendToOSDPostFlightData(Aircraft_t Aircraft);
