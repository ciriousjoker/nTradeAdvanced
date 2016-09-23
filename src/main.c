#include <os.h>
#include <nspireio2.h>
#include <libndls.h>
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>


int main()
{
	nio_console csl;
	lcd_incolor();
	clrscr();
	//					  54, 30 would be the normal max values, but this way a graphic bug is fixed and i do \n manually anyway
	nio_InitConsole(&csl, 55, 31, 0, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE);
	nio_DrawConsole(&csl);
	nio_drawing_enabled(&csl,TRUE);
	nio_cursor_type(&csl, NIO_CURSOR_UNDERSCORE);

	
	drawLogo(&csl, 3000);
	
	struct TradeOption *options;
	options = (struct TradeOption *)malloc(sizeof(struct TradeOption));
	
	for(int c = 0; c < 6; ++c)
	{
		options->trades[0][c] = -1;
		options->trades[1][c] = -1;
	}
	options->saved = 0;
	
	short stop = 0;
	while(stop == 0)
	{
		int choice = displayMenu(&csl);
		switch(choice){
			case 48:	// ASCII Code for 0	-> Exit
				stop = 1;
				break;
			case 49:	// ASCII Code for 1	-> Start trading
				if(options->saved == 0)
				{
					nio_printf(&csl, "\n\n[!]Please choose the Pokemons you\nwant to trade first!");
					wait_key_pressed();
					break;
				}
				TradePokemon(&csl, options);
				break;
			case 50:	// ASCII Code for 2	-> Choose Trading Options
				ChoosePokemon(options);
				options->saved = 1;
				break;
			case 51:	// ASCII Code for 3	-> Fix Savestates
				FixSaves(&csl);
				break;
			case 52:	// ASCII Code for 4	-> Display Help & Disclaimer
				nio_clear(&csl);
				nio_printf(&csl, HELPSTRING);
				wait_key_pressed();
				break;
			default:	// exit if invalid choice
				break;
		}
	}
	
	drawBye(&csl, 3000);
	free(options);
	clrscr();
	lcd_incolor();
	return 0;
}

// Displays the Menu string and returns the choice
int displayMenu(nio_console *csl)
{
	clrscr();
	nio_clear(csl);
	char choice[1];
	
	nio_printf(csl, HEADLINE);		// Print the Headline
	nio_printf(csl, MENUSTRING);	// Print the options
	nio_printf(csl, "> ");			// wait for input
	nio_fgets(choice,3,csl);		// store input in choice[]
	return (int)choice[0];			// return choice
}

// Fill struct TradeOption options with values
int ChoosePokemon(struct TradeOption *options)
{
	clrscr();
	
	char * title = "";	// Displays Number of the trade #%d
	char * subtitle = "Cancel to save your choice."; 
	char * msg1 = "Trade your Pokemon #";
	char * msg2 = "with Partner's Pokemon #";
	
	int valid	= 0;
	int value1	=	1;
	int value2	=	1;
	char popup_result[256];
	
	for(int c = 0; c < 6; ++c)
	{
		valid	=	0;
		while(valid == 0)
		{
			valid	=	1;
			sprintf (title, "Choose Trade %d/6", c+1);
			
			// return 0; if pressed cancel to save the struct
			if (show_2numeric_input(title, subtitle, msg1, &value1, 1, 6, msg2, &value2, 1, 6) != 1) return 0;
			
			// check for double trades
			for(int i = 0; i < 6; ++i)
			{
				if(options->trades[0][i] == value1)
				{
					show_msgbox("Notice", "You can't trade a Pokemon more than once. Pick different ones!");
					valid	=	0;
					continue;
				}
				if(options->trades[1][i] == value2)
				{
					show_msgbox("Notice", "You can't trade a Pokemon more than once. Pick different ones!");
					valid	=	0;
					continue;
				}
			}
		}
		
		options->trades[0][c] = value2-1;
		options->trades[1][c] = value1-1;
		
		sprintf(popup_result, "Your Pokemon #%d will be traded \nwith your partner's Pokemon #%d", value1, value2);
		show_msgbox("Information", popup_result);
	}
	show_msgbox("Saved", "Successfully saved your choices!");
	clrscr();
	return 0;
}


