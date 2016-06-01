// swad_global.c: global variables

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
#include <limits.h>		// For INT_MAX, LONG_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, calloc, free, etc
#include <string.h>		// For string functions
#include <sys/time.h>		// For gettimeofday
#include <sys/types.h>		// For getpid
#include <unistd.h>		// For getpid

#include "swad_action.h"
#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_exam.h"
#include "swad_global.h"
#include "swad_icon.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_theme.h"
#include "swad_web_service.h"

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
   Txt_Language_t Lan;

   setlocale (LC_ALL,"es_ES.utf8");

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

   Gbl.Form.Num = -1;			// Number of form. It's increased by 1 at the begin of each form
   Gbl.Form.Inside = false;		// Set to true inside a form to avoid nested forms

   Gbl.Error = false;

   Gbl.Layout.WritingHTMLStart =
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = false;

   Gbl.DB.DatabaseIsOpen = false;
   Gbl.DB.LockedTables = false;

   Gbl.HiddenParamsInsertedIntoDB = false;

   Gbl.Prefs.Language = Txt_Current_CGI_SWAD_Language;
   Gbl.Prefs.FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;	// Default first day of week
   Gbl.Prefs.Menu = Mnu_MENU_DEFAULT;		// Default menu
   Gbl.Prefs.Theme = The_THEME_DEFAULT;		// Default theme
   Gbl.Prefs.IconSet = Ico_ICON_SET_DEFAULT;	// Default icon set
   sprintf (Gbl.Prefs.IconsURL,"%s/%s",
            Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_PUBLIC_ICON);
   sprintf (Gbl.Prefs.PathTheme,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,
	    Cfg_ICON_FOLDER_THEMES,
	    The_ThemeId[Gbl.Prefs.Theme]);
   sprintf (Gbl.Prefs.PathIconSet,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_ICON_SETS,
	    Ico_IconSetId[Gbl.Prefs.IconSet]);

   Gbl.Session.NumSessions = 0;
   Gbl.Session.IsOpen = false;
   Gbl.Session.HasBeenDisconnected = false;

   Gbl.YearOK = false;

   Gbl.Usrs.Me.UsrIdLogin[0] = '\0';
   Gbl.Usrs.Me.LoginPlainPassword[0] = '\0';
   Gbl.Usrs.Me.UsrDat.UsrCod = -1L;
   Gbl.Usrs.Me.UsrDat.UsrIDNickOrEmail[0] = '\0';
   Usr_UsrDataConstructor (&Gbl.Usrs.Me.UsrDat);
   Usr_ResetMyLastData ();
   Gbl.Session.Id[0] = '\0';
   Gbl.Usrs.Me.Logged = false;
   Gbl.Usrs.Me.AvailableRoles = 0;
   Gbl.Usrs.Me.RoleFromSession              =
   Gbl.Usrs.Me.LoggedRole                   =
   Gbl.Usrs.Me.LoggedRoleBeforeCloseSession =
   Gbl.Usrs.Me.MaxRole                      = Rol_UNKNOWN;
   Gbl.Usrs.Me.IBelongToCurrentIns = false;
   Gbl.Usrs.Me.IBelongToCurrentCtr = false;
   Gbl.Usrs.Me.IBelongToCurrentDeg = false;
   Gbl.Usrs.Me.IBelongToCurrentCrs = false;
   Gbl.Usrs.Me.MyPhotoExists = false;
   Gbl.Usrs.Me.NumAccWithoutPhoto = 0;
   Gbl.Usrs.Me.TimeLastAccToThisFileBrowser = LONG_MAX;	// Initialized to a big value, so by default files are not shown as recent or new
   Gbl.Usrs.Me.MyInstitutions.Filled = false;
   Gbl.Usrs.Me.MyCentres.Filled = false;
   Gbl.Usrs.Me.MyDegrees.Filled = false;
   Gbl.Usrs.Me.MyCourses.Filled = false;
   Gbl.Usrs.Me.MyCourses.Num = 0;
   Gbl.Usrs.Me.MyAdminDegs.Num = 0;
   Gbl.Usrs.Me.MyAdminDegs.Lst = NULL;

   Gbl.Usrs.Other.UsrDat.UsrCod = -1L;
   Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail[0] = '\0';
   Usr_UsrDataConstructor (&Gbl.Usrs.Other.UsrDat);

   Gbl.Action.Act = ActUnk;
   Gbl.Action.UsesAJAX = false;
   Gbl.Action.Tab = TabUnk;

   Gbl.Usrs.LstGsts.NumUsrs = 0;
   Gbl.Usrs.LstGsts.Lst = NULL;

   Gbl.Usrs.LstStds.NumUsrs = 0;
   Gbl.Usrs.LstStds.Lst = NULL;

   Gbl.Usrs.LstTchs.NumUsrs = 0;
   Gbl.Usrs.LstTchs.Lst = NULL;

   Gbl.Usrs.LstAdms.NumUsrs = 0;
   Gbl.Usrs.LstAdms.Lst = NULL;

   Gbl.ExamAnnouncements.NumExaAnns = 0;
   Gbl.ExamAnnouncements.Lst = NULL;
   Gbl.ExamAnnouncements.HighlightExaCod = -1L;	// No exam announcement highlighted

   Gbl.Usrs.Select.All =
   Gbl.Usrs.Select.Std =
   Gbl.Usrs.Select.Tch =
   Gbl.Usrs.ListOtherRecipients = NULL;

   Gbl.Msg.Subject[0] = '\0';
   Gbl.Msg.ShowOnlyUnreadMsgs = false;
   Gbl.Msg.ExpandedMsgCod = -1L;

   Gbl.Pag.CurrentPage = 1;	// By default, go to the first page

   Gbl.CurrentCty.Cty.CtyCod = -1L;

   Gbl.CurrentIns.Ins.InsCod = -1L;	// Unknown institution
   Gbl.CurrentIns.Ins.ShortName[0] = '\0';
   Gbl.CurrentIns.Ins.FullName[0] = '\0';
   Gbl.CurrentIns.Ins.WWW[0] = '\0';
   Gbl.CurrentIns.Ins.NumCtrs = Gbl.CurrentIns.Ins.NumDpts = Gbl.CurrentIns.Ins.NumDegs = 0;
   Gbl.CurrentIns.Ins.NumUsrs = 0;

   Gbl.CurrentCtr.Ctr.CtrCod = -1L;
   Gbl.CurrentCtr.Ctr.InsCod = -1L;
   Gbl.CurrentCtr.Ctr.PlcCod = -1L;
   Gbl.CurrentCtr.Ctr.ShortName[0] = '\0';
   Gbl.CurrentCtr.Ctr.FullName[0] = '\0';

   Gbl.CurrentDegTyp.DegTyp.DegTypCod = -1L;
   Gbl.CurrentDegTyp.DegTyp.DegTypName[0] = '\0';

   Gbl.CurrentDeg.Deg.DegCod = -1L;
   Gbl.CurrentDeg.Deg.ShortName[0] = Gbl.CurrentDeg.Deg.FullName[0] = '\0';

   Gbl.CurrentCrs.Crs.CrsCod = -1L;
   Gbl.CurrentCrs.Crs.ShortName[0] = Gbl.CurrentCrs.Crs.FullName[0] = '\0';
   Gbl.CurrentCrs.Info.ShowMsgMustBeRead = 0;
   Gbl.CurrentCrs.Notices.HighlightNotCod = -1L;	// No notice highlighted

   Gbl.Inss.Num = 0;
   Gbl.Inss.Lst = NULL;
   Gbl.Inss.SelectedOrderType = Ins_DEFAULT_ORDER_TYPE;
   Gbl.Inss.EditingIns.InsCod = -1L;
   Gbl.Inss.EditingIns.CtyCod = -1L;
   Gbl.Inss.EditingIns.ShortName[0] = '\0';
   Gbl.Inss.EditingIns.FullName[0] = '\0';
   Gbl.Inss.EditingIns.WWW[0] = '\0';
   Gbl.Inss.EditingIns.NumCtrs = Gbl.Inss.EditingIns.NumDpts = Gbl.Inss.EditingIns.NumDegs = 0;
   Gbl.Inss.EditingIns.NumUsrs = 0;

   Gbl.Ctys.Num = 0;
   Gbl.Ctys.Lst = NULL;
   Gbl.Ctys.SelectedOrderType = Cty_DEFAULT_ORDER_TYPE;
   Gbl.Ctys.EditingCty.CtyCod = -1L;
   for (Lan = (Txt_Language_t) 1;
	Lan <= Txt_NUM_LANGUAGES;
	Lan++)
      Gbl.Ctys.EditingCty.Name[Lan][0] = '\0';

   Gbl.Ctrs.Num = 0;
   Gbl.Ctrs.Lst = NULL;
   Gbl.Ctrs.SelectedOrderType = Ctr_DEFAULT_ORDER_TYPE;
   Gbl.Ctrs.EditingCtr.CtrCod = -1L;
   Gbl.Ctrs.EditingCtr.InsCod = -1L;
   Gbl.Ctrs.EditingCtr.CtrCod = -1L;
   Gbl.Ctrs.EditingCtr.ShortName[0] = '\0';
   Gbl.Ctrs.EditingCtr.FullName[0] = '\0';
   Gbl.Ctrs.EditingCtr.WWW[0] = '\0';

   Gbl.Dpts.Num = 0;
   Gbl.Dpts.Lst = NULL;
   Gbl.Dpts.SelectedOrderType = Dpt_DEFAULT_ORDER_TYPE;
   Gbl.Dpts.EditingDpt.DptCod = -1L;
   Gbl.Dpts.EditingDpt.ShortName[0] = '\0';
   Gbl.Dpts.EditingDpt.FullName[0] = '\0';
   Gbl.Dpts.EditingDpt.WWW[0] = '\0';

   Gbl.Plcs.Num = 0;
   Gbl.Plcs.Lst = NULL;
   Gbl.Plcs.SelectedOrderType = Plc_DEFAULT_ORDER_TYPE;
   Gbl.Plcs.EditingPlc.PlcCod = -1L;
   Gbl.Plcs.EditingPlc.ShortName[0] = '\0';
   Gbl.Plcs.EditingPlc.FullName[0] = '\0';

   Gbl.Hlds.LstIsRead = false;	// List is not read
   Gbl.Hlds.Num = 0;
   Gbl.Hlds.Lst = NULL;
   Gbl.Hlds.SelectedOrderType = Hld_DEFAULT_ORDER_TYPE;
   Gbl.Hlds.EditingHld.HldCod = -1L;
   Gbl.Hlds.EditingHld.PlcCod = -1L;
   Gbl.Hlds.EditingHld.HldTyp = Hld_HOLIDAY;
   Gbl.Hlds.EditingHld.StartDate.Year  = Gbl.Hlds.EditingHld.EndDate.Year  = Gbl.Now.Date.Year;
   Gbl.Hlds.EditingHld.StartDate.Month = Gbl.Hlds.EditingHld.EndDate.Month = Gbl.Now.Date.Month;
   Gbl.Hlds.EditingHld.StartDate.Day   = Gbl.Hlds.EditingHld.EndDate.Day   = Gbl.Now.Date.Day;
   Gbl.Hlds.EditingHld.Name[0] = '\0';

   Gbl.Degs.DegTypes.Num = 0;
   Gbl.Degs.DegTypes.Lst = NULL;

   Gbl.Degs.EditingDegTyp.DegTypCod     = -1L;
   Gbl.Degs.EditingDegTyp.DegTypName[0] = '\0';

   Gbl.Degs.EditingDeg.DegCod       = -1L;
   Gbl.Degs.EditingDeg.ShortName[0] = '\0';
   Gbl.Degs.EditingDeg.FullName[0]  = '\0';
   Gbl.Degs.EditingDeg.WWW[0]       = '\0';
   Gbl.Degs.EditingDeg.LstCrss      = NULL;

   Gbl.Degs.EditingCrs.CrsCod       = -1L;
   Gbl.Degs.EditingCrs.DegCod       = -1L;
   Gbl.Degs.EditingCrs.Year         = 0;
   Gbl.Degs.EditingCrs.ShortName[0] = '\0';
   Gbl.Degs.EditingCrs.FullName[0]  = '\0';

   Gbl.CurrentCrs.Grps.NumGrps = 0;
   Gbl.CurrentCrs.Grps.WhichGrps = Grp_ALL_GROUPS;
   Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes = NULL;
   Gbl.CurrentCrs.Grps.GrpTypes.Num = 0;
   Gbl.CurrentCrs.Grps.GrpTypes.NestedCalls = 0;
   Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName[0] = '\0';
   Gbl.CurrentCrs.Grps.GrpTyp.MandatoryEnrollment = true;
   Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrollment = false;
   Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened = false;
   Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC = (time_t) 0;
   Gbl.CurrentCrs.Grps.GrpCod = -1L; // -1L stands for the whole course
   Gbl.CurrentCrs.Grps.GrpName[0] = '\0';
   Gbl.CurrentCrs.Grps.MaxStudents = INT_MAX;
   Gbl.CurrentCrs.Grps.Open = false;
   Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCod  = NULL;
   Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps = 0;
   Gbl.CurrentCrs.Grps.LstGrpsSel.NestedCalls = 0;

   Gbl.Usrs.ClassPhoto.AllGroups = true;

   Gbl.CurrentCrs.Records.Field.Name[0] = '\0';
   Gbl.CurrentCrs.Records.Field.NumLines = Rec_MIN_LINES_IN_EDITION_FIELD;
   Gbl.CurrentCrs.Records.Field.Visibility = Rec_HIDDEN_FIELD;

   Gbl.CurrentCrs.Records.LstFields.Lst = NULL;
   Gbl.CurrentCrs.Records.LstFields.Num = 0;
   Gbl.CurrentCrs.Records.LstFields.NestedCalls = 0;

   Gbl.Syllabus.EditionIsActive = false;
   Gbl.Syllabus.WhichSyllabus = Syl_DEFAULT_WHICH_SYLLABUS;

   Gbl.Search.WhatToSearch = Sch_SEARCH_ALL;
   Gbl.Search.Str[0] = '\0';

   Gbl.Asgs.LstIsRead = false;	// List is not read
   Gbl.Asgs.Num = 0;
   Gbl.Asgs.LstAsgCods = NULL;
   Gbl.Asgs.SelectedOrderType = Asg_DEFAULT_ORDER_TYPE;

   Gbl.AttEvents.LstIsRead = false;	// List is not read
   Gbl.AttEvents.Num = 0;
   Gbl.AttEvents.Lst = NULL;
   Gbl.AttEvents.SelectedOrderType = Att_DEFAULT_ORDER_TYPE;
   Gbl.AttEvents.AttCod = -1L;

   Gbl.Mails.Num = 0;
   Gbl.Mails.Lst = NULL;
   Gbl.Mails.SelectedOrderType = Mai_DEFAULT_ORDER_TYPE;

   Gbl.Banners.Num = 0;
   Gbl.Banners.Lst = NULL;
   Gbl.Banners.EditingBan.BanCod = -1L;
   Gbl.Banners.EditingBan.ShortName[0] = '\0';
   Gbl.Banners.EditingBan.FullName[0] = '\0';
   Gbl.Banners.EditingBan.WWW[0] = '\0';
   Gbl.Banners.BanCodClicked = 0L;

   Gbl.Links.Num = 0;
   Gbl.Links.Lst = NULL;
   Gbl.Links.EditingLnk.LnkCod = -1L;
   Gbl.Links.EditingLnk.ShortName[0] = '\0';
   Gbl.Links.EditingLnk.FullName[0] = '\0';
   Gbl.Links.EditingLnk.WWW[0] = '\0';

   Gbl.Usrs.Listing.RecsUsrs   = Rec_RECORD_USERS_UNKNOWN;
   Gbl.Usrs.Listing.RecsPerPag = 1;
   Gbl.Usrs.Listing.WithPhotos = true;

   Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;

   /* Statistics */
   Gbl.Stat.ClicksGroupedBy = Sta_CLICKS_CRS_PER_USR;
   Gbl.Stat.CountType   = Sta_TOTAL_CLICKS;
   Gbl.Stat.Role        = Sta_IDENTIFIED_USRS;
   Gbl.Stat.NumAction   = ActAll;
   Gbl.Stat.RowsPerPage = 50;
   Gbl.Stat.FigureType = Sta_USERS;

   Gbl.Scope.Current = Sco_SCOPE_CRS;

   Gbl.Usrs.Connected.TimeToRefreshInMs = Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS;

   /* Tests */
   Gbl.Test.Config.Pluggable = Tst_PLUGGABLE_UNKNOWN;
   Gbl.Test.NumQsts = Tst_CONFIG_DEFAULT_DEF_QUESTIONS;
   Gbl.Test.AllowTeachers = false;	// Must the exam be saved?
   Gbl.Test.AllAnsTypes = false;
   Gbl.Test.ListAnsTypes[0] = '\0';

   Gbl.Test.Tags.Num  = 0;
   Gbl.Test.Tags.All  = false;
   Gbl.Test.Tags.List = NULL;

   /* Forums */
   Gbl.Forum.ForumType = (For_ForumType_t) 0;
   Gbl.Forum.WhichForums = For_DEFAULT_WHICH_FORUMS;
   Gbl.Forum.SelectedOrderType = For_DEFAULT_ORDER;
   Gbl.Forum.ThreadToMove = -1L;

   /* User nickname */
   Gbl.Usrs.Me.UsrDat.Nickname[0] = '\0';

   /* File browser */
   Gbl.FileBrowser.Id = 0;
   Gbl.FileBrowser.Type = Brw_UNKNOWN;
   Gbl.FileBrowser.FileType = Brw_IS_UNKNOWN;
   Gbl.FileBrowser.UploadingWithDropzone = false;

   /* To alternate colors where listing rows */
   Gbl.RowEvenOdd = 0;
   Gbl.ColorRows[0] = "COLOR0";	// Darker
   Gbl.ColorRows[1] = "COLOR1";	// Lighter

   /* Imported data from external site */
   Gbl.Imported.ExternalUsrId[0] = '\0';
   Gbl.Imported.ExternalSesId[0] = '\0';
   Gbl.Imported.ExternalRole = Rol_UNKNOWN;

   Gbl.WebService.Function = Svc_unknown;
  }

