// swad_game_resource.c: links to games as program resources

/*
    SWAD (Shared Workspace At a Distance),
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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_game.h"
#include "swad_game_database.h"
#include "swad_game_resource.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"
#include "swad_theme.h"

/*****************************************************************************/
/***************************** Get link to game ******************************/
/*****************************************************************************/

void GamRsc_GetLinkToGame (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Games;
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   Games.Game.GamCod = Gam_GetPars (&Games);

   /***** Get game data *****/
   if (Games.Game.GamCod > 0)
      Gam_GetGameDataByCod (&Games.Game);

   /***** Copy link to game into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_GAME,Games.Game.GamCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Games.Game.GamCod > 0 ? Games.Game.Title :
   					  Txt_Games);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }
