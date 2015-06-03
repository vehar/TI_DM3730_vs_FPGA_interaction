/****************************************************************************/
/*																			*/
/*	Module:			mblaster.c	(MicroBlaster)								*/	
/*																			*/
/*					Copyright © Altera Corporation 2004						*/
/*																			*/
/*	Descriptions:	Main source file that manages configuration processes.	*/
/*					IO related control functions are defined in mb_io.c		*/
/*					module.													*/
/*																			*/
/*	Revisions:		1.0	12/10/01 Sang Beng Ng       						*/
/*					Supports Altera ByteBlaster hardware download cable		*/
/*					on Windows NT.											*/
/*					1.1 05/28/04 Chuin Tein Ong								*/
/*					Altera ByteBlaster II Download Cable support added.		*/
/*																			*/
/****************************************************************************/

#include "mb_io.h"
#include <stdio.h>

#if PORT == WINDOWS_NT
#include <conio.h>
#endif


/*////////////////////*/
/* Global Definitions */
/*////////////////////*/
#define SIG_DCLK			0 /* Passive Serial configuration signals */
#define	SIG_NCONFIG			1
#define SIG_DATA0			2
#define SIG_NSTATUS			3
#define SIG_CONFDONE		4

/* Port Mode for ByteBlaster II Cable */
#define PM_RESET_BBII 1 /* Reset */
#define PM_USER_BBII  0 /* User */

/* Port Mode for ByteBlasterMV Cable */
#define PM_RESET_BBMV 0 /* Reset */
#define PM_USER_BBMV  1 /* User */

#define S_CUR				1 /* SEEK_CUR */
#define S_END				2 /* SEEK_END */
#define S_SET				0 /* SEEK_SET */
/********************/
/* User definations */
/*//////////////////*/
#define INIT_CYCLE			50		/* extra DCLK cycles while initialization is in progress */
#define RECONF_COUNT_MAX	5		/* #auto-reconfiguration when error found */
#define	CHECK_EVERY_X_BYTE  10240	/* check NSTATUS pin for error for every X bytes */
									/* DO NOT put '0' */
#define CLOCK_X_CYCLE		0		/* clock another 'x' cycles during user mode (not necessary, 
									   for debug purpose only) */

/*///////////////////////*/
/* Functions Prototyping */
/*///////////////////////*/
int		CheckSignal			( int signal );
void	Dump2Port			( int signal, int data, int clk );
void	PrintError			( int configuration_count );
void	ProcessFileInput	( int finputid );
void	ProgramByte			( int one_byte );
void	SetPortMode			( int mode );

/* Version Number */
const char VERSION[4] = "1.1";

/*//////////////////*/
/* Global variables */
/*//////////////////*/

/* --------------- */
/* Important Notes */
/* --------------- */
/* The following variables are used throughout this file and specifically applies to PORT == WINDOWS_NT */
/* To port to other platforms, e.g. EMBEDDED, user should modify ReadByteBlaster and WriteByteBlaster */
/* functions to translate the signals to I/O port architecture of your system through software */
/* The summary of Port and Bit Position of parallel port architecture is shown below */

/*      Bit       7        6        5        4        3        2        1        0   */
/*      Port 0    -      DATA0      -        -        -        -     NCONFIG    DCLK */
/*      Port 1 !CONF_DONE  -        -      NSTATUS    -        -        -        -   */ /* ! - inverted */
/*      Port 2    -        -        -        -        -        -        -        -   */

/* sig_port_maskbit is a variable that tells the port (index from parallel port base address) and */
/*   the bit positions of signals used in passive serial mode. */
/*   sig_port_maskbit[X][0], where X - SIG_* (e.g. SIG_DCLK), tells the port where the signal */
/*   falls into. */
/*   sig_port_maskbit[X][1], where X - SIG_* (e.g. SIG_DCLK), tells the bit position of the signal */
/*   in the port. */
/*   the sequence is SIG_DCLK, SIG_NCONFIG, SIG_DATA0, SIG_NSTATUS and SIG_CONF_DONE */
int		sig_port_maskbit[5][2]	= { { 0,0x1 }, { 0,0x2 }, { 0,0x40 }, { 1,0x10 }, { 1,0x80 } };

/* port_mode_data sets the signals to particular values in different modes, namely RESET and USER modes. */
/*   port_mode_data[0][Y], where Y - port number, gives the values of each signal for each port in RESET mode. */
/*   port_mode_data[1][Y], where Y - port number, gives the values of each signal for each port in USER mode. */
int		port_mode_data[2][3]	= { {0x42, 0x0, 0x0E}, {0x42, 0x0, 0x0C} };

