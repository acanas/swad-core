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
    Copyright (C) 1999-2018 Antonio Cañas Vargas

    Català translation:
	Antonio Cañas Vargas
	Joan Lluís Díaz Rodríguez
    German translation:
	Antonio Cañas Vargas
	Rafael Barranco-Droege
    English translation (finished):
	Antonio Cañas Vargas
    Spanish translation (finished):
	Antonio Cañas Vargas
    French translation:
	Antonio Cañas Vargas
    Guarani translation:
	Antonio Cañas Vargas
    Italian translation (finished):
	Antonio Cañas Vargas
	Nicola Comunale Rizzo
	Francisco Manuel Herrero Pérez
	Giuseppe Antonio Pagin
	Antonella Grande
    Polish translation:
	Antonio Cañas Vargas
	Wojtek Kieca
	Tomasz Olechowski
	Mateusz Stanko
    Portuguese translation:
	Antonio Cañas Vargas

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
/******************************** Constants **********************************/
/*****************************************************************************/

#ifndef L
#define L 3	// English
#endif

const char *Txt_Country_NO_HTML =
#if   L==1
	"País";
#elif L==2
	"Land";
#elif L==3
	"Country";
#elif L==4
	"País";
#elif L==5
	"Pays;";
#elif L==6
	"Teta";
#elif L==7
	"Paese";
#elif L==8
	"Kraj";
#elif L==9
	"Pa&iacute;s";
#endif

const char *Txt_Confirmation_of_your_email_NO_HTML =	// Don't use HTML entities like &egrave; here
#if   L==1
	"Confirmación de su dirección de correo";	// Necessita traduccio
#elif L==2
	"Confirmation of your email";			// Need Übersetzung
#elif L==3
	"Confirmation of your email";
#elif L==4
	"Confirmación de su dirección de correo";
#elif L==5
	"Confirmation of your email";			// Besoin de traduction
#elif L==6
	"Confirmación de su dirección de correo";	// Okoteve traducción
#elif L==7
	"Confirmation of your email";			// Bisogno di traduzione
#elif L==8
	"Confirmation of your email";			// Potrzebujesz tlumaczenie
#elif L==9
	"Confirmação do seu email.";
#endif

// The following variables are compilated together in all languages because they are used in emails...
// ...and each email is sent in the language of the recipient

const char *Txt_Course_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	"",
	"Assignatura",
	"Kurs",
	"Course",
	"Asignatura",
	"Matière",
	"Mbo'esyry",
	"Corso",
	"Kurs",
	"Disciplina",
	};

const char *Txt_Dear_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &oacute; here
{
	{
	// Usr_SEX_UNKNOWN
	"",
	"Estimat/da",
	"Sehr geehrte/r",
	"Dear",
	"Estimado/a",
	"Cher/Chère",
	"Estimado/a",	// Okoteve traducción
	"Egregio/a",
	"Drogi",
	"Prezado/a",
	},
	{
	// Usr_SEX_FEMALE
	"",
	"Estimada",
	"Sehr geehrte",
	"Dear",
	"Estimada",
	"Chère",
	"Estimada",	// Okoteve traducción
	"Egregia",
	"Drogi",
	"Prezada",
	},
	{
	// Usr_SEX_MALE
	"",
	"Estimat",
	"Sehr geehrter",
	"Dear",
	"Estimado",
	"Cher",
	"Estimado",	// Okoteve traducción
	"Egregio",
	"Drogi",
	"Prezado",
	},
	{
	// Usr_SEX_ALL
	"",
	"Estimat/da",
	"Sehr geehrte/r",
	"Dear",
	"Estimado/a",
	"Cher/Chère",
	"Estimado/a",	// Okoteve traducción
	"Egregio/a",
	"Drogi",
	"Prezado/a",
	}
};

const char *Txt_Forum_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &oacute; here
	{
	"",
	"Fòrum",
	"Forum",
	"Forum",
	"Foro",
	"Forum",
	"Foro",		// Okoteve traducción
	"Forum",
	"Forum",
	"Fórum",
	};

const char *Txt_General_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &eacute; here
	{
	"",
	"General",
	"Allgemein",
	"General",
	"General",
	"Général",
	"General",	// Okoteve traducción
	"Generale",
	"Ogólne",
	"Geral",
	};

const char *Txt_Go_to_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &agrave; here
	{
	"",
	"Anar a",
	"Gehe zum",
	"Go to",
	"Ir a",
	"Aller à",
	"Ir a",		// Okoteve traducción
	"Vai a",
	"Idz do",
	"Ir para",
	};

