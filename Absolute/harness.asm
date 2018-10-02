
SEGMENT .d.text    exec

global __start_text
__start_text:

global d_j_appfat_cpp_init:function 5
d_j_appfat_cpp_init:
incbin EXE, 0x400, 5
global d_appfat_cpp_init:function 11
d_appfat_cpp_init:
incbin EXE, 0x405, 11
global d_appfat_cpp_free:function 26
d_appfat_cpp_free:
incbin EXE, 0x410, 26
global d_GetErrorStr:function 164
d_GetErrorStr:
incbin EXE, 0x42A, 164
global d_TraceErrorDD:function 1891
d_TraceErrorDD:
incbin EXE, 0x4CE, 1891
global d_TraceErrorDS:function 265
d_TraceErrorDS:
incbin EXE, 0xC31, 265
global d_TraceLastError:function 13
d_TraceLastError:
incbin EXE, 0xD3A, 13
global d_TermMsg:function 46
d_TermMsg:
incbin EXE, 0xD47, 46
global d_MsgBox:function 82
d_MsgBox:
incbin EXE, 0xD75, 82
global d_FreeDlg:function 105
d_FreeDlg:
incbin EXE, 0xDC7, 105
global d_DrawDlg:function 53
d_DrawDlg:
incbin EXE, 0xE30, 53
global d_DDErrMsg:function 35
d_DDErrMsg:
incbin EXE, 0xE65, 35
global d_DSErrMsg:function 35
d_DSErrMsg:
incbin EXE, 0xE88, 35
global d_center_window:function 146
d_center_window:
incbin EXE, 0xEAB, 146
global d_ErrDlg:function 141
d_ErrDlg:
incbin EXE, 0xF3D, 141
global d_FuncDlg:function 69
d_FuncDlg:
incbin EXE, 0xFCA, 69
global d_TextDlg:function 31
d_TextDlg:
incbin EXE, 0x100F, 31
global d_ErrOkDlg:function 110
d_ErrOkDlg:
incbin EXE, 0x102E, 110
global d_FileErrDlg:function 69
d_FileErrDlg:
incbin EXE, 0x109C, 69
global d_DiskFreeDlg:function 60
d_DiskFreeDlg:
incbin EXE, 0x10E1, 60
global d_InsertCDDlg:function 75
d_InsertCDDlg:
incbin EXE, 0x111D, 75
global d_DirErrorDlg:function 60
d_DirErrorDlg:
incbin EXE, 0x1168, 60
global d_InitAutomapOnce:function 68
d_InitAutomapOnce:
incbin EXE, 0x11A4, 68
global d_InitAutomap:function 268
d_InitAutomap:
incbin EXE, 0x11E8, 268
global d_StartAutomap:function 25
d_StartAutomap:
incbin EXE, 0x12F4, 25
global d_AutomapUp:function 13
d_AutomapUp:
incbin EXE, 0x130D, 13
global d_AutomapDown:function 13
d_AutomapDown:
incbin EXE, 0x131A, 13
global d_AutomapLeft:function 13
d_AutomapLeft:
incbin EXE, 0x1327, 13
global d_AutomapRight:function 13
d_AutomapRight:
incbin EXE, 0x1334, 13
global d_AutomapZoomIn:function 63
d_AutomapZoomIn:
incbin EXE, 0x1341, 63
global d_AutomapZoomOut:function 61
d_AutomapZoomOut:
incbin EXE, 0x1380, 61
global d_DrawAutomap:function 630
d_DrawAutomap:
incbin EXE, 0x13BD, 630
global d_DrawAutomapType:function 1909
d_DrawAutomapType:
incbin EXE, 0x1633, 1909
global d_DrawAutomapPlr:function 987
d_DrawAutomapPlr:
incbin EXE, 0x1DA8, 987
global d_GetAutomapType:function 199
d_GetAutomapType:
incbin EXE, 0x2183, 199
global d_DrawAutomapGame:function 221
d_DrawAutomapGame:
incbin EXE, 0x224A, 221
global d_SetAutomapView:function 438
d_SetAutomapView:
incbin EXE, 0x2327, 438
global d_AutomapZoomReset:function 62
d_AutomapZoomReset:
incbin EXE, 0x24DD, 62
global d_CaptureScreen:function 233
d_CaptureScreen:
incbin EXE, 0x251B, 233
global d_CaptureHdr:function 144
d_CaptureHdr:
incbin EXE, 0x2604, 144
global d_CapturePal:function 105
d_CapturePal:
incbin EXE, 0x2694, 105
global d_CapturePix:function 109
d_CapturePix:
incbin EXE, 0x26FD, 109
global d_CaptureEnc:function 62
d_CaptureEnc:
incbin EXE, 0x276A, 62
global d_CaptureFile:function 200
d_CaptureFile:
incbin EXE, 0x27A8, 200
global d_RedPalette:function 105
d_RedPalette:
incbin EXE, 0x2870, 105
global d_codec_decode:function 253
d_codec_decode:
incbin EXE, 0x28D9, 253
global d_j_sha1_reset:function 5
d_j_sha1_reset:
incbin EXE, 0x29D6, 5
global d_codec_init_key:function 209
d_codec_init_key:
incbin EXE, 0x29DB, 209
global d_codec_get_encoded_len:function 18
d_codec_get_encoded_len:
incbin EXE, 0x2AAC, 18
global d_codec_encode:function 278
d_codec_encode:
incbin EXE, 0x2ABE, 278
global d_DrawSpellCel:function 170
d_DrawSpellCel:
incbin EXE, 0x2BD4, 170
global d_SetSpellTrans:function 329
d_SetSpellTrans:
incbin EXE, 0x2C7E, 329
global d_DrawSpell:function 199
d_DrawSpell:
incbin EXE, 0x2DC7, 199
global d_DrawSpellList:function 1243
d_DrawSpellList:
incbin EXE, 0x2E8E, 1243
global d_SetSpell:function 67
d_SetSpell:
incbin EXE, 0x3369, 67
global d_SetSpeedSpell:function 107
d_SetSpeedSpell:
incbin EXE, 0x33AC, 107
global d_ToggleSpell:function 195
d_ToggleSpell:
incbin EXE, 0x3417, 195
global d_CPrintString:function 318
d_CPrintString:
incbin EXE, 0x34DA, 318
global d_AddPanelString:function 50
d_AddPanelString:
incbin EXE, 0x3618, 50
global d_ClearPanel:function 15
d_ClearPanel:
incbin EXE, 0x364A, 15
global d_DrawPanelBox:function 108
d_DrawPanelBox:
incbin EXE, 0x3659, 108
global d_InitPanelStr:function 5
d_InitPanelStr:
incbin EXE, 0x36C5, 5
global d_SetFlaskHeight:function 81
d_SetFlaskHeight:
incbin EXE, 0x36CA, 81
global d_DrawFlask:function 64
d_DrawFlask:
incbin EXE, 0x371B, 64
global d_DrawLifeFlask:function 153
d_DrawLifeFlask:
incbin EXE, 0x375B, 153
global d_UpdateLifeFlask:function 129
d_UpdateLifeFlask:
incbin EXE, 0x37F4, 129
global d_DrawManaFlask:function 129
d_DrawManaFlask:
incbin EXE, 0x3875, 129
global d_control_update_life_mana:function 116
d_control_update_life_mana:
incbin EXE, 0x38F6, 116
global d_UpdateManaFlask:function 172
d_UpdateManaFlask:
incbin EXE, 0x396A, 172
global d_InitControlPan:function 798
d_InitControlPan:
incbin EXE, 0x3A16, 798
global d_ClearCtrlPan:function 37
d_ClearCtrlPan:
incbin EXE, 0x3D34, 37
global d_DrawCtrlPan:function 177
d_DrawCtrlPan:
incbin EXE, 0x3D59, 177
global d_DoSpeedBook:function 328
d_DoSpeedBook:
incbin EXE, 0x3E0A, 328
global d_DoPanBtn:function 153
d_DoPanBtn:
incbin EXE, 0x3F52, 153
global d_control_set_button_down:function 21
d_control_set_button_down:
incbin EXE, 0x3FEB, 21
global d_control_check_btn_press:function 116
d_control_check_btn_press:
incbin EXE, 0x4000, 116
global d_DoAutoMap:function 44
d_DoAutoMap:
incbin EXE, 0x4074, 44
global d_CheckPanelInfo:function 836
d_CheckPanelInfo:
incbin EXE, 0x40A0, 836
global d_CheckBtnUp:function 413
d_CheckBtnUp:
incbin EXE, 0x43E4, 413
global d_FreeControlPan:function 276
d_FreeControlPan:
incbin EXE, 0x4581, 276
global d_control_WriteStringToBuffer:function 51
d_control_WriteStringToBuffer:
incbin EXE, 0x4695, 51
global d_DrawInfoBox:function 756
d_DrawInfoBox:
incbin EXE, 0x46C8, 756
global d_control_print_info_str:function 197
d_control_print_info_str:
incbin EXE, 0x49BC, 197
global d_PrintGameStr:function 87
d_PrintGameStr:
incbin EXE, 0x4A81, 87
global d_DrawChr:function 2432
d_DrawChr:
incbin EXE, 0x4AD8, 2432
global d_ADD_PlrStringXY:function 183
d_ADD_PlrStringXY:
incbin EXE, 0x5458, 183
global d_MY_PlrStringXY:function 187
d_MY_PlrStringXY:
incbin EXE, 0x550F, 187
global d_CheckLvlBtn:function 54
d_CheckLvlBtn:
incbin EXE, 0x55CA, 54
global d_ReleaseLvlBtn:function 52
d_ReleaseLvlBtn:
incbin EXE, 0x5600, 52
global d_DrawLevelUpIcon:function 70
d_DrawLevelUpIcon:
incbin EXE, 0x5634, 70
global d_CheckChrBtns:function 236
d_CheckChrBtns:
incbin EXE, 0x567A, 236
global d_ReleaseChrBtns:function 162
d_ReleaseChrBtns:
incbin EXE, 0x5766, 162
global d_DrawDurIcon:function 134
d_DrawDurIcon:
incbin EXE, 0x5808, 134
global d_DrawDurIcon4Item:function 122
d_DrawDurIcon4Item:
incbin EXE, 0x588E, 122
global d_RedBack:function 138
d_RedBack:
incbin EXE, 0x5908, 138
global d_GetSBookTrans:function 213
d_GetSBookTrans:
incbin EXE, 0x5992, 213
global d_DrawSpellBook:function 653
d_DrawSpellBook:
incbin EXE, 0x5A67, 653
global d_PrintSBookStr:function 194
d_PrintSBookStr:
incbin EXE, 0x5CF4, 194
global d_CheckSBook:function 322
d_CheckSBook:
incbin EXE, 0x5DB6, 322
global d_get_pieces_str:function 16
d_get_pieces_str:
incbin EXE, 0x5EF8, 16
global d_DrawGoldSplit:function 312
d_DrawGoldSplit:
incbin EXE, 0x5F08, 312
global d_control_drop_gold:function 302
d_control_drop_gold:
incbin EXE, 0x6040, 302
global d_control_remove_gold:function 182
d_control_remove_gold:
incbin EXE, 0x616E, 182
global d_control_set_gold_curs:function 70
d_control_set_gold_curs:
incbin EXE, 0x6224, 70
global d_DrawTalkPan:function 519
d_DrawTalkPan:
incbin EXE, 0x626A, 519
global d_control_print_talk_msg:function 130
d_control_print_talk_msg:
incbin EXE, 0x6471, 130
global d_control_check_talk_btn:function 90
d_control_check_talk_btn:
incbin EXE, 0x64F3, 90
global d_control_release_talk_btn:function 115
d_control_release_talk_btn:
incbin EXE, 0x654D, 115
global d_control_reset_talk_msg:function 58
d_control_reset_talk_msg:
incbin EXE, 0x65C0, 58
global d_control_type_message:function 71
d_control_type_message:
incbin EXE, 0x65FA, 71
global d_control_reset_talk:function 25
d_control_reset_talk:
incbin EXE, 0x6641, 25
global d_control_talk_last_key:function 64
d_control_talk_last_key:
incbin EXE, 0x665A, 64
global d_control_presskeys:function 106
d_control_presskeys:
incbin EXE, 0x669A, 106
global d_control_press_enter:function 190
d_control_press_enter:
incbin EXE, 0x6704, 190
global d_control_up_down:function 72
d_control_up_down:
incbin EXE, 0x67C2, 72
global d_InitCursor:function 22
d_InitCursor:
incbin EXE, 0x680A, 22
global d_FreeCursor:function 23
d_FreeCursor:
incbin EXE, 0x6820, 23
global d_SetICursor:function 52
d_SetICursor:
incbin EXE, 0x6837, 52
global d_SetCursor:function 35
d_SetCursor:
incbin EXE, 0x686B, 35
global d_NewCursor:function 5
d_NewCursor:
incbin EXE, 0x688E, 5
global d_InitLevelCursor:function 61
d_InitLevelCursor:
incbin EXE, 0x6893, 61
global d_CheckTown:function 301
d_CheckTown:
incbin EXE, 0x68D0, 301
global d_CheckRportal:function 300
d_CheckRportal:
incbin EXE, 0x69FD, 300
global d_CheckCursMove:function 3453
d_CheckCursMove:
incbin EXE, 0x6B29, 3453
global d_InitDead:function 438
d_InitDead:
incbin EXE, 0x78A6, 438
global d_AddDead:function 33
d_AddDead:
incbin EXE, 0x7A5C, 33
global d_SetDead:function 119
d_SetDead:
incbin EXE, 0x7A7D, 119
global d_LoadDebugGFX:function 27
d_LoadDebugGFX:
incbin EXE, 0x7AF4, 27
global d_FreeDebugGFX:function 18
d_FreeDebugGFX:
incbin EXE, 0x7B0F, 18
global d_CheckDungeonClear:function 144
d_CheckDungeonClear:
incbin EXE, 0x7B21, 144
global d_j_diablo_cpp_init:function 5
d_j_diablo_cpp_init:
incbin EXE, 0x7BB1, 5
global d_diablo_cpp_init:function 11
d_diablo_cpp_init:
incbin EXE, 0x7BB6, 11
global d_FreeGameMem:function 119
d_FreeGameMem:
incbin EXE, 0x7BC1, 119
global d_diablo_init_menu:function 170
d_diablo_init_menu:
incbin EXE, 0x7C38, 170
global d_run_game_loop:function 426
d_run_game_loop:
incbin EXE, 0x7CE2, 426
global d_start_game:function 79
d_start_game:
incbin EXE, 0x7E8C, 79
global d_free_game:function 67
d_free_game:
incbin EXE, 0x7EDB, 67
global d_diablo_get_not_running:function 44
d_diablo_get_not_running:
incbin EXE, 0x7F1E, 44
global d_WinMain:function 359
d_WinMain:
incbin EXE, 0x7F4A, 359
global d_diablo_parse_flags:function 176
d_diablo_parse_flags:
incbin EXE, 0x80B1, 176
global d_diablo_init_screen:function 80
d_diablo_init_screen:
incbin EXE, 0x8161, 80
global d_diablo_find_window:function 67
d_diablo_find_window:
incbin EXE, 0x81B1, 67
global d_diablo_reload_process:function 475
d_diablo_reload_process:
incbin EXE, 0x81F4, 475
global d_PressEscKey:function 143
d_PressEscKey:
incbin EXE, 0x83CF, 143
global d_DisableInputWndProc:function 211
d_DisableInputWndProc:
incbin EXE, 0x845E, 211
global d_GM_Game:function 641
d_GM_Game:
incbin EXE, 0x8531, 641
global d_LeftMouseDown:function 1082
d_LeftMouseDown:
incbin EXE, 0x87B2, 1082
global d_TryIconCurs:function 375
d_TryIconCurs:
incbin EXE, 0x8BEC, 375
global d_LeftMouseUp:function 69
d_LeftMouseUp:
incbin EXE, 0x8D63, 69
global d_RightMouseDown:function 225
d_RightMouseDown:
incbin EXE, 0x8DA8, 225
global d_j_gmenu_run_item:function 5
d_j_gmenu_run_item:
incbin EXE, 0x8E89, 5
global d_PressSysKey:function 34
d_PressSysKey:
incbin EXE, 0x8E8E, 34
global d_diablo_hotkey_msg:function 161
d_diablo_hotkey_msg:
incbin EXE, 0x8EB0, 161
global d_ReleaseKey:function 11
d_ReleaseKey:
incbin EXE, 0x8F51, 11
global d_PressKey:function 999
d_PressKey:
incbin EXE, 0x8F5C, 999
global d_diablo_pause_game:function 60
d_diablo_pause_game:
incbin EXE, 0x9343, 60
global d_PressChar:function 1042
d_PressChar:
incbin EXE, 0x937F, 1042
global d_LoadLvlGFX:function 291
d_LoadLvlGFX:
incbin EXE, 0x9791, 291
global d_LoadAllGFX:function 45
d_LoadAllGFX:
incbin EXE, 0x98B4, 45
global d_CreateLevel:function 195
d_CreateLevel:
incbin EXE, 0x98E1, 195
global d_LoadGameLevel:function 1343
d_LoadGameLevel:
incbin EXE, 0x99A4, 1343
global d_game_loop:function 80
d_game_loop:
incbin EXE, 0x9EE3, 80
global d_game_logic:function 180
d_game_logic:
incbin EXE, 0x9F33, 180
global d_timeout_cursor:function 132
d_timeout_cursor:
incbin EXE, 0x9FE7, 132
global d_diablo_color_cyc_logic:function 66
d_diablo_color_cyc_logic:
incbin EXE, 0xA06B, 66
global d_doom_get_frame_from_time:function 25
d_doom_get_frame_from_time:
incbin EXE, 0xA0AD, 25
global d_doom_alloc_cel:function 16
d_doom_alloc_cel:
incbin EXE, 0xA0C6, 16
global d_doom_cleanup:function 18
d_doom_cleanup:
incbin EXE, 0xA0D6, 18
global d_doom_load_graphics:function 76
d_doom_load_graphics:
incbin EXE, 0xA0E8, 76
global d_doom_init:function 42
d_doom_init:
incbin EXE, 0xA134, 42
global d_doom_close:function 22
d_doom_close:
incbin EXE, 0xA15E, 22
global d_doom_draw:function 98
d_doom_draw:
incbin EXE, 0xA174, 98
global d_DRLG_Init_Globals:function 163
d_DRLG_Init_Globals:
incbin EXE, 0xA1D6, 163
global d_LoadL1Dungeon:function 236
d_LoadL1Dungeon:
incbin EXE, 0xA279, 236
global d_DRLG_L1Floor:function 78
d_DRLG_L1Floor:
incbin EXE, 0xA365, 78
global d_DRLG_L1Pass3:function 242
d_DRLG_L1Pass3:
incbin EXE, 0xA3B3, 242
global d_DRLG_InitL1Vals:function 187
d_DRLG_InitL1Vals:
incbin EXE, 0xA4A5, 187
global d_LoadPreL1Dungeon:function 201
d_LoadPreL1Dungeon:
incbin EXE, 0xA560, 201
global d_CreateL5Dungeon:function 77
d_CreateL5Dungeon:
incbin EXE, 0xA629, 77
global d_DRLG_LoadL1SP:function 126
d_DRLG_LoadL1SP:
incbin EXE, 0xA676, 126
global d_DRLG_FreeL1SP:function 18
d_DRLG_FreeL1SP:
incbin EXE, 0xA6F4, 18
global d_DRLG_L5:function 617
d_DRLG_L5:
incbin EXE, 0xA706, 617
global d_DRLG_PlaceDoor:function 298
d_DRLG_PlaceDoor:
incbin EXE, 0xA96F, 298
global d_DRLG_L1Shadows:function 488
d_DRLG_L1Shadows:
incbin EXE, 0xAA99, 488
global d_DRLG_PlaceMiniSet:function 629
d_DRLG_PlaceMiniSet:
incbin EXE, 0xAC81, 629
global d_InitL5Dungeon:function 34
d_InitL5Dungeon:
incbin EXE, 0xAEF6, 34
global d_L5ClearFlags:function 27
d_L5ClearFlags:
incbin EXE, 0xAF18, 27
global d_L5firstRoom:function 563
d_L5firstRoom:
incbin EXE, 0xAF33, 563
global d_L5drawRoom:function 55
d_L5drawRoom:
incbin EXE, 0xB166, 55
global d_L5roomGen:function 519
d_L5roomGen:
incbin EXE, 0xB19D, 519
global d_L5checkRoom:function 100
d_L5checkRoom:
incbin EXE, 0xB3A4, 100
global d_L5GetArea:function 34
d_L5GetArea:
incbin EXE, 0xB408, 34
global d_L5makeDungeon:function 68
d_L5makeDungeon:
incbin EXE, 0xB42A, 68
global d_L5makeDmt:function 114
d_L5makeDmt:
incbin EXE, 0xB46E, 114
global d_L5AddWall:function 348
d_L5AddWall:
incbin EXE, 0xB4E0, 348
global d_L5HWallOk:function 160
d_L5HWallOk:
incbin EXE, 0xB63C, 160
global d_L5VWallOk:function 127
d_L5VWallOk:
incbin EXE, 0xB6DC, 127
global d_L5HorizWall:function 238
d_L5HorizWall:
incbin EXE, 0xB75B, 238
global d_L5VertWall:function 264
d_L5VertWall:
incbin EXE, 0xB849, 264
global d_L5tileFix:function 879
d_L5tileFix:
incbin EXE, 0xB951, 879
global d_DRLG_L5Subs:function 221
d_DRLG_L5Subs:
incbin EXE, 0xBCC0, 221
global d_L5FillChambers:function 1001
d_L5FillChambers:
incbin EXE, 0xBD9D, 1001
global d_DRLG_L5GChamber:function 321
d_DRLG_L5GChamber:
incbin EXE, 0xC186, 321
global d_DRLG_L5GHall:function 80
d_DRLG_L5GHall:
incbin EXE, 0xC2C7, 80
global d_DRLG_L5SetRoom:function 133
d_DRLG_L5SetRoom:
incbin EXE, 0xC317, 133
global d_DRLG_L5FloodTVal:function 111
d_DRLG_L5FloodTVal:
incbin EXE, 0xC39C, 111
global d_DRLG_L5FTVR:function 496
d_DRLG_L5FTVR:
incbin EXE, 0xC40B, 496
global d_DRLG_L5TransFix:function 136
d_DRLG_L5TransFix:
incbin EXE, 0xC5FB, 136
global d_DRLG_L5DirtFix:function 108
d_DRLG_L5DirtFix:
incbin EXE, 0xC683, 108
global d_DRLG_L5CornerFix:function 104
d_DRLG_L5CornerFix:
incbin EXE, 0xC6EF, 104
global d_InitDungeon:function 34
d_InitDungeon:
incbin EXE, 0xC757, 34
global d_L2LockoutFix:function 339
d_L2LockoutFix:
incbin EXE, 0xC779, 339
global d_L2DoorFix:function 53
d_L2DoorFix:
incbin EXE, 0xC8CC, 53
global d_LoadL2Dungeon:function 448
d_LoadL2Dungeon:
incbin EXE, 0xC901, 448
global d_DRLG_L2Pass3:function 242
d_DRLG_L2Pass3:
incbin EXE, 0xCAC1, 242
global d_LoadPreL2Dungeon:function 213
d_LoadPreL2Dungeon:
incbin EXE, 0xCBB3, 213
global d_CreateL2Dungeon:function 199
d_CreateL2Dungeon:
incbin EXE, 0xCC88, 199
global d_DRLG_LoadL2SP:function 85
d_DRLG_LoadL2SP:
incbin EXE, 0xCD4F, 85
global d_DRLG_FreeL2SP:function 18
d_DRLG_FreeL2SP:
incbin EXE, 0xCDA4, 18
global d_DRLG_L2:function 1726
d_DRLG_L2:
incbin EXE, 0xCDB6, 1726
global d_DRLG_L2PlaceMiniSet:function 605
d_DRLG_L2PlaceMiniSet:
incbin EXE, 0xD474, 605
global d_DRLG_L2PlaceRndSet:function 459
d_DRLG_L2PlaceRndSet:
incbin EXE, 0xD6D1, 459
global d_DRLG_L2Subs:function 256
d_DRLG_L2Subs:
incbin EXE, 0xD89C, 256
global d_DRLG_L2Shadows:function 207
d_DRLG_L2Shadows:
incbin EXE, 0xD99C, 207
global d_DRLG_L2SetRoom:function 133
d_DRLG_L2SetRoom:
incbin EXE, 0xDA6B, 133
global d_L2TileFix:function 95
d_L2TileFix:
incbin EXE, 0xDAF0, 95
global d_CreateDungeon:function 341
d_CreateDungeon:
incbin EXE, 0xDB4F, 341
global d_CreateRoom:function 1109
d_CreateRoom:
incbin EXE, 0xDCA4, 1109
global d_DefineRoom:function 292
d_DefineRoom:
incbin EXE, 0xE0F9, 292
global d_AddHall:function 143
d_AddHall:
incbin EXE, 0xE21D, 143
global d_GetHall:function 93
d_GetHall:
incbin EXE, 0xE2AC, 93
global d_ConnectHall:function 860
d_ConnectHall:
incbin EXE, 0xE309, 860
global d_CreateDoorType:function 88
d_CreateDoorType:
incbin EXE, 0xE665, 88
global d_PlaceHallExt:function 19
d_PlaceHallExt:
incbin EXE, 0xE6BD, 19
global d_DoPatternCheck:function 393
d_DoPatternCheck:
incbin EXE, 0xE6D0, 393
global d_DL2_FillVoids:function 1368
d_DL2_FillVoids:
incbin EXE, 0xE859, 1368
global d_DL2_Cont:function 61
d_DL2_Cont:
incbin EXE, 0xEDB1, 61
global d_DL2_NumNoChar:function 34
d_DL2_NumNoChar:
incbin EXE, 0xEDEE, 34
global d_DL2_DrawRoom:function 135
d_DL2_DrawRoom:
incbin EXE, 0xEE10, 135
global d_DL2_KnockWalls:function 213
d_DL2_KnockWalls:
incbin EXE, 0xEE97, 213
global d_DRLG_L2FloodTVal:function 111
d_DRLG_L2FloodTVal:
incbin EXE, 0xEF6C, 111
global d_DRLG_L2FTVR:function 496
d_DRLG_L2FTVR:
incbin EXE, 0xEFDB, 496
global d_DRLG_L2TransFix:function 136
d_DRLG_L2TransFix:
incbin EXE, 0xF1CB, 136
global d_L2DirtFix:function 108
d_L2DirtFix:
incbin EXE, 0xF253, 108
global d_DRLG_InitL2Vals:function 194
d_DRLG_InitL2Vals:
incbin EXE, 0xF2BF, 194
global d_AddFenceDoors:function 107
d_AddFenceDoors:
incbin EXE, 0xF381, 107
global d_FenceDoorFix:function 281
d_FenceDoorFix:
incbin EXE, 0xF3EC, 281
global d_DRLG_L3Anvil:function 272
d_DRLG_L3Anvil:
incbin EXE, 0xF505, 272
global d_FixL3Warp:function 104
d_FixL3Warp:
incbin EXE, 0xF615, 104
global d_FixL3HallofHeroes:function 116
d_FixL3HallofHeroes:
incbin EXE, 0xF67D, 116
global d_DRLG_L3LockRec:function 83
d_DRLG_L3LockRec:
incbin EXE, 0xF6F1, 83
global d_DRLG_L3Lockout:function 93
d_DRLG_L3Lockout:
incbin EXE, 0xF744, 93
global d_CreateL3Dungeon:function 173
d_CreateL3Dungeon:
incbin EXE, 0xF7A1, 173
global d_DRLG_L3:function 1073
d_DRLG_L3:
incbin EXE, 0xF84E, 1073
global d_InitL3Dungeon:function 54
d_InitL3Dungeon:
incbin EXE, 0xFC7F, 54
global d_DRLG_L3FillRoom:function 315
d_DRLG_L3FillRoom:
incbin EXE, 0xFCB5, 315
global d_DRLG_L3CreateBlock:function 464
d_DRLG_L3CreateBlock:
incbin EXE, 0xFDF0, 464
global d_DRLG_L3FloorArea:function 52
d_DRLG_L3FloorArea:
incbin EXE, 0xFFC0, 52
global d_DRLG_L3FillDiags:function 113
d_DRLG_L3FillDiags:
incbin EXE, 0xFFF4, 113
global d_DRLG_L3FillSingles:function 95
d_DRLG_L3FillSingles:
incbin EXE, 0x10065, 95
global d_DRLG_L3FillStraights:function 535
d_DRLG_L3FillStraights:
incbin EXE, 0x100C4, 535
global d_DRLG_L3Edges:function 33
d_DRLG_L3Edges:
incbin EXE, 0x102DB, 33
global d_DRLG_L3GetFloorArea:function 35
d_DRLG_L3GetFloorArea:
incbin EXE, 0x102FC, 35
global d_DRLG_L3MakeMegas:function 142
d_DRLG_L3MakeMegas:
incbin EXE, 0x1031F, 142
global d_DRLG_L3River:function 1639
d_DRLG_L3River:
incbin EXE, 0x103AD, 1639
global d_DRLG_L3Pool:function 350
d_DRLG_L3Pool:
incbin EXE, 0x10A14, 350
global d_DRLG_L3SpawnEdge:function 298
d_DRLG_L3SpawnEdge:
incbin EXE, 0x10B72, 298
global d_DRLG_L3Spawn:function 324
d_DRLG_L3Spawn:
incbin EXE, 0x10C9C, 324
global d_DRLG_L3PoolFix:function 148
d_DRLG_L3PoolFix:
incbin EXE, 0x10DE0, 148
global d_DRLG_L3PlaceMiniSet:function 527
d_DRLG_L3PlaceMiniSet:
incbin EXE, 0x10E74, 527
global d_DRLG_L3PlaceRndSet:function 395
d_DRLG_L3PlaceRndSet:
incbin EXE, 0x11083, 395
global d_DRLG_L3Wood:function 1072
d_DRLG_L3Wood:
incbin EXE, 0x1120E, 1072
global d_WoodVertU:function 76
d_WoodVertU:
incbin EXE, 0x1163E, 76
global d_WoodVertD:function 68
d_WoodVertD:
incbin EXE, 0x1168A, 68
global d_WoodHorizL:function 76
d_WoodHorizL:
incbin EXE, 0x116CE, 76
global d_WoodHorizR:function 68
d_WoodHorizR:
incbin EXE, 0x1171A, 68
global d_DRLG_L3Pass3:function 264
d_DRLG_L3Pass3:
incbin EXE, 0x1175E, 264
global d_LoadL3Dungeon:function 330
d_LoadL3Dungeon:
incbin EXE, 0x11866, 330
global d_LoadPreL3Dungeon:function 165
d_LoadPreL3Dungeon:
incbin EXE, 0x119B0, 165
global d_DRLG_LoadL4SP:function 88
d_DRLG_LoadL4SP:
incbin EXE, 0x11A55, 88
global d_DRLG_FreeL4SP:function 18
d_DRLG_FreeL4SP:
incbin EXE, 0x11AAD, 18
global d_DRLG_L4SetSPRoom:function 133
d_DRLG_L4SetSPRoom:
incbin EXE, 0x11ABF, 133
global d_L4SaveQuads:function 143
d_L4SaveQuads:
incbin EXE, 0x11B44, 143
global d_DRLG_L4SetRoom:function 94
d_DRLG_L4SetRoom:
incbin EXE, 0x11BD3, 94
global d_DRLG_LoadDiabQuads:function 258
d_DRLG_LoadDiabQuads:
incbin EXE, 0x11C31, 258
global d_IsDURWall:function 21
d_IsDURWall:
incbin EXE, 0x11D33, 21
global d_IsDLLWall:function 21
d_IsDLLWall:
incbin EXE, 0x11D48, 21
global d_L4FixRim:function 30
d_L4FixRim:
incbin EXE, 0x11D5D, 30
global d_DRLG_L4GeneralFix:function 53
d_DRLG_L4GeneralFix:
incbin EXE, 0x11D7B, 53
global d_CreateL4Dungeon:function 80
d_CreateL4Dungeon:
incbin EXE, 0x11DB0, 80
global d_DRLG_L4:function 989
d_DRLG_L4:
incbin EXE, 0x11E00, 989
global d_DRLG_L4Shadows:function 87
d_DRLG_L4Shadows:
incbin EXE, 0x121DD, 87
global d_InitL4Dungeon:function 71
d_InitL4Dungeon:
incbin EXE, 0x12234, 71
global d_L4makeDmt:function 80
d_L4makeDmt:
incbin EXE, 0x1227B, 80
global d_L4AddWall:function 759
d_L4AddWall:
incbin EXE, 0x122CB, 759
global d_L4HWallOk:function 174
d_L4HWallOk:
incbin EXE, 0x125C2, 174
global d_L4VWallOk:function 155
d_L4VWallOk:
incbin EXE, 0x12670, 155
global d_L4HorizWall:function 203
d_L4HorizWall:
incbin EXE, 0x1270B, 203
global d_L4VertWall:function 222
d_L4VertWall:
incbin EXE, 0x127D6, 222
global d_L4tileFix:function 3625
d_L4tileFix:
incbin EXE, 0x128B4, 3625
global d_DRLG_L4Subs:function 189
d_DRLG_L4Subs:
incbin EXE, 0x136DD, 189
global d_L4makeDungeon:function 279
d_L4makeDungeon:
incbin EXE, 0x1379A, 279
global d_uShape:function 307
d_uShape:
incbin EXE, 0x138B1, 307
global d_GetArea:function 34
d_GetArea:
incbin EXE, 0x139E4, 34
global d_L4firstRoom:function 306
d_L4firstRoom:
incbin EXE, 0x13A06, 306
global d_L4drawRoom:function 55
d_L4drawRoom:
incbin EXE, 0x13B38, 55
global d_L4roomGen:function 519
d_L4roomGen:
incbin EXE, 0x13B6F, 519
global d_L4checkRoom:function 108
d_L4checkRoom:
incbin EXE, 0x13D76, 108
global d_DRLG_L4PlaceMiniSet:function 610
d_DRLG_L4PlaceMiniSet:
incbin EXE, 0x13DE2, 610
global d_DRLG_L4FloodTVal:function 111
d_DRLG_L4FloodTVal:
incbin EXE, 0x14044, 111
global d_DRLG_L4FTVR:function 496
d_DRLG_L4FTVR:
incbin EXE, 0x140B3, 496
global d_DRLG_L4TransFix:function 184
d_DRLG_L4TransFix:
incbin EXE, 0x142A3, 184
global d_DRLG_L4Corners:function 53
d_DRLG_L4Corners:
incbin EXE, 0x1435B, 53
global d_DRLG_L4Pass3:function 264
d_DRLG_L4Pass3:
incbin EXE, 0x14390, 264
global d_j_dthread_cpp_init_1:function 5
d_j_dthread_cpp_init_1:
incbin EXE, 0x14498, 5
global d_dthread_cpp_init_1:function 11
d_dthread_cpp_init_1:
incbin EXE, 0x1449D, 11
global d_dthread_cpp_init_2:function 10
d_dthread_cpp_init_2:
incbin EXE, 0x144A8, 10
global d_dthread_init_mutex:function 12
d_dthread_init_mutex:
incbin EXE, 0x144B2, 12
global d_dthread_cleanup_mutex_atexit:function 12
d_dthread_cleanup_mutex_atexit:
incbin EXE, 0x144BE, 12
global d_dthread_cleanup_mutex:function 12
d_dthread_cleanup_mutex:
incbin EXE, 0x144CA, 12
global d_dthread_remove_player:function 51
d_dthread_remove_player:
incbin EXE, 0x144D6, 51
global d_dthread_send_delta:function 125
d_dthread_send_delta:
incbin EXE, 0x14509, 125
global d_dthread_start:function 109
d_dthread_start:
incbin EXE, 0x14586, 109
global d_dthread_handler:function 205
d_dthread_handler:
incbin EXE, 0x145F3, 205
global d_dthread_cleanup:function 162
d_dthread_cleanup:
incbin EXE, 0x146C0, 162
global d_j_dx_cpp_init_1:function 5
d_j_dx_cpp_init_1:
incbin EXE, 0x14762, 5
global d_dx_cpp_init_1:function 11
d_dx_cpp_init_1:
incbin EXE, 0x14767, 11
global d_dx_cpp_init_2:function 10
d_dx_cpp_init_2:
incbin EXE, 0x14772, 10
global d_dx_init_mutex:function 12
d_dx_init_mutex:
incbin EXE, 0x1477C, 12
global d_dx_cleanup_mutex_atexit:function 12
d_dx_cleanup_mutex_atexit:
incbin EXE, 0x14788, 12
global d_dx_cleanup_mutex:function 12
d_dx_cleanup_mutex:
incbin EXE, 0x14794, 12
global d_dx_init:function 277
d_dx_init:
incbin EXE, 0x147A0, 277
global d_dx_create_back_buffer:function 269
d_dx_create_back_buffer:
incbin EXE, 0x148B5, 269
global d_dx_create_primary_surface:function 88
d_dx_create_primary_surface:
incbin EXE, 0x149C2, 88
global d_dx_DirectDrawCreate:function 123
d_dx_DirectDrawCreate:
incbin EXE, 0x14A1A, 123
global d_j_lock_buf_priv:function 5
d_j_lock_buf_priv:
incbin EXE, 0x14A95, 5
global d_lock_buf_priv:function 134
d_lock_buf_priv:
incbin EXE, 0x14A9A, 134
global d_j_unlock_buf_priv:function 5
d_j_unlock_buf_priv:
incbin EXE, 0x14B20, 5
global d_unlock_buf_priv:function 123
d_unlock_buf_priv:
incbin EXE, 0x14B25, 123
global d_dx_cleanup:function 168
d_dx_cleanup:
incbin EXE, 0x14BA0, 168
global d_dx_reinit:function 92
d_dx_reinit:
incbin EXE, 0x14C48, 92
global d_j_dx_reinit:function 5
d_j_dx_reinit:
incbin EXE, 0x14CA4, 5
global d_j_effects_cpp_init:function 5
d_j_effects_cpp_init:
incbin EXE, 0x14CA9, 5
global d_effects_cpp_init:function 11
d_effects_cpp_init:
incbin EXE, 0x14CAE, 11
global d_effect_is_playing:function 41
d_effect_is_playing:
incbin EXE, 0x14CB9, 41
global d_sfx_stop:function 41
d_sfx_stop:
incbin EXE, 0x14CE2, 41
global d_InitMonsterSND:function 208
d_InitMonsterSND:
incbin EXE, 0x14D0B, 208
global d_FreeEffects:function 106
d_FreeEffects:
incbin EXE, 0x14DDB, 106
global d_PlayEffect:function 156
d_PlayEffect:
incbin EXE, 0x14E45, 156
global d_calc_snd_position:function 120
d_calc_snd_position:
incbin EXE, 0x14EE1, 120
global d_PlaySFX:function 24
d_PlaySFX:
incbin EXE, 0x14F59, 24
global d_PlaySFX_priv:function 185
d_PlaySFX_priv:
incbin EXE, 0x14F71, 185
global d_stream_play:function 109
d_stream_play:
incbin EXE, 0x1502A, 109
global d_RndSFX:function 106
d_RndSFX:
incbin EXE, 0x15097, 106
global d_PlaySfxLoc:function 56
d_PlaySfxLoc:
incbin EXE, 0x15101, 56
global d_FreeMonsterSnd:function 97
d_FreeMonsterSnd:
incbin EXE, 0x15139, 97
global d_sound_stop:function 32
d_sound_stop:
incbin EXE, 0x1519A, 32
global d_sound_update:function 69
d_sound_update:
incbin EXE, 0x151BA, 69
global d_effects_cleanup_sfx:function 43
d_effects_cleanup_sfx:
incbin EXE, 0x151FF, 43
global d_stream_update:function 77
d_stream_update:
incbin EXE, 0x1522A, 77
global d_priv_sound_init:function 97
d_priv_sound_init:
incbin EXE, 0x15277, 97
global d_sound_init:function 7
d_sound_init:
incbin EXE, 0x152D8, 7
global d_effects_play_sound:function 100
d_effects_play_sound:
incbin EXE, 0x152DF, 100
global d_Decrypt:function 76
d_Decrypt:
incbin EXE, 0x15343, 76
global d_Encrypt:function 80
d_Encrypt:
incbin EXE, 0x1538F, 80
global d_Hash:function 79
d_Hash:
incbin EXE, 0x153DF, 79
global d_InitHash:function 111
d_InitHash:
incbin EXE, 0x1542E, 111
global d_PkwareCompress:function 150
d_PkwareCompress:
incbin EXE, 0x1549D, 150
global d_PkwareBufferRead:function 52
d_PkwareBufferRead:
incbin EXE, 0x15533, 52
global d_PkwareBufferWrite:function 39
d_PkwareBufferWrite:
incbin EXE, 0x15567, 39
global d_PkwareDecompress:function 110
d_PkwareDecompress:
incbin EXE, 0x1558E, 110
global d_j_engine_cpp_init_1:function 5
d_j_engine_cpp_init_1:
incbin EXE, 0x155FC, 5
global d_engine_cpp_init_1:function 11
d_engine_cpp_init_1:
incbin EXE, 0x15601, 11
global d_CelDrawDatOnly:function 104
d_CelDrawDatOnly:
incbin EXE, 0x1560C, 104
global d_CelDecodeOnly:function 68
d_CelDecodeOnly:
incbin EXE, 0x15674, 68
global d_CelDecDatOnly:function 38
d_CelDecDatOnly:
incbin EXE, 0x156B8, 38
global d_CelDrawHdrOnly:function 123
d_CelDrawHdrOnly:
incbin EXE, 0x156DE, 123
global d_CelDecodeHdrOnly:function 83
d_CelDecodeHdrOnly:
incbin EXE, 0x15759, 83
global d_CelDecDatLightOnly:function 119
d_CelDecDatLightOnly:
incbin EXE, 0x157AC, 119
global d_CelDecDatLightEntry:function 101
d_CelDecDatLightEntry:
incbin EXE, 0x15823, 101
global d_CelDecDatLightTrans:function 221
d_CelDecDatLightTrans:
incbin EXE, 0x15888, 221
global d_CelDecodeLightOnly:function 88
d_CelDecodeLightOnly:
incbin EXE, 0x15965, 88
global d_CelDecodeHdrLightOnly:function 142
d_CelDecodeHdrLightOnly:
incbin EXE, 0x159BD, 142
global d_CelDecodeHdrLightTrans:function 116
d_CelDecodeHdrLightTrans:
incbin EXE, 0x15A4B, 116
global d_CelDrawHdrLightRed:function 284
d_CelDrawHdrLightRed:
incbin EXE, 0x15ABF, 284
global d_Cel2DecDatOnly:function 127
d_Cel2DecDatOnly:
incbin EXE, 0x15BDB, 127
global d_Cel2DrawHdrOnly:function 123
d_Cel2DrawHdrOnly:
incbin EXE, 0x15C5A, 123
global d_Cel2DecodeHdrOnly:function 85
d_Cel2DecodeHdrOnly:
incbin EXE, 0x15CD5, 85
global d_Cel2DecDatLightOnly:function 146
d_Cel2DecDatLightOnly:
incbin EXE, 0x15D2A, 146
global d_Cel2DecDatLightEntry:function 101
d_Cel2DecDatLightEntry:
incbin EXE, 0x15DBC, 101
global d_Cel2DecDatLightTrans:function 248
d_Cel2DecDatLightTrans:
incbin EXE, 0x15E21, 248
global d_Cel2DecodeHdrLight:function 144
d_Cel2DecodeHdrLight:
incbin EXE, 0x15F19, 144
global d_Cel2DecodeLightTrans:function 114
d_Cel2DecodeLightTrans:
incbin EXE, 0x15FA9, 114
global d_Cel2DrawHdrLightRed:function 289
d_Cel2DrawHdrLightRed:
incbin EXE, 0x1601B, 289
global d_CelDecodeRect:function 138
d_CelDecodeRect:
incbin EXE, 0x1613C, 138
global d_CelDecodeClr:function 250
d_CelDecodeClr:
incbin EXE, 0x161C6, 250
global d_CelDrawHdrClrHL:function 303
d_CelDrawHdrClrHL:
incbin EXE, 0x162C0, 303
global d_ENG_set_pixel:function 69
d_ENG_set_pixel:
incbin EXE, 0x163EF, 69
global d_engine_draw_pixel:function 137
d_engine_draw_pixel:
incbin EXE, 0x16434, 137
global d_DrawLine:function 1014
d_DrawLine:
incbin EXE, 0x164BD, 1014
global d_GetDirection:function 101
d_GetDirection:
incbin EXE, 0x168B3, 101
global d_SetRndSeed:function 20
d_SetRndSeed:
incbin EXE, 0x16918, 20
global d_GetRndSeed:function 31
d_GetRndSeed:
incbin EXE, 0x1692C, 31
global d_random:function 34
d_random:
incbin EXE, 0x1694B, 34
global d_engine_cpp_init_2:function 10
d_engine_cpp_init_2:
incbin EXE, 0x1696D, 10
global d_mem_init_mutex:function 12
d_mem_init_mutex:
incbin EXE, 0x16977, 12
global d_mem_atexit_mutex:function 12
d_mem_atexit_mutex:
incbin EXE, 0x16983, 12
global d_mem_free_mutex:function 12
d_mem_free_mutex:
incbin EXE, 0x1698F, 12
global d_DiabloAllocPtr:function 77
d_DiabloAllocPtr:
incbin EXE, 0x1699B, 77
global d_mem_free_dbg:function 48
d_mem_free_dbg:
incbin EXE, 0x169E8, 48
global d_LoadFileInMem:function 91
d_LoadFileInMem:
incbin EXE, 0x16A18, 91
global d_LoadFileWithMem:function 95
d_LoadFileWithMem:
incbin EXE, 0x16A73, 95
global d_Cl2ApplyTrans:function 115
d_Cl2ApplyTrans:
incbin EXE, 0x16AD2, 115
global d_Cl2DecodeFrm1:function 122
d_Cl2DecodeFrm1:
incbin EXE, 0x16B45, 122
global d_Cl2DecDatFrm1:function 136
d_Cl2DecDatFrm1:
incbin EXE, 0x16BBF, 136
global d_Cl2DecodeFrm2:function 126
d_Cl2DecodeFrm2:
incbin EXE, 0x16C47, 126
global d_Cl2DecDatFrm2:function 188
d_Cl2DecDatFrm2:
incbin EXE, 0x16CC5, 188
global d_Cl2DecodeFrm3:function 195
d_Cl2DecodeFrm3:
incbin EXE, 0x16D81, 195
global d_Cl2DecDatLightTbl1:function 165
d_Cl2DecDatLightTbl1:
incbin EXE, 0x16E44, 165
global d_Cl2DecodeLightTbl:function 154
d_Cl2DecodeLightTbl:
incbin EXE, 0x16EE9, 154
global d_Cl2DecodeFrm4:function 122
d_Cl2DecodeFrm4:
incbin EXE, 0x16F83, 122
global d_Cl2DecDatFrm4:function 156
d_Cl2DecDatFrm4:
incbin EXE, 0x16FFD, 156
global d_Cl2DecodeClrHL:function 143
d_Cl2DecodeClrHL:
incbin EXE, 0x17099, 143
global d_Cl2DecDatClrHL:function 208
d_Cl2DecDatClrHL:
incbin EXE, 0x17128, 208
global d_Cl2DecodeFrm5:function 195
d_Cl2DecodeFrm5:
incbin EXE, 0x171F8, 195
global d_Cl2DecDatLightTbl2:function 189
d_Cl2DecDatLightTbl2:
incbin EXE, 0x172BB, 189
global d_Cl2DecodeFrm6:function 154
d_Cl2DecodeFrm6:
incbin EXE, 0x17378, 154
global d_PlayInGameMovie:function 60
d_PlayInGameMovie:
incbin EXE, 0x17412, 60
global d_InitDiabloMsg:function 65
d_InitDiabloMsg:
incbin EXE, 0x1744E, 65
global d_ClrDiabloMsg:function 27
d_ClrDiabloMsg:
incbin EXE, 0x1748F, 27
global d_DrawDiabloMsg:function 515
d_DrawDiabloMsg:
incbin EXE, 0x174AA, 515
global d_exception_cpp_init:function 10
d_exception_cpp_init:
incbin EXE, 0x176AD, 10
global d_exception_install_filter:function 10
d_exception_install_filter:
incbin EXE, 0x176B7, 10
global d_j_exception_init_filter:function 12
d_j_exception_init_filter:
incbin EXE, 0x176C1, 12
global d_exception_init_filter:function 10
d_exception_init_filter:
incbin EXE, 0x176CD, 10
global d_TopLevelExceptionFilter:function 382
d_TopLevelExceptionFilter:
incbin EXE, 0x176D7, 382
global d_exception_hex_format:function 195
d_exception_hex_format:
incbin EXE, 0x17855, 195
global d_exception_unknown_module:function 231
d_exception_unknown_module:
incbin EXE, 0x17918, 231
global d_exception_call_stack:function 137
d_exception_call_stack:
incbin EXE, 0x179FF, 137
global d_exception_get_error_type:function 436
d_exception_get_error_type:
incbin EXE, 0x17A88, 436
global d_exception_set_filter:function 23
d_exception_set_filter:
incbin EXE, 0x17C3C, 23
global d_exception_set_filter_ptr:function 13
d_exception_set_filter_ptr:
incbin EXE, 0x17C53, 13
global d_exception_get_filter:function 6
d_exception_get_filter:
incbin EXE, 0x17C60, 6
global d_gamemenu_previous:function 41
d_gamemenu_previous:
incbin EXE, 0x17C66, 41
global d_gamemenu_enable_single:function 57
d_gamemenu_enable_single:
incbin EXE, 0x17C8F, 57
global d_gamemenu_enable_multi:function 16
d_gamemenu_enable_multi:
incbin EXE, 0x17CC8, 16
global d_gamemenu_off:function 9
d_gamemenu_off:
incbin EXE, 0x17CD8, 9
global d_gamemenu_handle_previous:function 19
d_gamemenu_handle_previous:
incbin EXE, 0x17CE1, 19
global d_j_gamemenu_previous:function 5
d_j_gamemenu_previous:
incbin EXE, 0x17CF4, 5
global d_gamemenu_new_game:function 66
d_gamemenu_new_game:
incbin EXE, 0x17CF9, 66
global d_gamemenu_quit_game:function 13
d_gamemenu_quit_game:
incbin EXE, 0x17D3B, 13
global d_gamemenu_load_game:function 118
d_gamemenu_load_game:
incbin EXE, 0x17D48, 118
global d_gamemenu_save_game:function 132
d_gamemenu_save_game:
incbin EXE, 0x17DBE, 132
global d_gamemenu_restart_town:function 10
d_gamemenu_restart_town:
incbin EXE, 0x17E42, 10
global d_gamemenu_options:function 32
d_gamemenu_options:
incbin EXE, 0x17E4C, 32
global d_gamemenu_get_music:function 25
d_gamemenu_get_music:
incbin EXE, 0x17E6C, 25
global d_gamemenu_sound_music_toggle:function 65
d_gamemenu_sound_music_toggle:
incbin EXE, 0x17E85, 65
global d_gamemenu_get_sound:function 25
d_gamemenu_get_sound:
incbin EXE, 0x17EC6, 25
global d_gamemenu_get_color_cycling:function 21
d_gamemenu_get_color_cycling:
incbin EXE, 0x17EDF, 21
global d_gamemenu_get_gamma:function 38
d_gamemenu_get_gamma:
incbin EXE, 0x17EF4, 38
global d_gamemenu_music_volume:function 137
d_gamemenu_music_volume:
incbin EXE, 0x17F1A, 137
global d_gamemenu_slider_music_sound:function 13
d_gamemenu_slider_music_sound:
incbin EXE, 0x17FA3, 13
global d_gamemenu_sound_volume:function 128
d_gamemenu_sound_volume:
incbin EXE, 0x17FB0, 128
global d_gamemenu_gamma:function 42
d_gamemenu_gamma:
incbin EXE, 0x18030, 42
global d_gamemenu_slider_gamma:function 16
d_gamemenu_slider_gamma:
incbin EXE, 0x1805A, 16
global d_gamemenu_color_cycling:function 33
d_gamemenu_color_cycling:
incbin EXE, 0x1806A, 33
global d_FillSolidBlockTbls:function 262
d_FillSolidBlockTbls:
incbin EXE, 0x1808B, 262
global d_gendung_418D91:function 1070
d_gendung_418D91:
incbin EXE, 0x18191, 1070
global d_gendung_4191BF:function 60
d_gendung_4191BF:
incbin EXE, 0x185BF, 60
global d_gendung_4191FB:function 127
d_gendung_4191FB:
incbin EXE, 0x185FB, 127
global d_gendung_get_dpiece_num_from_coord:function 72
d_gendung_get_dpiece_num_from_coord:
incbin EXE, 0x1867A, 72
global d_gendung_4192C2:function 73
d_gendung_4192C2:
incbin EXE, 0x186C2, 73
global d_SetDungeonMicros:function 319
d_SetDungeonMicros:
incbin EXE, 0x1870B, 319
global d_DRLG_InitTrans:function 45
d_DRLG_InitTrans:
incbin EXE, 0x1884A, 45
global d_DRLG_MRectTrans:function 89
d_DRLG_MRectTrans:
incbin EXE, 0x18877, 89
global d_DRLG_RectTrans:function 69
d_DRLG_RectTrans:
incbin EXE, 0x188D0, 69
global d_DRLG_CopyTrans:function 31
d_DRLG_CopyTrans:
incbin EXE, 0x18915, 31
global d_DRLG_ListTrans:function 49
d_DRLG_ListTrans:
incbin EXE, 0x18934, 49
global d_DRLG_AreaTrans:function 61
d_DRLG_AreaTrans:
incbin EXE, 0x18965, 61
global d_DRLG_InitSetPC:function 23
d_DRLG_InitSetPC:
incbin EXE, 0x189A2, 23
global d_DRLG_SetPC:function 83
d_DRLG_SetPC:
incbin EXE, 0x189B9, 83
global d_Make_SetPC:function 79
d_Make_SetPC:
incbin EXE, 0x18A0C, 79
global d_DRLG_WillThemeRoomFit:function 409
d_DRLG_WillThemeRoomFit:
incbin EXE, 0x18A5B, 409
global d_DRLG_CreateThemeRoom:function 1052
d_DRLG_CreateThemeRoom:
incbin EXE, 0x18BF4, 1052
global d_DRLG_PlaceThemeRooms:function 386
d_DRLG_PlaceThemeRooms:
incbin EXE, 0x19010, 386
global d_DRLG_HoldThemeRooms:function 141
d_DRLG_HoldThemeRooms:
incbin EXE, 0x19192, 141
global d_SkipThemeRoom:function 82
d_SkipThemeRoom:
incbin EXE, 0x1921F, 82
global d_InitLevels:function 26
d_InitLevels:
incbin EXE, 0x19271, 26
global d_gmenu_draw_pause:function 51
d_gmenu_draw_pause:
incbin EXE, 0x1928B, 51
global d_gmenu_print_text:function 89
d_gmenu_print_text:
incbin EXE, 0x192BE, 89
global d_FreeGMenu:function 89
d_FreeGMenu:
incbin EXE, 0x19317, 89
global d_gmenu_init_menu:function 120
d_gmenu_init_menu:
incbin EXE, 0x19370, 120
global d_gmenu_exception:function 12
d_gmenu_exception:
incbin EXE, 0x193E8, 12
global d_gmenu_call_proc:function 90
d_gmenu_call_proc:
incbin EXE, 0x193F4, 90
global d_gmenu_up_down:function 104
d_gmenu_up_down:
incbin EXE, 0x1944E, 104
global d_gmenu_draw:function 143
d_gmenu_draw:
incbin EXE, 0x194B6, 143
global d_gmenu_draw_menu_item:function 244
d_gmenu_draw_menu_item:
incbin EXE, 0x19545, 244
global d_gmenu_clear_buffer:function 57
d_gmenu_clear_buffer:
incbin EXE, 0x19639, 57
global d_gmenu_get_lfont:function 60
d_gmenu_get_lfont:
incbin EXE, 0x19672, 60
global d_gmenu_presskeys:function 124
d_gmenu_presskeys:
incbin EXE, 0x196AE, 124
global d_gmenu_left_right:function 80
d_gmenu_left_right:
incbin EXE, 0x1972A, 80
global d_gmenu_on_mouse_move:function 88
d_gmenu_on_mouse_move:
incbin EXE, 0x1977A, 88
global d_gmenu_valid_mouse_pos:function 47
d_gmenu_valid_mouse_pos:
incbin EXE, 0x197D2, 47
global d_gmenu_left_mouse:function 183
d_gmenu_left_mouse:
incbin EXE, 0x19801, 183
global d_gmenu_enable:function 14
d_gmenu_enable:
incbin EXE, 0x198B8, 14
global d_gmenu_slider_1:function 66
d_gmenu_slider_1:
incbin EXE, 0x198C6, 66
global d_gmenu_slider_get:function 61
d_gmenu_slider_get:
incbin EXE, 0x19908, 61
global d_gmenu_slider_3:function 14
d_gmenu_slider_3:
incbin EXE, 0x19945, 14
global d_InitHelp:function 18
d_InitHelp:
incbin EXE, 0x19953, 18
global d_DrawHelp:function 405
d_DrawHelp:
incbin EXE, 0x19965, 405
global d_DrawHelpLine:function 121
d_DrawHelpLine:
incbin EXE, 0x19AFA, 121
global d_DisplayHelp:function 28
d_DisplayHelp:
incbin EXE, 0x19B73, 28
global d_HelpScrollUp:function 16
d_HelpScrollUp:
incbin EXE, 0x19B8F, 16
global d_HelpScrollDown:function 20
d_HelpScrollDown:
incbin EXE, 0x19B9F, 20
global d_j_init_cpp_init:function 5
d_j_init_cpp_init:
incbin EXE, 0x19BB3, 5
global d_init_cpp_init:function 11
d_init_cpp_init:
incbin EXE, 0x19BB8, 11
global d_init_cleanup:function 137
d_init_cleanup:
incbin EXE, 0x19BC3, 137
global d_init_run_office_from_start_menu:function 109
d_init_run_office_from_start_menu:
incbin EXE, 0x19C4C, 109
global d_init_run_office:function 371
d_init_run_office:
incbin EXE, 0x19CB9, 371
global d_init_disable_screensaver:function 153
d_init_disable_screensaver:
incbin EXE, 0x19E2C, 153
global d_init_create_window:function 315
d_init_create_window:
incbin EXE, 0x19EC5, 315
global d_init_kill_mom_parent:function 33
d_init_kill_mom_parent:
incbin EXE, 0x1A000, 33
global d_init_find_mom_parent:function 80
d_init_find_mom_parent:
incbin EXE, 0x1A021, 80
global d_init_await_mom_parent_exit:function 48
d_init_await_mom_parent_exit:
incbin EXE, 0x1A071, 48
global d_init_archives:function 209
d_init_archives:
incbin EXE, 0x1A0A1, 209
global d_init_test_access:function 432
d_init_test_access:
incbin EXE, 0x1A172, 432
global d_init_strip_trailing_slash:function 24
d_init_strip_trailing_slash:
incbin EXE, 0x1A322, 24
global d_init_read_test_file:function 148
d_init_read_test_file:
incbin EXE, 0x1A33A, 148
global d_init_get_file_info:function 158
d_init_get_file_info:
incbin EXE, 0x1A3CE, 158
global d_MainWndProc:function 153
d_MainWndProc:
incbin EXE, 0x1A46C, 153
global d_init_activate_window:function 90
d_init_activate_window:
incbin EXE, 0x1A505, 90
global d_WindowProc:function 37
d_WindowProc:
incbin EXE, 0x1A55F, 37
global d_SetWindowProc:function 12
d_SetWindowProc:
incbin EXE, 0x1A584, 12
global d_j_interfac_cpp_init:function 5
d_j_interfac_cpp_init:
incbin EXE, 0x1A590, 5
global d_interfac_cpp_init:function 11
d_interfac_cpp_init:
incbin EXE, 0x1A595, 11
global d_interface_msg_pump:function 63
d_interface_msg_pump:
incbin EXE, 0x1A5A0, 63
global d_IncProgress:function 57
d_IncProgress:
incbin EXE, 0x1A5DF, 57
global d_DrawCutscene:function 117
d_DrawCutscene:
incbin EXE, 0x1A618, 117
global d_DrawProgress:function 41
d_DrawProgress:
incbin EXE, 0x1A68D, 41
global d_ShowProgress:function 831
d_ShowProgress:
incbin EXE, 0x1A6B6, 831
global d_FreeInterface:function 18
d_FreeInterface:
incbin EXE, 0x1A9F5, 18
global d_InitCutscene:function 525
d_InitCutscene:
incbin EXE, 0x1AA07, 525
global d_FreeInvGFX:function 18
d_FreeInvGFX:
incbin EXE, 0x1AC14, 18
global d_InitInv:function 75
d_InitInv:
incbin EXE, 0x1AC26, 75
global d_InvDrawSlotBack:function 83
d_InvDrawSlotBack:
incbin EXE, 0x1AC71, 83
global d_DrawInv:function 1948
d_DrawInv:
incbin EXE, 0x1ACC4, 1948
global d_DrawInvBelt:function 479
d_DrawInvBelt:
incbin EXE, 0x1B460, 479
global d_AutoPlace:function 308
d_AutoPlace:
incbin EXE, 0x1B63F, 308
global d_SpecialAutoPlace:function 365
d_SpecialAutoPlace:
incbin EXE, 0x1B773, 365
global d_GoldAutoPlace:function 457
d_GoldAutoPlace:
incbin EXE, 0x1B8E0, 457
global d_WeaponAutoPlace:function 157
d_WeaponAutoPlace:
incbin EXE, 0x1BAA9, 157
global d_SwapItem:function 61
d_SwapItem:
incbin EXE, 0x1BB46, 61
global d_CheckInvPaste:function 2892
d_CheckInvPaste:
incbin EXE, 0x1BB83, 2892
global d_CheckInvSwap:function 169
d_CheckInvSwap:
incbin EXE, 0x1C6CF, 169
global d_CheckInvCut:function 883
d_CheckInvCut:
incbin EXE, 0x1C778, 883
global d_inv_update_rem_item:function 55
d_inv_update_rem_item:
incbin EXE, 0x1CAEB, 55
global d_RemoveInvItem:function 238
d_RemoveInvItem:
incbin EXE, 0x1CB22, 238
global d_RemoveSpdBarItem:function 92
d_RemoveSpdBarItem:
incbin EXE, 0x1CC10, 92
global d_CheckInvItem:function 39
d_CheckInvItem:
incbin EXE, 0x1CC6C, 39
global d_CheckInvScrn:function 44
d_CheckInvScrn:
incbin EXE, 0x1CC93, 44
global d_CheckItemStats:function 76
d_CheckItemStats:
incbin EXE, 0x1CCBF, 76
global d_CheckBookLevel:function 116
d_CheckBookLevel:
incbin EXE, 0x1CD0B, 116
global d_CheckQuestItem:function 486
d_CheckQuestItem:
incbin EXE, 0x1CD7F, 486
global d_InvGetItem:function 276
d_InvGetItem:
incbin EXE, 0x1CF65, 276
global d_AutoGetItem:function 1162
d_AutoGetItem:
incbin EXE, 0x1D079, 1162
global d_FindGetItem:function 85
d_FindGetItem:
incbin EXE, 0x1D503, 85
global d_SyncGetItem:function 202
d_SyncGetItem:
incbin EXE, 0x1D558, 202
global d_CanPut:function 215
d_CanPut:
incbin EXE, 0x1D622, 215
global d_TryInvPut:function 195
d_TryInvPut:
incbin EXE, 0x1D6F9, 195
global d_DrawInvMsg:function 40
d_DrawInvMsg:
incbin EXE, 0x1D7BC, 40
global d_InvPutItem:function 597
d_InvPutItem:
incbin EXE, 0x1D7E4, 597
global d_SyncPutItem:function 676
d_SyncPutItem:
incbin EXE, 0x1DA39, 676
global d_CheckInvHLight:function 525
d_CheckInvHLight:
incbin EXE, 0x1DCDD, 525
global d_RemoveScroll:function 161
d_RemoveScroll:
incbin EXE, 0x1DEEA, 161
global d_UseScroll:function 183
d_UseScroll:
incbin EXE, 0x1DF8B, 183
global d_UseStaffCharge:function 61
d_UseStaffCharge:
incbin EXE, 0x1E042, 61
global d_UseStaff:function 68
d_UseStaff:
incbin EXE, 0x1E07F, 68
global d_StartGoldDrop:function 102
d_StartGoldDrop:
incbin EXE, 0x1E0C3, 102
global d_UseInvItem:function 632
d_UseInvItem:
incbin EXE, 0x1E129, 632
global d_DoTelekinesis:function 114
d_DoTelekinesis:
incbin EXE, 0x1E3A1, 114
global d_CalculateGold:function 85
d_CalculateGold:
incbin EXE, 0x1E413, 85
global d_DropItemBeforeTrig:function 46
d_DropItemBeforeTrig:
incbin EXE, 0x1E468, 46
global d_InitItemGFX:function 82
d_InitItemGFX:
incbin EXE, 0x1E496, 82
global d_ItemPlace:function 82
d_ItemPlace:
incbin EXE, 0x1E4E8, 82
global d_AddInitItems:function 276
d_AddInitItems:
incbin EXE, 0x1E53A, 276
global d_InitItems:function 210
d_InitItems:
incbin EXE, 0x1E64E, 210
global d_CalcPlrItemVals:function 1587
d_CalcPlrItemVals:
incbin EXE, 0x1E720, 1587
global d_CalcPlrScrolls:function 247
d_CalcPlrScrolls:
incbin EXE, 0x1ED53, 247
global d_CalcPlrStaff:function 77
d_CalcPlrStaff:
incbin EXE, 0x1EE4A, 77
global d_CalcSelfItems:function 250
d_CalcSelfItems:
incbin EXE, 0x1EE97, 250
global d_CalcPlrItemMin:function 101
d_CalcPlrItemMin:
incbin EXE, 0x1EF91, 101
global d_ItemMinStats:function 54
d_ItemMinStats:
incbin EXE, 0x1EFF6, 54
global d_CalcPlrBookVals:function 274
d_CalcPlrBookVals:
incbin EXE, 0x1F02C, 274
global d_CalcPlrInv:function 90
d_CalcPlrInv:
incbin EXE, 0x1F13E, 90
global d_SetPlrHandItem:function 256
d_SetPlrHandItem:
incbin EXE, 0x1F198, 256
global d_GetPlrHandSeed:function 12
d_GetPlrHandSeed:
incbin EXE, 0x1F298, 12
global d_GetGoldSeed:function 114
d_GetGoldSeed:
incbin EXE, 0x1F2A4, 114
global d_SetPlrHandSeed:function 3
d_SetPlrHandSeed:
incbin EXE, 0x1F316, 3
global d_SetPlrHandGoldCurs:function 53
d_SetPlrHandGoldCurs:
incbin EXE, 0x1F319, 53
global d_CreatePlrItems:function 426
d_CreatePlrItems:
incbin EXE, 0x1F34E, 426
global d_ItemSpaceOk:function 250
d_ItemSpaceOk:
incbin EXE, 0x1F4F8, 250
global d_GetItemSpace:function 246
d_GetItemSpace:
incbin EXE, 0x1F5F2, 246
global d_GetSuperItemSpace:function 142
d_GetSuperItemSpace:
incbin EXE, 0x1F6E8, 142
global d_GetSuperItemLoc:function 106
d_GetSuperItemLoc:
incbin EXE, 0x1F776, 106
global d_CalcItemValue:function 76
d_CalcItemValue:
incbin EXE, 0x1F7E0, 76
global d_GetBookSpell:function 232
d_GetBookSpell:
incbin EXE, 0x1F82C, 232
global d_GetStaffPower:function 465
d_GetStaffPower:
incbin EXE, 0x1F914, 465
global d_GetStaffSpell:function 357
d_GetStaffSpell:
incbin EXE, 0x1FAE5, 357
global d_GetItemAttrs:function 717
d_GetItemAttrs:
incbin EXE, 0x1FC4A, 717
global d_RndPL:function 17
d_RndPL:
incbin EXE, 0x1FF17, 17
global d_PLVal:function 64
d_PLVal:
incbin EXE, 0x1FF28, 64
global d_SaveItemPower:function 2695
d_SaveItemPower:
incbin EXE, 0x1FF68, 2695
global d_GetItemPower:function 813
d_GetItemPower:
incbin EXE, 0x209EF, 813
global d_GetItemBonus:function 165
d_GetItemBonus:
incbin EXE, 0x20D1C, 165
global d_SetupItem:function 138
d_SetupItem:
incbin EXE, 0x20DC1, 138
global d_RndItem:function 231
d_RndItem:
incbin EXE, 0x20E4B, 231
global d_RndUItem:function 248
d_RndUItem:
incbin EXE, 0x20F32, 248
global d_RndAllItems:function 141
d_RndAllItems:
incbin EXE, 0x2102A, 141
global d_RndTypeItems:function 138
d_RndTypeItems:
incbin EXE, 0x210B7, 138
global d_CheckUnique:function 208
d_CheckUnique:
incbin EXE, 0x21141, 208
global d_GetUniqueItem:function 331
d_GetUniqueItem:
incbin EXE, 0x21211, 331
global d_SpawnUnique:function 138
d_SpawnUnique:
incbin EXE, 0x2135C, 138
global d_ItemRndDur:function 62
d_ItemRndDur:
incbin EXE, 0x213E6, 62
global d_SetupAllItems:function 342
d_SetupAllItems:
incbin EXE, 0x21424, 342
global d_SpawnItem:function 278
d_SpawnItem:
incbin EXE, 0x2157A, 278
global d_CreateItem:function 155
d_CreateItem:
incbin EXE, 0x21690, 155
global d_CreateRndItem:function 165
d_CreateRndItem:
incbin EXE, 0x2172B, 165
global d_SetupAllUseful:function 109
d_SetupAllUseful:
incbin EXE, 0x217D0, 109
global d_CreateRndUseful:function 105
d_CreateRndUseful:
incbin EXE, 0x2183D, 105
global d_CreateTypeItem:function 164
d_CreateTypeItem:
incbin EXE, 0x218A6, 164
global d_RecreateItem:function 274
d_RecreateItem:
incbin EXE, 0x2194A, 274
global d_RecreateEar:function 313
d_RecreateEar:
incbin EXE, 0x21A5C, 313
global d_SpawnQuestItem:function 284
d_SpawnQuestItem:
incbin EXE, 0x21B95, 284
global d_SpawnRock:function 216
d_SpawnRock:
incbin EXE, 0x21CB1, 216
global d_RespawnItem:function 199
d_RespawnItem:
incbin EXE, 0x21D89, 199
global d_DeleteItem:function 52
d_DeleteItem:
incbin EXE, 0x21E50, 52
global d_ItemDoppel:function 90
d_ItemDoppel:
incbin EXE, 0x21E84, 90
global d_ProcessItems:function 212
d_ProcessItems:
incbin EXE, 0x21EDE, 212
global d_FreeItemGFX:function 29
d_FreeItemGFX:
incbin EXE, 0x21FB2, 29
global d_GetItemFrm:function 33
d_GetItemFrm:
incbin EXE, 0x21FCF, 33
global d_GetItemStr:function 115
d_GetItemStr:
incbin EXE, 0x21FF0, 115
global d_CheckIdentify:function 57
d_CheckIdentify:
incbin EXE, 0x22063, 57
global d_DoRepair:function 90
d_DoRepair:
incbin EXE, 0x2209C, 90
global d_RepairItem:function 118
d_RepairItem:
incbin EXE, 0x220F6, 118
global d_DoRecharge:function 113
d_DoRecharge:
incbin EXE, 0x2216C, 113
global d_RechargeItem:function 55
d_RechargeItem:
incbin EXE, 0x221DD, 55
global d_PrintItemOil:function 224
d_PrintItemOil:
incbin EXE, 0x22214, 224
global d_PrintItemPower:function 1596
d_PrintItemPower:
incbin EXE, 0x222F4, 1596
global d_DrawUBack:function 92
d_DrawUBack:
incbin EXE, 0x22930, 92
global d_PrintUString:function 207
d_PrintUString:
incbin EXE, 0x2298C, 207
global d_DrawULine:function 75
d_DrawULine:
incbin EXE, 0x22A5B, 75
global d_DrawUniqueInfo:function 310
d_DrawUniqueInfo:
incbin EXE, 0x22AA6, 310
global d_PrintItemMisc:function 248
d_PrintItemMisc:
incbin EXE, 0x22BDC, 248
global d_PrintItemDetails:function 525
d_PrintItemDetails:
incbin EXE, 0x22CD4, 525
global d_PrintItemDur:function 511
d_PrintItemDur:
incbin EXE, 0x22EE1, 511
global d_UseItem:function 1271
d_UseItem:
incbin EXE, 0x230E0, 1271
global d_StoreStatOk:function 69
d_StoreStatOk:
incbin EXE, 0x235D7, 69
global d_SmithItemOk:function 54
d_SmithItemOk:
incbin EXE, 0x2361C, 54
global d_RndSmithItem:function 111
d_RndSmithItem:
incbin EXE, 0x23652, 111
global d_BubbleSwapItem:function 52
d_BubbleSwapItem:
incbin EXE, 0x236C1, 52
global d_SortSmith:function 92
d_SortSmith:
incbin EXE, 0x236F5, 92
global d_SpawnSmith:function 207
d_SpawnSmith:
incbin EXE, 0x23751, 207
global d_PremiumItemOk:function 63
d_PremiumItemOk:
incbin EXE, 0x23820, 63
global d_RndPremiumItem:function 103
d_RndPremiumItem:
incbin EXE, 0x2385F, 103
global d_SpawnOnePremium:function 218
d_SpawnOnePremium:
incbin EXE, 0x238C6, 218
global d_SpawnPremium:function 204
d_SpawnPremium:
incbin EXE, 0x239A0, 204
global d_WitchItemOk:function 102
d_WitchItemOk:
incbin EXE, 0x23A6C, 102
global d_RndWitchItem:function 99
d_RndWitchItem:
incbin EXE, 0x23AD2, 99
global d_SortWitch:function 96
d_SortWitch:
incbin EXE, 0x23B35, 96
global d_WitchBookLevel:function 128
d_WitchBookLevel:
incbin EXE, 0x23B95, 128
global d_SpawnWitch:function 399
d_SpawnWitch:
incbin EXE, 0x23C15, 399
global d_RndBoyItem:function 95
d_RndBoyItem:
incbin EXE, 0x23DA4, 95
global d_SpawnBoy:function 152
d_SpawnBoy:
incbin EXE, 0x23E03, 152
global d_HealerItemOk:function 174
d_HealerItemOk:
incbin EXE, 0x23E9B, 174
global d_RndHealerItem:function 99
d_RndHealerItem:
incbin EXE, 0x23F49, 99
global d_SortHealer:function 96
d_SortHealer:
incbin EXE, 0x23FAC, 96
global d_SpawnHealer:function 331
d_SpawnHealer:
incbin EXE, 0x2400C, 331
global d_SpawnStoreGold:function 41
d_SpawnStoreGold:
incbin EXE, 0x24157, 41
global d_RecreateSmithItem:function 81
d_RecreateSmithItem:
incbin EXE, 0x24180, 81
global d_RecreatePremiumItem:function 107
d_RecreatePremiumItem:
incbin EXE, 0x241D1, 107
global d_RecreateBoyItem:function 101
d_RecreateBoyItem:
incbin EXE, 0x2423C, 101
global d_RecreateWitchItem:function 177
d_RecreateWitchItem:
incbin EXE, 0x242A1, 177
global d_RecreateHealerItem:function 102
d_RecreateHealerItem:
incbin EXE, 0x24352, 102
global d_RecreateTownItem:function 103
d_RecreateTownItem:
incbin EXE, 0x243B8, 103
global d_RecalcStoreStats:function 161
d_RecalcStoreStats:
incbin EXE, 0x2441F, 161
global d_ItemNoFlippy:function 47
d_ItemNoFlippy:
incbin EXE, 0x244C0, 47
global d_CreateSpellBook:function 201
d_CreateSpellBook:
incbin EXE, 0x244EF, 201
global d_CreateMagicItem:function 182
d_CreateMagicItem:
incbin EXE, 0x245B8, 182
global d_GetItemRecord:function 163
d_GetItemRecord:
incbin EXE, 0x2466E, 163
global d_NextItemRecord:function 70
d_NextItemRecord:
incbin EXE, 0x24711, 70
global d_SetItemRecord:function 71
d_SetItemRecord:
incbin EXE, 0x24757, 71
global d_PutItemRecord:function 165
d_PutItemRecord:
incbin EXE, 0x2479E, 165
global d_SetLightFX:function 119
d_SetLightFX:
incbin EXE, 0x24843, 119
global d_DoLighting:function 1014
d_DoLighting:
incbin EXE, 0x248BA, 1014
global d_DoUnLight:function 128
d_DoUnLight:
incbin EXE, 0x24CB0, 128
global d_DoUnVision:function 90
d_DoUnVision:
incbin EXE, 0x24D30, 90
global d_DoVision:function 649
d_DoVision:
incbin EXE, 0x24D8A, 649
global d_FreeLightTable:function 18
d_FreeLightTable:
incbin EXE, 0x25013, 18
global d_InitLightTable:function 16
d_InitLightTable:
incbin EXE, 0x25025, 16
global d_MakeLightTable:function 899
d_MakeLightTable:
incbin EXE, 0x25035, 899
global d_InitLightMax:function 22
d_InitLightMax:
incbin EXE, 0x253B8, 22
global d_InitLighting:function 30
d_InitLighting:
incbin EXE, 0x253CE, 30
global d_AddLight:function 106
d_AddLight:
incbin EXE, 0x253EC, 106
global d_AddUnLight:function 32
d_AddUnLight:
incbin EXE, 0x25456, 32
global d_ChangeLightRadius:function 79
d_ChangeLightRadius:
incbin EXE, 0x25476, 79
global d_ChangeLightXY:function 91
d_ChangeLightXY:
incbin EXE, 0x254C5, 91
global d_ChangeLightOff:function 91
d_ChangeLightOff:
incbin EXE, 0x25520, 91
global d_ChangeLight:function 108
d_ChangeLight:
incbin EXE, 0x2557B, 108
global d_ProcessLightList:function 249
d_ProcessLightList:
incbin EXE, 0x255E7, 249
global d_SavePreLighting:function 24
d_SavePreLighting:
incbin EXE, 0x256E0, 24
global d_InitVision:function 59
d_InitVision:
incbin EXE, 0x256F8, 59
global d_AddVision:function 109
d_AddVision:
incbin EXE, 0x25733, 109
global d_ChangeVisionRadius:function 65
d_ChangeVisionRadius:
incbin EXE, 0x257A0, 65
global d_ChangeVisionXY:function 74
d_ChangeVisionXY:
incbin EXE, 0x257E1, 74
global d_ProcessVisionList:function 244
d_ProcessVisionList:
incbin EXE, 0x2582B, 244
global d_lighting_color_cycling:function 69
d_lighting_color_cycling:
incbin EXE, 0x2591F, 69
global d_LoadGame:function 1406
d_LoadGame:
incbin EXE, 0x25964, 1406
global d_BLoad:function 14
d_BLoad:
incbin EXE, 0x25EE2, 14
global d_ILoad:function 60
d_ILoad:
incbin EXE, 0x25EF0, 60
global d_ILoad_2:function 60
d_ILoad_2:
incbin EXE, 0x25F2C, 60
global d_OLoad:function 23
d_OLoad:
incbin EXE, 0x25F68, 23
global d_LoadPlayer:function 42
d_LoadPlayer:
incbin EXE, 0x25F7F, 42
global d_LoadMonster:function 53
d_LoadMonster:
incbin EXE, 0x25FA9, 53
global d_LoadMissile:function 42
d_LoadMissile:
incbin EXE, 0x25FDE, 42
global d_LoadObject:function 34
d_LoadObject:
incbin EXE, 0x26008, 34
global d_LoadItem:function 53
d_LoadItem:
incbin EXE, 0x2602A, 53
global d_LoadPremium:function 42
d_LoadPremium:
incbin EXE, 0x2605F, 42
global d_LoadQuest:function 85
d_LoadQuest:
incbin EXE, 0x26089, 85
global d_LoadLighting:function 34
d_LoadLighting:
incbin EXE, 0x260DE, 34
global d_LoadVision:function 34
d_LoadVision:
incbin EXE, 0x26100, 34
global d_LoadPortal:function 35
d_LoadPortal:
incbin EXE, 0x26122, 35
global d_SaveGame:function 1214
d_SaveGame:
incbin EXE, 0x26145, 1214
global d_BSave:function 14
d_BSave:
incbin EXE, 0x26603, 14
global d_ISave:function 71
d_ISave:
incbin EXE, 0x26611, 71
global d_ISave_2:function 71
d_ISave_2:
incbin EXE, 0x26658, 71
global d_OSave:function 24
d_OSave:
incbin EXE, 0x2669F, 24
global d_SavePlayer:function 42
d_SavePlayer:
incbin EXE, 0x266B7, 42
global d_SaveMonster:function 42
d_SaveMonster:
incbin EXE, 0x266E1, 42
global d_SaveMissile:function 42
d_SaveMissile:
incbin EXE, 0x2670B, 42
global d_SaveObject:function 34
d_SaveObject:
incbin EXE, 0x26735, 34
global d_SaveItem:function 42
d_SaveItem:
incbin EXE, 0x26757, 42
global d_SavePremium:function 42
d_SavePremium:
incbin EXE, 0x26781, 42
global d_SaveQuest:function 89
d_SaveQuest:
incbin EXE, 0x267AB, 89
global d_SaveLighting:function 34
d_SaveLighting:
incbin EXE, 0x26804, 34
global d_SaveVision:function 34
d_SaveVision:
incbin EXE, 0x26826, 34
global d_SavePortal:function 35
d_SavePortal:
incbin EXE, 0x26848, 35
global d_SaveLevel:function 708
d_SaveLevel:
incbin EXE, 0x2686B, 708
global d_LoadLevel:function 707
d_LoadLevel:
incbin EXE, 0x26B2F, 707
global d_j_log_cpp_init_1:function 5
d_j_log_cpp_init_1:
incbin EXE, 0x26DF2, 5
global d_log_cpp_init_1:function 11
d_log_cpp_init_1:
incbin EXE, 0x26DF7, 11
global d_log_cpp_init_2:function 10
d_log_cpp_init_2:
incbin EXE, 0x26E02, 10
global d_log_init_mutex:function 12
d_log_init_mutex:
incbin EXE, 0x26E0C, 12
global d_j_log_cleanup_mutex:function 12
d_j_log_cleanup_mutex:
incbin EXE, 0x26E18, 12
global d_log_cleanup_mutex:function 12
d_log_cleanup_mutex:
incbin EXE, 0x26E24, 12
global d_log_flush:function 146
d_log_flush:
incbin EXE, 0x26E30, 146
global d_log_create:function 342
d_log_create:
incbin EXE, 0x26EC2, 342
global d_log_get_version:function 177
d_log_get_version:
incbin EXE, 0x27018, 177
global d_log_printf:function 172
d_log_printf:
incbin EXE, 0x270C9, 172
global d_log_dump_computer_info:function 153
d_log_dump_computer_info:
incbin EXE, 0x27175, 153
global d_j_mainmenu_cpp_init:function 5
d_j_mainmenu_cpp_init:
incbin EXE, 0x2720E, 5
global d_mainmenu_cpp_init:function 11
d_mainmenu_cpp_init:
incbin EXE, 0x27213, 11
global d_mainmenu_refresh_music:function 39
d_mainmenu_refresh_music:
incbin EXE, 0x2721E, 39
global d_mainmenu_create_hero:function 29
d_mainmenu_create_hero:
incbin EXE, 0x27245, 29
global d_mainmenu_select_hero_dialog:function 276
d_mainmenu_select_hero_dialog:
incbin EXE, 0x27262, 276
global d_mainmenu_loop:function 118
d_mainmenu_loop:
incbin EXE, 0x27376, 118
global d_mainmenu_single_player:function 14
d_mainmenu_single_player:
incbin EXE, 0x273EC, 14
global d_mainmenu_init_menu:function 54
d_mainmenu_init_menu:
incbin EXE, 0x273FA, 54
global d_mainmenu_multi_player:function 15
d_mainmenu_multi_player:
incbin EXE, 0x27430, 15
global d_mainmenu_play_intro:function 23
d_mainmenu_play_intro:
incbin EXE, 0x2743F, 23
global d_FreeQuestText:function 36
d_FreeQuestText:
incbin EXE, 0x27456, 36
global d_InitQuestText:function 42
d_InitQuestText:
incbin EXE, 0x2747A, 42
global d_InitQTextMsg:function 96
d_InitQTextMsg:
incbin EXE, 0x274A4, 96
global d_DrawQTextBack:function 92
d_DrawQTextBack:
incbin EXE, 0x27504, 92
global d_PrintQTextChr:function 162
d_PrintQTextChr:
incbin EXE, 0x27560, 162
global d_DrawQText:function 446
d_DrawQText:
incbin EXE, 0x27602, 446
global d_GetDamageAmt:function 1377
d_GetDamageAmt:
incbin EXE, 0x277C0, 1377
global d_CheckBlock:function 89
d_CheckBlock:
incbin EXE, 0x27D21, 89
global d_FindClosest:function 287
d_FindClosest:
incbin EXE, 0x27D7A, 287
global d_GetSpellLevel:function 43
d_GetSpellLevel:
incbin EXE, 0x27E99, 43
global d_GetDirection8:function 1578
d_GetDirection8:
incbin EXE, 0x27EC4, 1578
global d_GetDirection16:function 1680
d_GetDirection16:
incbin EXE, 0x284EE, 1680
global d_DeleteMissile:function 112
d_DeleteMissile:
incbin EXE, 0x28B7E, 112
global d_GetMissileVel:function 191
d_GetMissileVel:
incbin EXE, 0x28BEE, 191
global d_PutMissile:function 107
d_PutMissile:
incbin EXE, 0x28CAD, 107
global d_GetMissilePos:function 210
d_GetMissilePos:
incbin EXE, 0x28D18, 210
global d_MoveMissilePos:function 175
d_MoveMissilePos:
incbin EXE, 0x28DEA, 175
global d_MonsterTrapHit:function 418
d_MonsterTrapHit:
incbin EXE, 0x28E99, 418
global d_MonsterMHit:function 787
d_MonsterMHit:
incbin EXE, 0x2903B, 787
global d_PlayerMHit:function 953
d_PlayerMHit:
incbin EXE, 0x2934E, 953
global d_Plr2PlrMHit:function 724
d_Plr2PlrMHit:
incbin EXE, 0x29707, 724
global d_CheckMissileCol:function 762
d_CheckMissileCol:
incbin EXE, 0x299DB, 762
global d_SetMissAnim:function 132
d_SetMissAnim:
incbin EXE, 0x29CD5, 132
global d_SetMissDir:function 26
d_SetMissDir:
incbin EXE, 0x29D59, 26
global d_LoadMissileGFX:function 233
d_LoadMissileGFX:
incbin EXE, 0x29D73, 233
global d_InitMissileGFX:function 45
d_InitMissileGFX:
incbin EXE, 0x29E5C, 45
global d_FreeMissileGFX:function 105
d_FreeMissileGFX:
incbin EXE, 0x29E89, 105
global d_FreeMissiles:function 46
d_FreeMissiles:
incbin EXE, 0x29EF2, 46
global d_FreeMissiles2:function 46
d_FreeMissiles2:
incbin EXE, 0x29F20, 46
global d_InitMissiles:function 190
d_InitMissiles:
incbin EXE, 0x29F4E, 190
global d_AddLArrow:function 205
d_AddLArrow:
incbin EXE, 0x2A00C, 205
global d_AddArrow:function 209
d_AddArrow:
incbin EXE, 0x2A0D9, 209
global d_GetVileMissPos:function 158
d_GetVileMissPos:
incbin EXE, 0x2A1AA, 158
global d_AddRndTeleport:function 323
d_AddRndTeleport:
incbin EXE, 0x2A248, 323
global d_AddFirebolt:function 271
d_AddFirebolt:
incbin EXE, 0x2A38B, 271
global d_AddMagmaball:function 121
d_AddMagmaball:
incbin EXE, 0x2A49A, 121
global d_miss_null_33:function 70
d_miss_null_33:
incbin EXE, 0x2A513, 70
global d_AddTeleport:function 299
d_AddTeleport:
incbin EXE, 0x2A559, 299
global d_AddLightball:function 127
d_AddLightball:
incbin EXE, 0x2A684, 127
global d_AddFirewall:function 189
d_AddFirewall:
incbin EXE, 0x2A703, 189
global d_AddFireball:function 295
d_AddFireball:
incbin EXE, 0x2A7C0, 295
global d_AddLightctrl:function 108
d_AddLightctrl:
incbin EXE, 0x2A8E7, 108
global d_AddLightning:function 205
d_AddLightning:
incbin EXE, 0x2A953, 205
global d_AddMisexp:function 241
d_AddMisexp:
incbin EXE, 0x2AA20, 241
global d_AddWeapexp:function 107
d_AddWeapexp:
incbin EXE, 0x2AB11, 107
global d_CheckIfTrig:function 99
d_CheckIfTrig:
incbin EXE, 0x2AB7C, 99
global d_AddTown:function 541
d_AddTown:
incbin EXE, 0x2ABDF, 541
global d_AddFlash:function 197
d_AddFlash:
incbin EXE, 0x2ADFC, 197
global d_AddFlash2:function 194
d_AddFlash2:
incbin EXE, 0x2AEC1, 194
global d_AddManashield:function 119
d_AddManashield:
incbin EXE, 0x2AF83, 119
global d_AddFiremove:function 124
d_AddFiremove:
incbin EXE, 0x2AFFA, 124
global d_AddGuardian:function 546
d_AddGuardian:
incbin EXE, 0x2B076, 546
global d_AddChain:function 51
d_AddChain:
incbin EXE, 0x2B298, 51
global d_miss_null_11:function 51
d_miss_null_11:
incbin EXE, 0x2B2CB, 51
global d_miss_null_12:function 61
d_miss_null_12:
incbin EXE, 0x2B2FE, 61
global d_miss_null_13:function 63
d_miss_null_13:
incbin EXE, 0x2B33B, 63
global d_AddRhino:function 273
d_AddRhino:
incbin EXE, 0x2B37A, 273
global d_miss_null_32:function 220
d_miss_null_32:
incbin EXE, 0x2B48B, 220
global d_AddFlare:function 271
d_AddFlare:
incbin EXE, 0x2B567, 271
global d_AddAcid:function 120
d_AddAcid:
incbin EXE, 0x2B676, 120
global d_miss_null_1D:function 60
d_miss_null_1D:
incbin EXE, 0x2B6EE, 60
global d_AddAcidpud:function 100
d_AddAcidpud:
incbin EXE, 0x2B72A, 100
global d_AddStone:function 394
d_AddStone:
incbin EXE, 0x2B78E, 394
global d_AddGolem:function 194
d_AddGolem:
incbin EXE, 0x2B918, 194
global d_AddEtherealize:function 138
d_AddEtherealize:
incbin EXE, 0x2B9DA, 138
global d_miss_null_1F:function 19
d_miss_null_1F:
incbin EXE, 0x2BA64, 19
global d_miss_null_23:function 98
d_miss_null_23:
incbin EXE, 0x2BA77, 98
global d_AddBoom:function 83
d_AddBoom:
incbin EXE, 0x2BAD9, 83
global d_AddHeal:function 224
d_AddHeal:
incbin EXE, 0x2BB2C, 224
global d_AddHealOther:function 51
d_AddHealOther:
incbin EXE, 0x2BC0C, 51
global d_AddElement:function 259
d_AddElement:
incbin EXE, 0x2BC3F, 259
global d_AddIdentify:function 81
d_AddIdentify:
incbin EXE, 0x2BD42, 81
global d_AddFirewallC:function 354
d_AddFirewallC:
incbin EXE, 0x2BD93, 354
global d_AddInfra:function 103
d_AddInfra:
incbin EXE, 0x2BEF5, 103
global d_AddWave:function 75
d_AddWave:
incbin EXE, 0x2BF5C, 75
global d_AddNova:function 241
d_AddNova:
incbin EXE, 0x2BFA7, 241
global d_AddRepair:function 81
d_AddRepair:
incbin EXE, 0x2C098, 81
global d_AddRecharge:function 81
d_AddRecharge:
incbin EXE, 0x2C0E9, 81
global d_AddDisarm:function 51
d_AddDisarm:
incbin EXE, 0x2C13A, 51
global d_AddApoca:function 197
d_AddApoca:
incbin EXE, 0x2C16D, 197
global d_AddFlame:function 259
d_AddFlame:
incbin EXE, 0x2C232, 259
global d_AddFlamec:function 120
d_AddFlamec:
incbin EXE, 0x2C335, 120
global d_AddCbolt:function 235
d_AddCbolt:
incbin EXE, 0x2C3AD, 235
global d_AddHbolt:function 224
d_AddHbolt:
incbin EXE, 0x2C498, 224
global d_AddResurrect:function 55
d_AddResurrect:
incbin EXE, 0x2C578, 55
global d_AddResurrectBeam:function 68
d_AddResurrectBeam:
incbin EXE, 0x2C5AF, 68
global d_AddTelekinesis:function 51
d_AddTelekinesis:
incbin EXE, 0x2C5F3, 51
global d_AddBoneSpirit:function 235
d_AddBoneSpirit:
incbin EXE, 0x2C626, 235
global d_AddRportal:function 74
d_AddRportal:
incbin EXE, 0x2C711, 74
global d_AddDiabApoca:function 127
d_AddDiabApoca:
incbin EXE, 0x2C75B, 127
global d_AddMissile:function 457
d_AddMissile:
incbin EXE, 0x2C7DA, 457
global d_Sentfire:function 220
d_Sentfire:
incbin EXE, 0x2C9A3, 220
global d_MI_Dummy:function 1
d_MI_Dummy:
incbin EXE, 0x2CA7F, 1
global d_MI_Golem:function 327
d_MI_Golem:
incbin EXE, 0x2CA80, 327
global d_MI_SetManashield:function 11
d_MI_SetManashield:
incbin EXE, 0x2CBC7, 11
global d_MI_LArrow:function 766
d_MI_LArrow:
incbin EXE, 0x2CBD2, 766
global d_MI_Arrow:function 209
d_MI_Arrow:
incbin EXE, 0x2CED0, 209
global d_MI_Firebolt:function 697
d_MI_Firebolt:
incbin EXE, 0x2CFA1, 697
global d_MI_Lightball:function 232
d_MI_Lightball:
incbin EXE, 0x2D25A, 232
global d_mi_null_33:function 105
d_mi_null_33:
incbin EXE, 0x2D342, 105
global d_MI_Acidpud:function 115
d_MI_Acidpud:
incbin EXE, 0x2D3AB, 115
global d_MI_Firewall:function 369
d_MI_Firewall:
incbin EXE, 0x2D41E, 369
global d_MI_Fireball:function 1048
d_MI_Fireball:
incbin EXE, 0x2D58F, 1048
global d_MI_Lightctrl:function 500
d_MI_Lightctrl:
incbin EXE, 0x2D9A7, 500
global d_MI_Lightning:function 133
d_MI_Lightning:
incbin EXE, 0x2DB9B, 133
global d_MI_Town:function 427
d_MI_Town:
incbin EXE, 0x2DC20, 427
global d_MI_Flash:function 294
d_MI_Flash:
incbin EXE, 0x2DDCB, 294
global d_MI_Flash2:function 206
d_MI_Flash2:
incbin EXE, 0x2DEF1, 206
global d_MI_Manashield:function 602
d_MI_Manashield:
incbin EXE, 0x2DFBF, 602
global d_MI_Etherealize:function 228
d_MI_Etherealize:
incbin EXE, 0x2E219, 228
global d_MI_Firemove:function 459
d_MI_Firemove:
incbin EXE, 0x2E2FD, 459
global d_MI_Guardian:function 506
d_MI_Guardian:
incbin EXE, 0x2E4C8, 506
global d_MI_Chain:function 435
d_MI_Chain:
incbin EXE, 0x2E6C2, 435
global d_mi_null_11:function 52
d_mi_null_11:
incbin EXE, 0x2E875, 52
global d_MI_Weapexp:function 301
d_MI_Weapexp:
incbin EXE, 0x2E8A9, 301
global d_MI_Misexp:function 188
d_MI_Misexp:
incbin EXE, 0x2E9D6, 188
global d_MI_Acidsplat:function 145
d_MI_Acidsplat:
incbin EXE, 0x2EA92, 145
global d_MI_Teleport:function 265
d_MI_Teleport:
incbin EXE, 0x2EB23, 265
global d_MI_Stone:function 194
d_MI_Stone:
incbin EXE, 0x2EC2C, 194
global d_MI_Boom:function 97
d_MI_Boom:
incbin EXE, 0x2ECEE, 97
global d_MI_Rhino:function 385
d_MI_Rhino:
incbin EXE, 0x2ED4F, 385
global d_mi_null_32:function 420
d_mi_null_32:
incbin EXE, 0x2EED0, 420
global d_MI_FirewallC:function 367
d_MI_FirewallC:
incbin EXE, 0x2F074, 367
global d_MI_Infra:function 61
d_MI_Infra:
incbin EXE, 0x2F1E3, 61
global d_MI_Apoca:function 235
d_MI_Apoca:
incbin EXE, 0x2F220, 235
global d_MI_Wave:function 585
d_MI_Wave:
incbin EXE, 0x2F30B, 585
global d_MI_Nova:function 339
d_MI_Nova:
incbin EXE, 0x2F554, 339
global d_MI_Blodboil:function 17
d_MI_Blodboil:
incbin EXE, 0x2F6A7, 17
global d_MI_Flame:function 198
d_MI_Flame:
incbin EXE, 0x2F6B8, 198
global d_MI_Flamec:function 222
d_MI_Flamec:
incbin EXE, 0x2F77E, 222
global d_MI_Cbolt:function 390
d_MI_Cbolt:
incbin EXE, 0x2F85C, 390
global d_MI_Hbolt:function 317
d_MI_Hbolt:
incbin EXE, 0x2F9E2, 317
global d_MI_Element:function 889
d_MI_Element:
incbin EXE, 0x2FB1F, 889
global d_MI_Bonespirit:function 501
d_MI_Bonespirit:
incbin EXE, 0x2FE98, 501
global d_MI_ResurrectBeam:function 31
d_MI_ResurrectBeam:
incbin EXE, 0x3008D, 31
global d_MI_Rportal:function 302
d_MI_Rportal:
incbin EXE, 0x300AC, 302
global d_ProcessMissiles:function 347
d_ProcessMissiles:
incbin EXE, 0x301DA, 347
global d_missiles_process_charge:function 132
d_missiles_process_charge:
incbin EXE, 0x30335, 132
global d_ClearMissileSpot:function 38
d_ClearMissileSpot:
incbin EXE, 0x303B9, 38
global d_j_monster_cpp_init:function 5
d_j_monster_cpp_init:
incbin EXE, 0x303DF, 5
global d_monster_cpp_init:function 11
d_monster_cpp_init:
incbin EXE, 0x303E4, 11
global d_InitMonsterTRN:function 140
d_InitMonsterTRN:
incbin EXE, 0x303EF, 140
global d_InitLevelMonsters:function 84
d_InitLevelMonsters:
incbin EXE, 0x3047B, 84
global d_AddMonsterType:function 128
d_AddMonsterType:
incbin EXE, 0x304CF, 128
global d_GetLevelMTypes:function 682
d_GetLevelMTypes:
incbin EXE, 0x3054F, 682
global d_InitMonsterGFX:function 693
d_InitMonsterGFX:
incbin EXE, 0x307F9, 693
global d_ClearMVars:function 57
d_ClearMVars:
incbin EXE, 0x30AAE, 57
global d_InitMonster:function 900
d_InitMonster:
incbin EXE, 0x30AE7, 900
global d_ClrAllMonsters:function 165
d_ClrAllMonsters:
incbin EXE, 0x30E6B, 165
global d_MonstPlace:function 77
d_MonstPlace:
incbin EXE, 0x30F10, 77
global d_PlaceMonster:function 60
d_PlaceMonster:
incbin EXE, 0x30F5D, 60
global d_PlaceUniqueMonst:function 1263
d_PlaceUniqueMonst:
incbin EXE, 0x30F99, 1263
global d_PlaceQuestMonsters:function 626
d_PlaceQuestMonsters:
incbin EXE, 0x31488, 626
global d_PlaceGroup:function 651
d_PlaceGroup:
incbin EXE, 0x316FA, 651
global d_LoadDiabMonsts:function 178
d_LoadDiabMonsts:
incbin EXE, 0x31985, 178
global d_InitMonsters:function 518
d_InitMonsters:
incbin EXE, 0x31A37, 518
global d_PlaceUniques:function 209
d_PlaceUniques:
incbin EXE, 0x31C3D, 209
global d_SetMapMonsters:function 319
d_SetMapMonsters:
incbin EXE, 0x31D0E, 319
global d_DeleteMonster:function 36
d_DeleteMonster:
incbin EXE, 0x31E4D, 36
global d_AddMonster:function 80
d_AddMonster:
incbin EXE, 0x31E71, 80
global d_NewMonsterAnim:function 62
d_NewMonsterAnim:
incbin EXE, 0x31EC1, 62
global d_M_Ranged:function 39
d_M_Ranged:
incbin EXE, 0x31EFF, 39
global d_M_Talker:function 54
d_M_Talker:
incbin EXE, 0x31F26, 54
global d_M_Enemy:function 697
d_M_Enemy:
incbin EXE, 0x31F5C, 697
global d_M_GetDir:function 40
d_M_GetDir:
incbin EXE, 0x32215, 40
global d_M_CheckEFlag:function 96
d_M_CheckEFlag:
incbin EXE, 0x3223D, 96
global d_M_StartStand:function 140
d_M_StartStand:
incbin EXE, 0x3229D, 140
global d_M_StartDelay:function 38
d_M_StartDelay:
incbin EXE, 0x32329, 38
global d_M_StartSpStand:function 109
d_M_StartSpStand:
incbin EXE, 0x3234F, 109
global d_M_StartWalk:function 211
d_M_StartWalk:
incbin EXE, 0x323BC, 211
global d_M_StartWalk2:function 283
d_M_StartWalk2:
incbin EXE, 0x3248F, 283
global d_M_StartWalk3:function 332
d_M_StartWalk3:
incbin EXE, 0x325AA, 332
global d_M_StartAttack:function 113
d_M_StartAttack:
incbin EXE, 0x326F6, 113
global d_M_StartRAttack:function 136
d_M_StartRAttack:
incbin EXE, 0x32767, 136
global d_M_StartRSpAttack:function 145
d_M_StartRSpAttack:
incbin EXE, 0x327EF, 145
global d_M_StartSpAttack:function 116
d_M_StartSpAttack:
incbin EXE, 0x32880, 116
global d_M_StartEat:function 104
d_M_StartEat:
incbin EXE, 0x328F4, 104
global d_M_ClearSquares:function 191
d_M_ClearSquares:
incbin EXE, 0x3295C, 191
global d_M_GetKnockback:function 202
d_M_GetKnockback:
incbin EXE, 0x32A1B, 202
global d_M_StartHit:function 373
d_M_StartHit:
incbin EXE, 0x32AE5, 373
global d_M_DiabloDeath:function 498
d_M_DiabloDeath:
incbin EXE, 0x32C5A, 498
global d_M2MStartHit:function 384
d_M2MStartHit:
incbin EXE, 0x32E4C, 384
global d_MonstStartKill:function 502
d_MonstStartKill:
incbin EXE, 0x32FCC, 502
global d_M2MStartKill:function 517
d_M2MStartKill:
incbin EXE, 0x331C2, 517
global d_M_StartKill:function 126
d_M_StartKill:
incbin EXE, 0x333C7, 126
global d_M_SyncStartKill:function 155
d_M_SyncStartKill:
incbin EXE, 0x33445, 155
global d_M_StartFadein:function 205
d_M_StartFadein:
incbin EXE, 0x334E0, 205
global d_M_StartFadeout:function 197
d_M_StartFadeout:
incbin EXE, 0x335AD, 197
global d_M_StartHeal:function 152
d_M_StartHeal:
incbin EXE, 0x33672, 152
global d_M_ChangeLightOffset:function 106
d_M_ChangeLightOffset:
incbin EXE, 0x3370A, 106
global d_M_DoStand:function 127
d_M_DoStand:
incbin EXE, 0x33774, 127
global d_M_DoWalk:function 278
d_M_DoWalk:
incbin EXE, 0x337F3, 278
global d_M_DoWalk2:function 243
d_M_DoWalk2:
incbin EXE, 0x33909, 243
global d_M_DoWalk3:function 294
d_M_DoWalk3:
incbin EXE, 0x339FC, 294
global d_M_TryM2MHit:function 266
d_M_TryM2MHit:
incbin EXE, 0x33B22, 266
global d_M_TryH2HHit:function 1039
d_M_TryH2HHit:
incbin EXE, 0x33C2C, 1039
global d_M_DoAttack:function 386
d_M_DoAttack:
incbin EXE, 0x3403B, 386
global d_M_DoRAttack:function 245
d_M_DoRAttack:
incbin EXE, 0x341BD, 245
global d_M_DoRSpAttack:function 277
d_M_DoRSpAttack:
incbin EXE, 0x342B2, 277
global d_M_DoSAttack:function 183
d_M_DoSAttack:
incbin EXE, 0x343C7, 183
global d_M_DoFadein:function 101
d_M_DoFadein:
incbin EXE, 0x3447E, 101
global d_M_DoFadeout:function 130
d_M_DoFadeout:
incbin EXE, 0x344E3, 130
global d_M_DoHeal:function 144
d_M_DoHeal:
incbin EXE, 0x34565, 144
global d_M_DoTalk:function 645
d_M_DoTalk:
incbin EXE, 0x345F5, 645
global d_M_Teleport:function 321
d_M_Teleport:
incbin EXE, 0x3487A, 321
global d_M_DoGotHit:function 99
d_M_DoGotHit:
incbin EXE, 0x349BB, 99
global d_M_UpdateLeader:function 121
d_M_UpdateLeader:
incbin EXE, 0x34A1E, 121
global d_DoEnding:function 197
d_DoEnding:
incbin EXE, 0x34A97, 197
global d_PrepDoEnding:function 131
d_PrepDoEnding:
incbin EXE, 0x34B5C, 131
global d_M_DoDeath:function 269
d_M_DoDeath:
incbin EXE, 0x34BDF, 269
global d_M_DoSpStand:function 127
d_M_DoSpStand:
incbin EXE, 0x34CEC, 127
global d_M_DoDelay:function 169
d_M_DoDelay:
incbin EXE, 0x34D6B, 169
global d_M_DoStone:function 78
d_M_DoStone:
incbin EXE, 0x34E14, 78
global d_M_WalkDir:function 339
d_M_WalkDir:
incbin EXE, 0x34E62, 339
global d_GroupUnity:function 499
d_GroupUnity:
incbin EXE, 0x34FB5, 499
global d_M_CallWalk:function 269
d_M_CallWalk:
incbin EXE, 0x351A8, 269
global d_M_PathWalk:function 128
d_M_PathWalk:
incbin EXE, 0x352B5, 128
global d_M_CallWalk2:function 133
d_M_CallWalk2:
incbin EXE, 0x35335, 133
global d_M_DumbWalk:function 33
d_M_DumbWalk:
incbin EXE, 0x353BA, 33
global d_M_RoundWalk:function 214
d_M_RoundWalk:
incbin EXE, 0x353DB, 214
global d_MAI_Zombie:function 326
d_MAI_Zombie:
incbin EXE, 0x354B1, 326
global d_MAI_SkelSd:function 314
d_MAI_SkelSd:
incbin EXE, 0x355F7, 314
global d_MAI_Path:function 200
d_MAI_Path:
incbin EXE, 0x35731, 200
global d_MAI_Snake:function 662
d_MAI_Snake:
incbin EXE, 0x357F9, 662
global d_MAI_Bat:function 616
d_MAI_Bat:
incbin EXE, 0x35A8F, 616
global d_MAI_SkelBow:function 321
d_MAI_SkelBow:
incbin EXE, 0x35CF7, 321
global d_MAI_Fat:function 296
d_MAI_Fat:
incbin EXE, 0x35E38, 296
global d_MAI_Sneak:function 616
d_MAI_Sneak:
incbin EXE, 0x35F60, 616
global d_MAI_Fireman:function 548
d_MAI_Fireman:
incbin EXE, 0x361C8, 548
global d_MAI_Fallen:function 491
d_MAI_Fallen:
incbin EXE, 0x363EC, 491
global d_MAI_Cleaver:function 174
d_MAI_Cleaver:
incbin EXE, 0x365D7, 174
global d_MAI_Round:function 667
d_MAI_Round:
incbin EXE, 0x36685, 667
global d_MAI_GoatMc:function 8
d_MAI_GoatMc:
incbin EXE, 0x36920, 8
global d_MAI_Ranged:function 395
d_MAI_Ranged:
incbin EXE, 0x36928, 395
global d_MAI_GoatBow:function 10
d_MAI_GoatBow:
incbin EXE, 0x36AB3, 10
global d_MAI_Succ:function 11
d_MAI_Succ:
incbin EXE, 0x36ABD, 11
global d_MAI_AcidUniq:function 11
d_MAI_AcidUniq:
incbin EXE, 0x36AC8, 11
global d_MAI_Scav:function 644
d_MAI_Scav:
incbin EXE, 0x36AD3, 644
global d_MAI_Garg:function 308
d_MAI_Garg:
incbin EXE, 0x36D57, 308
global d_MAI_RoundRanged:function 776
d_MAI_RoundRanged:
incbin EXE, 0x36E8B, 776
global d_MAI_Magma:function 15
d_MAI_Magma:
incbin EXE, 0x37193, 15
global d_MAI_Storm:function 15
d_MAI_Storm:
incbin EXE, 0x371A2, 15
global d_MAI_Acid:function 15
d_MAI_Acid:
incbin EXE, 0x371B1, 15
global d_MAI_Diablo:function 15
d_MAI_Diablo:
incbin EXE, 0x371C0, 15
global d_MAI_RR2:function 783
d_MAI_RR2:
incbin EXE, 0x371CF, 783
global d_MAI_Mega:function 11
d_MAI_Mega:
incbin EXE, 0x374DE, 11
global d_MAI_Golum:function 539
d_MAI_Golum:
incbin EXE, 0x374E9, 539
global d_MAI_SkelKing:function 809
d_MAI_SkelKing:
incbin EXE, 0x37704, 809
global d_MAI_Rhino:function 754
d_MAI_Rhino:
incbin EXE, 0x37A2D, 754
global d_MAI_Counselor:function 858
d_MAI_Counselor:
incbin EXE, 0x37D1F, 858
global d_MAI_Garbud:function 261
d_MAI_Garbud:
incbin EXE, 0x38079, 261
global d_MAI_Zhar:function 324
d_MAI_Zhar:
incbin EXE, 0x3817E, 324
global d_MAI_SnotSpil:function 340
d_MAI_SnotSpil:
incbin EXE, 0x382C2, 340
global d_MAI_Lazurus:function 384
d_MAI_Lazurus:
incbin EXE, 0x38416, 384
global d_MAI_Lazhelp:function 189
d_MAI_Lazhelp:
incbin EXE, 0x38596, 189
global d_MAI_Lachdanan:function 229
d_MAI_Lachdanan:
incbin EXE, 0x38653, 229
global d_MAI_Warlord:function 225
d_MAI_Warlord:
incbin EXE, 0x38738, 225
global d_DeleteMonsterList:function 101
d_DeleteMonsterList:
incbin EXE, 0x38819, 101
global d_ProcessMonsters:function 839
d_ProcessMonsters:
incbin EXE, 0x3887E, 839
global d_FreeMonsters:function 108
d_FreeMonsters:
incbin EXE, 0x38BC5, 108
global d_DirOK:function 513
d_DirOK:
incbin EXE, 0x38C31, 513
global d_PosOkMissile:function 37
d_PosOkMissile:
incbin EXE, 0x38E32, 37
global d_CheckNoSolid:function 26
d_CheckNoSolid:
incbin EXE, 0x38E57, 26
global d_LineClearF:function 367
d_LineClearF:
incbin EXE, 0x38E71, 367
global d_LineClear:function 26
d_LineClear:
incbin EXE, 0x38FE0, 26
global d_LineClearF1:function 379
d_LineClearF1:
incbin EXE, 0x38FFA, 379
global d_SyncMonsterAnim:function 307
d_SyncMonsterAnim:
incbin EXE, 0x39175, 307
global d_M_FallenFear:function 234
d_M_FallenFear:
incbin EXE, 0x392A8, 234
global d_PrintMonstHistory:function 424
d_PrintMonstHistory:
incbin EXE, 0x39392, 424
global d_PrintUniqueHistory:function 135
d_PrintUniqueHistory:
incbin EXE, 0x3953A, 135
global d_MissToMonst:function 669
d_MissToMonst:
incbin EXE, 0x395C1, 669
global d_PosOkMonst:function 233
d_PosOkMonst:
incbin EXE, 0x3985E, 233
global d_PosOkMonst2:function 204
d_PosOkMonst2:
incbin EXE, 0x39947, 204
global d_PosOkMonst3:function 296
d_PosOkMonst3:
incbin EXE, 0x39A13, 296
global d_IsSkel:function 37
d_IsSkel:
incbin EXE, 0x39B3B, 37
global d_IsGoat:function 27
d_IsGoat:
incbin EXE, 0x39B60, 27
global d_M_SpawnSkel:function 173
d_M_SpawnSkel:
incbin EXE, 0x39B7B, 173
global d_ActivateSpawn:function 81
d_ActivateSpawn:
incbin EXE, 0x39C28, 81
global d_SpawnSkeleton:function 256
d_SpawnSkeleton:
incbin EXE, 0x39C79, 256
global d_PreSpawnSkeleton:function 147
d_PreSpawnSkeleton:
incbin EXE, 0x39D79, 147
global d_TalktoMonster:function 206
d_TalktoMonster:
incbin EXE, 0x39E0C, 206
global d_SpawnGolum:function 306
d_SpawnGolum:
incbin EXE, 0x39EDA, 306
global d_CanTalkToMonst:function 55
d_CanTalkToMonst:
incbin EXE, 0x3A00C, 55
global d_CheckMonsterHit:function 114
d_CheckMonsterHit:
incbin EXE, 0x3A043, 114
global d_encode_enemy:function 25
d_encode_enemy:
incbin EXE, 0x3A0B5, 25
global d_decode_enemy:function 101
d_decode_enemy:
incbin EXE, 0x3A0CE, 101
global d_j_movie_cpp_init:function 5
d_j_movie_cpp_init:
incbin EXE, 0x3A133, 5
global d_movie_cpp_init:function 11
d_movie_cpp_init:
incbin EXE, 0x3A138, 11
global d_play_movie:function 251
d_play_movie:
incbin EXE, 0x3A143, 251
global d_MovieWndProc:function 82
d_MovieWndProc:
incbin EXE, 0x3A23E, 82
global d_j_mpqapi_cpp_init:function 5
d_j_mpqapi_cpp_init:
incbin EXE, 0x3A290, 5
global d_mpqapi_cpp_init:function 11
d_mpqapi_cpp_init:
incbin EXE, 0x3A295, 11
global d_mpqapi_set_hidden:function 60
d_mpqapi_set_hidden:
incbin EXE, 0x3A2A0, 60
global d_mpqapi_store_creation_time:function 115
d_mpqapi_store_creation_time:
incbin EXE, 0x3A2DC, 115
global d_mpqapi_reg_load_modification_time:function 86
d_mpqapi_reg_load_modification_time:
incbin EXE, 0x3A34F, 86
global d_mpqapi_xor_buf:function 31
d_mpqapi_xor_buf:
incbin EXE, 0x3A3A5, 31
global d_mpqapi_reg_store_modification_time:function 57
d_mpqapi_reg_store_modification_time:
incbin EXE, 0x3A3C4, 57
global d_j_mpqapi_remove_hash_entry:function 5
d_j_mpqapi_remove_hash_entry:
incbin EXE, 0x3A3FD, 5
global d_mpqapi_remove_hash_entry:function 82
d_mpqapi_remove_hash_entry:
incbin EXE, 0x3A402, 82
global d_mpqapi_free_block:function 144
d_mpqapi_free_block:
incbin EXE, 0x3A454, 144
global d_mpqapi_new_block:function 63
d_mpqapi_new_block:
incbin EXE, 0x3A4E4, 63
global d_mpqapi_get_hash_index_of_path:function 48
d_mpqapi_get_hash_index_of_path:
incbin EXE, 0x3A523, 48
global d_mpqapi_get_hash_index:function 106
d_mpqapi_get_hash_index:
incbin EXE, 0x3A553, 106
global d_mpqapi_remove_hash_entries:function 59
d_mpqapi_remove_hash_entries:
incbin EXE, 0x3A5BD, 59
global d_mpqapi_write_file:function 69
d_mpqapi_write_file:
incbin EXE, 0x3A5F8, 69
global d_mpqapi_add_file:function 218
d_mpqapi_add_file:
incbin EXE, 0x3A63D, 218
global d_mpqapi_write_file_contents:function 517
d_mpqapi_write_file_contents:
incbin EXE, 0x3A717, 517
global d_mpqapi_find_free_block:function 84
d_mpqapi_find_free_block:
incbin EXE, 0x3A91C, 84
global d_mpqapi_rename:function 63
d_mpqapi_rename:
incbin EXE, 0x3A970, 63
global d_mpqapi_has_file:function 16
d_mpqapi_has_file:
incbin EXE, 0x3A9AF, 16
global d_mpqapi_open_archive:function 466
d_mpqapi_open_archive:
incbin EXE, 0x3A9BF, 466
global d_mpqapi_parse_archive_header:function 241
d_mpqapi_parse_archive_header:
incbin EXE, 0x3AB91, 241
global d_mpqapi_close_archive:function 123
d_mpqapi_close_archive:
incbin EXE, 0x3AC82, 123
global d_mpqapi_store_modified_time:function 115
d_mpqapi_store_modified_time:
incbin EXE, 0x3ACFD, 115
global d_mpqapi_flush_and_close:function 90
d_mpqapi_flush_and_close:
incbin EXE, 0x3AD70, 90
global d_mpqapi_write_header:function 150
d_mpqapi_write_header:
incbin EXE, 0x3ADCA, 150
global d_mpqapi_write_block_table:function 139
d_mpqapi_write_block_table:
incbin EXE, 0x3AE60, 139
global d_mpqapi_write_hash_table:function 142
d_mpqapi_write_hash_table:
incbin EXE, 0x3AEEB, 142
global d_mpqapi_can_seek:function 43
d_mpqapi_can_seek:
incbin EXE, 0x3AF79, 43
global d_j_msg_cpp_init:function 5
d_j_msg_cpp_init:
incbin EXE, 0x3AFA4, 5
global d_msg_cpp_init:function 11
d_msg_cpp_init:
incbin EXE, 0x3AFA9, 11
global d_msg_send_drop_pkt:function 27
d_msg_send_drop_pkt:
incbin EXE, 0x3AFB4, 27
global d_msg_send_packet:function 98
d_msg_send_packet:
incbin EXE, 0x3AFCF, 98
global d_msg_get_next_packet:function 60
d_msg_get_next_packet:
incbin EXE, 0x3B031, 60
global d_msg_wait_resync:function 128
d_msg_wait_resync:
incbin EXE, 0x3B06D, 128
global d_msg_free_packets:function 44
d_msg_free_packets:
incbin EXE, 0x3B0ED, 44
global d_msg_wait_for_turns:function 210
d_msg_wait_for_turns:
incbin EXE, 0x3B119, 210
global d_msg_process_net_packets:function 34
d_msg_process_net_packets:
incbin EXE, 0x3B1EB, 34
global d_msg_pre_packet:function 103
d_msg_pre_packet:
incbin EXE, 0x3B20D, 103
global d_DeltaExportData:function 183
d_DeltaExportData:
incbin EXE, 0x3B274, 183
global d_DeltaExportItem:function 48
d_DeltaExportItem:
incbin EXE, 0x3B32B, 48
global d_DeltaExportObject:function 20
d_DeltaExportObject:
incbin EXE, 0x3B35B, 20
global d_DeltaExportMonster:function 50
d_DeltaExportMonster:
incbin EXE, 0x3B36F, 50
global d_DeltaExportJunk:function 120
d_DeltaExportJunk:
incbin EXE, 0x3B3A1, 120
global d_msg_comp_level:function 28
d_msg_comp_level:
incbin EXE, 0x3B419, 28
global d_delta_init:function 71
d_delta_init:
incbin EXE, 0x3B435, 71
global d_delta_kill_monster:function 70
d_delta_kill_monster:
incbin EXE, 0x3B47C, 70
global d_delta_monster_hp:function 48
d_delta_monster_hp:
incbin EXE, 0x3B4C2, 48
global d_delta_sync_monster:function 66
d_delta_sync_monster:
incbin EXE, 0x3B4F2, 66
global d_delta_sync_golem:function 73
d_delta_sync_golem:
incbin EXE, 0x3B534, 73
global d_delta_leave_sync:function 210
d_delta_leave_sync:
incbin EXE, 0x3B57D, 210
global d_delta_portal_inited:function 14
d_delta_portal_inited:
incbin EXE, 0x3B64F, 14
global d_delta_quest_inited:function 14
d_delta_quest_inited:
incbin EXE, 0x3B65D, 14
global d_DeltaAddItem:function 263
d_DeltaAddItem:
incbin EXE, 0x3B66B, 263
global d_DeltaSaveLevel:function 72
d_DeltaSaveLevel:
incbin EXE, 0x3B772, 72
global d_DeltaLoadLevel:function 1209
d_DeltaLoadLevel:
incbin EXE, 0x3B7BA, 1209
global d_NetSendCmd:function 30
d_NetSendCmd:
incbin EXE, 0x3BC73, 30
global d_NetSendCmdGolem:function 54
d_NetSendCmdGolem:
incbin EXE, 0x3BC91, 54
global d_NetSendCmdLoc:function 44
d_NetSendCmdLoc:
incbin EXE, 0x3BCC7, 44
global d_NetSendCmdLocParam1:function 53
d_NetSendCmdLocParam1:
incbin EXE, 0x3BCF3, 53
global d_NetSendCmdLocParam2:function 61
d_NetSendCmdLocParam2:
incbin EXE, 0x3BD28, 61
global d_NetSendCmdLocParam3:function 70
d_NetSendCmdLocParam3:
incbin EXE, 0x3BD65, 70
global d_NetSendCmdParam1:function 40
d_NetSendCmdParam1:
incbin EXE, 0x3BDAB, 40
global d_NetSendCmdParam2:function 49
d_NetSendCmdParam2:
incbin EXE, 0x3BDD3, 49
global d_NetSendCmdParam3:function 57
d_NetSendCmdParam3:
incbin EXE, 0x3BE04, 57
global d_NetSendCmdQuest:function 71
d_NetSendCmdQuest:
incbin EXE, 0x3BE3D, 71
global d_NetSendCmdGItem:function 389
d_NetSendCmdGItem:
incbin EXE, 0x3BE84, 389
global d_NetSendCmdGItem2:function 107
d_NetSendCmdGItem2:
incbin EXE, 0x3C009, 107
global d_NetSendCmdReq2:function 91
d_NetSendCmdReq2:
incbin EXE, 0x3C074, 91
global d_NetSendCmdExtra:function 41
d_NetSendCmdExtra:
incbin EXE, 0x3C0CF, 41
global d_NetSendCmdPItem:function 355
d_NetSendCmdPItem:
incbin EXE, 0x3C0F8, 355
global d_NetSendCmdChItem:function 87
d_NetSendCmdChItem:
incbin EXE, 0x3C25B, 87
global d_NetSendCmdDelItem:function 34
d_NetSendCmdDelItem:
incbin EXE, 0x3C2B2, 34
global d_NetSendCmdDItem:function 357
d_NetSendCmdDItem:
incbin EXE, 0x3C2D4, 357
global d_NetSendCmdDamage:function 43
d_NetSendCmdDamage:
incbin EXE, 0x3C439, 43
global d_NetSendCmdString:function 57
d_NetSendCmdString:
incbin EXE, 0x3C464, 57
global d_RemovePlrPortal:function 31
d_RemovePlrPortal:
incbin EXE, 0x3C49D, 31
global d_ParseCmd:function 1398
d_ParseCmd:
incbin EXE, 0x3C4BC, 1398
global d_DeltaImportData:function 136
d_DeltaImportData:
incbin EXE, 0x3CA32, 136
global d_DeltaImportItem:function 59
d_DeltaImportItem:
incbin EXE, 0x3CABA, 59
global d_DeltaImportObject:function 20
d_DeltaImportObject:
incbin EXE, 0x3CAF5, 20
global d_DeltaImportMonster:function 61
d_DeltaImportMonster:
incbin EXE, 0x3CB09, 61
global d_DeltaImportJunk:function 171
d_DeltaImportJunk:
incbin EXE, 0x3CB46, 171
global d_On_SYNCDATA:function 11
d_On_SYNCDATA:
incbin EXE, 0x3CBF1, 11
global d_On_WALKXY:function 78
d_On_WALKXY:
incbin EXE, 0x3CBFC, 78
global d_On_ADDSTR:function 49
d_On_ADDSTR:
incbin EXE, 0x3CC4A, 49
global d_On_ADDMAG:function 49
d_On_ADDMAG:
incbin EXE, 0x3CC7B, 49
global d_On_ADDDEX:function 49
d_On_ADDDEX:
incbin EXE, 0x3CCAC, 49
global d_On_ADDVIT:function 49
d_On_ADDVIT:
incbin EXE, 0x3CCDD, 49
global d_On_SBSPELL:function 111
d_On_SBSPELL:
incbin EXE, 0x3CD0E, 111
global d_msg_errorf:function 71
d_msg_errorf:
incbin EXE, 0x3CD7D, 71
global d_On_GOTOGETITEM:function 82
d_On_GOTOGETITEM:
incbin EXE, 0x3CDC4, 82
global d_On_REQUESTGITEM:function 208
d_On_REQUESTGITEM:
incbin EXE, 0x3CE16, 208
global d_i_own_level:function 71
d_i_own_level:
incbin EXE, 0x3CEE6, 71
global d_On_GETITEM:function 272
d_On_GETITEM:
incbin EXE, 0x3CF2D, 272
global d_delta_get_item:function 259
d_delta_get_item:
incbin EXE, 0x3D03D, 259
global d_On_GOTOAGETITEM:function 82
d_On_GOTOAGETITEM:
incbin EXE, 0x3D140, 82
global d_On_REQUESTAGITEM:function 206
d_On_REQUESTAGITEM:
incbin EXE, 0x3D192, 206
global d_On_AGETITEM:function 270
d_On_AGETITEM:
incbin EXE, 0x3D260, 270
global d_On_ITEMEXTRA:function 91
d_On_ITEMEXTRA:
incbin EXE, 0x3D36E, 91
global d_On_PUTITEM:function 261
d_On_PUTITEM:
incbin EXE, 0x3D3C9, 261
global d_delta_put_item:function 171
d_delta_put_item:
incbin EXE, 0x3D4CE, 171
global d_check_update_plr:function 26
d_check_update_plr:
incbin EXE, 0x3D579, 26
global d_On_SYNCPUTITEM:function 241
d_On_SYNCPUTITEM:
incbin EXE, 0x3D593, 241
global d_On_RESPAWNITEM:function 166
d_On_RESPAWNITEM:
incbin EXE, 0x3D684, 166
global d_On_ATTACKXY:function 92
d_On_ATTACKXY:
incbin EXE, 0x3D72A, 92
global d_On_SATTACKXY:function 79
d_On_SATTACKXY:
incbin EXE, 0x3D786, 79
global d_On_RATTACKXY:function 79
d_On_RATTACKXY:
incbin EXE, 0x3D7D5, 79
global d_On_SPELLXYD:function 174
d_On_SPELLXYD:
incbin EXE, 0x3D824, 174
global d_On_SPELLXY:function 164
d_On_SPELLXY:
incbin EXE, 0x3D8D2, 164
global d_On_TSPELLXY:function 164
d_On_TSPELLXY:
incbin EXE, 0x3D976, 164
global d_On_OPOBJXY:function 112
d_On_OPOBJXY:
incbin EXE, 0x3DA1A, 112
global d_On_DISARMXY:function 112
d_On_DISARMXY:
incbin EXE, 0x3DA8A, 112
global d_On_OPOBJT:function 56
d_On_OPOBJT:
incbin EXE, 0x3DAFA, 56
global d_On_ATTACKID:function 173
d_On_ATTACKID:
incbin EXE, 0x3DB32, 173
global d_On_ATTACKPID:function 97
d_On_ATTACKPID:
incbin EXE, 0x3DBDF, 97
global d_On_RATTACKID:function 69
d_On_RATTACKID:
incbin EXE, 0x3DC40, 69
global d_On_RATTACKPID:function 69
d_On_RATTACKPID:
incbin EXE, 0x3DC85, 69
global d_On_SPELLID:function 154
d_On_SPELLID:
incbin EXE, 0x3DCCA, 154
global d_On_SPELLPID:function 154
d_On_SPELLPID:
incbin EXE, 0x3DD64, 154
global d_On_TSPELLID:function 154
d_On_TSPELLID:
incbin EXE, 0x3DDFE, 154
global d_On_TSPELLPID:function 154
d_On_TSPELLPID:
incbin EXE, 0x3DE98, 154
global d_On_KNOCKBACK:function 66
d_On_KNOCKBACK:
incbin EXE, 0x3DF32, 66
global d_On_RESURRECT:function 48
d_On_RESURRECT:
incbin EXE, 0x3DF74, 48
global d_On_HEALOTHER:function 49
d_On_HEALOTHER:
incbin EXE, 0x3DFA4, 49
global d_On_TALKXY:function 82
d_On_TALKXY:
incbin EXE, 0x3DFD5, 82
global d_On_NEWLVL:function 52
d_On_NEWLVL:
incbin EXE, 0x3E027, 52
global d_On_WARP:function 95
d_On_WARP:
incbin EXE, 0x3E05B, 95
global d_On_MONSTDEATH:function 105
d_On_MONSTDEATH:
incbin EXE, 0x3E0BA, 105
global d_On_KILLGOLEM:function 102
d_On_KILLGOLEM:
incbin EXE, 0x3E123, 102
global d_On_AWAKEGOLEM:function 180
d_On_AWAKEGOLEM:
incbin EXE, 0x3E189, 180
global d_On_MONSTDAMAGE:function 184
d_On_MONSTDAMAGE:
incbin EXE, 0x3E23D, 184
global d_On_PLRDEAD:function 56
d_On_PLRDEAD:
incbin EXE, 0x3E2F5, 56
global d_On_PLRDAMAGE:function 176
d_On_PLRDAMAGE:
incbin EXE, 0x3E32D, 176
global d_On_OPENDOOR:function 86
d_On_OPENDOOR:
incbin EXE, 0x3E3DD, 86
global d_delta_sync_object:function 37
d_delta_sync_object:
incbin EXE, 0x3E433, 37
global d_On_CLOSEDOOR:function 86
d_On_CLOSEDOOR:
incbin EXE, 0x3E458, 86
global d_On_OPERATEOBJ:function 86
d_On_OPERATEOBJ:
incbin EXE, 0x3E4AE, 86
global d_On_PLROPOBJ:function 88
d_On_PLROPOBJ:
incbin EXE, 0x3E504, 88
global d_On_BREAKOBJ:function 84
d_On_BREAKOBJ:
incbin EXE, 0x3E55C, 84
global d_On_CHANGEPLRITEMS:function 64
d_On_CHANGEPLRITEMS:
incbin EXE, 0x3E5B0, 64
global d_On_DELPLRITEMS:function 46
d_On_DELPLRITEMS:
incbin EXE, 0x3E5F0, 46
global d_On_PLRLEVEL:function 58
d_On_PLRLEVEL:
incbin EXE, 0x3E61E, 58
global d_On_DROPITEM:function 55
d_On_DROPITEM:
incbin EXE, 0x3E658, 55
global d_On_SEND_PLRINFO:function 58
d_On_SEND_PLRINFO:
incbin EXE, 0x3E68F, 58
global d_On_ACK_PLRINFO:function 5
d_On_ACK_PLRINFO:
incbin EXE, 0x3E6C9, 5
global d_On_PLAYER_JOINLEVEL:function 378
d_On_PLAYER_JOINLEVEL:
incbin EXE, 0x3E6CE, 378
global d_On_ACTIVATEPORTAL:function 217
d_On_ACTIVATEPORTAL:
incbin EXE, 0x3E848, 217
global d_delta_open_portal:function 59
d_delta_open_portal:
incbin EXE, 0x3E921, 59
global d_On_DEACTIVATEPORTAL:function 62
d_On_DEACTIVATEPORTAL:
incbin EXE, 0x3E95C, 62
global d_On_RETOWN:function 57
d_On_RETOWN:
incbin EXE, 0x3E99A, 57
global d_On_SETSTR:function 57
d_On_SETSTR:
incbin EXE, 0x3E9D3, 57
global d_On_SETDEX:function 57
d_On_SETDEX:
incbin EXE, 0x3EA0C, 57
global d_On_SETMAG:function 57
d_On_SETMAG:
incbin EXE, 0x3EA45, 57
global d_On_SETVIT:function 57
d_On_SETVIT:
incbin EXE, 0x3EA7E, 57
global d_On_STRING:function 53
d_On_STRING:
incbin EXE, 0x3EAB7, 53
global d_On_SYNCQUEST:function 66
d_On_SYNCQUEST:
incbin EXE, 0x3EAEC, 66
global d_On_ENDSHIELD:function 119
d_On_ENDSHIELD:
incbin EXE, 0x3EB2E, 119
global d_On_DEBUG:function 4
d_On_DEBUG:
incbin EXE, 0x3EBA5, 4
global d_On_NOVA:function 111
d_On_NOVA:
incbin EXE, 0x3EBA9, 111
global d_On_SETSHIELD:function 24
d_On_SETSHIELD:
incbin EXE, 0x3EC18, 24
global d_On_REMSHIELD:function 25
d_On_REMSHIELD:
incbin EXE, 0x3EC30, 25
global d_j_msgcmd_cpp_init_1:function 5
d_j_msgcmd_cpp_init_1:
incbin EXE, 0x3EC49, 5
global d_msgcmd_cpp_init_1:function 11
d_msgcmd_cpp_init_1:
incbin EXE, 0x3EC4E, 11
global d_msgcmd_cpp_init_2:function 10
d_msgcmd_cpp_init_2:
incbin EXE, 0x3EC59, 10
global d_msgcmd_init_event:function 10
d_msgcmd_init_event:
incbin EXE, 0x3EC63, 10
global d_msgcmd_cleanup_chatcmd_atexit:function 12
d_msgcmd_cleanup_chatcmd_atexit:
incbin EXE, 0x3EC6D, 12
global d_msgcmd_cleanup_chatcmd:function 20
d_msgcmd_cleanup_chatcmd:
incbin EXE, 0x3EC79, 20
global d_msgcmd_cmd_cleanup:function 10
d_msgcmd_cmd_cleanup:
incbin EXE, 0x3EC8D, 10
global d_msgcmd_send_chat:function 61
d_msgcmd_send_chat:
incbin EXE, 0x3EC97, 61
global d_msgcmd_add_server_cmd_W:function 17
d_msgcmd_add_server_cmd_W:
incbin EXE, 0x3ECD4, 17
global d_msgcmd_add_server_cmd:function 59
d_msgcmd_add_server_cmd:
incbin EXE, 0x3ECE5, 59
global d_msgcmd_init_chatcmd:function 22
d_msgcmd_init_chatcmd:
incbin EXE, 0x3ED20, 22
global d_msgcmd_free_event:function 40
d_msgcmd_free_event:
incbin EXE, 0x3ED36, 40
global d_msgcmd_delete_server_cmd_W:function 59
d_msgcmd_delete_server_cmd_W:
incbin EXE, 0x3ED5E, 59
global d_msgcmd_alloc_event:function 76
d_msgcmd_alloc_event:
incbin EXE, 0x3ED99, 76
global d_msgcmd_remove_event:function 47
d_msgcmd_remove_event:
incbin EXE, 0x3EDE5, 47
global d_msgcmd_event_type:function 113
d_msgcmd_event_type:
incbin EXE, 0x3EE14, 113
global d_msgcmd_cleanup_chatcmd_1:function 19
d_msgcmd_cleanup_chatcmd_1:
incbin EXE, 0x3EE85, 19
global d_msgcmd_cleanup_extern_msg:function 44
d_msgcmd_cleanup_extern_msg:
incbin EXE, 0x3EE98, 44
global d_j_multi_cpp_init:function 5
d_j_multi_cpp_init:
incbin EXE, 0x3EEC4, 5
global d_multi_cpp_init:function 11
d_multi_cpp_init:
incbin EXE, 0x3EEC9, 11
global d_multi_msg_add:function 14
d_multi_msg_add:
incbin EXE, 0x3EED4, 14
global d_NetSendLoPri:function 41
d_NetSendLoPri:
incbin EXE, 0x3EEE2, 41
global d_multi_copy_packet:function 66
d_multi_copy_packet:
incbin EXE, 0x3EF0B, 66
global d_multi_send_packet:function 104
d_multi_send_packet:
incbin EXE, 0x3EF4D, 104
global d_NetRecvPlrData:function 186
d_NetRecvPlrData:
incbin EXE, 0x3EFB5, 186
global d_NetSendHiPri:function 184
d_NetSendHiPri:
incbin EXE, 0x3F06F, 184
global d_multi_recv_packet:function 105
d_multi_recv_packet:
incbin EXE, 0x3F127, 105
global d_multi_send_msg_packet:function 126
d_multi_send_msg_packet:
incbin EXE, 0x3F190, 126
global d_multi_msg_countdown:function 47
d_multi_msg_countdown:
incbin EXE, 0x3F20E, 47
global d_multi_parse_turn:function 72
d_multi_parse_turn:
incbin EXE, 0x3F23D, 72
global d_multi_handle_turn_upper_bit:function 50
d_multi_handle_turn_upper_bit:
incbin EXE, 0x3F285, 50
global d_multi_player_left:function 19
d_multi_player_left:
incbin EXE, 0x3F2B7, 19
global d_multi_clear_left_tbl:function 68
d_multi_clear_left_tbl:
incbin EXE, 0x3F2CA, 68
global d_multi_player_left_msg:function 143
d_multi_player_left_msg:
incbin EXE, 0x3F30E, 143
global d_multi_net_ping:function 19
d_multi_net_ping:
incbin EXE, 0x3F39D, 19
global d_multi_handle_delta:function 168
d_multi_handle_delta:
incbin EXE, 0x3F3B0, 168
global d_multi_check_pkt_valid:function 8
d_multi_check_pkt_valid:
incbin EXE, 0x3F458, 8
global d_multi_mon_seeds:function 51
d_multi_mon_seeds:
incbin EXE, 0x3F460, 51
global d_multi_begin_timeout:function 149
d_multi_begin_timeout:
incbin EXE, 0x3F493, 149
global d_multi_check_drop_player:function 43
d_multi_check_drop_player:
incbin EXE, 0x3F528, 43
global d_multi_process_network_packets:function 714
d_multi_process_network_packets:
incbin EXE, 0x3F553, 714
global d_multi_handle_all_packets:function 39
d_multi_handle_all_packets:
incbin EXE, 0x3F81D, 39
global d_multi_process_tmsgs:function 51
d_multi_process_tmsgs:
incbin EXE, 0x3F844, 51
global d_multi_send_zero_packet:function 230
d_multi_send_zero_packet:
incbin EXE, 0x3F877, 230
global d_NetClose:function 71
d_NetClose:
incbin EXE, 0x3F95D, 71
global d_multi_event_handler:function 72
d_multi_event_handler:
incbin EXE, 0x3F9A4, 72
global d_multi_handle_events:function 168
d_multi_handle_events:
incbin EXE, 0x3F9EC, 168
global d_NetInit:function 766
d_NetInit:
incbin EXE, 0x3FA94, 766
global d_multi_clear_pkt:function 8
d_multi_clear_pkt:
incbin EXE, 0x3FD92, 8
global d_multi_send_pinfo:function 59
d_multi_send_pinfo:
incbin EXE, 0x3FD9A, 59
global d_InitNewSeed:function 48
d_InitNewSeed:
incbin EXE, 0x3FDD5, 48
global d_SetupLocalCoords:function 150
d_SetupLocalCoords:
incbin EXE, 0x3FE05, 150
global d_multi_init_single:function 110
d_multi_init_single:
incbin EXE, 0x3FE9B, 110
global d_multi_init_multi:function 210
d_multi_init_multi:
incbin EXE, 0x3FF09, 210
global d_multi_upgrade:function 60
d_multi_upgrade:
incbin EXE, 0x3FFDB, 60
global d_multi_player_joins:function 407
d_multi_player_joins:
incbin EXE, 0x40017, 407
global d_j_nthread_cpp_init_1:function 5
d_j_nthread_cpp_init_1:
incbin EXE, 0x401AE, 5
global d_nthread_cpp_init_1:function 11
d_nthread_cpp_init_1:
incbin EXE, 0x401B3, 11
global d_nthread_cpp_init_2:function 10
d_nthread_cpp_init_2:
incbin EXE, 0x401BE, 10
global d_nthread_init_mutex:function 12
d_nthread_init_mutex:
incbin EXE, 0x401C8, 12
global d_nthread_cleanup_mutex_atexit:function 12
d_nthread_cleanup_mutex_atexit:
incbin EXE, 0x401D4, 12
global d_nthread_cleanup_mutex:function 12
d_nthread_cleanup_mutex:
incbin EXE, 0x401E0, 12
global d_nthread_terminate_game:function 60
d_nthread_terminate_game:
incbin EXE, 0x401EC, 60
global d_nthread_send_and_recv_turn:function 130
d_nthread_send_and_recv_turn:
incbin EXE, 0x40228, 130
global d_nthread_recv_turns:function 172
d_nthread_recv_turns:
incbin EXE, 0x402AA, 172
global d_nthread_set_turn_upper_bit:function 11
d_nthread_set_turn_upper_bit:
incbin EXE, 0x40356, 11
global d_nthread_start:function 366
d_nthread_start:
incbin EXE, 0x40361, 366
global d_nthread_handler:function 118
d_nthread_handler:
incbin EXE, 0x404CF, 118
global d_nthread_cleanup:function 127
d_nthread_cleanup:
incbin EXE, 0x40545, 127
global d_nthread_ignore_mutex:function 43
d_nthread_ignore_mutex:
incbin EXE, 0x405C4, 43
global d_nthread_has_500ms_passed:function 46
d_nthread_has_500ms_passed:
incbin EXE, 0x405EF, 46
global d_InitObjectGFX:function 250
d_InitObjectGFX:
incbin EXE, 0x4061D, 250
global d_FreeObjectGFX:function 46
d_FreeObjectGFX:
incbin EXE, 0x40717, 46
global d_RndLocOk:function 91
d_RndLocOk:
incbin EXE, 0x40745, 91
global d_InitRndLocObj:function 215
d_InitRndLocObj:
incbin EXE, 0x407A0, 215
global d_InitRndLocBigObj:function 269
d_InitRndLocBigObj:
incbin EXE, 0x40877, 269
global d_InitRndLocObj5x5:function 183
d_InitRndLocObj5x5:
incbin EXE, 0x40984, 183
global d_ClrAllObjects:function 109
d_ClrAllObjects:
incbin EXE, 0x40A3B, 109
global d_AddTortures:function 247
d_AddTortures:
incbin EXE, 0x40AA8, 247
global d_AddCandles:function 73
d_AddCandles:
incbin EXE, 0x40B9F, 73
global d_AddBookLever:function 284
d_AddBookLever:
incbin EXE, 0x40BE8, 284
global d_InitRndBarrels:function 252
d_InitRndBarrels:
incbin EXE, 0x40D04, 252
global d_AddL1Objs:function 152
d_AddL1Objs:
incbin EXE, 0x40E00, 152
global d_AddL2Objs:function 126
d_AddL2Objs:
incbin EXE, 0x40E98, 126
global d_AddL3Objs:function 116
d_AddL3Objs:
incbin EXE, 0x40F16, 116
global d_WallTrapLocOk:function 22
d_WallTrapLocOk:
incbin EXE, 0x40F8A, 22
global d_AddL2Torches:function 236
d_AddL2Torches:
incbin EXE, 0x40FA0, 236
global d_TorchLocOK:function 39
d_TorchLocOK:
incbin EXE, 0x4108C, 39
global d_AddObjTraps:function 421
d_AddObjTraps:
incbin EXE, 0x410B3, 421
global d_AddChestTraps:function 140
d_AddChestTraps:
incbin EXE, 0x41258, 140
global d_LoadMapObjects:function 203
d_LoadMapObjects:
incbin EXE, 0x412E4, 203
global d_LoadMapObjs:function 135
d_LoadMapObjs:
incbin EXE, 0x413AF, 135
global d_AddDiabObjs:function 188
d_AddDiabObjs:
incbin EXE, 0x41436, 188
global d_AddStoryBooks:function 216
d_AddStoryBooks:
incbin EXE, 0x414F2, 216
global d_AddHookedBodies:function 213
d_AddHookedBodies:
incbin EXE, 0x415CA, 213
global d_AddL4Goodies:function 119
d_AddL4Goodies:
incbin EXE, 0x4169F, 119
global d_AddLazStand:function 258
d_AddLazStand:
incbin EXE, 0x41716, 258
global d_InitObjects:function 941
d_InitObjects:
incbin EXE, 0x41818, 941
global d_SetMapObjects:function 358
d_SetMapObjects:
incbin EXE, 0x41BC5, 358
global d_DeleteObject:function 80
d_DeleteObject:
incbin EXE, 0x41D2B, 80
global d_SetupObject:function 290
d_SetupObject:
incbin EXE, 0x41D7B, 290
global d_SetObjMapRange:function 52
d_SetObjMapRange:
incbin EXE, 0x41E9D, 52
global d_SetBookMsg:function 10
d_SetBookMsg:
incbin EXE, 0x41ED1, 10
global d_AddL1Door:function 81
d_AddL1Door:
incbin EXE, 0x41EDB, 81
global d_AddSCambBook:function 73
d_AddSCambBook:
incbin EXE, 0x41F2C, 73
global d_AddChest:function 178
d_AddChest:
incbin EXE, 0x41F75, 178
global d_AddL2Door:function 59
d_AddL2Door:
incbin EXE, 0x42027, 59
global d_AddL3Door:function 59
d_AddL3Door:
incbin EXE, 0x42062, 59
global d_AddSarc:function 81
d_AddSarc:
incbin EXE, 0x4209D, 81
global d_AddFlameTrap:function 40
d_AddFlameTrap:
incbin EXE, 0x420EE, 40
global d_AddFlameLvr:function 25
d_AddFlameLvr:
incbin EXE, 0x42116, 25
global d_AddTrap:function 91
d_AddTrap:
incbin EXE, 0x4212F, 91
global d_AddObjLight:function 55
d_AddObjLight:
incbin EXE, 0x4218A, 55
global d_AddBarrel:function 78
d_AddBarrel:
incbin EXE, 0x421C1, 78
global d_AddShrine:function 163
d_AddShrine:
incbin EXE, 0x4220F, 163
global d_AddBookcase:function 29
d_AddBookcase:
incbin EXE, 0x422B2, 29
global d_AddPurifyingFountain:function 57
d_AddPurifyingFountain:
incbin EXE, 0x422CF, 57
global d_AddArmorStand:function 50
d_AddArmorStand:
incbin EXE, 0x42308, 50
global d_AddDecap:function 46
d_AddDecap:
incbin EXE, 0x4233A, 46
global d_AddVilebook:function 32
d_AddVilebook:
incbin EXE, 0x42368, 32
global d_AddMagicCircle:function 41
d_AddMagicCircle:
incbin EXE, 0x42388, 41
global d_AddBookstand:function 19
d_AddBookstand:
incbin EXE, 0x423B1, 19
global d_AddPedistal:function 56
d_AddPedistal:
incbin EXE, 0x423C4, 56
global d_AddStoryBook:function 146
d_AddStoryBook:
incbin EXE, 0x423FC, 146
global d_AddWeaponRack:function 50
d_AddWeaponRack:
incbin EXE, 0x4248E, 50
global d_AddTorturedBody:function 46
d_AddTorturedBody:
incbin EXE, 0x424C0, 46
global d_GetRndObjLoc:function 138
d_GetRndObjLoc:
incbin EXE, 0x424EE, 138
global d_AddMushPatch:function 92
d_AddMushPatch:
incbin EXE, 0x42578, 92
global d_AddSlainHero:function 43
d_AddSlainHero:
incbin EXE, 0x425D4, 43
global d_AddObject:function 716
d_AddObject:
incbin EXE, 0x425FF, 716
global d_Obj_Light:function 234
d_Obj_Light:
incbin EXE, 0x428CB, 234
global d_Obj_Circle:function 370
d_Obj_Circle:
incbin EXE, 0x429B5, 370
global d_Obj_StopAnim:function 35
d_Obj_StopAnim:
incbin EXE, 0x42B27, 35
global d_Obj_Door:function 131
d_Obj_Door:
incbin EXE, 0x42B4A, 131
global d_Obj_Sarc:function 25
d_Obj_Sarc:
incbin EXE, 0x42BCD, 25
global d_ActivateTrapLine:function 111
d_ActivateTrapLine:
incbin EXE, 0x42BE6, 111
global d_Obj_FlameTrap:function 273
d_Obj_FlameTrap:
incbin EXE, 0x42C55, 273
global d_Obj_Trap:function 367
d_Obj_Trap:
incbin EXE, 0x42D66, 367
global d_Obj_BCrossDamage:function 253
d_Obj_BCrossDamage:
incbin EXE, 0x42ED5, 253
global d_ProcessObjects:function 407
d_ProcessObjects:
incbin EXE, 0x42FD2, 407
global d_ObjSetMicro:function 129
d_ObjSetMicro:
incbin EXE, 0x43169, 129
global d_objects_set_door_piece:function 120
d_objects_set_door_piece:
incbin EXE, 0x431EA, 120
global d_ObjSetMini:function 120
d_ObjSetMini:
incbin EXE, 0x43262, 120
global d_ObjL1Special:function 236
d_ObjL1Special:
incbin EXE, 0x432DA, 236
global d_ObjL2Special:function 252
d_ObjL2Special:
incbin EXE, 0x433C6, 252
global d_DoorSet:function 388
d_DoorSet:
incbin EXE, 0x434C2, 388
global d_RedoPlayerVision:function 53
d_RedoPlayerVision:
incbin EXE, 0x43646, 53
global d_OperateL1RDoor:function 449
d_OperateL1RDoor:
incbin EXE, 0x4367B, 449
global d_OperateL1LDoor:function 471
d_OperateL1LDoor:
incbin EXE, 0x4383C, 471
global d_OperateL2RDoor:function 354
d_OperateL2RDoor:
incbin EXE, 0x43A13, 354
global d_OperateL2LDoor:function 354
d_OperateL2LDoor:
incbin EXE, 0x43B75, 354
global d_OperateL3RDoor:function 357
d_OperateL3RDoor:
incbin EXE, 0x43CD7, 357
global d_OperateL3LDoor:function 357
d_OperateL3LDoor:
incbin EXE, 0x43E3C, 357
global d_MonstCheckDoors:function 546
d_MonstCheckDoors:
incbin EXE, 0x43FA1, 546
global d_ObjChangeMap:function 219
d_ObjChangeMap:
incbin EXE, 0x441C3, 219
global d_ObjChangeMapResync:function 177
d_ObjChangeMapResync:
incbin EXE, 0x4429E, 177
global d_OperateL1Door:function 143
d_OperateL1Door:
incbin EXE, 0x4434F, 143
global d_OperateLever:function 206
d_OperateLever:
incbin EXE, 0x443DE, 206
global d_OperateBook:function 549
d_OperateBook:
incbin EXE, 0x444AC, 549
global d_OperateBookLever:function 434
d_OperateBookLever:
incbin EXE, 0x446D1, 434
global d_OperateSChambBk:function 215
d_OperateSChambBk:
incbin EXE, 0x44883, 215
global d_OperateChest:function 393
d_OperateChest:
incbin EXE, 0x4495A, 393
global d_OperateMushPatch:function 213
d_OperateMushPatch:
incbin EXE, 0x44AE3, 213
global d_OperateInnSignChest:function 200
d_OperateInnSignChest:
incbin EXE, 0x44BB8, 200
global d_OperateSlainHero:function 212
d_OperateSlainHero:
incbin EXE, 0x44C80, 212
global d_OperateTrapLvr:function 183
d_OperateTrapLvr:
incbin EXE, 0x44D54, 183
global d_OperateSarc:function 209
d_OperateSarc:
incbin EXE, 0x44E0B, 209
global d_OperateL2Door:function 144
d_OperateL2Door:
incbin EXE, 0x44EDC, 144
global d_OperateL3Door:function 144
d_OperateL3Door:
incbin EXE, 0x44F6C, 144
global d_OperatePedistal:function 355
d_OperatePedistal:
incbin EXE, 0x44FFC, 355
global d_TryDisarm:function 212
d_TryDisarm:
incbin EXE, 0x4515F, 212
global d_ItemMiscIdIdx:function 24
d_ItemMiscIdIdx:
incbin EXE, 0x45233, 24
global d_OperateShrine:function 4127
d_OperateShrine:
incbin EXE, 0x4524B, 4127
global d_OperateSkelBook:function 158
d_OperateSkelBook:
incbin EXE, 0x4626A, 158
global d_OperateBookCase:function 224
d_OperateBookCase:
incbin EXE, 0x46308, 224
global d_OperateDecap:function 94
d_OperateDecap:
incbin EXE, 0x463E8, 94
global d_OperateArmorStand:function 198
d_OperateArmorStand:
incbin EXE, 0x46446, 198
global d_FindValidShrine:function 83
d_FindValidShrine:
incbin EXE, 0x4650C, 83
global d_OperateGoatShrine:function 75
d_OperateGoatShrine:
incbin EXE, 0x4655F, 75
global d_OperateCauldron:function 82
d_OperateCauldron:
incbin EXE, 0x465AA, 82
global d_OperateFountains:function 689
d_OperateFountains:
incbin EXE, 0x465FC, 689
global d_OperateWeaponRack:function 171
d_OperateWeaponRack:
incbin EXE, 0x468AD, 171
global d_OperateStoryBook:function 99
d_OperateStoryBook:
incbin EXE, 0x46958, 99
global d_OperateLazStand:function 101
d_OperateLazStand:
incbin EXE, 0x469BB, 101
global d_OperateObject:function 786
d_OperateObject:
incbin EXE, 0x46A20, 786
global d_SyncOpL1Door:function 113
d_SyncOpL1Door:
incbin EXE, 0x46D32, 113
global d_SyncOpL2Door:function 114
d_SyncOpL2Door:
incbin EXE, 0x46DA3, 114
global d_SyncOpL3Door:function 114
d_SyncOpL3Door:
incbin EXE, 0x46E15, 114
global d_SyncOpObject:function 422
d_SyncOpObject:
incbin EXE, 0x46E87, 422
global d_BreakCrux:function 194
d_BreakCrux:
incbin EXE, 0x4702D, 194
global d_BreakBarrel:function 628
d_BreakBarrel:
incbin EXE, 0x470EF, 628
global d_BreakObject:function 140
d_BreakObject:
incbin EXE, 0x47363, 140
global d_SyncBreakObj:function 33
d_SyncBreakObj:
incbin EXE, 0x473EF, 33
global d_SyncL1Doors:function 171
d_SyncL1Doors:
incbin EXE, 0x47410, 171
global d_SyncCrux:function 126
d_SyncCrux:
incbin EXE, 0x474BB, 126
global d_SyncLever:function 42
d_SyncLever:
incbin EXE, 0x47539, 42
global d_SyncQSTLever:function 111
d_SyncQSTLever:
incbin EXE, 0x47563, 111
global d_SyncPedistal:function 198
d_SyncPedistal:
incbin EXE, 0x475D2, 198
global d_SyncL2Doors:function 134
d_SyncL2Doors:
incbin EXE, 0x47698, 134
global d_SyncL3Doors:function 146
d_SyncL3Doors:
incbin EXE, 0x4771E, 146
global d_SyncObjectAnim:function 174
d_SyncObjectAnim:
incbin EXE, 0x477B0, 174
global d_GetObjectStr:function 759
d_GetObjectStr:
incbin EXE, 0x4785E, 759
global d_j_pack_cpp_init:function 5
d_j_pack_cpp_init:
incbin EXE, 0x47B55, 5
global d_pack_cpp_init:function 11
d_pack_cpp_init:
incbin EXE, 0x47B5A, 11
global d_PackPlayer:function 494
d_PackPlayer:
incbin EXE, 0x47B65, 494
global d_PackItem:function 267
d_PackItem:
incbin EXE, 0x47D53, 267
global d_VerifyGoldSeeds:function 114
d_VerifyGoldSeeds:
incbin EXE, 0x47E5E, 114
global d_UnPackPlayer:function 632
d_UnPackPlayer:
incbin EXE, 0x47ED0, 632
global d_UnPackItem:function 173
d_UnPackItem:
incbin EXE, 0x48148, 173
global d_j_palette_cpp_init:function 5
d_j_palette_cpp_init:
incbin EXE, 0x481F5, 5
global d_palette_cpp_init:function 11
d_palette_cpp_init:
incbin EXE, 0x481FA, 11
global d_SaveGamma:function 46
d_SaveGamma:
incbin EXE, 0x48205, 46
global d_palette_init:function 120
d_palette_init:
incbin EXE, 0x48233, 120
global d_LoadGamma:function 117
d_LoadGamma:
incbin EXE, 0x482AB, 117
global d_LoadSysPal:function 169
d_LoadSysPal:
incbin EXE, 0x48320, 169
global d_LoadPalette:function 92
d_LoadPalette:
incbin EXE, 0x483C9, 92
global d_LoadRndLvlPal:function 71
d_LoadRndLvlPal:
incbin EXE, 0x48425, 71
global d_ResetPal:function 43
d_ResetPal:
incbin EXE, 0x4846C, 43
global d_IncreaseGamma:function 57
d_IncreaseGamma:
incbin EXE, 0x48497, 57
global d_palette_update:function 55
d_palette_update:
incbin EXE, 0x484D0, 55
global d_ApplyGamma:function 201
d_ApplyGamma:
incbin EXE, 0x48507, 201
global d_DecreaseGamma:function 57
d_DecreaseGamma:
incbin EXE, 0x485D0, 57
global d_UpdateGamma:function 53
d_UpdateGamma:
incbin EXE, 0x48609, 53
global d_BlackPalette:function 7
d_BlackPalette:
incbin EXE, 0x4863E, 7
global d_SetFadeLevel:function 107
d_SetFadeLevel:
incbin EXE, 0x48645, 107
global d_PaletteFadeIn:function 86
d_PaletteFadeIn:
incbin EXE, 0x486B0, 86
global d_PaletteFadeOut:function 48
d_PaletteFadeOut:
incbin EXE, 0x48706, 48
global d_palette_update_caves:function 98
d_palette_update_caves:
incbin EXE, 0x48736, 98
global d_palette_update_quest_palette:function 46
d_palette_update_quest_palette:
incbin EXE, 0x48798, 46
global d_palette_get_colour_cycling:function 6
d_palette_get_colour_cycling:
incbin EXE, 0x487C6, 6
global d_palette_set_color_cycling:function 8
d_palette_set_color_cycling:
incbin EXE, 0x487CC, 8
global d_FindPath:function 255
d_FindPath:
incbin EXE, 0x487D4, 255
global d_path_get_h_cost:function 49
d_path_get_h_cost:
incbin EXE, 0x488D3, 49
global d_path_check_equal:function 24
d_path_check_equal:
incbin EXE, 0x48904, 24
global d_GetNextPath:function 42
d_GetNextPath:
incbin EXE, 0x4891C, 42
global d_path_solid_pieces:function 167
d_path_solid_pieces:
incbin EXE, 0x48946, 167
global d_path_get_path:function 130
d_path_get_path:
incbin EXE, 0x489ED, 130
global d_path_parent_path:function 299
d_path_parent_path:
incbin EXE, 0x48A6F, 299
global d_path_get_node1:function 25
d_path_get_node1:
incbin EXE, 0x48B9A, 25
global d_path_get_node2:function 25
d_path_get_node2:
incbin EXE, 0x48BB3, 25
global d_path_next_node:function 43
d_path_next_node:
incbin EXE, 0x48BCC, 43
global d_path_set_coords:function 153
d_path_set_coords:
incbin EXE, 0x48BF7, 153
global d_path_push_active_step:function 19
d_path_push_active_step:
incbin EXE, 0x48C90, 19
global d_path_pop_active_step:function 19
d_path_pop_active_step:
incbin EXE, 0x48CA3, 19
global d_path_new_step:function 54
d_path_new_step:
incbin EXE, 0x48CB6, 54
global d_j_pfile_cpp_init:function 5
d_j_pfile_cpp_init:
incbin EXE, 0x48CEC, 5
global d_pfile_cpp_init:function 11
d_pfile_cpp_init:
incbin EXE, 0x48CF1, 11
global d_pfile_init_save_directory:function 95
d_pfile_init_save_directory:
incbin EXE, 0x48CFC, 95
global d_pfile_check_available_space:function 104
d_pfile_check_available_space:
incbin EXE, 0x48D5B, 104
global d_pfile_write_hero:function 112
d_pfile_write_hero:
incbin EXE, 0x48DC3, 112
global d_pfile_get_save_num_from_name:function 40
d_pfile_get_save_num_from_name:
incbin EXE, 0x48E33, 40
global d_pfile_encode_hero:function 132
d_pfile_encode_hero:
incbin EXE, 0x48E5B, 132
global d_pfile_open_archive:function 81
d_pfile_open_archive:
incbin EXE, 0x48EDF, 81
global d_pfile_get_save_path:function 130
d_pfile_get_save_path:
incbin EXE, 0x48F30, 130
global d_pfile_flush:function 50
d_pfile_flush:
incbin EXE, 0x48FB2, 50
global d_pfile_create_player_description:function 118
d_pfile_create_player_description:
incbin EXE, 0x48FE4, 118
global d_pfile_create_save_file:function 200
d_pfile_create_save_file:
incbin EXE, 0x4905A, 200
global d_pfile_flush_W:function 33
d_pfile_flush_W:
incbin EXE, 0x49122, 33
global d_game_2_ui_player:function 141
d_game_2_ui_player:
incbin EXE, 0x49143, 141
global d_game_2_ui_class:function 19
d_game_2_ui_class:
incbin EXE, 0x491D0, 19
global d_pfile_ui_set_hero_infos:function 455
d_pfile_ui_set_hero_infos:
incbin EXE, 0x491E3, 455
global d_GetSaveDirectory:function 140
d_GetSaveDirectory:
incbin EXE, 0x493AA, 140
global d_pfile_read_hero:function 290
d_pfile_read_hero:
incbin EXE, 0x49436, 290
global d_pfile_open_save_archive:function 58
d_pfile_open_save_archive:
incbin EXE, 0x49558, 58
global d_pfile_SFileCloseArchive:function 7
d_pfile_SFileCloseArchive:
incbin EXE, 0x49592, 7
global d_pfile_archive_contains_game:function 51
d_pfile_archive_contains_game:
incbin EXE, 0x49599, 51
global d_pfile_ui_set_class_stats:function 68
d_pfile_ui_set_class_stats:
incbin EXE, 0x495CC, 68
global d_pfile_get_player_class:function 16
d_pfile_get_player_class:
incbin EXE, 0x49610, 16
global d_pfile_ui_save_create:function 223
d_pfile_ui_save_create:
incbin EXE, 0x49620, 223
global d_pfile_get_file_name:function 87
d_pfile_get_file_name:
incbin EXE, 0x496FF, 87
global d_pfile_delete_save:function 74
d_pfile_delete_save:
incbin EXE, 0x49756, 74
global d_pfile_read_player_from_save:function 121
d_pfile_read_player_from_save:
incbin EXE, 0x497A0, 121
global d_GetTempLevelNames:function 74
d_GetTempLevelNames:
incbin EXE, 0x49819, 74
global d_GetPermLevelNames:function 134
d_GetPermLevelNames:
incbin EXE, 0x49863, 134
global d_pfile_get_game_name:function 41
d_pfile_get_game_name:
incbin EXE, 0x498E9, 41
global d_pfile_remove_temp_files:function 81
d_pfile_remove_temp_files:
incbin EXE, 0x49912, 81
global d_GetTempSaveNames:function 53
d_GetTempSaveNames:
incbin EXE, 0x49963, 53
global d_pfile_rename_temp_to_perm:function 172
d_pfile_rename_temp_to_perm:
incbin EXE, 0x49998, 172
global d_GetPermSaveNames:function 53
d_GetPermSaveNames:
incbin EXE, 0x49A44, 53
global d_pfile_write_save_file:function 174
d_pfile_write_save_file:
incbin EXE, 0x49A79, 174
global d_pfile_strcpy:function 10
d_pfile_strcpy:
incbin EXE, 0x49B27, 10
global d_pfile_read:function 386
d_pfile_read:
incbin EXE, 0x49B31, 386
global d_pfile_update:function 51
d_pfile_update:
incbin EXE, 0x49CB3, 51
global d_j_player_cpp_init:function 5
d_j_player_cpp_init:
incbin EXE, 0x49CE6, 5
global d_player_cpp_init:function 11
d_player_cpp_init:
incbin EXE, 0x49CEB, 11
global d_SetPlayerGPtrs:function 27
d_SetPlayerGPtrs:
incbin EXE, 0x49CF6, 27
global d_LoadPlrGFX:function 607
d_LoadPlrGFX:
incbin EXE, 0x49D11, 607
global d_InitPlayerGFX:function 68
d_InitPlayerGFX:
incbin EXE, 0x49F70, 68
global d_InitPlrGFXMem:function 532
d_InitPlrGFXMem:
incbin EXE, 0x49FB4, 532
global d_GetPlrGFXSize:function 193
d_GetPlrGFXSize:
incbin EXE, 0x4A1C8, 193
global d_FreePlayerGFX:function 174
d_FreePlayerGFX:
incbin EXE, 0x4A289, 174
global d_NewPlrAnim:function 101
d_NewPlrAnim:
incbin EXE, 0x4A337, 101
global d_ClearPlrPVars:function 81
d_ClearPlrPVars:
incbin EXE, 0x4A39C, 81
global d_SetPlrAnims:function 528
d_SetPlrAnims:
incbin EXE, 0x4A3ED, 528
global d_ClearPlrRVars:function 119
d_ClearPlrRVars:
incbin EXE, 0x4A5FD, 119
global d_CreatePlayer:function 782
d_CreatePlayer:
incbin EXE, 0x4A674, 782
global d_CalcStatDiff:function 65
d_CalcStatDiff:
incbin EXE, 0x4A982, 65
global d_NextPlrLevel:function 261
d_NextPlrLevel:
incbin EXE, 0x4A9C3, 261
global d_AddPlrExperience:function 304
d_AddPlrExperience:
incbin EXE, 0x4AAC8, 304
global d_AddPlrMonstExper:function 68
d_AddPlrMonstExper:
incbin EXE, 0x4ABF8, 68
global d_InitPlayer:function 759
d_InitPlayer:
incbin EXE, 0x4AC3C, 759
global d_InitMultiView:function 58
d_InitMultiView:
incbin EXE, 0x4AF33, 58
global d_InitPlayerLoc:function 341
d_InitPlayerLoc:
incbin EXE, 0x4AF6D, 341
global d_SolidLoc:function 41
d_SolidLoc:
incbin EXE, 0x4B0C2, 41
global d_PlrDirOK:function 175
d_PlrDirOK:
incbin EXE, 0x4B0EB, 175
global d_PlrClrTrans:function 67
d_PlrClrTrans:
incbin EXE, 0x4B19A, 67
global d_PlrDoTrans:function 129
d_PlrDoTrans:
incbin EXE, 0x4B1DD, 129
global d_SetPlayerOld:function 55
d_SetPlayerOld:
incbin EXE, 0x4B25E, 55
global d_FixPlayerLocation:function 152
d_FixPlayerLocation:
incbin EXE, 0x4B295, 152
global d_StartStand:function 187
d_StartStand:
incbin EXE, 0x4B32D, 187
global d_StartWalkStand:function 136
d_StartWalkStand:
incbin EXE, 0x4B3E8, 136
global d_PM_ChangeLightOff:function 205
d_PM_ChangeLightOff:
incbin EXE, 0x4B470, 205
global d_PM_ChangeOffset:function 165
d_PM_ChangeOffset:
incbin EXE, 0x4B53D, 165
global d_StartWalk:function 458
d_StartWalk:
incbin EXE, 0x4B5E2, 458
global d_StartWalk2:function 547
d_StartWalk2:
incbin EXE, 0x4B7AC, 547
global d_StartWalk3:function 591
d_StartWalk3:
incbin EXE, 0x4B9CF, 591
global d_StartAttack:function 157
d_StartAttack:
incbin EXE, 0x4BC1E, 157
global d_StartRangeAttack:function 184
d_StartRangeAttack:
incbin EXE, 0x4BCBB, 184
global d_StartPlrBlock:function 179
d_StartPlrBlock:
incbin EXE, 0x4BD73, 179
global d_StartSpell:function 367
d_StartSpell:
incbin EXE, 0x4BE26, 367
global d_FixPlrWalkTags:function 205
d_FixPlrWalkTags:
incbin EXE, 0x4BF95, 205
global d_RemovePlrFromMap:function 118
d_RemovePlrFromMap:
incbin EXE, 0x4C062, 118
global d_StartPlrHit:function 293
d_StartPlrHit:
incbin EXE, 0x4C0D8, 293
global d_RespawnDeadItem:function 204
d_RespawnDeadItem:
incbin EXE, 0x4C1FD, 204
global d_StartPlayerKill:function 811
d_StartPlayerKill:
incbin EXE, 0x4C2C9, 811
global d_PlrDeadItem:function 255
d_PlrDeadItem:
incbin EXE, 0x4C5F4, 255
global d_DropHalfPlayersGold:function 1048
d_DropHalfPlayersGold:
incbin EXE, 0x4C6F3, 1048
global d_SyncPlrKill:function 144
d_SyncPlrKill:
incbin EXE, 0x4CB0B, 144
global d_j_StartPlayerKill:function 5
d_j_StartPlayerKill:
incbin EXE, 0x4CB9B, 5
global d_RemovePlrMissiles:function 305
d_RemovePlrMissiles:
incbin EXE, 0x4CBA0, 305
global d_InitLevelChange:function 162
d_InitLevelChange:
incbin EXE, 0x4CCD1, 162
global d_StartNewLvl:function 252
d_StartNewLvl:
incbin EXE, 0x4CD73, 252
global d_RestartTownLvl:function 141
d_RestartTownLvl:
incbin EXE, 0x4CE6F, 141
global d_StartWarpLvl:function 120
d_StartWarpLvl:
incbin EXE, 0x4CEFC, 120
global d_PM_DoStand:function 3
d_PM_DoStand:
incbin EXE, 0x4CF74, 3
global d_PM_DoWalk:function 366
d_PM_DoWalk:
incbin EXE, 0x4CF77, 366
global d_PM_DoWalk2:function 331
d_PM_DoWalk2:
incbin EXE, 0x4D0E5, 331
global d_PM_DoWalk3:function 385
d_PM_DoWalk3:
incbin EXE, 0x4D230, 385
global d_WeaponDur:function 267
d_WeaponDur:
incbin EXE, 0x4D3B1, 267
global d_PlrHitMonst:function 902
d_PlrHitMonst:
incbin EXE, 0x4D4BC, 902
global d_PlrHitPlr:function 551
d_PlrHitPlr:
incbin EXE, 0x4D842, 551
global d_PlrHitObj:function 61
d_PlrHitObj:
incbin EXE, 0x4DA69, 61
global d_PM_DoAttack:function 530
d_PM_DoAttack:
incbin EXE, 0x4DAA6, 530
global d_PM_DoRangeAttack:function 244
d_PM_DoRangeAttack:
incbin EXE, 0x4DCB8, 244
global d_ShieldDur:function 161
d_ShieldDur:
incbin EXE, 0x4DDAC, 161
global d_PM_DoBlock:function 121
d_PM_DoBlock:
incbin EXE, 0x4DE4D, 121
global d_PM_DoSpell:function 320
d_PM_DoSpell:
incbin EXE, 0x4DEC6, 320
global d_PM_DoGotHit:function 182
d_PM_DoGotHit:
incbin EXE, 0x4E006, 182
global d_ArmorDur:function 191
d_ArmorDur:
incbin EXE, 0x4E0BC, 191
global d_PM_DoDeath:function 167
d_PM_DoDeath:
incbin EXE, 0x4E17B, 167
global d_CheckNewPath:function 2968
d_CheckNewPath:
incbin EXE, 0x4E222, 2968
global d_PlrDeathModeOK:function 66
d_PlrDeathModeOK:
incbin EXE, 0x4EDBA, 66
global d_ValidatePlayer:function 310
d_ValidatePlayer:
incbin EXE, 0x4EDFC, 310
global d_ProcessPlayers:function 511
d_ProcessPlayers:
incbin EXE, 0x4EF32, 511
global d_CheckCheatStats:function 89
d_CheckCheatStats:
incbin EXE, 0x4F131, 89
global d_ClrPlrPath:function 48
d_ClrPlrPath:
incbin EXE, 0x4F18A, 48
global d_PosOkPlayer:function 228
d_PosOkPlayer:
incbin EXE, 0x4F1BA, 228
global d_MakePlrPath:function 209
d_MakePlrPath:
incbin EXE, 0x4F29E, 209
global d_CheckPlrSpell:function 680
d_CheckPlrSpell:
incbin EXE, 0x4F36F, 680
global d_SyncPlrAnim:function 342
d_SyncPlrAnim:
incbin EXE, 0x4F617, 342
global d_SyncInitPlrPos:function 375
d_SyncInitPlrPos:
incbin EXE, 0x4F76D, 375
global d_SyncInitPlr:function 36
d_SyncInitPlr:
incbin EXE, 0x4F8E4, 36
global d_CheckStats:function 281
d_CheckStats:
incbin EXE, 0x4F908, 281
global d_ModifyPlrStr:function 186
d_ModifyPlrStr:
incbin EXE, 0x4FA21, 186
global d_ModifyPlrMag:function 173
d_ModifyPlrMag:
incbin EXE, 0x4FADB, 173
global d_ModifyPlrDex:function 164
d_ModifyPlrDex:
incbin EXE, 0x4FB88, 164
global d_ModifyPlrVit:function 163
d_ModifyPlrVit:
incbin EXE, 0x4FC2C, 163
global d_SetPlayerHitPoints:function 79
d_SetPlayerHitPoints:
incbin EXE, 0x4FCCF, 79
global d_SetPlrStr:function 117
d_SetPlrStr:
incbin EXE, 0x4FD1E, 117
global d_SetPlrMag:function 76
d_SetPlrMag:
incbin EXE, 0x4FD93, 76
global d_SetPlrDex:function 117
d_SetPlrDex:
incbin EXE, 0x4FDDF, 117
global d_SetPlrVit:function 76
d_SetPlrVit:
incbin EXE, 0x4FE54, 76
global d_InitDungMsgs:function 36
d_InitDungMsgs:
incbin EXE, 0x4FEA0, 36
global d_PlayDungMsgs:function 623
d_PlayDungMsgs:
incbin EXE, 0x4FEC4, 623
global d_plrmsg_delay:function 55
d_plrmsg_delay:
incbin EXE, 0x50133, 55
global d_ErrorPlrMsg:function 73
d_ErrorPlrMsg:
incbin EXE, 0x5016A, 73
global d_EventPlrMsg:function 71
d_EventPlrMsg:
incbin EXE, 0x501B3, 71
global d_SendPlrMsg:function 106
d_SendPlrMsg:
incbin EXE, 0x501FA, 106
global d_ClearPlrMsg:function 42
d_ClearPlrMsg:
incbin EXE, 0x50264, 42
global d_InitPlrMsg:function 28
d_InitPlrMsg:
incbin EXE, 0x5028E, 28
global d_DrawPlrMsg:function 141
d_DrawPlrMsg:
incbin EXE, 0x502AA, 141
global d_PrintPlrMsg:function 199
d_PrintPlrMsg:
incbin EXE, 0x50337, 199
global d_InitPortals:function 38
d_InitPortals:
incbin EXE, 0x503FE, 38
global d_SetPortalStats:function 62
d_SetPortalStats:
incbin EXE, 0x50424, 62
global d_AddWarpMissile:function 116
d_AddWarpMissile:
incbin EXE, 0x50462, 116
global d_SyncPortals:function 91
d_SyncPortals:
incbin EXE, 0x504D6, 91
global d_AddInTownPortal:function 20
d_AddInTownPortal:
incbin EXE, 0x50531, 20
global d_ActivatePortal:function 69
d_ActivatePortal:
incbin EXE, 0x50545, 69
global d_DeactivatePortal:function 12
d_DeactivatePortal:
incbin EXE, 0x5058A, 12
global d_PortalOnLevel:function 34
d_PortalOnLevel:
incbin EXE, 0x50596, 34
global d_RemovePortalMissile:function 124
d_RemovePortalMissile:
incbin EXE, 0x505B8, 124
global d_SetCurrentPortal:function 7
d_SetCurrentPortal:
incbin EXE, 0x50634, 7
global d_GetPortalLevel:function 168
d_GetPortalLevel:
incbin EXE, 0x5063B, 168
global d_GetPortalLvlPos:function 99
d_GetPortalLvlPos:
incbin EXE, 0x506E3, 99
global d_PosOkPortal:function 72
d_PosOkPortal:
incbin EXE, 0x50746, 72
global d_InitQuests:function 462
d_InitQuests:
incbin EXE, 0x5078E, 462
global d_CheckQuests:function 563
d_CheckQuests:
incbin EXE, 0x5095C, 563
global d_ForceQuests:function 162
d_ForceQuests:
incbin EXE, 0x50B8F, 162
global d_QuestStatus:function 64
d_QuestStatus:
incbin EXE, 0x50C31, 64
global d_CheckQuestKill:function 889
d_CheckQuestKill:
incbin EXE, 0x50C71, 889
global d_DrawButcher:function 39
d_DrawButcher:
incbin EXE, 0x50FEA, 39
global d_DrawSkelKing:function 33
d_DrawSkelKing:
incbin EXE, 0x51011, 33
global d_DrawWarLord:function 144
d_DrawWarLord:
incbin EXE, 0x51032, 144
global d_DrawSChamber:function 186
d_DrawSChamber:
incbin EXE, 0x510C2, 186
global d_DrawLTBanner:function 140
d_DrawLTBanner:
incbin EXE, 0x5117C, 140
global d_DrawBlind:function 140
d_DrawBlind:
incbin EXE, 0x51208, 140
global d_DrawBlood:function 140
d_DrawBlood:
incbin EXE, 0x51294, 140
global d_DRLG_CheckQuests:function 145
d_DRLG_CheckQuests:
incbin EXE, 0x51320, 145
global d_SetReturnLvlPos:function 179
d_SetReturnLvlPos:
incbin EXE, 0x513B1, 179
global d_GetReturnLvlPos:function 57
d_GetReturnLvlPos:
incbin EXE, 0x51464, 57
global d_ResyncMPQuests:function 188
d_ResyncMPQuests:
incbin EXE, 0x5149D, 188
global d_ResyncQuests:function 806
d_ResyncQuests:
incbin EXE, 0x51559, 806
global d_PrintQLString:function 334
d_PrintQLString:
incbin EXE, 0x5187F, 334
global d_DrawQuestLog:function 140
d_DrawQuestLog:
incbin EXE, 0x519CD, 140
global d_StartQuestlog:function 112
d_StartQuestlog:
incbin EXE, 0x51A59, 112
global d_QuestlogUp:function 71
d_QuestlogUp:
incbin EXE, 0x51AC9, 71
global d_QuestlogDown:function 74
d_QuestlogDown:
incbin EXE, 0x51B10, 74
global d_QuestlogEnter:function 66
d_QuestlogEnter:
incbin EXE, 0x51B5A, 66
global d_QuestlogESC:function 85
d_QuestlogESC:
incbin EXE, 0x51B9C, 85
global d_SetMultiQuest:function 64
d_SetMultiQuest:
incbin EXE, 0x51BF1, 64
global d_SystemSupported:function 84
d_SystemSupported:
incbin EXE, 0x51C31, 84
global d_RestrictedTest:function 114
d_RestrictedTest:
incbin EXE, 0x51C85, 114
global d_ReadOnlyTest:function 126
d_ReadOnlyTest:
incbin EXE, 0x51CF7, 126
global d_j_scrollrt_cpp_init:function 5
d_j_scrollrt_cpp_init:
incbin EXE, 0x51D75, 5
global d_scrollrt_cpp_init:function 11
d_scrollrt_cpp_init:
incbin EXE, 0x51D7A, 11
global d_ClearCursor:function 15
d_ClearCursor:
incbin EXE, 0x51D85, 15
global d_DrawMissile:function 406
d_DrawMissile:
incbin EXE, 0x51D94, 406
global d_DrawClippedMissile:function 406
d_DrawClippedMissile:
incbin EXE, 0x51F2A, 406
global d_DrawDeadPlayer:function 224
d_DrawDeadPlayer:
incbin EXE, 0x520C0, 224
global d_DrawPlayer:function 491
d_DrawPlayer:
incbin EXE, 0x521A0, 491
global d_DrawClippedPlayer:function 469
d_DrawClippedPlayer:
incbin EXE, 0x5238B, 469
global d_DrawView:function 274
d_DrawView:
incbin EXE, 0x52560, 274
global d_DrawGame:function 517
d_DrawGame:
incbin EXE, 0x52672, 517
global d_scrollrt_draw_lower:function 1131
d_scrollrt_draw_lower:
incbin EXE, 0x52877, 1131
global d_scrollrt_draw_clipped_dungeon:function 1527
d_scrollrt_draw_clipped_dungeon:
incbin EXE, 0x52CE2, 1527
global d_DrawClippedMonster:function 243
d_DrawClippedMonster:
incbin EXE, 0x532D9, 243
global d_DrawClippedObject:function 281
d_DrawClippedObject:
incbin EXE, 0x533CC, 281
global d_scrollrt_draw_clipped_e_flag:function 324
d_scrollrt_draw_clipped_e_flag:
incbin EXE, 0x534E5, 324
global d_scrollrt_draw_lower_2:function 937
d_scrollrt_draw_lower_2:
incbin EXE, 0x53629, 937
global d_scrollrt_draw_clipped_dungeon_2:function 1591
d_scrollrt_draw_clipped_dungeon_2:
incbin EXE, 0x539D2, 1591
global d_scrollrt_draw_clipped_e_flag_2:function 404
d_scrollrt_draw_clipped_e_flag_2:
incbin EXE, 0x54009, 404
global d_scrollrt_draw_upper:function 1146
d_scrollrt_draw_upper:
incbin EXE, 0x5419D, 1146
global d_scrollrt_draw_dungeon:function 1581
d_scrollrt_draw_dungeon:
incbin EXE, 0x54617, 1581
global d_DrawMonster:function 243
d_DrawMonster:
incbin EXE, 0x54C44, 243
global d_DrawObject:function 326
d_DrawObject:
incbin EXE, 0x54D37, 326
global d_scrollrt_draw_e_flag:function 343
d_scrollrt_draw_e_flag:
incbin EXE, 0x54E7D, 343
global d_DrawZoom:function 606
d_DrawZoom:
incbin EXE, 0x54FD4, 606
global d_ClearScreenBuffer:function 51
d_ClearScreenBuffer:
incbin EXE, 0x55232, 51
global d_scrollrt_draw_game_screen:function 98
d_scrollrt_draw_game_screen:
incbin EXE, 0x55265, 98
global d_scrollrt_draw_cursor_back_buffer:function 143
d_scrollrt_draw_cursor_back_buffer:
incbin EXE, 0x552C7, 143
global d_scrollrt_draw_cursor_item:function 462
d_scrollrt_draw_cursor_item:
incbin EXE, 0x55356, 462
global d_DrawMain:function 655
d_DrawMain:
incbin EXE, 0x55524, 655
global d_DoBlitScreen:function 326
d_DoBlitScreen:
incbin EXE, 0x557B3, 326
global d_DrawAndBlit:function 300
d_DrawAndBlit:
incbin EXE, 0x558F9, 300
global d_ObjIndex:function 70
d_ObjIndex:
incbin EXE, 0x55A25, 70
global d_AddSKingObjs:function 175
d_AddSKingObjs:
incbin EXE, 0x55A6B, 175
global d_AddSChamObjs:function 59
d_AddSChamObjs:
incbin EXE, 0x55B1A, 59
global d_AddVileObjs:function 88
d_AddVileObjs:
incbin EXE, 0x55B55, 88
global d_DRLG_SetMapTrans:function 108
d_DRLG_SetMapTrans:
incbin EXE, 0x55BAD, 108
global d_LoadSetMap:function 509
d_LoadSetMap:
incbin EXE, 0x55C19, 509
global d_SHA1Clear:function 21
d_SHA1Clear:
incbin EXE, 0x55E16, 21
global d_SHA1Result:function 34
d_SHA1Result:
incbin EXE, 0x55E2B, 34
global d_SHA1Calculate:function 38
d_SHA1Calculate:
incbin EXE, 0x55E4D, 38
global d_SHA1Input:function 81
d_SHA1Input:
incbin EXE, 0x55E73, 81
global d_SHA1ProcessMessageBlock:function 446
d_SHA1ProcessMessageBlock:
incbin EXE, 0x55EC4, 446
global d_SHA1Reset:function 57
d_SHA1Reset:
incbin EXE, 0x56082, 57
global d_j_sound_cpp_init:function 5
d_j_sound_cpp_init:
incbin EXE, 0x560BB, 5
global d_sound_cpp_init:function 11
d_sound_cpp_init:
incbin EXE, 0x560C0, 11
global d_snd_update:function 87
d_snd_update:
incbin EXE, 0x560CB, 87
global d_snd_stop_snd:function 18
d_snd_stop_snd:
incbin EXE, 0x56122, 18
global d_snd_playing:function 44
d_snd_playing:
incbin EXE, 0x56134, 44
global d_snd_play_snd:function 217
d_snd_play_snd:
incbin EXE, 0x56160, 217
global d_sound_dup_channel:function 59
d_sound_dup_channel:
incbin EXE, 0x56239, 59
global d_sound_file_reload:function 147
d_sound_file_reload:
incbin EXE, 0x56274, 147
global d_sound_file_load:function 252
d_sound_file_load:
incbin EXE, 0x56307, 252
global d_sound_CreateSoundBuffer:function 93
d_sound_CreateSoundBuffer:
incbin EXE, 0x56403, 93
global d_sound_file_cleanup:function 43
d_sound_file_cleanup:
incbin EXE, 0x56460, 43
global d_snd_init:function 160
d_snd_init:
incbin EXE, 0x5648B, 160
global d_sound_load_volume:function 81
d_sound_load_volume:
incbin EXE, 0x5652B, 81
global d_sound_create_primary_buffer:function 258
d_sound_create_primary_buffer:
incbin EXE, 0x5657C, 258
global d_sound_DirectSoundCreate:function 129
d_sound_DirectSoundCreate:
incbin EXE, 0x5667E, 129
global d_sound_cleanup:function 89
d_sound_cleanup:
incbin EXE, 0x566FF, 89
global d_sound_store_volume:function 15
d_sound_store_volume:
incbin EXE, 0x56758, 15
global d_music_stop:function 44
d_music_stop:
incbin EXE, 0x56767, 44
global d_music_start:function 107
d_music_start:
incbin EXE, 0x56793, 107
global d_sound_disable_music:function 26
d_sound_disable_music:
incbin EXE, 0x567FE, 26
global d_sound_get_or_set_music_volume:function 35
d_sound_get_or_set_music_volume:
incbin EXE, 0x56818, 35
global d_sound_get_or_set_sound_volume:function 19
d_sound_get_or_set_sound_volume:
incbin EXE, 0x5683B, 19
global d_GetManaAmount:function 236
d_GetManaAmount:
incbin EXE, 0x5684E, 236
global d_UseMana:function 74
d_UseMana:
incbin EXE, 0x5693A, 74
global d_CheckSpell:function 81
d_CheckSpell:
incbin EXE, 0x56984, 81
global d_CastSpell:function 220
d_CastSpell:
incbin EXE, 0x569D5, 220
global d_DoResurrect:function 282
d_DoResurrect:
incbin EXE, 0x56AB1, 282
global d_PlacePlayer:function 291
d_PlacePlayer:
incbin EXE, 0x56BCB, 291
global d_DoHealOther:function 275
d_DoHealOther:
incbin EXE, 0x56CEE, 275
global d_InitStores:function 134
d_InitStores:
incbin EXE, 0x56E01, 134
global d_SetupTownStores:function 187
d_SetupTownStores:
incbin EXE, 0x56E87, 187
global d_FreeStoreMem:function 54
d_FreeStoreMem:
incbin EXE, 0x56F42, 54
global d_DrawSTextBack:function 94
d_DrawSTextBack:
incbin EXE, 0x56F78, 94
global d_PrintSString:function 524
d_PrintSString:
incbin EXE, 0x56FD6, 524
global d_DrawSLine:function 128
d_DrawSLine:
incbin EXE, 0x571E2, 128
global d_DrawSArrows:function 240
d_DrawSArrows:
incbin EXE, 0x57262, 240
global d_DrawSTextHelp:function 15
d_DrawSTextHelp:
incbin EXE, 0x57352, 15
global d_ClearSText:function 69
d_ClearSText:
incbin EXE, 0x57361, 69
global d_AddSLine:function 37
d_AddSLine:
incbin EXE, 0x573A6, 37
global d_AddSTextVal:function 13
d_AddSTextVal:
incbin EXE, 0x573CB, 13
global d_OffsetSTextY:function 13
d_OffsetSTextY:
incbin EXE, 0x573D8, 13
global d_AddSText:function 81
d_AddSText:
incbin EXE, 0x573E5, 81
global d_StoreAutoPlace:function 637
d_StoreAutoPlace:
incbin EXE, 0x57436, 637
global d_S_StartSmith:function 202
d_S_StartSmith:
incbin EXE, 0x576B3, 202
global d_S_ScrollSBuy:function 188
d_S_ScrollSBuy:
incbin EXE, 0x5777D, 188
global d_PrintStoreItem:function 634
d_PrintStoreItem:
incbin EXE, 0x57839, 634
global d_S_StartSBuy:function 192
d_S_StartSBuy:
incbin EXE, 0x57AB3, 192
global d_S_ScrollSPBuy:function 222
d_S_ScrollSPBuy:
incbin EXE, 0x57B73, 222
global d_S_StartSPBuy:function 224
d_S_StartSPBuy:
incbin EXE, 0x57C51, 224
global d_SmithSellOk:function 65
d_SmithSellOk:
incbin EXE, 0x57D31, 65
global d_S_ScrollSSell:function 231
d_S_ScrollSSell:
incbin EXE, 0x57D72, 231
global d_S_StartSSell:function 434
d_S_StartSSell:
incbin EXE, 0x57E59, 434
global d_SmithRepairOk:function 67
d_SmithRepairOk:
incbin EXE, 0x5800B, 67
global d_S_StartSRepair:function 588
d_S_StartSRepair:
incbin EXE, 0x5804E, 588
global d_AddStoreHoldRepair:function 163
d_AddStoreHoldRepair:
incbin EXE, 0x5829A, 163
global d_S_StartWitch:function 166
d_S_StartWitch:
incbin EXE, 0x5833D, 166
global d_S_ScrollWBuy:function 188
d_S_ScrollWBuy:
incbin EXE, 0x583E3, 188
global d_S_StartWBuy:function 202
d_S_StartWBuy:
incbin EXE, 0x5849F, 202
global d_WitchSellOk:function 91
d_WitchSellOk:
incbin EXE, 0x58569, 91
global d_S_StartWSell:function 621
d_S_StartWSell:
incbin EXE, 0x585C4, 621
global d_WitchRechargeOk:function 47
d_WitchRechargeOk:
incbin EXE, 0x58831, 47
global d_AddStoreHoldRecharge:function 134
d_AddStoreHoldRecharge:
incbin EXE, 0x58860, 134
global d_S_StartWRecharge:function 429
d_S_StartWRecharge:
incbin EXE, 0x588E6, 429
global d_S_StartNoMoney:function 58
d_S_StartNoMoney:
incbin EXE, 0x58A93, 58
global d_S_StartNoRoom:function 51
d_S_StartNoRoom:
incbin EXE, 0x58ACD, 51
global d_S_StartConfirm:function 371
d_S_StartConfirm:
incbin EXE, 0x58B00, 371
global d_S_StartBoy:function 189
d_S_StartBoy:
incbin EXE, 0x58C73, 189
global d_S_StartBBoy:function 205
d_S_StartBBoy:
incbin EXE, 0x58D30, 205
global d_S_StartHealer:function 168
d_S_StartHealer:
incbin EXE, 0x58DFD, 168
global d_S_ScrollHBuy:function 176
d_S_ScrollHBuy:
incbin EXE, 0x58EA5, 176
global d_S_StartHBuy:function 192
d_S_StartHBuy:
incbin EXE, 0x58F55, 192
global d_S_StartStory:function 121
d_S_StartStory:
incbin EXE, 0x59015, 121
global d_IdItemOk:function 20
d_IdItemOk:
incbin EXE, 0x5908E, 20
global d_AddStoreHoldId:function 68
d_AddStoreHoldId:
incbin EXE, 0x590A2, 68
global d_S_StartSIdentify:function 687
d_S_StartSIdentify:
incbin EXE, 0x590E6, 687
global d_S_StartIdShow:function 177
d_S_StartIdShow:
incbin EXE, 0x59395, 177
global d_S_StartTalk:function 290
d_S_StartTalk:
incbin EXE, 0x59446, 290
global d_S_StartTavern:function 132
d_S_StartTavern:
incbin EXE, 0x59568, 132
global d_S_StartBarMaid:function 114
d_S_StartBarMaid:
incbin EXE, 0x595EC, 114
global d_S_StartDrunk:function 114
d_S_StartDrunk:
incbin EXE, 0x5965E, 114
global d_StartStore:function 447
d_StartStore:
incbin EXE, 0x596D0, 447
global d_DrawSText:function 245
d_DrawSText:
incbin EXE, 0x5988F, 245
global d_STextESC:function 299
d_STextESC:
incbin EXE, 0x59984, 299
global d_STextUp:function 168
d_STextUp:
incbin EXE, 0x59AAF, 168
global d_STextDown:function 173
d_STextDown:
incbin EXE, 0x59B57, 173
global d_STextPrior:function 74
d_STextPrior:
incbin EXE, 0x59C04, 74
global d_STextNext:function 77
d_STextNext:
incbin EXE, 0x59C4E, 77
global d_S_SmithEnter:function 105
d_S_SmithEnter:
incbin EXE, 0x59C9B, 105
global d_SetGoldCurs:function 70
d_SetGoldCurs:
incbin EXE, 0x59D04, 70
global d_SetSpdbarGoldCurs:function 70
d_SetSpdbarGoldCurs:
incbin EXE, 0x59D4A, 70
global d_TakePlrsMoney:function 473
d_TakePlrsMoney:
incbin EXE, 0x59D90, 473
global d_SmithBuyItem:function 171
d_SmithBuyItem:
incbin EXE, 0x59F69, 171
global d_S_SBuyEnter:function 213
d_S_SBuyEnter:
incbin EXE, 0x5A014, 213
global d_SmithBuyPItem:function 149
d_SmithBuyPItem:
incbin EXE, 0x5A0E9, 149
global d_S_SPBuyEnter:function 244
d_S_SPBuyEnter:
incbin EXE, 0x5A17E, 244
global d_StoreGoldFit:function 214
d_StoreGoldFit:
incbin EXE, 0x5A272, 214
global d_PlaceStoreGold:function 200
d_PlaceStoreGold:
incbin EXE, 0x5A348, 200
global d_StoreSellItem:function 336
d_StoreSellItem:
incbin EXE, 0x5A410, 336
global d_S_SSellEnter:function 127
d_S_SSellEnter:
incbin EXE, 0x5A560, 127
global d_SmithRepairItem:function 215
d_SmithRepairItem:
incbin EXE, 0x5A5DF, 215
global d_S_SRepairEnter:function 129
d_S_SRepairEnter:
incbin EXE, 0x5A6B6, 129
global d_S_WitchEnter:function 104
d_S_WitchEnter:
incbin EXE, 0x5A737, 104
global d_WitchBuyItem:function 184
d_WitchBuyItem:
incbin EXE, 0x5A79F, 184
global d_S_WBuyEnter:function 213
d_S_WBuyEnter:
incbin EXE, 0x5A857, 213
global d_S_WSellEnter:function 127
d_S_WSellEnter:
incbin EXE, 0x5A92C, 127
global d_WitchRechargeItem:function 137
d_WitchRechargeItem:
incbin EXE, 0x5A9AB, 137
global d_S_WRechargeEnter:function 129
d_S_WRechargeEnter:
incbin EXE, 0x5AA34, 129
global d_S_BoyEnter:function 162
d_S_BoyEnter:
incbin EXE, 0x5AAB5, 162
global d_BoyBuyItem:function 58
d_BoyBuyItem:
incbin EXE, 0x5AB57, 58
global d_HealerBuyItem:function 260
d_HealerBuyItem:
incbin EXE, 0x5AB91, 260
global d_S_BBuyEnter:function 211
d_S_BBuyEnter:
incbin EXE, 0x5AC95, 211
global d_StoryIdItem:function 239
d_StoryIdItem:
incbin EXE, 0x5AD68, 239
global d_S_ConfirmEnter:function 160
d_S_ConfirmEnter:
incbin EXE, 0x5AE57, 160
global d_S_HealerEnter:function 168
d_S_HealerEnter:
incbin EXE, 0x5AEF7, 168
global d_S_HBuyEnter:function 213
d_S_HBuyEnter:
incbin EXE, 0x5AF9F, 213
global d_S_StoryEnter:function 86
d_S_StoryEnter:
incbin EXE, 0x5B074, 86
global d_S_SIDEnter:function 129
d_S_SIDEnter:
incbin EXE, 0x5B0CA, 129
global d_S_TalkEnter:function 255
d_S_TalkEnter:
incbin EXE, 0x5B14B, 255
global d_S_TavernEnter:function 78
d_S_TavernEnter:
incbin EXE, 0x5B24A, 78
global d_S_BarmaidEnter:function 78
d_S_BarmaidEnter:
incbin EXE, 0x5B298, 78
global d_S_DrunkEnter:function 78
d_S_DrunkEnter:
incbin EXE, 0x5B2E6, 78
global d_STextEnter:function 287
d_STextEnter:
incbin EXE, 0x5B334, 287
global d_CheckStoreBtn:function 311
d_CheckStoreBtn:
incbin EXE, 0x5B453, 311
global d_ReleaseStoreBtn:function 15
d_ReleaseStoreBtn:
incbin EXE, 0x5B58A, 15
global d_sync_all_monsters:function 133
d_sync_all_monsters:
incbin EXE, 0x5B599, 133
global d_sync_one_monster:function 166
d_sync_one_monster:
incbin EXE, 0x5B61E, 166
global d_sync_monster_active:function 83
d_sync_monster_active:
incbin EXE, 0x5B6C4, 83
global d_sync_monster_pos:function 111
d_sync_monster_pos:
incbin EXE, 0x5B717, 111
global d_sync_monster_active2:function 96
d_sync_monster_active2:
incbin EXE, 0x5B786, 96
global d_SyncPlrInv:function 481
d_SyncPlrInv:
incbin EXE, 0x5B7E6, 481
global d_SyncData:function 116
d_SyncData:
incbin EXE, 0x5B9C7, 116
global d_sync_monster_data:function 528
d_sync_monster_data:
incbin EXE, 0x5BA3B, 528
global d_sync_clear_pkt:function 37
d_sync_clear_pkt:
incbin EXE, 0x5BC4B, 37
global d_TFit_Shrine:function 291
d_TFit_Shrine:
incbin EXE, 0x5BC70, 291
global d_TFit_Obj5:function 223
d_TFit_Obj5:
incbin EXE, 0x5BD93, 223
global d_TFit_SkelRoom:function 82
d_TFit_SkelRoom:
incbin EXE, 0x5BE72, 82
global d_TFit_GoatShrine:function 69
d_TFit_GoatShrine:
incbin EXE, 0x5BEC4, 69
global d_CheckThemeObj3:function 127
d_CheckThemeObj3:
incbin EXE, 0x5BF09, 127
global d_TFit_Obj3:function 92
d_TFit_Obj3:
incbin EXE, 0x5BF88, 92
global d_CheckThemeReqs:function 128
d_CheckThemeReqs:
incbin EXE, 0x5BFE4, 128
global d_SpecialThemeFit:function 310
d_SpecialThemeFit:
incbin EXE, 0x5C064, 310
global d_CheckThemeRoom:function 312
d_CheckThemeRoom:
incbin EXE, 0x5C19A, 312
global d_InitThemes:function 437
d_InitThemes:
incbin EXE, 0x5C2D2, 437
global d_HoldThemeRooms:function 90
d_HoldThemeRooms:
incbin EXE, 0x5C487, 90
global d_PlaceThemeMonsts:function 225
d_PlaceThemeMonsts:
incbin EXE, 0x5C4E1, 225
global d_Theme_Barrel:function 216
d_Theme_Barrel:
incbin EXE, 0x5C5C2, 216
global d_Theme_Shrine:function 179
d_Theme_Shrine:
incbin EXE, 0x5C69A, 179
global d_Theme_MonstPit:function 153
d_Theme_MonstPit:
incbin EXE, 0x5C74D, 153
global d_Theme_SkelRoom:function 470
d_Theme_SkelRoom:
incbin EXE, 0x5C7E6, 470
global d_Theme_Treasure:function 331
d_Theme_Treasure:
incbin EXE, 0x5C9BC, 331
global d_Theme_Library:function 387
d_Theme_Library:
incbin EXE, 0x5CB07, 387
global d_Theme_Torture:function 211
d_Theme_Torture:
incbin EXE, 0x5CC8A, 211
global d_Theme_BloodFountain:function 70
d_Theme_BloodFountain:
incbin EXE, 0x5CD5D, 70
global d_Theme_Decap:function 211
d_Theme_Decap:
incbin EXE, 0x5CDA3, 211
global d_Theme_PurifyingFountain:function 70
d_Theme_PurifyingFountain:
incbin EXE, 0x5CE76, 70
global d_Theme_ArmorStand:function 241
d_Theme_ArmorStand:
incbin EXE, 0x5CEBC, 241
global d_Theme_GoatShrine:function 206
d_Theme_GoatShrine:
incbin EXE, 0x5CFAD, 206
global d_Theme_Cauldron:function 70
d_Theme_Cauldron:
incbin EXE, 0x5D07B, 70
global d_Theme_MurkyFountain:function 70
d_Theme_MurkyFountain:
incbin EXE, 0x5D0C1, 70
global d_Theme_TearFountain:function 70
d_Theme_TearFountain:
incbin EXE, 0x5D107, 70
global d_Theme_BrnCross:function 211
d_Theme_BrnCross:
incbin EXE, 0x5D14D, 211
global d_Theme_WeaponRack:function 241
d_Theme_WeaponRack:
incbin EXE, 0x5D220, 241
global d_UpdateL4Trans:function 32
d_UpdateL4Trans:
incbin EXE, 0x5D311, 32
global d_CreateThemeRooms:function 347
d_CreateThemeRooms:
incbin EXE, 0x5D331, 347
global d_tmsg_get:function 75
d_tmsg_get:
incbin EXE, 0x5D48C, 75
global d_tmsg_add:function 83
d_tmsg_add:
incbin EXE, 0x5D4D7, 83
global d_tmsg_cleanup:function 39
d_tmsg_cleanup:
incbin EXE, 0x5D52A, 39
global d_town_clear_upper_buf:function 102
d_town_clear_upper_buf:
incbin EXE, 0x5D551, 102
global d_town_clear_low_buf:function 111
d_town_clear_low_buf:
incbin EXE, 0x5D5B7, 111
global d_town_draw_clipped_e_flag:function 127
d_town_draw_clipped_e_flag:
incbin EXE, 0x5D626, 127
global d_town_draw_clipped_town:function 779
d_town_draw_clipped_town:
incbin EXE, 0x5D6A5, 779
global d_town_draw_lower:function 744
d_town_draw_lower:
incbin EXE, 0x5D9B0, 744
global d_town_draw_clipped_e_flag_2:function 161
d_town_draw_clipped_e_flag_2:
incbin EXE, 0x5DC98, 161
global d_town_draw_clipped_town_2:function 784
d_town_draw_clipped_town_2:
incbin EXE, 0x5DD39, 784
global d_town_draw_lower_2:function 833
d_town_draw_lower_2:
incbin EXE, 0x5E049, 833
global d_town_draw_e_flag:function 137
d_town_draw_e_flag:
incbin EXE, 0x5E38A, 137
global d_town_draw_town_all:function 784
d_town_draw_town_all:
incbin EXE, 0x5E413, 784
global d_town_draw_upper:function 826
d_town_draw_upper:
incbin EXE, 0x5E723, 826
global d_T_DrawGame:function 505
d_T_DrawGame:
incbin EXE, 0x5EA5D, 505
global d_T_DrawZoom:function 597
d_T_DrawZoom:
incbin EXE, 0x5EC56, 597
global d_T_DrawView:function 300
d_T_DrawView:
incbin EXE, 0x5EEAB, 300
global d_town_init_dpiece_defs_map:function 232
d_town_init_dpiece_defs_map:
incbin EXE, 0x5EFD7, 232
global d_T_FillSector:function 182
d_T_FillSector:
incbin EXE, 0x5F0BF, 182
global d_T_FillTile:function 113
d_T_FillTile:
incbin EXE, 0x5F175, 113
global d_T_Pass3:function 413
d_T_Pass3:
incbin EXE, 0x5F1E6, 413
global d_CreateTown:function 536
d_CreateTown:
incbin EXE, 0x5F383, 536
global d_GetActiveTowner:function 38
d_GetActiveTowner:
incbin EXE, 0x5F59B, 38
global d_SetTownerGPtrs:function 58
d_SetTownerGPtrs:
incbin EXE, 0x5F5C1, 58
global d_NewTownerAnim:function 52
d_NewTownerAnim:
incbin EXE, 0x5F5FB, 52
global d_InitTownerInfo:function 149
d_InitTownerInfo:
incbin EXE, 0x5F62F, 149
global d_InitQstSnds:function 77
d_InitQstSnds:
incbin EXE, 0x5F6C4, 77
global d_InitSmith:function 143
d_InitSmith:
incbin EXE, 0x5F711, 143
global d_InitBarOwner:function 150
d_InitBarOwner:
incbin EXE, 0x5F7A0, 150
global d_InitTownDead:function 144
d_InitTownDead:
incbin EXE, 0x5F836, 144
global d_InitWitch:function 143
d_InitWitch:
incbin EXE, 0x5F8C6, 143
global d_InitBarmaid:function 143
d_InitBarmaid:
incbin EXE, 0x5F955, 143
global d_InitBoy:function 150
d_InitBoy:
incbin EXE, 0x5F9E4, 150
global d_InitHealer:function 143
d_InitHealer:
incbin EXE, 0x5FA7A, 143
global d_InitTeller:function 143
d_InitTeller:
incbin EXE, 0x5FB09, 143
global d_InitDrunk:function 143
d_InitDrunk:
incbin EXE, 0x5FB98, 143
global d_InitCows:function 335
d_InitCows:
incbin EXE, 0x5FC27, 335
global d_InitTowners:function 77
d_InitTowners:
incbin EXE, 0x5FD76, 77
global d_FreeTownerGFX:function 66
d_FreeTownerGFX:
incbin EXE, 0x5FDC3, 66
global d_TownCtrlMsg:function 115
d_TownCtrlMsg:
incbin EXE, 0x5FE05, 115
global d_TownBlackSmith:function 14
d_TownBlackSmith:
incbin EXE, 0x5FE78, 14
global d_TownBarOwner:function 15
d_TownBarOwner:
incbin EXE, 0x5FE86, 15
global d_TownDead:function 120
d_TownDead:
incbin EXE, 0x5FE95, 120
global d_TownHealer:function 15
d_TownHealer:
incbin EXE, 0x5FF0D, 15
global d_TownStory:function 15
d_TownStory:
incbin EXE, 0x5FF1C, 15
global d_TownDrunk:function 15
d_TownDrunk:
incbin EXE, 0x5FF2B, 15
global d_TownBoy:function 15
d_TownBoy:
incbin EXE, 0x5FF3A, 15
global d_TownWitch:function 15
d_TownWitch:
incbin EXE, 0x5FF49, 15
global d_TownBarMaid:function 15
d_TownBarMaid:
incbin EXE, 0x5FF58, 15
global d_TownCow:function 15
d_TownCow:
incbin EXE, 0x5FF67, 15
global d_ProcessTowners:function 230
d_ProcessTowners:
incbin EXE, 0x5FF76, 230
global d_PlrHasItem:function 80
d_PlrHasItem:
incbin EXE, 0x6005C, 80
global d_TownerTalk:function 29
d_TownerTalk:
incbin EXE, 0x600AC, 29
global d_TalkToTowner:function 2847
d_TalkToTowner:
incbin EXE, 0x600C9, 2847
global d_CowSFX:function 189
d_CowSFX:
incbin EXE, 0x60BE8, 189
global d_j_track_cpp_init:function 5
d_j_track_cpp_init:
incbin EXE, 0x60CA5, 5
global d_track_cpp_init:function 11
d_track_cpp_init:
incbin EXE, 0x60CAA, 11
global d_track_repeat_walk:function 158
d_track_repeat_walk:
incbin EXE, 0x60CB5, 158
global d_track_process:function 76
d_track_process:
incbin EXE, 0x60D53, 76
global d_track_isscrolling:function 8
d_track_isscrolling:
incbin EXE, 0x60D9F, 8
global d_InitNoTriggers:function 15
d_InitNoTriggers:
incbin EXE, 0x60DA7, 15
global d_InitTownTriggers:function 399
d_InitTownTriggers:
incbin EXE, 0x60DB6, 399
global d_InitL1Triggers:function 169
d_InitL1Triggers:
incbin EXE, 0x60F45, 169
global d_InitL2Triggers:function 264
d_InitL2Triggers:
incbin EXE, 0x60FEE, 264
global d_InitL3Triggers:function 208
d_InitL3Triggers:
incbin EXE, 0x610F6, 208
global d_InitL4Triggers:function 324
d_InitL4Triggers:
incbin EXE, 0x611C6, 324
global d_InitSKingTriggers:function 48
d_InitSKingTriggers:
incbin EXE, 0x6130A, 48
global d_InitSChambTriggers:function 48
d_InitSChambTriggers:
incbin EXE, 0x6133A, 48
global d_InitPWaterTriggers:function 48
d_InitPWaterTriggers:
incbin EXE, 0x6136A, 48
global d_InitVPTriggers:function 48
d_InitVPTriggers:
incbin EXE, 0x6139A, 48
global d_ForceTownTrig:function 358
d_ForceTownTrig:
incbin EXE, 0x613CA, 358
global d_ForceL1Trig:function 284
d_ForceL1Trig:
incbin EXE, 0x61530, 284
global d_ForceL2Trig:function 515
d_ForceL2Trig:
incbin EXE, 0x6164C, 515
global d_ForceL3Trig:function 478
d_ForceL3Trig:
incbin EXE, 0x6184F, 478
global d_ForceL4Trig:function 585
d_ForceL4Trig:
incbin EXE, 0x61A2D, 585
global d_Freeupstairs:function 65
d_Freeupstairs:
incbin EXE, 0x61C76, 65
global d_ForceSKingTrig:function 104
d_ForceSKingTrig:
incbin EXE, 0x61CB7, 104
global d_ForceSChambTrig:function 104
d_ForceSChambTrig:
incbin EXE, 0x61D1F, 104
global d_ForcePWaterTrig:function 104
d_ForcePWaterTrig:
incbin EXE, 0x61D87, 104
global d_CheckTrigForce:function 174
d_CheckTrigForce:
incbin EXE, 0x61DEF, 174
global d_CheckTriggers:function 464
d_CheckTriggers:
incbin EXE, 0x61E9D, 464
global d_j_wave_cpp_init:function 5
d_j_wave_cpp_init:
incbin EXE, 0x6206D, 5
global d_wave_cpp_init:function 11
d_wave_cpp_init:
incbin EXE, 0x62072, 11
global d_WCloseFile:function 7
d_WCloseFile:
incbin EXE, 0x6207D, 7
global d_WGetFileSize:function 43
d_WGetFileSize:
incbin EXE, 0x62084, 43
global d_WGetFileArchive:function 87
d_WGetFileArchive:
incbin EXE, 0x620AF, 87
global d_WOpenFile:function 66
d_WOpenFile:
incbin EXE, 0x62106, 66
global d_WReadFile:function 82
d_WReadFile:
incbin EXE, 0x62148, 82
global d_WSetFilePointer:function 52
d_WSetFilePointer:
incbin EXE, 0x6219A, 52
global d_LoadWaveFormat:function 46
d_LoadWaveFormat:
incbin EXE, 0x621CE, 46
global d_AllocateMemFile:function 73
d_AllocateMemFile:
incbin EXE, 0x621FC, 73
global d_FreeMemFile:function 14
d_FreeMemFile:
incbin EXE, 0x62245, 14
global d_ReadWaveFile:function 202
d_ReadWaveFile:
incbin EXE, 0x62253, 202
global d_ReadMemFile:function 86
d_ReadMemFile:
incbin EXE, 0x6231D, 86
global d_FillMemFile:function 59
d_FillMemFile:
incbin EXE, 0x62373, 59
global d_SeekMemFile:function 30
d_SeekMemFile:
incbin EXE, 0x623AE, 30
global d_ReadWaveSection:function 87
d_ReadWaveSection:
incbin EXE, 0x623CC, 87
global d_LoadWaveFile:function 56
d_LoadWaveFile:
incbin EXE, 0x62423, 56
global d_j_engine_mem_free:function 5
d_j_engine_mem_free:
incbin EXE, 0x6245B, 5
global d_drawTopArchesUpperScreen:function 5677
d_drawTopArchesUpperScreen:
incbin EXE, 0x62460, 5677
global d_drawBottomArchesUpperScreen:function 3128
d_drawBottomArchesUpperScreen:
incbin EXE, 0x63A8D, 3128
global d_drawUpperScreen:function 3187
d_drawUpperScreen:
incbin EXE, 0x646C5, 3187
global d_drawTopArchesLowerScreen:function 6673
d_drawTopArchesLowerScreen:
incbin EXE, 0x65338, 6673
global d_drawBottomArchesLowerScreen:function 3874
d_drawBottomArchesLowerScreen:
incbin EXE, 0x66D49, 3874
global d_drawLowerScreen:function 3667
d_drawLowerScreen:
incbin EXE, 0x67C6B, 3667
global d_world_draw_black_tile:function 722
d_world_draw_black_tile:
incbin EXE, 0x68ABE, 722
global d_strlen:function 384
d_strlen:
incbin EXE, 0x68D90, 384
global d_sprintf:function 624
d_sprintf:
incbin EXE, 0x68F10, 624
global d_strcpy:function 16
d_strcpy:
incbin EXE, 0x69180, 16
global d_strcat:function 1936
d_strcat:
incbin EXE, 0x69190, 1936
global d_srand:function 16
d_srand:
incbin EXE, 0x69920, 16
global d_rand:function 3008
d_rand:
incbin EXE, 0x69930, 3008
global d_strchr:function 1264
d_strchr:
incbin EXE, 0x6A4F0, 1264
global d_time:function 53223
d_time:
incbin EXE, 0x6A9E0, 53223

