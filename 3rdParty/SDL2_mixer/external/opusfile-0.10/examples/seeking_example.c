/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE libopusfile SOFTWARE CODEC SOURCE CODE. *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE libopusfile SOURCE CODE IS (C) COPYRIGHT 1994-2012           *
 * by the Xiph.Org Foundation and contributors http://www.xiph.org/ *
 *                                                                  *
 ********************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*For fileno()*/
#if !defined(_POSIX_SOURCE)
# define _POSIX_SOURCE 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <opusfile.h>
#if defined(_WIN32)
# include "win32utf8.h"
# undef fileno
# define fileno _fileno
#endif

/*Use shorts, they're smaller.*/
#if !defined(OP_FIXED_POINT)
# define OP_FIXED_POINT (1)
#endif

#if defined(OP_FIXED_POINT)

typedef opus_int16 op_sample;

# define op_read_native op_read

/*TODO: The convergence after 80 ms of preroll is far from exact.
  Our comparison is very rough.
  Need to find some way to do this better.*/
# define MATCH_TOL (16384)

# define ABS(_x) ((_x)<0?-(_x):(_x))

# define MATCH(_a,_b) (ABS((_a)-(_b))<MATCH_TOL)

/*Don't have fixed-point downmixing code.*/
# undef OP_WRITE_SEEK_SAMPLES

#else

typedef float op_sample;

# define op_read_native op_read_float

/*TODO: The convergence after 80 ms of preroll is far from exact.
  Our comparison is very rough.
  Need to find some way to do this better.*/
# define MATCH_TOL (16384.0/32768)

# define FABS(_x) ((_x)<0?-(_x):(_x))

# define MATCH(_a,_b) (FABS((_a)-(_b))<MATCH_TOL)

# if defined(OP_WRITE_SEEK_SAMPLES)
/*Matrices for downmixing from the supported channel counts to stereo.*/
static const float DOWNMIX_MATRIX[8][8][2]={
  /*mono*/
  {
    {1.F,1.F}
  },
  /*stereo*/
  {
    {1.F,0.F},{0.F,1.F}
  },
  /*3.0*/
  {
    {0.5858F,0.F},{0.4142F,0.4142F},{0,0.5858F}
  },
  /*quadrophonic*/
  {
    {0.4226F,0.F},{0,0.4226F},{0.366F,0.2114F},{0.2114F,0.336F}
  },
  /*5.0*/
  {
    {0.651F,0.F},{0.46F,0.46F},{0,0.651F},{0.5636F,0.3254F},{0.3254F,0.5636F}
  },
  /*5.1*/
  {
    {0.529F,0.F},{0.3741F,0.3741F},{0.F,0.529F},{0.4582F,0.2645F},
    {0.2645F,0.4582F},{0.3741F,0.3741F}
  },
  /*6.1*/
  {
    {0.4553F,0.F},{0.322F,0.322F},{0.F,0.4553F},{0.3943F,0.2277F},
    {0.2277F,0.3943F},{0.2788F,0.2788F},{0.322F,0.322F}
  },
  /*7.1*/
  {
    {0.3886F,0.F},{0.2748F,0.2748F},{0.F,0.3886F},{0.3366F,0.1943F},
    {0.1943F,0.3366F},{0.3366F,0.1943F},{0.1943F,0.3366F},{0.2748F,0.2748F}
  }
};

static void write_samples(float *_samples,int _nsamples,int _nchannels){
  float stereo_pcm[120*48*2];
  int   i;
  for(i=0;i<_nsamples;i++){
    float l;
    float r;
    int   ci;
    l=r=0.F;
    for(ci=0;ci<_nchannels;ci++){
      l+=DOWNMIX_MATRIX[_nchannels-1][ci][0]*_samples[i*_nchannels+ci];
      r+=DOWNMIX_MATRIX[_nchannels-1][ci][1]*_samples[i*_nchannels+ci];
    }
    stereo_pcm[2*i+0]=l;
    stereo_pcm[2*i+1]=r;
  }
  fwrite(stereo_pcm,sizeof(*stereo_pcm)*2,_nsamples,stdout);
}
# endif

#endif

static long nfailures;

