// swad_text_no_html.c: text messages on screen, in several languages
/*****************************************************************************
		  #   #  ###       #   # ##### #   # #
		  ##  # #   #      #   #   #   ## ## #
		  # # # #   #      #####   #   # # # #
		  #  ## #   #      #   #   #   #   # #
		  #   #  ###       #   #   #   #   # #####
******************************************************************************/
/**** Don't use HTML entities like &oacute; when variable ends in NO_HTML ****/
/****              Charset must be windows-1252 in this file              ****/
/*****************************************************************************/

/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

    Catal� translation:
	Antonio Ca�as Vargas
	Joan Llu�s D�az Rodr�guez
    German translation:
	Antonio Ca�as Vargas
	Rafael Barranco-Droege
    English translation (finished):
	Antonio Ca�as Vargas
    Spanish translation (finished):
	Antonio Ca�as Vargas
    French translation:
	Antonio Ca�as Vargas
    Guarani translation:
	Antonio Ca�as Vargas
    Italian translation (finished):
	Antonio Ca�as Vargas
	Nicola Comunale Rizzo
	Francisco Manuel Herrero P�rez
	Giuseppe Antonio Pagin
	Antonella Grande
    Polish translation:
	Antonio Ca�as Vargas
	Wojtek Kieca
	Tomasz Olechowski
	Mateusz Stanko
    Portuguese translation:
	Antonio Ca�as Vargas

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

#include "swad_language.h"
#include "swad_notification.h"
#include "swad_user.h"

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

#ifndef L
#define L 3	// English
#endif

const char *Txt_Country_NO_HTML =
#if   L==1
	"Pa�s";
#elif L==2
	"Land";
#elif L==3
	"Country";
#elif L==4
	"Pa�s";
#elif L==5
	"Pays;";
#elif L==6
	"Teta";
#elif L==7
	"Paese";
#elif L==8
	"Kraj";
#elif L==9
	"Pa�s";
#elif L==10	// tr
	"Country";	// �eviri lazim!
#endif

const char *Txt_Confirmation_of_your_email_NO_HTML =	// Don't use HTML entities like &egrave; here
#if   L==1
	"Confirmaci� de la vostra adre�a de correu";
#elif L==2
	"Best�tigung Ihrer E-Mail";
#elif L==3
	"Confirmation of your email";
#elif L==4
	"Confirmaci�n de su direcci�n de correo";
#elif L==5
	"Confirmation de votre email";
#elif L==6
	"Confirmaci�n de su direcci�n de correo";	// Okoteve traducci�n
#elif L==7
	"Conferma della tua email";
#elif L==8
	"Potwierdzenie Twojego adresu e-mail";
#elif L==9
	"Confirma��o do seu email";
#elif L==10	// tr
	"Confirmation of your email";			// �eviri lazim!
#endif

// The following variables are compilated together in all languages because they are used in emails...
// ...and each email is sent in the language of the recipient

const char *Txt_Course_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Assignatura",
	[Lan_LANGUAGE_DE     ] = "Kurs",
	[Lan_LANGUAGE_EN     ] = "Course",
	[Lan_LANGUAGE_ES     ] = "Asignatura",
	[Lan_LANGUAGE_FR     ] = "Mati�re",
	[Lan_LANGUAGE_GN     ] = "Mbo'esyry",
	[Lan_LANGUAGE_IT     ] = "Corso",
	[Lan_LANGUAGE_PL     ] = "Kurs",
	[Lan_LANGUAGE_PT     ] = "Disciplina",
	};

const char *Txt_Dear_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &oacute; here
{
[Usr_SEX_UNKNOWN] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Estimat/da",
	[Lan_LANGUAGE_DE     ] = "Sehr geehrte/r",
	[Lan_LANGUAGE_EN     ] = "Dear",
	[Lan_LANGUAGE_ES     ] = "Estimado/a",
	[Lan_LANGUAGE_FR     ] = "Cher/Ch�re",
	[Lan_LANGUAGE_GN     ] = "Estimado/a",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Egregio/a",
	[Lan_LANGUAGE_PL     ] = "Drogi",
	[Lan_LANGUAGE_PT     ] = "Prezado/a",
	},
[Usr_SEX_FEMALE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Estimada",
	[Lan_LANGUAGE_DE     ] = "Sehr geehrte",
	[Lan_LANGUAGE_EN     ] = "Dear",
	[Lan_LANGUAGE_ES     ] = "Estimada",
	[Lan_LANGUAGE_FR     ] = "Ch�re",
	[Lan_LANGUAGE_GN     ] = "Estimada",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Egregia",
	[Lan_LANGUAGE_PL     ] = "Drogi",
	[Lan_LANGUAGE_PT     ] = "Prezada",
	},
[Usr_SEX_MALE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Estimat",
	[Lan_LANGUAGE_DE     ] = "Sehr geehrter",
	[Lan_LANGUAGE_EN     ] = "Dear",
	[Lan_LANGUAGE_ES     ] = "Estimado",
	[Lan_LANGUAGE_FR     ] = "Cher",
	[Lan_LANGUAGE_GN     ] = "Estimado",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Egregio",
	[Lan_LANGUAGE_PL     ] = "Drogi",
	[Lan_LANGUAGE_PT     ] = "Prezado",
	},
[Usr_SEX_ALL] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Estimat/da",
	[Lan_LANGUAGE_DE     ] = "Sehr geehrte/r",
	[Lan_LANGUAGE_EN     ] = "Dear",
	[Lan_LANGUAGE_ES     ] = "Estimado/a",
	[Lan_LANGUAGE_FR     ] = "Cher/Ch�re",
	[Lan_LANGUAGE_GN     ] = "Estimado/a",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Egregio/a",
	[Lan_LANGUAGE_PL     ] = "Drogi",
	[Lan_LANGUAGE_PT     ] = "Prezado/a",
	}
};

const char *Txt_Forum_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &oacute; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "F�rum",
	[Lan_LANGUAGE_DE     ] = "Forum",
	[Lan_LANGUAGE_EN     ] = "Forum",
	[Lan_LANGUAGE_ES     ] = "Foro",
	[Lan_LANGUAGE_FR     ] = "Forum",
	[Lan_LANGUAGE_GN     ] = "Foro",		// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Forum",
	[Lan_LANGUAGE_PL     ] = "Forum",
	[Lan_LANGUAGE_PT     ] = "F�rum",
	};

const char *Txt_General_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &eacute; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "General",
	[Lan_LANGUAGE_DE     ] = "Allgemein",
	[Lan_LANGUAGE_EN     ] = "General",
	[Lan_LANGUAGE_ES     ] = "General",
	[Lan_LANGUAGE_FR     ] = "G�n�ral",
	[Lan_LANGUAGE_GN     ] = "General",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Generale",
	[Lan_LANGUAGE_PL     ] = "Og�lne",
	[Lan_LANGUAGE_PT     ] = "Geral",
	};

