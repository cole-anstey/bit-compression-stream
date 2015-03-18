The following is the output from a simply written test program.
32 values with incremental bit sizes are added to the memory stream following by two string literals.
These values are then reread back to check that the values match.

Without packing 179 bytes would have been used; but with the lossless packing only 118 bytes.  A saving of 61 bytes !

```
Write to Memory Stream
----------------------
 1bits          1
 2bits          3
 3bits          7
 4bits         15
 5bits         31
 6bits         63
 7bits        127
 8bits        255
 9bits        511
10bits       1023
11bits       2047
12bits       4095
13bits       8191
14bits      16383
15bits      32767
16bits      65535
17bits     131071
18bits     262143
19bits     524287
20bits    1048575
21bits    2097151
22bits    4194303
23bits    8388607
24bits   16777215
25bits   33554431
26bits   67108863
27bits  134217727
28bits  268435455
29bits  536870911
30bits 1073741823
31bits 2147483647
32bits         -1
Hello COLE
Eating, Sleeping, Coding and Repeating.

Read from Memory Stream
-----------------------
 1bits          1
 2bits          3
 3bits          7
 4bits         15
 5bits         31
 6bits         63
 7bits        127
 8bits        255
 9bits        511
10bits       1023
11bits       2047
12bits       4095
13bits       8191
14bits      16383
15bits      32767
16bits      65535
17bits     131071
18bits     262143
19bits     524287
20bits    1048575
21bits    2097151
22bits    4194303
23bits    8388607
24bits   16777215
25bits   33554431
26bits   67108863
27bits  134217727
28bits  268435455
29bits  536870911
30bits 1073741823
31bits 2147483647
32bits         -1
Hello COLE
Eating, Sleeping, Coding and Repeating.

Uncompressed bytes: 179 Compressed bytes: 118  Byte difference: 61
```