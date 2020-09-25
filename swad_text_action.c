// swad_text_action.c: texts of actions, in several languages

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
	[ActSeeSocTmlGbl] =
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
	"Request to send a new password"		// Necessita traducció
#elif L==2	// de
	"Request to send a new password"		// Need Übersetzung
#elif L==3	// en
	"Request to send a new password"
#elif L==4	// es
	"Solicitar env&iacute;o de nueva contrase&ntilde;a"
#elif L==5	// fr
	"Request to send a new password"		// Besoin de traduction
#elif L==6	// gn
	"Solicitar env&iacute;o de nueva contrase&ntilde;a"	// Okoteve traducción
#elif L==7	// it
	"Request to send a new password"		// Bisogno di traduzione
#elif L==8	// pl
	"Request to send a new password"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request to send a new password"		// Precisa de tradução
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
	[ActRefNewSocPubGbl] =
#if   L==1	// ca
	"Refresh global timeline (new publications)"	// Necessita traducció
#elif L==2	// de
	"Refresh global timeline (new publications)"	// Need Übersetzung
#elif L==3	// en
	"Refresh global timeline (new publications)"
#elif L==4	// es
	"Refrescar timeline global (nuevas publicaciones)"
#elif L==5	// fr
	"Refresh global timeline (new publications)"	// Besoin de traduction
#elif L==6	// gn
	"Refrescar timeline global (nuevas publicaciones)"	// Okoteve traducción
#elif L==7	// it
	"Refresh global timeline (new publications)"	// Bisogno di traduzione
#elif L==8	// pl
	"Refresh global timeline (new publications)"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Refresh global timeline (new publications)"	// Precisa de tradução
#endif
	,
	[ActRefOldSocPubGbl] =
#if   L==1	// ca
	"Show previous publications in global timeline"	// Necessita traducció
#elif L==2	// de
	"Show previous publications in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Show previous publications in global timeline"
#elif L==4	// es
	"Mostrar publicaciones anteriores en timeline global"
#elif L==5	// fr
	"Show previous publications in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar publicaciones anteriores en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show previous publications in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show previous publications in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show previous publications in global timeline"	// Precisa de tradução
#endif
	,
	[ActRcvSocPstGbl] =
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
	[ActRcvSocComGbl] =
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
	[ActShoHidSocComGbl] =
#if   L==1	// ca
	"Show hidden comments in global timeline"	// Necessita traducció
#elif L==2	// de
	"Show hidden comments in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Show hidden comments in global timeline"
#elif L==4	// es
	"Mostrar comentarios ocultos en timeline global"
#elif L==5	// fr
	"Show hidden comments in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar comentarios ocultos en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show hidden comments in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show hidden comments in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show hidden comments in global timeline"	// Precisa de tradução
#endif
	,
	[ActAllShaSocNotGbl] =
#if   L==1	// ca
	"Show who shared a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Show who shared a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Show who shared a publication in global timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Show who shared a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show who shared a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show who shared a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who shared a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActAllFavSocNotGbl] =
#if   L==1	// ca
	"Show who favourited a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Show who favourited a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Show who favourited a publication in global timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Show who favourited a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActAllFavSocComGbl] =
#if   L==1	// ca
	"Show who favourited a comment in global timeline"	// Necessita traducció
#elif L==2	// de
	"Show who favourited a comment in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Show who favourited a comment in global timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline global"
#elif L==5	// fr
	"Show who favourited a comment in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a comment in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a comment in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a comment in global timeline"	// Precisa de tradução
#endif
	,
	[ActShaSocNotGbl] =
#if   L==1	// ca
	"Share a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Share a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Share a publication in global timeline"
#elif L==4	// es
	"Compartir una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Share a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Compartir una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Share a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Share a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Share a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActUnsSocNotGbl] =
#if   L==1	// ca
	"Unshare a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Unshare a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Unshare a publication in global timeline"
#elif L==4	// es
	"Dejar de compartir una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Unshare a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Dejar de compartir una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Unshare a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Unshare a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unshare a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActFavSocNotGbl] =
#if   L==1	// ca
	"Favourite a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Favourite a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Favourite a publication in global timeline"
#elif L==4	// es
	"Marcar como favorita una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Favourite a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorita una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Favourite a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActUnfSocNotGbl] =
#if   L==1	// ca
	"Unfavourite a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Unfavourite a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Unfavourite a publication in global timeline"
#elif L==4	// es
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Unfavourite a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActFavSocComGbl] =
#if   L==1	// ca
	"Favourite a comment in global timeline"	// Necessita traducció
#elif L==2	// de
	"Favourite a comment in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Favourite a comment in global timeline"
#elif L==4	// es
	"Marcar como favorito un comentario en timeline global"
#elif L==5	// fr
	"Favourite a comment in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorito un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Favourite a comment in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a comment in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a comment in global timeline"	// Precisa de tradução
#endif
	,
	[ActUnfSocComGbl] =
#if   L==1	// ca
	"Unfavourite a comment in global timeline"	// Necessita traducció
#elif L==2	// de
	"Unfavourite a comment in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Unfavourite a comment in global timeline"
#elif L==4	// es
	"Desmarcar como favorito un comentario en timeline global"
#elif L==5	// fr
	"Unfavourite a comment in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Desmarcar como favorito un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a comment in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a comment in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a comment in global timeline"	// Precisa de tradução
#endif
	,
	[ActReqRemSocPubGbl] =
#if   L==1	// ca
	"Request deletion of a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Request deletion of a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Request deletion of a publication in global timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Request deletion of a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActRemSocPubGbl] =
#if   L==1	// ca
	"Delete a publication in global timeline"	// Necessita traducció
#elif L==2	// de
	"Delete a publication in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Delete a publication in global timeline"
#elif L==4	// es
	"Eliminar una publicaci&oacute;n en timeline global"
#elif L==5	// fr
	"Delete a publication in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar una publicaci&oacute;n en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Delete a publication in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Delete a publication in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Delete a publication in global timeline"	// Precisa de tradução
#endif
	,
	[ActReqRemSocComGbl] =
#if   L==1	// ca
	"Request deletion of a comment in global timeline"	// Necessita traducció
#elif L==2	// de
	"Request deletion of a comment in global timeline"	// Need Übersetzung
#elif L==3	// en
	"Request deletion of a comment in global timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de un comentario en timeline global"
#elif L==5	// fr
	"Request deletion of a comment in global timeline"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de un comentario en timeline global"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a comment in global timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a comment in global timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a comment in global timeline"	// Precisa de tradução
#endif
	,
	[ActRemSocComGbl] =
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
	[ActRefOldSocPubUsr] =
#if   L==1	// ca
	"Show previous publications in user timeline"	// Necessita traducció
#elif L==2	// de
	"Show previous publications in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Show previous publications in user timeline"
#elif L==4	// es
	"Mostrar publicaciones anteriores en timeline de usuario"
#elif L==5	// fr
	"Show previous publications in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar publicaciones anteriores en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show previous publications in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show previous publications in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show previous publications in user timeline"	// Precisa de tradução
#endif
	,
	[ActRcvSocPstUsr] =
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
	[ActRcvSocComUsr] =
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
	[ActShoHidSocComUsr] =
#if   L==1	// ca
	"Show hidden comments in user timeline"		// Necessita traducció
#elif L==2	// de
	"Show hidden comments in user timeline"		// Need Übersetzung
#elif L==3	// en
	"Show hidden comments in user timeline"
#elif L==4	// es
	"Mostrar comentarios ocultos en timeline de usuario"
#elif L==5	// fr
	"Show hidden comments in user timeline"		// Besoin de traduction
#elif L==6	// gn
	"Mostrar comentarios ocultos en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show hidden comments in user timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Show hidden comments in user timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show hidden comments in user timeline"		// Precisa de tradução
#endif
	,
	[ActAllShaSocNotUsr] =
#if   L==1	// ca
	"Show who shared a publication in user timeline"	// Necessita traducció
#elif L==2	// de
	"Show who shared a publication in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Show who shared a publication in user timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Show who shared a publication in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes compartieron una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show who shared a publication in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show who shared a publication in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who shared a publication in user timeline"	// Precisa de tradução
#endif
	,
	[ActAllFavSocNotUsr] =
#if   L==1	// ca
	"Show who favourited a publication in user timeline"	// Necessita traducció
#elif L==2	// de
	"Show who favourited a publication in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Show who favourited a publication in user timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Show who favourited a publication in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorita una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a publication in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a publication in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a publication in user timeline"	// Precisa de tradução
#endif
	,
	[ActAllFavSocComUsr] =
#if   L==1	// ca
	"Show who favourited a comment in user timeline"	// Necessita traducció
#elif L==2	// de
	"Show who favourited a comment in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Show who favourited a comment in user timeline"
#elif L==4	// es
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline de usuario"
#elif L==5	// fr
	"Show who favourited a comment in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Mostrar qui&eacute;nes marcaron como favorito un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Show who favourited a comment in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Show who favourited a comment in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Show who favourited a comment in user timeline"	// Precisa de tradução
#endif
	,
	[ActShaSocNotUsr] =
#if   L==1	// ca
	"Share a publication in user timeline"		// Necessita traducció
#elif L==2	// de
	"Share a publication in user timeline"		// Need Übersetzung
#elif L==3	// en
	"Share a publication in user timeline"
#elif L==4	// es
	"Compartir una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Share a publication in user timeline"		// Besoin de traduction
#elif L==6	// gn
	"Compartir una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Share a publication in user timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Share a publication in user timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Share a publication in user timeline"		// Precisa de tradução
#endif
	,
	[ActUnsSocNotUsr] =
#if   L==1	// ca
	"Unshare a publication in user timeline"	// Necessita traducció
#elif L==2	// de
	"Unshare a publication in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Unshare a publication in user timeline"
#elif L==4	// es
	"Dejar de compartir una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Unshare a publication in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Dejar de compartir una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Unshare a publication in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Unshare a publication in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unshare a publication in user timeline"	// Precisa de tradução
#endif
	,
	[ActFavSocNotUsr] =
#if   L==1	// ca
	"Favourite a publication in user timeline"	// Necessita traducció
#elif L==2	// de
	"Favourite a publication in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Favourite a publication in user timeline"
#elif L==4	// es
	"Marcar como favorita una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Favourite a publication in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorita una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Favourite a publication in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a publication in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a publication in user timeline"	// Precisa de tradução
#endif
	,
	[ActUnfSocNotUsr] =
#if   L==1	// ca
	"Unfavourite a publication in user timeline"	// Necessita traducció
#elif L==2	// de
	"Unfavourite a publication in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Unfavourite a publication in user timeline"
#elif L==4	// es
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Unfavourite a publication in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Dejar de marcar como favorita una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a publication in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a publication in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a publication in user timeline"	// Precisa de tradução
#endif
	,
	[ActFavSocComUsr] =
#if   L==1	// ca
	"Favourite a comment in user timeline"		// Necessita traducció
#elif L==2	// de
	"Favourite a comment in user timeline"		// Need Übersetzung
#elif L==3	// en
	"Favourite a comment in user timeline"
#elif L==4	// es
	"Marcar como favorito un comentario en timeline de usuario"
#elif L==5	// fr
	"Favourite a comment in user timeline"		// Besoin de traduction
#elif L==6	// gn
	"Marcar como favorito un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Favourite a comment in user timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Favourite a comment in user timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Favourite a comment in user timeline"		// Precisa de tradução
#endif
	,
	[ActUnfSocComUsr] =
#if   L==1	// ca
	"Unfavourite a comment in user timeline"	// Necessita traducció
#elif L==2	// de
	"Unfavourite a comment in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Unfavourite a comment in user timeline"
#elif L==4	// es
	"Desmarcar como favorito un comentario en timeline de usuario"
#elif L==5	// fr
	"Unfavourite a comment in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Desmarcar como favorito un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Unfavourite a comment in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Unfavourite a comment in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Unfavourite a comment in user timeline"	// Precisa de tradução
#endif
	,
	[ActReqRemSocPubUsr] =
#if   L==1	// ca
	"Request deletion of a publication in user timeline"	// Necessita traducció
#elif L==2	// de
	"Request deletion of a publication in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Request deletion of a publication in user timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Request deletion of a publication in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a publication in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a publication in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a publication in user timeline"	// Precisa de tradução
#endif
	,
	[ActRemSocPubUsr] =
#if   L==1	// ca
	"Delete a publication in user timeline"		// Necessita traducció
#elif L==2	// de
	"Delete a publication in user timeline"		// Need Übersetzung
#elif L==3	// en
	"Delete a publication in user timeline"
#elif L==4	// es
	"Eliminar una publicaci&oacute;n en timeline de usuario"
#elif L==5	// fr
	"Delete a publication in user timeline"		// Besoin de traduction
#elif L==6	// gn
	"Eliminar una publicaci&oacute;n en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Delete a publication in user timeline"		// Bisogno di traduzione
#elif L==8	// pl
	"Delete a publication in user timeline"		// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Delete a publication in user timeline"		// Precisa de tradução
#endif
	,
	[ActReqRemSocComUsr] =
#if   L==1	// ca
	"Request deletion of a comment in user timeline"	// Necessita traducció
#elif L==2	// de
	"Request deletion of a comment in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Request deletion of a comment in user timeline"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de un comentario en timeline de usuario"
#elif L==5	// fr
	"Request deletion of a comment in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Solicitar eliminaci&oacute;n de un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Request deletion of a comment in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Request deletion of a comment in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Request deletion of a comment in user timeline"	// Precisa de tradução
#endif
	,
	[ActRemSocComUsr] =
#if   L==1	// ca
	"Delete a comment in user timeline"	// Necessita traducció
#elif L==2	// de
	"Delete a comment in user timeline"	// Need Übersetzung