const char *Txt_Go_to_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &agrave; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Anar a",
	[Lan_LANGUAGE_DE     ] = "Gehe zum",
	[Lan_LANGUAGE_EN     ] = "Go to",
	[Lan_LANGUAGE_ES     ] = "Ir a",
	[Lan_LANGUAGE_FR     ] = "Aller �",
	[Lan_LANGUAGE_GN     ] = "Ir a",		// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Vai a",
	[Lan_LANGUAGE_PL     ] = "Idz do",
	[Lan_LANGUAGE_PT     ] = "Ir para",
	};

const char *Txt_If_you_just_requested_from_X_the_confirmation_of_your_email_Y_NO_HTML =	// Don't use HTML entities like &egrave; here
#if   L==1
	"Si vost� acaba de sol�licitar des de %s"
	" la confirmaci� del seu correu %s,"
	" premi al seg�ent enlla�"
	" per confirmar aquesta adre�a:"
	" %s/?act=%ld&key=%s\n\n"
	"Si no heu estat vost�, no feu clic a l'enlla� anterior!"
	" En aquest cas, us recomanem que comproveu a %s"
	" si heu confirmat la vostra adre�a de correu.\n\n";
#elif L==2
	"Wenn Sie von %s die Best�tigung"
	" Ihrer E-Mail %s angefordert haben,"
	" klicken Sie auf den folgenden Link,"
	" um diese Anweisung zu best�tigen:"
	" %s/?act=%ld&key=%s\n\n"
	"Wenn Sie es nicht waren, klicken Sie nicht auf den obigen Link!"
	" In diesem Fall empfehlen wir Ihnen, in %s einzuchecken,"
	" wenn Sie Ihre E-Mail-Adresse best�tigt haben.\n\n";
#elif L==3
	"If you have just requested from %s"
	" the confirmation of your email %s,"
	" click on the following link"
	" to confirm that direction:"
	" %s/?act=%ld&key=%s\n\n"
	"If it was not you, do not click on the link above!"
	" In this case, we recommend that you check in %s"
	" if you have confirmed your email address.\n\n";
#elif L==4
	"Si usted acaba de solicitar desde %s"
	" la confirmaci�n de su correo %s,"
	" pulse en el siguiente enlace"
	" para confirmar dicha direcci�n:"
	" %s/?act=%ld&key=%s\n\n"
	"Si no ha sido usted, �no pulse en el enlace anterior!"
	" En este caso, le recomendamos que compruebe en %s"
	" si ha confirmado su direcci�n de correo.\n\n";
#elif L==5
	"Si vous avez demand� � %s"
	" la confirmation de votre email %s,"
	" cliquez sur le lien suivant"
	" pour confirmer cette direction:"
	" %s/?act=%ld&key=%s\n\n"
	"Si ce n'�tait pas vous, ne cliquez pas sur le lien ci-dessus!"
	" Dans ce cas, nous vous recommandons de v�rifier dans %s"
	" si vous avez confirm� votre adresse e-mail.\n\n";
#elif L==6
	"Si usted acaba de solicitar desde %s"
	" la confirmaci�n de su correo %s,"
	" pulse en el siguiente enlace"
	" para confirmar dicha direcci�n:"
	" %s/?act=%ld&key=%s\n\n"
	"Si no ha sido usted, �no pulse en el enlace anterior!"
	" En este caso, le recomendamos que compruebe en %s"
	" si ha confirmado su direcci�n de correo.\n\n";	// Okoteve traducci�n
#elif L==7
	"Se hai appena richiesto a %s"
	" la conferma della tua email %s,"
	" clicca sul seguente link"
	" per confermare quella direzione:"
	" %s/?act=%ld&key=%s\n\n"
	"Se non eri tu, non cliccare sul link qui sopra!"
	" In questo caso, ti consigliamo di controllare in %s"
	" se hai confermato il tuo indirizzo email.\n\n";
#elif L==8
	"Jesli wlasnie zazadales od %s"
	" potwierdzenia swojego e-maila %s,"
	" kliknij ponizszy link,"
	" aby potwierdzic ten kierunek:"
	" %s/?act=%ld&key=%s\n\n"
	"Jesli to nie Ty, nie klikaj w powyzszy link!"
	" W takim przypadku zalecamy sprawdzenie w %s,"
	" jesli potwierdziles sw�j adres e-mail.\n\n";
#elif L==9
	"Se voc� acabou de solicitar de %s"
	" a confirma��o do seu email %s,"
	" clique no seguinte link para confirmar a dire��o:"
	" %s/?act=%ld&key=%s\n\n"
	"Se n�o foi voc�, n�o clique no link acima!"
	" Nesse caso, recomendamos que voc� verifique em %s"
	" se voc� confirmou seu endere�o de e-mail.\n\n";
#elif L==10	// tr
	"If you have just requested from %s"
	" the confirmation of your email %s,"
	" click on the following link"
	" to confirm that direction:"
	" %s/?act=%ld&key=%s\n\n"
	"If it was not you, do not click on the link above!"
	" In this case, we recommend that you check in %s"
	" if you have confirmed your email address.\n\n";	// �eviri lazim!
#endif

const char *Txt_If_you_no_longer_wish_to_receive_email_notifications_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	"",
	"Si no desitgeu rebre m�s avisos per correu,"
	" desactiveu la opci� corresponent en els ajustos del seu perfil.",
	"Wenn Sie nicht mehr erhalten m�chten, E-Mail-Benachrichtigungen,"
	" Deaktivieren Sie die Option in Ihren Profileinstellungen.",
	"If you no longer wish to receive email notifications,"
	" uncheck the option in your profile settings.",
	"Si no desea recibir m�s avisos por correo,"
	" desactive la opci�n correspondiente en los ajustes de su perfil.",
	"Si vous ne souhaitez plus recevoir de notifications par email,"
	" d�cochez l'option dans les param�tres de votre profil.",
	"Si no desea recibir m�s avisos por correo,"
	" desactive la opci�n correspondiente en los ajustes de su perfil.",	// Okoteve traducci�n
	"Se non desideri pi� ricevere notifiche via email,"
	" deselezionare l'opzione nelle impostazioni del tuo profilo.",
	"Jesli nie chcesz otrzymywac powiadomien email,"
	" odznacz opcj&eogon; w ustawieniach swojego profilu.",
	"Se j� n�o desejar receber notifica��es por email,"
	" desmarque a op��o nas configura��es do seu perfil.",
	};

const char *Txt_Institutions_NO_HTML =
#if   L==1
	"Institucions";
#elif L==2
	"Hochschulen";
