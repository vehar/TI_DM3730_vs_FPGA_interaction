/*-------------------------------------------------------------------------*/
/*!
 * \file	drv_glob.h
 * \brief	Defines layout of shared memory segment.
 *
 * \b Purpose:
 *	This file contains the declarations of the structures residing in
 *	the reserved Driver Globals area of memory. This area is used by
 *	device drivers and by the ethernet bootloader to pass information
 *	to the kernel.
 *
 * Copyright (c) 2002-2004 Logic Product Development
 *
 * NOTICE:
 *	This file contains source code, ideas, techniques, and information
 *	(the Information) which are Proprietary and Confidential Information
 *	of Logic Product Development, Inc.  This Information may not be used
 *	by or disclosed to any third party except under written license, and
 *	shall be subject to the limitations prescribed under license.
 */
/*-------------------------------------------------------------------------*/

#ifndef _DRV_GLOB_H
#define _DRV_GLOB_H

#include <windows.h>
//#include "hardware.h"


/* Macro used to keep structures aligned. */
#define PAD(label,amt)  unsigned char Pad##label[amt]


/*-------------------------------------------------------------------------*/
/*!
 * drv_glob_boot_params
 *
 * Holds boot-time parameters.
 */
/*-------------------------------------------------------------------------*/
#pragma pack(1)
struct drv_glob_boot_params
{
unsigned char config_string[1024]; /*!< Parameter string passed from LoLo. */
};
#pragma pack()


/*-------------------------------------------------------------------------*/
/*!
 * drv_glob_dbg_enet
 *
 * This structure is used for debugging over ethernet.
 * Note that this struct should NOT be zeroed out by OEMInit()
 * as the eboot bootloader passes us state info.
 */
/*-------------------------------------------------------------------------*/
#define EBOOT_MAGIC_NUM	(0x45424F54) /* "EBOT" */

#pragma pack(1)
struct drv_glob_dbg_enet
{
/* Detects if ether bootloader is present.		*/
DWORD EbootMagicNum;

/* If non-zero, the ethernet card is present.	*/
UCHAR etherEnabled;
};
#pragma pack()

/*-------------------------------------------------------------------------*/
/*!
 * warm_boot_flag
 *
 * This section of driver globals gets populated with the value 'warmboot'
 * if a warm boot is performed. The function OEMInit() inside the OAL checks
 * this value each time it runs. If the value 'warmboot' is found, then the
 * kernel is booted normally. If 'warmboot' is _not_ found, the OAL will
 * clear the system's object store before proceeding to boot the kernel.
 */
/*-------------------------------------------------------------------------*/
#define WARM	(0x7761726d)	/* 'warm'	*/
#define BOOT	(0x626f6f74)	/* 'boot'	*/
#pragma pack(1)
struct drv_glob_warm_boot
{
	DWORD warm;
	DWORD boot;
};
#pragma pack()
/*-------------------------------------------------------------------------*/
/*!
 * DRIVER_GLOBALS
 *
 * This is the actual layout of the entire shared memory region.
 */
/*-------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct _DRIVER_GLOBALS
{
struct drv_glob_boot_params	boot;
struct drv_glob_dbg_enet	eth;
struct drv_glob_warm_boot	warm_boot;
PAD	(0,
		0x2000 -
		(	sizeof(struct drv_glob_dbg_enet)	+
		 	sizeof(struct drv_glob_boot_params)	+
		 	sizeof(struct drv_glob_warm_boot)
		)
	);	/* Pad to fill 0x2000 size */

} DRIVER_GLOBALS, *PDRIVER_GLOBALS;
#pragma pack()


#endif /* closes #ifndef _DRV_GLOB_H */

