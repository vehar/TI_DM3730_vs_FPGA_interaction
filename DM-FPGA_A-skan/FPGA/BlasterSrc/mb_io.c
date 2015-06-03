/****************************************************************************/
/*																			*/
/*	Module:			mb_io.c	(MicroBlaster)									*/	
/*																			*/
/*					Copyright (C) Altera Corporation 2004					*/
/*																			*/
/*	Descriptions:	Defines all IO control functions. operating system		*/
/*					is defined here. Functions are operating system 		*/
/*					dependant.												*/
/*																			*/
/*	Revisions:		1.0	12/10/01 Sang Beng Ng								*/
/*					Supports Altera ByteBlaster hardware download cable		*/
/*					on Windows NT.											*/
/*					1.1	05/28/04 Chuin Tein Ong								*/
/*					Supports both Altera ByteBlaster II and ByteBlaster MV  */
/*					download cables	on Windows NT.							*/
/*																			*/
/****************************************************************************/

#include "mb_io.h"
#include <stdio.h>

#if PORT == WINDOWS_NT
#include <windows.h>
#else if PORT == EMBEDDED
/* include your library here */
#endif


/*////////////////////*/
/* Global Definitions */
/*////////////////////*/
#if PORT == WINDOWS_NT

#define	PGDC_IOCTL_GET_DEVICE_INFO_PP	0x00166A00L
#define PGDC_IOCTL_READ_PORT_PP			0x00166A04L
#define PGDC_IOCTL_WRITE_PORT_PP		0x0016AA08L
#define PGDC_IOCTL_PROCESS_LIST_PP		0x0016AA1CL
#define PGDC_WRITE_PORT					0x0a82
#define PGDC_HDLC_NTDRIVER_VERSION		2
#define PORT_IO_BUFFER_SIZE				256

#endif /*PORT == WINDOWS_NT*/


/*//////////////////*/
/* Global Variables */
/*//////////////////*/
#if PORT == WINDOWS_NT

HANDLE	nt_device_handle		= INVALID_HANDLE_VALUE;
int		port_io_buffer_count	= 0;

struct PORT_IO_LIST_STRUCT
{
	USHORT command;
	USHORT data;
} port_io_buffer[PORT_IO_BUFFER_SIZE];

#else if PORT == EMBEDDED
/* define your global variables here */
#endif /*PORT == WINDOWS_NT*/


