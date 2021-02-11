// swad_form.c: forms to go to actions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Frm_StartFormInternal (Act_Action_t NextAction,bool PutParameterLocationIfNoSesion,
                                   const char *Id,const char *Anchor,const char *OnSubmit);

/*****************************************************************************/
/******************************** Start a form *******************************/
/*****************************************************************************/

void Frm_StartFormGoTo (Act_Action_t NextAction)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   snprintf (Gbl.Form.Id,sizeof (Gbl.Form.Id),
	     "form_%d",
	     Gbl.Form.Num);
   Frm_StartFormInternal (NextAction,false,Gbl.Form.Id,NULL,NULL);	// Do not put now parameter location
  }

void Frm_StartForm (Act_Action_t NextAction)
  {
   Frm_StartFormAnchorOnSubmit (NextAction,NULL,NULL);
  }

void Frm_StartFormAnchor (Act_Action_t NextAction,const char *Anchor)
  {
   Frm_StartFormAnchorOnSubmit (NextAction,Anchor,NULL);
  }

void Frm_StartFormOnSubmit (Act_Action_t NextAction,const char *OnSubmit)
  {
   Frm_StartFormAnchorOnSubmit (NextAction,NULL,OnSubmit);
  }

void Frm_StartFormAnchorOnSubmit (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   snprintf (Gbl.Form.Id,sizeof (Gbl.Form.Id),
	     "form_%d",
	     Gbl.Form.Num);
   Frm_StartFormInternal (NextAction,true,Gbl.Form.Id,Anchor,OnSubmit);	// Do put now parameter location (if no open session)
  }

void Frm_StartFormUnique (Act_Action_t NextAction)
  {
   Frm_StartFormUniqueAnchor (NextAction,NULL);
  }

void Frm_StartFormUniqueAnchor (Act_Action_t NextAction,const char *Anchor)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   snprintf (Gbl.Form.UniqueId,sizeof (Gbl.Form.UniqueId),
	     "form_%s_%d",
             Gbl.UniqueNameEncrypted,Gbl.Form.Num);
   Frm_StartFormInternal (NextAction,true,Gbl.Form.UniqueId,Anchor,NULL);	// Do put now parameter location (if no open session)
  }

void Frm_StartFormUniqueAnchorOnSubmit (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   snprintf (Gbl.Form.UniqueId,sizeof (Gbl.Form.UniqueId),
	     "form_%s_%d",
             Gbl.UniqueNameEncrypted,Gbl.Form.Num);
   Frm_StartFormInternal (NextAction,true,Gbl.Form.UniqueId,Anchor,OnSubmit);	// Do put now parameter location (if no open session)
  }

void Frm_StartFormId (Act_Action_t NextAction,const char *Id)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   Frm_StartFormInternal (NextAction,true,Id,NULL,NULL);	// Do put now parameter location (if no open session)
  }

// Id can not be NULL
static void Frm_StartFormInternal (Act_Action_t NextAction,bool PutParameterLocationIfNoSesion,
                                   const char *Id,const char *Anchor,const char *OnSubmit)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char ParamsStr[Frm_MAX_BYTES_PARAMS_STR + 1];

   if (!Gbl.Form.Inside)
     {
      /* Begin form */
      HTM_TxtF ("<form method=\"post\" action=\"%s/%s",
	        Cfg_URL_SWAD_CGI,
	        Lan_STR_LANG_ID[Gbl.Prefs.Language]);
      if (Anchor)
	 if (Anchor[0])
            HTM_TxtF ("#%s",Anchor);
      HTM_TxtF ("\" id=\"%s\"",Id);
      if (OnSubmit)
         if (OnSubmit[0])
            HTM_TxtF (" onsubmit=\"%s\"",OnSubmit);
      switch (Act_GetBrowserTab (NextAction))
	{
	 case Act_BRW_NEW_TAB:
	 case Act_DOWNLD_FILE:
	    HTM_Txt (" target=\"_blank\"");
	    break;
	 default:
	    break;
	}
      if (Act_GetContentType (NextAction) == Act_CONT_DATA)
	 HTM_Txt (" enctype=\"multipart/form-data\"");
      HTM_Txt (" accept-charset=\"windows-1252\">");

      /* Put basic form parameters */
      Frm_SetParamsForm (ParamsStr,NextAction,PutParameterLocationIfNoSesion);
      HTM_Txt (ParamsStr);

      Gbl.Form.Inside = true;
     }
  }

/*
   Form without action are used in exams.
   The accept-charset attribute specifies the character encodings that are to be used for the form submission
   But this type of form is sent via AJAX ==>
   ==> we use the value property of input fields to build the parameters sent using XMLHttp.send ==>
   ==> the value property is always codified in UTF-8 ==> accept-charset is irrelevant
*/
void Frm_StartFormNoAction (void)
  {
   if (!Gbl.Form.Inside)
     {
      /* Begin form */
      HTM_Txt ("<form accept-charset=\"UTF-8\""
	       " onsubmit=\"return false;\">");	// Form that can not be submitted, to avoid enter key to send it

      Gbl.Form.Inside = true;
     }
  }

void Frm_SetParamsForm (char ParamsStr[Frm_MAX_BYTES_PARAMS_STR + 1],Act_Action_t NextAction,
                        bool PutParameterLocationIfNoSesion)
  {
   char ParamAction[Frm_MAX_BYTES_PARAM_ACTION + 1];
   char ParamSession[Frm_MAX_BYTES_PARAM_SESSION + 1];
   char ParamLocation[Frm_MAX_BYTES_PARAM_LOCATION + 1];

   ParamAction[0] = '\0';
   ParamSession[0] = '\0';
   ParamLocation[0] = '\0';

   if (NextAction != ActUnk)
     {
      snprintf (ParamAction,sizeof (ParamAction),
	        "<input type=\"hidden\" name=\"act\" value=\"%ld\" />",
	        Act_GetActCod (NextAction));

      if (Gbl.Session.Id[0])
	 snprintf (ParamSession,sizeof (ParamSession),
		   "<input type=\"hidden\" name=\"ses\" value=\"%s\" />",
		   Gbl.Session.Id);
      else if (PutParameterLocationIfNoSesion)
	 // Extra parameters necessary when there's no open session
	{
	 /* If session is open, course code will be get from session data,
	    but if there is not an open session, and next action is known,
	    it is necessary to send a parameter with course code */
	 switch (Gbl.Hierarchy.Level)
	   {
	    case Hie_Lvl_CTY:	// Country
	       snprintf (ParamLocation,sizeof (ParamLocation),
			 "<input type=\"hidden\" name=\"cty\" value=\"%ld\" />",
			 Gbl.Hierarchy.Cty.CtyCod);
	       break;
	    case Hie_Lvl_INS:	// Institution
	       snprintf (ParamLocation,sizeof (ParamLocation),
			 "<input type=\"hidden\" name=\"ins\" value=\"%ld\" />",
			 Gbl.Hierarchy.Ins.InsCod);
	       break;
	    case Hie_Lvl_CTR:	// Centre
	       snprintf (ParamLocation,sizeof (ParamLocation),
			 "<input type=\"hidden\" name=\"ctr\" value=\"%ld\" />",
			 Gbl.Hierarchy.Ctr.CtrCod);
	       break;
	    case Hie_Lvl_DEG:	// Degree
	       snprintf (ParamLocation,sizeof (ParamLocation),
			 "<input type=\"hidden\" name=\"deg\" value=\"%ld\" />",
			 Gbl.Hierarchy.Deg.DegCod);
	       break;
	    case Hie_Lvl_CRS:	// Course
	       snprintf (ParamLocation,sizeof (ParamLocation),
			 "<input type=\"hidden\" name=\"crs\" value=\"%ld\" />",
			 Gbl.Hierarchy.Crs.CrsCod);
	       break;
	    default:
	       break;
	   }
	}
     }

   snprintf (ParamsStr,Frm_MAX_BYTES_PARAMS_STR + 1,
	     "%s%s%s",
	     ParamAction,ParamSession,ParamLocation);
  }

void Frm_EndForm (void)
  {
   if (Gbl.Form.Inside)
     {
      HTM_Txt ("</form>");
      Gbl.Form.Inside = false;
     }
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
          - a name for this execution (Gbl.UniqueNameEncrypted)
          - a number for each element in this execution (CountForThisExecution) *****/
   snprintf (UniqueId,Frm_MAX_BYTES_ID + 1,
	     "id_%s_%u",
             Gbl.UniqueNameEncrypted,
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
	 Lay_NotEnoughMemoryExit ();
     }
   else
      *Anchor = NULL;
  }

void Frm_FreeAnchorStr (char *Anchor)
  {
   if (Anchor)
     {
      free (Anchor);
      Anchor = NULL;
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
   extern const char *The_ClassFormInBox[The_NUM_THEMES];

   /***** Column/cell begin *****/
   if (TDClass)
      HTM_TD_Begin ("class=\"%s\"",TDClass);
   else
      HTM_TD_Begin (NULL);

   /***** Label *****/
   if (Id)
     {
      if (Id[0])
	 HTM_LABEL_Begin ("for=\"%s\" class=\"%s\"",
			  Id,The_ClassFormInBox[Gbl.Prefs.Theme]);
      else
	 HTM_LABEL_Begin ("class=\"%s\"",
			  The_ClassFormInBox[Gbl.Prefs.Theme]);
     }
   else
      HTM_LABEL_Begin ("class=\"DAT\"");
   HTM_TxtColon (Label);
   HTM_LABEL_End ();

   /***** Column/cell end *****/
   HTM_TD_End ();
  }