static void verify_seek(OggOpusFile *_of,opus_int64 _byte_offset,
 ogg_int64_t _pcm_offset,ogg_int64_t _pcm_length,op_sample *_bigassbuffer){
  opus_int64  byte_offset;
  ogg_int64_t pcm_offset;
  ogg_int64_t duration;
  op_sample   buffer[120*48*8];
  int         nchannels;
  int         nsamples;
  int         li;
  int         lj;
  int         i;
  byte_offset=op_raw_tell(_of);
  if(_byte_offset!=-1&&byte_offset<_byte_offset){
    fprintf(stderr,"\nRaw position out of tolerance: requested %li, "
     "got %li.\n",(long)_byte_offset,(long)byte_offset);
    nfailures++;
  }
  pcm_offset=op_pcm_tell(_of);
  if(_pcm_offset!=-1&&pcm_offset>_pcm_offset){
    fprintf(stderr,"\nPCM position out of tolerance: requested %li, "
     "got %li.\n",(long)_pcm_offset,(long)pcm_offset);
    nfailures++;
  }
  if(pcm_offset<0||pcm_offset>_pcm_length){
    fprintf(stderr,"\nPCM position out of bounds: got %li.\n",
     (long)pcm_offset);
    nfailures++;
  }
  nsamples=op_read_native(_of,buffer,sizeof(buffer)/sizeof(*buffer),&li);
  if(nsamples<0){
    fprintf(stderr,"\nFailed to read PCM data after seek: %i\n",nsamples);
    nfailures++;
    li=op_current_link(_of);
  }
  for(lj=0;lj<li;lj++){
    duration=op_pcm_total(_of,lj);
    if(0<=pcm_offset&&pcm_offset<duration){
      fprintf(stderr,"\nPCM data after seek came from the wrong link: "
       "expected %i, got %i.\n",lj,li);
      nfailures++;
    }
    pcm_offset-=duration;
    if(_bigassbuffer!=NULL)_bigassbuffer+=op_channel_count(_of,lj)*duration;
  }
  duration=op_pcm_total(_of,li);
  if(pcm_offset+nsamples>duration){
    fprintf(stderr,"\nPCM data after seek exceeded link duration: "
     "limit %li, got %li.\n",(long)duration,(long)(pcm_offset+nsamples));
    nfailures++;
  }
  nchannels=op_channel_count(_of,li);
  if(_bigassbuffer!=NULL){
    for(i=0;i<nsamples*nchannels;i++){
      if(!MATCH(buffer[i],_bigassbuffer[pcm_offset*nchannels+i])){
        ogg_int64_t j;
        fprintf(stderr,"\nData after seek doesn't match declared PCM "
         "position: mismatch %G\n",
         (double)buffer[i]-_bigassbuffer[pcm_offset*nchannels+i]);
        for(j=0;j<duration-nsamples;j++){
          for(i=0;i<nsamples*nchannels;i++){
            if(!MATCH(buffer[i],_bigassbuffer[j*nchannels+i]))break;
          }
          if(i==nsamples*nchannels){
            fprintf(stderr,"\nData after seek appears to match position %li.\n",
             (long)i);
          }
        }
        nfailures++;
        break;
      }
    }
  }
#if defined(OP_WRITE_SEEK_SAMPLES)
  write_samples(buffer,nsamples,nchannels);
#endif
}

#define OP_MIN(_a,_b) ((_a)<(_b)?(_a):(_b))

/*A simple wrapper that lets us count the number of underlying seek calls.*/

static op_seek_func real_seek;

static long nreal_seeks;

static int seek_stat_counter(void *_stream,opus_int64 _offset,int _whence){
  if(_whence==SEEK_SET)nreal_seeks++;
  /*SEEK_CUR with an offset of 0 is free, as is SEEK_END with an offset of 0
     (assuming we know the file size), so don't count them.*/
  else if(_offset!=0)nreal_seeks++;
  return (*real_seek)(_stream,_offset,_whence);
}

#define NSEEK_TESTS (1000)

static void print_duration(FILE *_fp,ogg_int64_t _nsamples){
  ogg_int64_t seconds;
  ogg_int64_t minutes;
  ogg_int64_t hours;
  ogg_int64_t days;
  ogg_int64_t weeks;
  seconds=_nsamples/48000;
  _nsamples-=seconds*48000;
  minutes=seconds/60;
  seconds-=minutes*60;
  hours=minutes/60;
  minutes-=hours*60;
  days=hours/24;
  hours-=days*24;
  weeks=days/7;
  days-=weeks*7;
  if(weeks)fprintf(_fp,"%liw",(long)weeks);
  if(weeks||days)fprintf(_fp,"%id",(int)days);
  if(weeks||days||hours){
    if(weeks||days)fprintf(_fp,"%02ih",(int)hours);
    else fprintf(_fp,"%ih",(int)hours);
  }
  if(weeks||days||hours||minutes){
    if(weeks||days||hours)fprintf(_fp,"%02im",(int)minutes);
    else fprintf(_fp,"%im",(int)minutes);
    fprintf(_fp,"%02i",(int)seconds);
  }
  else fprintf(_fp,"%i",(int)seconds);
  fprintf(_fp,".%03is",(int)(_nsamples+24)/48);
}

int main(int _argc,const char **_argv){
  OpusFileCallbacks  cb;
  OggOpusFile       *of;
  void              *fp;
#if defined(_WIN32)
  win32_utf8_setup(&_argc,&_argv);
#endif
  if(_argc!=2){
    fprintf(stderr,"Usage: %s <file.opus>\n",_argv[0]);
    return EXIT_FAILURE;
  }
  memset(&cb,0,sizeof(cb));
  if(strcmp(_argv[1],"-")==0)fp=op_fdopen(&cb,fileno(stdin),"rb");
  else{
    /*Try to treat the argument as a URL.*/
    fp=op_url_stream_create(&cb,_argv[1],
     OP_SSL_SKIP_CERTIFICATE_CHECK(1),NULL);
    /*Fall back assuming it's a regular file name.*/
    if(fp==NULL)fp=op_fopen(&cb,_argv[1],"rb");
  }
  if(cb.seek!=NULL){
    real_seek=cb.seek;
    cb.seek=seek_stat_counter;
  }
  of=op_open_callbacks(fp,&cb,NULL,0,NULL);
  if(of==NULL){
    fprintf(stderr,"Failed to open file '%s'.\n",_argv[1]);
    return EXIT_FAILURE;
  }
  if(op_seekable(of)){
    op_sample   *bigassbuffer;
    ogg_int64_t  size;
    ogg_int64_t  pcm_offset;
    ogg_int64_t  pcm_length;
    ogg_int64_t  nsamples;
    long         max_seeks;
    int          nlinks;
    int          ret;
    int          li;
    int          i;
    /*Because we want to do sample-level verification that the seek does what
       it claimed, decode the entire file into memory.*/
    nlinks=op_link_count(of);
    fprintf(stderr,"Opened file containing %i links with %li seeks "
     "(%0.3f per link).\n",nlinks,nreal_seeks,nreal_seeks/(double)nlinks);
    /*Reset the seek counter.*/
    nreal_seeks=0;
    nsamples=0;
    for(li=0;li<nlinks;li++){
      nsamples+=op_pcm_total(of,li)*op_channel_count(of,li);
    }
/*Until we find another way to do the comparisons that solves the MATCH_TOL
   problem, disable this.*/
#if 0
    bigassbuffer=_ogg_malloc(sizeof(*bigassbuffer)*nsamples);
    if(bigassbuffer==NULL){
      fprintf(stderr,
       "Buffer allocation failed. Seek offset detection disabled.\n");
    }
#else
    bigassbuffer=NULL;
#endif
    pcm_offset=op_pcm_tell(of);
    if(pcm_offset!=0){
      fprintf(stderr,"Initial PCM offset was not 0, got %li instead.!\n",
       (long)pcm_offset);
      nfailures++;
    }
/*Disabling the linear scan for now.
  Only test on non-borken files!*/
#if 0
    {
      op_sample    smallerbuffer[120*48*8];
      ogg_int64_t  pcm_print_offset;
      ogg_int64_t  si;
      opus_int32   bitrate;
      int          saw_hole;
      pcm_print_offset=pcm_offset-48000;
      bitrate=0;
      saw_hole=0;
      for(si=0;si<nsamples;){
        ogg_int64_t next_pcm_offset;
        opus_int32  next_bitrate;
        op_sample  *buf;
        int         buf_size;
        buf=bigassbuffer==NULL?smallerbuffer:bigassbuffer+si;
        buf_size=(int)OP_MIN(nsamples-si,
         (int)(sizeof(smallerbuffer)/sizeof(*smallerbuffer))),
        ret=op_read_native(of,buf,buf_size,&li);
        if(ret==OP_HOLE){
          /*Only warn once in a row.*/
          if(saw_hole)continue;
          saw_hole=1;
          /*This is just a warning.
            As long as the timestamps are still contiguous we're okay.*/
          fprintf(stderr,"\nHole in PCM data at sample %li\n",
           (long)pcm_offset);
          continue;
        }
        else if(ret<=0){
          fprintf(stderr,"\nFailed to read PCM data: %i\n",ret);
          exit(EXIT_FAILURE);
        }
        saw_hole=0;
        /*If we have gaps in the PCM positions, seeking is not likely to work
           near them.*/
        next_pcm_offset=op_pcm_tell(of);
        if(pcm_offset+ret!=next_pcm_offset){
          fprintf(stderr,"\nGap in PCM offset: expecting %li, got %li\n",
           (long)(pcm_offset+ret),(long)next_pcm_offset);
          nfailures++;
        }
        pcm_offset=next_pcm_offset;
        si+=ret*op_channel_count(of,li);
        if(pcm_offset>=pcm_print_offset+48000){
          next_bitrate=op_bitrate_instant(of);
          if(next_bitrate>=0)bitrate=next_bitrate;
          fprintf(stderr,"\r%s... [%li left] (%0.3f kbps)               ",
           bigassbuffer==NULL?"Scanning":"Loading",nsamples-si,bitrate/1000.0);
          pcm_print_offset=pcm_offset;
        }
      }
      ret=op_read_native(of,smallerbuffer,8,&li);
      if(ret<0){
        fprintf(stderr,"Failed to read PCM data: %i\n",ret);
        nfailures++;
      }
      if(ret>0){
        fprintf(stderr,"Read too much PCM data!\n");
        nfailures++;
      }
    }
#endif
    pcm_length=op_pcm_total(of,-1);
    size=op_raw_total(of,-1);
    fprintf(stderr,"\rLoaded (%0.3f kbps average).                        \n",
     op_bitrate(of,-1)/1000.0);
    fprintf(stderr,"Testing raw seeking to random places in %li bytes...\n",
     (long)size);
    max_seeks=0;
    for(i=0;i<NSEEK_TESTS;i++){
      long nseeks_tmp;
      opus_int64 byte_offset;
      nseeks_tmp=nreal_seeks;
      byte_offset=(opus_int64)(rand()/(double)RAND_MAX*size);
      fprintf(stderr,"\r\t%3i [raw position %li]...                ",
       i,(long)byte_offset);
      ret=op_raw_seek(of,byte_offset);
      if(ret<0){
        fprintf(stderr,"\nSeek failed: %i.\n",ret);
        nfailures++;
      }
      if(i==28){
        i=28;
      }
      verify_seek(of,byte_offset,-1,pcm_length,bigassbuffer);
      nseeks_tmp=nreal_seeks-nseeks_tmp;
      max_seeks=nseeks_tmp>max_seeks?nseeks_tmp:max_seeks;
    }
    fprintf(stderr,"\rTotal seek operations: %li (%.3f per raw seek, %li maximum).\n",
     nreal_seeks,nreal_seeks/(double)NSEEK_TESTS,max_seeks);
    nreal_seeks=0;
    fprintf(stderr,"Testing exact PCM seeking to random places in %li "
     "samples (",(long)pcm_length);
    print_duration(stderr,pcm_length);
    fprintf(stderr,")...\n");
    max_seeks=0;
    for(i=0;i<NSEEK_TESTS;i++){
      ogg_int64_t pcm_offset2;
      long        nseeks_tmp;
      nseeks_tmp=nreal_seeks;
      pcm_offset=(ogg_int64_t)(rand()/(double)RAND_MAX*pcm_length);
      fprintf(stderr,"\r\t%3i [PCM position %li]...                ",
       i,(long)pcm_offset);
      ret=op_pcm_seek(of,pcm_offset);
      if(ret<0){
        fprintf(stderr,"\nSeek failed: %i.\n",ret);
        nfailures++;
      }
      pcm_offset2=op_pcm_tell(of);
      if(pcm_offset!=pcm_offset2){
        fprintf(stderr,"\nDeclared PCM position did not perfectly match "
         "request: requested %li, got %li.\n",
         (long)pcm_offset,(long)pcm_offset2);
        nfailures++;
      }
      verify_seek(of,-1,pcm_offset,pcm_length,bigassbuffer);
      nseeks_tmp=nreal_seeks-nseeks_tmp;
      max_seeks=nseeks_tmp>max_seeks?nseeks_tmp:max_seeks;
    }
    fprintf(stderr,"\rTotal seek operations: %li (%.3f per exact seek, %li maximum).\n",
     nreal_seeks,nreal_seeks/(double)NSEEK_TESTS,max_seeks);
    nreal_seeks=0;
    fprintf(stderr,"OK.\n");
    _ogg_free(bigassbuffer);
  }
  else{
    fprintf(stderr,"Input was not seekable.\n");
    exit(EXIT_FAILURE);
  }
  op_free(of);
  if(nfailures>0){
    fprintf(stderr,"FAILED: %li failure conditions encountered.\n",nfailures);
  }
  return nfailures!=0?EXIT_FAILURE:EXIT_SUCCESS;
}
