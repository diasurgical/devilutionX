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
#include "text.h"

#include <string>
#include <iostream>

#include <vcclr.h>
using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace mpg123clr
{
	namespace id3
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

		///<summary>Wrapper for mpg123_id3v2.</summary>
		[StructLayout(LayoutKind::Sequential)]
		public ref struct mpg123id3v2
		{
		private:
			mpg123_id3v2* sb;

			array<mpg123text^>^ __clrcall MakeTextList(mpg123_text* ptr, size_t count);

		protected:
			///<summary>Finalizer.
			///<para>Does nothing - wrapper points to underlying memory, this class doesn't allocate memory.</para></summary>
			/// Implementation of CLR Finalize().
			!mpg123id3v2(void);

		internal:
			///<summary>Working Constructor.
			///<para>Maps to mpg123_id3v2 memory.</para>
			///</summary>
			mpg123id3v2(mpg123_id3v2* sb);

		public:
			///<summary>Constructor.
			///<para>Only creates object for use as "out" target in ID3 constructor.</para>
			///</summary>
			mpg123id3v2(void);

			///<summary>Destructor. Used for final object deletion.
			///<para>Calls finalizer for clean disposal of internal (unmanaged) library handles.</para>
			///</summary>
			/// Implementation of CLR Dispose().
			/// ~Destructor and !Finalizer are the prescribed implementation of Dispose() and Finalize().
			/// See Destructors and Finalizers in Visual C++
			~mpg123id3v2(void);

			///<summary>Get the ID3v2 title text.
			///<para>Property returns the last encountered ID3v2 title text.</para>
			///</summary>
			///<value>The last encountered ID3v2 title text.</value>
			property String^ title{String^ __clrcall get();}

			///<summary>Get the ID3v2 artist text.
			///<para>Property returns the last encountered ID3v2 artist text.</para>
			///</summary>
			///<value>The last encountered ID3v2 artist text.</value>
			property String^ artist{String^ __clrcall get();}

			///<summary>Get the ID3v2 album text.
			///<para>Property returns the last encountered ID3v2 album text.</para>
			///</summary>
			///<value>The last encountered ID3v2 album text.</value>
			property String^ album{String^ __clrcall get();}

			///<summary>Get the ID3v2 comment text.
			///<para>Property returns the last encountered ID3v2 comment text.</para>
			///</summary>
			///<value>The last encountered ID3v2 comment text.</value>
			property String^ comment{String^ __clrcall get();}

			///<summary>Get the ID3v2 year text.
			///<para>Property returns the last encountered ID3v2 year text.</para>
			///</summary>
			///<value>The last encountered ID3v2 year text.</value>
			property String^ year{String^ __clrcall get();}

			///<summary>Get the ID3v2 genre text.
			///<para>Property returns the last encountered ID3v2 genre text.</para>
			///</summary>
			///<value>The last encountered ID3v2 genre text.</value>
			property String^ genre{String^ __clrcall get();}

			///<summary>Get the ID3v2 version.
			///<para>Property returns the ID3v2 version.</para>
			///</summary>
			///<value>The ID3v2 version.</value>
			property int version{int __clrcall get();}


			///<summary>Get an array of Comments.
			///<para>Property returns an array of Comments.</para>
			///</summary>
			///<value>An array of Comments.</value>
			property array<mpg123clr::mpg123text^>^ Comments{array<mpg123clr::mpg123text^>^ __clrcall get();};

			///<summary>Get an array of ID3v2 text fields (including USLT).
			///<para>Property returns an array of ID3v2 text fields (including USLT).</para>
			///</summary>
			///<value>An array of ID3v2 text fields (including USLT).</value>
			property array<mpg123clr::mpg123text^>^ Texts{array<mpg123clr::mpg123text^>^ __clrcall get();};

			///<summary>Get an array of extra (TXXX) fields.
			///<para>Property returns an array of extra (TXXX) fields.</para>
			///</summary>
			///<value>An array of extra (TXXX) fields.</value>
			property array<mpg123clr::mpg123text^>^ Extras{array<mpg123clr::mpg123text^>^ __clrcall get();};

		};
	}
}
