unit mpg123_;
interface

uses
  Windows,
  SysUtils;
  
  // INITIAL CONVERSION BY ARTHUR 17/02/2008
  // USE http://www.mpg123.de/api/ FOR DOCUMENTATION SINCE I STRIPED ALL THE COMMENTS FROM THE UNIT

type
  PDWord = ^DWORD;
  PPbyte = ^PByte;
  PPchar = ^PChar;
  Pdouble = ^double;
  PPlongint = ^Plongint;
  off_t = Longint;
  Poff_t = ^off_t;
  PPoff_T = ^Poff_t;
  size_t = Cardinal;
  Psize_t = ^size_t;

type
  Pmpg123_handle_struct = ^Tmpg123_handle_struct;
  Tmpg123_handle_struct = packed record
    {undefined structure}
  end;

  Tmpg123_handle = Tmpg123_handle_struct;
  Pmpg123_handle = ^Tmpg123_handle;

var
  mpg123_init: function: Longint; cdecl;

  mpg123_exit: procedure; cdecl;

  mpg123_new: function(decoder: PChar; Error: Plongint): Pmpg123_handle; cdecl;

  mpg123_delete: procedure(mh: Pmpg123_handle); cdecl;

type
  Tmpg123_parms = Longint;
const
  MPG123_VERBOSE = 0;
  MPG123_FLAGS = 1;
  MPG123_ADD_FLAGS = 2;
  MPG123_FORCE_RATE = 3;
  MPG123_DOWN_SAMPLE = 4;
  MPG123_RVA = 5;
  MPG123_DOWNSPEED = 6;
  MPG123_UPSPEED = 7;
  MPG123_START_FRAME = 8;
  MPG123_DECODE_FRAMES = 9;
  MPG123_ICY_INTERVAL = 10;
  MPG123_OUTSCALE = 11;
  MPG123_TIMEOUT = 12;
  MPG123_REMOVE_FLAGS = 13;
  MPG123_RESYNC_LIMIT = 14;

type
  Tmpg123_param_flags = Longint;
const
  MPG123_FORCE_MONO = $7;
  MPG123_MONO_LEFT = $1;
  MPG123_MONO_RIGHT = $2;
  MPG123_MONO_MIX = $4;
  MPG123_FORCE_STEREO = $8;
  MPG123_FORCE_8BIT = $10;
  MPG123_QUIET = $20;
  MPG123_GAPLESS = $40;
  MPG123_NO_RESYNC = $80;

type
  Tmpg123_param_rva = Longint;
const
  MPG123_RVA_OFF = 0;
  MPG123_RVA_MIX = 1;
  MPG123_RVA_ALBUM = 2;
  MPG123_RVA_MAX = MPG123_RVA_ALBUM;

var
  mpg123_param: function(mh: Pmpg123_handle; _type: Tmpg123_parms; value: Longint; fvalue: double): Longint; cdecl;

  mpg123_getparam: function(mh: Pmpg123_handle; _type: Tmpg123_parms; val: Plongint; fval: Pdouble): Longint; cdecl;

type
  Tmpg123_errors = Longint;
const
  MPG123_DONE = -(12);
  MPG123_NEW_FORMAT = -(11);
  MPG123_NEED_MORE = -(10);
  MPG123_ERR = -(1);
  MPG123_OK = 0;
  MPG123_BAD_OUTFORMAT = 1;
  MPG123_BAD_CHANNEL = 2;
  MPG123_BAD_RATE = 3;
  MPG123_ERR_16TO8TABLE = 4;
  MPG123_BAD_PARAM = 5;
  MPG123_BAD_BUFFER = 6;
  MPG123_OUT_OF_MEM = 7;
  MPG123_NOT_INITIALIZED = 8;
  MPG123_BAD_DECODER = 9;
  MPG123_BAD_HANDLE = 10;
  MPG123_NO_BUFFERS = 11;
  MPG123_BAD_RVA = 12;
  MPG123_NO_GAPLESS = 13;
  MPG123_NO_SPACE = 14;
  MPG123_BAD_TYPES = 15;
  MPG123_BAD_BAND = 16;
  MPG123_ERR_NULL = 17;
  MPG123_ERR_READER = 18;
  MPG123_NO_SEEK_FROM_END = 19;
  MPG123_BAD_WHENCE = 20;
  MPG123_NO_TIMEOUT = 21;
  MPG123_BAD_FILE = 22;
  MPG123_NO_SEEK = 23;
  MPG123_NO_READER = 24;
  MPG123_BAD_PARS = 25;
  MPG123_BAD_INDEX_PAR = 26;
  MPG123_OUT_OF_SYNC = 27;
  MPG123_RESYNC_FAIL = 28;

