/***************************************************************************
 *  XSerialPort.cpp
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : Oct. 27, 2021 09:18:10 AM
 *  Author      : 
 *
 *  Version Info: 1.0
 *
 ***************************************************************************/

#include <string>
#include <iostream>                     // Includes cerr, endl
#include <stdexcept>

#include "ace/OS_main.h"
#include "ace/ACE.h"
#include "ace/OS_NS_string.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_time.h"
#include "ace/config-lite.h"
#include "ace/os_include/os_stdarg.h"
#include "ace/os_include/os_stddef.h"
#include "ace/os_include/os_termios.h"

#if defined(__UNIX) || defined(unix)
// Include Linux flush routine
#include <termios.h>
#endif

#include "XSerialPort.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef WIN32

static ACE_TCHAR const * const  s_PortName[] =
{
   ACE_TEXT("COM1"), ACE_TEXT("COM2"), ACE_TEXT("COM3"), ACE_TEXT("COM4"),
   ACE_TEXT("COM5"), ACE_TEXT("COM6"), ACE_TEXT("COM7"), ACE_TEXT("COM8"),
   ACE_TEXT("COM9"),
   ACE_TEXT("\\\\.\\COM10"), ACE_TEXT("\\\\.\\COM11"), ACE_TEXT("\\\\.\\COM12"),
   ACE_TEXT("\\\\.\\COM13"), ACE_TEXT("\\\\.\\COM14"), ACE_TEXT("\\\\.\\COM15"),
   ACE_TEXT("\\\\.\\COM16")
};

#elif defined(__UNIX) || defined(unix)

   /* 
    * Port path for Linux, ttySx represents serial ports on CPU and ttyFx 
    * represents serial ports on FPGA 
    */
static const char  *s_PortName[] =
{
   #if defined(__arm__) // for ARM-based architecture
      "/dev/ttymxc0", "/dev/ttymxc1", "/dev/ttymxc2", "/dev/ttymxc3",
      "/dev/ttymxc4", "/dev/ttymxc5", "/dev/ttymxc6", "/dev/ttymxc7",
      "/dev/ttymxc8", "/dev/ttymxc9", "/dev/ttymxc10", "/dev/ttymxc11",
      "/dev/ttymxc12", "/dev/ttymxc13", "/dev/ttymxc14", "/dev/ttymxc15"
   #else // PowerPC & other Linux architectures
      "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3",
      "/dev/ttyS4", "/dev/ttyS5", "/dev/ttyS6", "/dev/ttyS7",
      "/dev/ttyS8", "/dev/ttyS9", "/dev/ttyS10", "/dev/ttyS11",
      "/dev/ttyS12", "/dev/ttyS13", "/dev/ttyS14", "/dev/ttyS15"
   #endif
};

struct BaudRates 
{
   long         rate;
   unsigned int cflag;
};

//Baud rate flags table
static struct BaudRates SupportedBaudRates[] =
{
   //{ 250000, B250000 },
   { 115200, B115200 },
   {  57600, B57600  },
   {  38400, B38400  },
   {  19200, B19200  },
   {   9600, B9600   },
   {   4800, B4800   },
   {   2400, B2400   },
   {   1200, B1200   },
   {    600, B600    },
   {    300, B300    },
   {    150, B150    },
};

#endif

const int  MAX_X_PORTS = NUM_ELEMENTS(s_PortName);

ACE_TCHAR const * XSerialPort::PortName(int Port) const
{
   //! \TODO - do something other than hard-code the number of ports based on OS
   if (Port >= 0 && Port < MAX_X_PORTS)
      return s_PortName[Port];
	std::cout << "port number = " << Port << std::endl;
   return ( ACE_TEXT("Unknow port") );
}

size_t  XSerialPort::NumPorts() const
{
   return (MAX_X_PORTS);
}


/* \brief Returns the port id for the given port name.
 * Returns -1 if the name is not valid.
 *
 *  \param Port - Communication port index
 *
 *  \retval int  0 to NumPorts() for a valid port name, -1 if not valid.
 */
int  XSerialPort::GetPortIdByName( const ACE_TCHAR *portName )
{
   if (portName != NULL && *portName != 0)
   {
      for (int port=0; port<MAX_X_PORTS; ++port)
      {
#if WIN32
         if (_stricmp( portName, s_PortName[port]) == 0)
            return (port);
#else
         if (strcasecmp( portName, s_PortName[port]) == 0)
            return (port);
#endif
      }
   }

   return (-1);
}


XSerialPort::XSerialPort()
   :
#ifdef WIN32
   m_device(),
   m_connector(),
   m_serconparams(),
#else
   m_PosixHandle(-1),
#endif
   m_LastReadTimeout(-1),
   m_PortData()
{
}

XSerialPort::~XSerialPort()
{

}

