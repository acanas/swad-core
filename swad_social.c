// swad_social.c: social networking

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For PATH_MAX
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions
#include <sys/types.h>		// For time_t

#include "swad_constant.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_notice.h"
#include "swad_parameter.h"
#include "swad_social.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Soc_MAX_BYTES_SUMMARY 100

static const Act_Action_t Soc_DefaultActions[Soc_NUM_SOCIAL_EVENTS] =
  {
   ActUnk,		// Soc_EVENT_UNKNOWN

   /* Institution tab */
   ActSeeDocIns,	// Soc_EVENT_INS_DOC_PUB_FILE
   ActAdmComIns,	// Soc_EVENT_INS_SHA_PUB_FILE

   /* Centre tab */
   ActSeeDocCtr,	// Soc_EVENT_CTR_DOC_PUB_FILE
   ActAdmComCtr,	// Soc_EVENT_CTR_SHA_PUB_FILE

   /* Degree tab */
   ActSeeDocDeg,	// Soc_EVENT_DEG_DOC_PUB_FILE
   ActAdmComDeg,	// Soc_EVENT_DEG_SHA_PUB_FILE

   /* Course tab */
   ActSeeDocCrs,	// Soc_EVENT_CRS_DOC_PUB_FILE
   ActAdmShaCrs,	// Soc_EVENT_CRS_SHA_PUB_FILE

   /* Assessment tab */
   ActSeeExaAnn,	// Soc_EVENT_EXAM_ANNOUNCEMENT

   /* Users tab */

   /* Social tab */
   ActSeeSocAct,	// Soc_EVENT_SOCIAL_POST (action not used)

   /* Messages tab */
   ActShoNot,		// Soc_EVENT_NOTICE
   ActSeeFor,		// Soc_EVENT_FORUM_POST

   /* Statistics tab */

   /* Profile tab */

  };

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Soc_PutLinkToWriteANewPost (void);
static void Soc_GetAndWriteSocialPost (long PstCod);

static unsigned long Soc_ShowTimeline (const char *Query);
static Soc_SocialEvent_t Soc_GetSocialEventFromDB (const char *Str);
static void Soc_WriteEventDate (time_t TimeUTC);
static void Soc_StartFormGoToAction (Soc_SocialEvent_t SocialEvent,
                                     long CrsCod,long Cod);
static void Soc_GetEventSummary (Soc_SocialEvent_t SocialEvent,long Cod,
                                 char *SummaryStr,unsigned MaxChars);

static void Soc_PutFormToRemoveSocialEvent (long SocCod);
static void Soc_PutHiddenParamSocCod (long SocCod);

/*****************************************************************************/
/*********************** Show social activity (timeline) *********************/
/*****************************************************************************/

