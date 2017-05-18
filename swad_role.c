// swad_role.c: user's roles

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_role.h"
#include "swad_role_type.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

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

/*****************************************************************************/
/****************** Get number of available roles for me *********************/
/*****************************************************************************/

unsigned Rol_GetNumAvailableRoles (void)
  {
   Rol_Role_t Role;
   unsigned NumAvailableRoles = 0;

   for (Role = Rol_GST;
        Role < Rol_NUM_ROLES;
        Role++)
      if (Gbl.Usrs.Me.AvailableRoles & (1 << Role))
         NumAvailableRoles++;

   return NumAvailableRoles;
  }

/*****************************************************************************/
/************ Get maximum role of a user in all his/her courses **************/
/*****************************************************************************/

Rol_Role_t Rol_GetMaxRoleInCrss (unsigned Roles)
  {
   /***** User's role in one of her/his course can be ROL_STD or ROL_TCH *****/
   /* Check first if user is a teacher in any course */
   if (Roles & (1 << Rol_TCH))
      return Rol_TCH;

   /* Not a teacher. Check then if user is a student in any course */
   if (Roles & (1 << Rol_STD))
      return Rol_STD;

   /* Not a teacher or student */
   return Rol_GST;	// Guest means that this user is not registered in any course
  }

/*****************************************************************************/
/******************* Get my maximum role in a institution ********************/
/*****************************************************************************/

Rol_Role_t Rol_GetMyMaxRoleInIns (long InsCod)
  {
   unsigned NumMyIns;

   if (InsCod > 0)
     {
      /***** Fill the list with the institutions I belong to (if not already filled) *****/
      Usr_GetMyInstits ();

      /***** Check if the institution passed as parameter is any of my institutions *****/
      for (NumMyIns = 0;
           NumMyIns < Gbl.Usrs.Me.MyInss.Num;
           NumMyIns++)
         if (Gbl.Usrs.Me.MyInss.Inss[NumMyIns].InsCod == InsCod)
            return Gbl.Usrs.Me.MyInss.Inss[NumMyIns].MaxRole;
      return Rol_GST;
     }
   return Rol_UNK;   // No degree
  }

/*****************************************************************************/
/********************** Get my maximum role in a centre **********************/
/*****************************************************************************/

Rol_Role_t Rol_GetMyMaxRoleInCtr (long CtrCod)
  {
   unsigned NumMyCtr;

   if (CtrCod > 0)
     {
      /***** Fill the list with the centres I belong to (if not already filled) *****/
      Usr_GetMyCentres ();

      /***** Check if the centre passed as parameter is any of my centres *****/
      for (NumMyCtr = 0;
           NumMyCtr < Gbl.Usrs.Me.MyCtrs.Num;
           NumMyCtr++)
         if (Gbl.Usrs.Me.MyCtrs.Ctrs[NumMyCtr].CtrCod == CtrCod)
            return Gbl.Usrs.Me.MyCtrs.Ctrs[NumMyCtr].MaxRole;
      return Rol_GST;
     }
   return Rol_UNK;   // No centre
  }

/*****************************************************************************/
/********************** Get my maximum role in a degree **********************/
/*****************************************************************************/

Rol_Role_t Rol_GetMyMaxRoleInDeg (long DegCod)
  {
   unsigned NumMyDeg;

   if (DegCod > 0)
     {
      /***** Fill the list with the degrees I belong to (if not already filled) *****/
      Usr_GetMyDegrees ();

      /***** Check if the degree passed as parameter is any of my degrees *****/
      for (NumMyDeg = 0;
           NumMyDeg < Gbl.Usrs.Me.MyDegs.Num;
           NumMyDeg++)
         if (Gbl.Usrs.Me.MyDegs.Degs[NumMyDeg].DegCod == DegCod)
            return Gbl.Usrs.Me.MyDegs.Degs[NumMyDeg].MaxRole;
      return Rol_GST;
     }
   return Rol_UNK;   // No degree
  }

/*****************************************************************************/
/*************************** Get my role in a course *************************/
/*****************************************************************************/

Rol_Role_t Rol_GetMyRoleInCrs (long CrsCod)
  {
   unsigned NumMyCrs;

   if (CrsCod > 0)
     {
      /***** Fill the list with the courses I belong to (if not already filled) *****/
      Usr_GetMyCourses ();

      /***** Check if the course passed as parameter is any of my courses *****/
      for (NumMyCrs = 0;
           NumMyCrs < Gbl.Usrs.Me.MyCrss.Num;
           NumMyCrs++)
         if (Gbl.Usrs.Me.MyCrss.Crss[NumMyCrs].CrsCod == CrsCod)
            return Gbl.Usrs.Me.MyCrss.Crss[NumMyCrs].Role;
      return Rol_GST;
     }
   return Rol_UNK;   // No course
  }

/*****************************************************************************/
/********************** Get role of a user in a course ***********************/
/*****************************************************************************/

Rol_Role_t Rol_GetRoleInCrs (long CrsCod,long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Rol_Role_t Role = Rol_UNK;	// Default role (if no course selected or user don't belong to it)

   if (CrsCod > 0)
     {
      /***** Get rol of a user in a course from database.
             The result of the query will have one row or none *****/
      sprintf (Query,"SELECT Role FROM crs_usr"
                     " WHERE CrsCod=%ld AND UsrCod=%ld",
               CrsCod,UsrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get the role of a user in a course") == 1)        // User belongs to the course
        {
         row = mysql_fetch_row (mysql_res);
         Role = Rol_ConvertUnsignedStrToRole (row[0]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return Role;
  }

/*****************************************************************************/
/**************** Get roles of a user in all his/her courses *****************/
/*****************************************************************************/
// Roles >=0 ==> already filled/calculated ==> nothing to do
// Roles  <0 ==> not yet filled/calculated ==> get roles

void Rol_GetRolesInAllCrssIfNotYetGot (struct UsrData *UsrDat)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRole;
   unsigned NumRoles;

   /***** If roles is already filled ==> nothing to do *****/
   if (UsrDat->Roles < 0)	// Not yet filled
     {
      /***** Get distinct roles in all courses of the user from database *****/
      sprintf (Query,"SELECT DISTINCT(Role) FROM crs_usr WHERE UsrCod=%ld",
	       UsrDat->UsrCod);
      NumRoles = (unsigned) DB_QuerySELECT (Query,&mysql_res,
					    "can not get the roles of a user"
					    " in all his/her courses");
      for (NumRole = 0, UsrDat->Roles = 0;
	   NumRole < NumRoles;
	   NumRole++)
	{
	 row = mysql_fetch_row (mysql_res);
	 UsrDat->Roles |= (int) (1 << Rol_ConvertUnsignedStrToRole (row[0]));
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/********************** Get role from unsigned string ************************/
/*****************************************************************************/

Rol_Role_t Rol_ConvertUnsignedStrToRole (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Rol_NUM_ROLES)
         return (Rol_Role_t) UnsignedNum;

   return Rol_UNK;
  }

/*****************************************************************************/
/****** Get roles (several bits can be activated) from unsigned string *******/
/*****************************************************************************/

unsigned Rol_ConvertUnsignedStrToRoles (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      return UnsignedNum;
   return 0;
  }

/*****************************************************************************/
/*********************** Put a form to change my role ************************/
/*****************************************************************************/

void Rol_PutFormToChangeMyRole (void)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Rol_Role_t Role;

   Act_FormStart (ActChgMyRol);
   fprintf (Gbl.F.Out,"<select name=\"MyRole\" class=\"SEL_ROLE\""
                      " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);
   for (Role = Rol_GST;
        Role < Rol_NUM_ROLES;
        Role++)
     if (Gbl.Usrs.Me.AvailableRoles & (1 << Role))
        {
         fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Role);
         if (Role == Gbl.Usrs.Me.LoggedRole)
            fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",
                  Txt_ROLES_SINGUL_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
        }
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************************** Change my role *******************************/
/*****************************************************************************/

void Rol_ChangeMyRole (void)
  {
   Rol_Role_t NewRole;

   /***** Get parameter with the new logged role ******/
   NewRole = (Rol_Role_t)
	     Par_GetParToUnsignedLong ("MyRole",
                                       0,
                                       Rol_NUM_ROLES - 1,
                                       (unsigned long) Rol_UNK);
   if (NewRole != Rol_UNK)
     {
      /* Check if new role is allowed for me */
      if (!(Gbl.Usrs.Me.AvailableRoles & (1 << NewRole)))
         return;

      /* New role is correct and is allowed for me */
      if (NewRole != Gbl.Usrs.Me.LoggedRole)
	{
         /* New role is distinct to current role,
            so change my role... */
	 Gbl.Usrs.Me.LoggedRole = NewRole;
	 Gbl.Usrs.Me.RoleHasChanged = true;

	 /* ...update logged role in session... */
	 Ses_UpdateSessionDataInDB ();

	 /* ...and update logged role in list of connected */
	 Con_UpdateMeInConnectedList ();
	}
     }
  }

/*****************************************************************************/
/********************* Write selector of users' roles ************************/
/*****************************************************************************/

void Rol_WriteSelectorRoles (unsigned RolesAllowed,unsigned RolesSelected,
                             bool Disabled,bool SendOnChange)
  {
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Rol_Role_t Role;

   for (Role = Rol_UNK;
        Role <= Rol_SYS_ADM;
        Role++)
      if ((RolesAllowed & (1 << Role)))
	{
	 fprintf (Gbl.F.Out,"<label>"
	                    "<input type=\"checkbox\" name=\"Role\" value=\"%u\"",
		  (unsigned) Role);

	 if ((RolesSelected & (1 << Role)))
	    fprintf (Gbl.F.Out," checked=\"checked\"");

	 if (Disabled)
	    fprintf (Gbl.F.Out," disabled=\"disabled\"");

	 if (SendOnChange)
	    fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
		     Gbl.Form.Id);

	 fprintf (Gbl.F.Out," />"
	                    "%s"
	                    "</label>"
	                    "<br />",
		  Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
        }
  }

/*****************************************************************************/
/******************** Put hidden param with users' roles *********************/
/*****************************************************************************/

void Rol_PutHiddenParamRoles (unsigned Roles)
  {
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"Roles\" value=\"%u\" />",
	    Roles);
  }

/*****************************************************************************/
/************************* Get selected users' roles *************************/
/*****************************************************************************/

unsigned Rol_GetSelectedRoles (void)
  {
   char StrRoles[Rol_NUM_ROLES * (10 + 1)];
   const char *Ptr;
   char UnsignedStr[10 + 1];
   Rol_Role_t Role;
   unsigned Roles;

   /***** Try to get param "Roles" with multiple roles *****/
   Roles = (unsigned)
	   Par_GetParToUnsignedLong ("Roles",
                                     0,				// 000...000
                                     (1 << Rol_NUM_ROLES) - 1,	// 111...111
                                     0);			// 000...000

   /***** Try to get multiple param "Role" *****/
   Par_GetParMultiToText ("Role",StrRoles,Rol_NUM_ROLES * (10 + 1));
   for (Ptr = StrRoles;
        *Ptr;)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,10);
      Role = Rol_ConvertUnsignedStrToRole (UnsignedStr);
      if (Role != Rol_UNK)
         Roles |= (1 << Role);
     }

   return Roles;
  }

/*****************************************************************************/
/************ Get requested role of a user in current course *****************/
/*****************************************************************************/

Rol_Role_t Rol_GetRequestedRole (long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Rol_Role_t Role = Rol_UNK;

   /***** Get requested role from database *****/
   sprintf (Query,"SELECT Role FROM crs_usr_requests"
                  " WHERE CrsCod=%ld AND UsrCod=%ld",
            Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get requested role"))
     {
      /***** Get role *****/
      row = mysql_fetch_row (mysql_res);
      Role = Rol_ConvertUnsignedStrToRole (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Role;
  }
