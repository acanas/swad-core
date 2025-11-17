// swad_role.c: user's roles

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

#include "swad_action_list.h"
#include "swad_admin_database.h"
#include "swad_connected_database.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_role.h"
#include "swad_role_database.h"
#include "swad_role_type.h"
#include "swad_session_database.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Rol_Icons[Rol_NUM_ROLES] =
  {
   [Rol_UNK    ] = "user.svg",
   [Rol_GST    ] = "user.svg",
   [Rol_USR    ] = "user.svg",
   [Rol_STD    ] = "user.svg",
   [Rol_NET    ] = "person-chalkboard.svg",
   [Rol_TCH    ] = "person-chalkboard.svg",
   [Rol_DEG_ADM] = "user-tie.svg",
   [Rol_CTR_ADM] = "user-tie.svg",
   [Rol_INS_ADM] = "user-tie.svg",
   [Rol_SYS_ADM] = "user-tie.svg",
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Set my roles ********************************/
/*****************************************************************************/

void Rol_SetMyRoles (void)
  {
   static Usr_Can_t ICanBe[Exi_NUM_EXIST] =
     {
      [Exi_DOES_NOT_EXIST] = Usr_CAN_NOT,
      [Exi_EXISTS        ] = Usr_CAN,
     };
   Usr_Can_t ICanBeAdm[Hie_NUM_LEVELS] =
     {
      [Hie_INS] = Usr_CAN_NOT,
      [Hie_CTR] = Usr_CAN_NOT,
      [Hie_DEG] = Usr_CAN_NOT,
     };

   /***** Get my role in current course if not yet filled *****/
   if (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs == Rol_UNK)
      Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs = Rol_GetMyRoleInCrs (Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /***** Set the user's role I am logged *****/
   Rol_GetRolesInAllCrss (&Gbl.Usrs.Me.UsrDat);	// Get my roles if not yet got
   Gbl.Usrs.Me.Role.Max = Rol_GetMaxRoleInCrss ((unsigned) Gbl.Usrs.Me.UsrDat.Roles.InCrss);

   /***** Set the user's role I am logged *****/
   // My logged role is retrieved in this order from:
   // 1. It may have been retrieved from last data stored in database just after login
   // 2. If it is not known, it will be retrieved from current session
   // 3. If a course is selected, it will be retrieved from my role in this course
   // 4. If none of the former options is satisfied, it will be set to user role
   if (Gbl.Usrs.Me.Role.Logged == Rol_UNK)					// No role from last data
     {
      if (Gbl.Usrs.Me.Role.FromSession == Rol_UNK)				// No role from session
        {
	 if (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs == Rol_UNK)			// No role in current course
	    Gbl.Usrs.Me.Role.Logged = Rol_USR;					// User
	 else
	    Gbl.Usrs.Me.Role.Logged = Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs;	// Role in current course
        }
      else
	 Gbl.Usrs.Me.Role.Logged = Gbl.Usrs.Me.Role.FromSession;		// Role from session
     }

   /***** Check if I am administrator of current institution/center/degree *****/
   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
     {
      /* Check if I am and administrator of current institution */
      ICanBeAdm[Hie_INS] = ICanBe[Adm_DB_CheckIfUsrExistsAsAdm (Gbl.Usrs.Me.UsrDat.UsrCod,Hie_INS)];
      if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
	{
	 /* Check if I am and administrator of current center */
	 ICanBeAdm[Hie_CTR] = ICanBe[Adm_DB_CheckIfUsrExistsAsAdm (Gbl.Usrs.Me.UsrDat.UsrCod,Hie_CTR)];
	 if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
	    /* Check if I am and administrator of current degree */
	    ICanBeAdm[Hie_DEG] = ICanBe[Adm_DB_CheckIfUsrExistsAsAdm (Gbl.Usrs.Me.UsrDat.UsrCod,Hie_DEG)];
	}
     }

   /***** Check if I belong to current course *****/
   if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
     {
      Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] = Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Me.UsrDat);
      switch (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
	{
	 case Usr_BELONG:
	    Gbl.Usrs.Me.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Me.UsrDat);
	    break;
	 case Usr_DONT_BELONG:
	 default:
	    Gbl.Usrs.Me.UsrDat.Accepted = Usr_HAS_NOT_ACCEPTED;
	    break;
	}
     }
   else					// No course selected
     {
      Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] = Usr_DONT_BELONG;
      Gbl.Usrs.Me.UsrDat.Accepted = Usr_HAS_NOT_ACCEPTED;
     }

   /***** Check if I belong to current degree *****/
   if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
      switch (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
	{
	 case Usr_BELONG:
	    Gbl.Usrs.Me.IBelongToCurrent[Hie_DEG] = Usr_BELONG;
	    break;
	 case Usr_DONT_BELONG:
	 default:
	    Gbl.Usrs.Me.IBelongToCurrent[Hie_DEG] = Hie_CheckIfIBelongTo (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	    break;
	}
   else
      Gbl.Usrs.Me.IBelongToCurrent[Hie_DEG] = Usr_DONT_BELONG;

   /***** Check if I belong to current center *****/
   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
      switch (Gbl.Usrs.Me.IBelongToCurrent[Hie_DEG])
	{
	 case Usr_BELONG:
	    Gbl.Usrs.Me.IBelongToCurrent[Hie_CTR] = Usr_BELONG;
	    break;
	 case Usr_DONT_BELONG:
	 default:
	    Gbl.Usrs.Me.IBelongToCurrent[Hie_CTR] = Hie_CheckIfIBelongTo (Hie_CTR,Gbl.Hierarchy.Node[Hie_CTR].HieCod);
	    break;
	}
   else
      Gbl.Usrs.Me.IBelongToCurrent[Hie_CTR] = Usr_DONT_BELONG;

   /***** Check if I belong to current institution *****/
   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
      switch (Gbl.Usrs.Me.IBelongToCurrent[Hie_CTR])
	{
	 case Usr_BELONG:
	    Gbl.Usrs.Me.IBelongToCurrent[Hie_INS] = Usr_BELONG;
	    break;
	 case Usr_DONT_BELONG:
	 default:
	    Gbl.Usrs.Me.IBelongToCurrent[Hie_INS] = Hie_CheckIfIBelongTo (Hie_INS,
									  Gbl.Hierarchy.Node[Hie_INS].HieCod);
	    break;
	}
   else
      Gbl.Usrs.Me.IBelongToCurrent[Hie_INS] = Usr_DONT_BELONG;

   /***** Build my list of available roles for current course *****/
   if (Gbl.Hierarchy.HieLvl == Hie_CRS)
      switch (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
	{
	 case Usr_BELONG:
	    Gbl.Usrs.Me.Role.Available = (1 << Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs);
	    break;
	 case Usr_DONT_BELONG:
	 default:
	    if (Gbl.Usrs.Me.Role.Max >= Rol_STD)
	       Gbl.Usrs.Me.Role.Available = (1 << Rol_USR);
	    else
	       Gbl.Usrs.Me.Role.Available = (1 << Rol_GST);
	    break;
	}
   else if (Gbl.Usrs.Me.Role.Max >= Rol_STD)
      Gbl.Usrs.Me.Role.Available = (1 << Rol_USR);
   else
      Gbl.Usrs.Me.Role.Available = (1 << Rol_GST);

   if (ICanBeAdm[Hie_INS] == Usr_CAN)
      Gbl.Usrs.Me.Role.Available |= (1 << Rol_INS_ADM);
   if (ICanBeAdm[Hie_CTR] == Usr_CAN)
      Gbl.Usrs.Me.Role.Available |= (1 << Rol_CTR_ADM);
   if (ICanBeAdm[Hie_DEG] == Usr_CAN)
      Gbl.Usrs.Me.Role.Available |= (1 << Rol_DEG_ADM);
   if (Usr_CheckIfUsrExistsAsSuperuser (Gbl.Usrs.Me.UsrDat.UsrCod) == Exi_EXISTS)
      Gbl.Usrs.Me.Role.Available |= (1 << Rol_SYS_ADM);

   /***** Check if the role I am logged is now available for me (it's not forbidden) *****/
   if (!(Gbl.Usrs.Me.Role.Available &
	 (1 << Gbl.Usrs.Me.Role.Logged)))        // Current type I am logged is not available for me
      /* Set the lowest role available for me */
      for (Gbl.Usrs.Me.Role.Logged  = (Rol_Role_t) 0;
           Gbl.Usrs.Me.Role.Logged <= (Rol_Role_t) (Rol_NUM_ROLES - 1);
           Gbl.Usrs.Me.Role.Logged++)
         if (Gbl.Usrs.Me.Role.Available & (1 << Gbl.Usrs.Me.Role.Logged))
            break;
  }

/*****************************************************************************/
/****************** Get number of available roles for me *********************/
/*****************************************************************************/

unsigned Rol_GetNumAvailableRoles (void)
  {
   Rol_Role_t Role;
   unsigned NumAvailableRoles = 0;

   for (Role  = (Rol_Role_t) 1;
        Role <= (Rol_Role_t) Rol_NUM_ROLES - 1;
        Role++)
      if (Gbl.Usrs.Me.Role.Available & (1 << Role))
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

   /* Not a teacher. Check then if user is a non-editing teacher in any course */
   if (Roles & (1 << Rol_NET))
      return Rol_NET;

   /* Not a non-editing teacher or a teacher. Check then if user is a student in any course */
   if (Roles & (1 << Rol_STD))
      return Rol_STD;

   /* Not a teacher or student */
   return Rol_GST;	// Guest means that this user is not registered in any course
  }

/*****************************************************************************/
/***** Get my maximum role in a course/degree/center/institution/country *****/
/*****************************************************************************/

Rol_Role_t Rol_GetMyMaxRoleIn (Hie_Level_t HieLvl,long HieCod)
  {
   unsigned NumMyNode;

   /***** 1. Fast check: trivial cases *****/
   if (HieCod <= 0)
      return Rol_UNK;

   /***** Fill the list with the nodes I belong to (if not already filled) *****/
   Hie_GetMyHierarchy (HieLvl);

   /***** Check if the node passed as parameter is any of my nodes *****/
   for (NumMyNode = 0;
	NumMyNode < Gbl.Usrs.Me.Hierarchy[HieLvl].Num;
	NumMyNode++)
      if (Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes[NumMyNode].HieCod == HieCod)
	 return Gbl.Usrs.Me.Hierarchy[HieLvl].Nodes[NumMyNode].MaxRole;

   return Rol_GST;
  }

/*****************************************************************************/
/*************************** Get my role in a course *************************/
/*****************************************************************************/

void Rol_FlushCacheMyRoleInCurrentCrs (void)
  {
   Gbl.Cache.MyRoleInCurrentCrs.Status = Cac_INVALID;
  }

Rol_Role_t Rol_GetMyRoleInCrs (long HieCod)
  {
   Rol_Role_t Role;

   /***** 1. Fast check: trivial cases *****/
   if (HieCod <= 0)
      return Rol_UNK;

   /***** 2. Fast check: is my role in current course already calculated? *****/
   if (Gbl.Cache.MyRoleInCurrentCrs.Status == Cac_VALID &&
       HieCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      return Gbl.Cache.MyRoleInCurrentCrs.Role;

   /***** 3. Slow check: get my role from list of my courses *****/
   Role = Rol_GetMyMaxRoleIn (Hie_CRS,HieCod);

   /* Update my role in current course */
   if (HieCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod)
     {
      Gbl.Cache.MyRoleInCurrentCrs.Role   = Role;
      Gbl.Cache.MyRoleInCurrentCrs.Status = Cac_VALID;
     }

   return Role;
  }

/*****************************************************************************/
/********************** Get role of a user in a course ***********************/
/*****************************************************************************/

void Rol_FlushCacheRoleUsrInCrs (void)
  {
   Gbl.Cache.RoleUsrInCrs.Status = Cac_INVALID;
  }

Rol_Role_t Rol_GetRoleUsrInCrs (long UsrCod,long HieCod)
  {
   /***** 1. Fast check: trivial cases *****/
   if (UsrCod <= 0 ||
       HieCod <= 0)
      return Rol_UNK;

   /***** 2. Fast check: Is role in course already calculated? *****/
   if (Gbl.Cache.RoleUsrInCrs.Status == Cac_VALID &&
       UsrCod == Gbl.Cache.RoleUsrInCrs.UsrCod &&
       HieCod == Gbl.Cache.RoleUsrInCrs.HieCod)
      return Gbl.Cache.RoleUsrInCrs.Role;

   /***** 3. Slow check: Get role of a user in a course from database.
			 The result of the query will have one row or none *****/
   Gbl.Cache.RoleUsrInCrs.UsrCod = UsrCod;
   Gbl.Cache.RoleUsrInCrs.HieCod = HieCod;
   if (UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      /* Get my role in course */
      Gbl.Cache.RoleUsrInCrs.Role = Rol_GetMyRoleInCrs (HieCod);
   else
      /* Get role of the user in course from database */
      Gbl.Cache.RoleUsrInCrs.Role = Rol_DB_GetRoleUsrInCrs (UsrCod,HieCod);
   Gbl.Cache.RoleUsrInCrs.Status = Cac_VALID;
   return Gbl.Cache.RoleUsrInCrs.Role;
  }

/*****************************************************************************/
/**************** Get roles of a user in all his/her courses *****************/
/*****************************************************************************/
// Roles >=0 ==> already filled/calculated ==> nothing to do
// Roles  <0 ==> not yet filled/calculated ==> get roles

void Rol_GetRolesInAllCrss (struct Usr_Data *UsrDat)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRole;
   unsigned NumRoles;

   /***** Trivial check: if already filled, nothing to do *****/
   if (UsrDat->Roles.InCrss >= 0)
      return;

   /***** Get distinct roles in all courses of the user from database *****/
   NumRoles = Rol_DB_GetRolesInAllCrss (&mysql_res,UsrDat->UsrCod);
   for (NumRole = 0, UsrDat->Roles.InCrss = 0;
	NumRole < NumRoles;
	NumRole++)
     {
      row = mysql_fetch_row (mysql_res);
      UsrDat->Roles.InCrss |= (int) (1 << Rol_ConvertUnsignedStrToRole (row[0]));
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
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

void Rol_PutFormToChangeMyRole (const char *ClassSelect)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Rol_Role_t Role;
   unsigned RoleUnsigned;
   bool PutClassSelect;

   /***** Begin form *****/
   Frm_BeginForm (ActChgMyRol);

      PutClassSelect = false;
      if (ClassSelect)
	 if (ClassSelect[0])
	    PutClassSelect = true;
      if (PutClassSelect)
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			   "name=\"MyRole\" class=\"%s\"",ClassSelect);
      else
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			   "name=\"MyRole\"");
      for (Role  = (Rol_Role_t) 1;
	   Role <= (Rol_Role_t) (Rol_NUM_ROLES - 1);
	   Role++)
	if (Gbl.Usrs.Me.Role.Available & (1 << Role))
	   {
	    RoleUnsigned = (unsigned) Role;
	    HTM_OPTION (HTM_Type_UNSIGNED,&RoleUnsigned,
			Role == Gbl.Usrs.Me.Role.Logged ? HTM_SELECTED :
						   	  HTM_NO_ATTR,
			"%s",Txt_ROLES_SINGUL_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
	   }
      HTM_SELECT_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************************** Change my role *******************************/
/*****************************************************************************/

void Rol_ChangeMyRole (void)
  {
   Rol_Role_t NewRole;

   /***** Get parameter with the new logged role ******/
   NewRole = (Rol_Role_t)
	     Par_GetParUnsignedLong ("MyRole",
                                     0,
                                     Rol_NUM_ROLES - 1,
                                     (unsigned long) Rol_UNK);
   if (NewRole != Rol_UNK)
     {
      /* Check if new role is allowed for me */
      if (!(Gbl.Usrs.Me.Role.Available & (1 << NewRole)))
         return;

      /* New role is correct and is allowed for me */
      if (NewRole != Gbl.Usrs.Me.Role.Logged)
	{
         /* New role is distinct to current role,
            so change my role... */
	 Gbl.Usrs.Me.Role.Logged = NewRole;
	 Gbl.Usrs.Me.Role.HasChanged = true;

	 /* ...update logged role in session... */
	 Ses_DB_UpdateSession ();

	 /* ...and update logged role in list of connected */
	 Con_DB_UpdateMeInConnectedList ();
	}
     }
  }

/*****************************************************************************/
/********************* Write selector of users' roles ************************/
/*****************************************************************************/

void Rol_WriteSelectorRoles (unsigned RolesAllowed,unsigned RolesSelected,
                             HTM_Attributes_t Attributes)
  {
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Rol_Role_t Role;

   for (Role  = Rol_UNK;
        Role <= Rol_SYS_ADM;
        Role++)
      if ((RolesAllowed & (1 << Role)))
	{
	 HTM_LABEL_Begin (NULL);
	    HTM_INPUT_CHECKBOX ("Role",
				Attributes |
				((RolesSelected & (1 << Role)) ? HTM_CHECKED :
							         HTM_NO_ATTR),
				"id=\"Role\" value=\"%u\" class=\"INPUT_%s\"",
				(unsigned) Role,The_GetSuffix ());
	    HTM_Txt (Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
	 HTM_LABEL_End ();
	 HTM_BR ();
        }
  }

/*****************************************************************************/
/******************** Put hidden param with users' roles *********************/
/*****************************************************************************/

void Rol_PutParRoles (unsigned Roles)
  {
   Par_PutParUnsigned (NULL,"Roles",Roles);
  }

/*****************************************************************************/
/************************* Get selected users' roles *************************/
/*****************************************************************************/

unsigned Rol_GetSelectedRoles (void)
  {
   char StrRoles[Rol_NUM_ROLES * (Cns_MAX_DIGITS_UINT + 1)];
   const char *Ptr;
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];
   Rol_Role_t Role;
   unsigned Roles;

   /***** Try to get param "Roles" with multiple roles *****/
   Roles = (unsigned)
	   Par_GetParUnsignedLong ("Roles",
                                   0,				// 000...000
                                   (1 << Rol_NUM_ROLES) - 1,	// 111...111
                                   0);				// 000...000

   /***** Try to get multiple param "Role" *****/
   Par_GetParMultiToText ("Role",StrRoles,Rol_NUM_ROLES * (Cns_MAX_DIGITS_UINT + 1));
   for (Ptr = StrRoles;
        *Ptr;
       )
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UnsignedStr,Cns_MAX_DIGITS_UINT);
      Role = Rol_ConvertUnsignedStrToRole (UnsignedStr);
      if (Role != Rol_UNK)
         Roles |= (1 << Role);
     }

   return Roles;
  }
