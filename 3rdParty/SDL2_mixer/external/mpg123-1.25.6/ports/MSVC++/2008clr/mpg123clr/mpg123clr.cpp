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
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
	1.9.0.0 01-Oct-09	Technical cleanup - subst nullptr for NULL (mb)
	1.9.0.0 13-Oct-09	pin_ptr = nullptr on return (mb)
	1.9.0.1	24-Nov-09	performance update - removed try/finally (mb)
	1.10.0.0 30-Nov-09	release match - added mpg123_feature (mb)
	1.12.0.0 14-Apr-10	release match - added framebyframe and "handle" ReplaceReaders (mb)
	1.13.0.0 13-Jan-11	release match - added encsize (mb)
*/

// mpg123clr.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mpg123clr.h"


mpg123clr::mpg123::mpg123(void)
{
	mh = nullptr;
	useHandleReplacement = false;
	lastReplacementWasHandle = false;
}

mpg123clr::mpg123::mpg123(mpg123_handle* mh)
{
	this->mh = mh;
	useHandleReplacement = false;
	lastReplacementWasHandle = false;
}

// Destructor
mpg123clr::mpg123::~mpg123(void)
{
	// clean up code to release managed resources

	// call Finalizer to clean up unmanaged resources
	this->!mpg123();

	// CLI implements SuppressFinalize - therefore not required here...
	//GC::SuppressFinalize(this);
}

// Finalizer
mpg123clr::mpg123::!mpg123(void)
{
	// clean up unmanaged resources
	if (mh != nullptr) 
	{
		::mpg123_delete(mh);
		mh = nullptr;
	}
}

