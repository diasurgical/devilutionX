/*
	mpg123clr: MPEG Audio Decoder library Common Language Runtime version.

	copyright 2009 by Malcolm Boczek - free software under the terms of the LGPL 2.1
	mpg123clr.dll is a derivative work of libmpg123 - all original mpg123 licensing terms apply.

	All rights to this work freely assigned to the mpg123 project.
*/
/*
	libmpg123: MPEG Audio Decoder library

	copyright 1995-2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

*/
/*
	1.8.1.0	04-Aug-09	Initial release.
	1.9.0.0 01-Oct-09	Technical cleanup - subst nullptr for NULL (mb)
*/

#include "StdAfx.h"
#include "id3v1.h"

mpg123clr::id3::mpg123id3v1::mpg123id3v1(void)
{
}

mpg123clr::id3::mpg123id3v1::mpg123id3v1(mpg123_id3v1* sb)
{
	this->sb = sb;
}


// Destructor
mpg123clr::id3::mpg123id3v1::~mpg123id3v1(void)
{
	// clean up code to release managed resources
	// ...

	// call Finalizer to clean up unmanaged resources
	this->!mpg123id3v1();
}

// Finalizer
mpg123clr::id3::mpg123id3v1::!mpg123id3v1(void)
{
	// we do not create the underlying structure, so shouldn't dispose it either...
}

String^ mpg123clr::id3::mpg123id3v1::tag::get()
{
	if (sb == nullptr || sb->tag == nullptr) return gcnew String("");

	return Marshal::PtrToStringAnsi((IntPtr)sb->tag, (int)strnlen(sb->tag, 3));
}

String^ mpg123clr::id3::mpg123id3v1::title::get()
{
	if (sb == nullptr || sb->title == nullptr) return gcnew String("");

	return Marshal::PtrToStringAnsi((IntPtr)sb->title, (int)strnlen(sb->title, 30));
}

String^ mpg123clr::id3::mpg123id3v1::artist::get()
{
	if (sb == nullptr || sb->artist == nullptr) return gcnew String("");

	return Marshal::PtrToStringAnsi((IntPtr)sb->artist, (int)strnlen(sb->artist, 30));
}

String^ mpg123clr::id3::mpg123id3v1::album::get()
{
	if (sb == nullptr || sb->album == nullptr) return gcnew String("");

	return Marshal::PtrToStringAnsi((IntPtr)sb->album, (int)strnlen(sb->album, 30));
}

String^ mpg123clr::id3::mpg123id3v1::comment::get()
{
	if (sb == nullptr || sb->comment == nullptr) return gcnew String("");

	return Marshal::PtrToStringAnsi((IntPtr)sb->comment, (int)strnlen(sb->comment, 30));
}

int mpg123clr::id3::mpg123id3v1::genre::get()
{
	if (sb == nullptr) return 0;

	return sb->genre;
}


