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
   ActSeeFor,		// Soc_EVENT_FORUM_POST

   /* Messages tab */
   ActShoNot,		// Soc_EVENT_NOTICE

   /* Statistics tab */

   /* Profile tab */

  };

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

struct SocialEvent
  {
   long SocCod;
   Soc_SocialEvent_t SocialEvent;
   long UsrCod;
   long CtyCod;
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;
   long Cod;
   time_t DateTimeUTC;
  };

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

static unsigned long Soc_ShowTimeline (const char *Query,Act_Action_t UpdateAction);
static Soc_SocialEvent_t Soc_GetSocialEventFromDB (const char *Str);
static void Soc_WriteSocialEvent (const struct SocialEvent *Soc,
                                  struct UsrData *UsrDat,
                                  bool PutIconRemove);
static void Soc_WriteEventDate (time_t TimeUTC);
static void Soc_StartFormGoToAction (Soc_SocialEvent_t SocialEvent,
                                     long CrsCod,long Cod);
static void Soc_GetEventSummary (const struct SocialEvent *Soc,
                                 char *SummaryStr,unsigned MaxChars);

static void Soc_PutLinkToWriteANewPost (void);
static void Soc_GetAndWriteSocialPost (long PstCod);

static void Soc_PutFormToRemoveSocialEvent (long SocCod);
static void Soc_PutHiddenParamSocCod (long SocCod);
static long Soc_GetParamSocCod (void);
static void Soc_GetDataOfSocialEventByCod (struct SocialEvent *Soc);
static void Soc_GetDataOfSocialEventFromRow (MYSQL_ROW row,struct SocialEvent *Soc);

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
   Soc_ShowTimeline (Query,ActUnk);
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
   if (!Soc_ShowTimeline (Query,ActSeeSocAct))
      Lay_ShowAlert (Lay_INFO,"No hay actividad p&uacute;blica.");	// Need translation!!!
  }

/*****************************************************************************/
/*********************** Show social activity (timeline) *********************/
/*****************************************************************************/
// UpdateAction == ActUnk ==> no form to update is displayed

static unsigned long Soc_ShowTimeline (const char *Query,Act_Action_t UpdateAction)
  {
   extern const char *Txt_Public_activity;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumEvents;
   unsigned long NumEvent;
   struct SocialEvent Soc;
   struct UsrData UsrDat;

   /***** Get timeline from database *****/
   NumEvents = DB_QuerySELECT (Query,&mysql_res,"can not get social events");

   /***** List my timeline *****/
   if (NumEvents)	// Events found
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start frame *****/
      Lay_StartRoundFrame ("560px",Txt_Public_activity);

      /***** Form to update timeline *****/
      if (UpdateAction != ActUnk)
	 Act_PutLinkToUpdateAction (UpdateAction);

      /***** Start list *****/
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");

      /***** List events one by one *****/
      for (NumEvent = 0;
	   NumEvent < NumEvents;
	   NumEvent++)
	{
         /* Get next social event */
         row = mysql_fetch_row (mysql_res);
         Soc_GetDataOfSocialEventFromRow (row,&Soc);

         /* Write row for this social event */
         Soc_WriteSocialEvent (&Soc,&UsrDat,true);
        }

      /***** End list *****/
      fprintf (Gbl.F.Out,"</ul>");

      /***** End frame *****/
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
/**************************** Write social event *****************************/
/*****************************************************************************/

static void Soc_WriteSocialEvent (const struct SocialEvent *Soc,
                                  struct UsrData *UsrDat,
                                  bool PutIconRemove)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_SOCIAL_EVENT[Soc_NUM_SOCIAL_EVENTS];
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   extern const char *Txt_Country;
   struct Country Cty;
   struct Institution Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char ForumName[512];
   char SummaryStr[Cns_MAX_BYTES_TEXT+1];

   /***** Get details *****/
   /* Get author data */
   UsrDat->UsrCod = Soc->UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat);

   /* Get country data */
   Cty.CtyCod = Soc->CtyCod;
   Cty_GetDataOfCountryByCod (&Cty,Cty_GET_BASIC_DATA);

   /* Get institution data */
   Ins.InsCod = Soc->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

    /* Get centre data */
   Ctr.CtrCod = Soc->CtrCod;
   Ctr_GetDataOfCentreByCod (&Ctr);

   /* Get degree data */
   Deg.DegCod = Soc->DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /* Get course data */
   Crs.CrsCod = Soc->CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /* Get forum type of the post */
   if (Soc->SocialEvent == Soc_EVENT_FORUM_POST)
     {
      Gbl.Forum.ForumType = For_GetForumTypeOfAPost (Soc->Cod);
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

   /***** Start list item *****/
   fprintf (Gbl.F.Out,"<li>");

   /***** Left: write author's photo *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_LEFT_PHOTO\">");
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					NULL,
		     "PHOTO60x80",Pho_ZOOM);
   fprintf (Gbl.F.Out,"</div>");

   /***** Right: author's name, time and summary *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_CONTAINER\">");

   /* Write author's full name and nickname */
   Str_LimitLengthHTMLStr (UsrDat->FullName,20);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_AUTHOR\">"
		      "<span class=\"DAT_N_BOLD\">%s</span>"
		      "<span class=\"DAT_LIGHT\"> @%s</span>"
		      "</div>",
	    UsrDat->FullName,UsrDat->Nickname);

   /* Write date and time */
   Soc_WriteEventDate (Soc->DateTimeUTC);

   if (Soc->SocialEvent == Soc_EVENT_SOCIAL_POST)
     {
      /* Write post content */
      fprintf (Gbl.F.Out,"<div class=\"DAT\">");
      Soc_GetAndWriteSocialPost (Soc->Cod);
      fprintf (Gbl.F.Out,"</div>");

      /* Write form to remove this event */
      if (PutIconRemove &&
	  Gbl.Usrs.Me.Logged &&
          UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// I am the author
	 Soc_PutFormToRemoveSocialEvent (Soc->SocCod);
     }
   else
     {
      /* Write event type and location */
      fprintf (Gbl.F.Out,"<div>");
      Soc_StartFormGoToAction (Soc->SocialEvent,Crs.CrsCod,Soc->Cod);
      Act_LinkFormSubmit (Txt_SOCIAL_EVENT[Soc->SocialEvent],
			  The_ClassForm[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s</a>",
	       Txt_SOCIAL_EVENT[Soc->SocialEvent]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");

      if (Soc->SocialEvent == Soc_EVENT_FORUM_POST)
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
      Soc_GetEventSummary (Soc,SummaryStr,Soc_MAX_BYTES_SUMMARY);
      fprintf (Gbl.F.Out,"<div class=\"DAT\">%s</div>",SummaryStr);
     }

   /* End of right part */
   fprintf (Gbl.F.Out,"</div>");

   /***** End list item *****/
   fprintf (Gbl.F.Out,"</li>");
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

static void Soc_GetEventSummary (const struct SocialEvent *Soc,
                                 char *SummaryStr,unsigned MaxChars)
  {
   SummaryStr[0] = '\0';

   switch (Soc->SocialEvent)
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
	 Brw_GetSummaryAndContentOrSharedFile (SummaryStr,NULL,Soc->Cod,MaxChars,false);
         break;
      case Soc_EVENT_EXAM_ANNOUNCEMENT:
         Exa_GetSummaryAndContentExamAnnouncement (SummaryStr,NULL,Soc->Cod,MaxChars,false);
         break;
      case Soc_EVENT_SOCIAL_POST:
	 // Not applicable
         break;
      case Soc_EVENT_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,Soc->Cod,MaxChars,false);
         break;
      case Soc_EVENT_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,Soc->Cod,MaxChars,false);
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
/***************** Put contextual link to write a new post *******************/
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
   Lay_HelpPlainEditor ();

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
/*********************** Form to remove social event *************************/
/*****************************************************************************/

static void Soc_PutFormToRemoveSocialEvent (long SocCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social post *****/
   Act_FormStart (ActReqRemSocEvn);
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
/************** Put parameter with the code of a social event ****************/
/*****************************************************************************/

static void Soc_PutHiddenParamSocCod (long SocCod)
  {
   Par_PutHiddenParamLong ("SocCod",SocCod);
  }

/*****************************************************************************/
/************** Get parameter with the code of a social event ****************/
/*****************************************************************************/

static long Soc_GetParamSocCod (void)
  {
   char LongStr[1+10+1];	// String that holds the social event code
   long SocCod;

   /* Get social event code */
   Par_GetParToText ("SocCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&SocCod) != 1)
      Lay_ShowErrorAndExit ("Wrong code of social event.");

   return SocCod;
  }

/*****************************************************************************/
/******************* Request the removal of a social event *******************/
/*****************************************************************************/

void Soc_RequestRemovalSocialEvent (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct SocialEvent Soc;
   bool ICanRemove;
   struct UsrData UsrDat;

   /***** Get the code of the social event to remove *****/
   Soc.SocCod = Soc_GetParamSocCod ();

   /***** Get data of social event *****/
   Soc_GetDataOfSocialEventByCod (&Soc);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 Soc.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod &&
                 Soc.SocialEvent == Soc_EVENT_SOCIAL_POST);
   if (ICanRemove)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Form to ask for confirmation to remove this social post *****/
      /* Start form */
      Act_FormStart (ActRemSocEvn);
      Soc_PutHiddenParamSocCod (Soc.SocCod);
      Lay_ShowAlert (Lay_WARNING,Txt_Do_you_really_want_to_remove_the_following_comment);

      /* Show social event */
      Lay_StartRoundFrame ("560px",NULL);
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");
      Soc_WriteSocialEvent (&Soc,&UsrDat,false);
      fprintf (Gbl.F.Out,"</ul>");
      Lay_EndRoundFrame ();

      /* End form */
      Lay_PutRemoveButton (Txt_Remove);
      Act_FormEnd ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Write timeline again *****/
   Soc_ShowFollowingTimeline ();
  }

/*****************************************************************************/
/************************** Remove a social event ****************************/
/*****************************************************************************/

void Soc_RemoveSocialEvent (void)
  {
   extern const char *Txt_Comment_removed;
   struct SocialEvent Soc;
   bool ICanRemove;
   char Query[128];

   /***** Get the code of the social event to remove *****/
   Soc.SocCod = Soc_GetParamSocCod ();

   /***** Get data of social event *****/
   Soc_GetDataOfSocialEventByCod (&Soc);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 Soc.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod &&
                 Soc.SocialEvent == Soc_EVENT_SOCIAL_POST);
   if (ICanRemove)
     {
      /***** Remove social event *****/
      sprintf (Query,"DELETE FROM social WHERE SocCod='%ld'",Soc.SocCod);
      DB_QueryDELETE (Query,"can not remove a social event");

      /***** Remove social post *****/
      if (Soc.SocialEvent == Soc_EVENT_SOCIAL_POST)
	{
	 sprintf (Query,"DELETE FROM social_post WHERE PstCod='%ld'",Soc.Cod);
	 DB_QueryDELETE (Query,"can not remove a social post");
	}

      /***** Message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_Comment_removed);
     }

   /***** Write timeline after removing *****/
   Soc_ShowFollowingTimeline ();
  }

/*****************************************************************************/
/******************* Get assignment data using its code **********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialEventByCod (struct SocialEvent *Soc)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of social event from database *****/
   sprintf (Query,"SELECT SocCod,SocialEvent,UsrCod,"
	          "CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
	          "Cod,UNIX_TIMESTAMP(TimeEvent)"
                  " FROM social"
                  " WHERE SocCod='%ld'",
            Soc->SocCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social event"))
     {
      /***** Get social event *****/
      row = mysql_fetch_row (mysql_res);
      Soc_GetDataOfSocialEventFromRow (row,Soc);
     }
   else
     {
      /***** Reset fields of social event *****/
      Soc->SocialEvent = Soc_EVENT_UNKNOWN;
      Soc->UsrCod = -1L;
      Soc->CtyCod =
      Soc->InsCod =
      Soc->CtrCod =
      Soc->DegCod =
      Soc->CrsCod = -1L;
      Soc->Cod    = -1L;
      Soc->DateTimeUTC = (time_t) 0;
     }
  }

/*****************************************************************************/
/******************* Get assignment data using its code **********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialEventFromRow (MYSQL_ROW row,struct SocialEvent *Soc)
  {
   /* Get social code (row[0]) */
   Soc->SocCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Get event type (row[1]) */
   Soc->SocialEvent = Soc_GetSocialEventFromDB ((const char *) row[1]);

   /* Get (from) user code (row[2]) */
   Soc->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get country code (row[3]) */
   Soc->CtyCod = Str_ConvertStrCodToLongCod (row[3]);

   /* Get institution code (row[4]) */
   Soc->InsCod = Str_ConvertStrCodToLongCod (row[4]);

   /* Get centre code (row[5]) */
   Soc->CtrCod = Str_ConvertStrCodToLongCod (row[5]);

   /* Get degree code (row[6]) */
   Soc->DegCod = Str_ConvertStrCodToLongCod (row[6]);

   /* Get course code (row[7]) */
   Soc->CrsCod = Str_ConvertStrCodToLongCod (row[7]);

   /* Get file/post... code (row[8]) */
   Soc->Cod = Str_ConvertStrCodToLongCod (row[8]);

   /* Get time of the event (row[9]) */
   Soc->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[9]);
  }
