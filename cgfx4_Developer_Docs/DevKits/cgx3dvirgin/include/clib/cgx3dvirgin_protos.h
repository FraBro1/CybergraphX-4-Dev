/*
**	$VER: cgx3dvirgin_protos.h 3.0 (22.04.1998)
**
**	SAS/C protos file for cgx3dvirgin.library
**
**	Copyright © 1996-1998 by phase5 digital products
**      All Rights reserved.
**
*/

#ifndef CLIB_CGX3DVIRGIN_H
#define CLIB_CGX3DVIRGIN_H

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

View3D V3D_OpenViewTagList(ULONG,
                           ULONG,
                           ULONG,
                           struct TagList *);

View3D V3D_OpenViewTags(ULONG,
                        ULONG,
                        ULONG,
                        Tag, ...);

ULONG V3D_CloseView(View3D);

struct MsgPort *V3D_CreateEventPort(View3D,
                                    ULONG);

ULONG V3D_DeleteEventPort(struct MsgPort *);

ULONG V3D_ClearZBuffer(View3D);

ULONG V3D_FlipBuffer(View3D,
                     ULONG);

ULONG V3D_ClearBuffer(View3D,
                      ULONG);

void V3D_WaitTOV(View3D);

ULONG V3D_LockView(View3D);

void V3D_UnLockView(View3D);

ULONG V3D_GetViewAttr(View3D,
                      ULONG);

ULONG V3D_SetViewAttr(View3D,
                      ULONG,
                      ULONG);

TexHandle V3D_CreateTexHandleTagList(View3D,
                                     struct TagList *);

TexHandle V3D_CreateTexHandleTags(View3D,
                                  Tag, ...);

void V3D_DeleteTexHandle(TexHandle);

APTR V3D_LoadTextureTagList(UBYTE *,
                            struct TagList *);

APTR V3D_LoadTextureTags(UBYTE *,
                         Tag, ...);

ULONG V3D_GetTexHandleAttr(TexHandle,
                           ULONG);

ULONG V3D_SetTexHandleAttr(TexHandle,
                           ULONG,
                           ULONG);

ULONG V3D_DrawTriangle3D(View3D,
                         Triangle3D *,
                         ULONG);

ULONG V3D_DrawTriangle3DSet(View3D,
                            VertexV3Dtex **,
			    ULONG,
			    ULONG,
                            ULONG,
			    APTR);

ULONG V3D_DrawLine3D(View3D,
                     Line3D *,
                     ULONG);

ULONG V3D_BlitV3DTriangle(View3D,
			  V3DTriangle *,
			  TexHandle);

ULONG V3D_BlitV3DLine(View3D,
	 	      V3DLine *);

void V3D_FillRect(View3D,ULONG,ULONG,ULONG,ULONG,ULONG);

void V3D_MoveRect(View3D,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG);

void V3D_WriteRect(APTR,ULONG,ULONG,ULONG,View3D,ULONG,ULONG,ULONG,ULONG,ULONG);

void V3D_ReadRect(APTR,ULONG,ULONG,ULONG,View3D,ULONG,ULONG,ULONG,ULONG,ULONG);

#endif