#elif L==3
	"Institutions";
#elif L==4
	"Instituciones";
#elif L==5
	"�tablissements";
#elif L==6
	"Mbo'ehao";
#elif L==7
	"Istituzioni";
#elif L==8
	"Instytucje";
#elif L==9
	"Institu��es";
#elif L==10	// tr
	"Institutions";	// �eviri lazim!
#endif

const char *Txt_MSG_From_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "De",
	[Lan_LANGUAGE_DE     ] = "Von",
	[Lan_LANGUAGE_EN     ] = "From",
	[Lan_LANGUAGE_ES     ] = "De",
	[Lan_LANGUAGE_FR     ] = "De",
	[Lan_LANGUAGE_GN     ] = "De",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Da",
	[Lan_LANGUAGE_PL     ] = "Od",
	[Lan_LANGUAGE_PT     ] = "De",
	};

const char *Txt_Notifications_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Notificacions",
	[Lan_LANGUAGE_DE     ] = "Anmeldungen",
	[Lan_LANGUAGE_EN     ] = "Notifications",
	[Lan_LANGUAGE_ES     ] = "Notificaciones",
	[Lan_LANGUAGE_FR     ] = "Notifications",
	[Lan_LANGUAGE_GN     ] = "Momarandu",
	[Lan_LANGUAGE_IT     ] = "Notifiche",
	[Lan_LANGUAGE_PL     ] = "Powiadomienia",
	[Lan_LANGUAGE_PT     ] = "Notifica��es",
	};

const char *Txt_New_password_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &ntilde; here.
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova contrasenya",
	[Lan_LANGUAGE_DE     ] = "Neues Passwort",
	[Lan_LANGUAGE_EN     ] = "New password",
	[Lan_LANGUAGE_ES     ] = "Nueva contrase�a",
	[Lan_LANGUAGE_FR     ] = "Nouveau mot de passe",
	[Lan_LANGUAGE_GN     ] = "Nueva contrase�a",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova password",
	[Lan_LANGUAGE_PL     ] = "Nowe has&lstrok;o",
	[Lan_LANGUAGE_PT     ] = "Nova senha",
	};

const char *Txt_NOTIFY_EVENTS_SINGULAR_NO_HTML[Ntf_NUM_NOTIFY_EVENTS][1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &uuml; here.
{
[Ntf_EVENT_UNKNOWN] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Esdeveniment desconegut",
	[Lan_LANGUAGE_DE     ] = "Unbekannt Ereignis",
	[Lan_LANGUAGE_EN     ] = "Unknown event",
	[Lan_LANGUAGE_ES     ] = "Evento desconocido",
	[Lan_LANGUAGE_FR     ] = "�v�nement inconnu",
	[Lan_LANGUAGE_GN     ] = "Evento desconocido",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Evento sconosciuto",
	[Lan_LANGUAGE_PL     ] = "Nieznane zdarzenia",
	[Lan_LANGUAGE_PT     ] = "Evento desconhecido",
	},
[Ntf_EVENT_DOCUMENT_FILE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou document",
	[Lan_LANGUAGE_DE     ] = "Neue Dokumentdatei",
	[Lan_LANGUAGE_EN     ] = "New document file",
	[Lan_LANGUAGE_ES     ] = "Nuevo documento",
	[Lan_LANGUAGE_FR     ] = "Nouveau fichier de document",
	[Lan_LANGUAGE_GN     ] = "Nuevo documento",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo documento",
	[Lan_LANGUAGE_PL     ] = "Nowy plik dokumentu",
	[Lan_LANGUAGE_PT     ] = "Novo arquivo de documento",
	},
[Ntf_EVENT_TEACHERS_FILE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou arxiu de professors",
	[Lan_LANGUAGE_DE     ] = "Neue Lehrer Datei",
	[Lan_LANGUAGE_EN     ] = "New teachers' file",
	[Lan_LANGUAGE_ES     ] = "Nuevo archivo de profesores",
	[Lan_LANGUAGE_FR     ] = "Nouveau fichier d'enseignants",
	[Lan_LANGUAGE_GN     ] = "Nuevo archivo de profesores",		// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo file di professori",
	[Lan_LANGUAGE_PL     ] = "Nowy Nauczyciela plik",
	[Lan_LANGUAGE_PT     ] = "Novo arquivo dos professores",
	},
[Ntf_EVENT_SHARED_FILE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou arxiu compartit",
	[Lan_LANGUAGE_DE     ] = "Neue freigegebene Datei",
	[Lan_LANGUAGE_EN     ] = "New shared file",
	[Lan_LANGUAGE_ES     ] = "Nuevo archivo compartido",
	[Lan_LANGUAGE_FR     ] = "Nouveau fichier partag�",
	[Lan_LANGUAGE_GN     ] = "Nuevo archivo compartido",		// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo file condiviso",
	[Lan_LANGUAGE_PL     ] = "Nowy Udostepniony plik",
	[Lan_LANGUAGE_PT     ] = "Novo arquivo compartilhado",
	},
[Ntf_EVENT_ASSIGNMENT] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova activitat",
	[Lan_LANGUAGE_DE     ] = "Neue Aufgabe",
	[Lan_LANGUAGE_EN     ] = "New assignment",
	[Lan_LANGUAGE_ES     ] = "Nueva actividad",
	[Lan_LANGUAGE_FR     ] = "Nouvelle activit�",
	[Lan_LANGUAGE_GN     ] = "Nueva actividad",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova attivit�",
	[Lan_LANGUAGE_PL     ] = "Nowe zadania",
	[Lan_LANGUAGE_PT     ] = "Nova atividade",
	},
[Ntf_EVENT_CALL_FOR_EXAM] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova convocat�ria d'examen",
	[Lan_LANGUAGE_DE     ] = "Neue Aufrufe f�r Pr�fung",
	[Lan_LANGUAGE_EN     ] = "New call for exam",
	[Lan_LANGUAGE_ES     ] = "Nueva convocatoria de examen",
	[Lan_LANGUAGE_FR     ] = "Nouvelle convocation � un examen",
	[Lan_LANGUAGE_GN     ] = "Nueva convocatoria de examen",		// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo appello d'esame",
	[Lan_LANGUAGE_PL     ] = "Nowe ogloszenie egzaminu",
	[Lan_LANGUAGE_PT     ] = "Nova chamada para exame",
	},
[Ntf_EVENT_MARKS_FILE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou fitxer de calificacions",
	[Lan_LANGUAGE_DE     ] = "Neue Datei mit Bewertung",
	[Lan_LANGUAGE_EN     ] = "New file with marks",
	[Lan_LANGUAGE_ES     ] = "Nuevo archivo de calificaciones",
	[Lan_LANGUAGE_FR     ] = "Nouveau fichier avec des notes",
	[Lan_LANGUAGE_GN     ] = "Nuevo archivo de calificaciones",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo file con i risultati",
	[Lan_LANGUAGE_PL     ] = "Nowy plik ze znakami",
	[Lan_LANGUAGE_PT     ] = "Novo arquivo de notas",
	},
