// swad_global.h: global variables

#ifndef _SWAD_GBL
#define _SWAD_GBL
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

#include <limits.h>		// For maximum values
#include <stdio.h>		// For FILE
#include <sys/time.h>		// For tz
#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_API.h"
#include "swad_assignment.h"
#include "swad_browser.h"
#include "swad_connected.h"
#include "swad_config.h"
#include "swad_course.h"
#include "swad_enrolment.h"
#include "swad_file.h"
#include "swad_group.h"
#include "swad_hierarchy.h"
#include "swad_mail.h"
#include "swad_record.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Globals
  {
   struct
     {
      bool WritingHTMLStart;	// Used to avoid writing the HTML head when aborting program on error
      bool HTMLStartWritten;	// Used to avoid writing more than once the HTML head
      bool DivsEndWritten;	// Used to avoid writing more than once the HTML end
      bool HTMLEndWritten;	// Used to avoid writing more than once the HTML end
     } Layout;
   struct
     {
      Lan_Language_t Language;
      unsigned FirstDayOfWeek;
      Dat_Format_t DateFormat;
      Ico_IconSet_t IconSet;
      Mnu_Menu_t Menu;
      The_Theme_t Theme;
      unsigned SideCols;
      PhoSha_Shape_t PhotoShape;
     } Prefs;
   struct
     {
      unsigned NumSessions;
      bool IsOpen;
      bool HasBeenDisconnected;
      char Id[Cns_BYTES_SESSION_ID + 1];
      long UsrCod;
      bool ParsInsertedIntoDB;	// If parameters are inserted in the database in this session
     } Session;
   struct
     {
      Tab_Tab_t Tab;		// Current navigation tab
      Act_Action_t Act;		// Current action
      Act_Action_t Original;	// Used in some actions to know what action gave rise to the current action
      bool UsesAJAX;		// Do not generate full HTML page, only the content of a div
      bool IsAJAXAutoRefresh;	// It's an automatic refresh drom time to time
     } Action;
   struct
     {
      Hie_Level_t Level;	// Current level in the hierarchy: system, country, institution, center, degree or course
      struct Hie_List List[Hie_NUM_LEVELS];	// List of child nodes of current node
      struct Hie_Node Node[Hie_NUM_LEVELS];	// Current node
     } Hierarchy;
   struct
     {
      Hie_Level_t Current;
      unsigned Allowed;
     } Scope;
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
	 struct Usr_Data UsrDat;
	 struct Usr_Last UsrLast;
	 struct
	   {
	    unsigned Available;
	    Rol_Role_t FromSession;
	    Rol_Role_t Logged;
	    Rol_Role_t LoggedBeforeCloseSession;
	    Rol_Role_t Max;
	    bool HasChanged;	// Set when I have changed my role
	   } Role;
	 char UsrIdLogin[Cns_MAX_BYTES_USR_LOGIN + 1];	// String to store the ID, nickname or email entered in the user's login
         char LoginPlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1];
         char LoginEncryptedPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
         char PendingPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
	 char PathDir[PATH_MAX + 1];
	 bool Logged;
         bool MyPhotoExists;
         unsigned NumAccWithoutPhoto;
         char PhotoURL[WWW_MAX_BYTES_WWW + 1];
         time_t TimeLastAccToThisFileBrowser;
         bool ConfirmEmailJustSent;	// An email to confirm my email address has just been sent
         struct
           {
            struct Hie_MyNode *Nodes;	// List of courses/degrees/centers/institutions/countries
            unsigned Num;		// Number of courses/degrees/centers/institutions/countries
            bool Filled;		// List is already filled?
           } Hierarchy[Hie_NUM_LEVELS];	// My hierarchy
         Usr_Belong_t IBelongToCurrent[Hie_NUM_LEVELS];
	 Set_ShowUsrsType_t ListType;	// My preference about user's list type
	 unsigned NumFollowers;	// Number of users who follow me
	 unsigned NumFollowing;	// Number of users I follow
	} Me;		// The user logged
      struct
	{
	 struct Usr_Data UsrDat;
	 unsigned NumIDToConfirm;	// Used as hidden parameter to confirm a user's ID
	} Other;		// Another user, used for example to register in / remove from a course
      struct Usr_ListUsrs LstUsrs[Rol_NUM_ROLES];
      struct Usr_SelectedUsrs Selected;
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
	} ClassPhoto;
      struct
        {
         unsigned long TimeToRefreshInMs;
         unsigned NumUsr;
         unsigned NumUsrs;
         unsigned NumUsrsToList;
         struct Con_ConnectedUsrs Usrs[Rol_NUM_ROLES];
         struct
           {
            long UsrCod;
            bool ThisCrs;
            time_t TimeDiff;
           } Lst[Cfg_MAX_CONNECTED_SHOWN];
        } Connected;
      char FileNamePhoto[NAME_MAX + 1];	// File name (with no path and no .jpg) of the temporary file with the selected face
      Enr_EnrRemOneUsrAction_t EnrRemAction;	// Enrolment action
     } Usrs;
   struct
     {
      struct
        {
	 char AbsPriv[PATH_MAX + 1];	// Absolute path to the private directory of the course
	 char RelPubl[PATH_MAX + 1];	// Relative path to the public directory of the course
	 char URLPubl[PATH_MAX + 1];	// Abolute URL to the public part of the course
        } Path;
      struct Grp_Groups Grps;
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
	 char AboveRootFolder[PATH_MAX + 1];
	 char RootFolder[PATH_MAX + 1];
	} Path;
      char NewFilFolLnkName[NAME_MAX + 1];
      struct Brw_FilFolLnk FilFolLnk;
      unsigned Level;
      struct
        {
         Brw_FileBrowser_t FileBrowser;	// Type of the file browser
         long HieCod;			// Code of the institution/center/degree/course/group related to the file browser with the clipboard
	 long WorksUsrCod;		// User code of the user related to the works file browser with the clipboard
         unsigned Level;
         struct Brw_FilFolLnk FilFolLnk;
         bool IsThisTree;		// When showing a file browser, is it that corresponding to the clipboard?
         bool IsThisFile;		// When showing a row of a file browser, are we in the path of the clipboard?
        } Clipboard;
      struct
        {
         char L[2 + 1];		// Left directory: 2 first chars
         char R[NAME_MAX + 1];	// Right directory: rest of chars
        } TmpPubDir;
      bool HiddenLevels[1 + BrwSiz_MAX_DIR_LEVELS];
      struct Asg_Assignment Asg;	// Data of assignment when browsing level 1 of an assignment zone.
				        // TODO: Remove from global?
      struct
        {
	 bool CreateZIP;
         char TmpDir[NAME_MAX + 1];
        } ZIP;
     } FileBrowser;	// Struct used for a file browser

   /* Cache */
   struct
     {
      struct
	{
	 long HieCod;
	 Lan_Language_t Language;
	 char CtyName[Cty_MAX_BYTES_NAME + 1];
         bool Valid;
	} CountryName;
      struct
	{
	 long HieCod;
	 char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];
	 char CtyName[Nam_MAX_BYTES_FULL_NAME + 1];
         bool Valid;
	} InstitutionShrtNameAndCty;
      struct
        {
	 long HieCod;
	 unsigned NumDpts;
          bool Valid;
       } NumDptsInIns;
      struct
        {
	 long HieCod;
	 unsigned Num;
         bool Valid;
        } NumNodesInHieLvl[Hie_NUM_LEVELS][Hie_NUM_LEVELS];
      struct
        {
	 unsigned NumUsrs;
	 bool Valid;
        } NumUsrsWhoDontClaimToBelongToAnyCty;
      struct
        {
	 unsigned NumUsrs;
	 bool Valid;
        } NumUsrsWhoClaimToBelongToAnotherCty;
      struct
        {
	 long HieCod;
	 unsigned NumUsrs;
	 bool Valid;
        } NumUsrsWhoClaimToBelongTo[Hie_NUM_LEVELS];
      struct
	{
	 long UsrCod;
	 bool IsSuperuser;
	 bool Valid;
	} UsrIsSuperuser;
      struct
	{
	 long UsrCod;
	 long HieCod;
	 bool CountOnlyAcceptedCourses;
	 Usr_Belong_t Belongs;
	 bool Valid;
	} UsrBelongsTo[Hie_NUM_LEVELS];
      struct
	{
	 long UsrCod;
	 Usr_Belong_t Belongs;
	 bool Valid;
	} UsrBelongsToCurrentCrs;
      struct
	{
	 long UsrCod;
	 bool Accepted;
	 bool Valid;
	} UsrHasAcceptedInCurrentCrs;
      struct
	{
	 long UsrCod;
	 bool SharesAnyOfMyCrs;
	 bool Valid;
	} UsrSharesAnyOfMyCrs;
      struct
	{
	 long GrpCod;
	 Usr_Belong_t IBelong;
	 bool Valid;
	} IBelongToGrp;
      struct
	{
	 long UsrCod;
	 bool Shares;
	 bool Valid;
	} UsrSharesAnyOfMyGrpsInCurrentCrs;
      struct
	{
	 long UsrCod;
	 long CrsCod;
	 Rol_Role_t Role;
	 bool Valid;
	} RoleUsrInCrs;
      struct
	{
	 Rol_Role_t Role;
	 bool Valid;
	} MyRoleInCurrentCrs;
      struct
	{
	 long PrjCod;
	 unsigned RolesInProject;
	 bool Valid;
	} MyRolesInProject;
      struct
        {
	 long UsrCod;
	 unsigned NumFollowing;
	 unsigned NumFollowers;
 	 bool Valid;
       } Follow;
     } Cache;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Gbl_InitializeGlobals (void);
void Gbl_Cleanup (void);

#endif