global __end_text
__end_text:


SEGMENT .d.rdata   noexec

global __start_rdata
__start_rdata:

global imp_RegSetValueExA:data 4
imp_RegSetValueExA:
incbin EXE, 0x77A00, 4
global imp_RegQueryValueExA:data 4
imp_RegQueryValueExA:
incbin EXE, 0x77A04, 4
global imp_RegCloseKey:data 4
imp_RegCloseKey:
incbin EXE, 0x77A08, 4
global imp_GetUserNameA:data 4
imp_GetUserNameA:
incbin EXE, 0x77A0C, 4
global imp_RegOpenKeyExA:data 4
imp_RegOpenKeyExA:
incbin EXE, 0x77A10, 4
incbin EXE, 0x77A14, 4
global imp_UiValidPlayerName:data 4
imp_UiValidPlayerName:
incbin EXE, 0x77A18, 4
global imp_UiSelHeroSingDialog:data 4
imp_UiSelHeroSingDialog:
incbin EXE, 0x77A1C, 4
global imp_UiCreditsDialog:data 4
imp_UiCreditsDialog:
incbin EXE, 0x77A20, 4
global imp_UiMainMenuDialog:data 4
imp_UiMainMenuDialog:
incbin EXE, 0x77A24, 4
global imp_UiProgressDialog:data 4
imp_UiProgressDialog:
incbin EXE, 0x77A28, 4
global imp_UiProfileGetString:data 4
imp_UiProfileGetString:
incbin EXE, 0x77A2C, 4
global imp_UiProfileCallback:data 4
imp_UiProfileCallback:
incbin EXE, 0x77A30, 4
global imp_UiProfileDraw:data 4
imp_UiProfileDraw:
incbin EXE, 0x77A34, 4
global imp_UiCategoryCallback:data 4
imp_UiCategoryCallback:
incbin EXE, 0x77A38, 4
global imp_UiAppActivate:data 4
imp_UiAppActivate:
incbin EXE, 0x77A3C, 4
global imp_UiSelHeroMultDialog:data 4
imp_UiSelHeroMultDialog:
incbin EXE, 0x77A40, 4
global imp_UiSoundCallback:data 4
imp_UiSoundCallback:
incbin EXE, 0x77A44, 4
global imp_UiMessageBoxCallback:data 4
imp_UiMessageBoxCallback:
incbin EXE, 0x77A48, 4
global imp_UiDestroy:data 4
imp_UiDestroy:
incbin EXE, 0x77A4C, 4
global imp_UiCreateGameCallback:data 4
imp_UiCreateGameCallback:
incbin EXE, 0x77A50, 4
global imp_UiArtCallback:data 4
imp_UiArtCallback:
incbin EXE, 0x77A54, 4
global imp_UiSelectGame:data 4
imp_UiSelectGame:
incbin EXE, 0x77A58, 4
global imp_UiSelectProvider:data 4
imp_UiSelectProvider:
incbin EXE, 0x77A5C, 4
global imp_UiCreatePlayerDescription:data 4
imp_UiCreatePlayerDescription:
incbin EXE, 0x77A60, 4
global imp_UiSetupPlayerInfo:data 4
imp_UiSetupPlayerInfo:
incbin EXE, 0x77A64, 4
global imp_UiTitleDialog:data 4
imp_UiTitleDialog:
incbin EXE, 0x77A68, 4
global imp_UiCopyProtError:data 4
imp_UiCopyProtError:
incbin EXE, 0x77A6C, 4
global imp_UiInitialize:data 4
imp_UiInitialize:
incbin EXE, 0x77A70, 4
global imp_UiGetDataCallback:data 4
imp_UiGetDataCallback:
incbin EXE, 0x77A74, 4
global imp_UiAuthCallback:data 4
imp_UiAuthCallback:
incbin EXE, 0x77A78, 4
global imp_UiDrawDescCallback:data 4
imp_UiDrawDescCallback:
incbin EXE, 0x77A7C, 4
incbin EXE, 0x77A80, 4
global imp_GetSystemPaletteEntries:data 4
imp_GetSystemPaletteEntries:
incbin EXE, 0x77A84, 4
global imp_GetStockObject:data 4
imp_GetStockObject:
incbin EXE, 0x77A88, 4
global imp_GdiSetBatchLimit:data 4
imp_GdiSetBatchLimit:
incbin EXE, 0x77A8C, 4
global imp_GetDeviceCaps:data 4
imp_GetDeviceCaps:
incbin EXE, 0x77A90, 4
incbin EXE, 0x77A94, 4
global imp_SetEndOfFile:data 4
imp_SetEndOfFile:
incbin EXE, 0x77A98, 4
global imp_TlsSetValue:data 4
imp_TlsSetValue:
incbin EXE, 0x77A9C, 4
global imp_lstrcpynA:data 4
imp_lstrcpynA:
incbin EXE, 0x77AA0, 4
global imp_IsBadWritePtr:data 4
imp_IsBadWritePtr:
incbin EXE, 0x77AA4, 4
global imp_SetLastError:data 4
imp_SetLastError:
incbin EXE, 0x77AA8, 4
global imp_SetEnvironmentVariableA:data 4
imp_SetEnvironmentVariableA:
incbin EXE, 0x77AAC, 4
global imp_CompareStringW:data 4
imp_CompareStringW:
incbin EXE, 0x77AB0, 4
global imp_CompareStringA:data 4
imp_CompareStringA:
incbin EXE, 0x77AB4, 4
global imp_GetLocaleInfoW:data 4
imp_GetLocaleInfoW:
incbin EXE, 0x77AB8, 4
global imp_RaiseException:data 4
imp_RaiseException:
incbin EXE, 0x77ABC, 4
global imp_FlushFileBuffers:data 4
imp_FlushFileBuffers:
incbin EXE, 0x77AC0, 4
global imp_SetStdHandle:data 4
imp_SetStdHandle:
incbin EXE, 0x77AC4, 4
global imp_GetLocaleInfoA:data 4
imp_GetLocaleInfoA:
incbin EXE, 0x77AC8, 4
global imp_GetStringTypeW:data 4
imp_GetStringTypeW:
incbin EXE, 0x77ACC, 4
global imp_GetStringTypeA:data 4
imp_GetStringTypeA:
incbin EXE, 0x77AD0, 4
global imp_HeapCreate:data 4
imp_HeapCreate:
incbin EXE, 0x77AD4, 4
global imp_HeapDestroy:data 4
imp_HeapDestroy:
incbin EXE, 0x77AD8, 4
global imp_GetFileType:data 4
imp_GetFileType:
incbin EXE, 0x77ADC, 4
global imp_GetStdHandle:data 4
imp_GetStdHandle:
incbin EXE, 0x77AE0, 4
global imp_SetHandleCount:data 4
imp_SetHandleCount:
incbin EXE, 0x77AE4, 4
global imp_GetOEMCP:data 4
imp_GetOEMCP:
incbin EXE, 0x77AE8, 4
global imp_GetACP:data 4
imp_GetACP:
incbin EXE, 0x77AEC, 4
global imp_GetCPInfo:data 4
imp_GetCPInfo:
incbin EXE, 0x77AF0, 4
global imp_GetEnvironmentStringsW:data 4
imp_GetEnvironmentStringsW:
incbin EXE, 0x77AF4, 4
global imp_GetEnvironmentStrings:data 4
imp_GetEnvironmentStrings:
incbin EXE, 0x77AF8, 4
global imp_FreeEnvironmentStringsW:data 4
imp_FreeEnvironmentStringsW:
incbin EXE, 0x77AFC, 4
global imp_FreeEnvironmentStringsA:data 4
imp_FreeEnvironmentStringsA:
incbin EXE, 0x77B00, 4
global imp_RtlUnwind:data 4
imp_RtlUnwind:
incbin EXE, 0x77B04, 4
global imp_UnhandledExceptionFilter:data 4
imp_UnhandledExceptionFilter:
incbin EXE, 0x77B08, 4
global imp_HeapFree:data 4
imp_HeapFree:
incbin EXE, 0x77B0C, 4
global imp_HeapSize:data 4
imp_HeapSize:
incbin EXE, 0x77B10, 4
global imp_HeapAlloc:data 4
imp_HeapAlloc:
incbin EXE, 0x77B14, 4
global imp_HeapReAlloc:data 4
imp_HeapReAlloc:
incbin EXE, 0x77B18, 4
global imp_LCMapStringW:data 4
imp_LCMapStringW:
incbin EXE, 0x77B1C, 4
global imp_LCMapStringA:data 4
imp_LCMapStringA:
incbin EXE, 0x77B20, 4
global imp_WideCharToMultiByte:data 4
imp_WideCharToMultiByte:
incbin EXE, 0x77B24, 4
global imp_MultiByteToWideChar:data 4
imp_MultiByteToWideChar:
incbin EXE, 0x77B28, 4
global imp_TlsGetValue:data 4
imp_TlsGetValue:
incbin EXE, 0x77B2C, 4
global imp_TlsAlloc:data 4
imp_TlsAlloc:
incbin EXE, 0x77B30, 4
global imp_GetVersion:data 4
imp_GetVersion:
incbin EXE, 0x77B34, 4
global imp_FormatMessageA:data 4
imp_FormatMessageA:
incbin EXE, 0x77B38, 4
global imp_GetLastError:data 4
imp_GetLastError:
incbin EXE, 0x77B3C, 4
global imp_ExitProcess:data 4
imp_ExitProcess:
incbin EXE, 0x77B40, 4
global imp_Sleep:data 4
imp_Sleep:
incbin EXE, 0x77B44, 4
global imp_GetCurrentThreadId:data 4
imp_GetCurrentThreadId:
incbin EXE, 0x77B48, 4
global imp_DeleteFileA:data 4
imp_DeleteFileA:
incbin EXE, 0x77B4C, 4
global imp_CloseHandle:data 4
imp_CloseHandle:
incbin EXE, 0x77B50, 4
global imp_WriteFile:data 4
imp_WriteFile:
incbin EXE, 0x77B54, 4
global imp_CreateFileA:data 4
imp_CreateFileA:
incbin EXE, 0x77B58, 4
global imp_GetCurrentThread:data 4
imp_GetCurrentThread:
incbin EXE, 0x77B5C, 4
global imp_SetThreadPriority:data 4
imp_SetThreadPriority:
incbin EXE, 0x77B60, 4
global imp_CreateEventA:data 4
imp_CreateEventA:
incbin EXE, 0x77B64, 4
global imp_GetSystemTime:data 4
imp_GetSystemTime:
incbin EXE, 0x77B68, 4
global imp_GetTickCount:data 4
imp_GetTickCount:
incbin EXE, 0x77B6C, 4
global imp_GetModuleFileNameA:data 4
imp_GetModuleFileNameA:
incbin EXE, 0x77B70, 4
global imp_GetCurrentProcessId:data 4
imp_GetCurrentProcessId:
incbin EXE, 0x77B74, 4
global imp_InterlockedIncrement:data 4
imp_InterlockedIncrement:
incbin EXE, 0x77B78, 4
global imp_UnmapViewOfFile:data 4
imp_UnmapViewOfFile:
incbin EXE, 0x77B7C, 4
global imp_CreateProcessA:data 4
imp_CreateProcessA:
incbin EXE, 0x77B80, 4
global imp_MapViewOfFile:data 4
imp_MapViewOfFile:
incbin EXE, 0x77B84, 4
global imp_CreateFileMappingA:data 4
imp_CreateFileMappingA:
incbin EXE, 0x77B88, 4
global imp_GetSystemInfo:data 4
imp_GetSystemInfo:
incbin EXE, 0x77B8C, 4
global imp_GetPrivateProfileStringA:data 4
imp_GetPrivateProfileStringA:
incbin EXE, 0x77B90, 4
global imp_InitializeCriticalSection:data 4
imp_InitializeCriticalSection:
incbin EXE, 0x77B94, 4
global imp_DeleteCriticalSection:data 4
imp_DeleteCriticalSection:
incbin EXE, 0x77B98, 4
global imp_LeaveCriticalSection:data 4
imp_LeaveCriticalSection:
incbin EXE, 0x77B9C, 4
global imp_EnterCriticalSection:data 4
imp_EnterCriticalSection:
incbin EXE, 0x77BA0, 4
global imp_SetEvent:data 4
imp_SetEvent:
incbin EXE, 0x77BA4, 4
global imp_ResetEvent:data 4
imp_ResetEvent:
incbin EXE, 0x77BA8, 4
global imp_WaitForSingleObject:data 4
imp_WaitForSingleObject:
incbin EXE, 0x77BAC, 4
global imp_GetProcAddress:data 4
imp_GetProcAddress:
incbin EXE, 0x77BB0, 4
global imp_LoadLibraryA:data 4
imp_LoadLibraryA:
incbin EXE, 0x77BB4, 4
global imp_IsBadReadPtr:data 4
imp_IsBadReadPtr:
incbin EXE, 0x77BB8, 4
global imp_GetModuleHandleA:data 4
imp_GetModuleHandleA:
incbin EXE, 0x77BBC, 4
global imp_VirtualQuery:data 4
imp_VirtualQuery:
incbin EXE, 0x77BC0, 4
global imp_GetFileAttributesA:data 4
imp_GetFileAttributesA:
incbin EXE, 0x77BC4, 4
global imp_ReadFile:data 4
imp_ReadFile:
incbin EXE, 0x77BC8, 4
global imp_SetUnhandledExceptionFilter:data 4
imp_SetUnhandledExceptionFilter:
incbin EXE, 0x77BCC, 4
global imp_FindClose:data 4
imp_FindClose:
incbin EXE, 0x77BD0, 4
global imp_FindNextFileA:data 4
imp_FindNextFileA:
incbin EXE, 0x77BD4, 4
global imp_FindFirstFileA:data 4
imp_FindFirstFileA:
incbin EXE, 0x77BD8, 4
global imp_GetCurrentDirectoryA:data 4
imp_GetCurrentDirectoryA:
incbin EXE, 0x77BDC, 4
global imp_GetDriveTypeA:data 4
imp_GetDriveTypeA:
incbin EXE, 0x77BE0, 4
global imp_GetLogicalDriveStringsA:data 4
imp_GetLogicalDriveStringsA:
incbin EXE, 0x77BE4, 4
global imp_SetFilePointer:data 4
imp_SetFilePointer:
incbin EXE, 0x77BE8, 4
global imp_GetCommandLineA:data 4
imp_GetCommandLineA:
incbin EXE, 0x77BEC, 4
global imp_GetFileSize:data 4
imp_GetFileSize:
incbin EXE, 0x77BF0, 4
global imp_VirtualFree:data 4
imp_VirtualFree:
incbin EXE, 0x77BF4, 4
global imp_VirtualAlloc:data 4
imp_VirtualAlloc:
incbin EXE, 0x77BF8, 4
global imp_GetLocalTime:data 4
imp_GetLocalTime:
incbin EXE, 0x77BFC, 4
global imp_SetFileAttributesA:data 4
imp_SetFileAttributesA:
incbin EXE, 0x77C00, 4
global imp_TerminateProcess:data 4
imp_TerminateProcess:
incbin EXE, 0x77C04, 4
global imp_GetCurrentProcess:data 4
imp_GetCurrentProcess:
incbin EXE, 0x77C08, 4
global imp_GetWindowsDirectoryA:data 4
imp_GetWindowsDirectoryA:
incbin EXE, 0x77C0C, 4
global imp_GetDiskFreeSpaceA:data 4
imp_GetDiskFreeSpaceA:
incbin EXE, 0x77C10, 4
global imp_CopyFileA:data 4
imp_CopyFileA:
incbin EXE, 0x77C14, 4
global imp_OpenFile:data 4
imp_OpenFile:
incbin EXE, 0x77C18, 4
global imp_GetComputerNameA:data 4
imp_GetComputerNameA:
incbin EXE, 0x77C1C, 4
global imp_GetVersionExA:data 4
imp_GetVersionExA:
incbin EXE, 0x77C20, 4
global imp_InterlockedDecrement:data 4
imp_InterlockedDecrement:
incbin EXE, 0x77C24, 4
global imp_FileTimeToSystemTime:data 4
imp_FileTimeToSystemTime:
incbin EXE, 0x77C28, 4
global imp_FileTimeToLocalFileTime:data 4
imp_FileTimeToLocalFileTime:
incbin EXE, 0x77C2C, 4
global imp_ExitThread:data 4
imp_ExitThread:
incbin EXE, 0x77C30, 4
global imp_CreateThread:data 4
imp_CreateThread:
incbin EXE, 0x77C34, 4
global imp_GetStartupInfoA:data 4
imp_GetStartupInfoA:
incbin EXE, 0x77C38, 4
global imp_GetTimeZoneInformation:data 4
imp_GetTimeZoneInformation:
incbin EXE, 0x77C3C, 4
incbin EXE, 0x77C40, 4
global imp_ShellExecuteA:data 4
imp_ShellExecuteA:
incbin EXE, 0x77C44, 4
global imp_SHGetSpecialFolderLocation:data 4
imp_SHGetSpecialFolderLocation:
incbin EXE, 0x77C48, 4
global imp_SHGetPathFromIDListA:data 4
imp_SHGetPathFromIDListA:
incbin EXE, 0x77C4C, 4
incbin EXE, 0x77C50, 4
global imp_SFileGetFileArchive:data 4
imp_SFileGetFileArchive:
incbin EXE, 0x77C54, 4
global imp_SFileDdaSetVolume:data 4
imp_SFileDdaSetVolume:
incbin EXE, 0x77C58, 4
global imp_SFileDdaEnd:data 4
imp_SFileDdaEnd:
incbin EXE, 0x77C5C, 4
global imp_SVidDestroy:data 4
imp_SVidDestroy:
incbin EXE, 0x77C60, 4
global imp_SFileDdaDestroy:data 4
imp_SFileDdaDestroy:
incbin EXE, 0x77C64, 4
global imp_SVidInitialize:data 4
imp_SVidInitialize:
incbin EXE, 0x77C68, 4
global imp_SFileDdaInitialize:data 4
imp_SFileDdaInitialize:
incbin EXE, 0x77C6C, 4
global imp_SFileOpenFileEx:data 4
imp_SFileOpenFileEx:
incbin EXE, 0x77C70, 4
global imp_SFileGetFileSize:data 4
imp_SFileGetFileSize:
incbin EXE, 0x77C74, 4
global imp_SFileReadFile:data 4
imp_SFileReadFile:
incbin EXE, 0x77C78, 4
global imp_SFileSetFilePointer:data 4
imp_SFileSetFilePointer:
incbin EXE, 0x77C7C, 4
global imp_SRegSaveString:data 4
imp_SRegSaveString:
incbin EXE, 0x77C80, 4
global imp_SDrawUpdatePalette:data 4
imp_SDrawUpdatePalette:
incbin EXE, 0x77C84, 4
global imp_SNetGetProviderCaps:data 4
imp_SNetGetProviderCaps:
incbin EXE, 0x77C88, 4
global imp_SNetReceiveTurns:data 4
imp_SNetReceiveTurns:
incbin EXE, 0x77C8C, 4
global imp_SNetGetTurnsInTransit:data 4
imp_SNetGetTurnsInTransit:
incbin EXE, 0x77C90, 4
global imp_SNetSendTurn:data 4
imp_SNetSendTurn:
incbin EXE, 0x77C94, 4
global imp_SNetPerformUpgrade:data 4
imp_SNetPerformUpgrade:
incbin EXE, 0x77C98, 4
global imp_SNetInitializeProvider:data 4
imp_SNetInitializeProvider:
incbin EXE, 0x77C9C, 4
global imp_SNetCreateGame:data 4
imp_SNetCreateGame:
incbin EXE, 0x77CA0, 4
global imp_SDrawGetFrameWindow:data 4
imp_SDrawGetFrameWindow:
incbin EXE, 0x77CA4, 4
global imp_SFileOpenFile:data 4
imp_SFileOpenFile:
incbin EXE, 0x77CA8, 4
global imp_SNetGetGameInfo:data 4
imp_SNetGetGameInfo:
incbin EXE, 0x77CAC, 4
global imp_SNetRegisterEventHandler:data 4
imp_SNetRegisterEventHandler:
incbin EXE, 0x77CB0, 4
global imp_SNetUnregisterEventHandler:data 4
imp_SNetUnregisterEventHandler:
incbin EXE, 0x77CB4, 4
global imp_SNetReceiveMessage:data 4
imp_SNetReceiveMessage:
incbin EXE, 0x77CB8, 4
global imp_SNetSendMessage:data 4
imp_SNetSendMessage:
incbin EXE, 0x77CBC, 4
global imp_SNetSendServerChatCommand:data 4
imp_SNetSendServerChatCommand:
incbin EXE, 0x77CC0, 4
global imp_SNetDropPlayer:data 4
imp_SNetDropPlayer:
incbin EXE, 0x77CC4, 4
global imp_SNetGetOwnerTurnsWaiting:data 4
imp_SNetGetOwnerTurnsWaiting:
incbin EXE, 0x77CC8, 4
global imp_SErrGetLastError:data 4
imp_SErrGetLastError:
incbin EXE, 0x77CCC, 4
global imp_SRegSaveData:data 4
imp_SRegSaveData:
incbin EXE, 0x77CD0, 4
global imp_SRegLoadData:data 4
imp_SRegLoadData:
incbin EXE, 0x77CD4, 4
global imp_SVidPlayBegin:data 4
imp_SVidPlayBegin:
incbin EXE, 0x77CD8, 4
global imp_SVidPlayContinue:data 4
imp_SVidPlayContinue:
incbin EXE, 0x77CDC, 4
global imp_SVidPlayEnd:data 4
imp_SVidPlayEnd:
incbin EXE, 0x77CE0, 4
global imp_SErrSetLastError:data 4
imp_SErrSetLastError:
incbin EXE, 0x77CE4, 4
global imp_SStrCopy:data 4
imp_SStrCopy:
incbin EXE, 0x77CE8, 4
global imp_SDrawRealizePalette:data 4
imp_SDrawRealizePalette:
incbin EXE, 0x77CEC, 4
global imp_SFileSetBasePath:data 4
imp_SFileSetBasePath:
incbin EXE, 0x77CF0, 4
global imp_SFileOpenArchive:data 4
imp_SFileOpenArchive:
incbin EXE, 0x77CF4, 4
global imp_SRegLoadString:data 4
imp_SRegLoadString:
incbin EXE, 0x77CF8, 4
global imp_SFileCloseArchive:data 4
imp_SFileCloseArchive:
incbin EXE, 0x77CFC, 4
global imp_SNetSetBasePlayer:data 4
imp_SNetSetBasePlayer:
incbin EXE, 0x77D00, 4
global imp_StormDestroy:data 4
imp_StormDestroy:
incbin EXE, 0x77D04, 4
global imp_SFileDdaGetPos:data 4
imp_SFileDdaGetPos:
incbin EXE, 0x77D08, 4
global imp_SFileDdaBeginEx:data 4
imp_SFileDdaBeginEx:
incbin EXE, 0x77D0C, 4
global imp_SMemAlloc:data 4
imp_SMemAlloc:
incbin EXE, 0x77D10, 4
global imp_SErrGetErrorStr:data 4
imp_SErrGetErrorStr:
incbin EXE, 0x77D14, 4
global imp_SMemFree:data 4
imp_SMemFree:
incbin EXE, 0x77D18, 4
global imp_SNetLeaveGame:data 4
imp_SNetLeaveGame:
incbin EXE, 0x77D1C, 4
global imp_SDrawMessageBox:data 4
imp_SDrawMessageBox:
incbin EXE, 0x77D20, 4
global imp_SNetDestroy:data 4
imp_SNetDestroy:
incbin EXE, 0x77D24, 4
global imp_SRegLoadValue:data 4
imp_SRegLoadValue:
incbin EXE, 0x77D28, 4
global imp_SDrawManualInitialize:data 4
imp_SDrawManualInitialize:
incbin EXE, 0x77D2C, 4
global imp_SRegSaveValue:data 4
imp_SRegSaveValue:
incbin EXE, 0x77D30, 4
global imp_SFileCloseFile:data 4
imp_SFileCloseFile:
incbin EXE, 0x77D34, 4
global imp_SDrawDestroy:data 4
imp_SDrawDestroy:
incbin EXE, 0x77D38, 4
incbin EXE, 0x77D3C, 4
global imp_DestroyWindow:data 4
imp_DestroyWindow:
incbin EXE, 0x77D40, 4
global imp_SetFocus:data 4
imp_SetFocus:
incbin EXE, 0x77D44, 4
global imp_SetForegroundWindow:data 4
imp_SetForegroundWindow:
incbin EXE, 0x77D48, 4
global imp_GetTopWindow:data 4
imp_GetTopWindow:
incbin EXE, 0x77D4C, 4
global imp_TranslateMessage:data 4
imp_TranslateMessage:
incbin EXE, 0x77D50, 4
global imp_DispatchMessageA:data 4
imp_DispatchMessageA:
incbin EXE, 0x77D54, 4
global imp_GetLastActivePopup:data 4
imp_GetLastActivePopup:
incbin EXE, 0x77D58, 4
global imp_GetWindowThreadProcessId:data 4
imp_GetWindowThreadProcessId:
incbin EXE, 0x77D5C, 4
global imp_GetWindow:data 4
imp_GetWindow:
incbin EXE, 0x77D60, 4
global imp_GetForegroundWindow:data 4
imp_GetForegroundWindow:
incbin EXE, 0x77D64, 4
global imp_WaitForInputIdle:data 4
imp_WaitForInputIdle:
incbin EXE, 0x77D68, 4
global imp_SetCapture:data 4
imp_SetCapture:
incbin EXE, 0x77D6C, 4
global imp_GetSystemMetrics:data 4
imp_GetSystemMetrics:
incbin EXE, 0x77D70, 4
global imp_FindWindowA:data 4
imp_FindWindowA:
incbin EXE, 0x77D74, 4
global imp_SetCursorPos:data 4
imp_SetCursorPos:
incbin EXE, 0x77D78, 4
global imp_UpdateWindow:data 4
imp_UpdateWindow:
incbin EXE, 0x77D7C, 4
global imp_CreateWindowExA:data 4
imp_CreateWindowExA:
incbin EXE, 0x77D80, 4
global imp_RegisterClassExA:data 4
imp_RegisterClassExA:
incbin EXE, 0x77D84, 4
global imp_ShowCursor:data 4
imp_ShowCursor:
incbin EXE, 0x77D88, 4
global imp_LoadCursorA:data 4
imp_LoadCursorA:
incbin EXE, 0x77D8C, 4
global imp_LoadIconA:data 4
imp_LoadIconA:
incbin EXE, 0x77D90, 4
global imp_PostMessageA:data 4
imp_PostMessageA:
incbin EXE, 0x77D94, 4
global imp_GetClassNameA:data 4
imp_GetClassNameA:
incbin EXE, 0x77D98, 4
global imp_DefWindowProcA:data 4
imp_DefWindowProcA:
incbin EXE, 0x77D9C, 4
global imp_PostQuitMessage:data 4
imp_PostQuitMessage:
incbin EXE, 0x77DA0, 4
global imp_SetWindowLongA:data 4
imp_SetWindowLongA:
incbin EXE, 0x77DA4, 4
global imp_GetWindowLongA:data 4
imp_GetWindowLongA:
incbin EXE, 0x77DA8, 4
global imp_InvalidateRect:data 4
imp_InvalidateRect:
incbin EXE, 0x77DAC, 4
global imp_ReleaseCapture:data 4
imp_ReleaseCapture:
incbin EXE, 0x77DB0, 4
global imp_PeekMessageA:data 4
imp_PeekMessageA:
incbin EXE, 0x77DB4, 4
global imp_DialogBoxParamA:data 4
imp_DialogBoxParamA:
incbin EXE, 0x77DB8, 4
global imp_SetDlgItemTextA:data 4
imp_SetDlgItemTextA:
incbin EXE, 0x77DBC, 4
global imp_EndDialog:data 4
imp_EndDialog:
incbin EXE, 0x77DC0, 4
global imp_ReleaseDC:data 4
imp_ReleaseDC:
incbin EXE, 0x77DC4, 4
global imp_GetWindowRect:data 4
imp_GetWindowRect:
incbin EXE, 0x77DC8, 4
global imp_GetDC:data 4
imp_GetDC:
incbin EXE, 0x77DCC, 4
global imp_ShowWindow:data 4
imp_ShowWindow:
incbin EXE, 0x77DD0, 4
global imp_GetDesktopWindow:data 4
imp_GetDesktopWindow:
incbin EXE, 0x77DD4, 4
global imp_LoadImageA:data 4
imp_LoadImageA:
incbin EXE, 0x77DD8, 4
global imp_wvsprintfA:data 4
imp_wvsprintfA:
incbin EXE, 0x77DDC, 4
global imp_SetWindowPos:data 4
imp_SetWindowPos:
incbin EXE, 0x77DE0, 4
global imp_MessageBoxA:data 4
imp_MessageBoxA:
incbin EXE, 0x77DE4, 4
global imp_wsprintfA:data 4
imp_wsprintfA:
incbin EXE, 0x77DE8, 4
incbin EXE, 0x77DEC, 4
global imp_GetFileVersionInfoA:data 4
imp_GetFileVersionInfoA:
incbin EXE, 0x77DF0, 4
global imp_GetFileVersionInfoSizeA:data 4
imp_GetFileVersionInfoSizeA:
incbin EXE, 0x77DF4, 4
global imp_VerQueryValueA:data 4
imp_VerQueryValueA:
incbin EXE, 0x77DF8, 4
incbin EXE, 0x77DFC, 36670