[Ntf_EVENT_ENROLMENT_STD] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova inscripci� com a estudiant",
	[Lan_LANGUAGE_DE     ] = "New Einschreibung als Student",
	[Lan_LANGUAGE_EN     ] = "New enrolment as a student",
	[Lan_LANGUAGE_ES     ] = "Nueva inscripci�n como estudiante",
	[Lan_LANGUAGE_FR     ] = "Nouvelle inscriptionen tant qu'�tudiant",
	[Lan_LANGUAGE_GN     ] = "Nueva inscripci�n como estudiante",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova iscrizione come studente",
	[Lan_LANGUAGE_PL     ] = "Nowe zapisy jako student",
	[Lan_LANGUAGE_PT     ] = "Nova inscri��o como estudante",
	},
[Ntf_EVENT_ENROLMENT_TCH] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova inscripci� com a professor/a",
	[Lan_LANGUAGE_DE     ] = "New Einschreibung als Lehrkraft",
	[Lan_LANGUAGE_EN     ] = "New enrolment as a teacher",
	[Lan_LANGUAGE_ES     ] = "Nueva inscripci�n como profesor/a",
	[Lan_LANGUAGE_FR     ] = "Nouvelle inscription en tant qu'enseignant",
	[Lan_LANGUAGE_GN     ] = "Nueva inscripci�n como profesor/a",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova iscrizione come professore/ssa",
	[Lan_LANGUAGE_PL     ] = "Nowe zapisy jako nauczyciel",
	[Lan_LANGUAGE_PT     ] = "Nova inscri��o como professor/a",
	},
[Ntf_EVENT_ENROLMENT_REQUEST] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova petici� d'inscripci�",
	[Lan_LANGUAGE_DE     ] = "New Registrierungsanforderung",
	[Lan_LANGUAGE_EN     ] = "New enrolment request",
	[Lan_LANGUAGE_ES     ] = "Nueva petici�n de inscripci�n",
	[Lan_LANGUAGE_FR     ] = "Nouvelle demande d'inscription",
	[Lan_LANGUAGE_GN     ] = "Nueva petici�n de inscripci�n",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova richiesta di iscrizione",
	[Lan_LANGUAGE_PL     ] = "Nowe prosby o rejestracji",
	[Lan_LANGUAGE_PT     ] = "Novo pedido de inscri��o",
	},
[Ntf_EVENT_TML_COMMENT] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou comentari a un missatge social",
	[Lan_LANGUAGE_DE     ] = "Neue Kommentar zu soziale Beitrag",
	[Lan_LANGUAGE_EN     ] = "New comment to social post",
	[Lan_LANGUAGE_ES     ] = "Nuevo comentario a un mensaje social",
	[Lan_LANGUAGE_FR     ] = "Nouveau commentaire � message social",
	[Lan_LANGUAGE_GN     ] = "Nuevo comentario a un mensaje social",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo commento a un post sociale",
	[Lan_LANGUAGE_PL     ] = "Nowy komentarz do post spoleczna",
	[Lan_LANGUAGE_PT     ] = "Novo coment�rio a um post sociais",
	},
[Ntf_EVENT_TML_FAV] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou favorit a un missatge social",
	[Lan_LANGUAGE_DE     ] = "Neue Favorit zu soziale Beitrag",
	[Lan_LANGUAGE_EN     ] = "New favourite to social post",
	[Lan_LANGUAGE_ES     ] = "Nuevo favorito a un mensaje social",
	[Lan_LANGUAGE_FR     ] = "Nouveau favori � message social",
	[Lan_LANGUAGE_GN     ] = "Nuevo favorito a un mensaje social",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo preferito a un post sociale",
	[Lan_LANGUAGE_PL     ] = "Nowy ulubiony do post spoleczna",
	[Lan_LANGUAGE_PT     ] = "Novo favorito a um post sociais",
	},
[Ntf_EVENT_TML_SHARE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova compartici� de missatge social",
	[Lan_LANGUAGE_DE     ] = "Neue gemeinsame Nutzung von soziale Beitrag",
	[Lan_LANGUAGE_EN     ] = "New sharing of social post",
	[Lan_LANGUAGE_ES     ] = "Nueva compartici�n de mensaje social",
	[Lan_LANGUAGE_FR     ] = "Nouveau partage de message social",
	[Lan_LANGUAGE_GN     ] = "Nueva compartici�n de mensaje social",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova condivisione di post sociale",
	[Lan_LANGUAGE_PL     ] = "Nowy podzial post spoleczna",
	[Lan_LANGUAGE_PT     ] = "Nova partilha de post sociais",
	},
[Ntf_EVENT_TML_MENTION] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova menci�",
	[Lan_LANGUAGE_DE     ] = "Neue Erw�hnung",
	[Lan_LANGUAGE_EN     ] = "New mention",
	[Lan_LANGUAGE_ES     ] = "Nueva menci�n",
	[Lan_LANGUAGE_FR     ] = "Nouveau mention",
	[Lan_LANGUAGE_GN     ] = "Nueva menci�n",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo menzione",
	[Lan_LANGUAGE_PL     ] = "Nowa wzmianka",
	[Lan_LANGUAGE_PT     ] = "Nova men��o",
	},
[Ntf_EVENT_FOLLOWER] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou seguidor",
	[Lan_LANGUAGE_DE     ] = "Neue Anh&auml;nger",
	[Lan_LANGUAGE_EN     ] = "New follower",
	[Lan_LANGUAGE_ES     ] = "Nuevo seguidor",
	[Lan_LANGUAGE_FR     ] = "Nouveau suiveur",
	[Lan_LANGUAGE_GN     ] = "Nuevo seguidor",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo follower",
	[Lan_LANGUAGE_PL     ] = "Nowy obserwuj&aogon;",
	[Lan_LANGUAGE_PT     ] = "Novo seguidor",
	},
[Ntf_EVENT_FORUM_POST_COURSE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Novo comentari en un f�rum",
	[Lan_LANGUAGE_DE     ] = "Neue Nachricht in einem Forum",
	[Lan_LANGUAGE_EN     ] = "New post in a forum",
	[Lan_LANGUAGE_ES     ] = "Nuevo comentario en un foro",
	[Lan_LANGUAGE_FR     ] = "Nouveau post dans un forum",
	[Lan_LANGUAGE_GN     ] = "Nuevo comentario en un foro",		// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo post in un forum",
	[Lan_LANGUAGE_PL     ] = "Nowy post na forum",
	[Lan_LANGUAGE_PT     ] = "Novo post em um f�rum",
	},
[Ntf_EVENT_FORUM_REPLY] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova resposta en un f�rum",
	[Lan_LANGUAGE_DE     ] = "Neue Antwort in einem Forum",
	[Lan_LANGUAGE_EN     ] = "New reply in a forum",
	[Lan_LANGUAGE_ES     ] = "Nueva respuesta en un foro",
	[Lan_LANGUAGE_FR     ] = "Nouvelle r�ponse dans un forum",
	[Lan_LANGUAGE_GN     ] = "Nueva respuesta en un foro",		// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova risposta in un forum",
	[Lan_LANGUAGE_PL     ] = "Odpowiedz nowe na forum",
	[Lan_LANGUAGE_PT     ] = "Nova resposta em um f�rum",
	},
[Ntf_EVENT_NOTICE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou av�s",
	[Lan_LANGUAGE_DE     ] = "Neuen Ank�ndigungen",
	[Lan_LANGUAGE_EN     ] = "New notice",
	[Lan_LANGUAGE_ES     ] = "Nuevo aviso",
	[Lan_LANGUAGE_FR     ] = "Nouvel avis",
	[Lan_LANGUAGE_GN     ] = "Nuevo aviso",				// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo avviso",
	[Lan_LANGUAGE_PL     ] = "Nowe obwieszczenie",
	[Lan_LANGUAGE_PT     ] = "Novo an�ncio",
	},
[Ntf_EVENT_MESSAGE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nou missatge",
	[Lan_LANGUAGE_DE     ] = "Neue Nachricht",
	[Lan_LANGUAGE_EN     ] = "New message",
	[Lan_LANGUAGE_ES     ] = "Nuevo mensaje",
	[Lan_LANGUAGE_FR     ] = "Nouveau message",
	[Lan_LANGUAGE_GN     ] = "Nuevo mensaje",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo messaggio",
	[Lan_LANGUAGE_PL     ] = "Nowa wiadomosc",
	[Lan_LANGUAGE_PT     ] = "Nova mensagem",
	},
[Ntf_EVENT_SURVEY] =		// TODO: Move to assessment tab (also necessary in database) !!!!!!!!!
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova enquesta",
	[Lan_LANGUAGE_DE     ] = "Neuer Umfrage",
	[Lan_LANGUAGE_EN     ] = "New survey",
	[Lan_LANGUAGE_ES     ] = "Nueva encuesta",
	[Lan_LANGUAGE_FR     ] = "Nouveau sondage",
	[Lan_LANGUAGE_GN     ] = "Nueva encuesta",			// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuovo sondaggio",
	[Lan_LANGUAGE_PL     ] = "Nowe badania",
	[Lan_LANGUAGE_PT     ] = "Novo inqu&eacute;rito",
	},
[Ntf_EVENT_ENROLMENT_NET] =	// TODO: Move to users tab (also necessary in database) !!!!!!!!!
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Nova inscripci� com a professor/a no editor/a",
	[Lan_LANGUAGE_DE     ] = "New Einschreibung als nicht bearbeiteter Lehrkraft",
	[Lan_LANGUAGE_EN     ] = "New enrolment as a non-editing teacher",
	[Lan_LANGUAGE_ES     ] = "Nueva inscripci�n como profesor/a no editor/a",
	[Lan_LANGUAGE_FR     ] = "Nouvelle inscription en tant qu'enseignant/e non-�diteur/trice",
	[Lan_LANGUAGE_GN     ] = "Nueva inscripci�n como profesor/a no editor/a",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Nuova iscrizione come professore/ssa non-editing",
	[Lan_LANGUAGE_PL     ] = "Nowe zapisy jako nauczyciel nie edytujacy",
	[Lan_LANGUAGE_PT     ] = "Nova inscri��o como professor/a n�o editor/a",
	},
};

const char *Txt_NOTIFY_EVENTS_There_is_a_new_event_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Warning: it is very important to include %s in the following sentences
	{			// Don't use HTML entities like &uuml; here.
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Hi ha un nou esdeveniment en %s",
	[Lan_LANGUAGE_DE     ] = "Es ist eine neue Ereignis in %s",
	[Lan_LANGUAGE_EN     ] = "There is a new event in %s",
	[Lan_LANGUAGE_ES     ] = "Hay un nuevo evento en %s",
	[Lan_LANGUAGE_FR     ] = "Il y un nouvel �v�nement dans %s",
	[Lan_LANGUAGE_GN     ] = "Hay un nuevo evento en %s",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "C'� un nuovo evento in %s",
	[Lan_LANGUAGE_PL     ] = "Jest nowe wydarzenie w %s",
	[Lan_LANGUAGE_PT     ] = "Existe um novo evento em %s",
	};

const char *Txt_NOTIFY_EVENTS_There_are_X_new_events_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Warning: it is very important to include %u and %s in the following sentences
	{			// Don't use HTML entities like &uuml; here.
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Hi ha %u nous esdeveniments en %s",
	[Lan_LANGUAGE_DE     ] = "Es gibt %u neue Ereignisse in %s",
	[Lan_LANGUAGE_EN     ] = "There are %u new events in %s",
	[Lan_LANGUAGE_ES     ] = "Hay %u nuevos eventos en %s",
	[Lan_LANGUAGE_FR     ] = "Il y a %u nouveaux �v�nements dans %s",
	[Lan_LANGUAGE_GN     ] = "Hay %u nuevos eventos en %s",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Ci sono %u nuovi eventi in %s",
	[Lan_LANGUAGE_PL     ] = "Istnieje %u nowych wydarzen w %s",
	[Lan_LANGUAGE_PT     ] = "H� %u novos eventos em %s",
	};

const char *Txt_only_teachers_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &oacute; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = ", professors",
	[Lan_LANGUAGE_DE     ] = ", Lehrkr�fte",
	[Lan_LANGUAGE_EN     ] = ", teachers",
	[Lan_LANGUAGE_ES     ] = ", profesores",
	[Lan_LANGUAGE_FR     ] = ", enseignants",
	[Lan_LANGUAGE_GN     ] = ", profesores",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = ", professori",
	[Lan_LANGUAGE_PL     ] = ", nauczyciel",
	[Lan_LANGUAGE_PT     ] = ", professores",
	};

