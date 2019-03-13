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
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
*/

#include "StdAfx.h"
#include "error.h"

mpg123clr::mpg123error::mpg123error(void)
{
}

// Destructor cleans up all resources
mpg123clr::mpg123error::~mpg123error(void)
{
	// clean up code to release managed resources
	// ...

	// call Finalizer to clean up unmanaged resources
	this->!mpg123error();
}

// Finalizer cleans up unmanaged resources
mpg123clr::mpg123error::!mpg123error(void)
{
	// none!
}

String^ mpg123clr::mpg123error::mpg123_plain_strerror(mpg123clr::mpg::ErrorCode errcode)
{
	return gcnew String(::mpg123_plain_strerror((int)errcode));
}

