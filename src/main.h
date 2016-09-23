#ifndef MAIN_H
#define MAIN_H

// General
#define TRUE			1
#define FALSE			0

// Sizes
#define FILESIZE		131072			// 128 Kb in Bytes
//~ #define PERCENT			13107			// used for a little progressbar (10% of FILESIZE)
#define BLOCK_SIZE		4084			// BLOCK_SIZE = 4kb - DATABLOCK
#define CHKSUM_OFFSET	4086			// Offset of the first checksum-byte
#define POKEMON_SIZE	100				// Size of one Pokemon

// Offsets
#define FR_TEAM		0x38	// \__Offset for first Pokemon in the team
#define RSE_TEAM	0x238	// /

// Keycodes
#define ENTER			13				// ENTER ascii code (for putchar)
#define CR				10				// CARRIAGE RETURN ascii code (for putchar)
#define ESCAPE			27				// ESCAPE ascii code (for putchar)

// Files
#define FILE1			"1.sav.tns"			// savefile of player 1
#define FILE2			"2.sav.tns"			// savefile of player 2
#define FILE3			"1_after.sav.tns"	// savefile of player 1 after the trade
#define FILE4			"2_after.sav.tns"	// savefile of player 2 after the trade
#define FILE5			"1_fixed.sav.tns"	// savefile of player 2 after fixing
#define FILE6			"2_fixed.sav.tns"	// savefile of player 2 after fixing

// Strings
#define VERSION			"1.0"


//~                    VERSION 1.0                  
//~ [FIRST]
//~ Place savefiles in the 'My Documents' folder and
//~ name them:
//~ > '1.sav' (Your save)
//~ > '2.sav' (Your friend's save)
//~ 
//~ [MENU]
//~ [1] Trade           > Start the trade
//~ [2] Trading Options > Set which Pokemon to trade
//~ [3] Fix Savefiles   > Tries(!) to fix (1/2).sav
//~ 
//~ 
//~ [DISCLAIMER]
//~ This software comes without ANY warranty.
//~ I am not responsible for any earthquakes,
//~ atomic bombs or at the worst even
//~ a broken calculator, which have arisen
//~ from the use of this software.
//~ 
//~ Oh, by the way, it's published under the terms
//~ of the MIT license, so you can pretty much do
//~ whatever you want.
//~ 
//~ 
//~ 
//~ 
//~ 
//~ ~Philipp Bauer 2014	



//~ ~Bauer Philipp 2014
#define	HELPSTRING		"                   VERSION 1.0\n\n\n[FIRST]\nPlace savefiles in the 'My Documents' folder and\nname them:\n      > '1.sav' (Your save)\n      > '2.sav' (Your friend's save)\n\n[MENU]\n[1] Trade           > Start the trade\n[2] Trading Options > Set which Pokemon to trade\n[3] Fix Savefiles   > Tries(!) to fix (1/2).sav\n\n\n[DISCLAIMER]\nThis software comes without ANY warranty.\nI am not responsible for any earthquakes,\natomic bombs or at the worst even\na broken calculator, which have arisen\nfrom the use of this software.\n\nOh, by the way, it's published under the terms\nof the MIT license, so you can pretty much do\nwhatever you want.\n\n\n~Philipp Bauer 2014"	

//~ #####################################################
//~ #       ___________                    .___         #
//~ #   ____\__    ___/_______ _____     __| _/ ____    #
//~ #  /    \ |    |   \_  __ \\__  \   / __ |_/ __ \   #
//~ # |   |  \|    |    |  | \/ / __ \_/ /_/ |\  ___/   #
//~ # |___|  /|____|    |__|   (____  /\____ | \___  >  #
//~ #      \/                       \/      \/     \/   #
//~ #                      Advanced                     #
//~ #####################################################
#define	HEADLINE		"#####################################################\n#       ___________                    .___         #\n#   ____\\__    ___/_______ _____     __| _/ ____    #\n#  /    \\ |    |   \\_  __ \\\\__  \\   / __ |_/ __ \\   #\n# |   |  \\|    |    |  | \\/ / __ \\_/ /_/ |\\  ___/   #\n# |___|  /|____|    |__|   (____  /\\____ | \\___  >  #\n#      \\/                       \\/      \\/     \\/   #\n#                      Advanced                     #\n#####################################################\n"
#define	MENUSTRING		"[1] Trade\n[2] Trading Options\n[3] Fix Savefiles\n[4] Help & Disclaimer\n[0] Exit\n"




