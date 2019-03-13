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
	1.9.0.0 13-Oct-09	pin_ptr = nullptr on return (mb)
	1.9.0.1	24-Nov-09	performance update - removed try/finally (mb)
*/

#include "StdAfx.h"
#include "advanced.h"

mpg123clr::advpars::advpars(int% error)
{
	pin_ptr<int> err = &error;
	mp = ::mpg123_new_pars(err);

	err = nullptr;
}

mpg123clr::advpars::~advpars(void)
{
	// clean up code to release managed resources
	// ...

	// call Finalizer to clean up unmanaged resources
	this->!advpars();
}

mpg123clr::advpars::!advpars(void)
{
	if (mp != NULL) 
	{
		::mpg123_delete_pars(mp);
		mp = NULL;
	}
}

mpg123clr::mpg::ErrorCode mpg123clr::advpars::mpg123_fmt_none(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_fmt_none(mp);
}

mpg123clr::mpg::ErrorCode mpg123clr::advpars::mpg123_fmt_all(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_fmt_all(mp);
}

mpg123clr::mpg::ErrorCode mpg123clr::advpars::mpg123_fmt(int rate, mpg123clr::mpg::channelcount channels, int encodings)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_fmt(mp, rate, (int)channels, encodings);
}

mpg123clr::mpg::channelcount mpg123clr::advpars::mpg123_fmt_support(int rate, int encodings)
{
	return (mpg123clr::mpg::channelcount) ::mpg123_fmt_support(mp, rate, encodings);
}

mpg123clr::mpg::ErrorCode mpg123clr::advpars::mpg123_par(mpg123clr::mpg::parms type, int val, double fval)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_par(mp, (mpg123_parms) type, val, fval);
}

mpg123clr::mpg::ErrorCode mpg123clr::advpars::mpg123_getpar(mpg123clr::mpg::parms type, [Out] int% val, [Out] double% fval)
{
	// Avoid need for local intermediary variables
	pin_ptr<int> _val = &val;
	pin_ptr<double> _fval = &fval;

	int ret = ::mpg123_getpar(mp, (mpg123_parms) type, (long*)_val, _fval);

	_fval = nullptr;
	_val = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}