bool  XSerialPort::InitPort( int Port, long BaudRate, int Parity, int StopBits, int DataBits, bool FlowControl)
{
   const char *name = PortName(Port);

   m_PortData.PortNum = Port;

   return (InitPort( name, BaudRate, Parity, StopBits, DataBits, FlowControl));
}

bool  XSerialPort::InitPort( const char *PortName, long BaudRate, int Parity, int StopBits, int DataBits, bool FlowControl)
{
   if (!OpenSerialPort(PortName))
      return (false);

   return (SetPortParams( BaudRate, Parity, StopBits, DataBits, FlowControl));
}

/* Initialize the specified OPEN serial port with the specified port communication settings.

   \param BaudRate - Baud rate for the serial port
   \param Parity - Even or Odd
   \param StopBits - Number of Stop bits
   \param DataBits - data bits length for word
   \param FlowControl - Flow control is used for controlling RTS and CTS. This can also be used for controlling
   RS-485 duplex transceiver on NextGen target. Please set this to true for RS-485.

   \retval bool - return true if the serial port is opened succesfully otherwise returns false
*/
bool  XSerialPort::SetPortParams( long BaudRate, int Parity, int StopBits, int DataBits, bool FlowControl)
{
   // Port *MUST* be open to set its parameters.
   if (!IsOpen())
      return (false);

#if WIN32
   //Since GETPARAMS is not yet implemented in the ACE-5.4 version the below lines
   //were commented for the time being.
   /*
   if (m_device.control(ACE_TTY_IO::GETPARAMS, &m_serconparams))
      return (FALSE);
   */

   // Set parameters of params structure here
   m_serconparams.baudrate = BaudRate;

   switch(Parity)
   {
   default:
      {
      std::string message = "Invalid Parity in XSerialPort.";
      throw std::logic_error(message);
      }
      break;

   case XSerialPort::NoParity:
#if (ACE_MAJOR_VERSION <=5 && ACE_MINOR_VERSION <= 4) 
      m_serconparams.parityenb  = 0; // only needed in ACE 5.4.4 version
#endif
      m_serconparams.paritymode = "none";
      break;

   case XSerialPort::OddParity:
#if (ACE_MAJOR_VERSION <=5 && ACE_MINOR_VERSION <= 4) 
      m_serconparams.parityenb  = 1; // only needed in ACE 5.4.4 version
#endif
      m_serconparams.paritymode = "odd";
      break;

   case XSerialPort::EvenParity:
#if (ACE_MAJOR_VERSION <=5 && ACE_MINOR_VERSION <= 4) 
      m_serconparams.parityenb  = 1; // only needed in ACE 5.4.4 version
#endif
      m_serconparams.paritymode = "even";
      break;

   case XSerialPort::MarkParity:
#if (ACE_MAJOR_VERSION <=5 && ACE_MINOR_VERSION <= 4) 
      m_serconparams.parityenb  = 1; // only needed in ACE 5.4.4 version
#endif
      m_serconparams.paritymode = "mark";
      break;
   
   case XSerialPort::SpaceParity:
#if (ACE_MAJOR_VERSION <=5 && ACE_MINOR_VERSION <= 4) 
      m_serconparams.parityenb  = 1; // only needed in ACE 5.4.4 version
#endif
      m_serconparams.paritymode = "space";
      break;
   }

   switch (StopBits)
   {
      default:
         throw std::logic_error("Invalid StopBits in XSerialPort.");
         break;

      case 1:
         m_serconparams.stopbits = 1;  //ONESTOPBIT
         break;
      case 2:
         m_serconparams.stopbits = 2;  //TWOSTOPBITS
         break;
   }

   m_serconparams.databits = DataBits;

   //setting the cts enabled flag to false
   m_serconparams.ctsenb = 0;

   //setting the rcv enabled flag to true
   m_serconparams.rcvenb = 1;

   //set the default timeout period
   m_serconparams.readtimeoutmsec = 100;

   if (m_device.control(ACE_TTY_IO::SETPARAMS, &m_serconparams))
      return (false);

#else    // For Linux
   uint iter;

   //Setting current port options
   struct termios portOptions;

   for (iter = 0; iter < NUM_ELEMENTS(SupportedBaudRates); ++iter)
   {
      if (SupportedBaudRates[iter].rate <= BaudRate)
         break;
   }

   // If we don't find the baud rate, return false.
   if (iter >= NUM_ELEMENTS(SupportedBaudRates))
      return (false);

   cfsetispeed( &portOptions, SupportedBaudRates[iter].cflag);
   cfsetospeed( &portOptions, SupportedBaudRates[iter].cflag);

   cfmakeraw(&portOptions);/* To make port as RAW Port*/

   portOptions.c_cflag |= (CLOCAL | CREAD); /* Enable receiver and Set Local mode */

   switch(Parity)
   {
   case XSerialPort::MarkParity:
   case XSerialPort::SpaceParity:
   default:
      {
      std::string message = "Invalid Parity in XSerialPort.";
      throw std::logic_error(message);
      }
      break;

   case XSerialPort::NoParity:
      portOptions.c_cflag &= ~PARENB; /* No parity */
      break;

   case XSerialPort::OddParity:
      portOptions.c_cflag |= PARENB;   /* Enable parity */
      portOptions.c_cflag |= PARODD;   /* ODD parity */
      break;

   case XSerialPort::EvenParity:
      portOptions.c_cflag |= PARENB;   /* Enable parity */
      portOptions.c_cflag &= ~PARODD;  /* EVEN parity */
      break;
   }

   switch (StopBits)
   {
   default:
      throw std::logic_error("Invalid StopBits in XSerialPort.");
      break;
   case 1:
      portOptions.c_cflag &= ~CSTOPB;  /* 1 stop bit */
      break;
   case 2:
      portOptions.c_cflag |= CSTOPB;   /* 2 stop bits */
      break;
   }

   portOptions.c_cflag &= ~CSIZE;
   switch (DataBits)
   {
   default:
      throw std::logic_error("Invalid Parity in XSerialPort.");
      break;

   case 5:
      portOptions.c_cflag |= CS5;    /* 5 data bits */
      break;
   case 6:
      portOptions.c_cflag |= CS6;    /* 6 data bits */
      break;
   case 7:
      portOptions.c_cflag |= CS7;    /* 7 data bits */
      break;
   case 8:
      portOptions.c_cflag |= CS8;    /* 8 data bits */
      break;
   }

   if (FlowControl)
      portOptions.c_cflag |= CRTSCTS;
   else
      portOptions.c_cflag &= ~CRTSCTS;

   //set the input mode (canonial)
   portOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

   portOptions.c_cc[VTIME] = 1; // Value specified in 0.1 sec, maximum time delay between charecters received
   portOptions.c_cc[VMIN] = 0;   // Minimum number of charecters received, 0 value for pure timed read

   portOptions.c_oflag &= ~OPOST;

   portOptions.c_iflag |= IGNPAR;
   portOptions.c_iflag &= ~(IXON | IXOFF | IXANY | INPCK);

   /* Updating the Serial Port with modified options */

   tcsetattr(m_PosixHandle,TCSANOW,&portOptions);
#endif

   return (true);
}


