// swad_pagination.c: pagination of assignments, forums and messages

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_agenda.h"
#include "swad_attendance.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_message.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_program.h"
#include "swad_project.h"
#include "swad_session_database.h"
#include "swad_survey.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;
extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
extern const Act_Action_t For_ActionsSeePstFor[For_NUM_TYPES_FORUM];

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Pag_ParNumPag[Pag_NUM_WHAT_PAGINATE] =
  {
   [Pag_NONE             ] = NULL,
   [Pag_ASSIGNMENTS      ] = "NumPagAsg",
   [Pag_PROJECTS         ] = "NumPagPrj",
   [Pag_EXAMS            ] = "NumPagExa",
   [Pag_GAMES            ] = "NumPagGam",
   [Pag_RUBRICS          ] = "NumPagRub",
   [Pag_ATT_EVENTS       ] = "NumPagAtt",
   [Pag_THREADS_FORUM    ] = "NumPagThr",
   [Pag_POSTS_FORUM      ] = "NumPagPst",
   [Pag_MESSAGES_RECEIVED] = "NumPagRcv",
   [Pag_MESSAGES_SENT    ] = "NumPagSnt",
   [Pag_SURVEYS          ] = "NumPagSvy",
   [Pag_MY_AGENDA        ] = "NumPagMyAgd",
   [Pag_ANOTHER_AGENDA   ] = "NumPagOthAgd",
  };

/*****************************************************************************/
/******* Calculate variables related to the pagination of the messages *******/
/*****************************************************************************/
// Return the number of subsets of N elements in a set of NumElements elements
#define Pag_NumPags(NumItems) ((Pagination->NumItems+(Pag_ITEMS_PER_PAGE-1)) / Pag_ITEMS_PER_PAGE)
#define Pag_NUM_PAGES_BEFORE_CURRENT 1
#define Pag_NUM_PAGES_AFTER_CURRENT 1

void Pag_CalculatePagination (struct Pag_Pagination *Pagination)
  {
   Pagination->StartPage =
   Pagination->LeftPage  =
   Pagination->RightPage =
   Pagination->EndPage   = 1;
   Pagination->MoreThanOnePage = false;
   Pagination->NumPags = ((Pagination->NumItems + (Pag_ITEMS_PER_PAGE-1)) / Pag_ITEMS_PER_PAGE);
   if (Pagination->NumPags > 1)
     {
      Pagination->MoreThanOnePage = true;

      /* If page to show is 0 (special code), then last page must be shown.
         If page to show is greater than number of pages, then show last page also */
      if (Pagination->CurrentPage == 0 ||
          Pagination->CurrentPage > Pagination->NumPags)
         Pagination->CurrentPage = Pagination->NumPags;

      /* Compute first page with link around the current */
      if (Pagination->CurrentPage <= Pag_NUM_PAGES_BEFORE_CURRENT)
         Pagination->StartPage = 1;
      else
         Pagination->StartPage = Pagination->CurrentPage - Pag_NUM_PAGES_BEFORE_CURRENT;

      /* Compute last page with link around the current */
      if ((Pagination->EndPage = Pagination->CurrentPage + Pag_NUM_PAGES_AFTER_CURRENT) > Pagination->NumPags)
         Pagination->EndPage = Pagination->NumPags;

      /* Compute left page with link in the middle of first page and current page */
      Pagination->LeftPage = (1 + Pagination->StartPage) / 2;

      /* Compute right page with link in the middle of current page and last page */
      Pagination->RightPage = (Pagination->EndPage + Pagination->NumPags) / 2;
     }
   else      // Only one page
      Pagination->CurrentPage = 1;  // If there is only a page, the number of page to show is 1

   Pagination->LastItemVisible = Pagination->CurrentPage * Pag_ITEMS_PER_PAGE;
   Pagination->FirstItemVisible = Pagination->LastItemVisible - (Pag_ITEMS_PER_PAGE - 1);
   if (Pagination->LastItemVisible > Pagination->NumItems)
      Pagination->LastItemVisible = Pagination->NumItems;

   /* Default anchor */
   Pagination->Anchor = NULL;
  }

/*****************************************************************************/
/************** Show enlaces a distintas páginas of messages *****************/
/*****************************************************************************/

void Pag_WriteLinksToPagesCentered (Pag_WhatPaginate_t WhatPaginate,
                                    struct Pag_Pagination *Pagination,
                                    const void *Context,long Cod)
  {
   if (Pagination->MoreThanOnePage)
     {
      HTM_DIV_Begin ("class=\"CM\"");
	 Pag_WriteLinksToPages (WhatPaginate,Pagination,Context,Cod,
				Cns_ENABLED,NULL,"PAG_TXT",false);	// !!!!!!!!!!!!!!!!!!!!!!!!!!
      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/********************** Show links to pages of messages **********************/
/*****************************************************************************/

void Pag_WriteLinksToPages (Pag_WhatPaginate_t WhatPaginate,
                            struct Pag_Pagination *Pagination,
                            const void *Context,long Cod,
                            Cns_Enabled_t FirstMsgEnabled,
			    const char *Subject,const char *ClassTxt,
                            bool LinkToPagCurrent)
  {
   extern const char *Txt_Page_X_of_Y;
   extern const char *Txt_FORUM_Post_banned;
   Grp_WhichGroups_t WhichGroups;
   unsigned NumPage;
   char *Title;

   /***** Link to page 1, including a text *****/
   if (Subject)
     {
      HTM_DIV_Begin (NULL);
	 if (LinkToPagCurrent)
	   {
	    switch (WhatPaginate)
	      {
	       case Pag_ASSIGNMENTS:
		  Frm_BeginFormAnchor (ActSeeAllAsg,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
		  break;
	       case Pag_PROJECTS:
		  Frm_BeginFormAnchor (ActSeeAllPrj,Pagination->Anchor);
		     Prj_PutPars (&((struct Prj_Projects *) Context)->Filter,
				   ((struct Prj_Projects *) Context)->SelectedOrder,
				   1,
				   Cod);
		  break;
	       case Pag_EXAMS:
		  Frm_BeginFormAnchor (ActSeeAllExa,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Par_PutParOrder ((unsigned) ((struct Exa_Exams *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
		  break;
	       case Pag_GAMES:
		  Frm_BeginFormAnchor (ActSeeAllGam,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Par_PutParOrder ((unsigned) ((struct Gam_Games *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
		  break;
	       case Pag_RUBRICS:
		  Frm_BeginFormAnchor (ActSeeAllRub,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		  break;
	       case Pag_ATT_EVENTS:
		  Frm_BeginFormAnchor (ActSeeAllAtt,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Par_PutParOrder ((unsigned) ((struct Att_Events *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
		  break;
	       case Pag_THREADS_FORUM:
		  Frm_BeginFormAnchor (For_ActionsSeeFor[((struct For_Forums *) Context)->Forum.Type],
				       Pagination->Anchor);
		     For_PutAllParsForum (1,	// Page of threads = first
					  1,	// Page of posts   = first
					  ((struct For_Forums *) Context)->ForumSet,
					  ((struct For_Forums *) Context)->ThreadsOrder,
					  ((struct For_Forums *) Context)->Forum.HieCod,
					  -1L,
					  -1L);
		  break;
	       case Pag_POSTS_FORUM:
		  Frm_BeginFormAnchor (For_ActionsSeePstFor[((struct For_Forums *) Context)->Forum.Type],
				       Pagination->Anchor);
		     For_PutAllParsForum (((struct For_Forums *) Context)->CurrentPageThrs,	// Page of threads = current
					  1,	// Page of posts   = first
					  ((struct For_Forums *) Context)->ForumSet,
					  ((struct For_Forums *) Context)->ThreadsOrder,
					  ((struct For_Forums *) Context)->Forum.HieCod,
					  Cod,
					  -1L);
		  break;
	       case Pag_MESSAGES_RECEIVED:
		  Frm_BeginFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
		  break;
	       case Pag_MESSAGES_SENT:
		  Frm_BeginFormAnchor (ActSeeSntMsg,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
		  break;
	       case Pag_SURVEYS:
		  Frm_BeginFormAnchor (ActSeeAllSvy,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Par_PutParOrder ((unsigned) ((struct Svy_Surveys *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
		  break;
	       case Pag_MY_AGENDA:
		  Frm_BeginFormAnchor (ActSeeMyAgd,Pagination->Anchor);
		     Agd_PutParsMyAgenda (((struct Agd_Agenda *) Context)->Past__FutureEvents,
					  ((struct Agd_Agenda *) Context)->PrivatPublicEvents,
					  ((struct Agd_Agenda *) Context)->HiddenVisiblEvents,
					  ((struct Agd_Agenda *) Context)->SelectedOrder,
					  1,
					  Cod);
		  break;
	       case Pag_ANOTHER_AGENDA:
		  Frm_BeginFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
		     Agd_PutParEventsOrder (((struct Agd_Agenda *) Context)->SelectedOrder);
		     Pag_PutParPagNum (WhatPaginate,1);
		     Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
		  break;
	       default:
		  break;
	      }
	    if (asprintf (&Title,Txt_Page_X_of_Y,1,Pagination->NumPags) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,
	                             "class=\"LT BT_LINK %s_%s\"",
	                             ClassTxt,The_GetSuffix ());
	    free (Title);
	   }
	 else
	    HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
	 switch (FirstMsgEnabled)
	   {
	    case Cns_DISABLED:
	       HTM_TxtF ("[%s]",Txt_FORUM_Post_banned);
	       break;
	    case Cns_ENABLED:
	       HTM_Txt (Subject);
	       break;
	   }
	 if (LinkToPagCurrent)
	   {
	    HTM_BUTTON_End ();
	    Frm_EndForm ();
	   }
	 else
	    HTM_SPAN_End ();
      HTM_DIV_End ();
     }

   /***** Links to several pages start here *****/
   if (Pagination->MoreThanOnePage)
     {
      /***** Possible link to page 1 *****/
      if (Pagination->StartPage > 1)
        {
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_BeginFormAnchor (ActSeeAllAsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_PROJECTS:
               Frm_BeginFormAnchor (ActSeeAllPrj,Pagination->Anchor);
		  Prj_PutPars (&((struct Prj_Projects *) Context)->Filter,
			        ((struct Prj_Projects *) Context)->SelectedOrder,
			        1,
				Cod);
               break;
            case Pag_EXAMS:
               Frm_BeginFormAnchor (ActSeeAllExa,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Par_PutParOrder ((unsigned) ((struct Exa_Exams *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_GAMES:
               Frm_BeginFormAnchor (ActSeeAllGam,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Par_PutParOrder ((unsigned) ((struct Gam_Games *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_RUBRICS:
               Frm_BeginFormAnchor (ActSeeAllRub,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
               break;
            case Pag_ATT_EVENTS:
               Frm_BeginFormAnchor (ActSeeAllAtt,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Par_PutParOrder ((unsigned) ((struct Att_Events *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_THREADS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeeFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (1,	// Page of threads = first
				       1,	// Page of posts   = first
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       -1L,
				       -1L);
	       break;
            case Pag_POSTS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeePstFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (((struct For_Forums *) Context)->CurrentPageThrs,	// Page of threads = current
				       1,	// Page of posts   = first
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       Cod,
				       -1L);
	       break;
            case Pag_MESSAGES_RECEIVED:
               Frm_BeginFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_MESSAGES_SENT:
               Frm_BeginFormAnchor (ActSeeSntMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_SURVEYS:
               Frm_BeginFormAnchor (ActSeeAllSvy,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Par_PutParOrder ((unsigned) ((struct Svy_Surveys *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_MY_AGENDA:
               Frm_BeginFormAnchor (ActSeeMyAgd,Pagination->Anchor);
		  Agd_PutParsMyAgenda (((struct Agd_Agenda *) Context)->Past__FutureEvents,
				       ((struct Agd_Agenda *) Context)->PrivatPublicEvents,
				       ((struct Agd_Agenda *) Context)->HiddenVisiblEvents,
				       ((struct Agd_Agenda *) Context)->SelectedOrder,
				       1,
				       Cod);
               break;
            case Pag_ANOTHER_AGENDA:
               Frm_BeginFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
		  Agd_PutParEventsOrder (((struct Agd_Agenda *) Context)->SelectedOrder);
		  Pag_PutParPagNum (WhatPaginate,1);
		  Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
               break;
            default:
               break;
           }
	    if (asprintf (&Title,Txt_Page_X_of_Y,1,Pagination->NumPags) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,
	                             "class=\"BT_LINK PAG PAG_%s %s_%s\"",
	                             The_GetSuffix (),
	                             ClassTxt,The_GetSuffix ());
	       HTM_Unsigned (1);
	    HTM_BUTTON_End ();
	    free (Title);
         Frm_EndForm ();
         if (Pagination->LeftPage > 2)
           {
            HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
	       HTM_Txt ("&hellip;");
            HTM_SPAN_End ();
           }
        }

      /***** Posible link to page left *****/
      if (Pagination->LeftPage > 1 &&
	  Pagination->LeftPage < Pagination->StartPage)
        {
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_BeginFormAnchor (ActSeeAllAsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_PROJECTS:
               Frm_BeginFormAnchor (ActSeeAllPrj,Pagination->Anchor);
		  Prj_PutPars (&((struct Prj_Projects *) Context)->Filter,
				((struct Prj_Projects *) Context)->SelectedOrder,
				Pagination->LeftPage,
				Cod);
               break;
            case Pag_EXAMS:
               Frm_BeginFormAnchor (ActSeeAllExa,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Par_PutParOrder ((unsigned) ((struct Exa_Exams *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_GAMES:
               Frm_BeginFormAnchor (ActSeeAllGam,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Par_PutParOrder ((unsigned) ((struct Gam_Games *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_RUBRICS:
               Frm_BeginFormAnchor (ActSeeAllRub,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
               break;
            case Pag_ATT_EVENTS:
               Frm_BeginFormAnchor (ActSeeAllAtt,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Par_PutParOrder ((unsigned) ((struct Att_Events *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_THREADS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeeFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (Pagination->LeftPage,	// Page of threads = left
				       1,			// Page of posts   = first
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       -1L,
				       -1L);
               break;
            case Pag_POSTS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeePstFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (((struct For_Forums *) Context)->CurrentPageThrs,	// Page of threads = current
				       Pagination->LeftPage,	// Page of posts   = left
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       Cod,
				       -1L);
	       break;
            case Pag_MESSAGES_RECEIVED:
               Frm_BeginFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_MESSAGES_SENT:
               Frm_BeginFormAnchor (ActSeeSntMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_SURVEYS:
               Frm_BeginFormAnchor (ActSeeAllSvy,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Par_PutParOrder ((unsigned) ((struct Svy_Surveys *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_MY_AGENDA:
               Frm_BeginFormAnchor (ActSeeMyAgd,Pagination->Anchor);
		  Agd_PutParsMyAgenda (((struct Agd_Agenda *) Context)->Past__FutureEvents,
				       ((struct Agd_Agenda *) Context)->PrivatPublicEvents,
				       ((struct Agd_Agenda *) Context)->HiddenVisiblEvents,
				       ((struct Agd_Agenda *) Context)->SelectedOrder,
				       Pagination->LeftPage,
				       Cod);
               break;
            case Pag_ANOTHER_AGENDA:
               Frm_BeginFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
		  Agd_PutParEventsOrder (((struct Agd_Agenda *) Context)->SelectedOrder);
		  Pag_PutParPagNum (WhatPaginate,Pagination->LeftPage);
		  Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
               break;
            default:
               break;
           }
	    if (asprintf (&Title,Txt_Page_X_of_Y,
			  Pagination->LeftPage,Pagination->NumPags) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,
	                             "class=\"BT_LINK PAG PAG_%s %s_%s\"",
	                             The_GetSuffix (),
	                             ClassTxt,The_GetSuffix ());
	       HTM_Unsigned (Pagination->LeftPage);
	    HTM_BUTTON_End ();
	    free (Title);
         Frm_EndForm ();
         if (Pagination->LeftPage < Pagination->StartPage - 1)
           {
            HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
	       HTM_Txt ("&hellip;");
            HTM_SPAN_End ();
           }
        }

      /***** Loop to put links to the pages around the current one *****/
      for (NumPage = Pagination->StartPage;
	   NumPage <= Pagination->EndPage;
	   NumPage++)
        {
         if (asprintf (&Title,Txt_Page_X_of_Y,NumPage,Pagination->NumPags) < 0)
	    Err_NotEnoughMemoryExit ();
         if (!LinkToPagCurrent && NumPage == Pagination->CurrentPage)
           {
            HTM_SPAN_Begin ("title=\"%s\" class=\"PAG_CUR PAG_CUR_%s %s_%s\"",
                            Title,The_GetSuffix (),ClassTxt,The_GetSuffix ());
	       HTM_Unsigned (NumPage);
            HTM_SPAN_End ();
           }
         else
           {
            switch (WhatPaginate)
              {
               case Pag_ASSIGNMENTS:
                  Frm_BeginFormAnchor (ActSeeAllAsg,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
                  break;
	       case Pag_PROJECTS:
		  Frm_BeginFormAnchor (ActSeeAllPrj,Pagination->Anchor);
		     Prj_PutPars (&((struct Prj_Projects *) Context)->Filter,
				   ((struct Prj_Projects *) Context)->SelectedOrder,
				   NumPage,
				   Cod);
		  break;
               case Pag_EXAMS:
                  Frm_BeginFormAnchor (ActSeeAllExa,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Par_PutParOrder ((unsigned) ((struct Exa_Exams *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
                  break;
               case Pag_GAMES:
                  Frm_BeginFormAnchor (ActSeeAllGam,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Par_PutParOrder ((unsigned) ((struct Gam_Games *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
                  break;
               case Pag_RUBRICS:
                  Frm_BeginFormAnchor (ActSeeAllRub,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
                  break;
               case Pag_ATT_EVENTS:
                  Frm_BeginFormAnchor (ActSeeAllAtt,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Par_PutParOrder ((unsigned) ((struct Att_Events *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
                  break;
               case Pag_THREADS_FORUM:
                  Frm_BeginFormAnchor (For_ActionsSeeFor[((struct For_Forums *) Context)->Forum.Type],
                                       Pagination->Anchor);
		     For_PutAllParsForum (NumPage,	// Page of threads = number of page
					  1,		// Page of posts   = first
					  ((struct For_Forums *) Context)->ForumSet,
					  ((struct For_Forums *) Context)->ThreadsOrder,
					  ((struct For_Forums *) Context)->Forum.HieCod,
					  -1L,
					  -1L);
                  break;
               case Pag_POSTS_FORUM:
                  Frm_BeginFormAnchor (For_ActionsSeePstFor[((struct For_Forums *) Context)->Forum.Type],
                                       Pagination->Anchor);
		     For_PutAllParsForum (((struct For_Forums *) Context)->CurrentPageThrs,	// Page of threads = current
					  NumPage,	// Page of posts   = number of page
					  ((struct For_Forums *) Context)->ForumSet,
					  ((struct For_Forums *) Context)->ThreadsOrder,
					  ((struct For_Forums *) Context)->Forum.HieCod,
					  Cod,
					  -1L);
                  break;
               case Pag_MESSAGES_RECEIVED:
                  Frm_BeginFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
                  break;
               case Pag_MESSAGES_SENT:
                  Frm_BeginFormAnchor (ActSeeSntMsg,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
                  break;
               case Pag_SURVEYS:
                  Frm_BeginFormAnchor (ActSeeAllSvy,Pagination->Anchor);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Par_PutParOrder ((unsigned) ((struct Svy_Surveys *) Context)->SelectedOrder);
		     WhichGroups = Grp_GetParWhichGroups ();
		     Grp_PutParWhichGroups (&WhichGroups);
                  break;
               case Pag_MY_AGENDA:
                  Frm_BeginFormAnchor (ActSeeMyAgd,Pagination->Anchor);
		     Agd_PutParsMyAgenda (((struct Agd_Agenda *) Context)->Past__FutureEvents,
					  ((struct Agd_Agenda *) Context)->PrivatPublicEvents,
					  ((struct Agd_Agenda *) Context)->HiddenVisiblEvents,
					  ((struct Agd_Agenda *) Context)->SelectedOrder,
					  NumPage,
					  Cod);
                  break;
               case Pag_ANOTHER_AGENDA:
                  Frm_BeginFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
		     Agd_PutParEventsOrder (((struct Agd_Agenda *) Context)->SelectedOrder);
		     Pag_PutParPagNum (WhatPaginate,NumPage);
		     Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
                  break;
	       default:
		  break;
              }
	       HTM_BUTTON_Submit_Begin (Title,
	                                "class=\"BT_LINK PAG PAG_%s %s_%s\"",
	                                The_GetSuffix (),
	                                ClassTxt,The_GetSuffix ());
		  HTM_Unsigned (NumPage);
	       HTM_BUTTON_End ();
            Frm_EndForm ();
           }
         free (Title);
        }

      /***** Posible link to page right *****/
      if (Pagination->RightPage > Pagination->EndPage &&
	  Pagination->RightPage < Pagination->NumPags)
        {
         if (Pagination->RightPage > Pagination->EndPage + 1)
           {
            HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
	       HTM_Txt ("&hellip;");
            HTM_SPAN_End ();
           }
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_BeginFormAnchor (ActSeeAllAsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
	    case Pag_PROJECTS:
	       Frm_BeginFormAnchor (ActSeeAllPrj,Pagination->Anchor);
		  Prj_PutPars (&((struct Prj_Projects *) Context)->Filter,
				((struct Prj_Projects *) Context)->SelectedOrder,
				Pagination->RightPage,
				Cod);
	       break;
            case Pag_EXAMS:
               Frm_BeginFormAnchor (ActSeeAllExa,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Par_PutParOrder ((unsigned) ((struct Exa_Exams *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_GAMES:
               Frm_BeginFormAnchor (ActSeeAllGam,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Par_PutParOrder ((unsigned) ((struct Gam_Games *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_RUBRICS:
               Frm_BeginFormAnchor (ActSeeAllRub,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
               break;
            case Pag_ATT_EVENTS:
               Frm_BeginFormAnchor (ActSeeAllAtt,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Par_PutParOrder ((unsigned) ((struct Att_Events *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_THREADS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeeFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (Pagination->RightPage,	// Page of threads = right
				       1,			// Page of posts   = first
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       -1L,
				       -1L);
	       break;
            case Pag_POSTS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeePstFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (((struct For_Forums *) Context)->CurrentPageThrs,	// Page of threads = current
				       Pagination->RightPage,	// Page of posts   = right
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       Cod,
				       -1L);
               break;
            case Pag_MESSAGES_RECEIVED:
               Frm_BeginFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_MESSAGES_SENT:
               Frm_BeginFormAnchor (ActSeeSntMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_SURVEYS:
               Frm_BeginFormAnchor (ActSeeAllSvy,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Par_PutParOrder ((unsigned) ((struct Svy_Surveys *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
	    case Pag_MY_AGENDA:
	       Frm_BeginFormAnchor (ActSeeMyAgd,Pagination->Anchor);
		  Agd_PutParsMyAgenda (((struct Agd_Agenda *) Context)->Past__FutureEvents,
				       ((struct Agd_Agenda *) Context)->PrivatPublicEvents,
				       ((struct Agd_Agenda *) Context)->HiddenVisiblEvents,
				       ((struct Agd_Agenda *) Context)->SelectedOrder,
				       Pagination->RightPage,
				       Cod);
	       break;
	    case Pag_ANOTHER_AGENDA:
	       Frm_BeginFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
		  Agd_PutParEventsOrder (((struct Agd_Agenda *) Context)->SelectedOrder);
		  Pag_PutParPagNum (WhatPaginate,Pagination->RightPage);
		  Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
	       break;
            default:
               break;
           }
	    if (asprintf (&Title,Txt_Page_X_of_Y,
			  Pagination->RightPage,Pagination->NumPags) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,
	                             "class=\"BT_LINK PAG PAG_%s %s_%s\"",
	                             The_GetSuffix (),
	                             ClassTxt,The_GetSuffix ());
	       HTM_Unsigned (Pagination->RightPage);
	    HTM_BUTTON_End ();
	    free (Title);
         Frm_EndForm ();
        }

      /***** Possible link to last page *****/
      if (Pagination->EndPage < Pagination->NumPags)
        {
         if (Pagination->NumPags > Pagination->RightPage + 1)
           {
            HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
	       HTM_Txt ("&hellip;");
            HTM_SPAN_End ();
           }
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_BeginFormAnchor (ActSeeAllAsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
	    case Pag_PROJECTS:
	       Frm_BeginFormAnchor (ActSeeAllPrj,Pagination->Anchor);
		  Prj_PutPars (&((struct Prj_Projects *) Context)->Filter,
				((struct Prj_Projects *) Context)->SelectedOrder,
				Pagination->NumPags,
				Cod);
	       break;
            case Pag_EXAMS:
               Frm_BeginFormAnchor (ActSeeAllExa,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Par_PutParOrder ((unsigned) ((struct Exa_Exams *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_GAMES:
               Frm_BeginFormAnchor (ActSeeAllGam,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Par_PutParOrder ((unsigned) ((struct Gam_Games *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_RUBRICS:
               Frm_BeginFormAnchor (ActSeeAllRub,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
               break;
            case Pag_ATT_EVENTS:
               Frm_BeginFormAnchor (ActSeeAllAtt,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Par_PutParOrder ((unsigned) ((struct Att_Events *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
            case Pag_THREADS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeeFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (Pagination->NumPags,	// Page of threads = last
				       1,			// Page of posts   = first
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       -1L,
				       -1L);
               break;
            case Pag_POSTS_FORUM:
               Frm_BeginFormAnchor (For_ActionsSeePstFor[((struct For_Forums *) Context)->Forum.Type],
                                    Pagination->Anchor);
		  For_PutAllParsForum (((struct For_Forums *) Context)->CurrentPageThrs,	// Page of threads = current
				       Pagination->NumPags,	// Page of posts   = last
				       ((struct For_Forums *) Context)->ForumSet,
				       ((struct For_Forums *) Context)->ThreadsOrder,
				       ((struct For_Forums *) Context)->Forum.HieCod,
				       Cod,
				       -1L);
               break;
            case Pag_MESSAGES_RECEIVED:
               Frm_BeginFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_MESSAGES_SENT:
               Frm_BeginFormAnchor (ActSeeSntMsg,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Msg_PutParsMsgsFilters ((struct Msg_Messages *) Context);
               break;
            case Pag_SURVEYS:
               Frm_BeginFormAnchor (ActSeeAllSvy,Pagination->Anchor);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Par_PutParOrder ((unsigned) ((struct Svy_Surveys *) Context)->SelectedOrder);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
               break;
	    case Pag_MY_AGENDA:
	       Frm_BeginFormAnchor (ActSeeMyAgd,Pagination->Anchor);
		  Agd_PutParsMyAgenda (((struct Agd_Agenda *) Context)->Past__FutureEvents,
				       ((struct Agd_Agenda *) Context)->PrivatPublicEvents,
				       ((struct Agd_Agenda *) Context)->HiddenVisiblEvents,
				       ((struct Agd_Agenda *) Context)->SelectedOrder,
				       Pagination->NumPags,
				       Cod);
	       break;
	    case Pag_ANOTHER_AGENDA:
	       Frm_BeginFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
		  Agd_PutParEventsOrder (((struct Agd_Agenda *) Context)->SelectedOrder);
		  Pag_PutParPagNum (WhatPaginate,Pagination->NumPags);
		  Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
	       break;
            default:
               break;
           }
	    if (asprintf (&Title,Txt_Page_X_of_Y,
			  Pagination->NumPags,Pagination->NumPags) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,
	                             "class=\"BT_LINK PAG PAG_%s %s_%s\"",
	                             The_GetSuffix (),
	                             ClassTxt,The_GetSuffix ());
	       HTM_Unsigned (Pagination->NumPags);
	    HTM_BUTTON_End ();
	    free (Title);
         Frm_EndForm ();
        }
     }
  }

/*****************************************************************************/
/******************* Put hidden parameter number of page *********************/
/*****************************************************************************/

void Pag_PutParPagNum (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage)
  {
   Par_PutParUnsigned (NULL,Pag_ParNumPag[WhatPaginate],NumPage);
  }

/*****************************************************************************/
/************************ Get parameter number of page ***********************/
/*****************************************************************************/

unsigned Pag_GetParPagNum (Pag_WhatPaginate_t WhatPaginate)
  {
   unsigned CurrentPage;

   /***** Try to get parameter with the number of page *****/
   CurrentPage = (unsigned) Par_GetParUnsignedLong (Pag_ParNumPag[WhatPaginate],
						    1,
						    UINT_MAX,
						    0);
   if (CurrentPage == 0)
      /***** If there's no parameter page, return a default value *****/
      switch (WhatPaginate)
	{
	 case Pag_MESSAGES_RECEIVED:
	    if (Gbl.Action.Act == ActExpRcvMsg)
	       /* Expanding a message, perhaps it is the result of following a link
	          from a notification of received message */
	       /* Show the page corresponding to the expanded message */
	       return 1;	// Now set the current page to the first,
				// but later the correct page will be calculated

	    /* Show the last visited page */
	    return Pag_GetLastPageMsgFromSession (Pag_MESSAGES_RECEIVED);
	 case Pag_MESSAGES_SENT:
	    /* Show the last visited page */
	    return Pag_GetLastPageMsgFromSession (Pag_MESSAGES_SENT);
	 default:
	    return 1;
	}

   return CurrentPage;
  }

/*****************************************************************************/
/********* Get last page of received/sent messages stored in session *********/
/*****************************************************************************/

unsigned Pag_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate)
  {
   unsigned NumPage;

   switch (WhatPaginate)
     {
      case Pag_MESSAGES_RECEIVED:
      case Pag_MESSAGES_SENT:
	 /***** Get last page of received/sent messages from database *****/
	 if ((NumPage = Ses_DB_GetLastPageMsgFromSession (WhatPaginate)) == 0)
	    return 1;
	 return NumPage;
      default:
	 return 1;
     }
  }