const char *Txt_If_you_just_request_from_X_the_confirmation_of_your_email_Y_NO_HTML =	// Don't use HTML entities like &egrave; here
#if   L==1
	"Si usted acaba de solicitar desde %s"
	" la confirmación de su correo %s,"
	" pulse en el siguiente enlace"
	" para confirmar dicha dirección:"
	" %s/?act=%ld&key=%s\n\n"
	"Si no ha sido usted, ¡no pulse en el enlace anterior!"
	" En este caso, le recomendamos que compruebe en %s"
	" si ha confirmado su dirección de correo.\n\n";	// Necessita traduccio
#elif L==2
	"If you just request from %s"
	" the confirmation of your email %s,"
	" click on the following link"
	" to confirm that direction:"
	" %s/?act=%ld&key=%s\n\n"
	"If it was not you, do not click on the link above!"
	" In this case, we recommend that you check in %s"
	" if you have confirmed your email address.\n\n";	// Need Übersetzung
#elif L==3
	"If you just request from %s"
	" the confirmation of your email %s,"
	" click on the following link"
	" to confirm that direction:"
	" %s/?act=%ld&key=%s\n\n"
	"If it was not you, do not click on the link above!"
	" In this case, we recommend that you check in %s"
	" if you have confirmed your email address.\n\n";
#elif L==4
	"Si usted acaba de solicitar desde %s"
	" la confirmación de su correo %s,"
	" pulse en el siguiente enlace"
	" para confirmar dicha dirección:"
	" %s/?act=%ld&key=%s\n\n"
	"Si no ha sido usted, ¡no pulse en el enlace anterior!"
	" En este caso, le recomendamos que compruebe en %s"
	" si ha confirmado su dirección de correo.\n\n";
#elif L==5
	"If you just request from %s"
	" the confirmation of your email %s,"
	" click on the following link"
	" to confirm that direction:"
	" %s/?act=%ld&key=%s\n\n"
	"If it was not you, do not click on the link above!"
	" In this case, we recommend that you check in %s"
	" if you have confirmed your email address.\n\n";	// Besoin de traduction
#elif L==6
	"Si usted acaba de solicitar desde %s"
	" la confirmación de su correo %s,"
	" pulse en el siguiente enlace"
	" para confirmar dicha dirección:"
	" %s/?act=%ld&key=%s\n\n"
	"Si no ha sido usted, ¡no pulse en el enlace anterior!"
	" En este caso, le recomendamos que compruebe en %s"
	" si ha confirmado su dirección de correo.\n\n";	// Okoteve traducción
#elif L==7
	"If you just request from %s"
	" the confirmation of your email %s,"
	" click on the following link"
	" to confirm that direction:"
	" %s/?act=%ld&key=%s\n\n"
	"If it was not you, do not click on the link above!"
	" In this case, we recommend that you check in %s"
	" if you have confirmed your email address.\n\n";	// Bisogno di traduzione
#elif L==8
	"If you just request from %s"
	" the confirmation of your email %s,"
	" click on the following link"
	" to confirm that direction:"
	" %s/?act=%ld&key=%s\n\n"
	"If it was not you, do not click on the link above!"
	" In this case, we recommend that you check in %s"
	" if you have confirmed your email address.\n\n";	// Potrzebujesz tlumaczenie
#elif L==9
	"Se você acabou de solicitar de %s"
	" a confirmação do seu email %s,"
	" clique no seguinte link para confirmar a direção:"
	" %s/?act=%ld&key=%s\n\n"
	"Se não foi você, não clique no link acima!"
	" Nesse caso, recomendamos que você verifique em %s"
	" se você confirmou seu endereço de e-mail.\n\n";
#endif

const char *Txt_If_you_no_longer_wish_to_receive_email_notifications_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	"",
	"Si no desitgeu rebre més avisos per correu,"
	" desactiveu la opció corresponent en les preferències del seu perfil.",
	"Wenn Sie nicht mehr erhalten möchten, E-Mail-Benachrichtigungen,"
	" Deaktivieren Sie die Option in den Einstellungen Ihres Profils.",
	"If you no longer wish to receive email notifications,"
	" uncheck the option in the preferences of your profile.",
	"Si no desea recibir más avisos por correo,"
	" desactive la opción correspondiente en las preferencias de su perfil.",
	"Si vous ne souhaitez plus recevoir de notifications par email,"
	" décochez l'option dans les préférences de votre profil.",
	"Si no desea recibir más avisos por correo,"
	" desactive la opción correspondiente en las preferencias de su perfil.",	// Okoteve traducción
	"Se non desideri più ricevere notifiche via email,"
	" deselezionare l'opzione nelle preferenze del tuo profilo.",
	"Jesli nie chcesz otrzymywac powiadomien email,"
	" usu&nacute; zaznaczenie opcje w ustawieniach swojego profilu.",
	"Se já não desejar receber notificações por email,"
	" desmarque a opção nas preferências do seu perfil.",
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
	"Établissements";