bool XSerialPort::GetTimed( COMData& Data, Duration readTimeout)
{
   bool rval = false;

   if (!m_PortData.Opened)
   {
		ACE_ERROR((LM_ERROR, "Port %s not opened, read failure!\n", PortName(m_PortData.PortNum)));
   }
   else if (setPortReadTimeout(readTimeout)) // If we cannot set the timeout, we will not read: we fail.
   {
      char    read_data = 0;
      ssize_t NumberOfBytesRead = 0;

#ifdef WIN32
      //set the timeout, Right now ACE doesn't use this timeout entry in Windows platform!
      ACE_Time_Value  acetimeout( readTimeout / 1000, readTimeout % 1000 * 1000 );

      NumberOfBytesRead = m_device.recv_n( &read_data, (size_t)1, &acetimeout );

#else //For Linux

      NumberOfBytesRead = read( m_PosixHandle, &read_data, 1 );

#endif
      if (NumberOfBytesRead == 1)
      {
         // copy the received data to the caller requested buffer
         Data = uchar( read_data );
         rval = true;
      }
   }

   return(rval);
}


ssize_t XSerialPort::GetPacketTimed( char* recv_buffer, const size_t maxBytesRead, Duration readTimeout )
{
   if (!m_PortData.Opened)
   {
		ACE_ERROR((LM_ERROR, "Port %s not opened, read failure!\n", PortName(m_PortData.PortNum)));
		return -1;
   }

#ifdef WIN32
   // If we cannot set the timeout, return an error value.
   if (!setPortReadTimeout(readTimeout))
      return (-1);

   //set the timeout, ACE uses this timeout entry in Linux platforms only!
   ACE_Time_Value aceReadtimeout( readTimeout / 1000,
                                 (readTimeout % 1000) * 1000 );

   size_t numCharsRead = 0;
   m_device.recv_n( recv_buffer,
                    maxBytesRead,
                    &aceReadtimeout,
                    &numCharsRead );

#else //For Linux
   ssize_t numCharsRead = 0;

   timeval read_timeout;
   read_timeout.tv_sec = readTimeout / 1000;
   read_timeout.tv_usec = (readTimeout % 1000) * 1000;

   fd_set readset;
   FD_ZERO(&readset);
   FD_SET(m_PosixHandle, &readset);

   int select_rval = select(m_PosixHandle + 1, &readset, (fd_set *)NULL, (fd_set *)NULL, &read_timeout);
/*
   if (select_rval > 0 && FD_ISSET(m_PosixHandle, &readset))
   {
      numCharsRead = read( m_PosixHandle, recv_buffer, maxBytesRead);
   }
*/
   if (select_rval > 0 && FD_ISSET(m_PosixHandle, &readset))
   {
	  int count = 0;
      char recv[16];
	  while(true)
      {
      	count = read( m_PosixHandle, recv, 16);
		if (count <= 0)
			break;
		
		//overflow
		if (maxBytesRead < numCharsRead + count)
			break;
		memcpy(&recv_buffer[numCharsRead], recv, count);
		numCharsRead += count;
		
	  }
   }

#endif

   return (numCharsRead);
}


