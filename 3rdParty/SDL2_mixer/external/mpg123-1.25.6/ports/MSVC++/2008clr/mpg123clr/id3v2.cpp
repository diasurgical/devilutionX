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
	1.10.0.0 30-Nov-09	Add test for null base (sb) - in addition to null string test (mb)
						(required if v2 referenced prior to file scan)
*/

#include "StdAfx.h"
#include "id3v2.h"

mpg123clr::id3::mpg123id3v2::mpg123id3v2(void)
{
}

mpg123clr::id3::mpg123id3v2::mpg123id3v2(mpg123_id3v2* sb)
{
	this->sb = sb;
}


// Destructor
mpg123clr::id3::mpg123id3v2::~mpg123id3v2(void)
{
	// clean up code to release managed resources
	// ...

	// call Finalizer to clean up unmanaged resources
	this->!mpg123id3v2();
}

// Finalizer
mpg123clr::id3::mpg123id3v2::!mpg123id3v2(void)
{
	// we do not create the underlying structure, so shouldn't dispose it either...
}

String^ mpg123clr::id3::mpg123id3v2::title::get()
{
	// updated: more efficient "union style" mpg123str
	mpg123str^ str = (sb == nullptr || sb->title == nullptr) ? gcnew mpg123str("") : gcnew mpg123str(sb->title);

	// Deprecated remnant - uses "overlayed style" mpg123str - note inefficient PtrToStrucure...
	//	mpg123str^% str = gcnew mpg123str;
	//	Marshal::PtrToStructure((IntPtr)sb->title, str);

	return str->Text;
}

String^ mpg123clr::id3::mpg123id3v2::artist::get()
{
	mpg123str^ str = (sb == nullptr || sb->artist == nullptr) ? gcnew mpg123str("") : gcnew mpg123str(sb->artist);

	return str->Text;
}

String^ mpg123clr::id3::mpg123id3v2::album::get()
{
	mpg123str^ str = (sb == nullptr || sb->album == nullptr) ? gcnew mpg123str("") : gcnew mpg123str(sb->album);

	return str->Text;
}

String^ mpg123clr::id3::mpg123id3v2::comment::get()
{
	mpg123str^ str = (sb == nullptr || sb->comment == nullptr) ? gcnew mpg123str("") : gcnew mpg123str(sb->comment);

	return str->Text;
}

String^ mpg123clr::id3::mpg123id3v2::year::get()
{
	mpg123str^ str = (sb == nullptr || sb->year == nullptr) ? gcnew mpg123str("") : gcnew mpg123str(sb->year);

	return str->Text;
}

String^ mpg123clr::id3::mpg123id3v2::genre::get()
{
	mpg123str^ str = (sb == nullptr || sb->genre == nullptr) ? gcnew mpg123str("") : gcnew mpg123str(sb->genre);

	return str->Text;
}

int mpg123clr::id3::mpg123id3v2::version::get()
{
	return sb->version;
}

array<mpg123clr::mpg123text^>^ mpg123clr::id3::mpg123id3v2::MakeTextList(mpg123_text* ptr, size_t count)
{
	// WARN 4267 - clr limited to 32bit-length-size arrays!!
	array<mpg123clr::mpg123text^>^ ary = gcnew array<mpg123clr::mpg123text^>((int)count);

	for (int ii = 0; ii < ary->Length; ii++)
	{
		// Remnant - for explicit layout
		//		mpg123text^% txt = gcnew mpg123text;
		//		Marshal::PtrToStructure((IntPtr)ptr++, txt);

		// New code - for "handle style" structure 
		mpg123text^% txt = gcnew mpg123text(ptr++);

		ary[ii] = txt;
	}

	return ary;
}

array<mpg123clr::mpg123text^>^ mpg123clr::id3::mpg123id3v2::Comments::get()
{
	return MakeTextList(sb->comment_list, sb->comments);
}

array<mpg123clr::mpg123text^>^ mpg123clr::id3::mpg123id3v2::Texts::get()
{
	return MakeTextList(sb->text, sb->texts);
}

array<mpg123clr::mpg123text^>^ mpg123clr::id3::mpg123id3v2::Extras::get()
{
	return MakeTextList(sb->extra, sb->extras);
}