// Trade Pokemon using a TradeOption
int TradePokemon(nio_console *csl, struct TradeOption *options)
{
	nio_clear(csl);
	nio_printf(csl, "Press any key start ...\n");
	wait_key_pressed();
	
	unsigned char *buffer; // buffer of the first savefile
	buffer=malloc(FILESIZE*sizeof(unsigned char));
	memset( buffer, 0x0, FILESIZE);
	
	unsigned char *buffer2; // buffer of the second savefile
	buffer2=malloc(FILESIZE*sizeof(unsigned char));
	memset( buffer2, 0x0, FILESIZE);
	
    struct Pokemon *TeamData;	// import/export Pokemon for trading
	TeamData=malloc(12*sizeof(struct Pokemon)); // Create an array of 12 Pokemons for both teams
	memset( TeamData, 0x0, 12);
	
	// loading both savefiles into buffer & buffer2
	nio_printf(csl, ">> Loading first savefile\n");
	if(loadSaveFile(buffer, FILE1) != FILESIZE)
	{
		nio_printf(csl, "\n[!] Your savefile is corrupted or can't be opened\nBe sure to use 128kb .sav files with this program");
		wait_key_pressed();
		return -1;
	}
	
	nio_printf(csl, ">> Loading second savefile\n");
	if(loadSaveFile(buffer2, FILE2) != FILESIZE)
	{
		nio_printf(csl, "Your partner's savefile is corrupted or can't be opened\nBe sure to use 128kb .sav files with this program");
		wait_key_pressed();
		return -1;
	}
	
	
	// load both teams into TeamData[]
	nio_printf(csl, ">> Loading teams\n");
	for(int c = 0; c<6; c++)
	{
		TeamData[c] = exportPokemon(buffer, c);
		TeamData[c+6] = exportPokemon(buffer2, c);
		/*
		TeamData[0]	=	Pokemon 1 of team 1;
		...
		TeamData[5]	=	Pokemon 2 of team 1;
		TeamData[6]	=	Pokemon 1 of team 2;
		...
		TeamData[11]	=	Pokemon 6 of team 2;
		*/
	}
	
	// TRADING POKEMON
	nio_printf(csl, ">> Trading Pokemon\n");
	for(int i = 0; i < 6; i++)
	{
		if( (options->trades[0][i] >= 0) && (options->trades[1][i] >= 0) )
		{
			importPokemon(buffer2, TeamData[options->trades[1][i]], options->trades[0][i]);
			importPokemon(buffer,  TeamData[(options->trades[0][i])+6], options->trades[1][i]);
		}
	}
	
	// write both saves back to FILE3 and FILE4 and fix their checksums before
	// so they aren't corrupted
	fixChksum(buffer);
	fixChksum(buffer2);
	
	nio_printf(csl, ">> Writing back the first savefile\n");
	if(writeSaveFile(buffer, FILE3) != FILESIZE)
	{
		nio_printf(csl, "[!] Error, can't write first savefile\n");
		free(buffer);
		free(buffer2);
		free(TeamData);
		wait_key_pressed();
		return 0;
	}
	
	nio_printf(csl, ">> Writing back the second savefile\n");
	if(writeSaveFile(buffer2, FILE4) != FILESIZE)
	{
		nio_printf(csl, "[!] Error, can't write second savefile\n");
		free(buffer);
		free(buffer2);
		free(TeamData);
		wait_key_pressed();
		return 0;
	}
	
	nio_printf(csl, ">> Finished, returning to menu\n");
	
	free(buffer);
	free(buffer2);
	free(TeamData);
	wait_key_pressed();
	return 0;
}


// load savefile into buffer and return filesize for checking after
unsigned long loadSaveFile(unsigned char buffer[], char filename[])
{
	FILE * fptr  = fopen(filename, "rb");
	if(fptr == NULL) {
		return 0;
	}
	unsigned long filesize = fread(buffer,1,FILESIZE,fptr);
	fclose(fptr);
	return filesize;
}