#elif L==6
	"Mbo'ehao";
#elif L==7
	"Istituzioni";
#elif L==8
	"Instytucje";
#elif L==9
	"Instituções";
#endif

const char *Txt_MSG_From_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	"",
	"De",
	"Von",
	"From",
	"De",
	"De",
	"De",	// Okoteve traducción
	"Da",
	"Od",
	"De",
	};

const char *Txt_Notifications_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	"",
	"Notificacions",
	"Anmeldungen",
	"Notifications",
	"Notificaciones",
	"Notifications",
	"Momarandu",
	"Notifiche",
	"Powiadomienia",
	"Notificações",
	};

const char *Txt_New_password_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &ntilde; here.
	{
	"",
	"Nova contrasenya",
	"Neues Passwort",
	"New password",
	"Nueva contraseña",
	"Nouveau mot de passe",
	"Nueva contraseña",	// Okoteve traducción
	"Nuova password",
	"Nowe has&lstrok;o",
	"Nova senha",
	};

const char *Txt_NOTIFY_EVENTS_SINGULAR_NO_HTML[Ntf_NUM_NOTIFY_EVENTS][1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &uuml; here.
{
	{
	// Ntf_EVENT_UNKNOWN
	"",
	"Esdeveniment desconegut",
	"Unbekannt Ereignis",
	"Unknown event",
	"Evento desconocido",
	"Événement inconnu",
	"Evento desconocido",			// Okoteve traducción
	"Evento sconosciuto",
	"Nieznane zdarzenia",
	"Evento desconhecido",
	},
	{
	// Ntf_EVENT_DOCUMENT_FILE
	"",
	"Nou document",
	"Neue Dokumentdatei",
	"New document file",
	"Nuevo documento",
	"Nouveau fichier de document",
	"Nuevo documento",			// Okoteve traducción
	"Nuovo documento",
	"Nowy plik dokumentu",
	"Novo arquivo de documento",
	},
	{
	// Ntf_EVENT_TEACHERS_FILE
	"",
	"Nou arxiu de professors",
	"Neue Lehrer Datei",
	"New teachers' file",
	"Nuevo archivo de profesores",
	"Nouveau fichier d'enseignants",
	"Nuevo archivo de profesores",		// Okoteve traducción
	"Nuovo file di professori",
	"Nowy Nauczyciela plik",
	"Novo arquivo dos professores",
	},
	{
	// Ntf_EVENT_SHARED_FILE
	"",
	"Nou arxiu compartit",
	"Neue freigegebene Datei",
	"New shared file",
	"Nuevo archivo compartido",
	"Nouveau fichier partagé",
	"Nuevo archivo compartido",		// Okoteve traducción
	"Nuovo file condiviso",
	"Nowy Udostepniony plik",
	"Novo arquivo compartilhado",
	},
	{
	// Ntf_EVENT_ASSIGNMENT
	"",
	"Nova activitat",
	"Neue Aufgabe",
	"New assignment",
	"Nueva actividad",
	"Nouvelle activité",
	"Nueva actividad",			// Okoteve traducción
	"Nuova attività",
	"Nowe zadania",
	"Nova atividade",
	},
	{
	// Ntf_EVENT_EXAM_ANNOUNCEMENT
	"",
	"Nova convocatòria d'examen",
	"Neue Aufrufe für Prüfung",
	"New announcement of exam",
	"Nueva convocatoria de examen",
	"Nouvelle convocation à un examen",
	"Nueva convocatoria de examen",		// Okoteve traducción
	"Nuovo appello d'esame",
	"Nowe ogloszenie egzaminu",
	"Nova chamada para exame",
	},
	{
	// Ntf_EVENT_MARKS_FILE
	"",
	"Nou fitxer de calificacions",
	"Neue Datei mit Bewertung",
	"New file with marks",
	"Nuevo archivo de calificaciones",
	"Nouveau fichier avec des notes",
	"Nuevo archivo de calificaciones",	// Okoteve traducción
	"Nuovo file con i risultati",
	"Nowy plik ze znakami",
	"Novo arquivo de notas",
	},
	{
	// Ntf_EVENT_ENROLMENT_STD
	"",
	"Nova inscripció com a estudiant",
	"New Einschreibung als Student",
	"New enrolment as a student",
	"Nueva inscripción como estudiante",
	"Nouvelle inscriptionen tant qu'étudiant",
	"Nueva inscripción como estudiante",			// Okoteve traducción
	"Nuova iscrizione come studente",
	"Nowe zapisy jako student",
	"Nova inscrição como estudante",
	},
	{
	// Ntf_EVENT_ENROLMENT_TCH
	"",
	"Nova inscripció com a professor/a",
	"New Einschreibung als Lehrkraft",
	"New enrolment as a teacher",
	"Nueva inscripción como profesor/a",
	"Nouvelle inscription en tant qu'enseignant",
	"Nueva inscripción como profesor/a",			// Okoteve traducción
	"Nuova iscrizione come professore/ssa",
	"Nowe zapisy jako nauczyciel",
	"Nova inscrição como professor/a",
	},
	{
	// Ntf_EVENT_ENROLMENT_REQUEST
	"",
	"Nova petició d'inscripció",
	"New Registrierungsanforderung",
	"New enrolment request",
	"Nueva petición de inscripción",
	"Nouvelle demande d'inscription",
	"Nueva petición de inscripción",	// Okoteve traducción
	"Nuova richiesta di iscrizione",
	"Nowe prosby o rejestracji",
	"Novo pedido de inscrição",
	},
	{
	// Ntf_EVENT_TIMELINE_COMMENT
	"",
	"Nou comentari a un missatge social",
	"Neue Kommentar zu soziale Beitrag",
	"New comment to social post",
	"Nuevo comentario a un mensaje social",
	"Nouveau commentaire à message social",
	"Nuevo comentario a un mensaje social",			// Okoteve traducción
	"Nuovo commento a un post sociale",
	"Nowy komentarz do post spoleczna",
	"Novo comentário a um post sociais",
	},
	{
	// Ntf_EVENT_TIMELINE_FAV
	"",
	"Nou favorit a un missatge social",
	"Neue Favorit zu soziale Beitrag",
	"New favourite to social post",
	"Nuevo favorito a un mensaje social",
	"Nouveau favori à message social",
	"Nuevo favorito a un mensaje social",			// Okoteve traducción
	"Nuovo preferito a un post sociale",
	"Nowy ulubiony do post spoleczna",
	"Novo favorito a um post sociais",
	},
	{
	// Ntf_EVENT_TIMELINE_SHARE
	"",
	"Nova compartició de missatge social",
	"Neue gemeinsame Nutzung von soziale Beitrag",
	"New sharing of social post",
	"Nueva compartición de mensaje social",
	"Nouveau partage de message social",
	"Nueva compartición de mensaje social",			// Okoteve traducción
	"Nuova condivisione di post sociale",
	"Nowy podzial post spoleczna",
	"Nova partilha de post sociais",
	},
	{
	// Ntf_EVENT_TIMELINE_MENTION
	"",
	"Nova menció",
	"Neue Erwähnung",
	"New mention",
	"Nueva mención",
	"Nouveau mention",
	"Nueva mención",			// Okoteve traducción
	"Nuovo menzione",
	"Nowa wzmianka",
	"Nova menção",
	},
	{
	// Ntf_EVENT_FOLLOWER
	"",
	"Nou seguidor",
	"Neue Anh&auml;nger",
	"New follower",
	"Nuevo seguidor",
	"Nouveau suiveur",
	"Nuevo seguidor",	// Okoteve traducción
	"Nuovo follower",
	"Nowy obserwuj&aogon;",
	"Novo seguidor",
	},
	{
	// Ntf_EVENT_FORUM_POST_COURSE
	"",
	"Novo comentari en un fòrum",
	"Neue Nachricht in einem Forum",
	"New post in a forum",
	"Nuevo comentario en un foro",
	"Nouveau post dans un forum",
	"Nuevo comentario en un foro",		// Okoteve traducción
	"Nuovo post in un forum",
	"Nowy post na forum",
	"Novo post em um fórum",
	},
	{
	// Ntf_EVENT_FORUM_REPLY
	"",
	"Nova resposta en un fòrum",
	"Neue Antwort in einem Forum",
	"New reply in a forum",
	"Nueva respuesta en un foro",
	"Nouvelle réponse dans un forum",
	"Nueva respuesta en un foro",		// Okoteve traducción
	"Nuova risposta in un forum",
	"Odpowiedz nowe na forum",
	"Nova resposta em um fórum",
	},
	{
	// Ntf_EVENT_NOTICE
	"",
	"Nou avís",
	"Neuen Ankündigungen",
	"New notice",
	"Nuevo aviso",
	"Nouvel avis",
	"Nuevo aviso",				// Okoteve traducción
	"Nuovo avviso",
	"Nowe obwieszczenie",
	"Novo anúncio",
	},
	{
	// Ntf_EVENT_MESSAGE
	"",
	"Nou missatge",
	"Neue Nachricht",
	"New message",
	"Nuevo mensaje",
	"Nouveau message",
	"Nuevo mensaje",			// Okoteve traducción
	"Nuovo messaggio",
	"Nowa wiadomosc",
	"Nova mensagem",
	},
	{
	// Ntf_EVENT_SURVEY		// TODO: Move to assessment tab (also necessary in database) !!!!!!!!!
	"",
	"Nova enquesta",
	"Neuer Umfrage",
	"New survey",
	"Nueva encuesta",
	"Nouveau sondage",
	"Nueva encuesta",			// Okoteve traducción
	"Nuovo sondaggio",
	"Nowe badania",
	"Novo inqu&eacute;rito",
	},
	{
	// Ntf_EVENT_ENROLMENT_NET	// TODO: Move to users tab (also necessary in database) !!!!!!!!!
	"",
	"Nova inscripció com a professor/a no editor/a",
	"New Einschreibung als nicht bearbeiteter Lehrkraft",
	"New enrolment as a non-editing teacher",
	"Nueva inscripción como profesor/a no editor/a",
	"Nouvelle inscription en tant qu'enseignant/e non-éditeur/trice",
	"Nueva inscripción como profesor/a no editor/a",	// Okoteve traducción
	"Nuova iscrizione come professore/ssa non-editing",
	"Nowe zapisy jako nauczyciel nie edytujacy",
	"Nova inscrição como professor/a não editor/a",
	},
};

const char *Txt_NOTIFY_EVENTS_There_is_a_new_event_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Warning: it is very important to include %s in the following sentences
	{			// Don't use HTML entities like &uuml; here.
	"",
	"Hi ha un nou esdeveniment en %s",
	"Es ist eine neue Ereignis in %s",
	"There is a new event in %s",
	"Hay un nuevo evento en %s",
	"Il y un nouvel événement dans %s",
	"Hay un nuevo evento en %s",	// Okoteve traducción
	"C'è un nuovo evento in %s",
	"Jest nowe wydarzenie w %s",
	"Existe um novo evento em %s",
	};

const char *Txt_NOTIFY_EVENTS_There_are_X_new_events_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Warning: it is very important to include %u and %s in the following sentences
	{			// Don't use HTML entities like &uuml; here.
	"",
	"Hi ha %u nous esdeveniments en %s",
	"Es gibt %u neue Ereignisse in %s",
	"There are %u new events in %s",
	"Hay %u nuevos eventos en %s",
	"Il y a %u nouveaux événements dans %s",
	"Hay %u nuevos eventos en %s",	// Okoteve traducción
	"Ci sono %u nuovi eventi in %s",
	"Istnieje %u nowych wydarzen w %s",
	"Há %u novos eventos em %s",
	};

const char *Txt_only_teachers_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &oacute; here
	{
	"",
	", professors",
	", Lehrkräfte",
	", teachers",
	", profesores",
	", enseignants",
	", profesores",	// Okoteve traducción
	", professori",
	", nauczyciel",
	", professores",
	};

const char *Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	"",
	"Si us plau, no respongui a aquest correu generat automàticament.",
	"Bitte, antworten Sie nicht auf diese automatisch generierte E-Mail.",
	"Please do not reply to this automatically generated email.",
	"Por favor, no responda a este correo generado automáticamente.",
	"S'il vous plaît, ne pas répondre à ce courrier électronique généré automatiquement.",
	"Por favor, no responda a este correo generado automáticamente.",	// Okoteve traducción
	"Per favore, non rispondere a questa email generata automaticamente.",
	"Prosimy nie odpowiadac na to automatycznie wygenerowany email.",
	"Por favor, não responda a este email gerado automaticamente.",
	 };