// Structures
struct Pokemon {
	unsigned char Data[100];
};	// defines one Pokemon to hand it over to the import/export Pokemon methods

struct TradeOption {
	short trades[2][6];
	short saved;
};	// Used to save Trade Options


// Functions
void drawLogo(nio_console *csl, int time);												// Draws the Logo when starting
void drawBye(nio_console *csl, int time);												// Draws the Endscreen when exiting
unsigned long loadSaveFile(unsigned char buffer[], char filename[]);					// loads a savefile into the buffer
unsigned long writeSaveFile(unsigned char buffer[], char filename[]);					// writes (modified) buffer to a savefile
uint64_t swapBytes(uint64_t x);															// swaps the bytes of the checksum (because its calculation swapped it)
int64_t calculateChkSum(unsigned char *buffer, int64_t offset);							// Calculates the Checksum for an area of Offset -> Offset + 4kb (BLOCK_SIZE)
uint64_t getOffset(unsigned char *buffer, int SectionId);								// returns the offset for a section (always the offset of the newest save)
struct Pokemon exportPokemon(unsigned char *buffer, int index);							// writes a pokemon structure to the buffer
void importPokemon(unsigned char *buffer, struct Pokemon TempPoke, int index);			// creates a pokemon structure from the buffer
void fixChksum(unsigned char *buffer);													// fix checksum for a file (as char[])
int displayMenu(nio_console *csl);														// Displays the menu string
int TradePokemon(nio_console *csl, struct TradeOption *options);						// trades Pokemon (Trade Information is in struct TradeOption)
int ChoosePokemon(struct TradeOption *options);											// initializes the struct TradeOption
int FixSaves(nio_console *csl);
int getGameCode(unsigned char *buffer);													// Returns a code based on which gametype *buffer has (RSE, FRLG)


// I know that this looks extremely stupid, but ndless console can't handle big char arrays
void drawLogo(nio_console *csl, int time)
{
	clrscr();
	nio_clear(csl);
	nio_color(csl,NIO_COLOR_BLACK,NIO_COLOR_YELLOW);	// set color to Pikachu style
	
	
	char *LogoString[2];						//	I know that this looks rly stupid, but here's why i had to do it like this while still keeping it easy to edit:
	LogoString[0]=malloc(1000*sizeof(char));	//	part 1 of the string as ndless can't handle big char arrays
	LogoString[1]=malloc(1000*sizeof(char));	//	part 2, both have to be free()ed after else BAM!
	memset( LogoString[0], 0x0, 1000);			//	I don't know if i need these 2 lines but better work with fresh ram instead of already used
	memset( LogoString[1], 0x0, 1000);			//
	
	strcat(LogoString[0], "#####################################################\n");
	strcat(LogoString[0], "#                                                   #\n");
	strcat(LogoString[0], "#                /\"*-.                              #\n");
	strcat(LogoString[0], "#               /     `-.                           #\n");
	strcat(LogoString[0], "#              /        \"`-.                        #\n");
	strcat(LogoString[0], "#             /.....        `-.                     #\n");
	strcat(LogoString[0], "#                   \"*-.      .-'                   #\n");
	strcat(LogoString[0], "#                     .-'  .-'                      #\n");
	strcat(LogoString[0], "#                   <'   <'                         #\n");
	strcat(LogoString[0], "#                    `-.  `-.           .           #\n");
	strcat(LogoString[0], "#                      .'  .-'          $           #\n");
	strcat(LogoString[0], "#               _    .' .-'            :$;          #\n");
	strcat(LogoString[0], "#               T$bp.L.-*\"\"*-._        d$b          #\n");
	strcat(LogoString[0], "#                `TP `-.       `-.    : T$          #\n");
	strcat(LogoString[0], "#               .' `.   `.        `.  ;  ;          #\n");
	strcat(LogoString[1], "#              /     `.   \\ _.      \\:  :           #\n");
	strcat(LogoString[1], "#             /        `..-\"         ;  |           #\n");
	strcat(LogoString[1], "#            :          /               ;           #\n");
	strcat(LogoString[1], "#            ;  \\      / _             :            #\n");
	strcat(LogoString[1], "#           /`--'\\   .' $o$            |            #\n");
	strcat(LogoString[1], "#          /  /   `./-, `\"'      _     :            #\n");
	strcat(LogoString[1], "#          '-'     :  ;  _ '    $o$    ;            #\n");
	strcat(LogoString[1], "#                  ;Y\"   |\"-.   `\"'   /             #\n");
	strcat(LogoString[1], "#                  | `.  L.'    .-.  /`*.           #\n");
	strcat(LogoString[1], "#                  :   `-.     ;   :'    \\          #\n");
	strcat(LogoString[1], "#                   ;    :`*-._L.-'`-.    :         #\n");
	strcat(LogoString[1], "#                   :    ;            `-.*          #\n");
	strcat(LogoString[1], "#                    \\  /                           #\n");
	strcat(LogoString[1], "#                     \"\"                            #\n");
	strcat(LogoString[1], "#####################################################");
	
	nio_printf(csl, LogoString[0]);
	nio_printf(csl, LogoString[1]);
	
	sleep(time);
	free(LogoString[0]);
	free(LogoString[1]);
	nio_color(csl,NIO_COLOR_BLACK,NIO_COLOR_WHITE);
	clrscr();
}