/* port_data holds the current values of signals for every port. By default, they hold the values in */
/*   reset mode (PM_RESET_<ByteBlaster used>). */
/*   port_data[Z], where Z - port number, holds the value of the port. */
int		port_data[3]			= { 0x42, 0x0, 0x0E };/* Initial value for Port 0, 1 and 2 */


/********************************************************************************/
/*	Name:			Main          												*/
/*																				*/
/*	Parameters:		int argc, char* argv[]										*/
/*					- number of argument.										*/
/*					- argument character pointer.								*/ 
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Open programming file and initialize driver if required		*/
/*					(WINDOWS NT).												*/
/*																				*/
/********************************************************************************/
void main( int argc, char* argv[] )
{
	int file_id;

	/* Introduction */
	fprintf( stdout, "\n================================================\n" );
	fprintf( stdout, "MicroBlaster (MBlaster) Version %s", VERSION );
	fprintf( stdout, "\nALTERA CORPORATION" );
	fprintf( stdout, "\n MicroBlaster version %s supports both", VERSION);
	fprintf( stdout, "\n ByteBlaster II and ByteBlasterMV download cables.");
	fprintf( stdout, "\n MicroBlaster supports SINGLE-DEVICE and ");
	fprintf( stdout, "\n MULTI-DEVICE Passive Serial Configuration." );
	fprintf( stdout, "\n If you turn on the CLKUSR option in Quartus II, you need");
	fprintf( stdout, "\n to initialize the device(s) in order to enter user mode.");
	fprintf( stdout, "\n================================================\n" );

	/* Syntax check */
	if ( argc != 2 )
	{
		fprintf( stderr, "Error: Invalid number of argument! \nSyntax: \"%s <programming_file_name>\"\n", argv[0] );
		return;
	}

	/* Open programming file as READ and in BINARY */
	file_id = fopen_rbf( argv[1], "rb" );

	if ( file_id )
		fprintf( stdout, "Info: Programming file: \"%s\" opened...\n", argv[1] );
	else
	{
		fprintf( stderr, "Error: Could not open programming file: \"%s\"\n", argv[1] );
		return;
	}

	/* Initialize driver */
#if PORT == WINDOWS_NT
	InitNtDriver();
#endif /*PORT == WINDOWS_NT*/

	/* Processing and Programming */
	ProcessFileInput( file_id );

	/* Close driver and programming file */
#if PORT == WINDOWS_NT
	CloseNtDriver();
#endif /*PORT == WINDOWS_NT*/

	if ( file_id )
		fclose_rbf(file_id);
}