static void Soc_PutLinkToWriteANewPost (void)
  {
   extern const char *Txt_New_comment;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Lay_PutContextualLink (ActReqSocPst,NULL,"write64x64.gif",
			  Txt_New_comment,Txt_New_comment);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Form to write a new public comment ***********************/
/*****************************************************************************/

void Soc_FormSocialPost (void)
  {
   extern const char *Txt_New_comment;
   extern const char *Txt_Send_comment;

   /***** Form to write a new public comment *****/
   /* Start frame */
   Lay_StartRoundFrame ("560px",Txt_New_comment);

   /* Start form to write the post */
   Act_FormStart (ActRcvSocPst);

   /* Content of new post */
   fprintf (Gbl.F.Out,"<textarea name=\"Content\" cols=\"50\" rows=\"5\">"
		      "</textarea>");

   /* Send button */
   Lay_PutCreateButton (Txt_Send_comment);

   /* End form */
   Act_FormEnd ();

   /* End frame */
   Lay_EndRoundFrame ();

   /***** Write current timeline *****/
   Soc_ShowFollowingTimeline ();
  }

/*****************************************************************************/
/******************* Receive and store a new public post *********************/
/*****************************************************************************/

void Soc_ReceiveSocialPost (void)
  {
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   char Query[128+Cns_MAX_BYTES_LONG_TEXT];
   long PstCod;

   /***** Get and store new post *****/
   /* Get the content of the post */
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /* Insert post content in the database */
   sprintf (Query,"INSERT INTO social_post (Content) VALUES ('%s')",
            Content);
   PstCod = DB_QueryINSERTandReturnCode (Query,"can not create post");

   /* Insert post in social events */
   Soc_StoreSocialEvent (Soc_EVENT_SOCIAL_POST,PstCod);

   /***** Write current timeline *****/
   Soc_ShowFollowingTimeline ();
  }

/*****************************************************************************/
/***************** Get from database and write public post *******************/
/*****************************************************************************/

static void Soc_GetAndWriteSocialPost (long PstCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Get social post from database *****/
   sprintf (Query,"SELECT Content FROM social_post WHERE PstCod='%ld'",
            PstCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the content of a social post");

   /***** Result should have a unique row *****/
   if (NumRows == 1)
     {
      /***** Get number of rows *****/
      row = mysql_fetch_row (mysql_res);

      /****** Get content (row[0]) *****/
      strncpy (Content,row[0],Cns_MAX_BYTES_LONG_TEXT);
      Content[Cns_MAX_BYTES_LONG_TEXT] = '\0';
     }
   else
      Content[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Write content *****/
   Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
  }

/*****************************************************************************/
/*********** Show social activity (timeline) of a selected user **************/
/*****************************************************************************/

void Soc_ShowUsrTimeline (long UsrCod)
  {
   char Query[512];

   /***** Build query to show timeline including the users I am following *****/
   sprintf (Query,"SELECT SocCod,SocialEvent,UsrCod,"
	          "CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
	          "Cod,UNIX_TIMESTAMP(TimeEvent)"
                  " FROM social"
                  " WHERE UsrCod='%ld'"
                  " ORDER BY SocCod DESC LIMIT 10",
            UsrCod);

   /***** Show timeline *****/
   Soc_ShowTimeline (Query);
  }

/*****************************************************************************/
/***** Show social activity (timeline) including all the users I follow ******/
/*****************************************************************************/

void Soc_ShowFollowingTimeline (void)
  {
   char Query[512];

   /***** Link to write a new social post *****/
   if (Gbl.CurrentAct != ActReqSocPst)
      Soc_PutLinkToWriteANewPost ();

   /***** Show warning if I do not follow anyone *****/
   if (!Fol_GetNumFollowing (Gbl.Usrs.Me.UsrDat.UsrCod))
      Lay_ShowAlert (Lay_INFO,"Usted no sigue a ning&uacute;n usuario.");	// Need translation!!!

   /***** Build query to show timeline including the users I am following *****/
   sprintf (Query,"SELECT SocCod,SocialEvent,UsrCod,"
		  "CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
		  "Cod,UNIX_TIMESTAMP(TimeEvent)"
		  " FROM social"
		  " WHERE UsrCod IN"
		  " (SELECT '%ld'"
		  " UNION"
		  " SELECT FollowedCod FROM usr_follow WHERE FollowerCod='%ld')"
		  " ORDER BY SocCod DESC LIMIT 10",
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show timeline *****/
   if (!Soc_ShowTimeline (Query))
      Lay_ShowAlert (Lay_INFO,"No hay actividad p&uacute;blica.");	// Need translation!!!
  }

/*****************************************************************************/
/*********************** Show social activity (timeline) *********************/
/*****************************************************************************/

static unsigned long Soc_ShowTimeline (const char *Query)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Public_activity;
   extern const char *Txt_SOCIAL_EVENT[Soc_NUM_SOCIAL_EVENTS];
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   extern const char *Txt_Country;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumEvents;
   unsigned long NumEvent;
   long SocCod;
   Soc_SocialEvent_t SocialEvent;
   struct UsrData UsrDat;
   struct Country Cty;
   struct Institution Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   long Cod;
   char ForumName[512];
   time_t DateTimeUTC;	// Date-time of the event
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char SummaryStr[Cns_MAX_BYTES_TEXT+1];

   /***** Get timeline from database *****/
   NumEvents = DB_QuerySELECT (Query,&mysql_res,"can not get your notifications");

   /***** List my timeline *****/
   if (NumEvents)	// Events found
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List start *****/
      Lay_StartRoundFrame ("560px",Txt_Public_activity);
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");

      /***** List events one by one *****/
      for (NumEvent = 0;
	   NumEvent < NumEvents;
	   NumEvent++)
	{
         /***** Get next social event *****/
         row = mysql_fetch_row (mysql_res);

         /* Get social code (row[0]) */
         SocCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get event type (row[1]) */
         SocialEvent = Soc_GetSocialEventFromDB ((const char *) row[1]);

         /* Get (from) user code (row[2]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[2]);
         Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);		// Get user's data from the database

         /* Get country code (row[3]) */
         Cty.CtyCod = Str_ConvertStrCodToLongCod (row[3]);
         Cty_GetDataOfCountryByCod (&Cty,Cty_GET_BASIC_DATA);

         /* Get institution code (row[4]) */
         Ins.InsCod = Str_ConvertStrCodToLongCod (row[4]);
         Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

          /* Get centre code (row[5]) */
         Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[5]);
         Ctr_GetDataOfCentreByCod (&Ctr);

         /* Get degree code (row[6]) */
         Deg.DegCod = Str_ConvertStrCodToLongCod (row[6]);
         Deg_GetDataOfDegreeByCod (&Deg);

         /* Get course code (row[7]) */
         Crs.CrsCod = Str_ConvertStrCodToLongCod (row[7]);
         Crs_GetDataOfCourseByCod (&Crs);

         /* Get file/post... code (row[8]) */
         Cod = Str_ConvertStrCodToLongCod (row[8]);

         /* Get forum type of the post */
         if (SocialEvent == Soc_EVENT_FORUM_POST)
           {
            Gbl.Forum.ForumType = For_GetForumTypeOfAPost (Cod);
            For_SetForumName (Gbl.Forum.ForumType,
        	              &Ins,
        	              &Ctr,
        	              &Deg,
        	              &Crs,
        	              ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
            Gbl.Forum.Ins.InsCod = Ins.InsCod;
            Gbl.Forum.Ctr.CtrCod = Ctr.CtrCod;
            Gbl.Forum.Deg.DegCod = Deg.DegCod;
            Gbl.Forum.Crs.CrsCod = Crs.CrsCod;
           }

         /* Get time of the event (row[9]) */
         DateTimeUTC = Dat_GetUNIXTimeFromStr (row[9]);

         /***** Write row for this social event *****/
         fprintf (Gbl.F.Out,"<li>");

	 /* Left: write author's photo */
         fprintf (Gbl.F.Out,"<div class=\"SOCIAL_LEFT_PHOTO\">");
	 ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
	 Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					       NULL,
			   "PHOTO60x80",Pho_ZOOM);
         fprintf (Gbl.F.Out,"</div>");

         /* Right: author's name, time and summary */
         fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_CONTAINER\">");

	 /* Write author's full name and nickname */
         Str_LimitLengthHTMLStr (UsrDat.FullName,20);
         fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_AUTHOR\">"
                            "<span class=\"DAT_N_BOLD\">%s</span>"
                            "<span class=\"DAT_LIGHT\"> @%s</span>"
                            "</div>",
                  UsrDat.FullName,UsrDat.Nickname);

         /* Write date and time */
         Soc_WriteEventDate (DateTimeUTC);

         if (SocialEvent == Soc_EVENT_SOCIAL_POST)
           {
            /* Write post content */
	    fprintf (Gbl.F.Out,"<div class=\"DAT\">");
	    Soc_GetAndWriteSocialPost (Cod);
	    fprintf (Gbl.F.Out,"</div>");

            /* Write form to remove this event */
	    Soc_PutFormToRemoveSocialEvent (SocCod);
           }
         else
           {
            /* Write event type and location */
	    fprintf (Gbl.F.Out,"<div>");
	    Soc_StartFormGoToAction (SocialEvent,Crs.CrsCod,Cod);
	    Act_LinkFormSubmit (Txt_SOCIAL_EVENT[SocialEvent],
	                        The_ClassForm[Gbl.Prefs.Theme]);
	    fprintf (Gbl.F.Out,"%s</a>",
		     Txt_SOCIAL_EVENT[SocialEvent]);
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</div>");

	    if (SocialEvent == Soc_EVENT_FORUM_POST)
	       fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
	                Txt_Forum,ForumName);
	    else if (Crs.CrsCod > 0)
	       fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
	                Txt_Course,Crs.ShortName);
	    else if (Deg.DegCod > 0)
	       fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
	                Txt_Degree,Deg.ShortName);
	    else if (Ctr.CtrCod > 0)
	       fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
	                Txt_Centre,Ctr.ShortName);
	    else if (Ins.InsCod > 0)
	       fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
	                Txt_Institution,Ins.ShortName);
	    else if (Cty.CtyCod > 0)
	       fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
	                Txt_Country,Cty.Name[Gbl.Prefs.Language]);

	    /* Write content of the event */
	    Soc_GetEventSummary (SocialEvent,Cod,
				 SummaryStr,Soc_MAX_BYTES_SUMMARY);
	    fprintf (Gbl.F.Out,"<div class=\"DAT\">%s</div>",SummaryStr);
           }

         /* End of right part */
         fprintf (Gbl.F.Out,"</div>");

         /* End of list element */
         fprintf (Gbl.F.Out,"</li>");
        }

      /***** List end *****/
      fprintf (Gbl.F.Out,"</ul>");
      Lay_EndRoundFrame ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumEvents;
  }

