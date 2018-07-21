#include <stdio.h>

#define CHRONO 1
#define GAIA 2
#define METROID3 3
#define SD3 4
#define SOE 5
#define SOM 6
#define STAROCEAN 7
#define TERRANIGMA 8
#define TOP 9
#define ZELDA3 10

FILE* fp_sram;
unsigned char sram_memb[0x8000];								//32KB to store sram in memory
unsigned short* sram_memw = (unsigned short*)sram_memb;			//read the saveram data as 16bit words
unsigned short* sram_memwo = (unsigned short*)(sram_memb + 1);	//read unaligned 16bit words
short i, valid = 0, counter = 0, slot = 0;

unsigned long checksum = 0, checksum2 = 0, file_size = 0;

void check_which_saveram(void);
void fix_chrono_trigger(void);
void fix_gaia(void);
void fix_metroid3(void);
void fix_terranigma(void);
void fix_sd3(void);
void fix_soe(void);
void fix_som(void);
void fix_starocean(void);
void fix_top(void);
void fix_zelda3(void);

int main(int argc, char* argv[]){
	if (argc < 2) {
		printf("USEAGE: snessum file.srm\nCalculates the game saveram checksum of the following snes games:\n -Chrono Trigger\n -Illusion of Gaia\n -Metroid 3\n -Secret of Evermore\n -Secret of Mana\n -Seiken Densetsu 3\n -Tales of Phantasia\n -Terranigma\n -Star Ocean\n -Zelda 3\n");
		exit(1);
	}

	if ((fp_sram = fopen(argv[1], "rb+")) == NULL) {
		printf("ERROR: Cannot open saveram file '%s'\n", argv[1]);
		exit(1);
	}

	fseek(fp_sram, 0, SEEK_END);
	file_size = ftell(fp_sram);
	fseek(fp_sram, 0, SEEK_SET);
	if (file_size & ~0x0000E000) {
		fclose(fp_sram);
		printf("ERROR: %s has an odd size, must be 8KB 16KB or 32KB\n", argv[1]);
		exit(1);
	}

	fread(&sram_memb, 2, file_size >> 1, fp_sram);

	check_which_saveram();

	switch (valid) {
	case 0:
		printf("ERROR: %s is not a known saveram\n", argv[1]);
		break;
	case CHRONO:
		fix_chrono_trigger();
		break;
	case GAIA:
		fix_gaia();
		break;
	case METROID3:
		fix_metroid3();
		break;
	case SD3:
		fix_sd3();
		break;
	case SOE:
		fix_soe();
		break;
	case SOM:
		fix_som();
		break;
	case STAROCEAN:
		fix_starocean();
		break;
	case TERRANIGMA:
		fix_terranigma();
		break;
	case TOP:
		fix_top();
		break;
	case ZELDA3:
		fix_zelda3();
		break;
	}

	fseek(fp_sram, 0, SEEK_SET);
	fwrite(&sram_memb, 2, file_size >> 1, fp_sram);
	fclose(fp_sram);

	exit(0);
}

//some srams are more identifiable than others, check these in order
void check_which_saveram(){
	if (sram_memw[0x0] + sram_memw[0x08 / 2] == 0xFFFF && sram_memw[0x1FF0 / 2] + sram_memw[0x1FF8 / 2] == 0xFFFF) {
		valid = METROID3;
		return;
	}
	if (sram_memw[0x1FF8 / 2] == 0xE41B || sram_memw[0x1FFA / 2] == 0xE41B || sram_memw[0x1FFC / 2] == 0xE41B) {
		valid = CHRONO;
		return;
	}
	if (sram_memb[0x00] == 0xC5 && sram_memb[0x2BB] == 0x3A) {
		valid = SOM;
		return;
	}
	if (sram_memwo[0x3E4 / 2] == 0x55AA || sram_memwo[0x3E0 / 2] == 0x55AA) {
		valid = ZELDA3;
		return;
	}
	if (sram_memw[0x100 / 2] == 0x6154 && sram_memw[0x102 / 2] == 0x656C && sram_memw[0x104 / 2] == 0x00B0) {
		valid = TOP;
		return;
	}
	if (sram_memw[0] == 0x7865) {
		valid = SD3;
		return;
	}
	if (sram_memw[0x100 / 2] == 0x000F) {
		valid = TERRANIGMA;
		return;
	}
	if (sram_memwo[0] == 0x0936) {
		valid = STAROCEAN;
		return;
	}
	if (sram_memb[0x2F6] == 0x81 && sram_memb[0x306] == 0xE6) {
		valid = GAIA;
		return;
	}
	if (sram_memb[0] <= 1) {
		valid = SOE;
		return;
	}
}

void fix_chrono_trigger(){
	for (slot = 0; slot < 3; slot++) {
		checksum = 0;
		for (i = 0x9FE / 2; i >= 0; i--) {
			if (checksum > 0xFFFF) {
				checksum -= 0xFFFF;
			}
			checksum += sram_memw[i + slot * 0xA00 / 2];
		}
		checksum &= 0XFFFF;
		sram_memw[0x1FF0 / 2 + slot] = checksum;
	}
	printf("Chrono Trigger sram checksums fixed (3)\n");
}

