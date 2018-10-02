//rdata
ProfileStruct bnprofiles[4] =
{
  { "profile\\sex", '\x01', 1128, 8 },
  { "profile\\age", '\x01', 1130, 4 },
  { "profile\\location", '\x01', 1132, 40 },
  { "profile\\description", '\x01', 1134, 200 }
};
int profilemsg1[6] = { 1125, 1127, 1129, 1131, 1133, 0 };
int profilemsg2[6] = { 1126, 1128, 1130, 1132, 1134, 0 };
int Connect_cpp_float_value = 2139095040; // weak
int CopyProt_cpp_float_value = 2139095040; // weak
int cr8game_cpp_float_value = 2139095040; // weak
int CreaDung_cpp_float_value = 2139095040; // weak
int CreaStat_cpp_float_value = 2139095040; // weak
int credits_cpp_float_value = 2139095040; // weak
int DiabEdit_cpp_float_value = 2139095040; // weak
int DiabloUI_cpp_float_value = 2139095040; // weak
int disclaim_cpp_float_value = 2139095040; // weak
int doom_cpp_float_value = 2139095040; // weak
int EntName_cpp_float_value = 2139095040; // weak
int fade_cpp_float_value = 2139095040; // weak
int focus_cpp_float_value = 2139095040; // weak
int local_cpp_float_value = 2139095040; // weak
int mainmenu_cpp_float_value = 2139095040; // weak
int OkCancel_cpp_float_value = 2139095040; // weak
int Sbar_cpp_float_value = 2139095040; // weak
int Sbar_cpp_float_value2 = 2139095040; // weak
int SelClass_cpp_float_value = 2139095040; // weak
int SelHero_cpp_float_value = 2139095040; // weak
int SelList_cpp_float_value = 2139095040; // weak
int SelLoad_cpp_float_value = 2139095040; // weak
int SelYesNo_cpp_float_value = 2139095040; // weak
int Title_cpp_float_value = 2139095040; // weak
int titlesnd_cpp_float_value = 2139095040; // weak
int dword_10022258 = 4; // weak
ProfFntStruct proffnts[4] =
{
  { 8, "Arial", 400 },
  { 10, "Arial", 400 },
  { 10, "Arial", 700 },
  { 13, "Time New Roman", 400 }
};
unsigned char connect_subnet_ip[4][4] =
{
  {  13,   0,   0, 0 }, // 13.0.0.0
  { 128, 128, 128, 0 }, // 128.128.128.0
  {  14,   0,   0, 0 }, // 14.0.0.0
  { 255, 255, 255, 0 }  // 255.255.255.0
};
int creadung_msgtbl1[3] = { 1038, 1080, 0 };
int creadung_msgtbl2[2] = { 1097, 0 };
int creadung_msgtbl3[2] = { 1099, 0 };
int creadung_msgtbl4[3] = { 1056, 1054, 0 };
int creadung_msgtbl5[4] = { 1094, 1095, 1096, 0 };
short defstats[3][4] = { { 30, 15, 20, 30 }, { 25, 20, 30, 20 }, { 15, 35, 25, 20 } };
int dword_10022A2C[3] = { 1038, 1080, 0 };
int dword_10022A38[2] = { 1097, 0 };
int dword_10022A40[2] = { 1102, 0 };
int dword_10022A48[3] = { 1056, 1054, 0 };
int dword_10022A54[3] = { 1100, 1101, 0 };
int disclaim_msgtbl1[3] = { 1082, 1083, 0 };
int disclaim_msgtbl2[4] = { 1084, 1085, 1086, 0 };
int dword_10022AFC[2] = { 1038, 0 };
int dword_10022B04[3] = { 1056, 1054, 0 };
int dword_10022B10[2] = { 1116, 0 };
int entname_msgtbl1[2] = { 1038, 0 };
int entname_msgtbl2[3] = { 1056, 1054, 0 };
int entname_msgtbl3[2] = { 1065, 0 };
int menumsgs_1option[2] = { 1042, 0 };
int menumsgs_5options[6] = { 1044, 1001, 1002, 1003, 2, 0 };
int dword_10022C4C[2] = { 1038, 0 };
int dword_10022C54[2] = { 1080, 0 };
int dword_10022C5C[2] = { 1108, 0 };
int dword_10022CAC[2] = { 1026, 0 };
int dword_10022CB4[2] = { 2, 0 };
int selclass_msgtbl1[2] = { 1038, 0 };
int selclass_msgtbl2[3] = { 1056, 1054, 0 };
int selclass_msgtbl3[4] = { 1062, 1063, 1064, 0 };
int dword_10022ED8[3] = { 1038, 1080, 0 };
int dword_10022EE4[3] = { 1143, 1147, 0 };
int dword_10022EF0[4] = { 1081, 1076, 1144, 0 };
int dword_10022F00[2] = { 1075, 0 };
int dword_10022F08[4] = { 1056, 1054, 1145, 0 };
int dword_10022F18[7] = { 1069, 1070, 1071, 1072, 1073, 1074, 0 };
char *off_10022F8C[4] = { "Entry1", "Entry2", "Entry3", "Entry4" };
int dword_10022F9C[2] = { 1038, 0 };
int dword_10022FA4[3] = { 1056, 1054, 0 };
int dword_10022FB0[7] = { 1117, 1118, 1119, 1120, 1121, 1122, 0 };
int selhero_msgtbl_string[2] = { 1038, 0 };
int selhero_msgtbl_3[6] = { 1057, 1058, 1059, 1060, 1061, 0 };
int selhero_msgtbl_info[6] = { 1014, 1018, 1017, 1016, 1015, 0 };
int dword_100230F0[3] = { 1038, 1080, 0 };
int dword_100230FC[2] = { 1097, 0 };
int dword_10023104[2] = { 1098, 0 };
int dword_1002310C[3] = { 1056, 1054, 0 };
int dword_10023118[7] = { 1088, 1089, 1090, 1091, 1092, 1093, 0 };
int sellist_msgtbl1[2] = { 1038, 0 };
int sellist_msgtbl2[3] = { 1056, 1054, 0 };
int sellist_msgtbl3[2] = { 1006, 0 };
int sellist_msgtbl4[7] = { 1047, 1048, 1049, 1050, 1051, 1052, 0 };
int selload_msgtbl1[2] = { 1038, 0 };
int selload_msgtbl2[3] = { 1056, 1054, 0 };
int selload_msgtbl3[3] = { 1106, 1107, 0 };
int dword_100231CC[2] = { 1038, 0 };
int dword_100231D4[3] = { 1080, 1097, 0 };
int dword_100231E0[2] = { 1123, 0 };
int dword_100231E8[3] = { 1056, 1054, 0 };
int dword_100231F4[7] = { 1110, 1111, 1112, 1113, 1114, 1115, 0 };
int dword_10023244[3] = { 1038, 1080, 0 };
int dword_10023250[2] = { 1142, 0 };
int dword_10023258[2] = { 1146, 0 };
int dword_10023260[3] = { 1056, 1054, 0 };
int dword_1002326C[7] = { 1135, 1136, 1137, 1138, 1139, 1140, 0 };
int yesno_msgtbl2[2] = { 1026, 0 };
int yesno_msgtbl1[3] = { 1109, 2, 0 };
int titlemsgtbl[2] = { 1067, 0 };