/*****************************************************************************/
/************* Free memory, close files, remove lock file, etc. **************/
/*****************************************************************************/

void Gbl_Cleanup (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

   if (!Gbl.Action.UsesAJAX &&
       !Gbl.WebService.IsWebService &&
       Act_Actions[Gbl.Action.Act].BrowserWindow == Act_MAIN_WINDOW &&
       !Gbl.HiddenParamsInsertedIntoDB)
      Ses_RemoveHiddenParFromThisSession ();
   Usr_UsrDataDestructor (&Gbl.Usrs.Me.UsrDat);
   Usr_UsrDataDestructor (&Gbl.Usrs.Other.UsrDat);
   Rec_FreeListFields ();
   Grp_FreeListGrpTypesAndGrps ();
   Grp_FreeListCodSelectedGrps ();
   Crs_FreeListCoursesInDegree (&Gbl.Degs.EditingDeg);
   Deg_FreeListMyAdminDegs ();
   DT_FreeListDegreeTypes ();
   Ins_FreeListInstitutions ();
   Ctr_FreeListCentres ();
   Cty_FreeListCountries ();
   Dpt_FreeListDepartments ();
   Plc_FreeListPlaces ();
   Hld_FreeListHolidays ();
   Lnk_FreeListLinks ();
   Plg_FreeListPlugins ();
   Usr_FreeUsrsList (&Gbl.Usrs.LstAdms);
   Usr_FreeUsrsList (&Gbl.Usrs.LstTchs);
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);
   Usr_FreeListOtherRecipients ();
   Usr_FreeListsSelectedUsrCods ();
   Syl_FreeListItemsSyllabus ();
   Tst_FreeTagsList ();
   Exa_FreeMemExamAnnouncement ();
   Exa_FreeListExamAnnouncements ();
   if (Gbl.F.Tmp)
      fclose (Gbl.F.Tmp);
   Fil_CloseXMLFile ();
   Par_FreeParams ();
  }
