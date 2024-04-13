// swad_global.c: global variables

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include <locale.h>		// For setlocale
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For exit
#include <string.h>		// For string functions
#include <sys/time.h>		// For gettimeofday
#include <sys/types.h>		// For getpid
#include <unistd.h>		// For getpid

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_API.h"
#include "swad_calendar.h"
#include "swad_call_for_exam.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_degree_type.h"
#include "swad_department.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_icon.h"
#include "swad_link.h"
#include "swad_parameter.h"
#include "swad_plugin.h"
#include "swad_process.h"
#include "swad_program.h"
#include "swad_project.h"
#include "swad_role.h"
#include "swad_room.h"
#include "swad_session_database.h"
#include "swad_setting.h"
#include "swad_statistic.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public variables *****************************/
/*****************************************************************************/

struct Globals Gbl;	// All the global parameters and variables must be in this structure

/*****************************************************************************/
/************* Intialize globals variables when starting program *************/
/*****************************************************************************/

void Gbl_InitializeGlobals (void)
  {
   extern unsigned Txt_Current_CGI_SWAD_Language;
   Rol_Role_t Role;

   Gbl.Layout.WritingHTMLStart =
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = false;

   if (!setlocale (LC_ALL,"es_ES.utf8"))   // TODO: this should be internationalized!!!!!!!
      exit (1);

   Dat_SetStartExecutionTimeval ();
   Dat_SetStartExecutionTimeUTC ();
   Dat_GetAndConvertCurrentDateTime ();

   Prc_SetPID ();
   Par_SetIP ();

   Cry_CreateUniqueNameEncrypted (Cry_GetUniqueNameEncrypted ());

   srand ((unsigned int) Dat_GetStartExecutionTimeUTC ());	// Initialize seed for rand()

   Gbl.WebService.IsWebService = false;

   Fil_SetOutputFileToStdout ();

   Gbl.Prefs.Language       = Txt_Current_CGI_SWAD_Language;
   Gbl.Prefs.FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;	// Default first day of week
   Gbl.Prefs.DateFormat     = Dat_FORMAT_DEFAULT;		// Default date format
   Gbl.Prefs.Menu           = Mnu_MENU_DEFAULT;			// Default menu
   Gbl.Prefs.Theme          = The_THEME_DEFAULT;		// Default theme
   Gbl.Prefs.IconSet        = Ico_ICON_SET_DEFAULT;		// Default icon set

   Gbl.Session.NumSessions = 0;
   Gbl.Session.IsOpen = false;
   Gbl.Session.HasBeenDisconnected = false;
   Gbl.Session.Id[0] = '\0';
   Gbl.Session.ParsInsertedIntoDB = false;

   Gbl.Usrs.Me.UsrIdLogin[0] = '\0';
   Gbl.Usrs.Me.LoginPlainPassword[0] = '\0';
   Gbl.Usrs.Me.UsrDat.UsrCod = -1L;
   Gbl.Usrs.Me.UsrDat.UsrIDNickOrEmail[0] = '\0';
   Usr_UsrDataConstructor (&Gbl.Usrs.Me.UsrDat);
   Usr_ResetMyLastData ();
   Gbl.Usrs.Me.Logged = false;
   Gbl.Usrs.Me.Role.Available = 0;
   Gbl.Usrs.Me.Role.FromSession              =
   Gbl.Usrs.Me.Role.Logged                   =
   Gbl.Usrs.Me.Role.LoggedBeforeCloseSession =
   Gbl.Usrs.Me.Role.Max                      = Rol_UNK;
   Gbl.Usrs.Me.Role.HasChanged = false;
   Gbl.Usrs.Me.MyPhotoExists = false;
   Gbl.Usrs.Me.NumAccWithoutPhoto = 0;
   Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = LONG_MAX;	// Initialized to a big value, so by default files are not shown as recent or new
   Hie_ResetMyHierarchy ();
   Gbl.Usrs.Me.ConfirmEmailJustSent = false;	// An email to confirm my email address has not just been sent

   Gbl.Usrs.Other.UsrDat.UsrCod = -1L;
   Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail[0] = '\0';
   Usr_UsrDataConstructor (&Gbl.Usrs.Other.UsrDat);

   Gbl.Action.Act      = ActUnk;
   Gbl.Action.Original = ActUnk;	// Used in some actions to know what action gave rise to the current action
   Gbl.Action.UsesAJAX = false;
   Gbl.Action.IsAJAXAutoRefresh = false;
   Gbl.Action.Tab = TabUnk;

   Gbl.Usrs.Selected.Filled = false;	// Lists of encrypted codes of users selected from form are not filled
   Gbl.Usrs.Selected.ParSuffix = NULL;	// Don't add suffix to param names
   Gbl.Usrs.Selected.Option = Usr_OPTION_UNKNOWN;
   for (Role  = (Rol_Role_t) 0;
	Role <= (Rol_Role_t) (Rol_NUM_ROLES - 1);
	Role++)
     {
      Gbl.Usrs.LstUsrs[Role].Lst = NULL;
      Gbl.Usrs.LstUsrs[Role].NumUsrs = 0;
      Gbl.Usrs.Selected.List[Role] = NULL;
     }
   Gbl.Usrs.ListOtherRecipients = NULL;

   /***** Reset current hierarchy *****/
   Hie_ResetHierarchy ();

   Gbl.Crs.Info.ShowMsgMustBeRead = 0;

   Gbl.Crs.Notices.HighlightNotCod = -1L;	// No notice highlighted

   Gbl.Crs.Grps.NumGrps = 0;
   Gbl.Crs.Grps.WhichGrps = Grp_WHICH_GROUPS_DEFAULT;
   Gbl.Crs.Grps.GrpTypes.LstGrpTypes = NULL;
   Gbl.Crs.Grps.GrpTypes.NumGrpTypes = 0;
   Gbl.Crs.Grps.GrpTypes.NestedCalls = 0;
   Gbl.Crs.Grps.GrpTyp.GrpTypName[0] = '\0';
   Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment = true;
   Gbl.Crs.Grps.GrpTyp.MultipleEnrolment = false;
   Gbl.Crs.Grps.GrpTyp.MustBeOpened = false;
   Gbl.Crs.Grps.GrpTyp.OpenTimeUTC = (time_t) 0;
   Gbl.Crs.Grps.GrpCod = -1L; // -1L stands for the whole course
   Gbl.Crs.Grps.GrpName[0] = '\0';
   Gbl.Crs.Grps.RooCod = -1L; // -1L stands for no room assigned
   Gbl.Crs.Grps.MaxStudents = Grp_NUM_STUDENTS_NOT_LIMITED;
   Gbl.Crs.Grps.ClosedOrOpen = CloOpe_CLOSED;
   Gbl.Crs.Grps.LstGrpsSel.GrpCods  = NULL;
   Gbl.Crs.Grps.LstGrpsSel.NumGrps = 0;
   Gbl.Crs.Grps.LstGrpsSel.NestedCalls = 0;

   Gbl.Crs.Records.Field.Name[0] = '\0';
   Gbl.Crs.Records.Field.NumLines = Rec_MIN_LINES_IN_EDITION_FIELD;
   Gbl.Crs.Records.Field.Visibility = Rec_HIDDEN_FIELD;
   Gbl.Crs.Records.LstFields.Lst = NULL;
   Gbl.Crs.Records.LstFields.Num = 0;
   Gbl.Crs.Records.LstFields.NestedCalls = 0;

   Gbl.Usrs.Listing.RecsUsrs   = Rec_RECORD_USERS_UNKNOWN;
   Gbl.Usrs.Listing.RecsPerPag = Rec_DEF_RECORDS_PER_PAGE;
   Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;

   Gbl.Crs.Grps.AllGrps = true;
   Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;

   Gbl.Scope.Current = Hie_CRS;

   Gbl.Usrs.Connected.TimeToRefreshInMs = Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS;

   /* User nickname */
   Gbl.Usrs.Me.UsrDat.Nickname[0] = '\0';

   /* File browser */
   Gbl.FileBrowser.Id = 0;
   Gbl.FileBrowser.Type = Brw_UNKNOWN;
   Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_UNKNOWN;
   Gbl.FileBrowser.UploadingWithDropzone = false;

   Gbl.WebService.Function = API_unknown;

   /* Flush caches */
   Cty_FlushCacheCountryName ();
   Ins_FlushCacheFullNameAndCtyOfInstitution ();

   Hie_FlushCachedNumNodesInHieLvl (Hie_INS,Hie_CTY);	// Number of institutions in country
   Hie_FlushCachedNumNodesInHieLvl (Hie_CTR,Hie_CTY);	// Number of centers in country
   Hie_FlushCachedNumNodesInHieLvl (Hie_DEG,Hie_CTY);	// Number of degrees in country
   Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_CTY);	// Number of courses in country

   Dpt_FlushCacheNumDptsInIns ();				// Number of departments in institution
   Hie_FlushCachedNumNodesInHieLvl (Hie_CTR,Hie_INS);	// Number of centers in institution
   Hie_FlushCachedNumNodesInHieLvl (Hie_DEG,Hie_INS);	// Number of degrees in institution
   Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_INS);	// Number of courses in institution

   Hie_FlushCachedNumNodesInHieLvl (Hie_DEG,Hie_CTR);	// Number of degrees in center
   Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_CTR);	// Number of courses in center

   Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_DEG);	// Number of courses in degree

   Cty_FlushCacheNumUsrsWhoDontClaimToBelongToAnyCty ();
   Cty_FlushCacheNumUsrsWhoClaimToBelongToAnotherCty ();
   Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_CTY);
   Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_INS);
   Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_CTR);
   Usr_FlushCacheUsrIsSuperuser ();
   Hie_FlushCacheUsrBelongsTo (Hie_INS);
   Hie_FlushCacheUsrBelongsTo (Hie_CTR);
   Hie_FlushCacheUsrBelongsTo (Hie_DEG);
   Hie_FlushCacheUsrBelongsTo (Hie_CRS);
   Enr_FlushCacheUsrBelongsToCurrentCrs ();
   Enr_FlushCacheUsrHasAcceptedInCurrentCrs ();
   Enr_FlushCacheUsrSharesAnyOfMyCrs ();
   Rol_FlushCacheMyRoleInCurrentCrs ();
   Rol_FlushCacheRoleUsrInCrs ();
   Prj_FlushCacheMyRolesInProject ();
   Grp_FlushCacheIBelongToGrp ();
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   Fol_FlushCacheFollow ();
  }

