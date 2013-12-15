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
#include <windows.h>
#include <assert.h>

namespace std
{
#define EMPTY_BIT_MASK  0x00
#define MAX_BIT_MASK    0x80  // 10000000

class cistream : public std::istream
{
  BYTE _partially_uncompressed_byte;
  BYTE _partial_bit_mask;

public:
  cistream(std::streambuf* sb) : 
    std::istream(sb),
    _partial_bit_mask(EMPTY_BIT_MASK)
  {    
  }

  void read(PCHAR pbyte, std::streamsize size)
  {
    read(pbyte, size, (INT)size * 8/*bits*/);
  }

  /*
    value1
   value22
  value333
  76543210

  |76543210|76543210|76543210| bytes
  |1eulav22|eulav333|eulav|    values
  |54321065|43210765|43210|    compressed bits

  The bits are packed in little endian order.
  */
  void read(PCHAR pbyte, std::streamsize size, UINT bit_count)
  {
    // Bytes in little endian format LONGLONG 0xff04 = 04 ff 00 00 00 00 00 00
    assert(bit_count > 0 && "Unspecified bit count.");
    assert(bit_count <= (size * 8)/*bits*/ && "Bit overflow.");

    // Assign the complete size of the destination byte;
    // as the compressed bits may not span all bytes.
    memset (pbyte, 0x00, (size_t)size);

    BYTE bit_mask = 0x01;

    // Iterate through each bit; from high to low.
    for (UINT bit_no = 0; bit_no < bit_count; bit_no++)
    {
      // Check whether this is the last bit within the partial byte.
      if (_partial_bit_mask == EMPTY_BIT_MASK)
      {
        // May contain bits used during the *next* read.
        std::istream::read((PCHAR)&_partially_uncompressed_byte, sizeof(BYTE));

        _partial_bit_mask = MAX_BIT_MASK;
      }

      (*pbyte) |= (_partially_uncompressed_byte & _partial_bit_mask) ? bit_mask : EMPTY_BIT_MASK;
      
      bit_mask <<= 1;
      _partial_bit_mask >>= 1;

      // Check whether this is the last bit within the current byte.
      if (bit_mask == 0x00) // shifted out.
      {
         pbyte++;
        bit_mask = 0x01;
      }
    }
  }  
};

class costream : public std::ostream
{
  BYTE _partially_compressed_byte;
  BYTE _partial_bit_mask;

public:  
  costream(std::streambuf* sb) :
    std::ostream(sb),
    _partial_bit_mask(MAX_BIT_MASK)
  {   
  }

  void write(PCHAR pbyte, std::streamsize size)
  {
    write(pbyte, size, (INT)size * 8/*bits*/);
  }

  /*
    value1
   value22
  value333
  76543210

  |76543210|76543210|76543210| bytes
  |1eulav22|eulav333|eulav|    values
  |54321065|43210765|43210|    compressed bits

  The bits are packed in little endian order.
  */
  void write(PCHAR pbyte, std::streamsize size, UINT bit_count)
  {
    // Bytes in little endian format LONGLONG 0xff04 = 04 ff 00 00 00 00 00 00
    assert(bit_count > 0 && "Unspecified bit count.");
    assert(bit_count <= (size * 8)/*bits*/ && "Bit overflow.");

    BYTE value = *pbyte;

    // Iterate through each bit; from low to high.
    for (UINT bit_no = 0; bit_no < bit_count; bit_no++)
    {
      BYTE bit = (value & 0x01) ? _partial_bit_mask : EMPTY_BIT_MASK;
      _partially_compressed_byte |= bit;

      _partial_bit_mask >>= 1;

      // Check whether this is the last bit within the partial byte.
      if (_partial_bit_mask == EMPTY_BIT_MASK)
      {
        _partial_bit_mask = MAX_BIT_MASK;

        // Write the next available 
        std::ostream::write((PCHAR)&_partially_compressed_byte, sizeof(BYTE));

        _partially_compressed_byte = 0x00;
      }

      // Check whether this is the last bit within the current byte.
      if ((bit_no & 0x07) == 0x07)
      {
        assert(value <= 1 && "Bits being lost.");

        value = *(++pbyte); // Next byte.
      }
      else
      {
        value >>= 1;  // Next bit-0.
      }
    }
  }

  /*
  Flushes any partially compressed bytes.
  */
  void flush()
  {
    // Check whether there is partial byte remaining.
    if (_partial_bit_mask != EMPTY_BIT_MASK)
    {
      // Write the last partial byte. 
      std::ostream::write((PCHAR)&_partially_compressed_byte, sizeof(BYTE));
    }

    std::ostream::flush();
  }
};
}