#elif L==3	// en
	"Delete a comment in user timeline"
#elif L==4	// es
	"Eliminar un comentario en timeline de usuario"
#elif L==5	// fr
	"Delete a comment in user timeline"	// Besoin de traduction
#elif L==6	// gn
	"Eliminar un comentario en timeline de usuario"	// Okoteve traducción
#elif L==7	// it
	"Delete a comment in user timeline"	// Bisogno di traduzione
#elif L==8	// pl
	"Delete a comment in user timeline"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Delete a comment in user timeline"	// Precisa de tradução
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
	"Change first day of the week in calendar"	// Necessita traducció
#elif L==2	// de
	"Change first day of the week in calendar"	// Need Übersetzung
#elif L==3	// en
	"Change first day of the week in calendar"
#elif L==4	// es
	"Cambiar primer d&iacute;a de la semana en calendario"
#elif L==5	// fr
	"Change first day of the week in calendar"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar primer d&iacute;a de la semana en calendario"	// Okoteve traducción
#elif L==7	// it
	"Change first day of the week in calendar"	// Bisogno di traduzione
#elif L==8	// pl
	"Change first day of the week in calendar"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change first day of the week in calendar"	// Precisa de tradução
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
	"Change email domain information"	// Necessita traducció
#elif L==2	// de
	"Change email domain information"	// Need Übersetzung
#elif L==3	// en
	"Change email domain information"
#elif L==4	// es
	"Cambiar informaci&oacute;n de dominio de correo"
#elif L==5	// fr
	"Change email domain information"	// Besoin de traduction
#elif L==6	// gn
	"Cambiar informaci&oacute;n de dominio de correo"	// Okoteve traducción
#elif L==7	// it
	"Change email domain information"	// Bisogno di traduzione
#elif L==8	// pl
	"Change email domain information"	// Potrzebujesz tlumaczenie
#elif L==9	// pt
	"Change email domain information"	// Precisa de tradução
