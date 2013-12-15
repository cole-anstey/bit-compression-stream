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
#include "stdafx.h"

#include <windows.h>
#include <assert.h>

#include "cstreams.h"
#include "mstreambuf.h"

#define MEM_BUFFER_SIZE 5   // Will force multiple memory buffer resizes.
#define USED_BITS       32

int _tmain(int argc, _TCHAR* argv[])
{
#define COLE_LENGTH     11
#define EATING_LENGTH   40

  char* buffer = new char[MEM_BUFFER_SIZE];

#pragma region Write to Memory Stream
  printf("Write to Memory Stream\n");
  printf("----------------------\n");

  std::mstreambuf mbuf((PCHAR*)&buffer, MEM_BUFFER_SIZE);

  std::costream msw(&mbuf);

  INT bit_mask = 0x01;
  INT uncompressed_length = 0;

  for(int bit_no = 1; bit_no <= USED_BITS; bit_no++)
  {
    printf("%2dbits %10d\n", bit_no, bit_mask);

    msw.write((PCHAR)&bit_mask, sizeof(INT), bit_no);

    bit_mask |= (bit_mask << 0x01);
    uncompressed_length += sizeof(INT);
  }

  // Add an uncompressed string to the memory buffer.
  printf("Hello COLE\n");
  msw.write((PCHAR)&"Hello COLE", COLE_LENGTH);
  uncompressed_length += COLE_LENGTH;

  printf("Eating, Sleeping, Coding and Repeating.\n");
  msw.write((PCHAR)&"Eating, Sleeping, Coding and Repeating.", EATING_LENGTH);
  uncompressed_length += EATING_LENGTH;

  msw.flush();

  //msw->close(); // No need to call close on the mstreambuf class.
#pragma endregion

#pragma region Read from Memory Stream
  printf("\nRead from Memory Stream\n");
  printf("-----------------------\n");

  // Get the length of the memory buffer.
  INT compressed_length = (INT)msw.tellp();
  assert(compressed_length != -1 && "This may assert if MEM_BUFFER_SIZE overflows.");
  assert(compressed_length > 0 && "The memory is empty.");

  std::cistream msr(&mbuf);

  for(int bit_no = 1; bit_no <= USED_BITS; bit_no++)
  {
    INT value;

    msr.read((PCHAR)&value, sizeof(INT), bit_no);

    printf("%2dbits %10d\n", bit_no, value);
  }

  // Single values only.
  char szHello_COLE[COLE_LENGTH];
  msr.read((PCHAR)&szHello_COLE, COLE_LENGTH);
  printf("%s\n", szHello_COLE);

  char szEating[EATING_LENGTH];
  msr.read((PCHAR)&szEating, EATING_LENGTH);
  printf("%s\n", szEating);

  printf(
    "\nUncompressed bytes: %d Compressed bytes: %d  Byte difference: %d\n", 
    uncompressed_length, 
    compressed_length, 
    uncompressed_length - compressed_length);

  //msr->close(); // No need to call close on the mstreambuf class.
#pragma endregion
  
  delete[] buffer;

  return 0;
}