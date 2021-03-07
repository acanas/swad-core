// swad_global.h: global variables

#ifndef _SWAD_GBL
#define _SWAD_GBL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include <limits.h>		// For maximum values
#include <stdio.h>		// For FILE
#include <sys/time.h>		// For tz
#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_alert.h"
#include "swad_API.h"
#include "swad_assignment.h"
#include "swad_box.h"
#include "swad_center.h"
#include "swad_connected.h"
#include "swad_config.h"
#include "swad_country.h"
#include "swad_course.h"
#include "swad_cryptography.h"
#include "swad_degree_type.h"
#include "swad_enrolment.h"
#include "swad_file.h"
#include "swad_file_browser.h"
#include "swad_icon.h"
#include "swad_institution.h"
#include "swad_layout.h"
#include "swad_mail.h"
#include "swad_mark.h"
#include "swad_media.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_plugin.h"
#include "swad_record.h"
#include "swad_search.h"
#include "swad_session.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Globals
  {
   struct
     {
      char DatabasePassword[Cfg_MAX_BYTES_DATABASE_PASSWORD + 1];
      char SMTPPassword[Cfg_MAX_BYTES_SMTP_PASSWORD + 1];
     } Config;
   struct Files F;
   MYSQL mysql;
   pid_t PID;	// PID of current process
   struct
     {
      int Num;			// Number of form, used in form submit links
      char Id[32];		// Identifier string used in forms
      char UniqueId[32 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 10 + 1];	// Unique identifier string used in forms
      bool Inside;		// Set to true inside a form to avoid nested forms
     } Form;
   struct
     {
      int Nested;			// Index of top open box
      char *Ids[Box_MAX_NESTED];	// 0 <= box index < Box_MAX_NESTED
     } Box;
   struct
     {
      size_t Num;		// Number of alert
      struct
        {
	 Ale_AlertType_t Type;
	 char *Text;		// Message to be displayed
	 char *Section;		// Where to display the alert
        } List[Ale_MAX_ALERTS];
     } Alerts;		// Alert message created in a function and printed in a subsequent function.
   struct
     {
      size_t ContentLength;
      char *QueryString;	// String allocated dynamically with the arguments sent to the CGI
      struct Param *List;	// Linked list of parameters
      bool GetMethod;		// Am I accessing using GET method?
     } Params;

   Act_Content_t ContentReceivedByCGI;	/* Content send by the form and received by the CGI:
						Act_CONTENT_NORM (if CONTENT_TYPE==text/plain) or
						Act_CONT_DATA (if CONTENT_TYPE==multipart/form-data) */
   struct
     {
      char StrWithoutCRLF[Par_MAX_BYTES_BOUNDARY_WITHOUT_CR_LF + 1];
      char StrWithCRLF   [Par_MAX_BYTES_BOUNDARY_WITH_CR_LF    + 1];
      size_t LengthWithoutCRLF;
      size_t LengthWithCRLF;
     } Boundary;

   struct
     {
      bool WritingHTMLStart;	// Used to avoid writing the HTML head when aborting program on error
      bool HTMLStartWritten;	// Used to avoid writing more than once the HTML head
      bool DivsEndWritten;	// Used to avoid writing more than once the HTML end
      bool HTMLEndWritten;	// Used to avoid writing more than once the HTML end
     } Layout;
   struct
     {
      bool DatabaseIsOpen;
      bool LockedTables;
     } DB;

   bool HiddenParamsInsertedIntoDB;	// If parameters are inserted in the database in this execution

   /* To compute execution time of the program */
   struct timeval tvStart;
   struct timeval tvPageCreated;
   struct timeval tvPageSent;
   struct timezone tz;
   long TimeGenerationInMicroseconds;
   long TimeSendInMicroseconds;

   char IP[Cns_MAX_BYTES_IP + 1];
   char UniqueNameEncrypted[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];	// Used for session id, temporary directory names, etc.

   struct
     {
      Lan_Language_t Language;
      unsigned FirstDayOfWeek;
      Dat_Format_t DateFormat;
      Mnu_Menu_t Menu;
      unsigned SideCols;
      The_Theme_t Theme;
      Ico_IconSet_t IconSet;
      char URLTheme[PATH_MAX + 1];
      char URLIconSet[PATH_MAX + 1];
     } Prefs;

   struct
     {
      unsigned NumSessions;
      bool IsOpen;
      bool HasBeenDisconnected;
      char Id[Cns_BYTES_SESSION_ID + 1];
      long UsrCod;
     } Session;

   struct
     {
      Tab_Tab_t Tab;
      Act_Action_t Act;
      Act_Action_t Original;	// Used in some actions to know what action gave rise to the current action
      bool UsesAJAX;		// Do not generate full HTML page, only the content of a div
      bool IsAJAXAutoRefresh;	// It's an automatic refresh drom time to time
     } Action;
   time_t StartExecutionTimeUTC;
   struct DateTime Now;
   struct Date Yesterday;
   unsigned RowEvenOdd;	// To alternate row colors in listings
   char *ColorRows[2];
   const char *XMLPtr;
   struct
     {
      char FileName[PATH_MAX + 1];
     } HTMLOutput;
   struct
     {
      Hie_Lvl_Level_t Level;	// Current level in the hierarchy: system, country, institution, center, degree or course
      long Cod;			// Code of the current country, institution, center, degree or course
      struct ListCountries Ctys;// List of countries
      struct Cty_Countr Cty;	// Current country
      struct ListInstits Inss;	// List of institutions in current country
      struct Ins_Instit Ins;	// Current institution
      struct ListCenters Ctrs;	// List of centers in current institution
      struct Ctr_Center Ctr;	// Current center
      struct ListDegrees Degs;	// List of degrees in current center
      struct Deg_Degree Deg;	// Current degree
      struct ListCourses Crss;	// List of courses in current degree
      struct Crs_Course Crs;	// Current course. Aditional info about course is stored in Gbl.Crs.
     } Hierarchy;
   struct
     {
      unsigned Num;		// Number of degree types
      struct DegreeType *Lst;	// List of degree types
     } DegTypes;
   struct
     {
      unsigned Num;		// Number of mail domains
      struct Mail *Lst;		// List of mail domains
      Mai_DomainsOrder_t SelectedOrder;
     } Mails;
   struct
     {
      unsigned Num;		// Number of institutional links
      struct Link *Lst;		// List of institutional links
     } Links;
   struct
     {
      Sch_WhatToSearch_t WhatToSearch;
      char Str[Sch_MAX_BYTES_STRING_TO_FIND + 1];
      bool LogSearch;
     } Search;
  struct
     {
      unsigned Num;		// Number of plugins
      struct Plugin *Lst;	// List of plugins
     } Plugins;
   struct
     {
      bool IsWebService;	// Must generate HTML output (IsWebService==false) or SOAP-XML output (IsWebService==true)?
      long PlgCod;
      API_Function_t Function;
     } WebService;
   struct
     {
      struct
	{
	 struct UsrData UsrDat;
	 struct UsrLast UsrLast;
	 struct
	   {
	    unsigned Available;
	    Rol_Role_t FromSession;
	    Rol_Role_t Logged;
	    Rol_Role_t LoggedBeforeCloseSession;
	    Rol_Role_t Max;
	    bool HasChanged;	// Set when I have changed my role
	   } Role;
	 char UsrIdLogin[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];	// String to store the ID, nickname or email entered in the user's login
         char LoginPlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1];
         char LoginEncryptedPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
         char PendingPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
	 char PathDir[PATH_MAX + 1];
	 bool Logged;
         bool IBelongToCurrentIns;
         bool IBelongToCurrentCtr;
         bool IBelongToCurrentDeg;
         bool IBelongToCurrentCrs;
         bool MyPhotoExists;
         unsigned NumAccWithoutPhoto;
         char PhotoURL[PATH_MAX + 1];
         time_t TimeLastAccToThisFileBrowser;
         bool ConfirmEmailJustSent;	// An email to confirm my email address has just been sent
         struct
           {
            bool Filled;	// My institutions are already filled?
            unsigned Num;
            struct
              {
               long CtyCod;
               Rol_Role_t MaxRole;
              } Ctys[Cty_MAX_COUNTRS_PER_USR];
           } MyCtys;
         struct
           {
            bool Filled;	// My institutions are already filled?
            unsigned Num;
            struct
              {
               long InsCod;
               Rol_Role_t MaxRole;
              } Inss[Ins_MAX_INSTITS_PER_USR];
           } MyInss;
         struct
           {
            bool Filled;	// My centers are already filled?
            unsigned Num;
            struct
              {
               long CtrCod;
               Rol_Role_t MaxRole;
              } Ctrs[Ctr_MAX_CENTERS_PER_USR];
           } MyCtrs;
         struct
           {
            bool Filled;	// My degrees are already filled?
            unsigned Num;
            struct
              {
               long DegCod;
               Rol_Role_t MaxRole;
              } Degs[Deg_MAX_DEGREES_PER_USR];
           } MyDegs;
         struct
           {
            bool Filled;	// My courses are already filled?
            unsigned Num;
            struct
              {
               long CrsCod;
               Rol_Role_t Role;
               long DegCod;
              } Crss[Crs_MAX_COURSES_PER_USR];
           } MyCrss;
	 Usr_ShowUsrsType_t ListType;	// My preference about user's list type
	 unsigned NumFollowers;	// Number of users who follow me
	 unsigned NumFollowing;	// Number of users I follow
	} Me;		// The user logged
      struct
	{
	 struct UsrData UsrDat;
	 unsigned NumIDToConfirm;	// Used as hidden parameter to confirm a user's ID
	} Other;		// Another user, used for example to register in / remove from a course
      struct ListUsrs LstUsrs[Rol_NUM_ROLES];
      struct SelectedUsrs Selected;
      char *ListOtherRecipients;	// List of ID or nicks of users written explicitely on a form
      struct
	{
	 Rec_UsrsType_t RecsUsrs;	// Students' records or teachers' records
	 unsigned RecsPerPag;		// Number of rows of records in a row (only for printing)
	 bool WithPhotos;
	} Listing;
      struct
	{
	 unsigned Cols;
	 bool AllGroups;
	} ClassPhoto;
      struct
        {
         unsigned long TimeToRefreshInMs;
         unsigned NumUsr;
         unsigned NumUsrs;
         unsigned NumUsrsToList;
         struct ConnectedUsrs Usrs[Rol_NUM_ROLES];
         struct
           {
            long UsrCod;
            bool ThisCrs;
            time_t TimeDiff;
           } Lst[Cfg_MAX_CONNECTED_SHOWN];
        } Connected;
      char FileNamePhoto[NAME_MAX + 1];	// File name (with no path and no .jpg) of the temporary file with the selected face
      Enr_RegRemOneUsrAction_t RegRemAction;	// Enrolment action
     } Usrs;
   struct
     {
      struct UsrData *UsrDat;
      Rec_SharedRecordViewType_t TypeOfView;
     } Record;
   struct
     {
      Hie_Lvl_Level_t Current;
      Hie_Lvl_Level_t Default;
      unsigned Allowed;
     } Scope;
   struct
     {
      char PathPriv[PATH_MAX + 1];	// Absolute path to the private directory of the course
      char PathRelPubl[PATH_MAX + 1];   // Relative path to the public directory of the course
      char PathURLPubl[PATH_MAX + 1];   // Abolute URL to the public part of the course
      struct Grp_Groups Grps;
      struct Inf_Info Info;
      struct
	{
	 struct RecordField Field;
	 struct LstRecordFields LstFields;
	} Records;
      struct
	{
	 long HighlightNotCod;	// Notice code of a notice to be highlighted
	} Notices;
     } Crs;
   struct
     {
      unsigned Id;		// Each file browser in the page has a unique identifier
      Brw_FileBrowser_t Type;
      bool FullTree;		// Show full tree?
      bool ShowOnlyPublicFiles;	// Show only public files?
      bool UploadingWithDropzone;
      struct
        {
         unsigned long MaxFiles;
         unsigned long MaxFolds;
         unsigned long long int MaxQuota;
         unsigned NumLevls;
         unsigned long NumFolds;
         unsigned long NumFiles;
         unsigned long long int TotalSiz;
        } Size;
      struct
	{
	 char PathAboveRootFolder[PATH_MAX + 1];
	 char PathRootFolder[PATH_MAX + 1];
	} Priv;
      char NewFilFolLnkName[NAME_MAX + 1];
      struct FilFolLnk FilFolLnk;
      unsigned Level;
      Brw_IconViewEdit_t IconViewEdit;
      struct
        {
         Brw_FileBrowser_t FileBrowser;	// Type of the file browser
         long Cod;			// Code of the institution/center/degree/course/group related to the file browser with the clipboard
	 long WorksUsrCod;		// User code of the user related to the works file browser with the clipboard
         unsigned Level;
         struct FilFolLnk FilFolLnk;
         bool IsThisTree;		// When showing a file browser, is it that corresponding to the clipboard?
         bool IsThisFile;		// When showing a row of a file browser, are we in the path of the clipboard?
        } Clipboard;
      struct
        {
         char L[2 + 1];		// Left directory: 2 first chars
         char R[NAME_MAX + 1];	// Right directory: rest of chars
        } TmpPubDir;
      bool HiddenLevels[1 + Brw_MAX_DIR_LEVELS];
      const char *TxtStyle;
      const char *InputStyle;
      struct Asg_Assignment Asg;	// Data of assignment when browsing level 1 or an assignment zone.
				        // TODO: Remove from global?
      struct
        {
	 bool CreateZIP;
         char TmpDir[NAME_MAX + 1];
        } ZIP;
     } FileBrowser;	// Struct used for a file browser
   struct
     {
      struct DateTime DateIni;	// TODO: Remove in future versions
      struct DateTime DateEnd;	// TODO: Remove in future versions
      time_t TimeUTC[Dat_NUM_START_END_TIME];
     } DateRange;

   /* Cache */
   struct
     {
      struct
	{
	 long CtyCod;
	 Lan_Language_t Language;
	 char CtyName[Cty_MAX_BYTES_NAME + 1];
	} CountryName;
      struct
	{
	 long InsCod;
	 char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];
	} InstitutionShrtName;
      struct
	{
	 long InsCod;
	 char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];
	 char CtyName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];
	} InstitutionShrtNameAndCty;
      struct
        {
	 long InsCod;
	 unsigned NumDpts;
        } NumDptsInIns;
      struct
        {
         bool Valid;
	 long CtyCod;
	 unsigned NumInss;
        } NumInssInCty;
      struct
        {
	 long CtyCod;
	 unsigned NumCtrs;
        } NumCtrsInCty;
      struct
        {
	 long InsCod;
	 unsigned NumCtrs;
        } NumCtrsInIns;
      struct
        {
	 long CtyCod;
	 unsigned NumDegs;
        } NumDegsInCty;
      struct
        {
	 long InsCod;
	 unsigned NumDegs;
        } NumDegsInIns;
      struct
        {
	 long CtrCod;
	 unsigned NumDegs;
        } NumDegsInCtr;
      struct
        {
	 long CtyCod;
	 unsigned NumCrss;
        } NumCrssInCty;
      struct
        {
	 long InsCod;
	 unsigned NumCrss;
        } NumCrssInIns;
      struct
        {
	 long CtrCod;
	 unsigned NumCrss;
        } NumCrssInCtr;
      struct
        {
	 long DegCod;
	 unsigned NumCrss;
        } NumCrssInDeg;
      struct
        {
	 bool Valid;
	 unsigned NumUsrs;
        } NumUsrsWhoDontClaimToBelongToAnyCty;
      struct
        {
	 bool Valid;
	 unsigned NumUsrs;
        } NumUsrsWhoClaimToBelongToAnotherCty;
      struct
        {
	 long CtyCod;
	 unsigned NumUsrs;
        } NumUsrsWhoClaimToBelongToCty;
      struct
        {
	 long InsCod;
	 unsigned NumUsrs;
        } NumUsrsWhoClaimToBelongToIns;
      struct
        {
	 long CtrCod;
	 unsigned NumUsrs;
        } NumUsrsWhoClaimToBelongToCtr;
      struct
	{
	 long UsrCod;
	 bool IsSuperuser;
	} UsrIsSuperuser;
      struct
	{
	 long UsrCod;
	 long InsCod;
	 bool Belongs;
	} UsrBelongsToIns;
      struct
	{
	 long UsrCod;
	 long CtrCod;
	 bool Belongs;
	} UsrBelongsToCtr;
      struct
	{
	 long UsrCod;
	 long DegCod;
	 bool Belongs;
	} UsrBelongsToDeg;
      struct
	{
	 long UsrCod;
	 long CrsCod;
	 bool CountOnlyAcceptedCourses;
	 bool Belongs;
	} UsrBelongsToCrs;
      struct
	{
	 long UsrCod;
	 bool Belongs;
	} UsrBelongsToCurrentCrs;
      struct
	{
	 long UsrCod;
	 bool Accepted;
	} UsrHasAcceptedInCurrentCrs;
      struct
	{
	 long UsrCod;
	 bool SharesAnyOfMyCrs;
	} UsrSharesAnyOfMyCrs;
      struct
	{
	 long GrpCod;
	 bool IBelong;
	} IBelongToGrp;
      struct
	{
	 long UsrCod;
	 bool Shares;
	} UsrSharesAnyOfMyGrpsInCurrentCrs;
      struct
	{
	 long UsrCod;
	 long CrsCod;
	 Rol_Role_t Role;
	} RoleUsrInCrs;
      struct
	{
	 long PrjCod;
	 unsigned RolesInProject;
	} MyRolesInProject;
      struct
        {
	 long UsrCod;
	 unsigned NumFollowing;
	 unsigned NumFollowers;
        } Follow;
     } Cache;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Gbl_InitializeGlobals (void);
void Gbl_Cleanup (void);

#endif
