// swad_button.c: buttons to submit forms

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

#include "swad_button.h"
#include "swad_global.h"
#include "swad_HTML.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

extern const char *Txt_Add_questions;
extern const char *Txt_Answer_VERB;
extern const char *Txt_Change;
extern const char *Txt_Check;
extern const char *Txt_Confirm;
extern const char *Txt_Continue;
extern const char *Txt_Create;
extern const char *Txt_Create_account;
extern const char *Txt_Done;
extern const char *Txt_Edit;
extern const char *Txt_Enrol;
extern const char *Txt_Eliminate;
extern const char *Txt_Follow;
extern const char *Txt_Go;
extern const char *Txt_Its_me;
extern const char *Txt_Lock_editing;
extern const char *Txt_Not_duplicated;
extern const char *Txt_Paste;
extern const char *Txt_Reject;
extern const char *Txt_Remove;
extern const char *Txt_Reset;
extern const char *Txt_Save_changes;
extern const char *Txt_Search;
extern const char *Txt_Send;
extern const char *Txt_Show;
extern const char *Txt_Show_more_details;
extern const char *Txt_Show_questions;
extern const char *Txt_Similar_users;
extern const char *Txt_Skip_this_step;
extern const char *Txt_Unfollow;
extern const char *Txt_Unlock_editing;
extern const char *Txt_Upload;
extern const char *Txt_Use_this;
extern const char *Txt_View_results;

static struct
  {
   const char *Class;
   const char **Txt;
  } But_Buttons[Btn_NUM_BUTTON_TYPES] =
  {
   [Btn_ADD_QUESTIONS		] = {"BT_CREATE"	,&Txt_Add_questions	},
   [Btn_ANSWER			] = {"BT_CREATE"	,&Txt_Answer_VERB	},
   [Btn_CHANGE			] = {"BT_CREATE"	,&Txt_Change		},
   [Btn_CHECK			] = {"BT_CONFIRM"	,&Txt_Check		},
   [Btn_CONFIRM			] = {"BT_CONFIRM"	,&Txt_Confirm		},
   [Btn_CONTINUE		] = {"BT_CONFIRM"	,&Txt_Continue		},
   [Btn_CREATE			] = {"BT_CREATE"	,&Txt_Create		},
   [Btn_CREATE_ACCOUNT		] = {"BT_CREATE"	,&Txt_Create_account	},
   [Btn_DONE			] = {"BT_CONFIRM"	,&Txt_Done		},
   [Btn_EDIT			] = {"BT_CONFIRM"	,&Txt_Edit		},
   [Btn_ENROL			] = {"BT_CREATE"	,&Txt_Enrol		},
   [Btn_ELIMINATE		] = {"BT_REMOVE"	,&Txt_Eliminate		},
   [Btn_FOLLOW			] = {"BT_CREATE"	,&Txt_Follow		},
   [Btn_GO			] = {"BT_CONFIRM"	,&Txt_Go		},
   [Btn_ITS_ME			] = {"BT_CREATE"	,&Txt_Its_me		},
   [Btn_LOCK_EDITING		] = {"BT_REMOVE"	,&Txt_Lock_editing	},
   [Btn_NOT_DUPLICATED		] = {"BT_CONFIRM"	,&Txt_Not_duplicated	},
   [Btn_PASTE			] = {"BT_CONFIRM"	,&Txt_Paste		},
   [Btn_REJECT			] = {"BT_REMOVE"	,&Txt_Reject		},
   [Btn_REMOVE			] = {"BT_REMOVE"	,&Txt_Remove		},
   [Btn_RESET			] = {"BT_REMOVE"	,&Txt_Reset		},
   [Btn_SAVE_CHANGES		] = {"BT_CONFIRM"	,&Txt_Save_changes	},
   [Btn_SEARCH			] = {"BT_CONFIRM"	,&Txt_Search		},
   [Btn_SEND			] = {"BT_CREATE"	,&Txt_Send		},
   [Btn_SHOW			] = {"BT_CONFIRM"	,&Txt_Show		},
   [Btn_SHOW_MORE_DETAILS	] = {"BT_CONFIRM"	,&Txt_Show_more_details	},
   [Btn_SHOW_QUESTIONS		] = {"BT_CONFIRM"	,&Txt_Show_questions	},
   [Btn_SIMILAR_USERS		] = {"BT_CONFIRM"	,&Txt_Similar_users	},
   [Btn_SKIP_THIS_STEP		] = {"BT_CONFIRM"	,&Txt_Skip_this_step	},
   [Btn_UNFOLLOW		] = {"BT_REMOVE"	,&Txt_Unfollow		},
   [Btn_UNLOCK_EDITING		] = {"BT_CREATE"	,&Txt_Unlock_editing	},
   [Btn_UPLOAD			] = {"BT_CREATE"	,&Txt_Upload		},
   [Btn_USE_THIS		] = {"BT_CONFIRM"	,&Txt_Use_this		},
   [Btn_VIEW_RESULTS		] = {"BT_CONFIRM"	,&Txt_View_results	},
  };

/*****************************************************************************/
/********************** Put a button to submit a form ************************/
/*****************************************************************************/

void Btn_PutButton (Btn_Button_t Button)
  {
   if (Button != Btn_NO_BUTTON)
     {
      HTM_DIV_Begin ("class=\"CM\"");
	 HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT %s\"",
				  But_Buttons[Button].Class);
	    HTM_Txt (*But_Buttons[Button].Txt);
	 HTM_BUTTON_End ();
      HTM_DIV_End ();
     }
  }

void Btn_PutButtonInline (Btn_Button_t Button)
  {
   if (Button != Btn_NO_BUTTON)
     {
      HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT_INLINE %s\"",
			       But_Buttons[Button].Class);
	 HTM_Txt (*But_Buttons[Button].Txt);
      HTM_BUTTON_End ();
     }
  }
