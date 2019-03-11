/*
	feedseekclr: test program for mpg123clr, showing how to use fuzzy seeking in feeder mode
	copyright 2009 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org	

    based on feedseek.c example for libmpg123.
 
    Comment (Malcolm Boczek)
    this CLR example has been written to allow easy comparison to the original feedseek.c example
    and uses some constructs that would not normally be used in a C# environment,
        eg: byte[]/ASCII text, Marshal.Copy, static fields, lots of casts etc.
*/

/*
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;
using System.Runtime.InteropServices;

using mpg123clr;

namespace feedseekclr
{
    class Program
    {
        const int WAVE_FORMAT_PCM = 0x0001;
        const int WAVE_FORMAT_IEEE_FLOAT = 0x0003;

        static BinaryWriter _out;
        static long totaloffset, dataoffset;
        static int rate;
        static mpg123clr.mpg.channelcount channels;
        static mpg123clr.mpg.enc enc;
        static short bitspersample, wavformat;

        // write wav header
        static void initwav()
        {
            uint tmp32 = 0;
            ushort tmp16 = 0;
            byte[] rifftxt = new byte[] { (byte)'R', (byte)'I', (byte)'F', (byte)'F' };
            byte[] wavetxt = new byte[] { (byte)'W', (byte)'A', (byte)'V', (byte)'E' };
            byte[] fmttxt  = new byte[] { (byte)'f', (byte)'m', (byte)'t', (byte)' ' };
            byte[] datatxt = new byte[] { (byte)'d', (byte)'a', (byte)'t', (byte)'a' };

            _out.Write(rifftxt);
            totaloffset = _out.BaseStream.Position;

            _out.Write(tmp32); // total size
            _out.Write(wavetxt);
            _out.Write(fmttxt);

            tmp32 = 16;
            _out.Write(tmp32); // format length

            tmp16 = (ushort)wavformat;
            _out.Write(tmp16); // format

            tmp16 = (ushort)channels;
            _out.Write(tmp16); // channels

            tmp32 = (uint)rate;
            _out.Write(tmp32); // sample rate

            tmp32 = (uint) (rate * bitspersample / 8 * (int)channels);
            _out.Write(tmp32); // bytes / second

            tmp16 = (ushort)(bitspersample / 8 * (int)channels); // float 16 or signed int 16
            _out.Write(tmp16); // block align

            tmp16 = (ushort)bitspersample;
            _out.Write(tmp16); // bits per sample

            _out.Write(datatxt);

            tmp32 = 0;
            dataoffset = _out.BaseStream.Position;

            _out.Write(tmp32); // data length

        }

        // rewrite wav header with final length infos
        static void closewav()
        {
            uint tmp32 = 0;
            // ushort tmp16 = 0;

            int total = (int)_out.BaseStream.Position;

            _out.Seek((int)totaloffset, SeekOrigin.Begin);
            tmp32 = (uint)(total - (totaloffset + 4));

            _out.Write(tmp32);

            _out.Seek((int)dataoffset, SeekOrigin.Begin);

            tmp32 = (uint)(total - (dataoffset + 4));

            _out.Write(tmp32);
        }

        // determine correct wav format and bits per sample
        // from mpg123 enc value
        static void initwavformat()
        {
            if ((enc & mpg123clr.mpg.enc.enc_float_64) != 0)
            {
                bitspersample = 64;
                wavformat = WAVE_FORMAT_IEEE_FLOAT;
            }
            else if ((enc & mpg123clr.mpg.enc.enc_float_32) != 0)
            {
                bitspersample = 32;
                wavformat = WAVE_FORMAT_IEEE_FLOAT;
            }
            else if ((enc & mpg123clr.mpg.enc.enc_16) != 0)
            {
                bitspersample = 16;
                wavformat = WAVE_FORMAT_PCM;
            }
            else
            {
                bitspersample = 8;
                wavformat = WAVE_FORMAT_PCM;
            }
        }

        static void Main(string[] args)
        {
            const long INBUFF = 16384 * 2 * 2;

            int ret;
            mpg123clr.mpg.ErrorCode state;
            long inoffset,inc = 0;
            long outc = 0;
            byte[] buf = new byte[INBUFF];

            if (args.Length < 2)
            {
                Console.WriteLine("Please supply in and out filenames\n");
                Console.WriteLine("Press any key to exit:");
                while (Console.Read() == 0) ;

                return;
            }

            mpg123clr.mpg.ErrorCode err;

            err = mpg123.mpg123_init();

            mpg123 mp = new mpg123();
            err = mp.mpg123_new();

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Unable to create mpg123 handle: " + mpg123error.mpg123_plain_strerror(err));
                Console.WriteLine("Press any key to exit:");
                while (Console.Read() == 0) ;

                return;
            }

            mp.mpg123_param(mpg123clr.mpg.parms.verbose, 4, 0);

            err = mp.mpg123_param(mpg123clr.mpg.parms.flags, 
                (int) (mpg123clr.mpg.param_flags.fuzzy | 
                mpg123clr.mpg.param_flags.seekbuffer | 
                mpg123clr.mpg.param_flags.gapless), 0);

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Unable to set library options: " + mp.mpg123_strerror());
                Console.WriteLine("Press any key to exit:");
                while (Console.Read() == 0) ;

                return;
            }

            // Let the seek index auto-grow and contain an entry for every frame
            err = mp.mpg123_param(mpg123clr.mpg.parms.index_size, -1, 0);

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Unable to set index size: " + mp.mpg123_strerror());
                Console.WriteLine("Press any key to exit:");
                while (Console.Read() == 0) ;

                return;
            }

            // Use float output formats only
            err = mp.mpg123_format_none();

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Unable to disable all output formats: " + mp.mpg123_strerror());
                Console.WriteLine("Press any key to exit:");
                while (Console.Read() == 0) ;

                return;
            }

            int[] rates = mp.mpg123_rates();
            foreach (int rate in rates)
            {
                err = mp.mpg123_format(rate, mpg123clr.mpg.channelcount.both, mpg123clr.mpg.enc.enc_float_32);

                if (err != mpg123clr.mpg.ErrorCode.ok)
                {
                    Console.WriteLine("Unable to set float output formats: " + mp.mpg123_strerror());
                    Console.WriteLine("Press any key to exit:");
                    while (Console.Read() == 0) ;

                    return;
                }
            }

            err = mp.mpg123_open_feed();

            if (err != mpg123clr.mpg.ErrorCode.ok)
            {
                Console.WriteLine("Unable to open feed: " + mp.mpg123_strerror());
                Console.WriteLine("Press any key to exit:");
                while (Console.Read() == 0) ;

                return;
            }

            string filename = args[0];
            BinaryReader _in = new BinaryReader(File.Open(filename, FileMode.Open));

            _out = new BinaryWriter(File.Open(args[1], FileMode.Create));

            while ((ret = (int)(mp.mpg123_feedseek(95000, SeekOrigin.Begin, out inoffset))) == (int)mpg123clr.mpg.ErrorCode.need_more) // equiv to mpg123_feedseek
            {
                buf = _in.ReadBytes((int)INBUFF);

                if (buf.Length <= 0) break;

                inc += buf.Length;

                state = mp.mpg123_feed(buf, (uint)buf.Length); 

                if (state == mpg123clr.mpg.ErrorCode.err)
                {
                    Console.WriteLine("Feed error: " + mp.mpg123_strerror());
                    Console.WriteLine("Press any key to exit:");
                    while (Console.Read() == 0) ;

                    return;
                }
            }

            _in.BaseStream.Seek(inoffset, SeekOrigin.Begin);

            while (true)
            {
                buf = _in.ReadBytes((int)INBUFF);
                if (buf.Length <= 0) break;

                inc += buf.Length;

                err = mp.mpg123_feed(buf, (uint)buf.Length); 

                int num;
                uint bytes;
                IntPtr audio;

                while (err != mpg123clr.mpg.ErrorCode.err && err != mpg123clr.mpg.ErrorCode.need_more)
                {
                    err = mp.mpg123_decode_frame(out num, out audio, out bytes); 

                    if (err == mpg123clr.mpg.ErrorCode.new_format)
                    {
                        mp.mpg123_getformat(out rate, out channels, out enc); 

                        initwavformat();
                        initwav();
                    }

                    // (Surprisingly?) even though it does a Marshal.Copy it's as efficient as the pointer example below!!!
                    if (bytes > 0)
                    {
                        byte[] outbuf = new byte[bytes];
                        Marshal.Copy(audio, outbuf, 0, (int)bytes);

                        _out.Write(outbuf, 0, (int)bytes);
                    }

                    // Alternative example of direct usage of audio data via pointers - note it needs "unsafe"
                    //  and I'm fairly sure pointers should be "fixed" first
                    // if (bytes > 0)
                    // unsafe{
                    //        byte* p = (byte*)audio;
                    //        for (int ii = 0; ii < bytes; ii++)
                    //            _out.Write(*p++);
                    // }

                    outc += bytes;
                }

                if (err == mpg123clr.mpg.ErrorCode.err)
                {
                    Console.WriteLine("Error: " + mp.mpg123_strerror());
                    break;
                }

            }

            Console.WriteLine("Finished");

            closewav();

            _out.Close();
            _in.Close();

            mp.mpg123_delete();
            mp.Dispose();

            mpg123.mpg123_exit();

        }
    }
}
