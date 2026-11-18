// Philips RC5 commands:
// Chungchop universal.

#define UP_KEY 32		// UP Key (P+)
#define DOWN_KEY 33		// DOWN Key (P-)
#define ENTER_KEY 13		// ENTER Key
#define MUTE_KEY 13			// IR Mute KEY (RC5)
#define PLUS_KEY 16			// IR + KEY


#define NAVBUTTON_DELAY	300	// Delay between 2 repetitive hits (ms)
#define IR_KEY_DELAY 300  	// Infra red key delay (ms)





#define startY  3		// Menu origin, Y
#define startX  2		// Menu origin, X


char checkInputRequestFromUser();

void displayMenu_OSD(void);
void displayMenu_Main();
void displayMenu_Warnings();
void displayMenu_Tools();

void displayOSDSettingsMenu(void);
void resetToDefaultValues();
void getAllSettingsFromEEPROM();
