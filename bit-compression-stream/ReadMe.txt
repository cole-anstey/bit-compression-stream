bit-compression-stream
Copyright (C) 10th December 2013 - Cole Anstey

These classes provide a memory stream buffer and lossless bit packing stream. 
This can useful when transmitting data across a network or persisting to a file.

An application may be using a 32-bit integer; but only actually using 19-bits of it subsequently wasting 13-bits. 
Limiting the amount of bits with this technique can significantly reduce the size of any data. 