void mpg123clr::mpg123::mpg123_delete(void)
{
	if (mh != nullptr) 
	{
		::mpg123_delete(mh);
		mh = nullptr;
	}
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_init(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_init();
}

void mpg123clr::mpg123::mpg123_exit(void)
{
	::mpg123_exit();
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_new(String ^ decoder)
{
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(decoder)).ToPointer();

	int err;
	mh = ::mpg123_new(chars, &err);

	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return (mpg123clr::mpg::ErrorCode) err;
}
mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_new(void)
{
	int err;
	mh = ::mpg123_new(NULL,  &err);

	return (mpg123clr::mpg::ErrorCode) err;
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_parnew(mpg123clr::advpars^ par, String ^ decoder)
{
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(decoder)).ToPointer();

	int err;
	mh = ::mpg123_parnew(par->mp, chars,  &err);

	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return (mpg123clr::mpg::ErrorCode) err;
}
mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_parnew(mpg123clr::advpars^ par)
{
	int err;
	mh = ::mpg123_parnew(par->mp, NULL,  &err);

	return (mpg123clr::mpg::ErrorCode) err;
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_param(mpg123clr::mpg::parms type, int val, double fval)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_param(mh, (mpg123_parms)type, val, fval);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_getparam(mpg123clr::mpg::parms type, [Out] int% val, [Out] double% fval)
{
	// Avoid need for local intermediary variables
	pin_ptr<int> _val = &val;
	pin_ptr<double> _fval = &fval;

	int ret = ::mpg123_getparam(mh, (mpg123_parms)type, (long*) _val, _fval);

	_fval = nullptr;
	_val = nullptr;

	return (mpg123clr::mpg::ErrorCode) ret;
}

int mpg123clr::mpg123::mpg123_feature(mpg123clr::mpg::feature_set key)
{
	return ::mpg123_feature((mpg123_feature_set) key);
}

String^ mpg123clr::mpg123::mpg123_strerror(void)
{
	return gcnew String(::mpg123_strerror(mh));
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_errcode(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_errcode(mh);
}

array<String^>^ mpg123clr::mpg123::StringArrayFromPtr(const char** ptr)
{
	int count = 0;

	// count how many strings in array by walking up the array until NULL found
	while (*(ptr++) != NULL){ ++count; }

	// create an array of the correct size
	array<String^> ^ str = gcnew array<String^>(count);

	--ptr; // loop leaves ptr +2 beyond end of array - put ptr back to just after last index

	// walk back down the array, extracting the strings
	while (count-- > 0){ str[count] = gcnew String(*(--ptr)); }

	return str;
}

array<String^>^ mpg123clr::mpg123::mpg123_decoders(void)
{
	return StringArrayFromPtr(::mpg123_decoders());
}

array<String^>^ mpg123clr::mpg123::mpg123_supported_decoders(void)
{
	return StringArrayFromPtr(::mpg123_supported_decoders());
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_decoder(String^ name)
{
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(name)).ToPointer();

	int ret;

	// one of the few mpg123 places that fault with an exception...
	try	{ ret = ::mpg123_decoder(mh, chars); }
	catch(Exception^){ ret = MPG123_ERR;	}

	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return (mpg123clr::mpg::ErrorCode) ret;
}

String^ mpg123clr::mpg123::mpg123_current_decoder(void)
{
	return gcnew String(::mpg123_current_decoder(mh));
}

array<long>^ mpg123clr::mpg123::mpg123_rates(void)
{
	size_t number;
	const long* list;

	::mpg123_rates(&list, &number);

	array<long>^ rList = gcnew array<long>((int)number);

	int index = 0;

	// walk the array, extracting the rates
	while (index < (int)number){ rList[index++] = *(list++); }

	return rList;
}

array<mpg123clr::mpg::enc>^ mpg123clr::mpg123::mpg123_encodings(void)
{
	size_t number;
	const int* list;

	::mpg123_encodings(&list, &number);

	// WARN 4267 - assuming that the number of encodings will never exceed 32bits
	array<mpg123clr::mpg::enc>^ rList = gcnew array<mpg123clr::mpg::enc>((int)number);

	int index = 0;

	// walk the array, extracting the rates
	while (index < (int)number){ rList[index++] = (mpg123clr::mpg::enc) *(list++); }

	return rList;
}

int mpg123clr::mpg123::mpg123_encsize(mpg123clr::mpg::enc encoding)
{
	return ::mpg123_encsize((int) encoding);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_format_none(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_format_none(mh);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_format_all(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_format_all(mh);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_format(int rate, mpg123clr::mpg::channelcount channels, mpg123clr::mpg::enc encodings)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_format(mh, rate, (int) channels, (int) encodings);
}

mpg123clr::mpg::channelcount mpg123clr::mpg123::mpg123_format_support(int rate, mpg123clr::mpg::enc encodings)
{
	return (mpg123clr::mpg::channelcount) ::mpg123_format_support(mh, rate, (int) encodings);
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_getformat([Out] int% rate, [Out] mpg123clr::mpg::channelcount% channels, [Out] mpg123clr::mpg::enc% encodings)
{
	// either:-
	//
	// long _rate;
	// int _chan, _enc;
	// int ret = mpg123_getformat(mh, &_rate, &_chan, &_enc);
	// rate = _rate;
	// channels = _chan;
	// encodings = _enc;
	// return (mpg123clr::mpg::ErrorCode) ret;
	//
	// or:-
	// use pinned pointers instead
	pin_ptr<int> _rate = &rate;
	pin_ptr<mpg123clr::mpg::channelcount> _chan = &channels;
	pin_ptr<mpg123clr::mpg::enc> _enc = &encodings;

	int ret = ::mpg123_getformat(mh, (long*)_rate, (int*)_chan, (int*)_enc);

	_enc = nullptr;
	_chan = nullptr;
	_rate = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_open(String^ path)
{
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(path)).ToPointer();

	_ReplaceReader();

	int ret = ::mpg123_open(mh, chars);

	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_open_fd(int filedescriptor)
{
	_ReplaceReader();

	return (mpg123clr::mpg::ErrorCode) ::mpg123_open_fd(mh, filedescriptor);
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_open_handle(System::Object^ obj)
{
	// NOTE: You must have configured callbacks using mpg123_replace_reader_handle
	// before calling mpg123_open_handle!

	_ReplaceReader();	// activate callbacks

	// Make sure we free this in Close functions
	userObjectHandle = GCHandle::Alloc(obj);

	// NOTE: This sends a HANDLE not an ADDRESS, no pinning required
	return (mpg123clr::mpg::ErrorCode) ::mpg123_open_handle(mh, (void*)GCHandle::ToIntPtr(userObjectHandle).ToPointer());
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_open_feed(void)
{
	_ReplaceReader();

	return (mpg123clr::mpg::ErrorCode) ::mpg123_open_feed(mh);
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_close(void)
{
	// Closes 'mh' and calls Cleanup delegate if provided
	int ret = ::mpg123_close(mh);

	// See GCHandle.Free - caller must ensure Free called only once for a given handle.
	if (userObjectHandle.IsAllocated) userObjectHandle.Free();
		
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_read(array<unsigned char>^ buffer, [Out] size_t% count)
{
	pin_ptr<size_t> _count = &count;
	pin_ptr<unsigned char> _ptr = &buffer[0];

	int ret = ::mpg123_read(mh, _ptr, buffer->Length, _count);

	_ptr = nullptr;
	_count = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_read(array<unsigned char>^ buffer, size_t offset, size_t size, [Out] size_t% count)
{
	pin_ptr<size_t> _count = &count;
	// WARN 4267 - clr limited to 32bit-length-size arrays!!
	pin_ptr<unsigned char> _ptr = &buffer[(int)offset];

	int ret = ::mpg123_read(mh, _ptr, size, _count);

	_ptr = nullptr;
	_count = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}


mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_feed(array<unsigned char>^ inbuffer, size_t size)
{
	pin_ptr<unsigned char> _ptr = &inbuffer[0];

	int ret = ::mpg123_feed(mh, _ptr, size);

	_ptr = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_decode(array<unsigned char>^ inbuffer, size_t insize, array<unsigned char>^ outbuffer, size_t outsize, [Out] size_t% count)
{
	pin_ptr<size_t> _count = &count;
	pin_ptr<const unsigned char> _inptr = &inbuffer[0];
	pin_ptr<unsigned char> _outptr = &outbuffer[0];

	int ret = ::mpg123_decode(mh, _inptr, insize, _outptr, outsize, _count);

	_outptr = nullptr;
	_inptr = nullptr;
	_count = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_decode_frame([Out] off_t% num, [Out] IntPtr% audio, [Out] size_t% count)
{
	pin_ptr<size_t> _count = &count;
	pin_ptr<off_t> _num = &num;
	pin_ptr<IntPtr> _x = &audio;

	int ret = ::mpg123_decode_frame(mh, _num, (unsigned char**)_x, _count);

	_x = nullptr;
	_num = nullptr;
	_count = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_framebyframe_decode([Out] off_t% num, [Out] IntPtr% audio, [Out] size_t% count)
{
	// NOTE: must use framebyframe_next to obtain successive frames

	pin_ptr<size_t> _count = &count;
	pin_ptr<off_t> _num = &num;
	pin_ptr<IntPtr> _x = &audio;

	int ret = ::mpg123_framebyframe_decode(mh, _num, (unsigned char**)_x, _count);

	_x = nullptr;
	_num = nullptr;
	_count = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode  mpg123clr::mpg123::mpg123_framebyframe_next()
{
	// mpg123lib has warning, Experimental API. Watch for updates!!!
	// framebyframe_decode doesn't automatically move on to next frame,
	// use framebyframe_next to move on to "Find, read and parse the next mp3 frame"
	int ret = ::mpg123_framebyframe_next(mh);
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

long long mpg123clr::mpg123::mpg123_tell(void)
{
	return ::mpg123_tell(mh);
}

long long mpg123clr::mpg123::mpg123_tellframe(void)
{
	return ::mpg123_tellframe(mh);
}

long long mpg123clr::mpg123::mpg123_tell_stream(void)
{
	return ::mpg123_tell_stream(mh);
}

long long mpg123clr::mpg123::mpg123_seek(long long offset, SeekOrigin origin)
{
	return ::mpg123_seek(mh, (off_t)offset, (int)origin);
}

long long mpg123clr::mpg123::mpg123_feedseek(long long offset, SeekOrigin origin, [Out] long long% input_offset)
{
	// NOTE: off_t fiddles...
	// pin_ptr<int> _input_offset = &input_offset; // type mismatch between 32 and 64 bit offsets.

	off_t _input_offset;	// type accomodation

	off_t ret = ::mpg123_feedseek(mh, (off_t)offset, (int)origin, &_input_offset); // ok types

	input_offset = _input_offset;	// type conversion

	return ret;
}

long long mpg123clr::mpg123::mpg123_seek_frame(long long frameoffset, SeekOrigin origin)
{
	return ::mpg123_seek_frame(mh, (off_t)frameoffset, (int)origin);
}

long long mpg123clr::mpg123::mpg123_timeframe(double seconds)
{
	return ::mpg123_timeframe(mh, seconds);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_index([Out] array<long long>^% index, [Out] long long% step)
{
	// remnant: only works if off_t, index and step are 32 bit - possible redo for 64 bit library
	// pin_ptr<int> _step = &step;
	// int ret = mpg123_index(mh, &_list, (off_t*)_step, &_count);
	// index = gcnew array<int>((int)_count);
	// Marshal::Copy((IntPtr)_list, index, 0, (int)_count);

	// alternate: works for 32 and 64 bit libraries - returns long long values
	off_t* _list;
	size_t _count;
	off_t _step;	// type accomodation

	int ret = ::mpg123_index(mh, &_list, &_step, &_count);

	step = _step;	// type conversion

	// WARN 4267 - clr limited to 32bit-length-size arrays!!
	index = gcnew array<long long>((int)_count);

	// walk the array, extracting the rates
	int idx = 0;	// array length limited to 32bit
	while (idx < index->Length){ index[idx++] = *(_list++); }

	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_index([Out] IntPtr% indexarr, [Out] long long% step, [Out] size_t% fill)
{
	pin_ptr<size_t> _fill = &fill;
	pin_ptr<IntPtr> _x = &indexarr;	// NOTE: untyped index pointer.
	off_t _step;	// type accomodation

	int ret = ::mpg123_index(mh, (off_t**)_x, &_step, _fill);

	step = _step;	// type conversion

	_x = nullptr;
	_fill = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_position(
	long long frameoffset, long long bufferedbytes, [Out] long long% currentframe, [Out] long long% framesleft,
			[Out] double% currentseconds, [Out] double% secondsleft)
{
	// NOTE: off_t fiddles
	// pin_ptr<int> _currentframe = &currentframe;
	// pin_ptr<int> _framesleft = &framesleft;
	pin_ptr<double> _currentseconds = &currentseconds;
	pin_ptr<double> _secondsleft = &secondsleft;
	off_t _currentframe;	// type accomodation
	off_t _framesleft;		// type accomodation

	int ret =  ::mpg123_position(mh, (off_t)frameoffset, (off_t)bufferedbytes, &_currentframe, &_framesleft, _currentseconds, _secondsleft);

	currentframe = _currentframe;	// type conversion
	framesleft = _framesleft;		// type conversion

	_secondsleft = nullptr;
	_currentseconds = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

#pragma region Volume and Equalizer

		/// \defgroup mpg123_voleq mpg123 volume and equalizer
		///

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_eq(mpg123clr::mpg::channels channel, int band, double fval)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_eq(mh, (mpg123_channels)channel, band, fval);
}

double mpg123clr::mpg123::mpg123_geteq(mpg123clr::mpg::channels channel, int band)
{
	return ::mpg123_geteq(mh, (mpg123_channels)channel, band);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_reset_eq(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_reset_eq(mh);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_volume(double volume)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_volume(mh, volume);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_volume_change(double change)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_volume_change(mh, change);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_getvolume([Out] double% basevol, [Out] double% really, [Out] double% rva_db)
{
	pin_ptr<double> _basevol = &basevol;
	pin_ptr<double> _really = &really;
	pin_ptr<double> _rva_db = &rva_db;

	int ret = ::mpg123_getvolume(mh, _basevol, _really, _rva_db);

	_rva_db = nullptr;
	_really = nullptr;
	_basevol = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

#pragma endregion -Volume and Equalizer
#pragma region Status and Information

		// \defgroup mpg123_status mpg123 status and information
		//
		//
		//

// The "proper" way to manage structs...
mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_safeinfo([Out]mpeg_frameinfo^% finfo)
{
	// "out" our return reference and grab some memory
	finfo = gcnew mpeg_frameinfo;
	IntPtr pp = Marshal::AllocHGlobal(Marshal::SizeOf(finfo));

	// get the info

	// could cast away...
	//	int ret = mpg123_info(mh, reinterpret_cast<mpg123_frameinfo*>((int)pp));
	//	int ret = mpg123_info(mh, static_cast<mpg123_frameinfo*>((void*)pp));

	// or let the compiler decide...
	int ret = ::mpg123_info(mh, (mpg123_frameinfo*)(void*)pp);

	// marshal data into return object and free temporary memory
	Marshal::PtrToStructure(pp, finfo);
	Marshal::FreeHGlobal(pp);

	return (mpg123clr::mpg::ErrorCode) ret;
}

// The "efficient" way to manage structs...
mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_info([Out]mpeg_frameinfo^% finfo)
{
	finfo = gcnew mpeg_frameinfo;

	pin_ptr<mpg123clr::mpg::mpeg_version> _ptr = &finfo->version;

	// According to SizeOf...
	// The unmanaged and managed sizes of an object can differ, this would imply
	// that the memory layout for each would also be different, which would indicate
	// that using a ptr, derived from a ptr-to-a-managed type, in unmanaged code could
	// have unforseen results.
	//
	// In non-homogenous structs (like mpg123text) it can lead to corrupted CLR stack.
	// In homogenous structs (like finfo) it appears to be "managable".
	//
	// However, until it fails it'll do... and it's much faster (see mpg123_safeinfo(...)).

	// WARNING:
	// The epitome of "unsafe" as defined by CLR (using unmanaged pointers).
	// If we start to get CLR stack corruptions - check here first! (see SafeInfo for "safe" managed version)
	int ret = ::mpg123_info(mh, (mpg123_frameinfo*)_ptr);

	_ptr = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

size_t mpg123clr::mpg123::mpg123_safe_buffer(void)
{
	return ::mpg123_safe_buffer();
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_scan(void)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_scan(mh);
}

long long mpg123clr::mpg123::mpg123_length(void)
{
	return ::mpg123_length(mh);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_set_filesize(long long size)
{
	return (mpg123clr::mpg::ErrorCode) ::mpg123_set_filesize(mh, (off_t)size);
}

double mpg123clr::mpg123::mpg123_tpf(void)
{
	return ::mpg123_tpf(mh);
}

long mpg123clr::mpg123::mpg123_clip(void)
{
	return ::mpg123_clip(mh);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_getstate(mpg123clr::mpg::state key, [Out] int% val, [Out] double% fval)
{
	pin_ptr<int> _val = &val;
	pin_ptr<double> _fval = &fval;

	int ret = ::mpg123_getstate(mh, (mpg123_state)key, (long*) _val, _fval);

	_fval = nullptr;
	_val = nullptr;
	
	return (mpg123clr::mpg::ErrorCode) ret;
}

#pragma endregion -Status and Information

#pragma region Metadata Handling

		// \defgroup mpg123_metadata mpg123 metadata handling
		//
		// Functions to retrieve the metadata from MPEG Audio files and streams.
		// Also includes string handling functions.
		//

mpg123clr::id3::id3check  mpg123clr::mpg123::mpg123_meta_check(void)
{
	return (mpg123clr::id3::id3check ) ::mpg123_meta_check(mh);
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_id3([Out]id3::mpg123id3v1^% v1, [Out]id3::mpg123id3v2^% v2)
{
	mpg123_id3v1* pv1;
	mpg123_id3v2* pv2;

	// doc says "pv1 and pv2 may be set to NULL when there is no corresponding data."
	// they may also be set to point to empty structure...
	int ret = ::mpg123_id3(mh, &pv1, &pv2);

	v1 = gcnew mpg123clr::id3::mpg123id3v1(pv1);
	v2 = gcnew mpg123clr::id3::mpg123id3v2(pv2);

	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_icy([Out]IntPtr% icy_meta)
{
	char* _icy_meta;

	int ret = ::mpg123_icy(mh, &_icy_meta);

	icy_meta = (IntPtr) _icy_meta;

	return (mpg123clr::mpg::ErrorCode) ret;
}

array<unsigned char>^ mpg123clr::mpg123::mpg123_icy2utf8(IntPtr icy_text)
{
	// TODO: Do we really need this?
	// char* putf8 = mpg123_icy2utf8(const_cast<char*>(reinterpret_cast<char*>(icy_text.ToPointer())));

	// Or is this adequate?
	char* putf8 = ::mpg123_icy2utf8((const char*)(void*) icy_text);

	// WARN 4267 - clr limited to 32bit-length-size arrays!!
	array<unsigned char>^ ary = gcnew array<unsigned char>((int)strlen(putf8));

	Marshal::Copy((IntPtr)putf8, ary, 0, ary->Length);
	free(putf8);

	return ary;
}

#pragma endregion -Metadata Handling

#pragma region Low Level I/O

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_replace_buffer(IntPtr data, size_t size)
{
	// "data" buffer should be fixed BEFORE calling this function.

	// TODO: to cast or not to cast??
	// return (mpg123clr::mpg::ErrorCode) mpg123_replace_buffer(mh, reinterpret_cast<unsigned char*>(data.ToPointer()), size);

	return (mpg123clr::mpg::ErrorCode) ::mpg123_replace_buffer(mh, (unsigned char*)(void*) data, size);
}

size_t mpg123clr::mpg123::mpg123_outblock(void)
{
	return ::mpg123_outblock(mh);
}

void mpg123clr::mpg123::_ReplaceReader(void)
{
	if ((readDel == r_readDel) && (seekDel == r_seekDel)
		&& (readHDel == r_readHDel) && (seekDel == r_seekHDel) && (cleanHDel == r_cleanHDel)) return;

	// readDel and seekDel are "keep alive" fields that prevent GC of the delegates.
	// the underlying function pointers no not require "pinning".

	// Note: most examples use GCHandle::Alloc to "pin" the delegate but this is usually to prevent GC
	// outside of function scope and is not strictly necessary here since readDel and seekDel never go
	// out of scope.

	// See MS: c++, How to: Marshal Callbacks and Delegates Using C++ Interop
	// for more details.

	readDel = r_readDel;
	seekDel = r_seekDel;
	readHDel = r_readHDel;
	seekHDel = r_seekHDel;
	cleanHDel = r_cleanHDel;

	// just for clarity
	typedef off_t (_cdecl* SEEKCB)(int, off_t, int);
	typedef ssize_t (_cdecl* READCB)(int, void*, size_t);
	typedef off_t (_cdecl* HSEEKCB)(void*, off_t, int);
	typedef ssize_t (_cdecl* HREADCB)(void*, void*, size_t);
	typedef void (_cdecl* HCLEANCB)(void*);

	// NOTE: GetFunctionPointerForDelegate doesn't like nullptr
	// NOTE: I'm not suggesting that replace_reader and replace_reader_handle should be
	//       intermingled, just trying to maintain a clean stack

	if (!useHandleReplacement)
	{
		if (lastReplacementWasHandle)
			::mpg123_replace_reader_handle(mh, nullptr, nullptr, nullptr);

		::mpg123_replace_reader(
			mh, 
			(readDel != nullptr) ? (READCB)(Marshal::GetFunctionPointerForDelegate(readDel)).ToPointer() : nullptr, 
			(seekDel != nullptr) ? (SEEKCB)(Marshal::GetFunctionPointerForDelegate(seekDel)).ToPointer() : nullptr
			);
	}
	else
	{
		if (!lastReplacementWasHandle)	// can give redundant call on first use - microscopically inefficient - not catastrophic
			::mpg123_replace_reader(mh, nullptr, nullptr);

		::mpg123_replace_reader_handle(
			mh, 
			(readHDel != nullptr) ? (HREADCB)(Marshal::GetFunctionPointerForDelegate(readHDel)).ToPointer() : nullptr, 
			(seekHDel != nullptr) ? (HSEEKCB)(Marshal::GetFunctionPointerForDelegate(seekHDel)).ToPointer() : nullptr,
			(cleanHDel != nullptr) ? (HCLEANCB)(Marshal::GetFunctionPointerForDelegate(cleanHDel)).ToPointer() : nullptr
			);
	}
	lastReplacementWasHandle = useHandleReplacement;
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_replace_reader(ReadDelegate^ r_read, SeekDelegate^ r_lseek)
{
	// save temporary delegates to be implemented at next 'Open'
	r_readDel = r_read;
	r_seekDel = r_lseek;
	r_readHDel = nullptr;
	r_seekHDel = nullptr;
	r_cleanHDel = nullptr;
	useHandleReplacement = false;

	return mpg123clr::mpg::ErrorCode::ok;

}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_replace_reader_handle(ReadHandleDelegate^ rh_read, SeekHandleDelegate^ rh_lseek, CleanupHandleDelegate^ rh_clean)
{
	// save temporary delegates to be implemented at next 'Open'
	r_readHDel = rh_read;
	r_seekHDel = rh_lseek;
	r_cleanHDel = rh_clean;
	r_readDel = nullptr;
	r_seekDel = nullptr;
	useHandleReplacement = true;

	return mpg123clr::mpg::ErrorCode::ok;

}

#pragma endregion -Low Level I/O

#pragma region MS Unicode Extension

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_topen(String^ path)
{
	_ReplaceReader();	// mpg123_tOpen will replace its own reader, this is just for consistency

	const _TCHAR* chars = (const _TCHAR*)(Marshal::StringToHGlobalUni(path)).ToPointer();

	int ret = ::mpg123_topen(mh, chars);

	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return (mpg123clr::mpg::ErrorCode) ret;
}

mpg123clr::mpg::ErrorCode mpg123clr::mpg123::mpg123_tclose(void)
{
	// Not sure if t_close calls Cleanup (it shouldn't since t_open substitutes its own readers)
	int ret = ::mpg123_tclose(mh);

	// Fairly sure replace_reader_handle, topen and tclose are incompatible, just for consistency
	// See GCHandle.Free - caller must ensure Free called only once for a given handle.
	if (userObjectHandle.IsAllocated) userObjectHandle.Free();
		
	return (mpg123clr::mpg::ErrorCode) ret;
}

#pragma endregion -MS Unicode Extension