const char *Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Si us plau, no respongui a aquest correu generat autom�ticament.",
	[Lan_LANGUAGE_DE     ] = "Bitte, antworten Sie nicht auf diese automatisch generierte E-Mail.",
	[Lan_LANGUAGE_EN     ] = "Please do not reply to this automatically generated email.",
	[Lan_LANGUAGE_ES     ] = "Por favor, no responda a este correo generado autom�ticamente.",
	[Lan_LANGUAGE_FR     ] = "S'il vous pla�t, ne pas r�pondre � ce courrier �lectronique g�n�r� automatiquement.",
	[Lan_LANGUAGE_GN     ] = "Por favor, no responda a este correo generado autom�ticamente.",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "Per favore, non rispondere a questa email generata automaticamente.",
	[Lan_LANGUAGE_PL     ] = "Prosimy nie odpowiadac na to automatycznie wygenerowany email.",
	[Lan_LANGUAGE_PT     ] = "Por favor, n�o responda a este email gerado automaticamente.",
	 };

const char *Txt_TAB_Messages_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "Missatges",
	[Lan_LANGUAGE_DE     ] = "Nachrichten",
	[Lan_LANGUAGE_EN     ] = "Messages",
	[Lan_LANGUAGE_ES     ] = "Mensajes",
	[Lan_LANGUAGE_FR     ] = "Messages",
	[Lan_LANGUAGE_GN     ] = "Marandu",
	[Lan_LANGUAGE_IT     ] = "Messaggi",
	[Lan_LANGUAGE_PL     ] = "Wiadomosci",
	[Lan_LANGUAGE_PT     ] = "Mensagens",
	};

const char *Txt_The_following_password_has_been_assigned_to_you_to_log_in_X_NO_HTML =	// Warning: it is very important to include %s and %u in the following sentences. Don't use HTML entities like &egrave; here
#if   L==1
	"Se li ha assignat la seg�ent contrasenya per entrar a %s: %s\n"
	"Si desitja activar aquesta nova contrasenya,"
	" ha de entrar a %s amb el seu ID (DNI/c�dula), sobrenom o email"
	" i aquesta nova contrasenya abans de %u dies."
	" Un cop hagi entrat, pot canviar la contrasenya per una altra.\n"
	"Si no accedeix amb aquesta nova contrasenya,"
	" seguir� tenint la contrasenya que tingu�s fins ara.\n"
	"Si ha rebut aquest missatge sense sol&middot;licitar-lo,"
	" es deu a que alg� que coneix el vostre ID (DNI/c�dula), el vostre sobrenom o el vostre email"
	" ha sol&middot;licitat l'enviament de una nova contrasenya per a vost�.\n"
	"Aquesta nova contrasenya se ha enviat nom�s a la adre�a de correu %s,"
	" que �s la que apareix a la seva fitxa.\n\n";
#elif L==2
	"The following password has been assigned to you to log in %s: %s\n"
	"If you want to activate the new password,"
	" you must enter %s with your ID/nickname/email"
	" and this new password before %u days."
	" Once you have logged in, you can change your password.\n"
	"If you do not log in with this new password,"
	" the old password will remain valid.\n"
	"If you have received this message without having requested it,"
	" is that someone who knows your ID, nickname or email has requested a new password sent to you.\n"
	"This new password has been sent only to the email address %s,"
	" which is on your record card.\n\n";	// Need �bersetzung
#elif L==3
	"The following password has been assigned to you to log in %s: %s\n"
	"If you want to activate the new password,"
	" you must enter %s with your ID/nickname/email"
	" and this new password before %u days."
	" Once you have logged in, you can change your password.\n"
	"If you do not log in with this new password,"
	" the old password will remain valid.\n"
	"If you have received this message without having requested it,"
	" is that someone who knows your ID, nickname or email has requested a new password sent to you.\n"
	"This new password has been sent only to the email address %s,"
	" which is on your record card.\n\n";
#elif L==4
	"Se le ha asignado la siguiente contrase�a para entrar en %s: %s\n"
	"Si desea activar esta nueva contrase�a,"
	" debe entrar en %s con su ID (DNI/c�dula), apodo o direcci�n de correo"
	" y esta nueva contrase�a antes de %u d�as."
	" Una vez que haya entrado, puede cambiar la contrase�a por otra.\n"
	"Si no accede con esta nueva contrase�a,"
	" seguir� teniendo la contrase�a que tuviera hasta ahora.\n"
	"Si ha recibido este mensaje sin solicitarlo,"
	" se debe a que alguien que conoce su ID (DNI/c�dula), su apodo o su direcci�n de correo"
	" ha solicitado el env�o de una nueva contrase�a para usted.\n"
	"Esta nueva contrase�a se ha enviado s�lo a la direcci�n de correo %s,"
	" que es la que aparece en su ficha.\n\n";
#elif L==5
	"The following password has been assigned to you to log in %s: %s\n"
	"If you want to activate the new password,"
	" you must enter %s with your ID/nickname/email"
	" and this new password before %u days."
	" Once you have logged in, you can change your password.\n"
	"If you do not log in with this new password,"
	" the old password will remain valid.\n"
	"If you have received this message without having requested it,"
	" is that someone who knows your ID, nickname or email has requested a new password sent to you.\n"
	"This new password has been sent only to the email address %s,"
	" which is on your record card.\n\n";	// Besoin de traduction
#elif L==6
	"Se le ha asignado la siguiente contrase�a para entrar en %s: %s\n"
	"Si desea activar esta nueva contrase�a,"
	" debe entrar en %s con su ID (DNI/c&eacute;dula), apodo o direcci�n de correo"
	" y esta nueva contrase�a antes de %u d�as."
	" Una vez que haya entrado, puede cambiar la contrase�a por otra.\n"
	"Si no accede con esta nueva contrase�a,"
	" seguir� teniendo la contrase�a que tuviera hasta ahora.\n"
	"Si ha recibido este mensaje sin solicitarlo,"
	" se debe a que alguien que conoce su ID (DNI/c�dula), su apodo o su direcci�n de correo"
	" ha solicitado el env�o de una nueva contrase�a para usted.\n"
	"Esta nueva contrase�a se ha enviado s�lo a la direcci�n de correo %s,"
	" que es la que aparece en su ficha.\n\n";	// Okoteve traducci�n
#elif L==7
	"La seguente password ti � stata assegnata per entrare in %s: %s\n"
	"Se vuoi attivare la nuova password,"
	" devi entrare in %s con il tuo ID / nome utente / email"
	" e li c'� la nuova password prima di %u giorni."
	" Una volta che ti sei loggato, puoi scegliere la tua password.\n"
	"Se tu non entri con questa nuova password,"
	" rimarr� valida la vecchia password.\n"
	"Se hai ricevuto questo messaggio senza averlo richiesto,"
	" � perch� qualcuno che sa il tuo ID, nome utente o email ha richiesto una nuova password da inviarti.\n"
	"Questa nuova password � stata inviata solo all'indirizzo email %s,"
	" che � presente sulla tua scheda.\n\n";
