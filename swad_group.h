// swad_group.h: types of groups and groups

#ifndef _SWAD_GRP
#define _SWAD_GRP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_info.h"
#include "swad_room.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Grp_MAX_CHARS_GROUP_TYPE_NAME	(128 - 1)	// 127
#define Grp_MAX_BYTES_GROUP_TYPE_NAME	((Grp_MAX_CHARS_GROUP_TYPE_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Grp_MAX_CHARS_GROUP_NAME	(128 - 1)	// 127
#define Grp_MAX_BYTES_GROUP_NAME	((Grp_MAX_CHARS_GROUP_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Grp_MAX_STUDENTS_IN_A_GROUP	10000	// If max of students in a group is greater than this, it is considered infinite
#define Grp_NUM_STUDENTS_NOT_LIMITED  INT_MAX	// This number can be stored in database as an integer...
						// ...and means that a group has no limit of students

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Grp_NUM_WHICH_GROUP_TYPES 2
typedef enum
  {
   Grp_ONLY_GROUP_TYPES_WITH_GROUPS,
   Grp_ALL_GROUP_TYPES,
  } Grp_WhichGrpTypes_t;

#define Grp_NUM_MY_ALL_GROUPS 2
typedef enum
  {
   Grp_MY_GROUPS,
   Grp_ALL_GROUPS,
  } Grp_MyAllGrps_t;
#define Grp_MY_ALL_GROUPS_DEFAULT Grp_ALL_GROUPS

#define Grp_NUM_CLOSED_OPEN_GROUPS 2
typedef enum
  {
   Grp_ONLY_CLOSED_GROUPS,
   Grp_CLOSED_AND_OPEN_GROUPS,
  } Grp_ClosedOpenGrps_t;

#define Grp_NUM_MUST_BE_OPENED 2
typedef enum
  {
   Grp_MUST_NOT_BE_OPENED,
   Grp_MUST_BE_OPENED,
  } Grp_MustBeOpened_t;

#define Grp_NUM_FILEZONES 2
typedef enum
  {
   Grp_HAS_NOT_FILEZONES,
   Grp_HAS_FILEZONES,
  } Grp_FileZones_t;

#define Grp_NUM_OPTIONAL_MANDATORY 2
typedef enum
  {
   Grp_OPTIONAL,
   Grp_MANDATORY,
  } Grp_OptionalMandatory_t;

#define Grp_NUM_SINGLE_MULTIPLE 2
typedef enum
  {
   Grp_SINGLE,
   Grp_MULTIPLE,
  } Grp_SingleMultiple_t;

struct Group
  {
   long GrpCod;					// Code of group
   char Name[Grp_MAX_BYTES_GROUP_NAME + 1];	// Name of group
   struct
     {
      long RooCod;					// Room code
      char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];	// Room short name
     } Room;
   unsigned NumUsrs[Rol_NUM_ROLES];		// Number of users in the group
   unsigned MaxStds;				// Maximum number of students in the group
   CloOpe_ClosedOrOpen_t ClosedOrOpen;		// Group is closed or open?
   Grp_FileZones_t FileZones;			// Group has file zones?
  };

struct GroupType
  {
   long GrpTypCod;					// Code of type of group
   char Name[Grp_MAX_BYTES_GROUP_TYPE_NAME + 1];	// Name of type of group
   struct
     {
      Grp_OptionalMandatory_t OptionalMandatory;	// Enrolment is optional or mandatory?
      Grp_SingleMultiple_t SingleMultiple;		// Enrolment is single or multiple?
     } Enrolment;
   Grp_MustBeOpened_t MustBeOpened;			// Groups must be opened?
   time_t OpenTimeUTC;					// Open groups automatically in this date-time. If == 0, don't open.
   unsigned NumGrps;					// Number of groups of this type
   struct Group *LstGrps;				// List of groups of this type
  };

// Related with groups
struct GroupData
  {
   long CrsCod;			// Course code
   struct GroupType GrpTyp;	// Group type
   struct Group Grp;		// Group
  };

struct GroupTypes
  {
   struct GroupType *LstGrpTypes;		// List of types of group
   unsigned NumGrpTypes;			// Number of types of group
   unsigned NumGrpsTotal;			// Number of groups of any type
   int NestedCalls;				// Number of nested calls to the function that allocates memory for this list
  };

struct ListCodGrps
  {
   long *GrpCods;
   unsigned NumGrps;
   int NestedCalls;				// Number of nested calls to the function that allocates memory for this list
  };

struct ListGrpsAlreadySelec
  {
   long GrpTypCod;
   bool AlreadySelected;
  };

#define Grp_NUM_ASSOCIATIONS_TO_GROUPS 5
typedef enum
  {
   Grp_ASSIGNMENT,
   Grp_ATT_EVENT,
   Grp_SURVEY,
   Grp_EXA_EVENT,
   Grp_MATCH,
  } Grp_WhichIsAssociatedToGrp_t;

