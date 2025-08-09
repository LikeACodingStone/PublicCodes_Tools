//------------------------------------------------------------------------------
/*! \file AsciiHex.h 
 * \brief Template for converting variables to ascii hex strings
 *
 * Created: Oct. 29, 2021
 *
 * Description: Ascii Hex conseversion functions.
 *
 * Copyright Health Gene Technologies 2021-2022
 */
//------------------------------------------------------------------------------

#pragma once 

#include <ctype.h>                      // for toupper()
#include <boost/cstdint.hpp>            // for platform independent data types for int, long, etc

/*! \brief Template class to convert value to ASCII HEX form
 *
 * Generally, developers should use the typedefs below, instead of directly
 * using this class. \sa charToAsciiHex, shortToAsciiHex, longToAsciiHex,
 * floatToAsciiHex, doubleToAsciiHex, flagToAsciiHex.
 */
template<typename DATA>
struct ValueToAsciiHex
{
   char  m_text[2*sizeof(DATA)+1];

   const char *c_str()
   {
      return (m_text);
   }

   union ByteData
   {
      DATA          as_value;
      unsigned char as_bytes[sizeof(DATA)];

      void ByteSwap()  //!< PC is little endian, ARM is big endian
      {
        #ifndef __powerpc__
         volatile unsigned short dsize = sizeof(as_bytes)/2; // volatile is here to avoid warnings
         for (unsigned short i=0; i<dsize; ++i)
         {  
            unsigned short j    = (sizeof(as_bytes) - 1) - (unsigned short)i;
            unsigned char  temp = as_bytes[i];
            as_bytes[i]         = as_bytes[j];
            as_bytes[j]         = temp;
         }
        #endif
      }
   };

   /*! \brief Constructor requires data value.
    *
    * The expected usage of ValueToAsciiHex is shown below:
    *
    * short  sh=getShortValue();<br><br>
    * Output( ValueToAsciiHex<short>(sh).c_str() );<br>
    *
    */
   ValueToAsciiHex(DATA value) 
   {
      SetValue(value);
   }

   //! Alternate post-construction way to alter value.
   void  SetValue(DATA value) 
   {
      ByteData  convert={value};

      m_text[2*sizeof(DATA)] = 0;   // Terminate the string

      convert.ByteSwap(); //!< PC is little endian, ARM is big endian

      char *p = m_text;
      for (unsigned int i=0; i < sizeof(DATA); ++i)
      {
         *p = (convert.as_bytes[i] >> 4) + '0';
         if (*p > '9')
            *p += 7;

         p++;

         *p = (convert.as_bytes[i] & 0x0F) + '0';
         if (*p > '9')
            *p += 7;

         p++;
      }
   }
};

typedef ValueToAsciiHex<boost::int8_t>   charToAsciiHex;     //!< 1 byte  =  2 ascii hex characters  
typedef ValueToAsciiHex<boost::int16_t>  shortToAsciiHex;    //!< 2 bytes =  4 ascii hex characters
typedef ValueToAsciiHex<boost::int32_t>  longToAsciiHex;     //!< 4 bytes =  8 ascii hex characters
typedef ValueToAsciiHex<boost::int64_t>  longlongToAsciiHex; //!< 4 bytes =  8 ascii hex characters

typedef ValueToAsciiHex<float>  floatToAsciiHex;   //!< 4 bytes =  8 ascii hex characters
typedef ValueToAsciiHex<double> doubleToAsciiHex;  //!< 8 bytes = 16 ascii hex characters

//! Dummy type to specialize templates
struct OneCharFlag { char  oneChar; };

//! Specialization for 1 character flags
template<>
struct ValueToAsciiHex<OneCharFlag>
{
   char  m_text[2];

   const char *c_str()
   {
      return (m_text);
   }

   //! Constructor requires data value
   ValueToAsciiHex(char value) 
   {
      SetValue(value);
   }

   //! Alternate post-construction way to alter value.
   void  SetValue(char  value) 
   {
      m_text[0] = (value & 0x0F) + '0';
      if (m_text[0] > '9')
         m_text[0] += 7;   // 'A' - '0' - 10

      m_text[1] = 0;   // Terminate the string
   }
};

