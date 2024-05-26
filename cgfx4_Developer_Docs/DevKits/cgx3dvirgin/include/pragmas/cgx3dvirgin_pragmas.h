/*
**	$VER: cgx3dvirgin_pragmas.h 3.0 (22.04.1998)
**
**	SAS/C pragmas file for cgx3dvirgin.library
**
**	Copyright © 1996-1998 by phase5 digital products
**      All Rights reserved.
**
*/

/*---------------------------------*/
/*--- 3dview handling functions ---*/
/*---------------------------------*/
#pragma libcall CGX3DVirginBase V3D_OpenViewTagList 1e 821004
#pragma libcall CGX3DVirginBase V3D_CloseView 24 801
#pragma libcall CGX3DVirginBase V3D_CreateEventPort 2a 0802
#pragma libcall CGX3DVirginBase V3D_DeleteEventPort 30 801
#pragma libcall CGX3DVirginBase V3D_ClearZBuffer 36 801
#pragma libcall CGX3DVirginBase V3D_FlipBuffer 3c 0802
#pragma libcall CGX3DVirginBase V3D_ClearBuffer 42 0802
#pragma libcall CGX3DVirginBase V3D_WaitTOV 48 801
#pragma libcall CGX3DVirginBase V3D_LockView 4e 801
#pragma libcall CGX3DVirginBase V3D_UnLockView 54 801
#pragma libcall CGX3DVirginBase V3D_GetViewAttr 5a 0802
#pragma libcall CGX3DVirginBase V3D_SetViewAttr 60 10803
/*----------------------------------*/
/*--- texture handling functions ---*/
/*----------------------------------*/
#pragma libcall CGX3DVirginBase V3D_CreateTexHandleTagList 66 9802
#pragma libcall CGX3DVirginBase V3D_DeleteTexHandle 6c 801
#pragma libcall CGX3DVirginBase V3D_LoadTextureTagList 72 9802
/*#pragma libcall CGX3DVirginBase V3D_Private4 78 0*/
#pragma libcall CGX3DVirginBase V3D_GetTexHandleAttr 7e 0802
#pragma libcall CGX3DVirginBase V3D_SetTexHandleAttr 84 10803
/*pragma libcall CGX3DVirginBase V3D_Private5 8a 0*/
/*pragma libcall CGX3DVirginBase V3D_Private6 90 0*/
/*pragma libcall CGX3DVirginBase V3D_Private7 96 0*/
/*-------------------------*/
/*--- drawing functions ---*/
/*-------------------------*/
#pragma libcall CGX3DVirginBase V3D_DrawTriangle3DSet 9c A2109806
#pragma libcall CGX3DVirginBase V3D_DrawTriangle3D a2 09803
#pragma libcall CGX3DVirginBase V3D_BlitV3DTriangle a8 a9803
#pragma libcall CGX3DVirginBase V3D_BlitV3DLine ae 9802
#pragma libcall CGX3DVirginBase V3D_DrawLine3D b4 09803
#pragma libcall CGX3DVirginBase V3D_FillRect ba 43210806
#pragma libcall CGX3DVirginBase V3D_MoveRect c0 543210807
#pragma libcall CGX3DVirginBase V3D_WriteRect c6 76543921080a
#pragma libcall CGX3DVirginBase V3D_ReadRect cc 76543921080a

#pragma tagcall CGX3DVirginBase V3D_OpenViewTags 1e 821004
#pragma tagcall CGX3DVirginBase V3D_CreateTexHandleTags 66 9802
#pragma tagcall CGX3DVirginBase V3D_LoadTextureTags 72 9802
#pragma tagcall CGX3DVirginBase V3D_LinkTexMapTags 78 A9803