// Same as above...
void drawBye(nio_console *csl, int time)
{
	clrscr();
	nio_clear(csl);
	nio_color(csl,NIO_COLOR_BLACK,NIO_COLOR_LIGHTBLUE);
	
	
	char *EndString[2];
	EndString[0]=malloc(1000*sizeof(char));
	EndString[1]=malloc(1000*sizeof(char));
	memset( EndString[0], 0x0, 1000);
	memset( EndString[1], 0x0, 1000);
	
	strcat(EndString[0], "#####################################################\\n");
	strcat(EndString[0], "#                                                   #\\n");
	strcat(EndString[0], "# __________                                        #\\n");
	strcat(EndString[0], "# \\______   \\  ___.__.   ____                       #\\n");
	strcat(EndString[0], "#  |    |  _/ <   |  | _/ __ \\                      #\\n");
	strcat(EndString[0], "#  |    |   \\  \\___  | \\  ___/                      #\\n");
	strcat(EndString[0], "#  |______  /  / ____|  \\___  >                     #\\n");
	strcat(EndString[0], "#         \\/   \\/           \\/                      #\\n");
	strcat(EndString[0], "#                                                   #\\n");
	strcat(EndString[0], "#                            ___                    #\\n");
	strcat(EndString[0], "#                       _.-~~   ~~~-.               #\\n");
	strcat(EndString[0], "#                      /         _   ~.             #\\n");
	strcat(EndString[0], "#                     |#`       /#`    \\            #\\n");
	strcat(EndString[0], "#                     |-'|      |-'|    |           #\\n");
	strcat(EndString[0], "#                     /--        --     |-.         #\\n");
	strcat(EndString[1], "#                     \\__   .  .        / /\\_       #\\n");
	strcat(EndString[1], "#                      \\ ~~--___---~~/\\| |   ~-.    #\\n");
	strcat(EndString[1], "#                   .---`~~--____---_)  \\ \\-__  \\   #\\n");
	strcat(EndString[1], "#                   ) <    |__    __\\_   \\ \\     |  #\\n");
	strcat(EndString[1], "#                   ~-.__ /   ~~~~   \\   \\ \\     |  #\\n");
	strcat(EndString[1], "#                        ~-.   |     .~-.-' |    |  #\\n");
	strcat(EndString[1], "#                         | \\___|___/     / /     | #\\n");
	strcat(EndString[1], "#                         | /   |   \\     | |  /  | #\\n");
	strcat(EndString[1], "#                          \\     |   ~-___ \\ \\/  /  #\\n");
	strcat(EndString[1], "#                          /\\__ / `._ /   ~-\\ \\_/   #\\n");
	strcat(EndString[1], "#                         /    \\_____|      |`~     #\\n");
	strcat(EndString[1], "#                        |      |    |      |       #\\n");
	strcat(EndString[1], "#                         \\     |    |      |       #\\n");
	strcat(EndString[1], "#                         >______)   /_/\\/\\_\\       #\\n");
	strcat(EndString[1], "#####################################################");
	
	nio_printf(csl, EndString[0]);
	nio_printf(csl, EndString[1]);
	
	sleep(time);
	free(EndString[0]);
	free(EndString[1]);
	nio_color(csl,NIO_COLOR_BLACK,NIO_COLOR_WHITE);
	clrscr();
}
#endif
