/*
	mpg123clr: MPEG Audio Decoder library Common Language Runtime version.

	copyright 2009-2011 by Malcolm Boczek - free software under the terms of the LGPL 2.1
	mpg123clr.dll is a derivative work of libmpg123 - all original mpg123 licensing terms apply.

	All rights to this work freely assigned to the mpg123 project.
*/
/*
	libmpg123: MPEG Audio Decoder library

	copyright 1995-2011 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

*/
/*
	1.8.1.0	04-Aug-09	Initial release.
	1.9.0.0 16-Sep-09	1.9.0 Update - add enc_from_id3, store_utf8
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
	1.13.0.0 13-Jan-11	release match - added strlen (mb)
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


	///<summary>Wrapper for mpg123_string.
	///<para>mpg123str can be used as both (publicly) an instance object and (privately) a reference object.
	/// Construction and Finalizer operations perform differently depending on the instance type...
	///</para> 
	///<para>Instanced: i.e. new mpg123str();
	/// Normal operation of any object. Construction initializes memory and Destruction frees memory.
	///</para>
	///<para>Referenced: i.e. new mpg123str(sb);
	/// Construction and Destruction have no effect on the referenced object, mpg123str objects may be freely
	///  made and deleted without affecting the referenced object.
	///</para>
	///<para>However! 
	///</para>
	///<para>All methods operate on the referenced object (and NOT a copy of the object), the underlying mpg123_string
	///  is directly modified in situ. Therefore the referenced object can be initialized and disposed by
	///  explicitly calling Init() and Free().</para>
	///<para>The length of strings is limited to a 32bit value due to limitations of the CLI marshaler.
	///</para>
	///</summary>
	[StructLayout(LayoutKind::Sequential, CharSet=CharSet::Ansi)]
	public ref struct mpg123str
	{

	public:

		///<summary>text_encoding enumeration.</summary>
		enum class text_encoding
		{
			text_unknown  = mpg123_text_unknown,	/// Unkown encoding... mpg123_id3_encoding can return that on invalid codes.
			text_utf8     = mpg123_text_utf8,		/// UTF-8
			text_latin1   = mpg123_text_latin1,		/// ISO-8859-1. Note that sometimes latin1 in ID3 is abused for totally different encodings.
			text_icy      = mpg123_text_icy,		/// ICY metadata encoding, usually CP-1252 but we take it as UTF-8 if it qualifies as such.
			text_cp1252   = mpg123_text_cp1252,		/// Really CP-1252 without any guessing.
			text_utf16    = mpg123_text_utf16,		/// Some UTF-16 encoding. The last of a set of leading BOMs (byte order mark) rules.
													/// When there is no BOM, big endian ordering is used. Note that UCS-2 qualifies as UTF-8 when
													/// you don't mess with the reserved code points. If you want to decode little endian data
													/// without BOM you need to prepend 0xff 0xfe yourself.
			text_utf16bom = mpg123_text_utf16bom,	/// Just an alias for UTF-16, ID3v2 has this as distinct code.
			text_utf16be  = mpg123_text_utf16be,	/// Another alias for UTF16 from ID3v2. Note, that, because of the mess that is reality,
													/// BOMs are used if encountered. There really is not much distinction between the UTF16 types for mpg123
													/// One exception: Since this is seen in ID3v2 tags, leading null bytes are skipped for all other UTF16
													/// types (we expect a BOM before real data there), not so for utf16be!
			text_max      = mpg123_text_max			/// Placeholder for the maximum encoding value.
		};

		///<summary>id3_enc enumeration.</summary>
		enum class id3_enc
		{
			id3_latin1   = mpg123_id3_latin1,		/// Note: This sometimes can mean anything in practice... 
			id3_utf16bom = mpg123_id3_utf16bom,		/// UTF16, UCS-2 ... it's all the same for practical purposes. 
			id3_utf16be  = mpg123_id3_utf16be,		/// Big-endian UTF-16, BOM see note for mpg123_text_utf16be. 
			id3_utf8     = mpg123_id3_utf8,			/// Our lovely overly ASCII-compatible 8 byte encoding for the world. 
			id3_enc_max  = mpg123_id3_enc_max		/// Placeholder to check valid range of encoding byte. 
		};


	private:
		mpg123_string* sb;
		bool instanced;

	internal:
		///<summary>Reference Constructor. Does nothing to referenced mpg123_string structure.
		///<para>Reference objects may be freely created and deleted without affecting the underlying mpg123_string object.
		/// However, operations on the referenced object do modify the object in-situ (i.e. 'this' is not a 'copy'), and
		/// the referenced object may be explicitly initialized and disposed by calling the appropriate methods. (Init() and Free())</para>
		///<para>Recommended usage: using(mpg123str obj = new mpg123str(sb)){ use obj here }</para>
		///</summary>
		mpg123str(mpg123_string* sb);

		///<summary>Internal Constructor.
		///<para>Only used on empty fields.</para>
		///</summary>
		mpg123str(const char* str);

	protected:
		///<summary>Finalizer.
		///<para>Cleanly handles mpg123_free_string of instanced mpg123_string object.</para>
		///<para>Does not dispose referenced mpg123_string object. Referenced objects may be explicitly disposed by using Free().</para>
		///</summary>
		/// Implementation of CLR Finalize().
		!mpg123str(void);

	public:
		///<summary>Constructor, also initializes underlying mpg123_string structure.
		///<para>Instanced objects automatically dispose of underlying mpg123_string object.</para>
		///<para>Recommended usage: using(mpg123str obj = new mpg123str(sb)){ use obj here }</para>
		///</summary>
		mpg123str(void);

		///<summary>Destructor. Used for final object deletion.
		///<para>Instance objects call the finalizer for clean disposal of internal mpg123_string object.</para>
		///<para>Reference objects may be freely deleted without affecting the underlying mpg123_string object.
		/// However, the referenced object may be explicitly disposed by calling Free()</para>
		///</summary>
		// Implementation of CLR Dispose().
		// ~Destructor and !Finalizer are the prescribed implementation of Dispose() and Finalize().
		// See Destructors and Finalizers in Visual C++
		~mpg123str(void);

		///<summary>Append a C# string to this mpg123str.
		///<para>Returns 0 on error, 1 on success.</para>
		///</summary>
		///<param name="s">String to be appended.</param>
		///<returns>0 on error, 1 on success.</returns>
		int __clrcall mpg123_add_string(String ^ s);

		///<summary>Append a C# substring to this mpg123str.
		///<para>Returns 0 on error, 1 on success.</para>
		///</summary>
		///<param name="s">String to be appended.</param>
		///<param name="from">String offset to copy from.</param>
		///<param name="count">Number of characters to copy. (a null byte is always appended)</param>
		///<returns>0 on error, 1 on success.</returns>
		int __clrcall mpg123_add_substring(String ^ s, int from, int count);

		///<summary>Copy the contents of this string to another.
		///<para>Returns 0 on error, 1 on success.</para>
		///</summary>
		///<param name="to">Where to copy this string to.</param>
		///<returns>0 on error, 1 on success.</returns>
		int __clrcall mpg123_copy_string(mpg123str^ to);

		///<summary>Free-up mempory for an existing mpg123_string.
		///</summary>
		void __clrcall mpg123_free_string(void);

		///<summary>Increase size of a mpg123_string if necessary (it may stay larger).
		///<para>Returns 0 on error, 1 on success.</para>
		///</summary>
		///<param name="newSize">Required size.</param>
		///<returns>0 on error, 1 on success.</returns>
		int __clrcall mpg123_grow_string(int newSize);

		///<summary>Change the size of a mpg123_string.
		///<para>Returns 0 on error, 1 on success.</para>
		///</summary>
		///<param name="newSize">Required size.</param>
		///<returns>0 on error, 1 on success.</returns>
		int __clrcall mpg123_resize_string(int newSize);

		///<summary>Create and allocate memory for a new mpg123_string.
		///</summary>
		void __clrcall mpg123_init_string(void);

		///<summary>Set the contents to a C# string.
		///<para>Returns 0 on error, 1 on success.</para>
		///</summary>
		///<param name="s">String to be applied.</param>
		///<returns>0 on error, 1 on success.</returns>
		int __clrcall mpg123_set_string(String ^ s);

		///<summary>Set the contents to a C# substring.
		///<para>Returns 0 on error, 1 on success.</para>
		///</summary>
		///<param name="s">String to be applied.</param>
		///<param name="from">String offset to copy from.</param>
		///<param name="count">Number of characters to copy. (a null byte is always appended)</param>
		///<returns>0 on error, 1 on success.</returns>
		int __clrcall mpg123_set_substring(String ^ s, int from, int count);

		///<summary>Count characters in a mpg123 string (non-null bytes or UTF-8 characters).
		///<para>Even with the fill property, the character count is not obvious as there could be multiple trailing null bytes.</para>
		///<para>Returns the character count.</para>
		///</summary>
		///<param name="utf8">Flag to tell if the string is in utf8 encoding.</param>
		///<returns>Character count.</returns>
		long long __clrcall mpg123_strlen(bool utf8);

		///<summary>Get the number of used bytes. (including closing zero byte).
		///<para>Property returns the number of used bytes.</para>
		///</summary>
		///<value>The number of used bytes.</value>
		property int Fill{int __clrcall get();}			// property

		///<summary>Get the number of bytes allocated.
		///<para>Property returns the number of bytes allocated.</para>
		///</summary>
		///<value>The number of bytes allocated.</value>
		property int Size{int __clrcall get();}			// property

		///<summary>Get a C# string representation of the mpg123str.
		///<para>Property returns C# string text.</para>
		///</summary>
		///<value>C# string text.</value>
		property String^ Text{String^ __clrcall get();}

// 1.9.0.0 +add

		///<summary>Convert ID3 encoding byte to mpg123 encoding index.
		///<para>Returns the text_encoding enum of the converted value.</para>
		///</summary>
		///<param name="id3_enc_byte">The ID3 encoding byte to be converted.</param>
		///<returns>The text_encoding enum of the converted value.</returns>
		static text_encoding __clrcall mpg123_enc_from_id3(unsigned char id3_enc_byte);

		///<summary>Store text data in string, after converting to UTF-8 from indicated encoding.
		///<para>A prominent error can be that you provided an unknown encoding value, or this build of libmpg123 lacks support for certain encodings (ID3 or ICY stuff missing).
		/// Also, you might want to take a bit of care with preparing the data; for example, strip leading zeroes (I have seen that).</para>
		///<para>CLR - e.g. UnicodeEncoding(true, true) works with utf16be.</para>
		///<para>Returns 0 on error, 1 on success (on error, mpg123_free_string is called on sb).</para>
		///</summary>
		///<param name="enc">Mpg123 text encoding value.</param>
		///<param name="source">Source buffer with plain unsigned bytes.</param>
		///<param name="source_size">Number of bytes in the source buffer.</param>
		///<returns>0 on error, 1 on success (on error, mpg123_free_string is called on sb).</returns>
		int __clrcall mpg123_store_utf8(text_encoding enc, const unsigned char *source, size_t source_size);

// 1.9.0.0 -add

	};

}
