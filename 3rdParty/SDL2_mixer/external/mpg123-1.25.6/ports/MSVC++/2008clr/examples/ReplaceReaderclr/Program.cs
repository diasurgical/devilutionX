/*
	replacereaderclr: test program for mpg123clr, showing how to use ReplaceReader in a CLR enviro.
	copyright 2009 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org	

    initially written by Malcolm Boczek
  
    not to be used as an example of good coding practices, note the total absence of error handling!!!  
*/

/*
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
	1.12.0.0 14-Apr-10	Added ReplaceReaderHandle sample code (mb)
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;                        // for ReplaceReaderHandle example
using System.Runtime.InteropServices;   // for ReplaceReaderHandle example

using mpg123clr;

namespace ReplaceReaderclr
{
    class Program
    {
        private unsafe static int MyReadFunc(int a, void* b, uint c)
        {
            // need to call posix read function here...

            // PosixRead is an example, substitute your replacement function here.
            int ret = mpg123.PosixRead(a, b, c);

            return ret;
        }

        private static int MySeekFunc(int a, int b, int c)
        {
            // NOTE: Largefile conflict with use of "int" position values.
            // Convert to long if off_t is defined as long long

            // need to call posix lseek function here...

            // PosixSeek is an example, substitute your replacement function here.
            int ret = mpg123.PosixSeek(a, b, c);

            return ret;
        }

        private unsafe static int MyHandleReadFunc(void* a, void* b, uint c)
        {
            GCHandle gch = GCHandle.FromIntPtr((IntPtr)a);
            BinaryReader br = (BinaryReader)gch.Target;

            byte[] buf = br.ReadBytes((int)c);

            // NOTE: no discernible performance difference between Marshal.Copy and ptr++ loop
            Marshal.Copy(buf, 0, (IntPtr)b, buf.Length);

//            byte* ptr = (byte*)b;
//            for (int i = 0, l = buf.Length; i < l; i++)
//                *(ptr++) = buf[i];

            return buf.Length;
        }

        private unsafe static int MyHandleSeekFunc(void* a, int b, int c)
        {
            // NOTE: Largefile conflict with use of "int" position values.
            // Convert to long if off_t is defined as long long

            GCHandle gch = GCHandle.FromIntPtr((IntPtr)a);
            BinaryReader br = (BinaryReader)gch.Target;

            return (int)br.BaseStream.Seek(b, (SeekOrigin)c);
        }

        private unsafe static void MyHandleCleanFunc(void* a)
        {
            GCHandle gch = GCHandle.FromIntPtr((IntPtr)a);
            BinaryReader br = (BinaryReader)gch.Target;

            br.Close();
        }

        static unsafe void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("I need a file to work on:\n\nPress any key to exit.");
                while (Console.Read() == 0) ;
                return;
            }
            mpg123clr.mpg.ErrorCode err;

            string filename = args[0];

            err = mpg123.mpg123_init();
            Console.WriteLine("Init:");

            RunReplaceReaderTest(filename);
            RunReplaceReaderHandleTest(filename);
            RunFrameByFrameTest(filename);

            Console.WriteLine("\nPress any key to exit:");
            while (Console.Read() == 0) ;

            mpg123.mpg123_exit();
        }

        static unsafe void RunReplaceReaderTest(string filename)
        {
            mpg123clr.mpg.ErrorCode err;

            mpg123 mp = new mpg123();
            err = mp.mpg123_new();

            // ReplaceReader example
            mpg123clr.mpg123.ReadDelegate rdel = MyReadFunc;
            mpg123clr.mpg123.SeekDelegate sdel = MySeekFunc;
            err = mp.mpg123_replace_reader(rdel, sdel);

            //err = mp.mpg123_open(args[0]);
            err = mp.mpg123_open(filename);

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Error: " + mp.mpg123_strerror());
            }
            else
            {
                Console.WriteLine("Open:");

                // Show available decoders
                string[] Decoders = mp.mpg123_decoders();

                if (Decoders.Length > 0)
                {
                    Console.WriteLine("\nDecoders:");
                    foreach (string str in Decoders) Console.WriteLine(str);
                }

                // Show supported decoders
                string[] supDecoders = mp.mpg123_supported_decoders();

                if (supDecoders.Length > 0)
                {
                    Console.WriteLine("\nSupported Decoders:");
                    foreach (string str in supDecoders) Console.WriteLine(str);
                }

                // Show actual decoder
                Console.WriteLine("\nDecoder: " + mp.mpg123_current_decoder());

                // Show estimated file length
                Console.WriteLine("\nLength Estimate: " + mp.mpg123_length().ToString());

                // Scan - gets actual details including ID3v2 and Frame offsets
                err = mp.mpg123_scan();

                // Show actual file length
                if (err == mpg123clr.mpg.ErrorCode.ok) Console.WriteLine("Length Actual  : " + mp.mpg123_length().ToString());

                // Get ID3 data
                mpg123clr.id3.mpg123id3v1 iv1;
                mpg123clr.id3.mpg123id3v2 iv2;
                err = mp.mpg123_id3(out iv1, out iv2);

                // Show ID3v2 data
                Console.WriteLine("\nTitle  : " + iv2.title);
                Console.WriteLine("Artist : " + iv2.artist);
                Console.WriteLine("Album  : " + iv2.album);
                Console.WriteLine("Comment: " + iv2.comment);
                Console.WriteLine("Year   : " + iv2.year);

                // Demo seek (back to start of file - note: scan should already have done this)
                long pos = mp.mpg123_seek(0, System.IO.SeekOrigin.Begin);

                long[] frameindex;
                long step;
                err = mp.mpg123_index(out frameindex, out step);

                if (err == mpg123clr.mpg.ErrorCode.ok)
                {
                    Console.WriteLine("\nFrameIndex:");
                    foreach (long idx in frameindex)
                    {
                        // Console.WriteLine(idx.ToString());
                    }
                }

                int num;
                uint cnt;
                IntPtr audio;

                // Walk the file - effectively decode the data without using it...
                Console.WriteLine("\nWalking  : " + iv2.title);
                DateTime dte, dts = DateTime.Now;

                while (err == mpg123clr.mpg.ErrorCode.ok || err == mpg123clr.mpg.ErrorCode.new_format)
                {
                    err = mp.mpg123_decode_frame(out num, out audio, out cnt);

                    // do something with "audio" here....
                }

                dte = DateTime.Now;

                TimeSpan ts = dte - dts;
                Console.WriteLine("Duration:  " + ts.ToString());

                mp.mpg123_close();
            }

            mp.Dispose();
        }

        static unsafe void RunReplaceReaderHandleTest(string filename)
        {
            mpg123clr.mpg.ErrorCode err;

            mpg123 mp = new mpg123();
            err = mp.mpg123_new();

            // ReplaceReader example
            mpg123clr.mpg123.ReadHandleDelegate rdel = MyHandleReadFunc;
            mpg123clr.mpg123.SeekHandleDelegate sdel = MyHandleSeekFunc;
            mpg123clr.mpg123.CleanupHandleDelegate cdel = MyHandleCleanFunc;
            err = mp.mpg123_replace_reader_handle(rdel, sdel, cdel);

            //err = mp.mpg123_open(args[0]);
            BinaryReader br = new BinaryReader(File.OpenRead(filename));

            err = mp.mpg123_open_handle(br);

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Error: " + mp.mpg123_strerror());
            }
            else
            {
                Console.WriteLine("Open:");

                // Show available decoders
                string[] Decoders = mp.mpg123_decoders();

                if (Decoders.Length > 0)
                {
                    Console.WriteLine("\nDecoders:");
                    foreach (string str in Decoders) Console.WriteLine(str);
                }

                // Show supported decoders
                string[] supDecoders = mp.mpg123_supported_decoders();

                if (supDecoders.Length > 0)
                {
                    Console.WriteLine("\nSupported Decoders:");
                    foreach (string str in supDecoders) Console.WriteLine(str);
                }

                // Show actual decoder
                Console.WriteLine("\nDecoder: " + mp.mpg123_current_decoder());

                // Show estimated file length
                Console.WriteLine("\nLength Estimate: " + mp.mpg123_length().ToString());

                // Scan - gets actual details including ID3v2 and Frame offsets
                err = mp.mpg123_scan();

                // Show actual file length
                if (err == mpg123clr.mpg.ErrorCode.ok) Console.WriteLine("Length Actual  : " + mp.mpg123_length().ToString());

                // Get ID3 data
                mpg123clr.id3.mpg123id3v1 iv1;
                mpg123clr.id3.mpg123id3v2 iv2;
                err = mp.mpg123_id3(out iv1, out iv2);

                // Show ID3v2 data
                Console.WriteLine("\nTitle  : " + iv2.title);
                Console.WriteLine("Artist : " + iv2.artist);
                Console.WriteLine("Album  : " + iv2.album);
                Console.WriteLine("Comment: " + iv2.comment);
                Console.WriteLine("Year   : " + iv2.year);

                // Demo seek (back to start of file - note: scan should already have done this)
                long pos = mp.mpg123_seek(0, System.IO.SeekOrigin.Begin);

                long[] frameindex;
                long step;
                err = mp.mpg123_index(out frameindex, out step);

                if (err == mpg123clr.mpg.ErrorCode.ok)
                {
                    Console.WriteLine("\nFrameIndex:");
                    foreach (long idx in frameindex)
                    {
                        // Console.WriteLine(idx.ToString());
                    }
                }

                int num;
                uint cnt;
                IntPtr audio;

                // Walk the file - effectively decode the data without using it...
                Console.WriteLine("\nWalking  : " + iv2.title);
                DateTime dte, dts = DateTime.Now;

                while (err == mpg123clr.mpg.ErrorCode.ok || err == mpg123clr.mpg.ErrorCode.new_format)
                {
                    err = mp.mpg123_decode_frame(out num, out audio, out cnt);

                    // do something with "audio" here....
                }

                dte = DateTime.Now;

                TimeSpan ts = dte - dts;
                Console.WriteLine("Duration:  " + ts.ToString());

                mp.mpg123_close();
            }

            mp.Dispose();

        }

        static unsafe void RunFrameByFrameTest(string filename)
        {
            mpg123clr.mpg.ErrorCode err;

            mpg123 mp = new mpg123();
            err = mp.mpg123_new();

            err = mp.mpg123_open(filename);

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Error: " + mp.mpg123_strerror());
            }
            else
            {
                Console.WriteLine("Open:");

                // Show available decoders
                string[] Decoders = mp.mpg123_decoders();

                if (Decoders.Length > 0)
                {
                    Console.WriteLine("\nDecoders:");
                    foreach (string str in Decoders) Console.WriteLine(str);
                }

                // Show supported decoders
                string[] supDecoders = mp.mpg123_supported_decoders();

                if (supDecoders.Length > 0)
                {
                    Console.WriteLine("\nSupported Decoders:");
                    foreach (string str in supDecoders) Console.WriteLine(str);
                }

                // Show actual decoder
                Console.WriteLine("\nDecoder: " + mp.mpg123_current_decoder());

                // Show estimated file length
                Console.WriteLine("\nLength Estimate: " + mp.mpg123_length().ToString());

                // Scan - gets actual details including ID3v2 and Frame offsets
                err = mp.mpg123_scan();

                // Show actual file length
                if (err == mpg123clr.mpg.ErrorCode.ok) Console.WriteLine("Length Actual  : " + mp.mpg123_length().ToString());

                // Get ID3 data
                mpg123clr.id3.mpg123id3v1 iv1;
                mpg123clr.id3.mpg123id3v2 iv2;
                err = mp.mpg123_id3(out iv1, out iv2);

                // Show ID3v2 data
                Console.WriteLine("\nTitle  : " + iv2.title);
                Console.WriteLine("Artist : " + iv2.artist);
                Console.WriteLine("Album  : " + iv2.album);
                Console.WriteLine("Comment: " + iv2.comment);
                Console.WriteLine("Year   : " + iv2.year);

                // Demo seek (back to start of file - note: scan should already have done this)
                long pos = mp.mpg123_seek(0, System.IO.SeekOrigin.Begin);

                long[] frameindex;
                long step;
                err = mp.mpg123_index(out frameindex, out step);

                if (err == mpg123clr.mpg.ErrorCode.ok)
                {
                    Console.WriteLine("\nFrameIndex:");
                    foreach (long idx in frameindex)
                    {
                        // Console.WriteLine(idx.ToString());
                    }
                }

                int num;
                uint cnt;
                IntPtr audio;

                // Walk the file - effectively decode the data without using it...
                Console.WriteLine("\nFrame Walking  : " + iv2.title);
                DateTime dte, dts = DateTime.Now;

                while (err == mpg123clr.mpg.ErrorCode.ok || err == mpg123clr.mpg.ErrorCode.new_format)
                {
                    err = mp.mpg123_framebyframe_decode(out num, out audio, out cnt);
                    err = mp.mpg123_framebyframe_next();

                    // do something with "audio" here....
                }

                dte = DateTime.Now;

                TimeSpan ts = dte - dts;
                Console.WriteLine("Duration:  " + ts.ToString());

                mp.mpg123_close();
            }

            mp.Dispose();

        }
    }
}
