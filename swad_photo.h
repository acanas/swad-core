// swad_photo.h: Users' photos management

#ifndef _SWAD_PHO
#define _SWAD_PHO
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_user_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Usr_Can_t Pho_ICanChangeOtherUsrPhoto (struct Usr_Data *UsrDat);
void Pho_PutIconToChangeUsrPhoto (struct Usr_Data *UsrDat);
void Pho_ReqMyPhoto (void);
void Pho_ReqPhotoUsr (void);
void Pho_RecMyPhotoDetFaces (void);
void Pho_RecOtherUsrPhotoDetFaces (void);

void Pho_ReqRemMyPhoto (void);
void Pho_RemoveMyPhoto1 (void);
void Pho_RemoveMyPhoto2 (void);
void Pho_ReqRemUsrPhoto (void);
void Pho_RemoveUsrPhoto (void);

void Pho_UpdateMyPhoto1 (void);
void Pho_UpdateMyPhoto2 (void);
void Pho_ChangeUsrPhoto1 (void);
void Pho_ChangeUsrPhoto2 (void);

unsigned Pho_UpdateMyClicksWithoutPhoto (void);

void Pho_ShowUsrPhotoIfAllowed (struct Usr_Data *UsrDat,
                                const char *ClassPhoto,Pho_Zoom_t Zoom);
Pho_ShowPhotos_t Pho_ShowingUsrPhotoIsAllowed (struct Usr_Data *UsrDat,
					       char PhotoURL[WWW_MAX_BYTES_WWW + 1]);
Exi_Exist_t Pho_BuildLinkToPhoto (const struct Usr_Data *UsrDat,
				  char PhotoURL[WWW_MAX_BYTES_WWW + 1]);
void Pho_BuildHTMLUsrPhoto (const struct Usr_Data *UsrDat,const char *PhotoURL,
			    const char *ClassPhoto,Pho_Zoom_t Zoom,
			    char **ImgStr,
			    char **CaptionStr);
void Pho_ShowUsrPhoto (const struct Usr_Data *UsrDat,const char *PhotoURL,
                       const char *ClassPhoto,Pho_Zoom_t Zoom);

Err_SuccessOrError_t Pho_RemovePhoto (struct Usr_Data *UsrDat);
void Pho_UpdatePhotoName (struct Usr_Data *UsrDat);

void Pho_ChangePhotoVisibility (void);

void Pho_PutButtonShowPhotos (Pho_ShowPhotos_t ShowPhotos,
			      const char *FormId,const char *OnSubmit);
Pho_ShowPhotos_t Pho_GetParShowPhotos (void);
Pho_ShowPhotos_t Pho_GetMyPrefAboutListWithPhotosFromDB (void);

void Pho_CalcPhotoDegree (void);
void Pho_ShowPhotoDegree (void);
void Pho_PrintPhotoDegree (void);

void Pho_PutParTypeOfAvg (Pho_AvgPhotoTypeOfAverage_t TypeOfAverage);
void Pho_PutParPhotoSize (Pho_HowComputePhotoSize_t HowComputePhotoSize);
void Pho_PutParOrderDegrees (Pho_HowOrderDegrees_t HowOrderDegrees);

#endif
