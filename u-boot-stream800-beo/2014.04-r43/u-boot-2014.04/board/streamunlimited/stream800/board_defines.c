#include "board_defines.h"
#include "adc.h"
#include "ase_fep.h"

static struct ase_fep *fep = NULL;

const char *stream800ModuleVersionStrings[] = {
	[MV_unknown]	= "unknown",
	[MV_L0]		= "stream800 rev.L0",
	[MV_L1]		= "stream800 rev.L1",
	[MV_L2]		= "stream800 rev.L2",
	[MV_L0_DDR3]	= "stream800 rev.L0 w/ddr3",
	[MV_L1_DDR3_HE]	= "stream800 rev.L1 w/ddr3 High-End",
	[MV_BeoASE] = "Beo ASE",
};

const char *stream800CarrierBoardTypeStrings[] = {
	[CBT_unknown]			= "unknown",
	[CBT_SueFactoryTestBoard]	= "SUE factory test board",
	[CBT_SueFirstCarrierBoard]	= "SUE first carrier board",
	[CBT_SueDemoClientBoard]	= "SUE demo client board",
	[CBT_SueHighendDemoClientBoard]	= "SUE highend demo client board",
	[CBT_BeoASE_A9MK2]		= "B&O ASE A9MK2 board",
	[CBT_BeoASE_EZ2MK2]		= "B&O ASE EZ2MK2 board",
	[CBT_BeoASE_CA6]		= "B&O ASE CA6 board",
	[CBT_BeoASE_S47]		= "B&O ASE S47 board",
};

const char *boardStateStrings[] = {
	[BS_Off]		= "unknown",
	[BS_Normal]		= "normal",
	[BS_DontUnplug]		= "dont unplug/flashing new fw",
	[BS_HardFailure]	= "hard failure/cannot boot",
	[BS_BootingKernel]	= "booting kernel",
};

static struct SupportedModules {
	Stream800ModuleVersion moduleVersion;
	unsigned msb_min;	/* Ain0 */
	unsigned msb_max;
	unsigned lsb_min;	/* Ain6 */
	unsigned lsb_max;
} supportedModules[] = {
	{MV_L2,		0xCCC,	0xD54,	0x4CC,	0x555},
	/* L2 - Stream800/1WEB7 and L2 - Stream800/2WEB7: same config for
	 * different ADC values, as RAM and Bluetooth are auto-detected at
	 * runtime: */
	{MV_L2,		0xCCC,	0xD54,	0x111,	0x199},
	{MV_L2,		0xCCC,	0xD54,	0x199,	0x222},
	/* L2 - Stream800/2E7 (no wifi/bt module) */
	{MV_L2,		0xCCC,	0xD54,	0x2AA,	0x333},
	/* L2 - Stream800/1E7 (no wifi/bt module) */
	{MV_L2,		0xCCC,	0xD54,	0x5DD,	0x666},
	/* L2 - Stream800/2E7-4 (no wifi/bt module, 4G flash) */
	{MV_L2,		0xCCC,	0xD54,	0x6EE,	0x777},
	/* L2 - Stream800/2E8-4 (no wifi/bt module, 800Mhz, 4G flash) */
	{MV_L2,		0xCCC,	0xD54,	0x777,	0x7ff},
	/* L0 - Stream800/2WE8-D3 Teufel */
	{MV_L0_DDR3,	0x2AA,	0x333,	0x111,	0x199},
	/* L0 - Stream800/1WEB8-D3 */
	{MV_L0_DDR3,	0x2AA,	0x333,	0x199,	0x222},
	/* L1 - Stream800/4WE10-D3-4 */
	{MV_L1_DDR3_HE,	0x2AA,	0x333,	0x6EE,	0x777},
};

static struct SupportedCarrierBoards {
	Stream800CarrierBoardType boardType;
	unsigned msb_min;	// Ain3
	unsigned msb_max;
	unsigned lsb_min;	// Ain2
	unsigned lsb_max;
} supportedCarrierBoards[] = {
	{CBT_SueDemoClientBoard,	0xF76,	0xFFF,	0x000,	0x088},
	{CBT_SueFactoryTestBoard,	0x000,	0x088,	0x000,	0x088},		// L0
	{CBT_SueFactoryTestBoard,	0x6EE,	0x777,	0x000,	0x088},		// L1
	{CBT_SueHighendDemoClientBoard,	0xF76,	0xFFF,	0xF76,	0xFFF},		// demo client with DSD dac attached
	// CUSTOMIZE: add your board here
};

