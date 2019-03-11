#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
# needs mutagen
# grabbed from: http://code.activestate.com/recipes/577138-embed-lyrics-into-mp3-files-using-mutagen-uslt-tag/
# simplified to only work on one file and get lyrics from stdin
# I suspect this is public domain code. Just a usage example of the mutagen lib.

import os
import sys
import codecs
from mutagen.mp3 import MP3
from mutagen.id3 import ID3NoHeaderError
from mutagen.id3 import ID3, USLT

TEXT_ENCODING = 'utf8'
TEXT_LANG = 'XXX'
TEXT_DESC = ''

# get workdir from first arg or use current dir 
if (len(sys.argv) > 1):
	fname = sys.argv[1]
	print "fname=" + fname
else:
	print 'Give me at least a file name to work on, plus the lyrics from stdin'
	print 'Optionally, you can provide the language (3 lowercase letters) of the lyrics and a description'
	sys.exit()

if (len(sys.argv) > 2):
	TEXT_LANG = sys.argv[2]

if (len(sys.argv) > 3):
	TEXT_DESC = sys.argv[3]

print "reading lyrics from standard input ..."

lyrics = sys.stdin.read().strip()

# try to find the right encoding
for enc in ('utf8','iso-8859-1','iso-8859-15','cp1252','cp1251','latin1'):
	try:
		lyrics = lyrics.decode(enc)
		TEXT_DESC = TEXT_DESC.decode(enc)
		print enc,
		break
	except:
		pass

print "Adding lyrics to " + fname
print "Language: " + TEXT_LANG
print "Description: " + TEXT_DESC

# create ID3 tag if not exists
try: 
	tags = ID3(fname)
except ID3NoHeaderError:
	print "Adding ID3 header;",
	tags = ID3()

# remove old unsychronized lyrics
if len(tags.getall(u"USLT::'"+TEXT_LANG+"'")) != 0:
	print "Removing Lyrics."
	tags.delall(u"USLT::'"+TEXT_LANG+"'")
	#tags.save(fname) # hm, why?

#tags.add(USLT(encoding=3, lang=u'eng', desc=u'desc', text=lyrics))
# apparently the description is important when more than one 
# USLT frames are present
#tags[u"USLT::'eng'"] = (USLT(encoding=3, lang=u'eng', desc=u'desc', text=lyrics))
tags[u"USLT::'"+TEXT_LANG+"'"] = (USLT(encoding=3, lang=TEXT_LANG, desc=TEXT_DESC, text=lyrics))
print 'Added USLT frame to', fname

tags.save(fname)

print 'Done'

