/************************************************************************
 ** This file is part of the network simulator ns-2.                   **
 ** Copyright (C) 2006 by AutoNomos (www.auto-nomos.de)                **
 ** This part of ns-2 is free software; you can redistribute it and/or **
 ** modify it under the terms of the GNU General Public License,       **
 ** version 2.                                                         **
 **                                                                    **
 ** \author Axel Wegener <wegener@itm.uni-luebeck.de>                  **
 **                                                                    **
 ** Derived from Shawn (see www.swarmnet.de)                           **
 **                                                                    **
 ************************************************************************/

#include "storage.h"
#include <assert.h>

using namespace std;

#define NULLITER static_cast<list<unsigned char>::iterator>(0)

namespace itm
  {
// ----------------------------------------------------------------------
  Storage::Storage()
      : pos_(0), iter_(NULLITER), iterEnd_(NULLITER)
  {}

// ----------------------------------------------------------------------
  Storage::Storage(unsigned char packet[], int length)
      : pos_(0), iter_(0), iterEnd_(0)
  {
    if (length == -1)
      length = sizeof(packet) / sizeof(char);

    for (int i = 0; i < length; ++i)
      push_back(packet[i]);
    iter_ = begin();
    iterEnd_ = end();
  }

// ----------------------------------------------------------------------
  Storage::~Storage()
{}

// ----------------------------------------------------------------------
  void Storage::advance(int count)
  {
    assert ( count >= 0 );

    if (count != 1)
      iterEnd_ = NULLITER;
    pos_ += count;
    if (iter_ == NULLITER)
      iter_ = begin();
    ::advance(iter_, count);
  }

// ----------------------------------------------------------------------
  bool Storage::valid_pos()
  {
    if (iterEnd_ == end())
      {
        return (iter_ != NULLITER && iter_ != end());
      }
    else
      {
        bool v = (pos_ < size());
        if (v)
          iterEnd_ = end();
        return v;
      }
  }

// ----------------------------------------------------------------------
  void Storage::reset()
  {
    this->clear();
    pos_=0;
    iter_ = NULLITER;
  }
// ----------------------------------------------------------------------
  /**
  * Reads a char form the array
  * @return The read char (between 0 and 255)
  */
  unsigned char Storage::readChar()	throw()
  {
    assert( valid_pos() );
    if (iter_ == NULLITER)
      iter_ = begin();
    unsigned char hb = *iter_;
    advance(1);
    return hb;
  }

// ----------------------------------------------------------------------
  /**
  *
  */
  void Storage::writeChar(unsigned char value) throw()
  {
    push_back(value);
    advance(1);
  }

// ----------------------------------------------------------------------
  /**
  * Reads a byte form the array
  * @return The read byte (between 0 and 255)
  */
  int Storage::readByte()	throw()
  {
    return static_cast<int>(readChar());
  }

// ----------------------------------------------------------------------
  /**
  *
  */
  void Storage::writeByte(int value) throw()
  {
    writeChar( static_cast<unsigned char>(value & 0xFF) );
  }

// ----------------------------------------------------------------------
  /**
  *
  */
  void Storage::writeByte(unsigned char value) throw()
  {
    writeChar( value );
  }

// -----------------------------------------------------------------------
  /**
  * Reads a string form the array
  * @return The read string
  */
  std::string Storage::readString() throw()
  {
    string tmp;
    int len = readInt();
    for (int i = 0; i < len; i++)
      tmp += readChar();
    return tmp;
  }

// ----------------------------------------------------------------------
  /**
  * Writes a string into the array;
  * @param s		The string to be written
  */
  void Storage::writeString(std::string s) throw()
  {
    writeInt(s.length());
    for (string::iterator it = s.begin(); it!=s.end() ; it++)
      writeChar(*it);
  }

// ----------------------------------------------------------------------
  /**
  * Restores an integer, which was split up in two bytes according to the
  * specification, it must have been split by its row byte representation
  * with MSBF-order
  *
  * @return the unspoiled integer value (between -32767 and 32768)
  */
  int Storage::readShort() throw()
  {
    short value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&a);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        p_value[0] = readChar();
        p_value[1] = readChar();
      }
    else
      {
	// network is big endian
        p_value[1] = readChar();
        p_value[0] = readChar();
      }
    return value;
  }

// ----------------------------------------------------------------------
  void Storage::writeShort( int value ) throw()
  {
    short svalue = static_cast<short>(value);
    unsigned char *p_svalue = reinterpret_cast<unsigned char*>(svalue);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(a);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        writeChar(p_svalue[0]);
        writeChar(p_svalue[1]);
      }
    else
      {
	// network is big endian
        writeChar(p_svalue[1]);
        writeChar(p_svalue[0]);
      }
  }

// ----------------------------------------------------------------------

  /*
  * restores an integer, which was split up in four bytes acording to the
  * specification, it must have been split by its row byte representation
  * with MSBF-order
  *
  * @return the unspoiled integer value (between -2.147.483.647 and 2.147.483.648)
  */
  int Storage::readInt() throw()
  {
    int value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&a);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        p_value[0] = readChar();
        p_value[1] = readChar();
        p_value[2] = readChar();
        p_value[3] = readChar();
      }
    else
      {
	// network is big endian
        p_value[3] = readChar();
        p_value[2] = readChar();
        p_value[1] = readChar();
        p_value[0] = readChar();
      }
    return value;
  }

// ----------------------------------------------------------------------
  void Storage::writeInt( int value ) throw()
  {
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&a);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        writeChar(p_value[0]);
        writeChar(p_value[1]);
        writeChar(p_value[2]);
        writeChar(p_value[3]);
      }
    else
      {
	// network is big endian
        writeChar(p_value[3]);
        writeChar(p_value[2]);
        writeChar(p_value[1]);
        writeChar(p_value[0]);
      }
  }

// ----------------------------------------------------------------------

  /*
  * restores a float , which was split up in four bytes acording to the
  * specification, it must have been split by its row byte representation
  * with MSBF-order
  *
  * @return the unspoiled float value
  */
  float Storage::readFloat() throw()
  {
    float value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&a);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        p_value[0] = readChar();
        p_value[1] = readChar();
        p_value[2] = readChar();
        p_value[3] = readChar();
      }
    else
      {
	// network is big endian
        p_value[3] = readChar();
        p_value[2] = readChar();
        p_value[1] = readChar();
        p_value[0] = readChar();
      }

    return value;
  }

// ----------------------------------------------------------------------
  void Storage::writeFloat( float value ) throw()
  {
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&a);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        writeChar(p_value[0]);
        writeChar(p_value[1]);
        writeChar(p_value[2]);
        writeChar(p_value[3]);
      }
    else
      {
	// network is big endian
        writeChar(p_value[3]);
        writeChar(p_value[2]);
        writeChar(p_value[1]);
        writeChar(p_value[0]);
      }
  }

// ----------------------------------------------------------------------
  void Storage::writeDouble( double value ) throw ()
  {
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&a);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        for (int i=0; i<8; ++i)
          {
            writeChar(p_value[i]);
          }
      }
    else
      {
	// network is big endian
        for (int i=7; i>=0; --i)
          {
            writeChar(p_value[i]);
          }
      }
  }

// ----------------------------------------------------------------------
  double Storage::readDouble( ) throw ()
  {
    double value = 0;
    unsigned char *p_value = reinterpret_cast<unsigned char*>(&value);
    short a = 0x0102;
    unsigned char *p_a = reinterpret_cast<unsigned char*>(&value);
    if (p_a[0] == 0x01) // big endian
      {
	// network is big endian
        for (int i=0; i<8; ++i)
          {
            p_value[i] = readChar();
          }
      }
    else
      {
	// network is big endian
        for (int i=7; i>=0; --i)
          {
            p_value[i] = readChar();
          }
      }
    return value;
  }
}
/*-----------------------------------------------------------------------
 * Source  $Source$
 * Version $Revision: 202 $
 * Date    $Date: 2006-09-07 19:11:52 +0200 (Do, 07 Sep 2006) $
 *-----------------------------------------------------------------------
 * $Log$
 *-----------------------------------------------------------------------*/
