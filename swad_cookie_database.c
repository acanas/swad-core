// swad_cookie_database.c: user's preferences about cookies (operations with database)

// swad_cookies.c: user's preferences about cookies

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_cookie.h"
#include "swad_cookie_database.h"
#include "swad_database.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************** Update my settings about third party cookies *****************/
/*****************************************************************************/

void Coo_DB_UpdateMyPrefsCookies (void)
  {
   DB_QueryUPDATE ("can not update your preference about cookies",
		   "UPDATE usr_data"
		     " SET ThirdPartyCookies='%c'"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.Prefs.RefuseAcceptCookies == Coo_ACCEPT ? 'Y' :
							                        'N',
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }
