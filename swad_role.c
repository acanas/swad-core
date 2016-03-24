// swad_role.c: user's roles

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_role.h"

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

   for (Role = Rol__GUEST_;
        Role < Rol_NUM_ROLES;
        Role++)
      if (Gbl.Usrs.Me.AvailableRoles & (1 << Role))
         NumAvailableRoles++;

   return NumAvailableRoles;
  }

/*****************************************************************************/
/************ Get maximum role of a user in all his/her courses **************/
/*****************************************************************************/

Rol_Role_t Rol_GetMaxRole (unsigned Roles)
  {
   if (Roles & (1 << Rol_TEACHER))
      return Rol_TEACHER;
   if (Roles & (1 << Rol_STUDENT))
      return Rol_STUDENT;
   return Rol__GUEST_;
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
      Usr_GetMyInstitutions ();

      /***** Check if the institution passed as parameter is any of my institutions *****/
      for (NumMyIns = 0;
           NumMyIns < Gbl.Usrs.Me.MyInstitutions.Num;
           NumMyIns++)
         if (Gbl.Usrs.Me.MyInstitutions.Inss[NumMyIns].InsCod == InsCod)
            return Gbl.Usrs.Me.MyInstitutions.Inss[NumMyIns].MaxRole;
      return Rol__GUEST_;
     }
   return Rol_UNKNOWN;   // No degree
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
           NumMyCtr < Gbl.Usrs.Me.MyCentres.Num;
           NumMyCtr++)
         if (Gbl.Usrs.Me.MyCentres.Ctrs[NumMyCtr].CtrCod == CtrCod)
            return Gbl.Usrs.Me.MyCentres.Ctrs[NumMyCtr].MaxRole;
      return Rol__GUEST_;
     }
   return Rol_UNKNOWN;   // No centre
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
           NumMyDeg < Gbl.Usrs.Me.MyDegrees.Num;
           NumMyDeg++)
         if (Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].DegCod == DegCod)
            return Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].MaxRole;
      return Rol__GUEST_;
     }
   return Rol_UNKNOWN;   // No degree
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
           NumMyCrs < Gbl.Usrs.Me.MyCourses.Num;
           NumMyCrs++)
         if (Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod == CrsCod)
            return Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].Role;
      return Rol__GUEST_;
     }
   return Rol_UNKNOWN;   // No course
  }

/*****************************************************************************/
/********************** Get role of a user in a course ***********************/
/*****************************************************************************/

Rol_Role_t Rol_GetRoleInCrs (long CrsCod,long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Rol_Role_t Role;

   if (CrsCod > 0)
     {
      /***** Get rol of a user in a course from database.
             The result of the query will have one row or none *****/
      sprintf (Query,"SELECT Role FROM crs_usr"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               CrsCod,UsrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get the role of a user in a course") == 1)        // User belongs to the course
        {
         row = mysql_fetch_row (mysql_res);
         Role = Rol_ConvertUnsignedStrToRole (row[0]);
        }
      else                // User does not belong to the course
         Role = Rol_UNKNOWN;

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else        // No course
      Role = Rol_UNKNOWN;

   return Role;
  }

/*****************************************************************************/
/**************** Get roles of a user in all his/her courses *****************/
/*****************************************************************************/

unsigned Rol_GetRolesInAllCrss (long UsrCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRole;
   unsigned NumRoles;
   Rol_Role_t Role;
   unsigned Roles = 0;

   /***** Get distinct roles in all the courses of the user from database *****/
   sprintf (Query,"SELECT DISTINCT(Role) FROM crs_usr"
                  " WHERE UsrCod='%ld'",
            UsrCod);
   NumRoles = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get the roles of a user in all his/her courses");
   for (NumRole = 0;
        NumRole < NumRoles;
        NumRole++)
     {
      row = mysql_fetch_row (mysql_res);
      if ((Role = Rol_ConvertUnsignedStrToRole (row[0])) != Rol_UNKNOWN)
         Roles |= (1 << Role);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Roles;
  }

/*****************************************************************************/
/********************** Get role from unsigned string ************************/
/*****************************************************************************/

Rol_Role_t Rol_ConvertUnsignedStrToRole (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      return (UnsignedNum >= Rol_NUM_ROLES) ? Rol_UNKNOWN :
                                              (Rol_Role_t) UnsignedNum;
   return Rol_UNKNOWN;
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

void Rol_PutFormToChangeMyRole (bool FormInHead)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Rol_Role_t Role;

   Act_FormStart (ActChgMyRol);
   fprintf (Gbl.F.Out,"<select name=\"UsrTyp\"");
   if (FormInHead)
      fprintf (Gbl.F.Out," style=\"width:130px;\"");
   fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);
   for (Role = Rol__GUEST_;
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
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /***** Get parameter with the new logged role ******/
   Par_GetParToText ("UsrTyp",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
     {
      /* Check if new role is a correct type *****/
      if (UnsignedNum >= Rol_NUM_ROLES)
         return;

      /* Check if new role is allowed for me */
      if (!(Gbl.Usrs.Me.AvailableRoles & (1 << UnsignedNum)))
         return;

      /* New role is correct and is allowed for me, so change my logged user type */
      Gbl.Usrs.Me.LoggedRole = (Rol_Role_t) UnsignedNum;

      /* Update logged role in session */
      Ses_UpdateSessionDataInDB ();
     }
  }

/*****************************************************************************/
/********************* Write selector of users' roles ************************/
/*****************************************************************************/

void Rol_WriteSelectorRoles (unsigned RolesAllowed,unsigned RolesSelected,
                             bool SendOnChange)
  {
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Rol_Role_t Role;

   for (Role = Rol_UNKNOWN;
        Role <= Rol_SYS_ADM;
        Role++)
      if ((RolesAllowed & (1 << Role)))
	{
	 fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"Roles\" value=\"%u\"",
		  (unsigned) Role);

	 if ((RolesSelected & (1 << Role)))
	    fprintf (Gbl.F.Out," checked=\"checked\"");

	 if (SendOnChange)
	    fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
		     Gbl.Form.Id);

	 fprintf (Gbl.F.Out," />%s<br />",
		  Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
        }
  }

/*****************************************************************************/
/******************** Put hidden param with users' roles *********************/
/*****************************************************************************/

void Rol_PutHiddenParamRoles (unsigned Role)
  {
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"Roles\" value=\"%u\" />",
	    Role);
  }

/*****************************************************************************/
/************************* Get selected users' roles *************************/
/*****************************************************************************/

void Rol_GetSelectedRoles (unsigned *Roles)
  {
   char StrRoles[(10+1)*2];
   const char *Ptr;
   char UnsignedStr[10+1];
   Rol_Role_t Role;

   Par_GetParMultiToText ("Roles",StrRoles,(10+1)*2);
   *Roles = 0;
   for (Ptr = StrRoles;
        *Ptr;)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,10);
      if (sscanf (UnsignedStr,"%u",&Role) != 1)
         Lay_ShowErrorAndExit ("can not get user's role");
      *Roles |= (1 << Role);
     }
  }

/*****************************************************************************/
/************ Get requested role of a user in current course *****************/
/*****************************************************************************/

Rol_Role_t Rol_GetRequestedRole (long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Rol_Role_t Role = Rol_UNKNOWN;

   /***** Get requested role from database *****/
   sprintf (Query,"SELECT Role FROM crs_usr_requests"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
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
