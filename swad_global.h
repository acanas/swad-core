// swad_global.h: global variables

#ifndef _SWAD_GBL
#define _SWAD_GBL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include "swad_assignment.h"
#include "swad_attendance.h"
#include "swad_banner.h"
#include "swad_centre.h"
#include "swad_connected.h"
#include "swad_config.h"
#include "swad_country.h"
#include "swad_course.h"
#include "swad_cryptography.h"
#include "swad_department.h"
#include "swad_file.h"
#include "swad_file_browser.h"
#include "swad_forum.h"
#include "swad_holiday.h"
#include "swad_icon.h"
#include "swad_import.h"
#include "swad_institution.h"
#include "swad_layout.h"
#include "swad_link.h"
#include "swad_mail.h"
#include "swad_mark.h"
#include "swad_message.h"
#include "swad_menu.h"
#include "swad_password.h"
#include "swad_photo.h"
#include "swad_place.h"
#include "swad_plugin.h"
#include "swad_record.h"
#include "swad_search.h"
#include "swad_session.h"
#include "swad_survey.h"
#include "swad_syllabus.h"
#include "swad_test.h"
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
      char DatabasePassword[Cfg_MAX_BYTES_DATABASE_PASSWORD+1];
      char SMTPPassword[Cfg_MAX_BYTES_SMTP_PASSWORD+1];
     } Config;
   struct Files F;
   MYSQL mysql;
   pid_t PID;	// PID of current process
   int NumForm;			// Number of form, used in form submit links
   int NumFormConnectedUsrs;	// Number of form in list of connected users, used in form submit links
   char FormId[32];		// Identifier string used in forms
   bool Error;
   bool GetMethod;	// Am I accessed using GET method?
   struct soap *soap;	// gSOAP runtime environment
   Act_Content_t ContentReceivedByCGI;	/* Content send by the form and received by the CGI:
						Act_CONTENT_NORM (if CONTENT_TYPE==text/plain) or
						Act_CONTENT_DATA (if CONTENT_TYPE==multipart/form-data) */
   char DelimiterString[1000];
   char DelimiterStringIncludingInitialRet[2+1000];
   struct
     {
      bool WritingHTMLStart;	// Used to avoid writing the HTML head when aborting program on error
      bool HTMLStartWritten;	// Used to avoid writing more than once the HTML head
      bool TablEndWritten;	// Used to avoid writing more than once the HTML end
      bool HTMLEndWritten;	// Used to avoid writing more than once the HTML end
     } Layout;
   struct
     {
      bool DatabaseIsOpen;
      bool LockedTables;
     } DB;

   bool HiddenParamsInsertedIntoDB;	// Indica si se ha insertado algún parameter in the database in esta ejecución

   /* To compute execution time of the program */
   struct timeval tvStart,tvPageCreated,tvPageSent;
   struct timezone tz;
   long TimeGenerationInMicroseconds;
   long TimeSendInMicroseconds;

   char IP[Cns_MAX_LENGTH_IP+1];
   char UniqueNameEncrypted[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1];	// Used for session id, temporary directory names, etc.

   struct
     {
      char ExternalUsrId[Cfg_MAX_LENGTH_IMPORTED_USR_ID+1];	// External user ID
      char ExternalSesId[Cfg_MAX_LENGTH_IMPORTED_SESSION_ID+1];	// External session ID
      Rol_Role_t ExternalRole;
     } Imported;
   struct
     {
      Lay_Layout_t Layout;
      The_Theme_t Theme;
      char PathTheme[PATH_MAX+1];
      Txt_Language_t Language;
      unsigned SideCols;
      Ico_IconSet_t IconSet;
      char PathIconSet[PATH_MAX+1];
      char IconsURL[PATH_MAX+1];
      Mnu_Menu_t Menu;
     } Prefs;

   struct
     {
      unsigned NumSessions;
      bool IsOpen;
      bool HasBeenDisconnected;
      char Id[Ses_LENGTH_SESSION_ID+1];
      long UsrCod;
     } Session;

   bool YearOK;
   Act_Tab_t CurrentTab;
   Act_Action_t CurrentAct;
   time_t TimeStartExecution;
   struct tm *tblock;
   struct DateTime Now;
   struct Date Yesterday;
   char Message[Lay_MAX_BYTES_ALERT];	// String for alerts
   char Title[Lay_MAX_BYTES_TITLE];	// String for the help message in a link
   int RowEvenOdd;	// To alternate row colors in listings
   char *ColorRows[2];
   const char *XMLPtr;
   struct
     {
      char FileName[PATH_MAX+1];
     } HTMLOutput;
   struct
     {
      unsigned Num;		// Number of institutions
      struct Institution *Lst;	// List of institutions
      struct Institution EditingIns;
      Ins_InssOrderType_t SelectedOrderType;
     } Inss;
   struct
     {
      unsigned Num;		// Number of centres
      struct Centre *Lst;	// List of centres
      struct Centre EditingCtr;
      tCtrsOrderType SelectedOrderType;
     } Ctrs;
   struct
     {
      unsigned Num;		// Number of countries
      struct Country *Lst;	// List of countries
      struct Country EditingCty;
      Cty_CtysOrderType_t SelectedOrderType;
     } Ctys;
   struct
     {
      unsigned Num;		// Number of departments
      struct Department *Lst;	// List of departments
      struct Department EditingDpt;
      tDptsOrderType SelectedOrderType;
     } Dpts;
   struct
     {
      unsigned Num;		// Number of places
      struct Place *Lst;	// List of places
      struct Place EditingPlc;
      Plc_PlcsOrderType_t SelectedOrderType;
     } Plcs;
   struct
     {
      bool LstIsRead;		// Is the list already read from database, or it needs to be read?
      unsigned Num;		// Number of holidays
      struct Holiday *Lst;	// List of holidays
      struct Holiday EditingHld;
      Hld_OrderType_t SelectedOrderType;
     } Hlds;
   struct
     {
      unsigned Num;		// Number of mail domains
      struct Mail *Lst;		// List of mail domains
      struct Mail EditingMai;
      Mai_MailDomainsOrderType_t SelectedOrderType;
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
      struct
        {
         unsigned Num;			// Number of degrees
         struct Degree *Lst;		// List of degrees
        } AllDegs;
      struct DegreeType EditingDegTyp;
      struct Degree EditingDeg;
      struct Course EditingCrs;
     } Degs;
   struct
     {
      Sch_WhatToSearch_t WhatToSearch;
      char Str[Sch_MAX_LENGTH_STRING_TO_FIND+1];
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
         unsigned AvailableRoles;
         Rol_Role_t RoleFromSession;
         Rol_Role_t LoggedRole;
         Rol_Role_t LoggedRoleBeforeCloseSession;
         Rol_Role_t MaxRole;
	 char UsrIdLogin[Usr_MAX_BYTES_USR_LOGIN+1];	// String to store the ID, nickname or e-mail entered in the user's login
         char LoginPlainPassword[Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
         char LoginEncryptedPassword[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];
         char PendingPassword[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];
	 char PathDir[PATH_MAX+1];
	 bool Logged;
         bool IBelongToCurrentIns;
         bool IBelongToCurrentCtr;
         bool IBelongToCurrentDeg;
         bool IBelongToCurrentCrs;
         bool MyPhotoExists;
         unsigned NumAccWithoutPhoto;
         char PhotoURL[PATH_MAX+1];
         time_t TimeLastAccToThisFileBrowser;
         struct
           {
            bool Filled;	// My institutions are already filled?
            unsigned Num;
            struct
              {
               long InsCod;
               Rol_Role_t MaxRole;
              } Inss[Ins_MAX_INSTITUTIONS_PER_USR];
           } MyInstitutions;
         struct
           {
            bool Filled;	// My centres are already filled?
            unsigned Num;
            struct
              {
               long CtrCod;
               Rol_Role_t MaxRole;
              } Ctrs[Ctr_MAX_CENTRES_PER_USR];
           } MyCentres;
         struct
           {
            bool Filled;	// My degrees are already filled?
            unsigned Num;
            struct
              {
               long DegCod;
               Rol_Role_t MaxRole;
              } Degs[Deg_MAX_DEGREES_PER_USR];
           } MyDegrees;
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
           } MyCourses;
         struct
           {
            unsigned Num;
            struct Degree *Lst;
           } MyAdminDegs;	// List of degrees administrated by me
	 Usr_ShowUsrsType_t ListType;	// My preference about user's list type
	} Me;		// The user logged
      struct
	{
	 struct UsrData UsrDat;
	} Other;		// Another user, used for example to register in / remove from a course
      struct ListUsers LstGsts;
      struct ListUsers LstStds;
      struct ListUsers LstTchs;
      struct ListUsers LstAdms;
      struct
        {
         char *All;
         char *Std;
         char *Tch;
        } Select;	// Lists of ID of users from a form
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
         Con_WhereToShow_t WhereToShow;
         unsigned long TimeToRefreshInMs;
         unsigned NumTchs;
         unsigned NumStds;
         unsigned SexTchs;
         unsigned SexStds;
         unsigned NumUsr;
         unsigned NumUsrs;
         unsigned NumUsrsToList;
         struct
           {
            long UsrCod;
            bool ThisCrs;
            unsigned Seconds;
           } Lst[Cfg_MAX_CONNECTED_SHOWN];
        } Connected;
      char FileNamePhoto[NAME_MAX+1];	// File name (with no path and no .jpg) of the temporary file with the selected face
      bool Error;	// To signal that an error has happened
     } Usrs;
   struct
     {
      Sco_Scope_t Current;
      Sco_Scope_t Default;
      unsigned Allowed;
     } Scope;
   struct
     {
      struct Country Cty;
     } CurrentCty;
   struct
     {
      struct Institution Ins;
     } CurrentIns;
   struct
     {
      struct DegreeType DegTyp;
     } CurrentDegTyp;
   struct
     {
      struct Centre Ctr;
      struct Degree *LstDegs;	// List of degrees of current centre
     } CurrentCtr;
   struct
     {
      struct Degree Deg;
     } CurrentDeg;
   struct
     {
      struct Course Crs;
      char PathPriv[PATH_MAX+1];   // Absolute path to the private directory of the course
      char PathRelPubl[PATH_MAX+1];   // Relative path to the public directory of the course
      char PathURLPubl[PATH_MAX+1];   // Abolute URL to the public part of the course
      struct
        {
         unsigned NumGrps;
         struct GroupTypes GrpTypes;
         struct GroupType GrpTyp;
         long GrpCod;
         char GrpName[MAX_LENGTH_GROUP_NAME+1];
         int MaxStudents;
         bool Open;
         bool FileZones;
         struct ListCodGrps LstGrpsSel;
         Grp_WhichGroups_t WhichGrps;	// Show my groups or all groups
        } Grps;
      struct
	{
	 struct
	   {
	    char URL[Cns_MAX_BYTES_URL+1];	// URL
	    char PathRelFileURL [PATH_MAX+1];	// Relative path to a text file of texto containing the URL
	    char PathRelFileZIP [PATH_MAX+1];	// Relative path to a file ZIP with the complete web page
	    char PathRelWebPage [PATH_MAX+1];	// Relative path to a public directory with a complete web page
	    char URLWebPage [PATH_MAX+1];	// URL of a web page
	   } Links[Inf_NUM_INFO_TYPES];
	 char URL[Cns_MAX_BYTES_URL+1];
         bool MustBeRead[Inf_NUM_INFO_TYPES];	// Students must read info?
         bool ShowMsgMustBeRead;
	} Info;
      struct
	{
	 char PathDir[PATH_MAX+1];
	 unsigned NumItem;
	 bool EditionIsActive;
	 Syl_WhichSyllabus_t WhichSyllabus;
	} Syllabus;
      struct
        {
         struct RecordField Field;
         struct LstRecordFields LstFields;
        } Records;
      struct
        {
         long HighlightNotCod;	// Notice code of a notice to be highlighted
        } Notices;
     } CurrentCrs;
   struct
     {
      long CrsCod;
      char CrsFullName[Cns_MAX_BYTES_STRING+1];
      unsigned Year; // Number of year (0 (Optional), 1, 2, 3, 4, 5, 6) in the degree
      char Session[Cns_MAX_BYTES_STRING+1];	// Exam session is june, september, etc.
      struct Date CallDate;
      struct Date ExamDate;
      struct Hour StartTime;
      struct Hour Duration;
      char *Place;
      char *Mode;
      char *Structure;
      char *DocRequired;
      char *MatRequired;
      char *MatAllowed;
      char *OtherInfo;
     } ExamAnnouncement;
   struct
     {
      struct Date *Lst;		// List of dates of announcements of exams
      unsigned NumExamAnnounc;	// Number of announcements of exam in the list
     } LstExamAnnouncements;
   struct
     {
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
	 char PathAboveRootFolder[PATH_MAX+1];
	 char PathRootFolder[PATH_MAX+1];
	 char PathInTreeExceptFileOrFolder[PATH_MAX+1];
	 char FullPathInTree[PATH_MAX+1];
	} Priv;
      char NewFilFolLnkName[NAME_MAX+1];	// TODO: It should be PATH_MAX (for URL in links)
      char FilFolLnkName[NAME_MAX+1];
      Brw_FileType_t FileType;
      unsigned Level;
      bool ICanRemoveFileOrFolder;	// Can I remove a file or folder?
      bool ICanEditFileOrFolder;	// Can I modify (rename, create inside, etc.) a file or folder?
      struct
        {
         Brw_FileBrowser_t FileBrowser;	// Type of the file browser
         long InsCod;			// Code of the institution related to the file browser with the clipboard
         long CtrCod;			// Code of the centre related to the file browser with the clipboard
         long DegCod;			// Code of the degree related to the file browser with the clipboard
         long CrsCod;			// Code of the course related to the file browser with the clipboard
	 long GrpCod;			// Code of the group related to the file browser with the clipboard
	 long WorksUsrCod;		// User code of the user related to the works file browser with the clipboard
         char Path[PATH_MAX+1];		// Complete path in the file browser
         char FileName[NAME_MAX+1];	// File name, last part of complete path in the file browser
         Brw_FileType_t FileType;	// Folder, file or link
         bool IsThisTree;		// When showing a file browser, is it that corresponding to the clipboard?
         bool IsThisFile;		// When showing a row of a file browser, are we in the path of the clipboard?
        } Clipboard;
      char TmpPubDir[NAME_MAX+1];
      bool HiddenLevels[1+Brw_MAX_DIR_LEVELS];
      const char *TxtStyle;
      const char *InputStyle;
      struct Assignment Asg;	// Data of assignment when browsing level 1 or an assignment zone
      struct
        {
	 bool CreateZIP;
         char TmpDir[NAME_MAX+1];
        } ZIP;
     } FileBrowser;	// Struct used for a file browser
   struct
     {
      bool LstIsRead;	// Is the list already read from database, or it needs to be read?
      unsigned Num;	// Number of assignments
      long *LstAsgCods;	// List of assigment codes
      tAsgsOrderType SelectedOrderType;
     } Asgs;
   struct
     {
      bool LstIsRead;	// Is the list already read from database, or it needs to be read?
      unsigned Num;	// Number of attendance events
      struct AttendanceEvent *Lst;	// List of attendance events
      Att_EventsOrderType_t SelectedOrderType;
      long AttCod;
     } AttEvents;
   struct
     {
      For_WhichForums_t WhichForums;
      For_ForumType_t ForumType;
      struct Institution Ins;
      struct Centre Ctr;
      struct Degree Deg;
      struct Course Crs;
      int Level;
      For_ForumOrderType_t SelectedOrderType;
      char PathRelOld[PATH_MAX+1];
      long ThreadToMove;
     } Forum;
   struct
     {
      int MsgId;
      char Subject[Cns_MAX_BYTES_SUBJECT+1];
      unsigned NumCourses;
      struct
        {
         long CrsCod;
         char ShortName[Crs_MAX_LENGTH_COURSE_SHORT_NAME+1];
        } Courses[Crs_MAX_COURSES_PER_USR];	// Distinct courses in my messages sent or received
      long FilterCrsCod; 		// Show only messages sent from this course code
      char FilterCrsShortName[Crs_MAX_LENGTH_COURSE_SHORT_NAME+1];
      char FilterFromTo[Usr_MAX_LENGTH_USR_NAME_OR_SURNAME*3+1];	// Show only messages from/to these users
      char FilterContent[Msg_MAX_LENGTH_FILTER_CONTENT+1];		// Show only messages that match this content
      bool ShowOnlyUnreadMsgs;	// Show only unread messages (this option is applicable only for received messages)
      bool IsReply;		// Is the message I am editing a reply to a received message?
      long RepliedMsgCod;	// Original message code when I am editing a reply to a received message
      long ExpandedMsgCod;	// The current expanded message code
      char FileNameMail[PATH_MAX+1];
      FILE *FileMail;
     } Msg;
   struct
     {
      unsigned CurrentPage;
     } Pag;
   // struct
   //   {
   //    char WindowName[256];
   //   } Chat;
   struct
     {
      unsigned Day,Hour,Column;
      TT_ClassType_t ClassType;
      unsigned Duration;
      char Place[TT_MAX_BYTES_PLACE+1];
      char Group[TT_MAX_BYTES_GROUP+1];
      long GrpCod;		// Group code (-1 if no group selected)
     } TimeTable;
   struct
     {
      struct Tst_Config Config;
      long QstCod;
      Tst_AnswerType_t AnswerType;
      char TagText[Tst_MAX_TAGS_PER_QUESTION][Tst_MAX_BYTES_TAG+1];
      unsigned NumQsts;
      long QstCodes[Tst_MAX_QUESTIONS_PER_EXAM];	// Codes of the sent/received questions in a test
      char StrIndexesOneQst[Tst_MAX_QUESTIONS_PER_EXAM][Tst_MAX_SIZE_INDEXES_ONE_QST+1];	// 0 1 2 3, 3 0 2 1, etc.
      char StrAnswersOneQst[Tst_MAX_QUESTIONS_PER_EXAM][Tst_MAX_SIZE_ANSWERS_ONE_QST+1];	// Answers selected by user
      bool AllowTeachers;	// Can teachers of this course see the exam?
      bool AllTags;
      bool AllAnsTypes;
      unsigned NumTags;
      char *TagsList;
      struct
        {
         char *Text;
         size_t Length;
        } Stem, Feedback;
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
           } Options[Tst_MAX_OPTIONS_PER_QUESTION];
         long Integer;
         double FloatingPoint[2];
        } Answer;
      char ListAnsTypes[Tst_MAX_BYTES_LIST_ANSWER_TYPES+1];
      Tst_QuestionsOrder_t SelectedOrderType;
      struct
        {
         bool CreateXML;					// Create an XML file and Export questions into it?
         FILE *FileXML;
        } XML;
     } Test;
   struct
     {
      struct Date DateIni;
      struct Date DateEnd;
     } DateRange;
   struct
     {
      bool LstIsRead;	// Is the list already read from database, or it needs to be read?
      unsigned Num;	// Number of surveys
      long *LstSvyCods;	// List of survey codes
      tSvysOrderType SelectedOrderType;
     } Svys;
   struct
     {
      Sta_ClicksStatType_t ClicksStatType;
      Sta_UseStatType_t UseStatType;
      Sta_Role_t Role;
      Sta_CountType_t CountType;
      Act_Action_t NumAction;
      unsigned long FirstRow;
      unsigned long LastRow;
      unsigned long RowsPerPage;
      long DegTypCod;
      long DptCod;
      long NumIndicators;
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
   char QueryString[Cns_MAX_LENGTH_ARGS_SENT_TO_CGI+1]; // String with the arguments sent to the CGI
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Gbl_InitializeGlobals (void);
void Gbl_Cleanup (void);

#endif
