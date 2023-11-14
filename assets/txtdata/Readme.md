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
  All files contain at least one record. The last record in a file SHOULD end
    with a trailing newline, however some spreadsheet applications do not
    output trailing newlines so we allow flexibility here. This means that an
    empty file (zero-length or containing only a UTF8 BOM sequence) is valid
    and is typically interpreted as a header for a single unnamed column with
    no records.
  Records SHOULD contain the same number of fields.
*/
DXTxtFile     ::= utf8bom? ( singleRecord | record+ finalRecord? )

utf8bom       ::= #xEF #xBB #xBF

/*
  if parsing reaches EOF and the file has no record terminators then the file
  MUST be treated as containing a single record with no terminator
*/
singleRecord  ::= fields

/*
  if for some reason you want to end a file with a record containing a single
  empty field then the record MUST end with a valid terminator
*/
record        ::= fields recordterm

/*
  this means that the terminator is only truly optional for files where the
  final record contains a single field with at least one character, or at least
  two fields (as there will then be at least one field separator even if both
  fields are zero-length)
*/
finalRecord   ::= nonEmptyField | field fieldsep fields

/* an empty line is treated as a single empty field */
fields        ::= field ( fieldsep field )*

/* fields MAY be zero length */
field         ::= character*

/*
  unless the final record only consists of a single field, in which case it
  MUST contain at least one character
*/
nonEmptyField ::= character+

/*
  Any Char (see https://www.w3.org/TR/xml/#NT-Char) except Tab ("\t", 0x09),
  Line Feed ("\n", 0x0A), or Carriage Return ("\r", 0x0D) is allowed as a
  field value. For maximum portability characters in the discouraged ranges
  described in the linked section SHOULD NOT be used
*/
character     ::= [^#x9#xA#xD]

/* fields MUST be separated by tabs */
fieldsep      ::= #x9

/*
  records (other than the final record or the only record in a single-record
  file) MUST be terminated by line feeds, a cr/lf pair MAY be used
*/
recordterm    ::= #xD? #xA
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

### Experience.tsv
Experience contains the experience value thresholds before a character
advances to the next level. All numeric values in this file MUST be written in
base 10 with no decimal or thousands separators. The first row of this file is
used as a header and requires the following column names:

#### Level
A numeric value used to set the order for experience thresholds. The header
line MUST be the first line in the file. Levels SHOULD proceed in ascending
order after that starting from level 1. Levels up to 255 are supported, the
highest value will be used as the maximum character level. If you leave any
gaps then characters will not be able to advance past that level and experience
caps will not apply.

If you're familiar with Diablo 2 text files you might expect to use a MaxLevel
row to set character level limits and to have a level 0 line, we ignore these
lines in DevilutionX. As all characters start at level 1 we use the threshold
for level 1 to determine when characters advance past level 1. The highest
level defined in the file is the maximum level.

#### Experience
This column determines the experience points required for characters to
advance past that level. For example a file like:
```tsv
Level	Experience
1	2000
2	4000
3	6000
4	8000
5	10000
```
Could be used to have characters level up to a max of 5 every 2000 experience
points. They would start at level 1, level up to 2 at 2000 exp, level 3 at
4000 exp, level 4 at 6000 exp, then reach the maxium level of 5 at 8000 exp.
Characters would continue gaining experience until they hit 10000 experience
points and will not level up any further.

You should provide a value for every row up to (and including) the maximum
level you intend players to be able to reach. If you have an empty cell for an
experience value at a given level then characters will not be able to advance
past that level. They will continue to gain experience without a cap (up to the
hard limit of `2^32-1`, 4,294,967,295).

[d2-excel-plus]: https://github.com/Cjreek/D2ExcelPlus
[d2mods-info]: https://www.d2mods.info/forum/viewtopic.php?t=34455
[d2rmodding-utilities]: https://www.d2rmodding.com/utilities
[iana-tsv]: https://www.iana.org/assignments/media-types/text/tab-separated-values
[libreoffice]: https://www.libreoffice.org
[w3-xml-ebnf]: https://www.w3.org/TR/xml/#sec-notation