struct Grp_Groups
  {
   unsigned NumGrps;
   struct GroupTypes GrpTypes;
   struct GroupData GrpDat;
   bool AllGrps;		// All groups selected?
   struct ListCodGrps LstGrpsSel;
   Grp_MyAllGrps_t MyAllGrps;	// Show my groups or all groups
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Grp_WriteNamesOfSelectedGrps (void);
void Grp_ReqEditGroups (void);

void Grp_ShowFormToSelectSeveralGroups (Act_Action_t NextAction,
				        void (*FuncPars) (void *Args),void *Args,
				        const char *OnSubmit);
void Grp_PutParsCodGrps (void);
void Grp_GetParCodsSeveralGrpsToShowUsrs (void);
void Grp_GetParCodsSeveralGrps (void);
void Grp_FreeListCodSelectedGrps (void);
void Grp_ChangeMyGrpsAndShowChanges (void);
void Grp_ChangeUsrGrps (Usr_MeOrOther_t MeOrOther,Cns_Verbose_t Verbose);
bool Grp_ChangeGrpsAtomically (Usr_MeOrOther_t MeOrOther,
			       struct ListCodGrps *LstGrpsUsrWants);
bool Grp_CheckIfAtMostOneSingleEnrolmentGrpIsSelected (struct ListCodGrps *LstGrps,
						       bool CheckClosedGroupsIBelong);
void Grp_EnrolUsrIntoGroups (struct Usr_Data *UsrDat,Rol_Role_t Role,
			     struct ListCodGrps *LstGrps);
unsigned Grp_RemoveUsrFromGroups (struct Usr_Data *UsrDat,struct ListCodGrps *LstGrps);
void Grp_RemUsrFromAllGrpsInCrs (long UsrCod,long CrsCod);
void Grp_RemUsrFromAllGrps (long UsrCod);

void Grp_WriteTheWholeCourse (void);
void Grp_ListGrpsToEditAsgAttSvyEvtMch (Grp_WhichIsAssociatedToGrp_t WhichIsAssociatedToGrp,
                                        long Cod);

void Grp_ShowLstGrpsToChgMyGrps (void);
void Grp_ShowLstGrpsToChgOtherUsrsGrps (long UsrCod);

void Grp_GetListGrpTypesInCurrentCrs (Grp_WhichGrpTypes_t WhichGrpTypes);
void Grp_FreeListGrpTypesAndGrps (void);
void Grp_OpenGroupsAutomatically (void);
void Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_WhichGrpTypes_t WhichGrpTypes);
void Grp_GetGroupDataByCod (struct GroupData *GrpDat);

void Grp_FlushCacheIBelongToGrp (void);
Usr_Belong_t Grp_GetIfIBelongToGrp (long GrpCod);
void Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs (void);
bool Grp_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (const struct Usr_Data *UsrDat);

void Grp_GetLstCodGrpsWithFileZonesIBelong (struct ListCodGrps *LstGrps);
void Grp_GetNamesGrpsUsrBelongsTo (long UsrCod,long GrpTypCod,char *GroupNames);
void Grp_ReceiveNewGrpTyp (void);
void Grp_ReceiveNewGrp (void);
void Grp_ReqRemGroupType (void);
void Grp_ReqRemGroup (void);
void Grp_RemoveGroupType (void);
void Grp_RemoveGroup (void);
void Grp_OpenGroup (void);
void Grp_CloseGroup (void);
void Grp_EnableFileZonesGrp (void);
void Grp_DisableFileZonesGrp (void);

void Grp_ChangeGroupType (void);
void Grp_ChangeGroupRoom (void);

void Grp_ChangeMandatGrpTyp (void);
void Grp_ChangeMultiGrpTyp (void);
void Grp_ChangeOpenTimeGrpTyp (void);
void Grp_ChangeMaxStdsGrp (void);
unsigned Grp_ConvertToNumMaxStdsGrp (const char *StrMaxStudents);
void Grp_RenameGroupType (void);
void Grp_RenameGroup (void);
void Grp_GetLstCodsGrpWanted (struct ListCodGrps *LstGrpsWanted);
void Grp_FreeListCodGrp (struct ListCodGrps *LstGrps);
void Grp_PutParAllGroups (void);

void Grp_PutParMyAllGrps (void *WhichGrps);
void Grp_PutParWhichGrpsOnlyMyGrps (void);
void Grp_PutParWhichGrpsAllGrps (void);
void Grp_ShowFormToSelMyAllGrps (Act_Action_t Action,
                                 void (*FuncPars) (void *Args),void *Args);
Grp_MyAllGrps_t Grp_GetParMyAllGrps (void);

void Grp_DB_RemoveCrsGrps (long HieCod);

#endif