/********************************************************************************/
/*	Name:			ProcessFileInput											*/
/*																				*/
/*	Parameters:		FILE* finputid												*/
/*					- programming file pointer.									*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Get programming file size, parse through every single byte	*/
/*					and dump to parallel port.									*/
/*																				*/
/*					Configuration Hardware is verified before configuration		*/
/*					starts.														*/
/*																				*/
/*					For every [CHECK_EVERY_X_BYTE] bytes, NSTATUS pin is		*/
/*					checked for error. When the file size reached, CONF_DONE	*/
/*					pin is checked for configuration status. Then, another		*/
/*					[INIT_CYCLE] clock cycles are dumped while initialization	*/
/*					is in progress.												*/
/*																				*/
/*					Configuration process is restarted whenever error found.	*/
/*					The maximum number of auto-reconfiguration is				*/
/*					[RECONF_COUNT_MAX].											*/
/*																				*/
/********************************************************************************/
void ProcessFileInput( int finputid )
{
	int			program_done = 0;			/* programming process (configuration and initialization) done = 1 */
	int			seek_position = 0;			/* file pointer position */
	long int	file_size = 0;				/* programming file size */
	int			configuration_count = RECONF_COUNT_MAX;	/* # reprogramming after error */
	int			one_byte = 0;				/* the byte to program */
	long int	i = 0;						/* standard counter variable */
	int			confdone_ok = 1;			/* CONF_DONE pin. '1' - error */
	int			nstatus_ok = 0;				/* NSTATUS pin. '0' - error */
	int			clock_x_cycle = CLOCK_X_CYCLE; /* Clock another 'x' cycles during user mode ( not necessary, for debug purpose only) */
	int			BBMV=0;
	int			BBII=0;

	/* Get file size */
	seek_position = fseek_rbf( finputid, 0, S_END );

	if ( seek_position )
	{
		fprintf( stderr, "Error: End of file could not be located!" );
		return;
	}

	file_size	= ftell_rbf( finputid );
	fprintf( stdout, "Info: Programming file size: %ld\n", file_size );
	
	/* Hardware Verification */
#if PORT == WINDOWS_NT
	if(VerifyBBII())		
	{
		if(VerifyBBMV())
		{
			fprintf( stderr, "Error: Verifying hardware: ByteBlaster II or ByteBlasterMV not found or not installed properly...\n" );
			return;
		}
		else BBMV=1;
	}							
	else BBII=1;
#endif

	/* Start configuration */
	while ( !program_done && (configuration_count>0) )
	{
		/* Reset file pointer and parallel port registers */
		fseek_rbf( finputid, 0, S_SET );
		
		if (BBII)
			SetPortMode(PM_RESET_BBII);
		else if (BBMV)
			SetPortMode(PM_RESET_BBMV);

		fprintf( stdout, "\n***** Start configuration process *****\nPlease wait...\n" );

		/* Drive a transition of 0 to 1 to NCONFIG to indicate start of configuration */
		Dump2Port( SIG_NCONFIG, 0, 0 );
		Dump2Port( SIG_NCONFIG, 1, 0 );

		/* Loop through every single byte */
		for ( i = 0; i < file_size; i++ )
		{
			/*one_byte = fgetc( (FILE*) finputid );*/
			one_byte = fgetc_rbf( finputid );

			/* Progaram a byte */
			ProgramByte( one_byte );

			/* Check for error through NSTATUS for every 10KB programmed and the last byte */
			if ( !(i % CHECK_EVERY_X_BYTE) || (i == file_size - 1) )
			{
				nstatus_ok = CheckSignal( SIG_NSTATUS );

				if ( !nstatus_ok )
				{
					PrintError( configuration_count-1 );
					
					program_done = 0;
					break;
				}
				else 
					program_done = 1;
			}
		}

		configuration_count--;

		if ( !program_done )
			continue;

#if PORT == WINDOWS_NT
		/* Flush out the remaining data in Port0 */
		flush_ports();
#endif /*PORT == WINDOWS_NT*/

		/* Configuration end */
		/* Check CONF_DONE that indicates end of configuration */
		confdone_ok = CheckSignal( SIG_CONFDONE );

		if ( confdone_ok )
		{
			fprintf( stderr, "Error: Configuration done but contains error... CONF_DONE is %s\n", (confdone_ok? "LOW":"HIGH") );
			program_done = 0;
			PrintError( configuration_count );			
			if ( configuration_count == 0 )
				break;
		}
	
		/* if contain error during configuration, restart configuration */
		if ( !program_done )
			continue;

		/* program_done = 1; */

		/* Start initialization */
		/* Clock another extra DCLK cycles while initialization is in progress
		   through internal oscillator or driving clock cycles into CLKUSR pin */
		/* These extra DCLK cycles do not initialize the device into USER MODE */
		/* It is not required to drive extra DCLK cycles at the end of
		   configuration													   */
		/* The purpose of driving extra DCLK cycles here is to insert some delay
		   while waiting for the initialization of the device to complete before
		   checking the CONFDONE and NSTATUS signals at the end of whole 
		   configuration cycle 											       */
		for ( i = 0; i < INIT_CYCLE; i++ )
		{
			Dump2Port( SIG_DCLK, 0, 0 );
			Dump2Port( SIG_DCLK, 1, 0 );
		}
		/* Initialization end */

		nstatus_ok = CheckSignal( SIG_NSTATUS );
		confdone_ok = CheckSignal( SIG_CONFDONE );

		if ( !nstatus_ok || confdone_ok )
		{
			fprintf( stderr, "Error: Initialization finish but contains error: NSTATUS is %s and CONF_DONE is %s. Exiting...", (nstatus_ok?"HIGH":"LOW"), (confdone_ok?"LOW":"HIGH") );
			program_done = 0; 
			configuration_count = 0; /* No reconfiguration */
		}
	}

	/* Add another 'x' clock cycles while the device is in user mode.
	   This is not necessary and optional. Only used for debugging purposes */
	if ( clock_x_cycle > 0 )
	{
		fprintf( stdout, "Info: Clock another %d cycles in while device is in user mode...\n", clock_x_cycle );
		for ( i = 0; i < CLOCK_X_CYCLE; i++ )
		{
			Dump2Port( SIG_DCLK, 0, 0 );
			Dump2Port( SIG_DCLK, 1, 0 );
		}
	}

	if ( !program_done )
	{
		fprintf( stderr, "\nError: Configuration not successful! Error encountered...\n" );
		if (BBII)
			SetPortMode(PM_RESET_BBII);
		else if (BBMV)
			SetPortMode(PM_RESET_BBMV);
		return;
	}

	fprintf( stdout, "\nInfo: Configuration successful!\n" );

	if (BBII)
		SetPortMode(PM_USER_BBII);
	else if (BBMV)
		SetPortMode(PM_USER_BBMV);
}