#if PORT == WINDOWS_NT /* InitNtDriver, CloseNtDriver and flush_ports functions are needed for Windows NT only */
/********************************************************************************/
/*	Name:			InitNtDriver  												*/
/*																				*/
/*	Parameters:		None.          												*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Initiallize Windows NT Driver for ByteBlasterMV.			*/
/*																				*/
/********************************************************************************/
void InitNtDriver()
{
	int init_ok = 0;	/* Initialization OK */

	ULONG buffer[1];
	ULONG returned_length = 0;
	char nt_lpt_str[] = { '\\', '\\', '.', '\\', 'A', 'L', 'T', 'L', 'P', 'T', '1', '\0' }; 

	nt_device_handle = CreateFile( 
			nt_lpt_str,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

	if ( nt_device_handle == INVALID_HANDLE_VALUE )
		fprintf( stderr, "I/O Error: Cannot open device \"%s\"\n", nt_lpt_str );
	else
	{
		if ( DeviceIoControl(
				nt_device_handle,
				PGDC_IOCTL_GET_DEVICE_INFO_PP,
				(ULONG *) NULL,
				0,
				&buffer,
				sizeof(ULONG),
				&returned_length,
				NULL ))
		{
			if ( returned_length == sizeof( ULONG ) )
			{
				if (buffer[0] == PGDC_HDLC_NTDRIVER_VERSION)
				{
					init_ok = 1;
					fprintf( stdout, "Info: Port \"%s\" opened...\n", nt_lpt_str );
				}
				else
				{
					fprintf(stderr,
						"I/O Error:  device driver %s is not compatible\n(Driver version is %lu, expected version %lu.\n",
						nt_lpt_str,
						(unsigned long) buffer[0],
						(unsigned long) PGDC_HDLC_NTDRIVER_VERSION );
				}
			}	
			else
				fprintf(stderr, "I/O Error:  device driver %s is not compatible.\n", nt_lpt_str);		
		}

		if ( !init_ok )
		{
			fprintf( stderr, "I/O Error: DeviceIoControl not successful" );
			CloseHandle( nt_device_handle );
			nt_device_handle = INVALID_HANDLE_VALUE;
		}
	}

	if ( !init_ok )
	{
		fprintf( stderr, "Error: Driver initialization fail... Exiting...\n" );
		CloseNtDriver();
		exit(1);
	}
}

/********************************************************************************/
/*	Name:			CloseNtDriver 												*/
/*																				*/
/*	Parameters:		None.          												*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Close Windows NT Driver.									*/
/*																				*/
/********************************************************************************/
void CloseNtDriver()
{
	CloseHandle( nt_device_handle );
	nt_device_handle = INVALID_HANDLE_VALUE;
}

/********************************************************************************/
/*	Name:			flush_ports 												*/
/*																				*/
/*	Parameters:		None.          												*/
/*																				*/
/*	Return Value:	None.														*/
/*																				*/
/*	Descriptions:	Flush processes in [port_io_buffer]	and reset buffer		*/
/*					size to 0.													*/
/*																				*/
/********************************************************************************/
void flush_ports(void)
{
	ULONG n_writes = 0L;
	BOOL status;

	status = DeviceIoControl(
		nt_device_handle,			/* handle to device */
		PGDC_IOCTL_PROCESS_LIST_PP,	/* IO control code */
		(LPVOID)port_io_buffer,		/* IN buffer (list buffer) */
		port_io_buffer_count * sizeof(struct PORT_IO_LIST_STRUCT),/* length of IN buffer in bytes */
		(LPVOID)port_io_buffer,	/* OUT buffer (list buffer) */
		port_io_buffer_count * sizeof(struct PORT_IO_LIST_STRUCT),/* length of OUT buffer in bytes */
		&n_writes,					/* number of writes performed */
		0);							/* wait for operation to complete */

	if ((!status) || ((port_io_buffer_count * sizeof(struct PORT_IO_LIST_STRUCT)) != n_writes))
	{
		fprintf(stderr, "I/O Error:  Cannot access ByteBlaster hardware\n");
		CloseHandle(nt_device_handle);
		exit(1);
	}

	port_io_buffer_count = 0;
}

/******************************************************************/
/* Name:         VerifyBBII (ByteBlaster II)					  */
/*                                                                */
/* Parameters:   None.                                            */
/*                                                                */
/* Return Value: '0' if verification is successful;'1' if not.    */
/*               		                                          */
/* Descriptions: Verify if ByteBlaster II is properly attached to */
/*               the parallel port.                               */
/*                                                                */
/******************************************************************/
int VerifyBBII()
{
	int error = 0;
	int test_count = 0;
	int read_data = 0;
	
	for ( test_count = 0; test_count < 2; test_count++ )
	{
		/* Write '0' to Pin 6 (Data4) for the first test and '1' for the second test */
		int vector = (test_count) ? 0x10 : 0x0;/* 0001 0000:0000 0000... drive to Port0 */
		int expect = (test_count) ? 0x40 : 0x0;/* X1XX 0XXX:X0XX 0XXX... expect from Port1 */

		WriteByteBlaster( 0, vector, 1 );
		
		/* Expect '0' at Pin 10 (Ack) and Pin 15 (Error) for the first test */
		/* and '1' at Pin 10 (Ack) and '0' Pin 15 (Error) for the second test */
		read_data = ReadByteBlaster( 1 ) & 0x40;

		/* If no ByteBlaster II detected, error = 1 */
		if (test_count==0)
		{
			if(read_data==0x00)
				error=0;
			else error=1;
		}

		if (test_count==1)
		{
			if(read_data==0x40)
				error=error|0;
			else error=1;
		}
	}

	
	if (!error)
	{
		fprintf( stdout, "Info: Verifying hardware: ByteBlaster II found...\n" );
		return error;
	}
	else
		/*fprintf( stderr, "Error: Verifying hardware: ByteBlaster II not found or not installed properly...\n" );*/

	return error;
}

/******************************************************************/
/* Name:         VerifyBBMV (ByteBlasterMV) 					  */
/*                                                                */
/* Parameters:   None.                                            */
/*                                                                */
/* Return Value: '0' if verification is successful;'1' if not.    */
/*               		                                          */
/* Descriptions: Verify if ByteBlasterMV is properly attached to  */
/*               the parallel port.                               */
/*                                                                */
/******************************************************************/
int VerifyBBMV()
{
	int error = 0;
	int test_count = 0;
	int read_data = 0;
	
	for ( test_count = 0; test_count < 2; test_count++ )
	{
		/* Write '0' to Pin 7 and Pin 9 (Data5,7) for the first test and '1' for the second test */
		int vector = (test_count) ? 0xA0 : 0x0;/* 1010 0000:0000 0000... drive to Port0 */
		int expect = (test_count) ? 0x60 : 0x0;/* X11X XXXX:X00X XXXX... expect from Port1 */

		WriteByteBlaster( 0, vector, 1 );
		
		/* Expect '0' at Pin 10 and Pin 12 (Ack and Paper End) for the first test and '1' for the second test */
		read_data = ReadByteBlaster( 1 ) & 0x60;

		/* If no ByteBlasterMV detected, error = 1 */
		if (test_count==0)
		{
			if(read_data==0x00)
				error=0;
			else error=1;
		}

		if (test_count==1)
		{
			if(read_data==0x60)
				error=error|0;
			else error=1;
		}
	}
	
	if (!error)
	{
		fprintf( stdout, "Info: Verifying hardware: ByteBlasterMV found...\n" );
		return error;
	}
	else
		/*fprintf( stderr, "Error: Verifying hardware: ByteBlasterMV not found or not installed properly...\n" );*/

	return error;
}

#else if PORT == EMBEDDED

/* put your code for driver initializing, closing and flushing (optional) code here */

#endif /*PORT == WINDOWS_NT: InitNtDriver, CloseNtDriver and flush_ports functions are used for needed for Windows NT only */


/********************************************************************************/
/*	Name:			ReadByteBlaster												*/
/*																				*/
/*	Parameters:		int port       												*/
/*					- port number 0, 1, or 2. Index to parallel port base		*/
/*					  address.													*/
/*																				*/
/*	Return Value:	Integer, value of the port.									*/
/*																				*/
/*	Descriptions:	Read the value of the port registers.						*/
/*																				*/
/********************************************************************************/
int ReadByteBlaster( int port )
{
	int data = 0;

#if PORT == WINDOWS_NT
	int status = 0;
	int returned_length = 0;

	status = DeviceIoControl(
			nt_device_handle,			/* Handle to device */
			PGDC_IOCTL_READ_PORT_PP,	/* IO Control code for Read */
			(ULONG *)&port,				/* Buffer to driver. */
			sizeof(int),				/* Length of buffer in bytes. */
			(ULONG *)&data,				/* Buffer from driver. */
			sizeof(int),				/* Length of buffer in bytes. */
			(ULONG *)&returned_length,	/* Bytes placed in data_buffer. */
			NULL);						/* Wait for operation to complete */

	if ((!status) || (returned_length != sizeof(int)))
	{
		fprintf(stderr, "I/O error:  Cannot access ByteBlaster hardware\n");
		CloseHandle(nt_device_handle);
		CloseNtDriver();
		exit(1);
	}

#else if PORT == EMBEDDED

/* put your code for reading from I/O port here */
/* please refer to "global variables" section of mblaster.c for important notes */

#endif /*PORT == WINDOWS_NT*/

	return (data & 0xff);
}

/********************************************************************************/
/*	Name:			WriteByteBlaster											*/
/*																				*/
/*	Parameters:		int port, int data, int test								*/
/*					- port number 0, 1, or 2. Index to parallel port base		*/
/*					  address.													*/
/*					- value to written to port registers.						*/
/*					- purpose of write.											*/ 
/*																				*/
/*	Return Value:	None                       									*/
/*																				*/
/*	Descriptions:	Write [data] to [port] registers. When dump to Port0, if	*/
/*					[test] = '0', processes in [port_io_buffer] are dumped		*/
/*					when [PORT_IO_BUFFER_SIZE] is reached. If [test] = '1',		*/
/*					[data] is dumped immediately to Port0.						*/
/*																				*/
/********************************************************************************/
void WriteByteBlaster( int port, int data, int test )
{
	int status = 0;

#if PORT == WINDOWS_NT
	int returned_length = 0;
	int buffer[2];

	/* Collect up to [PORT_IO_BUFFER_SIZE] data for Port0, then flush them */
	/* if test = 1 or Port = 1 or Port = 2, writing to the ports are done immediately */
	if (port == 0 && test == 0)
	{
		port_io_buffer[port_io_buffer_count].data = (USHORT) data;
		port_io_buffer[port_io_buffer_count].command = PGDC_WRITE_PORT;
		++port_io_buffer_count;

		if (port_io_buffer_count >= PORT_IO_BUFFER_SIZE) flush_ports();
	}
	else
	{
		buffer[0] = port;
		buffer[1] = data;

		status = DeviceIoControl(
				nt_device_handle,			/* Handle to device */
				PGDC_IOCTL_WRITE_PORT_PP,	/* IO Control code for write */
				(ULONG *)&buffer,			/* Buffer to driver. */
				2 * sizeof(int),			/* Length of buffer in bytes. */
				(ULONG *)NULL,				/* Buffer from driver.  Not used. */
				0,							/* Length of buffer in bytes. */
				(ULONG *)&returned_length,	/* Bytes returned.  Should be zero. */
				NULL);						/* Wait for operation to complete */
	}
#else if PORT == EMBEDDED

/* put your code for writing to I/O port here */
/* please refer to "global variables" section of mblaster.c for important notes */

#endif /*PORT == WINDOWS_NT*/
}

int fopen_rbf( char argv[], char* mode)
{
	FILE* file_id;

	file_id = fopen( argv, mode );

	return (int) file_id;
}

int	fclose_rbf( int file_id)
{
	fclose( (FILE*) file_id);

	return 0;
}

int fseek_rbf( int finputid, int start, int end )
{
	int seek_position;

	seek_position = fseek( (FILE*) finputid, start, end );

	return seek_position;
}

int ftell_rbf( int finputid )
{
	int file_size;

	file_size = ftell( (FILE*) finputid );

	return file_size;
}

int fgetc_rbf( int finputid )
{
	int one_byte;

	one_byte = fgetc( (FILE*) finputid );

	return one_byte;
}

void delay ( int factor)
{
	int i;
	for (i=0;i<factor;i++);
}