// swad_social.c: social networking (timeline)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
#include "swad_profile.h"
#include "swad_social.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Soc_MAX_BYTES_SUMMARY 100
#define Soc_NUM_PUBS_IN_TIMELINE 100

static const Act_Action_t Soc_DefaultActions[Soc_NUM_SOCIAL_NOTES] =
  {
   ActUnk,		// Soc_NOTE_UNKNOWN

   /* Institution tab */
   ActSeeDocIns,	// Soc_NOTE_INS_DOC_PUB_FILE
   ActAdmComIns,	// Soc_NOTE_INS_SHA_PUB_FILE

   /* Centre tab */
   ActSeeDocCtr,	// Soc_NOTE_CTR_DOC_PUB_FILE
   ActAdmComCtr,	// Soc_NOTE_CTR_SHA_PUB_FILE

   /* Degree tab */
   ActSeeDocDeg,	// Soc_NOTE_DEG_DOC_PUB_FILE
   ActAdmComDeg,	// Soc_NOTE_DEG_SHA_PUB_FILE

   /* Course tab */
   ActSeeDocCrs,	// Soc_NOTE_CRS_DOC_PUB_FILE
   ActAdmShaCrs,	// Soc_NOTE_CRS_SHA_PUB_FILE

   /* Assessment tab */
   ActSeeExaAnn,	// Soc_NOTE_EXAM_ANNOUNCEMENT

   /* Users tab */

   /* Social tab */
   ActSeeSocTmlGbl,	// Soc_NOTE_SOCIAL_POST (action not used)
   ActSeeFor,		// Soc_NOTE_FORUM_POST

   /* Messages tab */
   ActShoNot,		// Soc_NOTE_NOTICE

   /* Statistics tab */

   /* Profile tab */

  };

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

struct SocialPublishing
  {
   long PubCod;
   long AuthorCod;
   long PublisherCod;
   long NotCod;
   time_t DateTimeUTC;
  };

struct SocialNote
  {
   long NotCod;
   Soc_NoteType_t NoteType;
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

static void Soc_ShowTimeline (const char *Query,Act_Action_t UpdateAction);
static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub);
static void Soc_WriteSocialNote (const struct SocialPublishing *SocPub,
                                 const struct SocialNote *SocNot,
                                 struct UsrData *UsrDat,
                                 bool PutIconRemove);
static void Soc_WriteNoteDate (time_t TimeUTC);
static void Soc_GetAndWriteSocialPost (long PstCod);
static void Soc_StartFormGoToAction (Soc_NoteType_t NoteType,
                                     long CrsCod,long Cod);
static void Soc_GetNoteSummary (const struct SocialNote *SocNot,
                                char *SummaryStr,unsigned MaxChars);
static void Soc_PublishSocialNoteInTimeline (struct SocialPublishing *SocPub);

static void Soc_PutLinkToWriteANewPost (Act_Action_t Action,void (*FuncParams) ());
static void Soc_FormSocialPost (void);
static void Soc_ReceiveSocialPost (void);

static void Soc_PutFormToShareSocialPublishing (long PubCod);
static void Soc_PutFormToUnshareSocialPublishing (long PubCod);
static void Soc_PutFormToRemoveSocialPublishing (long PubCod);
static void Soc_PutHiddenParamPubCod (long NotCod);
static long Soc_GetParamPubCod (void);

static void Soc_ShareSocialPublishing (void);
static void Soc_UnshareSocialPublishing (void);

static void Soc_RequestRemovalSocialPublishing (void);
static void Soc_RemoveSocialPublishing (void);

static void Soc_DeleteASocialPublishingFromDB (const struct SocialPublishing *SocPub,
                                               const struct SocialNote *SocNot);

static bool Soc_CheckIfNoteIsYetPublishedByMe (long NotCod);
static unsigned long Soc_GetNumPubsOfANote (long NotCod);
static void Soc_GetDataOfSocialPublishingByCod (struct SocialPublishing *SocPub);
static void Soc_GetDataOfSocialNoteByCod (struct SocialNote *SocNot);
static void Soc_GetDataOfSocialNoteFromRow (MYSQL_ROW row,struct SocialNote *SocNot);
static Soc_NoteType_t Soc_GetSocialNoteFromDB (const char *Str);

/*****************************************************************************/
/*********** Show social activity (timeline) of a selected user **************/
/*****************************************************************************/

