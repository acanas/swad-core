// swad_text_action.c: texts of actions, in several languages

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

#include "swad_action.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

#ifndef L
#define L 3	// English
#endif

const char *Txt_Actions[Act_NUM_ACTIONS] =
	{
	[ActAll] =
#if   L==1	// ca
	"Any action"					// Necessita traducció
#elif L==2	// de
	"Any action"					// Need Übersetzung
#elif L==3	// en
	"Any action"
#elif L==4	// es
	"Cualquier acci&oacute;n"
#elif L==5	// fr
	"Any action"					// Besoin de traduction
#elif L==6	// gn
	"Cualquier acci&oacute;n"			// Okoteve traducción
#elif L==7	// it
	"Any action"					// Bisogno di traduzione
#elif L==8	// pl
	"Any action"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Any action"					// Precisa de tradução
#endif
	,
	[ActUnk] =
#if   L==1	// ca
	"Unknown action"				// Necessita traducció
#elif L==2	// de
	"Unknown action"				// Need Übersetzung
#elif L==3	// en
	"Unknown action"
#elif L==4	// es
	"Acci&oacute;n desconocida"
#elif L==5	// fr
	"Unknown action"				// Besoin de traduction
#elif L==6	// gn
	"Acci&oacute;n desconocida"			// Okoteve traducción
#elif L==7	// it
	"Unknown action"				// Bisogno di traduzione
#elif L==8	// pl
	"Unknown action"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unknown action"				// Precisa de tradução
#endif
	,
	[ActMnu] =
#if   L==1	// ca
	"Show menu"					// Necessita traducció
#elif L==2	// de
	"Show menu"					// Need Übersetzung
#elif L==3	// en
	"Show menu"
#elif L==4	// es
	"Mostrar men&uacute;"
#elif L==5	// fr
	"Show menu"					// Besoin de traduction
#elif L==6	// gn
	"Mostrar men&uacute;"				// Okoteve traducción
#elif L==7	// it
	"Show menu"					// Bisogno di traduzione
#elif L==8	// pl
	"Show menu"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show menu"					// Precisa de tradução
#endif
	,
	[ActRefCon] =
#if   L==1	// ca
	"Refresh notifications and connected"		// Necessita traducció
#elif L==2	// de
	"Refresh notifications and connected"		// Need Übersetzung
#elif L==3	// en
	"Refresh notifications and connected"
#elif L==4	// es
	"Refrescar notificaciones y conectados"
#elif L==5	// fr
	"Refresh notifications and connected"		// Besoin de traduction
#elif L==6	// gn
	"Refrescar notificaciones y conectados"		// Okoteve traducción
#elif L==7	// it
	"Refresh notifications and connected"		// Bisogno di traduzione
#elif L==8	// pl
	"Refresh notifications and connected"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Refresh notifications and connected"		// Precisa de tradução
#endif
	,
	[ActWebSvc] =
#if   L==1	// ca
	"Web service (API function)"			// Necessita traducció
#elif L==2	// de
	"Web service (API function)"			// Need Übersetzung
#elif L==3	// en
	"Web service (API function)"
#elif L==4	// es
	"Servicio web (funci&oacute;n API)"
#elif L==5	// fr
	"Web service (API function)"			// Besoin de traduction
#elif L==6	// gn
	"Servicio web (funci&oacute;n API)"		// Okoteve traducción
#elif L==7	// it
	"Web service (API function)"			// Bisogno di traduzione
#elif L==8	// pl
	"Web service (API function)"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Web service (API function)"			// Precisa de tradução
#endif
	,
	[ActFrmLogIn] =
#if   L==1	// ca
	"Landing page"					// Necessita traducció
#elif L==2	// de
	"Landing page"					// Need Übersetzung
#elif L==3	// en
	"Landing page"
#elif L==4	// es
	"P&aacute;gina de inicio"
#elif L==5	// fr
	"Landing page"					// Besoin de traduction
#elif L==6	// gn
	"P&aacute;gina de inicio"			// Okoteve traducción
#elif L==7	// it
	"Landing page"					// Bisogno di traduzione
#elif L==8	// pl
	"Landing page"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Landing page"					// Precisa de tradução
#endif
	,
	[ActReqSch] =
#if   L==1	// ca
	"Request search"				// Necessita traducció
#elif L==2	// de
	"Request search"				// Need Übersetzung
#elif L==3	// en
	"Request search"
#elif L==4	// es
	"Solicitar b&uacute;squeda"
#elif L==5	// fr
	"Request search"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar b&uacute;squeda"			// Okoteve traducción
#elif L==7	// it
	"Request search"				// Bisogno di traduzione
#elif L==8	// pl
	"Request search"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request search"				// Precisa de tradução
#endif
	,
	[ActSeeGblTL] =
#if   L==1	// ca
	"Show timeline global"				// Necessita traducció
#elif L==2	// de
	"Show timeline global"				// Need Übersetzung
#elif L==3	// en
	"Show timeline global"
#elif L==4	// es
	"Mostrar timeline global"
#elif L==5	// fr
	"Show timeline global"				// Besoin de traduction
#elif L==6	// gn
	"Mostrar timeline global"			// Okoteve traducción
#elif L==7	// it
	"Show timeline global"				// Bisogno di traduzione
#elif L==8	// pl
	"Show timeline global"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show timeline global"				// Precisa de tradução
#endif
	,
	[ActSeeSocPrf] =
#if   L==1	// ca
	"Suggest list of users to follow"		// Necessita traducció
#elif L==2	// de
	"Suggest list of users to follow"		// Need Übersetzung
#elif L==3	// en
	"Suggest list of users to follow"
#elif L==4	// es
	"Sugerir lista de usuarios a seguir"
#elif L==5	// fr
	"Suggest list of users to follow"		// Besoin de traduction
#elif L==6	// gn
	"Sugerir lista de usuarios a seguir"		// Okoteve traducción
#elif L==7	// it
	"Suggest list of users to follow"		// Bisogno di traduzione
#elif L==8	// pl
	"Suggest list of users to follow"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Suggest list of users to follow"		// Precisa de tradução
#endif
	,
	[ActSeeCal] =
#if   L==1	// ca
	"Show calendar"					// Necessita traducció
#elif L==2	// de
	"Show calendar"					// Need Übersetzung
#elif L==3	// en
	"Show calendar"
#elif L==4	// es
	"Mostrar calendario"
#elif L==5	// fr
	"Show calendar"					// Besoin de traduction
#elif L==6	// gn
	"Mostrar calendario"				// Okoteve traducción
#elif L==7	// it
	"Show calendar"					// Bisogno di traduzione
#elif L==8	// pl
	"Show calendar"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show calendar"					// Precisa de tradução
#endif
	,
	[ActSeeNtf] =
#if   L==1	// ca
	"Show notifications"				// Necessita traducció
#elif L==2	// de
	"Show notifications"				// Need Übersetzung
#elif L==3	// en
	"Show notifications"
#elif L==4	// es
	"Mostrar notificaciones"
#elif L==5	// fr
	"Show notifications"				// Besoin de traduction
#elif L==6	// gn
	"Mostrar notificaciones"			// Okoteve traducción
#elif L==7	// it
	"Show notifications"				// Bisogno di traduzione
#elif L==8	// pl
	"Show notifications"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show notifications"				// Precisa de tradução
#endif
	,
	[ActLogIn] =
#if   L==1	// ca
	"Authenticate user"				// Necessita traducció
#elif L==2	// de
	"Authenticate user"				// Need Übersetzung
#elif L==3	// en
	"Authenticate user"
#elif L==4	// es
	"Autenticar usuario"
#elif L==5	// fr
	"Authenticate user"				// Besoin de traduction
#elif L==6	// gn
	"Autenticar usuario"				// Okoteve traducción
#elif L==7	// it
	"Authenticate user"				// Bisogno di traduzione
#elif L==8	// pl
	"Authenticate user"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Authenticate user"				// Precisa de tradução
#endif
	,
	[ActLogInNew] =
#if   L==1	// ca
	"Authenticate user in empty account"		// Necessita traducció
#elif L==2	// de
	"Authenticate user in empty account"		// Need Übersetzung
#elif L==3	// en
	"Authenticate user in empty account"
#elif L==4	// es
	"Autenticar usuario en cuenta vac&iacute;a"
#elif L==5	// fr
	"Authenticate user in empty account"		// Besoin de traduction
#elif L==6	// gn
	"Autenticar usuario en cuenta vac&iacute;a"	// Okoteve traducción
#elif L==7	// it
	"Authenticate user in empty account"		// Bisogno di traduzione
#elif L==8	// pl
	"Authenticate user in empty account"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Authenticate user in empty account"		// Precisa de tradução
#endif
	,
	[ActLogInLan] =
#if   L==1	// ca
	"Change language after authentication"		// Necessita traducció
#elif L==2	// de
	"Change language after authentication"		// Need Übersetzung
#elif L==3	// en
	"Change language after authentication"
#elif L==4	// es
	"Cambiar idioma tras autenticar"
#elif L==5	// fr
	"Change language after authentication"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar idioma tras autenticar"		// Okoteve traducción
#elif L==7	// it
	"Change language after authentication"		// Bisogno di traduzione
#elif L==8	// pl
	"Change language after authentication"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change language after authentication"		// Precisa de tradução
#endif
	,
	[ActAnnSee] =
#if   L==1	// ca
	"Mark announcement as seen"			// Necessita traducció
#elif L==2	// de
	"Mark announcement as seen"			// Need Übersetzung
#elif L==3	// en
	"Mark announcement as seen"
#elif L==4	// es
	"Marcar anuncio como visto"
#elif L==5	// fr
	"Mark announcement as seen"			// Besoin de traduction
#elif L==6	// gn
	"Marcar anuncio como visto"			// Okoteve traducción
#elif L==7	// it
	"Mark announcement as seen"			// Bisogno di traduzione
#elif L==8	// pl
	"Mark announcement as seen"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Mark announcement as seen"			// Precisa de tradução
#endif
	,
	[ActReqSndNewPwd] =
#if   L==1	// ca
	"Request to send a new password"			// Necessita traducció
#elif L==2	// de
	"Request to send a new password"			// Need Übersetzung
#elif L==3	// en
	"Request to send a new password"
#elif L==4	// es
	"Solicitar env&iacute;o de nueva contrase&ntilde;a"
#elif L==5	// fr
	"Request to send a new password"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar env&iacute;o de nueva contrase&ntilde;a"	// Okoteve traducción
#elif L==7	// it
	"Request to send a new password"			// Bisogno di traduzione
#elif L==8	// pl
	"Request to send a new password"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request to send a new password"			// Precisa de tradução
#endif
	,
	[ActSndNewPwd] =
#if   L==1	// ca
	"Send a new password"				// Necessita traducció
#elif L==2	// de
	"Send a new password"				// Need Übersetzung
#elif L==3	// en
	"Send a new password"
#elif L==4	// es
	"Enviar nueva contrase&ntilde;a"
#elif L==5	// fr
	"Send a new password"				// Besoin de traduction
#elif L==6	// gn
	"Enviar nueva contrase&ntilde;a"		// Okoteve traducción
#elif L==7	// it
	"Send a new password"				// Bisogno di traduzione
#elif L==8	// pl
	"Send a new password"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a new password"				// Precisa de tradução
#endif
	,
	[ActLogOut] =
#if   L==1	// ca
	"Close session"					// Necessita traducció
#elif L==2	// de
	"Close session"					// Need Übersetzung
#elif L==3	// en
	"Close session"
#elif L==4	// es
	"Cerrar sesi&oacute;n"
#elif L==5	// fr
	"Close session"					// Besoin de traduction
#elif L==6	// gn
	"Cerrar sesi&oacute;n"				// Okoteve traducción
#elif L==7	// it
	"Close session"					// Bisogno di traduzione
#elif L==8	// pl
	"Close session"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Close session"					// Precisa de tradução
#endif
	,
	[ActSch] =
#if   L==1	// ca
	"Search"					// Necessita traducció
#elif L==2	// de
	"Search"					// Need Übersetzung
#elif L==3	// en
	"Search"
#elif L==4	// es
	"Buscar"
#elif L==5	// fr
	"Search"					// Besoin de traduction
#elif L==6	// gn
	"Buscar"					// Okoteve traducción
#elif L==7	// it
	"Search"					// Bisogno di traduzione
#elif L==8	// pl
	"Search"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Search"					// Precisa de tradução
#endif
	,
	[ActRefNewPubGblTL] =
#if   L==1	// ca
	"Refresh global timeline (new publications)"		// Necessita traducció
#elif L==2	// de
	"Refresh global timeline (new publications)"		// Need Übersetzung
#elif L==3	// en
	"Refresh global timeline (new publications)"
#elif L==4	// es
	"Refrescar timeline global (nuevas publicaciones)"
#elif L==5	// fr
	"Refresh global timeline (new publications)"		// Besoin de traduction
#elif L==6	// gn
	"Refrescar timeline global (nuevas publicaciones)"	// Okoteve traducción
#elif L==7	// it
	"Refresh global timeline (new publications)"		// Bisogno di traduzione
#elif L==8	// pl
	"Refresh global timeline (new publications)"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Refresh global timeline (new publications)"		// Precisa de tradução
#endif
	,
	[ActRefOldPubGblTL] =
#if   L==1	// ca
	"Show previous publications in global timeline"		// Necessita traducció
#elif L==2	// de
	"Show previous publications in global timeline"		// Need Übersetzung
#elif L==3	// en
	"Show previous publications in global timeline"
#elif L==4	// es
	"Mostrar publicaciones anteriores en timeline global"
#elif L==5	// fr
	"Show previous publications in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar publicaciones anteriores en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show previous publications in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Show previous publications in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show previous publications in global timeline"		// Precisa de tradução
#endif
	,
	[ActRcvPstGblTL] =
#if   L==1	// ca
	"Create post in global timeline"		// Necessita traducció
#elif L==2	// de
	"Create post in global timeline"		// Need Übersetzung
#elif L==3	// en
	"Create post in global timeline"
#elif L==4	// es
	"Crear post en timeline global"
#elif L==5	// fr
	"Create post in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Crear post en timeline global"			// Okoteve traducción
#elif L==7	// it
	"Create post in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Create post in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create post in global timeline"		// Precisa de tradução
#endif
	,
	[ActRcvComGblTL] =
#if   L==1	// ca
	"Create comment in global timeline"		// Necessita traducció
#elif L==2	// de
	"Create comment in global timeline"		// Need Übersetzung
#elif L==3	// en
	"Create comment in global timeline"
#elif L==4	// es
	"Crear comentario en timeline global"
#elif L==5	// fr
	"Create comment in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Crear comentario en timeline global"		// Okoteve traducción
#elif L==7	// it
	"Create comment in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Create comment in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create comment in global timeline"		// Precisa de tradução
#endif
	,
	[ActShoHidComGblTL] =
#if   L==1	// ca
	"Show hidden comments in global timeline"	// Necessita traducció
#elif L==2	// de
	"Show hidden comments in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Show hidden comments in global timeline"
#elif L==4	// es
	"Mostrar comentarios ocultos en timeline global"
#elif L==5	// fr
	"Show hidden comments in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar comentarios ocultos en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show hidden comments in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Show hidden comments in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show hidden comments in global timeline"		// Precisa de tradução
#endif
	,
	[ActAllShaNotGblTL] =
#if   L==1	// ca
	"Show who shared a publication in global timeline"				// Necessita traducció
#elif L==2	// de
	"Show who shared a publication in global timeline"				// Need Übersetzung
#elif L==3	// en
	"Show who shared a publication in global timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Show who shared a publication in global timeline"				// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show who shared a publication in global timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Show who shared a publication in global timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who shared a publication in global timeline"				// Precisa de tradução
#endif
	,
	[ActAllFavNotGblTL] =
#if   L==1	// ca
	"Show who favourited a publication in global timeline"						// Necessita traducció
#elif L==2	// de
	"Show who favourited a publication in global timeline"						// Need Übersetzung
#elif L==3	// en
	"Show who favourited a publication in global timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Show who favourited a publication in global timeline"						// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a publication in global timeline"						// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a publication in global timeline"						// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a publication in global timeline"						// Precisa de tradução
#endif
	,
	[ActAllFavComGblTL] =
#if   L==1	// ca
	"Show who favourited a comment in global timeline"					// Necessita traducció
#elif L==2	// de
	"Show who favourited a comment in global timeline"					// Need Übersetzung
#elif L==3	// en
	"Show who favourited a comment in global timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline global"
#elif L==5	// fr
	"Show who favourited a comment in global timeline"					// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a comment in global timeline"					// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a comment in global timeline"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a comment in global timeline"					// Precisa de tradução
#endif
	,
	[ActShaNotGblTL] =
#if   L==1	// ca
	"Share a publication in global timeline"		// Necessita traducció
#elif L==2	// de
	"Share a publication in global timeline"		// Need Übersetzung
#elif L==3	// en
	"Share a publication in global timeline"
#elif L==4	// es
	"Compartir una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Share a publication in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Compartir una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Share a publication in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Share a publication in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Share a publication in global timeline"		// Precisa de tradução
#endif
	,
	[ActUnsNotGblTL] =
#if   L==1	// ca
	"Unshare a publication in global timeline"			// Necessita traducció
#elif L==2	// de
	"Unshare a publication in global timeline"			// Need Übersetzung
#elif L==3	// en
	"Unshare a publication in global timeline"
#elif L==4	// es
	"Dejar de compartir una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Unshare a publication in global timeline"			// Besoin de traduction
#elif L==6	// gn
	"Dejar de compartir una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Unshare a publication in global timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Unshare a publication in global timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unshare a publication in global timeline"			// Precisa de tradução
#endif
	,
	[ActFavNotGblTL] =
#if   L==1	// ca
	"Favourite a publication in global timeline"				// Necessita traducció
#elif L==2	// de
	"Favourite a publication in global timeline"				// Need Übersetzung
#elif L==3	// en
	"Favourite a publication in global timeline"
#elif L==4	// es
	"Marcar como favorita una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Favourite a publication in global timeline"				// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorita una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Favourite a publication in global timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a publication in global timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a publication in global timeline"				// Precisa de tradução
#endif
	,
	[ActUnfNotGblTL] =
#if   L==1	// ca
	"Unfavourite a publication in global timeline"					// Necessita traducció
#elif L==2	// de
	"Unfavourite a publication in global timeline"					// Need Übersetzung
#elif L==3	// en
	"Unfavourite a publication in global timeline"
#elif L==4	// es
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Unfavourite a publication in global timeline"					// Besoin de traduction
#elif L==6	// gn
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a publication in global timeline"					// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a publication in global timeline"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a publication in global timeline"					// Precisa de tradução
#endif
	,
	[ActFavComGblTL] =
#if   L==1	// ca
	"Favourite a comment in global timeline"		// Necessita traducció
#elif L==2	// de
	"Favourite a comment in global timeline"		// Need Übersetzung
#elif L==3	// en
	"Favourite a comment in global timeline"
#elif L==4	// es
	"Marcar como favorito un comentario en timeline global"
#elif L==5	// fr
	"Favourite a comment in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorito un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Favourite a comment in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a comment in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a comment in global timeline"		// Precisa de tradução
#endif
	,
	[ActUnfComGblTL] =
#if   L==1	// ca
	"Unfavourite a comment in global timeline"			// Necessita traducció
#elif L==2	// de
	"Unfavourite a comment in global timeline"			// Need Übersetzung
#elif L==3	// en
	"Unfavourite a comment in global timeline"
#elif L==4	// es
	"Desmarcar como favorito un comentario en timeline global"
#elif L==5	// fr
	"Unfavourite a comment in global timeline"			// Besoin de traduction
#elif L==6	// gn
	"Desmarcar como favorito un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a comment in global timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a comment in global timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a comment in global timeline"			// Precisa de tradução
#endif
	,
	[ActReqRemPubGblTL] =
#if   L==1	// ca
	"Request deletion of a publication in global timeline"				// Necessita traducció
#elif L==2	// de
	"Request deletion of a publication in global timeline"				// Need Übersetzung
#elif L==3	// en
	"Request deletion of a publication in global timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Request deletion of a publication in global timeline"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a publication in global timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a publication in global timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a publication in global timeline"				// Precisa de tradução
#endif
	,
	[ActRemPubGblTL] =
#if   L==1	// ca
	"Delete a publication in global timeline"		// Necessita traducció
#elif L==2	// de
	"Delete a publication in global timeline"		// Need Übersetzung
#elif L==3	// en
	"Delete a publication in global timeline"
#elif L==4	// es
	"Eliminar una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Delete a publication in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Delete a publication in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Delete a publication in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Delete a publication in global timeline"		// Precisa de tradução
#endif
	,
	[ActReqRemComGblTL] =
#if   L==1	// ca
	"Request deletion of a comment in global timeline"			// Necessita traducció
#elif L==2	// de
	"Request deletion of a comment in global timeline"			// Need Übersetzung
#elif L==3	// en
	"Request deletion of a comment in global timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de un comentario en timeline global"
#elif L==5	// fr
	"Request deletion of a comment in global timeline"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a comment in global timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a comment in global timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a comment in global timeline"			// Precisa de tradução
#endif
	,
	[ActRemComGblTL] =
#if   L==1	// ca
	"Delete a comment in global timeline"		// Necessita traducció
#elif L==2	// de
	"Delete a comment in global timeline"		// Need Übersetzung
#elif L==3	// en
	"Delete a comment in global timeline"
#elif L==4	// es
	"Eliminar un comentario en timeline global"
#elif L==5	// fr
	"Delete a comment in global timeline"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Delete a comment in global timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Delete a comment in global timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Delete a comment in global timeline"		// Precisa de tradução
#endif
	,
	[ActReqOthPubPrf] =
#if   L==1	// ca
	"Request to show public user profile"		// Necessita traducció
#elif L==2	// de
	"Request to show public user profile"		// Need Übersetzung
#elif L==3	// en
	"Request to show public user profile"
#elif L==4	// es
	"Solicitar mostrar perfil de usuario"
#elif L==5	// fr
	"Request to show public user profile"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar mostrar perfil de usuario"		// Okoteve traducción
#elif L==7	// it
	"Request to show public user profile"		// Bisogno di traduzione
#elif L==8	// pl
	"Request to show public user profile"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request to show public user profile"		// Precisa de tradução
#endif
	,
	[ActRefOldPubUsrTL] =
#if   L==1	// ca
	"Show previous publications in user timeline"			// Necessita traducció
#elif L==2	// de
	"Show previous publications in user timeline"			// Need Übersetzung
#elif L==3	// en
	"Show previous publications in user timeline"
#elif L==4	// es
	"Mostrar publicaciones anteriores en timeline de usuario"
#elif L==5	// fr
	"Show previous publications in user timeline"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar publicaciones anteriores en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show previous publications in user timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Show previous publications in user timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show previous publications in user timeline"			// Precisa de tradução
#endif
	,
	[ActRcvPstUsrTL] =
#if   L==1	// ca
	"Create post in user timeline"			// Necessita traducció
#elif L==2	// de
	"Create post in user timeline"			// Need Übersetzung
#elif L==3	// en
	"Create post in user timeline"
#elif L==4	// es
	"Crear post en timeline de usuario"
#elif L==5	// fr
	"Create post in user timeline"			// Besoin de traduction
#elif L==6	// gn
	"Crear post en timeline de usuario"		// Okoteve traducción
#elif L==7	// it
	"Create post in user timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Create post in user timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create post in user timeline"			// Precisa de tradução
#endif
	,
	[ActRcvComUsrTL] =
#if   L==1	// ca
	"Create comment in user timeline"		// Necessita traducció
#elif L==2	// de
	"Create comment in user timeline"		// Need Übersetzung
#elif L==3	// en
	"Create comment in user timeline"
#elif L==4	// es
	"Crear comentario en timeline de usuario"
#elif L==5	// fr
	"Create comment in user timeline"		// Besoin de traduction
#elif L==6	// gn
	"Crear comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Create comment in user timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Create comment in user timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create comment in user timeline"		// Precisa de tradução
#endif
	,
	[ActShoHidComUsrTL] =
#if   L==1	// ca
	"Show hidden comments in user timeline"			// Necessita traducció
#elif L==2	// de
	"Show hidden comments in user timeline"			// Need Übersetzung
#elif L==3	// en
	"Show hidden comments in user timeline"
#elif L==4	// es
	"Mostrar comentarios ocultos en timeline de usuario"
#elif L==5	// fr
	"Show hidden comments in user timeline"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar comentarios ocultos en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show hidden comments in user timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Show hidden comments in user timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show hidden comments in user timeline"			// Precisa de tradução
#endif
	,
	[ActAllShaNotUsrTL] =
#if   L==1	// ca
	"Show who shared a publication in user timeline"					// Necessita traducció
#elif L==2	// de
	"Show who shared a publication in user timeline"					// Need Übersetzung
#elif L==3	// en
	"Show who shared a publication in user timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Show who shared a publication in user timeline"					// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show who shared a publication in user timeline"					// Bisogno di traduzione
#elif L==8	// pl
	"Show who shared a publication in user timeline"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who shared a publication in user timeline"					// Precisa de tradução
#endif
	,
	[ActAllFavNotUsrTL] =
#if   L==1	// ca
	"Show who favourited a publication in user timeline"						// Necessita traducció
#elif L==2	// de
	"Show who favourited a publication in user timeline"						// Need Übersetzung
#elif L==3	// en
	"Show who favourited a publication in user timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Show who favourited a publication in user timeline"						// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a publication in user timeline"						// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a publication in user timeline"						// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a publication in user timeline"						// Precisa de tradução
#endif
	,
	[ActAllFavComUsrTL] =
#if   L==1	// ca
	"Show who favourited a comment in user timeline"					// Necessita traducció
#elif L==2	// de
	"Show who favourited a comment in user timeline"					// Need Übersetzung
#elif L==3	// en
	"Show who favourited a comment in user timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline de usuario"
#elif L==5	// fr
	"Show who favourited a comment in user timeline"					// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a comment in user timeline"					// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a comment in user timeline"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a comment in user timeline"					// Precisa de tradução
#endif
	,
	[ActShaNotUsrTL] =
#if   L==1	// ca
	"Share a publication in user timeline"				// Necessita traducció
#elif L==2	// de
	"Share a publication in user timeline"				// Need Übersetzung
#elif L==3	// en
	"Share a publication in user timeline"
#elif L==4	// es
	"Compartir una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Share a publication in user timeline"				// Besoin de traduction
#elif L==6	// gn
	"Compartir una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Share a publication in user timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Share a publication in user timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Share a publication in user timeline"				// Precisa de tradução
#endif
	,
	[ActUnsNotUsrTL] =
#if   L==1	// ca
	"Unshare a publication in user timeline"				// Necessita traducció
#elif L==2	// de
	"Unshare a publication in user timeline"				// Need Übersetzung
#elif L==3	// en
	"Unshare a publication in user timeline"
#elif L==4	// es
	"Dejar de compartir una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Unshare a publication in user timeline"				// Besoin de traduction
#elif L==6	// gn
	"Dejar de compartir una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Unshare a publication in user timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Unshare a publication in user timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unshare a publication in user timeline"				// Precisa de tradução
#endif
	,
	[ActFavNotUsrTL] =
#if   L==1	// ca
	"Favourite a publication in user timeline"				// Necessita traducció
#elif L==2	// de
	"Favourite a publication in user timeline"				// Need Übersetzung
#elif L==3	// en
	"Favourite a publication in user timeline"
#elif L==4	// es
	"Marcar como favorita una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Favourite a publication in user timeline"				// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorita una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Favourite a publication in user timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a publication in user timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a publication in user timeline"				// Precisa de tradução
#endif
	,
	[ActUnfNotUsrTL] =
#if   L==1	// ca
	"Unfavourite a publication in user timeline"					// Necessita traducció
#elif L==2	// de
	"Unfavourite a publication in user timeline"					// Need Übersetzung
#elif L==3	// en
	"Unfavourite a publication in user timeline"
#elif L==4	// es
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Unfavourite a publication in user timeline"					// Besoin de traduction
#elif L==6	// gn
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a publication in user timeline"					// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a publication in user timeline"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a publication in user timeline"					// Precisa de tradução
#endif
	,
	[ActFavComUsrTL] =
#if   L==1	// ca
	"Favourite a comment in user timeline"				// Necessita traducció
#elif L==2	// de
	"Favourite a comment in user timeline"				// Need Übersetzung
#elif L==3	// en
	"Favourite a comment in user timeline"
#elif L==4	// es
	"Marcar como favorito un comentario en timeline de usuario"
#elif L==5	// fr
	"Favourite a comment in user timeline"				// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorito un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Favourite a comment in user timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a comment in user timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a comment in user timeline"				// Precisa de tradução
#endif
	,
	[ActUnfComUsrTL] =
#if   L==1	// ca
	"Unfavourite a comment in user timeline"			// Necessita traducció
#elif L==2	// de
	"Unfavourite a comment in user timeline"			// Need Übersetzung
#elif L==3	// en
	"Unfavourite a comment in user timeline"
#elif L==4	// es
	"Desmarcar como favorito un comentario en timeline de usuario"
#elif L==5	// fr
	"Unfavourite a comment in user timeline"			// Besoin de traduction
#elif L==6	// gn
	"Desmarcar como favorito un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a comment in user timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a comment in user timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a comment in user timeline"			// Precisa de tradução
#endif
	,
	[ActReqRemPubUsrTL] =
#if   L==1	// ca
	"Request deletion of a publication in user timeline"				// Necessita traducció
#elif L==2	// de
	"Request deletion of a publication in user timeline"				// Need Übersetzung
#elif L==3	// en
	"Request deletion of a publication in user timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Request deletion of a publication in user timeline"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a publication in user timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a publication in user timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a publication in user timeline"				// Precisa de tradução
#endif
	,
	[ActRemPubUsrTL] =
#if   L==1	// ca
	"Delete a publication in user timeline"				// Necessita traducció
#elif L==2	// de
	"Delete a publication in user timeline"				// Need Übersetzung
#elif L==3	// en
	"Delete a publication in user timeline"
#elif L==4	// es
	"Eliminar una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Delete a publication in user timeline"				// Besoin de traduction
#elif L==6	// gn
	"Eliminar una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Delete a publication in user timeline"				// Bisogno di traduzione
#elif L==8	// pl
	"Delete a publication in user timeline"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Delete a publication in user timeline"				// Precisa de tradução
#endif
	,
	[ActReqRemComUsrTL] =
#if   L==1	// ca
	"Request deletion of a comment in user timeline"			// Necessita traducció
#elif L==2	// de
	"Request deletion of a comment in user timeline"			// Need Übersetzung
#elif L==3	// en
	"Request deletion of a comment in user timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de un comentario en timeline de usuario"
#elif L==5	// fr
	"Request deletion of a comment in user timeline"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a comment in user timeline"			// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a comment in user timeline"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a comment in user timeline"			// Precisa de tradução
#endif
	,
	[ActRemComUsrTL] =
#if   L==1	// ca
	"Delete a comment in user timeline"		// Necessita traducció
#elif L==2	// de
	"Delete a comment in user timeline"		// Need Übersetzung
#elif L==3	// en
	"Delete a comment in user timeline"
#elif L==4	// es
	"Eliminar un comentario en timeline de usuario"
#elif L==5	// fr
	"Delete a comment in user timeline"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Delete a comment in user timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Delete a comment in user timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Delete a comment in user timeline"		// Precisa de tradução
#endif
	,
	[ActSeeOthPubPrf] =
#if   L==1	// ca
	"Show user profile"			// Necessita traducció
#elif L==2	// de
	"Show user profile"			// Need Übersetzung
#elif L==3	// en
	"Show user profile"
#elif L==4	// es
	"Mostrar perfil de usuario"
#elif L==5	// fr
	"Show user profile"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar perfil de usuario"		// Okoteve traducción
#elif L==7	// it
	"Show user profile"			// Bisogno di traduzione
#elif L==8	// pl
	"Show user profile"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show user profile"			// Precisa de tradução
#endif
	,
	[ActCalFig] =
#if   L==1	// ca
	"Calculate user figures"		// Necessita traducció
#elif L==2	// de
	"Calculate user figures"		// Need Übersetzung
#elif L==3	// en
	"Calculate user figures"
#elif L==4	// es
	"Calcular cifras de un usuario"
#elif L==5	// fr
	"Calculate user figures"		// Besoin de traduction
#elif L==6	// gn
	"Calcular cifras de un usuario"		// Okoteve traducción
#elif L==7	// it
	"Calculate user figures"		// Bisogno di traduzione
#elif L==8	// pl
	"Calculate user figures"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Calculate user figures"		// Precisa de tradução
#endif
	,
	[ActFolUsr] =
#if   L==1	// ca
	"Follow a user"				// Necessita traducció
#elif L==2	// de
	"Follow a user"				// Need Übersetzung
#elif L==3	// en
	"Follow a user"
#elif L==4	// es
	"Seguir a un usuario"
#elif L==5	// fr
	"Follow a user"				// Besoin de traduction
#elif L==6	// gn
	"Seguir a un usuario"			// Okoteve traducción
#elif L==7	// it
	"Follow a user"				// Bisogno di traduzione
#elif L==8	// pl
	"Follow a user"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Follow a user"				// Precisa de tradução
#endif
	,
	[ActUnfUsr] =
#if   L==1	// ca
	"Unfollow a user"			// Necessita traducció
#elif L==2	// de
	"Unfollow a user"			// Need Übersetzung
#elif L==3	// en
	"Unfollow a user"
#elif L==4	// es
	"Dejar de seguir a un usuario"
#elif L==5	// fr
	"Unfollow a user"			// Besoin de traduction
#elif L==6	// gn
	"Dejar de seguir a un usuario"		// Okoteve traducción
#elif L==7	// it
	"Unfollow a user"			// Bisogno di traduzione
#elif L==8	// pl
	"Unfollow a user"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfollow a user"			// Precisa de tradução
#endif
	,
	[ActSeeFlg] =
#if   L==1	// ca
	"Show followed users"			// Necessita traducció
#elif L==2	// de
	"Show followed users"			// Need Übersetzung
#elif L==3	// en
	"Show followed users"
#elif L==4	// es
	"Mostrar usuarios seguidos"
#elif L==5	// fr
	"Show followed users"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar usuarios seguidos"		// Okoteve traducción
#elif L==7	// it
	"Show followed users"			// Bisogno di traduzione
#elif L==8	// pl
	"Show followed users"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show followed users"			// Precisa de tradução
#endif
	,
	[ActSeeFlr] =
#if   L==1	// ca
	"Show followers"			// Necessita traducció
#elif L==2	// de
	"Show followers"			// Need Übersetzung
#elif L==3	// en
	"Show followers"
#elif L==4	// es
	"Mostrar seguidores"
#elif L==5	// fr
	"Show followers"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar seguidores"			// Okoteve traducción
#elif L==7	// it
	"Show followers"			// Bisogno di traduzione
#elif L==8	// pl
	"Show followers"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show followers"			// Precisa de tradução
#endif
	,
	[ActPrnCal] =
#if   L==1	// ca
	"Print calendar"			// Necessita traducció
#elif L==2	// de
	"Print calendar"			// Need Übersetzung
#elif L==3	// en
	"Print calendar"
#elif L==4	// es
	"Imprimir calendario"
#elif L==5	// fr
	"Print calendar"			// Besoin de traduction
#elif L==6	// gn
	"Imprimir calendario"			// Okoteve traducción
#elif L==7	// it
	"Print calendar"			// Bisogno di traduzione
#elif L==8	// pl
	"Print calendar"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Print calendar"			// Precisa de tradução
#endif
	,
	[ActChgCal1stDay] =
#if   L==1	// ca
	"Change first day of the week in calendar"		// Necessita traducció
#elif L==2	// de
	"Change first day of the week in calendar"		// Need Übersetzung
#elif L==3	// en
	"Change first day of the week in calendar"
#elif L==4	// es
	"Cambiar primer d&iacute;a de la semana en calendario"
#elif L==5	// fr
	"Change first day of the week in calendar"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar primer d&iacute;a de la semana en calendario"	// Okoteve traducción
#elif L==7	// it
	"Change first day of the week in calendar"		// Bisogno di traduzione
#elif L==8	// pl
	"Change first day of the week in calendar"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change first day of the week in calendar"		// Precisa de tradução
#endif
	,
	[ActSeeNewNtf] =
#if   L==1	// ca
	"Show notifications"			// Necessita traducció
#elif L==2	// de
	"Show notifications"			// Need Übersetzung
#elif L==3	// en
	"Show notifications"
#elif L==4	// es
	"Mostrar notificaciones"
#elif L==5	// fr
	"Show notifications"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar notificaciones"		// Okoteve traducción
#elif L==7	// it
	"Show notifications"			// Bisogno di traduzione
#elif L==8	// pl
	"Show notifications"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show notifications"			// Precisa de tradução
#endif
	,
	[ActMrkNtfSee] =
#if   L==1	// ca
	"Mark notifications as seen"		// Necessita traducció
#elif L==2	// de
	"Mark notifications as seen"		// Need Übersetzung
#elif L==3	// en
	"Mark notifications as seen"
#elif L==4	// es
	"Marcar notificaciones como vistas"
#elif L==5	// fr
	"Mark notifications as seen"		// Besoin de traduction
#elif L==6	// gn
	"Marcar notificaciones como vistas"	// Okoteve traducción
#elif L==7	// it
	"Mark notifications as seen"		// Bisogno di traduzione
#elif L==8	// pl
	"Mark notifications as seen"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Mark notifications as seen"		// Precisa de tradução
#endif
	,
	[ActSeeMai] =
#if   L==1	// ca
	"Show mail domains"			// Necessita traducció
#elif L==2	// de
	"Show mail domains"			// Need Übersetzung
#elif L==3	// en
	"Show mail domains"
#elif L==4	// es
	"Mostrar dominios de correo"
#elif L==5	// fr
	"Show mail domains"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar dominios de correo"		// Okoteve traducción
#elif L==7	// it
	"Show mail domains"			// Bisogno di traduzione
#elif L==8	// pl
	"Show mail domains"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show mail domains"			// Precisa de tradução
#endif
	,
	[ActEdiMai] =
#if   L==1	// ca
	"Edit mail domains"			// Necessita traducció
#elif L==2	// de
	"Edit mail domains"			// Need Übersetzung
#elif L==3	// en
	"Edit mail domains"
#elif L==4	// es
	"Editar dominios de correo"
#elif L==5	// fr
	"Edit mail domains"			// Besoin de traduction
#elif L==6	// gn
	"Editar dominios de correo"		// Okoteve traducción
#elif L==7	// it
	"Edit mail domains"			// Bisogno di traduzione
#elif L==8	// pl
	"Edit mail domains"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit mail domains"			// Precisa de tradução
#endif
	,
	[ActNewMai] =
#if   L==1	// ca
	"Create email domain"			// Necessita traducció
#elif L==2	// de
	"Create email domain"			// Need Übersetzung
#elif L==3	// en
	"Create email domain"
#elif L==4	// es
	"Crear dominio de correo"
#elif L==5	// fr
	"Create email domain"			// Besoin de traduction
#elif L==6	// gn
	"Crear dominio de correo"		// Okoteve traducción
#elif L==7	// it
	"Create email domain"			// Bisogno di traduzione
#elif L==8	// pl
	"Create email domain"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create email domain"			// Precisa de tradução
#endif
	,
	[ActRemMai] =
#if   L==1	// ca
	"Remove email domain"			// Necessita traducció
#elif L==2	// de
	"Remove email domain"			// Need Übersetzung
#elif L==3	// en
	"Remove email domain"
#elif L==4	// es
	"Eliminar dominio de correo"
#elif L==5	// fr
	"Remove email domain"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar dominio de correo"		// Okoteve traducción
#elif L==7	// it
	"Remove email domain"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove email domain"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove email domain"			// Precisa de tradução
#endif
	,
	[ActRenMaiSho] =
#if   L==1	// ca
	"Change email domain"			// Necessita traducció
#elif L==2	// de
	"Change email domain"			// Need Übersetzung
#elif L==3	// en
	"Change email domain"
#elif L==4	// es
	"Cambiar dominio de correo"
#elif L==5	// fr
	"Change email domain"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar dominio de correo"		// Okoteve traducción
#elif L==7	// it
	"Change email domain"			// Bisogno di traduzione
#elif L==8	// pl
	"Change email domain"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change email domain"			// Precisa de tradução
#endif
	,
	[ActRenMaiFul] =
#if   L==1	// ca
	"Change email domain information"			// Necessita traducció
#elif L==2	// de
	"Change email domain information"			// Need Übersetzung
#elif L==3	// en
	"Change email domain information"
#elif L==4	// es
	"Cambiar informaci&oacute;n de dominio de correo"
#elif L==5	// fr
	"Change email domain information"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar informaci&oacute;n de dominio de correo"	// Okoteve traducción
#elif L==7	// it
	"Change email domain information"			// Bisogno di traduzione
#elif L==8	// pl
	"Change email domain information"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change email domain information"			// Precisa de tradução
#endif
	,
	[ActSeeSysInf] =
#if   L==1	// ca
	"Show information on the platform"			// Necessita traducció
#elif L==2	// de
	"Show information on the platform"			// Need Übersetzung
#elif L==3	// en
	"Show information on the platform"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre la plataforma"
#elif L==5	// fr
	"Show information on the platform"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar informaci&oacute;n sobre la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Show information on the platform"			// Bisogno di traduzione
#elif L==8	// pl
	"Show information on the platform"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show information on the platform"			// Precisa de tradução
#endif
	,
	[ActSeeCty] =
#if   L==1	// ca
	"List countries"	// Necessita traducció
#elif L==2	// de
	"List countries"	// Need Übersetzung
#elif L==3	// en
	"List countries"
#elif L==4	// es
	"Listar pa&iacute;ses"
#elif L==5	// fr
	"List countries"	// Besoin de traduction
#elif L==6	// gn
	"Listar pa&iacute;ses"	// Okoteve traducción
#elif L==7	// it
	"List countries"	// Bisogno di traduzione
#elif L==8	// pl
	"List countries"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List countries"	// Precisa de tradução
#endif
	,
	[ActSeePen] =
#if   L==1	// ca
	"Institutions, centers, titulacions i assignatures pendents d'activar"
#elif L==2	// de
	"Anh&auml;ngig Hochschulen, Lehrinstitute, Studiengang und Kursen"
#elif L==3	// en
	"Pending institutions, centers, degrees and courses"
#elif L==4	// es
	"Instituciones, centros, titulaciones y asignaturas pendientes de activar"
#elif L==5	// fr
	"&Eacute;tablissements, centers, &eacute;tudes et mati&egrave;res en attente"
#elif L==6	// gn
	"Instituciones, centros, titulaciones y asignaturas pendientes de activar"	// Okoteve traducción
#elif L==7	// it
	"Istituzioni, centri, lauree e corsi in attesa di essere attivati"
#elif L==8	// pl
	"Instytucje, centra, stopnie i kurs&otilde;w oczekuj&aogon;cy"
#elif L==9	// pt
	"Institu&ccedil;&otilde;es, centros, graus e disciplinas pendentes"
#endif
	,
	[ActSeeLnk] =
#if   L==1	// ca
	"See institutional links"		// Necessita traducció
#elif L==2	// de
	"See institutional links"		// Need Übersetzung
#elif L==3	// en
	"See institutional links"
#elif L==4	// es
	"Ver enlaces institucionales"
#elif L==5	// fr
	"See institutional links"		// Besoin de traduction
#elif L==6	// gn
	"Ver enlaces institucionales"		// Okoteve traducción
#elif L==7	// it
	"See institutional links"		// Bisogno di traduzione
#elif L==8	// pl
	"See institutional links"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See institutional links"		// Precisa de tradução
#endif
	,
	[ActLstPlg] =
#if   L==1	// ca
	"List plugins"			// Necessita traducció
#elif L==2	// de
	"List plugins"			// Need Übersetzung
#elif L==3	// en
	"List plugins"
#elif L==4	// es
	"Listar complementos"
#elif L==5	// fr
	"List plugins"			// Besoin de traduction
#elif L==6	// gn
	"Listar complementos"		// Okoteve traducción
#elif L==7	// it
	"List plugins"			// Bisogno di traduzione
#elif L==8	// pl
	"List plugins"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List plugins"			// Precisa de tradução
#endif
	,
	[ActMtn] =
#if   L==1	// ca
	"Manteniment"
#elif L==2	// de
	"Erhaltung"
#elif L==3	// en
	"Maintenance"
#elif L==4	// es
	"Mantenimiento"
#elif L==5	// fr
	"Entretien"
#elif L==6	// gn
	"&Ntilde;emyatyr&otilde;"
#elif L==7	// it
	"Manutenzione"
#elif L==8	// pl
	"Utrzymanie"
#elif L==9	// pt
	"Manuten&ccedil;&atilde;o"
#endif
	,
	[ActPrnSysInf] =
#if   L==1	// ca
	"Print information on the platform"			// Necessita traducció
#elif L==2	// de
	"Print information on the platform"			// Need Übersetzung
#elif L==3	// en
	"Print information on the platform"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre la plataforma"
#elif L==5	// fr
	"Print information on the platform"			// Besoin de traduction
#elif L==6	// gn
	"Imprimir informaci&oacute;n sobre la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Print information on the platform"			// Bisogno di traduzione
#elif L==8	// pl
	"Print information on the platform"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Print information on the platform"			// Precisa de tradução
#endif
	,
	[ActEdiCty] =
#if   L==1	// ca
	"Edit countries"		// Necessita traducció
#elif L==2	// de
	"Edit countries"		// Need Übersetzung
#elif L==3	// en
	"Edit countries"
#elif L==4	// es
	"Editar pa&iacute;ses"
#elif L==5	// fr
	"Edit countries"		// Besoin de traduction
#elif L==6	// gn
	"Editar pa&iacute;ses"		// Okoteve traducción
#elif L==7	// it
	"Edit countries"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit countries"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit countries"		// Precisa de tradução
#endif
	,
	[ActNewCty] =
#if   L==1	// ca
	"Request the creation of a country"			// Necessita traducció
#elif L==2	// de
	"Request the creation of a country"			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a country"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un pa&iacute;s"
#elif L==5	// fr
	"Request the creation of a country"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un pa&iacute;s"	// Okoteve traducción
#elif L==7	// it
	"Request the creation of a country"			// Bisogno di traduzione
#elif L==8	// pl
	"Request the creation of a country"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the creation of a country"			// Precisa de tradução
#endif
	,
	[ActRemCty] =
#if   L==1	// ca
	"Remove a country"		// Necessita traducció
#elif L==2	// de
	"Remove a country"		// Need Übersetzung
#elif L==3	// en
	"Remove a country"
#elif L==4	// es
	"Eliminar un pa&iacute;s"
#elif L==5	// fr
	"Remove a country"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un pa&iacute;s"	// Okoteve traducción
#elif L==7	// it
	"Remove a country"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a country"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a country"		// Precisa de tradução
#endif
	,
	[ActRenCty] =
#if   L==1	// ca
	"Change the name of a country"		// Necessita traducció
#elif L==2	// de
	"Change the name of a country"		// Need Übersetzung
#elif L==3	// en
	"Change the name of a country"
#elif L==4	// es
	"Cambiar el nombre de un pa&iacute;s"
#elif L==5	// fr
	"Change the name of a country"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre de un pa&iacute;s"	// Okoteve traducción
#elif L==7	// it
	"Change the name of a country"		// Bisogno di traduzione
#elif L==8	// pl
	"Change the name of a country"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the name of a country"		// Precisa de tradução
#endif
	,
	[ActChgCtyWWW] =
#if   L==1	// ca
	"Change the web of country"		// Necessita traducció
#elif L==2	// de
	"Change the web of country"		// Need Übersetzung
#elif L==3	// en
	"Change the web of country"
#elif L==4	// es
	"Cambiar la web de un pa&iacute;s"
#elif L==5	// fr
	"Change the web of country"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de un pa&iacute;s"	// Okoteve traducción
#elif L==7	// it
	"Change the web of country"		// Bisogno di traduzione
#elif L==8	// pl
	"Change the web of country"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the web of country"		// Precisa de tradução
#endif
	,
	[ActSeeBan] =
#if   L==1	// ca
	"See banners"		// Necessita traducció
#elif L==2	// de
	"See banners"		// Need Übersetzung
#elif L==3	// en
	"See banners"
#elif L==4	// es
	"Ver banners"
#elif L==5	// fr
	"See banners"		// Besoin de traduction
#elif L==6	// gn
	"Ver banners"		// Okoteve traducción
#elif L==7	// it
	"See banners"		// Bisogno di traduzione
#elif L==8	// pl
	"See banners"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See banners"		// Precisa de tradução
#endif
	,
	[ActEdiBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit banners"
#elif L==4	// es
	"Editar banners"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar banners"			// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a banner"
#elif L==4	// es
	"Solicitar la creaci&ocute;n de un banner"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&ocute;n de un banner"			// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a banner"
#elif L==4	// es
	"Solicitar la eliminaci&ocute;n de un banner"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&ocute;n de un banner"			// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show a hidden banner"
#elif L==4	// es
	"Mostrar un banner oculto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar un banner oculto"			// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a visible banner"
#elif L==4	// es
	"Ocultar un banner visible"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar un banner visible"			// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenBanSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the short name of a banner"
#elif L==4	// es
	"Solicitar el cambio del nombre breve de un banner"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio del nombre breve de un banner"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenBanFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the full name of a banner"
#elif L==4	// es
	"Solicitar el cambio del nombre completo de un banner"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio del nombre completo de un banner"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgBanImg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the image of a banner"
#elif L==4	// es
	"Solicitar el cambio de la imagen de un banner"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio de la imagen de un banner"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgBanWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the web of a banner"
#elif L==4	// es
	"Solicitar el cambio de la web de un banner"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio de la web de un banner"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActClkBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Go to a banner when clicked"
#elif L==4	// es
	"Ir a un banner al pulsar"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ir a un banner al pulsar"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit institutional links"
#elif L==4	// es
	"Editar enlaces institucionales"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar enlaces institucionales"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of an institutional link"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un enlace institucional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un enlace institucional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of an institutional link"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un enlace institucional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un enlace institucional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenLnkSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the short name of an institutional link"
#elif L==4	// es
	"Solicitar el cambio del nombre breve de un enlace institucional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio del nombre breve de un enlace institucional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenLnkFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the full name of an institutional link"
#elif L==4	// es
	"Solicitar el cambio del nombre completo de un enlace institucional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio del nombre completo de un enlace institucional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgLnkWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the web of an institutional link"
#elif L==4	// es
	"Solicitar el cambio de la web de un enlace institucional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio de la web de un enlace institucional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit plugins"
#elif L==4	// es
	"Editar complementos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar complementos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a plugin"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a plugin"
#elif L==4	// es
	"Eliminar un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a plugin"
#elif L==4	// es
	"Renombrar un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgPlgDes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the description of a plugin"
#elif L==4	// es
	"Cambiar la descripci&ocute;n de un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la descripci&ocute;n de un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgPlgLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the logo of a plugin"
#elif L==4	// es
	"Cambiar el logo de un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el logo de un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgPlgAppKey] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the application key of a plugin"
#elif L==4	// es
	"Cambiar la clave de aplicaci&oacute;n de un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la clave de aplicaci&oacute;n de un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgPlgURL] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the URL of a plugin"
#elif L==4	// es
	"Cambiar la URL de un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la URL de un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgPlgIP] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the IP address of a plugin"
#elif L==4	// es
	"Cambiar la direcci&oacute;n IP de un complemento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la direcci&oacute;n IP de un complemento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSetUp] =
#if   L==1	// ca
	"Instal lar"
#elif L==2	// de
	"Einrichten"
#elif L==3	// en
	"Set up"
#elif L==4	// es
	"Instalar"
#elif L==5	// fr
	"Installer"
#elif L==6	// gn
	"Instalar"	// Okoteve traducción
#elif L==7	// it
	"Setup"
#elif L==8	// pl
	"Set up"
#elif L==9	// pt
	"Instalar"
#endif
	,
	[ActReqRemOldCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the total removal of old courses"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de asignaturas antiguas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de asignaturas antiguas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemOldCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove completely old courses"
#elif L==4	// es
	"Eliminar completamente asignaturas antiguas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar completamente asignaturas antiguas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeCtyInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show information on the country"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre el pa&iacute;s"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar informaci&oacute;n sobre el pa&iacute;s"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List institutions"
#elif L==4	// es
	"Listar instituciones"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar instituciones"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnCtyInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print information on the country"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre el pa&iacute;s"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Imprimir informaci&oacute;n sobre el pa&iacute;s"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtyMapAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the map attribution of a country"
#elif L==4	// es
	"Cambiar la atribuci&oacute;n del mapa de un pa&iacute;s"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la atribuci&oacute;n del mapa de un pa&iacute;s"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit institutions"
#elif L==4	// es
	"Editar instituciones"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar instituciones"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of an institution"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create an institution"
#elif L==4	// es
	"Crear una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove institution"
#elif L==4	// es
	"Eliminar instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenInsSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of an institution"
#elif L==4	// es
	"Cambiar el nombre breve de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenInsFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of an institution"
#elif L==4	// es
	"Cambiar el nombre completo de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgInsWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the web of an institution"
#elif L==4	// es
	"Cambiar la web de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgInsSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the status of an institution"
#elif L==4	// es
	"Cambiar el estado de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el estado de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeInsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show information on the institution"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre la instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	""			// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List centers"
#elif L==4	// es
	"Listar centros"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar centros"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeePlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List places"
#elif L==4	// es
	"Listar lugares"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar lugares"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List departments"
#elif L==4	// es
	"Listar departamentos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar departamentos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List holidays"
#elif L==4	// es
	"Listar festivos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar festivos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnInsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print information on the institution"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre la instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Imprimir informaci&oacute;n sobre la instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgInsCtyCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the country of an institution in its configuration"
#elif L==4	// es
	"Cambiar el pa&iacute;s de una instituci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el pa&iacute;s de una instituci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenInsShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of an institution in its configuration"
#elif L==4	// es
	"Cambiar el nombre breve de una instituci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de una instituci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenInsFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of an institution in its configuration"
#elif L==4	// es
	"Cambiar el nombre completo de una instituci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de una instituci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgInsWWWCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the web of an institution in its configuration"
#elif L==4	// es
	"Cambiar la web de una instituci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de una instituci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqInsLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the logo of the current institution"
#elif L==4	// es
	"Solicitar el logo de una instituci&oacute;n actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el logo de una instituci&oacute;n actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRecInsLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the logo of the current institution"
#elif L==4	// es
	"Cambiar el logo de una instituci&oacute;n actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el logo de una instituci&oacute;n actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemInsLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove the logo of the current institution"
#elif L==4	// es
	"Eliminar el logo de una instituci&oacute;n actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar el logo de una instituci&oacute;n actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit centers"
#elif L==4	// es
	"Editar centros"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar centros"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a center"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a center"
#elif L==4	// es
	"Crear un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a center"
#elif L==4	// es
	"Eliminar un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the place of a center"
#elif L==4	// es
	"Cambiar el lugar de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el lugar de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCtrSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of a center"
#elif L==4	// es
	"Cambiar el nombre breve de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCtrFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of a center"
#elif L==4	// es
	"Cambiar el nombre completo de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the web of a center"
#elif L==4	// es
	"Cambiar la web de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the status of a center"
#elif L==4	// es
	"Cambiar el estado de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el estado de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit places"
#elif L==4	// es
	"Editar lugares"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar lugares"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a place"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un lugar"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un lugar"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a place"
#elif L==4	// es
	"Eliminar un lugar"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un lugar"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenPlcSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of a place"
#elif L==4	// es
	"Cambiar el nombre breve de un lugar"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de un lugar"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenPlcFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of a place"
#elif L==4	// es
	"Cambiar el nombre completo de un lugar"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de un lugar"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit departments"
#elif L==4	// es
	"Editar departamentos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar departamentos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a department"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un departamento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un departamento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a department"
#elif L==4	// es
	"Eliminar un departamento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un departamento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDptIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the institution of a department"
#elif L==4	// es
	"Cambiar la instituci&oacute;n de un departamento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la instituci&oacute;n de un departamento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenDptSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of a department"
#elif L==4	// es
	"Cambiar el nombre breve de un departamento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de un departamento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenDptFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of a department"
#elif L==4	// es
	"Cambiar el nombre completo de un departamento"
#elif L==4	// es
	""
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de un departamento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDptWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the web of a department"
#elif L==4	// es
	"Cambiar la web de un departamento"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de un departamento"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit holidays"
#elif L==4	// es
	"Editar festivos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar festivos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a holiday"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un festivo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un festivo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a holiday"
#elif L==4	// es
	"Eliminar un festivo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un festivo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHldPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change place of a holiday"
#elif L==4	// es
	"Cambiar el lugar de un festivo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el lugar de un festivo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHldTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the type of a holiday"
#elif L==4	// es
	"Cambiar el tipo de un festivo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el tipo de un festivo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHldStrDat] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the start date of a holiday"
#elif L==4	// es
	"Cambiar la fecha inicial de un festivo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la fecha inicial de un festivo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHldEndDat] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the end date of a holiday"
#elif L==4	// es
	"Cambiar la fecha final de un festivo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la fecha final de un festivo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change name of a holiday"
#elif L==4	// es
	"Cambiar el nombre de un festivo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre de un festivo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeCtrInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show information on the center"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre el centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar informaci&oacute;n sobre el centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List degrees in a center"
#elif L==4	// es
	"Listar titulaciones de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar titulaciones de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeBld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List buildings in a center"
#elif L==4	// es
	"Listar edificios de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar edificios de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeRoo] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List rooms in a center"
#elif L==4	// es
	"Listar salas de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar salas de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnCtrInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print information on the center"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre el centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Imprimir informaci&oacute;n sobre el centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrInsCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the institution of a center in its configuration"
#elif L==4	// es
	"Cambiar la instituci&oacute;n de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la instituci&oacute;n de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCtrShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name center in its configuration"
#elif L==4	// es
	"Cambiar el nombre breve de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCtrFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name center in its configuration"
#elif L==4	// es
	"Cambiar el nombre completo de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrPlcCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the place of a center in its configuration"
#elif L==4	// es
	"Cambiar el lugar de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el lugar de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrLatCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the latitude of a center in its configuration"
#elif L==4	// es
	"Cambiar la latitud de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la latitud de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrLgtCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the longitude of a center in its configuration"
#elif L==4	// es
	"Cambiar la longitud de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la longitud de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrAltCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the altitude of a center in its configuration"
#elif L==4	// es
	"Cambiar la altitud de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la altitud de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrWWWCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the web of a center in its configuration"
#elif L==4	// es
	"Cambiar la web de un centro en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de un centro en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqCtrLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the logo of the current center"
#elif L==4	// es
	"Solicitar el cambio de logo de un centro actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio de logo de un centro actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRecCtrLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the logo of the current center"
#elif L==4	// es
	"Cambiar el logo de un centro actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el logo de un centro actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemCtrLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove the logo of the current center"
#elif L==4	// es
	"Eliminar el logo de un centro actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar el logo de un centro actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqCtrPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the photo of the current center"
#elif L==4	// es
	"Solicitar el cambio de la foto de un centro actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio de la foto de un centro actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRecCtrPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the photo of the current center"
#elif L==4	// es
	"Cambiar la foto de un centro actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la foto de un centro actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCtrPhoAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the attribution of a center photo"
#elif L==4	// es
	"Cambiar la atribuci&oacute;n de la foto de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la atribuci&oacute;n de la foto de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List types of degrees"
#elif L==4	// es
	"Listar tipos de titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar tipos de titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the edition of the types of degrees"
#elif L==4	// es
	"Solicitar la edici&oacute;n de los tipos de titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la edici&oacute;n de los tipos de titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a type of degree"
#elif L==4	// es
	"Crear un tipo de titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un tipo de titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a type of degree"
#elif L==4	// es
	"Eliminar un tipo de titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un tipo de titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a type of degree"
#elif L==4	// es
	"Renombrar un tipo de titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar un tipo de titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit degrees"
#elif L==4	// es
	"Editar titulaciones"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar titulaciones"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a degree"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a degree"
#elif L==4	// es
	"Crear una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a degree"
#elif L==4	// es
	"Eliminar una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenDegSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of a degree"
#elif L==4	// es
	"Cambiar el nombre breve de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenDegFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of a degree"
#elif L==4	// es
	"Cambiar el nombre completo de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the type of a degree"
#elif L==4	// es
	"Cambiar el tipo de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el tipo de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDegWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the web of a degree"
#elif L==4	// es
	"Cambiar la web de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDegSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the status of a degree"
#elif L==4	// es
	"Cambiar el estado de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el estado de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiBld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit buildings"
#elif L==4	// es
	"Editar edificios"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar edificios"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewBld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a building"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un edificio"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un edificio"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemBld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a building"
#elif L==4	// es
	"Eliminar edificio"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar edificio"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenBldSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name of a building"
#elif L==4	// es
	"Cambiar nombre breve de edificio"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar nombre breve de edificio"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenBldFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name of a building"
#elif L==4	// es
	"Cambiar nombre completo de edificio"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar nombre completo de edificio"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenBldLoc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change location of a building"
#elif L==4	// es
	"Cambiar ubicaci&oacute;n de un edificio"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar ubicaci&oacute;n de un edificio"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiRoo] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit rooms"
#elif L==4	// es
	"Editar salas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar salas"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewRoo] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a room"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de una sala"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de una sala"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemRoo] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a room"
#elif L==4	// es
	"Eliminar sala"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar sala"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgRooBld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change building of a room"
#elif L==4	// es
	"Cambiar edificio de una sala"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar edificio de una sala"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgRooMaxUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change number of users of a room"
#elif L==4	// es
	"Cambiar n&uacute;mero de usuarios de una sala"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar n&uacute;mero de usuarios de una sala"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenRooSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name of a room"
#elif L==4	// es
	"Cambiar nombre breve de sala"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar nombre breve de sala"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenRooFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name of a room"
#elif L==4	// es
	"Cambiar nombre completo de sala"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar nombre completo de sala"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgRooFlo] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change floor of a room"
#elif L==4	// es
	"Cambiar planta de una sala"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar planta de una sala"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDegInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show information on the degree"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre la titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar informaci&oacute;n sobre la titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List courses of a degree"
#elif L==4	// es
	"Listar asignaturas de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar asignaturas de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnDegInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print information on the degree"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre la titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Imprimir informaci&oacute;n sobre la titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDegCtrCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the center of a degree in its configuration"
#elif L==4	// es
	"Cambiar el centro de una titulaci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el centro de una titulaci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenDegShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of a degree in its configuration"
#elif L==4	// es
	"Cambiar el nombre breve de una titulaci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de una titulaci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenDegFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of a degree in its configuration"
#elif L==4	// es
	"Cambiar el nombre completo de una titulaci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de una titulaci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDegWWWCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the web of a degree in its configuration"
#elif L==4	// es
	"Cambiar la web de una titulaci&oacute;n en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la web de una titulaci&oacute;n en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDegLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the logo of the current degree"
#elif L==4	// es
	"Solicitar el cambio del logo de la titulaci&oacute;n actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el cambio del logo de la titulaci&oacute;n actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRecDegLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the logo of the current degree"
#elif L==4	// es
	"Cambiar el logo de la titulaci&oacute;n actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el logo de la titulaci&oacute;n actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemDegLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove the logo of the current degree"
#elif L==4	// es
	"Eliminar el logo de la titulaci&oacute;n actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar el logo de la titulaci&oacute;n actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit courses"
#elif L==4	// es
	"Editar asignaturas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar asignaturas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a course"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a course"
#elif L==4	// es
	"Crear una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a course"
#elif L==4	// es
	"Eliminar una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgInsCrsCod] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the institutional code of a course"
#elif L==4	// es
	"Cambiar el c&oacute;digo institucional de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el c&oacute;digo institucional de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCrsYea] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the year/semester of a course"
#elif L==4	// es
	"Cambiar el a&ntilde;o/semestre de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el a&ntilde;o/semestre de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCrsSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of a course"
#elif L==4	// es
	"Cambiar el nombre breve de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCrsFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of a course"
#elif L==4	// es
	"Cambiar el nombre completo de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCrsSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the status of a course"
#elif L==4	// es
	"Cambiar el estado de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el estado de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show information on the course"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar informaci&oacute;n sobre la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeePrg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See educational schedule of a course"
#elif L==4	// es
	"Ver programaci&oacute;n did&aacute;ctica de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver programaci&oacute;n did&aacute;ctica de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmNewPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request new program item"
#elif L==4	// es
	"Solicitar nuevo &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar nuevo &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmChgPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit program item"
#elif L==4	// es
	"Editar &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new program item"
#elif L==4	// es
	"Crear nuevo &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear nuevo &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change program item"
#elif L==4	// es
	"Cambiar &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of program item"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove of program item"
#elif L==4	// es
	"Eliminar &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide program item"
#elif L==4	// es
	"Ocultar &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show program item"
#elif L==4	// es
	"Mostrar &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUp_PrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move up program item"
#elif L==4	// es
	"Subir &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Subir &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDwnPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move down program item"
#elif L==4	// es
	"Bajar &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Bajar &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLftPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move left program item"
#elif L==4	// es
	"Mover a la izquierda &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mover a la izquierda &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRgtPrgItm] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move right program item"
#elif L==4	// es
	"Mover a la derecha &iacute;tem del programa"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mover a la derecha &iacute;tem del programa"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the teaching guide of a course"
#elif L==4	// es
	"Ver la gu&iacute;a docente de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver la gu&iacute;a docente de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeSyl] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the syllabus (lectures or practicals)"
#elif L==4	// es
	"Ver el temario (teor&iacute;a o pr&aacute;cticas)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver el temario (teor&iacute;a o pr&aacute;cticas)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the bibliography"
#elif L==4	// es
	"Ver la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the FAQ"
#elif L==4	// es
	"Ver la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver la FAQ"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the links related to the course"
#elif L==4	// es
	"Ver los enlaces relacionados con la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los enlaces relacionados con la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the course timetable"
#elif L==4	// es
	"Ver el horario de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver el horario de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print information on the course"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Imprimir informaci&oacute;n sobre la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCrsDegCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the degree of a course in its configuration"
#elif L==4	// es
	"Cambiar la titulaci&oacute;n de una asignatura en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la titulaci&oacute;n de una asignatura en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCrsShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the short name of a course in its configuration"
#elif L==4	// es
	"Cambiar el nombre breve de una asignatura en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre breve de una asignatura en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenCrsFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the full name of a course in its configuration"
#elif L==4	// es
	"Cambiar el nombre completo de una asignatura en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el nombre completo de una asignatura en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgInsCrsCodCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the institutional code of a course in its configuration"
#elif L==4	// es
	"Cambiar el c&oacute;digo institucional de una asignatura en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el c&oacute;digo institucional de una asignatura en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCrsYeaCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the year/semester of a course in its configuration"
#elif L==4	// es
	"Cambiar el a&ntilde;o/semestre de una asignatura en su configuraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el a&ntilde;o/semestre de una asignatura en su configuraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the information about the course"
#elif L==4	// es
	"Editar la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the teaching guide of a course"
#elif L==4	// es
	"Editar la gu&iacute;a docente de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar la gu&iacute;a docente de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the lectures syllabus"
#elif L==4	// es
	"Ver el temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver el temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the practicals syllabus"
#elif L==4	// es
	"Ver el temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver el temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the syllabus of lectures"
#elif L==4	// es
	"Editar el temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar el temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the practicals syllabus"
#elif L==4	// es
	"Editar el temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar el temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDelItmSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an item from syllabus of lectures"
#elif L==4	// es
	"Eliminar un &iacute;tem del temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un &iacute;tem del temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDelItmSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an item from practicals syllabus"
#elif L==4	// es
	"Eliminar un &iacute;tem del temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un &iacute;tem del temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUp_IteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Raise the position of a subtree of the lectures syllabus"
#elif L==4	// es
	"Subir la posici&oacute;n de un sub&aacute;rbol del temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Subir la posici&oacute;n de un sub&aacute;rbol del temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUp_IteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Raise the position of a subtree of the practicals syllabus"
#elif L==4	// es
	"Subir la posici&oacute;n de un sub&aacute;rbol del temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Subir la posici&oacute;n de un sub&aacute;rbol del temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDwnIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Lower the position of a subtree of the lectures syllabus"
#elif L==4	// es
	"Bajar la posici&oacute;n de un sub&aacute;rbol del temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Bajar la posici&oacute;n de un sub&aacute;rbol del temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDwnIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Lower the position of a subtree of the practicals syllabus"
#elif L==4	// es
	"Bajar la posici&oacute;n de un sub&aacute;rbol del temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Bajar la posici&oacute;n de un sub&aacute;rbol del temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRgtIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Increase the level of an item on the lectures syllabus"
#elif L==4	// es
	"Aumentar el nivel de un &iacute;tem del temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Aumentar el nivel de un &iacute;tem del temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRgtIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Increase the level of an item on the practicals syllabus"
#elif L==4	// es
	"Aumentar el nivel de un &iacute;tem del temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Aumentar el nivel de un &iacute;tem del temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLftIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Decrease the level of an item on the lectures syllabus"
#elif L==4	// es
	"Disminuir el nivel de un &iacute;tem del temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Disminuir el nivel de un &iacute;tem del temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLftIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Decrease the level of an item on the practicals syllabus"
#elif L==4	// es
	"Disminuir el nivel de un &iacute;tem del temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Disminuir el nivel de un &iacute;tem del temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActInsIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Insert a new item in the lectures syllabus"
#elif L==4	// es
	"Insertar un nuevo &iacute;tem en el temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Insertar un nuevo &iacute;tem en el temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActInsIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Insert a new item in the practicals syllabus"
#elif L==4	// es
	"Insertar un nuevo &iacute;tem en el temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Insertar un nuevo &iacute;tem en el temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActModIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify an item in the lectures syllabus"
#elif L==4	// es
	"Modificar un &iacute;tem del temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Modificar un &iacute;tem del temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActModIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify an item in the practicals syllabus"
#elif L==4	// es
	"Modificar un &iacute;tem del temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Modificar un &iacute;tem del temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the bibliography"
#elif L==4	// es
	"Editar la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the FAQ"
#elif L==4	// es
	"Editar la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar la FAQ"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the course links"
#elif L==4	// es
	"Editar los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether students are forced to read the course information"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether students are forced to read the teaching guide"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether students are forced to read the lectures syllabus"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer el temario de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer el temario de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether students are forced to read the practicals syllabus"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer el temario de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer el temario de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether students are forced to read the bibliography"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read FAQ"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether students are forced to read the links"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer los enlaces"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer los enlaces"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the course information"
#elif L==4	// es
	"Cambiar si he le&iacute;do la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the teaching guide"
#elif L==4	// es
	"Cambiar si he le&iacute;do la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the lectures syllabus"
#elif L==4	// es
	"Cambiar si he le&iacute;do el programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do el programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the practicals syllabus"
#elif L==4	// es
	"Cambiar si he le&iacute;do el programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do el programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the bibliography"
#elif L==4	// es
	"Cambiar si he le&iacute;do la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the FAQ"
#elif L==4	// es
	"Cambiar si he le&iacute;do la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the course links"
#elif L==4	// es
	"Cambiar si he le&iacute;do los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the course information"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the teaching guide"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the lectures syllabus"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en el programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en el programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the practicals syllabus"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en el programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en el programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the bibliography"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the FAQ"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the course links"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the course information"
#elif L==4	// es
	"Crear una URL en la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the teaching guide"
#elif L==4	// es
	"Crear una URL en la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the lectures syllabus"
#elif L==4	// es
	"Crear una URL en el programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en el programa de teor&iacute;a"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the practicals syllabus"
#elif L==4	// es
	"Crear una URL en el programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en el programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the bibliography"
#elif L==4	// es
	"Crear una URL en la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the FAQ"
#elif L==4	// es
	"Crear una URL en la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the course links"
#elif L==4	// es
	"Crear una URL en los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the course information"
#elif L==4	// es
	"Crear una p&aacute;gina web en la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the teaching guide"
#elif L==4	// es
	"Crear una p&aacute;gina web en la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the lectures syllabus"
#elif L==4	// es
	"Crear una p&aacute;gina web en el programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en el programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the practicals syllabus"
#elif L==4	// es
	"Crear una p&aacute;gina web en el programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en el programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the bibliography"
#elif L==4	// es
	"Crear una p&aacute;gina web en la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the FAQ"
#elif L==4	// es
	"Crear una p&aacute;gina web en la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the course links"
#elif L==4	// es
	"Crear una p&aacute;gina web en los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of a course information"
#elif L==4	// es
	"Editor integrado de la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado de la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the teaching guide"
#elif L==4	// es
	"Editor integrado de la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado de la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the lectures syllabus"
#elif L==4	// es
	"Editor integrado del programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado del programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the practicals syllabus"
#elif L==4	// es
	"Editor integrado del programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado del programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the bibliography"
#elif L==4	// es
	"Editor integrado de la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado de la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the FAQ"
#elif L==4	// es
	"Editor integrado de la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado de la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of a course links"
#elif L==4	// es
	"Editor integrado de los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado de los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of a course information"
#elif L==4	// es
	"Editor de texto plano de la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano de la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of the teaching guide"
#elif L==4	// es
	"Editor de texto plano de la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano de la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of the lectures syllabus"
#elif L==4	// es
	"Editor de texto plano del programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano del programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of the practicals syllabus"
#elif L==4	// es
	"Editor de texto plano del programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano del programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of the bibliography"
#elif L==4	// es
	"Editor de texto plano de la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano de la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of the FAQ"
#elif L==4	// es
	"Editor de texto plano de la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano de la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of a course links"
#elif L==4	// es
	"Editor de texto plano de los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano de los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of a course information"
#elif L==4	// es
	"Editor de texto enriquecido de la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido de la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of the teaching guide"
#elif L==4	// es
	"Editor de texto enriquecido de la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido de la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of the lectures syllabus"
#elif L==4	// es
	"Editor de texto enriquecido del programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido del programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of the practicals syllabus"
#elif L==4	// es
	"Editor de texto enriquecido del programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido del programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of the bibliography"
#elif L==4	// es
	"Editor de texto enriquecido de la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido de la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of the FAQ"
#elif L==4	// es
	"Editor de texto enriquecido de la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido de la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of a course links"
#elif L==4	// es
	"Editor de texto enriquecido de los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido de los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of a course information"
#elif L==4	// es
	"Cambiar texto plano de la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano de la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of the teaching guide"
#elif L==4	// es
	"Cambiar texto plano de la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano de la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of the lectures syllabus"
#elif L==4	// es
	"Cambiar texto plano del programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano del programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of the practicals syllabus"
#elif L==4	// es
	"Cambiar texto plano del programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano del programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of the bibliography"
#elif L==4	// es
	"Cambiar texto plano de la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano de la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of the FAQ"
#elif L==4	// es
	"Cambiar texto plano de la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano de la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of a course links"
#elif L==4	// es
	"Cambiar texto plano de los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano de los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of a course information"
#elif L==4	// es
	"Cambiar texto enriquecido de la informaci&oacute;n de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido de la informaci&oacute;n de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of the teaching guide"
#elif L==4	// es
	"Cambiar texto enriquecido de la gu&iacute;a docente"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido de la gu&iacute;a docente"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of the lectures syllabus"
#elif L==4	// es
	"Cambiar texto enriquecido del programa de teor&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido del programa de teor&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of the practicals syllabus"
#elif L==4	// es
	"Cambiar texto enriquecido del programa de pr&aacute;cticas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido del programa de pr&aacute;cticas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of the bibliography"
#elif L==4	// es
	"Cambiar texto enriquecido de la bibliograf&iacute;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido de la bibliograf&iacute;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of the FAQ"
#elif L==4	// es
	"Cambiar texto enriquecido de la FAQ"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido de la FAQ"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of a course links"
#elif L==4	// es
	"Cambiar texto enriquecido de los enlaces de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido de los enlaces de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show print view of a course timetable"
#elif L==4	// es
	"Mostrar vista de impresi&oacute;n del horario de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar vista de impresi&oacute;n del horario de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the course timetable"
#elif L==4	// es
	"Editar el horario de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar el horario de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify the course timetable"
#elif L==4	// es
	"Modificar el horario de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Modificar el horario de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgCrsTT1stDay] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the first day of week of a course timetable"
#elif L==4	// es
	"Cambiar el primer d&iacute;a de la semana del horario de la asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el primer d&iacute;a de la semana del horario de la asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See assessment system"
#elif L==4	// es
	"Ver sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See assignments"
#elif L==4	// es
	"Ver actividades"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver actividades"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeePrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See projects"
#elif L==4	// es
	"Ver proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver proyectos"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAllExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See exam announcements"
#elif L==4	// es
	"Ver convocatorias de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver convocatorias de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the edition of test questions"
#elif L==4	// es
	"Solicitar la edici&oacute;n de preguntas de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la edici&oacute;n de preguntas de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqTst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request a test"
#elif L==4	// es
	"Solicitar un test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar un test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAllExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all exams"
#elif L==4	// es
	"Ver todos los ex&aacute;menes"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver todos los ex&aacute;menes"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAllGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all games"
#elif L==4	// es
	"Ver todos los juegos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver todos los juegos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAllSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See surveys"
#elif L==4	// es
	"Ver encuestas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver encuestas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the assessment system"
#elif L==4	// es
	"Editar el sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar el sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgFrcReaAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether students are forced to read the assessment system"
#elif L==4	// es
	"Cambiar si se obliga a los estudiantes a leer el sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si se obliga a los estudiantes a leer el sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgHavReaAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read the assessment system"
#elif L==4	// es
	"Cambiar si he le&iacute;do el sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar si he le&iacute;do el sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSelInfSrcAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of information shown in the assessment system"
#elif L==4	// es
	"Seleccionar el tipo de informaci&oacute;n mostrada en el sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar el tipo de informaci&oacute;n mostrada en el sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvURLAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a URL in the assessment system"
#elif L==4	// es
	"Crear una URL en el sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una URL en el sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPagAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a web page in the assessment system"
#elif L==4	// es
	"Crear una p&aacute;gina web en el sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una p&aacute;gina web en el sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEditorAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the assessment system"
#elif L==4	// es
	"Editor integrado del sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor integrado del sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlaTxtEdiAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Plain text editor of the assessment system"
#elif L==4	// es
	"Editor de texto plano del sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto plano del sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRchTxtEdiAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rich text editor of the assessment system"
#elif L==4	// es
	"Editor de texto enriquecido del sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editor de texto enriquecido del sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvPlaTxtAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change plain text of the assessment system"
#elif L==4	// es
	"Cambiar texto plano del sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto plano del sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvRchTxtAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change rich text of the assessment system"
#elif L==4	// es
	"Cambiar texto enriquecido del sistema de evaluaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar texto enriquecido del sistema de evaluaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmNewAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new assignment"
#elif L==4	// es
	"Editar una nueva actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOneAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit an assignment"
#elif L==4	// es
	"Editar una actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnOneAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show print view of an assignment"
#elif L==4	// es
	"Mostrar vista de impresi&oacute;n de una actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar vista de impresi&oacute;n de una actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create assignment"
#elif L==4	// es
	"Crear actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change an assignment"
#elif L==4	// es
	"Cambiar una actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar una actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of an assignment"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de una actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an assignment"
#elif L==4	// es
	"Eliminar una actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide an assignment"
#elif L==4	// es
	"Ocultar una actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide an assignment"
#elif L==4	// es
	"Mostrar una actividad"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una actividad"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqUsrPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select users to list their projects"
#elif L==4	// es
	"Seleccionar usuarios para listar sus proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar usuarios para listar sus proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeTblAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all projects in a table"
#elif L==4	// es
	"Mostrar todos los proyectos en una tabla"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar todos los proyectos en una tabla"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCfgPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Configure all projects"
#elif L==4	// es
	"Configurar todos los proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Configurar todos los proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvCfgPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change configuration of all projects"
#elif L==4	// es
	"Cambiar configuraci&oacute;n de todos los proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar configuraci&oacute;n de todos los proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqLckAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request locking of all projects"
#elif L==4	// es
	"Solicitar el bloqueo de todos los proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el bloqueo de todos los proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqUnlAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request unlocking of all projects"
#elif L==4	// es
	"Solicitar el desbloqueo de todos los proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el desbloqueo de todos los proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLckAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Lock all projects"
#elif L==4	// es
	"Bloquear todos los proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Bloquear todos los proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUnlAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unlock all projects"
#elif L==4	// es
	"Desbloquear todos los proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Desbloquear todos los proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmNewPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new project"
#elif L==4	// es
	"Editar un nuevo proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar un nuevo proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOnePrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a project"
#elif L==4	// es
	"Editar un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnOnePrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show primt view of a project"
#elif L==4	// es
	"Mostrar vista de impresi&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar vista de impresi&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new project"
#elif L==4	// es
	"Crear un nuevo proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change a project"
#elif L==4	// es
	"Cambiar un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a project"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a project"
#elif L==4	// es
	"Eliminar un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a project"
#elif L==4	// es
	"Ocultar un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a project"
#elif L==4	// es
	"Mostrar un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLckPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Lock project edition"
#elif L==4	// es
	"Bloquear edici&oacute;n de proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Bloquear edici&oacute;n de proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUnlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unlock project edition"
#elif L==4	// es
	"Desbloquear edici&oacute;n de proyectos"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Desbloquear edici&oacute;n de proyectos"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqAddStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request registering a student in a project"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n de un estudiante en un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n de un estudiante en un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqAddTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request registering a tutor in a project"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n de un tutor en un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n de un tutor en un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqAddEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request registering an evaluator in a project"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n de un evaluador en un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n de un evaluador en un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAddStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Register a student in a project"
#elif L==4	// es
	"Inscribir un estudiante en un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Inscribir un estudiante en un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAddTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Register a tutor in a project"
#elif L==4	// es
	"Inscribir un tutor en un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Inscribir un tutor en un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAddEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Register an evaluator in a project"
#elif L==4	// es
	"Inscribir un evaluador en un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Inscribir un evaluador en un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing a student from a project"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un estudiante de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un estudiante de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing a tutor from a project"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un tutor de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un tutor de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing an evaluator from a project"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un evaluador de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un evaluador de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a student from a project"
#elif L==4	// es
	"Eliminar un estudiante de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un estudiante de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a tutor from a project"
#elif L==4	// es
	"Eliminar un tutor de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un tutor de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an evaluator from a project"
#elif L==4	// es
	"Eliminar un evaluador de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un evaluador de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the documents of a project"
#elif L==4	// es
	"Administrar la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the documents zone of a project"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the documents zone of a project"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the documents zone of a project"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of a project"
#elif L==4	// es
	"Establecer origen de copia en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file into the documents zone of a project"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the documents zone of a project"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the documents zone of a project"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of a project"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of a project"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of a project"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocPrjDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file in the documents zone of a project using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo en la zona de documentos de un proyecto usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo en la zona de documentos de un proyecto usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocPrjCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file in the documents zone of a project using the classic way"
#elif L==4	// es
	"Enviar un archivo en la zona de documentos de un proyecto de la forma cl&aacute;sica"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo en la zona de documentos de un proyecto de la forma cl&aacute;sica"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the documents zone of a project"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the documents zone of a project"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the documents zone of a project"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a project"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the documents zone of a project"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a project"
#elif L==4	// es
	"Descargar un archivo en la zona de documentos de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo en la zona de documentos de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage a assessment zone of a project"
#elif L==4	// es
	"Administrar la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from assessment zone of a project"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from assessment zone of a project"
#elif L==4	// es
	"Eliminar un archivo de la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from assessment zone of a project"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the assessment zone of a project"
#elif L==4	// es
	"Establecer origen de copia en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file into assessment zone of a project"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from assessment zone of a project"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the assessment zone of a project"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the assessment zone of a project"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the assessment zone of a project"
#elif L==4	// es
	"Creat un nuevo enlace en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Creat un nuevo enlace en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the assessment zone of a project"
#elif L==4	// es
	"Renombrar una carpeta en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilAssPrjDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enviar a file in the assessment zone of a project using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de evaluaci&oacute;n de un proyecto usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de evaluaci&oacute;n de un proyecto usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilAssPrjCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enviar a file in the assessment zone of a project using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de evaluaci&oacute;n de un proyecto de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de evaluaci&oacute;n de un proyecto de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the assessment zone of a project"
#elif L==4	// es
	"Expandir una carpeta en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the assessment zone of a project"
#elif L==4	// es
	"Contraer una carpeta en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the assessment zone of a project"
#elif L==4	// es
	"Comprimir una carpeta en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the assessment zone of a project"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the assessment zone of a project"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the assessment zone of a project"
#elif L==4	// es
	"Descargar un archivo de la zona de evaluaci&oacute;n de un proyecto"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de evaluaci&oacute;n de un proyecto"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeOneExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See one exam announcement"
#elif L==4	// es
	"Ver una convocatoria de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver una convocatoria de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDatExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See exam announcements of a given date"
#elif L==4	// es
	"Ver convocatorias de examen de una fecha dada"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver convocatorias de examen de una fecha dada"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit an exam announcement"
#elif L==4	// es
	"Editar una convocatoria de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una convocatoria de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send an exam announcement"
#elif L==4	// es
	"Enviar una convocatoria de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar una convocatoria de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPrnExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See an exam announcement ready to be printed"
#elif L==4	// es
	"Ver una convocatoria de examen lista para imprimir"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver una convocatoria de examen lista para imprimir"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of an exam announcement"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de una convocatoria de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de una convocatoria de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an exam announcement"
#elif L==4	// es
	"Eliminar una convocatoria de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una convocatoria de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide exam announcement"
#elif L==4	// es
	"Ocultar una convocatoria de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una convocatoria de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUnhExaAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide exam announcement"
#elif L==4	// es
	"Mostrar una convocatoria de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una convocatoria de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOneTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a test question"
#elif L==4	// es
	"Editar una pregunta de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una pregunta de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqImpTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request an XML file with test questions to import"
#elif L==4	// es
	"Solicitar un archivo XML con preguntas de test para importar"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar un archivo XML con preguntas de test para importar"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActImpTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Import test questions from XML file"
#elif L==4	// es
	"Importar preguntas de test de un archivo XML"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Importar preguntas de test de un archivo XML"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLstTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List test questions for edition"
#elif L==4	// es
	"Listar preguntas de test para edici&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar preguntas de test para edici&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create/Modify a test question"
#elif L==4	// es
	"Crear/Modificar una pregunta de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear/Modificar una pregunta de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemSevTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing several test questions"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de varias preguntas de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de varias preguntas de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemSevTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove several test questions"
#elif L==4	// es
	"Eliminar varias preguntas de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar varias preguntas de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemOneTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing one test question"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una pregunta de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de una pregunta de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemOneTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a test question"
#elif L==4	// es
	"Eliminar una pregunta de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una pregunta de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgShfTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change shuffle of a test question"
#elif L==4	// es
	"Cambiar baraje de una pregunta de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar baraje de una pregunta de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiTag] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit tags in the question bank"
#elif L==4	// es
	"Editar descriptores del banco de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar descriptores del banco de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEnaTag] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a tag in the question bank"
#elif L==4	// es
	"Habilitar un descriptor del banco de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Habilitar un descriptor del banco de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDisTag] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a tag in the question bank"
#elif L==4	// es
	"Inhabilitar un descriptor del banco de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un descriptor del banco de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenTag] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a tag in the question bank"
#elif L==4	// es
	"Renombrar un descriptor del banco de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar un descriptor del banco de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeTst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See a test"
#elif L==4	// es
	"Ver un test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver un test"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqAssTst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request assessment of a test"
#elif L==4	// es
	"Solicitar evaluaci&oacute;n de un test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar evaluaci&oacute;n de un test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAssTst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Assess a test"
#elif L==4	// es
	"Evaluar un test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Evaluar un test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCfgTst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See test configuration"
#elif L==4	// es
	"Ver configuraci&oacute;n de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver configuraci&oacute;n de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvCfgTst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify test configuration"
#elif L==4	// es
	"Modificar configuraci&oacute;n de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Modificar configuraci&oacute;n de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqSeeMyTstRes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request to see my test results"
#elif L==4	// es
	"Solicitar ver mis resultados de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar ver mis resultados de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyTstResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See my test results"
#elif L==4	// es
	"Ver mis resultados de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mis resultados de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeOneTstResMe] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See one of my test results"
#elif L==4	// es
	"Ver uno de mis resultados de test"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver uno de mis resultados de test"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqSeeUsrTstRes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request test results for several users"
#elif L==4	// es
	"Solicitar resultados de test de varios usuarios"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar resultados de test de varios usuarios"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeUsrTstResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See test results for several users"
#elif L==4	// es
	"Ver los resultados de test de varios usuarios"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los resultados de test de varios usuarios"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeOneTstResOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See a test result for a user"
#elif L==4	// es
	"Ver un resultado de test de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver un resultado de test de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See exams"
#elif L==4	// es
	"Ver ex&aacute;menes"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver ex&aacute;menes"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmNewExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request new exam"
#elif L==4	// es
	"Solicitar nuevo examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar nuevo examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOneExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request exam edition"
#elif L==4	// es
	"Solicitar edici&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar edici&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new exam"
#elif L==4	// es
	"Crear nuevo examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear nuevo examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change exam"
#elif L==4	// es
	"Cambiar examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request exam removal"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove exam"
#elif L==4	// es
	"Eliminar examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide exam"
#elif L==4	// es
	"Ocultar examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide exam"
#elif L==4	// es
	"Mostrar examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmNewExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request new exam set"
#elif L==4	// es
	"Solicitar nuevo conjunto de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar nuevo conjunto de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new exam set"
#elif L==4	// es
	"Crear nuevo conjunto de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear nuevo conjunto de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request set of questions removal"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de conjunto de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de conjunto de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove set of questions"
#elif L==4	// es
	"Eliminar conjunto de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar conjunto de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUp_ExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move up set of questions"
#elif L==4	// es
	"Mover hacia arriba conjunto de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mover hacia arriba conjunto de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDwnExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move down set of questions"
#elif L==4	// es
	"Mover hacia abajo conjunto de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mover hacia abajo conjunto de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgTitExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change set of questions title"
#elif L==4	// es
	"Cambiar t&iacute;tulo de conjunto de preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar t&iacute;tulo de conjunto de preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgNumQstExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change number of questions from set to each exam print"
#elif L==4	// es
	"Cambiar n&ordm; de preguntas del conjunto en cada examen impreso"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar n&ordm; de preguntas del conjunto en cada examen impreso"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqAddQstExaSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request add question to exam"
#elif L==4	// es
	"Solicitar a&ntilde;adir pregunta a examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar a&ntilde;adir pregunta a examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLstTstQstForSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List test questions for exam"
#elif L==4	// es
	"Listar preguntas de test para examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar preguntas de test para examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAddQstToExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add a question to an exam"
#elif L==4	// es
	"A&ntilde;adir una pregunta a un examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"A&ntilde;adir una pregunta a un examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemSetQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request exam question removal"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de pregunta de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de pregunta de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemExaQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove exam question"
#elif L==4	// es
	"Eliminar pregunta de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar pregunta de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqNewExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request new exam session"
#elif L==4	// es
	"Solicitar nueva sesi&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar nueva sesi&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOneExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit exam session"
#elif L==4	// es
	"Editar sesi&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar sesi&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new exam session"
#elif L==4	// es
	"Crear nueva sesi&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear nueva sesi&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change exam session"
#elif L==4	// es
	"Cambiar sesi&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar sesi&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request exam session removal"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de sesi&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de sesi&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove exam session"
#elif L==4	// es
	"Eliminar sesi&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar sesi&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide exam session"
#elif L==4	// es
	"Ocultar sesi&oacute;n de examen"
#elif L==5	// fr
	"Ocultar sesi&oacute;n de examen"	// Besoin de traduction
#elif L==6	// gn
	""			// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUnhExaSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide exam session"
#elif L==4	// es
	"Mostrar sesi&oacute;n de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar sesi&oacute;n de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeExaPrn] =
#if   L==1	// ca
	"Comen&ccedil;ar / reprendre examen"
#elif L==2	// de
	"Pr&uacute;fung starten / fortsetzen"
#elif L==3	// en
	"Start / resume exam"
#elif L==4	// es
	"Comenzar / reanudar examen"
#elif L==5	// fr
	"Commencer / reprendre examen"
#elif L==6	// gn
	"Comenzar / reanudar examen"		// Okoteve traducción
#elif L==7	// it
	"Avvia / riprendi esame"
#elif L==8	// pl
	"Rozpocznij / wzn&oacute;w egzamin"
#elif L==9	// pt
	"Iniciar / retomar exame"
#endif
	,
	[ActAnsExaPrn] =
#if   L==1	// ca
	"Respondre pregunta d'examen"
#elif L==2	// de
	"Beantworten Sie die Pr&uuml;fungsfrage"
#elif L==3	// en
	"Answer exam question"
#elif L==4	// es
	"Responder pregunta de examen"
#elif L==5	// fr
	"R&eacute;pondre question d'examen"
#elif L==6	// gn
	"Responder pregunta de examen"		// Okoteve traducción
#elif L==7	// it
	"Rispondi domanda d'esame"
#elif L==8	// pl
	"Odpowiedz pytanie egzaminacyjne"
#elif L==9	// pt
	"Responda pergunta do exame"
#endif
	,
	[ActEndExaPrn] =
#if   L==1	// ca
	"Finalitzar examen"
#elif L==2	// de
	"Pr&uuml;fung beenden"
#elif L==3	// en
	"End exam"
#elif L==4	// es
	"Finalizar examen"
#elif L==5	// fr
	"Terminer examen"
#elif L==6	// gn
	"Finalizar examen"			// Okoteve traducción
#elif L==7	// it
	"Finire esame"
#elif L==8	// pl
	"Zako&nacute;czy&cacute; egzamin"
#elif L==9	// pt
	"Terminar exame"
#endif
	,
	[ActSeeMyExaResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See my exam results in a course"
#elif L==4	// es
	"Ver mis resultados de ex&aacute;menes en una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mis resultados de ex&aacute;menes en una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyExaResExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See my exam results in an exam"
#elif L==4	// es
	"Ver mis resultados de ex&aacute;menes en un examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mis resultados de ex&aacute;menes en un examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyExaResSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See my exam results in a session"
#elif L==4	// es
	"Ver mis resultados de ex&aacute;menes en una sesi&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mis resultados de ex&aacute;menes en una sesi&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeOneExaResMe] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See one of my exam results"
#elif L==4	// es
	"Ver uno de mis resultados de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver uno de mis resultados de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqSeeUsrExaRes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select users to see all their exam results"
#elif L==4	// es
	"Seleccionar usuarios para ver todos sus resultados de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar usuarios para ver todos sus resultados de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeUsrExaResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all exam results of a course"
#elif L==4	// es
	"Ver todos los resultados de examen de una sesi&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver todos los resultados de examen de una sesi&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeUsrExaResExa] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all exam results of an exam"
#elif L==4	// es
	"Ver todos los resultados de examen de un examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver todos los resultados de examen de un examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeUsrExaResSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all exam results of a session"
#elif L==4	// es
	"Ver todos los resultados de examen de una sesi&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver todos los resultados de examen de una sesi&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeOneExaResOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See one of the exam results of another user"
#elif L==4	// es
	"Ver uno de los resultados de examen de otro usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver uno de los resultados de examen de otro usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgVisExaRes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change visibility of exam results"
#elif L==4	// es
	"Cambiar visibilidad de resultados de examen"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar visibilidad de resultados de examen"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See a game"
#elif L==4	// es
	"Ver un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver un juego"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a match"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm the removal of a match"
#elif L==4	// es
	"Confirmar la eliminaci&oacute;n de una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Confirmar la eliminaci&oacute;n de una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqNewMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a new match"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de una nueva partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de una nueva partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new match and show it in a new tab"
#elif L==4	// es
	"Crear una nueva partida y mostrarla en una nueva pesta&ntilde;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva partida y mostrarla en una nueva pesta&ntilde;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change a match"
#elif L==4	// es
	"Cambiar una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActResMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Resume a match showing current question in a new tab"
#elif L==4	// es
	"Reanudar una partida mostrando la pregunta actual en una nueva pesta&ntilde;a"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Reanudar una partida mostrando la pregunta actual en una nueva pesta&ntilde;a"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActBckMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Go back when playing a match"
#elif L==4	// es
	"Ir atr&aacute;s mientras se juega una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ir atr&aacute;s mientras se juega una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPlyPauMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Play/pause current match"
#elif L==4	// es
	"Jugar/pausar partida actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Jugar/pausar partida actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFwdMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Go forward when playing a match"
#elif L==4	// es
	"Ir adelante mientras se juega una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ir adelante mientras se juega una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgNumColMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change number of columns when playing a match"
#elif L==4	// es
	"Cambiar n&uacute;mero de columnas mientras se juega una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar n&uacute;mero de columnas mientras se juega una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgVisResMchQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change visibility of question results when playing a match"
#elif L==4	// es
	"Cambiar visibilidad de los resultados de una pregunta mientras se juega una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar visibilidad de los resultados de una pregunta mientras se juega una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActMchCntDwn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Start match countdown"
#elif L==4	// es
	"Comenzar la cuenta atr&aacute;s en una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comenzar la cuenta atr&aacute;s en una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRefMchTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Refresh current question when playing a match (as teacher)"
#elif L==4	// es
	"Refrescar pregunta actual mientras se juega una partida (como profesor)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Refrescar pregunta actual mientras se juega una partida (como profesor)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActJoiMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show current question when playing a game (as student)"
#elif L==4	// es
	"Refrescar pregunta actual mientras se juega una partida (como estudiante)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Refrescar pregunta actual mientras se juega una partida (como estudiante)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMchAnsQstStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"View my answer to a match question"
#elif L==4	// es
	"Ver mi respuesta a una pregunta en una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mi respuesta a una pregunta en una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemMchAnsQstStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove my answer to a match question"
#elif L==4	// es
	"Eliminar mi respuesta a una pregunta de una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar mi respuesta a una pregunta de una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAnsMchQstStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Answer a match question"
#elif L==4	// es
	"Responder una pregunta de una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Responder una pregunta de una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRefMchStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Refresh current question while playing a match (as student)"
#elif L==4	// es
	"Refrescar pregunta actual mientras se juega una partida (como estudiante)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Refrescar pregunta actual mientras se juega una partida (como estudiante)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyMchResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all my matches results in the current course"
#elif L==4	// es
	"Ver todos mis resultados en partidas de la asignatura actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver todos mis resultados en partidas de la asignatura actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyMchResGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all my matches results in a given game"
#elif L==4	// es
	"Ver todos mis resultados en partidas de un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver todos mis resultados en partidas de un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyMchResMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See my results in a given match"
#elif L==4	// es
	"Ver mis resultados en una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mis resultados en una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeOneMchResMe] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See details of a match result of me as student"
#elif L==4	// es
	"Ver detalles del resultado de una partida m&iacute;a como estudiante"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver detalles del resultado de una partida m&iacute;a como estudiante"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqSeeUsrMchRes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select users and range of dates to see matches results"
#elif L==4	// es
	"Seleccionar usuarios y rango de fechas para ver los resultados de partidas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Seleccionar usuarios y rango de fechas para ver los resultados de partidas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeUsrMchResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all matches results of selected users in the current course"
#elif L==4	// es
	"Ver los resultados de todas las partidas de los usuarios seleccionados en la asignatura actual"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los resultados de todas las partidas de los usuarios seleccionados en la asignatura actual"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeUsrMchResGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See all matches results in a given game"
#elif L==4	// es
	"Ver los resultados de todas las partidas de un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los resultados de todas las partidas de un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeUsrMchResMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See results de un usuario in a given match"
#elif L==4	// es
	"Ver los resultados de un usuario en una partida"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los resultados de un usuario en una partida"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeOneMchResOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See details of a match result of other user"
#elif L==4	// es
	"Ver detalles del resultado de la partida de otro usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver detalles del resultado de la partida de otro usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgVisResMchUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the visibility of match results"
#elif L==4	// es
	"Cambiar la visibilidad de los resultados de partidas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la visibilidad de los resultados de partidas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActLstOneGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"View one game and its questions"
#elif L==4	// es
	"Ver un juego y sus preguntas"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver un juego y sus preguntas"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmNewGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new game"
#elif L==4	// es
	"Editar un nuevo juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar un nuevo juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOneGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a game"
#elif L==4	// es
	"Editar un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new game"
#elif L==4	// es
	"Crear un nuevo juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify the data of a game"
#elif L==4	// es
	"Modificar los datos de un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Modificar los datos de un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a game"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un juego"		// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a game"
#elif L==4	// es
	"Eliminar un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a game"
#elif L==4	// es
	"Ocultar un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show a game"
#elif L==4	// es
	"Mostrar un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAddOneGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add a new question to a game"
#elif L==4	// es
	"A&ntilde;adir una nueva pregunta a un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"A&ntilde;adir una nueva pregunta a un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActGamLstTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List questions to add some of them to a game"
#elif L==4	// es
	"Listar preguntas para a&ntilde;adir algunas de ellas a un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Listar preguntas para a&ntilde;adir algunas de ellas a un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAddTstQstToGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add the selected test questions to a game"
#elif L==4	// es
	"A&ntilde;adir las preguntas seleccionadas a un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"A&ntilde;adir las preguntas seleccionadas a un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a question of a game"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de una pregunta de un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de una pregunta de un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm the removal of a question of a game"
#elif L==4	// es
	"Confirmar la eliminaci&oacute;n de una pregunta de un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Confirmar la eliminaci&oacute;n de una pregunta de un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActUp_GamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move up a question of a game"
#elif L==4	// es
	"Mover hacia arriba una pregunta en un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mover hacia arriba una pregunta en un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDwnGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move down a question of a game"
#elif L==4	// es
	"Mover hacia abajo una pregunta en un juego"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mover hacia abajo una pregunta en un juego"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show a survey"
#elif L==4	// es
	"Mostrar una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAnsSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Answer a survey"
#elif L==4	// es
	"Responder una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Responder una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmNewSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new survey"
#elif L==4	// es
	"Editar una nueva encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOneSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a survey"
#elif L==4	// es
	"Editar una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActNewSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new survey"
#elif L==4	// es
	"Crear una nueva encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change a survey"
#elif L==4	// es
	"Cambiar una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a survey"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a survey"
#elif L==4	// es
	"Eliminar una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRstSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the reset of answers of a survey"
#elif L==4	// es
	"Solicitar la puesta a cero de las preguntas de una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la puesta a cero de las preguntas de una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRstSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reset answers of survey"
#elif L==4	// es
	"Poner a cero las respuestas de una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Poner a cero las respuestas de una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a survey"
#elif L==4	// es
	"Ocultar una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a survey"
#elif L==4	// es
	"Mostrar una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActEdiOneSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new question for a survey"
#elif L==4	// es
	"Editar una nueva pregunta para una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva pregunta para una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a question of a survey"
#elif L==4	// es
	"Crear una pregunta de una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una pregunta de una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a question of a survey"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de una pregunta de una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de una pregunta de una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm the removal of a question of a survey"
#elif L==4	// es
	"Confirmar la eliminaci&oacute;n de una pregunta de una encuesta"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Confirmar la eliminaci&oacute;n de una pregunta de una encuesta"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of an institution (see or manage)"
#elif L==4	// es
	"Zona de documentos de una instituci&oacute;n (ver o administrar)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Zona de documentos de una instituci&oacute;n (ver o administrar)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the shared files zone of an institution"
#elif L==4	// es
	"Administrar la zona de archivos compartidos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de archivos compartidos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of a center (see or manage)"
#elif L==4	// es
	"Zona de documentos de un centro (ver o administrar)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Zona de documentos de un centro (ver o administrar)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the shared files zone of a center"
#elif L==4	// es
	"Administrar la zona de archivos compartidos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de archivos compartidos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of a degree (see or manage)"
#elif L==4	// es
	"Zona de documentos de la titulaci&oacute;n (ver o administrar)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Zona de documentos de la titulaci&oacute;n (ver o administrar)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the shared files zone of a degree"
#elif L==4	// es
	"Administrar la zona de archivos compartidos de la titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de archivos compartidos de la titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAdmDocCrsGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of a course (see or manage)"
#elif L==4	// es
	"Zona de documentos de la asignatura (ver o administrar)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Zona de documentos de la asignatura (ver o administrar)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmTchCrsGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the management of the files of the teachers' documents zone of a course or of a group"
#elif L==4	// es
	"Solicitar la administraci&oacute;n de la zona de archivos de los profesores de la asignatura o de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la administraci&oacute;n de la zona de archivos de los profesores de la asignatura o de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmShaCrsGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the management of the files of the shared zone of a course or of a group"
#elif L==4	// es
	"Solicitar la administraci&oacute;n de la zona de archivos compartidos de la asignatura o de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la administraci&oacute;n de la zona de archivos compartidos de la asignatura o de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmAsgWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the assignments/works of a user"
#elif L==4	// es
	"Administrar las actividades/trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar las actividades/trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqAsgWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the assignments/works in the course"
#elif L==4	// es
	"Administrar las actividades/trabajos en una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar las actividades/trabajos en una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeAdmMrk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Marks zone (see or manage)"
#elif L==4	// es
	"Zona de calificaciones (ver o administrar)"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Zona de calificaciones (ver o administrar)"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage a briefcase of private archives"
#elif L==4	// es
	"Gestionar el malet&iacute;n de archivos privados"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Gestionar el malet&iacute;n de archivos privados"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the documents zone of an institution to view mode"
#elif L==4	// es
	"Cambiar la zona de documentos de una instituci&oacute;n a modo vista"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de una instituci&oacute;n a modo vista"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of an institution"
#elif L==4	// es
	"Ver los archivos de la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los archivos de la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in institution documents"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in institution documents"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in institution documents"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of an institution"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of an institution"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to administration mode of institution documents"
#elif L==4	// es
	"Cambiar modo de administraci&oacute;n de los documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar modo de administraci&oacute;n de los documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the documents zone of an institution"
#elif L==4	// es
	"Administrar la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the documents zone of an institution"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in the documents zone of an institution"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder the documents zone of an institution"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of an institution"
#elif L==4	// es
	"Establecer origen de copia en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the documents zone of an institution"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the documents zone of an institution"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or a file in the documents zone of an institution"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of an institution"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of an institution"
#elif L==4	// es
	"Crear una nuevo enlace la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nuevo enlace la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of an institution"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocInsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of an institution using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a una zona de documentos de una instituci&oacute;n usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a una zona de documentos de una instituci&oacute;n usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocInsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file in the documents zone of an institution using the classic way"
#elif L==4	// es
	"Enviar un archivo a una zona de documentos de una instituci&oacute;n de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a una zona de documentos de una instituci&oacute;n de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when managing the documents zone of an institution"
#elif L==4	// es
	"Expandir una carpeta al administrar la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta al administrar la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when administrating the documents zone of an institution"
#elif L==4	// es
	"Contraer una carpeta al administrar la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta al administrar la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when administrating the documents zone of an institution"
#elif L==4	// es
	"Comprimir una carpeta al administrar la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta al administrar la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a folder or a file of the documents zone of an institution"
#elif L==4	// es
	"Mostrar una carpeta o archivo al administrar la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una carpeta o archivo al administrar la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a folder or a file of the documents zone of an institution"
#elif L==4	// es
	"Ocultar una carpeta o archivo al administrar la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una carpeta o archivo al administrar la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of an institution"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the documents zone of an institution"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of an institution"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of an institution"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo compartido de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo compartido de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file of an institution"
#elif L==4	// es
	"Eliminar un archivo compartido de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo compartido de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty shared folder of an institution"
#elif L==4	// es
	"Eliminar una carpeta compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the shared zone of an institution"
#elif L==4	// es
	"Establecer origen de copia en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the shared zone of an institution"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the shared zone of an institution"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or a file in the shared zone of an institution"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the shared zone of an institution"
#elif L==4	// es
	"Crear una nueva carpeta en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the shared zone of an institution"
#elif L==4	// es
	"Crear un nuevo enlace en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the shared zone of an institution"
#elif L==4	// es
	"Eliminar una carpeta de la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta de la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaInsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to shared zone of an institution using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona compartida de una instituci&oacute;n usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona compartida de una instituci&oacute;n usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaInsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to shared zone of an institution using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona compartida de una instituci&oacute;n de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona compartida de una instituci&oacute;n de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the shared zone of an institution"
#elif L==4	// es
	"Expandir una carpeta en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the shared zone of an institution"
#elif L==4	// es
	"Contraer una carpeta en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the shared zone of an institution"
#elif L==4	// es
	"Comprimir una carpeta en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the shared zone of an institution"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the shared zone of an institution"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the shared zone of an institution"
#elif L==4	// es
	"Descargar un archivo de la zona compartida de una instituci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona compartida de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the documents zone of a center to view mode"
#elif L==4	// es
	"Cambiar la zona de documentos de un centro a modo vista"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de un centro a modo vista"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of a center"
#elif L==4	// es
	"Ver los archivos de la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los archivos de la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the documents zone of a center"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the documents zone of a center"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the documents zone of a center"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a center"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a center"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the documents zone of a center to management mode"
#elif L==4	// es
	"Cambiar la zona de documentos de un centro a modo administraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de un centro a modo administraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the documents zone of a center"
#elif L==4	// es
	"Administrar la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the documents zone of a center"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the documents zone of a center"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder the documents zone of a center"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of a center"
#elif L==4	// es
	"Establecer origen de copia en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the documents zone of a center"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the documents zone of a center"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or a file in the documents zone of a center"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of a center"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of a center"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of a center"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocCtrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of a center using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de un centro usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de un centro usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocCtrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of a center using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de un centro de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de un centro de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when administrating the documents zone of a center"
#elif L==4	// es
	"Expandir una carpeta al administrar la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta al administrar la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when administrating the documents zone of a center"
#elif L==4	// es
	"Contraer una carpeta al administrar la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta al administrar la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when administrating the documents zone of a center"
#elif L==4	// es
	"Comprimir una carpeta al administrar la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta al administrar la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a folder or a file of the documents zone of a center"
#elif L==4	// es
	"Mostrar una carpeta o archivo en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una carpeta o archivo en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a folder or a file of the documents zone of a center"
#elif L==4	// es
	"Ocultar una carpeta o archivo en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una carpeta o archivo en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a center"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the documents zone of a center"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a center"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of a center"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo compartido de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo compartido de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file of a center"
#elif L==4	// es
	"Eliminar un archivo compartido de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo compartido de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty shared folder of a center"
#elif L==4	// es
	"Eliminar una carpeta compartido vac&iacute;a de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta compartido vac&iacute;a de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the shared zone of a center"
#elif L==4	// es
	"Establecer origen de copia en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the shared zone of a center"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the shared zone of a center"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or a file in the shared zone of a center"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the shared zone of a center"
#elif L==4	// es
	"Crear una nueva carpeta en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the shared zone of a center"
#elif L==4	// es
	"Crear un nuevo enlace en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the shared zone of a center"
#elif L==4	// es
	"Renombrar una carpeta en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaCtrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the shared zone of a center using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona compartida de un centro usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona compartida de un centro usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaCtrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to shared zone of a center using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona compartida de un centro de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona compartida de un centro de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the shared zone of a center"
#elif L==4	// es
	"Expandir una carpeta en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the shared zone of a center"
#elif L==4	// es
	"Contraer una carpeta en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the shared zone of a center"
#elif L==4	// es
	"Comprimir una carpeta en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the shared zone of a center"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the shared zone of a center"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the shared zone of a center"
#elif L==4	// es
	"Descargar un archivo de la zona compartida de un centro"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona compartida de un centro"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the documents zone of a degree to view mode"
#elif L==4	// es
	"Cambiar la zona de documentos de la titulaci&oacute;n a modo vista"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de la titulaci&oacute;n a modo vista"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of a degree"
#elif L==4	// es
	"Ver los archivos de la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los archivos de la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the documents zone of a degree"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the documents zone of a degree"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the documents zone of a degree"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a degree"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a degree"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the documents zone of a degree to management mode"
#elif L==4	// es
	"Cambiar la zona de documentos de una titulaci&oacute;n a modo administraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de una titulaci&oacute;n a modo administraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the documents zone of a degree"
#elif L==4	// es
	"Administrar la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the documents zone of a degree"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the documents zone of a degree"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the documents zone of a degree"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of a degree"
#elif L==4	// es
	"Establecer origen de copia la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the documents zone of a degree"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the documents zone of a degree"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the documents zone of a degree"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of a degree"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of a degree"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of a degree"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocDegDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of a degree using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de una titulaci&oacute;n usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de una titulaci&oacute;n usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocDegCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of a degree using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de una titulaci&oacute;n de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de una titulaci&oacute;n de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when managing the documents zone of a degree"
#elif L==4	// es
	"Expandir una carpeta al administrar la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta al administrar la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when managing the documents zone of a degree"
#elif L==4	// es
	"Contraer una carpeta al administrar la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta al administrar la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when mamaging the documents zone of a degree"
#elif L==4	// es
	"Comprimir una carpeta al administrar la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta al administrar la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a folder or a file in the documents zone of a degree"
#elif L==4	// es
	"Mostrar una carpeta o archivo en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una carpeta o archivo en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a folder or a file in the documents zone of a degree"
#elif L==4	// es
	"Ocultar una carpeta o archivo en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una carpeta o archivo en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a degree"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the documents zone of a degree"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a degree"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the shared zone of a degree"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo compartido de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo compartido de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file from the shared zone of a degree"
#elif L==4	// es
	"Eliminar un archivo de la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the shared zone of a degree"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the shared zone of a degree"
#elif L==4	// es
	"Establecer origen de copia en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the shared zone of a degree"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the shared zone of a degree"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the shared zone of a degree"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the shared zone of a degree"
#elif L==4	// es
	"Crear una nueva carpeta en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the shared zone of a degree"
#elif L==4	// es
	"Crear un nuevo enlace en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the shared zone of a degree"
#elif L==4	// es
	"Renombrar una carpeta en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaDegDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the shared zone of a degree using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona compartida de una titulaci&oacute;n usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona compartida de una titulaci&oacute;n usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaDegCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file in the shared zone of a degree using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona compartida de una titulaci&oacute;n de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona compartida de una titulaci&oacute;n de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the shared zone of a degree"
#elif L==4	// es
	"Expandir una carpeta en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the shared zone of a degree"
#elif L==4	// es
	"Contraer una carpeta en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the shared zone of a degree"
#elif L==4	// es
	"Comprimir una carpeta en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the shared zone of a degree"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the shared zone of a degree"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the shared zone of a degree"
#elif L==4	// es
	"Descargar un archivo de la zona compartida de una titulaci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona compartida de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the documents zone of a course/group to view mode"
#elif L==4	// es
	"Cambiar la zona de documentos de una asignatura/grupo a modo vista"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de una asignatura/grupo a modo vista"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of a course"
#elif L==4	// es
	"Ver los archivos de la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los archivos de la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the documents zone of a course"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the documents zone of a course"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the documents zone of a course"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a course"
#elif L==4	// es
	"Descargar los metadatos de un archivo de la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar los metadatos de un archivo de la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of a group"
#elif L==4	// es
	"Ver los archivos de la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los archivos de la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the documents zone of a group"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the documents zone of a group"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the documents zone of a group"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a group"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a group"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the documents zone of a course/group to management mode"
#elif L==4	// es
	"Cambiar la zona de documentos de una asignatura/grupo a modo administraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de una asignatura/grupo a modo administraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the documents zone of a course"
#elif L==4	// es
	"Administrar la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the documents zone of a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in the documents zone of a course"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder the documents zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of a course"
#elif L==4	// es
	"Establecer origen de copia en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the documents zone of a course"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the documents zone of a course"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or a file in the documents zone of a course"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of a course"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of a course"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of a course"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of a course using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de una asignatura usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de una asignatura usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file in the documents zone of a course using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de una asignatura de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de una asignatura de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when managing the documents zone of a course"
#elif L==4	// es
	"Expandir una carpeta al administrar la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta al administrar la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when managing the documents zone of a course"
#elif L==4	// es
	"Contraer una carpeta al administrar la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta al administrar la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when managing the documents zone of a course"
#elif L==4	// es
	"Comprimir una carpeta al administrar la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta al administrar la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a folder or a file in the documents zone of a course"
#elif L==4	// es
	"Mostrar una carpeta o archivo en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una carpeta o archivo en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a folder or a file in the documents zone of a course"
#elif L==4	// es
	"Ocultar una carpeta o archivo en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una carpeta o archivo en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the documents zone of a course"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a course"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the documents zone of a group"
#elif L==4	// es
	"Administrar la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the documents zone of a group"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the documents zone of a group"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the documents zone of a group"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of a group"
#elif L==4	// es
	"Establecer origen de copia en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the documents zone of a group"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the documents zone of a group"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or a file in the documents zone of a group"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of a group"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of a group"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of a group"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of a group using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de un grupo mediante Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de un grupo mediante Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilDocGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the documents zone of a group using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de un grupo de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de un grupo de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when managing the documents zone of a group"
#elif L==4	// es
	"Expandir una carpeta al administrar la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta al administrar la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when mamaging the documents zone of a group"
#elif L==4	// es
	"Contraer una carpeta al administrar la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta al administrar la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when managing the documents zone of a group"
#elif L==4	// es
	"Comprimir una carpeta al administrar la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta al administrar la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a folder or a file in the documents zone of a group"
#elif L==4	// es
	"Mostrar una carpeta o archivo en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una carpeta o archivo en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a folder or a file in the documents zone of a group"
#elif L==4	// es
	"Ocultar una carpeta o archivo en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una carpeta o archivo en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the documents zone of a group"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the documents zone of a group"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the documents zone of a group"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToAdmTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the teachers' documents zone of a course/group to management mode"
#elif L==4	// es
	"Cambiar la zona de documentos de profesores de una asignatura/grupo a modo administraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de documentos de profesores de una asignatura/grupo a modo administraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the teachers' documents zone of a course"
#elif L==4	// es
	"Administrar la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the teachers' documents zone of a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from teachers' documents zone of a course"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the teachers' documents zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the teachers' documents zone of a course"
#elif L==4	// es
	"Establecer origen de copia en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the teachers' documents zone of a course"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an non-empty folder from the teachers' documents zone of a course"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or a file in the teachers' documents zone of a course"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the teachers' documents zone of a course"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the teachers' documents zone of a course"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the teachers' documents zone of a course"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilTchCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the teachers' documents zone of a course using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de profesores de una asignatura usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de profesores de una asignatura usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilTchCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the teachers' documents zone of a course using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de profesores de una asignatura de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de profesores de una asignatura de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the teachers' documents zone of a course"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the teachers' documents zone of a course"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the teachers' documents zone of a course"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the teachers' documents zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the teachers' documents zone of a course"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the teachers' documents zone of a course"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de profesores de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the teachers' documents zone of a group"
#elif L==4	// es
	"Administrar la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the teachers' documents zone of a group"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the teachers' documents zone of a group"
#elif L==4	// es
	"Eliminar un archivo de la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder of the teachers' documents zone of a group"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the teachers' documents zone of a group"
#elif L==4	// es
	"Establecer origen de copia en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the teachers' documents zone of a group"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder of the teachers' documents zone of a group"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the teachers' documents zone of a group"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the teachers' documents zone of a group"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the teachers' documents zone of a group"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the teachers' documents zone of a group"
#elif L==4	// es
	"Renombrar una carpeta en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilTchGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the teachers' documents zone of a group using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de profesores de un grupo usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de profesores de un grupo usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilTchGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file in the teachers' documents zone of a group using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de documentos de profesores de un grupo de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de documentos de profesores de un grupo de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the teachers' documents zone of a group"
#elif L==4	// es
	"Expandir una carpeta en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the teachers' documents zone of a group"
#elif L==4	// es
	"Contraer una carpeta en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the teachers' documents zone of a group"
#elif L==4	// es
	"Comprimir una carpeta en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the teachers' documents zone of a group"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the teachers' documents zone of a group"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the teachers' documents zone of a group"
#elif L==4	// es
	"Descargar un archivo de la zona de documentos de profesores de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de documentos de profesores de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToAdmSha] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the shared files zone of a course/group to management mode"
#elif L==4	// es
	"Cambiar la zona de archivos compartidos de una asignatura/grupo a modo administraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de archivos compartidos de una asignatura/grupo a modo administraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the shared files zone of a course"
#elif L==4	// es
	"Administrar la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo compartido de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo compartido de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the shared files zone of a course"
#elif L==4	// es
	"Eliminar un archivo de la zona de archivos compartidos una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de archivos compartidos una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the shared files zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the shared files zone of a course"
#elif L==4	// es
	"Establecer origen de copia en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the shared files zone of a course"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the shared files zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the shared files zone of a course"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the shared files zone of a course"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the shared files zone of a course"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the shared files zone of a course"
#elif L==4	// es
	"Renombrar una carpeta en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the shared files zone of a course using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de archivos compartidos de una asignatura usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de archivos compartidos de una asignatura usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the shared files zone of a course using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de archivos compartidos de una asignatura de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de archivos compartidos de una asignatura de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the shared files zone of a course"
#elif L==4	// es
	"Expandir una carpeta en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the shared files zone of a course"
#elif L==4	// es
	"Contract una carpeta en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contract una carpeta en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the shared files zone of a course"
#elif L==4	// es
	"Comprimir una carpeta en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the shared files zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the shared files zone of a course"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the shared files zone of a course"
#elif L==4	// es
	"Descargar un archivo de la zona de archivos compartidos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de archivos compartidos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the shared files zone of a group"
#elif L==4	// es
	"Administrar la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the shared files zone of a group"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the shared files zone of a group"
#elif L==4	// es
	"Eliminar un archivo de la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the shared files zone of a group"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the shared files zone of a group"
#elif L==4	// es
	"Establecer origen de copia en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the shared files zone of a group"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the shared files zone of a group"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the shared zone of a group"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the shared files zone of a group"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the shared files zone of a group"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the shared zone of a group"
#elif L==4	// es
	"Renombrar una carpeta en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the shared files zone of a group using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de archivos compartidos de un grupo usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de archivos compartidos de un grupo usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilShaGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the shared files zone of a group using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de archivos compartidos de un grupo de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de archivos compartidos de un grupo de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the shared files zone of a group"
#elif L==4	// es
	"Expandir una carpeta en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the shared files zone of a group"
#elif L==4	// es
	"Contraer una carpeta en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the shared files zone of a group"
#elif L==4	// es
	"Comprimir una carpeta en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the shared files zone of a group"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the shared files zone of a group"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the shared files zone of a group"
#elif L==4	// es
	"Descargar un archivo de la zona de archivos compartidos de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de archivos compartidos de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmAsgWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage all assignments/works in a course"
#elif L==4	// es
	"Gestionar todas las actividades/trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Gestionar todas las actividades/trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the assignments of a user"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the assignments of a user"
#elif L==4	// es
	"Eliminar un archivo de las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the assignments of a user"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in the assignments of a user"
#elif L==4	// es
	"Establecer origen de copia en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder in the assignments of a user"
#elif L==4	// es
	"Pegar un archivo o una carpeta en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar un archivo o una carpeta en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder of the assignments of a user"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file of assignments of a user"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the assignments of a user"
#elif L==4	// es
	"Crear una nueva carpeta en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the assignments of a user"
#elif L==4	// es
	"Crear un nuevo enlace en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the assignments of a user"
#elif L==4	// es
	"Renombrar una carpeta en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilAsgUsrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a new file to the assignments of a user using Dropzone.js"
#elif L==4	// es
	"Enviar un nuevo archivo a las actividades de un usuario usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un nuevo archivo a las actividades de un usuario usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilAsgUsrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a new file to the assignments of a user using the classic way"
#elif L==4	// es
	"Enviar un nuevo archivo a las actividades de un usuario de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un nuevo archivo a las actividades de un usuario de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the assignments of a user"
#elif L==4	// es
	"Expandir una carpeta en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the assignments of a user"
#elif L==4	// es
	"Contraer una carpeta en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the assignments of a user"
#elif L==4	// es
	"Comprimir una carpeta en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the assignments of a user"
#elif L==4	// es
	"Ver los metadatos de un archivo en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the assignments of a user"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the assignments of a user"
#elif L==4	// es
	"Descargar un archivo de las actividades de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de las actividades de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the works of a user"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the works of a user"
#elif L==4	// es
	"Eliminar un archivo de los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the works of a user"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in the works of a user"
#elif L==4	// es
	"Establecer origen de copia en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder in the works of a user"
#elif L==4	// es
	"Pegar un archivo or carpeta en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar un archivo or carpeta en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the works of a user"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the works of a user"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the works of a user"
#elif L==4	// es
	"Crear una nueva carpeta en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the works of a user"
#elif L==4	// es
	"Crear un nuevo enlace en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the works of a user"
#elif L==4	// es
	"Renombrar una carpeta en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilWrkUsrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the works of a user using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a los trabajos de un usuario usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a los trabajos de un usuario usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilWrkUsrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the works of a user using the classic way"
#elif L==4	// es
	"Enviar un archivo a los trabajos de un usuario de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a los trabajos de un usuario de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the works of a user"
#elif L==4	// es
	"Expandir una carpeta en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the works of a user"
#elif L==4	// es
	"Contraer una carpeta en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the works of a user"
#elif L==4	// es
	"Comprimir una carpeta en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the works of a user"
#elif L==4	// es
	"Ver los metadatos de un archivo en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the works of a user"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the works of a user"
#elif L==4	// es
	"Descargar un archivo de los trabajos de un usuario"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de los trabajos de un usuario"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the assignments zone of a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the assignments zone of a course"
#elif L==4	// es
	"Eliminar un archivo en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the assignments zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&acute;a de la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&acute;a de la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in the assignments zone of a course"
#elif L==4	// es
	"Establecer origen de copia en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder in the assignments zone of a course"
#elif L==4	// es
	"Pegar un archivo o una carpeta en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar un archivo o una carpeta en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the assignments zone of a course"
#elif L==4	// es
	"Eliminar una carpeta no vac&acute;a de la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&acute;a de la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the assignments zone of a course"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the assignments zone of a course"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the assignments zone of a course"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the assignments zone of a course"
#elif L==4	// es
	"Renombrar una carpeta en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilAsgCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the assignments zone of a course using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de actividades de una asignatura usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de actividades de una asignatura usando Dropzone.js" // Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilAsgCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the assignments zone of a course using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de actividades de una asignatura de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de actividades de una asignatura de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the assignments zone of a course"
#elif L==4	// es
	"Expandir una carpeta en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the assignments zone of a course"
#elif L==4	// es
	"Contraer una carpeta en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the assignments zone of a course"
#elif L==4	// es
	"Comprimir una carpeta en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the assignments zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the assignments zone of a course"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the assignments zone of a course"
#elif L==4	// es
	"Descargar un archivo de la zona de actividades de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de actividades de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the works zone of a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the works zone of a course"
#elif L==4	// es
	"Eliminar un archivo de la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the works zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in the works zone of a course"
#elif L==4	// es
	"Establecer origen de copia en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder in the works zone of a course"
#elif L==4	// es
	"Pegar un archivo o carpeta en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar un archivo o carpeta en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the works zone of a course"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the works zone of a course"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the works zone of a course"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreLnkWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the works zone of a course"
#elif L==4	// es
	"Crear un nuevo enlace en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the works zone of a course"
#elif L==4	// es
	"Renombrar una carpeta en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilWrkCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the works zone of a course using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de trabajos de una asignatura usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de trabajos de una asignatura usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilWrkCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the works zone of a course using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de trabajos de una asignatura de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de trabajos de una asignatura de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the works zone of a course"
#elif L==4	// es
	"Expandir una carpeta en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the works zone of a course"
#elif L==4	// es
	"Contraer una carpeta en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the works zone of a course"
#elif L==4	// es
	"Comprimir una carpeta en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the works zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the works zone of a course"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the works zone of a course"
#elif L==4	// es
	"Descargar un archivo de la zona de trabajos de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de trabajos de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToSeeMrk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the marks zone of a course/group to view mode"
#elif L==4	// es
	"Cambiar la zona de calificaciones de una asignatura/grupo a modo vista"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de calificaciones de una asignatura/grupo a modo vista"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the marks zone of a course"
#elif L==4	// es
	"Ver la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the marks zone of a course"
#elif L==4	// es
	"Expandir una carpeta en la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the marks zone of a  course"
#elif L==4	// es
	"Contraer una carpeta en la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the marks zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Individualized access to a marks file in a course"
#elif L==4	// es
	"Acceso individualizado a un archivo de calificaciones en una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Acceso individualizado a un archivo de calificaciones en una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the marks zone of a group"
#elif L==4	// es
	"Ver la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the marks zone of a group"
#elif L==4	// es
	"Expandir una carpeta en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the marks zone of a group"
#elif L==4	// es
	"Contraer una carpeta en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the marks zone of a group"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeMyMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Individualized access to a marks file in a group"
#elif L==4	// es
	"Acceso individualizado a un archivo de calificaciones en un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Acceso individualizado a un archivo de calificaciones en un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgToAdmMrk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the marks zone of a course/group to management mode"
#elif L==4	// es
	"Cambiar la zona de calificaciones de una asignatura/grupo a modo administraci&oacute;n"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la zona de calificaciones de una asignatura/grupo a modo administraci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the marks zone of a course"
#elif L==4	// es
	"Gestionar la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Gestionar la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in marks in course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in marks from a course"
#elif L==4	// es
	"Eliminar un archivo de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the marks zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the marks zone of a course"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the marks zone of a course"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the marks zone of a course"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the marks zone of a course"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the marks zone of a course"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the marks zone of a course"
#elif L==4	// es
	"Renombrar una carpeta en la zona de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilMrkCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the marks zone of a course using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de calificaciones de una asignatura usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de calificaciones de una asignatura usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilMrkCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the marks zone of a course using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de calificaciones de una asignatura de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de calificaciones de una asignatura de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the marks management zone of a course"
#elif L==4	// es
	"Expandir una carpeta en la zona de administración de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de administración de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the marks management zone of a course"
#elif L==4	// es
	"Contraer una carpeta en la zona de administraci&oacute;n de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de administraci&oacute;n de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the marks management zone of a course"
#elif L==4	// es
	"Comprimir una carpeta en la zona de administraci&oacute;n de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de administraci&oacute;n de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a folder or a file in the marks management zone of a course"
#elif L==4	// es
	"Mostrar una carpeta o archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una carpeta o archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a folder or a file in the marks management zone of a course"
#elif L==4	// es
	"Ocultar una carpeta o archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una carpeta o archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the marks management zone of a course"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the marks management zone of a course"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the marks management zone of a course"
#elif L==4	// es
	"Descargar un archivo de la zona de administraci&oacute;n de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de administraci&oacute;n de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgNumRowHeaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the number of head rows of a file in marks of a course"
#elif L==4	// es
	"Cambiar el n&uacute;mero de filas de cabecera de un archivo de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el n&uacute;mero de filas de cabecera de un archivo de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgNumRowFooCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the number of foot rows of a file in marks in course"
#elif L==4	// es
	"Cambiar el n&uacute;mero de filas de pie de un archivo de calificaciones de una asignatura"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el n&uacute;mero de filas de pie de un archivo de calificaciones de una asignatura"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Manage the marks zone of a group"
#elif L==4	// es
	"Administrar la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Administrar la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqRemFilMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from the marks zone of a group"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFilMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from the marks zone of a group"
#elif L==4	// es
	"Eliminar un archivo de la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemFolMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from the marks zone of a group"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCopMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set the source of copy in the marks zone of a group"
#elif L==4	// es
	"Establecer origen de copia en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActPasMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in the marks zone of a group"
#elif L==4	// es
	"Pegar una carpeta o archivo en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRemTreMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from the marks zone of a group"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActFrmCreMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in the marks zone of a group"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActCreFolMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the marks zone of a group"
#elif L==4	// es
	"Crear una nueva carpeta en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRenFolMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the marks zone of a group"
#elif L==4	// es
	"Renombrar una carpeta en la zona de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en la zona de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilMrkGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the marks zone of a group using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a la zona de calificaciones de un grupo usando Dropzone.js"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de calificaciones de un grupo usando Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActRcvFilMrkGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Send a file to the marks zone of a group using the classic way"
#elif L==4	// es
	"Enviar un archivo a la zona de calificaciones de un grupo de forma tradicional"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a la zona de calificaciones de un grupo de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActExpAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in the marks management zone of a group"
#elif L==4	// es
	"Expandir una carpeta en la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActConAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in the marks management zone of a group"
#elif L==4	// es
	"Contraer una carpeta en la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActZIPAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in the marks management zone of a group"
#elif L==4	// es
	"Comprimir una carpeta en la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActShoMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unhide a folder or a file of the marks management zone of a group"
#elif L==4	// es
	"Mostrar una carpeta de la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Mostrar una carpeta de la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActHidMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a folder or a file of the marks management zone of a group"
#elif L==4	// es
	"Ocultar una carpeta de la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ocultar una carpeta de la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqDatAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in the marks management zone of a group"
#elif L==4	// es
	"Ver los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgDatAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in the marks management zone of a group"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActDowAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from the marks management zone of a group"
#elif L==4	// es
	"Descargar un archivo de la zona de administraci&oacute;n de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de la zona de administraci&oacute;n de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgNumRowHeaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the number of head rows of a file in marks of a group"
#elif L==4	// es
	"Cambiar el n&uacute;mero de filas de cabecera de un archivo de calificaciones de un grupo"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el n&uacute;mero de filas de cabecera de un archivo de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActChgNumRowFooGrp] =
#if   L==1	// ca
	"Change the number of foot rows of a file in marks of a group"				// Necessita traducció
#elif L==2	// de
	"Change the number of foot rows of a file in marks of a group"				// Need Übersetzung
#elif L==3	// en
	"Change the number of foot rows of a file in marks of a group"
#elif L==4	// es
	"Cambiar el n&uacute;mero de filas de pie de un archivo de calificaciones de un grupo"
#elif L==5	// fr
	"Change the number of foot rows of a file in marks of a group"				// Besoin de traduction
#elif L==6	// gn
	"Cambiar el n&uacute;mero de filas de pie de un archivo de calificaciones de un grupo"	// Okoteve traducción
#elif L==7	// it
	"Change the number of foot rows of a file in marks of a group"				// Bisogno di traduzione
#elif L==8	// pl
	"Change the number of foot rows of a file in marks of a group"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the number of foot rows of a file in marks of a group"				// Precisa de tradução
#endif
	,
	[ActReqRemFilBrf] =
#if   L==1	// ca
	"Request removal of a file from a briefcase"				// Necessita traducció
#elif L==2	// de
	"Request removal of a file from a briefcase"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from a briefcase"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un archivo de un malet&iacute;n"
#elif L==5	// fr
	"Request removal of a file from a briefcase"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un archivo de un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a file from a briefcase"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a file from a briefcase"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a file from a briefcase"				// Precisa de tradução
#endif
	,
	[ActRemFilBrf] =
#if   L==1	// ca
	"Remove a file from a briefcase"		// Necessita traducció
#elif L==2	// de
	"Remove a file from a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Remove a file from a briefcase"
#elif L==4	// es
	"Eliminar un archivo de un malet&iacute;n"
#elif L==5	// fr
	"Remove a file from a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un archivo de un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a file from a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a file from a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a file from a briefcase"		// Precisa de tradução
#endif
	,
	[ActRemFolBrf] =
#if   L==1	// ca
	"Remove an empty folder from a briefcase"			// Necessita traducció
#elif L==2	// de
	"Remove an empty folder from a briefcase"			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from a briefcase"
#elif L==4	// es
	"Eliminar una carpeta vac&iacute;a de un malet&iacute;n"
#elif L==5	// fr
	"Remove an empty folder from a briefcase"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta vac&iacute;a de un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove an empty folder from a briefcase"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove an empty folder from a briefcase"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove an empty folder from a briefcase"			// Precisa de tradução
#endif
	,
	[ActCopBrf] =
#if   L==1	// ca
	"Set source of copy in a briefcase"			// Necessita traducció
#elif L==2	// de
	"Set source of copy in a briefcase"			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in a briefcase"
#elif L==4	// es
	"Establecer origen de copia en un malet&iacute;n"
#elif L==5	// fr
	"Set source of copy in a briefcase"			// Besoin de traduction
#elif L==6	// gn
	"Establecer origen de copia en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Set source of copy in a briefcase"			// Bisogno di traduzione
#elif L==8	// pl
	"Set source of copy in a briefcase"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Set source of copy in a briefcase"			// Precisa de tradução
#endif
	,
	[ActPasBrf] =
#if   L==1	// ca
	"Paste a folder or a file in a briefcase"		// Necessita traducció
#elif L==2	// de
	"Paste a folder or a file in a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Paste a folder or a file in a briefcase"
#elif L==4	// es
	"Pegar una carpeta o archivo en un malet&iacute;n"
#elif L==5	// fr
	"Paste a folder or a file in a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Pegar una carpeta o archivo en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Paste a folder or a file in a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Paste a folder or a file in a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a folder or a file in a briefcase"		// Precisa de tradução
#endif
	,
	[ActRemTreBrf] =
#if   L==1	// ca
	"Remove a non-empty folder from a briefcase"			// Necessita traducció
#elif L==2	// de
	"Remove a non-empty folder from a briefcase"			// Need Übersetzung
#elif L==3	// en
	"Remove a non-empty folder from a briefcase"
#elif L==4	// es
	"Eliminar una carpeta no vac&iacute;a de un malet&iacute;n"
#elif L==5	// fr
	"Remove a non-empty folder from a briefcase"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar una carpeta no vac&iacute;a de un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a non-empty folder from a briefcase"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a non-empty folder from a briefcase"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a non-empty folder from a briefcase"			// Precisa de tradução
#endif
	,
	[ActFrmCreBrf] =
#if   L==1	// ca
	"Edit a new folder or file in a briefcase"			// Necessita traducció
#elif L==2	// de
	"Edit a new folder or file in a briefcase"			// Need Übersetzung
#elif L==3	// en
	"Edit a new folder or file in a briefcase"
#elif L==4	// es
	"Editar una nueva carpeta o archivo en un malet&iacute;n"
#elif L==5	// fr
	"Edit a new folder or file in a briefcase"			// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva carpeta o archivo en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Edit a new folder or file in a briefcase"			// Bisogno di traduzione
#elif L==8	// pl
	"Edit a new folder or file in a briefcase"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit a new folder or file in a briefcase"			// Precisa de tradução
#endif
	,
	[ActCreFolBrf] =
#if   L==1	// ca
	"Create a new folder in a briefcase"		// Necessita traducció
#elif L==2	// de
	"Create a new folder in a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Create a new folder in a briefcase"
#elif L==4	// es
	"Crear una nueva carpeta en un malet&iacute;n"
#elif L==5	// fr
	"Create a new folder in a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva carpeta en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Create a new folder in a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new folder in a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new folder in a briefcase"		// Precisa de tradução
#endif
	,
	[ActCreLnkBrf] =
#if   L==1	// ca
	"Create a new link in a briefcase"		// Necessita traducció
#elif L==2	// de
	"Create a new link in a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Create a new link in a briefcase"
#elif L==4	// es
	"Crear un nuevo enlace en un malet&iacute;n"
#elif L==5	// fr
	"Create a new link in a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo enlace en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Create a new link in a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new link in a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new link in a briefcase"		// Precisa de tradução
#endif
	,
	[ActRenFolBrf] =
#if   L==1	// ca
	"Rename a folder of a briefcase"		// Necessita traducció
#elif L==2	// de
	"Rename a folder of a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Rename a folder of a briefcase"
#elif L==4	// es
	"Renombrar una carpeta en un malet&iacute;n"
#elif L==5	// fr
	"Rename a folder of a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Renombrar una carpeta en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Rename a folder of a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Rename a folder of a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Rename a folder of a briefcase"		// Precisa de tradução
#endif
	,
	[ActRcvFilBrfDZ] =
#if   L==1	// ca
	"Send a file to a briefcase using Dropzone.js"			// Necessita traducció
#elif L==2	// de
	"Send a file to a briefcase using Dropzone.js"			// Need Übersetzung
#elif L==3	// en
	"Send a file to a briefcase using Dropzone.js"
#elif L==4	// es
	"Enviar un archivo a un malet&iacute;n using Dropzone.js"
#elif L==5	// fr
	"Send a file to a briefcase using Dropzone.js"			// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a un malet&iacute;n using Dropzone.js"	// Okoteve traducción
#elif L==7	// it
	"Send a file to a briefcase using Dropzone.js"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a file to a briefcase using Dropzone.js"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a file to a briefcase using Dropzone.js"			// Precisa de tradução
#endif
	,
	[ActRcvFilBrfCla] =
#if   L==1	// ca
	"Send a file to a briefcase using the classic way"		// Necessita traducció
#elif L==2	// de
	"Send a file to a briefcase using the classic way"		// Need Übersetzung
#elif L==3	// en
	"Send a file to a briefcase using the classic way"
#elif L==4	// es
	"Enviar un archivo a un malet&iacute;n de forma tradicional"
#elif L==5	// fr
	"Send a file to a briefcase using the classic way"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un archivo a un malet&iacute;n de forma tradicional"	// Okoteve traducción
#elif L==7	// it
	"Send a file to a briefcase using the classic way"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a file to a briefcase using the classic way"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a file to a briefcase using the classic way"		// Precisa de tradução
#endif
	,
	[ActExpBrf] =
#if   L==1	// ca
	"Expand a folder in a briefcase"		// Necessita traducció
#elif L==2	// de
	"Expand a folder in a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Expand a folder in a briefcase"
#elif L==4	// es
	"Expandir una carpeta en un malet&iacute;n"
#elif L==5	// fr
	"Expand a folder in a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Expandir una carpeta en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Expand a folder in a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Expand a folder in a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Expand a folder in a briefcase"		// Precisa de tradução
#endif
	,
	[ActConBrf] =
#if   L==1	// ca
	"Contract a folder in a briefcase"		// Necessita traducció
#elif L==2	// de
	"Contract a folder in a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Contract a folder in a briefcase"
#elif L==4	// es
	"Contraer una carpeta en un malet&iacute;n"
#elif L==5	// fr
	"Contract a folder in a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Contraer una carpeta en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Contract a folder in a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Contract a folder in a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Contract a folder in a briefcase"		// Precisa de tradução
#endif
	,
	[ActZIPBrf] =
#if   L==1	// ca
	"Compress a folder in a briefcase"		// Necessita traducció
#elif L==2	// de
	"Compress a folder in a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Compress a folder in a briefcase"
#elif L==4	// es
	"Comprimir una carpeta en un malet&iacute;n"
#elif L==5	// fr
	"Compress a folder in a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Comprimir una carpeta en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Compress a folder in a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Compress a folder in a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Compress a folder in a briefcase"		// Precisa de tradução
#endif
	,
	[ActReqDatBrf] =
#if   L==1	// ca
	"See the metadata of a file in a briefcase"		// Necessita traducció
#elif L==2	// de
	"See the metadata of a file in a briefcase"		// Need Übersetzung
#elif L==3	// en
	"See the metadata of a file in a briefcase"
#elif L==4	// es
	"Ver los metadatos de un archivo en un malet&iacute;n"
#elif L==5	// fr
	"See the metadata of a file in a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Ver los metadatos de un archivo en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"See the metadata of a file in a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"See the metadata of a file in a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the metadata of a file in a briefcase"		// Precisa de tradução
#endif
	,
	[ActChgDatBrf] =
#if   L==1	// ca
	"Change the metadata of a file in a briefcase"			// Necessita traducció
#elif L==2	// de
	"Change the metadata of a file in a briefcase"			// Need Übersetzung
#elif L==3	// en
	"Change the metadata of a file in a briefcase"
#elif L==4	// es
	"Cambiar los metadatos de un archivo en un malet&iacute;n"
#elif L==5	// fr
	"Change the metadata of a file in a briefcase"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar los metadatos de un archivo en un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Change the metadata of a file in a briefcase"			// Bisogno di traduzione
#elif L==8	// pl
	"Change the metadata of a file in a briefcase"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the metadata of a file in a briefcase"			// Precisa de tradução
#endif
	,
	[ActDowBrf] =
#if   L==1	// ca
	"Download a file from a briefcase"		// Necessita traducció
#elif L==2	// de
	"Download a file from a briefcase"		// Need Übersetzung
#elif L==3	// en
	"Download a file from a briefcase"
#elif L==4	// es
	"Descargar un archivo de un malet&iacute;n"
#elif L==5	// fr
	"Download a file from a briefcase"		// Besoin de traduction
#elif L==6	// gn
	"Descargar un archivo de un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Download a file from a briefcase"		// Bisogno di traduzione
#elif L==8	// pl
	"Download a file from a briefcase"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Download a file from a briefcase"		// Precisa de tradução
#endif
	,
	[ActReqRemOldBrf] =
#if   L==1	// ca
	"Request removal of old files from a briefcase"					// Necessita traducció
#elif L==2	// de
	"Request removal of old files from a briefcase"					// Need Übersetzung
#elif L==3	// en
	"Request removal of old files from a briefcase"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de archivos antiguos de un malet&iacute;n"
#elif L==5	// fr
	"Request removal of old files from a briefcase"					// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de archivos antiguos de un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request removal of old files from a briefcase"					// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of old files from a briefcase"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of old files from a briefcase"					// Precisa de tradução
#endif
	,
	[ActRemOldBrf] =
#if   L==1	// ca
	"Remove old files in a briefcase"			// Necessita traducció
#elif L==2	// de
	"Remove old files in a briefcase"			// Need Übersetzung
#elif L==3	// en
	"Remove old files in a briefcase"
#elif L==4	// es
	"Eliminar archivos antiguos de un malet&iacute;n"
#elif L==5	// fr
	"Remove old files in a briefcase"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar archivos antiguos de un malet&iacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove old files in a briefcase"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove old files in a briefcase"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove old files in a briefcase"			// Precisa de tradução
#endif
	,
	[ActReqSelGrp] =
#if   L==1	// ca
	"Request the selection of groups of students"			// Necessita traducció
#elif L==2	// de
	"Request the selection of groups of students"			// Need Übersetzung
#elif L==3	// en
	"Request the selection of groups of students"
#elif L==4	// es
	"Solicitar la selecci&oacute;n de grupos de estudiantes"
#elif L==5	// fr
	"Request the selection of groups of students"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la selecci&oacute;n de grupos de estudiantes"	// Okoteve traducción
#elif L==7	// it
	"Request the selection of groups of students"			// Bisogno di traduzione
#elif L==8	// pl
	"Request the selection of groups of students"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the selection of groups of students"			// Precisa de tradução
#endif
	,
	[ActLstStd] =
#if   L==1	// ca
	"Show class photo or list of students"		// Necessita traducció
#elif L==2	// de
	"Show class photo or list of students"		// Need Übersetzung
#elif L==3	// en
	"Show class photo or list of students"
#elif L==4	// es
	"Mostrar foto de clase o lista de estudiantes"
#elif L==5	// fr
	"Show class photo or list of students"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar foto de clase o lista de estudiantes"	// Okoteve traducción
#elif L==7	// it
	"Show class photo or list of students"		// Bisogno di traduzione
#elif L==8	// pl
	"Show class photo or list of students"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show class photo or list of students"		// Precisa de tradução
#endif
	,
	[ActLstTch] =
#if   L==1	// ca
	"Show class photo or list of teachers"		// Necessita traducció
#elif L==2	// de
	"Show class photo or list of teachers"		// Need Übersetzung
#elif L==3	// en
	"Show class photo or list of teachers"
#elif L==4	// es
	"Mostrar foto de clase o lista de profesores"
#elif L==5	// fr
	"Show class photo or list of teachers"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar foto de clase o lista de profesores"	// Okoteve traducción
#elif L==7	// it
	"Show class photo or list of teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Show class photo or list of teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show class photo or list of teachers"		// Precisa de tradução
#endif
	,
	[ActLstOth] =
#if   L==1	// ca
	"Show class photo or list of guests"		// Necessita traducció
#elif L==2	// de
	"Show class photo or list of guests"		// Need Übersetzung
#elif L==3	// en
	"Show class photo or list of guests"
#elif L==4	// es
	"Mostrar foto de clase o lista de invitados"
#elif L==5	// fr
	"Show class photo or list of guests"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar foto de clase o lista de invitados"	// Okoteve traducción
#elif L==7	// it
	"Show class photo or list of guests"		// Bisogno di traduzione
#elif L==8	// pl
	"Show class photo or list of guests"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show class photo or list of guests"		// Precisa de tradução
#endif
	,
	[ActSeeAtt] =
#if   L==1	// ca
	"Show attendance events"	// Necessita traducció
#elif L==2	// de
	"Show attendance events"	// Need Übersetzung
#elif L==3	// en
	"Show attendance events"
#elif L==4	// es
	"Mostrar eventos de asistencia"
#elif L==5	// fr
	"Show attendance events"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar eventos de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Show attendance events"	// Bisogno di traduzione
#elif L==8	// pl
	"Show attendance events"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show attendance events"	// Precisa de tradução
#endif
	,
	[ActReqSignUp] =
#if   L==1	// ca
	"Apply for my enrolment"		// Necessita traducció
#elif L==2	// de
	"Apply for my enrolment"		// Need Übersetzung
#elif L==3	// en
	"Apply for my enrolment"
#elif L==4	// es
	"Solicitar mi inscripci&oacute;n"
#elif L==5	// fr
	"Apply for my enrolment"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar mi inscripci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Apply for my enrolment"		// Bisogno di traduzione
#elif L==8	// pl
	"Apply for my enrolment"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Apply for my enrolment"		// Precisa de tradução
#endif
	,
	[ActSeeSignUpReq] =
#if   L==1	// ca
	"Show pending requests for inscription in the current course"			// Necessita traducció
#elif L==2	// de
	"Show pending requests for inscription in the current course"			// Need Übersetzung
#elif L==3	// en
	"Show pending requests for inscription in the current course"
#elif L==4	// es
	"Mostrar solicitudes de inscripci&oacute;n pendientes en la asignatura actual"
#elif L==5	// fr
	"Show pending requests for inscription in the current course"			// Besoin de traduction
#elif L==6	// gn
	"Mostrar solicitudes de inscripci&oacute;n pendientes en la asignatura actual"	// Okoteve traducción
#elif L==7	// it
	"Show pending requests for inscription in the current course"			// Bisogno di traduzione
#elif L==8	// pl
	"Show pending requests for inscription in the current course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show pending requests for inscription in the current course"			// Precisa de tradução
#endif
	,
	[ActLstCon] =
#if   L==1	// ca
	"List connected users"		// Necessita traducció
#elif L==2	// de
	"List connected users"		// Need Übersetzung
#elif L==3	// en
	"List connected users"
#elif L==4	// es
	"Listar usuarios conectados"
#elif L==5	// fr
	"List connected users"		// Besoin de traduction
#elif L==6	// gn
	"Listar usuarios conectados"	// Okoteve traducción
#elif L==7	// it
	"List connected users"		// Bisogno di traduzione
#elif L==8	// pl
	"List connected users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List connected users"		// Precisa de tradução
#endif
	,
	[ActChgGrp] =
#if   L==1	// ca
	"Change my groups"	// Necessita traducció
#elif L==2	// de
	"Change my groups"	// Need Übersetzung
#elif L==3	// en
	"Change my groups"
#elif L==4	// es
	"Cambiar mis grupos"
#elif L==5	// fr
	"Change my groups"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar mis grupos"	// Okoteve traducción
#elif L==7	// it
	"Change my groups"	// Bisogno di traduzione
#elif L==8	// pl
	"Change my groups"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my groups"	// Precisa de tradução
#endif
	,
	[ActReqEdiGrp] =
#if   L==1	// ca
	"Request the edition of groups"		// Necessita traducció
#elif L==2	// de
	"Request the edition of groups"		// Need Übersetzung
#elif L==3	// en
	"Request the edition of groups"
#elif L==4	// es
	"Solicitar la edici&oacute;n de grupos"
#elif L==5	// fr
	"Request the edition of groups"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar la edici&oacute;n de grupos"	// Okoteve traducción
#elif L==7	// it
	"Request the edition of groups"		// Bisogno di traduzione
#elif L==8	// pl
	"Request the edition of groups"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the edition of groups"		// Precisa de tradução
#endif
	,
	[ActNewGrpTyp] =
#if   L==1	// ca
	"Request the creation of a type of group"		// Necessita traducció
#elif L==2	// de
	"Request the creation of a type of group"		// Need Übersetzung
#elif L==3	// en
	"Request the creation of a type of group"
#elif L==4	// es
	"Solicitar la creaci&oacute;n de un tipo de grupo"
#elif L==5	// fr
	"Request the creation of a type of group"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar la creaci&oacute;n de un tipo de grupo"	// Okoteve traducción
#elif L==7	// it
	"Request the creation of a type of group"		// Bisogno di traduzione
#elif L==8	// pl
	"Request the creation of a type of group"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the creation of a type of group"		// Precisa de tradução
#endif
	,
	[ActReqRemGrpTyp] =
#if   L==1	// ca
	"Request removal of a type of group"			// Necessita traducció
#elif L==2	// de
	"Request removal of a type of group"			// Need Übersetzung
#elif L==3	// en
	"Request removal of a type of group"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un tipo de grupo"
#elif L==5	// fr
	"Request removal of a type of group"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un tipo de grupo"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a type of group"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a type of group"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a type of group"			// Precisa de tradução
#endif
	,
	[ActRemGrpTyp] =
#if   L==1	// ca
	"Remove a type of group"	// Necessita traducció
#elif L==2	// de
	"Remove a type of group"	// Need Übersetzung
#elif L==3	// en
	"Remove a type of group"
#elif L==4	// es
	"Eliminar un tipo de grupo"
#elif L==5	// fr
	"Remove a type of group"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un tipo de grupo"	// Okoteve traducción
#elif L==7	// it
	"Remove a type of group"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove a type of group"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a type of group"	// Precisa de tradução
#endif
	,
	[ActRenGrpTyp] =
#if   L==1	// ca
	"Rename a type of group"	// Necessita traducció
#elif L==2	// de
	"Rename a type of group"	// Need Übersetzung
#elif L==3	// en
	"Rename a type of group"
#elif L==4	// es
	"Renombrar un tipo de grupo"
#elif L==5	// fr
	"Rename a type of group"	// Besoin de traduction
#elif L==6	// gn
	"Renombrar un tipo de grupo"	// Okoteve traducción
#elif L==7	// it
	"Rename a type of group"	// Bisogno di traduzione
#elif L==8	// pl
	"Rename a type of group"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Rename a type of group"	// Precisa de tradução
#endif
	,
	[ActChgMdtGrpTyp] =
#if   L==1	// ca
	"Change if it is mandatory to register in groups of a type"	// Necessita traducció
#elif L==2	// de
	"Change if it is mandatory to register in groups of a type"	// Need Übersetzung
#elif L==3	// en
	"Change if it is mandatory to register in groups of a type"
#elif L==4	// es
	"Cambiar si es obligatorio registrarse en grupos de un tipo"
#elif L==5	// fr
	"Change if it is mandatory to register in groups of a type"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar si es obligatorio registrarse en grupos de un tipo"	// Okoteve traducción
#elif L==7	// it
	"Change if it is mandatory to register in groups of a type"	// Bisogno di traduzione
#elif L==8	// pl
	"Change if it is mandatory to register in groups of a type"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change if it is mandatory to register in groups of a type"	// Precisa de tradução
#endif
	,
	[ActChgMulGrpTyp] =
#if   L==1	// ca
	"Change if it is possible to register in multiple groups of a type"	// Necessita traducció
#elif L==2	// de
	"Change if it is possible to register in multiple groups of a type"	// Need Übersetzung
#elif L==3	// en
	"Change if it is possible to register in multiple groups of a type"
#elif L==4	// es
	"Cambiar si es posible registrarse en varios grupos de un tipo"
#elif L==5	// fr
	"Change if it is possible to register in multiple groups of a type"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar si es posible registrarse en varios grupos de un tipo"		// Okoteve traducción
#elif L==7	// it
	"Change if it is possible to register in multiple groups of a type"	// Bisogno di traduzione
#elif L==8	// pl
	"Change if it is possible to register in multiple groups of a type"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change if it is possible to register in multiple groups of a type"	// Precisa de tradução
#endif
	,
	[ActChgTimGrpTyp] =
#if   L==1	// ca
	"Change the date when groups of a type will open"			// Necessita traducció
#elif L==2	// de
	"Change the date when groups of a type will open"			// Need Übersetzung
#elif L==3	// en
	"Change the date when groups of a type will open"
#elif L==4	// es
	"Cambiar la fecha en la que los grupos de un tipo se abrir&aacute;n"
#elif L==5	// fr
	"Change the date when groups of a type will open"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la fecha en la que los grupos de un tipo se abrir&aacute;n"	// Okoteve traducción
#elif L==7	// it
	"Change the date when groups of a type will open"			// Bisogno di traduzione
#elif L==8	// pl
	"Change the date when groups of a type will open"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the date when groups of a type will open"			// Precisa de tradução
#endif
	,
	[ActNewGrp] =
#if   L==1	// ca
	"Create a group"	// Necessita traducció
#elif L==2	// de
	"Create a group"	// Need Übersetzung
#elif L==3	// en
	"Create a group"
#elif L==4	// es
	"Crear un grupo"
#elif L==5	// fr
	"Create a group"	// Besoin de traduction
#elif L==6	// gn
	"Crear un grupo"	// Okoteve traducción
#elif L==7	// it
	"Create a group"	// Bisogno di traduzione
#elif L==8	// pl
	"Create a group"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a group"	// Precisa de tradução
#endif
	,
	[ActReqRemGrp] =
#if   L==1	// ca
	"Request removal of a group"			// Necessita traducció
#elif L==2	// de
	"Request removal of a group"			// Need Übersetzung
#elif L==3	// en
	"Request removal of a group"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un grupo"
#elif L==5	// fr
	"Request removal of a group"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un grupo"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a group"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a group"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a group"			// Precisa de tradução
#endif
	,
	[ActRemGrp] =
#if   L==1	// ca
	"Remove a group"	// Necessita traducció
#elif L==2	// de
	"Remove a group"	// Need Übersetzung
#elif L==3	// en
	"Remove a group"
#elif L==4	// es
	"Eliminar un grupo"
#elif L==5	// fr
	"Remove a group"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un grupo"	// Okoteve traducción
#elif L==7	// it
	"Remove a group"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove a group"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a group"	// Precisa de tradução
#endif
	,
	[ActOpeGrp] =
#if   L==1	// ca
	"Open a group"		// Necessita traducció
#elif L==2	// de
	"Open a group"		// Need Übersetzung
#elif L==3	// en
	"Open a group"
#elif L==4	// es
	"Abrir un grupo"
#elif L==5	// fr
	"Open a group"		// Besoin de traduction
#elif L==6	// gn
	"Abrir un grupo"	// Okoteve traducción
#elif L==7	// it
	"Open a group"		// Bisogno di traduzione
#elif L==8	// pl
	"Open a group"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Open a group"		// Precisa de tradução
#endif
	,
	[ActCloGrp] =
#if   L==1	// ca
	"Close a group"		// Necessita traducció
#elif L==2	// de
	"Close a group"		// Need Übersetzung
#elif L==3	// en
	"Close a group"
#elif L==4	// es
	"Cerrar un grupo"
#elif L==5	// fr
	"Close a group"		// Besoin de traduction
#elif L==6	// gn
	"Cerrar un grupo"	// Okoteve traducción
#elif L==7	// it
	"Close a group"		// Bisogno di traduzione
#elif L==8	// pl
	"Close a group"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Close a group"		// Precisa de tradução
#endif
	,
	[ActEnaFilZonGrp] =
#if   L==1	// ca
	"Enable filezones of a group"			// Necessita traducció
#elif L==2	// de
	"Enable filezones of a group"			// Need Übersetzung
#elif L==3	// en
	"Enable filezones of a group"
#elif L==4	// es
	"Habilitar zonas de archivos de un grupo"
#elif L==5	// fr
	"Enable filezones of a group"			// Besoin de traduction
#elif L==6	// gn
	"Habilitar zonas de archivos de un grupo"	// Okoteve traducción
#elif L==7	// it
	"Enable filezones of a group"			// Bisogno di traduzione
#elif L==8	// pl
	"Enable filezones of a group"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable filezones of a group"			// Precisa de tradução
#endif
	,
	[ActDisFilZonGrp] =
#if   L==1	// ca
	"Disable filezones of a group"			// Necessita traducció
#elif L==2	// de
	"Disable filezones of a group"			// Need Übersetzung
#elif L==3	// en
	"Disable filezones of a group"
#elif L==4	// es
	"Inhabilitar zonas de archivos de un grupo"
#elif L==5	// fr
	"Disable filezones of a group"			// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar zonas de archivos de un grupo"	// Okoteve traducción
#elif L==7	// it
	"Disable filezones of a group"			// Bisogno di traduzione
#elif L==8	// pl
	"Disable filezones of a group"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable filezones of a group"			// Precisa de tradução
#endif
	,
	[ActChgGrpTyp] =
#if   L==1	// ca
	"Change the type of group of a group"	// Necessita traducció
#elif L==2	// de
	"Change the type of group of a group"	// Need Übersetzung
#elif L==3	// en
	"Change the type of group of a group"
#elif L==4	// es
	"Cambiar el tipo de grupo de un grupo"
#elif L==5	// fr
	"Change the type of group of a group"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar el tipo de grupo de un grupo"	// Okoteve traducción
#elif L==7	// it
	"Change the type of group of a group"	// Bisogno di traduzione
#elif L==8	// pl
	"Change the type of group of a group"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the type of group of a group"	// Precisa de tradução
#endif
	,
	[ActRenGrp] =
#if   L==1	// ca
	"Rename a group"	// Necessita traducció
#elif L==2	// de
	"Rename a group"	// Need Übersetzung
#elif L==3	// en
	"Rename a group"
#elif L==4	// es
	"Renombrar un grupo"
#elif L==5	// fr
	"Rename a group"	// Besoin de traduction
#elif L==6	// gn
	"Renombrar un grupo"	// Okoteve traducción
#elif L==7	// it
	"Rename a group"	// Bisogno di traduzione
#elif L==8	// pl
	"Rename a group"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Rename a group"	// Precisa de tradução
#endif
	,
	[ActChgGrpRoo] =
#if   L==1	// ca
	"Change the room of a group"	// Necessita traducció
#elif L==2	// de
	"Change the room of a group"	// Need Übersetzung
#elif L==3	// en
	"Change the room of a group"
#elif L==4	// es
	"Cambiar la sala de un grupo"
#elif L==5	// fr
	"Change the room of a group"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar la sala de un grupo"	// Okoteve traducción
#elif L==7	// it
	"Change the room of a group"	// Bisogno di traduzione
#elif L==8	// pl
	"Change the room of a group"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the room of a group"	// Precisa de tradução
#endif
	,
	[ActChgMaxStdGrp] =
#if   L==1	// ca
	"Change the maximum number of students in a group"			// Necessita traducció
#elif L==2	// de
	"Change the maximum number of students in a group"			// Need Übersetzung
#elif L==3	// en
	"Change the maximum number of students in a group"
#elif L==4	// es
	"Cambiar el n&uacute;mero m&aacute;ximo de estudiantes de un grupo"
#elif L==5	// fr
	"Change the maximum number of students in a group"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el n&uacute;mero m&aacute;ximo de estudiantes de un grupo"	// Okoteve traducción
#elif L==7	// it
	"Change the maximum number of students in a group"			// Bisogno di traduzione
#elif L==8	// pl
	"Change the maximum number of students in a group"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the maximum number of students in a group"			// Precisa de tradução
#endif
	,
	[ActLstGst] =
#if   L==1	// ca
	"List the main data of the administrators"		// Necessita traducció
#elif L==2	// de
	"List the main data of the administrators"		// Need Übersetzung
#elif L==3	// en
	"List the main data of the administrators"
#elif L==4	// es
	"Listar los datos principales de los administradores"
#elif L==5	// fr
	"List the main data of the administrators"		// Besoin de traduction
#elif L==6	// gn
	"Listar los datos principales de los administradores"	// Okoteve traducción
#elif L==7	// it
	"List the main data of the administrators"		// Bisogno di traduzione
#elif L==8	// pl
	"List the main data of the administrators"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List the main data of the administrators"		// Precisa de tradução
#endif
	,
	[ActPrnGstPho] =
#if   L==1	// ca
	"Show the class photo of guests ready to be printed"		// Necessita traducció
#elif L==2	// de
	"Show the class photo of guests ready to be printed"		// Need Übersetzung
#elif L==3	// en
	"Show the class photo of guests ready to be printed"
#elif L==4	// es
	"Mostrar la foto de clase de invitados lista para imprimir"
#elif L==5	// fr
	"Show the class photo of guests ready to be printed"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar la foto de clase de invitados lista para imprimir"	// Okoteve traducción
#elif L==7	// it
	"Show the class photo of guests ready to be printed"		// Bisogno di traduzione
#elif L==8	// pl
	"Show the class photo of guests ready to be printed"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show the class photo of guests ready to be printed"		// Precisa de tradução
#endif
	,
	[ActPrnStdPho] =
#if   L==1	// ca
	"Show the class photo of students ready to be printed"		// Necessita traducció
#elif L==2	// de
	"Show the class photo of students ready to be printed"		// Need Übersetzung
#elif L==3	// en
	"Show the class photo of students ready to be printed"
#elif L==4	// es
	"Mostrar la foto de clase de estudiantes lista para imprimir"
#elif L==5	// fr
	"Show the class photo of students ready to be printed"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar la foto de clase de estudiantes lista para imprimir"	// Okoteve traducción
#elif L==7	// it
	"Show the class photo of students ready to be printed"		// Bisogno di traduzione
#elif L==8	// pl
	"Show the class photo of students ready to be printed"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show the class photo of students ready to be printed"		// Precisa de tradução
#endif
	,
	[ActPrnTchPho] =
#if   L==1	// ca
	"Show the class photo of teachers ready to be printed"		// Necessita traducció
#elif L==2	// de
	"Show the class photo of teachers ready to be printed"		// Need Übersetzung
#elif L==3	// en
	"Show the class photo of teachers ready to be printed"
#elif L==4	// es
	"Mostrar la foto de clase de profesores lista para imprimir"
#elif L==5	// fr
	"Show the class photo of teachers ready to be printed"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar la foto de clase de profesores lista para imprimir"	// Okoteve traducción
#elif L==7	// it
	"Show the class photo of teachers ready to be printed"		// Bisogno di traduzione
#elif L==8	// pl
	"Show the class photo of teachers ready to be printed"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show the class photo of teachers ready to be printed"		// Precisa de tradução
#endif
	,
	[ActLstGstAll] =
#if   L==1	// ca
	"List the full data of the guests"		// Necessita traducció
#elif L==2	// de
	"List the full data of the guests"		// Need Übersetzung
#elif L==3	// en
	"List the full data of the guests"
#elif L==4	// es
	"Listar los datos completos de los invitados"
#elif L==5	// fr
	"List the full data of the guests"		// Besoin de traduction
#elif L==6	// gn
	"Listar los datos completos de los invitados"	// Okoteve traducción
#elif L==7	// it
	"List the full data of the guests"		// Bisogno di traduzione
#elif L==8	// pl
	"List the full data of the guests"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List the full data of the guests"		// Precisa de tradução
#endif
	,
	[ActLstStdAll] =
#if   L==1	// ca
	"List the full data of the students"		// Necessita traducció
#elif L==2	// de
	"List the full data of the students"		// Need Übersetzung
#elif L==3	// en
	"List the full data of the students"
#elif L==4	// es
	"Listar los datos completos de los estudiantes"
#elif L==5	// fr
	"List the full data of the students"		// Besoin de traduction
#elif L==6	// gn
	"Listar los datos completos de los estudiantes"	// Okoteve traducción
#elif L==7	// it
	"List the full data of the students"		// Bisogno di traduzione
#elif L==8	// pl
	"List the full data of the students"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List the full data of the students"		// Precisa de tradução
#endif
	,
	[ActLstTchAll] =
#if   L==1	// ca
	"List the full data of the teachers"		// Necessita traducció
#elif L==2	// de
	"List the full data of the teachers"		// Need Übersetzung
#elif L==3	// en
	"List the full data of the teachers"
#elif L==4	// es
	"Listar los datos completos de los profesores"
#elif L==5	// fr
	"List the full data of the teachers"		// Besoin de traduction
#elif L==6	// gn
	"Listar los datos completos de los profesores"	// Okoteve traducción
#elif L==7	// it
	"List the full data of the teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"List the full data of the teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List the full data of the teachers"		// Precisa de tradução
#endif
	,
	[ActSeeRecOneStd] =
#if   L==1	// ca
	"See the record of a student"	// Necessita traducció
#elif L==2	// de
	"See the record of a student"	// Need Übersetzung
#elif L==3	// en
	"See the record of a student"
#elif L==4	// es
	"Ver la ficha de un estudiante"
#elif L==5	// fr
	"See the record of a student"	// Besoin de traduction
#elif L==6	// gn
	"Ver la ficha de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"See the record of a student"	// Bisogno di traduzione
#elif L==8	// pl
	"See the record of a student"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the record of a student"	// Precisa de tradução
#endif
	,
	[ActSeeRecOneTch] =
#if   L==1	// ca
	"See the record of a teacher"	// Necessita traducció
#elif L==2	// de
	"See the record of a teacher"	// Need Übersetzung
#elif L==3	// en
	"See the record of a teacher"
#elif L==4	// es
	"Ver la ficha de un profesor"
#elif L==5	// fr
	"See the record of a teacher"	// Besoin de traduction
#elif L==6	// gn
	"Ver la ficha de un profesor"	// Okoteve traducción
#elif L==7	// it
	"See the record of a teacher"	// Bisogno di traduzione
#elif L==8	// pl
	"See the record of a teacher"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the record of a teacher"	// Precisa de tradução
#endif
	,
	[ActDoActOnSevGst] =
#if   L==1	// ca
	"Do an action on several selected guests"				// Necessita traducció
#elif L==2	// de
	"Do an action on several selected guests"				// Need Übersetzung
#elif L==3	// en
	"Do an action on several selected guests"
#elif L==4	// es
	"Realizar una acci&oacute;n sobre varios invitados seleccionados"
#elif L==5	// fr
	"Do an action on several selected guests"				// Besoin de traduction
#elif L==6	// gn
	"Realizar una acci&oacute;n sobre varios invitados seleccionados"	// Okoteve traducción
#elif L==7	// it
	"Do an action on several selected guests"				// Bisogno di traduzione
#elif L==8	// pl
	"Do an action on several selected guests"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Do an action on several selected guests"				// Precisa de tradução
#endif
	,
	[ActDoActOnSevStd] =
#if   L==1	// ca
	"Do an action on several selected students"				// Necessita traducció
#elif L==2	// de
	"Do an action on several selected students"				// Need Übersetzung
#elif L==3	// en
	"Do an action on several selected students"
#elif L==4	// es
	"Realizar una acci&oacute;n sobre varios estudiantes seleccionados"
#elif L==5	// fr
	"Do an action on several selected students"				// Besoin de traduction
#elif L==6	// gn
	"Realizar una acci&oacute;n sobre varios estudiantes seleccionados"	// Okoteve traducción
#elif L==7	// it
	"Do an action on several selected students"				// Bisogno di traduzione
#elif L==8	// pl
	"Do an action on several selected students"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Do an action on several selected students"				// Precisa de tradução
#endif
	,
	[ActDoActOnSevTch] =
#if   L==1	// ca
	"Do an action on several selected teachers"				// Necessita traducció
#elif L==2	// de
	"Do an action on several selected teachers"				// Need Übersetzung
#elif L==3	// en
	"Do an action on several selected teachers"
#elif L==4	// es
	"Realizar una acci&oacute;n sobre varios profesores seleccionados"
#elif L==5	// fr
	"Do an action on several selected teachers"				// Besoin de traduction
#elif L==6	// gn
	"Realizar una acci&oacute;n sobre varios profesores seleccionados"	// Okoteve traducción
#elif L==7	// it
	"Do an action on several selected teachers"				// Bisogno di traduzione
#elif L==8	// pl
	"Do an action on several selected teachers"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Do an action on several selected teachers"				// Precisa de tradução
#endif
	,
	[ActSeeRecSevGst] =
#if   L==1	// ca
	"See the records of several selected guests"		// Necessita traducció
#elif L==2	// de
	"See the records of several selected guests"		// Need Übersetzung
#elif L==3	// en
	"See the records of several selected guests"
#elif L==4	// es
	"Ver las fichas de varios invitados seleccionados"
#elif L==5	// fr
	"See the records of several selected guests"		// Besoin de traduction
#elif L==6	// gn
	"Ver las fichas de varios invitados seleccionados"	// Okoteve traducción
#elif L==7	// it
	"See the records of several selected guests"		// Bisogno di traduzione
#elif L==8	// pl
	"See the records of several selected guests"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the records of several selected guests"		// Precisa de tradução
#endif
	,
	[ActSeeRecSevStd] =
#if   L==1	// ca
	"See the records of several selected students"		// Necessita traducció
#elif L==2	// de
	"See the records of several selected students"		// Need Übersetzung
#elif L==3	// en
	"See the records of several selected students"
#elif L==4	// es
	"Ver las fichas de varios estudiantes seleccionados"
#elif L==5	// fr
	"See the records of several selected students"		// Besoin de traduction
#elif L==6	// gn
	"Ver las fichas de varios estudiantes seleccionados"	// Okoteve traducción
#elif L==7	// it
	"See the records of several selected students"		// Bisogno di traduzione
#elif L==8	// pl
	"See the records of several selected students"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the records of several selected students"		// Precisa de tradução
#endif
	,
	[ActSeeRecSevTch] =
#if   L==1	// ca
	"See the records of several selected teachers"		// Necessita traducció
#elif L==2	// de
	"See the records of several selected teachers"		// Need Übersetzung
#elif L==3	// en
	"See the records of several selected teachers"
#elif L==4	// es
	"Ver las fichas de varios profesores seleccionados"
#elif L==5	// fr
	"See the records of several selected teachers"		// Besoin de traduction
#elif L==6	// gn
	"Ver las fichas de varios profesores seleccionados"	// Okoteve traducción
#elif L==7	// it
	"See the records of several selected teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"See the records of several selected teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the records of several selected teachers"		// Precisa de tradução
#endif
	,
	[ActPrnRecSevGst] =
#if   L==1	// ca
	"See the records of several selected guests ready to print"		// Necessita traducció
#elif L==2	// de
	"See the records of several selected guests ready to print"		// Need Übersetzung
#elif L==3	// en
	"See the records of several selected guests ready to print"
#elif L==4	// es
	"Ver las fichas de varios invitados seleccionados lista para imprimir"
#elif L==5	// fr
	"See the records of several selected guests ready to print"		// Besoin de traduction
#elif L==6	// gn
	"Ver las fichas de varios invitados seleccionados lista para imprimir"	// Okoteve traducción
#elif L==7	// it
	"See the records of several selected guests ready to print"		// Bisogno di traduzione
#elif L==8	// pl
	"See the records of several selected guests ready to print"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the records of several selected guests ready to print"		// Precisa de tradução
#endif
	,
	[ActPrnRecSevStd] =
#if   L==1	// ca
	"See the records of several selected students ready to print"			// Necessita traducció
#elif L==2	// de
	"See the records of several selected students ready to print"			// Need Übersetzung
#elif L==3	// en
	"See the records of several selected students ready to print"
#elif L==4	// es
	"Ver las fichas de varios estudiantes seleccionados lista para imprimir"
#elif L==5	// fr
	"See the records of several selected students ready to print"			// Besoin de traduction
#elif L==6	// gn
	"Ver las fichas de varios estudiantes seleccionados lista para imprimir"	// Okoteve traducción
#elif L==7	// it
	"See the records of several selected students ready to print"			// Bisogno di traduzione
#elif L==8	// pl
	"See the records of several selected students ready to print"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the records of several selected students ready to print"			// Precisa de tradução
#endif
	,
	[ActPrnRecSevTch] =
#if   L==1	// ca
	"See the records of several selected teachers ready to print"		// Necessita traducció
#elif L==2	// de
	"See the records of several selected teachers ready to print"		// Need Übersetzung
#elif L==3	// en
	"See the records of several selected teachers ready to print"
#elif L==4	// es
	"Ver las fichas de varios profesores seleccionados lista para imprimir"
#elif L==5	// fr
	"See the records of several selected teachers ready to print"		// Besoin de traduction
#elif L==6	// gn
	"Ver las fichas de varios profesores seleccionados lista para imprimir"	// Okoteve traducción
#elif L==7	// it
	"See the records of several selected teachers ready to print"		// Bisogno di traduzione
#elif L==8	// pl
	"See the records of several selected teachers ready to print"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the records of several selected teachers ready to print"		// Precisa de tradução
#endif
	,
	[ActRcvRecOthUsr] =
#if   L==1	// ca
	"Update record fields of a student in a course"				// Necessita traducció
#elif L==2	// de
	"Update record fields of a student in a course"				// Need Übersetzung
#elif L==3	// en
	"Update record fields of a student in a course"
#elif L==4	// es
	"Actualizar los campos de la ficha del estudiante en una asignatura"
#elif L==5	// fr
	"Update record fields of a student in a course"				// Besoin de traduction
#elif L==6	// gn
	"Actualizar los campos de la ficha del estudiante en una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Update record fields of a student in a course"				// Bisogno di traduzione
#elif L==8	// pl
	"Update record fields of a student in a course"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update record fields of a student in a course"				// Precisa de tradução
#endif
	,
	[ActEdiRecFie] =
#if   L==1	// ca
	"Edit the record fields of students"		// Necessita traducció
#elif L==2	// de
	"Edit the record fields of students"		// Need Übersetzung
#elif L==3	// en
	"Edit the record fields of students"
#elif L==4	// es
	"Editar los campos de fichas de estudiantes"
#elif L==5	// fr
	"Edit the record fields of students"		// Besoin de traduction
#elif L==6	// gn
	"Editar los campos de fichas de estudiantes"	// Okoteve traducción
#elif L==7	// it
	"Edit the record fields of students"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit the record fields of students"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit the record fields of students"		// Precisa de tradução
#endif
	,
	[ActNewFie] =
#if   L==1	// ca
	"Create a record field"		// Necessita traducció
#elif L==2	// de
	"Create a record field"		// Need Übersetzung
#elif L==3	// en
	"Create a record field"
#elif L==4	// es
	"Crear un campo de ficha"
#elif L==5	// fr
	"Create a record field"		// Besoin de traduction
#elif L==6	// gn
	"Crear un campo de ficha"	// Okoteve traducción
#elif L==7	// it
	"Create a record field"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a record field"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a record field"		// Precisa de tradução
#endif
	,
	[ActReqRemFie] =
#if   L==1	// ca
	"Request removal of a record field"			// Necessita traducció
#elif L==2	// de
	"Request removal of a record field"			// Need Übersetzung
#elif L==3	// en
	"Request removal of a record field"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un campo de ficha"
#elif L==5	// fr
	"Request removal of a record field"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un campo de ficha"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a record field"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a record field"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a record field"			// Precisa de tradução
#endif
	,
	[ActRemFie] =
#if   L==1	// ca
	"Remove a record field"		// Necessita traducció
#elif L==2	// de
	"Remove a record field"		// Need Übersetzung
#elif L==3	// en
	"Remove a record field"
#elif L==4	// es
	"Eliminar un campo de ficha"
#elif L==5	// fr
	"Remove a record field"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un campo de ficha"	// Okoteve traducción
#elif L==7	// it
	"Remove a record field"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a record field"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a record field"		// Precisa de tradução
#endif
	,
	[ActRenFie] =
#if   L==1	// ca
	"Rename a record field"		// Necessita traducció
#elif L==2	// de
	"Rename a record field"		// Need Übersetzung
#elif L==3	// en
	"Rename a record field"
#elif L==4	// es
	"Renombrar un campo de ficha"
#elif L==5	// fr
	"Rename a record field"		// Besoin de traduction
#elif L==6	// gn
	"Renombrar un campo de ficha"	// Okoteve traducción
#elif L==7	// it
	"Rename a record field"		// Bisogno di traduzione
#elif L==8	// pl
	"Rename a record field"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Rename a record field"		// Precisa de tradução
#endif
	,
	[ActChgRowFie] =
#if   L==1	// ca
	"Change the number of form lines in a record field"				// Necessita traducció
#elif L==2	// de
	"Change the number of form lines in a record field"				// Need Übersetzung
#elif L==3	// en
	"Change the number of form lines in a record field"
#elif L==4	// es
	"Cambiar el n&uacute;mero de l&iacute;neas del formulario de un campo de ficha"
#elif L==5	// fr
	"Change the number of form lines in a record field"				// Besoin de traduction
#elif L==6	// gn
	"Cambiar el n&uacute;mero de l&iacute;neas del formulario de un campo de ficha"	// Okoteve traducción
#elif L==7	// it
	"Change the number of form lines in a record field"				// Bisogno di traduzione
#elif L==8	// pl
	"Change the number of form lines in a record field"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the number of form lines in a record field"				// Precisa de tradução
#endif
	,
	[ActChgVisFie] =
#if   L==1	// ca
	"Change the visibility of a record field"	// Necessita traducció
#elif L==2	// de
	"Change the visibility of a record field"	// Need Übersetzung
#elif L==3	// en
	"Change the visibility of a record field"
#elif L==4	// es
	"Cambiar la visibilidad de un campo de ficha"
#elif L==5	// fr
	"Change the visibility of a record field"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar la visibilidad de un campo de ficha"	// Okoteve traducción
#elif L==7	// it
	"Change the visibility of a record field"	// Bisogno di traduzione
#elif L==8	// pl
	"Change the visibility of a record field"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the visibility of a record field"	// Precisa de tradução
#endif
	,
	[ActRcvRecCrs] =
#if   L==1	// ca
	"Update fields of my record in a course"		// Necessita traducció
#elif L==2	// de
	"Update fields of my record in a course"		// Need Übersetzung
#elif L==3	// en
	"Update fields of my record in a course"
#elif L==4	// es
	"Actualizar campos de mi ficha en una asignatura"
#elif L==5	// fr
	"Update fields of my record in a course"		// Besoin de traduction
#elif L==6	// gn
	"Actualizar campos de mi ficha en una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Update fields of my record in a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Update fields of my record in a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update fields of my record in a course"		// Precisa de tradução
#endif
	,
	[ActFrmLogInUsrAgd] =
#if   L==1	// ca
	"See form to log in and see a user's public agenda"						// Necessita traducció
#elif L==2	// de
	"See form to log in and see a user's public agenda"						// Need Übersetzung
#elif L==3	// en
	"See form to log in and see a user's public agenda"
#elif L==4	// es
	"Ver formulario para iniciar sesi&oacute;n y ver la agenda p&uacute;blica de un usuario"
#elif L==5	// fr
	"See form to log in and see a user's public agenda"						// Besoin de traduction
#elif L==6	// gn
	"Ver formulario para iniciar sesi&oacute;n y ver la agenda p&uacute;blica de un usuario"	// Okoteve traducción
#elif L==7	// it
	"See form to log in and see a user's public agenda"						// Bisogno di traduzione
#elif L==8	// pl
	"See form to log in and see a user's public agenda"						// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See form to log in and see a user's public agenda"						// Precisa de tradução
#endif
	,
	[ActLogInUsrAgd] =
#if   L==1	// ca
	"See a user's public agenda after log in"				// Necessita traducció
#elif L==2	// de
	"See a user's public agenda after log in"				// Need Übersetzung
#elif L==3	// en
	"See a user's public agenda after log in"
#elif L==4	// es
	"Ver la agenda p&uacute;blica de un usuario tras iniciar sesi&oacute;n"
#elif L==5	// fr
	"See a user's public agenda after log in"				// Besoin de traduction
#elif L==6	// gn
	"Ver la agenda p&uacute;blica de un usuario tras iniciar sesi&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See a user's public agenda after log in"				// Bisogno di traduzione
#elif L==8	// pl
	"See a user's public agenda after log in"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See a user's public agenda after log in"				// Precisa de tradução
#endif
	,
	[ActLogInUsrAgdLan] =
#if   L==1	// ca
	"See a user's public agenda after change language"			// Necessita traducció
#elif L==2	// de
	"See a user's public agenda after change language"			// Need Übersetzung
#elif L==3	// en
	"See a user's public agenda after change language"
#elif L==4	// es
	"Ver la agenda p&uacute;blica de un usuario tras cambiar idioma"
#elif L==5	// fr
	"See a user's public agenda after change language"			// Besoin de traduction
#elif L==6	// gn
	"Ver la agenda p&uacute;blica de un usuario tras cambiar idioma"	// Okoteve traducción
#elif L==7	// it
	"See a user's public agenda after change language"			// Bisogno di traduzione
#elif L==8	// pl
	"See a user's public agenda after change language"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See a user's public agenda after change language"			// Precisa de tradução
#endif
	,
	[ActSeeUsrAgd] =
#if   L==1	// ca
	"See a user's public agenda"			// Necessita traducció
#elif L==2	// de
	"See a user's public agenda"			// Need Übersetzung
#elif L==3	// en
	"See a user's public agenda"
#elif L==4	// es
	"Ver la agenda p&uacute;blica de un usuario"
#elif L==5	// fr
	"See a user's public agenda"			// Besoin de traduction
#elif L==6	// gn
	"Ver la agenda p&uacute;blica de un usuario"	// Okoteve traducción
#elif L==7	// it
	"See a user's public agenda"			// Bisogno di traduzione
#elif L==8	// pl
	"See a user's public agenda"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See a user's public agenda"			// Precisa de tradução
#endif
	,
	[ActReqEnrSevStd] =
#if   L==1	// ca
	"Request the enrolment/removing of several students to/from a course"				// Necessita traducció
#elif L==2	// de
	"Request the enrolment/removing of several students to/from a course"				// Need Übersetzung
#elif L==3	// en
	"Request the enrolment/removing of several students to/from a course"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de varios estudiantes a/de un asignatura"
#elif L==5	// fr
	"Request the enrolment/removing of several students to/from a course"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de varios estudiantes a/de un asignatura"	// Okoteve traducción
#elif L==7	// it
	"Request the enrolment/removing of several students to/from a course"				// Bisogno di traduzione
#elif L==8	// pl
	"Request the enrolment/removing of several students to/from a course"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the enrolment/removing of several students to/from a course"				// Precisa de tradução
#endif
	,
	[ActReqEnrSevNET] =
#if   L==1	// ca
	"Request the enrolment/removing of several non-editing teachers to/from a course"				// Necessita traducció
#elif L==2	// de
	"Request the enrolment/removing of several non-editing teachers to/from a course"				// Need Übersetzung
#elif L==3	// en
	"Request the enrolment/removing of several non-editing teachers to/from a course"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de varios profesores no editores a/de un asignatura"
#elif L==5	// fr
	"Request the enrolment/removing of several non-editing teachers to/from a course"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de varios profesores no editores a/de un asignatura"	// Okoteve traducción
#elif L==7	// it
	"Request the enrolment/removing of several non-editing teachers to/from a course"				// Bisogno di traduzione
#elif L==8	// pl
	"Request the enrolment/removing of several non-editing teachers to/from a course"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the enrolment/removing of several non-editing teachers to/from a course"				// Precisa de tradução
#endif
	,
	[ActReqEnrSevTch] =
#if   L==1	// ca
	"Request the enrolment/removing of several teachers to/from a course"				// Necessita traducció
#elif L==2	// de
	"Request the enrolment/removing of several teachers to/from a course"				// Need Übersetzung
#elif L==3	// en
	"Request the enrolment/removing of several teachers to/from a course"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de varios profesores a/de un asignatura"
#elif L==5	// fr
	"Request the enrolment/removing of several teachers to/from a course"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de varios profesores a/de un asignatura"	// Okoteve traducción
#elif L==7	// it
	"Request the enrolment/removing of several teachers to/from a course"				// Bisogno di traduzione
#elif L==8	// pl
	"Request the enrolment/removing of several teachers to/from a course"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the enrolment/removing of several teachers to/from a course"				// Precisa de tradução
#endif
	,
	[ActReqLstUsrAtt] =
#if   L==1	// ca
	"Request listing of attendance of several students to several events"		// Necessita traducció
#elif L==2	// de
	"Request listing of attendance of several students to several events"		// Need Übersetzung
#elif L==3	// en
	"Request listing of attendance of several students to several events"
#elif L==4	// es
	"Solicitar el listado de asistencia de varios estudiantes a varios eventos"
#elif L==5	// fr
	"Request listing of attendance of several students to several events"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar el listado de asistencia de varios estudiantes a varios eventos"	// Okoteve traducción
#elif L==7	// it
	"Request listing of attendance of several students to several events"		// Bisogno di traduzione
#elif L==8	// pl
	"Request listing of attendance of several students to several events"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request listing of attendance of several students to several events"		// Precisa de tradução
#endif
	,
	[ActSeeLstMyAtt] =
#if   L==1	// ca
	"List my attendance as student to several events"	// Necessita traducció
#elif L==2	// de
	"List my attendance as student to several events"	// Need Übersetzung
#elif L==3	// en
	"List my attendance as student to several events"
#elif L==4	// es
	"Listar mi asistencia como estudiante a varios eventos"
#elif L==5	// fr
	"List my attendance as student to several events"	// Besoin de traduction
#elif L==6	// gn
	"Listar mi asistencia como estudiante a varios eventos"	// Okoteve traducción
#elif L==7	// it
	"List my attendance as student to several events"	// Bisogno di traduzione
#elif L==8	// pl
	"List my attendance as student to several events"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List my attendance as student to several events"	// Precisa de tradução
#endif
	,
	[ActPrnLstMyAtt] =
#if   L==1	// ca
	"Print my attendance as student to several events"		// Necessita traducció
#elif L==2	// de
	"Print my attendance as student to several events"		// Need Übersetzung
#elif L==3	// en
	"Print my attendance as student to several events"
#elif L==4	// es
	"Imprimir mi asistencia como estudiante a varios eventos"
#elif L==5	// fr
	"Print my attendance as student to several events"		// Besoin de traduction
#elif L==6	// gn
	"Imprimir mi asistencia como estudiante a varios eventos"	// Okoteve traducción
#elif L==7	// it
	"Print my attendance as student to several events"		// Bisogno di traduzione
#elif L==8	// pl
	"Print my attendance as student to several events"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Print my attendance as student to several events"		// Precisa de tradução
#endif
	,
	[ActSeeLstUsrAtt] =
#if   L==1	// ca
	"List the attendance of several students to several events"	// Necessita traducció
#elif L==2	// de
	"List the attendance of several students to several events"	// Need Übersetzung
#elif L==3	// en
	"List the attendance of several students to several events"
#elif L==4	// es
	"Listar la asistencia de varios estudiantes a varios eventos"
#elif L==5	// fr
	"List the attendance of several students to several events"	// Besoin de traduction
#elif L==6	// gn
	"Listar la asistencia de varios estudiantes a varios eventos"	// Okoteve traducción
#elif L==7	// it
	"List the attendance of several students to several events"	// Bisogno di traduzione
#elif L==8	// pl
	"List the attendance of several students to several events"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List the attendance of several students to several events"	// Precisa de tradução
#endif
	,
	[ActPrnLstUsrAtt] =
#if   L==1	// ca
	"Print attendance of several students to several events"	// Necessita traducció
#elif L==2	// de
	"Print attendance of several students to several events"	// Need Übersetzung
#elif L==3	// en
	"Print attendance of several students to several events"
#elif L==4	// es
	"Imprimir la asistencia de varios estudiantes a varios eventos"
#elif L==5	// fr
	"Print attendance of several students to several events"	// Besoin de traduction
#elif L==6	// gn
	"Imprimir la asistencia de varios estudiantes a varios eventos"	// Okoteve traducción
#elif L==7	// it
	"Print attendance of several students to several events"	// Bisogno di traduzione
#elif L==8	// pl
	"Print attendance of several students to several events"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Print attendance of several students to several events"	// Precisa de tradução
#endif
	,
	[ActFrmNewAtt] =
#if   L==1	// ca
	"Edit a new attendance event"		// Necessita traducció
#elif L==2	// de
	"Edit a new attendance event"		// Need Übersetzung
#elif L==3	// en
	"Edit a new attendance event"
#elif L==4	// es
	"Editar un nuevo evento de asistencia"
#elif L==5	// fr
	"Edit a new attendance event"		// Besoin de traduction
#elif L==6	// gn
	"Editar un nuevo evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Edit a new attendance event"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit a new attendance event"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit a new attendance event"		// Precisa de tradução
#endif
	,
	[ActEdiOneAtt] =
#if   L==1	// ca
	"Edit one attendance event"		// Necessita traducció
#elif L==2	// de
	"Edit one attendance event"		// Need Übersetzung
#elif L==3	// en
	"Edit one attendance event"
#elif L==4	// es
	"Editar un evento de asistencia"
#elif L==5	// fr
	"Edit one attendance event"		// Besoin de traduction
#elif L==6	// gn
	"Editar un evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Edit one attendance event"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit one attendance event"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit one attendance event"		// Precisa de tradução
#endif
	,
	[ActNewAtt] =
#if   L==1	// ca
	"Create a new attendance event"		// Necessita traducció
#elif L==2	// de
	"Create a new attendance event"		// Need Übersetzung
#elif L==3	// en
	"Create a new attendance event"
#elif L==4	// es
	"Crear un nuevo evento de asistencia"
#elif L==5	// fr
	"Create a new attendance event"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Create a new attendance event"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new attendance event"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new attendance event"		// Precisa de tradução
#endif
	,
	[ActChgAtt] =
#if   L==1	// ca
	"Change an attendance event"		// Necessita traducció
#elif L==2	// de
	"Change an attendance event"		// Need Übersetzung
#elif L==3	// en
	"Change an attendance event"
#elif L==4	// es
	"Cambiar un evento de asistencia"
#elif L==5	// fr
	"Change an attendance event"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar un evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Change an attendance event"		// Bisogno di traduzione
#elif L==8	// pl
	"Change an attendance event"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change an attendance event"		// Precisa de tradução
#endif
	,
	[ActReqRemAtt] =
#if   L==1	// ca
	"Request removal of an attendance event"			// Necessita traducció
#elif L==2	// de
	"Request removal of an attendance event"			// Need Übersetzung
#elif L==3	// en
	"Request removal of an attendance event"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un evento de asistencia"
#elif L==5	// fr
	"Request removal of an attendance event"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Request removal of an attendance event"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of an attendance event"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of an attendance event"			// Precisa de tradução
#endif
	,
	[ActRemAtt] =
#if   L==1	// ca
	"Remove an attendance event"		// Necessita traducció
#elif L==2	// de
	"Remove an attendance event"		// Need Übersetzung
#elif L==3	// en
	"Remove an attendance event"
#elif L==4	// es
	"Eliminar un evento de asistencia"
#elif L==5	// fr
	"Remove an attendance event"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Remove an attendance event"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove an attendance event"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove an attendance event"		// Precisa de tradução
#endif
	,
	[ActHidAtt] =
#if   L==1	// ca
	"Hide an attendance event"		// Necessita traducció
#elif L==2	// de
	"Hide an attendance event"		// Need Übersetzung
#elif L==3	// en
	"Hide an attendance event"
#elif L==4	// es
	"Ocultar un evento de asistencia"
#elif L==5	// fr
	"Hide an attendance event"		// Besoin de traduction
#elif L==6	// gn
	"Ocultar un evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Hide an attendance event"		// Bisogno di traduzione
#elif L==8	// pl
	"Hide an attendance event"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Hide an attendance event"		// Precisa de tradução
#endif
	,
	[ActShoAtt] =
#if   L==1	// ca
	"Unhide an attendance event"		// Necessita traducció
#elif L==2	// de
	"Unhide an attendance event"		// Need Übersetzung
#elif L==3	// en
	"Unhide an attendance event"
#elif L==4	// es
	"Mostrar un evento de asistencia"
#elif L==5	// fr
	"Unhide an attendance event"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar un evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Unhide an attendance event"		// Bisogno di traduzione
#elif L==8	// pl
	"Unhide an attendance event"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unhide an attendance event"		// Precisa de tradução
#endif
	,
	[ActSeeOneAtt] =
#if   L==1	// ca
	"List students who attended to an event"	// Necessita traducció
#elif L==2	// de
	"List students who attended to an event"	// Need Übersetzung
#elif L==3	// en
	"List students who attended to an event"
#elif L==4	// es
	"Listar estudiantes que asistieron a un evento"
#elif L==5	// fr
	"List students who attended to an event"	// Besoin de traduction
#elif L==6	// gn
	"Listar estudiantes que asistieron a un evento"	// Okoteve traducción
#elif L==7	// it
	"List students who attended to an event"	// Bisogno di traduzione
#elif L==8	// pl
	"List students who attended to an event"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List students who attended to an event"	// Precisa de tradução
#endif
	,
	[ActRecAttStd] =
#if   L==1	// ca
	"Save students who attended to an event and comments"		// Necessita traducció
#elif L==2	// de
	"Save students who attended to an event and comments"		// Need Übersetzung
#elif L==3	// en
	"Save students who attended to an event and comments"
#elif L==4	// es
	"Guardar estudiantes que asistieron a un evento y comentarios"
#elif L==5	// fr
	"Save students who attended to an event and comments"		// Besoin de traduction
#elif L==6	// gn
	"Guardar estudiantes que asistieron a un evento y comentarios"	// Okoteve traducción
#elif L==7	// it
	"Save students who attended to an event and comments"		// Bisogno di traduzione
#elif L==8	// pl
	"Save students who attended to an event and comments"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Save students who attended to an event and comments"		// Precisa de tradução
#endif
	,
	[ActRecAttMe] =
#if   L==1	// ca
	"Save my comments as student in an attendance event"			// Necessita traducció
#elif L==2	// de
	"Save my comments as student in an attendance event"			// Need Übersetzung
#elif L==3	// en
	"Save my comments as student in an attendance event"
#elif L==4	// es
	"Guardar mis comentarios como estudiante en un evento de asistencia"
#elif L==5	// fr
	"Save my comments as student in an attendance event"			// Besoin de traduction
#elif L==6	// gn
	"Guardar mis comentarios como estudiante en un evento de asistencia"	// Okoteve traducción
#elif L==7	// it
	"Save my comments as student in an attendance event"			// Bisogno di traduzione
#elif L==8	// pl
	"Save my comments as student in an attendance event"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Save my comments as student in an attendance event"			// Precisa de tradução
#endif
	,
	[ActSignUp] =
#if   L==1	// ca
	"Apply for my enrolment"		// Necessita traducció
#elif L==2	// de
	"Apply for my enrolment"		// Need Übersetzung
#elif L==3	// en
	"Apply for my enrolment"
#elif L==4	// es
	"Solicitar mi inscripci&oacute;n"
#elif L==5	// fr
	"Apply for my enrolment"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar mi inscripci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Apply for my enrolment"		// Bisogno di traduzione
#elif L==8	// pl
	"Apply for my enrolment"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Apply for my enrolment"		// Precisa de tradução
#endif
	,
	[ActUpdSignUpReq] =
#if   L==1	// ca
	"Update pending requests for inscription"			// Necessita traducció
#elif L==2	// de
	"Update pending requests for inscription"			// Need Übersetzung
#elif L==3	// en
	"Update pending requests for inscription"
#elif L==4	// es
	"Actualizar solicitudes de inscripci&oacute;n pendientes"
#elif L==5	// fr
	"Update pending requests for inscription"			// Besoin de traduction
#elif L==6	// gn
	"Actualizar solicitudes de inscripci&oacute;n pendientes"	// Okoteve traducción
#elif L==7	// it
	"Update pending requests for inscription"			// Bisogno di traduzione
#elif L==8	// pl
	"Update pending requests for inscription"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update pending requests for inscription"			// Precisa de tradução
#endif
	,
	[ActReqRejSignUp] =
#if   L==1	// ca
	"Request the rejection of a user's enrolment"			// Necessita traducció
#elif L==2	// de
	"Request the rejection of a user's enrolment"			// Need Übersetzung
#elif L==3	// en
	"Request the rejection of a user's enrolment"
#elif L==4	// es
	"Solicitar el rechazo a la inscripci&oacute;n de un usuario"
#elif L==5	// fr
	"Request the rejection of a user's enrolment"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar el rechazo a la inscripci&oacute;n de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Request the rejection of a user's enrolment"			// Bisogno di traduzione
#elif L==8	// pl
	"Request the rejection of a user's enrolment"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the rejection of a user's enrolment"			// Precisa de tradução
#endif
	,
	[ActRejSignUp] =
#if   L==1	// ca
	"Reject the enrolment"			// Necessita traducció
#elif L==2	// de
	"Reject the enrolment"			// Need Übersetzung
#elif L==3	// en
	"Reject the enrolment"
#elif L==4	// es
	"Rechazar la inscripci&oacute;n"
#elif L==5	// fr
	"Reject the enrolment"			// Besoin de traduction
#elif L==6	// gn
	"Rechazar la inscripci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Reject the enrolment"			// Bisogno di traduzione
#elif L==8	// pl
	"Reject the enrolment"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Reject the enrolment"			// Precisa de tradução
#endif
	,
	[ActReqMdfOneOth] =
#if   L==1	// ca
	"Request a user's ID for enrolment/removal"					// Necessita traducció
#elif L==2	// de
	"Request a user's ID for enrolment/removal"					// Need Übersetzung
#elif L==3	// en
	"Request a user's ID for enrolment/removal"
#elif L==4	// es
	"Solicitar el ID de un usuario para inscripci&oacute;n/eliminaci&oacute;n"
#elif L==5	// fr
	"Request a user's ID for enrolment/removal"					// Besoin de traduction
#elif L==6	// gn
	"Solicitar el ID de un usuario para inscripci&oacute;n/eliminaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request a user's ID for enrolment/removal"					// Bisogno di traduzione
#elif L==8	// pl
	"Request a user's ID for enrolment/removal"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request a user's ID for enrolment/removal"					// Precisa de tradução
#endif
	,
	[ActReqMdfOneStd] =
#if   L==1	// ca
	"Request a student's ID for enrolment/removal"					// Necessita traducció
#elif L==2	// de
	"Request a student's ID for enrolment/removal"					// Need Übersetzung
#elif L==3	// en
	"Request a student's ID for enrolment/removal"
#elif L==4	// es
	"Solicitar el ID de un estudiante para inscripci&oacute;n/eliminaci&oacute;n"
#elif L==5	// fr
	"Request a student's ID for enrolment/removal"					// Besoin de traduction
#elif L==6	// gn
	"Solicitar el ID de un estudiante para inscripci&oacute;n/eliminaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request a student's ID for enrolment/removal"					// Bisogno di traduzione
#elif L==8	// pl
	"Request a student's ID for enrolment/removal"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request a student's ID for enrolment/removal"					// Precisa de tradução
#endif
	,
	[ActReqMdfOneTch] =
#if   L==1	// ca
	"Request a teacher's ID for enrolment/removal"					// Necessita traducció
#elif L==2	// de
	"Request a teacher's ID for enrolment/removal"					// Need Übersetzung
#elif L==3	// en
	"Request a teacher's ID for enrolment/removal"
#elif L==4	// es
	"Solicitar el ID de un profesor para inscripci&oacute;n/eliminaci&oacute;n"
#elif L==5	// fr
	"Request a teacher's ID for enrolment/removal"					// Besoin de traduction
#elif L==6	// gn
	"Solicitar el ID de un profesor para inscripci&oacute;n/eliminaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request a teacher's ID for enrolment/removal"					// Bisogno di traduzione
#elif L==8	// pl
	"Request a teacher's ID for enrolment/removal"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request a teacher's ID for enrolment/removal"					// Precisa de tradução
#endif
	,
	[ActReqMdfOth] =
#if   L==1	// ca
	"Request enrolment/removal of a user"				// Necessita traducció
#elif L==2	// de
	"Request enrolment/removal of a user"				// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removal of a user"
#elif L==4	// es
	"Solicitar la inscripcion/eliminaci&oacute;n de un usuario"
#elif L==5	// fr
	"Request enrolment/removal of a user"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripcion/eliminaci&oacute;n de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Request enrolment/removal of a user"				// Bisogno di traduzione
#elif L==8	// pl
	"Request enrolment/removal of a user"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request enrolment/removal of a user"				// Precisa de tradução
#endif
	,
	[ActReqMdfStd] =
#if   L==1	// ca
	"Request enrolment/removal of a student"				// Necessita traducció
#elif L==2	// de
	"Request enrolment/removal of a student"				// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removal of a student"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de un estudiante"
#elif L==5	// fr
	"Request enrolment/removal of a student"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Request enrolment/removal of a student"				// Bisogno di traduzione
#elif L==8	// pl
	"Request enrolment/removal of a student"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request enrolment/removal of a student"				// Precisa de tradução
#endif
	,
	[ActReqMdfNET] =
#if   L==1	// ca
	"Request enrolment/removal of a non-editing teacher"				// Necessita traducció
#elif L==2	// de
	"Request enrolment/removal of a non-editing teacher"				// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removal of a non-editing teacher"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de un profesor no editor"
#elif L==5	// fr
	"Request enrolment/removal of a non-editing teacher"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de un profesor no editor"	// Okoteve traducción
#elif L==7	// it
	"Request enrolment/removal of a non-editing teacher"				// Bisogno di traduzione
#elif L==8	// pl
	"Request enrolment/removal of a non-editing teacher"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request enrolment/removal of a non-editing teacher"				// Precisa de tradução
#endif
	,
	[ActReqMdfTch] =
#if   L==1	// ca
	"Request enrolment/removal of a teacher"				// Necessita traducció
#elif L==2	// de
	"Request enrolment/removal of a teacher"				// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removal of a teacher"
#elif L==4	// es
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de un profesor"
#elif L==5	// fr
	"Request enrolment/removal of a teacher"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la inscripci&oacute;n/eliminaci&oacute;n de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Request enrolment/removal of a teacher"				// Bisogno di traduzione
#elif L==8	// pl
	"Request enrolment/removal of a teacher"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request enrolment/removal of a teacher"				// Precisa de tradução
#endif
	,
	[ActReqOthPho] =
#if   L==1	// ca
	"Edit a user's photo"		// Necessita traducció
#elif L==2	// de
	"Edit a user's photo"		// Need Übersetzung
#elif L==3	// en
	"Edit a user's photo"
#elif L==4	// es
	"Editar la foto de un usuario"
#elif L==5	// fr
	"Edit a user's photo"		// Besoin de traduction
#elif L==6	// gn
	"Editar la foto de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Edit a user's photo"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit a user's photo"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit a user's photo"		// Precisa de tradução
#endif
	,
	[ActReqStdPho] =
#if   L==1	// ca
	"Edit a student's photo"		// Necessita traducció
#elif L==2	// de
	"Edit a student's photo"		// Need Übersetzung
#elif L==3	// en
	"Edit a student's photo"
#elif L==4	// es
	"Editar la foto de un estudiante"
#elif L==5	// fr
	"Edit a student's photo"		// Besoin de traduction
#elif L==6	// gn
	"Editar la foto de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Edit a student's photo"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit a student's photo"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit a student's photo"		// Precisa de tradução
#endif
	,
	[ActReqTchPho] =
#if   L==1	// ca
	"Edit a teacher's photo"	// Necessita traducció
#elif L==2	// de
	"Edit a teacher's photo"	// Need Übersetzung
#elif L==3	// en
	"Edit a teacher's photo"
#elif L==4	// es
	"Editar la foto de un profesor"
#elif L==5	// fr
	"Edit a teacher's photo"	// Besoin de traduction
#elif L==6	// gn
	"Editar la foto de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Edit a teacher's photo"	// Bisogno di traduzione
#elif L==8	// pl
	"Edit a teacher's photo"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit a teacher's photo"	// Precisa de tradução
#endif
	,
	[ActDetOthPho] =
#if   L==1	// ca
	"Send a user's photo and detect faces on it"			// Necessita traducció
#elif L==2	// de
	"Send a user's photo and detect faces on it"			// Need Übersetzung
#elif L==3	// en
	"Send a user's photo and detect faces on it"
#elif L==4	// es
	"Enviar la foto de un usuario y detectar rostros en ella"
#elif L==5	// fr
	"Send a user's photo and detect faces on it"			// Besoin de traduction
#elif L==6	// gn
	"Enviar la foto de un usuario y detectar rostros en ella"	// Okoteve traducción
#elif L==7	// it
	"Send a user's photo and detect faces on it"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a user's photo and detect faces on it"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a user's photo and detect faces on it"			// Precisa de tradução
#endif
	,
	[ActDetStdPho] =
#if   L==1	// ca
	"Send a student's photo and detect faces on it"			// Necessita traducció
#elif L==2	// de
	"Send a student's photo and detect faces on it"			// Need Übersetzung
#elif L==3	// en
	"Send a student's photo and detect faces on it"
#elif L==4	// es
	"Enviar la foto de un estudiante y detectar rostros en ella"
#elif L==5	// fr
	"Send a student's photo and detect faces on it"			// Besoin de traduction
#elif L==6	// gn
	"Enviar la foto de un estudiante y detectar rostros en ella"	// Okoteve traducción
#elif L==7	// it
	"Send a student's photo and detect faces on it"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a student's photo and detect faces on it"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a student's photo and detect faces on it"			// Precisa de tradução
#endif
	,
	[ActDetTchPho] =
#if   L==1	// ca
	"Send a teacher's photo and detect faces on it"			// Necessita traducció
#elif L==2	// de
	"Send a teacher's photo and detect faces on it"			// Need Übersetzung
#elif L==3	// en
	"Send a teacher's photo and detect faces on it"
#elif L==4	// es
	"Enviar la foto de un profesor y detectar rostros en ella"
#elif L==5	// fr
	"Send a teacher's photo and detect faces on it"			// Besoin de traduction
#elif L==6	// gn
	"Enviar la foto de un profesor y detectar rostros en ella"	// Okoteve traducción
#elif L==7	// it
	"Send a teacher's photo and detect faces on it"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a teacher's photo and detect faces on it"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a teacher's photo and detect faces on it"			// Precisa de tradução
#endif
	,
	[ActUpdOthPho] =
#if   L==1	// ca
	"Update a user's photo"			// Necessita traducció
#elif L==2	// de
	"Update a user's photo"			// Need Übersetzung
#elif L==3	// en
	"Update a user's photo"
#elif L==4	// es
	"Actualizar la foto de un usuario"
#elif L==5	// fr
	"Update a user's photo"			// Besoin de traduction
#elif L==6	// gn
	"Actualizar la foto de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Update a user's photo"			// Bisogno di traduzione
#elif L==8	// pl
	"Update a user's photo"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update a user's photo"			// Precisa de tradução
#endif
	,
	[ActUpdStdPho] =
#if   L==1	// ca
	"Update a student's photo"		// Necessita traducció
#elif L==2	// de
	"Update a student's photo"		// Need Übersetzung
#elif L==3	// en
	"Update a student's photo"
#elif L==4	// es
	"Actualizar la foto de un estudiante"
#elif L==5	// fr
	"Update a student's photo"		// Besoin de traduction
#elif L==6	// gn
	"Actualizar la foto de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Update a student's photo"		// Bisogno di traduzione
#elif L==8	// pl
	"Update a student's photo"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update a student's photo"		// Precisa de tradução
#endif
	,
	[ActUpdTchPho] =
#if   L==1	// ca
	"Update a teacher's photo"		// Necessita traducció
#elif L==2	// de
	"Update a teacher's photo"		// Need Übersetzung
#elif L==3	// en
	"Update a teacher's photo"
#elif L==4	// es
	"Actualizar la foto de un profesor"
#elif L==5	// fr
	"Update a teacher's photo"		// Besoin de traduction
#elif L==6	// gn
	"Actualizar la foto de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Update a teacher's photo"		// Bisogno di traduzione
#elif L==8	// pl
	"Update a teacher's photo"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update a teacher's photo"		// Precisa de tradução
#endif
	,
	[ActReqRemOthPho] =
#if   L==1	// ca
	"Request removal of a user's photo"				// Necessita traducció
#elif L==2	// de
	"Request removal of a user's photo"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a user's photo"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de la foto de un usuario"
#elif L==5	// fr
	"Request removal of a user's photo"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de la foto de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a user's photo"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a user's photo"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a user's photo"				// Precisa de tradução
#endif
	,
	[ActReqRemStdPho] =
#if   L==1	// ca
	"Request removal of a student's photo"				// Necessita traducció
#elif L==2	// de
	"Request removal of a student's photo"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a student's photo"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de la foto de un estudiante"
#elif L==5	// fr
	"Request removal of a student's photo"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de la foto de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a student's photo"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a student's photo"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a student's photo"				// Precisa de tradução
#endif
	,
	[ActReqRemTchPho] =
#if   L==1	// ca
	"Request removal of a teacher's photo"				// Necessita traducció
#elif L==2	// de
	"Request removal of a teacher's photo"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a teacher's photo"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de la foto de un profesor"
#elif L==5	// fr
	"Request removal of a teacher's photo"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de la foto de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a teacher's photo"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a teacher's photo"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a teacher's photo"				// Precisa de tradução
#endif
	,
	[ActRemOthPho] =
#if   L==1	// ca
	"Remove a user's photo"			// Necessita traducció
#elif L==2	// de
	"Remove a user's photo"			// Need Übersetzung
#elif L==3	// en
	"Remove a user's photo"
#elif L==4	// es
	"Eliminar la foto de un usuario"
#elif L==5	// fr
	"Remove a user's photo"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar la foto de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Remove a user's photo"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a user's photo"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a user's photo"			// Precisa de tradução
#endif
	,
	[ActRemStdPho] =
#if   L==1	// ca
	"Remove a student's photo"		// Necessita traducció
#elif L==2	// de
	"Remove a student's photo"		// Need Übersetzung
#elif L==3	// en
	"Remove a student's photo"
#elif L==4	// es
	"Eliminar la foto de un estudiante"
#elif L==5	// fr
	"Remove a student's photo"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar la foto de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Remove a student's photo"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a student's photo"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a student's photo"		// Precisa de tradução
#endif
	,
	[ActRemTchPho] =
#if   L==1	// ca
	"Remove a teacher's photo"		// Necessita traducció
#elif L==2	// de
	"Remove a teacher's photo"		// Need Übersetzung
#elif L==3	// en
	"Remove a teacher's photo"
#elif L==4	// es
	"Eliminar la foto de un profesor"
#elif L==5	// fr
	"Remove a teacher's photo"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar la foto de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Remove a teacher's photo"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a teacher's photo"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a teacher's photo"		// Precisa de tradução
#endif
	,
	[ActCreOth] =
#if   L==1	// ca
	"Create a new guest"		// Necessita traducció
#elif L==2	// de
	"Create a new guest"		// Need Übersetzung
#elif L==3	// en
	"Create a new guest"
#elif L==4	// es
	"Crear un nuevo invitado"
#elif L==5	// fr
	"Create a new guest"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo invitado"	// Okoteve traducción
#elif L==7	// it
	"Create a new guest"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new guest"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new guest"		// Precisa de tradução
#endif
	,
	[ActCreStd] =
#if   L==1	// ca
	"Create a new student"		// Necessita traducció
#elif L==2	// de
	"Create a new student"		// Need Übersetzung
#elif L==3	// en
	"Create a new student"
#elif L==4	// es
	"Crear un nuevo estudiante"
#elif L==5	// fr
	"Create a new student"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo estudiante"	// Okoteve traducción
#elif L==7	// it
	"Create a new student"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new student"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new student"		// Precisa de tradução
#endif
	,
	[ActCreNET] =
#if   L==1	// ca
	"Create a new non-editing teacher"	// Necessita traducció
#elif L==2	// de
	"Create a new non-editing teacher"	// Need Übersetzung
#elif L==3	// en
	"Create a new non-editing teacher"
#elif L==4	// es
	"Crear un nuevo profesor no editor"
#elif L==5	// fr
	"Create a new non-editing teacher"	// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo profesor no editor"	// Okoteve traducción
#elif L==7	// it
	"Create a new non-editing teacher"	// Bisogno di traduzione
#elif L==8	// pl
	"Create a new non-editing teacher"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new non-editing teacher"	// Precisa de tradução
#endif
	,
	[ActCreTch] =
#if   L==1	// ca
	"Create a new teacher"		// Necessita traducció
#elif L==2	// de
	"Create a new teacher"		// Need Übersetzung
#elif L==3	// en
	"Create a new teacher"
#elif L==4	// es
	"Crear un nuevo profesor"
#elif L==5	// fr
	"Create a new teacher"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo profesor"	// Okoteve traducción
#elif L==7	// it
	"Create a new teacher"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new teacher"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new teacher"		// Precisa de tradução
#endif
	,
	[ActUpdOth] =
#if   L==1	// ca
	"Update a user's data and groups"		// Necessita traducció
#elif L==2	// de
	"Update a user's data and groups"		// Need Übersetzung
#elif L==3	// en
	"Update a user's data and groups"
#elif L==4	// es
	"Actualizar los datos y grupos de un usuario"
#elif L==5	// fr
	"Update a user's data and groups"		// Besoin de traduction
#elif L==6	// gn
	"Actualizar los datos y grupos de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Update a user's data and groups"		// Bisogno di traduzione
#elif L==8	// pl
	"Update a user's data and groups"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update a user's data and groups"		// Precisa de tradução
#endif
	,
	[ActUpdStd] =
#if   L==1	// ca
	"Update a student's data and groups"			// Necessita traducció
#elif L==2	// de
	"Update a student's data and groups"			// Need Übersetzung
#elif L==3	// en
	"Update a student's data and groups"
#elif L==4	// es
	"Actualizar los datos y grupos de un estudiante"
#elif L==5	// fr
	"Update a student's data and groups"			// Besoin de traduction
#elif L==6	// gn
	"Actualizar los datos y grupos de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Update a student's data and groups"			// Bisogno di traduzione
#elif L==8	// pl
	"Update a student's data and groups"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update a student's data and groups"			// Precisa de tradução
#endif
	,
	[ActUpdNET] =
#if   L==1	// ca
	"Update a non-editing teacher's data and groups"		// Necessita traducció
#elif L==2	// de
	"Update a non-editing teacher's data and groups"		// Need Übersetzung
#elif L==3	// en
	"Update a non-editing teacher's data and groups"
#elif L==4	// es
	"Actualizar los datos y grupos de un profesor no editor"
#elif L==5	// fr
	"Update a non-editing teacher's data and groups"		// Besoin de traduction
#elif L==6	// gn
	"Actualizar los datos y grupos de un profesor no editor"	// Okoteve traducción
#elif L==7	// it
	"Update a non-editing teacher's data and groups"		// Bisogno di traduzione
#elif L==8	// pl
	"Update a non-editing teacher's data and groups"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update a non-editing teacher's data and groups"		// Precisa de tradução
#endif
	,
	[ActUpdTch] =
#if   L==1	// ca
	"Update a teacher's data and groups"		// Necessita traducció
#elif L==2	// de
	"Update a teacher's data and groups"		// Need Übersetzung
#elif L==3	// en
	"Update a teacher's data and groups"
#elif L==4	// es
	"Actualizar los datos y grupos de un profesor"
#elif L==5	// fr
	"Update a teacher's data and groups"		// Besoin de traduction
#elif L==6	// gn
	"Actualizar los datos y grupos de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Update a teacher's data and groups"		// Bisogno di traduzione
#elif L==8	// pl
	"Update a teacher's data and groups"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update a teacher's data and groups"		// Precisa de tradução
#endif
	,
	[ActReqAccEnrStd] =
#if   L==1	// ca
	"See whether to accept or reject my enrolment as student"		// Necessita traducció
#elif L==2	// de
	"See whether to accept or reject my enrolment as student"		// Need Übersetzung
#elif L==3	// en
	"See whether to accept or reject my enrolment as student"
#elif L==4	// es
	"Ver si aceptar o rechazar mi inscripci&oacute;n como estudiante"
#elif L==5	// fr
	"See whether to accept or reject my enrolment as student"		// Besoin de traduction
#elif L==6	// gn
	"Ver si aceptar o rechazar mi inscripci&oacute;n como estudiante"	// Okoteve traducción
#elif L==7	// it
	"See whether to accept or reject my enrolment as student"		// Bisogno di traduzione
#elif L==8	// pl
	"See whether to accept or reject my enrolment as student"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See whether to accept or reject my enrolment as student"		// Precisa de tradução
#endif
	,
	[ActReqAccEnrNET] =
#if   L==1	// ca
	"See whether to accept or reject my enrolment as non-editing teacher"		// Necessita traducció
#elif L==2	// de
	"See whether to accept or reject my enrolment as non-editing teacher"		// Need Übersetzung
#elif L==3	// en
	"See whether to accept or reject my enrolment as non-editing teacher"
#elif L==4	// es
	"Ver si aceptar o rechazar mi inscripci&oacute;n como profesor no editor"
#elif L==5	// fr
	"See whether to accept or reject my enrolment as non-editing teacher"		// Besoin de traduction
#elif L==6	// gn
	"Ver si aceptar o rechazar mi inscripci&oacute;n como profesor no editor"	// Okoteve traducción
#elif L==7	// it
	"See whether to accept or reject my enrolment as non-editing teacher"		// Bisogno di traduzione
#elif L==8	// pl
	"See whether to accept or reject my enrolment as non-editing teacher"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See whether to accept or reject my enrolment as non-editing teacher"		// Precisa de tradução
#endif
	,
	[ActReqAccEnrTch] =
#if   L==1	// ca
	"See whether to accept or reject my enrolment as teacher"	// Necessita traducció
#elif L==2	// de
	"See whether to accept or reject my enrolment as teacher"	// Need Übersetzung
#elif L==3	// en
	"See whether to accept or reject my enrolment as teacher"
#elif L==4	// es
	"Ver si aceptar o rechazar mi inscripci&oacute;n como profesor"
#elif L==5	// fr
	"See whether to accept or reject my enrolment as teacher"	// Besoin de traduction
#elif L==6	// gn
	"Ver si aceptar o rechazar mi inscripci&oacute;n como profesor"	// Okoteve traducción
#elif L==7	// it
	"See whether to accept or reject my enrolment as teacher"	// Bisogno di traduzione
#elif L==8	// pl
	"See whether to accept or reject my enrolment as teacher"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See whether to accept or reject my enrolment as teacher"	// Precisa de tradução
#endif
	,
	[ActAccEnrStd] =
#if   L==1	// ca
	"Accept my enrolment as student"		// Necessita traducció
#elif L==2	// de
	"Accept my enrolment as student"		// Need Übersetzung
#elif L==3	// en
	"Accept my enrolment as student"
#elif L==4	// es
	"Aceptar mi inscripci&oacute;n como estudiante"
#elif L==5	// fr
	"Accept my enrolment as student"		// Besoin de traduction
#elif L==6	// gn
	"Aceptar mi inscripci&oacute;n como estudiante"	// Okoteve traducción
#elif L==7	// it
	"Accept my enrolment as student"		// Bisogno di traduzione
#elif L==8	// pl
	"Accept my enrolment as student"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Accept my enrolment as student"		// Precisa de tradução
#endif
	,
	[ActAccEnrNET] =
#if   L==1	// ca
	"Accept my enrolment as non-editing teacher"		// Necessita traducció
#elif L==2	// de
	"Accept my enrolment as non-editing teacher"		// Need Übersetzung
#elif L==3	// en
	"Accept my enrolment as non-editing teacher"
#elif L==4	// es
	"Aceptar mi inscripci&oacute;n como profesor no editor"
#elif L==5	// fr
	"Accept my enrolment as non-editing teacher"		// Besoin de traduction
#elif L==6	// gn
	"Aceptar mi inscripci&oacute;n como profesor no editor"	// Okoteve traducción
#elif L==7	// it
	"Accept my enrolment as non-editing teacher"		// Bisogno di traduzione
#elif L==8	// pl
	"Accept my enrolment as non-editing teacher"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Accept my enrolment as non-editing teacher"		// Precisa de tradução
#endif
	,
	[ActAccEnrTch] =
#if   L==1	// ca
	"Accept my enrolment as teacher"		// Necessita traducció
#elif L==2	// de
	"Accept my enrolment as teacher"		// Need Übersetzung
#elif L==3	// en
	"Accept my enrolment as teacher"
#elif L==4	// es
	"Aceptar mi inscripci&oacute;n como profesor"
#elif L==5	// fr
	"Accept my enrolment as teacher"		// Besoin de traduction
#elif L==6	// gn
	"Aceptar mi inscripci&oacute;n como profesor"	// Okoteve traducción
#elif L==7	// it
	"Accept my enrolment as teacher"		// Bisogno di traduzione
#elif L==8	// pl
	"Accept my enrolment as teacher"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Accept my enrolment as teacher"		// Precisa de tradução
#endif
	,
	[ActRemMe_Std] =
#if   L==1	// ca
	"Reject my enrolment as student"			// Necessita traducció
#elif L==2	// de
	"Reject my enrolment as student"			// Need Übersetzung
#elif L==3	// en
	"Reject my enrolment as student"
#elif L==4	// es
	"Rechazar mi inscripci&oacute;n como estudiante"
#elif L==5	// fr
	"Reject my enrolment as student"			// Besoin de traduction
#elif L==6	// gn
	"Rechazar mi inscripci&oacute;n como estudiante"	// Okoteve traducción
#elif L==7	// it
	"Reject my enrolment as student"			// Bisogno di traduzione
#elif L==8	// pl
	"Reject my enrolment as student"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Reject my enrolment as student"			// Precisa de tradução
#endif
	,
	[ActRemMe_NET] =
#if   L==1	// ca
	"Reject my enrolment as non-editing teacher"			// Necessita traducció
#elif L==2	// de
	"Reject my enrolment as non-editing teacher"			// Need Übersetzung
#elif L==3	// en
	"Reject my enrolment as non-editing teacher"
#elif L==4	// es
	"Rechazar mi inscripci&oacute;n como profesor no editor"
#elif L==5	// fr
	"Reject my enrolment as non-editing teacher"			// Besoin de traduction
#elif L==6	// gn
	"Rechazar mi inscripci&oacute;n como profesor no editor"	// Okoteve traducción
#elif L==7	// it
	"Reject my enrolment as non-editing teacher"			// Bisogno di traduzione
#elif L==8	// pl
	"Reject my enrolment as non-editing teacher"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Reject my enrolment as non-editing teacher"			// Precisa de tradução
#endif
	,
	[ActRemMe_Tch] =
#if   L==1	// ca
	"Reject my enrolment as teacher"		// Necessita traducció
#elif L==2	// de
	"Reject my enrolment as teacher"		// Need Übersetzung
#elif L==3	// en
	"Reject my enrolment as teacher"
#elif L==4	// es
	"Rechazar mi inscripci&oacute;n como profesor"
#elif L==5	// fr
	"Reject my enrolment as teacher"		// Besoin de traduction
#elif L==6	// gn
	"Rechazar mi inscripci&oacute;n como profesor"	// Okoteve traducción
#elif L==7	// it
	"Reject my enrolment as teacher"		// Bisogno di traduzione
#elif L==8	// pl
	"Reject my enrolment as teacher"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Reject my enrolment as teacher"		// Precisa de tradução
#endif
	,
	[ActNewAdmIns] =
#if   L==1	// ca
	"Register an administrador in an institution"		// Necessita traducció
#elif L==2	// de
	"Register an administrador in an institution"		// Need Übersetzung
#elif L==3	// en
	"Register an administrador in an institution"
#elif L==4	// es
	"Inscribir un administrador en una instituci&oacute;n"
#elif L==5	// fr
	"Register an administrador in an institution"		// Besoin de traduction
#elif L==6	// gn
	"Inscribir un administrador en una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Register an administrador in an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Register an administrador in an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Register an administrador in an institution"		// Precisa de tradução
#endif
	,
	[ActRemAdmIns] =
#if   L==1	// ca
	"Remove an administrador from an institution"		// Necessita traducció
#elif L==2	// de
	"Remove an administrador from an institution"		// Need Übersetzung
#elif L==3	// en
	"Remove an administrador from an institution"
#elif L==4	// es
	"Eliminar un administrador de una instituci&oacute;n"
#elif L==5	// fr
	"Remove an administrador from an institution"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un administrador de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove an administrador from an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove an administrador from an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove an administrador from an institution"		// Precisa de tradução
#endif
	,
	[ActNewAdmCtr] =
#if   L==1	// ca
	"Register an administrador in a center"		// Necessita traducció
#elif L==2	// de
	"Register an administrador in a center"		// Need Übersetzung
#elif L==3	// en
	"Register an administrador in a center"
#elif L==4	// es
	"Inscribir un administrador en un centro"
#elif L==5	// fr
	"Register an administrador in a center"		// Besoin de traduction
#elif L==6	// gn
	"Inscribir un administrador en un centro"	// Okoteve traducción
#elif L==7	// it
	"Register an administrador in a center"		// Bisogno di traduzione
#elif L==8	// pl
	"Register an administrador in a center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Register an administrador in a center"		// Precisa de tradução
#endif
	,
	[ActRemAdmCtr] =
#if   L==1	// ca
	"Remove an administrador from a center"		// Necessita traducció
#elif L==2	// de
	"Remove an administrador from a center"		// Need Übersetzung
#elif L==3	// en
	"Remove an administrador from a center"
#elif L==4	// es
	"Eliminar un administrador de un centro"
#elif L==5	// fr
	"Remove an administrador from a center"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un administrador de un centro"	// Okoteve traducción
#elif L==7	// it
	"Remove an administrador from a center"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove an administrador from a center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove an administrador from a center"		// Precisa de tradução
#endif
	,
	[ActNewAdmDeg] =
#if   L==1	// ca
	"Register an administrador in a degree"			// Necessita traducció
#elif L==2	// de
	"Register an administrador in a degree"			// Need Übersetzung
#elif L==3	// en
	"Register an administrador in a degree"
#elif L==4	// es
	"Inscribir un administrador en una titulaci&oacute;n"
#elif L==5	// fr
	"Register an administrador in a degree"			// Besoin de traduction
#elif L==6	// gn
	"Inscribir un administrador en una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Register an administrador in a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Register an administrador in a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Register an administrador in a degree"			// Precisa de tradução
#endif
	,
	[ActRemAdmDeg] =
#if   L==1	// ca
	"Remove an administrador from a degree"			// Necessita traducció
#elif L==2	// de
	"Remove an administrador from a degree"			// Need Übersetzung
#elif L==3	// en
	"Remove an administrador from a degree"
#elif L==4	// es
	"Eliminar un administrador en una titulaci&oacute;n"
#elif L==5	// fr
	"Remove an administrador from a degree"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un administrador en una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove an administrador from a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove an administrador from a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove an administrador from a degree"			// Precisa de tradução
#endif
	,
	[ActRcvFrmEnrSevStd] =
#if   L==1	// ca
	"Register/remove several students in/from a course"		// Necessita traducció
#elif L==2	// de
	"Register/remove several students in/from a course"		// Need Übersetzung
#elif L==3	// en
	"Register/remove several students in/from a course"
#elif L==4	// es
	"Inscribir/eliminar varios estudiantes en/de una asignatura"
#elif L==5	// fr
	"Register/remove several students in/from a course"		// Besoin de traduction
#elif L==6	// gn
	"Inscribir/eliminar varios estudiantes en/de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Register/remove several students in/from a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Register/remove several students in/from a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Register/remove several students in/from a course"		// Precisa de tradução
#endif
	,
	[ActRcvFrmEnrSevNET] =
#if   L==1	// ca
	"Register/remove several non-editing teachers in/from a course"		// Necessita traducció
#elif L==2	// de
	"Register/remove several non-editing teachers in/from a course"		// Need Übersetzung
#elif L==3	// en
	"Register/remove several non-editing teachers in/from a course"
#elif L==4	// es
	"Inscribir/eliminar varios profesores no editores en/de una asignatura"
#elif L==5	// fr
	"Register/remove several non-editing teachers in/from a course"		// Besoin de traduction
#elif L==6	// gn
	"Inscribir/eliminar varios profesores no editores en/de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Register/remove several non-editing teachers in/from a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Register/remove several non-editing teachers in/from a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Register/remove several non-editing teachers in/from a course"		// Precisa de tradução
#endif
	,
	[ActRcvFrmEnrSevTch] =
#if   L==1	// ca
	"Register/remove several teachers in/from a course"		// Necessita traducció
#elif L==2	// de
	"Register/remove several teachers in/from a course"		// Need Übersetzung
#elif L==3	// en
	"Register/remove several teachers in/from a course"
#elif L==4	// es
	"Inscribir/eliminar varios profesores en/de una asignatura"
#elif L==5	// fr
	"Register/remove several teachers in/from a course"		// Besoin de traduction
#elif L==6	// gn
	"Inscribir/eliminar varios profesores en/de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Register/remove several teachers in/from a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Register/remove several teachers in/from a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Register/remove several teachers in/from a course"		// Precisa de tradução
#endif
	,
	[ActCnfID_Oth] =
#if   L==1	// ca
	"Confirm a user's ID"			// Necessita traducció
#elif L==2	// de
	"Confirm a user's ID"			// Need Übersetzung
#elif L==3	// en
	"Confirm a user's ID"
#elif L==4	// es
	"Confirmar el ID de un usuario"
#elif L==5	// fr
	"Confirm a user's ID"			// Besoin de traduction
#elif L==6	// gn
	"Confirmar el ID de un usuario"		// Okoteve traducción
#elif L==7	// it
	"Confirm a user's ID"			// Bisogno di traduzione
#elif L==8	// pl
	"Confirm a user's ID"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Confirm a user's ID"			// Precisa de tradução
#endif
	,
	[ActCnfID_Std] =
#if   L==1	// ca
	"Confirm a student's ID"		// Necessita traducció
#elif L==2	// de
	"Confirm a student's ID"		// Need Übersetzung
#elif L==3	// en
	"Confirm a student's ID"
#elif L==4	// es
	"Confirmar el ID de un estudiante"
#elif L==5	// fr
	"Confirm a student's ID"		// Besoin de traduction
#elif L==6	// gn
	"Confirmar el ID de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Confirm a student's ID"		// Bisogno di traduzione
#elif L==8	// pl
	"Confirm a student's ID"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Confirm a student's ID"		// Precisa de tradução
#endif
	,
	[ActCnfID_Tch] =
#if   L==1	// ca
	"Confirm a teacher's ID"		// Necessita traducció
#elif L==2	// de
	"Confirm a teacher's ID"		// Need Übersetzung
#elif L==3	// en
	"Confirm a teacher's ID"
#elif L==4	// es
	"Confirmar el ID de un profesor"
#elif L==5	// fr
	"Confirm a teacher's ID"		// Besoin de traduction
#elif L==6	// gn
	"Confirmar el ID de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Confirm a teacher's ID"		// Bisogno di traduzione
#elif L==8	// pl
	"Confirm a teacher's ID"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Confirm a teacher's ID"		// Precisa de tradução
#endif
	,
	[ActFrmAccOth] =
#if   L==1	// ca
	"Forms to the change a user's account"			// Necessita traducció
#elif L==2	// de
	"Forms to the change a user's account"			// Need Übersetzung
#elif L==3	// en
	"Forms to the change a user's account"
#elif L==4	// es
	"Formularios para cambiar la cuenta de un usuario"
#elif L==5	// fr
	"Forms to the change a user's account"			// Besoin de traduction
#elif L==6	// gn
	"Formularios para cambiar la cuenta de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Forms to the change a user's account"			// Bisogno di traduzione
#elif L==8	// pl
	"Forms to the change a user's account"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Forms to the change a user's account"			// Precisa de tradução
#endif
	,
	[ActFrmAccStd] =
#if   L==1	// ca
	"Forms to the change a student's account"		// Necessita traducció
#elif L==2	// de
	"Forms to the change a student's account"		// Need Übersetzung
#elif L==3	// en
	"Forms to the change a student's account"
#elif L==4	// es
	"Formularios para cambiar la cuenta de un estudiante"
#elif L==5	// fr
	"Forms to the change a student's account"		// Besoin de traduction
#elif L==6	// gn
	"Formularios para cambiar la cuenta de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Forms to the change a student's account"		// Bisogno di traduzione
#elif L==8	// pl
	"Forms to the change a student's account"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Forms to the change a student's account"		// Precisa de tradução
#endif
	,
	[ActFrmAccTch] =
#if   L==1	// ca
	"Forms to the change a teacher's account"		// Necessita traducció
#elif L==2	// de
	"Forms to the change a teacher's account"		// Need Übersetzung
#elif L==3	// en
	"Forms to the change a teacher's account"
#elif L==4	// es
	"Formularios para cambiar la cuenta de un profesor"
#elif L==5	// fr
	"Forms to the change a teacher's account"		// Besoin de traduction
#elif L==6	// gn
	"Formularios para cambiar la cuenta de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Forms to the change a teacher's account"		// Bisogno di traduzione
#elif L==8	// pl
	"Forms to the change a teacher's account"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Forms to the change a teacher's account"		// Precisa de tradução
#endif
	,
	[ActRemOldNicOth] =
#if   L==1	// ca
	"Remove a user's nickname"		// Necessita traducció
#elif L==2	// de
	"Remove a user's nickname"		// Need Übersetzung
#elif L==3	// en
	"Remove a user's nickname"
#elif L==4	// es
	"Eliminar el apodo de un usuario"
#elif L==5	// fr
	"Remove a user's nickname"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar el apodo de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Remove a user's nickname"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a user's nickname"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a user's nickname"		// Precisa de tradução
#endif
	,
	[ActRemOldNicStd] =
#if   L==1	// ca
	"Remove a student's nickname"		// Necessita traducció
#elif L==2	// de
	"Remove a student's nickname"		// Need Übersetzung
#elif L==3	// en
	"Remove a student's nickname"
#elif L==4	// es
	"Eliminar el apodo de un estudiante"
#elif L==5	// fr
	"Remove a student's nickname"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar el apodo de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Remove a student's nickname"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a student's nickname"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a student's nickname"		// Precisa de tradução
#endif
	,
	[ActRemOldNicTch] =
#if   L==1	// ca
	"Remove a teacher's nickname"		// Necessita traducció
#elif L==2	// de
	"Remove a teacher's nickname"		// Need Übersetzung
#elif L==3	// en
	"Remove a teacher's nickname"
#elif L==4	// es
	"Eliminar el apodo de un profesor"
#elif L==5	// fr
	"Remove a teacher's nickname"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar el apodo de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Remove a teacher's nickname"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a teacher's nickname"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a teacher's nickname"		// Precisa de tradução
#endif
	,
	[ActChgNicOth] =
#if   L==1	// ca
	"Change a user's nickname"		// Necessita traducció
#elif L==2	// de
	"Change a user's nickname"		// Need Übersetzung
#elif L==3	// en
	"Change a user's nickname"
#elif L==4	// es
	"Cambiar el apodo de un usuario"
#elif L==5	// fr
	"Change a user's nickname"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar el apodo de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Change a user's nickname"		// Bisogno di traduzione
#elif L==8	// pl
	"Change a user's nickname"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change a user's nickname"		// Precisa de tradução
#endif
	,
	[ActChgNicStd] =
#if   L==1	// ca
	"Change a student's nickname"		// Necessita traducció
#elif L==2	// de
	"Change a student's nickname"		// Need Übersetzung
#elif L==3	// en
	"Change a student's nickname"
#elif L==4	// es
	"Cambiar el apodo de un estudiante"
#elif L==5	// fr
	"Change a student's nickname"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar el apodo de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Change a student's nickname"		// Bisogno di traduzione
#elif L==8	// pl
	"Change a student's nickname"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change a student's nickname"		// Precisa de tradução
#endif
	,
	[ActChgNicTch] =
#if   L==1	// ca
	"Change a teacher's nickname"		// Necessita traducció
#elif L==2	// de
	"Change a teacher's nickname"		// Need Übersetzung
#elif L==3	// en
	"Change a teacher's nickname"
#elif L==4	// es
	"Cambiar el apodo de un profesor"
#elif L==5	// fr
	"Change a teacher's nickname"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar el apodo de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Change a teacher's nickname"		// Bisogno di traduzione
#elif L==8	// pl
	"Change a teacher's nickname"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change a teacher's nickname"		// Precisa de tradução
#endif
	,
	[ActRemID_Oth] =
#if   L==1	// ca
	"Remove a user's ID"			// Necessita traducció
#elif L==2	// de
	"Remove a user's ID"			// Need Übersetzung
#elif L==3	// en
	"Remove a user's ID"
#elif L==4	// es
	"Eliminar el ID de un usuario"
#elif L==5	// fr
	"Remove a user's ID"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar el ID de un usuario"		// Okoteve traducción
#elif L==7	// it
	"Remove a user's ID"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a user's ID"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a user's ID"			// Precisa de tradução
#endif
	,
	[ActRemID_Std] =
#if   L==1	// ca
	"Remove a student's ID"			// Necessita traducció
#elif L==2	// de
	"Remove a student's ID"			// Need Übersetzung
#elif L==3	// en
	"Remove a student's ID"
#elif L==4	// es
	"Eliminar el ID de un estudiante"
#elif L==5	// fr
	"Remove a student's ID"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar el ID de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Remove a student's ID"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a student's ID"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a student's ID"			// Precisa de tradução
#endif
	,
	[ActRemID_Tch] =
#if   L==1	// ca
	"Remove a teacher's ID"			// Necessita traducció
#elif L==2	// de
	"Remove a teacher's ID"			// Need Übersetzung
#elif L==3	// en
	"Remove a teacher's ID"
#elif L==4	// es
	"Eliminar el ID de un profesor"
#elif L==5	// fr
	"Remove a teacher's ID"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar el ID de un profesor"		// Okoteve traducción
#elif L==7	// it
	"Remove a teacher's ID"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a teacher's ID"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a teacher's ID"			// Precisa de tradução
#endif
	,
	[ActNewID_Oth] =
#if   L==1	// ca
	"Create a new user's ID"		// Necessita traducció
#elif L==2	// de
	"Create a new user's ID"		// Need Übersetzung
#elif L==3	// en
	"Create a new user's ID"
#elif L==4	// es
	"Crear un nuevo ID de un usuario"
#elif L==5	// fr
	"Create a new user's ID"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo ID de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Create a new user's ID"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new user's ID"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new user's ID"		// Precisa de tradução
#endif
	,
	[ActNewID_Std] =
#if   L==1	// ca
	"Create a new student's ID"		// Necessita traducció
#elif L==2	// de
	"Create a new student's ID"		// Need Übersetzung
#elif L==3	// en
	"Create a new student's ID"
#elif L==4	// es
	"Crear un nuevo ID de un estudiante"
#elif L==5	// fr
	"Create a new student's ID"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo ID de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Create a new student's ID"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new student's ID"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new student's ID"		// Precisa de tradução
#endif
	,
	[ActNewID_Tch] =
#if   L==1	// ca
	"Create a new teacher's ID"		// Necessita traducció
#elif L==2	// de
	"Create a new teacher's ID"		// Need Übersetzung
#elif L==3	// en
	"Create a new teacher's ID"
#elif L==4	// es
	"Crear un nuevo ID de un profesor"
#elif L==5	// fr
	"Create a new teacher's ID"		// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo ID de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Create a new teacher's ID"		// Bisogno di traduzione
#elif L==8	// pl
	"Create a new teacher's ID"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new teacher's ID"		// Precisa de tradução
#endif
	,
	[ActChgPwdOth] =
#if   L==1	// ca
	"Change a user's password"			// Necessita traducció
#elif L==2	// de
	"Change a user's password"			// Need Übersetzung
#elif L==3	// en
	"Change a user's password"
#elif L==4	// es
	"Cambiar la contrase&ntilde;a de un usuario"
#elif L==5	// fr
	"Change a user's password"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la contrase&ntilde;a de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Change a user's password"			// Bisogno di traduzione
#elif L==8	// pl
	"Change a user's password"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change a user's password"			// Precisa de tradução
#endif
	,
	[ActChgPwdStd] =
#if   L==1	// ca
	"Change a student's password"			// Necessita traducció
#elif L==2	// de
	"Change a student's password"			// Need Übersetzung
#elif L==3	// en
	"Change a student's password"
#elif L==4	// es
	"Cambiar la contrase&ntilde;a de un estudiante"
#elif L==5	// fr
	"Change a student's password"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la contrase&ntilde;a de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Change a student's password"			// Bisogno di traduzione
#elif L==8	// pl
	"Change a student's password"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change a student's password"			// Precisa de tradução
#endif
	,
	[ActChgPwdTch] =
#if   L==1	// ca
	"Change a teacher's password"			// Necessita traducció
#elif L==2	// de
	"Change a teacher's password"			// Need Übersetzung
#elif L==3	// en
	"Change a teacher's password"
#elif L==4	// es
	"Cambiar la contrase&ntilde;a de un profesor"
#elif L==5	// fr
	"Change a teacher's password"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la contrase&ntilde;a de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Change a teacher's password"			// Bisogno di traduzione
#elif L==8	// pl
	"Change a teacher's password"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change a teacher's password"			// Precisa de tradução
#endif
	,
	[ActRemMaiOth] =
#if   L==1	// ca
	"Remove a user's email"					// Necessita traducció
#elif L==2	// de
	"Remove a user's email"					// Need Übersetzung
#elif L==3	// en
	"Remove a user's email"
#elif L==4	// es
	"Eliminar el correo electr&oacute;nico de un usuario"
#elif L==5	// fr
	"Remove a user's email"					// Besoin de traduction
#elif L==6	// gn
	"Eliminar el correo electr&oacute;nico de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Remove a user's email"					// Bisogno di traduzione
#elif L==8	// pl
	"Remove a user's email"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a user's email"					// Precisa de tradução
#endif
	,
	[ActRemMaiStd] =
#if   L==1	// ca
	"Remove a student's email"					// Necessita traducció
#elif L==2	// de
	"Remove a student's email"					// Need Übersetzung
#elif L==3	// en
	"Remove a student's email"
#elif L==4	// es
	"Eliminar el correo electr&oacute;nico de un estudiante"
#elif L==5	// fr
	"Remove a student's email"					// Besoin de traduction
#elif L==6	// gn
	"Eliminar el correo electr&oacute;nico de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Remove a student's email"					// Bisogno di traduzione
#elif L==8	// pl
	"Remove a student's email"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a student's email"					// Precisa de tradução
#endif
	,
	[ActRemMaiTch] =
#if   L==1	// ca
	"Remove a teacher's email"				// Necessita traducció
#elif L==2	// de
	"Remove a teacher's email"				// Need Übersetzung
#elif L==3	// en
	"Remove a teacher's email"
#elif L==4	// es
	"Eliminar el correo electr&oacute;nico de un profesor"
#elif L==5	// fr
	"Remove a teacher's email"				// Besoin de traduction
#elif L==6	// gn
	"Eliminar el correo electr&oacute;nico de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Remove a teacher's email"				// Bisogno di traduzione
#elif L==8	// pl
	"Remove a teacher's email"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a teacher's email"				// Precisa de tradução
#endif
	,
	[ActNewMaiOth] =
#if   L==1	// ca
	"Create a new user's email"					// Necessita traducció
#elif L==2	// de
	"Create a new user's email"					// Need Übersetzung
#elif L==3	// en
	"Create a new user's email"
#elif L==4	// es
	"Crear un nuevo correo electr&oacute;nico de un usuario"
#elif L==5	// fr
	"Create a new user's email"					// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo correo electr&oacute;nico de un usuario"	// Okoteve traducción
#elif L==7	// it
	"Create a new user's email"					// Bisogno di traduzione
#elif L==8	// pl
	"Create a new user's email"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new user's email"					// Precisa de tradução
#endif
	,
	[ActNewMaiStd] =
#if   L==1	// ca
	"Create a new student's email"					// Necessita traducció
#elif L==2	// de
	"Create a new student's email"					// Need Übersetzung
#elif L==3	// en
	"Create a new student's email"
#elif L==4	// es
	"Crear un nuevo correo electr&oacute;nico de un estudiante"
#elif L==5	// fr
	"Create a new student's email"					// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo correo electr&oacute;nico de un estudiante"	// Okoteve traducción
#elif L==7	// it
	"Create a new student's email"					// Bisogno di traduzione
#elif L==8	// pl
	"Create a new student's email"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new student's email"					// Precisa de tradução
#endif
	,
	[ActNewMaiTch] =
#if   L==1	// ca
	"Create a new teacher's email"					// Necessita traducció
#elif L==2	// de
	"Create a new teacher's email"					// Need Übersetzung
#elif L==3	// en
	"Create a new teacher's email"
#elif L==4	// es
	"Crear un nuevo correo electr&oacute;nico de un profesor"
#elif L==5	// fr
	"Create a new teacher's email"					// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo correo electr&oacute;nico de un profesor"	// Okoteve traducción
#elif L==7	// it
	"Create a new teacher's email"					// Bisogno di traduzione
#elif L==8	// pl
	"Create a new teacher's email"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new teacher's email"					// Precisa de tradução
#endif
	,
	[ActRemStdCrs] =
#if   L==1	// ca
	"Remove a student from a course"		// Necessita traducció
#elif L==2	// de
	"Remove a student from a course"		// Need Übersetzung
#elif L==3	// en
	"Remove a student from a course"
#elif L==4	// es
	"Eliminar un estudiante de una asignatura"
#elif L==5	// fr
	"Remove a student from a course"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un estudiante de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove a student from a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a student from a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a student from a course"		// Precisa de tradução
#endif
	,
	[ActRemNETCrs] =
#if   L==1	// ca
	"Remove a non-editing teacher from a course"		// Necessita traducció
#elif L==2	// de
	"Remove a non-editing teacher from a course"		// Need Übersetzung
#elif L==3	// en
	"Remove a non-editing teacher from a course"
#elif L==4	// es
	"Eliminar un profesor no editor de una asignatura"
#elif L==5	// fr
	"Remove a non-editing teacher from a course"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un profesor no editor de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove a non-editing teacher from a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a non-editing teacher from a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a non-editing teacher from a course"		// Precisa de tradução
#endif
	,
	[ActRemTchCrs] =
#if   L==1	// ca
	"Remove a teacher from a course"		// Necessita traducció
#elif L==2	// de
	"Remove a teacher from a course"		// Need Übersetzung
#elif L==3	// en
	"Remove a teacher from a course"
#elif L==4	// es
	"Eliminar un profesor de una asignatura"
#elif L==5	// fr
	"Remove a teacher from a course"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un profesor de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove a teacher from a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a teacher from a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a teacher from a course"		// Precisa de tradução
#endif
	,
	[ActRemUsrGbl] =
#if   L==1	// ca
	"Eliminate completely a user from the platform"		// Necessita traducció
#elif L==2	// de
	"Eliminate completely a user from the platform"		// Need Übersetzung
#elif L==3	// en
	"Eliminate completely a user from the platform"
#elif L==4	// es
	"Eliminar un usuario completamente de la plataforma"
#elif L==5	// fr
	"Eliminate completely a user from the platform"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un usuario completamente de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Eliminate completely a user from the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Eliminate completely a user from the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Eliminate completely a user from the platform"		// Precisa de tradução
#endif
	,
	[ActReqRemAllStdCrs] =
#if   L==1	// ca
	"Request removal of all the students from a course"				// Necessita traducció
#elif L==2	// de
	"Request removal of all the students from a course"				// Need Übersetzung
#elif L==3	// en
	"Request removal of all the students from a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de todos los estudiantes de una asignatura"
#elif L==5	// fr
	"Request removal of all the students from a course"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de todos los estudiantes de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Request removal of all the students from a course"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of all the students from a course"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of all the students from a course"				// Precisa de tradução
#endif
	,
	[ActRemAllStdCrs] =
#if   L==1	// ca
	"Remove all the students from a course"			// Necessita traducció
#elif L==2	// de
	"Remove all the students from a course"			// Need Übersetzung
#elif L==3	// en
	"Remove all the students from a course"
#elif L==4	// es
	"Eliminar todos los estudiantes de una asignatura"
#elif L==5	// fr
	"Remove all the students from a course"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar todos los estudiantes de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove all the students from a course"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove all the students from a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove all the students from a course"			// Precisa de tradução
#endif
	,
	[ActReqRemOldUsr] =
#if   L==1	// ca
	"Request the complete elimination of old users"			// Necessita traducció
#elif L==2	// de
	"Request the complete elimination of old users"			// Need Übersetzung
#elif L==3	// en
	"Request the complete elimination of old users"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n completa de usuarios antiguos"
#elif L==5	// fr
	"Request the complete elimination of old users"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n completa de usuarios antiguos"	// Okoteve traducción
#elif L==7	// it
	"Request the complete elimination of old users"			// Bisogno di traduzione
#elif L==8	// pl
	"Request the complete elimination of old users"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the complete elimination of old users"			// Precisa de tradução
#endif
	,
	[ActRemOldUsr] =
#if   L==1	// ca
	"Eliminate completely old users"		// Necessita traducció
#elif L==2	// de
	"Eliminate completely old users"		// Need Übersetzung
#elif L==3	// en
	"Eliminate completely old users"
#elif L==4	// es
	"Eliminar completamente usuarios antiguos"
#elif L==5	// fr
	"Eliminate completely old users"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar completamente usuarios antiguos"	// Okoteve traducción
#elif L==7	// it
	"Eliminate completely old users"		// Bisogno di traduzione
#elif L==8	// pl
	"Eliminate completely old users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Eliminate completely old users"		// Precisa de tradução
#endif
	,
	[ActLstDupUsr] =
#if   L==1	// ca
	"List possible duplicate users"		// Necessita traducció
#elif L==2	// de
	"List possible duplicate users"		// Need Übersetzung
#elif L==3	// en
	"List possible duplicate users"
#elif L==4	// es
	"Listar posibles usuarios duplicados"
#elif L==5	// fr
	"List possible duplicate users"		// Besoin de traduction
#elif L==6	// gn
	"Listar posibles usuarios duplicados"	// Okoteve traducción
#elif L==7	// it
	"List possible duplicate users"		// Bisogno di traduzione
#elif L==8	// pl
	"List possible duplicate users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List possible duplicate users"		// Precisa de tradução
#endif
	,
	[ActLstSimUsr] =
#if   L==1	// ca
	"List users similar to a given one (possible duplicates)"	// Necessita traducció
#elif L==2	// de
	"List users similar to a given one (possible duplicates)"	// Need Übersetzung
#elif L==3	// en
	"List users similar to a given one (possible duplicates)"
#elif L==4	// es
	"Listar usuarios similares a uno dado (posibles duplicados)"
#elif L==5	// fr
	"List users similar to a given one (possible duplicates)"	// Besoin de traduction
#elif L==6	// gn
	"Listar usuarios similares a uno dado (posibles duplicados)"	// Okoteve traducción
#elif L==7	// it
	"List users similar to a given one (possible duplicates)"	// Bisogno di traduzione
#elif L==8	// pl
	"List users similar to a given one (possible duplicates)"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List users similar to a given one (possible duplicates)"	// Precisa de tradução
#endif
	,
	[ActRemDupUsr] =
#if   L==1	// ca
	"Remove a user from a list of possible duplicate users"			// Necessita traducció
#elif L==2	// de
	"Remove a user from a list of possible duplicate users"			// Need Übersetzung
#elif L==3	// en
	"Remove a user from a list of possible duplicate users"
#elif L==4	// es
	"Eliminar a un usuario de una lista de posibles usuarios duplicados"
#elif L==5	// fr
	"Remove a user from a list of possible duplicate users"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar a un usuario de una lista de posibles usuarios duplicados"	// Okoteve traducción
#elif L==7	// it
	"Remove a user from a list of possible duplicate users"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a user from a list of possible duplicate users"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a user from a list of possible duplicate users"			// Precisa de tradução
#endif
	,
	[ActReqFolSevStd] =
#if   L==1	// ca
	"Request follow several students"	// Necessita traducció
#elif L==2	// de
	"Request follow several students"	// Need Übersetzung
#elif L==3	// en
	"Request follow several students"
#elif L==4	// es
	"Solicitar seguir a varios estudiantes"
#elif L==5	// fr
	"Request follow several students"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar seguir a varios estudiantes"	// Okoteve traducción
#elif L==7	// it
	"Request follow several students"	// Bisogno di traduzione
#elif L==8	// pl
	"Request follow several students"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request follow several students"	// Precisa de tradução
#endif
	,
	[ActReqFolSevTch] =
#if   L==1	// ca
	"Request follow several teachers"	// Necessita traducció
#elif L==2	// de
	"Request follow several teachers"	// Need Übersetzung
#elif L==3	// en
	"Request follow several teachers"
#elif L==4	// es
	"Solicitar seguir a varios profesores"
#elif L==5	// fr
	"Request follow several teachers"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar seguir a varios profesores"	// Okoteve traducción
#elif L==7	// it
	"Request follow several teachers"	// Bisogno di traduzione
#elif L==8	// pl
	"Request follow several teachers"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request follow several teachers"	// Precisa de tradução
#endif
	,
	[ActReqUnfSevStd] =
#if   L==1	// ca
	"Request unfollow several students"			// Necessita traducció
#elif L==2	// de
	"Request unfollow several students"			// Need Übersetzung
#elif L==3	// en
	"Request unfollow several students"
#elif L==4	// es
	"Solicitar dejar de seguir a varios estudiantes"
#elif L==5	// fr
	"Request unfollow several students"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar dejar de seguir a varios estudiantes"	// Okoteve traducción
#elif L==7	// it
	"Request unfollow several students"			// Bisogno di traduzione
#elif L==8	// pl
	"Request unfollow several students"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request unfollow several students"			// Precisa de tradução
#endif
	,
	[ActReqUnfSevTch] =
#if   L==1	// ca
	"Request unfollow several teachers"		// Necessita traducció
#elif L==2	// de
	"Request unfollow several teachers"		// Need Übersetzung
#elif L==3	// en
	"Request unfollow several teachers"
#elif L==4	// es
	"Solicitar dejar de seguir a varios profesores"
#elif L==5	// fr
	"Request unfollow several teachers"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar dejar de seguir a varios profesores"	// Okoteve traducción
#elif L==7	// it
	"Request unfollow several teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Request unfollow several teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request unfollow several teachers"		// Precisa de tradução
#endif
	,
	[ActFolSevStd] =
#if   L==1	// ca
	"Follow several students"	// Necessita traducció
#elif L==2	// de
	"Follow several students"	// Need Übersetzung
#elif L==3	// en
	"Follow several students"
#elif L==4	// es
	"Seguir a varios estudiantes"
#elif L==5	// fr
	"Follow several students"	// Besoin de traduction
#elif L==6	// gn
	"Seguir a varios estudiantes"	// Okoteve traducción
#elif L==7	// it
	"Follow several students"	// Bisogno di traduzione
#elif L==8	// pl
	"Follow several students"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Follow several students"	// Precisa de tradução
#endif
	,
	[ActFolSevTch] =
#if   L==1	// ca
	"Follow several teachers"	// Necessita traducció
#elif L==2	// de
	"Follow several teachers"	// Need Übersetzung
#elif L==3	// en
	"Follow several teachers"
#elif L==4	// es
	"Seguir a varios profesores"
#elif L==5	// fr
	"Follow several teachers"	// Besoin de traduction
#elif L==6	// gn
	"Seguir a varios profesores"	// Okoteve traducción
#elif L==7	// it
	"Follow several teachers"	// Bisogno di traduzione
#elif L==8	// pl
	"Follow several teachers"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Follow several teachers"	// Precisa de tradução
#endif
	,
	[ActUnfSevStd] =
#if   L==1	// ca
	"Unfollow several students"		// Necessita traducció
#elif L==2	// de
	"Unfollow several students"		// Need Übersetzung
#elif L==3	// en
	"Unfollow several students"
#elif L==4	// es
	"Dejar de seguir a varios estudiantes"
#elif L==5	// fr
	"Unfollow several students"		// Besoin de traduction
#elif L==6	// gn
	"Dejar de seguir a varios estudiantes"	// Okoteve traducción
#elif L==7	// it
	"Unfollow several students"		// Bisogno di traduzione
#elif L==8	// pl
	"Unfollow several students"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfollow several students"		// Precisa de tradução
#endif
	,
	[ActUnfSevTch] =
#if   L==1	// ca
	"Unfollow several teachers"		// Necessita traducció
#elif L==2	// de
	"Unfollow several teachers"		// Need Übersetzung
#elif L==3	// en
	"Unfollow several teachers"
#elif L==4	// es
	"Dejar de seguir a varios profesores"
#elif L==5	// fr
	"Unfollow several teachers"		// Besoin de traduction
#elif L==6	// gn
	"Dejar de seguir a varios profesores"	// Okoteve traducción
#elif L==7	// it
	"Unfollow several teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Unfollow several teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfollow several teachers"		// Precisa de tradução
#endif
	,
	[ActSeeAnn] =
#if   L==1	// ca
	"Show global announcements"	// Necessita traducció
#elif L==2	// de
	"Show global announcements"	// Need Übersetzung
#elif L==3	// en
	"Show global announcements"
#elif L==4	// es
	"Mostrar anuncios globales"
#elif L==5	// fr
	"Show global announcements"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar anuncios globales"	// Okoteve traducción
#elif L==7	// it
	"Show global announcements"	// Bisogno di traduzione
#elif L==8	// pl
	"Show global announcements"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show global announcements"	// Precisa de tradução
#endif
	,
	[ActSeeAllNot] =
#if   L==1	// ca
	"See all notices"	// Necessita traducció
#elif L==2	// de
	"See all notices"	// Need Übersetzung
#elif L==3	// en
	"See all notices"
#elif L==4	// es
	"Ver todos los avisos"
#elif L==5	// fr
	"See all notices"	// Besoin de traduction
#elif L==6	// gn
	"Ver todos los avisos"	// Okoteve traducción
#elif L==7	// it
	"See all notices"	// Bisogno di traduzione
#elif L==8	// pl
	"See all notices"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See all notices"	// Precisa de tradução
#endif
	,
	[ActSeeFor] =
#if   L==1	// ca
	"See the top level of the forums"	// Necessita traducció
#elif L==2	// de
	"See the top level of the forums"	// Need Übersetzung
#elif L==3	// en
	"See the top level of the forums"
#elif L==4	// es
	"Ver el nivel superior de los foros"
#elif L==5	// fr
	"See the top level of the forums"	// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior de los foros"	// Okoteve traducción
#elif L==7	// it
	"See the top level of the forums"	// Bisogno di traduzione
#elif L==8	// pl
	"See the top level of the forums"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the top level of the forums"	// Precisa de tradução
#endif
	,
	[ActSeeChtRms] =
#if   L==1	// ca
	"See the chat rooms"	// Necessita traducció
#elif L==2	// de
	"See the chat rooms"	// Need Übersetzung
#elif L==3	// en
	"See the chat rooms"
#elif L==4	// es
	"Ver las salas de chat"
#elif L==5	// fr
	"See the chat rooms"	// Besoin de traduction
#elif L==6	// gn
	"Ver las salas de chat"	// Okoteve traducción
#elif L==7	// it
	"See the chat rooms"	// Bisogno di traduzione
#elif L==8	// pl
	"See the chat rooms"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the chat rooms"	// Precisa de tradução
#endif
	,
	[ActSeeRcvMsg] =
#if   L==1	// ca
	"Show the messages received from other users"	// Necessita traducció
#elif L==2	// de
	"Show the messages received from other users"	// Need Übersetzung
#elif L==3	// en
	"Show the messages received from other users"
#elif L==4	// es
	"Ver mensajes recibidos de otros usuarios"
#elif L==5	// fr
	"Show the messages received from other users"	// Besoin de traduction
#elif L==6	// gn
	"Ver mensajes recibidos de otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Show the messages received from other users"	// Bisogno di traduzione
#elif L==8	// pl
	"Show the messages received from other users"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show the messages received from other users"	// Precisa de tradução
#endif
	,
	[ActReqMaiUsr] =
#if   L==1	// ca
	"Select users to send an email to them"		// Necessita traducció
#elif L==2	// de
	"Select users to send an email to them"		// Need Übersetzung
#elif L==3	// en
	"Select users to send an email to them"
#elif L==4	// es
	"Seleccionar usuarios para enviarles un correo"
#elif L==5	// fr
	"Select users to send an email to them"		// Besoin de traduction
#elif L==6	// gn
	"Seleccionar usuarios para enviarles un correo"	// Okoteve traducción
#elif L==7	// it
	"Select users to send an email to them"		// Bisogno di traduzione
#elif L==8	// pl
	"Select users to send an email to them"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Select users to send an email to them"		// Precisa de tradução
#endif
	,
	[ActWriAnn] =
#if   L==1	// ca
	"See form to create a new global announcement"		// Necessita traducció
#elif L==2	// de
	"See form to create a new global announcement"		// Need Übersetzung
#elif L==3	// en
	"See form to create a new global announcement"
#elif L==4	// es
	"Ver formulario para crear un nuevo anuncio global"
#elif L==5	// fr
	"See form to create a new global announcement"		// Besoin de traduction
#elif L==6	// gn
	"Ver formulario para crear un nuevo anuncio global"	// Okoteve traducción
#elif L==7	// it
	"See form to create a new global announcement"		// Bisogno di traduzione
#elif L==8	// pl
	"See form to create a new global announcement"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See form to create a new global announcement"		// Precisa de tradução
#endif
	,
	[ActRcvAnn] =
#if   L==1	// ca
	"Create a new global announcement"	// Necessita traducció
#elif L==2	// de
	"Create a new global announcement"	// Need Übersetzung
#elif L==3	// en
	"Create a new global announcement"
#elif L==4	// es
	"Crear un nuevo anuncio global"
#elif L==5	// fr
	"Create a new global announcement"	// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo anuncio global"		// Okoteve traducción
#elif L==7	// it
	"Create a new global announcement"	// Bisogno di traduzione
#elif L==8	// pl
	"Create a new global announcement"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new global announcement"	// Precisa de tradução
#endif
	,
	[ActHidAnn] =
#if   L==1	// ca
	"Hide a global announcement"	// Necessita traducció
#elif L==2	// de
	"Hide a global announcement"	// Need Übersetzung
#elif L==3	// en
	"Hide a global announcement"
#elif L==4	// es
	"Ocultar un anuncio global"
#elif L==5	// fr
	"Hide a global announcement"	// Besoin de traduction
#elif L==6	// gn
	"Ocultar un anuncio global"	// Okoteve traducción
#elif L==7	// it
	"Hide a global announcement"	// Bisogno di traduzione
#elif L==8	// pl
	"Hide a global announcement"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Hide a global announcement"	// Precisa de tradução
#endif
	,
	[ActRevAnn] =
#if   L==1	// ca
	"Unhide a global announcement"	// Necessita traducció
#elif L==2	// de
	"Unhide a global announcement"	// Need Übersetzung
#elif L==3	// en
	"Unhide a global announcement"
#elif L==4	// es
	"Mostrar un anuncio global"
#elif L==5	// fr
	"Unhide a global announcement"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar un anuncio global"	// Okoteve traducción
#elif L==7	// it
	"Unhide a global announcement"	// Bisogno di traduzione
#elif L==8	// pl
	"Unhide a global announcement"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unhide a global announcement"	// Precisa de tradução
#endif
	,
	[ActRemAnn] =
#if   L==1	// ca
	"Remove a global announcement"	// Necessita traducció
#elif L==2	// de
	"Remove a global announcement"	// Need Übersetzung
#elif L==3	// en
	"Remove a global announcement"
#elif L==4	// es
	"Eliminar un anuncio global"
#elif L==5	// fr
	"Remove a global announcement"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un anuncio global"	// Okoteve traducción
#elif L==7	// it
	"Remove a global announcement"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove a global announcement"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a global announcement"	// Precisa de tradução
#endif
	,
	[ActSeeOneNot] =
#if   L==1	// ca
	"Show (expand) a notice"	// Necessita traducció
#elif L==2	// de
	"Show (expand) a notice"	// Need Übersetzung
#elif L==3	// en
	"Show (expand) a notice"
#elif L==4	// es
	"Mostrar (expandir) un aviso"
#elif L==5	// fr
	"Show (expand) a notice"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar (expandir) un aviso"	// Okoteve traducción
#elif L==7	// it
	"Show (expand) a notice"	// Bisogno di traduzione
#elif L==8	// pl
	"Show (expand) a notice"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show (expand) a notice"	// Precisa de tradução
#endif
	,
	[ActWriNot] =
#if   L==1	// ca
	"Write a new notice"		// Necessita traducció
#elif L==2	// de
	"Write a new notice"		// Need Übersetzung
#elif L==3	// en
	"Write a new notice"
#elif L==4	// es
	"Escribir un nuevo aviso"
#elif L==5	// fr
	"Write a new notice"		// Besoin de traduction
#elif L==6	// gn
	"Escribir un nuevo aviso"	// Okoteve traducción
#elif L==7	// it
	"Write a new notice"		// Bisogno di traduzione
#elif L==8	// pl
	"Write a new notice"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Write a new notice"		// Precisa de tradução
#endif
	,
	[ActRcvNot] =
#if   L==1	// ca
	"Create a new notice"	// Necessita traducció
#elif L==2	// de
	"Create a new notice"	// Need Übersetzung
#elif L==3	// en
	"Create a new notice"
#elif L==4	// es
	"Crear un nuevo aviso"
#elif L==5	// fr
	"Create a new notice"	// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo aviso"	// Okoteve traducción
#elif L==7	// it
	"Create a new notice"	// Bisogno di traduzione
#elif L==8	// pl
	"Create a new notice"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new notice"	// Precisa de tradução
#endif
	,
	[ActHidNot] =
#if   L==1	// ca
	"Hide a notice"		// Necessita traducció
#elif L==2	// de
	"Hide a notice"		// Need Übersetzung
#elif L==3	// en
	"Hide a notice"
#elif L==4	// es
	"Ocultar un aviso"
#elif L==5	// fr
	"Hide a notice"		// Besoin de traduction
#elif L==6	// gn
	"Ocultar un aviso"	// Okoteve traducción
#elif L==7	// it
	"Hide a notice"		// Bisogno di traduzione
#elif L==8	// pl
	"Hide a notice"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Hide a notice"		// Precisa de tradução
#endif
	,
	[ActRevNot] =
#if   L==1	// ca
	"Unhide a notice"	// Necessita traducció
#elif L==2	// de
	"Unhide a notice"	// Need Übersetzung
#elif L==3	// en
	"Unhide a notice"
#elif L==4	// es
	"Mostrar un aviso"
#elif L==5	// fr
	"Unhide a notice"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar un aviso"	// Okoteve traducción
#elif L==7	// it
	"Unhide a notice"	// Bisogno di traduzione
#elif L==8	// pl
	"Unhide a notice"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unhide a notice"	// Precisa de tradução
#endif
	,
	[ActReqRemNot] =
#if   L==1	// ca
	"Request removal of a notice"			// Necessita traducció
#elif L==2	// de
	"Request removal of a notice"			// Need Übersetzung
#elif L==3	// en
	"Request removal of a notice"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un aviso"
#elif L==5	// fr
	"Request removal of a notice"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un aviso"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a notice"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a notice"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a notice"			// Precisa de tradução
#endif
	,
	[ActRemNot] =
#if   L==1	// ca
	"Remove a notice"	// Necessita traducció
#elif L==2	// de
	"Remove a notice"	// Need Übersetzung
#elif L==3	// en
	"Remove a notice"
#elif L==4	// es
	"Eliminar un aviso"
#elif L==5	// fr
	"Remove a notice"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un aviso"	// Okoteve traducción
#elif L==7	// it
	"Remove a notice"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove a notice"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a notice"	// Precisa de tradução
#endif
	,
	[ActSeeForCrsUsr] =
#if   L==1	// ca
	"See top level of forum of users of a course"			// Necessita traducció
#elif L==2	// de
	"See top level of forum of users of a course"			// Need Übersetzung
#elif L==3	// en
	"See top level of forum of users of a course"
#elif L==4	// es
	"Ver el nivel superior del foro de usuarios de una asignatura"
#elif L==5	// fr
	"See top level of forum of users of a course"			// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of users of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of users of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of users of a course"			// Precisa de tradução
#endif
	,
	[ActSeeForCrsTch] =
#if   L==1	// ca
	"See top level of forum of teachers of a course"			// Necessita traducció
#elif L==2	// de
	"See top level of forum of teachers of a course"			// Need Übersetzung
#elif L==3	// en
	"See top level of forum of teachers of a course"
#elif L==4	// es
	"Ver el nivel superior del foro de profesores de una asignatura"
#elif L==5	// fr
	"See top level of forum of teachers of a course"			// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of teachers of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of teachers of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of teachers of a course"			// Precisa de tradução
#endif
	,
	[ActSeeForDegUsr] =
#if   L==1	// ca
	"See top level of forum of users of a degree"				// Necessita traducció
#elif L==2	// de
	"See top level of forum of users of a degree"				// Need Übersetzung
#elif L==3	// en
	"See top level of forum of users of a degree"
#elif L==4	// es
	"Ver el nivel superior del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"See top level of forum of users of a degree"				// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of users of a degree"				// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of users of a degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of users of a degree"				// Precisa de tradução
#endif
	,
	[ActSeeForDegTch] =
#if   L==1	// ca
	"See top level of forum of teachers of a degree"			// Necessita traducció
#elif L==2	// de
	"See top level of forum of teachers of a degree"			// Need Übersetzung
#elif L==3	// en
	"See top level of forum of teachers of a degree"
#elif L==4	// es
	"Ver el nivel superior del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"See top level of forum of teachers of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of teachers of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of teachers of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of teachers of a degree"			// Precisa de tradução
#endif
	,
	[ActSeeForCtrUsr] =
#if   L==1	// ca
	"See top level of forum of users of a center"			// Necessita traducció
#elif L==2	// de
	"See top level of forum of users of a center"			// Need Übersetzung
#elif L==3	// en
	"See top level of forum of users of a center"
#elif L==4	// es
	"Ver el nivel superior del foro de usuarios de un centro"
#elif L==5	// fr
	"See top level of forum of users of a center"			// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of users of a center"			// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of users of a center"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of users of a center"			// Precisa de tradução
#endif
	,
	[ActSeeForCtrTch] =
#if   L==1	// ca
	"See top level of forum of teachers of a center"		// Necessita traducció
#elif L==2	// de
	"See top level of forum of teachers of a center"		// Need Übersetzung
#elif L==3	// en
	"See top level of forum of teachers of a center"
#elif L==4	// es
	"Ver el nivel superior del foro de profesores de un centro"
#elif L==5	// fr
	"See top level of forum of teachers of a center"		// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of teachers of a center"		// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of teachers of a center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of teachers of a center"		// Precisa de tradução
#endif
	,
	[ActSeeForInsUsr] =
#if   L==1	// ca
	"See top level of forum of users of an institution"			// Necessita traducció
#elif L==2	// de
	"See top level of forum of users of an institution"			// Need Übersetzung
#elif L==3	// en
	"See top level of forum of users of an institution"
#elif L==4	// es
	"Ver el nivel superior del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"See top level of forum of users of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of users of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of users of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of users of an institution"			// Precisa de tradução
#endif
	,
	[ActSeeForInsTch] =
#if   L==1	// ca
	"See top level of forum of teachers of an institution"				// Necessita traducció
#elif L==2	// de
	"See top level of forum of teachers of an institution"				// Need Übersetzung
#elif L==3	// en
	"See top level of forum of teachers of an institution"
#elif L==4	// es
	"Ver el nivel superior del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"See top level of forum of teachers of an institution"				// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of teachers of an institution"				// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of teachers of an institution"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of teachers of an institution"				// Precisa de tradução
#endif
	,
	[ActSeeForGenUsr] =
#if   L==1	// ca
	"See top level of general forum of users"		// Necessita traducció
#elif L==2	// de
	"See top level of general forum of users"		// Need Übersetzung
#elif L==3	// en
	"See top level of general forum of users"
#elif L==4	// es
	"Ver el nivel superior del foro de usuarios general"
#elif L==5	// fr
	"See top level of general forum of users"		// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"See top level of general forum of users"		// Bisogno di traduzione
#elif L==8	// pl
	"See top level of general forum of users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of general forum of users"		// Precisa de tradução
#endif
	,
	[ActSeeForGenTch] =
#if   L==1	// ca
	"See top level of general forum of teachers"		// Necessita traducció
#elif L==2	// de
	"See top level of general forum of teachers"		// Need Übersetzung
#elif L==3	// en
	"See top level of general forum of teachers"
#elif L==4	// es
	"Ver el nivel superior del foro de profesores general"
#elif L==5	// fr
	"See top level of general forum of teachers"		// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"See top level of general forum of teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"See top level of general forum of teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of general forum of teachers"		// Precisa de tradução
#endif
	,
	[ActSeeForSWAUsr] =
#if   L==1	// ca
	"See top level of forum of users of the platform"		// Necessita traducció
#elif L==2	// de
	"See top level of forum of users of the platform"		// Need Übersetzung
#elif L==3	// en
	"See top level of forum of users of the platform"
#elif L==4	// es
	"Ver el nivel superior del foro de usuarios de la plataforma"
#elif L==5	// fr
	"See top level of forum of users of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of users of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of users of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of users of the platform"		// Precisa de tradução
#endif
	,
	[ActSeeForSWATch] =
#if   L==1	// ca
	"See top level of forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"See top level of forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"See top level of forum of teachers of the platform"
#elif L==4	// es
	"Ver el nivel superior del foro de profesores de la plataforma"
#elif L==5	// fr
	"See top level of forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Ver el nivel superior del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"See top level of forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"See top level of forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See top level of forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActSeePstForCrsUsr] =
#if   L==1	// ca
	"See the messages of a thread of the forum of users of a course"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of users of a course"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of users of a course"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de usuarios de una asignatura"
#elif L==5	// fr
	"See the messages of a thread of the forum of users of a course"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of users of a course"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of users of a course"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of users of a course"	// Precisa de tradução
#endif
	,
	[ActSeePstForCrsTch] =
#if   L==1	// ca
	"See the messages of a thread of the forum of teachers of a course"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of teachers of a course"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of teachers of a course"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de profesores de una asignatura"
#elif L==5	// fr
	"See the messages of a thread of the forum of teachers of a course"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of teachers of a course"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of teachers of a course"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of teachers of a course"	// Precisa de tradução
#endif
	,
	[ActSeePstForDegUsr] =
#if   L==1	// ca
	"See the messages of a thread of the forum of users of a degree"		// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of users of a degree"		// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of users of a degree"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"See the messages of a thread of the forum of users of a degree"		// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of users of a degree"		// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of users of a degree"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of users of a degree"		// Precisa de tradução
#endif
	,
	[ActSeePstForDegTch] =
#if   L==1	// ca
	"See the messages of a thread of the forum of teachers of a degree"		// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of teachers of a degree"		// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of teachers of a degree"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"See the messages of a thread of the forum of teachers of a degree"		// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of teachers of a degree"		// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of teachers of a degree"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of teachers of a degree"		// Precisa de tradução
#endif
	,
	[ActSeePstForCtrUsr] =
#if   L==1	// ca
	"See the messages of a thread of the forum of users of a center"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of users of a center"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of users of a center"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de usuarios de un centro"
#elif L==5	// fr
	"See the messages of a thread of the forum of users of a center"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de usuarios de un centro"		// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of users of a center"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of users of a center"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of users of a center"	// Precisa de tradução
#endif
	,
	[ActSeePstForCtrTch] =
#if   L==1	// ca
	"See the messages of a thread of the forum of teachers of a center"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of teachers of a center"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of teachers of a center"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de profesores de un centro"
#elif L==5	// fr
	"See the messages of a thread of the forum of teachers of a center"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of teachers of a center"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of teachers of a center"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of teachers of a center"	// Precisa de tradução
#endif
	,
	[ActSeePstForInsUsr] =
#if   L==1	// ca
	"See the messages of a thread of the forum of users of an institution"		// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of users of an institution"		// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of users of an institution"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"See the messages of a thread of the forum of users of an institution"		// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of users of an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of users of an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of users of an institution"		// Precisa de tradução
#endif
	,
	[ActSeePstForInsTch] =
#if   L==1	// ca
	"See the messages of a thread of the forum of teachers of an institution"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of teachers of an institution"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of teachers of an institution"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"See the messages of a thread of the forum of teachers of an institution"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of teachers of an institution"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of teachers of an institution"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of teachers of an institution"	// Precisa de tradução
#endif
	,
	[ActSeePstForGenUsr] =
#if   L==1	// ca
	"See the messages of a thread of the general forum of users"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the general forum of users"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the general forum of users"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de usuarios general"
#elif L==5	// fr
	"See the messages of a thread of the general forum of users"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the general forum of users"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the general forum of users"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the general forum of users"	// Precisa de tradução
#endif
	,
	[ActSeePstForGenTch] =
#if   L==1	// ca
	"See the messages of a thread of the forum of teachers general"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of teachers general"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of teachers general"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de profesores general"
#elif L==5	// fr
	"See the messages of a thread of the forum of teachers general"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of teachers general"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of teachers general"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of teachers general"	// Precisa de tradução
#endif
	,
	[ActSeePstForSWAUsr] =
#if   L==1	// ca
	"See the messages of a thread of the forum of users of the platform"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of users of the platform"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of users of the platform"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de usuarios de la plataforma"
#elif L==5	// fr
	"See the messages of a thread of the forum of users of the platform"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of users of the platform"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of users of the platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of users of the platform"	// Precisa de tradução
#endif
	,
	[ActSeePstForSWATch] =
#if   L==1	// ca
	"See the messages of a thread of the forum of teachers of the platform"	// Necessita traducció
#elif L==2	// de
	"See the messages of a thread of the forum of teachers of the platform"	// Need Übersetzung
#elif L==3	// en
	"See the messages of a thread of the forum of teachers of the platform"
#elif L==4	// es
	"Ver los mensajes de un hilo del foro de profesores de la plataforma"
#elif L==5	// fr
	"See the messages of a thread of the forum of teachers of the platform"	// Besoin de traduction
#elif L==6	// gn
	"Ver los mensajes de un hilo del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"See the messages of a thread of the forum of teachers of the platform"	// Bisogno di traduzione
#elif L==8	// pl
	"See the messages of a thread of the forum of teachers of the platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages of a thread of the forum of teachers of the platform"	// Precisa de tradução
#endif
	,
	[ActRcvThrForCrsUsr] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of users of a course"			// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of users of a course"			// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of users of a course"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Send the first message of a new thread of forum of users of a course"			// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of users of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of users of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of users of a course"			// Precisa de tradução
#endif
	,
	[ActRcvThrForCrsTch] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of teachers of a course"		// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of teachers of a course"		// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of teachers of a course"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de una asignatura"
#elif L==5	// fr
	"Send the first message of a new thread of forum of teachers of a course"		// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of teachers of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of teachers of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of teachers of a course"		// Precisa de tradução
#endif
	,
	[ActRcvThrForDegUsr] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of users of a degree"				// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of users of a degree"				// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of users of a degree"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Send the first message of a new thread of forum of users of a degree"				// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of users of a degree"				// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of users of a degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of users of a degree"				// Precisa de tradução
#endif
	,
	[ActRcvThrForDegTch] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of teachers of a degree"			// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of teachers of a degree"			// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of teachers of a degree"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Send the first message of a new thread of forum of teachers of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of teachers of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of teachers of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of teachers of a degree"			// Precisa de tradução
#endif
	,
	[ActRcvThrForCtrUsr] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of users of center"		// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of users of center"		// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of users of center"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de un centro"
#elif L==5	// fr
	"Send the first message of a new thread of forum of users of center"		// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of users of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of users of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of users of center"		// Precisa de tradução
#endif
	,
	[ActRcvThrForCtrTch] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of teachers of center"		// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of teachers of center"		// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of teachers of center"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de un centro"
#elif L==5	// fr
	"Send the first message of a new thread of forum of teachers of center"		// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of teachers of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of teachers of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of teachers of center"		// Precisa de tradução
#endif
	,
	[ActRcvThrForInsUsr] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of users of an institution"			// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of users of an institution"			// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of users of an institution"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Send the first message of a new thread of forum of users of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of users of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of users of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of users of an institution"			// Precisa de tradução
#endif
	,
	[ActRcvThrForInsTch] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of teachers of an institution"			// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of teachers of an institution"			// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of teachers of an institution"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Send the first message of a new thread of forum of teachers of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of teachers of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of teachers of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of teachers of an institution"			// Precisa de tradução
#endif
	,
	[ActRcvThrForGenUsr] =
#if   L==1	// ca
	"Send the first message of a new thread of general forum of users"		// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of general forum of users"		// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of general forum of users"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios general"
#elif L==5	// fr
	"Send the first message of a new thread of general forum of users"		// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of general forum of users"		// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of general forum of users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of general forum of users"		// Precisa de tradução
#endif
	,
	[ActRcvThrForGenTch] =
#if   L==1	// ca
	"Send the first message of a new thread of general forum of teachers"		// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of general forum of teachers"		// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of general forum of teachers"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores general"
#elif L==5	// fr
	"Send the first message of a new thread of general forum of teachers"		// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of general forum of teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of general forum of teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of general forum of teachers"		// Precisa de tradução
#endif
	,
	[ActRcvThrForSWAUsr] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of users of the platform"		// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of users of the platform"		// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of users of the platform"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Send the first message of a new thread of forum of users of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of users of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of users of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of users of the platform"		// Precisa de tradução
#endif
	,
	[ActRcvThrForSWATch] =
#if   L==1	// ca
	"Send the first message of a new thread of forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"Send the first message of a new thread of forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"Send the first message of a new thread of forum of teachers of the platform"
#elif L==4	// es
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de la plataforma"
#elif L==5	// fr
	"Send the first message of a new thread of forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Enviar el primer mensaje de un nuevo hilo del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Send the first message of a new thread of forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Send the first message of a new thread of forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send the first message of a new thread of forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActRcvRepForCrsUsr] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of users of a course"			// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of users of a course"			// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of users of a course"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of users of a course"			// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of users of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of users of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of users of a course"			// Precisa de tradução
#endif
	,
	[ActRcvRepForCrsTch] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of teachers of a course"			// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of teachers of a course"			// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of teachers of a course"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de una asignatura"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of teachers of a course"			// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of teachers of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of teachers of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of teachers of a course"			// Precisa de tradução
#endif
	,
	[ActRcvRepForDegUsr] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of users of a degree"				// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of users of a degree"				// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of users of a degree"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of users of a degree"				// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of users of a degree"				// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of users of a degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of users of a degree"				// Precisa de tradução
#endif
	,
	[ActRcvRepForDegTch] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of teachers of a degree"				// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of teachers of a degree"				// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of teachers of a degree"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of teachers of a degree"				// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of teachers of a degree"				// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of teachers of a degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of teachers of a degree"				// Precisa de tradução
#endif
	,
	[ActRcvRepForCtrUsr] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of users of center"		// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of users of center"		// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of users of center"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de un centro"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of users of center"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of users of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of users of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of users of center"		// Precisa de tradução
#endif
	,
	[ActRcvRepForCtrTch] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of teachers of center"		// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of teachers of center"		// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of teachers of center"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de un centro"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of teachers of center"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of teachers of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of teachers of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of teachers of center"		// Precisa de tradução
#endif
	,
	[ActRcvRepForInsUsr] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of users of an institution"			// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of users of an institution"			// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of users of an institution"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of users of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of users of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of users of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of users of an institution"			// Precisa de tradução
#endif
	,
	[ActRcvRepForInsTch] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of teachers of an institution"			// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of teachers of an institution"			// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of teachers of an institution"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of teachers of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of teachers of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of teachers of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of teachers of an institution"			// Precisa de tradução
#endif
	,
	[ActRcvRepForGenUsr] =
#if   L==1	// ca
	"Send a reply message in a thread of the general forum of users"		// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the general forum of users"		// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the general forum of users"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios general"
#elif L==5	// fr
	"Send a reply message in a thread of the general forum of users"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the general forum of users"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the general forum of users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the general forum of users"		// Precisa de tradução
#endif
	,
	[ActRcvRepForGenTch] =
#if   L==1	// ca
	"Send a reply message in a thread of the general forum of teachers"		// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the general forum of teachers"		// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the general forum of teachers"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de profesores general"
#elif L==5	// fr
	"Send a reply message in a thread of the general forum of teachers"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the general forum of teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the general forum of teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the general forum of teachers"		// Precisa de tradução
#endif
	,
	[ActRcvRepForSWAUsr] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of users of the platform"		// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of users of the platform"		// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of users of the platform"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of users of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of users of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of users of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of users of the platform"		// Precisa de tradução
#endif
	,
	[ActRcvRepForSWATch] =
#if   L==1	// ca
	"Send a reply message in a thread of the forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"Send a reply message in a thread of the forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"Send a reply message in a thread of the forum of teachers of the platform"
#elif L==4	// es
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de la plataforma"
#elif L==5	// fr
	"Send a reply message in a thread of the forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje de respuesta en un hilo del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Send a reply message in a thread of the forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a reply message in a thread of the forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a reply message in a thread of the forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActReqDelThrCrsUsr] =
#if   L==1	// ca
	"Request removal of a thread of forum of users of a course"				// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of users of a course"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of users of a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Request removal of a thread of forum of users of a course"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of users of a course"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of users of a course"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of users of a course"				// Precisa de tradução
#endif
	,
	[ActReqDelThrCrsTch] =
#if   L==1	// ca
	"Request removal of a thread of forum of teachers of a course"				// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of teachers of a course"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of teachers of a course"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de una asignatura"
#elif L==5	// fr
	"Request removal of a thread of forum of teachers of a course"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of teachers of a course"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of teachers of a course"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of teachers of a course"				// Precisa de tradução
#endif
	,
	[ActReqDelThrDegUsr] =
#if   L==1	// ca
	"Request removal of a thread of forum of users of a degree"					// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of users of a degree"					// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of users of a degree"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Request removal of a thread of forum of users of a degree"					// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of users of a degree"					// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of users of a degree"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of users of a degree"					// Precisa de tradução
#endif
	,
	[ActReqDelThrDegTch] =
#if   L==1	// ca
	"Request removal of a thread of forum of teachers of a degree"					// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of teachers of a degree"					// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of teachers of a degree"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Request removal of a thread of forum of teachers of a degree"					// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of teachers of a degree"					// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of teachers of a degree"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of teachers of a degree"					// Precisa de tradução
#endif
	,
	[ActReqDelThrCtrUsr] =
#if   L==1	// ca
	"Request removal of a thread of forum of users of center"			// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of users of center"			// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of users of center"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de un centro"
#elif L==5	// fr
	"Request removal of a thread of forum of users of center"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of users of center"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of users of center"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of users of center"			// Precisa de tradução
#endif
	,
	[ActReqDelThrCtrTch] =
#if   L==1	// ca
	"Request removal of a thread of forum of teachers of center"				// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of teachers of center"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of teachers of center"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de un centro"
#elif L==5	// fr
	"Request removal of a thread of forum of teachers of center"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of teachers of center"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of teachers of center"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of teachers of center"				// Precisa de tradução
#endif
	,
	[ActReqDelThrInsUsr] =
#if   L==1	// ca
	"Request removal of a thread of forum of users of an institution"				// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of users of an institution"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of users of an institution"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Request removal of a thread of forum of users of an institution"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of users of an institution"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of users of an institution"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of users of an institution"				// Precisa de tradução
#endif
	,
	[ActReqDelThrInsTch] =
#if   L==1	// ca
	"Request removal of a thread of forum of teachers of an institution"				// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of teachers of an institution"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of teachers of an institution"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Request removal of a thread of forum of teachers of an institution"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of teachers of an institution"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of teachers of an institution"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of teachers of an institution"				// Precisa de tradução
#endif
	,
	[ActReqDelThrGenUsr] =
#if   L==1	// ca
	"Request removal of a thread of general forum of users"				// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of general forum of users"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of general forum of users"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios general"
#elif L==5	// fr
	"Request removal of a thread of general forum of users"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of general forum of users"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of general forum of users"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of general forum of users"				// Precisa de tradução
#endif
	,
	[ActReqDelThrGenTch] =
#if   L==1	// ca
	"Request removal of a thread of general forum of teachers"			// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of general forum of teachers"			// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of general forum of teachers"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores general"
#elif L==5	// fr
	"Request removal of a thread of general forum of teachers"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of general forum of teachers"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of general forum of teachers"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of general forum of teachers"			// Precisa de tradução
#endif
	,
	[ActReqDelThrSWAUsr] =
#if   L==1	// ca
	"Request removal of a thread of forum of users of the platform"				// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of users of the platform"				// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of users of the platform"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Request removal of a thread of forum of users of the platform"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of users of the platform"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of users of the platform"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of users of the platform"				// Precisa de tradução
#endif
	,
	[ActReqDelThrSWATch] =
#if   L==1	// ca
	"Request removal of a thread of forum of teachers of the platform"			// Necessita traducció
#elif L==2	// de
	"Request removal of a thread of forum of teachers of the platform"			// Need Übersetzung
#elif L==3	// en
	"Request removal of a thread of forum of teachers of the platform"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de la plataforma"
#elif L==5	// fr
	"Request removal of a thread of forum of teachers of the platform"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un hilo del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Request removal of a thread of forum of teachers of the platform"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of a thread of forum of teachers of the platform"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of a thread of forum of teachers of the platform"			// Precisa de tradução
#endif
	,
	[ActDelThrForCrsUsr] =
#if   L==1	// ca
	"Remove a thread of forum of users of a course"			// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of users of a course"			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of a course"
#elif L==4	// es
	"Eliminar un hilo del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Remove a thread of forum of users of a course"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of users of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of users of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of users of a course"			// Precisa de tradução
#endif
	,
	[ActDelThrForCrsTch] =
#if   L==1	// ca
	"Remove a thread of forum of teachers of a course"		// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of teachers of a course"		// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of a course"
#elif L==4	// es
	"Eliminar un hilo del foro de profesores de una asignatura"
#elif L==5	// fr
	"Remove a thread of forum of teachers of a course"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of teachers of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of teachers of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of teachers of a course"		// Precisa de tradução
#endif
	,
	[ActDelThrForDegUsr] =
#if   L==1	// ca
	"Remove a thread of forum of users of a degree"				// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of users of a degree"				// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of a degree"
#elif L==4	// es
	"Eliminar un hilo del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Remove a thread of forum of users of a degree"				// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of users of a degree"				// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of users of a degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of users of a degree"				// Precisa de tradução
#endif
	,
	[ActDelThrForDegTch] =
#if   L==1	// ca
	"Remove a thread of forum of teachers of a degree"			// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of teachers of a degree"			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of a degree"
#elif L==4	// es
	"Eliminar un hilo del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Remove a thread of forum of teachers of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of teachers of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of teachers of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of teachers of a degree"			// Precisa de tradução
#endif
	,
	[ActDelThrForCtrUsr] =
#if   L==1	// ca
	"Remove a thread of forum of users of center"		// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of users of center"		// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of center"
#elif L==4	// es
	"Eliminar un hilo del foro de usuarios de un centro"
#elif L==5	// fr
	"Remove a thread of forum of users of center"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of users of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of users of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of users of center"		// Precisa de tradução
#endif
	,
	[ActDelThrForCtrTch] =
#if   L==1	// ca
	"Remove a thread of forum of teachers of center"	// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of teachers of center"	// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of center"
#elif L==4	// es
	"Eliminar un hilo del foro de profesores de un centro"
#elif L==5	// fr
	"Remove a thread of forum of teachers of center"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of teachers of center"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of teachers of center"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of teachers of center"	// Precisa de tradução
#endif
	,
	[ActDelThrForInsUsr] =
#if   L==1	// ca
	"Remove a thread of forum of users of an institution"			// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of users of an institution"			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of an institution"
#elif L==4	// es
	"Eliminar un hilo del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Remove a thread of forum of users of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of users of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of users of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of users of an institution"			// Precisa de tradução
#endif
	,
	[ActDelThrForInsTch] =
#if   L==1	// ca
	"Remove a thread of forum of teachers of an institution"		// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of teachers of an institution"		// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of an institution"
#elif L==4	// es
	"Eliminar un hilo del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Remove a thread of forum of teachers of an institution"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of teachers of an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of teachers of an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of teachers of an institution"		// Precisa de tradução
#endif
	,
	[ActDelThrForGenUsr] =
#if   L==1	// ca
	"Remove a thread of general forum of users"	// Necessita traducció
#elif L==2	// de
	"Remove a thread of general forum of users"	// Need Übersetzung
#elif L==3	// en
	"Remove a thread of general forum of users"
#elif L==4	// es
	"Eliminar un hilo del foro de usuarios general"
#elif L==5	// fr
	"Remove a thread of general forum of users"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of general forum of users"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of general forum of users"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of general forum of users"	// Precisa de tradução
#endif
	,
	[ActDelThrForGenTch] =
#if   L==1	// ca
	"Remove a thread of general forum of teachers"		// Necessita traducció
#elif L==2	// de
	"Remove a thread of general forum of teachers"		// Need Übersetzung
#elif L==3	// en
	"Remove a thread of general forum of teachers"
#elif L==4	// es
	"Eliminar un hilo del foro de profesores general"
#elif L==5	// fr
	"Remove a thread of general forum of teachers"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of general forum of teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of general forum of teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of general forum of teachers"		// Precisa de tradução
#endif
	,
	[ActDelThrForSWAUsr] =
#if   L==1	// ca
	"Remove a thread of forum of users of the platform"		// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of users of the platform"		// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of the platform"
#elif L==4	// es
	"Eliminar un hilo del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Remove a thread of forum of users of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of users of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of users of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of users of the platform"		// Precisa de tradução
#endif
	,
	[ActDelThrForSWATch] =
#if   L==1	// ca
	"Remove a thread of forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"Remove a thread of forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of the platform"
#elif L==4	// es
	"Eliminar un hilo del foro de profesores de la plataforma"
#elif L==5	// fr
	"Remove a thread of forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un hilo del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Remove a thread of forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a thread of forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a thread of forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActCutThrForCrsUsr] =
#if   L==1	// ca
	"Cut a thread of forum of users of a course"		// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of users of a course"		// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of a course"
#elif L==4	// es
	"Cortar un hilo del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Cut a thread of forum of users of a course"		// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of users of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of users of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of users of a course"		// Precisa de tradução
#endif
	,
	[ActCutThrForCrsTch] =
#if   L==1	// ca
	"Cut a thread of forum of teachers of a course"			// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of teachers of a course"			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of a course"
#elif L==4	// es
	"Cortar un hilo del foro de profesores de una asignatura"
#elif L==5	// fr
	"Cut a thread of forum of teachers of a course"			// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of teachers of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of teachers of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of teachers of a course"			// Precisa de tradução
#endif
	,
	[ActCutThrForDegUsr] =
#if   L==1	// ca
	"Cut a thread of forum of users of a degree"			// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of users of a degree"			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of a degree"
#elif L==4	// es
	"Cortar un hilo del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Cut a thread of forum of users of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of users of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of users of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of users of a degree"			// Precisa de tradução
#endif
	,
	[ActCutThrForDegTch] =
#if   L==1	// ca
	"Cut a thread of forum of teachers of a degree"				// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of teachers of a degree"				// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of a degree"
#elif L==4	// es
	"Cortar un hilo del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Cut a thread of forum of teachers of a degree"				// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of teachers of a degree"				// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of teachers of a degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of teachers of a degree"				// Precisa de tradução
#endif
	,
	[ActCutThrForCtrUsr] =
#if   L==1	// ca
	"Cut a thread of forum of users of center"		// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of users of center"		// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of center"
#elif L==4	// es
	"Cortar un hilo del foro de usuarios de un centro"
#elif L==5	// fr
	"Cut a thread of forum of users of center"		// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of users of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of users of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of users of center"		// Precisa de tradução
#endif
	,
	[ActCutThrForCtrTch] =
#if   L==1	// ca
	"Cut a thread of forum of teachers of center"		// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of teachers of center"		// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of center"
#elif L==4	// es
	"Cortar un hilo del foro de profesores de un centro"
#elif L==5	// fr
	"Cut a thread of forum of teachers of center"		// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of teachers of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of teachers of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of teachers of center"		// Precisa de tradução
#endif
	,
	[ActCutThrForInsUsr] =
#if   L==1	// ca
	"Cut a thread of forum of users of an institution"		// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of users of an institution"		// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of an institution"
#elif L==4	// es
	"Cortar un hilo del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Cut a thread of forum of users of an institution"		// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of users of an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of users of an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of users of an institution"		// Precisa de tradução
#endif
	,
	[ActCutThrForInsTch] =
#if   L==1	// ca
	"Cut a thread of forum of teachers of an institution"			// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of teachers of an institution"			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of an institution"
#elif L==4	// es
	"Cortar un hilo del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Cut a thread of forum of teachers of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of teachers of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of teachers of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of teachers of an institution"			// Precisa de tradução
#endif
	,
	[ActCutThrForGenUsr] =
#if   L==1	// ca
	"Cut a thread of general forum of users"	// Necessita traducció
#elif L==2	// de
	"Cut a thread of general forum of users"	// Need Übersetzung
#elif L==3	// en
	"Cut a thread of general forum of users"
#elif L==4	// es
	"Cortar un hilo del foro de usuarios general"
#elif L==5	// fr
	"Cut a thread of general forum of users"	// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of general forum of users"	// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of general forum of users"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of general forum of users"	// Precisa de tradução
#endif
	,
	[ActCutThrForGenTch] =
#if   L==1	// ca
	"Cut a thread of general forum of teachers"	// Necessita traducció
#elif L==2	// de
	"Cut a thread of general forum of teachers"	// Need Übersetzung
#elif L==3	// en
	"Cut a thread of general forum of teachers"
#elif L==4	// es
	"Cortar un hilo del foro de profesores general"
#elif L==5	// fr
	"Cut a thread of general forum of teachers"	// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of general forum of teachers"	// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of general forum of teachers"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of general forum of teachers"	// Precisa de tradução
#endif
	,
	[ActCutThrForSWAUsr] =
#if   L==1	// ca
	"Cut a thread of forum of users of the platform"	// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of users of the platform"	// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of the platform"
#elif L==4	// es
	"Cortar un hilo del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Cut a thread of forum of users of the platform"	// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of users of the platform"	// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of users of the platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of users of the platform"	// Precisa de tradução
#endif
	,
	[ActCutThrForSWATch] =
#if   L==1	// ca
	"Cut a thread of forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"Cut a thread of forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of the platform"
#elif L==4	// es
	"Cortar un hilo del foro de profesores de la plataforma"
#elif L==5	// fr
	"Cut a thread of forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Cortar un hilo del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Cut a thread of forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Cut a thread of forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Cut a thread of forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActPasThrForCrsUsr] =
#if   L==1	// ca
	"Paste a thread of forum of users of a course"		// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of users of a course"		// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of a course"
#elif L==4	// es
	"Pegar un hilo del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Paste a thread of forum of users of a course"		// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of users of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of users of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of users of a course"		// Precisa de tradução
#endif
	,
	[ActPasThrForCrsTch] =
#if   L==1	// ca
	"Paste a thread of forum of teachers of a course"		// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of teachers of a course"		// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of a course"
#elif L==4	// es
	"Pegar un hilo del foro de profesores de una asignatura"
#elif L==5	// fr
	"Paste a thread of forum of teachers of a course"		// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of teachers of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of teachers of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of teachers of a course"		// Precisa de tradução
#endif
	,
	[ActPasThrForDegUsr] =
#if   L==1	// ca
	"Paste a thread of forum of users of a degree"			// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of users of a degree"			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of a degree"
#elif L==4	// es
	"Pegar un hilo del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Paste a thread of forum of users of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of users of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of users of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of users of a degree"			// Precisa de tradução
#endif
	,
	[ActPasThrForDegTch] =
#if   L==1	// ca
	"Paste a thread of forum of teachers of a degree"		// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of teachers of a degree"		// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of a degree"
#elif L==4	// es
	"Pegar un hilo del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Paste a thread of forum of teachers of a degree"		// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of teachers of a degree"		// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of teachers of a degree"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of teachers of a degree"		// Precisa de tradução
#endif
	,
	[ActPasThrForCtrUsr] =
#if   L==1	// ca
	"Paste a thread of forum of users of center"		// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of users of center"		// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of center"
#elif L==4	// es
	"Pegar un hilo del foro de usuarios de un centro"
#elif L==5	// fr
	"Paste a thread of forum of users of center"		// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of users of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of users of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of users of center"		// Precisa de tradução
#endif
	,
	[ActPasThrForCtrTch] =
#if   L==1	// ca
	"Paste a thread of forum of teachers of center"		// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of teachers of center"		// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of center"
#elif L==4	// es
	"Pegar un hilo del foro de profesores de un centro"
#elif L==5	// fr
	"Paste a thread of forum of teachers of center"		// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of teachers of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of teachers of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of teachers of center"		// Precisa de tradução
#endif
	,
	[ActPasThrForInsUsr] =
#if   L==1	// ca
	"Paste a thread of forum of users of an institution"		// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of users of an institution"		// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of an institution"
#elif L==4	// es
	"Pegar un hilo del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Paste a thread of forum of users of an institution"		// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of users of an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of users of an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of users of an institution"		// Precisa de tradução
#endif
	,
	[ActPasThrForInsTch] =
#if   L==1	// ca
	"Paste a thread of forum of teachers of an institution"			// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of teachers of an institution"			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of an institution"
#elif L==4	// es
	"Pegar un hilo del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Paste a thread of forum of teachers of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of teachers of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of teachers of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of teachers of an institution"			// Precisa de tradução
#endif
	,
	[ActPasThrForGenUsr] =
#if   L==1	// ca
	"Paste a thread of general forum of users"	// Necessita traducció
#elif L==2	// de
	"Paste a thread of general forum of users"	// Need Übersetzung
#elif L==3	// en
	"Paste a thread of general forum of users"
#elif L==4	// es
	"Pegar un hilo del foro de usuarios general"
#elif L==5	// fr
	"Paste a thread of general forum of users"	// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of general forum of users"	// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of general forum of users"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of general forum of users"	// Precisa de tradução
#endif
	,
	[ActPasThrForGenTch] =
#if   L==1	// ca
	"Paste a thread of general forum of teachers"	// Necessita traducció
#elif L==2	// de
	"Paste a thread of general forum of teachers"	// Need Übersetzung
#elif L==3	// en
	"Paste a thread of general forum of teachers"
#elif L==4	// es
	"Pegar un hilo del foro de profesores general"
#elif L==5	// fr
	"Paste a thread of general forum of teachers"	// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of general forum of teachers"	// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of general forum of teachers"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of general forum of teachers"	// Precisa de tradução
#endif
	,
	[ActPasThrForSWAUsr] =
#if   L==1	// ca
	"Paste a thread of forum of users of the platform"	// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of users of the platform"	// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of the platform"
#elif L==4	// es
	"Pegar un hilo del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Paste a thread of forum of users of the platform"	// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of users of the platform"	// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of users of the platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of users of the platform"	// Precisa de tradução
#endif
	,
	[ActPasThrForSWATch] =
#if   L==1	// ca
	"Paste a thread of forum of teachers of the platform"	// Necessita traducció
#elif L==2	// de
	"Paste a thread of forum of teachers of the platform"	// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of the platform"
#elif L==4	// es
	"Pegar un hilo del foro de profesores de la plataforma"
#elif L==5	// fr
	"Paste a thread of forum of teachers of the platform"	// Besoin de traduction
#elif L==6	// gn
	"Pegar un hilo del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Paste a thread of forum of teachers of the platform"	// Bisogno di traduzione
#elif L==8	// pl
	"Paste a thread of forum of teachers of the platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Paste a thread of forum of teachers of the platform"	// Precisa de tradução
#endif
	,
	[ActDelPstForCrsUsr] =
#if   L==1	// ca
	"Remove a message of forum of users of a course"		// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of users of a course"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of a course"
#elif L==4	// es
	"Eliminar un mensaje del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Remove a message of forum of users of a course"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of users of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of users of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of users of a course"		// Precisa de tradução
#endif
	,
	[ActDelPstForCrsTch] =
#if   L==1	// ca
	"Remove a message of forum of teachers of a course"		// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of teachers of a course"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of a course"
#elif L==4	// es
	"Eliminar un mensaje del foro de profesores de una asignatura"
#elif L==5	// fr
	"Remove a message of forum of teachers of a course"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of teachers of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of teachers of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of teachers of a course"		// Precisa de tradução
#endif
	,
	[ActDelPstForDegUsr] =
#if   L==1	// ca
	"Remove a message of forum of users of a degree"			// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of users of a degree"			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of a degree"
#elif L==4	// es
	"Eliminar un mensaje del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Remove a message of forum of users of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of users of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of users of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of users of a degree"			// Precisa de tradução
#endif
	,
	[ActDelPstForDegTch] =
#if   L==1	// ca
	"Remove a message of forum of teachers of a degree"			// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of teachers of a degree"			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of a degree"
#elif L==4	// es
	"Eliminar un mensaje del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Remove a message of forum of teachers of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of teachers of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of teachers of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of teachers of a degree"			// Precisa de tradução
#endif
	,
	[ActDelPstForCtrUsr] =
#if   L==1	// ca
	"Remove a message of forum of users of a center"	// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of users of a center"	// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of a center"
#elif L==4	// es
	"Eliminar un mensaje del foro de usuarios de un centro"
#elif L==5	// fr
	"Remove a message of forum of users of a center"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of users of a center"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of users of a center"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of users of a center"	// Precisa de tradução
#endif
	,
	[ActDelPstForCtrTch] =
#if   L==1	// ca
	"Remove a message of forum of teachers of a center"		// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of teachers of a center"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of a center"
#elif L==4	// es
	"Eliminar un mensaje del foro de profesores de un centro"
#elif L==5	// fr
	"Remove a message of forum of teachers of a center"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of teachers of a center"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of teachers of a center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of teachers of a center"		// Precisa de tradução
#endif
	,
	[ActDelPstForInsUsr] =
#if   L==1	// ca
	"Remove a message of forum of users of an institution"			// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of users of an institution"			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of an institution"
#elif L==4	// es
	"Eliminar un mensaje del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Remove a message of forum of users of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of users of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of users of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of users of an institution"			// Precisa de tradução
#endif
	,
	[ActDelPstForInsTch] =
#if   L==1	// ca
	"Remove a message of forum of teachers of an institution"		// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of teachers of an institution"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of an institution"
#elif L==4	// es
	"Eliminar un mensaje del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Remove a message of forum of teachers of an institution"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of teachers of an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of teachers of an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of teachers of an institution"		// Precisa de tradução
#endif
	,
	[ActDelPstForGenUsr] =
#if   L==1	// ca
	"Remove a message of general forum of users"		// Necessita traducció
#elif L==2	// de
	"Remove a message of general forum of users"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of general forum of users"
#elif L==4	// es
	"Eliminar un mensaje del foro de usuarios general"
#elif L==5	// fr
	"Remove a message of general forum of users"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of general forum of users"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of general forum of users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of general forum of users"		// Precisa de tradução
#endif
	,
	[ActDelPstForGenTch] =
#if   L==1	// ca
	"Remove a message of general forum of teachers"		// Necessita traducció
#elif L==2	// de
	"Remove a message of general forum of teachers"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of general forum of teachers"
#elif L==4	// es
	"Eliminar un mensaje del foro de profesores general"
#elif L==5	// fr
	"Remove a message of general forum of teachers"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of general forum of teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of general forum of teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of general forum of teachers"		// Precisa de tradução
#endif
	,
	[ActDelPstForSWAUsr] =
#if   L==1	// ca
	"Remove a message of forum of users of the platform"		// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of users of the platform"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of the platform"
#elif L==4	// es
	"Eliminar un mensaje del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Remove a message of forum of users of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of users of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of users of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of users of the platform"		// Precisa de tradução
#endif
	,
	[ActDelPstForSWATch] =
#if   L==1	// ca
	"Remove a message of forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"Remove a message of forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of the platform"
#elif L==4	// es
	"Eliminar un mensaje del foro de profesores de la plataforma"
#elif L==5	// fr
	"Remove a message of forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Remove a message of forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message of forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message of forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActEnbPstForCrsUsr] =
#if   L==1	// ca
	"Enable a message of forum of users of a course"		// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of users of a course"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of a course"
#elif L==4	// es
	"Habilitar un mensaje del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Enable a message of forum of users of a course"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of users of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of users of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of users of a course"		// Precisa de tradução
#endif
	,
	[ActEnbPstForCrsTch] =
#if   L==1	// ca
	"Enable a message of forum of teachers of a course"		// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of teachers of a course"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of a course"
#elif L==4	// es
	"Habilitar un mensaje del foro de profesores de una asignatura"
#elif L==5	// fr
	"Enable a message of forum of teachers of a course"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of teachers of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of teachers of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of teachers of a course"		// Precisa de tradução
#endif
	,
	[ActEnbPstForDegUsr] =
#if   L==1	// ca
	"Enable a message of forum of users of a degree"			// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of users of a degree"			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of a degree"
#elif L==4	// es
	"Habilitar un mensaje del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Enable a message of forum of users of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of users of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of users of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of users of a degree"			// Precisa de tradução
#endif
	,
	[ActEnbPstForDegTch] =
#if   L==1	// ca
	"Enable a message of forum of teachers of a degree"			// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of teachers of a degree"			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of a degree"
#elif L==4	// es
	"Habilitar un mensaje del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Enable a message of forum of teachers of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of teachers of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of teachers of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of teachers of a degree"			// Precisa de tradução
#endif
	,
	[ActEnbPstForCtrUsr] =
#if   L==1	// ca
	"Enable a message of forum of users of center"			// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of users of center"			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of center"
#elif L==4	// es
	"Habilitar un mensaje del foro de usuarios de un centro"
#elif L==5	// fr
	"Enable a message of forum of users of center"			// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of users of center"			// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of users of center"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of users of center"			// Precisa de tradução
#endif
	,
	[ActEnbPstForCtrTch] =
#if   L==1	// ca
	"Enable a message of forum of teachers of center"		// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of teachers of center"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of center"
#elif L==4	// es
	"Habilitar un mensaje del foro de profesores de un centro"
#elif L==5	// fr
	"Enable a message of forum of teachers of center"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of teachers of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of teachers of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of teachers of center"		// Precisa de tradução
#endif
	,
	[ActEnbPstForInsUsr] =
#if   L==1	// ca
	"Enable a message of forum of users of an institution"			// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of users of an institution"			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of an institution"
#elif L==4	// es
	"Habilitar un mensaje del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Enable a message of forum of users of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of users of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of users of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of users of an institution"			// Precisa de tradução
#endif
	,
	[ActEnbPstForInsTch] =
#if   L==1	// ca
	"Enable a message of forum of teachers of an institution"		// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of teachers of an institution"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of an institution"
#elif L==4	// es
	"Habilitar un mensaje del foro de profesores de una instituci&oacute;n"
#elif L==5	// fr
	"Enable a message of forum of teachers of an institution"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de profesores de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of teachers of an institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of teachers of an institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of teachers of an institution"		// Precisa de tradução
#endif
	,
	[ActEnbPstForGenUsr] =
#if   L==1	// ca
	"Enable a message of general forum of users"		// Necessita traducció
#elif L==2	// de
	"Enable a message of general forum of users"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of general forum of users"
#elif L==4	// es
	"Habilitar un mensaje del foro de usuarios general"
#elif L==5	// fr
	"Enable a message of general forum of users"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of general forum of users"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of general forum of users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of general forum of users"		// Precisa de tradução
#endif
	,
	[ActEnbPstForGenTch] =
#if   L==1	// ca
	"Enable a message of general forum of teachers"		// Necessita traducció
#elif L==2	// de
	"Enable a message of general forum of teachers"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of general forum of teachers"
#elif L==4	// es
	"Habilitar un mensaje del foro de profesores general"
#elif L==5	// fr
	"Enable a message of general forum of teachers"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of general forum of teachers"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of general forum of teachers"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of general forum of teachers"		// Precisa de tradução
#endif
	,
	[ActEnbPstForSWAUsr] =
#if   L==1	// ca
	"Enable a message of forum of users of the platform"		// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of users of the platform"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of the platform"
#elif L==4	// es
	"Habilitar un mensaje del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Enable a message of forum of users of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of users of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of users of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of users of the platform"		// Precisa de tradução
#endif
	,
	[ActEnbPstForSWATch] =
#if   L==1	// ca
	"Enable a message of forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"Enable a message of forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of the platform"
#elif L==4	// es
	"Habilitar un mensaje del foro de profesores de la plataforma"
#elif L==5	// fr
	"Enable a message of forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Habilitar un mensaje del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Enable a message of forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Enable a message of forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enable a message of forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActDisPstForCrsUsr] =
#if   L==1	// ca
	"Disable a message of forum of users of a course"		// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of users of a course"		// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of a course"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de usuarios de una asignatura"
#elif L==5	// fr
	"Disable a message of forum of users of a course"		// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de usuarios de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of users of a course"		// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of users of a course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of users of a course"		// Precisa de tradução
#endif
	,
	[ActDisPstForCrsTch] =
#if   L==1	// ca
	"Disable a message of forum of teachers of a course"			// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of teachers of a course"			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of a course"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de profesores de una asignatura"
#elif L==5	// fr
	"Disable a message of forum of teachers of a course"			// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de profesores de una asignatura"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of teachers of a course"			// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of teachers of a course"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of teachers of a course"			// Precisa de tradução
#endif
	,
	[ActDisPstForDegUsr] =
#if   L==1	// ca
	"Disable a message of forum of users of a degree"			// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of users of a degree"			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of a degree"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de usuarios de una titulaci&oacute;n"
#elif L==5	// fr
	"Disable a message of forum of users of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de usuarios de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of users of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of users of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of users of a degree"			// Precisa de tradução
#endif
	,
	[ActDisPstForDegTch] =
#if   L==1	// ca
	"Disable a message of forum of teachers of a degree"				// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of teachers of a degree"				// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of a degree"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de profesores de una titulaci&oacute;n"
#elif L==5	// fr
	"Disable a message of forum of teachers of a degree"				// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de profesores de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of teachers of a degree"				// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of teachers of a degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of teachers of a degree"				// Precisa de tradução
#endif
	,
	[ActDisPstForCtrUsr] =
#if   L==1	// ca
	"Disable a message of forum of users of center"			// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of users of center"			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of center"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de usuarios de un centro"
#elif L==5	// fr
	"Disable a message of forum of users of center"			// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de usuarios de un centro"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of users of center"			// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of users of center"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of users of center"			// Precisa de tradução
#endif
	,
	[ActDisPstForCtrTch] =
#if   L==1	// ca
	"Disable a message of forum of teachers of center"		// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of teachers of center"		// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of center"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de profesores de un centro"
#elif L==5	// fr
	"Disable a message of forum of teachers of center"		// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de profesores de un centro"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of teachers of center"		// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of teachers of center"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of teachers of center"		// Precisa de tradução
#endif
	,
	[ActDisPstForInsUsr] =
#if   L==1	// ca
	"Disable a message of forum of users of an institution"			// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of users of an institution"			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of an institution"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de usuarios de una instituci&oacute;n"
#elif L==5	// fr
	"Disable a message of forum of users of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de usuarios de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of users of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of users of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of users of an institution"			// Precisa de tradução
#endif
	,
	[ActDisPstForInsTch] =
#if   L==1	// ca
	"Disable a message of forum of teachers of an institution"			// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of teachers of an institution"			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of an institution"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de profesofres de una instituci&oacute;n"
#elif L==5	// fr
	"Disable a message of forum of teachers of an institution"			// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de profesofres de una instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of teachers of an institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of teachers of an institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of teachers of an institution"			// Precisa de tradução
#endif
	,
	[ActDisPstForGenUsr] =
#if   L==1	// ca
	"Disable a message of general forum of users"		// Necessita traducció
#elif L==2	// de
	"Disable a message of general forum of users"		// Need Übersetzung
#elif L==3	// en
	"Disable a message of general forum of users"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de usuarios general"
#elif L==5	// fr
	"Disable a message of general forum of users"		// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de usuarios general"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of general forum of users"		// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of general forum of users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of general forum of users"		// Precisa de tradução
#endif
	,
	[ActDisPstForGenTch] =
#if   L==1	// ca
	"Disable a message of general forum of teachers"	// Necessita traducció
#elif L==2	// de
	"Disable a message of general forum of teachers"	// Need Übersetzung
#elif L==3	// en
	"Disable a message of general forum of teachers"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de profesores general"
#elif L==5	// fr
	"Disable a message of general forum of teachers"	// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de profesores general"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of general forum of teachers"	// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of general forum of teachers"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of general forum of teachers"	// Precisa de tradução
#endif
	,
	[ActDisPstForSWAUsr] =
#if   L==1	// ca
	"Disable a message of forum of users of the platform"		// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of users of the platform"		// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of the platform"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de usuarios de la plataforma"
#elif L==5	// fr
	"Disable a message of forum of users of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de usuarios de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of users of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of users of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of users of the platform"		// Precisa de tradução
#endif
	,
	[ActDisPstForSWATch] =
#if   L==1	// ca
	"Disable a message of forum of teachers of the platform"		// Necessita traducció
#elif L==2	// de
	"Disable a message of forum of teachers of the platform"		// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of the platform"
#elif L==4	// es
	"Inhabilitar un mensaje del foro de profesores de la plataforma"
#elif L==5	// fr
	"Disable a message of forum of teachers of the platform"		// Besoin de traduction
#elif L==6	// gn
	"Inhabilitar un mensaje del foro de profesores de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Disable a message of forum of teachers of the platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Disable a message of forum of teachers of the platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Disable a message of forum of teachers of the platform"		// Precisa de tradução
#endif
	,
	[ActCht] =
#if   L==1	// ca
	"Enter in a chat room to chat"			// Necessita traducció
#elif L==2	// de
	"Enter in a chat room to chat"			// Need Übersetzung
#elif L==3	// en
	"Enter in a chat room to chat"
#elif L==4	// es
	"Entrar en una sala de chat para chatear"
#elif L==5	// fr
	"Enter in a chat room to chat"			// Besoin de traduction
#elif L==6	// gn
	"Entrar en una sala de chat para chatear"	// Okoteve traducción
#elif L==7	// it
	"Enter in a chat room to chat"			// Bisogno di traduzione
#elif L==8	// pl
	"Enter in a chat room to chat"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Enter in a chat room to chat"			// Precisa de tradução
#endif
	,
	[ActReqMsgUsr] =
#if   L==1	// ca
	"Write message to other users"		// Necessita traducció
#elif L==2	// de
	"Write message to other users"		// Need Übersetzung
#elif L==3	// en
	"Write message to other users"
#elif L==4	// es
	"Redactar mensaje a otros usuarios"
#elif L==5	// fr
	"Write message to other users"		// Besoin de traduction
#elif L==6	// gn
	"Redactar mensaje a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Write message to other users"		// Bisogno di traduzione
#elif L==8	// pl
	"Write message to other users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Write message to other users"		// Precisa de tradução
#endif
	,
	[ActSeeSntMsg] =
#if   L==1	// ca
	"See the messages sent to other users"		// Necessita traducció
#elif L==2	// de
	"See the messages sent to other users"		// Need Übersetzung
#elif L==3	// en
	"See the messages sent to other users"
#elif L==4	// es
	"Ver mensajes enviados a otros usuarios"
#elif L==5	// fr
	"See the messages sent to other users"		// Besoin de traduction
#elif L==6	// gn
	"Ver mensajes enviados a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"See the messages sent to other users"		// Bisogno di traduzione
#elif L==8	// pl
	"See the messages sent to other users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the messages sent to other users"		// Precisa de tradução
#endif
	,
	[ActRcvMsgUsr] =
#if   L==1	// ca
	"Send a message to other users"		// Necessita traducció
#elif L==2	// de
	"Send a message to other users"		// Need Übersetzung
#elif L==3	// en
	"Send a message to other users"
#elif L==4	// es
	"Enviar un mensaje a otros usuarios"
#elif L==5	// fr
	"Send a message to other users"		// Besoin de traduction
#elif L==6	// gn
	"Enviar un mensaje a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Send a message to other users"		// Bisogno di traduzione
#elif L==8	// pl
	"Send a message to other users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send a message to other users"		// Precisa de tradução
#endif
	,
	[ActReqDelAllSntMsg] =
#if   L==1	// ca
	"Request removal of all messages sent to other users"					// Necessita traducció
#elif L==2	// de
	"Request removal of all messages sent to other users"					// Need Übersetzung
#elif L==3	// en
	"Request removal of all messages sent to other users"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de todos los mensajes enviados a otros usuarios"
#elif L==5	// fr
	"Request removal of all messages sent to other users"					// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de todos los mensajes enviados a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Request removal of all messages sent to other users"					// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of all messages sent to other users"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of all messages sent to other users"					// Precisa de tradução
#endif
	,
	[ActReqDelAllRcvMsg] =
#if   L==1	// ca
	"Request removal of all messages received from other users"				// Necessita traducció
#elif L==2	// de
	"Request removal of all messages received from other users"				// Need Übersetzung
#elif L==3	// en
	"Request removal of all messages received from other users"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de todos los mensajes recibidos de otros usuarios"
#elif L==5	// fr
	"Request removal of all messages received from other users"				// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de todos los mensajes recibidos de otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Request removal of all messages received from other users"				// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of all messages received from other users"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of all messages received from other users"				// Precisa de tradução
#endif
	,
	[ActDelAllSntMsg] =
#if   L==1	// ca
	"Remove all messages sent to other users"		// Necessita traducció
#elif L==2	// de
	"Remove all messages sent to other users"		// Need Übersetzung
#elif L==3	// en
	"Remove all messages sent to other users"
#elif L==4	// es
	"Eliminar todos los mensajes enviados a otros usuarios"
#elif L==5	// fr
	"Remove all messages sent to other users"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar todos los mensajes enviados a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Remove all messages sent to other users"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove all messages sent to other users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove all messages sent to other users"		// Precisa de tradução
#endif
	,
	[ActDelAllRcvMsg] =
#if   L==1	// ca
	"Remove all messages received from other users"			// Necessita traducció
#elif L==2	// de
	"Remove all messages received from other users"			// Need Übersetzung
#elif L==3	// en
	"Remove all messages received from other users"
#elif L==4	// es
	"Eliminar todos los mensajes recibidos de otros usuarios"
#elif L==5	// fr
	"Remove all messages received from other users"			// Besoin de traduction
#elif L==6	// gn
	"Eliminar todos los mensajes recibidos de otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Remove all messages received from other users"			// Bisogno di traduzione
#elif L==8	// pl
	"Remove all messages received from other users"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove all messages received from other users"			// Precisa de tradução
#endif
	,
	[ActDelSntMsg] =
#if   L==1	// ca
	"Remove a message sent to other users"		// Necessita traducció
#elif L==2	// de
	"Remove a message sent to other users"		// Need Übersetzung
#elif L==3	// en
	"Remove a message sent to other users"
#elif L==4	// es
	"Eliminar un mensaje enviado a otros usuarios"
#elif L==5	// fr
	"Remove a message sent to other users"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje enviado a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Remove a message sent to other users"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message sent to other users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message sent to other users"		// Precisa de tradução
#endif
	,
	[ActDelRcvMsg] =
#if   L==1	// ca
	"Remove a message received from other user"		// Necessita traducció
#elif L==2	// de
	"Remove a message received from other user"		// Need Übersetzung
#elif L==3	// en
	"Remove a message received from other user"
#elif L==4	// es
	"Eliminar un mensaje recibido de otros usuarios"
#elif L==5	// fr
	"Remove a message received from other user"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar un mensaje recibido de otros usuarios"	// Okoteve traducción
#elif L==7	// it
	"Remove a message received from other user"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove a message received from other user"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove a message received from other user"		// Precisa de tradução
#endif
	,
	[ActExpSntMsg] =
#if   L==1	// ca
	"See (expand) a sent message"		// Necessita traducció
#elif L==2	// de
	"See (expand) a sent message"		// Need Übersetzung
#elif L==3	// en
	"See (expand) a sent message"
#elif L==4	// es
	"Ver (expandir) un mensaje enviado"
#elif L==5	// fr
	"See (expand) a sent message"		// Besoin de traduction
#elif L==6	// gn
	"Ver (expandir) un mensaje enviado"	// Okoteve traducción
#elif L==7	// it
	"See (expand) a sent message"		// Bisogno di traduzione
#elif L==8	// pl
	"See (expand) a sent message"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See (expand) a sent message"		// Precisa de tradução
#endif
	,
	[ActExpRcvMsg] =
#if   L==1	// ca
	"See (expand) a received message"	// Necessita traducció
#elif L==2	// de
	"See (expand) a received message"	// Need Übersetzung
#elif L==3	// en
	"See (expand) a received message"
#elif L==4	// es
	"Ver (expandir) un mensaje recibido"
#elif L==5	// fr
	"See (expand) a received message"	// Besoin de traduction
#elif L==6	// gn
	"Ver (expandir) un mensaje recibido"	// Okoteve traducción
#elif L==7	// it
	"See (expand) a received message"	// Bisogno di traduzione
#elif L==8	// pl
	"See (expand) a received message"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See (expand) a received message"	// Precisa de tradução
#endif
	,
	[ActConSntMsg] =
#if   L==1	// ca
	"Hide (contract) a sent message"	// Necessita traducció
#elif L==2	// de
	"Hide (contract) a sent message"	// Need Übersetzung
#elif L==3	// en
	"Hide (contract) a sent message"
#elif L==4	// es
	"Ocultar (contraer) un mensaje enviado"
#elif L==5	// fr
	"Hide (contract) a sent message"	// Besoin de traduction
#elif L==6	// gn
	"Ocultar (contraer) un mensaje enviado"	// Okoteve traducción
#elif L==7	// it
	"Hide (contract) a sent message"	// Bisogno di traduzione
#elif L==8	// pl
	"Hide (contract) a sent message"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Hide (contract) a sent message"	// Precisa de tradução
#endif
	,
	[ActConRcvMsg] =
#if   L==1	// ca
	"Hide (contract) a received message"		// Necessita traducció
#elif L==2	// de
	"Hide (contract) a received message"		// Need Übersetzung
#elif L==3	// en
	"Hide (contract) a received message"
#elif L==4	// es
	"Ocultar (contraer) un mensaje recibido"
#elif L==5	// fr
	"Hide (contract) a received message"		// Besoin de traduction
#elif L==6	// gn
	"Ocultar (contraer) un mensaje recibido"	// Okoteve traducción
#elif L==7	// it
	"Hide (contract) a received message"		// Bisogno di traduzione
#elif L==8	// pl
	"Hide (contract) a received message"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Hide (contract) a received message"		// Precisa de tradução
#endif
	,
	[ActLstBanUsr] =
#if   L==1	// ca
	"List banned users"		// Necessita traducció
#elif L==2	// de
	"List banned users"		// Need Übersetzung
#elif L==3	// en
	"List banned users"
#elif L==4	// es
	"Listar usuarios baneados"
#elif L==5	// fr
	"List banned users"		// Besoin de traduction
#elif L==6	// gn
	"Listar usuarios baneados"	// Okoteve traducción
#elif L==7	// it
	"List banned users"		// Bisogno di traduzione
#elif L==8	// pl
	"List banned users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List banned users"		// Precisa de tradução
#endif
	,
	[ActBanUsrMsg] =
#if   L==1	// ca
	"Ban the sender of a message when showing the received messages"	// Necessita traducció
#elif L==2	// de
	"Ban the sender of a message when showing the received messages"	// Need Übersetzung
#elif L==3	// en
	"Ban the sender of a message when showing the received messages"
#elif L==4	// es
	"Banear el emisor de un mensaje al mostrar los mensajes recibidos"
#elif L==5	// fr
	"Ban the sender of a message when showing the received messages"	// Besoin de traduction
#elif L==6	// gn
	"Banear el emisor de un mensaje al mostrar los mensajes recibidos"	// Okoteve traducción
#elif L==7	// it
	"Ban the sender of a message when showing the received messages"	// Bisogno di traduzione
#elif L==8	// pl
	"Ban the sender of a message when showing the received messages"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Ban the sender of a message when showing the received messages"	// Precisa de tradução
#endif
	,
	[ActUnbUsrMsg] =
#if   L==1	// ca
	"Unban the sender of a message when showing the received messages"	// Necessita traducció
#elif L==2	// de
	"Unban the sender of a message when showing the received messages"	// Need Übersetzung
#elif L==3	// en
	"Unban the sender of a message when showing the received messages"
#elif L==4	// es
	"Permitir el emisor de un mensaje al mostrar los mensajes recibidos"
#elif L==5	// fr
	"Unban the sender of a message when showing the received messages"	// Besoin de traduction
#elif L==6	// gn
	"Permitir el emisor de un mensaje al mostrar los mensajes recibidos"	// Okoteve traducción
#elif L==7	// it
	"Unban the sender of a message when showing the received messages"	// Bisogno di traduzione
#elif L==8	// pl
	"Unban the sender of a message when showing the received messages"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unban the sender of a message when showing the received messages"	// Precisa de tradução
#endif
	,
	[ActUnbUsrLst] =
#if   L==1	// ca
	"Unban a user when listing banned users"		// Necessita traducció
#elif L==2	// de
	"Unban a user when listing banned users"		// Need Übersetzung
#elif L==3	// en
	"Unban a user when listing banned users"
#elif L==4	// es
	"Permitir un usuario al listar los usuarios baneados"
#elif L==5	// fr
	"Unban a user when listing banned users"		// Besoin de traduction
#elif L==6	// gn
	"Permitir un usuario al listar los usuarios baneados"	// Okoteve traducción
#elif L==7	// it
	"Unban a user when listing banned users"		// Bisogno di traduzione
#elif L==8	// pl
	"Unban a user when listing banned users"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unban a user when listing banned users"		// Precisa de tradução
#endif
	,
	[ActMaiUsr] =
#if   L==1	// ca
	"See email addresses to send an email"	// Necessita traducció
#elif L==2	// de
	"See email addresses to send an email"	// Need Übersetzung
#elif L==3	// en
	"See email addresses to send an email"
#elif L==4	// es
	"Ver direcciones para enviar un correo"
#elif L==5	// fr
	"See email addresses to send an email"	// Besoin de traduction
#elif L==6	// gn
	"Ver direcciones para enviar un correo"	// Okoteve traducción
#elif L==7	// it
	"See email addresses to send an email"	// Bisogno di traduzione
#elif L==8	// pl
	"See email addresses to send an email"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See email addresses to send an email"	// Precisa de tradução
#endif
	,
	[ActReqUseGbl] =
#if   L==1	// ca
	"Request seeing use of the platform"	// Necessita traducció
#elif L==2	// de
	"Request seeing use of the platform"	// Need Übersetzung
#elif L==3	// en
	"Request seeing use of the platform"
#elif L==4	// es
	"Solicitar ver el uso de la plataforma"
#elif L==5	// fr
	"Request seeing use of the platform"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar ver el uso de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"Request seeing use of the platform"	// Bisogno di traduzione
#elif L==8	// pl
	"Request seeing use of the platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request seeing use of the platform"	// Precisa de tradução
#endif
	,
	[ActSeePhoDeg] =
#if   L==1	// ca
	"See a class photo with the average photos of the students of each degree"			// Necessita traducció
#elif L==2	// de
	"See a class photo with the average photos of the students of each degree"			// Need Übersetzung
#elif L==3	// en
	"See a class photo with the average photos of the students of each degree"
#elif L==4	// es
	"Ver una foto de clase con las fotos promedio de los estudiantes de cada titulaci&oacute;n"
#elif L==5	// fr
	"See a class photo with the average photos of the students of each degree"			// Besoin de traduction
#elif L==6	// gn
	"Ver una foto de clase con las fotos promedio de los estudiantes de cada titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See a class photo with the average photos of the students of each degree"			// Bisogno di traduzione
#elif L==8	// pl
	"See a class photo with the average photos of the students of each degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See a class photo with the average photos of the students of each degree"			// Precisa de tradução
#endif
	,
	[ActReqStaCrs] =
#if   L==1	// ca
	"Request statistics of courses"			// Necessita traducció
#elif L==2	// de
	"Request statistics of courses"			// Need Übersetzung
#elif L==3	// en
	"Request statistics of courses"
#elif L==4	// es
	"Solicitar estad&iacute;sticas de asignaturas"
#elif L==5	// fr
	"Request statistics of courses"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar estad&iacute;sticas de asignaturas"	// Okoteve traducción
#elif L==7	// it
	"Request statistics of courses"			// Bisogno di traduzione
#elif L==8	// pl
	"Request statistics of courses"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request statistics of courses"			// Precisa de tradução
#endif
	,
	[ActReqAccGbl] =
#if   L==1	// ca
	"Request query of clicks to the complete platform"	// Necessita traducció
#elif L==2	// de
	"Request query of clicks to the complete platform"	// Need Übersetzung
#elif L==3	// en
	"Request query of clicks to the complete platform"
#elif L==4	// es
	"Solicitar consulta de clics a la plataforma completa"
#elif L==5	// fr
	"Request query of clicks to the complete platform"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar consulta de clics a la plataforma completa"	// Okoteve traducción
#elif L==7	// it
	"Request query of clicks to the complete platform"	// Bisogno di traduzione
#elif L==8	// pl
	"Request query of clicks to the complete platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request query of clicks to the complete platform"	// Precisa de tradução
#endif
	,
	[ActReqMyUsgRep] =
#if   L==1	// ca
	"Request my usage report"	// Necessita traducció
#elif L==2	// de
	"Request my usage report"	// Need Übersetzung
#elif L==3	// en
	"Request my usage report"
#elif L==4	// es
	"Solicitar mi informe de uso"
#elif L==5	// fr
	"Request my usage report"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar mi informe de uso"	// Okoteve traducción
#elif L==7	// it
	"Request my usage report"	// Bisogno di traduzione
#elif L==8	// pl
	"Request my usage report"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request my usage report"	// Precisa de tradução
#endif
	,
	[ActMFUAct] =
#if   L==1	// ca
	"See most frequently used actions"		// Necessita traducció
#elif L==2	// de
	"See most frequently used actions"		// Need Übersetzung
#elif L==3	// en
	"See most frequently used actions"
#elif L==4	// es
	"Ver acciones m&aacute;s frecuentemente usadas"
#elif L==5	// fr
	"See most frequently used actions"		// Besoin de traduction
#elif L==6	// gn
	"Ver acciones m&aacute;s frecuentemente usadas"	// Okoteve traducción
#elif L==7	// it
	"See most frequently used actions"		// Bisogno di traduzione
#elif L==8	// pl
	"See most frequently used actions"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See most frequently used actions"		// Precisa de tradução
#endif
	,
	[ActSeeUseGbl] =
#if   L==1	// ca
	"See use of the platform"	// Necessita traducció
#elif L==2	// de
	"See use of the platform"	// Need Übersetzung
#elif L==3	// en
	"See use of the platform"
#elif L==4	// es
	"Ver uso de la plataforma"
#elif L==5	// fr
	"See use of the platform"	// Besoin de traduction
#elif L==6	// gn
	"Ver uso de la plataforma"	// Okoteve traducción
#elif L==7	// it
	"See use of the platform"	// Bisogno di traduzione
#elif L==8	// pl
	"See use of the platform"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See use of the platform"	// Precisa de tradução
#endif
	,
	[ActPrnPhoDeg] =
#if   L==1	// ca
	"See print view of the class photo with the average photos of the students of each degree"				// Necessita traducció
#elif L==2	// de
	"See print view of the class photo with the average photos of the students of each degree"				// Need Übersetzung
#elif L==3	// en
	"See print view of the class photo with the average photos of the students of each degree"
#elif L==4	// es
	"Ver vista de impresi&oacute;n de la foto de clase con las fotos promedio de los estudiantes de cada titulaci&oacute;n"
#elif L==5	// fr
	"See print view of the class photo with the average photos of the students of each degree"				// Besoin de traduction
#elif L==6	// gn
	"Ver vista de impresi&oacute;n de la foto de clase con las fotos promedio de los estudiantes de cada titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"See print view of the class photo with the average photos of the students of each degree"				// Bisogno di traduzione
#elif L==8	// pl
	"See print view of the class photo with the average photos of the students of each degree"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See print view of the class photo with the average photos of the students of each degree"				// Precisa de tradução
#endif
	,
	[ActCalPhoDeg] =
#if   L==1	// ca
	"Compute the average photos of the students of a degree"			// Necessita traducció
#elif L==2	// de
	"Compute the average photos of the students of a degree"			// Need Übersetzung
#elif L==3	// en
	"Compute the average photos of the students of a degree"
#elif L==4	// es
	"Calcular las fotos promedio de los estudiantes de una titulaci&oacute;n"
#elif L==5	// fr
	"Compute the average photos of the students of a degree"			// Besoin de traduction
#elif L==6	// gn
	"Calcular las fotos promedio de los estudiantes de una titulaci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Compute the average photos of the students of a degree"			// Bisogno di traduzione
#elif L==8	// pl
	"Compute the average photos of the students of a degree"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Compute the average photos of the students of a degree"			// Precisa de tradução
#endif
	,
	[ActSeeAccGbl] =
#if   L==1	// ca
	"Query clicks to the complete platform"		// Necessita traducció
#elif L==2	// de
	"Query clicks to the complete platform"		// Need Übersetzung
#elif L==3	// en
	"Query clicks to the complete platform"
#elif L==4	// es
	"Consultar clics a la plataforma completa"
#elif L==5	// fr
	"Query clicks to the complete platform"		// Besoin de traduction
#elif L==6	// gn
	"Consultar clics a la plataforma completa"	// Okoteve traducción
#elif L==7	// it
	"Query clicks to the complete platform"		// Bisogno di traduzione
#elif L==8	// pl
	"Query clicks to the complete platform"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Query clicks to the complete platform"		// Precisa de tradução
#endif
	,
	[ActReqAccCrs] =
#if   L==1	// ca
	"Request query of clicks in the current course"		// Necessita traducció
#elif L==2	// de
	"Request query of clicks in the current course"		// Need Übersetzung
#elif L==3	// en
	"Request query of clicks in the current course"
#elif L==4	// es
	"Solicitar consulta de clics en la asignatura actual"
#elif L==5	// fr
	"Request query of clicks in the current course"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar consulta de clics en la asignatura actual"	// Okoteve traducción
#elif L==7	// it
	"Request query of clicks in the current course"		// Bisogno di traduzione
#elif L==8	// pl
	"Request query of clicks in the current course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request query of clicks in the current course"		// Precisa de tradução
#endif
	,
	[ActSeeAccCrs] =
#if   L==1	// ca
	"Query clicks in the current course"		// Necessita traducció
#elif L==2	// de
	"Query clicks in the current course"		// Need Übersetzung
#elif L==3	// en
	"Query clicks in the current course"
#elif L==4	// es
	"Consultar clics en la asignatura actual"
#elif L==5	// fr
	"Query clicks in the current course"		// Besoin de traduction
#elif L==6	// gn
	"Consultar clics en la asignatura actual"	// Okoteve traducción
#elif L==7	// it
	"Query clicks in the current course"		// Bisogno di traduzione
#elif L==8	// pl
	"Query clicks in the current course"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Query clicks in the current course"		// Precisa de tradução
#endif
	,
	[ActSeeAllStaCrs] =
#if   L==1	// ca
	"See statistics of courses"			// Necessita traducció
#elif L==2	// de
	"See statistics of courses"			// Need Übersetzung
#elif L==3	// en
	"See statistics of courses"
#elif L==4	// es
	"Ver estad&iacute;sticas de asignaturas"
#elif L==5	// fr
	"See statistics of courses"			// Besoin de traduction
#elif L==6	// gn
	"Ver estad&iacute;sticas de asignaturas"	// Okoteve traducción
#elif L==7	// it
	"See statistics of courses"			// Bisogno di traduzione
#elif L==8	// pl
	"See statistics of courses"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See statistics of courses"			// Precisa de tradução
#endif
	,
	[ActLstClk] =
#if   L==1	// ca
	"List the last clicks"			// Necessita traducció
#elif L==2	// de
	"List the last clicks"			// Need Übersetzung
#elif L==3	// en
	"List the last clicks"
#elif L==4	// es
	"Listar los &uacute;ltimos clics"
#elif L==5	// fr
	"List the last clicks"			// Besoin de traduction
#elif L==6	// gn
	"Listar los &uacute;ltimos clics"	// Okoteve traducción
#elif L==7	// it
	"List the last clicks"			// Bisogno di traduzione
#elif L==8	// pl
	"List the last clicks"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"List the last clicks"			// Precisa de tradução
#endif
	,
	[ActRefLstClk] =
#if   L==1	// ca
	"Refresh the last clicks in real time"			// Necessita traducció
#elif L==2	// de
	"Refresh the last clicks in real time"			// Need Übersetzung
#elif L==3	// en
	"Refresh the last clicks in real time"
#elif L==4	// es
	"Refrescar los &ucute;ltimos clics en tiempo real"
#elif L==5	// fr
	"Refresh the last clicks in real time"			// Besoin de traduction
#elif L==6	// gn
	"Refrescar los &ucute;ltimos clics en tiempo real"	// Okoteve traducción
#elif L==7	// it
	"Refresh the last clicks in real time"			// Bisogno di traduzione
#elif L==8	// pl
	"Refresh the last clicks in real time"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Refresh the last clicks in real time"			// Precisa de tradução
#endif
	,
	[ActSeeMyUsgRep] =
#if   L==1	// ca
	"See my usage report"	// Necessita traducció
#elif L==2	// de
	"See my usage report"	// Need Übersetzung
#elif L==3	// en
	"See my usage report"
#elif L==4	// es
	"Ver mi informe de uso"
#elif L==5	// fr
	"See my usage report"	// Besoin de traduction
#elif L==6	// gn
	"Ver mi informe de uso"	// Okoteve traducción
#elif L==7	// it
	"See my usage report"	// Bisogno di traduzione
#elif L==8	// pl
	"See my usage report"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See my usage report"	// Precisa de tradução
#endif
	,
	[ActFrmRolSes] =
#if   L==1	// ca
	"See form to log out and to change the current role"			// Necessita traducció
#elif L==2	// de
	"See form to log out and to change the current role"			// Need Übersetzung
#elif L==3	// en
	"See form to log out and to change the current role"
#elif L==4	// es
	"Ver formulario para cerrar sesi&oacute;n y para cambiar el rol"
#elif L==5	// fr
	"See form to log out and to change the current role"			// Besoin de traduction
#elif L==6	// gn
	"Ver formulario para cerrar sesi&oacute;n y para cambiar el rol"	// Okoteve traducción
#elif L==7	// it
	"See form to log out and to change the current role"			// Bisogno di traduzione
#elif L==8	// pl
	"See form to log out and to change the current role"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See form to log out and to change the current role"			// Precisa de tradução
#endif
	,
	[ActMyCrs] =
#if   L==1	// ca
	"Select one of my courses"		// Necessita traducció
#elif L==2	// de
	"Select one of my courses"		// Need Übersetzung
#elif L==3	// en
	"Select one of my courses"
#elif L==4	// es
	"Seleccionar una de mis asignaturas"
#elif L==5	// fr
	"Select one of my courses"		// Besoin de traduction
#elif L==6	// gn
	"Seleccionar una de mis asignaturas"	// Okoteve traducción
#elif L==7	// it
	"Select one of my courses"		// Bisogno di traduzione
#elif L==8	// pl
	"Select one of my courses"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Select one of my courses"		// Precisa de tradução
#endif
	,
	[ActSeeMyTT] =
#if   L==1	// ca
	"See the timetable of all my courses"		// Necessita traducció
#elif L==2	// de
	"See the timetable of all my courses"		// Need Übersetzung
#elif L==3	// en
	"See the timetable of all my courses"
#elif L==4	// es
	"Ver el horario de todas mis asignaturas"
#elif L==5	// fr
	"See the timetable of all my courses"		// Besoin de traduction
#elif L==6	// gn
	"Ver el horario de todas mis asignaturas"	// Okoteve traducción
#elif L==7	// it
	"See the timetable of all my courses"		// Bisogno di traduzione
#elif L==8	// pl
	"See the timetable of all my courses"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the timetable of all my courses"		// Precisa de tradução
#endif
	,
	[ActSeeMyAgd] =
#if   L==1	// ca
	"See my full agenda (personal organizer)"	// Necessita traducció
#elif L==2	// de
	"See my full agenda (personal organizer)"	// Need Übersetzung
#elif L==3	// en
	"See my full agenda (personal organizer)"
#elif L==4	// es
	"Ver mi agenda completa (organizador personal)"
#elif L==5	// fr
	"See my full agenda (personal organizer)"	// Besoin de traduction
#elif L==6	// gn
	"Ver mi agenda completa (organizador personal)"	// Okoteve traducción
#elif L==7	// it
	"See my full agenda (personal organizer)"	// Bisogno di traduzione
#elif L==8	// pl
	"See my full agenda (personal organizer)"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See my full agenda (personal organizer)"	// Precisa de tradução
#endif
	,
	[ActFrmMyAcc] =
#if   L==1	// ca
	"See form to create or change my account"	// Necessita traducció
#elif L==2	// de
	"See form to create or change my account"	// Need Übersetzung
#elif L==3	// en
	"See form to create or change my account"
#elif L==4	// es
	"Ver formulario para crear o cambiar mi cuenta"
#elif L==5	// fr
	"See form to create or change my account"	// Besoin de traduction
#elif L==6	// gn
	"Ver formulario para crear o cambiar mi cuenta"	// Okoteve traducción
#elif L==7	// it
	"See form to create or change my account"	// Bisogno di traduzione
#elif L==8	// pl
	"See form to create or change my account"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See form to create or change my account"	// Precisa de tradução
#endif
	,
	[ActReqEdiRecSha] =
#if   L==1	// ca
	"Request the edition of the record with the personal data of the user"		// Necessita traducció
#elif L==2	// de
	"Request the edition of the record with the personal data of the user"		// Need Übersetzung
#elif L==3	// en
	"Request the edition of the record with the personal data of the user"
#elif L==4	// es
	"Solicitar la edici&oacute;n de la ficha con los datos personales del usuario"
#elif L==5	// fr
	"Request the edition of the record with the personal data of the user"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar la edici&oacute;n de la ficha con los datos personales del usuario"	// Okoteve traducción
#elif L==7	// it
	"Request the edition of the record with the personal data of the user"		// Bisogno di traduzione
#elif L==8	// pl
	"Request the edition of the record with the personal data of the user"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the edition of the record with the personal data of the user"		// Precisa de tradução
#endif
	,
	[ActReqEdiSet] =
#if   L==1	// ca
	"See forms to edit settings"		// Necessita traducció
#elif L==2	// de
	"See forms to edit settings"		// Need Übersetzung
#elif L==3	// en
	"See forms to edit settings"
#elif L==4	// es
	"Ver formularios para editar ajustes"
#elif L==5	// fr
	"See forms to edit settings"		// Besoin de traduction
#elif L==6	// gn
	"Ver formularios para editar ajustes"	// Okoteve traducción
#elif L==7	// it
	"See forms to edit settings"		// Bisogno di traduzione
#elif L==8	// pl
	"See forms to edit settings"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See forms to edit settings"		// Precisa de tradução
#endif
	,
	[ActChgMyRol] =
#if   L==1	// ca
	"Change user's role"		// Necessita traducció
#elif L==2	// de
	"Change user's role"		// Need Übersetzung
#elif L==3	// en
	"Change user's role"
#elif L==4	// es
	"Cambiar el rol de usuario"
#elif L==5	// fr
	"Change user's role"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar el rol de usuario"	// Okoteve traducción
#elif L==7	// it
	"Change user's role"		// Bisogno di traduzione
#elif L==8	// pl
	"Change user's role"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change user's role"		// Precisa de tradução
#endif
	,
	[ActFrmNewEvtMyAgd] =
#if   L==1	// ca
	"Edit a new event in my agenda"		// Necessita traducció
#elif L==2	// de
	"Edit a new event in my agenda"		// Need Übersetzung
#elif L==3	// en
	"Edit a new event in my agenda"
#elif L==4	// es
	"Editar un nuevo evento en mi agenda"
#elif L==5	// fr
	"Edit a new event in my agenda"		// Besoin de traduction
#elif L==6	// gn
	"Editar un nuevo evento en mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Edit a new event in my agenda"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit a new event in my agenda"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit a new event in my agenda"		// Precisa de tradução
#endif
	,
	[ActEdiOneEvtMyAgd] =
#if   L==1	// ca
	"Edit one event from my agenda"	// Necessita traducció
#elif L==2	// de
	"Edit one event from my agenda"	// Need Übersetzung
#elif L==3	// en
	"Edit one event from my agenda"
#elif L==4	// es
	"Editar un evento de mi agenda"
#elif L==5	// fr
	"Edit one event from my agenda"	// Besoin de traduction
#elif L==6	// gn
	"Editar un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Edit one event from my agenda"	// Bisogno di traduzione
#elif L==8	// pl
	"Edit one event from my agenda"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit one event from my agenda"	// Precisa de tradução
#endif
	,
	[ActNewEvtMyAgd] =
#if   L==1	// ca
	"Create a new event in my agenda"	// Necessita traducció
#elif L==2	// de
	"Create a new event in my agenda"	// Need Übersetzung
#elif L==3	// en
	"Create a new event in my agenda"
#elif L==4	// es
	"Crear un nuevo evento en mi agenda"
#elif L==5	// fr
	"Create a new event in my agenda"	// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo evento en mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Create a new event in my agenda"	// Bisogno di traduzione
#elif L==8	// pl
	"Create a new event in my agenda"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new event in my agenda"	// Precisa de tradução
#endif
	,
	[ActChgEvtMyAgd] =
#if   L==1	// ca
	"Modify data of an event from my agenda"	// Necessita traducció
#elif L==2	// de
	"Modify data of an event from my agenda"	// Need Übersetzung
#elif L==3	// en
	"Modify data of an event from my agenda"
#elif L==4	// es
	"Modificar los datos de un evento de mi agenda"
#elif L==5	// fr
	"Modify data of an event from my agenda"	// Besoin de traduction
#elif L==6	// gn
	"Modificar los datos de un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Modify data of an event from my agenda"	// Bisogno di traduzione
#elif L==8	// pl
	"Modify data of an event from my agenda"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Modify data of an event from my agenda"	// Precisa de tradução
#endif
	,
	[ActReqRemEvtMyAgd] =
#if   L==1	// ca
	"Request removal of an event from my agenda"			// Necessita traducció
#elif L==2	// de
	"Request removal of an event from my agenda"			// Need Übersetzung
#elif L==3	// en
	"Request removal of an event from my agenda"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de un evento de mi agenda"
#elif L==5	// fr
	"Request removal of an event from my agenda"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Request removal of an event from my agenda"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of an event from my agenda"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of an event from my agenda"			// Precisa de tradução
#endif
	,
	[ActRemEvtMyAgd] =
#if   L==1	// ca
	"Remove an event from my agenda"	// Necessita traducció
#elif L==2	// de
	"Remove an event from my agenda"	// Need Übersetzung
#elif L==3	// en
	"Remove an event from my agenda"
#elif L==4	// es
	"Eliminar un evento de mi agenda"
#elif L==5	// fr
	"Remove an event from my agenda"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Remove an event from my agenda"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove an event from my agenda"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove an event from my agenda"	// Precisa de tradução
#endif
	,
	[ActHidEvtMyAgd] =
#if   L==1	// ca
	"Hide an event from my agenda"		// Necessita traducció
#elif L==2	// de
	"Hide an event from my agenda"		// Need Übersetzung
#elif L==3	// en
	"Hide an event from my agenda"
#elif L==4	// es
	"Ocultar un evento de mi agenda"
#elif L==5	// fr
	"Hide an event from my agenda"		// Besoin de traduction
#elif L==6	// gn
	"Ocultar un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Hide an event from my agenda"		// Bisogno di traduzione
#elif L==8	// pl
	"Hide an event from my agenda"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Hide an event from my agenda"		// Precisa de tradução
#endif
	,
	[ActShoEvtMyAgd] =
#if   L==1	// ca
	"Unhide an event from my agenda"	// Necessita traducció
#elif L==2	// de
	"Unhide an event from my agenda"	// Need Übersetzung
#elif L==3	// en
	"Unhide an event from my agenda"
#elif L==4	// es
	"Mostrar un evento de mi agenda"
#elif L==5	// fr
	"Unhide an event from my agenda"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Unhide an event from my agenda"	// Bisogno di traduzione
#elif L==8	// pl
	"Unhide an event from my agenda"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unhide an event from my agenda"	// Precisa de tradução
#endif
	,
	[ActPrvEvtMyAgd] =
#if   L==1	// ca
	"Make private an event from my agenda"	// Necessita traducció
#elif L==2	// de
	"Make private an event from my agenda"	// Need Übersetzung
#elif L==3	// en
	"Make private an event from my agenda"
#elif L==4	// es
	"Hacer privado un evento de mi agenda"
#elif L==5	// fr
	"Make private an event from my agenda"	// Besoin de traduction
#elif L==6	// gn
	"Hacer privado un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Make private an event from my agenda"	// Bisogno di traduzione
#elif L==8	// pl
	"Make private an event from my agenda"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Make private an event from my agenda"	// Precisa de tradução
#endif
	,
	[ActPubEvtMyAgd] =
#if   L==1	// ca
	"Make public an event from my agenda"		// Necessita traducció
#elif L==2	// de
	"Make public an event from my agenda"		// Need Übersetzung
#elif L==3	// en
	"Make public an event from my agenda"
#elif L==4	// es
	"Hacer p&uacute;blico un evento de mi agenda"
#elif L==5	// fr
	"Make public an event from my agenda"		// Besoin de traduction
#elif L==6	// gn
	"Hacer p&uacute;blico un evento de mi agenda"	// Okoteve traducción
#elif L==7	// it
	"Make public an event from my agenda"		// Bisogno di traduzione
#elif L==8	// pl
	"Make public an event from my agenda"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Make public an event from my agenda"		// Precisa de tradução
#endif
	,
	[ActPrnAgdQR] =
#if   L==1	// ca
	"See agenda QR code ready to print"			// Necessita traducció
#elif L==2	// de
	"See agenda QR code ready to print"			// Need Übersetzung
#elif L==3	// en
	"See agenda QR code ready to print"
#elif L==4	// es
	"Ver c&oacute;digo QR de agenda listo para imprimir"
#elif L==5	// fr
	"See agenda QR code ready to print"			// Besoin de traduction
#elif L==6	// gn
	"Ver c&oacute;digo QR de agenda listo para imprimir"	// Okoteve traducción
#elif L==7	// it
	"See agenda QR code ready to print"			// Bisogno di traduzione
#elif L==8	// pl
	"See agenda QR code ready to print"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See agenda QR code ready to print"			// Precisa de tradução
#endif
	,
	[ActChkUsrAcc] =
#if   L==1	// ca
	"Check if already exists a new account without password associated to a ID"		// Necessita traducció
#elif L==2	// de
	"Check if already exists a new account without password associated to a ID"		// Need Übersetzung
#elif L==3	// en
	"Check if already exists a new account without password associated to a ID"
#elif L==4	// es
	"Comprobar si ya existe una nueva cuenta sin contrase&ntilde;a asociada a un ID"
#elif L==5	// fr
	"Check if already exists a new account without password associated to a ID"		// Besoin de traduction
#elif L==6	// gn
	"Comprobar si ya existe una nueva cuenta sin contrase&ntilde;a asociada a un ID"	// Okoteve traducción
#elif L==7	// it
	"Check if already exists a new account without password associated to a ID"		// Bisogno di traduzione
#elif L==8	// pl
	"Check if already exists a new account without password associated to a ID"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Check if already exists a new account without password associated to a ID"		// Precisa de tradução
#endif
	,
	[ActCreMyAcc] =
#if   L==1	// ca
	"Edit a new user account"	// Necessita traducció
#elif L==2	// de
	"Edit a new user account"	// Need Übersetzung
#elif L==3	// en
	"Edit a new user account"
#elif L==4	// es
	"Editar una nueva cuenta"
#elif L==5	// fr
	"Edit a new user account"	// Besoin de traduction
#elif L==6	// gn
	"Editar una nueva cuenta"	// Okoteve traducción
#elif L==7	// it
	"Edit a new user account"	// Bisogno di traduzione
#elif L==8	// pl
	"Edit a new user account"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit a new user account"	// Precisa de tradução
#endif
	,
	[ActCreUsrAcc] =
#if   L==1	// ca
	"Create new user account"	// Necessita traducció
#elif L==2	// de
	"Create new user account"	// Need Übersetzung
#elif L==3	// en
	"Create new user account"
#elif L==4	// es
	"Crear una nueva cuenta"
#elif L==5	// fr
	"Create new user account"	// Besoin de traduction
#elif L==6	// gn
	"Crear una nueva cuenta"	// Okoteve traducción
#elif L==7	// it
	"Create new user account"	// Bisogno di traduzione
#elif L==8	// pl
	"Create new user account"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create new user account"	// Precisa de tradução
#endif
	,
	[ActRemMyID] =
#if   L==1	// ca
	"Remove one of my user's IDs"		// Necessita traducció
#elif L==2	// de
	"Remove one of my user's IDs"		// Need Übersetzung
#elif L==3	// en
	"Remove one of my user's IDs"
#elif L==4	// es
	"Eliminar uno de mis ID de usuario"
#elif L==5	// fr
	"Remove one of my user's IDs"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar uno de mis ID de usuario"	// Okoteve traducción
#elif L==7	// it
	"Remove one of my user's IDs"		// Bisogno di traduzione
#elif L==8	// pl
	"Remove one of my user's IDs"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove one of my user's IDs"		// Precisa de tradução
#endif
	,
	[ActChgMyID] =
#if   L==1	// ca
	"Create a new user's ID for me"			// Necessita traducció
#elif L==2	// de
	"Create a new user's ID for me"			// Need Übersetzung
#elif L==3	// en
	"Create a new user's ID for me"
#elif L==4	// es
	"Crear un nuevo ID de usuario para m&iacute;"
#elif L==5	// fr
	"Create a new user's ID for me"			// Besoin de traduction
#elif L==6	// gn
	"Crear un nuevo ID de usuario para m&iacute;"	// Okoteve traducción
#elif L==7	// it
	"Create a new user's ID for me"			// Bisogno di traduzione
#elif L==8	// pl
	"Create a new user's ID for me"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Create a new user's ID for me"			// Precisa de tradução
#endif
	,
	[ActRemMyNck] =
#if   L==1	// ca
	"Remove one of my old nicknames"	// Necessita traducció
#elif L==2	// de
	"Remove one of my old nicknames"	// Need Übersetzung
#elif L==3	// en
	"Remove one of my old nicknames"
#elif L==4	// es
	"Eliminar uno de mis antiguos apodos"
#elif L==5	// fr
	"Remove one of my old nicknames"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar uno de mis antiguos apodos"	// Okoteve traducción
#elif L==7	// it
	"Remove one of my old nicknames"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove one of my old nicknames"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove one of my old nicknames"	// Precisa de tradução
#endif
	,
	[ActChgMyNck] =
#if   L==1	// ca
	"Change my nickname"	// Necessita traducció
#elif L==2	// de
	"Change my nickname"	// Need Übersetzung
#elif L==3	// en
	"Change my nickname"
#elif L==4	// es
	"Cambiar mi apodo"
#elif L==5	// fr
	"Change my nickname"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi apodo"	// Okoteve traducción
#elif L==7	// it
	"Change my nickname"	// Bisogno di traduzione
#elif L==8	// pl
	"Change my nickname"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my nickname"	// Precisa de tradução
#endif
	,
	[ActRemMyMai] =
#if   L==1	// ca
	"Remove one of my old emails"				// Necessita traducció
#elif L==2	// de
	"Remove one of my old emails"				// Need Übersetzung
#elif L==3	// en
	"Remove one of my old emails"
#elif L==4	// es
	"Eliminar una de mis antiguas direcciones de correo"
#elif L==5	// fr
	"Remove one of my old emails"				// Besoin de traduction
#elif L==6	// gn
	"Eliminar una de mis antiguas direcciones de correo"	// Okoteve traducción
#elif L==7	// it
	"Remove one of my old emails"				// Bisogno di traduzione
#elif L==8	// pl
	"Remove one of my old emails"				// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove one of my old emails"				// Precisa de tradução
#endif
	,
	[ActChgMyMai] =
#if   L==1	// ca
	"Change my email address"		// Necessita traducció
#elif L==2	// de
	"Change my email address"		// Need Übersetzung
#elif L==3	// en
	"Change my email address"
#elif L==4	// es
	"Cambiar mi direcci&oacute;n de correo"
#elif L==5	// fr
	"Change my email address"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi direcci&oacute;n de correo"	// Okoteve traducción
#elif L==7	// it
	"Change my email address"		// Bisogno di traduzione
#elif L==8	// pl
	"Change my email address"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my email address"		// Precisa de tradução
#endif
	,
	[ActCnfMai] =
#if   L==1	// ca
	"Confirm email address"			// Necessita traducció
#elif L==2	// de
	"Confirm email address"			// Need Übersetzung
#elif L==3	// en
	"Confirm email address"
#elif L==4	// es
	"Confirmar direcci&oacute;n de correo"
#elif L==5	// fr
	"Confirm email address"			// Besoin de traduction
#elif L==6	// gn
	"Confirmar direcci&oacute;n de correo"	// Okoteve traducción
#elif L==7	// it
	"Confirm email address"			// Bisogno di traduzione
#elif L==8	// pl
	"Confirm email address"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Confirm email address"			// Precisa de tradução
#endif
	,
	[ActChgMyPwd] =
#if   L==1	// ca
	"Change my password"		// Necessita traducció
#elif L==2	// de
	"Change my password"		// Need Übersetzung
#elif L==3	// en
	"Change my password"
#elif L==4	// es
	"Cambiar mi contrase&ntilde;a"
#elif L==5	// fr
	"Change my password"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi contrase&ntilde;a"	// Okoteve traducción
#elif L==7	// it
	"Change my password"		// Bisogno di traduzione
#elif L==8	// pl
	"Change my password"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my password"		// Precisa de tradução
#endif
	,
	[ActReqRemMyAcc] =
#if   L==1	// ca
	"Request removal of my account"			// Necessita traducció
#elif L==2	// de
	"Request removal of my account"			// Need Übersetzung
#elif L==3	// en
	"Request removal of my account"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de mi cuenta"
#elif L==5	// fr
	"Request removal of my account"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de mi cuenta"	// Okoteve traducción
#elif L==7	// it
	"Request removal of my account"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of my account"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of my account"			// Precisa de tradução
#endif
	,
	[ActRemMyAcc] =
#if   L==1	// ca
	"Remove my account"	// Necessita traducció
#elif L==2	// de
	"Remove my account"	// Need Übersetzung
#elif L==3	// en
	"Remove my account"
#elif L==4	// es
	"Eliminar mi cuenta"
#elif L==5	// fr
	"Remove my account"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar mi cuenta"	// Okoteve traducción
#elif L==7	// it
	"Remove my account"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove my account"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove my account"	// Precisa de tradução
#endif
	,
	[ActChgMyData] =
#if   L==1	// ca
	"Update my personal data"		// Necessita traducció
#elif L==2	// de
	"Update my personal data"		// Need Übersetzung
#elif L==3	// en
	"Update my personal data"
#elif L==4	// es
	"Actualizar mis datos personales"
#elif L==5	// fr
	"Update my personal data"		// Besoin de traduction
#elif L==6	// gn
	"Actualizar mis datos personales"	// Okoteve traducción
#elif L==7	// it
	"Update my personal data"		// Bisogno di traduzione
#elif L==8	// pl
	"Update my personal data"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update my personal data"		// Precisa de tradução
#endif
	,
	[ActReqMyPho] =
#if   L==1	// ca
	"Request sending of my photo"		// Necessita traducció
#elif L==2	// de
	"Request sending of my photo"		// Need Übersetzung
#elif L==3	// en
	"Request sending of my photo"
#elif L==4	// es
	"Solicitar el env&iacute;o de mi foto"
#elif L==5	// fr
	"Request sending of my photo"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar el env&iacute;o de mi foto"	// Okoteve traducción
#elif L==7	// it
	"Request sending of my photo"		// Bisogno di traduzione
#elif L==8	// pl
	"Request sending of my photo"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request sending of my photo"		// Precisa de tradução
#endif
	,
	[ActDetMyPho] =
#if   L==1	// ca
	"Send my photo and detect faces on it"		// Necessita traducció
#elif L==2	// de
	"Send my photo and detect faces on it"		// Need Übersetzung
#elif L==3	// en
	"Send my photo and detect faces on it"
#elif L==4	// es
	"Enviar mi foto y detectar rostros en ella"
#elif L==5	// fr
	"Send my photo and detect faces on it"		// Besoin de traduction
#elif L==6	// gn
	"Enviar mi foto y detectar rostros en ella"	// Okoteve traducción
#elif L==7	// it
	"Send my photo and detect faces on it"		// Bisogno di traduzione
#elif L==8	// pl
	"Send my photo and detect faces on it"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Send my photo and detect faces on it"		// Precisa de tradução
#endif
	,
	[ActUpdMyPho] =
#if   L==1	// ca
	"Update my photo"	// Necessita traducció
#elif L==2	// de
	"Update my photo"	// Need Übersetzung
#elif L==3	// en
	"Update my photo"
#elif L==4	// es
	"Actualizar mi foto"
#elif L==5	// fr
	"Update my photo"	// Besoin de traduction
#elif L==6	// gn
	"Actualizar mi foto"	// Okoteve traducción
#elif L==7	// it
	"Update my photo"	// Bisogno di traduzione
#elif L==8	// pl
	"Update my photo"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Update my photo"	// Precisa de tradução
#endif
	,
	[ActReqRemMyPho] =
#if   L==1	// ca
	"Request removal of my photo"			// Necessita traducció
#elif L==2	// de
	"Request removal of my photo"			// Need Übersetzung
#elif L==3	// en
	"Request removal of my photo"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de mi foto"
#elif L==5	// fr
	"Request removal of my photo"			// Besoin de traduction
#elif L==6	// gn
	"Solicitar la eliminaci&oacute;n de mi foto"	// Okoteve traducción
#elif L==7	// it
	"Request removal of my photo"			// Bisogno di traduzione
#elif L==8	// pl
	"Request removal of my photo"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request removal of my photo"			// Precisa de tradução
#endif
	,
	[ActRemMyPho] =
#if   L==1	// ca
	"Remove my photo"	// Necessita traducció
#elif L==2	// de
	"Remove my photo"	// Need Übersetzung
#elif L==3	// en
	"Remove my photo"
#elif L==4	// es
	"Eliminar mi foto"
#elif L==5	// fr
	"Remove my photo"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar mi foto"	// Okoteve traducción
#elif L==7	// it
	"Remove my photo"	// Bisogno di traduzione
#elif L==8	// pl
	"Remove my photo"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Remove my photo"	// Precisa de tradução
#endif
	,
	[ActChgCtyMyIns] =
#if   L==1	// ca
	"Change the country of my institution"			// Necessita traducció
#elif L==2	// de
	"Change the country of my institution"			// Need Übersetzung
#elif L==3	// en
	"Change the country of my institution"
#elif L==4	// es
	"Cambiar el pa&iacute;s de mi instituci&oacute;n"
#elif L==5	// fr
	"Change the country of my institution"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el pa&iacute;s de mi instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Change the country of my institution"			// Bisogno di traduzione
#elif L==8	// pl
	"Change the country of my institution"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the country of my institution"			// Precisa de tradução
#endif
	,
	[ActChgMyIns] =
#if   L==1	// ca
	"Change my institution"		// Necessita traducció
#elif L==2	// de
	"Change my institution"		// Need Übersetzung
#elif L==3	// en
	"Change my institution"
#elif L==4	// es
	"Cambiar mi instituci&oacute;n"
#elif L==5	// fr
	"Change my institution"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi instituci&oacute;n"	// Okoteve traducción
#elif L==7	// it
	"Change my institution"		// Bisogno di traduzione
#elif L==8	// pl
	"Change my institution"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my institution"		// Precisa de tradução
#endif
	,
	[ActChgMyCtr] =
#if   L==1	// ca
	"Change my center"	// Necessita traducció
#elif L==2	// de
	"Change my center"	// Need Übersetzung
#elif L==3	// en
	"Change my center"
#elif L==4	// es
	"Cambiar mi centro"
#elif L==5	// fr
	"Change my center"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi centro"	// Okoteve traducción
#elif L==7	// it
	"Change my center"	// Bisogno di traduzione
#elif L==8	// pl
	"Change my center"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my center"	// Precisa de tradução
#endif
	,
	[ActChgMyDpt] =
#if   L==1	// ca
	"Change my department"		// Necessita traducció
#elif L==2	// de
	"Change my department"		// Need Übersetzung
#elif L==3	// en
	"Change my department"
#elif L==4	// es
	"Cambiar mi departamento"
#elif L==5	// fr
	"Change my department"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi departamento"	// Okoteve traducción
#elif L==7	// it
	"Change my department"		// Bisogno di traduzione
#elif L==8	// pl
	"Change my department"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my department"		// Precisa de tradução
#endif
	,
	[ActChgMyOff] =
#if   L==1	// ca
	"Change my office"	// Necessita traducció
#elif L==2	// de
	"Change my office"	// Need Übersetzung
#elif L==3	// en
	"Change my office"
#elif L==4	// es
	"Cambiar mi despacho"
#elif L==5	// fr
	"Change my office"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi despacho"	// Okoteve traducción
#elif L==7	// it
	"Change my office"	// Bisogno di traduzione
#elif L==8	// pl
	"Change my office"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my office"	// Precisa de tradução
#endif
	,
	[ActChgMyOffPho] =
#if   L==1	// ca
	"Change my office phone"			// Necessita traducció
#elif L==2	// de
	"Change my office phone"			// Need Übersetzung
#elif L==3	// en
	"Change my office phone"
#elif L==4	// es
	"Cambiar el tel&eacute;fono de mi despacho"
#elif L==5	// fr
	"Change my office phone"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar el tel&eacute;fono de mi despacho"	// Okoteve traducción
#elif L==7	// it
	"Change my office phone"			// Bisogno di traduzione
#elif L==8	// pl
	"Change my office phone"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my office phone"			// Precisa de tradução
#endif
	,
	[ActReqEdiMyNet] =
#if   L==1	// ca
	"Request the edition of my social networks"		// Necessita traducció
#elif L==2	// de
	"Request the edition of my social networks"		// Need Übersetzung
#elif L==3	// en
	"Request the edition of my social networks"
#elif L==4	// es
	"Solicitar la edici&oacute;n de mis redes sociales"
#elif L==5	// fr
	"Request the edition of my social networks"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar la edici&oacute;n de mis redes sociales"	// Okoteve traducción
#elif L==7	// it
	"Request the edition of my social networks"		// Bisogno di traduzione
#elif L==8	// pl
	"Request the edition of my social networks"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request the edition of my social networks"		// Precisa de tradução
#endif
	,
	[ActChgMyNet] =
#if   L==1	// ca
	"Change my web and social networks"	// Necessita traducció
#elif L==2	// de
	"Change my web and social networks"	// Need Übersetzung
#elif L==3	// en
	"Change my web and social networks"
#elif L==4	// es
	"Cambiar mi web y mis redes sociales"
#elif L==5	// fr
	"Change my web and social networks"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar mi web y mis redes sociales"	// Okoteve traducción
#elif L==7	// it
	"Change my web and social networks"	// Bisogno di traduzione
#elif L==8	// pl
	"Change my web and social networks"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change my web and social networks"	// Precisa de tradução
#endif
	,
	[ActChgThe] =
#if   L==1	// ca
	"Change theme"		// Necessita traducció
#elif L==2	// de
	"Change theme"		// Need Übersetzung
#elif L==3	// en
	"Change theme"
#elif L==4	// es
	"Cambiar tema"
#elif L==5	// fr
	"Change theme"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar tema"		// Okoteve traducción
#elif L==7	// it
	"Change theme"		// Bisogno di traduzione
#elif L==8	// pl
	"Change theme"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change theme"		// Precisa de tradução
#endif
	,
	[ActReqChgLan] =
#if   L==1	// ca
	"Request change of language"	// Necessita traducció
#elif L==2	// de
	"Request change of language"	// Need Übersetzung
#elif L==3	// en
	"Request change of language"
#elif L==4	// es
	"Solicitar cambio de idioma"
#elif L==5	// fr
	"Request change of language"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar cambio de idioma"	// Okoteve traducción
#elif L==7	// it
	"Request change of language"	// Bisogno di traduzione
#elif L==8	// pl
	"Request change of language"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request change of language"	// Precisa de tradução
#endif
	,
	[ActChgLan] =
#if   L==1	// ca
	"Change language"	// Necessita traducció
#elif L==2	// de
	"Change language"	// Need Übersetzung
#elif L==3	// en
	"Change language"
#elif L==4	// es
	"Cambiar idioma"
#elif L==5	// fr
	"Change language"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar idioma"	// Okoteve traducción
#elif L==7	// it
	"Change language"	// Bisogno di traduzione
#elif L==8	// pl
	"Change language"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change language"	// Precisa de tradução
#endif
	,
	[ActChg1stDay] =
#if   L==1	// ca
	"Change first day of the week"			// Necessita traducció
#elif L==2	// de
	"Change first day of the week"			// Need Übersetzung
#elif L==3	// en
	"Change first day of the week"
#elif L==4	// es
	"Cambiar primer d&iacute;a de la semana"
#elif L==5	// fr
	"Change first day of the week"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar primer d&iacute;a de la semana"	// Okoteve traducción
#elif L==7	// it
	"Change first day of the week"			// Bisogno di traduzione
#elif L==8	// pl
	"Change first day of the week"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change first day of the week"			// Precisa de tradução
#endif
	,
	[ActChgDatFmt] =
#if   L==1	// ca
	"Change date format"		// Necessita traducció
#elif L==2	// de
	"Change date format"		// Need Übersetzung
#elif L==3	// en
	"Change date format"
#elif L==4	// es
	"Cambiar formato de fechas"
#elif L==5	// fr
	"Change date format"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar formato de fechas"	// Okoteve traducción
#elif L==7	// it
	"Change date format"		// Bisogno di traduzione
#elif L==8	// pl
	"Change date format"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change date format"		// Precisa de tradução
#endif
	,
	[ActChgCol] =
#if   L==1	// ca
	"Change side columns"		// Necessita traducció
#elif L==2	// de
	"Change side columns"		// Need Übersetzung
#elif L==3	// en
	"Change side columns"
#elif L==4	// es
	"Cambiar columnas laterales"
#elif L==5	// fr
	"Change side columns"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar columnas laterales"	// Okoteve traducción
#elif L==7	// it
	"Change side columns"		// Bisogno di traduzione
#elif L==8	// pl
	"Change side columns"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change side columns"		// Precisa de tradução
#endif
	,
	[ActChgIco] =
#if   L==1	// ca
	"Change icon set"		// Necessita traducció
#elif L==2	// de
	"Change icon set"		// Need Übersetzung
#elif L==3	// en
	"Change icon set"
#elif L==4	// es
	"Cambiar conjunto de iconos"
#elif L==5	// fr
	"Change icon set"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar conjunto de iconos"	// Okoteve traducción
#elif L==7	// it
	"Change icon set"		// Bisogno di traduzione
#elif L==8	// pl
	"Change icon set"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change icon set"		// Precisa de tradução
#endif
	,
	[ActChgMnu] =
#if   L==1	// ca
	"Change menu"		// Necessita traducció
#elif L==2	// de
	"Change menu"		// Need Übersetzung
#elif L==3	// en
	"Change menu"
#elif L==4	// es
	"Cambiar men&uacute;"
#elif L==5	// fr
	"Change menu"		// Besoin de traduction
#elif L==6	// gn
	"Cambiar men&uacute;"	// Okoteve traducción
#elif L==7	// it
	"Change menu"		// Bisogno di traduzione
#elif L==8	// pl
	"Change menu"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change menu"		// Precisa de tradução
#endif
	,
	[ActChgPriPho] =
#if   L==1	// ca
	"Change the privacy of my photo"	// Necessita traducció
#elif L==2	// de
	"Change the privacy of my photo"	// Need Übersetzung
#elif L==3	// en
	"Change the privacy of my photo"
#elif L==4	// es
	"Cambiar la privacidad de mi foto"
#elif L==5	// fr
	"Change the privacy of my photo"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar la privacidad de mi foto"	// Okoteve traducción
#elif L==7	// it
	"Change the privacy of my photo"	// Bisogno di traduzione
#elif L==8	// pl
	"Change the privacy of my photo"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the privacy of my photo"	// Precisa de tradução
#endif
	,
	[ActChgBasPriPrf] =
#if   L==1	// ca
	"Change the privacy of my basic public profile"			// Necessita traducció
#elif L==2	// de
	"Change the privacy of my basic public profile"			// Need Übersetzung
#elif L==3	// en
	"Change the privacy of my basic public profile"
#elif L==4	// es
	"Cambiar la privacidad de mi perfil de usuario b&aacute;sico"
#elif L==5	// fr
	"Change the privacy of my basic public profile"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la privacidad de mi perfil de usuario b&aacute;sico"	// Okoteve traducción
#elif L==7	// it
	"Change the privacy of my basic public profile"			// Bisogno di traduzione
#elif L==8	// pl
	"Change the privacy of my basic public profile"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change the privacy of my basic public profile"			// Precisa de tradução
#endif
	,
	[ActChgExtPriPrf] =
#if   L==1	// ca
	"Change privacy of my extended public profile"			// Necessita traducció
#elif L==2	// de
	"Change privacy of my extended public profile"			// Need Übersetzung
#elif L==3	// en
	"Change privacy of my extended public profile"
#elif L==4	// es
	"Cambiar la privacidad de mi perfil de usuario extendido"
#elif L==5	// fr
	"Change privacy of my extended public profile"			// Besoin de traduction
#elif L==6	// gn
	"Cambiar la privacidad de mi perfil de usuario extendido"	// Okoteve traducción
#elif L==7	// it
	"Change privacy of my extended public profile"			// Bisogno di traduzione
#elif L==8	// pl
	"Change privacy of my extended public profile"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change privacy of my extended public profile"			// Precisa de tradução
#endif
	,
	[ActChgCooPrf] =
#if   L==1	// ca
	"Change setting about third party cookies"	// Necessita traducció
#elif L==2	// de
	"Change setting about third party cookies"	// Need Übersetzung
#elif L==3	// en
	"Change setting about third party cookies"
#elif L==4	// es
	"Cambiar el ajuste sobre cookies de terceros"
#elif L==5	// fr
	"Change setting about third party cookies"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar el ajuste sobre cookies de terceros"	// Okoteve traducción
#elif L==7	// it
	"Change setting about third party cookies"	// Bisogno di traduzione
#elif L==8	// pl
	"Change setting about third party cookies"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change setting about third party cookies"	// Precisa de tradução
#endif
	,
	[ActChgNtfPrf] =
#if   L==1	// ca
	"Change whether to notify by email"	// Necessita traducció
#elif L==2	// de
	"Change whether to notify by email"	// Need Übersetzung
#elif L==3	// en
	"Change whether to notify by email"
#elif L==4	// es
	"Cambiar si notificar por correo"
#elif L==5	// fr
	"Change whether to notify by email"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar si notificar por correo"	// Okoteve traducción
#elif L==7	// it
	"Change whether to notify by email"	// Bisogno di traduzione
#elif L==8	// pl
	"Change whether to notify by email"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change whether to notify by email"	// Precisa de tradução
#endif
	,
	[ActPrnUsrQR] =
#if   L==1	// ca
	"See my QR code ready to print"			// Necessita traducció
#elif L==2	// de
	"See my QR code ready to print"			// Need Übersetzung
#elif L==3	// en
	"See my QR code ready to print"
#elif L==4	// es
	"Ver mi c&oacute;digo QR listo para imprimir"
#elif L==5	// fr
	"See my QR code ready to print"			// Besoin de traduction
#elif L==6	// gn
	"Ver mi c&oacute;digo QR listo para imprimir"	// Okoteve traducción
#elif L==7	// it
	"See my QR code ready to print"			// Bisogno di traduzione
#elif L==8	// pl
	"See my QR code ready to print"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See my QR code ready to print"			// Precisa de tradução
#endif
	,
	[ActPrnMyTT] =
#if   L==1	// ca
	"See the print view of the timetable of all my courses"			// Necessita traducció
#elif L==2	// de
	"See the print view of the timetable of all my courses"			// Need Übersetzung
#elif L==3	// en
	"See the print view of the timetable of all my courses"
#elif L==4	// es
	"Ver la vista de impresi&oacute;n del horario de todas mis asignaturas"
#elif L==5	// fr
	"See the print view of the timetable of all my courses"			// Besoin de traduction
#elif L==6	// gn
	"Ver la vista de impresi&oacute;n del horario de todas mis asignaturas"	// Okoteve traducción
#elif L==7	// it
	"See the print view of the timetable of all my courses"			// Bisogno di traduzione
#elif L==8	// pl
	"See the print view of the timetable of all my courses"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"See the print view of the timetable of all my courses"			// Precisa de tradução
#endif
	,
	[ActEdiTut] =
#if   L==1	// ca
	"Edit my office timetable"		// Necessita traducció
#elif L==2	// de
	"Edit my office timetable"		// Need Übersetzung
#elif L==3	// en
	"Edit my office timetable"
#elif L==4	// es
	"Editar mi horario de tutor&iacute;as"
#elif L==5	// fr
	"Edit my office timetable"		// Besoin de traduction
#elif L==6	// gn
	"Editar mi horario de tutor&iacute;as"	// Okoteve traducción
#elif L==7	// it
	"Edit my office timetable"		// Bisogno di traduzione
#elif L==8	// pl
	"Edit my office timetable"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Edit my office timetable"		// Precisa de tradução
#endif
	,
	[ActChgTut] =
#if   L==1	// ca
	"Modify my office timetable"			// Necessita traducció
#elif L==2	// de
	"Modify my office timetable"			// Need Übersetzung
#elif L==3	// en
	"Modify my office timetable"
#elif L==4	// es
	"Modificar mi horario de tutor&iacute;as"
#elif L==5	// fr
	"Modify my office timetable"			// Besoin de traduction
#elif L==6	// gn
	"Modificar mi horario de tutor&iacute;as"	// Okoteve traducción
#elif L==7	// it
	"Modify my office timetable"			// Bisogno di traduzione
#elif L==8	// pl
	"Modify my office timetable"			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Modify my office timetable"			// Precisa de tradução
#endif
	,
	[ActChgMyTT1stDay] =
#if   L==1	// ca
	"Change first day of week and show office timetable"					// Necessita traducció
#elif L==2	// de
	"Change first day of week and show office timetable"					// Need Übersetzung
#elif L==3	// en
	"Change first day of week and show office timetable"
#elif L==4	// es
	"Cambiar el primer d&iacute;a de la semana y mostrar el horario de tutor&iacute;as"
#elif L==5	// fr
	"Change first day of week and show office timetable"					// Besoin de traduction
#elif L==6	// gn
	"Cambiar el primer d&iacute;a de la semana y mostrar el horario de tutor&iacute;as"	// Okoteve traducción
#elif L==7	// it
	"Change first day of week and show office timetable"					// Bisogno di traduzione
#elif L==8	// pl
	"Change first day of week and show office timetable"					// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change first day of week and show office timetable"					// Precisa de tradução
#endif
	};
