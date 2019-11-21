// swad_pagination.c: pagination of assignments, forums and messages

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
/********************************** Headers **********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_project.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;
extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
extern const Act_Action_t For_ActionsSeePstFor[For_NUM_TYPES_FORUM];

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Pag_ParamNumPag[Pag_NUM_WHAT_PAGINATE] =
  {
   [Pag_ASSIGNMENTS      ] = "NumPagAsg",
   [Pag_PROJECTS         ] = "NumPagPrj",
   [Pag_GAMES            ] = "NumPagGam",
   [Pag_SURVEYS          ] = "NumPagSvy",
   [Pag_ATT_EVENTS       ] = "NumPagAtt",
   [Pag_THREADS_FORUM    ] = "NumPagThr",
   [Pag_POSTS_FORUM      ] = "NumPagPst",
   [Pag_MESSAGES_RECEIVED] = "NumPagRcv",
   [Pag_MESSAGES_SENT    ] = "NumPagSnt",
   [Pag_MY_AGENDA        ] = "NumPagMyAgd",
   [Pag_ANOTHER_AGENDA   ] = "NumPagOthAgd",
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******* Calculate variables related to the pagination of the messages *******/
/*****************************************************************************/
// Return the number of subsets of N elements in a set of NumElements elements
#define NumSubsetsOfNElements(NumElements,N) ((NumElements+(N-1)) / N)
#define NUM_PAGES_BEFORE_CURRENT 1
#define NUM_PAGES_AFTER_CURRENT 1

void Pag_CalculatePagination (struct Pagination *Pagination)
  {
   Pagination->StartPage =
   Pagination->LeftPage  =
   Pagination->RightPage =
   Pagination->EndPage   = 1;
   Pagination->MoreThanOnePage = false;
   if ((Pagination->NumPags = NumSubsetsOfNElements (Pagination->NumItems,Pag_ITEMS_PER_PAGE)) > 1)
     {
      Pagination->MoreThanOnePage = true;

      /* If page to show is 0 (special code), then last page must be shown.
         If page to show is greater than number of pages, then show last page also */
      if (Pagination->CurrentPage == 0 ||
          Pagination->CurrentPage > Pagination->NumPags)
         Pagination->CurrentPage = Pagination->NumPags;

      /* Compute first page with link around the current */
      if (Pagination->CurrentPage <= NUM_PAGES_BEFORE_CURRENT)
         Pagination->StartPage = 1;
      else
         Pagination->StartPage = Pagination->CurrentPage - NUM_PAGES_BEFORE_CURRENT;

      /* Compute last page with link around the current */
      if ((Pagination->EndPage = Pagination->CurrentPage + NUM_PAGES_AFTER_CURRENT) > Pagination->NumPags)
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
                                    long ThrCod,
                                    struct Pagination *Pagination)
  {
   HTM_DIV_Begin ("class=\"CM\"");
   Pag_WriteLinksToPages (WhatPaginate,
                          ThrCod,
                          Pagination,true,NULL,"PAG_TXT",false);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Show links to pages of messages **********************/
/*****************************************************************************/

void Pag_WriteLinksToPages (Pag_WhatPaginate_t WhatPaginate,
                            long ThrCod,
                            struct Pagination *Pagination,
                            bool FirstMsgEnabled,
			    const char *Subject,const char *ClassTxt,
                            bool LinkToPagCurrent)
  {
   extern const char *Txt_Page_X_of_Y;
   extern const char *Txt_FORUM_Post_banned;
   unsigned NumPage;
   char *ClassLink;

   /***** Link to page 1, including a text *****/
   if (Subject)
     {
      HTM_DIV_Begin (NULL);
      if (LinkToPagCurrent)
        {
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_StartFormAnchor (ActSeeAsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_PROJECTS:
               Frm_StartFormAnchor (ActSeePrj,Pagination->Anchor);
               Prj_PutParams (&Gbl.Prjs.Filter,
                              Gbl.Prjs.SelectedOrder,
                              1,
                              -1L);
               break;
            case Pag_GAMES:
               Frm_StartFormAnchor (ActSeeAllGam,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_SURVEYS:
               Frm_StartFormAnchor (ActSeeAllSvy,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Frm_StartFormAnchor (ActSeeAtt,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Att_PutHiddenParamAttOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_THREADS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                            1,	// Page of posts   = first
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    -1L,
					    -1L);
               break;
            case Pag_POSTS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeePstFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                            1,				// Page of posts   = first
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    ThrCod,
					    -1L);
	       break;
            case Pag_MESSAGES_RECEIVED:
               Frm_StartFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Frm_StartFormAnchor (ActSeeSntMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MY_AGENDA:
               Frm_StartFormAnchor (ActSeeMyAgd,Pagination->Anchor);
               Agd_PutParamsMyAgenda (Gbl.Agenda.Past__FutureEvents,
                                      Gbl.Agenda.PrivatPublicEvents,
                                      Gbl.Agenda.HiddenVisiblEvents,
			              Gbl.Agenda.SelectedOrder,
                                      1,
                                      -1L);
               break;
            case Pag_ANOTHER_AGENDA:
               Frm_StartFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
               Agd_PutHiddenParamEventsOrder ();
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Usr_PutParamOtherUsrCodEncrypted ();
               break;
           }
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Page_X_of_Y,
                   1,Pagination->NumPags);
	 if (asprintf (&ClassLink,"BT_LINK LT %s",ClassTxt) < 0)
	    Lay_NotEnoughMemoryExit ();
         HTM_BUTTON_SUBMIT_Begin (Gbl.Title,ClassLink,NULL);
         free (ClassLink);
        }
      else
         HTM_SPAN_Begin ("class=\"%s\"",ClassTxt);
      if (FirstMsgEnabled)
         HTM_Txt (Subject);
      else
         HTM_TxtF ("[%s]",Txt_FORUM_Post_banned);
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
      if (asprintf (&ClassLink,"BT_LINK PAG %s",ClassTxt) < 0)
	 Lay_NotEnoughMemoryExit ();

      /***** Possible link to page 1 *****/
      if (Pagination->StartPage > 1)
        {
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_StartFormAnchor (ActSeeAsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_PROJECTS:
               Frm_StartFormAnchor (ActSeePrj,Pagination->Anchor);
               Prj_PutParams (&Gbl.Prjs.Filter,
                              Gbl.Prjs.SelectedOrder,
                              1,
                              -1L);
               break;
            case Pag_GAMES:
               Frm_StartFormAnchor (ActSeeAllGam,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_SURVEYS:
               Frm_StartFormAnchor (ActSeeAllSvy,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Frm_StartFormAnchor (ActSeeAtt,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Att_PutHiddenParamAttOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_THREADS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                            1,	// Page of posts   = first
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    -1L,
					    -1L);
	       break;
            case Pag_POSTS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeePstFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                            1,				// Page of posts   = first
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    ThrCod,
					    -1L);
	       break;
            case Pag_MESSAGES_RECEIVED:
               Frm_StartFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Frm_StartFormAnchor (ActSeeSntMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MY_AGENDA:
               Frm_StartFormAnchor (ActSeeMyAgd,Pagination->Anchor);
               Agd_PutParamsMyAgenda (Gbl.Agenda.Past__FutureEvents,
                                      Gbl.Agenda.PrivatPublicEvents,
                                      Gbl.Agenda.HiddenVisiblEvents,
			              Gbl.Agenda.SelectedOrder,
                                      1,
                                      -1L);
               break;
            case Pag_ANOTHER_AGENDA:
               Frm_StartFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
               Agd_PutHiddenParamEventsOrder ();
               Pag_PutHiddenParamPagNum (WhatPaginate,1);
               Usr_PutParamOtherUsrCodEncrypted ();
               break;
           }
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Page_X_of_Y,
                   1,Pagination->NumPags);
         HTM_BUTTON_SUBMIT_Begin (Gbl.Title,ClassLink,NULL);
         HTM_Unsigned (1);
         HTM_BUTTON_End ();
         Frm_EndForm ();
         if (Pagination->LeftPage > 2)
           {
            HTM_SPAN_Begin ("class=\"%s\"",ClassTxt);
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
               Frm_StartFormAnchor (ActSeeAsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->LeftPage);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_PROJECTS:
               Frm_StartFormAnchor (ActSeePrj,Pagination->Anchor);
               Prj_PutParams (&Gbl.Prjs.Filter,
                              Gbl.Prjs.SelectedOrder,
                              Pagination->LeftPage,
                              -1L);
               break;
            case Pag_GAMES:
               Frm_StartFormAnchor (ActSeeAllGam,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->LeftPage);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_SURVEYS:
               Frm_StartFormAnchor (ActSeeAllSvy,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->LeftPage);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Frm_StartFormAnchor (ActSeeAtt,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->LeftPage);
               Att_PutHiddenParamAttOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_THREADS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Pagination->LeftPage,	// Page of threads = left
                                            1,				// Page of posts   = first
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    -1L,
					    -1L);
               break;
            case Pag_POSTS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeePstFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                            Pagination->LeftPage,	// Page of posts   = left
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    ThrCod,
					    -1L);
	       break;
            case Pag_MESSAGES_RECEIVED:
               Frm_StartFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->LeftPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Frm_StartFormAnchor (ActSeeSntMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->LeftPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MY_AGENDA:
               Frm_StartFormAnchor (ActSeeMyAgd,Pagination->Anchor);
               Agd_PutParamsMyAgenda (Gbl.Agenda.Past__FutureEvents,
                                      Gbl.Agenda.PrivatPublicEvents,
                                      Gbl.Agenda.HiddenVisiblEvents,
			              Gbl.Agenda.SelectedOrder,
                                      Pagination->LeftPage,
                                      -1L);
               break;
            case Pag_ANOTHER_AGENDA:
               Frm_StartFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
               Agd_PutHiddenParamEventsOrder ();
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->LeftPage);
               Usr_PutParamOtherUsrCodEncrypted ();
               break;
           }
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Page_X_of_Y,
                   Pagination->LeftPage,
                   Pagination->NumPags);
         HTM_BUTTON_SUBMIT_Begin (Gbl.Title,ClassLink,NULL);
         HTM_Unsigned (Pagination->LeftPage);
         HTM_BUTTON_End ();
         Frm_EndForm ();
         if (Pagination->LeftPage < Pagination->StartPage - 1)
           {
            HTM_SPAN_Begin ("class=\"%s\"",ClassTxt);
            HTM_Txt ("&hellip;");
            HTM_SPAN_End ();
           }
        }

      /***** Loop to put links to the pages around the current one *****/
      for (NumPage = Pagination->StartPage;
	   NumPage <= Pagination->EndPage;
	   NumPage++)
        {
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Page_X_of_Y,
		   NumPage,Pagination->NumPags);
         if (!LinkToPagCurrent && NumPage == Pagination->CurrentPage)
           {
            HTM_SPAN_Begin ("title=\"%s\" class=\"PAG_CUR %s\"",Gbl.Title,ClassTxt);
            HTM_Unsigned (NumPage);
            HTM_SPAN_End ();
           }
         else
           {
            switch (WhatPaginate)
              {
               case Pag_ASSIGNMENTS:
                  Frm_StartFormAnchor (ActSeeAsg,Pagination->Anchor);
                  Pag_PutHiddenParamPagNum (WhatPaginate,NumPage);
                  Asg_PutHiddenParamAsgOrder ();
                  Grp_PutParamWhichGrps ();
                  break;
	       case Pag_PROJECTS:
		  Frm_StartFormAnchor (ActSeePrj,Pagination->Anchor);
		  Prj_PutParams (&Gbl.Prjs.Filter,
                                 Gbl.Prjs.SelectedOrder,
                                 NumPage,
                                 -1L);
		  break;
               case Pag_GAMES:
                  Frm_StartFormAnchor (ActSeeAllGam,Pagination->Anchor);
                  Pag_PutHiddenParamPagNum (WhatPaginate,NumPage);
                  Asg_PutHiddenParamAsgOrder ();
                  Grp_PutParamWhichGrps ();
                  break;
               case Pag_SURVEYS:
                  Frm_StartFormAnchor (ActSeeAllSvy,Pagination->Anchor);
                  Pag_PutHiddenParamPagNum (WhatPaginate,NumPage);
                  Asg_PutHiddenParamAsgOrder ();
                  Grp_PutParamWhichGrps ();
                  break;
               case Pag_ATT_EVENTS:
                  Frm_StartFormAnchor (ActSeeAtt,Pagination->Anchor);
                  Pag_PutHiddenParamPagNum (WhatPaginate,NumPage);
                  Att_PutHiddenParamAttOrder ();
                  Grp_PutParamWhichGrps ();
                  break;
               case Pag_THREADS_FORUM:
                  Frm_StartFormAnchor (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                                       Pagination->Anchor);
		  For_PutAllHiddenParamsForum (NumPage,	// Page of threads = number of page
                                               1,	// Page of posts   = first
                                               Gbl.Forum.ForumSet,
					       Gbl.Forum.ThreadsOrder,
					       Gbl.Forum.ForumSelected.Location,
					       -1L,
					       -1L);
                  break;
               case Pag_POSTS_FORUM:
                  Frm_StartFormAnchor (For_ActionsSeePstFor[Gbl.Forum.ForumSelected.Type],
                                       Pagination->Anchor);
		  For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                               NumPage,				// Page of posts   = number of page
                                               Gbl.Forum.ForumSet,
					       Gbl.Forum.ThreadsOrder,
					       Gbl.Forum.ForumSelected.Location,
					       ThrCod,
					       -1L);
                  break;
               case Pag_MESSAGES_RECEIVED:
                  Frm_StartFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
                  Pag_PutHiddenParamPagNum (WhatPaginate,NumPage);
                  Msg_PutHiddenParamsMsgsFilters ();
                  break;
               case Pag_MESSAGES_SENT:
                  Frm_StartFormAnchor (ActSeeSntMsg,Pagination->Anchor);
                  Pag_PutHiddenParamPagNum (WhatPaginate,NumPage);
                  Msg_PutHiddenParamsMsgsFilters ();
                  break;
               case Pag_MY_AGENDA:
                  Frm_StartFormAnchor (ActSeeMyAgd,Pagination->Anchor);
                  Agd_PutParamsMyAgenda (Gbl.Agenda.Past__FutureEvents,
                                         Gbl.Agenda.PrivatPublicEvents,
                                         Gbl.Agenda.HiddenVisiblEvents,
			                 Gbl.Agenda.SelectedOrder,
                                         NumPage,
                                         -1L);
                  break;
               case Pag_ANOTHER_AGENDA:
                  Frm_StartFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
                  Agd_PutHiddenParamEventsOrder ();
                  Pag_PutHiddenParamPagNum (WhatPaginate,NumPage);
                  Usr_PutParamOtherUsrCodEncrypted ();
                  break;
              }
            HTM_BUTTON_SUBMIT_Begin (Gbl.Title,ClassLink,NULL);
            HTM_Unsigned (NumPage);
            HTM_BUTTON_End ();
            Frm_EndForm ();
           }
        }

      /***** Posible link to page right *****/
      if (Pagination->RightPage > Pagination->EndPage &&
	  Pagination->RightPage < Pagination->NumPags)
        {
         if (Pagination->RightPage > Pagination->EndPage + 1)
           {
            HTM_SPAN_Begin ("class=\"%s\"",ClassTxt);
            HTM_Txt ("&hellip;");
            HTM_SPAN_End ();
           }
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_StartFormAnchor (ActSeeAsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->RightPage);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
	    case Pag_PROJECTS:
	       Frm_StartFormAnchor (ActSeePrj,Pagination->Anchor);
	       Prj_PutParams (&Gbl.Prjs.Filter,
                              Gbl.Prjs.SelectedOrder,
			      Pagination->RightPage,
			      -1L);
	       break;
            case Pag_GAMES:
               Frm_StartFormAnchor (ActSeeAllGam,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->RightPage);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_SURVEYS:
               Frm_StartFormAnchor (ActSeeAllSvy,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->RightPage);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Frm_StartFormAnchor (ActSeeAtt,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->RightPage);
               Att_PutHiddenParamAttOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_THREADS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Pagination->RightPage,	// Page of threads = right
                                            1,				// Page of posts   = first
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    -1L,
					    -1L);
	       break;
            case Pag_POSTS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeePstFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                            Pagination->RightPage,	// Page of posts   = right
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    ThrCod,
					    -1L);
               break;
            case Pag_MESSAGES_RECEIVED:
               Frm_StartFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->RightPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Frm_StartFormAnchor (ActSeeSntMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->RightPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
	    case Pag_MY_AGENDA:
	       Frm_StartFormAnchor (ActSeeMyAgd,Pagination->Anchor);
	       Agd_PutParamsMyAgenda (Gbl.Agenda.Past__FutureEvents,
	                              Gbl.Agenda.PrivatPublicEvents,
	                              Gbl.Agenda.HiddenVisiblEvents,
			              Gbl.Agenda.SelectedOrder,
	                              Pagination->RightPage,
	                              -1L);
	       break;
	    case Pag_ANOTHER_AGENDA:
	       Frm_StartFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
	       Agd_PutHiddenParamEventsOrder ();
	       Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->RightPage);
	       Usr_PutParamOtherUsrCodEncrypted ();
	       break;
           }
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Page_X_of_Y,
                   Pagination->RightPage,Pagination->NumPags);
         HTM_BUTTON_SUBMIT_Begin (Gbl.Title,ClassLink,NULL);
         HTM_Unsigned (Pagination->RightPage);
         HTM_BUTTON_End ();
         Frm_EndForm ();
        }

      /***** Possible link to last page *****/
      if (Pagination->EndPage < Pagination->NumPags)
        {
         if (Pagination->NumPags > Pagination->RightPage + 1)
           {
            HTM_SPAN_Begin ("class=\"%s\"",ClassTxt);
            HTM_Txt ("&hellip;");
            HTM_SPAN_End ();
           }
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Frm_StartFormAnchor (ActSeeAsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->NumPags);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
	    case Pag_PROJECTS:
	       Frm_StartFormAnchor (ActSeePrj,Pagination->Anchor);
	       Prj_PutParams (&Gbl.Prjs.Filter,
                              Gbl.Prjs.SelectedOrder,
			      Pagination->NumPags,
			      -1L);
	       break;
            case Pag_GAMES:
               Frm_StartFormAnchor (ActSeeAllGam,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->NumPags);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_SURVEYS:
               Frm_StartFormAnchor (ActSeeAllSvy,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->NumPags);
               Asg_PutHiddenParamAsgOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Frm_StartFormAnchor (ActSeeAtt,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->NumPags);
               Att_PutHiddenParamAttOrder ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_THREADS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Pagination->NumPags,	// Page of threads = last
                                            1,				// Page of posts   = first
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    -1L,
					    -1L);
               break;
            case Pag_POSTS_FORUM:
               Frm_StartFormAnchor (For_ActionsSeePstFor[Gbl.Forum.ForumSelected.Type],
                                    Pagination->Anchor);
	       For_PutAllHiddenParamsForum (Gbl.Forum.CurrentPageThrs,	// Page of threads = current
                                            Pagination->NumPags,	// Page of posts   = last
                                            Gbl.Forum.ForumSet,
					    Gbl.Forum.ThreadsOrder,
					    Gbl.Forum.ForumSelected.Location,
					    ThrCod,
					    -1L);
               break;
            case Pag_MESSAGES_RECEIVED:
               Frm_StartFormAnchor (ActSeeRcvMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->NumPags);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Frm_StartFormAnchor (ActSeeSntMsg,Pagination->Anchor);
               Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->NumPags);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
	    case Pag_MY_AGENDA:
	       Frm_StartFormAnchor (ActSeeMyAgd,Pagination->Anchor);
	       Agd_PutParamsMyAgenda (Gbl.Agenda.Past__FutureEvents,
	                              Gbl.Agenda.PrivatPublicEvents,
	                              Gbl.Agenda.HiddenVisiblEvents,
			              Gbl.Agenda.SelectedOrder,
	                              Pagination->NumPags,
	                              -1L);
	       break;
	    case Pag_ANOTHER_AGENDA:
	       Frm_StartFormAnchor (ActSeeUsrAgd,Pagination->Anchor);
	       Agd_PutHiddenParamEventsOrder ();
	       Pag_PutHiddenParamPagNum (WhatPaginate,Pagination->NumPags);
	       Usr_PutParamOtherUsrCodEncrypted ();
	       break;
           }
         snprintf (Gbl.Title,sizeof (Gbl.Title),
                   Txt_Page_X_of_Y,
                   Pagination->NumPags,Pagination->NumPags);
         HTM_BUTTON_SUBMIT_Begin (Gbl.Title,ClassLink,NULL);
         HTM_Unsigned (Pagination->NumPags);
         HTM_BUTTON_End ();
         Frm_EndForm ();
        }

      free (ClassLink);
     }
  }

/*****************************************************************************/
/******************* Put hidden parameter number of page *********************/
/*****************************************************************************/

void Pag_PutHiddenParamPagNum (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage)
  {
   Par_PutHiddenParamUnsigned (NULL,Pag_ParamNumPag[WhatPaginate],NumPage);
  }

/*****************************************************************************/
/************************ Get parameter number of page ***********************/
/*****************************************************************************/

unsigned Pag_GetParamPagNum (Pag_WhatPaginate_t WhatPaginate)
  {
   unsigned CurrentPage;

   /***** Try to get parameter with the number of page *****/
   CurrentPage = (unsigned) Par_GetParToUnsignedLong (Pag_ParamNumPag[WhatPaginate],
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
	       CurrentPage = 1;	// Now set the current page to the first,
				// but later the correct page will be calculated
	    else
	       /* Show the last visited page */
	       CurrentPage = Pag_GetLastPageMsgFromSession (Pag_MESSAGES_RECEIVED);
	    break;
	 case Pag_MESSAGES_SENT:
	    /* Show the last visited page */
	    CurrentPage = Pag_GetLastPageMsgFromSession (Pag_MESSAGES_SENT);
	    break;
	 default:
	    CurrentPage = 1;
	    break;
	}

   return CurrentPage;
  }

/*****************************************************************************/
/********* Save last page of received/sent messages into session *************/
/*****************************************************************************/

void Pag_SaveLastPageMsgIntoSession (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage)
  {
   /***** Save last page of received/sent messages *****/
   DB_QueryUPDATE ("can not update last page of messages",
		   "UPDATE sessions SET %s=%u WHERE SessionId='%s'",
                   WhatPaginate == Pag_MESSAGES_RECEIVED ? "LastPageMsgRcv" :
        	                                           "LastPageMsgSnt",
                   NumPage,Gbl.Session.Id);
  }

/*****************************************************************************/
/********* Get last page of received/sent messages stored in session *********/
/*****************************************************************************/

unsigned Pag_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumPage;

   /***** Get last page of received/sent messages from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get last page of messages",
			     "SELECT %s FROM sessions"
			     " WHERE SessionId='%s'",
			     WhatPaginate == Pag_MESSAGES_RECEIVED ? "LastPageMsgRcv" :
								     "LastPageMsgSnt",
			     Gbl.Session.Id);

   /***** Check number of rows of the result ****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting last page of messages.");

   /***** Get last page of messages *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumPage) == 1)
      if (NumPage == 0)
         NumPage = 1;

   return NumPage;
  }