#elif L==8
	"The following password has been assigned to you to log in %s: %s\n"
	"If you want to activate the new password,"
	" you must enter %s with your ID/nickname/email"
	" and this new password before %u days."
	" Once you have logged in, you can change your password.\n"
	"If you do not log in with this new password,"
	" the old password will remain valid.\n"
	"If you have received this message without having requested it,"
	" is that someone who knows your ID, nickname or email has requested a new password sent to you.\n"
	"This new password has been sent only to the email address %s,"
	" which is on your record card.\n\n";	// Potrzebujesz tlumaczenie
#elif L==9
	"A seguinte senha foi atribu�da a voc� para efetuar login em %s: %s\n"
	"Se voc� quiser ativar a nova senha,"
	" voc� deve digitar %s com seu ID/alcunha/e-mail"
	" e esta nova senha antes de %u dias."
	" Depois de ter logado, voc� pode alterar sua senha.\n"
	"Se voc� n�o fizer login com essa nova senha,"
	" a senha antiga permanecer� v�lida.\n"
	"Se voc� recebeu esta mensagem sem solicit�-la,"
	" algu�m que conhece seu ID, alcunha ou e-mail solicitou uma nova senha enviada para voc�.\n"
	"Esta nova senha foi enviada apenas para o endere�o de e-mail %s,"
	" que est� no seu cart�o de registro.\n\n";
#elif L==10	// tr
	"The following password has been assigned to you to log in %s: %s\n"
	"If you want to activate the new password,"
	" you must enter %s with your ID/nickname/email"
	" and this new password before %u days."
	" Once you have logged in, you can change your password.\n"
	"If you do not log in with this new password,"
	" the old password will remain valid.\n"
	"If you have received this message without having requested it,"
	" is that someone who knows your ID, nickname or email has requested a new password sent to you.\n"
	"This new password has been sent only to the email address %s,"
	" which is on your record card.\n\n";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_could_not_create_file_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: no s'ha pogut crear el fitxer.";
#elif L==2
	"%s: Datei konnte nicht erstellt werden.";
#elif L==3
	"%s: could not create file.";
#elif L==4
	"%s: no se ha podido crear el archivo.";
#elif L==5
	"%s: impossible de cr�er le fichier.";
#elif L==6
	"%s: no se ha podido crear el archivo.";	// Okoteve traducci�n
#elif L==7
	"%s: impossibile creare il file.";
#elif L==8
	"%s: nie mozna utworzyc plik.";
#elif L==9
	"%s: n�o foi poss�vel criar arquivo.";
#elif L==10	// tr
	"%s: could not create file.";			// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_Forbidden_NO_HTML =
#if   L==1
	"No podeu crear fitxers aqu�.";
#elif L==2
	"Sie k�nnen hier keine Dateien erstellen.";
#elif L==3
	"You can not create files here.";
#elif L==4
	"No puede crear archivos aqu�.";
#elif L==5
	"Vous ne pouvez pas cr�er de fichiers ici.";
#elif L==6
	"No puede crear archivos aqu�.";	// Okoteve traducci�n
#elif L==7
	"Non puoi creare file qui.";
#elif L==8
	"Nie mozesz tutaj tworzyc plik�w.";
#elif L==9
	"Voc� n�o pode criar arquivos aqui.";
#elif L==10	// tr
	"You can not create files here.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_You_must_specify_the_file_NO_HTML =
#if   L==1
	"Heu d'especificar el fitxer.";
#elif L==2
	"Die Datei muss angegeben werden.";
#elif L==3
	"You must specify the file.";
#elif L==4
	"Debe especificar el archivo.";
#elif L==5
	"Vous devez sp�cifier le fichier.";
#elif L==6
	"Debe especificar el archivo.";	// Okoteve traducci�n
#elif L==7
	"� necessario specificare il file.";
#elif L==8
	"Musisz okreslic plik.";
#elif L==9
	"Voc� deve especificar o arquivo.";
#elif L==10	// tr
	"You must specify the file.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_X_extension_not_allowed_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: extensi� no permesa.";
#elif L==2
	"%s: Endung nicht unterst�tzt.";
#elif L==3
	"%s: extension not allowed.";
#elif L==4
	"%s: extensi�n no permitida.";
#elif L==5
	"%s: extension non autoris�e.";
#elif L==6
	"%s: extensi�n no permitida.";	// Okoteve traducci�n
#elif L==7
	"%s: estensione non permessa.";
#elif L==8
	"%s: rozszerzenie pliku jest niedozwolone.";
#elif L==9
	"%s: extens�o n�o permitida.";
#elif L==10	// tr
	"%s: extension not allowed.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_X_file_already_exists_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: l'arxiu ja existeix.";
#elif L==2
	"%s: Die Datei existiert bereits.";
#elif L==3
	"%s: file already exists.";
#elif L==4
	"%s: el archivo ya existe.";
#elif L==5
	"%s: le fichier existe d�j�.";
#elif L==6
	"%s: el archivo ya existe.";	// Okoteve traducci�n
#elif L==7
	"%s: il file esiste gi�.";
#elif L==8
	"%s: plik juz istnieje.";
#elif L==9
	"%s: o arquivo j� existe.";
#elif L==10	// tr
	"%s: file already exists.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML =	// Warning: it is very important to include %lu in the following sentences
#if   L==1
	"Arxiu massa gran (m�xim %lu MB) /"
	" altre problema d'enviament.";
#elif L==2
	"Datei zu gro� (Maximum %lu MiB) /"
	" ein weiteres Upload-Problem.";
#elif L==3
	"File too large (maximum %lu MiB) /"
	" another upload problem.";
#elif L==4
	"Archivo demasiado grande (m�ximo %lu MiB) /"
	" otro problema de env�o.";
#elif L==5
	"Fichier est trop gros (maximum %lu MiB) /"
	" un autre probl�me de t�l�chargement.";
#elif L==6
	"Archivo demasiado grande (m�ximo %lu MiB) /"
	" otro problema de env�o.";		// Okoteve traducci�n
#elif L==7
	"File troppo grande (massimo %lu MiB) /"
	" un altro problema di caricamento.";
#elif L==8
	"Plik jest za duzy (maksimum %lu MiB) /"
	" kolejny problem wgrac.";
#elif L==9
	"Arquivo muito grande (m�ximo %lu MiB) /"
	" outro problema de upload.";
#elif L==10	// tr
	"File too large (maximum %lu MiB) /"
	" another upload problem.";		// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_Invalid_name_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"Nom no v�lid.";
#elif L==2
	"Ung�ltiger Name.";
#elif L==3
	"Invalid name.";
#elif L==4
	"Nombre no v�lido.";
#elif L==5
	"Nom incorrect.";
#elif L==6
	"Nombre no v�lido.";	// Okoteve traducci�n