/*****************************************************************************/
/************* Free memory, close files, remove lock file, etc. **************/
/*****************************************************************************/

void Gbl_Cleanup (void)
  {
   Rol_Role_t Role;

   if (!Gbl.Action.UsesAJAX &&
       !Gbl.WebService.IsWebService &&
       Act_GetBrowserTab (Gbl.Action.Act) == Act_1ST_TAB)
      Ses_DB_RemovePar ();

   Hie_FreeMyHierarchy ();

   Usr_UsrDataDestructor (&Gbl.Usrs.Me.UsrDat);
   Usr_UsrDataDestructor (&Gbl.Usrs.Other.UsrDat);
   Rec_FreeListFields ();
   Grp_FreeListGrpTypesAndGrps ();
   Grp_FreeListCodSelectedGrps ();
   Hie_FreeList (Hie_DEG);
   Hie_FreeList (Hie_CTR);
   Hie_FreeList (Hie_INS);
   Hie_FreeList (Hie_CTY);
   Hie_FreeList (Hie_SYS);

   for (Role  = (Rol_Role_t) 0;
	Role <= (Rol_Role_t) (Rol_NUM_ROLES - 1);
	Role++)
      Usr_FreeUsrsList (Role);

   Usr_FreeListOtherRecipients ();
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
   Syl_FreeListItemsSyllabus ();
   Par_FreePars ();
   Ale_ResetAllAlerts ();
  }
