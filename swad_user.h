// swad_user.h: users

#ifndef _SWAD_USR
#define _SWAD_USR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_forum.h"
#include "swad_user_type.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Usr_FORM_TO_SELECT_USRS_ID "form_sel_usrs"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Usr_InformAboutNumClicksBeforePhoto (void);

void Usr_UsrDataConstructor (struct Usr_Data *UsrDat);
void Usr_ResetUsrDataExceptUsrCodAndIDs (struct Usr_Data *UsrDat);
void Usr_ResetMyLastData (void);
void Usr_UsrDataDestructor (struct Usr_Data *UsrDat);
void Usr_GetAllUsrDataFromUsrCod (struct Usr_Data *UsrDat,
                                  Usr_GetPrefs_t GetPrefs,
                                  Usr_GetRoleInCurrentCrs_t GetRoleInCurrentCrs);
void Usr_AllocateListUsrCods (struct Usr_ListUsrCods *ListUsrCods);
void Usr_FreeListUsrCods (struct Usr_ListUsrCods *ListUsrCods);
Usr_MeOrOther_t Usr_ItsMe (long UsrCod);
void Usr_GetUsrCodFromEncryptedUsrCod (struct Usr_Data *UsrDat);
void Usr_GetUsrDataFromUsrCod (struct Usr_Data *UsrDat,
                               Usr_GetPrefs_t GetPrefs,
                               Usr_GetRoleInCurrentCrs_t GetRoleInCurrentCrs);

void Usr_BuildFullName (struct Usr_Data *UsrDat);

void Usr_WriteFirstNameBRSurnames (const struct Usr_Data *UsrDat);

void Usr_FlushCachesUsr (void);

void Usr_FlushCacheUsrIsSuperuser (void);
bool Usr_CheckIfUsrIsSuperuser (long UsrCod);

Usr_Can_t Usr_ICanChangeOtherUsrData (const struct Usr_Data *UsrDat);
Usr_Can_t Usr_CheckIfICanEditOtherUsr (const struct Usr_Data *UsrDat);

Usr_Can_t Usr_CheckIfICanViewRecordStd (const struct Usr_Data *UsrDat);
Usr_Can_t Usr_CheckIfICanViewRecordTch (struct Usr_Data *UsrDat);
Usr_Can_t Usr_CheckIfICanViewTstExaMchResult (const struct Usr_Data *UsrDat);
Usr_Can_t Usr_CheckIfICanViewAsgWrk (const struct Usr_Data *UsrDat);
Usr_Can_t Usr_CheckIfICanViewAtt (const struct Usr_Data *UsrDat);

void Usr_WriteLandingPage (void);
void Usr_WriteFormLogout (void);
void Usr_Logout (void);
void Usr_PutLinkToLogin (void);
void Usr_WriteFormLogin (Act_Action_t NextAction,void (*FuncPars) (void));
void Usr_WelcomeUsr (void);

void Usr_CreateBirthdayStrDB (const struct Usr_Data *UsrDat,
                              char BirthdayStrDB[Usr_BIRTHDAY_STR_DB_LENGTH + 1]);
void Usr_FilterUsrBirthday (struct Dat_Date *Birthday);

void Usr_PutFormLogIn (void);
void Usr_WriteLoggedUsrHead (void);
void Usr_PutFormLogOut (void);
void Usr_GetParUsrIdLogin (void);
unsigned Usr_GetParOtherUsrIDNickOrEMailAndGetUsrCods (struct Usr_ListUsrCods *ListUsrCods);

void Usr_PutParMyUsrCodEncrypted (void *EncryptedUsrCod);
void Usr_PutParOtherUsrCodEncrypted (void *EncryptedUsrCod);
void Usr_PutParUsrCodEncrypted (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);
void Usr_GetParOtherUsrCodEncrypted (struct Usr_Data *UsrDat);
void Usr_GetParOtherUsrCodEncryptedAndGetListIDs (void);
bool Usr_GetParOtherUsrCodEncryptedAndGetUsrData (void);

void Usr_ChkUsrGetUsrDataAndAdjustAction (void);

void Usr_ShowFormsLogoutAndRole (void);

bool Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (struct Usr_Data *UsrDat,
                                              Usr_GetPrefs_t GetPrefs,
                                              Usr_GetRoleInCurrentCrs_t GetRoleInCurrentCrs);
void Usr_UpdateMyLastData (void);
void Usr_InsertMyLastCrsTabAndTime (void);

void Usr_WriteRowUsrMainData (unsigned NumUsr,struct Usr_Data *UsrDat,
                              bool PutCheckBoxToSelectUsr,Rol_Role_t Role,
			      struct Usr_SelectedUsrs *SelectedUsrs,
			      Pho_ShowPhotos_t ShowPhotos);

void Usr_GetListUsrs (Hie_Level_t HieLvl,Rol_Role_t Role);

void Usr_SearchListUsrs (Hie_Level_t HieLvl,Rol_Role_t Role);

void Usr_GetUnorderedStdsCodesInDeg (long DegCod);

void Usr_GetListUsrsFromQuery (char *Query,Hie_Level_t HieLvl,Rol_Role_t Role);

void Usr_CopyBasicUsrDataFromList (struct Usr_Data *UsrDat,
                                   const struct Usr_InList *UsrInList);
void Usr_FreeUsrsList (Rol_Role_t Role);

bool Usr_GetIfShowBigList (unsigned NumUsrs,
                           void (*FuncPars) (void *Args),void *Args,
                           const char *OnSubmit);

void Usr_CreateListSelectedUsrsCodsAndFillWithOtherUsr (struct Usr_SelectedUsrs *SelectedUsrs);
void Usr_PutParSelectedUsrsCods (const struct Usr_SelectedUsrs *SelectedUsrs);
void Usr_GetListsSelectedEncryptedUsrsCods (struct Usr_SelectedUsrs *SelectedUsrs,
					    Usr_GetListAllUsrs_t GetListAllUsrs);

bool Usr_GetListMsgRecipientsWrittenExplicitelyBySender (bool WriteErrorMsgs);

bool Usr_FindEncUsrCodInListOfSelectedEncUsrCods (const char *EncryptedUsrCodToFind,
						  struct Usr_SelectedUsrs *SelectedUsrs);
bool Usr_CheckIfThereAreUsrsInListOfSelectedEncryptedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs);
unsigned Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs);
void Usr_FreeListsSelectedEncryptedUsrsCods (struct Usr_SelectedUsrs *SelectedUsrs);

void Usr_GetListSelectedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs,
				 unsigned NumUsrsInList,
				 long **LstSelectedUsrCods);
void Usr_FreeListSelectedUsrCods (long *LstSelectedUsrCods);

void Usr_CreateSubqueryUsrCods (long *LstSelectedUsrCods,
				unsigned NumUsrsInList,
				char **SubQueryUsrs);
void Usr_FreeSubqueryUsrCods (char *SubQueryUsrs);

void Usr_FreeListOtherRecipients (void);

void Usr_ShowFormsToSelectUsrListType (void (*FuncPars) (void *Args),void *Args,
				       const char *OnSubmit,
				       Pho_ShowPhotos_t ShowPhotos);
unsigned Usr_GetColumnsForSelectUsrs (Pho_ShowPhotos_t ShowPhotos);
void Usr_PutCheckboxToSelectUser (Rol_Role_t Role,
				  const char *EncryptedUsrCod,
				  bool UsrIsTheMsgSender,
				  struct Usr_SelectedUsrs *SelectedUsrs);
void Usr_WriteHeaderFieldsUsrDat (bool PutCheckBoxToSelectUsr,
				  Pho_ShowPhotos_t ShowPhotos);

void Usr_PutFormToSelectUsrsToGoToAct (struct Usr_SelectedUsrs *SelectedUsrs,
				       Act_Action_t NextAction,
				       void (*FuncPars) (void *Args),void *Args,
				       const char *Title,
                                       const char *HelpLink,
                                       Btn_Button_t Button,
				       Frm_PutForm_t PutFormDateRange);
void Usr_GetSelectedUsrsAndGoToAct (struct Usr_SelectedUsrs *SelectedUsrs,
				    void (*FuncWhenUsrsSelected) (void *ArgsSelected),void *ArgsSelected,
                                    void (*FuncWhenNoUsrsSelected) (void *ArgsNoSelected),void *ArgsNoSelected);
void Usr_ListUsersToSelect (struct Usr_SelectedUsrs *SelectedUsrs,
			    Pho_ShowPhotos_t ShowPhotos);
void Usr_PutCheckboxToSelectAllUsers (struct Usr_SelectedUsrs *SelectedUsrs,
				      Rol_Role_t Role);
void Usr_WriteNumUsrsInList (Rol_Role_t Role);

void Usr_ListAllDataGsts (void);
void Usr_ListAllDataStds (void);
void Usr_ListAllDataTchs (void);
unsigned Usr_ListUsrsFound (Hie_Level_t HieLvl,Rol_Role_t Role,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1]);
void Usr_ListDataAdms (void);

void Usr_ListGuests (void);
void Usr_ListStudents (void);
void Usr_ListTeachers (void);

void Usr_PutListUsrsActions (const Usr_Can_t ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS]);
void Usr_DoActionOnUsrs1 (void);
void Usr_DoActionOnUsrs2 (void);

void Usr_SeeGstClassPhotoPrn (void);
void Usr_SeeStdClassPhotoPrn (void);
void Usr_SeeTchClassPhotoPrn (void);

void Usr_ConstructPathUsr (long UsrCod,char PathUsr[PATH_MAX + 1]);

void Usr_ShowWarningNoUsersFound (Rol_Role_t Role);

unsigned Usr_GetTotalNumberOfUsers (Hie_Level_t HieLvl);

void Usr_WriteAuthor (struct Usr_Data *UsrDat,For_Disabled_t Disabled);
void Usr_WriteAuthor1Line (long UsrCod,HidVis_HiddenOrVisible_t HiddenOrVisible);

void Usr_ShowTableCellWithUsrData (struct Usr_Data *UsrDat,unsigned NumRows);

void Usr_PutWhoIcon (Usr_Who_t Who);
void Usr_PutParWho (Usr_Who_t Who);
Usr_Who_t Usr_GetParWho (void);

void Usr_THTchsPlusStds (void);

//-------------------------------- Figures ------------------------------------
void Usr_GetAndShowUsersStats (Hie_Level_t HieLvl);
void Usr_GetAndShowUsersRanking (Hie_Level_t HieLvl);

#endif