/*****************************************************************************/
/****** Get social event type from string number coming from database ********/
/*****************************************************************************/

static Soc_SocialEvent_t Soc_GetSocialEventFromDB (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Soc_NUM_SOCIAL_EVENTS)
         return (Soc_SocialEvent_t) UnsignedNum;

   return Soc_EVENT_UNKNOWN;
  }

/*****************************************************************************/
/**************** Write the date of creation of a social event ***************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

static void Soc_WriteEventDate (time_t TimeUTC)
  {
   extern const char *Txt_Today;
   static unsigned UniqueId = 0;

   UniqueId++;

   /***** Start cell *****/
   fprintf (Gbl.F.Out,"<div id=\"date_%u\" class=\"SOCIAL_RIGHT_TIME DAT_LIGHT\""
	              " style=\"display:inline-block;\">",
            UniqueId);

   /***** Write date and time *****/
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateTimeFromUTC('date_%u',%ld,'&nbsp;','%s');"
                      "</script>",
            UniqueId,(long) TimeUTC,Txt_Today);

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</div>");
  }


/*****************************************************************************/
/*********** Put form to go to an action depending on the event **************/
/*****************************************************************************/

static void Soc_StartFormGoToAction (Soc_SocialEvent_t SocialEvent,
                                     long CrsCod,long Cod)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   struct FileMetadata FileMetadata;
   long GrpCod = -1L;
   char PathUntilFileName[PATH_MAX+1];
   char FileName[NAME_MAX+1];
   Act_Action_t Action = ActUnk;				// Initialized to avoid warning

   /***** Parameters depending on the type of event *****/
   switch (SocialEvent)
     {
      case Soc_EVENT_INS_DOC_PUB_FILE:
      case Soc_EVENT_INS_SHA_PUB_FILE:
      case Soc_EVENT_CTR_DOC_PUB_FILE:
      case Soc_EVENT_CTR_SHA_PUB_FILE:
      case Soc_EVENT_DEG_DOC_PUB_FILE:
      case Soc_EVENT_DEG_SHA_PUB_FILE:
      case Soc_EVENT_CRS_DOC_PUB_FILE:
      case Soc_EVENT_CRS_SHA_PUB_FILE:
	 if (Cod > 0)	// File code
	   {
	    FileMetadata.FilCod = Cod;
            Brw_GetFileMetadataByCod (&FileMetadata);
            Brw_GetCrsGrpFromFileMetadata (FileMetadata.FileBrowser,FileMetadata.Cod,&CrsCod,&GrpCod);
	    Str_SplitFullPathIntoPathAndFileName (FileMetadata.Path,
						  PathUntilFileName,
						  FileName);
	   }
	 switch (SocialEvent)
	   {
	    case Soc_EVENT_INS_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocIns : ActSeeDocIns;
	       break;
	    case Soc_EVENT_INS_SHA_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatShaIns : ActAdmComIns;
	       break;
	    case Soc_EVENT_CTR_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocCtr : ActSeeDocCtr;
	       break;
	    case Soc_EVENT_CTR_SHA_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatShaCtr : ActAdmComCtr;
	       break;
	    case Soc_EVENT_DEG_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocDeg : ActSeeDocDeg;
	       break;
	    case Soc_EVENT_DEG_SHA_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatShaDeg : ActAdmComDeg;
	       break;
	    case Soc_EVENT_CRS_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocCrs : ActSeeDocCrs;
	       break;
	    case Soc_EVENT_CRS_SHA_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatShaCrs : ActAdmShaCrs;
	       break;
	    default:	// Not aplicable here
	       break;
	   }
         Act_FormStart (Action);
	 Grp_PutParamGrpCod (-1L);
	 if (Cod > 0)	// File code
	    Brw_PutParamsPathAndFile (Brw_IS_FILE,PathUntilFileName,FileName);
	 break;
      case Soc_EVENT_NOTICE:
         Act_FormStart (Soc_DefaultActions[SocialEvent]);
	 Not_PutHiddenParamNotCod (Cod);
	 break;
      case Soc_EVENT_FORUM_POST:
	 Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
	 For_PutAllHiddenParamsForum ();
	 break;
      default:
         Act_FormStart (Soc_DefaultActions[SocialEvent]);
	 break;
     }

   /***** Parameter to go to another course *****/
   if (CrsCod > 0 &&				// Course specified
       CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
      Crs_PutParamCrsCod (CrsCod);		// Go to another course
  }

