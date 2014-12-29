// swad_pagination.c: pagination of assignments, forums and messages

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL

#include "swad_action.h"
#include "swad_database.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_parameter.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;
extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
extern const Act_Action_t For_ActionsSeePstFor[For_NUM_TYPES_FORUM];

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

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
   Pagination->StartPage = Pagination->LeftPage = Pagination->RightPage = Pagination->EndPage = 1;
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
      if ((Pagination->StartPage = Pagination->CurrentPage - NUM_PAGES_BEFORE_CURRENT) < 1)
         Pagination->StartPage = 1;

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
   Pagination->FirstItemVisible = Pagination->LastItemVisible - (Pag_ITEMS_PER_PAGE-1);
   if (Pagination->LastItemVisible > Pagination->NumItems)
      Pagination->LastItemVisible = Pagination->NumItems;
  }

/*****************************************************************************/
/************** Show enlaces a distintas páginas of messages *****************/
/*****************************************************************************/

void Pag_WriteLinksToPagesCentered (Pag_WhatPaginate_t WhatPaginate,long ThrCod,struct Pagination *Pagination)
  {
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Pag_WriteLinksToPages (WhatPaginate,ThrCod,Pagination,true,NULL,"TIT",false);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Show links to pages of messages **********************/
/*****************************************************************************/

void Pag_WriteLinksToPages (Pag_WhatPaginate_t WhatPaginate,long ThrCod,struct Pagination *Pagination,
                            bool FirstMsgEnabled,const char *Subject,const char *Font,
                            bool LinkToPagCurrent)
  {
   extern const char *Txt_Page;
   extern const char *Txt_See_page_X_of_Y;
   extern const char *Txt_first_message_not_allowed;
   int NumPage;

   /***** Link to page 1, including a text *****/
   if (Subject)
     {
      if (LinkToPagCurrent)
        {
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Act_FormStart (ActSeeAsg);
               Pag_PutHiddenParamPagNum (1);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Act_FormStart (ActSeeAtt);
               Pag_PutHiddenParamPagNum (1);
               Att_PutHiddenParamAttOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_MESSAGES_RECEIVED:
               Act_FormStart (ActSeeRcvMsg);
               Pag_PutHiddenParamPagNum (1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Act_FormStart (ActSeeSntMsg);
               Pag_PutHiddenParamPagNum (1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_THREADS_FORUM:
               Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (1);
               For_PutAllHiddenParamsForum ();
               break;
            case Pag_POSTS_FORUM:
               Act_FormStart (For_ActionsSeePstFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (1);
               For_PutHiddenParamThrCod (ThrCod);
               For_PutAllHiddenParamsForum ();
	       break;
            case Pag_SURVEYS:
               Act_FormStart (ActSeeAllSvy);
               Pag_PutHiddenParamPagNum (1);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
           }
         sprintf (Gbl.Title,Txt_See_page_X_of_Y,
                  1,(unsigned) Pagination->NumPags);
         Act_LinkFormSubmit (Gbl.Title,Font);
        }
      else
         fprintf (Gbl.F.Out,"<span class=\"%s\">",Font);
      if (FirstMsgEnabled)
         fprintf (Gbl.F.Out,"%s",Subject);
      else
         fprintf (Gbl.F.Out,"[%s]",
                  Txt_first_message_not_allowed);
      fprintf (Gbl.F.Out,"%s",
               LinkToPagCurrent ? "</a>"
	                          "</form>" :
        	                  "</span>");
     }

   if (Pagination->MoreThanOnePage)
     {
      /***** Links to several pages start here *****/
      fprintf (Gbl.F.Out,"<table style=\"margin:0 auto; border-spacing:5px;\">"
                         "<tr>"
                         "<td class=\"%s\" style=\"text-align:left;\">"
                         "["
                         "</td>"
                         "<td class=\"%s\" style=\"text-align:left;\">"
                         "%s"
                         "</td>",
               Font,Font,Txt_Page);

      /***** Possible link to page 1 *****/
      if (Pagination->StartPage > 1)
        {
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">",
                  Font);
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Act_FormStart (ActSeeAsg);
               Pag_PutHiddenParamPagNum (1);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Act_FormStart (ActSeeAtt);
               Pag_PutHiddenParamPagNum (1);
               Att_PutHiddenParamAttOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_MESSAGES_RECEIVED:
               Act_FormStart (ActSeeRcvMsg);
               Pag_PutHiddenParamPagNum (1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Act_FormStart (ActSeeSntMsg);
               Pag_PutHiddenParamPagNum (1);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_THREADS_FORUM:
               Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (1);
               For_PutAllHiddenParamsForum ();
               break;
            case Pag_POSTS_FORUM:
               Act_FormStart (For_ActionsSeePstFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (1);
               For_PutHiddenParamThrCod (ThrCod);
               For_PutAllHiddenParamsForum ();
	       break;
            case Pag_SURVEYS:
               Act_FormStart (ActSeeAllSvy);
               Pag_PutHiddenParamPagNum (1);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
           }
         sprintf (Gbl.Title,Txt_See_page_X_of_Y,
                  1,(unsigned) Pagination->NumPags);
         Act_LinkFormSubmit (Gbl.Title,Font);
         fprintf (Gbl.F.Out,"1</a>"
                            "</form>"
                            "</td>");
         if (Pagination->LeftPage > 2)
            fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">"
        	               "..."
        	               "</td>",
        	     Font);
        }

      /***** Posible link to page left *****/
      if (Pagination->LeftPage > 1 && Pagination->LeftPage < Pagination->StartPage)
        {
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">",
                  Font);
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Act_FormStart (ActSeeAsg);
               Pag_PutHiddenParamPagNum (Pagination->LeftPage);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Act_FormStart (ActSeeAtt);
               Pag_PutHiddenParamPagNum (Pagination->LeftPage);
               Att_PutHiddenParamAttOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_MESSAGES_RECEIVED:
               Act_FormStart (ActSeeRcvMsg);
               Pag_PutHiddenParamPagNum (Pagination->LeftPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Act_FormStart (ActSeeSntMsg);
               Pag_PutHiddenParamPagNum (Pagination->LeftPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_THREADS_FORUM:
               Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (Pagination->LeftPage);
               For_PutAllHiddenParamsForum ();
               break;
            case Pag_POSTS_FORUM:
               Act_FormStart (For_ActionsSeePstFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (Pagination->LeftPage);
               For_PutHiddenParamThrCod (ThrCod);
               For_PutAllHiddenParamsForum ();
	       break;
            case Pag_SURVEYS:
               Act_FormStart (ActSeeAllSvy);
               Pag_PutHiddenParamPagNum (Pagination->LeftPage);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
           }
         sprintf (Gbl.Title,Txt_See_page_X_of_Y,
                  (unsigned) Pagination->LeftPage,(unsigned) Pagination->NumPags);
         Act_LinkFormSubmit (Gbl.Title,Font);
         fprintf (Gbl.F.Out,"%u</a>"
                            "</form>"
                            "</td>",
                  (unsigned) Pagination->LeftPage);
         if (Pagination->LeftPage < Pagination->StartPage-1)
            fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">"
        	               "..."
        	               "</td>",
                     Font);
        }

      /***** Loop to put links to the pages around the current one *****/
      for (NumPage = Pagination->StartPage;
	   NumPage <= Pagination->EndPage;
	   NumPage++)
        {
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">",
                  Font);
         if (!LinkToPagCurrent && NumPage == Pagination->CurrentPage)
            fprintf (Gbl.F.Out,"<u>%u</u>",(unsigned) NumPage);
         else
           {
            switch (WhatPaginate)
              {
               case Pag_ASSIGNMENTS:
                  Act_FormStart (ActSeeAsg);
                  Pag_PutHiddenParamPagNum (NumPage);
                  Asg_PutHiddenParamAsgOrderType ();
                  Grp_PutParamWhichGrps ();
                  break;
               case Pag_ATT_EVENTS:
                  Act_FormStart (ActSeeAtt);
                  Pag_PutHiddenParamPagNum (NumPage);
                  Att_PutHiddenParamAttOrderType ();
                  Grp_PutParamWhichGrps ();
                  break;
               case Pag_MESSAGES_RECEIVED:
                  Act_FormStart (ActSeeRcvMsg);
                  Pag_PutHiddenParamPagNum (NumPage);
                  Msg_PutHiddenParamsMsgsFilters ();
                  break;
               case Pag_MESSAGES_SENT:
                  Act_FormStart (ActSeeSntMsg);
                  Pag_PutHiddenParamPagNum (NumPage);
                  Msg_PutHiddenParamsMsgsFilters ();
                  break;
               case Pag_THREADS_FORUM:
                  Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
                  Pag_PutHiddenParamPagNum (NumPage);
                  For_PutAllHiddenParamsForum ();
                  break;
               case Pag_POSTS_FORUM:
                  Act_FormStart (For_ActionsSeePstFor[Gbl.Forum.ForumType]);
                  Pag_PutHiddenParamPagNum (NumPage);
                  For_PutHiddenParamThrCod (ThrCod);
                  For_PutAllHiddenParamsForum ();
                  break;
               case Pag_SURVEYS:
                  Act_FormStart (ActSeeAllSvy);
                  Pag_PutHiddenParamPagNum (NumPage);
                  Asg_PutHiddenParamAsgOrderType ();
                  Grp_PutParamWhichGrps ();
                  break;
              }
            sprintf (Gbl.Title,Txt_See_page_X_of_Y,
                     (unsigned) NumPage,(unsigned) Pagination->NumPags);
            Act_LinkFormSubmit (Gbl.Title,Font);
            fprintf (Gbl.F.Out,"%u</a>"
        	               "</form>",
        	     (unsigned) NumPage);
           }
         fprintf (Gbl.F.Out,"</td>");
        }

      /***** Posible link to page right *****/
      if (Pagination->RightPage > Pagination->EndPage && Pagination->RightPage < Pagination->NumPags)
        {
         if (Pagination->RightPage > Pagination->EndPage+1)
            fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">"
        	               "..."
        	               "</td>",
                     Font);
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">",
                  Font);
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Act_FormStart (ActSeeAsg);
               Pag_PutHiddenParamPagNum (Pagination->RightPage);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Act_FormStart (ActSeeAtt);
               Pag_PutHiddenParamPagNum (Pagination->RightPage);
               Att_PutHiddenParamAttOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_MESSAGES_RECEIVED:
               Act_FormStart (ActSeeRcvMsg);
               Pag_PutHiddenParamPagNum (Pagination->RightPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Act_FormStart (ActSeeSntMsg);
               Pag_PutHiddenParamPagNum (Pagination->RightPage);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_THREADS_FORUM:
               Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (Pagination->RightPage);
               For_PutAllHiddenParamsForum ();
               break;
            case Pag_POSTS_FORUM:
               Act_FormStart (For_ActionsSeePstFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (Pagination->RightPage);
               For_PutHiddenParamThrCod (ThrCod);
               For_PutAllHiddenParamsForum ();
               break;
            case Pag_SURVEYS:
               Act_FormStart (ActSeeAllSvy);
               Pag_PutHiddenParamPagNum (Pagination->RightPage);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
           }
         sprintf (Gbl.Title,Txt_See_page_X_of_Y,
                  (unsigned) Pagination->RightPage,(unsigned) Pagination->NumPags);
         Act_LinkFormSubmit (Gbl.Title,Font);
         fprintf (Gbl.F.Out,"%u</a>"
                            "</form>"
                            "</td>",
                  (unsigned) Pagination->RightPage);
        }

      /***** Possible link to last page *****/
      if (Pagination->EndPage < Pagination->NumPags)
        {
         if (Pagination->NumPags > Pagination->RightPage+1)
            fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">"
        	               "..."
        	               "</td>",
                     Font);
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">",
                  Font);
         switch (WhatPaginate)
           {
            case Pag_ASSIGNMENTS:
               Act_FormStart (ActSeeAsg);
               Pag_PutHiddenParamPagNum (Pagination->NumPags);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_ATT_EVENTS:
               Act_FormStart (ActSeeAtt);
               Pag_PutHiddenParamPagNum (Pagination->NumPags);
               Att_PutHiddenParamAttOrderType ();
               Grp_PutParamWhichGrps ();
               break;
            case Pag_MESSAGES_RECEIVED:
               Act_FormStart (ActSeeRcvMsg);
               Pag_PutHiddenParamPagNum (Pagination->NumPags);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_MESSAGES_SENT:
               Act_FormStart (ActSeeSntMsg);
               Pag_PutHiddenParamPagNum (Pagination->NumPags);
               Msg_PutHiddenParamsMsgsFilters ();
               break;
            case Pag_THREADS_FORUM:
               Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (Pagination->NumPags);
               For_PutAllHiddenParamsForum ();
               break;
            case Pag_POSTS_FORUM:
               Act_FormStart (For_ActionsSeePstFor[Gbl.Forum.ForumType]);
               Pag_PutHiddenParamPagNum (Pagination->NumPags);
               For_PutHiddenParamThrCod (ThrCod);
               For_PutAllHiddenParamsForum ();
               break;
            case Pag_SURVEYS:
               Act_FormStart (ActSeeAllSvy);
               Pag_PutHiddenParamPagNum (Pagination->NumPags);
               Asg_PutHiddenParamAsgOrderType ();
               Grp_PutParamWhichGrps ();
               break;
           }
         sprintf (Gbl.Title,Txt_See_page_X_of_Y,
                  (unsigned) Pagination->NumPags,(unsigned) Pagination->NumPags);
         Act_LinkFormSubmit (Gbl.Title,Font);
         fprintf (Gbl.F.Out,"%u</a>"
                            "</form>"
                            "</td>",
                  (unsigned) Pagination->NumPags);
        }
      fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;\">"
	                 "]"
	                 "</td>"
	                 "</tr>"
	                 "</table>",
	       Font);
     }
  }

/*****************************************************************************/
/******************* Put hidden parameter number of page *********************/
/*****************************************************************************/

void Pag_PutHiddenParamPagNum (unsigned NumPage)
  {
   Par_PutHiddenParamUnsigned ("NumPage",NumPage);
  }

/*****************************************************************************/
/************************ Get parameter number of page ***********************/
/*****************************************************************************/

void Pag_GetParamPagNum (Pag_WhatPaginate_t WhatPaginate)
  {
   static bool FirstCall = true;
   char UnsignedStr[10+1];
   unsigned NumPage;

   if (FirstCall)
     {
      FirstCall = false;

      /***** Try to get parameter with the number of page *****/
      Par_GetParToText ("NumPage",UnsignedStr,10);
      if (sscanf (UnsignedStr,"%u",&NumPage) == 1)
         Gbl.Pag.CurrentPage = NumPage;
      else
         /***** If there's no parameter page, return a default value *****/
         switch (WhatPaginate)
           {
            case Pag_MESSAGES_RECEIVED:
               if (Gbl.CurrentAct == ActExpRcvMsg)	// Expanding a message, perhaps it is the result of following a link
							// from a notification of received message
                  // Show the page corresponding to the expanded message
                  Gbl.Pag.CurrentPage = 1;	// Now set the current page to the first, but later the correct page will be calculated
               else
	          // Show the last visited page
                  Gbl.Pag.CurrentPage = Pag_GetLastPageMsgFromSession (Pag_MESSAGES_RECEIVED);
               break;
            case Pag_MESSAGES_SENT:
               // Show the last visited page
               Gbl.Pag.CurrentPage = Pag_GetLastPageMsgFromSession (Pag_MESSAGES_SENT);
               break;
            default:
               Gbl.Pag.CurrentPage = 1;
               break;
           }
     }
  }

/*****************************************************************************/
/********* Save last page of received/sent messages into session *************/
/*****************************************************************************/

void Pag_SaveLastPageMsgIntoSession (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage)
  {
   char Query[512];

   /***** Save last page of received/sent messages *****/
   sprintf (Query,"UPDATE sessions SET %s='%u' WHERE SessionId='%s'",
            WhatPaginate == Pag_MESSAGES_RECEIVED ? "LastPageMsgRcv" :
        	                                    "LastPageMsgSnt",
            NumPage,Gbl.Session.Id);
   DB_QueryUPDATE (Query,"can not update last page of messages");
  }

/*****************************************************************************/
/********* Get last page of received/sent messages stored in session *********/
/*****************************************************************************/

unsigned Pag_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumPage;

   /***** Get last page of received/sent messages from database *****/
   sprintf (Query,"SELECT %s FROM sessions WHERE SessionId='%s'",
            WhatPaginate == Pag_MESSAGES_RECEIVED ? "LastPageMsgRcv" :
        	                                    "LastPageMsgSnt",
            Gbl.Session.Id);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get last page of messages");

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