global __end_rdata
__end_rdata:


SEGMENT .d.data    noexec write

global __start_data
__start_data:

incbin EXE, 0x80E00, 114432
global sghArchive:data 13268
sghArchive:
incbin EXE, 0x9CD00, 13268
global gbMusicOn:data 1
gbMusicOn:
incbin EXE, 0xA00D4, 1
global gbSoundOn:data 1
gbSoundOn:
incbin EXE, 0xA00D5, 1
global gbDupSounds:data 69510
gbDupSounds:
incbin EXE, 0xA00D6, 69510
global WorldBoolFlag:data 4
WorldBoolFlag:
incbin EXE, 0xB105C, 4
global gdwCurrentMask:data 5
gdwCurrentMask:
incbin EXE, 0xB1060, 5
global gpCelFrame:data 4
gpCelFrame:
incbin EXE, 0xB1065, 4
global gpDrawMask:data 18119
gpDrawMask:
incbin EXE, 0xB1069, 18119

global __end_data
__end_data:


SEGMENT .d.bss     noexec write nobits

global __start_bss
__start_bss:

global appfat_terminated:data 4
appfat_terminated:
resb 4
global sz_error_buf:data 256
sz_error_buf:
resb 256
global terminating:data 4
terminating:
resb 4
global cleanup_thread_id:data 8
cleanup_thread_id:
resb 8
global automaptype:data 1024
automaptype:
resb 1024
global MapX:data 4
MapX:
resb 4
global MapY:data 4
MapY:
resb 4
global automapflag:data 4
automapflag:
resb 4
global AMbyte_4B7E4C:data 32
AMbyte_4B7E4C:
resb 32
global automapview:data 1600
automapview:
resb 1600
global AutoMapScale:data 4
AutoMapScale:
resb 4
global AutoMapXOfs:data 4
AutoMapXOfs:
resb 4
global AutoMapYOfs:data 4
AutoMapYOfs:
resb 4
global AutoMapPosBits:data 4
AutoMapPosBits:
resb 4
global AutoMapXPos:data 4
AutoMapXPos:
resb 4
global AutoMapYPos:data 4
AutoMapYPos:
resb 4
global AMPlayerX:data 4
AMPlayerX:
resb 4
global AMPlayerY:data 4
AMPlayerY:
resb 4
global sgbNextTalkSave:data 1
sgbNextTalkSave:
resb 1
global sgbTalkSavePos:data 3
sgbTalkSavePos:
resb 3
global pDurIcons:data 4
pDurIcons:
resb 4
global pChrButtons:data 4
pChrButtons:
resb 4
global drawhpflag:data 4
drawhpflag:
resb 4
global dropGoldFlag:data 4
dropGoldFlag:
resb 4
global panbtn:data 32
panbtn:
resb 32
global chrbtn:data 16
chrbtn:
resb 16
global pMultiBtns:data 4
pMultiBtns:
resb 4
global pPanelButtons:data 4
pPanelButtons:
resb 4
global pChrPanel:data 4
pChrPanel:
resb 4
global lvlbtndown:data 4
lvlbtndown:
resb 4
global sgszTalkSave:data 640
sgszTalkSave:
resb 640
global dropGoldValue:data 4
dropGoldValue:
resb 4
global drawmanaflag:data 4
drawmanaflag:
resb 4
global chrbtnactive:data 4
chrbtnactive:
resb 4
global sgszTalkMsg:data 80
sgszTalkMsg:
resb 80
global pPanelText:data 4
pPanelText:
resb 4
global frame_4B8800:data 4
frame_4B8800:
resb 4
global pLifeBuff:data 4
pLifeBuff:
resb 4
global pBtmBuff:data 4
pBtmBuff:
resb 4
global pTalkBtns:data 4
pTalkBtns:
resb 4
global pstrjust:data 16
pstrjust:
resb 16
global pnumlines:data 4
pnumlines:
resb 4
global pinfoflag:data 4
pinfoflag:
resb 4
global talkbtndown:data 12
talkbtndown:
resb 12
global pSpell:data 4
pSpell:
resb 4
global pManaBuff:data 4
pManaBuff:
resb 4
global infoclr:data 4
infoclr:
resb 4
global sgbPlrTalkTbl:data 4
sgbPlrTalkTbl:
resb 4
global pGBoxBuff:data 4
pGBoxBuff:
resb 4
global pSBkBtnCel:data 4
pSBkBtnCel:
resb 4
global tempstr:data 260
tempstr:
resb 260
global sbooktab:data 4
sbooktab:
resb 4
global pSplType:data 4
pSplType:
resb 4
global frame:data 4
frame:
resb 4
global initialDropGoldIndex:data 4
initialDropGoldIndex:
resb 4
global talkflag:data 4
talkflag:
resb 4
global pSBkIconCels:data 4
pSBkIconCels:
resb 4
global sbookflag:data 4
sbookflag:
resb 4
global chrflag:data 4
chrflag:
resb 4
global drawbtnflag:data 4
drawbtnflag:
resb 4
global pSpellBkCel:data 4
pSpellBkCel:
resb 4
global infostr:data 260
infostr:
resb 260
global numpanbtns:data 4
numpanbtns:
resb 4
global pStatusPanel:data 4
pStatusPanel:
resb 4
global panelstr:data 256
panelstr:
resb 256
global panelflag:data 4
panelflag:
resb 4
global byte_4B8B88:data 256
byte_4B8B88:
resb 256
global initialDropGoldValue:data 4
initialDropGoldValue:
resb 4
global pSpellCels:data 4
pSpellCels:
resb 4
global panbtndown:data 4
panbtndown:
resb 4
global pTalkPanel:data 4
pTalkPanel:
resb 4
global spselflag:data 4
spselflag:
resb 4
global cursH:data 4
cursH:
resb 4
global icursH28:data 4
icursH28:
resb 4
global cursW:data 4
cursW:
resb 4
global pcursmonst:data 4
pcursmonst:
resb 4
global icursW28:data 4
icursW28:
resb 4
global pCursCels:data 4
pCursCels:
resb 4
global icursH:data 4
icursH:
resb 4
global pcursinvitem:data 4
pcursinvitem:
resb 4
global icursW:data 4
icursW:
resb 4
global pcursitem:data 1
pcursitem:
resb 1
global pcursobj:data 1
pcursobj:
resb 1
global pcursplr:data 2
pcursplr:
resb 2
global cursmx:data 4
cursmx:
resb 4
global cursmy:data 4
cursmy:
resb 4
global dword_4B8CCC:data 4
dword_4B8CCC:
resb 4
global pcurs:data 8
pcurs:
resb 8
global spurtndx:data 8
spurtndx:
resb 8
global dead:data 1488
dead:
resb 1488
global stonendx:data 16460
stonendx:
resb 16460
global pSquareCel:data 4
pSquareCel:
resb 4
global dMonsDbg:data 213248
dMonsDbg:
resb 213248
global dFlagDbg:data 213268
dFlagDbg:
resb 213268
global diablo_cpp_init_value:data 4
diablo_cpp_init_value:
resb 4
global ghMainWnd:data 24
ghMainWnd:
resb 24
global glMid1Seed:data 72
glMid1Seed:
resb 72
global glMid2Seed:data 68
glMid2Seed:
resb 68
global gnLevelTypeTbl:data 68
gnLevelTypeTbl:
resb 68
global MouseY:data 4
MouseY:
resb 4
global MouseX:data 4
MouseX:
resb 4
global gbGameLoopStartup:data 4
gbGameLoopStartup:
resb 4
global glSeedTbl:data 68
glSeedTbl:
resb 68
global gbRunGame:data 4
gbRunGame:
resb 4
global glMid3Seed:data 68
glMid3Seed:
resb 68
global gbRunGameResult:data 4
gbRunGameResult:
resb 4
global zoomflag:data 4
zoomflag:
resb 4
global gbProcessPlayers:data 4
gbProcessPlayers:
resb 4
global glEndSeed:data 68
glEndSeed:
resb 68
global dword_5256E8:data 4
dword_5256E8:
resb 4
global ghInst:data 4
ghInst:
resb 4
global DebugMonsters:data 40
DebugMonsters:
resb 40
global cineflag:data 4
cineflag:
resb 4
global drawpanflag:data 4
drawpanflag:
resb 4
global visiondebug:data 4
visiondebug:
resb 4
global scrollflag:data 4
scrollflag:
resb 4
global light4flag:data 4
light4flag:
resb 4
global leveldebug:data 4
leveldebug:
resb 4
global monstdebug:data 4
monstdebug:
resb 4
global trigdebug:data 4
trigdebug:
resb 4
global setseed:data 4
setseed:
resb 4
global debugmonsttypes:data 4
debugmonsttypes:
resb 4
global PauseMode:data 4
PauseMode:
resb 4
global sgnTimeoutCurs:data 4
sgnTimeoutCurs:
resb 4
global sgbMouseDown:data 4
sgbMouseDown:
resb 4
global color_cycle_timer:data 12
color_cycle_timer:
resb 12
global pDoomCel:data 4
pDoomCel:
resb 4
global doomflag:data 8
doomflag:
resb 8
global L5dungeon:data 6400
L5dungeon:
resb 6400
global mydflags:data 1600
mydflags:
resb 1600
global setloadflag:data 4
setloadflag:
resb 4
global HR1:data 4
HR1:
resb 4
global HR2:data 4
HR2:
resb 4
global HR3:data 4
HR3:
resb 4
global VR1:data 4
VR1:
resb 4
global VR2:data 4
VR2:
resb 4
global VR3:data 4
VR3:
resb 4
global pSetPiece:data 8
pSetPiece:
resb 8
global nSx1:data 4
nSx1:
resb 4
global nSx2:data 4
nSx2:
resb 4
global nSy1:data 4
nSy1:
resb 4
global nSy2:data 4
nSy2:
resb 4
global nRoomCnt:data 4
nRoomCnt:
resb 4
global predungeon:data 1604
predungeon:
resb 1604
global RoomList:data 1620
RoomList:
resb 1620
global pHallList:data 4
pHallList:
resb 4
global lavapool:data 4
lavapool:
resb 4
global abyssx:data 4
abyssx:
resb 4
global lockoutcnt:data 4
lockoutcnt:
resb 4
global lockout:data 1600
lockout:
resb 1600
global diabquad1x:data 4
diabquad1x:
resb 4
global diabquad1y:data 4
diabquad1y:
resb 4
global diabquad3x:data 4
diabquad3x:
resb 4
global diabquad3y:data 4
diabquad3y:
resb 4
global diabquad2x:data 4
diabquad2x:
resb 4
global diabquad2y:data 4
diabquad2y:
resb 4
global diabquad4x:data 4
diabquad4x:
resb 4
global diabquad4y:data 4
diabquad4y:
resb 4
global hallok:data 80
hallok:
resb 80
global l4holdx:data 4
l4holdx:
resb 4
global l4holdy:data 4
l4holdy:
resb 4
global SP4x1:data 4
SP4x1:
resb 4
global SP4x2:data 4
SP4x2:
resb 4
global SP4y1:data 4
SP4y1:
resb 4
global SP4y2:data 4
SP4y2:
resb 4
global L4dungeon:data 6400
L4dungeon:
resb 6400
global dung:data 404
dung:
resb 404
global dthread_cpp_init_value:data 8
dthread_cpp_init_value:
resb 8
global CriticalSection:data 24
CriticalSection:
resb 24
global ThreadId:data 4
ThreadId:
resb 4
global sgpInfoHead:data 8
sgpInfoHead:
resb 8
global sghWorkToDoEvent:data 4
sghWorkToDoEvent:
resb 4
global sgpBackBuf:data 4
sgpBackBuf:
resb 4
global dx_cpp_init_value:data 4
dx_cpp_init_value:
resb 4
global lpDDInterface:data 4
lpDDInterface:
resb 4
global lpDDPalette:data 4
lpDDPalette:
resb 4
global sgdwLockCount:data 4
sgdwLockCount:
resb 4
global gpBuffer:data 4
gpBuffer:
resb 4
global lpDDSBackBuf:data 4
lpDDSBackBuf:
resb 4
global lpDDSPrimary:data 28
lpDDSPrimary:
resb 28
global gbBackBuf:data 1
gbBackBuf:
resb 1
global gbEmulate:data 3
gbEmulate:
resb 3
global hModule:data 4
hModule:
resb 4
global effects_cpp_init_value:data 4
effects_cpp_init_value:
resb 4
global sfxdelay:data 4
sfxdelay:
resb 4
global sfxdnum:data 4
sfxdnum:
resb 4
global sfx_stream:data 4
sfx_stream:
resb 4
global sfx_data_cur:data 4
sfx_data_cur:
resb 4
global hashtable:data 5124
hashtable:
resb 5124
global engine_cpp_init_value:data 4
engine_cpp_init_value:
resb 4
global gbPixelCol:data 4
gbPixelCol:
resb 4
global dword_52B970:data 4
dword_52B970:
resb 4
global orgseed:data 4
orgseed:
resb 4
global sgnWidth:data 4
sgnWidth:
resb 4
global sglGameSeed:data 4
sglGameSeed:
resb 4
global sgMemCrit:data 24
sgMemCrit:
resb 24
global SeedCount:data 4
SeedCount:
resb 4
global dword_52B99C:data 4
dword_52B99C:
resb 4
global msgtable:data 80
msgtable:
resb 80
global msgdelay:data 1
msgdelay:
resb 1
global msgflag:data 1
msgflag:
resb 1
global msgcnt:data 6
msgcnt:
resb 6
global lpTopLevelExceptionFilter:data 8
lpTopLevelExceptionFilter:
resb 8
global level_frame_types:data 4096
level_frame_types:
resb 4096
global themeCount:data 4
themeCount:
resb 4
global nTransTable:data 2052
nTransTable:
resb 2052
global dMonster:data 50176
dMonster:
resb 50176
global dungeon:data 1600
dungeon:
resb 1600
global dObject:data 12544
dObject:
resb 12544
global pSpeedCels:data 4
pSpeedCels:
resb 4
global nlevel_frames:data 4
nlevel_frames:
resb 4
global pdungeon:data 1600
pdungeon:
resb 1600
global dDead:data 12544
dDead:
resb 12544
global dpiece_defs_map_1:data 401408
dpiece_defs_map_1:
resb 401408
global dTransVal2:data 12544
dTransVal2:
resb 12544
global TransVal:data 4
TransVal:
resb 4
global dword_5A5594:data 4
dword_5A5594:
resb 4
global dflags:data 1600
dflags:
resb 1600
global dPiece:data 50176
dPiece:
resb 50176
global dTransVal:data 12544
dTransVal:
resb 12544
global setloadflag_2:data 4
setloadflag_2:
resb 4
global tile_defs:data 8192
tile_defs:
resb 8192
global pMegaTiles:data 4
pMegaTiles:
resb 4
global pLevelPieces:data 4
pLevelPieces:
resb 4
global gnDifficulty:data 4
gnDifficulty:
resb 4
global block_lvid:data 2052
block_lvid:
resb 2052
global dung_map:data 12544
dung_map:
resb 12544
global nTrapTable:data 2049
nTrapTable:
resb 2049
global leveltype:data 1
leveltype:
resb 1
global currlevel:data 2
currlevel:
resb 2
global TransList:data 256
TransList:
resb 256
global nSolidTable:data 2052
nSolidTable:
resb 2052
global level_frame_count:data 8196
level_frame_count:
resb 8196
global ScrollInfo:data 20
ScrollInfo:
resb 20
global pDungeonCels:data 4
pDungeonCels:
resb 4
global speed_cel_frame_num_from_light_index_frame_num:data 8192
speed_cel_frame_num_from_light_index_frame_num:
resb 8192
global themeLoc:data 1000
themeLoc:
resb 1000
global dPlayer:data 12544
dPlayer:
resb 12544
global dword_5C2FF8:data 4
dword_5C2FF8:
resb 4
global dword_5C2FFC:data 4
dword_5C2FFC:
resb 4
global scr_pix_width:data 4
scr_pix_width:
resb 4
global scr_pix_height:data 4
scr_pix_height:
resb 4
global dArch:data 12544
dArch:
resb 12544
global nBlockTable:data 2052
nBlockTable:
resb 2052
global level_special_cel:data 4
level_special_cel:
resb 4
global dFlags:data 12544
dFlags:
resb 12544
global dItem:data 12544
dItem:
resb 12544
global setlvlnum:data 4
setlvlnum:
resb 4
global level_frame_sizes:data 8192
level_frame_sizes:
resb 8192
global nMissileTable:data 2052
nMissileTable:
resb 2052
global pSetPiece_2:data 4
pSetPiece_2:
resb 4
global setlvltype:data 1
setlvltype:
resb 1
global setlevel:data 3
setlevel:
resb 3
global LvlViewY:data 4
LvlViewY:
resb 4
global LvlViewX:data 4
LvlViewX:
resb 4
global dmaxx:data 4
dmaxx:
resb 4
global dmaxy:data 4
dmaxy:
resb 4
global setpc_h:data 4
setpc_h:
resb 4
global setpc_w:data 4
setpc_w:
resb 4
global setpc_x:data 4
setpc_x:
resb 4
global ViewX:data 4
ViewX:
resb 4
global ViewY:data 4
ViewY:
resb 4
global setpc_y:data 12
setpc_y:
resb 12
global dMissile:data 12552
dMissile:
resb 12552
global dminx:data 4
dminx:
resb 4
global dminy:data 4
dminy:
resb 4
global dpiece_defs_map_2:data 401408
dpiece_defs_map_2:
resb 401408
global optbar_cel:data 4
optbar_cel:
resb 4
global byte_634464:data 4
byte_634464:
resb 4
global PentSpin_cel:data 4
PentSpin_cel:
resb 4
global sgpCurrItem:data 4
sgpCurrItem:
resb 4
global BigTGold_cel:data 4
BigTGold_cel:
resb 4
global dword_634474:data 4
dword_634474:
resb 4
global byte_634478:data 4
byte_634478:
resb 4
global dword_63447C:data 4
dword_63447C:
resb 4
global dword_634480:data 4
dword_634480:
resb 4
global option_cel:data 4
option_cel:
resb 4
global sgpLogo:data 4
sgpLogo:
resb 4
global dword_63448C:data 4
dword_63448C:
resb 4
global help_select_line:data 4
help_select_line:
resb 4
global dword_634494:data 4
dword_634494:
resb 4
global helpflag:data 1136
helpflag:
resb 1136
global displayinghelp:data 88
displayinghelp:
resb 88
global HelpTop:data 8
HelpTop:
resb 8
global fileinfo:data 20
fileinfo:
resb 20
global init_cpp_init_value:data 4
init_cpp_init_value:
resb 4
global gbActive:data 4
gbActive:
resb 4
global diablo_exe_path:data 260
diablo_exe_path:
resb 260
global unused_mpq:data 4
unused_mpq:
resb 4
global patch_rt_mpq_path:data 260
patch_rt_mpq_path:
resb 260
global CurrentProc:data 4
CurrentProc:
resb 4
global diabdat_mpq:data 4
diabdat_mpq:
resb 4
global diabdat_mpq_path:data 260
diabdat_mpq_path:
resb 260
global patch_rt_mpq:data 4
patch_rt_mpq:
resb 4
global killed_mom_parent:data 4
killed_mom_parent:
resb 4
global screensaver_enabled_prev:data 4
screensaver_enabled_prev:
resb 4
global sgpBackCel:data 4
sgpBackCel:
resb 4
global interfac_cpp_init_value:data 4
interfac_cpp_init_value:
resb 4
global sgdwProgress:data 4
sgdwProgress:
resb 4
global progress_id:data 4
progress_id:
resb 4
global invflag:data 4
invflag:
resb 4
global pInvCels:data 4
pInvCels:
resb 4
global drawsbarflag:data 4
drawsbarflag:
resb 4
global sgdwLastTime:data 4
sgdwLastTime:
resb 4
global itemactive:data 508
itemactive:
resb 508
global uitemflag:data 4
uitemflag:
resb 4
global itemavail:data 512
itemavail:
resb 512
global curruitem:data 368
curruitem:
resb 368
global itemrecord:data 1664
itemrecord:
resb 1664
global doppitem:data 368
doppitem:
resb 368
global item:data 47104
item:
resb 47104
global itemhold:data 40
itemhold:
resb 40
global Item2Frm:data 140
Item2Frm:
resb 140
global UniqueItemFlag:data 512
UniqueItemFlag:
resb 512
global numitems:data 4
numitems:
resb 4
global gnNumGetRecords:data 8
gnNumGetRecords:
resb 8
global VisionList:data 1664
VisionList:
resb 1664
global lightactive:data 32
lightactive:
resb 32
global LightList:data 1664
LightList:
resb 1664
global numlights:data 4
numlights:
resb 4
global dung_map_radius:data 2048
dung_map_radius:
resb 2048
global dovision:data 4
dovision:
resb 4
global numvision:data 4
numvision:
resb 4
global lightmax:data 4
lightmax:
resb 4
global dolighting:data 4
dolighting:
resb 4
global dung_map_rgba:data 16384
dung_map_rgba:
resb 16384
global visionid:data 4
visionid:
resb 4
global pLightTbl:data 8
pLightTbl:
resb 8
global lightflag:data 4
lightflag:
resb 4
global tbuff:data 36
tbuff:
resb 36
global FileName:data 260
FileName:
resb 260
global log_buffer:data 388
log_buffer:
resb 388
global lpAddress:data 4
lpAddress:
resb 4
global nNumberOfBytesToWrite:data 4
nNumberOfBytesToWrite:
resb 4
global mainmenu_cpp_init_value:data 4
mainmenu_cpp_init_value:
resb 4
global chr_name_str:data 16
chr_name_str:
resb 16
global qtexty:data 4
qtexty:
resb 4
global qtextptr:data 4
qtextptr:
resb 4
global qtextSpd:data 4
qtextSpd:
resb 4
global qtextflag:data 4
qtextflag:
resb 4
global scrolltexty:data 4
scrolltexty:
resb 4
global sgLastScroll:data 4
sgLastScroll:
resb 4
global pMedTextCels:data 4
pMedTextCels:
resb 4
global pTextBoxCels:data 8
pTextBoxCels:
resb 8
global missileactive:data 500
missileactive:
resb 500
global missileavail:data 500
missileavail:
resb 500
global missile:data 22000
missile:
resb 22000
global nummissiles:data 4
nummissiles:
resb 4
global ManashieldFlag:data 4
ManashieldFlag:
resb 4
global chain:data 1500
chain:
resb 1500
global MissilePreFlag:data 4
MissilePreFlag:
resb 4
global numchains:data 8
numchains:
resb 8
global MissileFileFlag:data 4
MissileFileFlag:
resb 4
global monster_cpp_init_value:data 4
monster_cpp_init_value:
resb 4
global monstkills:data 800
monstkills:
resb 800
global monstactive:data 800
monstactive:
resb 800
global nummonsters:data 4
nummonsters:
resb 4
global sgbSaveSoundOn:data 4
sgbSaveSoundOn:
resb 4
global monster:data 45600
monster:
resb 45600
global totalmonsters:data 8
totalmonsters:
resb 8
global Monsters:data 5249
Monsters:
resb 5249
global END_Monsters_17:data 271
END_Monsters_17:
resb 271
global monstimgtot:data 4
monstimgtot:
resb 4
global uniquetrans:data 4
uniquetrans:
resb 4
global nummtypes:data 4
nummtypes:
resb 4
global movie_cpp_init_value:data 4
movie_cpp_init_value:
resb 4
global movie_playing:data 4
movie_playing:
resb 4
global loop_movie:data 4
loop_movie:
resb 4
global mpqapi_cpp_init_value:data 4
mpqapi_cpp_init_value:
resb 4
global sgdwMpqOffset:data 4
sgdwMpqOffset:
resb 4
global mpq_buf:data 4096
mpq_buf:
resb 4096
global sgpHashTbl:data 4
sgpHashTbl:
resb 4
global save_archive_modified:data 4
save_archive_modified:
resb 4
global sgpBlockTbl:data 4
sgpBlockTbl:
resb 4
global save_archive_open:data 4
save_archive_open:
resb 4
global sgdwOwnerWait:data 4
sgdwOwnerWait:
resb 4
global msg_cpp_init_value:data 4
msg_cpp_init_value:
resb 4
global sgdwRecvOffset:data 4
sgdwRecvOffset:
resb 4
global sgnCurrMegaPlayer:data 4
sgnCurrMegaPlayer:
resb 4
global sgLevels:data 80257
sgLevels:
resb 80257
global sbLastCmd:data 3
sbLastCmd:
resb 3
global sgpCurrPkt:data 4
sgpCurrPkt:
resb 4
global sgRecvBuf:data 4722
sgRecvBuf:
resb 4722
global sgbRecvCmd:data 6
sgbRecvCmd:
resb 6
global sgLocals:data 27200
sgLocals:
resb 27200
global sgJunk:data 32
sgJunk:
resb 32
global sgpMegaPkt:data 4
sgpMegaPkt:
resb 4
global sgbDeltaChanged:data 1
sgbDeltaChanged:
resb 1
global sgbDeltaChunks:data 3
sgbDeltaChunks:
resb 3
global deltaload:data 4
deltaload:
resb 4
global gbBufferMsgs:data 4
gbBufferMsgs:
resb 4
global dword_676198:data 4
dword_676198:
resb 4
global msg_err_timer:data 4
msg_err_timer:
resb 4
global msgcmd_cpp_init_value:data 8
msgcmd_cpp_init_value:
resb 8
global sgChat_Cmd:data 12
sgChat_Cmd:
resb 12
global sgdwMsgCmdTimer:data 4
sgdwMsgCmdTimer:
resb 4
global gbSomebodyWonGameKludge:data 8
gbSomebodyWonGameKludge:
resb 8
global pkdata_6761C0:data 4100
pkdata_6761C0:
resb 4100
global szPlayerDescript:data 128
szPlayerDescript:
resb 128
global sgwPackPlrOffsetTbl:data 12
sgwPackPlrOffsetTbl:
resb 12
global netplr:data 5064
netplr:
resb 5064
global sgbPlayerTurnBitTbl:data 4
sgbPlayerTurnBitTbl:
resb 4
global sgbPlayerLeftGameTbl:data 4
sgbPlayerLeftGameTbl:
resb 4
global multi_cpp_init_value:data 4
multi_cpp_init_value:
resb 4
global sgbSentThisCycle:data 4
sgbSentThisCycle:
resb 4
global dword_678628:data 4
dword_678628:
resb 4
global gbActivePlayers:data 1
gbActivePlayers:
resb 1
global gbGameDestroyed:data 3
gbGameDestroyed:
resb 3
global sgbSendDeltaTbl:data 8
sgbSendDeltaTbl:
resb 8
global sgGameInitInfo:data 8
sgGameInitInfo:
resb 8
global byte_678640:data 4
byte_678640:
resb 4
global sglTimeoutStart:data 4
sglTimeoutStart:
resb 4
global sgdwPlayerLeftReasonTbl:data 16
sgdwPlayerLeftReasonTbl:
resb 16
global pkdata_678658:data 4100
pkdata_678658:
resb 4100
global sgdwGameLoops:data 4
sgdwGameLoops:
resb 4
global gbMaxPlayers:data 1
gbMaxPlayers:
resb 1
global sgbTimeout:data 3
sgbTimeout:
resb 3
global szPlayerName:data 128
szPlayerName:
resb 128
global gbDeltaSender:data 4
gbDeltaSender:
resb 4
global sgbNetInited:data 8
sgbNetInited:
resb 8
global player_state:data 16
player_state:
resb 16
global nthread_cpp_init_value:data 4
nthread_cpp_init_value:
resb 4
global byte_679704:data 4
byte_679704:
resb 4
global gdwMsgLenTbl:data 40
gdwMsgLenTbl:
resb 40
global gdwDeltaBytesSec:data 8
gdwDeltaBytesSec:
resb 8
global gdwTurnsInTransit:data 4
gdwTurnsInTransit:
resb 4
global glpMsgTbl:data 20
glpMsgTbl:
resb 20
global sgbSyncCountdown:data 9
sgbSyncCountdown:
resb 9
global sgbPacketCountdown:data 1
sgbPacketCountdown:
resb 1
global sgbThreadIsRunning:data 2
sgbThreadIsRunning:
resb 2
global gdwLargestMsgSize:data 4
gdwLargestMsgSize:
resb 4
global gdwNormalMsgSize:data 8
gdwNormalMsgSize:
resb 8
global trapid:data 4
trapid:
resb 4
global trapdir:data 4
trapdir:
resb 4
global pObjCels:data 160
pObjCels:
resb 160
global ObjFileList:data 40
ObjFileList:
resb 40
global objectactive:data 508
objectactive:
resb 508
global nobjects:data 4
nobjects:
resb 4
global leverid:data 4
leverid:
resb 4
global objectavail:data 508
objectavail:
resb 508
global object:data 15240
object:
resb 15240
global InitObjFlag:data 4
InitObjFlag:
resb 4
global numobjfiles:data 4
numobjfiles:
resb 4
global hero_cpp_init_value:data 4
hero_cpp_init_value:
resb 4
global logical_palette:data 1024
logical_palette:
resb 1024
global palette_cpp_init_value:data 4
palette_cpp_init_value:
resb 4
global system_palette:data 1024
system_palette:
resb 1024
global orig_palette:data 1024
orig_palette:
resb 1024
global gdwPalEntries:data 8
gdwPalEntries:
resb 8
global path_nodes:data 15600
path_nodes:
resb 15600
global gdwCurPathStep:data 4
gdwCurPathStep:
resb 4
global gdwCurNodes:data 4
gdwCurNodes:
resb 4
global pnode_vals:data 100
pnode_vals:
resb 100
global pnode_ptr:data 4
pnode_ptr:
resb 4
global pnode_tblptr:data 1200
pnode_tblptr:
resb 1200
global path_2_nodes:data 15608
path_2_nodes:
resb 15608
global pfile_cpp_init_value:data 4
pfile_cpp_init_value:
resb 4
global hero_names:data 320
hero_names:
resb 320
global gbValidSaveFile:data 4
gbValidSaveFile:
resb 4
global save_prev_tc:data 8
save_prev_tc:
resb 8
global plr_lframe_size:data 4
plr_lframe_size:
resb 4
global plr_wframe_size:data 4
plr_wframe_size:
resb 4
global plr_gfx_flag:data 4
plr_gfx_flag:
resb 4
global player_cpp_init_value:data 4
player_cpp_init_value:
resb 4
global plr_aframe_size:data 4
plr_aframe_size:
resb 4
global myplr:data 4
myplr:
resb 4
global plr:data 86880
plr:
resb 86880
global plr_fframe_size:data 4
plr_fframe_size:
resb 4
global plr_qframe_size:data 4
plr_qframe_size:
resb 4
global deathflag:data 4
deathflag:
resb 4
global plr_hframe_size:data 4
plr_hframe_size:
resb 4
global plr_bframe_size:data 4
plr_bframe_size:
resb 4
global plr_gfx_bflag:data 4
plr_gfx_bflag:
resb 4
global plr_sframe_size:data 4
plr_sframe_size:
resb 4
global deathdelay:data 4
deathdelay:
resb 4
global plr_dframe_size:data 8
plr_dframe_size:
resb 8
global plrmsg_ticks:data 4
plrmsg_ticks:
resb 4
global plr_msg_slot:data 4
plr_msg_slot:
resb 4
global plr_msgs:data 1216
plr_msgs:
resb 1216
global portal:data 96
portal:
resb 96
global portalindex:data 4
portalindex:
resb 4
global END_portalstruct:data 4
END_portalstruct:
resb 4
global qtopline:data 4
qtopline:
resb 4
global questlog:data 4
questlog:
resb 4
global pQLogCel:data 8
pQLogCel:
resb 8
global quests:data 384
quests:
resb 384
global qline:data 4
qline:
resb 4
global qlist:data 64
qlist:
resb 64
global numqlines:data 8
numqlines:
resb 8
global WaterDone:data 4
WaterDone:
resb 4
global ReturnLvlY:data 4
ReturnLvlY:
resb 4
global ReturnLvlX:data 4
ReturnLvlX:
resb 4
global ReturnLvlT:data 4
ReturnLvlT:
resb 4
global ALLQUESTS:data 4
ALLQUESTS:
resb 4
global ReturnLvl:data 8
ReturnLvl:
resb 8
global light_table_index:data 4
light_table_index:
resb 4
global screen_y_times_768:data 4096
screen_y_times_768:
resb 4096
global scrollrt_cpp_init_value:data 4
scrollrt_cpp_init_value:
resb 4
global sgdwCursWdtOld:data 4
sgdwCursWdtOld:
resb 4
global sgdwCursX:data 4
sgdwCursX:
resb 4
global sgdwCursY:data 4
sgdwCursY:
resb 4
global gpBufEnd:data 4
gpBufEnd:
resb 4
global sgdwCursHgt:data 4
sgdwCursHgt:
resb 4
global level_cel_block:data 4
level_cel_block:
resb 4
global sgdwCursXOld:data 4
sgdwCursXOld:
resb 4
global sgdwCursYOld:data 4
sgdwCursYOld:
resb 4
global arch_draw_type:data 8
arch_draw_type:
resb 8
global DDS_desc:data 108
DDS_desc:
resb 108
global cel_transparency_active:data 4
cel_transparency_active:
resb 4
global level_piece_id:data 4
level_piece_id:
resb 4
global sgdwCursWdt:data 4
sgdwCursWdt:
resb 4
global DrawPlrProc:data 4
DrawPlrProc:
resb 4
global sgSaveBack:data 8192
sgSaveBack:
resb 8192
global draw_monster_num:data 4
draw_monster_num:
resb 4
global sgdwCursHgtOld:data 8
sgdwCursHgtOld:
resb 8
global sgSHA1:data 276
sgSHA1:
resb 276
global sound_cpp_init_value:data 4
sound_cpp_init_value:
resb 4
global DSBs:data 32
DSBs:
resb 32
global sglpDS:data 4
sglpDS:
resb 4
global gbSndInited:data 4
gbSndInited:
resb 4
global sglMusicVolume:data 4
sglMusicVolume:
resb 4
global sglSoundVolume:data 4
sglSoundVolume:
resb 4
global hDsound_dll:data 4
hDsound_dll:
resb 4
global sgpMusicTrack:data 4
sgpMusicTrack:
resb 4
global sglpDSB:data 8
sglpDSB:
resb 8
global stextup:data 4
stextup:
resb 4
global storenumh:data 4
storenumh:
resb 4
global stextlhold:data 8
stextlhold:
resb 8
global boyitem:data 368
boyitem:
resb 368
global stextshold:data 8
stextshold:
resb 8
global premiumitem:data 2208
premiumitem:
resb 2208
global pSTextBoxCels:data 4
pSTextBoxCels:
resb 4
global premiumlevel:data 4
premiumlevel:
resb 4
global talker:data 8
talker:
resb 8
global stext:data 3744
stext:
resb 3744
global stextsize:data 4
stextsize:
resb 4
global stextsmax:data 4
stextsmax:
resb 4
global InStoreFlag:data 8
InStoreFlag:
resb 8
global storehold:data 17664
storehold:
resb 17664
global gossipstart:data 8
gossipstart:
resb 8
global witchitem:data 7360
witchitem:
resb 7360
global stextscrl:data 4
stextscrl:
resb 4
global numpremium:data 4
numpremium:
resb 4
global healitem:data 7360
healitem:
resb 7360
global golditem:data 368
golditem:
resb 368
global storehidx:data 48
storehidx:
resb 48
global pSTextSlidCels:data 4
pSTextSlidCels:
resb 4
global stextvhold:data 4
stextvhold:
resb 4
global stextsel:data 4
stextsel:
resb 4
global stextscrldbtn:data 4
stextscrldbtn:
resb 4
global gossipend:data 4
gossipend:
resb 4
global pCelBuff:data 4
pCelBuff:
resb 4
global stextsval:data 4
stextsval:
resb 4
global boylevel:data 4
boylevel:
resb 4
global smithitem:data 7360
smithitem:
resb 7360
global stextdown:data 4
stextdown:
resb 4
global stextscrlubtn:data 1
stextscrlubtn:
resb 1
global stextflag:data 3
stextflag:
resb 3
global sync_word_6AA708:data 400
sync_word_6AA708:
resb 400
global syncmonsters:data 4
syncmonsters:
resb 4
global sync_word_6AA89C:data 400
sync_word_6AA89C:
resb 400
global syncitems:data 8
syncitems:
resb 8
global sgnSyncPInv:data 4
sgnSyncPInv:
resb 4
global numthemes:data 4
numthemes:
resb 4
global armorFlag:data 4
armorFlag:
resb 4
global ThemeGoodIn:data 16
ThemeGoodIn:
resb 16
global weaponFlag:data 4
weaponFlag:
resb 4
global treasureFlag:data 4
treasureFlag:
resb 4
global mFountainFlag:data 4
mFountainFlag:
resb 4
global cauldronFlag:data 4
cauldronFlag:
resb 4
global tFountainFlag:data 4
tFountainFlag:
resb 4
global zharlib:data 4
zharlib:
resb 4
global themex:data 4
themex:
resb 4
global themey:data 4
themey:
resb 4
global themeVar1:data 8
themeVar1:
resb 8
global themes:data 400
themes:
resb 400
global pFountainFlag:data 4
pFountainFlag:
resb 4
global bFountainFlag:data 4
bFountainFlag:
resb 4
global bCrossFlag:data 4
bCrossFlag:
resb 4
global sgpTimedMsgHead:data 4
sgpTimedMsgHead:
resb 4
global storeflag:data 4
storeflag:
resb 4
global sgnCowMsg:data 4
sgnCowMsg:
resb 4
global numtowners:data 4
numtowners:
resb 4
global sgdwCowClicks:data 4
sgdwCowClicks:
resb 4
global bannerflag:data 4
bannerflag:
resb 4
global boyloadflag:data 4
boyloadflag:
resb 4
global pCowCels:data 8
pCowCels:
resb 8
global towner:data 3712
towner:
resb 3712
global sgbIsScrolling:data 4
sgbIsScrolling:
resb 4
global track_cpp_init_value:data 4
track_cpp_init_value:
resb 4
global sgdwLastWalk:data 4
sgdwLastWalk:
resb 4
global sgbIsWalking:data 4
sgbIsWalking:
resb 4
global trigflag:data 24
trigflag:
resb 24
global trigs:data 80
trigs:
resb 80
global TWarpFrom:data 4
TWarpFrom:
resb 4
global wave_cpp_init_value:data 5928
wave_cpp_init_value:
resb 5928

global __end_bss
__end_bss:

