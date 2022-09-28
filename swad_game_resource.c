// swad_game_resource.c: links to games as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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

#include "swad_alert.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_game_database.h"
#include "swad_game_resource.h"
#include "swad_program_database.h"

/*****************************************************************************/
/***************************** Get link to game ******************************/
/*****************************************************************************/

void GamRsc_GetLinkToGame (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Gam_Games Games;
   long GamCod;
   char Title[Gam_MAX_BYTES_TITLE + 1];

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   GamCod = Gam_GetParams (&Games);

   /***** Get game title *****/
   GamRsc_GetTitleFromGamCod (GamCod,Title,sizeof (Title) - 1);

   /***** Copy link to game into resource clipboard *****/
   Prg_DB_CopyToClipboard (PrgRsc_GAME,GamCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/*********************** Write game in course program ************************/
/*****************************************************************************/

void GamRsc_WriteGameInCrsProgram (long GamCod,bool PutFormToGo,
                                   const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[Act_NUM_ACTIONS];
   Act_Action_t NextAction;
   char Title[Gam_MAX_BYTES_TITLE + 1];

   /***** Get game title *****/
   GamRsc_GetTitleFromGamCod (GamCod,Title,sizeof (Title) - 1);

   /***** Begin form to go to game *****/
   if (PutFormToGo)
     {
      NextAction = (GamCod > 0)	? ActSeeGam :	// Game specified
				  ActSeeAllGam;	// All games
      Frm_BeginForm (NextAction);
         Gam_PutParamGameCod (GamCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write game title of exam *****/
   HTM_Txt (Title);

   /***** End form to go to game *****/
   if (PutFormToGo)
     {
      /* End form */
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/*********************** Get game title from game code ***********************/
/*****************************************************************************/

void GamRsc_GetTitleFromGamCod (long GamCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Games;
   char TitleFromDB[Gam_MAX_BYTES_TITLE + 1];

   if (GamCod > 0)
     {
      /***** Get game title *****/
      Gam_DB_GetGameTitle (GamCod,TitleFromDB);
      Str_Copy (Title,TitleFromDB,TitleSize);
     }
   else
      /***** Generic title for all games *****/
      Str_Copy (Title,Txt_Games,TitleSize);
  }
