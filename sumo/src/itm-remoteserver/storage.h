/************************************************************************
 ** This file is part of the network simulator ns-2.                   **
 ** Copyright (C) 2006 by AutoNomos (www.auto-nomos.de)                **
 ** This part of ns-2 is free software; you can redistribute it and/or **
 ** modify it under the terms of the GNU General Public License,       **
 ** version 2.                                                         **
 **                                                                    **
 ** \author Axel Wegener <wegener@itm.uni-luebeck.de>                  **
 **                                                                    **
 ************************************************************************/
#ifndef storage_h
#define storage_h

#include <list>
#include <string>

namespace itm
  {
  class Storage: public std::list<unsigned char>
    {
    private:
      unsigned int pos_;
      std::list<unsigned char>::iterator iter_;
      std::list<unsigned char>::iterator iterEnd_;


    public:
      Storage();
      Storage(unsigned char[], int length=-1);
      virtual ~Storage();

      void advance(int);
      bool valid_pos();

      void reset();

      virtual unsigned char readChar() throw();
      virtual void writeChar(unsigned char) throw();

      int readByte() throw();
      virtual void writeByte(int) throw();
      virtual void writeByte(unsigned char) throw();

      virtual std::string readString() throw();
      virtual void writeString(std::string s) throw();

      virtual int readShort() throw();
      virtual void writeShort(int) throw();

      virtual int readInt() throw();
      virtual void writeInt(int) throw();

      virtual float readFloat() throw();
      virtual void writeFloat( float ) throw();

      virtual double readDouble() throw();
      virtual void writeDouble( double ) throw();
    };
}
#endif
/*-----------------------------------------------------------------------
 * Source  $Source$
 * Version $Revision: 202 $
 * Date    $Date: 2006-09-07 19:11:52 +0200 (Do, 07 Sep 2006) $
 *-----------------------------------------------------------------------
 * $Log$
 *-----------------------------------------------------------------------*/
