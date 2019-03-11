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
		///<summary>id3check enumeration.</summary>
		public enum class id3check
		{
			none		= 0,
			id3			= 2,				/**< 0010 There is some ID3 info. */
			upd_id3		= MPG123_ID3,		/**< 0011 There is some ID3 info. Also matches 0010 or NEW_ID3. */
			new_id3		= MPG123_NEW_ID3,	/**< 0001 There is ID3 info that changed since last call to mpg123_id3. */
			icy			= 8,				/**< 1100 There is some ICY info. Also matches 0100 or NEW_ICY.*/
			upd_icy		= MPG123_ICY,		/**< 1100 There is some ICY info. Also matches 0100 or NEW_ICY.*/
			new_icy		= MPG123_NEW_ICY,	/**< 0100 There is ICY info that changed since last call to mpg123_icy. */
		};

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

		///<summary>Wrapper for mpg123_id3v1.</summary>
		[StructLayout(LayoutKind::Sequential)]
		public ref struct mpg123id3v1
		{
		private:
			mpg123_id3v1* sb;
		protected:
			///<summary>Finalizer.
			///<para>Cleanly handles mpg123_delete of internal (unmanaged) mpg123 handle.</para></summary>
			/// Implementation of CLR Finalize().
			!mpg123id3v1(void);
		internal:
			///<summary>Working Constructor.
			///<para>Maps to mpg123_id3v1 memory.</para>
			///</summary>
			mpg123id3v1(mpg123_id3v1* sb);
		public:
			///<summary>Constructor.
			///<para>Only creates object for use as "out" target in ID3 constructor.</para>
			///</summary>
			mpg123id3v1(void);

			///<summary>Destructor. Used for final object deletion.
			///<para>Calls finalizer for clean disposal of internal (unmanaged) library handles.</para>
			///</summary>
			/// Implementation of CLR Dispose().
			/// ~Destructor and !Finalizer are the prescribed implementation of Dispose() and Finalize().
			/// See Destructors and Finalizers in Visual C++
			~mpg123id3v1(void);

			///<summary>Get the ID3v1 tag descriptor.
			///<para>Property returns the ID3v1 tag (should always be "TAG").</para>
			///</summary>
			///<value>The ID3v1 tag (should always be "TAG").</value>
			property String^ tag{String^ __clrcall get();}

			///<summary>Get the ID3v1 title text.
			///<para>Property returns the ID3v1 title text.</para>
			///</summary>
			///<value>The ID3v1 title text.</value>
			property String^ title{String^ __clrcall get();}

			///<summary>Get the ID3v1 artist text.
			///<para>Property returns the ID3v1 artist text.</para>
			///</summary>
			///<value>The ID3v1 artist text.</value>
			property String^ artist{String^ __clrcall get();}

			///<summary>Get the ID3v1 album text.
			///<para>Property returns the ID3v1 album text.</para>
			///</summary>
			///<value>The ID3v1 album text.</value>
			property String^ album{String^ __clrcall get();}

			///<summary>Get the ID3v1 comment text.
			///<para>Property returns the ID3v1 comment text.</para>
			///</summary>
			///<value>The ID3v1 comment text.</value>
			property String^ comment{String^ __clrcall get();}

			///<summary>Get the ID3v1 genre index.
			///<para>Property returns the ID3v1 genre index.</para>
			///</summary>
			///<value>The ID3v1 genre index.</value>
			property int genre{int __clrcall get();}


		};
	}
}