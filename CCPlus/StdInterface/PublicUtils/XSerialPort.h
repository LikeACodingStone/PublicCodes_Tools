/***************************************************************************
 *  XSerialPort.h
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : Aug. 27, 2021 09:12:50 AM
 *  Author      : 
 *
 *  Version Info: 1.0
 *
 *  This serial port class is based on ACE library to implement RS232
 *  communication and cross the platform.
 *
 ***************************************************************************/
#pragma once

#include "HGT-common.h"
#include "ace/TTY_IO.h"
#include "ace/DEV_Connector.h"
#include "ace/Time_Value.h"

/*! \brief Communication port status representation structure
*/

typedef unsigned short COMData; /* low byte = data; high byte = errors */
typedef long  Time;
typedef long  Duration;

struct PortRec
{
   bool        IntsOn;  //!< Flag to determine the interrupts on/off status
#ifdef WIN32
   ACE_HANDLE  handle;  //!< Handle to the opened communication port (an ACE_HANDLE on Windows)
#else
   int         handle;  //!< Handle to the opened communication port (a file descriptor on Linux)
#endif
   bool        Opened;  //!< Flag to determine whether communication port is open or not
   int         PortNum; //!< COM port number

   /**
    * Having a default constructor isn't just a nicety, since doing a memset() to
    * all zeros is INCORRECT on non-WIN32 platforms.
    *
    * LOVE the completely random naming style for the elements of this structure.
    * It's awesome.
    */
   PortRec() : IntsOn(false), handle(ACE_INVALID_HANDLE), Opened(false), PortNum(-1)
   {
   }

   /**
    * Having a default constructor isn't just a nicety, since doing a memset() to
    * all zeros is INCORRECT on non-WIN32 platforms.
    */
   void clear()
   {
      IntsOn  = false;
      Opened  = false;
      PortNum = -1;
      handle  = ACE_INVALID_HANDLE;
   }
};

/*! \brief This class handles the communication with the serial port using ACE and POSIX system calls.
*/
class DLL_EXPORT XSerialPort
{
public:
   //! Default Constructor
   XSerialPort();

   //! Virtual Destructor
   virtual ~XSerialPort();

   //! \todo Change code to use this enum!  Do not pass "int" for this!
   enum Parity
   {
      NoParity,
      OddParity,
      EvenParity,
      MarkParity,
      SpaceParity
   };

   /*! \brief Method to initialize the specified serial port with the specified port communication settings

      \param Port - Communication port index
      \param BaudRate - Baud rate for the serial port
      \param Parity - Even or Odd
      \param StopBits - Number of Stop bits
      \param DataBits - data bits length for word
      \param FlowControl - Flow control is used for controlling RTS and CTS. This can also be used for controlling
      RS-485 duplex transceiver on NextGen target. Please set this to true for RS-485.

      \retval bool - return true if the serial port is opened succesfully otherwise returns false
   */
   bool InitPort( int Port, long BaudRate, int Parity, int StopBits, int DataBits, bool FlowControl);

   /*! \brief Method to initialize the specified serial port with the specified port communication settings

      \param PortName - Communication port string name (os dependent)
      \param BaudRate - Baud rate for the serial port
      \param Parity - Even or Odd
      \param StopBits - Number of Stop bits
      \param DataBits - data bits length for word
      \param FlowControl - Flow control is used for controlling RTS and CTS. This can also be used for controlling
      RS-485 duplex transceiver on NextGen target. Please set this to true for RS-485.

      \retval bool - return true if the serial port is opened succesfully otherwise returns false
   */
   bool  InitPort( const char *PortName, long BaudRate, int Parity, int StopBits, int DataBits, bool FlowControl);


   /*! \brief Initialize the specified OPEN serial port with the specified port communication settings.

      \param PortName - Communication port string name (os dependent)
      \param BaudRate - Baud rate for the serial port
      \param Parity - Even or Odd
      \param StopBits - Number of Stop bits
      \param DataBits - data bits length for word
      \param FlowControl - Flow control is used for controlling RTS and CTS. This can also be used for controlling
      RS-485 duplex transceiver on NextGen target. Please set this to true for RS-485.

      \retval bool - return true if the serial port is opened succesfully otherwise returns false
   */
   bool  SetPortParams( long BaudRate, int Parity, int StopBits, int DataBits, bool FlowControl);

   /*! \brief Transmits the requested data buffer on to the communication port.

      \param Data - Data buffer to transmit
      \param Length - Length of the buffer

      \retval size_t - Returns the number of bytes transmitted,
                     -1 indicates failed to write to fail onto the communication port
   */
   size_t SendBlock( void * Data, unsigned int Length);

   /*! \brief Gets a single byte of data from the communication port, if available with in given time

      \param Data - Data buffer for received chars
      \param Timeout - Time before get unblocks and returns with an error

      \retval bool - Returns status of read operation
   */
   bool GetTimed( COMData& Data, Duration Timeout);

   /*! \brief Gets the requested number of bytes from the communication port,
    * if available within given amount of time.
    *
    * \param recv_buffer - Destination buffer to receive the data
    * \param maxByteReadCount - max bytes to read from the communication port
    * \param readTimeout - Maximum wait time for the requested number of bytes
    *  before returning to the caller
    *
    * \retval - Returns results of read operation
    *
    */
   ssize_t GetPacketTimed( char* recv_buffer, const size_t maxBytReadCount, Duration readTimeout );

   /*! \brief
    *
    *  \param Port - Communication port index
    *
    *  \retval ACE_TCHAR -
    */
   ACE_TCHAR const *PortName(int Port) const;

   /*! \brief Returns the number of ports.
    *
    *  \param Port - Communication port index
    *
    *  \retval ACE_TCHAR -
    */
   size_t  NumPorts() const;

   /*! \brief Returns the port id for the given port name. Returns -1 if the name is not valid.
    *
    *  \param Port - Communication port index
    *
    *  \retval ACE_TCHAR -
    */
   int  GetPortIdByName( const ACE_TCHAR *portName );

   /*! \brief

      \param Port - Communication port index

      \retval bool -
   */
   bool OpenSerialPort( const char *portName );

   /*! \brief Closes the serial port.

      \retval bool - false if the port is already closed, or if an error occurs while closing it.
   */
   bool CloseSerialPort();

   /*! \brief Tells if the port is open or not.

      \retval bool - true if the port is open, false if the port is closed.
   */
   bool IsOpen() const { return (m_PortData.Opened); }

   /*! \brief Clear the port's receive buffer
    *
    * In Linux, issues an ioctl to the port
    * 
    * In Windows, this is a NOP
    *
    *
    * \returns true if successful
    */
   bool FlushPort();

   /*! \brief Returns the internal port handle.
    *
    * In Linux, this is the posix (level 1 I/O) device int.
    * In Windows, this is the HANDLE
    *
    * \author abaglioni (5/6/2012)
    *
    * \return int Port Handle.
    */

#ifdef WIN32
   HANDLE  GetHandle() const
   {
      return (m_device.get_handle());
   }
#else
    
   int  GetHandle() const
   {
      return (m_PosixHandle);
   }
#endif

	bool  setPortReadTimeout( Duration readTimeout );
private:
   

#ifdef WIN32
   //! TTY serial communication I/O device
   ACE_TTY_IO                   m_device;

   ACE_DEV_Connector            m_connector;

   //! ACE representation for specifying the serial communication parameters
   ACE_TTY_IO::Serial_Params    m_serconparams;

#else
   int   m_PosixHandle;
#endif

   Duration m_LastReadTimeout;

   //! Serial port communications statistics array
   PortRec  m_PortData;

};