void Soc_ShowTimelineUsr (void)
  {
   char Query[512];

   /***** Link to write a new social post (public comment) *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod &&	// It's me
       Gbl.CurrentAct != ActReqSocPstUsr)		// Not writing a new post
      Soc_PutLinkToWriteANewPost (ActReqSocPstUsr,Usr_PutParamOtherUsrCodEncrypted);

   /***** Build query to show timeline with publishing of a unique user *****/
   sprintf (Query,"SELECT PubCod,AuthorCod,PublisherCod,NotCod,UNIX_TIMESTAMP(TimePublish)"
                  " FROM social_timeline"
                  " WHERE PublisherCod='%ld'"
                  " ORDER BY PubCod DESC LIMIT %u",
            Gbl.Usrs.Other.UsrDat.UsrCod,
            Soc_NUM_PUBS_IN_TIMELINE);

   /***** Show timeline *****/
   Soc_ShowTimeline (Query,ActSeePubPrf);
  }

/*****************************************************************************/
/***** Show social activity (timeline) including all the users I follow ******/
/*****************************************************************************/

void Soc_ShowTimelineGbl (void)
  {
   extern const char *Txt_You_dont_follow_any_user;
   char Query[512];

   /***** Link to write a new social post (public comment) *****/
   if (Gbl.CurrentAct != ActReqSocPstGbl)	// Not writing a new post
      Soc_PutLinkToWriteANewPost (ActReqSocPstGbl,NULL);

   /***** If I follow someone... *****/
   if (Fol_GetNumFollowing (Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      /***** Create temporary table with publishing codes *****/
      sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS pub_cods");
      if (mysql_query (&Gbl.mysql,Query))
	 DB_ExitOnMySQLError ("can not remove temporary tables");

      sprintf (Query,"CREATE TEMPORARY TABLE pub_cods (PubCod BIGINT NOT NULL,UNIQUE INDEX(PubCod)) ENGINE=MEMORY"
		     " SELECT MIN(PubCod) AS PubCod"
		     " FROM social_timeline"
		     " WHERE PublisherCod IN"
		     " (SELECT '%ld'"
		     " UNION"
		     " SELECT FollowedCod FROM usr_follow WHERE FollowerCod='%ld')"
		     " GROUP BY NotCod"
		     " ORDER BY PubCod DESC LIMIT %u",
	       Gbl.Usrs.Me.UsrDat.UsrCod,
	       Gbl.Usrs.Me.UsrDat.UsrCod,
	       Soc_NUM_PUBS_IN_TIMELINE);
      if (mysql_query (&Gbl.mysql,Query))
	 DB_ExitOnMySQLError ("can not create temporary table");

      /***** Build query to show timeline including the users I am following *****/
      sprintf (Query,"SELECT PubCod,AuthorCod,PublisherCod,NotCod,UNIX_TIMESTAMP(TimePublish)"
		     " FROM social_timeline WHERE PubCod IN "
		     "(SELECT PubCod FROM pub_cods)"
		     " ORDER BY PubCod DESC");

      /***** Show timeline *****/
      Soc_ShowTimeline (Query,ActSeeSocTmlGbl);

      /***** Drop temporary table with publishing codes *****/
      sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS pub_cods");
      if (mysql_query (&Gbl.mysql,Query))
	 DB_ExitOnMySQLError ("can not remove temporary tables");
     }
   else	// I do not follow anyone
      /***** Show warning if I do not follow anyone *****/
      Lay_ShowAlert (Lay_INFO,Txt_You_dont_follow_any_user);
  }

/*****************************************************************************/
/*********************** Show social activity (timeline) *********************/
/*****************************************************************************/

static void Soc_ShowTimeline (const char *Query,Act_Action_t UpdateAction)
  {
   extern const char *Txt_Public_activity;
   extern const char *Txt_No_public_activity;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPublishings;
   unsigned long NumPub;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   struct UsrData UsrDat;

   /***** Get timeline from database *****/
   NumPublishings = DB_QuerySELECT (Query,&mysql_res,"can not get social notes");

   /***** List my timeline *****/
   if (NumPublishings)	// Publishings found in timeline
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start frame *****/
      Lay_StartRoundFrame ("560px",Txt_Public_activity);

      /***** Form to update timeline *****/
      Act_PutLinkToUpdateAction (UpdateAction);

      /***** Start list *****/
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");

      /***** List publishings in timeline one by one *****/
      for (NumPub = 0;
	   NumPub < NumPublishings;
	   NumPub++)
	{
         /* Get next social publishing */
         row = mysql_fetch_row (mysql_res);
         Soc_GetDataOfSocialPublishingFromRow (row,&SocPub);

         /* Get and write social note */
         SocNot.NotCod = SocPub.NotCod;
         Soc_GetDataOfSocialNoteByCod (&SocNot);
         Soc_WriteSocialNote (&SocPub,&SocNot,&UsrDat,true);
        }

      /***** End list *****/
      fprintf (Gbl.F.Out,"</ul>");

      /***** End frame *****/
      Lay_EndRoundFrame ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else	// No publishing found in timeline
      Lay_ShowAlert (Lay_INFO,Txt_No_public_activity);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Get data of social publishing using its code *****************/
/*****************************************************************************/

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub)
  {
   /* Get social publishing code (row[0]) */
   SocPub->PubCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Get author's code (row[1]) */
   SocPub->AuthorCod = Str_ConvertStrCodToLongCod (row[1]);

   /* Get publisher's code (row[2]) */
   SocPub->PublisherCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get social note code (row[3]) */
   SocPub->NotCod = Str_ConvertStrCodToLongCod (row[3]);

   /* Get time of the note (row[4]) */
   SocPub->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[4]);
  }

