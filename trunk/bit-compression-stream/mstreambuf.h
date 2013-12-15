/*
* bit-compression-stream
* Copyright (C) 10th December 2013 - Cole Anstey
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* See http://code.google.com/p/bit-compression-stream/
*/
#pragma once

#include <iosfwd>
#include <istream>
#include <ostream>
#include <fstream>
#include <iostream>

namespace std
{
/*
There are three pointers used by both input and output streams.

For an output stream buffer, there are:
  * the 'put base pointer', as returned from std::streambuf::pbase(), 
    which points to the first element of the buffer's internal array.
  * the 'put pointer', as returned from std::streambuf::pptr(), which points 
    to the next character in the buffer that may be written to.
  * the 'end put pointer' as returned from std::streambuf::epptr(), which 
    points to one-past-the-last-element of the buffer's internal array.

  The buffer is written to via xsputn().
  [ ][ ][ ][ ][ ][ ][ ][*]
   ^        ^           ^
   |        |           streambuf::epptr() - end put pointer
   |        streambuf::pptr() - put pointer
   streambuf::pbase() - put base pointer

  Typically, pbase() and epptr() won't change at all; it will only be pptr() that changes as the buffer is used.

For an input stream buffer, there are 3 different pointers, which have a roughly the same purpose.
  * the 'end back pointer', as returned from std::streambuf::eback(), which points to 
    the last character (lowest in address) in the buffer's internal array in to which a 
    character may be put back.
  * the 'get pointer', as returned from std::streambuf::gptr(), which points to the character in 
    the buffer that will be extracted next by the istream.
  * the 'end get pointer', as returned from std::streambuf::egptr(), which points to 
    one-past-the-last-element of the buffer's internal array.

  The buffer is read from via xsgetn().
  [ ][ ][ ][ ][ ][ ][ ][*]
   ^        ^           ^
   |        |           streambuf::egptr() - end get pointer
   |        streambuf::gptr() - get pointer
   streambuf::eback() - end get pointer

  Again, it's typically the case that eback() and egptr() won't change during the life-time of the streambuf.

see: http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
*/
#define RESIZE_BYTES  8

struct mstreambuf : std::streambuf
{
  char** _pbegin;
  std::streamsize _count;

  mstreambuf(char** pbegin, std::streamsize count) :
    _pbegin(pbegin),
    _count(count)
  {
    char* begin = *pbegin;
    char* end = begin + count;

    // Sets the values that define the boundaries of the accessible part of the controlled sequence.
    // First, Next, Last
    this->setg(begin, begin, end);  // Input.
    this->setp(begin, begin, end);  // Output.
  }

  virtual std::streamsize xsputn(const char *_Ptr, std::streamsize _Count)
  {
    // Check whether any space is available.
    if (_Pnavail() == 0)
    {
      // Resize the used buffer.
      int old_count = (int)_count;

      // Create the new buffer and duplicate the existing data.
      _count += RESIZE_BYTES;
      char* new_begin = new char[(int)_count];

      memcpy(new_begin, *_pbegin, old_count);

      // Update First, Next, Last.
      int nextg = streambuf::gptr() - streambuf::eback();
      this->setg(new_begin, new_begin + nextg, new_begin + _count);  // Input.

      int nextp = streambuf::pptr() - streambuf::pbase();
      this->setp(new_begin, new_begin + nextp, new_begin + _count);  // Output.

      // Update the external buffer pointer.
      delete[] *_pbegin; 
      *_pbegin = new_begin;
    }

    return std::streambuf::xsputn(_Ptr, _Count);
  }

  /*
  Required by tellp() and tellg(); otherwise -1 is returned.
  */
  virtual pos_type seekoff(off_type /*off*/, std::ios_base::seekdir way, std::ios_base::openmode /*which*/ = std::ios_base::in | std::ios_base::out)
  {
    char* estart = pbase();
    char* eend = NULL;    

    /*
    Seems to be the same regardless of whether 'which' equals std::ios_base::in(tellp()) or std::ios_base::out(tellg()).
    char* gstart = eback();
    char* gend = gptr();
    int glen = gend - gstart;
    */
    if (way == std::ios_base::beg)
    {
      eend = pbase();
    }
    else if (way == std::ios_base::end)
    {
      eend = epptr();
    }
    else
    {
      // current.
      eend = pptr();
    }

    pos_type pos = eend - estart;

    return pos;
  }

  /*
  Not required; though some forums state this requires implementing.
  virtual pos_type seekpos(pos_type, std::ios_base::openmode = std::ios_base::in | std::ios_base::out)
  {            
    // change to specified position, according to mode
    return (std::streampos(std::_BADOFF));
  }
  */
};
}