// ref: 0x10010306
void __fastcall TitleSnd_SetSoundFunction(void (__stdcall *func)(char *file))
{
	gfnSoundFunction = func;
}

// ref: 0x1001030D
void __cdecl TitleSnd_InitSoundFunc()
{
	gfnSoundFunction = 0;
}

// ref: 0x10010315
void __cdecl TitleSnd_PlayMoveSound()
{
	if ( gfnSoundFunction )
		gfnSoundFunction("sfx\\items\\titlemov.wav");
}

// ref: 0x1001031F
void __cdecl TitleSnd_PlaySelectSound()
{
	if ( gfnSoundFunction )
		gfnSoundFunction("sfx\\items\\titlslct.wav");
}

// ref: 0x1001032E
void __cdecl TitleSnd_cpp_init()
{
	titlesnd_cpp_float = titlesnd_cpp_float_value;
}
// 1001F480: using guessed type int titlesnd_cpp_float_value;
// 1002A590: using guessed type int titlesnd_cpp_float;