/*****************************************************************************/
/***************************** Write social note *****************************/
/*****************************************************************************/

static void Soc_WriteSocialNote (const struct SocialPublishing *SocPub,
                                 const struct SocialNote *SocNot,
                                 struct UsrData *UsrDat,
                                 bool PutIconRemove)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_SOCIAL_NOTE[Soc_NUM_SOCIAL_NOTES];
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
   UsrDat->UsrCod = SocNot->UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat);

   /* Get country data */
   Cty.CtyCod = SocNot->CtyCod;
   Cty_GetDataOfCountryByCod (&Cty,Cty_GET_BASIC_DATA);

   /* Get institution data */
   Ins.InsCod = SocNot->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

    /* Get centre data */
   Ctr.CtrCod = SocNot->CtrCod;
   Ctr_GetDataOfCentreByCod (&Ctr);

   /* Get degree data */
   Deg.DegCod = SocNot->DegCod;
   Deg_GetDataOfDegreeByCod (&Deg);

   /* Get course data */
   Crs.CrsCod = SocNot->CrsCod;
   Crs_GetDataOfCourseByCod (&Crs);

   /* Get forum type of the post */
   if (SocNot->NoteType == Soc_NOTE_FORUM_POST)
     {
      Gbl.Forum.ForumType = For_GetForumTypeOfAPost (SocNot->Cod);
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
   fprintf (Gbl.F.Out,"<li class=\"SOCIAL_PUB\">");

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
   Soc_WriteNoteDate (SocNot->DateTimeUTC);

   if (SocNot->NoteType == Soc_NOTE_SOCIAL_POST)
     {
      /* Write post content */
      fprintf (Gbl.F.Out,"<div class=\"DAT\">");
      Soc_GetAndWriteSocialPost (SocNot->Cod);
      fprintf (Gbl.F.Out,"</div>");

      /***** Put icons to share/unshare/remove *****/
      if (Gbl.Usrs.Me.Logged)
	{
	 if (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// I am the author
	   {
	    if (PutIconRemove)
	       /* Put icon to remove this publishing */
	       Soc_PutFormToRemoveSocialPublishing (SocPub->PubCod);
	   }
	 else							// I am not the author
	   {
	    if (Soc_CheckIfNoteIsYetPublishedByMe (SocNot->NotCod))	// I have yet published this social note
	       /* Put icon to unshare this publishing */
	       Soc_PutFormToUnshareSocialPublishing (SocPub->PubCod);
	    else							// I have not yet published this social note
	       /* Put icon to share this publishing */
	       Soc_PutFormToShareSocialPublishing (SocPub->PubCod);
	   }
	}
     }
   else
     {
      /* Write note type and location */
      fprintf (Gbl.F.Out,"<div>");
      Soc_StartFormGoToAction (SocNot->NoteType,Crs.CrsCod,SocNot->Cod);
      Act_LinkFormSubmit (Txt_SOCIAL_NOTE[SocNot->NoteType],
			  The_ClassForm[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s</a>",
	       Txt_SOCIAL_NOTE[SocNot->NoteType]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");

      if (SocNot->NoteType == Soc_NOTE_FORUM_POST)
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

      /* Write content of the note */
      Soc_GetNoteSummary (SocNot,SummaryStr,Soc_MAX_BYTES_SUMMARY);
      fprintf (Gbl.F.Out,"<div class=\"DAT\">%s</div>",SummaryStr);
     }

   /* End of right part */
   fprintf (Gbl.F.Out,"</div>");

   /***** End list item *****/
   fprintf (Gbl.F.Out,"</li>");
  }

/*****************************************************************************/
/**************** Write the date of creation of a social note ****************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

static void Soc_WriteNoteDate (time_t TimeUTC)
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
   sprintf (Query,"SELECT Content FROM social_posts WHERE PstCod='%ld'",
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
/********* Put form to go to an action depending on the social note **********/
/*****************************************************************************/

static void Soc_StartFormGoToAction (Soc_NoteType_t NoteType,
                                     long CrsCod,long Cod)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   struct FileMetadata FileMetadata;
   long GrpCod = -1L;
   char PathUntilFileName[PATH_MAX+1];
   char FileName[NAME_MAX+1];
   Act_Action_t Action = ActUnk;				// Initialized to avoid warning

   /***** Parameters depending on the type of note *****/
   switch (NoteType)
     {
      case Soc_NOTE_INS_DOC_PUB_FILE:
      case Soc_NOTE_INS_SHA_PUB_FILE:
      case Soc_NOTE_CTR_DOC_PUB_FILE:
      case Soc_NOTE_CTR_SHA_PUB_FILE:
      case Soc_NOTE_DEG_DOC_PUB_FILE:
      case Soc_NOTE_DEG_SHA_PUB_FILE:
      case Soc_NOTE_CRS_DOC_PUB_FILE:
      case Soc_NOTE_CRS_SHA_PUB_FILE:
	 if (Cod > 0)	// File code
	   {
	    FileMetadata.FilCod = Cod;
            Brw_GetFileMetadataByCod (&FileMetadata);
            Brw_GetCrsGrpFromFileMetadata (FileMetadata.FileBrowser,FileMetadata.Cod,&CrsCod,&GrpCod);
	    Str_SplitFullPathIntoPathAndFileName (FileMetadata.Path,
						  PathUntilFileName,
						  FileName);
	   }
	 switch (NoteType)
	   {
	    case Soc_NOTE_INS_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocIns : ActSeeDocIns;
	       break;
	    case Soc_NOTE_INS_SHA_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatShaIns : ActAdmComIns;
	       break;
	    case Soc_NOTE_CTR_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocCtr : ActSeeDocCtr;
	       break;
	    case Soc_NOTE_CTR_SHA_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatShaCtr : ActAdmComCtr;
	       break;
	    case Soc_NOTE_DEG_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocDeg : ActSeeDocDeg;
	       break;
	    case Soc_NOTE_DEG_SHA_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatShaDeg : ActAdmComDeg;
	       break;
	    case Soc_NOTE_CRS_DOC_PUB_FILE:
	       Action = (Cod > 0) ? ActReqDatSeeDocCrs : ActSeeDocCrs;
	       break;
	    case Soc_NOTE_CRS_SHA_PUB_FILE:
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
      case Soc_NOTE_NOTICE:
         Act_FormStart (Soc_DefaultActions[NoteType]);
	 Not_PutHiddenParamNotCod (Cod);
	 break;
      case Soc_NOTE_FORUM_POST:
	 Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
	 For_PutAllHiddenParamsForum ();
	 break;
      default:
         Act_FormStart (Soc_DefaultActions[NoteType]);
	 break;
     }

   /***** Parameter to go to another course *****/
   if (CrsCod > 0 &&				// Course specified
       CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
      Crs_PutParamCrsCod (CrsCod);		// Go to another course
  }

/*****************************************************************************/
/******************* Get social note summary and content *********************/
/*****************************************************************************/

static void Soc_GetNoteSummary (const struct SocialNote *SocNot,
                                char *SummaryStr,unsigned MaxChars)
  {
   SummaryStr[0] = '\0';

   switch (SocNot->NoteType)
     {
      case Soc_NOTE_UNKNOWN:
          break;
      case Soc_NOTE_INS_DOC_PUB_FILE:
      case Soc_NOTE_INS_SHA_PUB_FILE:
      case Soc_NOTE_CTR_DOC_PUB_FILE:
      case Soc_NOTE_CTR_SHA_PUB_FILE:
      case Soc_NOTE_DEG_DOC_PUB_FILE:
      case Soc_NOTE_DEG_SHA_PUB_FILE:
      case Soc_NOTE_CRS_DOC_PUB_FILE:
      case Soc_NOTE_CRS_SHA_PUB_FILE:
	 Brw_GetSummaryAndContentOrSharedFile (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
      case Soc_NOTE_EXAM_ANNOUNCEMENT:
         Exa_GetSummaryAndContentExamAnnouncement (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
      case Soc_NOTE_SOCIAL_POST:
	 // Not applicable
         break;
      case Soc_NOTE_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
      case Soc_NOTE_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
     }
  }

/*****************************************************************************/
/************** Store and publish a social note into database ****************/
/*****************************************************************************/

void Soc_StoreAndPublishSocialNote (Soc_NoteType_t NoteType,long Cod)
  {
   char Query[512];
   long CtyCod;
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;
   struct SocialPublishing SocPub;

   if (NoteType == Soc_NOTE_FORUM_POST)
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

   /***** Store social note *****/
   sprintf (Query,"INSERT INTO social_notes (NoteType,UsrCod,"
	          "CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
	          "Cod,TimeNote)"
                  " VALUES ('%u','%ld',"
                  "'%ld','%ld','%ld','%ld','%ld',"
                  "'%ld',NOW())",
            (unsigned) NoteType,Gbl.Usrs.Me.UsrDat.UsrCod,
            CtyCod,InsCod,CtrCod,DegCod,CrsCod,
            Cod);
   SocPub.NotCod = DB_QueryINSERTandReturnCode (Query,"can not create new social note");

   /***** Publish social note in timeline *****/
   SocPub.AuthorCod    =
   SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Soc_PublishSocialNoteInTimeline (&SocPub);
  }

/*****************************************************************************/
/***************** Put contextual link to write a new post *******************/
/*****************************************************************************/

static void Soc_PublishSocialNoteInTimeline (struct SocialPublishing *SocPub)
  {
   char Query[256];

   /***** Publish social note in timeline *****/
   sprintf (Query,"INSERT INTO social_timeline"
	          " (AuthorCod,PublisherCod,NotCod,TimePublish)"
                  " VALUES"
                  " ('%ld','%ld','%ld',NOW())",
            SocPub->AuthorCod,SocPub->PublisherCod,SocPub->NotCod);
   DB_QueryINSERT (Query,"can not publish social note");
  }

/*****************************************************************************/
/***************** Put contextual link to write a new post *******************/
/*****************************************************************************/

static void Soc_PutLinkToWriteANewPost (Act_Action_t Action,void (*FuncParams) ())
  {
   extern const char *Txt_New_comment;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Lay_PutContextualLink (Action,FuncParams,"write64x64.gif",
			  Txt_New_comment,Txt_New_comment);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Form to write a new public comment ***********************/
/*****************************************************************************/

void Soc_FormSocialPostGbl (void)
  {
   /***** Form to write a new public comment *****/
   Soc_FormSocialPost ();

   /***** Write current timeline (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_FormSocialPostUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Form to write a new public comment *****/
   Soc_FormSocialPost ();

   /***** Write current timeline (user) *****/
   Soc_ShowTimelineUsr ();
  }

static void Soc_FormSocialPost (void)
  {
   extern const char *Txt_New_comment;
   extern const char *Txt_Send_comment;

   /***** Form to write a new public comment *****/
   /* Start frame */
   Lay_StartRoundFrame ("560px",Txt_New_comment);

   /* Start form to write the post */
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStart (ActRcvSocPstUsr);
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActRcvSocPstGbl);

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
  }

/*****************************************************************************/
/******************* Receive and store a new public post *********************/
/*****************************************************************************/

void Soc_ReceiveSocialPostGbl (void)
  {
   /***** Receive and store social post *****/
   Soc_ReceiveSocialPost ();

   /***** Write updated timeline after publishing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_ReceiveSocialPostUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Receive and store social post *****/
   Soc_ReceiveSocialPost ();

   /***** Write updated timeline after publishing (user) *****/
   Soc_ShowTimelineUsr ();
  }

static void Soc_ReceiveSocialPost (void)
  {
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   char Query[128+Cns_MAX_BYTES_LONG_TEXT];
   long PstCod;

   /***** Get and store new post *****/
   /* Get the content of the post */
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /* Insert post content in the database */
   sprintf (Query,"INSERT INTO social_posts (Content) VALUES ('%s')",
            Content);
   PstCod = DB_QueryINSERTandReturnCode (Query,"can not create post");

   /* Insert post in social notes */
   Soc_StoreAndPublishSocialNote (Soc_NOTE_SOCIAL_POST,PstCod);
  }

/*****************************************************************************/
/********************* Form to share social publishing ***********************/
/*****************************************************************************/

static void Soc_PutFormToShareSocialPublishing (long PubCod)
  {
   extern const char *Txt_Share;

   /***** Form to share social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStart (ActShaSocPubUsr);
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActShaSocPubGbl);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/share64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Share,
	    Txt_Share);
   Act_FormEnd ();
  }

/*****************************************************************************/
/************ Form to unshare (stop sharing) social publishing ***************/
/*****************************************************************************/

static void Soc_PutFormToUnshareSocialPublishing (long PubCod)
  {
   extern const char *Txt_Shared;

   /***** Form to share social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStart (ActUnsSocPubUsr);
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActUnsSocPubGbl);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/shared64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Shared,
	    Txt_Shared);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Form to remove social publishing ***********************/
/*****************************************************************************/

static void Soc_PutFormToRemoveSocialPublishing (long PubCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStart (ActReqRemSocPubUsr);
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActReqRemSocPubGbl);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON ICON_HIGHLIGHT\">"
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
/*********** Put parameter with the code of a social publishing **************/
/*****************************************************************************/

static void Soc_PutHiddenParamPubCod (long PubCod)
  {
   Par_PutHiddenParamLong ("PubCod",PubCod);
  }

/*****************************************************************************/
/*********** Get parameter with the code of a social publishing **************/
/*****************************************************************************/

static long Soc_GetParamPubCod (void)
  {
   char LongStr[1+10+1];	// String that holds the social note code
   long PubCod;

   /* Get social note code */
   Par_GetParToText ("PubCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&PubCod) != 1)
      Lay_ShowErrorAndExit ("Wrong code of social publishing.");

   return PubCod;
  }

/*****************************************************************************/
/************************* Share a social publishing *************************/
/*****************************************************************************/

void Soc_ShareSocialPubGbl (void)
  {
   /***** Share social publishing *****/
   Soc_ShareSocialPublishing ();

   /***** Write updated timeline after sharing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_ShareSocialPubUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Share social publishing *****/
   Soc_ShareSocialPublishing ();

   /***** Write updated timeline after sharing (user) *****/
   Soc_ShowTimelineUsr ();
  }

static void Soc_ShareSocialPublishing (void)
  {
   extern const char *Txt_Shared;
   struct SocialPublishing SocPub;
   bool ICanShare;
   bool IHavePublishedThisNote;

   /***** Get the code of the social publishing to share *****/
   SocPub.PubCod = Soc_GetParamPubCod ();

   /***** Get data of social publishing *****/
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   ICanShare = (Gbl.Usrs.Me.Logged &&
                SocPub.AuthorCod != Gbl.Usrs.Me.UsrDat.UsrCod);		// I am not the author
   if (ICanShare)
     {
      /***** Check if I have yet shared this social note *****/
      IHavePublishedThisNote = Soc_CheckIfNoteIsYetPublishedByMe (SocPub.NotCod);
      if (!IHavePublishedThisNote)
	{
	 /***** Share (publish social note in timeline) *****/
	 SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 Soc_PublishSocialNoteInTimeline (&SocPub);

         /***** Message of success *****/
         Lay_ShowAlert (Lay_SUCCESS,Txt_Shared);
	}
     }
  }

/*****************************************************************************/
/************** Unshare a previously shared social publishing ****************/
/*****************************************************************************/

void Soc_UnshareSocialPubGbl (void)
  {
   /***** Unshare a previously shared social publishing *****/
   Soc_UnshareSocialPublishing ();

   /***** Write updated timeline after unsharing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_UnshareSocialPubUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Unshare a previously shared social publishing *****/
   Soc_UnshareSocialPublishing ();

   /***** Write updated timeline after unsharing (user) *****/
   Soc_ShowTimelineUsr ();
  }

static void Soc_UnshareSocialPublishing (void)
  {
   extern const char *Txt_Unshared;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   bool ICanUnshare;
   bool IHavePublishedThisNote;

   /***** Get the code of the social publishing to unshare *****/
   SocPub.PubCod = Soc_GetParamPubCod ();

   /***** Get data of social publishing *****/
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   /***** Get data of social note *****/
   SocNot.NotCod = SocPub.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanUnshare = (Gbl.Usrs.Me.Logged &&
                  SocPub.AuthorCod != Gbl.Usrs.Me.UsrDat.UsrCod);		// I am not the author
   if (ICanUnshare)
     {
      /***** Check if I have yet shared this social note *****/
      IHavePublishedThisNote = Soc_CheckIfNoteIsYetPublishedByMe (SocPub.NotCod);
      if (IHavePublishedThisNote)
	{
         /***** Delete social publishing from database *****/
	 Soc_DeleteASocialPublishingFromDB (&SocPub,&SocNot);

	 /***** Message of success *****/
	 Lay_ShowAlert (Lay_SUCCESS,Txt_Unshared);
	}
     }
  }

/*****************************************************************************/
/**************** Request the removal of a social publishing *****************/
/*****************************************************************************/

void Soc_RequestRemSocialPubGbl (void)
  {
   /***** Request the removal of social publishing *****/
   Soc_RequestRemovalSocialPublishing ();

   /***** Write timeline again (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_RequestRemSocialPubUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Request the removal of social publishing *****/
   Soc_RequestRemovalSocialPublishing ();

   /***** Write timeline again (user) *****/
   Soc_ShowTimelineUsr ();
  }

static void Soc_RequestRemovalSocialPublishing (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   bool ICanRemove;
   struct UsrData UsrDat;

   /***** Get the code of the social publishing to remove *****/
   SocPub.PubCod = Soc_GetParamPubCod ();

   /***** Get data of social publishing *****/
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   /***** Get data of social note *****/
   SocNot.NotCod = SocPub.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocPub.PublisherCod == Gbl.Usrs.Me.UsrDat.UsrCod &&
                 SocNot.NoteType == Soc_NOTE_SOCIAL_POST);
   if (ICanRemove)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Form to ask for confirmation to remove this social post *****/
      /* Start form */
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	{
	 Act_FormStart (ActRemSocPubUsr);
	 Usr_PutParamOtherUsrCodEncrypted ();
	}
      else
	 Act_FormStart (ActRemSocPubGbl);
      Soc_PutHiddenParamPubCod (SocPub.PubCod);
      Lay_ShowAlert (Lay_WARNING,Txt_Do_you_really_want_to_remove_the_following_comment);

      /* Show social note */
      Lay_StartRoundFrame ("560px",NULL);
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");
      Soc_WriteSocialNote (&SocPub,&SocNot,&UsrDat,false);
      fprintf (Gbl.F.Out,"</ul>");
      Lay_EndRoundFrame ();

      /* End form */
      Lay_PutRemoveButton (Txt_Remove);
      Act_FormEnd ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
  }

/*****************************************************************************/
/************************ Remove a social publishing *************************/
/*****************************************************************************/

void Soc_RemoveSocialPubGbl (void)
  {
   /***** Remove a social publishing *****/
   Soc_RemoveSocialPublishing ();

   /***** Write updated timeline after removing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_RemoveSocialPubUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Remove a social publishing *****/
   Soc_RemoveSocialPublishing ();

   /***** Write updated timeline after removing (user) *****/
   Soc_ShowTimelineUsr ();
  }

static void Soc_RemoveSocialPublishing (void)
  {
   extern const char *Txt_Comment_removed;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   bool ICanRemove;

   /***** Get the code of the social publishing to remove *****/
   SocPub.PubCod = Soc_GetParamPubCod ();

   /***** Get data of social publishing *****/
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   /***** Get data of social note *****/
   SocNot.NotCod = SocPub.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocPub.PublisherCod == Gbl.Usrs.Me.UsrDat.UsrCod &&
                 SocNot.NoteType == Soc_NOTE_SOCIAL_POST);
   if (ICanRemove)
     {
      /***** Delete social publishing from database *****/
      Soc_DeleteASocialPublishingFromDB (&SocPub,&SocNot);

      /***** Message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_Comment_removed);
     }
  }

/*****************************************************************************/
/**************** Delete a social publishing from database *******************/
/*****************************************************************************/

static void Soc_DeleteASocialPublishingFromDB (const struct SocialPublishing *SocPub,
                                               const struct SocialNote *SocNot)
  {
   char Query[128];
   unsigned long NumPubs;

   /***** Remove social publishing *****/
   sprintf (Query,"DELETE FROM social_timeline WHERE PubCod='%ld'",
	    SocPub->PubCod);
   DB_QueryDELETE (Query,"can not remove a social publishing");

   /***** Count number of times this note
	  is published in timeline after removal *****/
   NumPubs = Soc_GetNumPubsOfANote (SocNot->NotCod);

   if (NumPubs == 0)	// This was the last publishing of this note
     {
      /***** Remove social note *****/
      sprintf (Query,"DELETE FROM social_notes WHERE NotCod='%ld'",
	       SocNot->NotCod);
      DB_QueryDELETE (Query,"can not remove a social note");

      /***** Remove social post *****/
      if (SocNot->NoteType == Soc_NOTE_SOCIAL_POST)
	{
	 sprintf (Query,"DELETE FROM social_posts WHERE PstCod='%ld'",
		  SocNot->Cod);
	 DB_QueryDELETE (Query,"can not remove a social post");
	}
     }
  }

/*****************************************************************************/
/***** Check if I have published a social note (I authored or shared it) *****/
/*****************************************************************************/

static bool Soc_CheckIfNoteIsYetPublishedByMe (long NotCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM social_timeline"
	          " WHERE NotCod='%ld' AND PublisherCod='%ld'",
	    NotCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if you have published a social note") != 0);
  }

/*****************************************************************************/
/*********** Get number of publishings in timeline of a note code ************/
/*****************************************************************************/

static unsigned long Soc_GetNumPubsOfANote (long NotCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM social_timeline WHERE NotCod='%ld'",
	    NotCod);
   return DB_QueryCOUNT (Query,"can not get number of publishing of a note");
  }

/*****************************************************************************/
/********* Get data of social publishing in timeline using its code **********/
/*****************************************************************************/

static void Soc_GetDataOfSocialPublishingByCod (struct SocialPublishing *SocPub)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of social publishing from database *****/
   sprintf (Query,"SELECT PubCod,AuthorCod,PublisherCod,NotCod,UNIX_TIMESTAMP(TimePublish)"
                  " FROM social_timeline"
                  " WHERE PubCod='%ld'",
            SocPub->PubCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social note"))
     {
      /***** Get social note *****/
      row = mysql_fetch_row (mysql_res);
      Soc_GetDataOfSocialPublishingFromRow (row,SocPub);
     }
   else
     {
      /***** Reset fields of social publishing *****/
      SocPub->AuthorCod    = -1L;
      SocPub->PublisherCod = -1L;
      SocPub->NotCod       = -1L;
      SocPub->DateTimeUTC  = (time_t) 0;
     }
  }

/*****************************************************************************/
/**************** Get data of social note using its code *********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialNoteByCod (struct SocialNote *SocNot)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of social note from database *****/
   sprintf (Query,"SELECT NotCod,NoteType,UsrCod,"
	          "CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
	          "Cod,UNIX_TIMESTAMP(TimeNote)"
                  " FROM social_notes"
                  " WHERE NotCod='%ld'",
            SocNot->NotCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social note"))
     {
      /***** Get social note *****/
      row = mysql_fetch_row (mysql_res);
      Soc_GetDataOfSocialNoteFromRow (row,SocNot);
     }
   else
     {
      /***** Reset fields of social note *****/
      SocNot->NoteType    = Soc_NOTE_UNKNOWN;
      SocNot->UsrCod      = -1L;
      SocNot->CtyCod      = -1L;
      SocNot->InsCod      = -1L;
      SocNot->CtrCod      = -1L;
      SocNot->DegCod      = -1L;
      SocNot->CrsCod      = -1L;
      SocNot->Cod         = -1L;
      SocNot->DateTimeUTC = (time_t) 0;
     }
  }

/*****************************************************************************/
/*************** Get data of social note using its code **********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialNoteFromRow (MYSQL_ROW row,struct SocialNote *SocNot)
  {
   /* Get social code (row[0]) */
   SocNot->NotCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Get note type (row[1]) */
   SocNot->NoteType = Soc_GetSocialNoteFromDB ((const char *) row[1]);

   /* Get (from) user code (row[2]) */
   SocNot->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get country code (row[3]) */
   SocNot->CtyCod = Str_ConvertStrCodToLongCod (row[3]);

   /* Get institution code (row[4]) */
   SocNot->InsCod = Str_ConvertStrCodToLongCod (row[4]);

   /* Get centre code (row[5]) */
   SocNot->CtrCod = Str_ConvertStrCodToLongCod (row[5]);

   /* Get degree code (row[6]) */
   SocNot->DegCod = Str_ConvertStrCodToLongCod (row[6]);

   /* Get course code (row[7]) */
   SocNot->CrsCod = Str_ConvertStrCodToLongCod (row[7]);

   /* Get file/post... code (row[8]) */
   SocNot->Cod = Str_ConvertStrCodToLongCod (row[8]);

   /* Get time of the note (row[9]) */
   SocNot->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[9]);
  }

/*****************************************************************************/
/****** Get social note type from string number coming from database *********/
/*****************************************************************************/

static Soc_NoteType_t Soc_GetSocialNoteFromDB (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Soc_NUM_SOCIAL_NOTES)
         return (Soc_NoteType_t) UnsignedNum;

   return Soc_NOTE_UNKNOWN;
  }