/*****************************************************************************/
/******************* Get social event summary and content ********************/
/*****************************************************************************/

static void Soc_GetEventSummary (Soc_SocialEvent_t SocialEvent,long Cod,
                                 char *SummaryStr,unsigned MaxChars)
  {
   SummaryStr[0] = '\0';

   switch (SocialEvent)
     {
      case Soc_EVENT_UNKNOWN:
          break;
      case Soc_EVENT_INS_DOC_PUB_FILE:
      case Soc_EVENT_INS_SHA_PUB_FILE:
      case Soc_EVENT_CTR_DOC_PUB_FILE:
      case Soc_EVENT_CTR_SHA_PUB_FILE:
      case Soc_EVENT_DEG_DOC_PUB_FILE:
      case Soc_EVENT_DEG_SHA_PUB_FILE:
      case Soc_EVENT_CRS_DOC_PUB_FILE:
      case Soc_EVENT_CRS_SHA_PUB_FILE:
	 Brw_GetSummaryAndContentOrSharedFile (SummaryStr,NULL,Cod,MaxChars,false);
         break;
      case Soc_EVENT_EXAM_ANNOUNCEMENT:
         Exa_GetSummaryAndContentExamAnnouncement (SummaryStr,NULL,Cod,MaxChars,false);
         break;
      case Soc_EVENT_SOCIAL_POST:
	 // TODO: Implement social posts
         break;
      case Soc_EVENT_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,Cod,MaxChars,false);
         break;
      case Soc_EVENT_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,Cod,MaxChars,false);
         break;
     }
  }

