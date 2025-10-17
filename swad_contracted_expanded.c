// swad_contracted_expanded.c: types and constants related to contracted/expanded

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_contracted_expanded.h"

/*****************************************************************************/
/********** Get if expanded or contracted from a 'Y'/'N' character ***********/
/*****************************************************************************/

ConExp_ContractedOrExpanded_t CloOpe_GetExpandedFromYN (char Ch)
  {
   return Ch == 'Y' ? ConExp_EXPANDED :
		      ConExp_CONTRACTED;
  }
