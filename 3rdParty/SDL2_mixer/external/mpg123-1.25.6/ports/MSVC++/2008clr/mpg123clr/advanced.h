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

#pragma once

#pragma warning(disable : 4635)
#include "mpg123.h"
#pragma warning(default : 4635)

#include "enum.h"
#include "error.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace mpg123clr
{
	// Recommended usage when creating reference type on the managed heap (not using stack semantics 
	//  for reference types...) [see (Microsoft) Destructors and Finalizers in Visual C++]
	//
	// C++/CLI
	// A myA;
	//		// use myA here
	//
	// Equivalent user code:
	//
	// A ^ myA = gcnew A;
	// try { /* use myA here */ } finally { delete myA; }
	//
	//
	// C# (from wikipedia)
	// using(A myA = new A()) { /* Use myA here */ }
    // Compiler calls myA.Dispose(), in a "finally" of a "try/finally" block
	//
    // Equivalent user code:
	//
    //  A myA = new myA();
    //  try { /* Use myA here */ } finally { myA.Dispose(); }
	//
	// Otherwise Finalizer will be nondeterministically called by GC


	///<summary> CLR wrapper for mpg123_pars object.</summary>
	public ref class advpars
	{
	protected public:
		mpg123_pars*	mp;

	protected:
		!advpars(void);

	public:
		///<summary>Constructor.</summary>
		///<param name="error">Returns MPG123_OK or mpg123 error code.</param>
		advpars([Out] int% error);

		///<summary>Destructor.</summary>
		~advpars(void);

		///<summary>Configure mpg123 parameters to accept no output formats.
		///<para>Use to clear default parameters prior to applying specific settings.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_fmt_none(void);

		///<summary>Configure mpg123 parameters to accept all formats including custom rate you may set - this is the default.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_fmt_all(void);

		///<summary>Configure detailed output formats.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="rate">Sample rate (Hertz)</param>
		///<param name="channels">Combination of channelcount.stereo and channelcount.mono</param>
		///<param name="encodings">Combination of accepted encodings for rate and channels e.g. enc.enc_signed16 | enc.enc_ulaw_8 (or 0 for none)</param>
		///<returns>MPG123 error codes.</returns>
		/// VERSION CHECK: long as int for Intellisense
		mpg123clr::mpg::ErrorCode __clrcall mpg123_fmt(int rate, mpg123clr::mpg::channelcount channels, int encodings);

		///<summary>Get available support for supplied rate and encoding.
		///<para>Returns 0 for no support (includes invalid parameters), or combination of channelcount.stereo and channelcount.mono.</para>
		///</summary>
		///<param name="rate">Sample rate (Hertz)</param>
		///<param name="encoding">Combination of accepted encodings for rate and channels e.g. enc.enc_signed16 | enc.enc_ulaw_8 (or 0 for none)</param>
		///<returns>Returns 0 for no support (includes invalid parameters), or combination of channelcount.stereo and channelcount.mono.</returns>
		/// VERSION CHECK: long as int for Intellisense
		mpg123clr::mpg::channelcount __clrcall mpg123_fmt_support(int rate, int encoding);

		///<summary>Set a specific advpars value.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="type">Specifies the advpars parameter to be set.</param>
		///<param name="val">The integer value to be applied.</param>
		///<param name="fval">The real value to be applied.</param>
		///<returns>MPG123 error codes.</returns>
		/// VERSION CHECK: long as int for Intellisense
		mpg123clr::mpg::ErrorCode __clrcall mpg123_par(mpg123clr::mpg::parms type, int val, double fval);

		///<summary>Get a specific advpars value.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="type">Specifies the advpars parameter to be returned.</param>
		///<param name="val">Returns the applicable integer value.</param>
		///<param name="fval">Returns the applicable real value.</param>
		///<returns>MPG123 error codes.</returns>
		/// VERSION CHECK: long as int for Intellisense
		mpg123clr::mpg::ErrorCode __clrcall mpg123_getpar(mpg123clr::mpg::parms type, [Out] int% val, [Out] double% fval);
	};

}