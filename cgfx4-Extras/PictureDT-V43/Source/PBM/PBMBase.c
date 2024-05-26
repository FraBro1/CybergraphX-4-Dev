
#include "PBM.h"

struct ExecBase *SysBase;
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct DosLibrary *DOSBase;
struct Library *SuperClassBase,*DataTypesBase,*UtilityBase,*IFFParseBase;

LONG __stdargs ReleasePBMBase(struct PBMBase *PBMBase)

{
 if (!FreeClass(PBMBase->PB_Class))
  {
   AddClass (PBMBase->PB_Class);
   return FALSE;
  }

 CloseLibrary (SuperClassBase);
 CloseLibrary (IFFParseBase);
 CloseLibrary (UtilityBase);
 CloseLibrary (DataTypesBase);
 CloseLibrary (&DOSBase->dl_lib);
 CloseLibrary (&IntuitionBase->LibNode);
 CloseLibrary (&GfxBase->LibNode);

 return TRUE;
}

ULONG __asm __saveds DisPatch(REG(a0) Class *C,REG(a2) Object *O,REG(a1) Msg M)

{
 ULONG Result;

 switch (M->MethodID)
  {
   case OM_NEW:
    if (O!=(Object *)C)
     {
      (void)SetIoErr(ERROR_NOT_IMPLEMENTED);
      Result=0L;
      break;
     }

    if (Result=DoSuperMethodA(C,O,M))
     if (!ReadPBM(C,(Object *)Result,((struct opSet *)M)->ops_AttrList))
      {
       CoerceMethod (C,(Object *)Result,OM_DISPOSE);
       Result=0L;
      }
    break;
   case DTM_WRITE:
    if (((struct dtWrite *)M)->dtw_Mode==DTWM_RAW)
     {
      (void)SetIoErr(ERROR_NOT_IMPLEMENTED);
      Result=0L;
      break;
     }
   default:
    Result=DoSuperMethodA(C,O,M);
  }

 return Result;
}

Class __regargs *InitClass(struct PBMBase *PBMBase)

{
 Class *PBMClass;

 if (PBMClass=MakeClass(PBMDTCLASS,PICTUREDTCLASS,NULL,0L,0L))
  {
   PBMClass->cl_Dispatcher.h_Entry=(ULONG (*)())DisPatch;
   PBMClass->cl_UserData=(ULONG)PBMBase;
   AddClass (PBMClass);
  }
 return PBMClass;
}

struct PBMBase __stdargs *InitPBMBase(struct PBMBase *PBMBase)

{
 if (GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",39L))
  {
   if (IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",39L))
    {
     if (DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",39L))
      {
       if (DataTypesBase=OpenLibrary("datatypes.library",39L))
        {
         if (UtilityBase=OpenLibrary("utility.library",39L))
          {
           if (IFFParseBase=OpenLibrary("iffparse.library",39L))
            {
             if (SuperClassBase=OpenLibrary("datatypes/picture.datatype",43L))
              {
               if (PBMBase->PB_Class=InitClass(PBMBase)) return PBMBase;

               CloseLibrary (SuperClassBase);
              }
             CloseLibrary (IFFParseBase);
            }
           CloseLibrary (UtilityBase);
          }
         CloseLibrary (DataTypesBase);
        }
       CloseLibrary (&DOSBase->dl_lib);
      }
     CloseLibrary (&IntuitionBase->LibNode);
    }
   CloseLibrary (&GfxBase->LibNode);
  }

 return NULL;
}