const char *Txt_TAB_Messages_NO_HTML[1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
	{
	"",
	"Missatges",
	"Nachrichten",
	"Messages",
	"Mensajes",
	"Messages",
	"Marandu",
	"Messaggi",
	"Wiadomosci",
	"Mensagens",
	};

const char *Txt_The_following_password_has_been_assigned_to_you_to_log_in_X_NO_HTML =	// Warning: it is very important to include %s and %u in the following sentences. Don't use HTML entities like &egrave; here
#if   L==1
	"Se li ha assignat la següent contrasenya per entrar a %s: %s\n"
	"Si desitja activar aquesta nova contrasenya,"
	" ha de entrar a %s amb el seu ID (DNI/cédula), sobrenom o email"
	" i aquesta nova contrasenya abans de %u dies."
	" Un cop hagi entrat, pot canviar la contrasenya per una altra.\n"
	"Si no accedeix amb aquesta nova contrasenya,"
	" seguirà tenint la contrasenya que tingués fins ara.\n"
	"Si ha rebut aquest missatge sense sol&middot;licitar-lo,"
	" es deu a que algú que coneix el vostre ID (DNI/cédula), el vostre sobrenom o el vostre email"
	" ha sol&middot;licitat l'enviament de una nova contrasenya per a vostè.\n"
	"Aquesta nova contrasenya se ha enviat només a la adreça de correu %s,"
	" que és la que apareix a la seva fitxa.\n\n";
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
	" which is on your record card.\n\n";	// Need Übersetzung
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
	"Se le ha asignado la siguiente contraseña para entrar en %s: %s\n"
	"Si desea activar esta nueva contraseña,"
	" debe entrar en %s con su ID (DNI/cédula), apodo o dirección de correo"
	" y esta nueva contraseña antes de %u días."
	" Una vez que haya entrado, puede cambiar la contraseña por otra.\n"
	"Si no accede con esta nueva contraseña,"
	" seguirá teniendo la contraseña que tuviera hasta ahora.\n"
	"Si ha recibido este mensaje sin solicitarlo,"
	" se debe a que alguien que conoce su ID (DNI/cédula), su apodo o su dirección de correo"
	" ha solicitado el envío de una nueva contraseña para usted.\n"
	"Esta nueva contraseña se ha enviado sólo a la dirección de correo %s,"
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
	"Se le ha asignado la siguiente contraseña para entrar en %s: %s\n"
	"Si desea activar esta nueva contraseña,"
	" debe entrar en %s con su ID (DNI/c&eacute;dula), apodo o dirección de correo"
	" y esta nueva contraseña antes de %u días."
	" Una vez que haya entrado, puede cambiar la contraseña por otra.\n"
	"Si no accede con esta nueva contraseña,"
	" seguirá teniendo la contraseña que tuviera hasta ahora.\n"
	"Si ha recibido este mensaje sin solicitarlo,"
	" se debe a que alguien que conoce su ID (DNI/cédula), su apodo o su dirección de correo"
	" ha solicitado el envío de una nueva contraseña para usted.\n"
	"Esta nueva contraseña se ha enviado sólo a la dirección de correo %s,"
	" que es la que aparece en su ficha.\n\n";	// Okoteve traducción
#elif L==7
	"La seguente password ti è stata assegnata per entrare in %s: %s\n"
	"Se vuoi attivare la nuova password,"
	" devi entrare in %s con il tuo ID / nome utente / email"
	" e li c'è la nuova password prima di %u giorni."
	" Una volta che ti sei loggato, puoi scegliere la tua password.\n"
	"Se tu non entri con questa nuova password,"
	" rimarrà valida la vecchia password.\n"
	"Se hai ricevuto questo messaggio senza averlo richiesto,"
	" è perchè qualcuno che sa il tuo ID, nome utente o email ha richiesto una nuova password da inviarti.\n"
	"Questa nuova password è stata inviata solo all'indirizzo email %s,"
	" che è presente sulla tua scheda.\n\n";
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
	"A seguinte senha foi atribuída a você para efetuar login em %s: %s\n"
	"Se você quiser ativar a nova senha,"
	" você deve digitar %s com seu ID/alcunha/e-mail"
	" e esta nova senha antes de %u dias."
	" Depois de ter logado, você pode alterar sua senha.\n"
	"Se você não fizer login com essa nova senha,"
	" a senha antiga permanecerá válida.\n"
	"Se você recebeu esta mensagem sem solicitá-la,"
	" alguém que conhece seu ID, alcunha ou e-mail solicitou uma nova senha enviada para você.\n"
	"Esta nova senha foi enviada apenas para o endereço de e-mail %s,"
	" que está no seu cartão de registro.\n\n";
#endif

const char *Txt_Unknown_or_without_photo =
#if   L==1
	"Desconocido/a o sin fotograf&iacute;a";	// Necessita traduccio
#elif L==2
	"Unknown or without photo";			// Need Übersetzung
#elif L==3
	"Unknown or without photo";
#elif L==4
	"Desconocido/a o sin fotograf&iacute;a";
#elif L==5
	"Unknown or without photo";			// Besoin de traduction
#elif L==6
	"Desconocido/a o sin fotograf&iacute;a";	// Okoteve traducción
#elif L==7
	"Sconosciuto/a o senza foto";
#elif L==8
	"Unknown or without photo";			// Potrzebujesz tlumaczenie
#elif L==9
	"Desconhecido/a ou sem foto";
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
	"%s: impossible de créer le fichier.";
#elif L==6
	"%s: no se ha podido crear el archivo.";	// Okoteve traducción
#elif L==7
	"%s: impossibile creare il file.";
#elif L==8
	"%s: nie mozna utworzyc plik.";
#elif L==9
	"%s: não foi possível criar arquivo.";
#endif

const char *Txt_UPLOAD_FILE_Forbidden_NO_HTML =
#if   L==1
	"No puede crear archivos aquí.";	// Necessita traduccio
#elif L==2
	"You can not create files here.";	// Need Übersetzung
#elif L==3
	"You can not create files here.";
#elif L==4
	"No puede crear archivos aquí.";
#elif L==5
	"You can not create files here.";	// Besoin de traduction
#elif L==6
	"No puede crear archivos aquí.";	// Okoteve traducción
#elif L==7
	"Non puoi creare file qui.";
#elif L==8
	"You can not create files here.";	// Potrzebujesz tlumaczenie
#elif L==9
	"Você não pode criar arquivos aqui.";
#endif

const char *Txt_UPLOAD_FILE_You_must_specify_the_file_NO_HTML =
#if   L==1
	"Debe especificar el archivo.";	// Necessita traduccio
#elif L==2
	"Die Datei muss angegeben werden.";
#elif L==3
	"You must specify the file.";
#elif L==4
	"Debe especificar el archivo.";
#elif L==5
	"Vous devez spécifier le fichier.";
#elif L==6
	"Debe especificar el archivo.";	// Okoteve traducción
#elif L==7
	"È necessario specificare il file.";
#elif L==8
	"You must specify the file.";	// Potrzebujesz tlumaczenie
#elif L==9
	"Você deve especificar o arquivo.";
#endif

const char *Txt_UPLOAD_FILE_X_extension_not_allowed_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: extensión no permitida.";		// Necessita traduccio
#elif L==2
	"%s: Endung nicht unterstützt.";
#elif L==3
	"%s: extension not allowed.";
#elif L==4
	"%s: extensión no permitida.";
#elif L==5
	"%s: extension not allowed.";		// Besoin de traduction
#elif L==6
	"%s: extensión no permitida.";		// Okoteve traducción
#elif L==7
	"%s: estensione non permessa.";
#elif L==8
	"%s: extension not allowed.";		// Potrzebujesz tlumaczenie
#elif L==9
	"%s: extensão não permitida.";
#endif

const char *Txt_UPLOAD_FILE_X_file_already_exists_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: el archivo ya existe.";	// Necessita traduccio
#elif L==2
	"%s: file already exists.";	// Need Übersetzung
#elif L==3
	"%s: file already exists.";
#elif L==4
	"%s: el archivo ya existe.";
#elif L==5
	"%s: file already exists.";	// Besoin de traduction
#elif L==6
	"%s: el archivo ya existe.";	// Okoteve traducción
#elif L==7
	"%s: file already exists.";	// Bisogno di traduzione
#elif L==8
	"%s: file already exists.";	// Potrzebujesz tlumaczenie
#elif L==9
	"%s: o arquivo já existe.";
#endif

const char *Txt_UPLOAD_FILE_File_too_large_maximum_X_MiB_NO_HTML =	// Warning: it is very important to include %lu in the following sentences
#if   L==1
	"Archivo demasiado grande (máximo %lu MiB) /"
	" un altre problema d'enviament.";	// Necessita traduccio
#elif L==2
	"Datei zu groß (Maximum %lu MiB) /"
	" ein weiteres Upload-Problem.";
#elif L==3
	"File too large (maximum %lu MiB) /"
	" another upload problem.";
#elif L==4
	"Archivo demasiado grande (máximo %lu MiB) /"
	" otro problema de envío.";
#elif L==5
	"Fichier est trop gros (maximum %lu MiB) /"
	" un autre problème de téléchargement.";
#elif L==6
	"Archivo demasiado grande (máximo %lu MiB) /"
	" otro problema de envío.";		// Okoteve traducción
#elif L==7
	"File troppo grande (massimo %lu MiB) /"
	" un altro problema di caricamento.";
#elif L==8
	"Plik jest za duzy (maksimum %lu MiB) /"
	" kolejny problem wgrac.";
#elif L==9
	"Arquivo muito grande (máximo %lu MiB) /"
	" outro problema de upload.";
#endif

const char *Txt_UPLOAD_FILE_Invalid_name_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"Nombre no válido.";	// Necessita traduccio
#elif L==2
	"Invalid name.";	// Need Übersetzung
#elif L==3
	"Invalid name.";
#elif L==4
	"Nombre no válido.";
#elif L==5
	"Invalid name.";	// Besoin de traduction
#elif L==6
	"Nombre no válido.";	// Okoteve traducción
#elif L==7
	"Nome non valido.";
#elif L==8
	"Invalid name.";	// Potrzebujesz tlumaczenie
#elif L==9
	"Nome inválido.";
#endif

const char *Txt_UPLOAD_FILE_X_invalid_name_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: nombre no válido.";	// Necessita traduccio
#elif L==2
	"%s: invalid name.";		// Need Übersetzung
#elif L==3
	"%s: invalid name.";
#elif L==4
	"%s: nombre no válido.";
#elif L==5
	"%s: invalid name.";		// Besoin de traduction
#elif L==6
	"%s: nombre no válido.";	// Okoteve traducción
#elif L==7
	"%s: nome non valido.";
#elif L==8
	"%s: invalid name.";		// Potrzebujesz tlumaczenie
#elif L==9
	"%s: nome inválido.";
#endif

const char *Txt_UPLOAD_FILE_X_MIME_type_Y_not_allowed_NO_HTML =	// Warning: it is very important to include two %s in the following sentences
#if   L==1
	"%s: tipo MIME %s no permitido.";	// Necessita traduccio
#elif L==2
	"%s: MIME-Typ %s nicht unterstützt.";
#elif L==3
	"%s: MIME type %s not allowed.";
#elif L==4
	"%s: tipo MIME %s no permitido.";
#elif L==5
	"%s: MIME type %s not allowed.";	// Besoin de traduction
#elif L==6
	"%s: tipo MIME %s no permitido.";	// Okoteve traducción
#elif L==7
	"%s: tipo MIME %s non consentito.";
#elif L==8
	"%s: MIME type %s not allowed.";	// Potrzebujesz tlumaczenie
#elif L==9
	"%s: Tipo MIME %s não permitido.";
#endif

const char *Txt_UPLOAD_FILE_X_not_HTML_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: no HTML.";		// Necessita traduccio
#elif L==2
	"%s: not HTML.";	// Need Übersetzung
#elif L==3
	"%s: not HTML.";
#elif L==4
	"%s: no HTML.";		// Bisogno di traduzione
#elif L==5
	"%s: not HTML.";	// Besoin de traduction
#elif L==6
	"%s: no HTML.";		// Okoteve traducción
#elif L==7
	"%s: not HTML.";
#elif L==8
	"%s: not HTML.";	// Potrzebujesz tlumaczenie
#elif L==9
	"%s: não HTML.";
#endif

const char *Txt_UPLOAD_FILE_X_quota_exceeded_NO_HTML =	// Warning: it is very important to include %s in the following sentences
#if   L==1
	"%s: cuota excedida.";		// Necessita traduccio
#elif L==2
	"%s: Kontingent überschritten.";
#elif L==3
	"%s: quota exceeded.";
#elif L==4
	"%s: cuota excedida.";
#elif L==5
	"%s: quota dépassé.";
#elif L==6
	"%s: cuota excedida.";		// Okoteve traducción
#elif L==7
	"%s: quota superata.";
#elif L==8
	"%s: przekroczono przydzial.";
#elif L==9
	"5s: quota excedida.";
#endif

const char *Txt_UPLOAD_FILE_Upload_time_too_long_maximum_X_minutes_NO_HTML =	// Warning: it is very important to include %lu in the following sentences
#if   L==1
	"Tiempo de subida demasiado largo (máximo %lu minutos).";	// Necessita traduccio
#elif L==2
	"Upload ist zu lang (Maximum %lu Minute).";
#elif L==3
	"Upload time too long (maximum %lu minutes).";
#elif L==4
	"Tiempo de subida demasiado largo (máximo %lu minutos).";
#elif L==5
	"Temps de téléchargement trop longs (maximum %lu minutes).";
#elif L==6
	"Tiempo de subida demasiado largo (máximo %lu minutos).";	// Okoteve traducción
#elif L==7
	"Durata del caricamento troppo lungo (massimo %lu minuti).";
#elif L==8
	"Upload time too long (maximum %lu minuty).";			// Potrzebujesz tlumaczenie
#elif L==9
	"Tempo de upload muito longo (máximo %lu minutos).";
#endif

const char *Txt_user_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES] =	// Don't use HTML entities like &egrave; here
{
	{
	// Usr_SEX_UNKNOWN
	"",
	"usuari/a",
	"Benutzer",
	"user",
	"usuario/a",
	"utilisateur/se",
	"puruhára",
	"utente",
	"uzytkownika",
	"utilizador/a",
	},
	{
	// Usr_SEX_FEMALE
	"",
	"usuaria",
	"Benutzer",
	"user",
	"usuaria",
	"utilisateuse",
	"usuaria",	// Okoteve traducción
	"utente",
	"uzytkownika",
	"utilizadora",
	},
	{
	// Usr_SEX_MALE
	"",
	"usuari",
	"Benutzer",
	"user",
	"usuario",
	"utilisateur",
	"puruhára",
	"utente",
	"uzytkownika",
	"utilizador",
	},
	{
	// Usr_SEX_ALL
	"",
	"usuari/a",
	"Benutzer",
	"user",
	"usuario/a",
	"utilisateur/se",
	"puruhára",
	"utente",
	"uzytkownika",
	"utilizador/a",
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
	"Puruh&aacute;ra";
#elif L==7
	"Utenti";
#elif L==8
	"Uzytkowników";
#elif L==9
	"Utilizadores";
#endif
