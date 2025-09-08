#include "names.h"

const char *const systemNames[] = {
	"NES/FC/Dendy",
	"VS System",
	"PlayChoice-10",
	"Famiclone with Decimal Mode",
	"NES/FC with EPSM module",
	"VT01 with STN palette",
	"VT02",
	"VT03",
	"VT09",
	"VT32",
	"VT369",
	"UMC UM6578",
	"Famicom Network System",
	"Reserved (0x0d)",
	"Reserved (0x0e)",
	"Reserved (0x0f)"
};

const char *const regionNames[] = {
	"RP2C02 (NTSC)",
	"RP2C07 (PAL)",
	"Dual-region",
	"UA6538 (Dendy)"
};

const char *const vsSystemPpuNames[] = {
	"RP2C03B",
	"RP2C03G",
	"RP2C04-0001",
	"RP2C04-0002",
	"RP2C04-0003",
	"RP2C04-0004",
	"RC2C03B",
	"RC2C03C",
	"RC2C05-01",
	"RC2C05-02",
	"RC2C05-03",
	"RC2C05-04",
	"RC2C05-05",
	"Reserved (0x0d)",
	"Reserved (0x0e)",
	"Reserved (0x0f)"
};

const char *const vsSystemTypes[] = {
	"VS Unisystem",
	"RBI Baseball",
	"TKO Boxing",
	"Super Xevious",
	"Ice Climber Japan",
	"VS Dualsystem",
	"Raid on the Bungeling Bay"
};

const char *const inputTypes[] = {
	"Unspecified",
	"Standard NES/Famicom controllers",
	"NES Four Score/Satellite",
	"Famicom Four Players",
	"VS System (1P via 0x4016)",
	"VS System (1P via 0x4017)",
	"Reserved",
	"VS Zapper",
	"Zapper (0x4017)",
	"Two Zappers",
	"Bandai Hyper Shot Lightgun",
	"Power Pad Side A",
	"Power Pad Side B",
	"Family Trainer Side A",
	"Family Trainer Side B",
	"Arkanoid Vaus Controller (NES)",
	"Arkanoid Vaus Controller (Famicom)",
	"Two Vaus Controllers plus Famicom Data Recorder",
	"Konami Hyper Shot Controller",
	"Coconuts Pachinko Controller",
	"Exciting Boxing Punching Bag",
	"Jissen Mahjong Controller",
	"Party Tap",
	"Oeka Kids Tablet",
	"Sunsoft Barcode Battler",
	"Miracle Piano Keyboard",
	"Pokkun Moguraa",
	"Top Rider",
	"Double-Fisted",
	"Famicom 3D System",
	"Doremikko Keyboard",
	"R.O.B. Gyro Set",
	"Famicom Data Recorder",
	"ASCII Turbo File",
	"IGS Storage Battle Box",
	"Family BASIC Keyboard plus Famicom Data Recorder",
	"Dongda PEC-586 Keyboard",
	"Bit Corp. Bit-79 Keyboard",
	"Subor Keyboard",
	"Subor Keyboard plus mouse (3x8-bit protocol)",
	"Subor Keyboard plus mouse (24-bit protocol via 0x4016)",
	"SNES Mouse (0x4017.d0)",
	"Multicart",
	"Two SNES controllers replacing the two standard NES controllers",
	"RacerMate Bicycle",
	"U-Force",
	"R.O.B. Stack-Up",
	"City Patrolman Lightgun",
	"Sharp C1 Cassette Interface",
	"Standard Controller with swapped Left-Right/Up-Down/B-A",
	"Excalibur Sudoku Pad",
	"ABL Pinball",
	"Golden Nugget Casino extra buttons",
	"Unknown famiclone keyboard used by the 'Golden Key' educational cartridge",
	"Subor Keyboard plus mouse (24-bit protocol via 0x4017)",
	"Port test controller",
	"Bandai Multi Game Player Gamepad buttons",
	"Venom TV Dance Mat",
	"LG TV Remote Control",
	"Famicom Network Controller",
	"King Fishing Controller",
	"Croaky Karaoke Controller"
};

const char *const officialPrgSizes[] = {
	"64",
	"16",
	"32",
	"128",
	"256",
	"512"
};

const char *const officialChrSizes[] = {
	"8",
	"16",
	"32",
	"64 or 128",
	"256"
};

const char *const officialMapperNames[] = {
	"NROM",
	"CNROM",
	"UNROM",
	"GNROM",
	"MMC"
};