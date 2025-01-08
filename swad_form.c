// swad_form.c: forms to go to actions

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static bool Frm_Inside = false;

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static inline void Frm_SetInside (bool Inside);

static void Frm_BeginFormInternal (Act_Action_t NextAction,bool PutParLocationIfNoSesion,
                                   const char *Id,const char *Anchor,const char *OnSubmit);

/*****************************************************************************/
/************** Set to true inside a form to avoid nested forms **************/
/*****************************************************************************/

static inline void Frm_SetInside (bool Inside)
  {
   Frm_Inside = Inside;
  }

bool Frm_CheckIfInside (void)
  {
   return Frm_Inside;
  }

/*****************************************************************************/
/******************************** Begin a form *******************************/
/*****************************************************************************/

void Frm_BeginFormGoTo (Act_Action_t NextAction)
  {
   Frm_BeginFormInternal (NextAction,false,NULL,NULL,NULL);	// Do not put now parameter location
  }

void Frm_BeginForm (Act_Action_t NextAction)
  {
   Frm_BeginFormInternal (NextAction,true,NULL,NULL,NULL);	// Do put now parameter location (if no open session)
  }

void Frm_BeginFormAnchor (Act_Action_t NextAction,const char *Anchor)
  {
   Frm_BeginFormInternal (NextAction,true,NULL,Anchor,NULL);	// Do put now parameter location (if no open session)
  }

void Frm_BeginFormOnSubmit (Act_Action_t NextAction,const char *OnSubmit)
  {
   Frm_BeginFormInternal (NextAction,true,NULL,NULL,OnSubmit);	// Do put now parameter location (if no open session)
  }

void Frm_BeginFormAnchorOnSubmit (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit)
  {
   Frm_BeginFormInternal (NextAction,true,NULL,Anchor,OnSubmit);// Do put now parameter location (if no open session)
  }

void Frm_BeginFormId (Act_Action_t NextAction,const char *Id)
  {
   Frm_BeginFormInternal (NextAction,true,Id,NULL,NULL);	// Do put now parameter location (if no open session)
  }

static void Frm_BeginFormInternal (Act_Action_t NextAction,bool PutParLocationIfNoSesion,
                                   const char *Id,const char *Anchor,const char *OnSubmit)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char ParsStr[Frm_MAX_BYTES_PARAMS_STR + 1];

   if (!Frm_CheckIfInside ())
     {
      /* Begin form */
      HTM_TxtF ("<form method=\"post\" action=\"%s/%s",
	        Cfg_URL_SWAD_CGI,
	        Lan_STR_LANG_ID[Gbl.Prefs.Language]);
      if (Anchor)
	 if (Anchor[0])
            HTM_TxtF ("#%s",Anchor);
      if (Id)
	{
	 if (Id[0])
            HTM_TxtF ("\" id=\"%s\"",Id);
	 else
            HTM_Txt ("\"");
	}
      else
         HTM_Txt ("\"");
      if (OnSubmit)
         if (OnSubmit[0])
            HTM_TxtF (" onsubmit=\"%s\"",OnSubmit);
      switch (Act_GetBrowserTab (NextAction))
	{
	 case Act_NEW:
	 case Act_DWN:
	    HTM_Txt (" target=\"_blank\"");
	    break;
	 default:
	    break;
	}
      if (Act_GetContentType (NextAction) == Act_DATA)
	 HTM_Txt (" enctype=\"multipart/form-data\"");
      HTM_Txt (" accept-charset=\"windows-1252\">");

      /* Put basic form parameters */
      Frm_SetParsForm (ParsStr,NextAction,PutParLocationIfNoSesion);
      HTM_Txt (ParsStr);

      Frm_SetInside (true);
     }
  }

/*
   Form without action are used in exams.
   The accept-charset attribute specifies the character encodings that are to be used for the form submission
   But this type of form is sent via AJAX ==>
   ==> we use the value property of input fields to build the parameters sent using XMLHttp.send ==>
   ==> the value property is always codified in UTF-8 ==> accept-charset is irrelevant
*/
void Frm_BeginFormNoAction (void)
  {
   if (!Frm_CheckIfInside ())
     {
      /* Begin form */
      HTM_Txt ("<form accept-charset=\"UTF-8\""
	       " onsubmit=\"return false;\">");	// Form that can not be submitted, to avoid enter key to send it

      Frm_SetInside (true);
     }
  }