typedef ValueToAsciiHex<OneCharFlag> flagToAsciiHex;  //!< 1 nybble = 1 ascii hex character

/*! \brief Typeless Construction Function 
 *
 *
 * Use this instead of template class when type can change & digits don't matter.
 */ 
template<typename DATA>
inline ValueToAsciiHex<DATA>  ToAsciiHex( DATA value )
{
   return (ValueToAsciiHex<DATA>(value));
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/*! \brief Template class to convert ASCII HEX form
 *
 * Generally, developers should use the typedefs below, instead of directly
 * using this class. \sa asciiHexToChar, asciiHexToShort, asciiHexToLong,
 * asciiHexToFloat, asciiHexToDouble, asciiHexToFlag.
 */
template<typename DATA>
struct AsciiHexToValue
{
private:
   DATA  m_Value;

   union ByteData
   {
      DATA          as_value;
      unsigned char as_bytes[sizeof(DATA)];

      void ByteSwap()  //!< PC is little endian, ARM is big endian
      {
        #ifndef __powerpc__
         volatile unsigned short dsize = sizeof(as_bytes)/2; // volatile is here to avoid warnings
         for (unsigned short i=0; i<dsize; ++i)
         {  
            unsigned short j    = (sizeof(as_bytes) - 1) - i;
            unsigned char  temp = as_bytes[i];
            as_bytes[i]         = as_bytes[j];
            as_bytes[j]         = temp;
         }
        #endif
      }
   };

public:
   //!< Function converts hex text to data type
   DATA Convert( const char *hex )
   {
      ByteData  convert;

      for (unsigned int i=0; i < sizeof(DATA); ++i)
      {
         unsigned char hi_nybble = toupper( *hex ) - '0';
         if (hi_nybble > 9)
            hi_nybble -= 7;

         hex++;

         unsigned char lo_nybble = toupper( *hex ) - '0';
         if (lo_nybble > 9)
            lo_nybble -= 7;

         convert.as_bytes[i] = (hi_nybble << 4) | lo_nybble;
         hex++;
      }

      convert.ByteSwap(); //!< PC is little endian, ARM is big endian
      return (convert.as_value);
   }

   //!< Constructor sets the internal value and fills in value
   AsciiHexToValue( const char *hex, DATA &value)
   {
      m_Value = Convert(hex);
      value   = m_Value;
   }

   //!< Constructor sets the internal value
   AsciiHexToValue( const char *hex )
   {
      m_Value = Convert(hex);
   }

   DATA  getValue() const { return (m_Value); }
};

typedef AsciiHexToValue<unsigned char>  asciiHexToUChar;   //!<  2 ascii hex characters = 1 byte
typedef AsciiHexToValue<char>           asciiHexToChar;    //!<  2 ascii hex characters = 1 byte  
typedef AsciiHexToValue<unsigned short> asciiHexToUShort;  //!<  4 ascii hex characters = 2 bytes
typedef AsciiHexToValue<short>          asciiHexToShort;   //!<  4 ascii hex characters = 2 bytes
typedef AsciiHexToValue<unsigned long>  asciiHexToULong;   //!<  8 ascii hex characters = 4 bytes
typedef AsciiHexToValue<long>           asciiHexToLong;    //!<  8 ascii hex characters = 4 bytes
typedef AsciiHexToValue<float>          asciiHexToFloat;   //!<  8 ascii hex characters = 4 bytes
typedef AsciiHexToValue<double>         asciiHexToDouble;  //!< 16 ascii hex characters = 8 bytes

//! Specialization for 1 character flags
template<>
struct AsciiHexToValue<OneCharFlag>
{
   AsciiHexToValue( const char *hex, char &value) //!< Constructor fills in value
   {
      value = toupper( *hex ) - '0';
      if (value > 9)
         value -= 7;   // 'A' - '0' - 10
   }
};

typedef AsciiHexToValue<OneCharFlag> asciiHexToFlag;  //!< 1 ascii hex character = 1 byte