var
  mpg123_plain_strerror: function(errcode: Longint): PChar; cdecl;

  mpg123_strerror: function(mh: Pmpg123_handle): PChar; cdecl;

  mpg123_errcode: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_decoders: function: PPchar; cdecl;

  mpg123_supported_decoders: function: PPchar; cdecl;

  mpg123_decoder: function(mh: Pmpg123_handle; decoder_name: PChar): Longint; cdecl;

type
  Tmpg123_enc_enum = Longint;
const
  MPG123_ENC_16 = $40;
  MPG123_ENC_SIGNED = $80;
  MPG123_ENC_8 = $0F;
  MPG123_ENC_SIGNED_16 = (MPG123_ENC_16 or MPG123_ENC_SIGNED) or $10;
  MPG123_ENC_UNSIGNED_16 = MPG123_ENC_16 or $20;
  MPG123_ENC_UNSIGNED_8 = $01;
  MPG123_ENC_SIGNED_8 = MPG123_ENC_SIGNED or $02;
  MPG123_ENC_ULAW_8 = $04;
  MPG123_ENC_ALAW_8 = $08;
  MPG123_ENC_ANY = ((((MPG123_ENC_SIGNED_16 or MPG123_ENC_UNSIGNED_16) or MPG123_ENC_UNSIGNED_8) or MPG123_ENC_SIGNED_8) or MPG123_ENC_ULAW_8) or MPG123_ENC_ALAW_8;

type
  Tmpg123_channelcount = Longint;
const
  MPG123_MONO = 1;
  MPG123_STEREO = 2;

var
  mpg123_rates: procedure(list: PPlongint; number: Psize_t); cdecl;

  mpg123_encodings: procedure(list: PPlongint; number: Psize_t); cdecl;

  mpg123_format_none: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_format_all: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_format: function(mh: Pmpg123_handle; rate: Longint; channels: Longint; encodings: Longint): Longint; cdecl;

  mpg123_format_support: function(mh: Pmpg123_handle; rate: Longint; encoding: Longint): Longint; cdecl;

  mpg123_getformat: function(mh: Pmpg123_handle; rate: Plongint; channels: Plongint; encoding: Plongint): Longint; cdecl;

  mpg123_open: function(mh: Pmpg123_handle; path: PChar): Longint; cdecl;

  mpg123_open_fd: function(mh: Pmpg123_handle; fd: Longint): Longint; cdecl;

  mpg123_open_feed: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_close: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_read: function(mh: Pmpg123_handle; outmemory: PByte; outmemsize: size_t; done: Psize_t): Longint; cdecl;

  mpg123_decode: function(mh: Pmpg123_handle; inmemory: PByte; inmemsize: size_t; outmemory: PByte; outmemsize: size_t;
    done: Psize_t): Longint; cdecl;

  mpg123_decode_frame: function(mh: Pmpg123_handle; num: Poff_t; audio: PPbyte; bytes: Psize_t): Longint; cdecl;

  mpg123_tell: function(mh: Pmpg123_handle): off_t; cdecl;

  mpg123_tellframe: function(mh: Pmpg123_handle): off_t; cdecl;

  mpg123_seek: function(mh: Pmpg123_handle; sampleoff: off_t; whence: Longint): off_t; cdecl;

  mpg123_feedseek: function(mh: Pmpg123_handle; sampleoff: off_t; whence: Longint; input_offset: Poff_t): off_t; cdecl;

  mpg123_seek_frame: function(mh: Pmpg123_handle; frameoff: off_t; whence: Longint): off_t; cdecl;

  mpg123_timeframe: function(mh: Pmpg123_handle; sec: double): off_t; cdecl;

  mpg123_index: function(mh: Pmpg123_handle; offsets: PPoff_t; step: Poff_t; fill: Psize_t): Longint; cdecl;

  mpg123_position: function(mh: Pmpg123_handle; frame_offset: off_t; buffered_bytes: off_t; current_frame: Poff_t; frames_left: Poff_t;
    current_seconds: Pdouble; seconds_left: Pdouble): Longint; cdecl;

type
  Tmpg123_channels = Longint;
const
  MPG123_LEFT = $1;
  MPG123_RIGHT = $2;

var
  mpg123_eq: function(mh: Pmpg123_handle; channel: Tmpg123_channels; band: Longint; val: double): Longint; cdecl;

  mpg123_reset_eq: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_volume: function(mh: Pmpg123_handle; vol: double): Longint; cdecl;

  mpg123_volume_change: function(mh: Pmpg123_handle; change: double): Longint; cdecl;

  mpg123_getvolume: function(mh: Pmpg123_handle; base: Pdouble; really: Pdouble; rva_db: Pdouble): Longint; cdecl;

type
  Tmpg123_vbr = Longint;
const
  MPG123_CBR = 0;
  MPG123_VBR = 1;
  MPG123_ABR = 2;

type
  Tmpg123_version = Longint;
const
  MPG123_1_0 = 0;
  MPG123_2_0 = 1;
  MPG123_2_5 = 2;

type
  Tmpg123_mode = Longint;
const
  MPG123_M_STEREO = 0;
  MPG123_M_JOINT = 1;
  MPG123_M_DUAL = 2;
  MPG123_M_MONO = 3;

type
  Tmpg123_flags = Longint;
const
  MPG123_CRC = $1;
  MPG123_COPYRIGHT = $2;
  MPG123_PRIVATE = $4;
  MPG123_ORIGINAL = $8;

type
  Pmpg123_frameinfo = ^Tmpg123_frameinfo;
  Tmpg123_frameinfo = packed record
    version: Tmpg123_version;
    layer: Longint;
    rate: Longint;
    mode: Tmpg123_mode;
    mode_ext: Longint;
    framesize: Longint;
    Flags: Tmpg123_flags;
    emphasis: Longint;
    bitrate: Longint;
    abr_rate: Longint;
    vbr: Tmpg123_vbr;
  end;

var
  mpg123_info: function(mh: Pmpg123_handle; mi: Pmpg123_frameinfo): Longint; cdecl;

  mpg123_safe_buffer: function: size_t; cdecl;

  mpg123_scan: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_length: function(mh: Pmpg123_handle): off_t; cdecl;

  mpg123_tpf: function(mh: Pmpg123_handle): double; cdecl;

  mpg123_clip: function(mh: Pmpg123_handle): Longint; cdecl;

type

  Pmpg123_string = ^Tmpg123_string;
  Tmpg123_string = packed record
    p: PChar;
    Size: size_t;
    fill: size_t;
  end;

var
  mpg123_init_string: procedure(sb: Pmpg123_string); cdecl;

  mpg123_free_string: procedure(sb: Pmpg123_string); cdecl;

  mpg123_resize_string: function(sb: Pmpg123_string; news: size_t): Longint; cdecl;

  mpg123_copy_string: function(from: Pmpg123_string; _to: Pmpg123_string): Longint; cdecl;

  mpg123_add_string: function(sb: Pmpg123_string; stuff: PChar): Longint; cdecl;

  mpg123_set_string: function(sb: Pmpg123_string; stuff: PChar): Longint; cdecl;

type

  Pmpg123_text = ^Tmpg123_text;
  Tmpg123_text = packed record
    lang: array[0..2] of Char;
    id: array[0..3] of Char;
    description: Tmpg123_string;
    Text: Tmpg123_string;
  end;

  PPmpg123_id3v2 = ^Pmpg123_id3v2;
  Pmpg123_id3v2 = ^Tmpg123_id3v2;
  Tmpg123_id3v2 = packed record
    version: Byte;
    title: Pmpg123_string;
    artist: Pmpg123_string;
    album: Pmpg123_string;
    year: Pmpg123_string;
    genre: Pmpg123_string;
    comment: Pmpg123_string;
    comment_list: Pmpg123_text;
    comments: size_t;
    Text: Pmpg123_text;
    texts: size_t;
    extra: Pmpg123_text;
    extras: size_t;
  end;

  PPmpg123_id3v1 = ^Pmpg123_id3v1;
  Pmpg123_id3v1 = ^Tmpg123_id3v1;
  Tmpg123_id3v1 = packed record
    tag: array[0..2] of Char;
    title: array[0..29] of Char;
    artist: array[0..29] of Char;
    album: array[0..29] of Char;
    year: array[0..3] of Char;
    comment: array[0..29] of Char;
    genre: Byte;
  end;