/********************************************************************************/
/*	Name:			CheckSignal													*/
/*																				*/
/*	Parameters:		int signal						 							*/
/*					- name of the signal (SIG_*).								*/
/*																				*/
/*	Return Value:	Integer, the value of the signal. '0' is returned if the	*/
/*					value of the signal is LOW, if not, the signal is HIGH.		*/
/*																				*/
/*	Descriptions:	Return the value of the signal.								*/
/*																				*/
/********************************************************************************/
int CheckSignal( int signal )
{
	int value = 1;
	int	readout = 0;

	readout = ReadByteBlaster( 1 );
	value = readout & sig_port_maskbit[signal][1];

	return value;
}

/********************************************************************************/
/*	Name:			Dump2Port													*/
/*																				*/
/*	Parameters:		int signal, int data, int clk	 							*/
/*					- name of the signal (SIG_*).								*/
/*					- value to be dumped to the signal.							*/
/*					- assert a LOW to HIGH transition to SIG_DCLK togther with	*/
/*					  [signal].													*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Dump [data] to [signal]. If [clk] is '1', a clock pulse is	*/
/*					generated after the [data] is dumped to [signal].			*/
/*																				*/
/********************************************************************************/
void Dump2Port( int signal, int data, int clk )
{
	/* Get signal port number */
	int port = sig_port_maskbit[signal][0];

	/* Get signal mask bit*/
	int mask;
	
	/* If clk == 1, drive signal with [data] and drive SIG_DCLK with '0' together. Then drive SIG_DCLK with '1' */
	/* That is to create a positive edge pulse */
	if ( clk )
		mask = sig_port_maskbit[signal][1] | sig_port_maskbit[SIG_DCLK][1];
	else
		mask = sig_port_maskbit[signal][1];
	
	/* AND signal bit with '0', then OR with [data] */
	mask = ~mask;
	port_data[port] = ( port_data[port] & mask ) | ( data * sig_port_maskbit[signal][1] );
	
	WriteByteBlaster( port, port_data[port], 0 );

	if ( clk )
	{
		WriteByteBlaster( port, ( port_data[port] | sig_port_maskbit[SIG_DCLK][1] ), 0 );
	}
}

/********************************************************************************/
/*	Name:			SetPortMode													*/
/*																				*/
/*	Parameters:		int mode  						 							*/
/*					- The mode of the port (PM_*)								*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Set the parallel port registers to particular values.		*/
/*																				*/
/********************************************************************************/
void SetPortMode( int mode )
{
	/* write to Port 0 and Port 2 with predefined values */
	port_data[0] = port_mode_data[mode][0];
	port_data[2] = port_mode_data[mode][2];
	WriteByteBlaster( 0, port_data[0], 1 );
	WriteByteBlaster( 2, port_data[2], 1 );
}

/********************************************************************************/
/*	Name:			ProgramByte													*/
/*																				*/
/*	Parameters:		int one_byte					 							*/
/*					- The byte to dump.											*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Dump to parallel port bit by bit, from least significant	*/
/*					bit to most significant bit. A positive edge clock pulse	*/
/*					is also asserted.											*/
/*																				*/
/********************************************************************************/
void ProgramByte( int one_byte )
{
	int	bit = 0;
	int i = 0;

	/* write from LSb to MSb */
	for ( i = 0; i < 8; i++ )
	{
		bit = one_byte >> i;
		bit = bit & 0x1;
		
		/* Dump to DATA0 and insert a positive edge pulse at the same time */
		Dump2Port( SIG_DATA0, bit, 1 );
	}
}

/********************************************************************************/
/*	Name:			PrintError 													*/
/*																				*/
/*	Parameters:		int configuration_count										*/
/*					- # auto-reconfiguration left  								*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Print error message to standard error.						*/
/*																				*/
/********************************************************************************/
void PrintError( int configuration_count )
{
	if ( configuration_count == 0 )
		fprintf( stderr, "Error: Error in configuration #%d... \nError: Maximum number of reconfiguration reached. Exiting...\n", (RECONF_COUNT_MAX-configuration_count) );
	else
	{
		fprintf( stderr, "Error: Error in configuration #%d... Restart configuration. Ready? <Press any key to continue>\n", (RECONF_COUNT_MAX-configuration_count) );
#if PORT == WINDOWS_NT
		getch();
#endif
	}
}