#endif
	,
	[ActSeeSysInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show information on the platform"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre la plataforma"
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
	[ActSeeCty] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List countries"
#elif L==4	// es
	""
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
	[ActSeePen] =
#if   L==1	// ca
	"Institutions, centres, titulacions i assignatures pendents d'activar"
#elif L==2	// de
	"Anh&auml;ngig Hochschulen, Lehrinstitute, Studiengang und Kursen"
#elif L==3	// en
	"Pending institutions, centres, degrees and courses"
#elif L==4	// es
	"Instituciones, centros, titulaciones y asignaturas pendientes de activar"
#elif L==5	// fr
	"&Eacute;tablissements, centres, &eacute;tudes et mati&egrave;res en attente"
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
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See institutional links"
#elif L==4	// es
	""
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
	[ActLstPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List plugins"
#elif L==4	// es
	""
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
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print information on the platform"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre la plataforma"
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
	[ActEdiCty] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit countries"
#elif L==4	// es
	""
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
	[ActNewCty] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a country"
#elif L==4	// es
	""
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
	[ActRemCty] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a country"
#elif L==4	// es
	""
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
	[ActRenCty] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the name of a country"
#elif L==4	// es
	""
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
	[ActChgCtyWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change web of country"
#elif L==4	// es
	""
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
	[ActSeeBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See banners"
#elif L==4	// es
	""
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
	[ActEdiBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit banners"
#elif L==4	// es
	""
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
	[ActNewBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a banner"
#elif L==4	// es
	""
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
	[ActRemBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a banner"
#elif L==4	// es
	""
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
	[ActShoBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show a hidden banner"
#elif L==4	// es
	""
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
	[ActHidBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a visible banner"
#elif L==4	// es
	""
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
	[ActRenBanSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the short name of a banner"
#elif L==4	// es
	""
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
	[ActRenBanFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the full name of a banner"
#elif L==4	// es
	""
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
	[ActChgBanImg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the image of a banner"
#elif L==4	// es
	""
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
	[ActChgBanWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the web of a banner"
#elif L==4	// es
	""
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
	[ActClkBan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Go to a banner when clicked"
#elif L==4	// es
	""
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
	[ActEdiLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit institutional links"
#elif L==4	// es
	""
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
	[ActNewLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of an institutional link"
#elif L==4	// es
	""
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
	[ActRemLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of an institutional link"
#elif L==4	// es
	""
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
	[ActRenLnkSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the short name of an institutional link"
#elif L==4	// es
	""
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
	[ActRenLnkFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the full name of an institutional link"
#elif L==4	// es
	""
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
	[ActChgLnkWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the change of the web of an institutional link"
#elif L==4	// es
	""
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
	[ActEdiPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit plugins"
#elif L==4	// es
	""
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
	[ActNewPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a plugin"
#elif L==4	// es
	""
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
	[ActRemPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a plugin"
#elif L==4	// es
	""
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
	[ActRenPlg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a plugin"
#elif L==4	// es
	""
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
	[ActChgPlgDes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the description of a plugin"
#elif L==4	// es
	""
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
	[ActChgPlgLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the logo of a plugin"
#elif L==4	// es
	""
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
	[ActChgPlgAppKey] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the application key of a plugin"
#elif L==4	// es
	""
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
	[ActChgPlgURL] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the URL of a plugin"
#elif L==4	// es
	""
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
	[ActChgPlgIP] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the IP address of a plugin"
#elif L==4	// es
	""
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
	""
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
	[ActRemOldCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove completely old courses"
#elif L==4	// es
	""
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
	""			// Okoteve traducción
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
	""
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
	""			// Okoteve traducción
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
	"Change map attribution of country"
#elif L==4	// es
	""
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
	[ActEdiIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit institutions"
#elif L==4	// es
	""
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
	[ActReqIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a request for a new institution (a teacher makes the petition to an administrator)"
#elif L==4	// es
	""
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
	[ActNewIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of an institution"
#elif L==4	// es
	""
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
	[ActRemIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove institution"
#elif L==4	// es
	""
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
	[ActRenInsSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name of institution"
#elif L==4	// es
	""
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
	[ActRenInsFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name of institution"
#elif L==4	// es
	""
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
	[ActChgInsWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change web of institution"
#elif L==4	// es
	""
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
	[ActChgInsSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of status of an institution"
#elif L==4	// es
	""
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
	"List centres"
#elif L==4	// es
	""
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
	[ActSeePlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List places"
#elif L==4	// es
	""
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
	[ActSeeDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List departments"
#elif L==4	// es
	""
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
	[ActSeeHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List holidays"
#elif L==4	// es
	""
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
	""			// Okoteve traducción
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
	"Change country of institution in institution configuration"
#elif L==4	// es
	""
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
	[ActRenInsShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name of institution in institution configuration"
#elif L==4	// es
	""
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
	[ActRenInsFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name of institution in institution configuration"
#elif L==4	// es
	""
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
	[ActChgInsWWWCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change web of institution in institution configuration"
#elif L==4	// es
	""
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
	[ActReqInsLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send the logo of the current institution"
#elif L==4	// es
	""
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
	[ActRecInsLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and store the logo of the current institution"
#elif L==4	// es
	""
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
	[ActRemInsLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove the logo of the current institution"
#elif L==4	// es
	""
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
	[ActEdiCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit centres"
#elif L==4	// es
	""
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
	[ActReqCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a request for a new centre (a teacher makes the petition to an administrator)"
#elif L==4	// es
	""
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
	[ActNewCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a centre"
#elif L==4	// es
	""
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
	[ActRemCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove centre"
#elif L==4	// es
	""
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
	[ActChgCtrPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the place of a centre"
#elif L==4	// es
	""
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
	[ActRenCtrSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name centre"
#elif L==4	// es
	""
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
	[ActRenCtrFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name centre"
#elif L==4	// es
	""
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
	[ActChgCtrWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change web of centre"
#elif L==4	// es
	""
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
	[ActChgCtrSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of status of a centre"
#elif L==4	// es
	""
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
	[ActEdiPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit places"
#elif L==4	// es
	""
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
	[ActNewPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a place"
#elif L==4	// es
	""
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
	[ActRemPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a place"
#elif L==4	// es
	""
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
	[ActRenPlcSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name of a place"
#elif L==4	// es
	""
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
	[ActRenPlcFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name of a place"
#elif L==4	// es
	""
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
	[ActEdiDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit departments"
#elif L==4	// es
	""
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
	[ActNewDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a department"
#elif L==4	// es
	""
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
	[ActRemDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove department"
#elif L==4	// es
	""
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
	[ActChgDptIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the institution of a department"
#elif L==4	// es
	""
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
	[ActRenDptSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name department"
#elif L==4	// es
	""
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
	[ActRenDptFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name department"
#elif L==4	// es
	""
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
	[ActChgDptWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change web of department"
#elif L==4	// es
	""
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
	[ActEdiHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit holidays"
#elif L==4	// es
	""
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
	[ActNewHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a holiday"
#elif L==4	// es
	""
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
	[ActRemHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a holiday"
#elif L==4	// es
	""
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
	[ActChgHldPlc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change place of a holiday"
#elif L==4	// es
	""
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
	[ActChgHldTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change type of a holiday"
#elif L==4	// es
	""
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
	[ActChgHldStrDat] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change start date of a holiday"
#elif L==4	// es
	""
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
	[ActChgHldEndDat] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change end date of a holiday"
#elif L==4	// es
	""
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
	[ActRenHld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change name of a holiday"
#elif L==4	// es
	""
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
	[ActSeeCtrInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show information on the centre"
#elif L==4	// es
	"Mostrar informaci&oacute;n sobre el centro"
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
	[ActSeeDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List degrees in a centre"
#elif L==4	// es
	""
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
	[ActSeeBld] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List buildings in a centre"
#elif L==4	// es
	"Listar edificios de un centro"
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
	[ActSeeRoo] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List rooms in a centre"
#elif L==4	// es
	"Listar salas de un centro"
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
	[ActPrnCtrInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print information on the centre"
#elif L==4	// es
	"Imprimir informaci&oacute;n sobre el centro"
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
	[ActChgCtrInsCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the institution of a centre in centre configuration"
#elif L==4	// es
	""
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
	[ActRenCtrShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change short name centre in centre configuration"
#elif L==4	// es
	""
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
	[ActRenCtrFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change full name centre in centre configuration"
#elif L==4	// es
	""
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
	[ActChgCtrPlcCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change place of centre in centre configuration"
#elif L==4	// es
	""
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
	[ActChgCtrLatCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change latitude of centre in centre configuration"
#elif L==4	// es
	""
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
	[ActChgCtrLgtCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change longitude of centre in centre configuration"
#elif L==4	// es
	""
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
	[ActChgCtrAltCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change altitude of centre in centre configuration"
#elif L==4	// es
	""
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
	[ActChgCtrWWWCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change web of centre in centre configuration"
#elif L==4	// es
	""
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
	[ActReqCtrLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send the logo of the current centre"
#elif L==4	// es
	""
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
	[ActRecCtrLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and store the logo of the current centre"
#elif L==4	// es
	""
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
	[ActRemCtrLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove the logo of the current centre"
#elif L==4	// es
	""
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
	[ActReqCtrPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send the photo of the current centre"
#elif L==4	// es
	""
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
	[ActRecCtrPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and store the photo of the current centre"
#elif L==4	// es
	""
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
	[ActChgCtrPhoAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change attribution of centre photo"
#elif L==4	// es
	""
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
	[ActSeeDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List types of degrees"
#elif L==4	// es
	""
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
	[ActEdiDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request edition of types of degrees"
#elif L==4	// es
	""
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
	[ActNewDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a type of degree"
#elif L==4	// es
	""
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
	[ActRemDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a type of degree"
#elif L==4	// es
	""
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
	[ActRenDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request renaming of a type of degree"
#elif L==4	// es
	""
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
	[ActEdiDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request edition of degrees of a type"
#elif L==4	// es
	""
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
	[ActReqDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a request for a new degree (a teacher makes the petition to an administrator)"
#elif L==4	// es
	""
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
	[ActNewDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a degree"
#elif L==4	// es
	""
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
	[ActRemDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a degree"
#elif L==4	// es
	""
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
	[ActRenDegSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the short name of a degree"
#elif L==4	// es
	""
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
	[ActRenDegFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the full name of a degree"
#elif L==4	// es
	""
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
	[ActChgDegTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the type of a degree"
#elif L==4	// es
	""
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
	[ActChgDegWWW] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the web of a degree"
#elif L==4	// es
	""
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
	[ActChgDegSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of status of a degree"
#elif L==4	// es
	""
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""
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
	""			// Okoteve traducción
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
	"Request change of the centre of a degree in degree configuration"
#elif L==4	// es
	""
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
	[ActRenDegShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the short name of a degree in degree configuration"
#elif L==4	// es
	""
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
	[ActRenDegFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the full name of a degree in degree configuration"
#elif L==4	// es
	""
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
	[ActChgDegWWWCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of the web of a degree in degree configuration"
#elif L==4	// es
	""
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
	[ActReqDegLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send the logo of the current degree"
#elif L==4	// es
	""
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
	[ActRecDegLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and store the logo of the current degree"
#elif L==4	// es
	""
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
	[ActRemDegLog] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove the logo of the current degree"
#elif L==4	// es
	""
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
	[ActEdiCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request edition of courses of a degree"
#elif L==4	// es
	""
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
	[ActReqCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a request for a new course (a teacher makes the petition to an administrator)"
#elif L==4	// es
	""
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
	[ActNewCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a course"
#elif L==4	// es
	""
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
	[ActRemCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a course"
#elif L==4	// es
	""
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
	[ActChgInsCrsCod] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of institutional code of a course"
#elif L==4	// es
	""
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
	[ActChgCrsYea] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of year of a course inside of its degree"
#elif L==4	// es
	""
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
	[ActRenCrsSho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of short name of a course"
#elif L==4	// es
	""
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
	[ActRenCrsFul] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of full name of a course"
#elif L==4	// es
	""
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
	[ActChgCrsSta] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of status of a course"
#elif L==4	// es
	""
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
	""			// Okoteve traducción
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
	"See educational schedule of the course"
#elif L==4	// es
	"Ver programaci&oacute;n did&aacute;ctica de la asignatura"
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Request the removal of program item"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de &iacute;tem del programa"
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Show teaching guide of the course"
#elif L==4	// es
	""
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
	[ActSeeSyl] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show syllabus (lectures or practicals)"
#elif L==4	// es
	""
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
	[ActSeeBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the bibliography"
#elif L==4	// es
	""
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
	[ActSeeFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the FAQ"
#elif L==4	// es
	""
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
	[ActSeeCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show links related to the course"
#elif L==4	// es
	""
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
	[ActSeeCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the timetable"
#elif L==4	// es
	""
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
	""			// Okoteve traducción
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
	"Request change of degree in course configuration"
#elif L==4	// es
	""
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
	[ActRenCrsShoCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of short name of a course in course configuration"
#elif L==4	// es
	""
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
	[ActRenCrsFulCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change of full name of a course in course configuration"
#elif L==4	// es
	""
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
	[ActChgInsCrsCodCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change institutional code in course configuration"
#elif L==4	// es
	""
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
	[ActChgCrsYeaCfg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change year/semester in course configuration"
#elif L==4	// es
	""
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
	[ActEdiCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit general information about the course"
#elif L==4	// es
	""
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
	[ActEdiTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit teaching guide of the course"
#elif L==4	// es
	""
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
	[ActSeeSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActSeeSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActEdiSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActEdiSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActDelItmSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a item from syllabus of lectures"
#elif L==4	// es
	""
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
	[ActDelItmSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a item from syllabus of practicals"
#elif L==4	// es
	""
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
	[ActUp_IteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Subir the posición of a subtree of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActUp_IteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Subir the posición of a subtree of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActDwnIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Bajar the posición of a subtree of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActDwnIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Bajar the position of a subtree of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActRgtIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Aumentar the level of a item of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActRgtIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Aumentar the level of a item of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActLftIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disminuir the level of a item of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActLftIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disminuir the level of a item of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActInsIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Insertar a new item in the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActInsIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Insertar a new item in the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActModIteSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify a item of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActModIteSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify a item of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActEdiBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the bibliography"
#elif L==4	// es
	""
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
	[ActEdiFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the FAQ"
#elif L==4	// es
	""
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
	[ActEdiCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the links relacionados with the course"
#elif L==4	// es
	""
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
	[ActChgFrcReaCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read course info"
#elif L==4	// es
	""
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
	[ActChgFrcReaTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read teaching guide"
#elif L==4	// es
	""
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
	[ActChgFrcReaSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read lectures syllabus"
#elif L==4	// es
	""
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
	[ActChgFrcReaSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read practicals syllabus"
#elif L==4	// es
	""
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
	[ActChgFrcReaBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read bibliography"
#elif L==4	// es
	""
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
	[ActChgFrcReaFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read FAQ"
#elif L==4	// es
	""
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
	[ActChgFrcReaCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change force students to read links"
#elif L==4	// es
	""
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
	[ActChgHavReaCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read course info"
#elif L==4	// es
	""
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
	[ActChgHavReaTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read teaching guide"
#elif L==4	// es
	""
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
	[ActChgHavReaSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read lectures syllabus"
#elif L==4	// es
	""
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
	[ActChgHavReaSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read practicals syllabus"
#elif L==4	// es
	""
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
	[ActChgHavReaBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read bibliography"
#elif L==4	// es
	""
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
	[ActChgHavReaFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read FAQ"
#elif L==4	// es
	""
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
	[ActChgHavReaCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read links"
#elif L==4	// es
	""
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
	[ActSelInfSrcCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of info shown in the general information about the course"
#elif L==4	// es
	""
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
	[ActSelInfSrcTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of info shown in the teaching guide"
#elif L==4	// es
	""
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
	[ActSelInfSrcSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of info shown in the lectures syllabus"
#elif L==4	// es
	""
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
	[ActSelInfSrcSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of info shown in the practicals syllabus"
#elif L==4	// es
	""
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
	[ActSelInfSrcBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of info shown in the bibliography"
#elif L==4	// es
	""
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
	[ActSelInfSrcFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of info shown in the FAQ"
#elif L==4	// es
	""
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
	[ActSelInfSrcCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select the type of info shown in the links"
#elif L==4	// es
	""
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
	[ActRcvURLCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link a the general information about the course"
#elif L==4	// es
	""
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
	[ActRcvURLTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link a the teaching guide"
#elif L==4	// es
	""
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
	[ActRcvURLSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link al syllabus of lectures"
#elif L==4	// es
	""
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
	[ActRcvURLSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link al syllabus of practicals"
#elif L==4	// es
	""
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
	[ActRcvURLBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link a bibliography"
#elif L==4	// es
	""
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
	[ActRcvURLFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link a FAQ"
#elif L==4	// es
	""
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
	[ActRcvURLCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link a links"
#elif L==4	// es
	""
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
	[ActRcvPagCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with the general information about the course"
#elif L==4	// es
	""
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
	[ActRcvPagTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with the teaching guide"
#elif L==4	// es
	""
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
	[ActRcvPagSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActRcvPagSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActRcvPagBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with bibliography"
#elif L==4	// es
	""
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
	[ActRcvPagFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with FAQ"
#elif L==4	// es
	""
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
	[ActRcvPagCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with links"
#elif L==4	// es
	""
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
	[ActEditorCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the general information about the course"
#elif L==4	// es
	""
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
	[ActEditorTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the teaching guide"
#elif L==4	// es
	""
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
	[ActEditorSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActEditorSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActEditorBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of bibliography"
#elif L==4	// es
	""
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
	[ActEditorFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of FAQ"
#elif L==4	// es
	""
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
	[ActEditorCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of links"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the general information about the course"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the teaching guide"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the bibliography"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the FAQ"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the links"
#elif L==4	// es
	""
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
	[ActRchTxtEdiCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the general information about the course"
#elif L==4	// es
	""
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
	[ActRchTxtEdiTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the teaching guide"
#elif L==4	// es
	""
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
	[ActRchTxtEdiSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActRchTxtEdiSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActRchTxtEdiBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the bibliography"
#elif L==4	// es
	""
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
	[ActRchTxtEdiFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the FAQ"
#elif L==4	// es
	""
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
	[ActRchTxtEdiCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of the links"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the general information about the course"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the teaching guide"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the bibliography"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the FAQ"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the links"
#elif L==4	// es
	""
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
	[ActRcvRchTxtCrsInf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the general information about the course"
#elif L==4	// es
	""
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
	[ActRcvRchTxtTchGui] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the teaching guide"
#elif L==4	// es
	""
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
	[ActRcvRchTxtSylLec] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the syllabus of lectures"
#elif L==4	// es
	""
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
	[ActRcvRchTxtSylPra] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the syllabus of practicals"
#elif L==4	// es
	""
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
	[ActRcvRchTxtBib] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the bibliography"
#elif L==4	// es
	""
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
	[ActRcvRchTxtFAQ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the FAQ"
#elif L==4	// es
	""
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
	[ActRcvRchTxtCrsLnk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the links"
#elif L==4	// es
	""
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
	[ActPrnCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show print view of the timetable"
#elif L==4	// es
	""
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
	[ActEdiCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the timetable"
#elif L==4	// es
	""
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
	[ActChgCrsTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify the timetable of the course"
#elif L==4	// es
	""
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
	[ActChgCrsTT1stDay] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change first day of week and show timetable of the course"
#elif L==4	// es
	""
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Change force students to read assessment system"
#elif L==4	// es
	""
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
	[ActChgHavReaAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change if I have read assessment system"
#elif L==4	// es
	""
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
	[ActSelInfSrcAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Selec. type of assessment"
#elif L==4	// es
	""
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
	[ActRcvURLAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a link a assessment"
#elif L==4	// es
	""
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
	[ActRcvPagAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a page with assessment"
#elif L==4	// es
	""
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
	[ActEditorAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Integrated editor of assessment"
#elif L==4	// es
	""
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
	[ActPlaTxtEdiAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of plain text of assessment"
#elif L==4	// es
	""
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
	[ActRchTxtEdiAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Editor of rich text of assessment"
#elif L==4	// es
	""
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
	[ActRcvPlaTxtAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the plain text of the assessment system"
#elif L==4	// es
	""
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
	[ActRcvRchTxtAss] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and change the rich text of the assessment system"
#elif L==4	// es
	""
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
	[ActFrmNewAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new assignment"
#elif L==4	// es
	""
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
	[ActEdiOneAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit one assignment"
#elif L==4	// es
	""
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
	[ActPrnOneAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print one assignment"
#elif L==4	// es
	""
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
	[ActNewAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new assignment"
#elif L==4	// es
	""
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
	[ActChgAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify data of an existing assignment"
#elif L==4	// es
	""
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
	[ActReqRemAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of an assignment"
#elif L==4	// es
	""
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
	[ActRemAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove assignment"
#elif L==4	// es
	""
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
	[ActHidAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide assignment"
#elif L==4	// es
	""
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
	[ActShoAsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show assignment"
#elif L==4	// es
	""
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
	[ActReqUsrPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select users to list their projects"
#elif L==4	// es
	""
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
	[ActSeeTblAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all projects in a table"
#elif L==4	// es
	""
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
	[ActCfgPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Configure all projects"
#elif L==4	// es
	""
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
	[ActRcvCfgPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive configuration of all projects"
#elif L==4	// es
	""
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
	[ActReqLckAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request locking of all projects"
#elif L==4	// es
	""
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
	[ActReqUnlAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request unlocking of all projects"
#elif L==4	// es
	""
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
	[ActLckAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Lock all projects"
#elif L==4	// es
	""
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
	[ActUnlAllPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unlock all projects"
#elif L==4	// es
	""
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
	[ActFrmNewPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new project"
#elif L==4	// es
	""
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
	[ActEdiOnePrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit one project"
#elif L==4	// es
	""
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
	[ActPrnOnePrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print one project"
#elif L==4	// es
	""
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
	[ActNewPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new project"
#elif L==4	// es
	""
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
	[ActChgPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify data of an existing project"
#elif L==4	// es
	""
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
	[ActReqRemPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of an project"
#elif L==4	// es
	""
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
	[ActRemPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove project"
#elif L==4	// es
	""
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
	[ActHidPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide project"
#elif L==4	// es
	""
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
	[ActShoPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show project"
#elif L==4	// es
	""
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
	[ActLckPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Lock project edition"
#elif L==4	// es
	""
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
	[ActUnlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unlock project edition"
#elif L==4	// es
	""
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
	[ActReqAddStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request adding a student to a project"
#elif L==4	// es
	""
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
	[ActReqAddTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request adding a tutor to a project"
#elif L==4	// es
	""
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
	[ActReqAddEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request adding an evaluator to a project"
#elif L==4	// es
	""
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
	[ActAddStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add a student to a project"
#elif L==4	// es
	""
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
	[ActAddTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add a tutor to a project"
#elif L==4	// es
	""
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
	[ActAddEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add an evaluator to a project"
#elif L==4	// es
	""
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
	[ActReqRemStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing a student from a project"
#elif L==4	// es
	""
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
	[ActReqRemTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing a tutor from a project"
#elif L==4	// es
	""
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
	[ActReqRemEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removing an evaluator from a project"
#elif L==4	// es
	""
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
	[ActRemStdPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a student from a project"
#elif L==4	// es
	""
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
	[ActRemTutPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a tutor from a project"
#elif L==4	// es
	""
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
	[ActRemEvlPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an emulator from a project"
#elif L==4	// es
	""
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
	[ActAdmDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin. project documents"
#elif L==4	// es
	""
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
	[ActReqRemFilDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from project documents"
#elif L==4	// es
	""
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
	[ActRemFilDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from project documents"
#elif L==4	// es
	""
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
	[ActRemFolDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from project documents"
#elif L==4	// es
	""
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
	[ActCopDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in project documents"
#elif L==4	// es
	""
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
	[ActPasDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file into project documents"
#elif L==4	// es
	""
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
	[ActRemTreDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non empty folder from project documents"
#elif L==4	// es
	""
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
	[ActFrmCreDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a folder or file in project documents"
#elif L==4	// es
	""
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
	[ActCreFolDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in project documents"
#elif L==4	// es
	""
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
	[ActCreLnkDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in project documents"
#elif L==4	// es
	""
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
	[ActRenFolDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in project documents"
#elif L==4	// es
	""
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
	[ActRcvFilDocPrjDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in project documents using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilDocPrjCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in project documents using the classic way"
#elif L==4	// es
	""
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
	[ActExpDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in project documents"
#elif L==4	// es
	""
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
	[ActConDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in project documents"
#elif L==4	// es
	""
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
	[ActZIPDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in project documents"
#elif L==4	// es
	""
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
	[ActReqDatDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file in project documents"
#elif L==4	// es
	""
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
	[ActChgDatDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file in project documents"
#elif L==4	// es
	""
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
	[ActDowDocPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from project documents"
#elif L==4	// es
	""
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
	[ActAdmAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin. project assessment"
#elif L==4	// es
	""
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
	[ActReqRemFilAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file from project assessment"
#elif L==4	// es
	""
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
	[ActRemFilAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file from project assessment"
#elif L==4	// es
	""
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
	[ActRemFolAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder from project assessment"
#elif L==4	// es
	""
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
	[ActCopAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in project assessment"
#elif L==4	// es
	""
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
	[ActPasAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file into project assessment"
#elif L==4	// es
	""
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
	[ActRemTreAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non empty folder from project assessment"
#elif L==4	// es
	""
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
	[ActFrmCreAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a folder or file in project assessment"
#elif L==4	// es
	""
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
	[ActCreFolAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in project assessment"
#elif L==4	// es
	""
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
	[ActCreLnkAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in project assessment"
#elif L==4	// es
	""
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
	[ActRenFolAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in project assessment"
#elif L==4	// es
	""
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
	[ActRcvFilAssPrjDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in project assessment using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilAssPrjCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in project assessment using the classic way"
#elif L==4	// es
	""
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
	[ActExpAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in project assessment"
#elif L==4	// es
	""
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
	[ActConAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in project assessment"
#elif L==4	// es
	""
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
	[ActZIPAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in project assessment"
#elif L==4	// es
	""
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
	[ActReqDatAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file in project assessment"
#elif L==4	// es
	""
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
	[ActChgDatAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file in project assessment"
#elif L==4	// es
	""
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
	[ActDowAssPrj] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file from project assessment"
#elif L==4	// es
	""
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Receive an exam announcement"
#elif L==4	// es
	"Recibir una convocatoria de examen"
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
	""			// Okoteve traducción
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
	"Request the removal of an exam announcement"
#elif L==4	// es
	"Solicitar la eliminaci&oacute;n de una convocatoria de examen"
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Request test results of several users"
#elif L==4	// es
	"Solicitar resultados de test de varios usuarios"
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
	[ActSeeUsrTstResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See test results of several users"
#elif L==4	// es
	"Ver resultados de test de varios usuarios"
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
	[ActSeeOneTstResOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See one test result of another user"
#elif L==4	// es
	"Ver un resultado de test de otro usuario"
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Add question to exam"
#elif L==4	// es
	"A&ntilde;adir pregunta a exam"
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Comenzar / reanudar examen"			// Okoteve traducción
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
	"Responder pregunta de examen"			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Request the removal of a match"
#elif L==4	// es
	"Solicitar eliminaci&oacute;n de una partida"
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Create a new match show it in a new tab"
#elif L==4	// es
	"Crear una nueva partida y mostrarla en una nueva pesta&ntilde;a"
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	""			// Okoteve traducción
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
	"Refresh current question when playing a game (as student)"
#elif L==4	// es
	""
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
	[ActSeeMyMchResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all my matches results in the current course"
#elif L==4	// es
	""
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
	[ActSeeMyMchResGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all my matches results in a given game"
#elif L==4	// es
	""
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
	[ActSeeMyMchResMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all my matches results in a given match"
#elif L==4	// es
	""
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
	[ActSeeOneMchResMe] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show one match result of me as student"
#elif L==4	// es
	""
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
	[ActReqSeeUsrMchRes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select users and range of dates to see matches results"
#elif L==4	// es
	""
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
	[ActSeeUsrMchResCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all matches results of selected users in the current course"
#elif L==4	// es
	""
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
	[ActSeeUsrMchResGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all matches results in a given game"
#elif L==4	// es
	""
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
	[ActSeeUsrMchResMch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all matches results in a given game"
#elif L==4	// es
	""
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
	[ActSeeOneMchResOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show one match result of other user"
#elif L==4	// es
	""
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
	[ActChgVisResMchUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change visibility of match results for students"
#elif L==4	// es
	""
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
	[ActFrmNewGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new game"
#elif L==4	// es
	""
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
	[ActEdiOneGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit one game"
#elif L==4	// es
	""
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
	[ActNewGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new game"
#elif L==4	// es
	""
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
	[ActChgGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify data of an existing game"
#elif L==4	// es
	""
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
	[ActReqRemGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a game"
#elif L==4	// es
	""
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
	[ActRemGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove game"
#elif L==4	// es
	""
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
	[ActHidGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide game"
#elif L==4	// es
	""
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
	[ActShoGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show game"
#elif L==4	// es
	""
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
	[ActAddOneGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add a new question to a game"
#elif L==4	// es
	""
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
	[ActGamLstTstQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List test questions to select one or several questions"
#elif L==4	// es
	""
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
	[ActAddTstQstToGam] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Add selected test questions to game"
#elif L==4	// es
	""
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
	[ActReqRemGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a question of a game"
#elif L==4	// es
	""
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
	[ActRemGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm the removal of a question of a game"
#elif L==4	// es
	""
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
	[ActUp_GamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move up a question of a game"
#elif L==4	// es
	""
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
	[ActDwnGamQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Move down a question of a game"
#elif L==4	// es
	""
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
	[ActSeeSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show one survey"
#elif L==4	// es
	""
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
	[ActAnsSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Answer a survey"
#elif L==4	// es
	""
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
	[ActFrmNewSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new survey"
#elif L==4	// es
	""
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
	[ActEdiOneSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit one survey"
#elif L==4	// es
	""
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
	[ActNewSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new survey"
#elif L==4	// es
	""
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
	[ActChgSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify data of an existing survey"
#elif L==4	// es
	""
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
	[ActReqRemSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a survey"
#elif L==4	// es
	""
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
	[ActRemSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove survey"
#elif L==4	// es
	""
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
	[ActReqRstSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the reset of answers of a survey"
#elif L==4	// es
	""
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
	[ActRstSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reset answers of survey"
#elif L==4	// es
	""
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
	[ActHidSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide survey"
#elif L==4	// es
	""
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
	[ActShoSvy] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show survey"
#elif L==4	// es
	""
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
	[ActEdiOneSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit a new question for a survey"
#elif L==4	// es
	""
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
	[ActRcvSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a question of a survey"
#elif L==4	// es
	""
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
	[ActReqRemSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a question of a survey"
#elif L==4	// es
	""
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
	[ActRemSvyQst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm the removal of a question of a survey"
#elif L==4	// es
	""
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
	[ActSeeAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of the institution (see or admin)"
#elif L==4	// es
	""
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
	[ActAdmShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the shared files zone of the institution"
#elif L==4	// es
	""
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
	[ActSeeAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of the centre (see or admin)"
#elif L==4	// es
	""
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
	[ActAdmShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the shared files zone of the centre"
#elif L==4	// es
	""
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
	[ActSeeAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of the degree (see or admin)"
#elif L==4	// es
	""
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
	[ActAdmShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the shared files zone of the degree"
#elif L==4	// es
	""
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
	[ActSeeAdmDocCrsGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Documents zone of the course (see or admin)"
#elif L==4	// es
	""
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
	[ActAdmTchCrsGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the administration of the files of the teachers' zone of the course or of a group"
#elif L==4	// es
	""
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
	[ActAdmShaCrsGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the administration of the files of the shared zone of the course or of a group"
#elif L==4	// es
	""
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
	[ActAdmAsgWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"One user sends works of the course"
#elif L==4	// es
	""
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
	[ActReqAsgWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"A teacher requests edition of works sent to the course"
#elif L==4	// es
	""
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
	[ActSeeAdmMrk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Marks zone (see or admin)"
#elif L==4	// es
	""
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
	[ActAdmBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the briefcase of private archives"
#elif L==4	// es
	""
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
	[ActChgToSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to see institution documents"
#elif L==4	// es
	""
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
	[ActSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActExpSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in institution documents"
#elif L==4	// es
	""
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
	[ActConSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in institution documents"
#elif L==4	// es
	""
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
	[ActZIPSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in institution documents"
#elif L==4	// es
	""
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
	[ActReqDatSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActDowSeeDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file in documents zone of the institution"
#elif L==4	// es
	""
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
	[ActChgToAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to administrate institution documents"
#elif L==4	// es
	""
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
	[ActAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the files of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActReqRemFilDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActRemFilDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActRemFolDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActCopDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActPasDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActRemTreDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActFrmCreDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActCreFolDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActCreLnkDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActRenFolDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActRcvFilDocInsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the institution using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilDocInsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the institution using the classic way"
#elif L==4	// es
	""
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
	[ActExpAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when administrating the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActConAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when administrating the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActZIPAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when administrating the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActShoDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show hidden folder or file of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActHidDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide folder or file of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActReqDatAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActChgDatAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActDowAdmDocIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the documents zone of the institution"
#elif L==4	// es
	""
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
	[ActReqRemFilShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of the institution"
#elif L==4	// es
	""
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
	[ActRemFilShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file of the institution"
#elif L==4	// es
	""
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
	[ActRemFolShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty shared folder of the institution"
#elif L==4	// es
	""
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
	[ActCopShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActPasShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActRemTreShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of shared zone of the institution"
#elif L==4	// es
	""
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
	[ActFrmCreShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActCreFolShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActCreLnkShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActRenFolShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActRcvFilShaInsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the institution using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilShaInsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the institution using the classic way"
#elif L==4	// es
	""
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
	[ActExpShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActConShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActZIPShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in shared zone of the institution"
#elif L==4	// es
	""
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
	[ActReqDatShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the shared zone of the institution"
#elif L==4	// es
	""
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
	[ActChgDatShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the shared zone of the institution"
#elif L==4	// es
	""
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
	[ActDowShaIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the shared zone of the institution"
#elif L==4	// es
	""
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
	[ActChgToSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to see centre documents"
#elif L==4	// es
	""
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
	[ActSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActExpSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in centre documents"
#elif L==4	// es
	""
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
	[ActConSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in centre documents"
#elif L==4	// es
	""
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
	[ActZIPSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in centre documents"
#elif L==4	// es
	""
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
	[ActReqDatSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActDowSeeDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file in documents zone of the centre"
#elif L==4	// es
	""
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
	[ActChgToAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to administrate centre documents"
#elif L==4	// es
	""
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
	[ActAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the files of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActReqRemFilDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActRemFilDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActRemFolDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActCopDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActPasDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActRemTreDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActFrmCreDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActCreFolDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActCreLnkDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActRenFolDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActRcvFilDocCtrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the centre using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilDocCtrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the centre using the classic way"
#elif L==4	// es
	""
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
	[ActExpAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when administrating the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActConAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when administrating the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActZIPAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when administrating the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActShoDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show hidden folder or file of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActHidDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide folder or file of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActReqDatAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActChgDatAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActDowAdmDocCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the documents zone of the centre"
#elif L==4	// es
	""
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
	[ActReqRemFilShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of the centre"
#elif L==4	// es
	""
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
	[ActRemFilShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file of the centre"
#elif L==4	// es
	""
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
	[ActRemFolShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty shared folder of the centre"
#elif L==4	// es
	""
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
	[ActCopShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActPasShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActRemTreShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of shared zone of the centre"
#elif L==4	// es
	""
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
	[ActFrmCreShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActCreFolShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActCreLnkShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActRenFolShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActRcvFilShaCtrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the centre using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilShaCtrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the centre using the classic way"
#elif L==4	// es
	""
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
	[ActExpShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActConShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActZIPShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in shared zone of the centre"
#elif L==4	// es
	""
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
	[ActReqDatShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the shared zone of the centre"
#elif L==4	// es
	""
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
	[ActChgDatShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the shared zone of the centre"
#elif L==4	// es
	""
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
	[ActDowShaCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the shared zone of the centre"
#elif L==4	// es
	""
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
	[ActChgToSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to see degree documents"
#elif L==4	// es
	""
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
	[ActSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActExpSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in degree documents"
#elif L==4	// es
	""
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
	[ActConSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in degree documents"
#elif L==4	// es
	""
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
	[ActZIPSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in degree documents"
#elif L==4	// es
	""
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
	[ActReqDatSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActDowSeeDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file in documents zone of the degree"
#elif L==4	// es
	""
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
	[ActChgToAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to administrate degree documents"
#elif L==4	// es
	""
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
	[ActAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the files of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActReqRemFilDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActRemFilDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActRemFolDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActCopDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActPasDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActRemTreDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActFrmCreDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActCreFolDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActCreLnkDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActRenFolDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActRcvFilDocDegDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the degree using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilDocDegCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the degree using the classic way"
#elif L==4	// es
	""
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
	[ActExpAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when administrating the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActConAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when administrating the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActZIPAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when administrating the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActShoDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show hidden folder or file of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActHidDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide folder or file of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActReqDatAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActChgDatAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActDowAdmDocDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the documents zone of the degree"
#elif L==4	// es
	""
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
	[ActReqRemFilShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of the degree"
#elif L==4	// es
	""
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
	[ActRemFilShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file of the degree"
#elif L==4	// es
	""
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
	[ActRemFolShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty shared folder of the degree"
#elif L==4	// es
	""
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
	[ActCopShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActPasShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActRemTreShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of shared zone of the degree"
#elif L==4	// es
	""
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
	[ActFrmCreShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActCreFolShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActCreLnkShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActRenFolShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActRcvFilShaDegDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the degree using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilShaDegCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the degree using the classic way"
#elif L==4	// es
	""
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
	[ActExpShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActConShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActZIPShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in shared zone of the degree"
#elif L==4	// es
	""
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
	[ActReqDatShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the shared zone of the degree"
#elif L==4	// es
	""
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
	[ActChgDatShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the shared zone of the degree"
#elif L==4	// es
	""
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
	[ActDowShaDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the shared zone of the degree"
#elif L==4	// es
	""
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
	[ActChgToSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to see course/group documents"
#elif L==4	// es
	""
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
	[ActSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of the course"
#elif L==4	// es
	""
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
	[ActExpSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in course documents"
#elif L==4	// es
	""
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
	[ActConSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in course documents"
#elif L==4	// es
	""
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
	[ActZIPSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in course documents"
#elif L==4	// es
	""
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
	[ActReqDatSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of the course"
#elif L==4	// es
	""
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
	[ActDowSeeDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file in documents zone of the course"
#elif L==4	// es
	""
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
	[ActSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See the files of the documents zone of a group"
#elif L==4	// es
	""
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
	[ActExpSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in group documents"
#elif L==4	// es
	""
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
	[ActConSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in group documents"
#elif L==4	// es
	""
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
	[ActZIPSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in group documents"
#elif L==4	// es
	""
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
	[ActReqDatSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of a group"
#elif L==4	// es
	""
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
	[ActDowSeeDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the documents zone of a group"
#elif L==4	// es
	""
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
	[ActChgToAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to administrate course/group documents"
#elif L==4	// es
	""
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
	[ActAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the files of the documents zone of the course"
#elif L==4	// es
	""
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
	[ActReqRemFilDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActRemFilDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActRemFolDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty the documents zone of the course"
#elif L==4	// es
	""
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
	[ActCopDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActPasDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActRemTreDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActFrmCreDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActCreFolDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActCreLnkDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActRenFolDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of the course"
#elif L==4	// es
	""
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
	[ActRcvFilDocCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the course using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilDocCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of the course using the classic way"
#elif L==4	// es
	""
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
	[ActExpAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when administrating the documents zone of the course"
#elif L==4	// es
	""
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
	[ActConAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when administrating the documents zone of the course"
#elif L==4	// es
	""
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
	[ActZIPAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when administrating the documents zone of the course"
#elif L==4	// es
	""
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
	[ActShoDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show hidden folder or file of the documents zone"
#elif L==4	// es
	""
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
	[ActHidDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide folder or file of the documents zone"
#elif L==4	// es
	""
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
	[ActReqDatAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone"
#elif L==4	// es
	""
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
	[ActChgDatAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the documents zone"
#elif L==4	// es
	""
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
	[ActDowAdmDocCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the documents zone"
#elif L==4	// es
	""
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
	[ActAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the files of the documents zone of a group"
#elif L==4	// es
	""
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
	[ActReqRemFilDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActRemFilDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActRemFolDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActCopDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActPasDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActRemTreDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActFrmCreDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActCreFolDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActCreLnkDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActRenFolDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in the documents zone of a group"
#elif L==4	// es
	""
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
	[ActRcvFilDocGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of a group using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilDocGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the documents zone of a group using the classic way"
#elif L==4	// es
	""
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
	[ActExpAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder when administrating the documents zone of a group"
#elif L==4	// es
	""
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
	[ActConAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder when administrating the documents zone of a group"
#elif L==4	// es
	""
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
	[ActZIPAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder when administrating the documents zone of a group"
#elif L==4	// es
	""
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
	[ActShoDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show hidden folder or file of download"
#elif L==4	// es
	""
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
	[ActHidDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide folder or file of download"
#elif L==4	// es
	""
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
	[ActReqDatAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the documents zone of a group"
#elif L==4	// es
	""
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
	[ActChgDatAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the documents zone of a group"
#elif L==4	// es
	""
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
	[ActDowAdmDocGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the documents zone of a group"
#elif L==4	// es
	""
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
	[ActChgToAdmTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to administrate teachers' files of the course/group"
#elif L==4	// es
	""
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
	[ActAdmTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the teachers' files zone of the course"
#elif L==4	// es
	""
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
	[ActReqRemFilTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a teachers' file of the course"
#elif L==4	// es
	""
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
	[ActRemFilTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a teachers' file of the course"
#elif L==4	// es
	""
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
	[ActRemFolTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty teachers' folder of the course"
#elif L==4	// es
	""
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
	[ActCopTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActPasTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActRemTreTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActFrmCreTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActCreFolTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActCreLnkTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActRenFolTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActRcvFilTchCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in teachers' zone of the course using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilTchCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in teachers' zone of the course using the classic way"
#elif L==4	// es
	""
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
	[ActExpTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActConTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActZIPTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActReqDatTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActChgDatTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActDowTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the teachers' zone of the course"
#elif L==4	// es
	""
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
	[ActAdmTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the teachers' files zone of a group"
#elif L==4	// es
	""
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
	[ActReqRemFilTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a teachers' file of a group"
#elif L==4	// es
	""
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
	[ActRemFilTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a teachers' file of a group"
#elif L==4	// es
	""
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
	[ActRemFolTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty teachers' folder of a group"
#elif L==4	// es
	""
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
	[ActCopTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActPasTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActRemTreTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActFrmCreTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a folder or file in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActCreFolTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActCreLnkTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActRenFolTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActRcvFilTchGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in teachers' zone of a group using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilTchGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in teachers' zone of a group using the classic way"
#elif L==4	// es
	""
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
	[ActExpTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActConTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActZIPTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActReqDatTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActChgDatTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActDowTchGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the teachers' zone of a group"
#elif L==4	// es
	""
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
	[ActChgToAdmSha] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to administrate shared files of the course/group"
#elif L==4	// es
	""
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
	[ActAdmShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the shared files zone of the course"
#elif L==4	// es
	""
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
	[ActReqRemFilShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of the course"
#elif L==4	// es
	""
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
	[ActRemFilShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file of the course"
#elif L==4	// es
	""
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
	[ActRemFolShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a empty shared folder of the course"
#elif L==4	// es
	""
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
	[ActCopShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in shared zone of the course"
#elif L==4	// es
	""
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
	[ActPasShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in shared zone of the course"
#elif L==4	// es
	""
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
	[ActRemTreShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of shared zone of the course"
#elif L==4	// es
	""
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
	[ActFrmCreShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in shared zone of the course"
#elif L==4	// es
	""
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
	[ActCreFolShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in shared zone of the course"
#elif L==4	// es
	""
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
	[ActCreLnkShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in shared zone of the course"
#elif L==4	// es
	""
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
	[ActRenFolShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in shared zone of the course"
#elif L==4	// es
	""
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
	[ActRcvFilShaCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the course using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilShaCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of the course using the classic way"
#elif L==4	// es
	""
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
	[ActExpShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in shared zone of the course"
#elif L==4	// es
	""
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
	[ActConShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in shared zone of the course"
#elif L==4	// es
	""
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
	[ActZIPShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in shared zone of the course"
#elif L==4	// es
	""
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
	[ActReqDatShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the shared zone of the course"
#elif L==4	// es
	""
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
	[ActChgDatShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the shared zone of the course"
#elif L==4	// es
	""
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
	[ActDowShaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the shared zone of the course"
#elif L==4	// es
	""
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
	[ActAdmShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the shared files zone of a group"
#elif L==4	// es
	""
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
	[ActReqRemFilShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a shared file of a group"
#elif L==4	// es
	""
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
	[ActRemFilShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a shared file of a group"
#elif L==4	// es
	""
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
	[ActRemFolShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty shared of a group"
#elif L==4	// es
	""
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
	[ActCopShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in shared zone of a group"
#elif L==4	// es
	""
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
	[ActPasShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in shared zone of a group"
#elif L==4	// es
	""
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
	[ActRemTreShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of shared zone of a group"
#elif L==4	// es
	""
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
	[ActFrmCreShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in shared zone of a group"
#elif L==4	// es
	""
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
	[ActCreFolShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in shared zone of a group"
#elif L==4	// es
	""
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
	[ActCreLnkShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in shared zone of a group"
#elif L==4	// es
	""
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
	[ActRenFolShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in shared zone of a group"
#elif L==4	// es
	""
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
	[ActRcvFilShaGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of a group using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilShaGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in shared zone of a group using the classic way"
#elif L==4	// es
	""
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
	[ActExpShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in shared zone of a group"
#elif L==4	// es
	""
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
	[ActConShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in shared zone of a group"
#elif L==4	// es
	""
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
	[ActZIPShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in shared zone of a group"
#elif L==4	// es
	""
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
	[ActReqDatShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the shared zone of a group"
#elif L==4	// es
	""
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
	[ActChgDatShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of the shared zone of a group"
#elif L==4	// es
	""
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
	[ActDowShaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of the shared zone of a group"
#elif L==4	// es
	""
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
	[ActAdmAsgWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit all the works sent to the course"
#elif L==4	// es
	""
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
	[ActReqRemFilAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file of assignments from a user"
#elif L==4	// es
	""
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
	[ActRemFilAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file of assignments from a user"
#elif L==4	// es
	""
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
	[ActRemFolAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActCopAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in assignments of a user"
#elif L==4	// es
	""
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
	[ActPasAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActRemTreAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a not empty folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActFrmCreAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new folder or file of assignments of a user"
#elif L==4	// es
	""
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
	[ActCreFolAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActCreLnkAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link of assignments of a user"
#elif L==4	// es
	""
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
	[ActRenFolAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActRcvFilAsgUsrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a new file of assignments of a user using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilAsgUsrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a new file of assignments of a user using the classic way"
#elif L==4	// es
	""
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
	[ActExpAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActConAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActZIPAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder of assignments of a user"
#elif L==4	// es
	""
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
	[ActReqDatAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of assignments of a user"
#elif L==4	// es
	""
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
	[ActChgDatAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of assignments of a user"
#elif L==4	// es
	""
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
	[ActDowAsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of assignments of a user"
#elif L==4	// es
	""
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
	[ActReqRemFilWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file of works from a user"
#elif L==4	// es
	""
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
	[ActRemFilWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file of works from a user"
#elif L==4	// es
	""
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
	[ActRemFolWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder of works of a user"
#elif L==4	// es
	""
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
	[ActCopWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in works of a user"
#elif L==4	// es
	""
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
	[ActPasWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder of works of a user"
#elif L==4	// es
	""
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
	[ActRemTreWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a not empty folder of works of a user"
#elif L==4	// es
	""
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
	[ActFrmCreWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new folder or file of works of a user"
#elif L==4	// es
	""
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
	[ActCreFolWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder of works of a user"
#elif L==4	// es
	""
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
	[ActCreLnkWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link of works of a user"
#elif L==4	// es
	""
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
	[ActRenFolWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder of works of a user"
#elif L==4	// es
	""
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
	[ActRcvFilWrkUsrDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a new file of works of a user using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilWrkUsrCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a new file of works of a user using the classic way"
#elif L==4	// es
	""
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
	[ActExpWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder of works of a user"
#elif L==4	// es
	""
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
	[ActConWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder of works of a user"
#elif L==4	// es
	""
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
	[ActZIPWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder of works of a user"
#elif L==4	// es
	""
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
	[ActReqDatWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of works of a user"
#elif L==4	// es
	""
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
	[ActChgDatWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of works of a user"
#elif L==4	// es
	""
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
	[ActDowWrkUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of works of a user"
#elif L==4	// es
	""
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
	[ActReqRemFilAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file of assignments in the course"
#elif L==4	// es
	""
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
	[ActRemFilAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file of assignments in the course"
#elif L==4	// es
	""
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
	[ActRemFolAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder of assignments in the course"
#elif L==4	// es
	""
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
	[ActCopAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in assignments in the course"
#elif L==4	// es
	""
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
	[ActPasAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder of assignments in the course"
#elif L==4	// es
	""
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
	[ActRemTreAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a not empty folder of assignments in the course"
#elif L==4	// es
	""
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
	[ActFrmCreAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new folder or file of assignments in the course"
#elif L==4	// es
	""
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
	[ActCreFolAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder of assignments in the course"
#elif L==4	// es
	""
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
	[ActCreLnkAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link of assignments in the course"
#elif L==4	// es
	""
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
	[ActRenFolAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder of assignments in the course"
#elif L==4	// es
	""
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
	[ActRcvFilAsgCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file of assignments in the course using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilAsgCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file of assignments in the course using the classic way"
#elif L==4	// es
	""
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
	[ActExpAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder of assignments in a course"
#elif L==4	// es
	""
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
	[ActConAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder of assignments in a course"
#elif L==4	// es
	""
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
	[ActZIPAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder of assignments in a course"
#elif L==4	// es
	""
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
	[ActReqDatAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of assignments in a course"
#elif L==4	// es
	""
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
	[ActChgDatAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of assignments in a course"
#elif L==4	// es
	""
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
	[ActDowAsgCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of assignments in a course"
#elif L==4	// es
	""
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
	[ActReqRemFilWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file of works in the course"
#elif L==4	// es
	""
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
	[ActRemFilWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file of works in the course"
#elif L==4	// es
	""
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
	[ActRemFolWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an empty folder of works in the course"
#elif L==4	// es
	""
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
	[ActCopWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set origin of copy in works in the course"
#elif L==4	// es
	""
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
	[ActPasWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a file or folder of works in the course"
#elif L==4	// es
	""
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
	[ActRemTreWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a not empty folder of works in the course"
#elif L==4	// es
	""
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
	[ActFrmCreWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new folder or file of works in the course"
#elif L==4	// es
	""
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
	[ActCreFolWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder of works in the course"
#elif L==4	// es
	""
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
	[ActCreLnkWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link of works in the course"
#elif L==4	// es
	""
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
	[ActRenFolWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder of works in the course"
#elif L==4	// es
	""
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
	[ActRcvFilWrkCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file of works in the course using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilWrkCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file of works in the course using the classic way"
#elif L==4	// es
	""
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
	[ActExpWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder of works in a course"
#elif L==4	// es
	""
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
	[ActConWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder of works in a course"
#elif L==4	// es
	""
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
	[ActZIPWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder of works in a course"
#elif L==4	// es
	""
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
	[ActReqDatWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of works in a course"
#elif L==4	// es
	""
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
	[ActChgDatWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file of works in a course"
#elif L==4	// es
	""
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
	[ActDowWrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file of works in a course"
#elif L==4	// es
	""
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
	[ActChgToSeeMrk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to see marks in course/group"
#elif L==4	// es
	""
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
	[ActSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See marks in course"
#elif L==4	// es
	""
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
	[ActExpSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in marks in course"
#elif L==4	// es
	""
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
	[ActConSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in marks in course"
#elif L==4	// es
	""
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
	[ActReqDatSeeMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the marks zone of the course"
#elif L==4	// es
	""
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
	[ActSeeMyMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Individualized access to marks in course"
#elif L==4	// es
	""
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
	[ActSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See marks in group"
#elif L==4	// es
	""
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
	[ActExpSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in marks in group"
#elif L==4	// es
	""
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
	[ActConSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in marks in group"
#elif L==4	// es
	""
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
	[ActReqDatSeeMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file of the marks zone of a group"
#elif L==4	// es
	""
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
	[ActSeeMyMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Individualized access to marks in group"
#elif L==4	// es
	""
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
	[ActChgToAdmMrk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change to admin marks in course/group"
#elif L==4	// es
	""
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
	[ActAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the files of the zone of marks in course"
#elif L==4	// es
	""
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
	[ActReqRemFilMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file of marks in course"
#elif L==4	// es
	""
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
	[ActRemFilMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file of marks in course"
#elif L==4	// es
	""
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
	[ActRemFolMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty of marks in course"
#elif L==4	// es
	""
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
	[ActCopMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in marks in course"
#elif L==4	// es
	""
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
	[ActPasMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in marks in course"
#elif L==4	// es
	""
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
	[ActRemTreMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of marks in course"
#elif L==4	// es
	""
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
	[ActFrmCreMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in marks in course"
#elif L==4	// es
	""
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
	[ActCreFolMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in marks in course"
#elif L==4	// es
	""
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
	[ActRenFolMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in marks in course"
#elif L==4	// es
	""
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
	[ActRcvFilMrkCrsDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file of marks in course using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilMrkCrsCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file of marks in course using the classic way"
#elif L==4	// es
	""
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
	[ActExpAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in marks administration in course"
#elif L==4	// es
	""
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
	[ActConAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in marks administration in course"
#elif L==4	// es
	""
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
	[ActZIPAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in marks administration in course"
#elif L==4	// es
	""
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
	[ActShoMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show hidden folder or file of the marks administration in course"
#elif L==4	// es
	""
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
	[ActHidMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide folder or file of the marks administration in course"
#elif L==4	// es
	""
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
	[ActReqDatAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file in marks administration in course"
#elif L==4	// es
	""
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
	[ActChgDatAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file in marks administration in course"
#elif L==4	// es
	""
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
	[ActDowAdmMrkCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file in marks administration in course"
#elif L==4	// es
	""
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
	[ActChgNumRowHeaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the number of rows of cabecera of a file of marks in course"
#elif L==4	// es
	""
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
	[ActChgNumRowFooCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the number of rows of pie of a file of marks in course"
#elif L==4	// es
	""
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
	[ActAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Admin the files of the zone of marks in group"
#elif L==4	// es
	""
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
	[ActReqRemFilMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file of marks in group"
#elif L==4	// es
	""
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
	[ActRemFilMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file of marks in group"
#elif L==4	// es
	""
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
	[ActRemFolMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty of marks in group"
#elif L==4	// es
	""
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
	[ActCopMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in marks in group"
#elif L==4	// es
	""
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
	[ActPasMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in marks in group"
#elif L==4	// es
	""
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
	[ActRemTreMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of marks in group"
#elif L==4	// es
	""
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
	[ActFrmCreMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in marks in group"
#elif L==4	// es
	""
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
	[ActCreFolMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in marks in group"
#elif L==4	// es
	""
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
	[ActRenFolMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder in marks in group"
#elif L==4	// es
	""
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
	[ActRcvFilMrkGrpDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file to marks in group using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilMrkGrpCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file to marks in group using the classic way"
#elif L==4	// es
	""
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
	[ActExpAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in marks administration in group"
#elif L==4	// es
	""
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
	[ActConAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in marks administration in group"
#elif L==4	// es
	""
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
	[ActZIPAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in marks administration in group"
#elif L==4	// es
	""
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
	[ActShoMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show hidden folder or file of the marks administration in group"
#elif L==4	// es
	""
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
	[ActHidMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide folder or file of the marks administration in group"
#elif L==4	// es
	""
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
	[ActReqDatAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file in marks administration in group"
#elif L==4	// es
	""
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
	[ActChgDatAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file in marks administration in group"
#elif L==4	// es
	""
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
	[ActDowAdmMrkGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file in marks administration in group"
#elif L==4	// es
	""
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
	[ActChgNumRowHeaGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the number of rows of cabecera of a file of marks in group"
#elif L==4	// es
	""
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
	[ActChgNumRowFooGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the number of rows of pie of a file of marks in group"
#elif L==4	// es
	""
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
	[ActReqRemFilBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a file of the briefcase"
#elif L==4	// es
	""
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
	[ActRemFilBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a file of the briefcase"
#elif L==4	// es
	""
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
	[ActRemFolBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder empty of the briefcase"
#elif L==4	// es
	""
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
	[ActCopBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Set source of copy in the briefcase"
#elif L==4	// es
	""
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
	[ActPasBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a folder or file in the briefcase"
#elif L==4	// es
	""
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
	[ActRemTreBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a folder no empty of the briefcase"
#elif L==4	// es
	""
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
	[ActFrmCreBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to crear a folder or file in the briefcase"
#elif L==4	// es
	""
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
	[ActCreFolBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new folder in the briefcase"
#elif L==4	// es
	""
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
	[ActCreLnkBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new link in the briefcase"
#elif L==4	// es
	""
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
	[ActRenFolBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a folder of the briefcase"
#elif L==4	// es
	""
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
	[ActRcvFilBrfDZ] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the briefcase using Dropzone.js"
#elif L==4	// es
	""
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
	[ActRcvFilBrfCla] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a file in the briefcase using the classic way"
#elif L==4	// es
	""
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
	[ActExpBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Expand a folder in briefcase"
#elif L==4	// es
	""
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
	[ActConBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Contract a folder in briefcase"
#elif L==4	// es
	""
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
	[ActZIPBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compress a folder in briefcase"
#elif L==4	// es
	""
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
	[ActReqDatBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for metadata of a file in the briefcase"
#elif L==4	// es
	""
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
	[ActChgDatBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change metadata of a file in the briefcase"
#elif L==4	// es
	""
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
	[ActDowBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Download a file in the briefcase"
#elif L==4	// es
	""
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
	[ActReqRemOldBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask for removing old files in the briefcase"
#elif L==4	// es
	""
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
	[ActRemOldBrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove old files in the briefcase"
#elif L==4	// es
	""
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
	[ActReqSelGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the selection of groups of students"
#elif L==4	// es
	""
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
	[ActLstStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show class photo or list of students"
#elif L==4	// es
	""
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
	[ActLstTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show class photo or list of teachers"
#elif L==4	// es
	""
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
	[ActLstOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show class photo or list of guests"
#elif L==4	// es
	""
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
	[ActSeeAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show attendance events"
#elif L==4	// es
	""
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
	[ActReqSignUp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Apply for my enrolment"
#elif L==4	// es
	""
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
	[ActSeeSignUpReq] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show pending requests for inscription in the current course"
#elif L==4	// es
	""
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
	[ActLstCon] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List connected users"
#elif L==4	// es
	""
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
	[ActChgGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my groups"
#elif L==4	// es
	""
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
	[ActReqEdiGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the edition of groups"
#elif L==4	// es
	""
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
	[ActNewGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a type of group"
#elif L==4	// es
	""
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
	[ActReqRemGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a type of group"
#elif L==4	// es
	""
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
	[ActRemGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a type of group of students"
#elif L==4	// es
	""
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
	[ActRenGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request renaming of a type of group of students"
#elif L==4	// es
	""
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
	[ActChgMdtGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change if it is mandatory to register in groups of a type"
#elif L==4	// es
	""
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
	[ActChgMulGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change if it is possible to register in multiple groups of a type"
#elif L==4	// es
	""
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
	[ActChgTimGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change when the groups of a type will be open"
#elif L==4	// es
	""
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
	[ActNewGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a group"
#elif L==4	// es
	""
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
	[ActReqRemGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a group"
#elif L==4	// es
	""
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
	[ActRemGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a group"
#elif L==4	// es
	""
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
	[ActOpeGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Open a group"
#elif L==4	// es
	""
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
	[ActCloGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Close a group"
#elif L==4	// es
	""
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
	[ActEnaFilZonGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable filezones of a group"
#elif L==4	// es
	""
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
	[ActDisFilZonGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable filezones of a group"
#elif L==4	// es
	""
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
	[ActChgGrpTyp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the type of group of a group"
#elif L==4	// es
	""
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
	[ActRenGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Rename a group"
#elif L==4	// es
	""
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
	[ActChgGrpRoo] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the room of a group"
#elif L==4	// es
	"Cambiar la sala de un grupo"
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
	[ActChgMaxStdGrp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the maximum number of students in a group"
#elif L==4	// es
	""
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
	[ActLstGst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List main data of administrators"
#elif L==4	// es
	""
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
	[ActPrnGstPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the class photo of guests ready to be printed"
#elif L==4	// es
	""
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
	[ActPrnStdPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the class photo of students ready to be printed"
#elif L==4	// es
	""
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
	[ActPrnTchPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the class photo of teachers ready to be printed"
#elif L==4	// es
	""
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
	[ActLstGstAll] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List in another window the full data of guests"
#elif L==4	// es
	""
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
	[ActLstStdAll] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List in another window the full data of students"
#elif L==4	// es
	""
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
	[ActLstTchAll] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List in another window the full data of teachers"
#elif L==4	// es
	""
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
	[ActSeeRecOneStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show record of one selected student"
#elif L==4	// es
	""
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
	[ActSeeRecOneTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show record of one selected teacher"
#elif L==4	// es
	""
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
	[ActDoActOnSevGst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Do action on several selected guests"
#elif L==4	// es
	""
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
	[ActDoActOnSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Do action on several selected students"
#elif L==4	// es
	""
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
	[ActDoActOnSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Do action on several selected teachers"
#elif L==4	// es
	""
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
	[ActSeeRecSevGst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show records of several selected guests"
#elif L==4	// es
	""
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
	[ActSeeRecSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show records of several selected students"
#elif L==4	// es
	""
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
	[ActSeeRecSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show records of several selected teachers"
#elif L==4	// es
	""
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
	[ActPrnRecSevGst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show records of several selected guests ready to be printed"
#elif L==4	// es
	""
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
	[ActPrnRecSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show records of several selected students ready to be printed"
#elif L==4	// es
	""
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
	[ActPrnRecSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show records of several selected teachers ready to be printed"
#elif L==4	// es
	""
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
	[ActRcvRecOthUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update record fields of a student in this course"
#elif L==4	// es
	""
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
	[ActEdiRecFie] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the edition of record fields of students"
#elif L==4	// es
	""
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
	[ActNewFie] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the creation of a record field"
#elif L==4	// es
	""
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
	[ActReqRemFie] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of record field"
#elif L==4	// es
	""
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
	[ActRemFie] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a record field"
#elif L==4	// es
	""
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
	[ActRenFie] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request renaming of record field"
#elif L==4	// es
	""
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
	[ActChgRowFie] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change in number of lines of form of a record field"
#elif L==4	// es
	""
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
	[ActChgVisFie] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request change in visibility of a record field"
#elif L==4	// es
	""
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
	[ActRcvRecCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and update fields of my record in this course"
#elif L==4	// es
	""
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
	[ActFrmLogInUsrAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Put form to log in to show another user's public agenda"
#elif L==4	// es
	""
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
	[ActLogInUsrAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show another user's public agenda after log in"
#elif L==4	// es
	""
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
	[ActLogInUsrAgdLan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show another user's public agenda after change language"
#elif L==4	// es
	""
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
	[ActSeeUsrAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show another user's public agenda"
#elif L==4	// es
	""
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
	[ActReqEnrSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the enrolment/removing of several students to / from current course"
#elif L==4	// es
	""
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
	[ActReqEnrSevNET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the enrolment/removing of several non-editing teachers to / from current course"
#elif L==4	// es
	""
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
	[ActReqEnrSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the enrolment/removing of several teachers to / from current course"
#elif L==4	// es
	""
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
	[ActReqLstUsrAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request listing of attendance of several students to several events"
#elif L==4	// es
	""
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
	[ActSeeLstMyAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List my attendance as student to several events"
#elif L==4	// es
	""
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
	[ActPrnLstMyAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print my attendance as student to several events"
#elif L==4	// es
	""
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
	[ActSeeLstUsrAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List attendance of several students to several events"
#elif L==4	// es
	""
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
	[ActPrnLstUsrAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Print attendance of several students to several events"
#elif L==4	// es
	""
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
	[ActFrmNewAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new attendance event"
#elif L==4	// es
	""
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
	[ActEdiOneAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit one attendance event"
#elif L==4	// es
	""
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
	[ActNewAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new attendance event"
#elif L==4	// es
	""
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
	[ActChgAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify data of an existing attendance event"
#elif L==4	// es
	""
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
	[ActReqRemAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of an attendance event"
#elif L==4	// es
	""
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
	[ActRemAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove attendance event"
#elif L==4	// es
	""
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
	[ActHidAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide attendance event"
#elif L==4	// es
	""
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
	[ActShoAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show attendance event"
#elif L==4	// es
	""
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
	[ActSeeOneAtt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List students who attended to an event"
#elif L==4	// es
	""
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
	[ActRecAttStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Save students who attended to an event and comments"
#elif L==4	// es
	""
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
	[ActRecAttMe] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Save my comments as student in an attendance event"
#elif L==4	// es
	""
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
	[ActSignUp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Apply for my enrolment"
#elif L==4	// es
	""
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
	[ActUpdSignUpReq] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update pending requests for inscription in the current course"
#elif L==4	// es
	""
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
	[ActReqRejSignUp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask if reject the enrolment of a user in a course"
#elif L==4	// es
	""
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
	[ActRejSignUp] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reject the enrolment of a user in a course"
#elif L==4	// es
	""
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
	[ActReqMdfOneOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request a user's ID for enrolment/removing"
#elif L==4	// es
	""
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
	[ActReqMdfOneStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request a student's ID for enrolment/removing"
#elif L==4	// es
	""
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
	[ActReqMdfOneTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request a teacher's ID for enrolment/removing"
#elif L==4	// es
	""
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
	[ActReqMdfOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removing of a user"
#elif L==4	// es
	""
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
	[ActReqMdfStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removing of a student"
#elif L==4	// es
	""
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
	[ActReqMdfNET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removing of a non-editing teacher"
#elif L==4	// es
	""
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
	[ActReqMdfTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request enrolment/removing of a teacher"
#elif L==4	// es
	""
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
	[ActReqOthPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send the photo of another user"
#elif L==4	// es
	""
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
	[ActReqStdPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send the photo of another user"
#elif L==4	// es
	""
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
	[ActReqTchPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send the photo of another user"
#elif L==4	// es
	""
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
	[ActDetOthPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive other user's photo and detect faces on it"
#elif L==4	// es
	""
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
	[ActDetStdPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive other user's photo and detect faces on it"
#elif L==4	// es
	""
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
	[ActDetTchPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive other user's photo and detect faces on it"
#elif L==4	// es
	""
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
	[ActUpdOthPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update other user's photo"
#elif L==4	// es
	""
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
	[ActUpdStdPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update other user's photo"
#elif L==4	// es
	""
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
	[ActUpdTchPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update other user's photo"
#elif L==4	// es
	""
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
	[ActReqRemOthPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of other user's photo"
#elif L==4	// es
	""
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
	[ActReqRemStdPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of student's photo"
#elif L==4	// es
	""
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
	[ActReqRemTchPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of teacher's photo"
#elif L==4	// es
	""
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
	[ActRemOthPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove other user's photo"
#elif L==4	// es
	""
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
	[ActRemStdPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove student's photo"
#elif L==4	// es
	""
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
	[ActRemTchPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove teacher's photo"
#elif L==4	// es
	""
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
	[ActCreOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new guest"
#elif L==4	// es
	""
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
	[ActCreStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new student"
#elif L==4	// es
	""
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
	[ActCreNET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new non-editing teacher"
#elif L==4	// es
	""
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
	[ActCreTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new teacher"
#elif L==4	// es
	""
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
	[ActUpdOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update another user's data and groups"
#elif L==4	// es
	""
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
	[ActUpdStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update another student's data and groups"
#elif L==4	// es
	""
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
	[ActUpdNET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update another non-editing teacher's data and groups"
#elif L==4	// es
	""
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
	[ActUpdTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update another teacher's data and groups"
#elif L==4	// es
	""
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
	[ActReqAccEnrStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm acceptation / refusion of enrolment as student in current course"
#elif L==4	// es
	""
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
	[ActReqAccEnrNET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm acceptation / refusion of enrolment as non-editing teacher in current course"
#elif L==4	// es
	""
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
	[ActReqAccEnrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm acceptation / refusion of enrolment as teacher in current course"
#elif L==4	// es
	""
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
	[ActAccEnrStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Accept enrolment as student in current course"
#elif L==4	// es
	""
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
	[ActAccEnrNET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Accept enrolment as non-editing teacher in current course"
#elif L==4	// es
	""
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
	[ActAccEnrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Accept enrolment as teacher in current course"
#elif L==4	// es
	""
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
	[ActRemMe_Std] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reject enrolment as student in current course"
#elif L==4	// es
	""
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
	[ActRemMe_NET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reject enrolment as non-editing teacher in current course"
#elif L==4	// es
	""
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
	[ActRemMe_Tch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reject enrolment as teacher in current course"
#elif L==4	// es
	""
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
	[ActNewAdmIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Register an administrador in this institution"
#elif L==4	// es
	""
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
	[ActRemAdmIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an administrador of this institution"
#elif L==4	// es
	""
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
	[ActNewAdmCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Register an administrador in this centre"
#elif L==4	// es
	""
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
	[ActRemAdmCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an administrador of this centre"
#elif L==4	// es
	""
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
	[ActNewAdmDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Register an administrador in this degree"
#elif L==4	// es
	""
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
	[ActRemAdmDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an administrador of this degree"
#elif L==4	// es
	""
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
	[ActRcvFrmEnrSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a form with IDs of students to be registered/removed to/from current course"
#elif L==4	// es
	""
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
	[ActRcvFrmEnrSevNET] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a form with IDs of non-editing teachers to be registered/removed to/from current course"
#elif L==4	// es
	""
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
	[ActRcvFrmEnrSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a form with IDs of teachers to be registered/removed to/from current course"
#elif L==4	// es
	""
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
	[ActCnfID_Oth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm another user's ID"
#elif L==4	// es
	""
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
	[ActCnfID_Std] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm another user's ID"
#elif L==4	// es
	""
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
	[ActCnfID_Tch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm another user's ID"
#elif L==4	// es
	""
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
	[ActFrmAccOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show forms to the change another user's account"
#elif L==4	// es
	""
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
	[ActFrmAccStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show forms to the change another user's account"
#elif L==4	// es
	""
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
	[ActFrmAccTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show forms to the change another user's account"
#elif L==4	// es
	""
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
	[ActRemOldNicOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of another user's old nicknames"
#elif L==4	// es
	""
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
	[ActRemOldNicStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of another user's old nicknames"
#elif L==4	// es
	""
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
	[ActRemOldNicTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of another user's old nicknames"
#elif L==4	// es
	""
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
	[ActChgNicOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change another user's nickname"
#elif L==4	// es
	""
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
	[ActChgNicStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change another user's nickname"
#elif L==4	// es
	""
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
	[ActChgNicTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change another user's nickname"
#elif L==4	// es
	""
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
	[ActRemID_Oth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of the IDs of another user"
#elif L==4	// es
	""
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
	[ActRemID_Std] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of the IDs of another user"
#elif L==4	// es
	""
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
	[ActRemID_Tch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of the IDs of another user"
#elif L==4	// es
	""
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
	[ActNewID_Oth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new user's ID for another user"
#elif L==4	// es
	""
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
	[ActNewID_Std] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new user's ID for another user"
#elif L==4	// es
	""
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
	[ActNewID_Tch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new user's ID for another user"
#elif L==4	// es
	""
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
	[ActChgPwdOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the password of another user"
#elif L==4	// es
	""
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
	[ActChgPwdStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the password of another user"
#elif L==4	// es
	""
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
	[ActChgPwdTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the password of another user"
#elif L==4	// es
	""
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
	[ActRemMaiOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of the email of another user"
#elif L==4	// es
	""
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
	[ActRemMaiStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of the email of another user"
#elif L==4	// es
	""
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
	[ActRemMaiTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of the email of another user"
#elif L==4	// es
	""
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
	[ActNewMaiOth] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new user's email for another user"
#elif L==4	// es
	""
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
	[ActNewMaiStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new user's email for another user"
#elif L==4	// es
	""
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
	[ActNewMaiTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new user's email for another user"
#elif L==4	// es
	""
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
	[ActRemStdCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a student from the current course"
#elif L==4	// es
	""
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
	[ActRemNETCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a non-editing teacher from the current course"
#elif L==4	// es
	""
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
	[ActRemTchCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a teacher from the current course"
#elif L==4	// es
	""
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
	[ActRemUsrGbl] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Eliminate completely a user from the platform"
#elif L==4	// es
	""
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
	[ActReqRemAllStdCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of all the students from the current course"
#elif L==4	// es
	""
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
	[ActRemAllStdCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove all the students from the current course"
#elif L==4	// es
	""
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
	[ActReqRemOldUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the complete elimination of old users"
#elif L==4	// es
	""
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
	[ActRemOldUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Eliminate completely old users"
#elif L==4	// es
	""
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
	[ActLstDupUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List possible duplicate users"
#elif L==4	// es
	""
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
	[ActLstSimUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List users similar to a given one (possible duplicates)"
#elif L==4	// es
	""
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
	[ActRemDupUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove user from list of possible duplicate users"
#elif L==4	// es
	""
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
	[ActReqFolSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request follow several students"
#elif L==4	// es
	""
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
	[ActReqFolSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request follow several teachers"
#elif L==4	// es
	""
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
	[ActReqUnfSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request unfollow several students"
#elif L==4	// es
	""
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
	[ActReqUnfSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request unfollow several teachers"
#elif L==4	// es
	""
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
	[ActFolSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Follow several students"
#elif L==4	// es
	""
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
	[ActFolSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Follow several teachers"
#elif L==4	// es
	""
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
	[ActUnfSevStd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unfollow several students"
#elif L==4	// es
	""
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
	[ActUnfSevTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unfollow several teachers"
#elif L==4	// es
	""
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
	[ActSeeAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show global announcements"
#elif L==4	// es
	""
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
	[ActSeeAllNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show all notices"
#elif L==4	// es
	""
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
	[ActSeeFor] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the level superior of the forums"
#elif L==4	// es
	""
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
	[ActSeeChtRms] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the chat rooms"
#elif L==4	// es
	""
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
	[ActSeeMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages received from other users"
#elif L==4	// es
	"Ver mensajes recibidos de otros usuarios"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mensajes recibidos de otros usuarios"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqMsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Write message to other users"
#elif L==4	// es
	"Redactar mensaje a otros usuarios"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Redactar mensaje a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeRcvMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages received from other users"
#elif L==4	// es
	"Ver mensajes recibidos de otros usuarios"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mensajes recibidos de otros usuarios"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActSeeSntMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages sent to other users"
#elif L==4	// es
	"Ver mensajes enviados a otros usuarios"
#elif L==5	// fr
	""			// Besoin de traduction
#elif L==6	// gn
	"Ver mensajes enviados a otros usuarios"	// Okoteve traducción
#elif L==7	// it
	""			// Bisogno di traduzione
#elif L==8	// pl
	""			// Potrzebujesz tlumaczenie
#elif L==9	// pt
	""			// Precisa de tradução
#endif
	,
	[ActReqMaiUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select users to send an email to them"
#elif L==4	// es
	""
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
	[ActWriAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to create a new global announcement"
#elif L==4	// es
	""
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
	[ActRcvAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and create a new global announcement"
#elif L==4	// es
	""
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
	[ActHidAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a global announcement that was active"
#elif L==4	// es
	""
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
	[ActRevAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reveal a global announcement that was hidden"
#elif L==4	// es
	""
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
	[ActRemAnn] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove global announcement"
#elif L==4	// es
	""
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
	[ActSeeOneNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show (expand) a notice"
#elif L==4	// es
	""
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
	[ActWriNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Write a new notice"
#elif L==4	// es
	""
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
	[ActRcvNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive and create a new notice"
#elif L==4	// es
	""
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
	[ActHidNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide a notice that was active"
#elif L==4	// es
	""
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
	[ActRevNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Reveal a notice that was hidden"
#elif L==4	// es
	""
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
	[ActReqRemNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request removal of a notice"
#elif L==4	// es
	""
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
	[ActRemNot] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a notice"
#elif L==4	// es
	""
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
	[ActSeeForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of users of the course"
#elif L==4	// es
	""
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
	[ActSeeForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActSeeForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActSeeForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActSeeForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of users of the centre"
#elif L==4	// es
	""
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
	[ActSeeForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of teachers of the centre"
#elif L==4	// es
	""
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
	[ActSeeForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActSeeForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActSeeForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of users general"
#elif L==4	// es
	""
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
	[ActSeeForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of teachers general"
#elif L==4	// es
	""
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
	[ActSeeForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActSeeForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show top level of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActSeePstForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of users of the course"
#elif L==4	// es
	""
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
	[ActSeePstForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActSeePstForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of users of the degree"
#elif L==4	// es
	""
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
	[ActSeePstForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActSeePstForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of users of the centre"
#elif L==4	// es
	""
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
	[ActSeePstForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of teachers of the centre"
#elif L==4	// es
	""
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
	[ActSeePstForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of users of the institution"
#elif L==4	// es
	""
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
	[ActSeePstForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActSeePstForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of users general"
#elif L==4	// es
	""
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
	[ActSeePstForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of teachers general"
#elif L==4	// es
	""
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
	[ActSeePstForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of users of the platform"
#elif L==4	// es
	""
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
	[ActSeePstForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the messages of a thread of the forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActRcvThrForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of users of the course"
#elif L==4	// es
	""
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
	[ActRcvThrForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActRcvThrForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActRcvThrForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActRcvThrForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of users of centre"
#elif L==4	// es
	""
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
	[ActRcvThrForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActRcvThrForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActRcvThrForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActRcvThrForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of users general"
#elif L==4	// es
	""
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
	[ActRcvThrForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of teachers general"
#elif L==4	// es
	""
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
	[ActRcvThrForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActRcvThrForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive the first message of a new thread of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActRcvRepForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of users of the course"
#elif L==4	// es
	""
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
	[ActRcvRepForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActRcvRepForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of users of the degree"
#elif L==4	// es
	""
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
	[ActRcvRepForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActRcvRepForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of users of centre"
#elif L==4	// es
	""
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
	[ActRcvRepForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActRcvRepForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of users of the institution"
#elif L==4	// es
	""
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
	[ActRcvRepForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActRcvRepForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of users general"
#elif L==4	// es
	""
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
	[ActRcvRepForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of teachers general"
#elif L==4	// es
	""
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
	[ActRcvRepForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of users of the platform"
#elif L==4	// es
	""
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
	[ActRcvRepForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive a message of answer in a thread existente in the forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActReqDelThrCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of users of the course"
#elif L==4	// es
	""
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
	[ActReqDelThrCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActReqDelThrDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActReqDelThrDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActReqDelThrCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of users of centre"
#elif L==4	// es
	""
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
	[ActReqDelThrCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActReqDelThrInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActReqDelThrInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActReqDelThrGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of users general"
#elif L==4	// es
	""
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
	[ActReqDelThrGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of teachers general"
#elif L==4	// es
	""
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
	[ActReqDelThrSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActReqDelThrSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of a thread of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActDelThrForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of the course"
#elif L==4	// es
	""
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
	[ActDelThrForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActDelThrForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActDelThrForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActDelThrForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of centre"
#elif L==4	// es
	""
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
	[ActDelThrForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActDelThrForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActDelThrForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActDelThrForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users general"
#elif L==4	// es
	""
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
	[ActDelThrForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers general"
#elif L==4	// es
	""
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
	[ActDelThrForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActDelThrForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a thread of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActCutThrForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of the course"
#elif L==4	// es
	""
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
	[ActCutThrForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActCutThrForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActCutThrForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActCutThrForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of centre"
#elif L==4	// es
	""
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
	[ActCutThrForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActCutThrForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActCutThrForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActCutThrForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users general"
#elif L==4	// es
	""
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
	[ActCutThrForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers general"
#elif L==4	// es
	""
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
	[ActCutThrForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActCutThrForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Cut a thread of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActPasThrForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of the course"
#elif L==4	// es
	""
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
	[ActPasThrForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActPasThrForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActPasThrForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActPasThrForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of centre"
#elif L==4	// es
	""
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
	[ActPasThrForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActPasThrForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActPasThrForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActPasThrForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users general"
#elif L==4	// es
	""
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
	[ActPasThrForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers general"
#elif L==4	// es
	""
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
	[ActPasThrForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActPasThrForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Paste a thread of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActDelPstForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of the course"
#elif L==4	// es
	""
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
	[ActDelPstForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActDelPstForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActDelPstForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActDelPstForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of centre"
#elif L==4	// es
	""
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
	[ActDelPstForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActDelPstForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActDelPstForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActDelPstForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users general"
#elif L==4	// es
	""
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
	[ActDelPstForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers general"
#elif L==4	// es
	""
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
	[ActDelPstForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActDelPstForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActEnbPstForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of the course"
#elif L==4	// es
	""
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
	[ActEnbPstForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActEnbPstForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActEnbPstForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActEnbPstForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of centre"
#elif L==4	// es
	""
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
	[ActEnbPstForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActEnbPstForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActEnbPstForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActEnbPstForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users general"
#elif L==4	// es
	""
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
	[ActEnbPstForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers general"
#elif L==4	// es
	""
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
	[ActEnbPstForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActEnbPstForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enable a message of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActDisPstForCrsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of the course"
#elif L==4	// es
	""
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
	[ActDisPstForCrsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of the course"
#elif L==4	// es
	""
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
	[ActDisPstForDegUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of the degree"
#elif L==4	// es
	""
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
	[ActDisPstForDegTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of the degree"
#elif L==4	// es
	""
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
	[ActDisPstForCtrUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of centre"
#elif L==4	// es
	""
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
	[ActDisPstForCtrTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of centre"
#elif L==4	// es
	""
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
	[ActDisPstForInsUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of the institution"
#elif L==4	// es
	""
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
	[ActDisPstForInsTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of the institution"
#elif L==4	// es
	""
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
	[ActDisPstForGenUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users general"
#elif L==4	// es
	""
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
	[ActDisPstForGenTch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers general"
#elif L==4	// es
	""
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
	[ActDisPstForSWAUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of users of the platform"
#elif L==4	// es
	""
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
	[ActDisPstForSWATch] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Disable a message of forum of teachers of the platform"
#elif L==4	// es
	""
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
	[ActCht] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Enter in a chat room to chat"
#elif L==4	// es
	""
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
	[ActRcvMsgUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Sent/Receive a message of a user"
#elif L==4	// es
	""
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
	[ActReqDelAllSntMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of todos the messages sent to other users"
#elif L==4	// es
	""
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
	[ActReqDelAllRcvMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of todos the messages received from other users"
#elif L==4	// es
	""
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
	[ActDelAllSntMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove todos the messages sent to other users"
#elif L==4	// es
	""
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
	[ActDelAllRcvMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove all messages received from other users"
#elif L==4	// es
	""
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
	[ActDelSntMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message sent to other users"
#elif L==4	// es
	""
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
	[ActDelRcvMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove a message received from other user"
#elif L==4	// es
	""
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
	[ActExpSntMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See (expand) sent message"
#elif L==4	// es
	""
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
	[ActExpRcvMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"See (expand) received message"
#elif L==4	// es
	""
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
	[ActConSntMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide (contract) sent message"
#elif L==4	// es
	""
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
	[ActConRcvMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide (contract) received message"
#elif L==4	// es
	""
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
	[ActLstBanUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List banned users"
#elif L==4	// es
	""
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
	[ActBanUsrMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ban the sender of a message when showing received messages"
#elif L==4	// es
	""
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
	[ActUnbUsrMsg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unban the sender of a message when showing received messages"
#elif L==4	// es
	""
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
	[ActUnbUsrLst] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Unban a user when listing banned users"
#elif L==4	// es
	""
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
	[ActMaiUsr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	""
#elif L==4	// es
	""
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
	[ActReqUseGbl] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request showing use of the platform"
#elif L==4	// es
	""
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
	[ActSeePhoDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show a class photo with the average photos of the students of each degree"
#elif L==4	// es
	""
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
	[ActReqStaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request statistics of courses"
#elif L==4	// es
	""
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
	[ActReqAccGbl] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request query of clicks to the complete platform"
#elif L==4	// es
	""
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
	[ActReqMyUsgRep] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request my usage report"
#elif L==4	// es
	""
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
	[ActMFUAct] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show most frequently used actions"
#elif L==4	// es
	""
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
	[ActSeeUseGbl] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show use of the platform"
#elif L==4	// es
	""
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
	[ActPrnPhoDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show print view of the class photo with the average photos of the students of each degree"
#elif L==4	// es
	""
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
	[ActCalPhoDeg] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Compute the average photos of the students of each degree"
#elif L==4	// es
	""
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
	[ActSeeAccGbl] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Query clicks to the complete platform"
#elif L==4	// es
	""
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
	[ActReqAccCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request query of clicks in the course"
#elif L==4	// es
	""
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
	[ActSeeAccCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Query clicks to current course"
#elif L==4	// es
	""
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
	[ActSeeAllStaCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show statistics of courses"
#elif L==4	// es
	""
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
	[ActLstClk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"List last clicks"
#elif L==4	// es
	""
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
	[ActRefLstClk] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Refresh last clicks in real time via AJAX"
#elif L==4	// es
	""
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
	[ActSeeMyUsgRep] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show my usage report"
#elif L==4	// es
	""
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
	[ActFrmRolSes] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to log out and to change current role in this session"
#elif L==4	// es
	""
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
	[ActMyCrs] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Select one of my courses"
#elif L==4	// es
	""
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
	[ActSeeMyTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the timetable of all courses of the logged user"
#elif L==4	// es
	""
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
	[ActSeeMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show my full agenda (personal organizer)"
#elif L==4	// es
	""
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
	[ActFrmMyAcc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to the creation or change of user's account"
#elif L==4	// es
	""
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
	[ActReqEdiRecSha] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the edition of the record with the personal data of the user"
#elif L==4	// es
	""
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
	[ActReqEdiSet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show forms to edit settings"
#elif L==4	// es
	""
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
	[ActChgMyRol] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change type of logged user"
#elif L==4	// es
	""
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
	[ActFrmNewEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new event in my agenda"
#elif L==4	// es
	""
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
	[ActEdiOneEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit one event from my agenda"
#elif L==4	// es
	""
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
	[ActNewEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new event in my agenda"
#elif L==4	// es
	""
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
	[ActChgEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify data of an event from my agenda"
#elif L==4	// es
	""
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
	[ActReqRemEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of an event from my agenda"
#elif L==4	// es
	""
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
	[ActRemEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove an event from my agenda"
#elif L==4	// es
	""
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
	[ActHidEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Hide an event from my agenda"
#elif L==4	// es
	""
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
	[ActShoEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show an event from my agenda"
#elif L==4	// es
	""
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
	[ActPrvEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Make private an event from my agenda"
#elif L==4	// es
	""
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
	[ActPubEvtMyAgd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Make public an event from my agenda"
#elif L==4	// es
	""
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
	[ActPrnAgdQR] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show agenda QR code ready to print"
#elif L==4	// es
	""
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
	[ActChkUsrAcc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Check if already exists a new account without password associated to a ID"
#elif L==4	// es
	""
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
	[ActCreMyAcc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Form to create a new account for me"
#elif L==4	// es
	""
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
	[ActCreUsrAcc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create new user account"
#elif L==4	// es
	""
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
	[ActRemMyID] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of my user's IDs"
#elif L==4	// es
	""
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
	[ActChgMyID] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Create a new user's ID for me"
#elif L==4	// es
	""
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
	[ActRemMyNck] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of my old nicknames"
#elif L==4	// es
	""
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
	[ActChgMyNck] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my nickname"
#elif L==4	// es
	""
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
	[ActRemMyMai] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove one of my old emails"
#elif L==4	// es
	""
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
	[ActChgMyMai] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my email address"
#elif L==4	// es
	""
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
	[ActCnfMai] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Confirm email address"
#elif L==4	// es
	""
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
	[ActChgMyPwd] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my password"
#elif L==4	// es
	""
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
	[ActReqRemMyAcc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of my account"
#elif L==4	// es
	""
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
	[ActRemMyAcc] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove my account"
#elif L==4	// es
	""
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
	[ActChgMyData] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update my personal data"
#elif L==4	// es
	""
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
	[ActReqMyPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show form to send my photo"
#elif L==4	// es
	""
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
	[ActDetMyPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Receive my photo and detect faces on it"
#elif L==4	// es
	""
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
	[ActUpdMyPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Update my photo"
#elif L==4	// es
	""
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
	[ActReqRemMyPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the removal of my photo"
#elif L==4	// es
	""
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
	[ActRemMyPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Remove my photo"
#elif L==4	// es
	""
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
	[ActChgCtyMyIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change the country of my institution"
#elif L==4	// es
	""
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
	[ActChgMyIns] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my institution"
#elif L==4	// es
	""
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
	[ActChgMyCtr] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my centre"
#elif L==4	// es
	""
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
	[ActChgMyDpt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my department"
#elif L==4	// es
	""
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
	[ActChgMyOff] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my office"
#elif L==4	// es
	""
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
	[ActChgMyOffPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my office phone"
#elif L==4	// es
	""
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
	[ActReqEdiMyNet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Request the edition of my social networks"
#elif L==4	// es
	""
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
	[ActChgMyNet] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change my web and social networks"
#elif L==4	// es
	""
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
	[ActChgThe] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change theme"
#elif L==4	// es
	""
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
	[ActReqChgLan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Ask if change language"
#elif L==4	// es
	""
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
	[ActChgLan] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change language"
#elif L==4	// es
	""
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
	[ActChg1stDay] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change first day of the week"
#elif L==4	// es
	""
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
	[ActChgDatFmt] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change date format"
#elif L==4	// es
	""
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
	[ActChgCol] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change side columns"
#elif L==4	// es
	""
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
	[ActChgIco] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change icon set"
#elif L==4	// es
	""
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
	[ActChgMnu] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change menu"
#elif L==4	// es
	""
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
	[ActChgPriPho] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change privacy of my photo"
#elif L==4	// es
	""
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
	[ActChgBasPriPrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change privacy of my basic public profile"
#elif L==4	// es
	""
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
	[ActChgExtPriPrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change privacy of my extended public profile"
#elif L==4	// es
	""
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
	[ActChgCooPrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change setting about third party cookies"
#elif L==4	// es
	""
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
	[ActChgNtfPrf] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change whether to notify by email new messages"
#elif L==4	// es
	""
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
	[ActPrnUsrQR] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show my QR code ready to print"
#elif L==4	// es
	""
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
	[ActPrnMyTT] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Show the timetable listo to impresión of all my courses"
#elif L==4	// es
	""
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
	[ActEdiTut] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Edit the timetable of tutorías"
#elif L==4	// es
	""
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
	[ActChgTut] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Modify the timetable of tutorías"
#elif L==4	// es
	""
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
	[ActChgMyTT1stDay] =
#if   L==1	// ca
	""			// Necessita traducció
#elif L==2	// de
	""			// Need Übersetzung
#elif L==3	// en
	"Change first day of week and show timetable of the course"
#elif L==4	// es
	""
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
	};