// write new savefile from buffer
unsigned long writeSaveFile(unsigned char buffer[], char filename[])
{
	FILE * fptr  = fopen(filename, "wb");
	if(fptr == NULL) {
		return 0;
	}
	unsigned long filesize = fwrite (buffer , 1, FILESIZE, fptr);
	fclose(fptr);
	return filesize;
}

// write Pokemon into struct Pokemon and return it
struct Pokemon exportPokemon(unsigned char *buffer, int index)
{
	int VersionOffset	=	RSE_TEAM;		// Emerald/Ruby/Sapphire Version as default version
	if(getGameCode(buffer) == 0x01)			// FireRed/LeafGreen
	{
		VersionOffset	=	FR_TEAM;
	} else {								// Ruby/Sapphire/Emerald
		VersionOffset	=	RSE_TEAM;
	}
	int64_t StartOffset = getOffset(buffer, 0x01) + VersionOffset + (100*index);
	printf("[EXPORT #%d] VersionOffset:	->	%X", index, (unsigned int) VersionOffset);
	printf("Startoffset:	->	%X\n", (unsigned int) StartOffset);
	struct Pokemon TempPoke;
	for(int c=0; c<POKEMON_SIZE; c++)
	{
		TempPoke.Data[c]	= buffer[StartOffset+c];
	}
	return TempPoke;
}

// Write Temppoke to char buffer at index
void importPokemon(unsigned char *buffer, struct Pokemon TempPoke, int index)
{
	int VersionOffset	=	RSE_TEAM;		// Emerald/Ruby/Sapphire Version as default version
	if(getGameCode(buffer) == 0x01)			// FireRed/LeafGreen
	{
		VersionOffset	=	FR_TEAM;
	} else {								// Ruby/Sapphire/Emerald
		VersionOffset	=	RSE_TEAM;
	}
	
	// = Offset of Team Section + Offset of the Team Content + (100(Bytes) * index)
	int64_t StartOffset = getOffset(buffer, 0x01);
	StartOffset	+=	VersionOffset;
	StartOffset	+=	(100*index);
	printf("[IMPORT to #%d] VersionOffset:	->	%X", index, (unsigned int) VersionOffset);
	printf("Startoffset:	->	%X\n", (unsigned int) StartOffset);
	
	for(int c=0; c<POKEMON_SIZE; ++c)
	{
		buffer[StartOffset+c] = TempPoke.Data[c];
	}
}

// only used for calculateChksum to swap the 2 Bytes
uint64_t swapBytes(uint64_t x)
{
	uint64_t tmp = x;
	x = (x & 0xFF)<<8;
	x += tmp>>8;
	return x;
}

int64_t calculateChkSum(unsigned char *buffer, int64_t offset)
{
	uint64_t bytesum  = 0;	// Sum of all Bytes in the data part of a section
	uint64_t ChkSum = 0;	// final checksum
	
	for(int c=0; c<BLOCK_SIZE; c+=4)
	{
		/*
			> split it up into 4 Byte parts and change the order (little endian)
			> use bitshifting to build one final value
			> add final value to bytesum
		*/
		uint64_t tmp  = (uint64_t) buffer[(uint64_t)offset+c];
		uint64_t tmp2 = (uint64_t) buffer[(uint64_t)offset+c+1];
		uint64_t tmp3 = (uint64_t) buffer[(uint64_t)offset+c+2];
		uint64_t tmp4 = (uint64_t) buffer[(uint64_t)offset+c+3];
		
	
		tmp2 = tmp2 << 8;
		tmp3 = tmp3 << 16;
		tmp4 = tmp4 << 24;
		bytesum += (tmp+tmp2+tmp3+tmp4);
	}
	
	// add 16 lower to 16 upper bits
	ChkSum = bytesum & 0xFFFFFFFF;	//truncate to 32 bits
	ChkSum =  bytesum>>16;			//get 16 upper bits
	ChkSum += bytesum;				//add to bytesum
	ChkSum = ChkSum & 0xFFFF;		//truncate to 16 Bits to get the lower 16 bits
	
	ChkSum = swapBytes(ChkSum);		//Swap Bytes because idk, sry dude but it only works this way
	
	return ChkSum;
}

