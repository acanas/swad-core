// swad_global.c: global variables

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <locale.h>		// For setlocale
#include <stdlib.h>		// For exit
#include <string.h>		// For string functions
#include <sys/time.h>		// For gettimeofday
#include <sys/types.h>		// For getpid
#include <unistd.h>		// For getpid

#include "swad_action.h"
#include "swad_API.h"
#include "swad_calendar.h"
#include "swad_classroom.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_exam.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_icon.h"
#include "swad_parameter.h"
#include "swad_project.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public variables *****************************/
/*****************************************************************************/

struct Globals Gbl;	// All the global parameters and variables must be in this structure

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************* Intialize globals variables when starting program *************/
/*****************************************************************************/

void Gbl_InitializeGlobals (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   Rol_Role_t Role;

   Gbl.Layout.WritingHTMLStart =
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = false;

   if (!setlocale (LC_ALL,"es_ES.utf8"))   // TODO: this should be internationalized!!!!!!!
      exit (1);

   gettimeofday (&Gbl.tvStart, &Gbl.tz);
   Dat_GetStartExecutionTimeUTC ();
   Dat_GetAndConvertCurrentDateTime ();

   Gbl.Config.DatabasePassword[0] = '\0';
   Gbl.Config.SMTPPassword[0] = '\0';

   Gbl.TimeGenerationInMicroseconds = Gbl.TimeSendInMicroseconds = 0L;
   Gbl.PID = getpid ();
   Sta_GetRemoteAddr ();

   Cry_CreateUniqueNameEncrypted (Gbl.UniqueNameEncrypted);

   srand ((unsigned int) Gbl.StartExecutionTimeUTC);	// Initialize seed for rand()

   Gbl.WebService.IsWebService = false;

   Gbl.Params.ContentLength = 0;
   Gbl.Params.QueryString = NULL;
   Gbl.Params.List = NULL;
   Gbl.Params.GetMethod = false;

   Gbl.F.Out = stdout;
   Gbl.F.Tmp = NULL;
   Gbl.F.XML = NULL;
   Gbl.F.Rep = NULL;	// Report

   Gbl.Form.Num = -1;		// Number of form. It's increased by 1 at the begin of each form
   Gbl.Form.Inside = false;	// Set to true inside a form to avoid nested forms

   Gbl.Box.Nested = -1;	// -1 means no box open

   Gbl.Alerts.Num = 0;	// No pending alerts to be shown

   Gbl.DB.DatabaseIsOpen = false;
   Gbl.DB.LockedTables = false;

   Gbl.HiddenParamsInsertedIntoDB = false;

   Gbl.Prefs.Language       = Txt_Current_CGI_SWAD_Language;
   Gbl.Prefs.FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;	// Default first day of week
   Gbl.Prefs.DateFormat     = Dat_FORMAT_DEFAULT;		// Default date format
   Gbl.Prefs.Menu           = Mnu_MENU_DEFAULT;			// Default menu
   Gbl.Prefs.Theme          = The_THEME_DEFAULT;		// Default theme
   Gbl.Prefs.IconSet        = Ico_ICON_SET_DEFAULT;		// Default icon set
   snprintf (Gbl.Prefs.URLTheme,sizeof (Gbl.Prefs.URLTheme),
	     "%s/%s",
             Cfg_URL_ICON_THEMES_PUBLIC,
	     The_ThemeId[Gbl.Prefs.Theme]);
   snprintf (Gbl.Prefs.URLIconSet,sizeof (Gbl.Prefs.URLIconSet),
	     "%s/%s",
	     Cfg_URL_ICON_SETS_PUBLIC,
	     Ico_IconSetId[Gbl.Prefs.IconSet]);

   Gbl.Session.NumSessions = 0;
   Gbl.Session.IsOpen = false;
   Gbl.Session.HasBeenDisconnected = false;
   Gbl.Session.Id[0] = '\0';

   Gbl.Usrs.Me.UsrIdLogin[0] = '\0';
   Gbl.Usrs.Me.LoginPlainPassword[0] = '\0';
   Gbl.Usrs.Me.UsrDat.UsrCod = -1L;
   Gbl.Usrs.Me.UsrDat.UsrIDNickOrEmail[0] = '\0';
   Usr_UsrDataConstructor (&Gbl.Usrs.Me.UsrDat);
   Usr_ResetMyLastData ();
   Gbl.Session.Id[0] = '\0';
   Gbl.Usrs.Me.Logged = false;
   Gbl.Usrs.Me.Role.Available = 0;
   Gbl.Usrs.Me.Role.FromSession              =
   Gbl.Usrs.Me.Role.Logged                   =
   Gbl.Usrs.Me.Role.LoggedBeforeCloseSession =
   Gbl.Usrs.Me.Role.Max                          = Rol_UNK;
   Gbl.Usrs.Me.Role.HasChanged = false;
   Gbl.Usrs.Me.IBelongToCurrentIns = false;
   Gbl.Usrs.Me.IBelongToCurrentCtr = false;
   Gbl.Usrs.Me.IBelongToCurrentDeg = false;
   Gbl.Usrs.Me.IBelongToCurrentCrs = false;
   Gbl.Usrs.Me.MyPhotoExists = false;
   Gbl.Usrs.Me.NumAccWithoutPhoto = 0;
   Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = LONG_MAX;	// Initialized to a big value, so by default files are not shown as recent or new
   Gbl.Usrs.Me.MyInss.Filled = false;
   Gbl.Usrs.Me.MyCtrs.Filled = false;
   Gbl.Usrs.Me.MyDegs.Filled = false;
   Gbl.Usrs.Me.MyCrss.Filled = false;
   Gbl.Usrs.Me.MyCrss.Num = 0;
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
   Gbl.Usrs.Selected.Option = Usr_OPTION_UNKNOWN;
   for (Role = (Rol_Role_t) 0;
	Role < Rol_NUM_ROLES;
	Role++)
     {
      Gbl.Usrs.LstUsrs[Role].Lst = NULL;
      Gbl.Usrs.LstUsrs[Role].NumUsrs = 0;
      Gbl.Usrs.Selected.List[Role] = NULL;
     }
   Gbl.Usrs.ListOtherRecipients = NULL;

   Gbl.ExamAnns.NumExaAnns = 0;
   Gbl.ExamAnns.Lst = NULL;
   Gbl.ExamAnns.NewExaCod = -1L;
   Gbl.ExamAnns.HighlightExaCod = -1L;
   Gbl.ExamAnns.HighlightDate[0] = '\0';	// No exam announcements highlighted

   Gbl.Msg.Subject[0] = '\0';
   Gbl.Msg.ShowOnlyUnreadMsgs = false;
   Gbl.Msg.ExpandedMsgCod = -1L;

   /***** Reset current hierarchy *****/
   Hie_ResetHierarchy ();

   Gbl.Hierarchy.Ins.ShrtName[0] = '\0';
   Gbl.Hierarchy.Ins.FullName[0] = '\0';
   Gbl.Hierarchy.Ins.WWW[0] = '\0';
   Gbl.Hierarchy.Ins.Ctrs.Num = Gbl.Hierarchy.Ins.NumDpts = Gbl.Hierarchy.Ins.NumDegs = 0;
   Gbl.Hierarchy.Ins.NumUsrs = 0;

   Gbl.Hierarchy.Ctr.ShrtName[0] = '\0';
   Gbl.Hierarchy.Ctr.FullName[0] = '\0';
   Gbl.Hierarchy.Ctr.Degs.Num = 0;
   Gbl.Hierarchy.Ctr.Degs.Lst = NULL;

   Gbl.Hierarchy.Deg.ShrtName[0] = Gbl.Hierarchy.Deg.FullName[0] = '\0';

   Gbl.Hierarchy.Crs.ShrtName[0] = Gbl.Hierarchy.Crs.FullName[0] = '\0';
   Gbl.Crs.Info.ShowMsgMustBeRead = 0;
   Gbl.Crs.Notices.HighlightNotCod = -1L;	// No notice highlighted

   Gbl.Hierarchy.Cty.Inss.Num = 0;
   Gbl.Hierarchy.Cty.Inss.Lst = NULL;
   Gbl.Hierarchy.Cty.Inss.SelectedOrder = Ins_ORDER_DEFAULT;

   Gbl.Hierarchy.Sys.Ctys.Num = 0;
   Gbl.Hierarchy.Sys.Ctys.Lst = NULL;
   Gbl.Hierarchy.Sys.Ctys.SelectedOrder = Cty_ORDER_DEFAULT;

   Gbl.Hierarchy.Ins.Ctrs.Num = 0;
   Gbl.Hierarchy.Ins.Ctrs.Lst = NULL;
   Gbl.Hierarchy.Ins.Ctrs.SelectedOrder = Ctr_ORDER_DEFAULT;

   Gbl.Dpts.Num = 0;
   Gbl.Dpts.Lst = NULL;
   Gbl.Dpts.SelectedOrder = Dpt_ORDER_DEFAULT;

   Gbl.Plcs.Num = 0;
   Gbl.Plcs.Lst = NULL;
   Gbl.Plcs.SelectedOrder = Plc_ORDER_DEFAULT;

   Gbl.Hlds.LstIsRead = false;	// List is not read
   Gbl.Hlds.Num = 0;
   Gbl.Hlds.Lst = NULL;
   Gbl.Hlds.SelectedOrder = Hld_DEFAULT_ORDER_TYPE;

   Gbl.DegTypes.Num = 0;
   Gbl.DegTypes.Lst = NULL;

   Gbl.Crs.Grps.NumGrps = 0;
   Gbl.Crs.Grps.WhichGrps = Grp_WHICH_GROUPS_DEFAULT;
   Gbl.Crs.Grps.GrpTypes.LstGrpTypes = NULL;
   Gbl.Crs.Grps.GrpTypes.Num = 0;
   Gbl.Crs.Grps.GrpTypes.NestedCalls = 0;
   Gbl.Crs.Grps.GrpTyp.GrpTypName[0] = '\0';
   Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment = true;
   Gbl.Crs.Grps.GrpTyp.MultipleEnrolment = false;
   Gbl.Crs.Grps.GrpTyp.MustBeOpened = false;
   Gbl.Crs.Grps.GrpTyp.OpenTimeUTC = (time_t) 0;
   Gbl.Crs.Grps.GrpCod = -1L; // -1L stands for the whole course
   Gbl.Crs.Grps.GrpName[0] = '\0';
   Gbl.Crs.Grps.ClaCod = -1L; // -1L stands for no classroom assigned
   Gbl.Crs.Grps.MaxStudents = Grp_NUM_STUDENTS_NOT_LIMITED;
   Gbl.Crs.Grps.Open = false;
   Gbl.Crs.Grps.LstGrpsSel.GrpCods  = NULL;
   Gbl.Crs.Grps.LstGrpsSel.NumGrps = 0;
   Gbl.Crs.Grps.LstGrpsSel.NestedCalls = 0;

   Gbl.Usrs.ClassPhoto.AllGroups = true;

   Gbl.Crs.Records.Field.Name[0] = '\0';
   Gbl.Crs.Records.Field.NumLines = Rec_MIN_LINES_IN_EDITION_FIELD;
   Gbl.Crs.Records.Field.Visibility = Rec_HIDDEN_FIELD;

   Gbl.Crs.Records.LstFields.Lst = NULL;
   Gbl.Crs.Records.LstFields.Num = 0;
   Gbl.Crs.Records.LstFields.NestedCalls = 0;

   Gbl.Syllabus.EditionIsActive = false;
   Gbl.Syllabus.WhichSyllabus = Syl_DEFAULT_WHICH_SYLLABUS;

   Gbl.Search.WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;
   Gbl.Search.Str[0] = '\0';
   Gbl.Search.LogSearch = false;

   Gbl.Asgs.LstIsRead = false;	// List is not read
   Gbl.Asgs.Num = 0;
   Gbl.Asgs.LstAsgCods = NULL;
   Gbl.Asgs.SelectedOrder = Asg_ORDER_DEFAULT;

   /* Projects */
   Gbl.Prjs.Config.Editable = Prj_EDITABLE_DEFAULT;
   Gbl.Prjs.Filter.Who    = Prj_FILTER_WHO_DEFAULT;
   Gbl.Prjs.Filter.Assign = Prj_FILTER_ASSIGNED_DEFAULT |
	                    Prj_FILTER_NONASSIG_DEFAULT;
   Gbl.Prjs.Filter.Hidden = Prj_FILTER_HIDDEN_DEFAULT |
	                    Prj_FILTER_VISIBL_DEFAULT;
   Gbl.Prjs.Filter.Faulti = Prj_FILTER_FAULTY_DEFAULT |
	                    Prj_FILTER_FAULTLESS_DEFAULT;
   Gbl.Prjs.Filter.DptCod = Prj_FILTER_DPT_DEFAULT;
   Gbl.Prjs.LstIsRead = false;	// List is not read
   Gbl.Prjs.Num = 0;
   Gbl.Prjs.LstPrjCods = NULL;
   Gbl.Prjs.SelectedOrder = Prj_ORDER_DEFAULT;
   Gbl.Prjs.PrjCod = -1L;

   Gbl.AttEvents.LstIsRead = false;	// List is not read
   Gbl.AttEvents.Num = 0;
   Gbl.AttEvents.Lst = NULL;
   Gbl.AttEvents.SelectedOrder = Att_ORDER_DEFAULT;
   Gbl.AttEvents.AttCod = -1L;
   Gbl.AttEvents.StrAttCodsSelected = NULL;

   Gbl.Mails.Num = 0;
   Gbl.Mails.Lst = NULL;
   Gbl.Mails.SelectedOrder = Mai_ORDER_DEFAULT;

   Gbl.Banners.Num = 0;
   Gbl.Banners.Lst = NULL;
   Gbl.Banners.BanCodToEdit = -1L;
   Gbl.Banners.BanCodClicked = 0L;

   Gbl.Links.Num = 0;
   Gbl.Links.Lst = NULL;

   Gbl.Usrs.Listing.RecsUsrs   = Rec_RECORD_USERS_UNKNOWN;
   Gbl.Usrs.Listing.RecsPerPag = Rec_DEF_RECORDS_PER_PAGE;
   Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;

   Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;

   /* Statistics */
   Gbl.Stat.ClicksGroupedBy = Sta_CLICKS_GROUPED_BY_DEFAULT;
   Gbl.Stat.CountType       = Sta_COUNT_TYPE_DEFAULT;
   Gbl.Stat.Role            = Sta_ROLE_DEFAULT;
   Gbl.Stat.NumAction       = Sta_NUM_ACTION_DEFAULT;
   Gbl.Stat.RowsPerPage     = Sta_DEF_ROWS_PER_PAGE;
   Gbl.Figures.FigureType   = Fig_FIGURE_TYPE_DEF;

   Gbl.Scope.Current = Hie_CRS;

   Gbl.Usrs.Connected.TimeToRefreshInMs = Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS;

   /* Tests */
   Gbl.Test.Config.Pluggable = Tst_PLUGGABLE_UNKNOWN;
   Gbl.Test.NumQsts = Tst_CONFIG_DEFAULT_DEF_QUESTIONS;
   Gbl.Test.AllowTeachers = false;	// Must the test result be saved?
   Gbl.Test.AllAnsTypes = false;
   Gbl.Test.ListAnsTypes[0] = '\0';

   Gbl.Test.Tags.Num  = 0;
   Gbl.Test.Tags.All  = false;
   Gbl.Test.Tags.List = NULL;

   /* Games for remote control */
   Gbl.Games.ListQuestions = NULL;

   /* Public activity */
   Gbl.Timeline.Who = TL_DEFAULT_WHO;

   /* Forums */
   Gbl.Forum.ForumSet = For_DEFAULT_FORUM_SET;
   Gbl.Forum.ForumSelected.Type = For_FORUM_UNKNOWN;
   Gbl.Forum.ForumSelected.Location = -1L;
   Gbl.Forum.ForumSelected.ThrCod = -1L;
   Gbl.Forum.ForumSelected.PstCod = -1L;
   Gbl.Forum.ThreadsOrder = For_DEFAULT_ORDER;
   Gbl.Forum.ThreadToMove = -1L;

   /* User nickname */
   Gbl.Usrs.Me.UsrDat.Nickname[0] = '\0';

   /* File browser */
   Gbl.FileBrowser.Id = 0;
   Gbl.FileBrowser.Type = Brw_UNKNOWN;
   Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_UNKNOWN;
   Gbl.FileBrowser.UploadingWithDropzone = false;

   /* Agenda */
   Gbl.Agenda.Past__FutureEvents = Agd_DEFAULT_PAST___EVENTS |
	                           Agd_DEFAULT_FUTURE_EVENTS;
   Gbl.Agenda.PrivatPublicEvents = Agd_DEFAULT_PRIVAT_EVENTS |
	                           Agd_DEFAULT_PUBLIC_EVENTS;
   Gbl.Agenda.HiddenVisiblEvents = Agd_DEFAULT_HIDDEN_EVENTS |
	                           Agd_DEFAULT_VISIBL_EVENTS;
   Gbl.Agenda.SelectedOrder = Agd_ORDER_DEFAULT;
   Gbl.Agenda.AgdCodToEdit = -1L;

   /* To alternate colors where listing rows */
   Gbl.RowEvenOdd = 0;
   Gbl.ColorRows[0] = "COLOR0";	// Darker
   Gbl.ColorRows[1] = "COLOR1";	// Lighter

   Gbl.WebService.Function = API_unknown;

   /* Flush caches */
   Cty_FlushCacheCountryName ();
   Ins_FlushCacheShortNameOfInstitution ();
   Ins_FlushCacheFullNameAndCtyOfInstitution ();
   Usr_FlushCacheUsrIsSuperuser ();
   Usr_FlushCacheUsrBelongsToIns ();
   Usr_FlushCacheUsrBelongsToCtr ();
   Usr_FlushCacheUsrBelongsToDeg ();
   Usr_FlushCacheUsrBelongsToCrs ();
   Usr_FlushCacheUsrBelongsToCurrentCrs ();
   Usr_FlushCacheUsrHasAcceptedInCurrentCrs ();
   Usr_FlushCacheUsrSharesAnyOfMyCrs ();
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
       Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
      Ses_RemoveHiddenParFromThisSession ();
   Usr_FreeMyCourses ();
   Usr_FreeMyDegrees ();
   Usr_FreeMyCentres ();
   Usr_FreeMyInstits ();
   Usr_FreeMyCountrs ();
   Usr_UsrDataDestructor (&Gbl.Usrs.Me.UsrDat);
   Usr_UsrDataDestructor (&Gbl.Usrs.Other.UsrDat);
   Rec_FreeListFields ();
   Grp_FreeListGrpTypesAndGrps ();
   Grp_FreeListCodSelectedGrps ();
   Crs_FreeListCoursesInCurrentDegree ();
   Deg_FreeListDegs (&Gbl.Hierarchy.Ctr.Degs);
   DT_FreeListDegreeTypes ();
   Ins_FreeListInstitutions ();
   Ctr_FreeListCentres ();
   Cty_FreeListCountries ();
   Dpt_FreeListDepartments ();
   Plc_FreeListPlaces ();
   Hld_FreeListHolidays ();
   Lnk_FreeListLinks ();
   Plg_FreeListPlugins ();

   for (Role = (Rol_Role_t) 0;
	Role < Rol_NUM_ROLES;
	Role++)
      Usr_FreeUsrsList (Role);

   Usr_FreeListOtherRecipients ();
   Usr_FreeListsSelectedUsrsCods ();
   Syl_FreeListItemsSyllabus ();
   Tst_FreeTagsList ();
   Exa_FreeMemExamAnnouncement ();
   Exa_FreeListExamAnnouncements ();
   if (Gbl.F.Tmp)
      fclose (Gbl.F.Tmp);
   Fil_CloseXMLFile ();
   Fil_CloseReportFile ();
   Par_FreeParams ();
   Ale_ResetAllAlerts ();
  }
