Data files are based on the format used by Diablo 2. This is a format very
similar to [IANA TSV][iana-tsv] but with fewer restrictions. Existing tools
such as AFJ Sheet, Diablo 2 Excel File Editor, [D2ExcelPlus][d2-excel-plus],
or spreadsheet programs such as Excel and [LibreOffice][libreoffice] Calc can
be used to read/modify these files. If using a program like Excel or
LibreOffice you will need to check that the output matches the expected format
(tabs as delimiters, field values saved with leading spaces and quote
characters preserved, tabs and newlines in fields stripped or transformed to
spaces).

## Format Specification
For modders, the important thing to keep in mind is that values cannot contain
tab characters or line breaks. If you make sure the file looks roughly the
same after changing your values you shouldn't get any errors.

A formal description of the format using
[W3C's EBNF for XML notation][w3-xml-ebnf]:
```
/*
  Files MAY start with a UTF8 BOM (mainly to play nicer with Excel).
  The first record in a file SHOULD be used as a header
    Implementations are free to treat records however they want, using
    multiple records as headers or none at all.
  Files SHOULD contain at least one record.
    An empty file (zero-length or containing only a UTF8 BOM sequence) is
    valid and is typically interpreted as a header for a single unnamed column
    with no records.
  Records SHOULD contain the same number of fields.
  Files SHOULD end with a trailing newline.
    Note that while a trailing newline is treated as if the file has a final
    record containing a single empty field, in practice these records SHOULD
    be discarded. This is mainly to handle the typical output from spreadsheet
    applications like Excel.
*/
DXTxtFile ::= utf8bom? header ( recordsep, record )*

utf8bom   ::= #xEF #xBB #xBF

header    ::= record

/* an empty line is treated as a single empty field */
record    ::= field ( fieldsep field )*

/* fields MAY be zero length */
field     ::= character*

/*
  Any Char (see https://www.w3.org/TR/xml/#NT-Char) except Tab ("\t", 0x09),
  Line Feed ("\n", 0x0A), or Carriage Return ("\r", 0x0D) is allowed as a
  field value. For maximum portability characters in the discouraged ranges
  described in the linked section SHOULD NOT be used
*/
character ::= [^#x9#xA#xD]

/* fields MUST be separated by tabs */
fieldsep  ::= #x9

/* records MUST be separated by line feeds, a cr/lf pair MAY be used */
recordsep ::= #xD? #xA
```

## File Descriptions
The following documentation describes how these files are used in DevilutionX.
Diablo and Hellfire do not use external text files, you cannot use these files
to change the behaviour in the original games or the GoG versions. Diablo 2
uses a similar but distinct format, [ThePhrozenKeep][d2mods-info] provide a
good reference for modding that game. Diablo 2 Resurrected uses a different
format again, refer to the help files provided alongside the game data
(`Data/Global/Excel`) (also available online at
[D2:R Modding][d2rmodding-utilities]).

### Experience.txt
Experience contains the experience value thresholds before a character
advances to the next level. All numeric values in this file MUST be written in
base 10 with no decimal or thousands separators. The first row of this file is
used as a header and requires the following column names:

#### Level
A numeric value used to set the order for remaining values, or the special
`MaxLevel` value which is used to determine the maximum level for each class.
The header line MUST be the first line in the file. The `MaxLevel` line MUST
be present, it SHOULD be the second line but can appear later in the file.
Levels SHOULD proceed in ascending order after that. If you leave any gaps
then characters will not be able to advance past that level and experience
caps will not apply.

For example you could set a maximum level of 36 for the Rogue and Sorcerer
while leaving the other classes able to level to 50 with the following values:
```tsv
Level	Warrior	Rogue	Sorcerer	Monk	Bard	Barbarian	ExpRatio
MaxLevel	50	36	36	50	50	50	1024
0	0	0	0	0	0	0	1024
1	2000	2000	2000	2000	2000	2000	1024
...
```

The first row following the `MaxLevel` line SHOULD be `0	0	0	0	0	0	0	1024` (as
all characters start at level 1, we ignore these values and use the threshold
for level 1 to determine when characters of a given class advance past level
1). There should also be at least as many rows as the highest value provided
for `MaxLevel`. If you specify a `MaxLevel` of `60` for a class but only
provide experience values up to `50` then that class will level up to `51` but
no further.

#### Warrior/Rogue/Sorcerer/Monk/Bard/Barbarian
These columns determine the experience points required for each class to
advance past that level. For example a file like:
```tsv
Level	Warrior	Rogue	Sorcerer	Monk	Bard	Barbarian	ExpRatio
MaxLevel	5	3	1	1	1	1	1024
0	0	0	0	0	0	0	1024
1	2000	5000	0	0	0	0	1024
2	4000	8000					1024
3	6000	10000					1024
4	8000						1024
5	10000						1024
```
Could be used to have Warriors level up to 2 at 2000 exp, reach level 3 at
4000 exp, level 4 at 6000 exp, then reach their max level of 5 at 8000 exp.
Rogues would level slower, reaching level 2 at 5000 exp and maxing out at
level 3 at 8000 exp. Both classes would stop gaining experience once they
hit 10000 experience points. Other classes will not level up (as their max
level is 1) and would not gain experience (as their experience cap is 0 at
level 1).

Empty values/cells are allowed, however make sure that you provide values for
every row up to (and including) the maximum level for that class. If you have
an empty cell for a column (class) at a row earlier than the `MaxLevel` value
characters of that class will not be able to advance past that level. They
will continue to gain experience without a cap (up to the hard limit of
`2^32-1`, 4,294,967,295). You probably still want to set an experience cap for
every level and class combination even if you limit some classes to lower
maximum levels to avoid accidently triggering this behaviour.

#### ExpRatio
This value is not currently used. It's present in the file to match the format
used by Diablo 2. It'll likely be interpreted as a fixed point value (e.g.
`1.025`) if it does become useful.

[d2-excel-plus]: https://github.com/Cjreek/D2ExcelPlus
[d2mods-info]: https://www.d2mods.info/forum/viewtopic.php?t=34455
[d2rmodding-utilities]: https://www.d2rmodding.com/utilities
[iana-tsv]: https://www.iana.org/assignments/media-types/text/tab-separated-values
[libreoffice]: https://www.libreoffice.org
[w3-xml-ebnf]: https://www.w3.org/TR/xml/#sec-notation