/*****************************************************************************/
/********************* Store a social event into database ********************/
/*****************************************************************************/

void Soc_StoreSocialEvent (Soc_SocialEvent_t SocialEvent,long Cod)
  {
   char Query[512];
   long CtyCod;
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;

   if (SocialEvent == Soc_EVENT_FORUM_POST)
     {
      // CtyCod = Gbl.Forum.Cty.CtyCod;
      // InsCod = Gbl.Forum.Ins.InsCod;
      // CtrCod = Gbl.Forum.Ctr.CtrCod;
      // DegCod = Gbl.Forum.Deg.DegCod;
      // CrsCod = Gbl.Forum.Crs.CrsCod;
      CtyCod = -1L;
      InsCod = -1L;
      CtrCod = -1L;
      DegCod = -1L;
      CrsCod = -1L;
    }
   else
     {
      CtyCod = Gbl.CurrentCty.Cty.CtyCod;
      InsCod = Gbl.CurrentIns.Ins.InsCod;
      CtrCod = Gbl.CurrentCtr.Ctr.CtrCod;
      DegCod = Gbl.CurrentDeg.Deg.DegCod;
      CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
     }

   /***** Store notify event *****/
   sprintf (Query,"INSERT INTO social (SocialEvent,UsrCod,"
	          "CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
	          "Cod,TimeEvent)"
                  " VALUES ('%u','%ld',"
                  "'%ld','%ld','%ld','%ld','%ld',"
                  "'%ld',NOW())",
            (unsigned) SocialEvent,Gbl.Usrs.Me.UsrDat.UsrCod,
            CtyCod,InsCod,CtrCod,DegCod,CrsCod,
            Cod);
   DB_QueryINSERT (Query,"can not create new social event");
  }

/*****************************************************************************/
/*********************** Form to remove social event *************************/
/*****************************************************************************/

static void Soc_PutFormToRemoveSocialEvent (long SocCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social post *****/
   Act_FormStart (ActRemSocEvn);
   Soc_PutHiddenParamSocCod (SocCod);
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/remove-on64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
   Act_FormEnd ();
  }

/*****************************************************************************/
/**************************** Remove social event ****************************/
/*****************************************************************************/

static void Soc_PutHiddenParamSocCod (long SocCod)
  {
   Par_PutHiddenParamLong ("SocCod",SocCod);
  }

/*****************************************************************************/
/**************************** Remove social event ****************************/
/*****************************************************************************/

void Soc_RemoveSocialEvent (void)
  {
   Lay_ShowAlert (Lay_INFO,"Not implemented...");

   /***** Write current timeline *****/
   Soc_ShowFollowingTimeline ();
  }