//data+bss
int artfont_cpp_float = 0; // weak
FontStruct font42g;
FontStruct *sgpCurrFont;
FontStruct font30g;
FontStruct font16s;
FontStruct font24s;
FontStruct font16g;
FontStruct font24g;
FontStruct font30s;
FontStruct font42y;
LPARAM dword_10029400; // idb
int dword_10029404; // weak
int dword_10029408; // weak
int dword_1002940C; // weak
BYTE *dword_10029410; // idb
int dword_10029414; // weak
int dword_10029418; // weak
int dword_1002941C; // weak
HGDIOBJ dword_10029420; // idb
HGDIOBJ dword_10029424; // idb
BYTE *dword_10029428; // idb
void *dword_1002942C; // idb
int (__stdcall *dword_10029430)(_DWORD, _DWORD, _DWORD, _DWORD); // weak
void *dword_10029434; // idb
int dword_10029438[4]; // weak
char nullcharacter; /* check */
HGDIOBJ dword_10029450; // idb
int dword_10029454; // weak
int dword_10029458; // weak
int dword_10029460[3]; // idb
int dword_1002946C; // weak
HGDIOBJ dword_10029470; // idb
int dword_10029478; // weak
int dword_10029480; // weak
int dword_10029488; // weak
int dword_1002948C; // weak
int Connect_cpp_float; // weak
int special_frames; // weak
DWORD heroport_data[2];
char connect_categorystr[128];
char connect_plrinfostr[128];
int heronum_frames2; // weak
DWORD special_data[2];
int heroport_frames; // weak
DWORD heronum_data[2];
int heronum_frames; // idb
int connect_draw_height; // idb
BYTE *connect_data1; // idb
BYTE *connect_data2; // idb
BYTE *connect_data3; // idb
void *connect_data4; // idb
HANDLE connect_trans[10];
char *connect_charname;
int connect_color_text; // weak
HGLOBAL copyprot_popupart; // idb
HGLOBAL copyprot_artpal; // idb
int CopyProt_cpp_float; // weak
HGLOBAL copyprot_btnart; // idb
HGDIOBJ cr8game_hobject; // idb
int cr8game_cpp_float; // weak
int cr8_playercount; // weak
DWORD *cr8_somegamestruct;
int cr8_dword_10029638; // weak
int cr8_dword_1002963C; // weak
int cr8_dword_10029640; // weak
int *cr8game_playerID; // idb
_gamedata cr8_gamedata;
HWND cr8_sendmsg1; // idb
HWND cr8_sendmsg2; // idb
int cr8_dword_10029658; // weak
DWORD cr8diffbtns_size[2];
int cr8_dword_10029668; // weak
int cr8_dword_1002966C; // idb
char cr8_gamename[32];
char cr8_gamepassword[32];
BYTE *cr8_creat_bg_ptr;
BYTE *cr8_but_xsm_ptr; // idb
BYTE *cr8_diffbtns_ptr; // idb
int creadung_playername; // weak
int *creadung_playerID; // idb
int CreaDung_cpp_float; // weak
int creadung_dword_100296C8; // weak
int creadung_delspinners; // weak
DWORD *crea_somegamestruct;
int creadung_lasterror; // weak
int creadung_dword_100296D8; // weak
char *creadung_gamename;
int CreaStat_cpp_float; // weak
int credittext_size; // weak
int credits_cpp_float; // weak
HGLOBAL credittext_rsrc; // idb
int credit_vertical_pos2; // idb
int credit_horz_pos; // idb
int credit_vertical_pos1; // weak
int credit_line_count; // weak
void *credit_back_img; // idb
HANDLE creditsobj; // idb
int DiabEdit_cpp_float; // weak
int DiabloUI_cpp_float; // weak
int sgbUiIsInitialized; // weak
HINSTANCE ghUiInst; // idb
int backbmp_flag1; // weak
int backbmp_flag2; // weak
int backbmp_flag3; // weak
int app_is_active; // weak
int sgbIsSpawn; // weak
int dword_10029730; // weak
int dword_10029738; // weak
char byte_1002973C; // idb
char byte_100297BC; // idb
int dword_1002983C; // weak
int dword_10029840; // weak
int dword_10029844; // weak
void *dword_10029848; // idb
int dword_1002984C; // weak
int disclaim_cpp_float; // weak
int doom_cpp_float; // weak
LPSTR dword_10029858; // idb
int dword_1002985C; // weak
int EntName_cpp_float; // weak
char *entname_charname;
int fade_cpp_float; // weak
int sgbFadeRange; // idb
tagPALETTEENTRY fadepal[256];
int sgbIsFading; // weak
HANDLE SpinnerTransOut[8];
int focus_spin_width; // idb
int focus_spin_height; // weak
int focus_cpp_float; // weak
int sgbSpinnersLoaded; // weak
int dword_10029CA8; // weak
int dword_10029CAC; // weak
int sgnSpinnerFrame; // weak
int local_cpp_float; // weak
DWORD gdwCursData[2]; // weak
tagPALETTEENTRY artpal[256];
HGDIOBJ objPalette; // idb
BYTE *gpCursorArt;
BYTE *gpCursorArt2;
int mainmenu_cpp_float; // weak
char menu_version_str[64];
int menu_item_timer; // weak
int dword_1002A120; // weak
int dword_1002A124; // weak
int dword_1002A128; // weak
int dword_1002A12C; // weak
int dword_1002A130; // weak
int dword_1002A134; // weak
int dword_1002A138; // weak
int dword_1002A13C; // weak
int dword_1002A140; // weak
int dword_1002A144; // weak
int dword_1002A148; // weak
void *dword_1002A14C; // idb
int dword_1002A150; // weak
char byte_1002A154; // idb
char byte_1002A1D4; // idb
int dword_1002A254; // weak
int dword_1002A258; // weak
int dword_1002A25C; // weak
int (*dword_1002A260)(void); // weak
char byte_1002A264; // idb
int OkCancel_cpp_float; // weak
int dword_1002A2E8; // weak
int dword_1002A2EC; // weak
int dword_1002A2F0; // weak
int (*dword_1002A2F4)(void); // weak
int dword_1002A2F8; // weak
BOOL dword_1002A2FC; // idb
int dword_1002A300; // weak
int dword_1002A304; // weak
DWORD dword_1002A308; // idb
DWORD dword_1002A310; // idb
BYTE *dword_1002A318; // idb
BYTE *dword_1002A31C; // idb
BYTE *dword_1002A320; // idb
BYTE *dword_1002A324; // idb
void *dword_1002A328; // idb
int Sbar_cpp_float; // weak
int Sbar_cpp_float2; // weak
int SelClass_cpp_float; // weak
int dword_1002A34C; // idb
int dword_1002A350; // weak
int dword_1002A354; // weak
char *dword_1002A358; // idb
int dword_1002A35C; // weak
int dword_1002A360; // idb
int dword_1002A364; // weak
int dword_1002A368; // weak
int dword_1002A36C; // weak
int dword_1002A370; // weak
int dword_1002A374; // weak
char *dword_1002A378; // idb
int dword_1002A37C; // weak
char byte_1002A380[128]; // weak
int dword_1002A400; // weak
int dword_1002A404; // weak
int dword_1002A408; // weak
BOOL (__stdcall *selhero_fnstats)(int, _uidefaultstats *);
int SelHero_cpp_float; // weak
DWORD selhero_sizedata[2]; // idb
int selhero_difficulty; // weak
int selhero_hero_hassaved; // weak
int selhero_numheroesleft; // weak
char selhero_herolevel[4];
BOOL (__stdcall *selhero_fnremove)(_uiheroinfo *);
BOOL (__stdcall *selhero_fninfo)(BOOL (__stdcall *fninfo)(_uiheroinfo *));
char selhero_heromag[4];
char selhero_heronamestr[16];
BOOL (__stdcall *selhero_fncreate)(_uiheroinfo *);
char selhero_herodex[4];
_uiheroinfo *sgpHeroInfo;
int selhero_is_created; // weak
_uiheroinfo heroinfo_create;
int selhero_is_good; // idb
char selhero_herostr[4];
char selhero_herovit[4];
BYTE *selhero_buffer;
int dword_1002A49C; // weak
void *dword_1002A4A0; // idb
int dword_1002A4A4; // weak
int dword_1002A4A8; // weak
int dword_1002A4AC; // weak
int dword_1002A4B0; // weak
int dword_1002A4B4; // weak
int dword_1002A4B8; // idb
int dword_1002A4BC; // weak
int SelList_cpp_float; // weak
_uiheroinfo *sellist_pheroinfo;
int SelLoad_cpp_float; // weak
int dword_1002A4CC; // weak
int dword_1002A4D0; // weak
void *dword_1002A4D4; // idb
int dword_1002A4D8; // idb
int dword_1002A4DC; // weak
int dword_1002A4E0; // weak
int dword_1002A4E4; // weak
int dword_1002A4E8; // weak
_uiheroinfo *dword_1002A4EC; // idb
int dword_1002A4F0; // weak
int dword_1002A4F4; // idb
char *yesno_dialog_string;
int SelYesNo_cpp_float; // weak
int yesno_remove_focus; // weak
char *yesno_hero_name;
int (*YesNoFunc)(void); // weak
int yesno_is_popup; // weak
HANDLE titlePHTrans[30];
int Title_cpp_float; // weak
int titleTransIdx; // weak
int titlesnd_cpp_float; // weak
void (__stdcall *gfnSoundFunction)(char *file);
