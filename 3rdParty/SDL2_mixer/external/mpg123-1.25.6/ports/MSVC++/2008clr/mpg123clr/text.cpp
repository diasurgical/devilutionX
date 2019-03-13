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
*/

#include "StdAfx.h"
#include "text.h"

mpg123clr::mpg123text::mpg123text(void)
{
}

mpg123clr::mpg123text::mpg123text(mpg123_text* sb)
{
	this->sb = sb;
}

// Destructor
mpg123clr::mpg123text::~mpg123text(void)
{
	// clean up code to release managed resources
	// ...

	// call Finalizer to clean up unmanaged resources
	this->!mpg123text();
}

// Finalizer
mpg123clr::mpg123text::!mpg123text(void)
{
}

String^ mpg123clr::mpg123text::lang::get()
{
	return Marshal::PtrToStringAnsi((IntPtr)sb->lang, (int)strnlen(sb->lang, 3));
}

String^ mpg123clr::mpg123text::id::get()
{
	return Marshal::PtrToStringAnsi((IntPtr)sb->id, (int)strnlen(sb->id, 4));
}

String^ mpg123clr::mpg123text::description::get()
{
	mpg123str^ str = gcnew mpg123str(&sb->description);
	return str->Text;
}

String^ mpg123clr::mpg123text::text::get()
{
	mpg123str^ str = gcnew mpg123str(&sb->text);
	return str->Text;
}