void board_detect(struct Stream800Board *board)
{
	if (!board) {
		BUG();
		return;
	}

#ifdef ASE_BUILD
	{
		struct ase_fep_version version;
		printf("Detecting ASE board\n");

		fep = init_ase_fep();

		version = version_ase_fep(fep);
		if (version.revision >= 'A' && version.revision <= 'V') {
			board->moduleVersion = MV_BeoASE;
		}

		if ((version.productId == a29mk2_version_a) ||
			(version.productId == a29mk2))  {
			board->carrierBoardType = CBT_BeoASE_A9MK2;
		} else if (version.productId == ez2mk2) {
			board->carrierBoardType = CBT_BeoASE_EZ2MK2;
		} else if (version.productId == ca6 ) {
			board->carrierBoardType = CBT_BeoASE_CA6;
		} else if (version.productId == s47) {
			board->carrierBoardType = CBT_BeoASE_S47;
		}

		/*
		* We need to set to some default values otherwise board
		* will not boot and loop in bootloader which brick board
		*/
		if (board->moduleVersion == MV_unknown) {
			printf("Module unknown. Setting to ASE\n");
			board->moduleVersion = MV_BeoASE;
		}

		if (board->carrierBoardType == CBT_unknown) {
			printf("Carrierboard unknown. Setting to A9MK2.\n");
			board->carrierBoardType = CBT_BeoASE_A9MK2;
		}
		return;
	}
#endif

	board->moduleVersion = MV_unknown;
	board->carrierBoardType = CBT_unknown;

	{
		unsigned MV_MSB = read_adc_channel(0);	// Module Version (MSB)
		unsigned MV_LSB = read_adc_channel(6);	// Module Version (LSB)
		unsigned CBT_MSB = read_adc_channel(3);	// Carrier Board Type (MSB)
		unsigned CBT_LSB = read_adc_channel(2);	// Carrier Board Type (LSB)
		int i;

		printf("MV_MSB: 0x%03X, MV_LSB: 0x%03X, CBT_MSB: 0x%03X, CBT_LSB: 0x%03X\n",
			MV_MSB, MV_LSB, CBT_MSB, CBT_LSB);

		// Detecting module version
		for (i = 0; i < sizeof(supportedModules) / sizeof(supportedModules[0]); i++) {
			if ((MV_MSB >= supportedModules[i].msb_min) && (MV_MSB <= supportedModules[i].msb_max)
				&& (MV_LSB >= supportedModules[i].lsb_min) && (MV_LSB <= supportedModules[i].lsb_max)) {
				board->moduleVersion = supportedModules[i].moduleVersion;
				break;
			}
		}

		// Detecting carrier board type
		for (i = 0; i < sizeof(supportedCarrierBoards) / sizeof(supportedCarrierBoards[0]); i++) {
			if ((CBT_MSB >= supportedCarrierBoards[i].msb_min) && (CBT_MSB <= supportedCarrierBoards[i].msb_max)
				&& (CBT_LSB >= supportedCarrierBoards[i].lsb_min) && (CBT_LSB <= supportedCarrierBoards[i].lsb_max)) {
				board->carrierBoardType = supportedCarrierBoards[i].boardType;
				break;
			}
		}

		// Try to check if we run some of obsolete revision of stream800 module
		// L1 has not correctly implemented board detection support in HW, values used for detection are only experimental
		if (board->moduleVersion == MV_unknown || board->carrierBoardType == CBT_unknown) {
			if (MV_MSB >= 0xDA0 && MV_MSB <= 0xF00 && MV_LSB >= 0xBA0 && MV_LSB <= 0xD00
				&& CBT_MSB >= 0xCA0 && CBT_MSB <= 0xE00 && CBT_LSB >= 0xCA0 && CBT_LSB <= 0xE00) {
				board->moduleVersion = MV_L1;
				board->carrierBoardType = CBT_SueDemoClientBoard;
		// It is not recommend to use L1 or L2 with CBT_SueFirstCarrierBoard, values used for detection are only experimental
			} else if (MV_MSB >= 0xDE0 && MV_MSB <= 0xF00 && MV_LSB >= 0xBD0 && MV_LSB <= 0xCD0
				&& CBT_MSB >= 0xE80 && CBT_MSB <= 0xEB0 && CBT_LSB >= 0x4C0 && CBT_LSB <= 0x510) {
				board->moduleVersion = MV_L1;
				board->carrierBoardType = CBT_SueFirstCarrierBoard;
			} else if (MV_MSB >= 0xCE0 && MV_MSB <= 0xD30 && MV_LSB >= 0x4D0 && MV_LSB <= 0x540
				&& CBT_MSB >= 0xE50 && CBT_MSB <= 0xEF0 && CBT_LSB >= 0x000 && CBT_LSB <= 0x050) {
				board->moduleVersion = MV_L2;
				board->carrierBoardType = CBT_SueFirstCarrierBoard;
			}
		}

		if (board->moduleVersion == MV_unknown || board->carrierBoardType == CBT_unknown) {
			printf("MV_MSB: 0x%03X, MV_LSB: 0x%03X, CBT_MSB: 0x%03X, CBT_LSB: 0x%03X\n",
				MV_MSB, MV_LSB, CBT_MSB, CBT_LSB);
		}

		/* When carrier board is unknown set to democlient
		 * It should work but some peripherials will not work but
		 * board will boot up. Without that there is issue with
		 * dtb loading because cramfsdir isn't properly set when unknown.
		 */
		if (board->carrierBoardType == CBT_unknown) {
			printf("Carrierboard unknown. Setting to Democlient.\n");
			board->carrierBoardType = CBT_SueDemoClientBoard;
		}
	}
}

void board_printf_info(const struct Stream800Board *board)
{
	printf("Detected module: '%s', carrier board: '%s'\n",
	       stream800ModuleVersionStrings[board->moduleVersion],
	       stream800CarrierBoardTypeStrings[board->carrierBoardType]);
}


struct ase_fep *fep_instance(void)
{
	return fep;
}
