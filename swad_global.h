// swad_global.h: global variables

#ifndef _SWAD_GBL
#define _SWAD_GBL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include <limits.h>		// For maximum values
#include <stdio.h>		// For FILE
#include <sys/time.h>		// For tz
#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_alert.h"
#include "swad_agenda.h"
#include "swad_assignment.h"
#include "swad_attendance.h"
#include "swad_banner.h"
#include "swad_centre.h"
#include "swad_classroom.h"
#include "swad_connected.h"
#include "swad_config.h"
#include "swad_country.h"
#include "swad_course.h"
#include "swad_cryptography.h"
#include "swad_department.h"
#include "swad_degree_type.h"
#include "swad_enrolment.h"
#include "swad_exam.h"
#include "swad_figure.h"
#include "swad_file.h"
#include "swad_file_browser.h"
#include "swad_forum.h"
#include "swad_game.h"
#include "swad_holiday.h"
#include "swad_icon.h"
#include "swad_indicator.h"
#include "swad_institution.h"
#include "swad_layout.h"
#include "swad_link.h"
#include "swad_mail.h"
#include "swad_mark.h"
#include "swad_media.h"
#include "swad_menu.h"
#include "swad_message.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_photo.h"
#include "swad_place.h"
#include "swad_plugin.h"
#include "swad_project.h"
#include "swad_record.h"
#include "swad_search.h"
#include "swad_session.h"
#include "swad_survey.h"
#include "swad_syllabus.h"
#include "swad_test.h"
#include "swad_timeline.h"
#include "swad_timetable.h"
#include "swad_web_service.h"

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

   struct soap *soap;	// gSOAP runtime environment

   struct
     {
      bool WritingHTMLStart;	// Used to avoid writing the HTML head when aborting program on error
      bool HTMLStartWritten;	// Used to avoid writing more than once the HTML head
      bool DivsEndWritten;	// Used to avoid writing more than once the HTML end
      bool HTMLEndWritten;	// Used to avoid writing more than once the HTML end
      unsigned NestedBox;
     } Layout;
   struct
     {
      bool DatabaseIsOpen;
      bool LockedTables;
     } DB;

   bool HiddenParamsInsertedIntoDB;	// If parameters are inserted in the database in this execution

   /* To compute execution time of the program */
   struct timeval tvStart,tvPageCreated,tvPageSent;
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
   char Title[Lay_MAX_BYTES_TITLE + 1];		// String for the help message in a link
   unsigned RowEvenOdd;	// To alternate row colors in listings
   char *ColorRows[2];
   const char *XMLPtr;
   struct
     {
      char FileName[PATH_MAX + 1];
     } HTMLOutput;
   struct
     {
      unsigned Num;		// Number of institutions
      struct Instit *Lst;	// List of institutions
      struct Instit EditingIns;
      Ins_Order_t SelectedOrder;
     } Inss;
   struct
     {
      unsigned Num;		// Number of centres
      struct Centre *Lst;	// List of centres
      struct Centre EditingCtr;
      Ctr_Order_t SelectedOrder;
     } Ctrs;
   struct
     {
      unsigned Num;		// Number of countries
      struct Country *Lst;	// List of countries
      struct Country EditingCty;
      Cty_Order_t SelectedOrder;
     } Ctys;
   struct
     {
      unsigned Num;		// Number of departments
      struct Department *Lst;	// List of departments
      struct Department EditingDpt;
      Dpt_Order_t SelectedOrder;
     } Dpts;
   struct
     {
      unsigned Num;		// Number of places
      struct Place *Lst;	// List of places
      struct Place EditingPlc;
      Plc_Order_t SelectedOrder;
     } Plcs;
   struct
     {
      unsigned Num;		// Number of classrooms
      struct Classroom *Lst;	// List of classrooms
      struct Classroom EditingCla;
      Cla_Order_t SelectedOrder;
     } Classrooms;
   struct
     {
      bool LstIsRead;		// Is the list already read from database, or it needs to be read?
      unsigned Num;		// Number of holidays
      struct Holiday *Lst;	// List of holidays
      struct Holiday EditingHld;
      Hld_Order_t SelectedOrder;
     } Hlds;
   struct
     {
      unsigned Num;		// Number of mail domains
      struct Mail *Lst;		// List of mail domains
      struct Mail EditingMai;
      Mai_DomainsOrder_t SelectedOrder;
     } Mails;
   struct
     {
      unsigned Num;			// Number of banners
      struct Banner *Lst;		// List of banners
      struct Banner EditingBan;
      long BanCodClicked;
     } Banners;
   struct
     {
      unsigned Num;		// Number of institutional links
      struct Link *Lst;		// List of institutional links
      struct Link EditingLnk;
     } Links;
   struct
     {
      struct
        {
         unsigned Num;			// Number of degree types
         struct DegreeType *Lst;	// List of degree types
        } DegTypes;
      struct DegreeType EditingDegTyp;
      struct Degree EditingDeg;
      struct Course EditingCrs;
     } Degs;
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
      struct Plugin EditingPlg;
     } Plugins;
   struct
     {
      bool IsWebService;	// Must generate HTML output (IsWebService==false) or SOAP-XML output (IsWebService==true)?
      long PlgCod;
      Svc_Function_t Function;
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
            bool Filled;	// My centres are already filled?
            unsigned Num;
            struct
              {
               long CtrCod;
               Rol_Role_t MaxRole;
              } Ctrs[Ctr_MAX_CENTRES_PER_USR];
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
      struct
        {
         char *List[Rol_NUM_ROLES];	// Lists of encrypted codes of users selected from a form
         bool Filled;			// If lists are already filled/readed
         Usr_ListUsrsOption_t Option;	// What option I have selected to do with these selected users
        } Selected;
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
      bool LstIsRead;	// Is the list already read from database, or it needs to be read?
      unsigned Num;	// Number of events
      long *LstAgdCods;	// List of agenda codes
      unsigned Past__FutureEvents;
      unsigned PrivatPublicEvents;
      unsigned HiddenVisiblEvents;
      Agd_Order_t SelectedOrder;
      long AgdCodToEdit;	// Used as parameter in contextual links
      unsigned CurrentPage;
     } Agenda;
   struct
     {
      Hie_Level_t Current;
      Hie_Level_t Default;
      unsigned Allowed;
     } Scope;
   struct
     {
      Hie_Level_t Level;	// Current level in the hierarchy: system, country, institution, centre, degree or course
      long Cod;			// Code of the current country, institution, centre, degree or course
      struct Country Cty;
      struct Instit Ins;
      struct DegreeType DegTyp;
      struct Centre Ctr;
      struct Degree Deg;
      struct
	{
	 struct Course Crs;
	 char PathPriv[PATH_MAX + 1];   // Absolute path to the private directory of the course
	 char PathRelPubl[PATH_MAX + 1];   // Relative path to the public directory of the course
	 char PathURLPubl[PATH_MAX + 1];   // Abolute URL to the public part of the course
	 struct
	   {
	    unsigned NumGrps;
	    struct GroupTypes GrpTypes;
	    struct GroupType GrpTyp;
	    long GrpCod;		// Group to be edited, removed...
	    char GrpName[Grp_MAX_BYTES_GROUP_NAME + 1];
	    long ClaCod;
	    unsigned MaxStudents;
	    bool Open;
	    bool FileZones;
	    struct ListCodGrps LstGrpsSel;
	    Grp_WhichGroups_t WhichGrps;	// Show my groups or all groups
	   } Grps;
	 struct
	   {
	    Inf_InfoType_t Type;
	    char URL[Cns_MAX_BYTES_WWW + 1];
	    bool MustBeRead[Inf_NUM_INFO_TYPES];	// Students must read info?
	    bool ShowMsgMustBeRead;
	   } Info;
	 struct
	   {
	    struct RecordField Field;
	    struct LstRecordFields LstFields;
	   } Records;
	 struct
	   {
	    long NotCod;		// Notice to be edited, removed... used as parameter
	    long HighlightNotCod;	// Notice code of a notice to be highlighted
	   } Notices;
	} Crs;
     } Hierarchy;
   struct
     {
      char PathDir[PATH_MAX + 1];
      unsigned NumItem;		// Item beeing edited
      unsigned ParamNumItem;	// Used as parameter in forms
      bool EditionIsActive;
      Syl_WhichSyllabus_t WhichSyllabus;
     } Syllabus;
   struct
     {
      struct Exa_ExamCodeAndDate *Lst;	// List of exam announcements
      unsigned NumExaAnns;	// Number of announcements of exam in the list
      long NewExaCod;		// New exam announcement just created
      long HighlightExaCod;	// Exam announcement to be highlighted
      char HighlightDate[4 + 2 + 2 + 1];	// Date with exam announcements to be highlighted (in YYYYMMDD format)
      long ExaCod;		// Used to put contextual icons
      const char *Anchor;	// Used to put contextual icons
      struct ExamData ExaDat;
     } ExamAnns;
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
	 char PathInTreeUntilFilFolLnk[PATH_MAX + 1];
	 char FullPathInTree[PATH_MAX + 1];
	} Priv;
      char NewFilFolLnkName[NAME_MAX + 1];
      char FilFolLnkName[NAME_MAX + 1];
      Brw_FileType_t FileType;
      unsigned Level;
      bool ICanEditFileOrFolder;	// Can I modify (remove, rename, create inside, etc.) a file or folder?
      Brw_IconViewEdit_t IconViewEdit;
      struct
        {
         Brw_FileBrowser_t FileBrowser;	// Type of the file browser
         long Cod;			// Code of the institution/centre/degree/course/group related to the file browser with the clipboard
	 long WorksUsrCod;		// User code of the user related to the works file browser with the clipboard
         char Path[PATH_MAX + 1];	// Complete path in the file browser
         char FileName[NAME_MAX + 1];	// File name, last part of complete path in the file browser
         unsigned Level;
         Brw_FileType_t FileType;	// Folder, file or link
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
      struct Assignment Asg;	// Data of assignment when browsing level 1 or an assignment zone
      struct
        {
	 bool CreateZIP;
         char TmpDir[NAME_MAX + 1];
        } ZIP;
     } FileBrowser;	// Struct used for a file browser
   struct
     {
      bool LstIsRead;		// Is the list already read from database, or it needs to be read?
      unsigned Num;		// Number of assignments
      long *LstAsgCods;		// List of assigment codes
      Dat_StartEndTime_t SelectedOrder;
      long AsgCodToEdit;	// Used as parameter in contextual links
      unsigned CurrentPage;
     } Asgs;
   struct
     {
      struct Prj_Filter Filter;
      bool LstIsRead;		// Is the list already read from database, or it needs to be read?
      unsigned Num;		// Number of projects
      long *LstPrjCods;		// List of project codes
      Prj_Order_t SelectedOrder;
      unsigned CurrentPage;
      long PrjCod;		// Current project
     } Prjs;
   struct
     {
      bool LstIsRead;	// Is the list already read from database, or it needs to be read?
      unsigned Num;	// Number of attendance events
      struct AttendanceEvent *Lst;	// List of attendance events
      Dat_StartEndTime_t SelectedOrder;
      long AttCod;
      bool ShowDetails;
      char *StrAttCodsSelected;
      long AttCodToEdit;	// Used as parameter in contextual links
      unsigned CurrentPage;
     } AttEvents;
   struct
     {
      TL_WhichUsrs_t WhichUsrs;
      long NotCod;		// Used as parameter about social note to be edited, removed...
      long PubCod;		// Used as parameter about social publishing to be edited, removed...
     } Timeline;
   struct
     {
      For_ForumSet_t ForumSet;
      For_Order_t ThreadsOrder;
      unsigned CurrentPageThrs;
      unsigned CurrentPagePsts;
      struct Forum ForumSelected;	// Forum type, location, thread and post
      long ThreadToMove;
     } Forum;
   struct
     {
      long AnnCod;
     } Announcements;
   struct
     {
      Msg_TypeOfMessages_t TypeOfMessages;
      unsigned NumMsgs;
      int MsgId;
      char Subject[Cns_MAX_BYTES_SUBJECT + 1];
      unsigned NumCourses;
      struct
        {
         long CrsCod;
         char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
        } Courses[Crs_MAX_COURSES_PER_USR];	// Distinct courses in my messages sent or received
      long FilterCrsCod;	// Show only messages sent from this course code
      char FilterCrsShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
      char FilterFromTo[Usr_MAX_BYTES_FULL_NAME + 1];		// Show only messages from/to these users
      char FilterContent[Msg_MAX_BYTES_FILTER_CONTENT + 1];	// Show only messages that match this content
      bool ShowOnlyUnreadMsgs;	// Show only unread messages (this option is applicable only for received messages)
      long ExpandedMsgCod;	// The current expanded message code
      struct
        {
         bool IsReply;			// Is the message I am editing a reply?
         long OriginalMsgCod;		// Original message code when I am editing a reply
        } Reply;
      bool ShowOnlyOneRecipient;	// Shown only a selected recipient or also other potential recipients?
      char FileNameMail[PATH_MAX + 1];
      FILE *FileMail;
      unsigned CurrentPage;
      long MsgCod;	// Used as parameter with message to be removed
     } Msg;
   struct
     {
      struct
        {
	 struct TT_Range Range;
	 unsigned HoursPerDay;		// From start hour to end hour
	 unsigned SecondsPerInterval;
	 unsigned IntervalsPerHour;
	 unsigned IntervalsPerDay;
	 unsigned IntervalsBeforeStartHour;
        } Config;
      TT_TimeTableType_t Type;
      TT_TimeTableView_t View;
      // bool Editing;
      unsigned Weekday;
      unsigned Interval;
      unsigned Column;
      TT_ClassType_t ClassType;
      unsigned DurationIntervals;
      char Info[TT_MAX_BYTES_INFO + 1];
      // char Place[TT_MAX_BYTES_PLACE + 1];
      long GrpCod;		// Group code (-1 if no group selected)
      struct
        {
	 bool PutIconEditCrsTT;
	 bool PutIconEditOfficeHours;
	 bool PutIconPrint;
        } ContextualIcons;
     } TimeTable;
   struct
     {
      struct Tst_Config Config;
      long QstCod;
      struct
        {
         unsigned Num;
         bool All;
         char *List;
         char Txt[Tst_MAX_TAGS_PER_QUESTION][Tst_MAX_BYTES_TAG + 1];
        } Tags;
      Tst_AnswerType_t AnswerType;
      unsigned NumQsts;
      long QstCodes[Tst_MAX_QUESTIONS_PER_TEST];	// Codes of the sent/received questions in a test
      char StrIndexesOneQst[Tst_MAX_QUESTIONS_PER_TEST][Tst_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
      char StrAnswersOneQst[Tst_MAX_QUESTIONS_PER_TEST][Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user
      bool AllowTeachers;	// Can teachers of this course see the test result?
      bool AllAnsTypes;
      struct
        {
         char *Text;
         size_t Length;
        } Stem, Feedback;
      struct Media Media;
      bool Shuffle;
      struct
        {
         unsigned NumOptions;
         char TF;
         struct
           {
            bool Correct;
            char *Text;
            char *Feedback;
            struct Media Media;
           } Options[Tst_MAX_OPTIONS_PER_QUESTION];
         long Integer;
         double FloatingPoint[2];
        } Answer;
      char ListAnsTypes[Tst_MAX_BYTES_LIST_ANSWER_TYPES + 1];
      Tst_QuestionsOrder_t SelectedOrder;
      struct
        {
         bool CreateXML;					// Create an XML file and Export questions into it?
         FILE *FileXML;
        } XML;
     } Test;
   struct
     {
      struct DateTime DateIni;	// TODO: Remove in future versions
      struct DateTime DateEnd;	// TODO: Remove in future versions
      time_t TimeUTC[2];
     } DateRange;
   struct
     {
      bool LstIsRead;	// Is the list already read from database, or it needs to be read?
      unsigned Num;	// Number of surveys
      long *LstGamCods;	// List of game codes
      Gam_Order_t SelectedOrder;
      long CurrentGamCod;	// Used as parameter in contextual links
      long CurrentQstCod;	// Used as parameter in contextual links
      unsigned CurrentPage;
      char *ListQuestions;
     } Games;
   struct
     {
      bool LstIsRead;	// Is the list already read from database, or it needs to be read?
      unsigned Num;	// Number of surveys
      long *LstSvyCods;	// List of survey codes
      Svy_Order_t SelectedOrder;
      long SvyCodToEdit;	// Used as parameter in contextual links
      long SvyQstCodToEdit;	// Used as parameter in contextual links
      unsigned CurrentPage;
     } Svys;
   struct
     {
      Fig_FigureType_t FigureType;
     } Figures;
   struct
     {
      Sta_ClicksGroupedBy_t ClicksGroupedBy;
      Sta_Role_t Role;
      Sta_CountType_t CountType;
      Act_Action_t NumAction;
      unsigned long FirstRow;
      unsigned long LastRow;
      unsigned long RowsPerPage;
      long DegTypCod;
      long DptCod;
      char StrIndicatorsSelected[Ind_MAX_SIZE_INDICATORS_SELECTED + 1];
      bool IndicatorsSelected[1 + Ind_NUM_INDICATORS];
      struct
        {
         Pho_AvgPhotoTypeOfAverage_t TypeOfAverage;
         Pho_HowComputePhotoSize_t HowComputePhotoSize;
         Pho_HowOrderDegrees_t HowOrderDegrees;
         int MaxStds;
         int MaxStdsWithPhoto;
         float MaxPercent;
        } DegPhotos;
     } Stat;
   struct
     {
      char *Str;
     } QR;

   /* Cache */
   struct
     {
      struct
	{
	 long CtyCod;
	 char CtyName[Cty_MAX_BYTES_NAME + 1];
	} CountryName;
      struct
	{
	 long InsCod;
	 char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
	} InstitutionShrtName;
      struct
	{
	 long InsCod;
	 char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
	 char CtyName[Hie_MAX_BYTES_FULL_NAME + 1];
	} InstitutionShrtNameAndCty;
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
	 Prj_RoleInProject_t RoleInProject;
	} MyRoleInProject;
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