#elif L==7
	"Nome non valido.";
#elif L==8
	"Bledna nazwa.";
#elif L==9
	"Nome inv�lido.";
#elif L==10	// tr
	"Invalid name.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML =	// Warning: it is very important to include two %s in the following sentences
#if   L==1
	"%s: tipus MIME %s no perm�s.";
#elif L==2
	"%s: MIME-Typ %s nicht unterst�tzt.";
#elif L==3
	"%s: MIME type %s not allowed.";
#elif L==4
	"%s: tipo MIME %s no permitido.";
#elif L==5
	"%s: Type MIME %s non autoris�.";
#elif L==6
	"%s: tipo MIME %s no permitido.";	// Okoteve traducci�n
#elif L==7
	"%s: tipo MIME %s non consentito.";
#elif L==8
	"%s: Typ MIME %s jest niedozwolony.";
#elif L==9
	"%s: Tipo MIME %s n�o permitido.";
#elif L==10	// tr
	"%s: MIME type %s not allowed.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_X_not_HTML_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: no HTML.";
#elif L==2
	"%s: nicht HTML.";
#elif L==3
	"%s: not HTML.";
#elif L==4
	"%s: no HTML.";
#elif L==5
	"%s: pas HTML.";
#elif L==6
	"%s: no HTML.";		// Okoteve traducci�n
#elif L==7
	"%s: non HTML.";
#elif L==8
	"%s: nie HTML.";
#elif L==9
	"%s: n�o HTML.";
#elif L==10	// tr
	"%s: not HTML.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_X_quota_exceeded_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: quota excedida.";
#elif L==2
	"%s: Kontingent �berschritten.";
#elif L==3
	"%s: quota exceeded.";
#elif L==4
	"%s: cuota excedida.";
#elif L==5
	"%s: quota d�pass�.";
#elif L==6
	"%s: cuota excedida.";	// Okoteve traducci�n
#elif L==7
	"%s: quota superata.";
#elif L==8
	"%s: przekroczono przydzial.";
#elif L==9
	"5s: quota excedida.";
#elif L==10	// tr
	"%s: quota exceeded.";	// �eviri lazim!
#endif

const char *Txt_UPLOAD_FILE_Upload_time_too_long_maximum_X_minutes_NO_HTML =	// Warning: it is very important to include %lu in the following sentences
#if   L==1
	"Temps de pujada massa llarg (m�xim %lu minuts).";
#elif L==2
	"Upload ist zu lang (Maximum %lu Minute).";
#elif L==3
	"Upload time too long (maximum %lu minutes).";
#elif L==4
	"Tiempo de subida demasiado largo (m�ximo %lu minutos).";
#elif L==5
	"Temps de t�l�chargement trop longs (maximum %lu minutes).";
#elif L==6
	"Tiempo de subida demasiado largo (m�ximo %lu minutos).";	// Okoteve traducci�n
#elif L==7
	"Durata del caricamento troppo lungo (massimo %lu minuti).";
#elif L==8
	"Upload time too long (maximum %lu minuty).";			// Potrzebujesz tlumaczenie
#elif L==9
	"Tempo de upload muito longo (m�ximo %lu minutos).";
#elif L==10	// tr
	"Upload time too long (maximum %lu minutes).";			// �eviri lazim!
#endif

const char *Txt_user_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
{
[Usr_SEX_UNKNOWN] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "usuari/a",
	[Lan_LANGUAGE_DE     ] = "Benutzer",
	[Lan_LANGUAGE_EN     ] = "user",
	[Lan_LANGUAGE_ES     ] = "usuario/a",
	[Lan_LANGUAGE_FR     ] = "utilisateur/se",
	[Lan_LANGUAGE_GN     ] = "puruh�ra",
	[Lan_LANGUAGE_IT     ] = "utente",
	[Lan_LANGUAGE_PL     ] = "uzytkownika",
	[Lan_LANGUAGE_PT     ] = "utilizador/a",
	[Lan_LANGUAGE_TR     ] = "",	// �eviri lazim!
	},
[Usr_SEX_FEMALE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "usuaria",
	[Lan_LANGUAGE_DE     ] = "Benutzer",
	[Lan_LANGUAGE_EN     ] = "user",
	[Lan_LANGUAGE_ES     ] = "usuaria",
	[Lan_LANGUAGE_FR     ] = "utilisateuse",
	[Lan_LANGUAGE_GN     ] = "usuaria",	// Okoteve traducci�n
	[Lan_LANGUAGE_IT     ] = "utente",
	[Lan_LANGUAGE_PL     ] = "uzytkownika",
	[Lan_LANGUAGE_PT     ] = "utilizadora",
	[Lan_LANGUAGE_TR     ] = "",	// �eviri lazim!
	},
[Usr_SEX_MALE] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "usuari",
	[Lan_LANGUAGE_DE     ] = "Benutzer",
	[Lan_LANGUAGE_EN     ] = "user",
	[Lan_LANGUAGE_ES     ] = "usuario",
	[Lan_LANGUAGE_FR     ] = "utilisateur",
	[Lan_LANGUAGE_GN     ] = "puruh�ra",
	[Lan_LANGUAGE_IT     ] = "utente",
	[Lan_LANGUAGE_PL     ] = "uzytkownika",
	[Lan_LANGUAGE_PT     ] = "utilizador",
	[Lan_LANGUAGE_TR     ] = "",	// �eviri lazim!
	},
[Usr_SEX_ALL] =
	{
	[Lan_LANGUAGE_UNKNOWN] = "",
	[Lan_LANGUAGE_CA     ] = "usuari/a",
	[Lan_LANGUAGE_DE     ] = "Benutzer",
	[Lan_LANGUAGE_EN     ] = "user",
	[Lan_LANGUAGE_ES     ] = "usuario/a",
	[Lan_LANGUAGE_FR     ] = "utilisateur/se",
	[Lan_LANGUAGE_GN     ] = "puruh�ra",
	[Lan_LANGUAGE_IT     ] = "utente",
	[Lan_LANGUAGE_PL     ] = "uzytkownika",
	[Lan_LANGUAGE_PT     ] = "utilizador/a",
	[Lan_LANGUAGE_TR     ] = "",	// �eviri lazim!
	}
};

const char *Txt_Users_NO_HTML =
#if   L==1
	"Usuaris";
#elif L==2
	"Benutzer";
#elif L==3
	"Users";
#elif L==4
	"Usuarios";
#elif L==5
	"Utilisateurs";
#elif L==6
	"Puruh�ra";
#elif L==7
	"Utenti";
#elif L==8
	"Uzytkownik�w";
#elif L==9
	"Utilizadores";
#elif L==10	// tr
	"Users";	// �eviri lazim!
#endif
