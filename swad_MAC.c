// swad_MAC.c: MAC addresses

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stddef.h>		// For NULL

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_MAC.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/* Parameters used in forms to edit MAC address */
struct MAC_Pars
  {
   long RooCod;					// Room code
   char MACstr[MAC_LENGTH_MAC_ADDRESS + 1];	// MAC address
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void MAC_PutPars (void *Args);
static void MAC_PutFormToEditMACAddress (Act_Action_t NextAction,const char *Anchor,
                                         void (*FuncPars) (void *Args),void *Args);

/*****************************************************************************/
/**************** Put hidden parameters to edit a MAC address ****************/
/*****************************************************************************/

static void MAC_PutPars (void *Args)
  {
   if (Args)
     {
      ParCod_PutPar   (ParCod_Roo,((struct MAC_Pars *) Args)->RooCod);
      Par_PutParString (NULL,"MAC",((struct MAC_Pars *) Args)->MACstr);
     }
  }

/*****************************************************************************/
/************************ Put form to edit a MAC address *********************/
/*****************************************************************************/

static void MAC_PutFormToEditMACAddress (Act_Action_t NextAction,const char *Anchor,
                                         void (*FuncPars) (void *Args),void *Args)
  {
   /***** Form to enter a new MAC address *****/
   Frm_BeginFormAnchor (NextAction,Anchor);
      FuncPars (Args);
      HTM_INPUT_TEXT ("NewMAC",MAC_LENGTH_MAC_ADDRESS,((struct MAC_Pars *) Args)->MACstr,
		      HTM_SUBMIT_ON_CHANGE,
		      "size=\"8\" class=\"INPUT_%s\"",
		      The_GetSuffix ());
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************ List several MAC addresses *************************/
/*****************************************************************************/

void MAC_ListMACAddresses (unsigned NumMACs,MYSQL_RES **mysql_res)
  {
   MYSQL_ROW row;
   unsigned NumMAC;
   unsigned long long MACnum;
   char MACstr[MAC_LENGTH_MAC_ADDRESS + 1];

   /***** Write the MAC addresses *****/
   for (NumMAC = 0;
	NumMAC < NumMACs;
	NumMAC++)
     {
      /* Get next MAC address */
      row = mysql_fetch_row (*mysql_res);

      /* Write break line */
      if (NumMAC)
	 HTM_BR ();

      /* Write MAC address (row[0]) */
      if (sscanf (row[0],"%llu",&MACnum) == 1)
	{
         MAC_MACnumToMACstr (MACnum,MACstr);
         HTM_Txt (MACstr);
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ List several MAC addresses *************************/
/*****************************************************************************/

void MAC_EditMACAddresses (long RooCod,const char *Anchor,
                           unsigned NumMACs,MYSQL_RES **mysql_res)
  {
   MYSQL_ROW row;
   unsigned NumMAC;
   unsigned long long MACnum;
   struct MAC_Pars Pars;

   /***** Write the forms to enter the MAC addresses *****/
   for (NumMAC = 0;
	NumMAC < NumMACs;
	NumMAC++)
     {
      /* Get next MAC address */
      row = mysql_fetch_row (*mysql_res);

      /* Write MAC address (row[0]) */
      if (sscanf (row[0],"%llu",&MACnum) == 1)
	{
         Pars.RooCod = RooCod;				// Code (i.e. room code)
         MAC_MACnumToMACstr (MACnum,Pars.MACstr);	// Current MAC address in xx:xx:xx:xx:xx:xx format
         MAC_PutFormToEditMACAddress (ActChgRooMAC,Anchor,
                                      MAC_PutPars,&Pars);

	 /* Write break line */
	 HTM_BR ();
	}
     }

   /* Form to enter a new MAC address */
   Pars.RooCod = RooCod;	// Room code
   Pars.MACstr[0] = '\0';	// Current MAC address in xx:xx:xx:xx:xx:xx format
   MAC_PutFormToEditMACAddress (ActChgRooMAC,Anchor,
                                MAC_PutPars,&Pars);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/****************** Get MAC address as a number from a form ******************/
/*****************************************************************************/

unsigned long long MAC_GetMACnumFromForm (const char *ParName)
  {
   char MACstr[MAC_LENGTH_MAC_ADDRESS * Str_MAX_BYTES_PER_CHAR + 1];
   unsigned long long MAC[MAC_NUM_BYTES];
   unsigned long long MACnum;

   /***** Get parameter *****/
   Par_GetParText (ParName,MACstr,MAC_LENGTH_MAC_ADDRESS * Str_MAX_BYTES_PER_CHAR);

   if (MACstr[0])	// Not empty
     {
      /***** Try to scan it in xx:xx:xx:xx:xx:xx format (where x are hexadecimal digits) *****/
      if (sscanf (MACstr,"%02llx:%02llx:%02llx:%02llx:%02llx:%02llx",
                  &MAC[0],&MAC[1],&MAC[2],&MAC[3],&MAC[4],&MAC[5]) == MAC_NUM_BYTES)
	 return (MAC[0] << (CHAR_BIT * 5)) +
		(MAC[1] << (CHAR_BIT * 4)) +
		(MAC[2] << (CHAR_BIT * 3)) +
		(MAC[3] << (CHAR_BIT * 2)) +
		(MAC[4] << (CHAR_BIT * 1)) +
		(MAC[5] << (CHAR_BIT * 0));

      /***** Try to scan it in xxxxxxxxxxxx format (where x are hexadecimal digits) ******/
      if (sscanf (MACstr,"%llx",&MACnum) == 1)
	 return MACnum;
     }

   return 0;
  }

/*****************************************************************************/
/**** Convert from MAC as a number to string in xx:xx:xx:xx:xx:xx format *****/
/*****************************************************************************/

void MAC_MACnumToMACstr (unsigned long long MACnum,char MACstr[MAC_LENGTH_MAC_ADDRESS + 1])
  {
   if (MACnum)
      snprintf (MACstr,MAC_LENGTH_MAC_ADDRESS + 1,"%02x:%02x:%02x:%02x:%02x:%02x",
		(unsigned char) ((MACnum >> (CHAR_BIT * 5)) & ((1 << CHAR_BIT) - 1)),
		(unsigned char) ((MACnum >> (CHAR_BIT * 4)) & ((1 << CHAR_BIT) - 1)),
		(unsigned char) ((MACnum >> (CHAR_BIT * 3)) & ((1 << CHAR_BIT) - 1)),
		(unsigned char) ((MACnum >> (CHAR_BIT * 2)) & ((1 << CHAR_BIT) - 1)),
		(unsigned char) ((MACnum >> (CHAR_BIT * 1)) & ((1 << CHAR_BIT) - 1)),
		(unsigned char) ((MACnum >> (CHAR_BIT * 0)) & ((1 << CHAR_BIT) - 1)));
   else
      MACstr[0] = '\0';
  }