void Frm_SetParsForm (char ParsStr[Frm_MAX_BYTES_PARAMS_STR + 1],
		      Act_Action_t NextAction,
                      bool PutParLocationIfNoSession)
  {
   static const char *ParName[Hie_NUM_LEVELS] =
     {
      [Hie_CTY] = "cty",
      [Hie_INS] = "ins",
      [Hie_CTR] = "ctr",
      [Hie_DEG] = "deg",
      [Hie_CRS] = "crs",
     };
   char ParAction[Frm_MAX_BYTES_PARAM_ACTION + 1];
   char ParSession[Frm_MAX_BYTES_PARAM_SESSION + 1];
   char ParLocation[Frm_MAX_BYTES_PARAM_LOCATION + 1];

   ParAction[0] = '\0';
   ParSession[0] = '\0';
   ParLocation[0] = '\0';

   if (NextAction != ActUnk)
     {
      snprintf (ParAction,sizeof (ParAction),
	        "<input type=\"hidden\" name=\"act\" value=\"%ld\" />",
	        Act_GetActCod (NextAction));

      if (Gbl.Session.Id[0])
	 snprintf (ParSession,sizeof (ParSession),
		   "<input type=\"hidden\" name=\"ses\" value=\"%s\" />",
		   Gbl.Session.Id);
      else if (PutParLocationIfNoSession &&
	       Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].HieCod > 0)
	 // Extra parameters necessary when there's no open session
	 /* If session is open, course/degree/... code will be get from session data,
	    but if there is not an open session, and next action is known,
	    it is necessary to send a parameter with course/degree/... code */
	 snprintf (ParLocation,sizeof (ParLocation),
		   "<input type=\"hidden\" name=\"%s\" value=\"%ld\" />",
		   ParName[Gbl.Hierarchy.Level],
		   Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].HieCod);
     }

   snprintf (ParsStr,Frm_MAX_BYTES_PARAMS_STR + 1,"%s%s%s",
	     ParAction,ParSession,ParLocation);
  }

void Frm_EndForm (void)
  {
   if (Frm_CheckIfInside ())
     {
      HTM_Txt ("</form>");
      Frm_SetInside (false);
     }
  }

/*****************************************************************************/
/********************** Put a form and a table to create *********************/
/*****************************************************************************/

void Frm_BeginFormTable (Act_Action_t NextAction,const char *Anchor,
                         void (*FuncPars) (void *Args),void *Args,
                         const char *ClassTable)
  {
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];

   /***** Begin fieldset *****/
   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (Txt_Actions[NextAction]);

      /***** Begin form *****/
      Frm_BeginFormAnchor (NextAction,Anchor);
         if (FuncPars)
            FuncPars (Args);

   	 /***** Begin table *****/
         HTM_TABLE_Begin (ClassTable);
  }

void Frm_EndFormTable (Btn_Button_t Button)
  {
   extern const char *Txt_Create;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Remove;
   static const char **TxtButton[Btn_NUM_BUTTON_TYPES] =
     {
      [Btn_CREATE_BUTTON ] = &Txt_Create,
      [Btn_CONFIRM_BUTTON] = &Txt_Save_changes,
      [Btn_REMOVE_BUTTON ] = &Txt_Remove,
     };

         /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Send button *****/
         if (Button != Btn_NO_BUTTON)
            Btn_PutButton (Button,*TxtButton[Button]);

      /***** End form *****/
      Frm_EndForm ();

   /***** End fieldset *****/
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/***************************** Get unique Id *********************************/
/*****************************************************************************/

void Frm_SetUniqueId (char UniqueId[Frm_MAX_BYTES_ID + 1])
  {
   static unsigned CountForThisExecution = 0;

   /***** Create Id. The id must be unique,
          the page content may be updated via AJAX.
          So, Id uses:
          - a name for this execution
          - a number for each element in this execution *****/
   snprintf (UniqueId,Frm_MAX_BYTES_ID + 1,"id_%s_%u",
             Cry_GetUniqueNameEncrypted (),
             ++CountForThisExecution);
  }

/*****************************************************************************/
/****************** Build/free anchor string given a code ********************/
/*****************************************************************************/

void Frm_SetAnchorStr (long Cod,char **Anchor)
  {
   if (Cod > 0)
     {
      if (asprintf (Anchor,"cod_%ld",
		    Cod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
      *Anchor = NULL;
  }

void Frm_FreeAnchorStr (char **Anchor)
  {
   if (*Anchor)
     {
      free (*Anchor);
      *Anchor = NULL;
     }
  }

/*****************************************************************************/
/************************* Show label column in form *************************/
/*****************************************************************************/

// Id    == NULL ==> label class = data
// Id[0] == '\0' ==> label class = form, no label for
// Id[0] != '\0' ==> label class = form, label for

void Frm_LabelColumn (const char *TDClass,const char *Id,const char *Label)
  {
   /***** Column/cell begin *****/
   if (TDClass)
      HTM_TD_Begin ("class=\"%s\"",TDClass);
   else
      HTM_TD_Begin (NULL);

   /***** Label *****/
   if (Id)
     {
      if (Id[0])
	 HTM_LABEL_Begin ("for=\"%s\" class=\"FORM_IN_%s\"",Id,The_GetSuffix ());
      else
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
     }
   else
      HTM_LABEL_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
   HTM_TxtColon (Label);
   HTM_LABEL_End ();

   /***** Column/cell end *****/
   HTM_TD_End ();
  }
