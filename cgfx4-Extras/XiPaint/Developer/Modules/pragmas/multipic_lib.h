/* "multipic.library"*/
/**/
#pragma libcall MultiPicBase MP_Open 1E 0802
#pragma libcall MultiPicBase MP_Close 24 801
#pragma libcall MultiPicBase MP_Info 2A 801
#pragma libcall MultiPicBase MP_Read 30 0CBA9806
#pragma libcall MultiPicBase MP_ReadPalette 36 09803
#pragma libcall MultiPicBase MP_SetBufferAttrs 3C 9802
#pragma libcall MultiPicBase MP_IoErr 42 0
#pragma libcall MultiPicBase MP_Fault 48 18003
/**/
/* (v2)*/
/**/
#pragma libcall MultiPicBase MP_SetProp 4E 0802
#pragma libcall MultiPicBase MP_FindProp 54 0802
#pragma libcall MultiPicBase MP_SetImageAttrs 5A 9802
#pragma libcall MultiPicBase MP_Write 60 0CBA9806
#pragma libcall MultiPicBase MP_WritePalette 66 09803
#pragma libcall MultiPicBase MP_WriteProp 6C 2109805
#pragma libcall MultiPicBase MP_NextFormatInfo 72 10803
/**/
/* (v3)*/
/**/
/*pragma libcall MultiPicBase MP_SetIoErr 78 0802*/
/**/
#pragma libcall MultiPicBase MP_GetIoErr 7E 801
/**/
/*pragma libcall MultiPicBase MP_Reserved5 84 0*/
/*pragma libcall MultiPicBase MP_Reserved6 8A 0*/