const
  MPG123_ID3 = $3;

  MPG123_NEW_ID3 = $1;

  MPG123_ICY = $C;

  MPG123_NEW_ICY = $4;

var
  mpg123_meta_check: function(mh: Pmpg123_handle): Longint; cdecl;

  mpg123_id3_: function(mh: Pmpg123_handle; var v1: Pmpg123_id3v1; var v2: Pmpg123_id3v2): Longint; cdecl;

  mpg123_icy_: function(mh: Pmpg123_handle; icy_meta: PPchar): Longint; cdecl;

type
  Pmpg123_pars_struct = ^Tmpg123_pars_struct;
  Tmpg123_pars_struct = packed record
    {undefined structure}
  end;

  Tmpg123_pars = Tmpg123_pars_struct;
  Pmpg123_pars = ^Tmpg123_pars;

var
  mpg123_parnew: function(mp: Pmpg123_pars; decoder: PChar; Error: Plongint): Pmpg123_handle; cdecl;

  mpg123_new_pars: function(Error: Plongint): Pmpg123_pars; cdecl;

  mpg123_delete_pars: procedure(mp: Pmpg123_pars); cdecl;

  mpg123_fmt_none: function(mp: Pmpg123_pars): Longint; cdecl;

  mpg123_fmt_all: function(mp: Pmpg123_pars): Longint; cdecl;

  mpg123_fmt: function(mh: Pmpg123_pars; rate: Longint; channels: Longint; encodings: Longint): Longint; cdecl;

  mpg123_fmt_support: function(mh: Pmpg123_pars; rate: Longint; encoding: Longint): Longint; cdecl;

  mpg123_par: function(mp: Pmpg123_pars; _type: Tmpg123_parms; value: Longint; fvalue: double): Longint; cdecl;

  mpg123_getpar: function(mp: Pmpg123_pars; _type: Tmpg123_parms; val: Plongint; fval: Pdouble): Longint; cdecl;

  mpg123_replace_buffer: function(mh: Pmpg123_handle; data: PByte; Size: size_t): Longint; cdecl;

  mpg123_outblock: function(mh: Pmpg123_handle): size_t; cdecl;

  //mpg123_replace_reader : function(mh:Pmpg123_handle; r_read:function (_para1:longint; _para2:pointer; _para3:size_t):Tssize_t; r_lseek:function (_para1:longint; _para2:off_t; _para3:longint):off_t):longint;

implementation

var
  hlib: THandle;

procedure Freempg123;
begin
  FreeLibrary(hlib);
end;

