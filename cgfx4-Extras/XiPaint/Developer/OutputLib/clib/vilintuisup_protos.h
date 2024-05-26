
#define SPECIAL

SPECIAL struct Screen *OpenVillageScreen  (struct Dimensions *dm);
SPECIAL void           CloseVillageScreen (struct Screen *s);
SPECIAL void           UnLockVillageScreen(struct Screen *s);
SPECIAL APTR           LockVillageScreen  (struct Screen *s);
SPECIAL void           WaitVillageBlit(void);
SPECIAL LONG           VillageBlitCopy(struct Screen *s,struct VilCopyRecord *rec);
SPECIAL BOOL           IsVillageScreen(struct Screen *s);
SPECIAL ULONG          GetMemSize(struct Screen *s);
SPECIAL LONG           VillageRectFill(struct Screen *s,struct VilFillRecord *rec);
SPECIAL ULONG          VillageModeRequest(struct TagItem *ti);
SPECIAL struct List   *VillageModeList(struct Remember **key,struct TagItem *ti);
SPECIAL ULONG          VillageGetSegment(void);
SPECIAL void           VillageSetSegment(ULONG segnum);
SPECIAL ULONG          VillageBestModeID(struct TagItem *ti);
SPECIAL struct Screen *OpenVillageScreenTagList(struct TagItem *ti);