void fix_gaia(){
	for (slot = 1; slot < 4; slot++) {
		checksum = 0x3652;
		checksum2 = 0x3652;
		for (i = 0; i < 0xFE; i++) {
			checksum += sram_memw[i + slot * 0x100];
			checksum2 ^= sram_memw[i + slot * 0x100];
		}
		sram_memw[0x1FC / 2 + slot * 0x100] = checksum;
		sram_memw[0x1FE / 2 + slot * 0x100] = checksum2;
	}
	printf("Illusion of Gaia sram checksums fixed (3)\n");
}

void fix_metroid3(){
	for (slot = 0; slot < 3; slot++) {
		checksum = 0;
		for (i = 0x10; i < 0x66C; i += 2) {
			checksum += sram_memw[(slot * 0x65C + i) / 2];
		}
		checksum &= 0xFFFF;
		sram_memw[slot] = checksum;
		sram_memw[slot + 4] = 0xFFFF - checksum;
		sram_memw[slot + 0x1FF0 / 2] = checksum;
		sram_memw[slot + 0x1FF8 / 2] = 0xFFFF - checksum;
	}
	printf("Super Metroid sram checksums fixed (3)\n");
}

void fix_sd3(){
	for (slot = 0; slot < 3; slot++) {
		checksum = 0;
		for (i = 0; i < 0x78E; i++)
			checksum += sram_memb[i + 0x70 + slot * 0x800];
		sram_memw[0x7FE / 2 + slot * 0x400] = checksum;
	}
	printf("Seiken Densetsu 3 sram checksums fixed (3)\n");
}

void fix_soe(){
	for (slot = 0; slot < 4; slot++) {
		checksum = 0x43F;
		(unsigned char)checksum += sram_memb[4 + slot * 0x331];
		for (i = 0; i < 0x32E; i++) {
			checksum <<= 1;
			if (checksum > 0xFFFF)
				checksum -= 0xFFFF;
			(unsigned char)checksum += sram_memb[5 + i + slot * 0x331];
		}
		sram_memw[1] = checksum;
		(unsigned char*)sram_memw += 0x331;
	}
	printf("Secret of Evermore sram checksums fixed (4)\n");
}

void fix_som(){
	for (slot = 0; slot < 4; slot++) {
		checksum = 0;
		for (i = 3; i < 0x2BB; i++) {
			checksum += sram_memb[i + (slot * 0x400)];
		}
		checksum &= 0xFFFF;
		sram_memwo[slot * 0x400 / 2] = checksum;
	}
	printf("Secret of Mana sram checksums fixed (4)\n");
}

void fix_starocean(){
	for (slot = 0; slot < 3; slot++) {
		checksum = 0;
		counter = sram_memwo[0 + slot * 0xA00 / 2] - 1; //seems data can vary in length
		for (i = 0; i < counter; i++) {
			checksum += sram_memb[i + slot * 0xA00];
		}
		checksum ^= 0xFF;
		checksum++;
		checksum &= 0xFF;
		sram_memb[counter + slot * 0xA00] = checksum;
	}
	printf("Star Ocean sram checksums fixed (3)\n");
}

void fix_terranigma(){
	for (slot = 0; slot < 3; slot++) {
		checksum = 0x5236;
		checksum2 = 0x5236;
		for (i = 0; i < 0x4FA / 2; i++) {
			checksum += sram_memw[i + (slot * 0xA00 + 0x100) / 2];
			checksum2 ^= sram_memw[i + (slot * 0xA00 + 0x100) / 2];
		}

		sram_memw[(0x5FA + slot * 0xA00) / 2] = checksum;
		sram_memw[(0x5FC + slot * 0xA00) / 2] = checksum2;
	}
	printf("Terranigma sram checksums fixed (3)\n");
}

void fix_top(){
	for (slot = 0; slot < 3; slot++) {
		checksum = 0;
		counter = sram_memw[0x106 / 2 + slot * 0x500]; //seems data can vary in length
		for (i = 0; i < counter; i++) {
			checksum += sram_memb[i + 0x100 + slot * 0xA00];
		}
		checksum ^= 0xFF;
		checksum++;
		checksum &= 0xFF;
		sram_memb[0xA5A + slot * 0xA00] = checksum;
	}
	printf("Tales of Phantasia checksums fixed (3)\n");
}

void fix_zelda3(){
	for (slot = 0; slot < 3; slot++) {
		checksum = 0;
		for (i = 0; i < 0x4FE / 2; i++)
			checksum += sram_memw[i + slot * 0x280];
		checksum = 0x5A5A - checksum;
		checksum &= 0XFFFF;
		sram_memw[0x4FE / 2 + slot * 0x280] = checksum;
	}
	printf("Zelda 3 sram checksums fixed (3)\n");
}
