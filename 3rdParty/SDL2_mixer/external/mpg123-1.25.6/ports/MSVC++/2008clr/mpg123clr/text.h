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

#pragma once

#pragma warning(disable : 4635)
#include "mpg123.h"
#pragma warning(default : 4635)

#include "string.h"

#include <string>
#include <iostream>

#include <vcclr.h>
using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace mpg123clr
{
	// Recommended usage when creating reference type on the managed heap (not using stack semantics 
	//  for reference types...) [see Destructors and Finalizers in Visual C++]
	//
	// A ^ myA = gcnew A;
	// try
	// {
	//     use myA
	// }
	// finally
	// {
	//     delete myA;
	// }


	///<summary>Wrapper for mpg123_text.</summary>
	[StructLayout(LayoutKind::Sequential, CharSet=CharSet::Ansi)]
	public ref struct mpg123text
	{
	private:
		mpg123_text* sb;

	internal:
		///<summary>Constructor.
		///<para>Recommended usage: only as tracking marker to existing mpg123_text objects.</para></summary>
		mpg123text(void);

		///<summary>Reference Constructor.
		///<para>Recommended usage: only as tracking marker to existing mpg123_text objects.</para></summary>
		mpg123text(mpg123_text* sb);

	protected:
		///<summary>Finalizer.
		///<para>Does nothing. mpg123text can only be instanced as a reference to an existing mpg123_text object (which is responsible for its own disposal.</para></summary>
		/// Implementation of CLR Finalize().
		!mpg123text(void);

	public:
		///<summary>Destructor. Used for final object deletion.
		///<para>Calls finalizer.</para>
		///</summary>
		/// Implementation of CLR Dispose().
		/// ~Destructor and !Finalizer are the prescribed implementation of Dispose() and Finalize().
		/// See Destructors and Finalizers in Visual C++
		~mpg123text(void);

		///<summary>Get a string representation of the 3-letter language code.
		///<para>Only COMM and USLT have a language element.</para>
		///<para>Property returns string representation of the 3-letter language code.</para>
		///</summary>
		///<value>String representation of the 3-letter language code.</value>
		property String^ lang{String^ __clrcall get();}

		///<summary>Get a string representation of the ID3v2 field id. (i.e. TALB, TPE2 etc.)
		///<para>Property returns string representation of the ID3v2 field id. (i.e. TALB, TPE2 etc.)</para>
		///</summary>
		///<value>String representation of the ID3v2 field id. (i.e. TALB, TPE2 etc.)</value>
		property String^ id{String^ __clrcall get();}

		///<summary>Get a string representation of the description field.
		///<para>Only COMM and TXXX have a description element.</para>
		///<para>Property returns string representation of the description field.</para>
		///</summary>
		///<value>String representation of the description field.</value>
		property String^ description{String^ __clrcall get();}

		///<summary>Get a string representation of the ID3 tag text.
		///<para>This is for COMM, TXXX and all the other text fields.</para>
		///<para>Property returns string representation of the ID3 tag text.</para>
		///</summary>
		///<value>String representation of the ID3 tag text.</value>
		property String^ text{String^ __clrcall get();}

	};
}
