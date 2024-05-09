// swad_agenda.c: user's agenda (personal organizer)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_agenda.h"
#include "swad_agenda_database.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_QR.h"
#include "swad_setting.h"
#include "swad_string.h"
#include "swad_www.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *ParPast__FutureName = "Past__Future";
static const char *ParPrivatPublicName = "PrivatPublic";
static const char *ParHiddenVisiblName = "HiddenVisibl";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Agd_ResetAgenda (struct Agd_Agenda *Agenda);

static void Agd_ShowMyAgenda (struct Agd_Agenda *Agenda);

static void Agd_ShowFormToSelPast__FutureEvents (const struct Agd_Agenda *Agenda);
static void Agd_ShowFormToSelPrivatPublicEvents (const struct Agd_Agenda *Agenda);
static void Agd_ShowFormToSelHiddenVisiblEvents (const struct Agd_Agenda *Agenda);

static void Agd_PutParPast__FutureEvents (unsigned Past__FutureEvents);
static void Agd_PutParPrivatPublicEvents (unsigned PrivatPublicEvents);
static void Agd_PutParHiddenVisiblEvents (unsigned HiddenVisiblEvents);
static unsigned Agd_GetParsPast__FutureEvents (void);
static unsigned Agd_GetParsPrivatPublicEvents (void);
static unsigned Agd_GetParsHiddenVisiblEvents (void);

static void Agd_ShowEvents (struct Agd_Agenda *Agenda,
                            Agd_AgendaType_t AgendaType);
static void Agd_ShowEventsToday (struct Agd_Agenda *Agenda,
                                 Agd_AgendaType_t AgendaType);
static void Agd_WriteHeaderListEvents (const struct Agd_Agenda *Agenda,
                                       Agd_AgendaType_t AgendaType);

static void Agd_PutIconsMyFullAgenda (void *Agenda);
static void Agd_PutIconsMyPublicAgenda (void *EncryptedUsrCod);
static void Agd_PutIconToCreateNewEvent (void *Agenda);
static void Agd_PutIconToViewEditMyFullAgenda (void *EncryptedUsrCod);
static void Agd_PutIconToShowQR (void);
static void Agd_PutIconsOtherPublicAgenda (void *EncryptedUsrCod);

static void Agd_ShowOneEvent (struct Agd_Agenda *Agenda,
                              Agd_AgendaType_t AgendaType,long AgdCod);
static void Agd_GetParEventOrder (struct Agd_Agenda *Agenda);
static void Agd_PutFormsToRemEditOneEvent (struct Agd_Agenda *Agenda,
                                           struct Agd_Event *AgdEvent,
                                           const char *Anchor);

static void Agd_PutCurrentParsMyAgenda (void *Agenda);
static void Agd_GetPars (struct Agd_Agenda *Agenda,
                           Agd_AgendaType_t AgendaType);

static void Agd_GetListEvents (struct Agd_Agenda *Agenda,
                               Agd_AgendaType_t AgendaType);
static void Agd_GetventDataByCod (struct Agd_Event *AgdEvent);

static void Agd_FreeListEvents (struct Agd_Agenda *Agenda);

static void Agd_HideUnhideEvent (HidVis_HiddenOrVisible_t HiddenOrVisible);

/*****************************************************************************/
/******************* Check if I can view a user's agenda *********************/
/*****************************************************************************/

Usr_Can_t Agd_CheckIfICanViewUsrAgenda (struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return Usr_CAN_NOT;

   /***** 2. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return Usr_CAN;

   /***** 3. Fast check: Am I logged as system admin? *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return Usr_CAN;

   /***** 4. Slow check: Get if user shares any course with me from database *****/
   return Enr_CheckIfUsrSharesAnyOfMyCrs (UsrDat) ? Usr_CAN :
						    Usr_CAN_NOT;
  }

/*****************************************************************************/
/*************************** Reset agenda context ****************************/
/*****************************************************************************/

static void Agd_ResetAgenda (struct Agd_Agenda *Agenda)
  {
   Agenda->LstIsRead          = false;
   Agenda->Num                = 0;
   Agenda->LstAgdCods         = NULL;
   Agenda->Past__FutureEvents = Agd_DEFAULT_PAST___EVENTS |
	                        Agd_DEFAULT_FUTURE_EVENTS;
   Agenda->PrivatPublicEvents = Agd_DEFAULT_PRIVAT_EVENTS |
	                        Agd_DEFAULT_PUBLIC_EVENTS;
   Agenda->HiddenVisiblEvents = Agd_DEFAULT_HIDDEN_EVENTS |
	                        Agd_DEFAULT_VISIBL_EVENTS;
   Agenda->SelectedOrder      = Agd_ORDER_DEFAULT;
   Agenda->AgdCodToEdit       = -1L;
   Agenda->CurrentPage        = 0;
  }

/*****************************************************************************/
/********** Put form to log in and then show another user's agenda ***********/
/*****************************************************************************/

void Agd_PutFormLogInToShowUsrAgenda (void)
  {
   Usr_WriteFormLogin (ActLogInUsrAgd,Agd_PutParAgd);
  }

void Agd_PutParAgd (void)
  {
   char NickWithArr[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];

   snprintf (NickWithArr,sizeof (NickWithArr),"@%s",Gbl.Usrs.Other.UsrDat.Nickname);
   Par_PutParString (NULL,"agd",NickWithArr);
  }

/*****************************************************************************/
/******************************* Show my agenda ******************************/
/*****************************************************************************/

void Agd_GetParsAndShowMyAgenda (void)
  {
   struct Agd_Agenda Agenda;

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Show my agenda *****/
   Agd_ShowMyAgenda (&Agenda);
  }

static void Agd_ShowMyAgenda (struct Agd_Agenda *Agenda)
  {
   extern const char *Hlp_PROFILE_Agenda;
   extern const char *Txt_My_agenda;

   /***** Begin box *****/
   Box_BoxBegin (Txt_My_agenda,Agd_PutIconsMyFullAgenda,Agenda,
		 Hlp_PROFILE_Agenda,Box_NOT_CLOSABLE);

      /***** Put forms to choice which events to show *****/
      Set_BeginSettingsHead ();
      Agd_ShowFormToSelPast__FutureEvents (Agenda);
      Agd_ShowFormToSelPrivatPublicEvents (Agenda);
      Agd_ShowFormToSelHiddenVisiblEvents (Agenda);
      Set_EndSettingsHead ();

      /***** Show the current events in the user's agenda *****/
      Agd_ShowEventsToday (Agenda,Agd_MY_AGENDA_TODAY);

      /***** Show all my events *****/
      Agd_ShowEvents (Agenda,Agd_MY_AGENDA);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Show form to select past / future events ********************/
/*****************************************************************************/

static void Agd_ShowFormToSelPast__FutureEvents (const struct Agd_Agenda *Agenda)
  {
   extern const char *Txt_AGENDA_PAST___FUTURE_EVENTS[Agd_NUM_PAST_FUTURE_EVENTS];
   Agd_Past__FutureEvents_t PstFut;
   static const char *Icon[Agd_NUM_PAST_FUTURE_EVENTS] =
     {
      [Agd_PAST___EVENTS] = "calendar-minus.svg",
      [Agd_FUTURE_EVENTS] = "calendar-plus.svg",
     };

   Set_BeginOneSettingSelector ();
   for (PstFut  = Agd_PAST___EVENTS;
	PstFut <= Agd_FUTURE_EVENTS;
	PstFut++)
     {
      Set_BeginPref ((Agenda->Past__FutureEvents & (1 << PstFut)));
	 Frm_BeginForm (ActSeeMyAgd);
	    Agd_PutParsMyAgenda (Agenda->Past__FutureEvents ^ (1 << PstFut),	// Toggle
				 Agenda->PrivatPublicEvents,
				 Agenda->HiddenVisiblEvents,
				 Agenda->SelectedOrder,
				 Agenda->CurrentPage,
				 -1L);
	    Ico_PutSettingIconLink (Icon[PstFut],Ico_BLACK,
				    Txt_AGENDA_PAST___FUTURE_EVENTS[PstFut]);
	 Frm_EndForm ();
      Set_EndPref ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************** Show form to select private / public events ******************/
/*****************************************************************************/

static void Agd_ShowFormToSelPrivatPublicEvents (const struct Agd_Agenda *Agenda)
  {
   extern const char *Txt_AGENDA_PRIVAT_PUBLIC_EVENTS[2];
   Agd_PrivatPublicEvents_t PrvPub;
   static const struct
     {
      const char *Icon;
      Ico_Color_t Color;
     } Icon[Agd_NUM_PRIVAT_PUBLIC_EVENTS] =
     {
      [Agd_PRIVAT_EVENTS] = {"lock.svg"  ,Ico_RED  },
      [Agd_PUBLIC_EVENTS] = {"unlock.svg",Ico_GREEN},
     };

   Set_BeginOneSettingSelector ();
   for (PrvPub  = Agd_PRIVAT_EVENTS;
	PrvPub <= Agd_PUBLIC_EVENTS;
	PrvPub++)
     {
      Set_BeginPref ((Agenda->PrivatPublicEvents & (1 << PrvPub)));
	 Frm_BeginForm (ActSeeMyAgd);
	    Agd_PutParsMyAgenda (Agenda->Past__FutureEvents,
				 Agenda->PrivatPublicEvents ^ (1 << PrvPub),	// Toggle
				 Agenda->HiddenVisiblEvents,
				 Agenda->SelectedOrder,
				 Agenda->CurrentPage,
				 -1L);
	    Ico_PutSettingIconLink (Icon[PrvPub].Icon,Icon[PrvPub].Color,
				    Txt_AGENDA_PRIVAT_PUBLIC_EVENTS[PrvPub]);
	 Frm_EndForm ();
      Set_EndPref ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************* Show form to select hidden / visible events *******************/
/*****************************************************************************/

static void Agd_ShowFormToSelHiddenVisiblEvents (const struct Agd_Agenda *Agenda)
  {
   extern struct Ico_IconColor Ico_HiddenVisible[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_AGENDA_HIDDEN_VISIBLE_EVENTS[HidVis_NUM_HIDDEN_VISIBLE];
   HidVis_HiddenOrVisible_t HiddenOrVisible;

   Set_BeginOneSettingSelector ();
   for (HiddenOrVisible  = HidVis_HIDDEN;
	HiddenOrVisible <= HidVis_VISIBLE;
	HiddenOrVisible++)
     {
      Set_BeginPref ((Agenda->HiddenVisiblEvents & (1 << HiddenOrVisible)));
	 Frm_BeginForm (ActSeeMyAgd);
	    Agd_PutParsMyAgenda (Agenda->Past__FutureEvents,
				 Agenda->PrivatPublicEvents,
				 Agenda->HiddenVisiblEvents ^ (1 << HiddenOrVisible),	// Toggle
				 Agenda->SelectedOrder,
				 Agenda->CurrentPage,
				 -1L);
	    Ico_PutSettingIconLink (Ico_HiddenVisible[HiddenOrVisible].Icon,
	                            Ico_HiddenVisible[HiddenOrVisible].Color,
				    Txt_AGENDA_HIDDEN_VISIBLE_EVENTS[HiddenOrVisible]);
	 Frm_EndForm ();
      Set_EndPref ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************************ Put hidden params for events ***********************/
/*****************************************************************************/

static void Agd_PutParPast__FutureEvents (unsigned Past__FutureEvents)
  {
   Par_PutParUnsigned (NULL,ParPast__FutureName,Past__FutureEvents);
  }

static void Agd_PutParPrivatPublicEvents (unsigned PrivatPublicEvents)
  {
   Par_PutParUnsigned (NULL,ParPrivatPublicName,PrivatPublicEvents);
  }

static void Agd_PutParHiddenVisiblEvents (unsigned HiddenVisiblEvents)
  {
   Par_PutParUnsigned (NULL,ParHiddenVisiblName,HiddenVisiblEvents);
  }

/*****************************************************************************/
/************************ Get hidden params for events ***********************/
/*****************************************************************************/

static unsigned Agd_GetParsPast__FutureEvents (void)
  {
   return (unsigned) Par_GetParUnsignedLong (ParPast__FutureName,
					     0,
					     (1 << Agd_PAST___EVENTS) |
					     (1 << Agd_FUTURE_EVENTS),
					     Agd_DEFAULT_PAST___EVENTS |
					     Agd_DEFAULT_FUTURE_EVENTS);
  }

static unsigned Agd_GetParsPrivatPublicEvents (void)
  {
   return (unsigned) Par_GetParUnsignedLong (ParPrivatPublicName,
					     0,
					     (1 << Agd_PRIVAT_EVENTS) |
					     (1 << Agd_PUBLIC_EVENTS),
					     Agd_DEFAULT_PRIVAT_EVENTS |
					     Agd_DEFAULT_PUBLIC_EVENTS);
  }

static unsigned Agd_GetParsHiddenVisiblEvents (void)
  {
   return (unsigned) Par_GetParUnsignedLong (ParHiddenVisiblName,
					     0,
					     (1 << HidVis_HIDDEN) |
					     (1 << HidVis_VISIBLE),
					     Agd_DEFAULT_HIDDEN_EVENTS |
					     Agd_DEFAULT_VISIBL_EVENTS);
  }

/*****************************************************************************/
/************************ Show another user's agenda *************************/
/*****************************************************************************/

void Agd_ShowUsrAgenda (void)
  {
   extern const char *Hlp_PROFILE_Agenda_public_agenda;
   extern const char *Txt_Public_agenda_USER;
   struct Agd_Agenda Agenda;
   Usr_MeOrOther_t MeOrOther;
   char *Title;
   static struct Usr_Data *UsrDat[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = &Gbl.Usrs.Me.UsrDat,
      [Usr_OTHER] = &Gbl.Usrs.Other.UsrDat,
     };
   static void (*FuncPutIcons[Usr_NUM_ME_OR_OTHER]) (void *EncryptedUsrCod) =
     {
      [Usr_ME   ] = Agd_PutIconsMyPublicAgenda,
      [Usr_OTHER] = Agd_PutIconsOtherPublicAgenda,
     };

   /***** Get user *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
      switch (Agd_CheckIfICanViewUsrAgenda (&Gbl.Usrs.Other.UsrDat))
	{
	 case Usr_CAN:
	    /***** Reset agenda context *****/
	    Agd_ResetAgenda (&Agenda);

	    /***** Begin box *****/
	    MeOrOther = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
	    if (asprintf (&Title,Txt_Public_agenda_USER,UsrDat[MeOrOther]->FullName) < 0)
	       Err_NotEnoughMemoryExit ();
	    Box_BoxBegin (Title,
			  FuncPutIcons[MeOrOther],UsrDat[MeOrOther]->EnUsrCod,
			  Hlp_PROFILE_Agenda_public_agenda,Box_NOT_CLOSABLE);
	    free (Title);

	       /***** Show the current events in the user's agenda *****/
	       Agd_ShowEventsToday (&Agenda,Agd_ANOTHER_AGENDA_TODAY);

	       /***** Show all visible events in the user's agenda *****/
	       Agd_ShowEvents (&Agenda,Agd_ANOTHER_AGENDA);

	    /***** End box *****/
	    Box_BoxEnd ();
	    break;
         case Usr_CAN_NOT:
         default:
            Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
            break;
	}
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/***************** Show another user's agenda after log in *******************/
/*****************************************************************************/

void Agd_ShowOtherAgendaAfterLogIn (void)
  {
   extern const char *Hlp_PROFILE_Agenda_public_agenda;
   extern unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *Txt_Public_agenda_USER;
   extern const char *Txt_Switching_to_LANGUAGE[1 + Lan_NUM_LANGUAGES];
   struct Agd_Agenda Agenda;
   Usr_MeOrOther_t MeOrOther;
   char *Title;
   static struct Usr_Data *UsrDat[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = &Gbl.Usrs.Me.UsrDat,
      [Usr_OTHER] = &Gbl.Usrs.Other.UsrDat,
     };
   static void (*FuncPutIcons[Usr_NUM_ME_OR_OTHER]) (void *EncryptedUsrCod) =
     {
      [Usr_ME   ] = Agd_PutIconToViewEditMyFullAgenda,
      [Usr_OTHER] = Agd_PutIconsOtherPublicAgenda,
     };

   if (Gbl.Usrs.Me.Logged)
     {
      if (Gbl.Usrs.Me.UsrDat.Prefs.Language == Txt_Current_CGI_SWAD_Language)
        {
	 /***** Get user *****/
	 /* If nickname is correct, user code is already got from nickname */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,        // Existing user
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CRS))
	   {
	    /***** Reset agenda context *****/
	    Agd_ResetAgenda (&Agenda);

	    /***** Begin box *****/
	    MeOrOther = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
	    if (asprintf (&Title,Txt_Public_agenda_USER,UsrDat[MeOrOther]->FullName) < 0)
	       Err_NotEnoughMemoryExit ();
	    Box_BoxBegin (Title,
	                  FuncPutIcons[MeOrOther],UsrDat[MeOrOther]->EnUsrCod,
			  Hlp_PROFILE_Agenda_public_agenda,Box_NOT_CLOSABLE);
            free (Title);

	       /***** Show the current events in the user's agenda *****/
	       Agd_ShowEventsToday (&Agenda,Agd_ANOTHER_AGENDA_TODAY);

	       /***** Show all visible events in the user's agenda *****/
	       Agd_ShowEvents (&Agenda,Agd_ANOTHER_AGENDA);

	    /***** End box *****/
	    Box_BoxEnd ();
           }
	 else
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
       }
      else
	 /* The current language is not my preferred language
	    ==> change automatically to my language */
         Ale_ShowAlert (Ale_INFO,Txt_Switching_to_LANGUAGE[Gbl.Usrs.Me.UsrDat.Prefs.Language]);
     }
  }

/*****************************************************************************/
/*************************** Show events in agenda ***************************/
/*****************************************************************************/

static void Agd_ShowEvents (struct Agd_Agenda *Agenda,
                            Agd_AgendaType_t AgendaType)
  {
   extern const char *Hlp_PROFILE_Agenda;
   extern const char *Txt_Public_agenda_USER;
   extern const char *Txt_My_agenda;
   extern const char *Txt_No_events;
   struct Pag_Pagination Pagination;
   unsigned NumEvent;
   static const Pag_WhatPaginate_t WhatPaginate[Agd_NUM_AGENDA_TYPES] =
     {
      [Agd_MY_AGENDA_TODAY     ] = Pag_MY_AGENDA,	// not used
      [Agd_MY_AGENDA           ] = Pag_MY_AGENDA,
      [Agd_ANOTHER_AGENDA_TODAY] = Pag_ANOTHER_AGENDA,	// not used
      [Agd_ANOTHER_AGENDA      ] = Pag_ANOTHER_AGENDA,
     };

   /***** Get parameters *****/
   Agd_GetPars (Agenda,AgendaType);

   /***** Get list of events *****/
   Agd_GetListEvents (Agenda,AgendaType);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Agenda->Num;
   Pagination.CurrentPage = (int) Agenda->CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Agenda->CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   Pag_WriteLinksToPagesCentered (WhatPaginate[AgendaType],&Pagination,
				  Agenda,-1L);

   if (Agenda->Num)
     {
      /***** Begin table *****/
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Table head *****/
	 Agd_WriteHeaderListEvents (Agenda,AgendaType);

	 /***** Write all events *****/
	 for (NumEvent  = Pagination.FirstItemVisible, The_ResetRowColor ();
	      NumEvent <= Pagination.LastItemVisible;
	      NumEvent++, The_ChangeRowColor ())
	    Agd_ShowOneEvent (Agenda,AgendaType,Agenda->LstAgdCods[NumEvent - 1]);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_No_events);

   /***** Write again links to pages *****/
   Pag_WriteLinksToPagesCentered (WhatPaginate[AgendaType],&Pagination,
				  Agenda,-1L);

   /***** Free list of events *****/
   Agd_FreeListEvents (Agenda);
  }

/*****************************************************************************/
/************************ Show today events in agenda ************************/
/*****************************************************************************/

static void Agd_ShowEventsToday (struct Agd_Agenda *Agenda,
                                 Agd_AgendaType_t AgendaType)
  {
   extern const char *Hlp_PROFILE_Agenda;
   extern const char *Hlp_PROFILE_Agenda_public_agenda;
   extern const char *Txt_Today;
   extern const char *Txt_Public_agenda_USER;
   extern const char *Txt_My_agenda;
   extern const char *Txt_No_events;
   unsigned NumEvent;

   /***** Get parameters *****/
   Agd_GetPars (Agenda,AgendaType);

   /***** Get list of events *****/
   Agd_GetListEvents (Agenda,AgendaType);

   if (Agenda->Num)
     {
      /***** Begin box and table *****/
      switch (AgendaType)
        {
	 case Agd_MY_AGENDA_TODAY:
	    Box_BoxTableShadowBegin (Txt_Today,NULL,NULL,
				     Hlp_PROFILE_Agenda,2);
	    break;
	 case Agd_ANOTHER_AGENDA_TODAY:
	    Box_BoxTableShadowBegin (Txt_Today,NULL,NULL,
			             Hlp_PROFILE_Agenda_public_agenda,2);
            break;
	 default:
	    break;
        }

	 /***** Table head *****/
	 Agd_WriteHeaderListEvents (Agenda,AgendaType);

	 /***** Write all events *****/
	 for (NumEvent = 0, The_ResetRowColor ();
	      NumEvent < Agenda->Num;
	      NumEvent++, The_ChangeRowColor ())
	    Agd_ShowOneEvent (Agenda,AgendaType,Agenda->LstAgdCods[NumEvent]);

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free list of events *****/
   Agd_FreeListEvents (Agenda);
  }

/*****************************************************************************/
/*************** Put contextual icon to view/edit my agenda ******************/
/*****************************************************************************/

static void Agd_WriteHeaderListEvents (const struct Agd_Agenda *Agenda,
                                       Agd_AgendaType_t AgendaType)
  {
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Event;
   extern const char *Txt_Location;
   Dat_StartEndTime_t Order;

   /***** Table head *****/
   HTM_TR_Begin (NULL);

      for (Order  = (Dat_StartEndTime_t) 0;
	   Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   Order++)
	{
         HTM_TH_Begin (HTM_HEAD_LEFT);
	    switch (AgendaType)
	      {
	       case Agd_MY_AGENDA_TODAY:
	       case Agd_MY_AGENDA:
		  Frm_BeginForm (ActSeeMyAgd);
		     Pag_PutParPagNum (Pag_MY_AGENDA,Agenda->CurrentPage);
		  break;
	       case Agd_ANOTHER_AGENDA_TODAY:
	       case Agd_ANOTHER_AGENDA:
		  Frm_BeginForm (ActSeeUsrAgd);
		     Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
		     Pag_PutParPagNum (Pag_ANOTHER_AGENDA,Agenda->CurrentPage);
		  break;
	      }
	    Agd_PutParsMyAgenda (Agenda->Past__FutureEvents,
				 Agenda->PrivatPublicEvents,
				 Agenda->HiddenVisiblEvents,
				 Order,
				 Agenda->CurrentPage,
				 -1L);

	       HTM_BUTTON_Submit_Begin (Txt_START_END_TIME_HELP[Order],
	                                "class=\"BT_LINK\"");
		  if (Order == Agenda->SelectedOrder)
		     HTM_U_Begin ();

		  HTM_Txt (Txt_START_END_TIME[Order]);

		  if (Order == Agenda->SelectedOrder)
		     HTM_U_End ();
	       HTM_BUTTON_End ();

	    Frm_EndForm ();
	 HTM_TH_End ();
	}

      HTM_TH (Txt_Event   ,HTM_HEAD_LEFT);
      HTM_TH (Txt_Location,HTM_HEAD_LEFT);

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Put contextual icons in agenda ***********************/
/*****************************************************************************/

static void Agd_PutIconsMyFullAgenda (void *Agenda)
  {
   /***** Put icon to create a new event *****/
   Agd_PutIconToCreateNewEvent (Agenda);

   /***** Put icon to show QR code *****/
   Agd_PutIconToShowQR ();
  }

static void Agd_PutIconsMyPublicAgenda (void *EncryptedUsrCod)
  {
   /***** Put icon to view/edit my full agenda *****/
   Agd_PutIconToViewEditMyFullAgenda (EncryptedUsrCod);

   /***** Put icon to show QR code *****/
   Agd_PutIconToShowQR ();
  }

static void Agd_PutIconToCreateNewEvent (void *Agenda)
  {
   ((struct Agd_Agenda *) Agenda)->AgdCodToEdit = -1L;
   Ico_PutContextualIconToAdd (ActFrmNewEvtMyAgd,NULL,
			       Agd_PutCurrentParsMyAgenda,Agenda);
  }

static void Agd_PutIconToViewEditMyFullAgenda (void *EncryptedUsrCod)
  {
   Ico_PutContextualIconToEdit (ActSeeMyAgd,NULL,NULL,EncryptedUsrCod);
  }

static void Agd_PutIconToShowQR (void)
  {
   char URL[WWW_MAX_BYTES_WWW + 1];
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   snprintf (URL,sizeof (URL),"%s/%s?agd=@%s",
             Cfg_URL_SWAD_CGI,Lan_STR_LANG_ID[Gbl.Prefs.Language],
             Gbl.Usrs.Me.UsrDat.Nickname);
   QR_PutLinkToPrintQRCode (ActPrnAgdQR,
                            QR_PutParQRString,URL);
  }

static void Agd_PutIconsOtherPublicAgenda (void *EncryptedUsrCod)
  {
   /***** Button to view user's public profile *****/
   if (Pri_CheckIfICanView (Gbl.Usrs.Other.UsrDat.BaPrfVisibility,
		            &Gbl.Usrs.Other.UsrDat) == Usr_CAN)
      Lay_PutContextualLinkOnlyIcon (ActSeeOthPubPrf,NULL,
                                     Usr_PutParOtherUsrCodEncrypted,EncryptedUsrCod,
			             "user.svg",Ico_BLACK);

   /***** Button to view user's record card *****/
   if (Usr_CheckIfICanViewRecordStd (&Gbl.Usrs.Other.UsrDat) == Usr_CAN)
      /* View student's records: common record card and course record card */
      Lay_PutContextualLinkOnlyIcon (ActSeeRecOneStd,NULL,
                                     Usr_PutParOtherUsrCodEncrypted,EncryptedUsrCod,
			             "address-card.svg",Ico_BLACK);
   else if (Usr_CheckIfICanViewRecordTch (&Gbl.Usrs.Other.UsrDat) == Usr_CAN)
      Lay_PutContextualLinkOnlyIcon (ActSeeRecOneTch,NULL,
			             Usr_PutParOtherUsrCodEncrypted,EncryptedUsrCod,
			             "address-card.svg",Ico_BLACK);
  }

/*****************************************************************************/
/******************************* Show one event ******************************/
/*****************************************************************************/

static void Agd_ShowOneEvent (struct Agd_Agenda *Agenda,
                              Agd_AgendaType_t AgendaType,long AgdCod)
  {
   extern const char *Dat_TimeStatusClass[Dat_NUM_TIME_STATUS][HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   struct Agd_Event AgdEvent;
   Dat_StartEndTime_t StartEndTime;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get data of this event *****/
   AgdEvent.AgdCod = AgdCod;
   switch (AgendaType)
     {
      case Agd_MY_AGENDA_TODAY:
      case Agd_MY_AGENDA:
	 AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
         break;
      case Agd_ANOTHER_AGENDA_TODAY:
      case Agd_ANOTHER_AGENDA:
	 AgdEvent.UsrCod = Gbl.Usrs.Other.UsrDat.UsrCod;
         break;
     }
   Agd_GetventDataByCod (&AgdEvent);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (AgdEvent.AgdCod,&Anchor);

   /***** Write first row of data of this event *****/
   HTM_TR_Begin (NULL);

      /* Start/end date/time */
      UniqueId++;
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 if (asprintf (&Id,"agd_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
		       Id,
		       Dat_TimeStatusClass[AgdEvent.TimeStatus][AgdEvent.HiddenOrVisible],
		       The_GetSuffix (),
		       The_GetColorRows ());
	    Dat_WriteLocalDateHMSFromUTC (Id,AgdEvent.TimeUTC[StartEndTime],
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					  Dat_WRITE_TODAY |
					  Dat_WRITE_DATE_ON_SAME_DAY |
					  Dat_WRITE_WEEK_DAY |
					  Dat_WRITE_HOUR |
					  Dat_WRITE_MINUTE);
	 HTM_TD_End ();
	 free (Id);
	}

      /* Event */
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	 HTM_ARTICLE_Begin (Anchor);
	    HTM_SPAN_Begin ("class=\"%s_%s\"",
			    HidVis_TitleClass[AgdEvent.HiddenOrVisible],
			    The_GetSuffix ());
	       HTM_Txt (AgdEvent.Title);
	    HTM_SPAN_End ();
	 HTM_ARTICLE_End ();
      HTM_TD_End ();

      /* Location */
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	 HTM_SPAN_Begin ("class=\"%s_%s\"",
			 HidVis_TitleClass[AgdEvent.HiddenOrVisible],
		         The_GetSuffix ());
	    HTM_Txt (AgdEvent.Location);
	 HTM_SPAN_End ();
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Write second row of data of this event *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
	 switch (AgendaType)
	   {
	    case Agd_MY_AGENDA_TODAY:
	    case Agd_MY_AGENDA:
	       /* Forms to remove/edit this event */
	       Agd_PutFormsToRemEditOneEvent (Agenda,&AgdEvent,Anchor);
	       break;
	    default:
	       break;
	   }
      HTM_TD_End ();

      /* Text of the event */
      HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
	 HTM_DIV_Begin ("class=\"PAR %s_%s\"",
	                HidVis_DataClass[AgdEvent.HiddenOrVisible],
	                The_GetSuffix ());
	    Agd_DB_GetEventTxt (&AgdEvent,Txt);
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      Txt,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
	    ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
	    HTM_Txt (Txt);
	 HTM_DIV_End ();
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/******************* Put a link (form) to edit one event *********************/
/*****************************************************************************/

static void Agd_PutFormsToRemEditOneEvent (struct Agd_Agenda *Agenda,
                                           struct Agd_Event *AgdEvent,
                                           const char *Anchor)
  {
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhEvtMyAgd,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidEvtMyAgd,	// Visible ==> action to hide
     };

   Agenda->AgdCodToEdit = AgdEvent->AgdCod;	// Used as parameter in contextual links

   /***** Icon to remove event *****/
   Ico_PutContextualIconToRemove (ActReqRemEvtMyAgd,NULL,
                                  Agd_PutCurrentParsMyAgenda,Agenda);

   /***** Icon to hide/unhide event *****/
   Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
				      Agd_PutCurrentParsMyAgenda,Agenda,
				      AgdEvent->HiddenOrVisible);

   /***** Icon to edit event *****/
   Ico_PutContextualIconToEdit (ActEdiOneEvtMyAgd,NULL,
                                Agd_PutCurrentParsMyAgenda,Agenda);

   /***** Icon to make event public/private *****/
   if (AgdEvent->Public)
      Lay_PutContextualLinkOnlyIcon (ActPrvEvtMyAgd,NULL,
				     Agd_PutCurrentParsMyAgenda,Agenda,
			             "unlock.svg",Ico_GREEN);
   else
      Lay_PutContextualLinkOnlyIcon (ActPubEvtMyAgd,NULL,
	                             Agd_PutCurrentParsMyAgenda,Agenda,
			             "lock.svg",Ico_RED);
  }

/*****************************************************************************/
/****************** Parameters passed in my agenda forms *********************/
/*****************************************************************************/

static void Agd_PutCurrentParsMyAgenda (void *Agenda)
  {
   if (Agenda)
      Agd_PutParsMyAgenda (((struct Agd_Agenda *) Agenda)->Past__FutureEvents,
			   ((struct Agd_Agenda *) Agenda)->PrivatPublicEvents,
			   ((struct Agd_Agenda *) Agenda)->HiddenVisiblEvents,
			   ((struct Agd_Agenda *) Agenda)->SelectedOrder,
			   ((struct Agd_Agenda *) Agenda)->CurrentPage,
			   ((struct Agd_Agenda *) Agenda)->AgdCodToEdit);
  }

/* The following function is called
   when one or more parameters must be passed explicitely.
   Each parameter is passed only if its value is distinct to default. */

void Agd_PutParsMyAgenda (unsigned Past__FutureEvents,
                            unsigned PrivatPublicEvents,
                            unsigned HiddenVisiblEvents,
			    Dat_StartEndTime_t Order,
                            unsigned NumPage,
                            long AgdCodToEdit)
  {
   if (Past__FutureEvents != (Agd_DEFAULT_PAST___EVENTS |
	                      Agd_DEFAULT_FUTURE_EVENTS))
      Agd_PutParPast__FutureEvents (Past__FutureEvents);

   if (PrivatPublicEvents != (Agd_DEFAULT_PRIVAT_EVENTS |
	                      Agd_DEFAULT_PUBLIC_EVENTS))
      Agd_PutParPrivatPublicEvents (PrivatPublicEvents);

   if (HiddenVisiblEvents != (Agd_DEFAULT_HIDDEN_EVENTS |
	                      Agd_DEFAULT_VISIBL_EVENTS))
      Agd_PutParHiddenVisiblEvents (HiddenVisiblEvents);

   if (Order != Agd_ORDER_DEFAULT)
      Par_PutParOrder ((unsigned) Order);

   if (NumPage > 1)
      Pag_PutParPagNum (Pag_MY_AGENDA,NumPage);

   ParCod_PutPar (ParCod_Agd,AgdCodToEdit);
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of events ***********/
/*****************************************************************************/

static void Agd_GetPars (struct Agd_Agenda *Agenda,
                           Agd_AgendaType_t AgendaType)
  {
   static const Pag_WhatPaginate_t WhatPaginate[Agd_NUM_AGENDA_TYPES] =
     {
      [Agd_MY_AGENDA_TODAY     ] = Pag_MY_AGENDA,	// not used
      [Agd_MY_AGENDA           ] = Pag_MY_AGENDA,
      [Agd_ANOTHER_AGENDA_TODAY] = Pag_ANOTHER_AGENDA,	// not used
      [Agd_ANOTHER_AGENDA      ] = Pag_ANOTHER_AGENDA,
     };

   if (AgendaType == Agd_MY_AGENDA)
     {
      Agenda->Past__FutureEvents = Agd_GetParsPast__FutureEvents ();
      Agenda->PrivatPublicEvents = Agd_GetParsPrivatPublicEvents ();
      Agenda->HiddenVisiblEvents = Agd_GetParsHiddenVisiblEvents ();
     }
   Agd_GetParEventOrder (Agenda);
   Agenda->CurrentPage = Pag_GetParPagNum (WhatPaginate[AgendaType]);
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of events ******/
/*****************************************************************************/

void Agd_PutParEventsOrder (Dat_StartEndTime_t SelectedOrder)
  {
   if (SelectedOrder != Agd_ORDER_DEFAULT)
      Par_PutParOrder ((unsigned) SelectedOrder);
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of events ***********/
/*****************************************************************************/

static void Agd_GetParEventOrder (struct Agd_Agenda *Agenda)
  {
   static bool AlreadyGot = false;

   if (!AlreadyGot)
     {
      Agenda->SelectedOrder = (Dat_StartEndTime_t)
			      Par_GetParUnsignedLong ("Order",
						      0,
						      Dat_NUM_START_END_TIME - 1,
						      (unsigned long) Agd_ORDER_DEFAULT);
      AlreadyGot = true;
     }
  }

/*****************************************************************************/
/************************* Get list of agenda events *************************/
/*****************************************************************************/

#define Agd_MAX_BYTES_SUBQUERY 128

static void Agd_GetListEvents (struct Agd_Agenda *Agenda,
                               Agd_AgendaType_t AgendaType)
  {
   MYSQL_RES *mysql_res;
   unsigned NumEvent;

   /***** Initialize list of events *****/
   Agd_FreeListEvents (Agenda);

   /***** Trivial check: anything to get from database? *****/
   switch (AgendaType)
     {
      case Agd_MY_AGENDA_TODAY:
      case Agd_MY_AGENDA:
	 if (Agenda->Past__FutureEvents == 0 ||
             Agenda->PrivatPublicEvents == 0 ||
             Agenda->HiddenVisiblEvents == 0)	// All selectors are off
	   {
	    // Nothing to get from database
	    Agenda->LstIsRead = true;
	    return;
	   }
	 break;
      default:
	 break;
     }

   /***** Get list of events from database *****/
   if ((Agenda->Num = Agd_DB_GetListEvents (&mysql_res,Agenda,AgendaType))) // Events found...
     {
      /***** Create list of events *****/
      if ((Agenda->LstAgdCods = calloc ((size_t) Agenda->Num,
					sizeof (*Agenda->LstAgdCods))) == NULL)
	 Err_NotEnoughMemoryExit ();

      /***** Get the events codes *****/
      for (NumEvent = 0;
	   NumEvent < Agenda->Num;
	   NumEvent++)
	 /* Get next event code */
	 if ((Agenda->LstAgdCods[NumEvent] = DB_GetNextCode (mysql_res)) < 0)
	    Err_WrongEventExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Agenda->LstIsRead = true;
  }

/*****************************************************************************/
/*********************** Get event data using its code ***********************/
/*****************************************************************************/

static void Agd_GetventDataByCod (struct Agd_Event *AgdEvent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of event from database *****/
   if (Agd_DB_GetEventDataByCod (&mysql_res,AgdEvent))	// Event found...
     {
      /* Get row:
      row[0] AgdCod
      row[1] Public
      row[2] Hidden
      row[3] UNIX_TIMESTAMP(StartTime)
      row[4] UNIX_TIMESTAMP(EndTime)
      row[5] NOW()>EndTime	// Past event?
      row[6] NOW()<StartTime	// Future event?
      row[7] Event
      row[8] Location
      */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the event (row[0]) */
      AgdEvent->AgdCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get whether the event is public or not (row[1])
         and whether it is hidden or not (row[2])  */
      AgdEvent->Public = (row[1][0] == 'Y');
      AgdEvent->HiddenOrVisible = HidVid_GetHiddenOrVisible (row[2][0]);

      /* Get start date (row[3]) and end date (row[4]) in UTC time */
      AgdEvent->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[3]);
      AgdEvent->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get whether the event is past, present or future (row(5), row[6]) */
      AgdEvent->TimeStatus = ((row[5][0] == '1') ? Dat_PAST :
	                     ((row[6][0] == '1') ? Dat_FUTURE :
	                	                   Dat_PRESENT));

      /* Get the event (row[7]) and its location (row[8]) */
      Str_Copy (AgdEvent->Title   ,row[7],sizeof (AgdEvent->Title   ) - 1);
      Str_Copy (AgdEvent->Location,row[8],sizeof (AgdEvent->Location) - 1);
     }
   else
     {
      /***** Clear all event data *****/
      AgdEvent->AgdCod                = -1L;
      AgdEvent->Public                = false;
      AgdEvent->HiddenOrVisible       = HidVis_VISIBLE;
      AgdEvent->TimeUTC[Dat_STR_TIME] =
      AgdEvent->TimeUTC[Dat_END_TIME] = (time_t) 0;
      AgdEvent->TimeStatus            = Dat_FUTURE;
      AgdEvent->Title[0]              = '\0';
      AgdEvent->Location[0]           = '\0';
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Free list of events *****************************/
/*****************************************************************************/

static void Agd_FreeListEvents (struct Agd_Agenda *Agenda)
  {
   if (Agenda->LstIsRead && Agenda->LstAgdCods)
     {
      /***** Free memory used by the list of events *****/
      free (Agenda->LstAgdCods);
      Agenda->LstAgdCods = NULL;
      Agenda->Num = 0;
      Agenda->LstIsRead = false;
     }
  }

/*****************************************************************************/
/************** Ask for confirmation of removing of an event *****************/
/*****************************************************************************/

void Agd_AskRemEvent (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_event_X;
   struct Agd_Agenda Agenda;
   struct Agd_Event AgdEvent;

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Get event code *****/
   AgdEvent.AgdCod = ParCod_GetAndCheckPar (ParCod_Agd);

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetventDataByCod (&AgdEvent);

   /***** Show question and button to remove event *****/
   Agenda.AgdCodToEdit = AgdEvent.AgdCod;
   Ale_ShowAlertRemove (ActRemEvtMyAgd,NULL,
                        Agd_PutCurrentParsMyAgenda,&Agenda,
			Txt_Do_you_really_want_to_remove_the_event_X,
	                AgdEvent.Title);

   /***** Show events again *****/
   Agd_ShowMyAgenda (&Agenda);
  }

/*****************************************************************************/
/****************************** Remove an event ******************************/
/*****************************************************************************/

void Agd_RemoveEvent (void)
  {
   extern const char *Txt_Event_X_removed;
   struct Agd_Agenda Agenda;
   struct Agd_Event AgdEvent;

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Get event code *****/
   AgdEvent.AgdCod = ParCod_GetAndCheckPar (ParCod_Agd);

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetventDataByCod (&AgdEvent);

   /***** Remove event *****/
   Agd_DB_RemoveEvent (&AgdEvent);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Event_X_removed,
	          AgdEvent.Title);

   /***** Show events again *****/
   Agd_ShowMyAgenda (&Agenda);
  }

/*****************************************************************************/
/**************************** Hide/unhide event ******************************/
/*****************************************************************************/

void Agd_HideEvent (void)
  {
   Agd_HideUnhideEvent (HidVis_HIDDEN);
  }

void Agd_UnhideEvent (void)
  {
   Agd_HideUnhideEvent (HidVis_VISIBLE);
  }

static void Agd_HideUnhideEvent (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Agd_Agenda Agenda;
   struct Agd_Event AgdEvent;

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Get event code *****/
   AgdEvent.AgdCod = ParCod_GetAndCheckPar (ParCod_Agd);

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetventDataByCod (&AgdEvent);

   /***** Unhide event *****/
   Agd_DB_HideOrUnhideEvent (AgdEvent.AgdCod,AgdEvent.UsrCod,HiddenOrVisible);

   /***** Show events again *****/
   Agd_ShowMyAgenda (&Agenda);
  }

/*****************************************************************************/
/****************************** Make event private ***************************/
/*****************************************************************************/

void Agd_MakeEventPrivate (void)
  {
   extern const char *Txt_Event_X_is_now_private;
   struct Agd_Agenda Agenda;
   struct Agd_Event AgdEvent;

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Get event code *****/
   AgdEvent.AgdCod = ParCod_GetAndCheckPar (ParCod_Agd);

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetventDataByCod (&AgdEvent);

   /***** Make event private *****/
   Agd_DB_MakeEventPrivate (&AgdEvent);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Event_X_is_now_private,
	          AgdEvent.Title);

   /***** Show events again *****/
   Agd_ShowMyAgenda (&Agenda);
  }

/*****************************************************************************/
/******** Make event public (make it visible to users of my courses) *********/
/*****************************************************************************/

void Agd_MakeEventPublic (void)
  {
   extern const char *Txt_Event_X_is_now_visible_to_users_of_your_courses;
   struct Agd_Agenda Agenda;
   struct Agd_Event AgdEvent;

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Get event code *****/
   AgdEvent.AgdCod = ParCod_GetAndCheckPar (ParCod_Agd);

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetventDataByCod (&AgdEvent);

   /***** Make event public *****/
   Agd_DB_MakeEventPublic (&AgdEvent);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Event_X_is_now_visible_to_users_of_your_courses,
                  AgdEvent.Title);

   /***** Show events again *****/
   Agd_ShowMyAgenda (&Agenda);
  }

/*****************************************************************************/
/******************** Put a form to create a new event ***********************/
/*****************************************************************************/

void Agd_ReqCreatOrEditEvent (void)
  {
   extern const char *Hlp_PROFILE_Agenda_edit_event;
   extern const char *Txt_Event;
   extern const char *Txt_Location;
   extern const char *Txt_Description;
   extern const char *Txt_Create;
   extern const char *Txt_Save_changes;
   struct Agd_Agenda Agenda;
   struct Agd_Event AgdEvent;
   bool ItsANewEvent;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Get the code of the event *****/
   ItsANewEvent = ((AgdEvent.AgdCod = ParCod_GetPar (ParCod_Agd)) <= 0);

   /***** Get from the database the data of the event *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   if (ItsANewEvent)
     {
      /* Initialize to empty event */
      AgdEvent.AgdCod = -1L;
      AgdEvent.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
      AgdEvent.TimeUTC[Dat_END_TIME] = AgdEvent.TimeUTC[Dat_STR_TIME] + (2 * 60 * 60);	// +2 hours
      AgdEvent.TimeStatus = Dat_FUTURE;
      AgdEvent.Title[0]    = '\0';
      AgdEvent.Location[0] = '\0';
     }
   else
     {
      /* Get data of the event from database */
      Agd_GetventDataByCod (&AgdEvent);

      /* Get text of the event from database */
      Agd_DB_GetEventTxt (&AgdEvent,Txt);
     }

   /***** Begin form *****/
   if (ItsANewEvent)
     {
      Frm_BeginForm (ActNewEvtMyAgd);
      Agenda.AgdCodToEdit = -1L;
     }
   else
     {
      Frm_BeginForm (ActChgEvtMyAgd);
      Agenda.AgdCodToEdit = AgdEvent.AgdCod;
     }
   Agd_PutCurrentParsMyAgenda (&Agenda);

      /***** Begin box and table *****/
      Box_BoxTableBegin (AgdEvent.Title[0] ? AgdEvent.Title :
					     Txt_Event,
			 NULL,NULL,
			 Hlp_PROFILE_Agenda_edit_event,Box_NOT_CLOSABLE,2);

	 /***** Event *****/
	 /* Begin table row */
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Event",Txt_Event);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_INPUT_TEXT ("Event",Agd_MAX_CHARS_EVENT,AgdEvent.Title,
			       HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Event\" class=\"Frm_C2_INPUT INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 /* End table row */
	 HTM_TR_End ();

	 /***** Location *****/
	 /* Begin table row */
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Location",Txt_Location);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_INPUT_TEXT ("Location",Agd_MAX_CHARS_LOCATION,AgdEvent.Location,
			       HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Location\" class=\"Frm_C2_INPUT INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 /* End table row */
	 HTM_TR_End ();

	 /***** Start and end dates *****/
	 Dat_PutFormStartEndClientLocalDateTimes (AgdEvent.TimeUTC,
						  Dat_FORM_SECONDS_OFF,
						  SetHMS);

	 /***** Text *****/
	 /* Begin table row */
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Txt",Txt_Description);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_TEXTAREA_Begin (HTM_ENABLED,
				   "id=\"Txt\" name=\"Txt\" rows=\"5\""
				   " class=\"Frm_C2_INPUT INPUT_%s\"",
				   The_GetSuffix ());
		  if (!ItsANewEvent)
		     HTM_Txt (Txt);
	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();

	 /* End table row */
	 HTM_TR_End ();

      /***** End table, send button and end box *****/
      if (ItsANewEvent)
	 Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create);
      else
	 Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show current events, if any *****/
   Agd_ShowMyAgenda (&Agenda);
  }

/*****************************************************************************/
/********************* Receive form to create a new event ********************/
/*****************************************************************************/

void Agd_ReceiveEvent (void)
  {
   extern const char *Txt_Created_new_event_X;
   extern const char *Txt_The_event_has_been_modified;
   struct Agd_Agenda Agenda;
   struct Agd_Event AgdEvent;
   bool ItsANewEvent;
   bool NewEventIsCorrect = true;
   char EventTxt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset agenda context *****/
   Agd_ResetAgenda (&Agenda);

   /***** Get parameters *****/
   Agd_GetPars (&Agenda,Agd_MY_AGENDA);

   /***** Set author of the event *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;

   /***** Get the code of the event *****/
   ItsANewEvent = ((AgdEvent.AgdCod = ParCod_GetPar (ParCod_Agd)) <= 0);

   /***** Get start/end date-times *****/
   AgdEvent.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   AgdEvent.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /***** Get event location *****/
   Par_GetParText ("Location",AgdEvent.Location,Agd_MAX_BYTES_LOCATION);

   /***** Get event title *****/
   Par_GetParText ("Event",AgdEvent.Title,Agd_MAX_BYTES_EVENT);

   /***** Get event description *****/
   Par_GetParHTML ("Txt",EventTxt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (AgdEvent.TimeUTC[Dat_STR_TIME] == 0)
      AgdEvent.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   if (AgdEvent.TimeUTC[Dat_END_TIME] == 0)
      AgdEvent.TimeUTC[Dat_END_TIME] = AgdEvent.TimeUTC[Dat_STR_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Check if event is correct *****/
   if (!AgdEvent.Location[0])	// If there is no event
     {
      NewEventIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   /***** Check if event is correct *****/
   if (!AgdEvent.Title[0])	// If there is no event
     {
      NewEventIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   /***** Create a new event or update an existing one *****/
   if (NewEventIsCorrect)
     {
      if (ItsANewEvent)
	{
         AgdEvent.AgdCod = Agd_DB_CreateEvent (&AgdEvent,EventTxt);	// Add new event to database

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_event_X,
		        AgdEvent.Title);
	}
      else
        {
	 Agd_DB_UpdateEvent (&AgdEvent,EventTxt);

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_event_has_been_modified);
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Show events again *****/
      Agd_ShowMyAgenda (&Agenda);
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Agd_ReqCreatOrEditEvent ();
  }

/*****************************************************************************/
/************************** Show an agenda QR code ***************************/
/*****************************************************************************/

void Agd_PrintAgdQRCode (void)
  {
   extern const char *Txt_Where_s_USER;
   char *Title;

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Where_s_USER,Gbl.Usrs.Me.UsrDat.FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE);
   free (Title);

      /***** Print QR code ****/
      QR_PrintQRCode ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Get and show number of users who have chosen a language ***********/
/*****************************************************************************/

void Agd_GetAndShowAgendasStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_agendas;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_events;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_Number_of_events_per_user;
   unsigned NumEvents;
   unsigned NumUsrs;
   unsigned NumUsrsTotal;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_AGENDAS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_agendas,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_events         ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_users          ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users         ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_events_per_user,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Number of agenda events *****/
      NumEvents = Agd_DB_GetNumEvents (Gbl.Scope.Current);

      /***** Number of users with agenda events *****/
      NumUsrs = Agd_DB_GetNumUsrsWithEvents (Gbl.Scope.Current);

      /***** Get total number of users in current scope *****/
      NumUsrsTotal = Usr_GetTotalNumberOfUsers ();

      /***** Write number of users who have chosen each language *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Unsigned (NumEvents);
	 HTM_TD_Unsigned (NumUsrs);
	 HTM_TD_Percentage (NumUsrs,NumUsrsTotal);
	 HTM_TD_Ratio (NumEvents,NumUsrs);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