procedure Loadmpg123(lib: PChar);
begin
  hlib := LoadLibrary(lib);
  mpg123_init := GetProcAddress(hlib, 'mpg123_init');
  mpg123_exit := GetProcAddress(hlib, 'mpg123_exit');
  mpg123_new := GetProcAddress(hlib, 'mpg123_new');
  mpg123_delete := GetProcAddress(hlib, 'mpg123_delete');
  mpg123_param := GetProcAddress(hlib, 'mpg123_param');
  mpg123_getparam := GetProcAddress(hlib, 'mpg123_getparam');
  mpg123_plain_strerror := GetProcAddress(hlib, 'mpg123_plain_strerror');
  mpg123_strerror := GetProcAddress(hlib, 'mpg123_strerror');
  mpg123_errcode := GetProcAddress(hlib, 'mpg123_errcode');
  mpg123_decoders := GetProcAddress(hlib, 'mpg123_decoders');
  mpg123_supported_decoders := GetProcAddress(hlib, 'mpg123_supported_decoders');
  mpg123_decoder := GetProcAddress(hlib, 'mpg123_decoder');
  mpg123_rates := GetProcAddress(hlib, 'mpg123_rates');
  mpg123_encodings := GetProcAddress(hlib, 'mpg123_encodings');
  mpg123_format_none := GetProcAddress(hlib, 'mpg123_format_none');
  mpg123_format_all := GetProcAddress(hlib, 'mpg123_format_all');
  mpg123_format := GetProcAddress(hlib, 'mpg123_format');
  mpg123_format_support := GetProcAddress(hlib, 'mpg123_format_support');
  mpg123_getformat := GetProcAddress(hlib, 'mpg123_getformat');
  mpg123_open := GetProcAddress(hlib, 'mpg123_open');
  mpg123_open_fd := GetProcAddress(hlib, 'mpg123_open_fd');
  mpg123_open_feed := GetProcAddress(hlib, 'mpg123_open_feed');
  mpg123_close := GetProcAddress(hlib, 'mpg123_close');
  mpg123_read := GetProcAddress(hlib, 'mpg123_read');
  mpg123_decode := GetProcAddress(hlib, 'mpg123_decode');
  mpg123_decode_frame := GetProcAddress(hlib, 'mpg123_decode_frame');
  mpg123_tell := GetProcAddress(hlib, 'mpg123_tell');
  mpg123_tellframe := GetProcAddress(hlib, 'mpg123_tellframe');
  mpg123_seek := GetProcAddress(hlib, 'mpg123_seek');
  mpg123_feedseek := GetProcAddress(hlib, 'mpg123_feedseek');
  mpg123_seek_frame := GetProcAddress(hlib, 'mpg123_seek_frame');
  mpg123_timeframe := GetProcAddress(hlib, 'mpg123_timeframe');
  mpg123_index := GetProcAddress(hlib, 'mpg123_index');
  mpg123_position := GetProcAddress(hlib, 'mpg123_position');
  mpg123_eq := GetProcAddress(hlib, 'mpg123_eq');
  mpg123_reset_eq := GetProcAddress(hlib, 'mpg123_reset_eq');
  mpg123_volume := GetProcAddress(hlib, 'mpg123_volume');
  mpg123_volume_change := GetProcAddress(hlib, 'mpg123_volume_change');
  mpg123_getvolume := GetProcAddress(hlib, 'mpg123_getvolume');
  mpg123_info := GetProcAddress(hlib, 'mpg123_info');
  mpg123_safe_buffer := GetProcAddress(hlib, 'mpg123_safe_buffer');
  mpg123_scan := GetProcAddress(hlib, 'mpg123_scan');
  mpg123_length := GetProcAddress(hlib, 'mpg123_length');
  mpg123_tpf := GetProcAddress(hlib, 'mpg123_tpf');
  mpg123_clip := GetProcAddress(hlib, 'mpg123_clip');
  mpg123_init_string := GetProcAddress(hlib, 'mpg123_init_string');
  mpg123_free_string := GetProcAddress(hlib, 'mpg123_free_string');
  mpg123_resize_string := GetProcAddress(hlib, 'mpg123_resize_string');
  mpg123_copy_string := GetProcAddress(hlib, 'mpg123_copy_string');
  mpg123_add_string := GetProcAddress(hlib, 'mpg123_add_string');
  mpg123_set_string := GetProcAddress(hlib, 'mpg123_set_string');
  mpg123_meta_check := GetProcAddress(hlib, 'mpg123_meta_check');
  mpg123_id3_ := GetProcAddress(hlib, 'mpg123_id3');
  mpg123_icy_ := GetProcAddress(hlib, 'mpg123_icy');
  mpg123_parnew := GetProcAddress(hlib, 'mpg123_parnew');
  mpg123_new_pars := GetProcAddress(hlib, 'mpg123_new_pars');
  mpg123_delete_pars := GetProcAddress(hlib, 'mpg123_delete_pars');
  mpg123_fmt_none := GetProcAddress(hlib, 'mpg123_fmt_none');
  mpg123_fmt_all := GetProcAddress(hlib, 'mpg123_fmt_all');
  mpg123_fmt := GetProcAddress(hlib, 'mpg123_fmt');
  mpg123_fmt_support := GetProcAddress(hlib, 'mpg123_fmt_support');
  mpg123_par := GetProcAddress(hlib, 'mpg123_par');
  mpg123_getpar := GetProcAddress(hlib, 'mpg123_getpar');
  mpg123_replace_buffer := GetProcAddress(hlib, 'mpg123_replace_buffer');
  mpg123_outblock := GetProcAddress(hlib, 'mpg123_outblock');
  //mpg123_replace_reader := GetProcAddress(hlib,'mpg123_replace_reader');
end;

initialization
  Loadmpg123('libmpg123-0.dll');
finalization
  Freempg123;

end.