bool  XSerialPort::setPortReadTimeout( Duration readTimeout )
{
   // Don't keep resetting the timeout if it is already set.
   if (readTimeout != m_LastReadTimeout)
   {
      m_LastReadTimeout = readTimeout;

#ifdef WIN32
      // the recv_n call of ACE is working fine under Linux platform but under
      // windows platform the timeout parameter is ignored. Hence ACE_TTY_IO
      // control method to set the serial port parameters is explicitly used
      // under windows platform.

      m_serconparams.readtimeoutmsec = readTimeout;
      m_serconparams.readmincharacters = -1;
      m_serconparams.rcvenb = TRUE;

      if (m_device.control( ACE_TTY_IO::SETPARAMS, &m_serconparams) != 0)
      {
         return (false);
      }

#else //For Linux
      // Convert the time delay to 100ms units, rounded up to nearest 100ms.
      ulong  waitTime = (readTimeout + 99) / 100;

      //Setting current port options
      struct termios  portOptions;

      //Get the current port options
      tcgetattr( m_PosixHandle, &portOptions);
      portOptions.c_cc[VTIME] = waitTime; // Value specified in 0.1 sec, maximum time delay between charecters received
      portOptions.c_cc[VMIN]  = 0;        // Minimum number of characters received, 0 value for pure timed read

      //Set the modified port options
      tcsetattr( m_PosixHandle, TCSANOW, &portOptions);

      fcntl( m_PosixHandle, F_SETFL, 0);

#endif
   }

   return (true);
}

size_t XSerialPort::SendBlock( void * Data, unsigned int Length)
{
   size_t rval = 0;
   if (m_PortData.Opened)
   {
      size_t NumberOfBytesWritten = 0;

#ifdef WIN32
      NumberOfBytesWritten = m_device.send(Data,Length);
#else //For Linux
      NumberOfBytesWritten = write( m_PosixHandle, Data, Length );
#endif

      rval = NumberOfBytesWritten;
   }
   else
   {
	   ACE_ERROR((LM_ERROR, "Port %s not opened, send failure!\n", PortName(m_PortData.PortNum)));
   }

   return(rval);
}


// This opens a serial port
//   Return true     ->if port is present and no errors occurred
//   Return false    ->otherwise
bool XSerialPort::OpenSerialPort( const char  *PortName )
{
   // If port is already open, must close it before re-opening it.
   // Trying to open an already open port is an error.
   if (m_PortData.Opened)
      return false;

#ifdef WIN32
   // Even if we fail, it's nice to keep track of what we failed at.
   //

   int retstat = m_connector.connect( m_device, ACE_DEV_Addr(PortName));
   if (retstat == -1 )
      return false;

   m_PortData.handle  = m_device.get_handle();
   m_PortData.Opened  = true;
#else
   m_PosixHandle = open( PortName, O_RDWR | O_NOCTTY );
   if (m_PosixHandle == -1 )
      return false;

   m_PortData.handle = m_PosixHandle;
   m_PortData.Opened = true;
#endif

   return true;
}

// This routine close the respective handle for a opened serial port
// It sets Opened to false and Set hanlde to zero (0)
bool XSerialPort::CloseSerialPort()
{
#ifdef WIN32
   if (m_PortData.Opened && !(m_device.close()))
   {
      m_PortData.Opened  = false;
      m_PortData.handle  = ACE_INVALID_HANDLE;
      m_PortData.PortNum = -1;
      return true;
   }

#else    //For Linux

   if (m_PortData.Opened && close(m_PosixHandle) == 0)
   {
      m_PortData.Opened  = false;
      m_PortData.handle  = m_PosixHandle = -1;
      m_PortData.PortNum = -1;
      return true;
   }

#endif

   return false;
}

bool XSerialPort::FlushPort()
{
   // Port *MUST* be open to set its parameters.
   if (!IsOpen())
   {
      return false;
   }

#ifdef WIN32

   // Only supported on the target
   return true;

#else

   int rc = tcflush(m_PosixHandle, TCIOFLUSH);
   return (rc == 0);

#endif
}