//fixes all checksums of a buffer, so the savefiles isn't corrupted after the trade
void fixChksum(unsigned char *buffer)
{
	int64_t ChkSum = 0;
	int64_t Offset = 0;
	
	for(int c = 0; c < 14; c++)
	{
		Offset	=	getOffset(buffer, c);
		ChkSum	=	calculateChkSum(buffer, Offset);
		buffer[Offset+CHKSUM_OFFSET]	=	ChkSum >> 8;
		buffer[Offset+CHKSUM_OFFSET+1]	=	ChkSum & 0xFF;
	}
	
}

// returns the offset of a section
uint64_t getOffset(unsigned char *buffer, int SectionId)
{
	// saveindex to determine which savestate is the latest
	// There are 2 saves per 128 kb .sav file if you haven't noticed
	int64_t SaveIndex	=	-1;
	
	//-1 to check if something went wrong
	int64_t	Offset	=	-1;	
	
	for( int c = 0; c<FILESIZE; c+=0x1000)
	{
		if(buffer[c+0xFF4]	==	SectionId)
		{
			if(buffer[c+0xFFC]	>=	SaveIndex)
			{
				Offset	=	c;
			}
		}
	}
	
	if(Offset<0)
	{
		//~ printf("\n\n[!] Offset not found for SectionID:	%X\nSaveIndex:	%X\n\n", (unsigned int) SectionId, (unsigned int) SaveIndex);
		return -1;
	}
	return Offset;
}

// Fix corrupt savefiles (only possible for wrong checksums)
int FixSaves(nio_console *csl)
{
	nio_clear(csl);
	nio_printf(csl, "Press any key start ...\n");
	wait_key_pressed();
	nio_clear(csl);
	
	unsigned char *buffer; // buffer of the first savefile
	buffer=malloc(FILESIZE*sizeof(unsigned char));
	memset( buffer, 0x0, FILESIZE);
	
	unsigned char *buffer2; // buffer of the second savefile
	buffer2=malloc(FILESIZE*sizeof(unsigned char));
	memset( buffer2, 0x0, FILESIZE);
	
	unsigned long result; //result of loadSaveFile to check if its a) there and b) exactly 128kb big
	
	nio_printf(csl, ">> Fixing first savefile\n");
	result = loadSaveFile(buffer, FILE1);
	if(result == FILESIZE)
	{
		fixChksum(buffer);
		nio_printf(csl, ">> Writing back the first savefile\n");
		writeSaveFile(buffer, FILE5);
	} else {
		if(result == 0){
			nio_printf(csl, "\n[!] '1.sav' not found, continue with second\n");
		} else {
			nio_printf(csl, "\n[!] Sorry but i can't repair '1.sav'\nBe sure to use 128kb .sav files with this program");
		}
		wait_key_pressed();
	}
	
	
	nio_printf(csl, ">> Fixing second savefile\n");
	result = loadSaveFile(buffer2, FILE2);
	if(result == FILESIZE)
	{
		fixChksum(buffer2);
		nio_printf(csl, ">> Writing back the second savefile\n");
		writeSaveFile(buffer2, FILE6);
	} else {
		if(result == 0){
			nio_printf(csl, "\n[!] '2.sav' not found, exit\n");
		} else {
			nio_printf(csl, "\n[!] Sorry but i can't repair '2.sav'\nBe sure to use 128kb .sav files with this program");
		}
		wait_key_pressed();
	}
	
	nio_printf(csl, ">> Finished, returning to menu\n");
	free(buffer);
	free(buffer2);
	wait_key_pressed();
	return 0;
}

int getGameCode(unsigned char *buffer)
{
	uint64_t GameCodeOffset = getOffset(buffer, 0x00);
	GameCodeOffset	+=	172;
	return buffer[GameCodeOffset];
}