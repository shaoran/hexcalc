# hexcalc
A handy hex calculator and register contents visualiser for assembly programmers

## Learning by example

This program is a useful tool for bit-level programmers that often need to know
quickly how to interpret certain bit fields that are part of a long hexadecimal
number.

For example, let's assume you're testing the code of a new CPU.  You have
written an assembly program for your CPU and are now simulating how it runs.
Suddenly, the simulation stops with an error message.  In order to find out
what has gone wrong, you look up what value was stored in a special-purpose
register called `cause` the moment the simulation stopped.  The value is
`0xdeadbeef`.  What does that mean?

The specs say that bits `8..3` store the exception code, a numeric ID that
tells you what kind of error occurred.  For example, the specs say, the value
`7` stands for the execution of a reserved instruction, the value `12` means an
arithmetic overflow, `29` means loading an illegal address has been attempted,
etc.  Well, how do you quickly know how a part of `eef` translates into `12`,
`7` or `29`?  With practice, you've become very good at computing that kind of
things in your head, but sometimes you wish you had a tool so you can be
completely sure in less time.

Luckily, you discover nifty useful `hexcalc`.  You start it and enter
`deadbeef` when the prompt appears.  The program helps you visualise the
contents of a 32-bit register with value `0xdeadbeef`.

```
hex-calc> deadbeef
decimal: 3735928559
    hex:     d    e    a    d    b    e    e    f
    bin:  1101 1110 1010 1101 1011 1110 1110 1111
         +----+----+----+----+----+----+----+----+
indices:  3  2 2  2 2  2 1  1 1  1 1  0 0  0 0  0
          1  8 7  4 3  0 9  6 5  2 1  8 7  4 3  0
```

The third line (labelled `bin`) shows each of the 32 bits of the register when
the register stores the value `0xdeadbeef`.  The bits are grouped in fours,
each group of four corresponding to a hexadecimal digit.  The second line
(labelled `hex`) shows which hexadecimal digit represents the four bits below
it.  The first line (labelled `dec`) shows the decimal equivalent to
`0xdeadbeef` and to `11011110101011011011111011101111`.

The two lines below the dashes represent the indices of the bits above.  Due to
space limitations, 2-digit indices are written from top to bottom.  Thus,

```
3
1
```

stands for index `31`.  Indices can be turned off or on with command `I`:

```
hex-calc> I
decimal: 3735928559
    hex:     d    e    a    d    b    e    e    f
    bin:  1101 1110 1010 1101 1011 1110 1110 1111
hex-calc> I
decimal: 3735928559
    hex:     d    e    a    d    b    e    e    f
    bin:  1101 1110 1010 1101 1011 1110 1110 1111
         +----+----+----+----+----+----+----+----+
indices:  3  2 2  2 2  2 1  1 1  1 1  0 0  0 0  0
          1  8 7  4 3  0 9  6 5  2 1  8 7  4 3  0
```

Now you can highlight indices `8..3` with command `l`:

```
hex-calc> l 8 3
decimal: 3735928559
    hex:     d    e    a    d    b    e    e    f
    bin:  1101 1110 1010 1101 1011 1110 1110 1111
         +----+----+----+----+----+----+----+----+
indices:  3  2 2  2 2  2 1  1 1  1 1  0 0  0 0  0
          1  8 7  4 3  0 9  6 5  2 1  8 7  4 3  0
highlighted bin: 01 1101
highlighted hex:  1    d
highlighted dec: 29
```

That shows the contents of the register again, but bits `8..3` are shown in a
different colour.  Additionally, three new lines are printed; these show the
binary, hexadecimal and decimal representation of the contents of bits `8..3`.

Now, assume you are interested in other fields of this register as well, but
you don't want to highlight each field separately.  Luckily, you have a file
containing the specification of all registers in your CPU. (Use command `h R`
to get info on how to write such a file.)  So you load that file with command
`R` and use command `s` to show the contents of all bit fields defined for
register `cause`:

```
hex-calc> R specs
available register definitions:
    cause
    cpuconfig
    units
    version
hex-calc> s cause
         cause            bin      hex   dec
--------------------------------------------
        field3 [20..16] = 01101    d     13
        field2 [15..14] = 10       2     2
        field1 [13..11] = 111      7     7
exception_code [ 8.. 3] = 011101   1d    29
```

You can also flip a few bits here and there to see how the value of the whole
register and of the different fields changes:

```
hex-calc> i 14
decimal: 3735944943
    hex:     d    e    a    d    f    e    e    f
    bin:  1101 1110 1010 1101 1111 1110 1110 1111
         +----+----+----+----+----+----+----+----+
indices:  3  2 2  2 2  2 1  1 1  1 1  0 0  0 0  0
          1  8 7  4 3  0 9  6 5  2 1  8 7  4 3  0
hex-calc> i 8 3
decimal: 3735944983
    hex:     d    e    a    d    f    f    1    7
    bin:  1101 1110 1010 1101 1111 1111 0001 0111
         +----+----+----+----+----+----+----+----+
indices:  3  2 2  2 2  2 1  1 1  1 1  0 0  0 0  0
          1  8 7  4 3  0 9  6 5  2 1  8 7  4 3  0
hex-calc> S
         cause            bin      hex   dec
--------------------------------------------
        field3 [20..16] = 01101    d     13
        field2 [15..14] = 11       3     3
        field1 [13..11] = 111      7     7
exception_code [ 8.. 3] = 100010   22    34
```

Finally, you can use command `h` to get a list of available commands, and `h
COMMAND` to get detailed info on a particular command.

## Installing

Use the provided `Makefile` to compile this project.

```shell
cd /root/of/hexcalc/project/bin/
make
```

That will create the executable file `hexcalc` in the `bin` directory.  That
directory also contains a file called `specs` which contains the definition of
four registers of a fictive CPU.  This file is provided as an example for how
to use command `s`.

Alternatively, feel free to write your own `Makefile` or to use your favourite
IDE to compile the application.
