#ifndef _BOARD_DEFINES_H_
#define _BOARD_DEFINES_H_

#include <common.h>

/** Module version
 * \see https://extern.streamunlimited.com:8443/display/Stream800/Stream800+Hardware+Configuration
 */
typedef enum {
	MV_unknown,
	MV_L0,
	MV_L1,
	MV_L2,
	MV_L0_DDR3,
	MV_L1_DDR3_HE,
	MV_BeoASE,
} Stream800ModuleVersion;

extern const char *stream800ModuleVersionStrings[];

/** Carrier board type
 * \see https://extern.streamunlimited.com:8443/display/Stream800/Stream800+Hardware+Configuration
 */
typedef enum {
	CBT_unknown,
	CBT_SueFactoryTestBoard,		///< SUE factory test board
	CBT_SueFirstCarrierBoard,		///< SUE Carrier Board
	CBT_SueDemoClientBoard,			///< SUE demo client
	CBT_SueHighendDemoClientBoard,		///< SUE highend demo client
	CBT_BeoASE_A9MK2,
	CBT_BeoASE_EZ2MK2,
	CBT_BeoASE_CA6,
	CBT_BeoASE_S47,
} Stream800CarrierBoardType;

extern const char *stream800CarrierBoardTypeStrings[];

struct Stream800Board {
	Stream800ModuleVersion moduleVersion;
	Stream800CarrierBoardType carrierBoardType;
};

typedef enum {
	BS_Off = 0,
	BS_Normal,
	BS_DontUnplug,		/* flashing new FW, ... */
	BS_HardFailure,		/* cannot boot, ... */
	BS_BootingKernel,
} BoardState;

extern const char *boardStateStrings[];

Stream800ModuleVersion get_module_version(void);
Stream800CarrierBoardType get_carrier_board_type(void);

void board_detect(struct Stream800Board*);

void board_printf_info(const struct Stream800Board *);

struct ase_fep *fep_instance(void);

#endif /* _BOARD_DEFINES_H_ */
