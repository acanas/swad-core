// swad_action.c: actions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include <stddef.h>		// For NULL
#include <stdio.h>		// For fprintf
#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_action.h"
#include "swad_agenda.h"
#include "swad_announcement.h"
#include "swad_banner.h"
#include "swad_calendar.h"
#include "swad_centre_config.h"
#include "swad_config.h"
#include "swad_cookie.h"
#include "swad_country.h"
#include "swad_course.h"
#include "swad_course_config.h"
#include "swad_chat.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_degree_config.h"
#include "swad_degree_type.h"
#include "swad_duplicate.h"
#include "swad_exam.h"
#include "swad_enrolment.h"
#include "swad_figure.h"
#include "swad_follow.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_indicator.h"
#include "swad_language.h"
#include "swad_log.h"
#include "swad_mail.h"
#include "swad_mark.h"
#include "swad_match.h"
#include "swad_match_result.h"
#include "swad_MFU.h"
#include "swad_network.h"
#include "swad_nickname.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_project.h"
#include "swad_QR.h"
#include "swad_report.h"
#include "swad_role.h"
#include "swad_search.h"
#include "swad_setting.h"
#include "swad_setup.h"
#include "swad_tab.h"
#include "swad_test_import.h"
#include "swad_timeline.h"
#include "swad_zip.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/
/*
1380 actions in one CGI:
	  0. ActAll			Any action (used for statistics)
	  1. ActUnk			Unknown action
	  2. ActMnu			Show menu of a tab
	  3. ActRefCon			Refresh number of notifications and connected users via AJAX
	  4. ActWebSvc			Call plugin function

Start:
          5. ActFrmLogIn		Show landing page (forms to log in and to create a new account)
	  6. ActReqSch			Request search in system tab
          7. ActSeeSocTmlGbl		Show social timeline (global)
          8. ActSeeSocPrf		Suggest list of users to follow
	  9. ActSeeCal			Show the academic calendar
         10. ActSeeNtf			Show my recent notifications

         11. ActLogIn			Authentify user internally (directly from the platform)
         12. ActLogInNew		Authentify user internally (directly from the platform, only if user has not password)
         13. ActLogInLan		Change language to my language just after authentication
         14. ActAnnSee			Mark announcement as seen
         15. ActReqSndNewPwd		Show form to send a new password via email
         16. ActSndNewPwd		Send a new password via email
         17. ActLogOut			Close session

	 18. ActSch			Search for courses, teachers, documents...

	 19. ActRefNewSocPubGbl		Refresh recent social timeline via AJAX
	 20. ActRefOldSocPubGbl		View old social timeline with users I follow via AJAX
         21. ActRcvSocPstGbl		Receive a public social post to be displayed in the timeline (global)
         22. ActRcvSocComGbl		Comment a social note in the timeline (global)
         23. ActShoHidSocComGbl		Show hidden comments in a social note (global)
         24. ActAllShaSocNotGbl		Show all users who have shared a note in the timeline (global)
         25. ActAllFavSocNotGbl		Show all users who have favourited a note in the timeline (global)
         26. ActAllFavSocComGbl		Show all users who have favourited a comment in the timeline (global)
         27. ActShaSocNotGbl		Share a social note in the timeline (global)
         28. ActUnsSocNotGbl		Unshare a previously shared social note in the timeline (global)
         29. ActFavSocNotGbl		Favourite a social note in the timeline (global)
         30. ActUnfSocNotGbl		Unfavourite a previously favourited social note in the timeline (global)
         31. ActFavSocComGbl		Favourite a social comment in the timeline (global)
         32. ActUnfSocComGbl		Unfavourite a previously favourited social comment in the timeline (global)
         33. ActReqRemSocPubGbl		Request the removal of a social publishing in the timeline (global)
         34. ActRemSocPubGbl		Remove a social publishing in the timeline (global)
         35. ActReqRemSocComGbl		Request the removal of a comment in a social note (global)
         36. ActRemSocComGbl		Remove of a comment in a social note (global)

         37. ActReqOthPubPrf		Request @nickname to show a public user's profile

	 38. ActRefOldSocPubUsr		View old social timeline of a user via AJAX
         39. ActRcvSocPstUsr		Receive a public social post to be displayed in the timeline (user)
         40. ActRcvSocComUsr		Comment a social note in the timeline (user)
         41. ActShoHidSocComUsr		Show hidden comments in a social note (user)
         42. ActAllShaSocNotUsr		Show all users who have shared a note in the timeline (user)
         43. ActAllFavSocNotUsr		Show all users who have favourited a note in the timeline (user)
         44. ActAllFavSocComUsr		Show all users who have favourited a comment in the timeline (user)
         45. ActShaSocNotUsr		Share a social note in the timeline (user)
         46. ActUnsSocNotUsr		Unshare a previously shared social note in the timeline (user)
         47. ActFavSocNotUsr		Favourite a social note in the timeline (user)
         48. ActUnfSocNotUsr		Unfavourite a previously favourited social note in the timeline (user)
         49. ActFavSocComUsr		Favourite a social comment in the timeline (user)
         50. ActUnfSocComUsr		Unfavourite a previously favourited social comment in the timeline (user)
         51. ActReqRemSocPubUsr		Request the removal of a social publishing in the timeline (user)
         52. ActRemSocPubUsr		Remove a social publishing in the timeline (user)
         53. ActReqRemSocComUsr		Request the removal of a comment in a social note (user)
         54. ActRemSocComUsr		Remove of a comment in a social note (user)

         55. ActCalFig			Calculate user's figures and store into user's figures

         56. ActFolUsr			Follow another user
         57. ActUnfUsr			Unfollow another user
         58. ActSeeFlg			Show following
         59. ActSeeFlr			Show followers

	 60. ActPrnSysCrs		Show print view of the academic calendar
	 61. ActChgSysCrs1stDay		Change first day of week and show academic calendar

         62. ActSeeNewNtf		Show my recent notifications (link in top heading)
         63. ActMrkNtfSee		Mark all my notifications as seen
         64. ActSeeMai			See mail domains
         65. ActEdiMai			Edit mail domains
         66. ActNewMai			Request the creation of a mail domain
         67. ActRemMai			Request the removal of a mail domain
         68. ActRenMaiSho		Request the change of a mail domain
         69. ActRenMaiFul		Request the change of the info of a mail domain

System:
	 70. ActSeeCty			List countries
	 71. ActSeePen			List pending institutions, centres, degrees and courses in hierarchy
	 72. ActSeeLnk			See institutional links
	 73. ActLstPlg			List plugins
	 74. ActSetUp			Initial set up

	 75. ActEdiCty			Edit countries
	 76. ActNewCty			Request the creation of a country
	 77. ActRemCty			Remove a country
	 78. ActRenCty			Change the name of a country
	 79. ActChgCtyWWW		Change web of country

	 80. ActSeeDegTyp		List types of degrees
	 81. ActEdiDegTyp		Request edition of types of degrees
	 82. ActNewDegTyp		Request the creation of a type of degree
	 83. ActRemDegTyp		Request the removal of a type of degree
	 84. ActRenDegTyp		Request renaming of a type of degree

	 85. ActReqRemOldCrs		Request the total removal of old courses
	 86. ActRemOldCrs		Remove completely old courses

	 87. ActSeeBan			See banners
	 88. ActEdiBan			Edit banners
	 89. ActNewBan			Request the creation of a banner
	 90. ActRemBan			Request the removal of a banner
	 91. ActShoBan			Show a hidden banner
	 92. ActHidBan			Hide a visible banner
	 93. ActRenBanSho		Request the change of the short name of a banner
	 94. ActRenBanFul		Request the change of the full name of a banner
	 95. ActChgBanImg		Request the change of the image of a banner
	 96. ActChgBanWWW		Request the change of the web of a banner
	 97. ActClkBan			Go to a banner when clicked

	 98. ActEdiLnk			Edit institutional links
	 99. ActNewLnk			Request the creation of an institutional link
	100. ActRemLnk			Request the removal of an institutional link
	101. ActRenLnkSho		Request the change of the short name of an institutional link
	102. ActRenLnkFul		Request the change of the full name of an institutional link
	103. ActChgLnkWWW		Request the change of the web of an institutional link
	104. ActEdiPlg			Edit plugins
	105. ActNewPlg			Request the creation of a plugin
	106. ActRemPlg			Remove a plugin
	107. ActRenPlg			Rename a plugin
	108. ActChgPlgDes		Change the description of a plugin
	109. ActChgPlgLog		Change the logo of a plugin
	110. ActChgPlgAppKey		Change the application key of a plugin
	111. ActChgPlgURL		Change the URL of a plugin
	112. ActChgPlgIP		Change the IP address of a plugin

Country:
	113. ActSeeCtyInf		Show information on the current country
	114. ActSeeIns			List institutions

	115. ActPrnCtyInf		Print information on the current country
	116. ActChgCtyMapAtt		Change map attribution of country

	117. ActEdiIns			Edit institutions
	118. ActReqIns			Request the creation of a request for a new institution (a teacher makes the petition to an administrator)
	119. ActNewIns			Request the creation of an institution
	120. ActRemIns			Remove institution
	121. ActRenInsSho		Change short name of institution
	122. ActRenInsFul		Change full name of institution
	123. ActChgInsWWW		Change web of institution
	124. ActChgInsSta		Request change of status of an institution

	125. ActPrnCtyCrs		Show print view of the academic calendar
	126. ActChgCtyCrs1stDay		Change first day of week and show academic calendar

Institution:
	127. ActSeeInsInf		Show information on the current institution
	128. ActSeeCtr			List centres
	129. ActSeePlc			List places
	130. ActSeeDpt			List departments

	131. ActPrnInsInf		Print information on the current institution
	132. ActChgInsCtyCfg		Change country of institution in institution configuration
	133. ActRenInsShoCfg		Change short name of institution in institution configuration
	134. ActRenInsFulCfg		Change full name of institution in institution configuration
	135. ActChgInsWWWCfg		Change web of institution in institution configuration
	136. ActReqInsLog		Show form to send the logo of the current institution
	137. ActRecInsLog		Receive and store the logo of the current institution
	138. ActRemInsLog		Remove the logo of the current institution

	139. ActEdiCtr			Edit centres
	140. ActReqCtr			Request the creation of a request for a new centre (a teacher makes the petition to an administrator)
	141. ActNewCtr			Request the creation of a centre
	142. ActRemCtr			Remove centre
	143. ActChgCtrPlc		Request change of the place of a centre
	144. ActRenCtrSho		Change short name centre
	145. ActRenCtrFul		Change full name centre
	146. ActChgCtrWWW		Change web of centre
	147. ActChgCtrSta		Request change of status of a centre

	148. ActEdiDpt			Edit departments
	149. ActNewDpt			Request the creation of a department
	150. ActRemDpt			Remove department
	151. ActChgDptIns		Request change of the institution of a department
	152. ActRenDptSho		Change short name department
	153. ActRenDptFul		Change full name department
	154. ActChgDptWWW		Change web of department

	155. ActEdiPlc			Edit places
	156. ActNewPlc			Request the creation of a place
	157. ActRemPlc			Remove a place
	158. ActRenPlcSho		Change short name of a place
	159. ActRenPlcFul		Change full name of a place

	160. ActSeeHld			List holidays
	161. ActEdiHld			Edit holidays
	162. ActNewHld			Request the creation of a holiday
	163. ActRemHld			Remove a holiday
	164. ActChgHldPlc		Change place of a holiday
	165. ActChgHldTyp		Change type of a holiday
	166. ActChgHldStrDat		Change start date of a holiday
	167. ActChgHldEndDat		Change end date of a holiday
	168. ActRenHld			Change name of a holiday

	169. ActPrnInsCrs		Show print view of the academic calendar
	170. ActChgInsCrs1stDay		Change first day of week and show academic calendar

Centre:
	171. ActSeeCtrInf		Show information on the current centre
	172. ActSeeDeg			List degrees in a centre
	173. ActSeeCla			List classrooms in a centre

	174. ActPrnCtrInf		Print information on the current centre
	175. ActChgCtrInsCfg		Request change of the institution of a centre in centre configuration
	176. ActRenCtrShoCfg		Change short name centre in centre configuration
	177. ActRenCtrFulCfg		Change full name centre in centre configuration
	178. ActChgCtrPlcCfg		Change place of centre in centre configuration
	NEW. ActChgCtrLatCfg		Change latitude of centre in centre configuration
	NEW. ActChgCtrLgtCfg		Change longitude of centre in centre configuration
	NEW. ActChgCtrAltCfg		Change altitude of centre in centre configuration
	179. ActChgCtrWWWCfg		Change web of centre in centre configuration
	180. ActReqCtrLog		Show form to send the logo of the current centre
	181. ActRecCtrLog		Receive and store the logo of the current centre
	182. ActRemCtrLog		Remove the logo of the current centre
	183. ActReqCtrPho		Show form to send the photo of the current centre
	184. ActRecCtrPho		Receive and store the photo of the current centre
	185. ActChgCtrPhoAtt		Change attribution of centre photo

	186. ActEdiDeg			Request edition of degrees of a type
	187. ActReqDeg			Request the creation of a request for a new degree (a teacher makes the petition to an administrator)
	188. ActNewDeg			Request the creation of a degree
	189. ActRemDeg			Request the removal of a degree
	190. ActRenDegSho		Request change of the short name of a degree
	191. ActRenDegFul		Request change of the full name of a degree
	192. ActChgDegTyp		Request change of the type of a degree
	193. ActChgDegWWW		Request change of the web of a degree
	194. ActChgDegSta		Request change of status of a degree

	195. ActEdiCla			Edit classrooms
	196. ActNewCla			Request the creation of a classroom
	197. ActRemCla			Remove a classroom
	198. ActRenClaSho		Change short name of a classroom
	199. ActRenClaFul		Change full name of a classroom
	200. ActChgClaMaxStd		Change number of students in a classroom
	201. ActRenClaLoc		Change location of a classroom

Degree:
	202. ActSeeDegInf		Show information on the current degree
	203. ActSeeCrs			List courses of a degree

	204. ActPrnDegInf		Print information on the current degree
	205. ActChgDegCtrCfg		Request change of the centre of a degree in degree configuration
	206. ActRenDegShoCfg		Request change of the short name of a degree in degree configuration
	207. ActRenDegFulCfg		Request change of the full name of a degree in degree configuration
	208. ActChgDegWWWCfg		Request change of the web of a degree in degree configuration
	209. ActReqDegLog		Show form to send the logo of the current degree
	210. ActRecDegLog		Receive and store the logo of the current degree
	211. ActRemDegLog		Remove the logo of the current degree

	212. ActEdiCrs			Request edition of courses of a degree
	213. ActReqCrs			Request the creation of a request for a new course (a teacher makes the petition to an administrator)
	214. ActNewCrs			Request the creation of a course
	215. ActRemCrs			Request the removal of a course
	216. ActChgInsCrsCod		Request change of institutional code of a course
	217. ActChgCrsYea		Request change of year of a course inside of its degree
	218. ActRenCrsSho		Request change of short name of a course
	219. ActRenCrsFul		Request change of full name of a course
	220. ActChgCrsSta		Request change of status of a course

Course:
	221. ActSeeCrsInf		Show information on the current course
	222. ActSeeTchGui		Show teaching guide of the course
	223. ActSeeSyl			Show syllabus (lectures or practicals)
	224. ActSeeSylLec		Show the syllabus of lectures
	225. ActSeeSylPra		Show the syllabus of practicals
	226. ActSeeBib			Show the bibliography
	227. ActSeeFAQ			Show the FAQ
	228. ActSeeCrsLnk		Show links related to the course
	229. ActSeeCrsTT		Show the timetable

	230. ActPrnCrsInf		Print information on the course
	231. ActChgCrsDegCfg		Request change of degree in course configuration
	232. ActRenCrsShoCfg		Request change of short name of a course in course configuration
	233. ActRenCrsFulCfg		Request change of full name of a course in course configuration
	234. ActChgInsCrsCodCfg		Change institutional code in course configuration
	235. ActChgCrsYeaCfg		Change year/semester in course configuration
	236. ActEdiCrsInf		Edit general information about the course
	237. ActEdiTchGui		Edit teaching guide of the course
	238. ActEdiSylLec		Edit the syllabus of lectures
	239. ActEdiSylPra		Edit the syllabus of practicals
	240. ActDelItmSylLec		Remove a item from syllabus of lectures
	241. ActDelItmSylPra		Remove a item from syllabus of practicals
	242. ActUp_IteSylLec		Subir the posición of a subtree of the syllabus of lectures
	243. ActUp_IteSylPra		Subir the posición of a subtree of the syllabus of practicals
	244. ActDwnIteSylLec		Bajar the posición of a subtree of the syllabus of lectures
	245. ActDwnIteSylPra		Bajar the posición of a subtree of the syllabus of practicals
	246. ActRgtIteSylLec		Aumentar the level of a item of the syllabus of lectures
	247. ActRgtIteSylPra		Aumentar the level of a item of the syllabus of practicals
	248. ActLftIteSylLec		Disminuir the level of a item of the syllabus of lectures
	249. ActLftIteSylPra		Disminuir the level of a item of the syllabus of practicals
	250. ActInsIteSylLec		Insertar a new item in the syllabus of lectures
	251. ActInsIteSylPra		Insertar a new item in the syllabus of practicals
	252. ActModIteSylLec		Modify a item of the syllabus of lectures
	253. ActModIteSylPra		Modify a item of the syllabus of practicals

	254. ActEdiBib			Edit the bibliography
	255. ActEdiFAQ			Edit the FAQ
	256. ActEdiCrsLnk		Edit the links relacionados with the course
	257. ActChgFrcReaCrsInf		Change force students to read course info
	258. ActChgFrcReaTchGui		Change force students to read teaching guide
	259. ActChgFrcReaSylLec		Change force students to read lectures syllabus
	260. ActChgFrcReaSylPra		Change force students to read practicals syllabus
	261. ActChgFrcReaBib		Change force students to read bibliography
	262. ActChgFrcReaFAQ		Change force students to read FAQ
	263. ActChgFrcReaCrsLnk		Change force students to read links
	264. ActChgHavReaCrsInf		Change if I have read course info
	265. ActChgHavReaTchGui		Change if I have read teaching guide
	266. ActChgHavReaSylLec		Change if I have read lectures syllabus
	267. ActChgHavReaSylPra		Change if I have read practicals syllabus
	268. ActChgHavReaBib		Change if I have read bibliography
	269. ActChgHavReaFAQ		Change if I have read FAQ
	270. ActChgHavReaCrsLnk		Change if I have read links
	271. ActSelInfSrcCrsInf		Select the type of info shown in the general information about the course
	272. ActSelInfSrcTchGui		Select the type of info shown in the teaching guide
	273. ActSelInfSrcSylLec		Select the type of info shown in the lectures syllabus
	274. ActSelInfSrcSylPra		Select the type of info shown in the practicals syllabus
	275. ActSelInfSrcBib		Select the type of info shown in the bibliography
	276. ActSelInfSrcFAQ		Select the type of info shown in the FAQ
	277. ActSelInfSrcCrsLnk		Select the type of info shown in the links
	278. ActRcvURLCrsInf		Receive a link a the general information about the course
	279. ActRcvURLTchGui		Receive a link a the teaching guide
	280. ActRcvURLSylLec		Receive a link al syllabus of lectures
	281. ActRcvURLSylPra		Receive a link al syllabus of practicals
	282. ActRcvURLBib		Receive a link a bibliography
	283. ActRcvURLFAQ		Receive a link a FAQ
	284. ActRcvURLCrsLnk		Receive a link a links
	285. ActRcvPagCrsInf		Receive a page with the general information about the course
	286. ActRcvPagTchGui		Receive a page with the teaching guide
	287. ActRcvPagSylLec		Receive a page with the syllabus of lectures
	288. ActRcvPagSylPra		Receive a page with the syllabus of practicals
	289. ActRcvPagBib		Receive a page with bibliography
	290. ActRcvPagFAQ		Receive a page with FAQ
	291. ActRcvPagCrsLnk		Receive a page with links
	292. ActEditorCrsInf		Integrated editor of the general information about the course
	293. ActEditorTchGui		Integrated editor of the teaching guide
	294. ActEditorSylLec		Integrated editor of the syllabus of lectures
	295. ActEditorSylPra		Integrated editor of the syllabus of practicals
	296. ActEditorBib		Integrated editor of bibliography
	297. ActEditorFAQ		Integrated editor of FAQ
	298. ActEditorCrsLnk		Integrated editor of links
	299. ActPlaTxtEdiCrsInf		Editor of plain text of the general information about the course
	300. ActPlaTxtEdiTchGui		Editor of plain text of the teaching guide
	301. ActPlaTxtEdiSylLec		Editor of plain text of the syllabus of lectures
	302. ActPlaTxtEdiSylPra		Editor of plain text of the syllabus of practicals
	303. ActPlaTxtEdiBib		Editor of plain text of the bibliography
	304. ActPlaTxtEdiFAQ		Editor of plain text of the FAQ
	305. ActPlaTxtEdiCrsLnk		Editor of plain text of the links
	306. ActRchTxtEdiCrsInf		Editor of plain text of the general information about the course
	307. ActRchTxtEdiTchGui		Editor of plain text of the teaching guide
	308. ActRchTxtEdiSylLec		Editor of plain text of the syllabus of lectures
	309. ActRchTxtEdiSylPra		Editor of plain text of the syllabus of practicals
	310. ActRchTxtEdiBib		Editor of plain text of the bibliography
	311. ActRchTxtEdiFAQ		Editor of plain text of the FAQ
	312. ActRchTxtEdiCrsLnk		Editor of plain text of the links
	313. ActRcvPlaTxtCrsInf		Receive and change the plain text of the general information about the course
	314. ActRcvPlaTxtTchGui		Receive and change the plain text of the teaching guide
	315. ActRcvPlaTxtSylLec		Receive and change the plain text of the syllabus of lectures
	316. ActRcvPlaTxtSylPra		Receive and change the plain text of the syllabus of practicals
	317. ActRcvPlaTxtBib		Receive and change the plain text of the bibliography
	318. ActRcvPlaTxtFAQ		Receive and change the plain text of the FAQ
	319. ActRcvPlaTxtCrsLnk		Receive and change the plain text of the links
	320. ActRcvPlaTxtCrsInf		Receive and change the rich text of the general information about the course
	321. ActRcvPlaTxtTchGui		Receive and change the rich text of the teaching guide
	322. ActRcvPlaTxtSylLec		Receive and change the rich text of the syllabus of lectures
	323. ActRcvPlaTxtSylPra		Receive and change the rich text of the syllabus of practicals
	324. ActRcvPlaTxtBib		Receive and change the rich text of the bibliography
	325. ActRcvPlaTxtFAQ		Receive and change the rich text of the FAQ
	326. ActRcvPlaTxtCrsLnk		Receive and change the rich text of the links

	327. ActPrnCrsTT		Show print view of the timetable
	328. ActEdiCrsTT		Edit the timetable
	329. ActChgCrsTT		Modify the timetable of the course
        330. ActChgCrsTT1stDay		Change first day of week and show timetable of the course

Assessment:
	331. ActSeeAss			Show the assessment system
	332. ActSeeAsg			Show assignments
	333. ActSeePrj			Show projects
	334. ActReqTst			Request a test of self-assesment
        335. ActSeeAllGam		Remote control
        336. ActSeeAllSvy		List all surveys in pages
	337. ActSeeAllExaAnn		Show the exam announcements

	338. ActEdiAss			Edit the assessment system
	339. ActChgFrcReaAss		Change force students to read assessment system
	340. ActChgHavReaAss		Change if I have read assessment system
	341. ActSelInfSrcAss		Selec. type of assessment
	342. ActRcvURLAss		Receive a link a assessment
	343. ActRcvPagAss		Receive a page with assessment
	344. ActEditorAss		Integrated editor of assessment
	345. ActPlaTxtEdiAss		Editor of plain text of assessment
	346. ActRchTxtEdiAss		Editor of rich text of assessment
	347. ActRcvPlaTxtAss		Receive and change the plain text of the assessment system
	348. ActRcvRchTxtAss		Receive and change the rich text of the assessment system

	349. ActFrmNewAsg		Form to create a new assignment
	350. ActEdiOneAsg		Edit one assignment
	351. ActPrnOneAsg		Print one assignment
	352. ActNewAsg			Create new assignment
	353. ActChgAsg			Modify data of an existing assignment
	354. ActReqRemAsg		Request the removal of an assignment
	355. ActRemAsg			Remove assignment
	356. ActHidAsg			Hide assignment
	357. ActShoAsg			Show assignment

        358. ActReqUsrPrj		Select users to list their projects
	359. ActSeeTblAllPrj		Show all projects in a table
	360. ActCfgPrj			Configure all projects
	361. ActRcvCfgPrj		Receive configuration of all projects
	362. ActReqLckAllPrj		Request locking of all projects
	363. ActReqUnlAllPrj		Request unlocking of all projects
	364. ActLckAllPrj		Lock all projects
	365. ActUnlAllPrj		Unlock all projects

	366. ActFrmNewPrj		Form to create a new project
	367. ActEdiOnePrj		Edit one project
	368. ActPrnOnePrj		Print one project
	369. ActNewPrj			Create new project
	370. ActChgPrj			Modify data of an existing project
	371. ActReqRemPrj		Request the removal of an project
	372. ActRemPrj			Remove project
	373. ActHidPrj			Hide project
	374. ActShoPrj			Show project
	375. ActLckPrj			Lock project edition
	376. ActUnlPrj			Unlock project edition
	377. ActReqAddStdPrj		Request adding a student to a project
	378. ActReqAddTutPrj		Request adding a tutor to a project
	379. ActReqAddEvlPrj		Request adding an evaluator to a project
	380. ActAddStdPrj		Add a student to a project
	381. ActAddTutPrj		Add a tutor to a project
	382. ActAddEvlPrj		Add an evaluator to a project
	383. ActReqRemStdPrj		Request removing a student from a project
	384. ActReqRemTutPrj		Request removing a tutor from a project
	385. ActReqRemEvlPrj		Request removing an evaluator from a project
	386. ActRemStdPrj		Remove a student from a project
	387. ActRemTutPrj		Remove a tutor from a project
	388. ActRemEvlPrj		Remove an emulator from a project

	389. ActAdmDocPrj		Admin. project documents
        390. ActReqRemFilDocPrj		Request removal of a file from project documents
        391. ActRemFilDocPrj		Remove a file from project documents
        392. ActRemFolDocPrj		Remove an empty folder from project documents
        393. ActCopDocPrj		Set source of copy in project documents
        394. ActPasDocPrj		Paste a folder or file into project documents
        395. ActRemTreDocPrj		Remove a non empty folder from project documents
        396. ActFrmCreDocPrj		Form to create a folder or file in project documents
        397. ActCreFolDocPrj		Create a new folder in project documents
        398. ActCreLnkDocPrj		Create a new link in project documents
        399. ActRenFolDocPrj		Rename a folder in project documents
        400. ActRcvFilDocPrjDZ		Receive a file in project documents using Dropzone.js
        401. ActRcvFilDocPrjCla		Receive a file in project documents using the classic way
        402. ActExpDocPrj		Expand a folder in project documents
        403. ActConDocPrj		Contract a folder in project documents
        404. ActZIPDocPrj		Compress a folder in project documents
        405. ActReqDatDocPrj		Ask for metadata of a file in project documents
        406. ActChgDatDocPrj		Change metadata of a file in project documents
        407. ActDowDocPrj		Download a file from project documents

	408. ActAdmAssPrj		Admin. project assessment
        409. ActReqRemFilAssPrj		Request removal of a file from project assessment
        410. ActRemFilAssPrj		Remove a file from project assessment
        411. ActRemFolAssPrj		Remove an empty folder from project assessment
        412. ActCopAssPrj		Set source of copy in project assessment
        413. ActPasAssPrj		Paste a folder or file into project assessment
        414. ActRemTreAssPrj		Remove a non empty folder from project assessment
        415. ActFrmCreAssPrj		Form to create a folder or file in project assessment
        416. ActCreFolAssPrj		Create a new folder in project assessment
        417. ActCreLnkAssPrj		Create a new link in project assessment
        418. ActRenFolAssPrj		Rename a folder in project assessment
        419. ActRcvFilAssPrjDZ		Receive a file in project assessment using Dropzone.js
        420. ActRcvFilAssPrjCla		Receive a file in project assessment using the classic way
        421. ActExpAssPrj		Expand a folder in project assessment
        422. ActConAssPrj		Contract a folder in project assessment
        423. ActZIPAssPrj		Compress a folder in project assessment
        424. ActReqDatAssPrj		Ask for metadata of a file in project assessment
        425. ActChgDatAssPrj		Change metadata of a file in project assessment
        426. ActDowAssPrj		Download a file from project assessment

	427. ActSeeTst			Show the seft-assessment test
	428. ActAssTst			Assess a self-assessment test
	429. ActEdiTstQst		Request the edition of self-assessment questions
	430. ActEdiOneTstQst		Edit one self-assesment test question
	431. ActReqImpTstQst		Put form to ask for an XML with test questions to import
	432. ActImpTstQst		Import test questions from XML file
	433. ActLstTstQst		List for edition several self-assessment test questions
	434. ActRcvTstQst		Receive a question of self-assessment
	435. ActShfTstQst		Change shuffle of of a question of self-assessment
	436. ActReqRemTstQst		Request removing a self-assesment test question
	437. ActRemTstQst		Remove a self-assesment test question
	438. ActCfgTst			Request renaming of tags of questions of self-assesment
	439. ActEnableTag		Enable a tag
	440. ActDisableTag		Disable a tag
	441. ActRenTag			Rename a tag
	442. ActRcvCfgTst		Receive configuration of test
	443. ActReqSeeMyTstRes		Select range of dates to see my test results
	444. ActSeeMyTstRes		Show my test results
	445. ActReqSeeUsrTstRes		Select users and range of dates to see test results
	446. ActSeeUsrTstRes		Show test results of several users
	447. ActSeeOneTstResMe		Show one test result of me as student
	448. ActSeeOneTstResOth		Show one test result of other user

        449. ActSeeSvy			Show one survey
        450. ActAnsSvy			Answer a survey
        451. ActFrmNewSvy		Form to create a new survey
        452. ActEdiOneSvy		Edit one survey
        453. ActNewSvy			Create new survey
        454. ActChgSvy			Modify data of an existing survey
        455. ActReqRemSvy		Request the removal of a survey
        456. ActRemSvy			Remove survey
        457. ActReqRstSvy		Request the reset of answers of a survey
        458. ActRstSvy			Reset answers of survey
        459. ActHidSvy			Hide survey
        460. ActShoSvy			Show survey
        461. ActEdiOneSvyQst		Edit a new question for a survey
        462. ActRcvSvyQst		Receive a question of a survey
        463. ActReqRemSvyQst		Request the removal of a question of a survey
        464. ActRemSvyQst		Confirm the removal of a question of a survey

        465. ActSeeGam			Show one game

        466. ActReqRemMch		Request the removal of a match
        467. ActRemMch			Confirm the removal of a match
        468. ActReqNewMch		Put form to create a new match
        469. ActNewMch			Create a new match showing first question in a new browser tab
        470. ActResMch			Resume an unfinished match showing current question in a new browser tab
        471. ActBckMch			Go back when playing a match
        472. ActPlyPauMch		Play/pause current match
        473. ActFwdMch			Go forward when playing a match
        474. ActChgNumColMch		Change number of columns when playing a match
        475. ActChgVisResMchQst		Change visibility of question results when playing a match
        476. ActMchCntDwn		Start match countdown
        477. ActRefMchTch		Refresh current question when playing a match (as teacher)

        478. ActJoiMch			Show current question when playing a game (as student)
        479. ActSeeMchAnsQstStd		View my answer to a match question (as student)
        480. ActRemMchAnsQstStd		Remove my answer to a match question (as student)
        481. ActAnsMchQstStd		Answer a match question (as student)
        482. ActRefMchStd		Refresh current question when playing a game (as student)

	483. ActSeeMyMchResCrs		Show all my matches results in the current course
	484. ActSeeMyMchResGam		Show all my matches results in a given game
	485. ActSeeMyMchResMch		Show all my matches results in a given match
	486. ActSeeOneMchResMe		Show one match result of me as student

	487. ActReqSeeAllMchRes		Select users and range of dates to see matches results
	488. ActSeeAllMchResCrs		Show all matches results of selected users in the current course
	489. ActSeeAllMchResGam		Show all matches results in a given game
	490. ActSeeAllMchResMch		Show all matches results in a given game
	491. ActSeeOneMchResOth		Show one match result of other user

        492. ActChgVisResMchUsr		Change visibility of match results for students

        493. ActFrmNewGam		Form to create a new game
        494. ActEdiOneGam		Edit one game
        495. ActNewGam			Create new game
        496. ActChgGam			Modify data of an existing game
        497. ActReqRemGam		Request the removal of a game
        498. ActRemGam			Remove game
        499. ActHidGam			Hide game
        500. ActShoGam			Show game
        501. ActAddOneGamQst		Add a new question to a game
        502. ActGamLstTstQst		List test questions to select one or several questions
        503. ActAddTstQstToGam		Add selected test questions to game
        504. ActReqRemGamQst		Request the removal of a question of a game
        505. ActRemGamQst		Confirm the removal of a question of a game
        506. ActUp_GamQst		Move up a question of a game
        507. ActDwnGamQst		Move down a question of a game

	508. ActSeeOneExaAnn		Show one exam announcement
	509. ActSeeDatExaAnn		Show exam announcements of a given date
	510. ActEdiExaAnn		Edit an exam announcement
	511. ActRcvExaAnn		Receive an exam announcement
	512. ActPrnExaAnn		Show an exam announcement ready to be printed
	513. ActReqRemExaAnn		Request the removal of an exam announcement
	514. ActRemExaAnn		Remove an exam announcement
	515. ActHidExaAnn		Hide exam announcement
	516. ActShoExaAnn		Show exam announcement
Files:
	517. ActSeeAdmDocIns		Documents zone of the institution (see or admin)
	518. ActAdmShaIns		Admin the shared files zone of the institution
	519. ActSeeAdmDocCtr		Documents zone of the centre (see or admin)
	520. ActAdmShaCtr		Admin the shared files zone of the centre
	521. ActSeeAdmDocDeg		Documents zone of the degree (see or admin)
	522. ActAdmShaDeg		Admin the shared files zone of the degree
	523. ActSeeAdmDocCrsGrp		Documents zone of the course (see or admin)
	524. ActAdmTchCrsGrp		Request the administration of the files of the teachers' zone of the course or of a group
	525. ActAdmShaCrsGrp		Request the administration of the files of the shared zone of the course or of a group
        526. ActAdmAsgWrkUsr		One user sends works of the course
        527. ActReqAsgWrkCrs		A teacher requests edition of works sent to the course
	528. ActSeeAdmMrk		Marks zone (see or admin)
        529. ActAdmBrf			Show the briefcase of private archives

	530. ActChgToSeeDocIns		Change to see institution documents

	531. ActSeeDocIns		See the files of the documents zone of the institution
	532. ActExpSeeDocIns		Expand a folder in institution documents
	533. ActConSeeDocIns		Contract a folder in institution documents
	534. ActZIPSeeDocIns		Compress a folder in institution documents
	535. ActReqDatSeeDocIns		Ask for metadata of a file of the documents zone of the institution
	536. ActDowSeeDocIns		Download a file in documents zone of the institution

	537. ActChgToAdmDocIns		Change to administrate institution documents

	538. ActAdmDocIns		Admin the files of the documents zone of the institution
	539. ActReqRemFilDocIns		Request removal of a file in the documents zone of the institution
	540. ActRemFilDocIns		Remove a file in the documents zone of the institution
	541. ActRemFolDocIns		Remove a folder empty the documents zone of the institution
	542. ActCopDocIns		Set source of copy in the documents zone of the institution
	543. ActPasDocIns		Paste a folder or file in the documents zone of the institution
	544. ActRemTreDocIns		Remove a folder no empty in the documents zone of the institution
	545. ActFrmCreDocIns		Form to crear a folder or file in the documents zone of the institution
	546. ActCreFolDocIns		Create a new folder in the documents zone of the institution
	547. ActCreLnkDocIns		Create a new link in the documents zone of the institution
	548. ActRenFolDocIns		Rename a folder in the documents zone of the institution
	549. ActRcvFilDocInsDZ		Receive a file in the documents zone of the institution using Dropzone.js
	550. ActRcvFilDocInsCla		Receive a file in the documents zone of the institution using the classic way
	551. ActExpAdmDocIns		Expand a folder when administrating the documents zone of the institution
	552. ActConAdmDocIns		Contract a folder when administrating the documents zone of the institution
	553. ActZIPAdmDocIns		Compress a folder when administrating the documents zone of the institution
	554. ActShoDocIns		Show hidden folder or file of the documents zone of the institution
	555. ActHidDocIns		Hide folder or file of the documents zone of the institution
	556. ActReqDatAdmDocIns		Ask for metadata of a file of the documents zone of the institution
	557. ActChgDatAdmDocIns		Change metadata of a file of the documents zone of the institution
	558. ActDowAdmDocIns		Download a file of the documents zone of the institution

	559. ActReqRemFilShaIns		Request removal of a shared file of the institution
	560. ActRemFilShaIns		Remove a shared file of the institution
	561. ActRemFolShaIns		Remove an empty shared folder of the institution
	562. ActCopShaIns		Set source of copy in shared zone of the institution
	563. ActPasShaIns		Paste a folder or file in shared zone of the institution
	564. ActRemTreShaIns		Remove a folder no empty of shared zone of the institution
	565. ActFrmCreShaIns		Form to crear a folder or file in shared zone of the institution
	566. ActCreFolShaIns		Create a new folder in shared zone of the institution
	567. ActCreLnkShaIns		Create a new link in shared zone of the institution
	568. ActRenFolShaIns		Rename a folder in shared zone of the institution
	569. ActRcvFilShaInsDZ		Receive a file in shared zone of the institution using Dropzone.js
	570. ActRcvFilShaInsCla		Receive a file in shared zone of the institution using the classic way
	571. ActExpShaIns		Expand a folder in shared zone of the institution
	572. ActConShaIns		Contract a folder in shared zone of the institution
	573. ActZIPShaIns		Compress a folder in shared zone of the institution
	574. ActReqDatShaIns		Ask for metadata of a file of the shared zone of the institution
	575. ActChgDatShaIns		Change metadata of a file of the shared zone of the institution
	576. ActDowShaIns		Download a file of the shared zone of the institution

	577. ActChgToSeeDocCtr		Change to see centre documents

	578. ActSeeDocCtr		See the files of the documents zone of the centre
	579. ActExpSeeDocCtr		Expand a folder in centre documents
	580. ActConSeeDocCtr		Contract a folder in centre documents
	581. ActZIPSeeDocCtr		Compress a folder in centre documents
	582. ActReqDatSeeDocCtr		Ask for metadata of a file of the documents zone of the centre
	583. ActDowSeeDocCtr		Download a file in documents zone of the centre

	584. ActChgToAdmDocCtr		Change to administrate centre documents

	585. ActAdmDocCtr		Admin the files of the documents zone of the centre
	586. ActReqRemFilDocCtr		Request removal of a file in the documents zone of the centre
	587. ActRemFilDocCtr		Remove a file in the documents zone of the centre
	588. ActRemFolDocCtr		Remove a folder empty the documents zone of the centre
	589. ActCopDocCtr		Set source of copy in the documents zone of the centre
	590. ActPasDocCtr		Paste a folder or file in the documents zone of the centre
	591. ActRemTreDocCtr		Remove a folder no empty in the documents zone of the centre
	592. ActFrmCreDocCtr		Form to crear a folder or file in the documents zone of the centre
	593. ActCreFolDocCtr		Create a new folder in the documents zone of the centre
	594. ActCreLnkDocCtr		Create a new link in the documents zone of the centre
	595. ActRenFolDocCtr		Rename a folder in the documents zone of the centre
	596. ActRcvFilDocCtrDZ		Receive a file in the documents zone of the centre using Dropzone.js
	597. ActRcvFilDocCtrCla		Receive a file in the documents zone of the centre using the classic way
	598. ActExpAdmDocCtr		Expand a folder when administrating the documents zone of the centre
	599. ActConAdmDocCtr		Contract a folder when administrating the documents zone of the centre
	600. ActZIPAdmDocCtr		Compress a folder when administrating the documents zone of the centre
	601. ActShoDocCtr		Show hidden folder or file of the documents zone of the centre
	602. ActHidDocCtr		Hide folder or file of the documents zone of the centre
	603. ActReqDatAdmDocCtr		Ask for metadata of a file of the documents zone of the centre
	604. ActChgDatAdmDocCtr		Change metadata of a file of the documents zone of the centre
	605. ActDowAdmDocCtr		Download a file of the documents zone of the centre

	606. ActReqRemFilShaCtr		Request removal of a shared file of the centre
	607. ActRemFilShaCtr		Remove a shared file of the centre
	608. ActRemFolShaCtr		Remove an empty shared folder of the centre
	609. ActCopShaCtr		Set source of copy in shared zone of the centre
	610. ActPasShaCtr		Paste a folder or file in shared zone of the centre
	611. ActRemTreShaCtr		Remove a folder no empty of shared zone of the centre
	612. ActFrmCreShaCtr		Form to crear a folder or file in shared zone of the centre
	613. ActCreFolShaCtr		Create a new folder in shared zone of the centre
	614. ActCreLnkShaCtr		Create a new link in shared zone of the centre
	615. ActRenFolShaCtr		Rename a folder in shared zone of the centre
	616. ActRcvFilShaCtrDZ		Receive a file in shared zone of the centre using Dropzone.js
	617. ActRcvFilShaCtrCla		Receive a file in shared zone of the centre using the classic way
	618. ActExpShaCtr		Expand a folder in shared zone of the centre
	619. ActConShaCtr		Contract a folder in shared zone of the centre
	620. ActZIPShaCtr		Compress a folder in shared zone of the centre
	621. ActReqDatShaCtr		Ask for metadata of a file of the shared zone of the centre
	622. ActChgDatShaCtr		Change metadata of a file of the shared zone of the centre
	623. ActDowShaCtr		Download a file of the shared zone of the centre

	624. ActChgToSeeDocDeg		Change to see degree documents

	625. ActSeeDocDeg		See the files of the documents zone of the degree
	626. ActExpSeeDocDeg		Expand a folder in degree documents
	627. ActConSeeDocDeg		Contract a folder in degree documents
	628. ActZIPSeeDocDeg		Compress a folder in degree documents
	629. ActReqDatSeeDocDeg		Ask for metadata of a file of the documents zone of the degree
	630. ActDowSeeDocDeg		Download a file in documents zone of the degree

	631. ActChgToAdmDocDeg		Change to administrate degree documents

	632. ActAdmDocDeg		Admin the files of the documents zone of the degree
	633. ActReqRemFilDocDeg		Request removal of a file in the documents zone of the degree
	634. ActRemFilDocDeg		Remove a file in the documents zone of the degree
	635. ActRemFolDocDeg		Remove a folder empty the documents zone of the degree
	636. ActCopDocDeg		Set source of copy in the documents zone of the degree
	637. ActPasDocDeg		Paste a folder or file in the documents zone of the degree
	638. ActRemTreDocDeg		Remove a folder no empty in the documents zone of the degree
	639. ActFrmCreDocDeg		Form to crear a folder or file in the documents zone of the degree
	640. ActCreFolDocDeg		Create a new folder in the documents zone of the degree
	641. ActCreLnkDocDeg		Create a new link in the documents zone of the degree
	642. ActRenFolDocDeg		Rename a folder in the documents zone of the degree
	643. ActRcvFilDocDegDZ		Receive a file in the documents zone of the degree using Dropzone.js
	644. ActRcvFilDocDegCla		Receive a file in the documents zone of the degree using the classic way
	645. ActExpAdmDocDeg		Expand a folder when administrating the documents zone of the degree
	646. ActConAdmDocDeg		Contract a folder when administrating the documents zone of the degree
	647. ActZIPAdmDocDeg		Compress a folder when administrating the documents zone of the degree
	648. ActShoDocDeg		Show hidden folder or file of the documents zone of the degree
	649. ActHidDocDeg		Hide folder or file of the documents zone of the degree
	650. ActReqDatAdmDocDeg		Ask for metadata of a file of the documents zone of the degree
	651. ActChgDatAdmDocDeg		Change metadata of a file of the documents zone of the degree
	652. ActDowAdmDocDeg		Download a file of the documents zone of the degree

	653. ActReqRemFilShaDeg		Request removal of a shared file of the degree
	654. ActRemFilShaDeg		Remove a shared file of the degree
	655. ActRemFolShaDeg		Remove an empty shared folder of the degree
	656. ActCopShaDeg		Set source of copy in shared zone of the degree
	657. ActPasShaDeg		Paste a folder or file in shared zone of the degree
	658. ActRemTreShaDeg		Remove a folder no empty of shared zone of the degree
	659. ActFrmCreShaDeg		Form to crear a folder or file in shared zone of the degree
	660. ActCreFolShaDeg		Create a new folder in shared zone of the degree
	661. ActCreLnkShaDeg		Create a new link in shared zone of the degree
	662. ActRenFolShaDeg		Rename a folder in shared zone of the degree
	663. ActRcvFilShaDegDZ		Receive a file in shared zone of the degree using Dropzone.js
	664. ActRcvFilShaDegCla		Receive a file in shared zone of the degree using the classic way
	665. ActExpShaDeg		Expand a folder in shared zone of the degree
	666. ActConShaDeg		Contract a folder in shared zone of the degree
	667. ActZIPShaDeg		Compress a folder in shared zone of the degree
	668. ActReqDatShaDeg		Ask for metadata of a file of the shared zone of the degree
	669. ActChgDatShaDeg		Change metadata of a file of the shared zone of the degree
	670. ActDowShaDeg		Download a file of the shared zone of the degree

	671. ActChgToSeeDocCrs		Change to see course/group documents

	672. ActSeeDocCrs		See the files of the documents zone of the course
	673. ActExpSeeDocCrs		Expand a folder in course documents
	674. ActConSeeDocCrs		Contract a folder in course documents
	675. ActZIPSeeDocCrs		Compress a folder in course documents
	676. ActReqDatSeeDocCrs		Ask for metadata of a file of the documents zone of the course
	677. ActDowSeeDocCrs		Download a file in documents zone of the course

	678. ActSeeDocGrp		See the files of the documents zone of a group
	679. ActExpSeeDocGrp		Expand a folder in group documents
	680. ActConSeeDocGrp		Contract a folder in group documents
	681. ActZIPSeeDocGrp		Compress a folder in group documents
	682. ActReqDatSeeDocGrp		Ask for metadata of a file of the documents zone of a group
	683. ActDowSeeDocGrp		Download a file of the documents zone of a group

	684. ActChgToAdmDocCrs		Change to administrate course/group documents

	685. ActAdmDocCrs		Admin the files of the documents zone of the course
	686. ActReqRemFilDocCrs		Request removal of a file in the documents zone of the course
	687. ActRemFilDocCrs		Remove a file in the documents zone of the course
	688. ActRemFolDocCrs		Remove a folder empty the documents zone of the course
	689. ActCopDocCrs		Set source of copy in the documents zone of the course
	690. ActPasDocCrs		Paste a folder or file in the documents zone of the course
	691. ActRemTreDocCrs		Remove a folder no empty in the documents zone of the course
	692. ActFrmCreDocCrs		Form to crear a folder or file in the documents zone of the course
	693. ActCreFolDocCrs		Create a new folder in the documents zone of the course
	694. ActCreLnkDocCrs		Create a new link in the documents zone of the course
	695. ActRenFolDocCrs		Rename a folder in the documents zone of the course
	696. ActRcvFilDocCrsDZ		Receive a file in the documents zone of the course using Dropzone.js
	697. ActRcvFilDocCrsCla		Receive a file in the documents zone of the course using the classic way
	698. ActExpAdmDocCrs		Expand a folder when administrating the documents zone of the course
	699. ActConAdmDocCrs		Contract a folder when administrating the documents zone of the course
	700. ActZIPAdmDocCrs		Compress a folder when administrating the documents zone of the course
	701. ActShoDocCrs		Show hidden folder or file of the documents zone
	702. ActHidDocCrs		Hide folder or file of the documents zone
	703. ActReqDatAdmDocCrs		Ask for metadata of a file of the documents zone
	704. ActChgDatAdmDocCrs		Change metadata of a file of the documents zone
	705. ActDowAdmDocCrs		Download a file of the documents zone

	706. ActAdmDocGrp		Admin the files of the documents zone of a group
	707. ActReqRemFilDocGrp		Request removal of a file in the documents zone of a group
	708. ActRemFilDocGrp		Remove a file in the documents zone of a group
	709. ActRemFolDocGrp		Remove a folder empty in the documents zone of a group
	710. ActCopDocGrp		Set source of copy in the documents zone of a group
	711. ActPasDocGrp		Paste a folder or file in the documents zone of a group
	712. ActRemTreDocGrp		Remove a folder no empty in the documents zone of a group
	713. ActFrmCreDocGrp		Form to crear a folder or file in the documents zone of a group
	714. ActCreFolDocGrp		Create a new folder in the documents zone of a group
	715. ActCreLnkDocGrp		Create a new link in the documents zone of a group
	716. ActRenFolDocGrp		Rename a folder in the documents zone of a group
	717. ActRcvFilDocGrpDZ		Receive a file in the documents zone of a group using Dropzone.js
	718. ActRcvFilDocGrpCla		Receive a file in the documents zone of a group using the classic way
	719. ActExpAdmDocGrp		Expand a folder when administrating the documents zone of a group
	720. ActConAdmDocGrp		Contract a folder when administrating the documents zone of a group
	721. ActZIPAdmDocGrp		Compress a folder when administrating the documents zone of a group
	722. ActShoDocGrp		Show hidden folder or file of download
	723. ActHidDocGrp		Hide folder or file of download
	724. ActReqDatAdmDocGrp		Ask for metadata of a file of the documents zone of a group
	725. ActChgDatAdmDocGrp		Change metadata of a file of the documents zone of a group
	726. ActDowAdmDocGrp		Download a file of the documents zone of a group

	727. ActChgToAdmTch		Change to administrate teachers' files of the course/group

	728. ActAdmTchCrs		Admin the teachers' files zone of the course
	729. ActReqRemFilTchCrs		Request removal of a teachers' file of the course
	730. ActRemFilTchCrs		Remove a teachers' file of the course
	731. ActRemFolTchCrs		Remove an empty teachers' folder of the course
	732. ActCopTchCrs		Set source of teachers' zone of the course
	733. ActPasTchCrs		Paste a folder or file in teachers' zone of the course
	734. ActRemTreTchCrs		Remove a folder no empty of teachers' zone of the course
	735. ActFrmCreTchCrs		Form to crear a folder or file in teachers' zone of the course
	736. ActCreFolTchCrs		Create a new folder in teachers' zone of the course
	737. ActCreLnkTchCrs		Create a new link in teachers' zone of the course
	738. ActRenFolTchCrs		Rename a folder in teachers' zone of the course
	739. ActRcvFilTchCrsDZ		Receive a file in teachers' zone of the course using Dropzone.js
	740. ActRcvFilTchCrsCla		Receive a file in teachers' zone of the course using the classic way
	741. ActExpTchCrs		Expand a folder in teachers' zone of the course
	742. ActConTchCrs		Contract a folder in teachers' zone of the course
	743. ActZIPTchCrs		Compress a folder in teachers' zone of the course
	744. ActReqDatTchCrs		Ask for metadata of a file of the teachers' zone of the course
	745. ActChgDatTchCrs		Change metadata of a file of the teachers' zone of the course
	746. ActDowTchCrs		Download a file of the teachers' zone of the course

	747. ActAdmTchGrp		Admin the teachers' files zone of a group
	748. ActReqRemFilTchGrp		Request removal of a teachers' file of a group
	749. ActRemFilTchGrp		Remove a teachers' file of a group
	750. ActRemFolTchGrp		Remove an empty teachers' folder of a group
	751. ActCopTchGrp		Set source of copy in teachers' zone of a group
	752. ActPasTchGrp		Paste a folder or file in teachers' zone of a group
	753. ActRemTreTchGrp		Remove a folder no empty of teachers' zone of a group
	754. ActFrmCreTchGrp		Form to crear a folder or file in teachers' zone of a group
	755. ActCreFolTchGrp		Create a new folder in teachers' zone of a group
	756. ActCreLnkTchGrp		Create a new link in teachers' zone of a group
	757. ActRenFolTchGrp		Rename a folder in teachers' zone of a group
	758. ActRcvFilTchGrpDZ		Receive a file in teachers' zone of a group using Dropzone.js
	759. ActRcvFilTchGrpCla		Receive a file in teachers' zone of a group using the classic way
	760. ActExpTchGrp		Expand a folder in teachers' zone of a group
	761. ActConTchGrp		Contract a folder in teachers' zone of a group
	762. ActZIPTchGrp		Compress a folder in teachers' zone of a group
	763. ActReqDatTchGrp		Ask for metadata of a file of the teachers' zone of a group
	764. ActChgDatTchGrp		Change metadata of a file of the teachers' zone of a group
	765. ActDowTchGrp		Download a file of the teachers' zone of a group

	766. ActChgToAdmSha		Change to administrate shared files of the course/group

	767. ActAdmShaCrs		Admin the shared files zone of the course
	768. ActReqRemFilShaCrs		Request removal of a shared file of the course
	769. ActRemFilShaCrs		Remove a shared file of the course
	770. ActRemFolShaCrs		Remove a empty shared folder of the course
	771. ActCopShaCrs		Set source of copy in shared zone of the course
	772. ActPasShaCrs		Paste a folder or file in shared zone of the course
	773. ActRemTreShaCrs		Remove a folder no empty of shared zone of the course
	774. ActFrmCreShaCrs		Form to crear a folder or file in shared zone of the course
	775. ActCreFolShaCrs		Create a new folder in shared zone of the course
	776. ActCreLnkShaCrs		Create a new link in shared zone of the course
	777. ActRenFolShaCrs		Rename a folder in shared zone of the course
	778. ActRcvFilShaCrsDZ		Receive a file in shared zone of the course using Dropzone.js
	779. ActRcvFilShaCrsCla		Receive a file in shared zone of the course using the classic way
	780. ActExpShaCrs		Expand a folder in shared zone of the course
	781. ActConShaCrs		Contract a folder in shared zone of the course
	782. ActZIPShaCrs		Compress a folder in shared zone of the course
	783. ActReqDatShaCrs		Ask for metadata of a file of the shared zone of the course
	784. ActChgDatShaCrs		Change metadata of a file of the shared zone of the course
	785. ActDowShaCrs		Download a file of the shared zone of the course

	786. ActAdmShaGrp		Admin the shared files zone of a group
	787. ActReqRemFilShaGrp		Request removal of a shared file of a group
	788. ActRemFilShaGrp		Remove a shared file of a group
	789. ActRemFolShaGrp		Remove a folder empty común of a group
	790. ActCopShaGrp		Set source of copy in shared zone of a group
	791. ActPasShaGrp		Paste a folder or file in shared zone of a group
	792. ActRemTreShaGrp		Remove a folder no empty of shared zone of a group
	793. ActFrmCreShaGrp		Form to crear a folder or file in shared zone of a group
	794. ActCreFolShaGrp		Create a new folder in shared zone of a group
	795. ActCreLnkShaGrp		Create a new link in shared zone of a group
	796. ActRenFolShaGrp		Rename a folder in shared zone of a group
	797. ActRcvFilShaGrpDZ		Receive a file in shared zone of a group using Dropzone.js
	798. ActRcvFilShaGrpCla		Receive a file in shared zone of a group using the classic way
	799. ActExpShaGrp		Expand a folder in shared zone of a group
	800. ActConShaGrp		Contract a folder in shared zone of a group
	801. ActZIPShaGrp		Compress a folder in shared zone of a group
	802. ActReqDatShaGrp		Ask for metadata of a file of the shared zone of a group
	803. ActChgDatShaGrp		Change metadata of a file of the shared zone of a group
	804. ActDowShaGrp		Download a file of the shared zone of a group

        805. ActAdmAsgWrkCrs		Edit all the works sent to the course
	806. ActReqRemFilAsgUsr		Request removal of a file of assignments from a user
	807. ActRemFilAsgUsr		Remove a file of assignments from a user
	808. ActRemFolAsgUsr		Remove an empty folder of assignments of a user
	809. ActCopAsgUsr		Set origin of copy in assignments of a user
	810. ActPasAsgUsr		Paste a file or folder of assignments of a user
	811. ActRemTreAsgUsr		Remove a not empty folder of assignments of a user
	812. ActFrmCreAsgUsr		Form to create a new folder or file of assignments of a user
	813. ActCreFolAsgUsr		Create a new folder of assignments of a user
	814. ActCreLnkAsgUsr		Create a new link of assignments of a user
	815. ActRenFolAsgUsr		Rename a folder of assignments of a user
	816. ActRcvFilAsgUsrDZ		Receive a new file of assignments of a user using Dropzone.js
	817. ActRcvFilAsgUsrCla		Receive a new file of assignments of a user using the classic way
	818. ActExpAsgUsr		Expand a folder of assignments of a user
	819. ActConAsgUsr		Contract a folder of assignments of a user
	820. ActZIPAsgUsr		Compress a folder of assignments of a user
	821. ActReqDatAsgUsr		Ask for metadata of a file of assignments of a user
	822. ActChgDatAsgUsr		Change metadata of a file of assignments of a user
	823. ActDowAsgUsr		Download a file of assignments of a user
	824. ActReqRemFilWrkUsr		Request removal of a file of works from a user
	825. ActRemFilWrkUsr		Remove a file of works from a user
	826. ActRemFolWrkUsr		Remove an empty folder of works of a user
	827. ActCopWrkUsr		Set origin of copy in works of a user
	828. ActPasWrkUsr		Paste a file or folder of works of a user
	829. ActRemTreWrkUsr		Remove a not empty folder of works of a user
	830. ActFrmCreWrkUsr		Form to create a new folder or file of works of a user
	831. ActCreFolWrkUsr		Create a new folder of works of a user
	832. ActCreLnkWrkUsr		Create a new link of works of a user
	833. ActRenFolWrkUsr		Rename a folder of works of a user
	834. ActRcvFilWrkUsrDZ		Receive a new file of works of a user using Dropzone.js
	835. ActRcvFilWrkUsrCla		Receive a new file of works of a user using the classic way
	836. ActExpWrkUsr		Expand a folder of works of a user
	837. ActConWrkUsr		Contract a folder of works of a user
	838. ActZIPWrkUsr		Compress a folder of works of a user
	839. ActReqDatWrkUsr		Ask for metadata of a file of works of a user
	840. ActChgDatWrkUsr		Change metadata of a file of works of a user
	841. ActDowWrkUsr		Download a file of works of a user
	842. ActReqRemFilAsgCrs		Request removal of a file of assignments in the course
	843. ActRemFilAsgCrs		Remove a file of assignments in the course
	844. ActRemFolAsgCrs		Remove an empty folder of assignments in the course
	845. ActCopAsgCrs		Set origin of copy in assignments in the course
	846. ActPasAsgCrs		Paste a file or folder of assignments in the course
	847. ActRemTreAsgCrs		Remove a not empty folder of assignments in the course
	848. ActFrmCreAsgCrs		Form to create a new folder or file of assignments in the course
	849. ActCreFolAsgCrs		Create a new folder of assignments in the course
	850. ActCreLnkAsgCrs		Create a new link of assignments in the course
	851. ActRenFolAsgCrs		Rename a folder of assignments in the course
	852. ActRcvFilAsgCrsDZ		Receive a file of assignments in the course using Dropzone.js
	853. ActRcvFilAsgCrsCla		Receive a file of assignments in the course using the classic way
	854. ActExpAsgCrs		Expand a folder of assignments in a course
	855. ActConAsgCrs		Contract a folder of assignments in a course
	856. ActZIPAsgCrs		Compress a folder of assignments in a course
	857. ActReqDatAsgCrs		Ask for metadata of a file of assignments in a course
	858. ActChgDatAsgCrs		Change metadata of a file of assignments in a course
	859. ActDowAsgCrs		Download a file of assignments in a course
	860. ActReqRemFilWrkCrs		Request removal of a file of works in the course
	861. ActRemFilWrkCrs		Remove a file of works in the course
	862. ActRemFolWrkCrs		Remove an empty folder of works in the course
	863. ActCopWrkCrs		Set origin of copy in works in the course
	864. ActPasWrkCrs		Paste a file or folder of works in the course
	865. ActRemTreWrkCrs		Remove a not empty folder of works in the course
	866. ActFrmCreWrkCrs		Form to create a new folder or file of works in the course
	867. ActCreFolWrkCrs		Create a new folder of works in the course
	868. ActCreLnkWrkCrs		Create a new link of works in the course
	869. ActRenFolWrkCrs		Rename a folder of works in the course
	870. ActRcvFilWrkCrsDZ		Receive a file of works in the course using Dropzone.js
	871. ActRcvFilWrkCrsCla		Receive a file of works in the course using the classic way
	872. ActExpWrkCrs		Expand a folder of works in a course
	873. ActConWrkCrs		Contract a folder of works in a course
	874. ActZIPWrkCrs		Compress a folder of works in a course
	875. ActReqDatWrkCrs		Ask for metadata of a file of works in a course
	876. ActChgDatWrkCrs		Change metadata of a file of works in a course
	877. ActDowWrkCrs		Download a file of works in a course

	878. ActChgToSeeMrk		Change to see marks in course/group

	879. ActSeeMrkCrs		See marks in course
	880. ActExpSeeMrkCrs		Expand a folder in marks in course
	881. ActConSeeMrkCrs		Contract a folder in marks in course
	882. ActReqDatSeeMrkCrs		Ask for metadata of a file of the marks zone of the course
	883. ActSeeMyMrkCrs		Individualized access to marks in course

	884. ActSeeMrkGrp		See marks in group
	885. ActExpSeeMrkGrp		Expand a folder in marks in group
	886. ActConSeeMrkGrp		Contract a folder in marks in group
	887. ActReqDatSeeMrkGrp		Ask for metadata of a file of the marks zone of a group
	888. ActSeeMyMrkGrp		Individualized access to marks in group

	889. ActChgToAdmMrk		Change to admin marks in course/group

	890. ActAdmMrkCrs		Admin the files of the zone of marks in course
	891. ActReqRemFilMrkCrs		Request removal of a file of marks in course
	892. ActRemFilMrkCrs		Remove a file of marks in course
	893. ActRemFolMrkCrs		Remove a folder empty of marks in course
	894. ActCopMrkCrs		Set source of copy in marks in course
	895. ActPasMrkCrs		Paste a folder or file in marks in course
	896. ActRemTreMrkCrs		Remove a folder no empty of marks in course
	897. ActFrmCreMrkCrs		Form to crear a folder or file in marks in course
	898. ActCreFolMrkCrs		Create a new folder in marks in course
	899. ActRenFolMrkCrs		Rename a folder in marks in course
	900. ActRcvFilMrkCrsDZ		Receive a file of marks in course using Dropzone.js
	901. ActRcvFilMrkCrsCla		Receive a file of marks in course using the classic way
	902. ActExpAdmMrkCrs		Expand a folder in marks administration in course
	903. ActConAdmMrkCrs		Contract a folder in marks administration in course
	904. ActZIPAdmMrkCrs		Compress a folder in marks administration in course
	905. ActShoMrkCrs		Show hidden folder or file of the marks administration in course
	906. ActHidMrkCrs		Hide folder or file of the marks administration in course
	907. ActReqDatAdmMrkCrs		Ask for metadata of a file in marks administration in course
	908. ActChgDatAdmMrkCrs		Change metadata of a file in marks administration in course
	909. ActDowAdmMrkCrs		Download a file in marks administration in course
	910. ActChgNumRowHeaCrs		Change the number of rows of cabecera of a file of marks in course
	911. ActChgNumRowFooCrs		Change the number of rows of pie of a file of marks in course

	912. ActAdmMrkGrp		Admin the files of the zone of marks in group
	913. ActReqRemFilMrkGrp		Request removal of a file of marks in group
	914. ActRemFilMrkGrp		Remove a file of marks in group
	915. ActRemFolMrkGrp		Remove a folder empty of marks in group
	916. ActCopMrkGrp		Set source of copy in marks in group
	917. ActPasMrkGrp		Paste a folder or file in marks in group
	918. ActRemTreMrkGrp		Remove a folder no empty of marks in group
	919. ActFrmCreMrkGrp		Form to crear a folder or file in marks in group
	920. ActCreFolMrkGrp		Create a new folder in marks in group
	921. ActRenFolMrkGrp		Rename a folder in marks in group
	922. ActRcvFilMrkGrpDZ		Receive a file to marks in group using Dropzone.js
	923. ActRcvFilMrkGrpCla		Receive a file to marks in group using the classic way
	924. ActExpAdmMrkGrp		Expand a folder in marks administration in group
	925. ActConAdmMrkGrp		Contract a folder in marks administration in group
	926. ActZIPAdmMrkGrp		Compress a folder in marks administration in group
	927. ActShoMrkGrp		Show hidden folder or file of the marks administration in group
	928. ActHidMrkGrp		Hide folder or file of the marks administration in group
	929. ActReqDatAdmMrkGrp		Ask for metadata of a file in marks administration in group
	930. ActChgDatAdmMrkGrp		Change metadata of a file in marks administration in group
	931. ActDowAdmMrkGrp		Download a file in marks administration in group
	932. ActChgNumRowHeaGrp		Change the number of rows of cabecera of a file of marks in group
	933. ActChgNumRowFooGrp		Change the number of rows of pie of a file of marks in group

        934. ActReqRemFilBrf		Request removal of a file of the briefcase
        935. ActRemFilBrf		Remove a file of the briefcase
        936. ActRemFolBrf		Remove a folder empty of the briefcase
        937. ActCopBrf			Set source of copy in the briefcase
        938. ActPasBrf			Paste a folder or file in the briefcase
        939. ActRemTreBrf		Remove a folder no empty of the briefcase
        940. ActFrmCreBrf		Form to crear a folder or file in the briefcase
        941. ActCreFolBrf		Create a new folder in the briefcase
        942. ActCreLnkBrf		Create a new link in the briefcase
        943. ActRenFolBrf		Rename a folder of the briefcase
        944. ActRcvFilBrfDZ		Receive a file in the briefcase using Dropzone.js
        945. ActRcvFilBrfCla		Receive a file in the briefcase using the classic way
        946. ActExpBrf			Expand a folder in briefcase
        947. ActConBrf			Contract a folder in briefcase
        948. ActZIPBrf			Compress a folder in briefcase
        949. ActReqDatBrf		Ask for metadata of a file in the briefcase
        950. ActChgDatBrf		Change metadata of a file in the briefcase
        951. ActDowBrf			Download a file in the briefcase
        952. ActReqRemOldBrf		Ask for removing old files in the briefcase
        953. ActRemOldBrf		Remove old files in the briefcase
Users:
	954. ActReqSelGrp		Request the selection of groups of students
	955. ActLstStd			Show class photo or list of students
	956. ActLstTch			Show class photo or list of teachers
	957. ActLstOth			Show class photo or list of guests
	958. ActSeeAtt			Show attendance events
	959. ActReqSignUp		Apply for my enrolment
	960. ActSeeSignUpReq		Show pending requests for inscription in the current course
	961. ActLstCon			List connected users

	962. ActChgGrp			Change my groups
	963. ActReqEdiGrp		Request the edition of groups
	964. ActNewGrpTyp		Request the creation of a type of group
	965. ActReqRemGrpTyp		Request the removal of a type of group
	966. ActRemGrpTyp		Remove a type of group of students
	967. ActRenGrpTyp		Request renaming of a type of group of students
	968. ActChgMdtGrpTyp		Request change if it is mandatory to register in groups of a type
	969. ActChgMulGrpTyp		Request change if it is possible to register in multiple groups of a type
	970. ActChgTimGrpTyp		Request change when the groups of a type will be open
	971. ActNewGrp			Request the creation of a group
	972. ActReqRemGrp		Request the removal of a group
	973. ActRemGrp			Remove a group
	974. ActOpeGrp			Open a group
	975. ActCloGrp			Close a group
	976. ActEnaFilZonGrp		Enable filezones of a group
	977. ActDisFilZonGrp		Disable filezones of a group
	978. ActChgGrpTyp		Change the type of group of a group
	979. ActRenGrp			Rename a group
	980. ActChgGrpCla		Change the classroom of a group
	981. ActChgMaxStdGrp		Change the maximum number of students in a group

	982. ActLstGst			List main data of administrators

	983. ActPrnGstPho		Show the class photo of guests ready to be printed
	984. ActPrnStdPho		Show the class photo of students ready to be printed
	985. ActPrnTchPho		Show the class photo of teachers ready to be printed
	986. ActLstGstAll		List in another window the full data of guests
	987. ActLstStdAll		List in another window the full data of students
	988. ActLstTchAll		List in another window the full data of teachers

	989. ActSeeRecOneStd		Show record of one selected student
	990. ActSeeRecOneTch		Show record of one selected teacher
	991. ActDoActOnSevGst		Do action on several selected guests
	992. ActDoActOnSevStd		Do action on several selected students
	993. ActDoActOnSevTch		Do action on several selected teachers
	994. ActPrnRecSevGst		Show records of several selected guests
	995. ActPrnRecSevStd		Show records of several selected students
	996. ActPrnRecSevTch		Show records of several selected teachers
	997. ActPrnRecSevGst		Show records of several selected guests ready to be printed
	998. ActPrnRecSevStd		Show records of several selected students ready to be printed
	999. ActPrnRecSevTch		Show records of several selected teachers ready to be printed
       1000. ActRcvRecOthUsr		Update record fields of a student in this course
       1001. ActEdiRecFie		Request the edition of record fields of students
       1002. ActNewFie			Request the creation of a record field
       1003. ActReqRemFie		Request the removal of record field
       1004. ActRemFie			Remove a record field
       1005. ActRenFie			Request renaming of record field
       1006. ActChgRowFie		Request change in number of lines of form of a record field
       1007. ActChgVisFie		Request change in visibility of a record field
       1008. ActRcvRecCrs		Receive and update fields of my record in this course

       1009. ActFrmLogInUsrAgd		Put form to log in to show another user's public agenda
       1010. ActLogInUsrAgd		Show another user's public agenda after log in
       1011. ActLogInUsrAgdLan		Show another user's public agenda after change language
       1012. ActSeeUsrAgd		Show another user's public agenda

       1013. ActReqEnrSevStd		Request the enrolment/removing of several students to / from current course
       1014. ActReqEnrSevNET		Request the enrolment/removing of several non-editing teachers to / from current course
       1015. ActReqEnrSevTch		Request the enrolment/removing of several teachers to / from current course

       1016. ActReqLstUsrAtt		Request listing of attendance of several students to several events
       1017. ActSeeLstMyAtt		List my attendance as student to several events
       1018. ActPrnLstMyAtt		Print my attendance as student to several events
       1019. ActSeeLstUsrAtt		List attendance of several students to several events
       1020. ActPrnLstUsrAtt		Print attendance of several students to several events
       1021. ActFrmNewAtt		Form to create a new attendance event
       1022. ActEdiOneAtt		Edit one attendance event
       1023. ActNewAtt			Create new attendance event
       1024. ActChgAtt			Modify data of an existing attendance event
       1025. ActReqRemAtt		Request the removal of an attendance event
       1026. ActRemAtt			Remove attendance event
       1027. ActHidAtt			Hide attendance event
       1028. ActShoAtt			Show attendance event
       1029. ActSeeOneAtt		List students who attended to an event
       1030. ActRecAttStd		Save students who attended to an event and comments
       1031. ActRecAttMe		Save my comments as student in an attendance event

       1032. ActSignUp			Apply for my enrolment
       1033. ActUpdSignUpReq		Update pending requests for inscription in the current course
       1034. ActReqRejSignUp		Ask if reject the enrolment of a user in a course
       1035. ActRejSignUp		Reject the enrolment of a user in a course

       1036. ActReqMdfOneOth		Request a user's ID for enrolment/removing
       1037. ActReqMdfOneStd		Request a student's ID for enrolment/removing
       1038. ActReqMdfOneTch		Request a teacher's ID for enrolment/removing
       1039. ActReqMdfOth		Request enrolment/removing of a user
       1040. ActReqMdfStd		Request enrolment/removing of a student
       1041. ActReqMdfNET		Request enrolment/removing of a non-editing teacher
       1042. ActReqMdfTch		Request enrolment/removing of a teacher
       1043. ActReqOthPho		Show form to send the photo of another user
       1044. ActReqStdPho		Show form to send the photo of another user
       1045. ActReqTchPho		Show form to send the photo of another user
       1046. ActDetOthPho		Receive other user's photo and detect faces on it
       1047. ActDetStdPho		Receive other user's photo and detect faces on it
       1048. ActDetTchPho		Receive other user's photo and detect faces on it
       1049. ActUpdOthPho		Update other user's photo
       1050. ActUpdStdPho		Update other user's photo
       1051. ActUpdTchPho		Update other user's photo
       1052. ActRemOthPho		Request the removal of other user's photo
       1053. ActRemStdPho		Request the removal of student's photo
       1054. ActRemTchPho		Request the removal of teacher's photo
       1055. ActRemOthPho		Remove other user's photo
       1056. ActRemStdPho		Remove student's photo
       1057. ActRemTchPho		Remove teacher's photo
       1058. ActCreOth			Create a new guest
       1059. ActCreStd			Create a new student
       1060. ActCreNET			Create a new non.editing teacher
       1061. ActCreTch			Create a new teacher
       1062. ActUpdOth			Update another user's data and groups
       1063. ActUpdStd			Update another student's data and groups
       1064. ActUpdNET			Update another non-editing teacher's data and groups
       1065. ActUpdTch			Update another teacher's data and groups
       1066. ActReqAccEnrStd		Confirm acceptation / refusion of enrolment as student in current course
       1067. ActReqAccEnrNET		Confirm acceptation / refusion of enrolment as non-editing teacher in current course
       1068. ActReqAccEnrTch		Confirm acceptation / refusion of enrolment as teacher in current course
       1069. ActAccEnrStd		Accept enrolment as student in current course
       1070. ActAccEnrNET		Accept enrolment as non-editing teacher in current course
       1071. ActAccEnrTch		Accept enrolment as teacher in current course
       1072. ActRemMe_Std		Reject enrolment as student in current course
       1073. ActRemMe_NET		Reject enrolment as non-editing teacher in current course
       1074. ActRemMe_Tch		Reject enrolment as teacher in current course

       1075. ActNewAdmIns		Register an administrador in this institution
       1076. ActRemAdmIns		Remove an administrador of this institution
       1077. ActNewAdmCtr		Register an administrador in this centre
       1078. ActRemAdmCtr		Remove an administrador of this centre
       1079. ActNewAdmDeg		Register an administrador in this degree
       1080. ActRemAdmDeg		Remove an administrador of this degree

       1081. ActRcvFrmEnrSevStd		Receive a form with IDs of students to be registered/removed to/from current course
       1082. ActRcvFrmEnrSevNET		Receive a form with IDs of non-editing teachers to be registered/removed to/from current course
       1083. ActRcvFrmEnrSevTch		Receive a form with IDs of teachers to be registered/removed to/from current course

       1084. ActCnfID_Oth		Confirm another user's ID
       1085. ActCnfID_Std		Confirm another user's ID
       1086. ActCnfID_Tch		Confirm another user's ID

       1087. ActFrmAccOth		Show forms to the change another user's account
       1088. ActFrmAccStd		Show forms to the change another user's account
       1089. ActFrmAccTch		Show forms to the change another user's account

       1090. ActRemOldNicOth		Remove one of another user's old nicknames
       1091. ActRemOldNicStd		Remove one of another user's old nicknames
       1092. ActRemOldNicTch		Remove one of another user's old nicknames
       1093. ActChgNicOth		Change another user's nickname
       1094. ActChgNicStd		Change another user's nickname
       1095. ActChgNicTch		Change another user's nickname

       1096. ActRemID_Oth		Remove one of the IDs of another user
       1097. ActRemID_Std		Remove one of the IDs of another user
       1098. ActRemID_Tch		Remove one of the IDs of another user
       1099. ActNewID_Oth		Create a new user's ID for another user
       1100. ActNewID_Std		Create a new user's ID for another user
       1101. ActNewID_Tch		Create a new user's ID for another user

       1102. ActChgPwdOth		Change the password of another user
       1103. ActChgPwdStd		Change the password of another user
       1104. ActChgPwdTch		Change the password of another user

       1105. ActRemMaiOth		Remove one of the email of another user
       1106. ActRemMaiStd		Remove one of the email of another user
       1107. ActRemMaiTch		Remove one of the email of another user
       1108. ActNewMaiOth		Create a new user's email for another user
       1109. ActNewMaiStd		Create a new user's email for another user
       1110. ActNewMaiTch		Create a new user's email for another user

       1111. ActRemStdCrs		Remove a student from the current course
       1112. ActRemNETCrs		Remove a non-editing teacher from the current course
       1113. ActRemTchCrs		Remove a teacher from the current course
       1114. ActRemUsrGbl		Eliminate completely a user from the platform
       1115. ActReqRemAllStdCrs		Request the removal of all the students from the current course
       1116. ActRemAllStdCrs		Remove all the students from the current course
       1117. ActReqRemOldUsr		Request the complete elimination of old users
       1118. ActRemOldUsr		Eliminate completely old users

       1119. ActLstDupUsr		List possible duplicate users
       1120. ActLstSimUsr		List users similar to a given one (possible duplicates)
       1121. ActRemDupUsr		Remove user from list of possible duplicate users

       1122. ActReqFolSevStd		Request follow several students
       1123. ActReqFolSevTch		Request follow several teachers
       1124. ActReqUnfSevStd		Request unfollow several students
       1125. ActReqUnfSevTch		Request unfollow several teachers
       1126. ActFolSevStd		Follow several students
       1127. ActFolSevTch		Follow several teachers
       1128. ActUnfSevStd		Unfollow several students
       1129. ActUnfSevTch		Unfollow several teachers

Messages:
       1130. ActSeeAnn			Show global announcements
       1131. ActSeeAllNot		Show all notices
       1132. ActSeeFor			Show the level superior of the forums
       1133. ActSeeChtRms		Show the chat rooms
       1134. ActReqMsgUsr		Write message to several users
       1135. ActSeeRcvMsg		Show the messages received from other users (link in menu)
       1136. ActSeeSntMsg		Show the messages sent to other users
       1137. ActReqMaiUsr		Select users to send an email to them
       1138. ActMaiUsr			Send an email to users
       1139. ActWriAnn			Show form to create a new global announcement
       1140. ActRcvAnn			Receive and create a new global announcement
       1141. ActHidAnn			Hide a global announcement that was active
       1142. ActRevAnn			Reveal a global announcement that was hidden
       1143. ActRemAnn			Remove global announcement
       1144. ActSeeOneNot		Show (expand) a notice
       1145. ActWriNot			Write a new notice
       1146. ActRcvNot			Receive and create a new notice
       1147. ActHidNot			Hide a notice that was active
       1148. ActRevNot			Reveal a notice that was hidden
       1149. ActReqRemNot		Request removal of a notice
       1150. ActRemNot			Remove a notice

       1151. ActSeeForCrsUsr		Show top level of forum of users of the course
       1152. ActSeeForCrsTch		Show top level of forum of teachers of the course
       1153. ActSeeForDegUsr		Show top level of forum of users of the degree
       1154. ActSeeForDegTch		Show top level of forum of teachers of the degree
       1155. ActSeeForCtrUsr		Show top level of forum of users of the centre
       1156. ActSeeForCtrTch		Show top level of forum of teachers of the centre
       1157. ActSeeForInsUsr		Show top level of forum of users of the institution
       1158. ActSeeForInsTch		Show top level of forum of teachers of the institution
       1159. ActSeeForGenUsr		Show top level of forum of users general
       1160. ActSeeForGenTch		Show top level of forum of teachers general
       1161. ActSeeForSWAUsr		Show top level of forum of users of the platform
       1162. ActSeeForSWATch		Show top level of forum of teachers of the platform
       1163. ActSeePstForCrsUsr		Show the messages of a thread of the forum of users of the course
       1164. ActSeePstForCrsTch		Show the messages of a thread of the forum of teachers of the course
       1165. ActSeePstForDegUsr		Show the messages of a thread of the forum of users of the degree
       1166. ActSeePstForDegTch		Show the messages of a thread of the forum of teachers of the degree
       1167. ActSeePstForCtrUsr		Show the messages of a thread of the forum of users of the centre
       1168. ActSeePstForCtrTch		Show the messages of a thread of the forum of teachers of the centre
       1169. ActSeePstForInsUsr		Show the messages of a thread of the forum of users of the institution
       1170. ActSeePstForInsTch		Show the messages of a thread of the forum of teachers of the institution
       1171. ActSeePstForGenUsr		Show the messages of a thread of the forum of users general
       1172. ActSeePstForGenTch		Show the messages of a thread of the forum of teachers general
       1173. ActSeePstForSWAUsr		Show the messages of a thread of the forum of users of the platform
       1174. ActSeePstForSWATch		Show the messages of a thread of the forum of teachers of the platform
       1175. ActRcvThrForCrsUsr		Receive the first message of a new thread of forum of users of the course
       1176. ActRcvThrForCrsTch		Receive the first message of a new thread of forum of teachers of the course
       1177. ActRcvThrForDegUsr		Receive the first message of a new thread of forum of users of the degree
       1178. ActRcvThrForDegTch		Receive the first message of a new thread of forum of teachers of the degree
       1179. ActRcvThrForCtrUsr		Receive the first message of a new thread of forum of users of centre
       1180. ActRcvThrForCtrTch		Receive the first message of a new thread of forum of teachers of centre
       1181. ActRcvThrForInsUsr		Receive the first message of a new thread of forum of users of the institution
       1182. ActRcvThrForInsTch		Receive the first message of a new thread of forum of teachers of the institution
       1183. ActRcvThrForGenUsr		Receive the first message of a new thread of forum of users general
       1184. ActRcvThrForGenTch		Receive the first message of a new thread of forum of teachers general
       1185. ActRcvThrForSWAUsr		Receive the first message of a new thread of forum of users of the platform
       1186. ActRcvThrForSWATch		Receive the first message of a new thread of forum of teachers of the platform
       1187. ActRcvRepForCrsUsr		Receive a message of answer in a thread existente in the forum of users of the course
       1188. ActRcvRepForCrsTch		Receive a message of answer in a thread existente in the forum of teachers of the course
       1189. ActRcvRepForDegUsr		Receive a message of answer in a thread existente in the forum of users of the degree
       1190. ActRcvRepForDegTch		Receive a message of answer in a thread existente in the forum of teachers of the degree
       1191. ActRcvRepForCtrUsr		Receive a message of answer in a thread existente in the forum of users of centre
       1192. ActRcvRepForCtrTch		Receive a message of answer in a thread existente in the forum of teachers of centre
       1193. ActRcvRepForInsUsr		Receive a message of answer in a thread existente in the forum of users of the institution
       1194. ActRcvRepForInsTch		Receive a message of answer in a thread existente in the forum of teachers of the institution
       1195. ActRcvRepForGenUsr		Receive a message of answer in a thread existente in the forum of users general
       1196. ActRcvRepForGenTch		Receive a message of answer in a thread existente in the forum of teachers general
       1197. ActRcvRepForSWAUsr		Receive a message of answer in a thread existente in the forum of users of the platform
       1198. ActRcvRepForSWATch		Receive a message of answer in a thread existente in the forum of teachers of the platform
       1199. ActReqDelThrCrsUsr		Request the removal of a thread of forum of users of the course
       1200. ActReqDelThrCrsTch		Request the removal of a thread of forum of teachers of the course
       1201. ActReqDelThrDegUsr		Request the removal of a thread of forum of users of the degree
       1202. ActReqDelThrDegTch		Request the removal of a thread of forum of teachers of the degree
       1203. ActReqDelThrCtrUsr		Request the removal of a thread of forum of users of centre
       1204. ActReqDelThrCtrTch		Request the removal of a thread of forum of teachers of centre
       1205. ActReqDelThrInsUsr		Request the removal of a thread of forum of users of the institution
       1206. ActReqDelThrInsTch		Request the removal of a thread of forum of teachers of the institution
       1207. ActReqDelThrGenUsr		Request the removal of a thread of forum of users general
       1208. ActReqDelThrGenTch		Request the removal of a thread of forum of teachers general
       1209. ActReqDelThrSWAUsr		Request the removal of a thread of forum of users of the platform
       1210. ActReqDelThrSWATch		Request the removal of a thread of forum of teachers of the platform
       1211. ActDelThrForCrsUsr		Remove a thread of forum of users of the course
       1212. ActDelThrForCrsTch		Remove a thread of forum of teachers of the course
       1213. ActDelThrForDegUsr		Remove a thread of forum of users of the degree
       1214. ActDelThrForDegTch		Remove a thread of forum of teachers of the degree
       1215. ActDelThrForCtrUsr		Remove a thread of forum of users of centre
       1216. ActDelThrForCtrTch		Remove a thread of forum of teachers of centre
       1217. ActDelThrForInsUsr		Remove a thread of forum of users of the institution
       1218. ActDelThrForInsTch		Remove a thread of forum of teachers of the institution
       1219. ActDelThrForGenUsr		Remove a thread of forum of users general
       1220. ActDelThrForGenTch		Remove a thread of forum of teachers general
       1221. ActDelThrForSWAUsr		Remove a thread of forum of users of the platform
       1222. ActDelThrForSWATch		Remove a thread of forum of teachers of the platform
       1223. ActCutThrForCrsUsr		Cut a thread of forum of users of the course
       1224. ActCutThrForCrsTch		Cut a thread of forum of teachers of the course
       1225. ActCutThrForDegUsr		Cut a thread of forum of users of the degree
       1226. ActCutThrForDegTch		Cut a thread of forum of teachers of the degree
       1227. ActCutThrForCtrUsr		Cut a thread of forum of users of centre
       1228. ActCutThrForCtrTch		Cut a thread of forum of teachers of centre
       1229. ActCutThrForInsUsr		Cut a thread of forum of users of the institution
       1230. ActCutThrForInsTch		Cut a thread of forum of teachers of the institution
       1231. ActCutThrForGenUsr		Cut a thread of forum of users general
       1232. ActCutThrForGenTch		Cut a thread of forum of teachers general
       1233. ActCutThrForSWAUsr		Cut a thread of forum of users of the platform
       1234. ActCutThrForSWATch		Cut a thread of forum of teachers of the platform
       1235. ActPasThrForCrsUsr		Paste a thread of forum of users of the course
       1236. ActPasThrForCrsTch		Paste a thread of forum of teachers of the course
       1237. ActPasThrForDegUsr		Paste a thread of forum of users of the degree
       1238. ActPasThrForDegTch		Paste a thread of forum of teachers of the degree
       1239. ActPasThrForCtrUsr		Paste a thread of forum of users of centre
       1240. ActPasThrForCtrTch		Paste a thread of forum of teachers of centre
       1241. ActPasThrForInsUsr		Paste a thread of forum of users of the institution
       1242. ActPasThrForInsTch		Paste a thread of forum of teachers of the institution
       1243. ActPasThrForGenUsr		Paste a thread of forum of users general
       1244. ActPasThrForGenTch		Paste a thread of forum of teachers general
       1245. ActPasThrForSWAUsr		Paste a thread of forum of users of the platform
       1246. ActPasThrForSWATch		Paste a thread of forum of teachers of the platform
       1247. ActDelPstForCrsUsr		Remove a message of forum of users of the course
       1248. ActDelPstForCrsTch		Remove a message of forum of teachers of the course
       1249. ActDelPstForDegUsr		Remove a message of forum of users of the degree
       1250. ActDelPstForDegTch		Remove a message of forum of teachers of the degree
       1251. ActDelPstForCtrUsr		Remove a message of forum of users of centre
       1252. ActDelPstForCtrTch		Remove a message of forum of teachers of centre
       1253. ActDelPstForInsUsr		Remove a message of forum of users of the institution
       1254. ActDelPstForInsTch		Remove a message of forum of teachers of the institution
       1255. ActDelPstForGenUsr		Remove a message of forum of users general
       1256. ActDelPstForGenTch		Remove a message of forum of teachers general
       1257. ActDelPstForSWAUsr		Remove a message of forum of users of the platform
       1258. ActDelPstForSWATch		Remove a message of forum of teachers of the platform
       1259. ActEnbPstForCrsUsr		Enable a message of forum of users of the course
       1260. ActEnbPstForCrsTch		Enable a message of forum of teachers of the course
       1261. ActEnbPstForDegUsr		Enable a message of forum of users of the degree
       1262. ActEnbPstForDegTch		Enable a message of forum of teachers of the degree
       1263. ActEnbPstForCtrUsr		Enable a message of forum of users of centre
       1264. ActEnbPstForCtrTch		Enable a message of forum of teachers of centre
       1265. ActEnbPstForInsUsr		Enable a message of forum of users of the institution
       1266. ActEnbPstForInsTch		Enable a message of forum of teachers of the institution
       1267. ActEnbPstForGenUsr		Enable a message of forum of users general
       1268. ActEnbPstForGenTch		Enable a message of forum of teachers general
       1269. ActEnbPstForSWAUsr		Enable a message of forum of users of the platform
       1270. ActEnbPstForSWATch		Enable a message of forum of teachers of the platform
       1271. ActDisPstForCrsUsr		Disable a message of forum of users of the course
       1272. ActDisPstForCrsTch		Disable a message of forum of teachers of the course
       1273. ActDisPstForDegUsr		Disable a message of forum of users of the degree
       1274. ActDisPstForDegTch		Disable a message of forum of teachers of the degree
       1275. ActDisPstForCtrUsr		Disable a message of forum of users of centre
       1276. ActDisPstForCtrTch		Disable a message of forum of teachers of centre
       1277. ActDisPstForInsUsr		Disable a message of forum of users of the institution
       1278. ActDisPstForInsTch		Disable a message of forum of teachers of the institution
       1279. ActDisPstForGenUsr		Disable a message of forum of users general
       1280. ActDisPstForGenTch		Disable a message of forum of teachers general
       1281. ActDisPstForSWAUsr		Disable a message of forum of users of the platform
       1282. ActDisPstForSWATch		Disable a message of forum of teachers of the platform

       1283. ActCht			Enter in a chat room to chat

       1284. ActRcvMsgUsr		Sent/Receive a message of a user
       1285. ActReqDelAllSntMsg		Request the removal of todos the messages sent to other users
       1286. ActReqDelAllRcvMsg		Request the removal of todos the messages received from other users
       1287. ActDelAllSntMsg		Remove todos the messages sent to other users
       1288. ActDelAllRcvMsg		Remove todos the messages received from other users
       1289. ActDelSntMsg		Remove a message sent to other users
       1290. ActDelRcvMsg		Remove a message received from other user
       1291. ActExpSntMsg		See (expand) sent message
       1292. ActExpRcvMsg		See (expand) received message
       1293. ActConSntMsg		Hide (contract) sent message
       1294. ActConRcvMsg		Hide (contract) received message
       1295. ActLstBanUsr		List banned users
       1296. ActBanUsrMsg		Ban the sender of a message when showing received messages
       1297. ActUnbUsrMsg		Unban the sender of a message when showing received messages
       1298. ActUnbUsrLst		Unban a user when listing banned users

Statistics:
       1299. ActReqUseGbl		Request showing use of the platform
       1300. ActSeePhoDeg		Show a class photo with the average photos of the students of each degree
       1301. ActReqStaCrs		Request statistics of courses
       1302. ActReqAccGbl		Request query of clicks to the complete platform
       1303. ActReqMyUsgRep		Request my usage report
       1304. ActMFUAct			Show most frequently used actions

       1305. ActSeeUseGbl		Show use of the platform
       1306. ActPrnPhoDeg		Show vista of impresión of the class photo with the average photos of the students of each degree.
       1307. ActCalPhoDeg		Compute the average photos of the students of each degree
       1308. ActSeeAccGbl		Query clicks to the complete platform
       1309. ActReqAccCrs		Request query of clicks in the course
       1310. ActSeeAccCrs		Query clicks to current course
       1311. ActSeeAllStaCrs		Show statistics of courses

       1312. ActLstClk			List last clicks
       1313. ActRefLstClk		Refresh last clicks in real time via AJAX

       1314. ActSeeMyUsgRep		Show my usage report

Profile:
       1315. ActFrmRolSes		Show form to log out and to change current role in this session
       1316. ActMyCrs			Select one of my courses
       1317. ActSeeMyTT			Show the timetable of all courses of the logged user
       1318. ActSeeMyAgd		Show my full agenda (personal organizer)
       1319. ActFrmMyAcc		Show form to the creation or change of user's account
       1320. ActReqEdiRecSha		Request the edition of the record with the personal data of the user
       1321. ActReqEdiSet		Show forms to edit settings

       1322. ActChgMyRol		Change type of logged user

       1323. ActFrmNewEvtMyAgd		Form to create a new event in my agenda
       1324. ActEdiOneEvtMyAgd		Edit one event from my agenda
       1325. ActNewEvtMyAgd		Create a new event in my agenda
       1326. ActChgEvtMyAgd		Modify data of an event from my agenda
       1327. ActReqRemEvtMyAgd		Request the removal of an event from my agenda
       1328. ActRemEvtMyAgd		Remove an event from my agenda
       1329. ActHidEvtMyAgd		Hide an event from my agenda
       1330. ActShoEvtMyAgd		Show an event from my agenda
       1331. ActPrvEvtMyAgd		Make private an event from my agenda
       1332. ActPubEvtMyAgd		Make public an event from my agenda
       1333. ActPrnAgdQR		Show agenda QR code ready to print

       1334. ActChkUsrAcc		Check if already exists a new account without password associated to a ID
       1335. ActCreMyAcc		Form to create a new account for me
       1336. ActCreUsrAcc		Create new user account
       1337. ActRemMyID			Remove one of my user's IDs
       1338. ActChgMyID			Create a new user's ID for me
       1339. ActRemMyNck		Remove one of my old nicknames
       1340. ActChgMyNck		Change my nickname
       1341. ActRemMyMai		Remove one of my old emails
       1342. ActChgMyMai		Change my email address
       1343. ActCnfMai			Confirm email address
       1344. ActChgMyPwd		Change my password
       1345. ActReqRemMyAcc		Request the removal of my account
       1346. ActRemMyAcc		Remove my account

       1347. ActChgMyData		Update my personal data

       1348. ActReqMyPho		Show form to send my photo
       1349. ActDetMyPho		Receive my photo and detect faces on it
       1350. ActUpdMyPho		Update my photo
       1351. ActReqRemMyPho		Request the removal of my photo
       1352. ActRemMyPho		Remove my photo

       1353. ActChgCtyMyIns		Change the country of my institution
       1354. ActChgMyIns		Change my institution
       1355. ActChgMyCtr		Change my centre
       1356. ActChgMyDpt		Change my department
       1357. ActChgMyOff		Change my office
       1358. ActChgMyOffPho		Change my office phone

       1359. ActReqEdiMyNet		Request the edition of my social networks
       1360. ActChgMyNet		Change my web and social networks

       1361. ActChgThe			Change theme
       1362. ActReqChgLan		Ask if change language
       1363. ActChgLan			Change language
       1364. ActChg1stDay		Change first day of the week
       1365. ActChgDatFmt		Change date format
       1366. ActChgCol			Change side columns
       1367. ActHidLftCol		Hide left side column
       1368. ActHidRgtCol		Hide right side column
       1369. ActShoLftCol		Show left side column
       1370. ActShoRgtCol		Show right side column
       1371. ActChgIco			Change icon set
       1372. ActChgMnu			Change menu
       1373. ActChgPriPho		Change privacy of my photo
       1374. ActChgBasPriPrf		Change privacy of my basic public profile
       1375. ActChgExtPriPrf		Change privacy of my extended public profile
       1376. ActChgCooPrf		Change setting about third party cookies
       1377. ActChgNtfPrf		Change whether to notify by email new messages
       1378. ActPrnUsrQR		Show my QR code ready to print5

       1379. ActPrnMyTT			Show the timetable listo to impresión of all my courses
       1380. ActEdiTut			Edit the timetable of tutorías
       1381. ActChgTut			Modify the timetable of tutorías
       1382. ActChgMyTT1stDay		Change first day of week and show timetable of the course
*/

#define Act_DEFAULT_ACTION_AFTER_LOGIN ActSeeSocTmlGbl

const struct Act_Actions Act_Actions[Act_NUM_ACTIONS] =
  {
   // TabUnk ******************************************************************
   // Actions not in menu:
   [ActAll		] = { 645,-1,TabUnk,ActAll		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,NULL				,NULL},	// Used for statistics
   [ActUnk		] = { 194,-1,TabUnk,ActUnk		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,NULL				,NULL},
   [ActMnu		] = {   2,-1,TabUnk,ActMnu		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,NULL				,NULL},
   [ActRefCon		] = { 845,-1,TabUnk,ActRefCon		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RFRESH,NULL				,Lay_RefreshNotifsAndConnected	,NULL},
   [ActWebSvc		] = { 892,-1,TabUnk,ActWebSvc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_WEB_SERVICE,NULL				,API_WebService			,NULL},

   // TabStr ******************************************************************
   [ActFrmLogIn		] = {1521, 0,TabStr,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WriteLandingPage		,"power-off"		},
   [ActReqSch		] = { 627, 1,TabStr,ActReqSch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Sch_ReqSysSearch		,"search"		},
   [ActSeeSocTmlGbl	] = {1490, 2,TabStr,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TL_ShowTimelineGbl1		,TL_ShowTimelineGbl2		,"comment-dots"		},
   [ActSeeSocPrf	] = {1520, 3,TabStr,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_SeeSocialProfiles		,"user-check"		},
   [ActSeeCal		] = {1622, 4,TabStr,ActSeeCal		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cal_ShowCalendar		,"calendar"		},
   [ActSeeNtf         	] = { 990, 5,TabStr,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ntf_ShowMyNotifications	,"bell"			},

   [ActLogIn		] = {   6,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WelcomeUsr			,NULL},
   [ActLogInNew		] = {1585,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WelcomeUsr			,NULL},
   [ActLogInLan		] = {1077,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WelcomeUsr			,NULL},
   [ActAnnSee		] = {1234,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_MarkAnnouncementAsSeen	,NULL},
   [ActReqSndNewPwd	] = { 665,-1,TabUnk,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pwd_ShowFormSendNewPwd		,NULL},
   [ActSndNewPwd	] = { 633,-1,TabUnk,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pwd_ChkIdLoginAndSendNewPwd	,NULL},
   [ActLogOut		] = {  10,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Ses_CloseSession		,Usr_Logout			,NULL},

   [ActSch		] = { 628,-1,TabUnk,ActReqSch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Sch_GetParamsSearch		,Sch_SysSearch			,NULL},

   [ActRefNewSocPubGbl	] = {1509,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RFRESH,NULL				,TL_RefreshNewTimelineGbl	,NULL},
   [ActRefOldSocPubGbl	] = {1510,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_RefreshOldTimelineGbl	,NULL},
   [ActRcvSocPstGbl	] = {1492,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,TL_ShowTimelineGbl1		,TL_ReceivePostGbl		,NULL},
   [ActRcvSocComGbl	] = {1503,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,TL_ShowTimelineGbl1		,TL_ReceiveCommentGbl		,NULL},
   [ActShoHidSocComGbl	] = {1806,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowHiddenCommentsGbl	,NULL},
   [ActAllShaSocNotGbl	] = {1766,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowAllSharersNoteGbl	,NULL},
   [ActAllFavSocNotGbl	] = {1767,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowAllFaversNoteGbl	,NULL},
   [ActAllFavSocComGbl	] = {1768,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowAllFaversComGbl		,NULL},
   [ActShaSocNotGbl	] = {1495,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShaNoteGbl			,NULL},
   [ActUnsSocNotGbl	] = {1496,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_UnsNoteGbl			,NULL},
   [ActFavSocNotGbl	] = {1512,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_FavNoteGbl			,NULL},
   [ActUnfSocNotGbl	] = {1513,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_UnfNoteGbl			,NULL},
   [ActFavSocComGbl	] = {1516,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_FavCommentGbl		,NULL},
   [ActUnfSocComGbl	] = {1517,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_UnfCommentGbl		,NULL},
   [ActReqRemSocPubGbl	] = {1494,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TL_ShowTimelineGbl1		,TL_RequestRemNoteGbl		,NULL},
   [ActRemSocPubGbl	] = {1493,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TL_ShowTimelineGbl1		,TL_RemoveNoteGbl		,NULL},
   [ActReqRemSocComGbl	] = {1505,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TL_ShowTimelineGbl1		,TL_RequestRemComGbl		,NULL},
   [ActRemSocComGbl	] = {1507,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TL_ShowTimelineGbl1		,TL_RemoveComGbl		,NULL},

   [ActReqOthPubPrf	] = {1401,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_RequestUserProfile		,NULL},

   [ActRefOldSocPubUsr	] = {1511,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_RefreshOldTimelineUsr	,NULL},
   [ActRcvSocPstUsr	] = {1498,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,TL_ReceivePostUsr		,NULL},
   [ActRcvSocComUsr	] = {1504,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,TL_ReceiveCommentUsr		,NULL},
   [ActShoHidSocComUsr	] = {1807,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowHiddenCommentsUsr	,NULL},
   [ActAllShaSocNotUsr	] = {1769,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowAllSharersNoteUsr	,NULL},
   [ActAllFavSocNotUsr	] = {1770,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowAllFaversNoteUsr	,NULL},
   [ActAllFavSocComUsr	] = {1771,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShowAllFaversComUsr		,NULL},
   [ActShaSocNotUsr	] = {1499,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_ShaNoteUsr			,NULL},
   [ActUnsSocNotUsr	] = {1500,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_UnsNoteUsr			,NULL},
   [ActFavSocNotUsr	] = {1514,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_FavNoteUsr			,NULL},
   [ActUnfSocNotUsr	] = {1515,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_UnfNoteUsr			,NULL},
   [ActFavSocComUsr	] = {1518,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_FavCommentUsr		,NULL},
   [ActUnfSocComUsr	] = {1519,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TL_UnfCommentUsr		,NULL},
   [ActReqRemSocPubUsr	] = {1501,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TL_RequestRemNoteUsr		,NULL},
   [ActRemSocPubUsr	] = {1502,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TL_RemoveNoteUsr		,NULL},
   [ActReqRemSocComUsr	] = {1506,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TL_RequestRemComUsr		,NULL},
   [ActRemSocComUsr	] = {1508,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TL_RemoveComUsr		,NULL},

   [ActSeeOthPubPrf	] = {1402,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_GetUsrDatAndShowUserProfile,NULL},
   [ActCalFig		] = {1405,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_CalculateFigures		,NULL},

   [ActFolUsr		] = {1410,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Fol_FollowUsr1			,Fol_FollowUsr2			,NULL},
   [ActUnfUsr		] = {1411,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Fol_UnfollowUsr1		,Fol_UnfollowUsr2		,NULL},
   [ActSeeFlg		] = {1412,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ListFollowing		,NULL},
   [ActSeeFlr		] = {1413,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ListFollowers		,NULL},

   [ActPrnCal		] = {1623,-1,TabUnk,ActSeeCal		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Cal_PrintCalendar		,NULL},
   [ActChgCal1stDay	] = {1624,-1,TabUnk,ActSeeCal		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Cal_ShowCalendar		,NULL},

   [ActSeeNewNtf	] = { 991,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ntf_ShowMyNotifications	,NULL},
   [ActMrkNtfSee	] = {1146,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ntf_MarkAllNotifAsSeen		,NULL},

   [ActSeeMai		] = { 855,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_SeeMailDomains		,NULL},
   [ActEdiMai		] = { 856,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_EditMailDomains		,NULL},
   [ActNewMai		] = { 857,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_RecFormNewMailDomain	,Mai_ContEditAfterChgMai	,NULL},
   [ActRemMai		] = { 860,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_RemoveMailDomain		,Mai_ContEditAfterChgMai	,NULL},
   [ActRenMaiSho	] = { 858,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_RenameMailDomainShort	,Mai_ContEditAfterChgMai	,NULL},
   [ActRenMaiFul	] = { 859,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_RenameMailDomainFull	,Mai_ContEditAfterChgMai	,NULL},

   // TabSys ******************************************************************
   // Actions in menu:
   [ActSeeCty		] = { 862, 0,TabSys,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_ListCountries1		,Cty_ListCountries2		,"globe"		},
   [ActSeePen		] = {1060, 1,TabSys,ActSeePen		,    0,    0,    0,    0,    0,    0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Hie_SeePending			,"sitemap"		},
   [ActSeeLnk		] = { 748, 2,TabSys,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Lnk_SeeLinks			,"link"			},
   [ActLstPlg		] = { 777, 3,TabSys,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plg_ListPlugins		,"puzzle-piece"		},
   [ActSetUp		] = { 840, 4,TabSys,ActSetUp		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,SUp_SetUp			,"bolt"			},

   // Actions not in menu:
   [ActEdiCty		] = { 863,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cty_EditCountries		,NULL},
   [ActNewCty		] = { 864,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_RecFormNewCountry		,Cty_ContEditAfterChgCty	,NULL},
   [ActRemCty		] = { 893,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_RemoveCountry		,Cty_ContEditAfterChgCty	,NULL},
   [ActRenCty		] = { 866,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_RenameCountry		,Cty_ContEditAfterChgCty	,NULL},
   [ActChgCtyWWW	] = {1157,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_ChangeCtyWWW		,Cty_ContEditAfterChgCty	,NULL},

   [ActReqRemOldCrs	] = {1109,-1,TabUnk,ActSeePen		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_AskRemoveOldCrss		,NULL},
   [ActRemOldCrs	] = {1110,-1,TabUnk,ActSeePen		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_RemoveOldCrss		,NULL},

   [ActSeeBan		] = {1137,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3FF,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ban_SeeBanners			,NULL},
   [ActEdiBan		] = {1138,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ban_EditBanners		,NULL},
   [ActNewBan		] = {1139,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_RecFormNewBanner		,Ban_ContEditAfterChgBan	,NULL},
   [ActRemBan		] = {1140,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_RemoveBanner		,Ban_ContEditAfterChgBan	,NULL},
   [ActShoBan		] = {1212,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_ShowBanner			,Ban_ContEditAfterChgBan	,NULL},
   [ActHidBan		] = {1213,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_HideBanner			,Ban_ContEditAfterChgBan	,NULL},
   [ActRenBanSho	] = {1141,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_RenameBannerShort		,Ban_ContEditAfterChgBan	,NULL},
   [ActRenBanFul	] = {1142,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_RenameBannerFull		,Ban_ContEditAfterChgBan	,NULL},
   [ActChgBanImg	] = {1144,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_ChangeBannerImg		,Ban_ContEditAfterChgBan	,NULL},
   [ActChgBanWWW	] = {1143,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_ChangeBannerWWW		,Ban_ContEditAfterChgBan	,NULL},
   [ActClkBan		] = {1145,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_DOWNLD_FILE,Ban_ClickOnBanner		,NULL				,NULL},

   [ActEdiLnk		] = { 749,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Lnk_EditLinks			,NULL},
   [ActNewLnk		] = { 750,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_RecFormNewLink		,Lnk_ContEditAfterChgLnk	,NULL},
   [ActRemLnk		] = { 897,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_RemoveLink			,Lnk_ContEditAfterChgLnk	,NULL},
   [ActRenLnkSho	] = { 753,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_RenameLinkShort		,Lnk_ContEditAfterChgLnk	,NULL},
   [ActRenLnkFul	] = { 751,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_RenameLinkFull		,Lnk_ContEditAfterChgLnk	,NULL},
   [ActChgLnkWWW	] = { 752,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_ChangeLinkWWW		,Lnk_ContEditAfterChgLnk	,NULL},

   [ActEdiPlg		] = { 778,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plg_EditPlugins		,NULL},
   [ActNewPlg		] = { 779,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_RecFormNewPlg		,Plg_ContEditAfterChgPlg	,NULL},
   [ActRemPlg		] = { 889,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_RemovePlugin		,Plg_ContEditAfterChgPlg	,NULL},
   [ActRenPlg		] = { 782,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_RenamePlugin		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgDes	] = { 888,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgDescription	,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgLog	] = { 781,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgLogo		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgAppKey	] = { 986,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgAppKey		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgURL	] = { 783,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgURL		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgIP		] = { 780,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgIP		,Plg_ContEditAfterChgPlg	,NULL},

   // TabCty ******************************************************************
   // Actions in menu:
   [ActSeeCtyInf	] = {1155, 0,TabCty,ActSeeCtyInf	,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cty_ShowConfiguration		,"info"			},
   [ActSeeIns		] = { 696, 1,TabCty,ActSeeIns		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ins_ShowInssOfCurrentCty	,"university"		},

   // Actions not in menu:
   [ActPrnCtyInf	] = {1156,-1,TabUnk,ActSeeCtyInf	,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Cty_PrintConfiguration		,NULL},
   [ActChgCtyMapAtt	] = {1158,-1,TabUnk,ActSeeCtyInf	,    0,    0,    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cty_ChangeCtyMapAttribution	,NULL},

   [ActEdiIns		] = { 697,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ins_EditInstitutions		,NULL},
   [ActReqIns		] = {1210,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RecFormReqIns		,Ins_ContEditAfterChgIns	,NULL},
   [ActNewIns		] = { 698,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RecFormNewIns		,Ins_ContEditAfterChgIns	,NULL},
   [ActRemIns		] = { 759,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RemoveInstitution		,Ins_ContEditAfterChgIns	,NULL},
   [ActRenInsSho	] = { 702,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RenameInsShort		,Ins_ContEditAfterChgIns	,NULL},
   [ActRenInsFul	] = { 701,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RenameInsFull		,Ins_ContEditAfterChgIns	,NULL},
   [ActChgInsWWW	] = { 700,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_ChangeInsWWW		,Ins_ContEditAfterChgIns	,NULL},
   [ActChgInsSta	] = {1211,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_ChangeInsStatus		,Ins_ContEditAfterChgIns	,NULL},

   // TabIns ******************************************************************
   // Actions in menu:
   [ActSeeInsInf	] = {1153, 0,TabIns,ActSeeInsInf	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ins_ShowConfiguration		,"info"			},
   [ActSeeCtr		] = { 676, 1,TabIns,ActSeeCtr		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ctr_ShowCtrsOfCurrentIns	,"building"		},
   [ActSeePlc		] = { 703, 2,TabIns,ActSeePlc		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plc_SeePlaces			,"map-marker-alt"	},
   [ActSeeDpt		] = { 675, 3,TabIns,ActSeeDpt		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dpt_SeeDepts			,"users"		},
   [ActSeeHld		] = { 707, 4,TabIns,ActSeeHld		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Hld_SeeHolidays		,"calendar-day"		},

   // Actions not in menu:
   [ActPrnInsInf	] = {1154,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Ins_PrintConfiguration		,NULL},
   [ActChgInsCtyCfg	] = {1590,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_ChangeInsCtyInConfig	,Ins_ContEditAfterChgInsInConfig,NULL},
   [ActRenInsShoCfg	] = {1592,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RenameInsShortInConfig	,Ins_ContEditAfterChgInsInConfig,NULL},
   [ActRenInsFulCfg	] = {1591,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RenameInsFullInConfig	,Ins_ContEditAfterChgInsInConfig,NULL},
   [ActChgInsWWWCfg	] = {1593,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ins_ChangeInsWWWInConfig	,NULL},
   [ActReqInsLog	] = {1245,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ins_RequestLogo		,NULL},
   [ActRecInsLog	] = { 699,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,Ins_ReceiveLogo		,Ins_ShowConfiguration		,NULL},
   [ActRemInsLog	] = {1341,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,Ins_RemoveLogo			,Ins_ShowConfiguration		,NULL},

   [ActEdiCtr		] = { 681,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ctr_EditCentres		,NULL},
   [ActReqCtr		] = {1208,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RecFormReqCtr		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActNewCtr		] = { 685,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RecFormNewCtr		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActRemCtr		] = { 686,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RemoveCentre		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActChgCtrPlc	] = { 706,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ChangeCtrPlc		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActRenCtrSho	] = { 682,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RenameCentreShort		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActRenCtrFul	] = { 684,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RenameCentreFull		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActChgCtrWWW	] = { 683,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ChangeCtrWWW		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActChgCtrSta	] = {1209,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ChangeCtrStatus		,Ctr_ContEditAfterChgCtr	,NULL},

   [ActEdiPlc		] = { 704,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plc_EditPlaces			,NULL},
   [ActNewPlc		] = { 705,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_RecFormNewPlace		,Plc_ContEditAfterChgPlc	,NULL},
   [ActRemPlc		] = { 776,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_RemovePlace		,Plc_ContEditAfterChgPlc	,NULL},
   [ActRenPlcSho	] = { 894,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_RenamePlaceShort		,Plc_ContEditAfterChgPlc	,NULL},
   [ActRenPlcFul	] = { 895,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_RenamePlaceFull		,Plc_ContEditAfterChgPlc	,NULL},

   [ActEdiDpt		] = { 677,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dpt_EditDepartments		,NULL},
   [ActNewDpt		] = { 687,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_RecFormNewDpt		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActRemDpt		] = { 690,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_RemoveDepartment		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActChgDptIns	] = { 721,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_ChangeDepartIns		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActRenDptSho	] = { 688,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_RenameDepartShort		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActRenDptFul	] = { 689,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_RenameDepartFull		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActChgDptWWW	] = { 691,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_ChangeDptWWW		,Dpt_ContEditAfterChgDpt	,NULL},

   [ActEdiHld		] = { 713,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Hld_EditHolidays		,NULL},
   [ActNewHld		] = { 714,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_RecFormNewHoliday		,Hld_ContEditAfterChgHld	,NULL},
   [ActRemHld		] = { 716,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_RemoveHoliday		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldPlc	] = { 896,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeHolidayPlace		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldTyp	] = { 715,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeHolidayType		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldStrDat	] = { 717,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeStartDate		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldEndDat	] = { 718,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeEndDate		,Hld_ContEditAfterChgHld	,NULL},
   [ActRenHld		] = { 766,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_RenameHoliday		,Hld_ContEditAfterChgHld	,NULL},

   // TabCtr ******************************************************************
   // Actions in menu:
   [ActSeeCtrInf	] = {1151, 0,TabCtr,ActSeeCtrInf	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ShowConfiguration		,"info"			},
   [ActSeeDeg		] = {1011, 1,TabCtr,ActSeeDeg		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Deg_ShowDegsOfCurrentCtr	,"graduation-cap"	},
   [ActSeeCla		] = {1744, 2,TabCtr,ActSeeCla		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cla_SeeClassrooms		,"classroom"		},

   // Actions not in menu:
   [ActPrnCtrInf	] = {1152,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,CtrCfg_PrintConfiguration		,NULL},
   [ActChgCtrInsCfg	] = {1589,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CtrCfg_ChangeCtrInsInConfig	,CtrCfg_ContEditAfterChgCtrInConfig,NULL},
   [ActRenCtrShoCfg	] = {1595,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CtrCfg_RenameCentreShortInConfig	,CtrCfg_ContEditAfterChgCtrInConfig,NULL},
   [ActRenCtrFulCfg	] = {1594,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CtrCfg_RenameCentreFullInConfig	,CtrCfg_ContEditAfterChgCtrInConfig,NULL},
   [ActChgCtrPlcCfg	] = {1648,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrPlcInConfig	,NULL},
   [ActChgCtrLatCfg	] = {1815,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrLatitudeInConfig	,NULL},
   [ActChgCtrLgtCfg	] = {1816,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrLongitudeInConfig	,NULL},
   [ActChgCtrAltCfg	] = {1817,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrAltitudeInConfig	,NULL},
   [ActChgCtrWWWCfg	] = {1596,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrWWWInConfig	,NULL},
   [ActReqCtrLog	] = {1244,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_RequestLogo		,NULL},
   [ActRecCtrLog	] = {1051,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,CtrCfg_ReceiveLogo		,CtrCfg_ShowConfiguration		,NULL},
   [ActRemCtrLog	] = {1342,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,CtrCfg_RemoveLogo			,CtrCfg_ShowConfiguration		,NULL},
   [ActReqCtrPho	] = {1160,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_RequestPhoto		,NULL},
   [ActRecCtrPho	] = {1161,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,CtrCfg_ReceivePhoto		,NULL},
   [ActChgCtrPhoAtt	] = {1159,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrPhotoAttribution	,NULL},

   [ActSeeDegTyp	] = {1013,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DT_SeeDegreeTypesInDegTab	,NULL},
   [ActEdiDegTyp	] = { 573,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DT_EditDegreeTypes		,NULL},
   [ActNewDegTyp	] = { 537,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DT_RecFormNewDegreeType	,DT_ContEditAfterChgDegTyp	,NULL},
   [ActRemDegTyp	] = { 545,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DT_RemoveDegreeType		,DT_ContEditAfterChgDegTyp	,NULL},
   [ActRenDegTyp	] = { 538,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DT_RenameDegreeType		,DT_ContEditAfterChgDegTyp	,NULL},

   [ActEdiDeg		] = { 536,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Deg_EditDegrees		,NULL},
   [ActReqDeg		] = {1206,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RecFormReqDeg		,Deg_ContEditAfterChgDeg	,NULL},
   [ActNewDeg		] = { 540,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RecFormNewDeg		,Deg_ContEditAfterChgDeg	,NULL},
   [ActRemDeg		] = { 542,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RemoveDegree		,Deg_ContEditAfterChgDeg	,NULL},
   [ActRenDegSho	] = { 546,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RenameDegreeShort		,Deg_ContEditAfterChgDeg	,NULL},
   [ActRenDegFul	] = { 547,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RenameDegreeFull		,Deg_ContEditAfterChgDeg	,NULL},
   [ActChgDegTyp	] = { 544,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ChangeDegreeType		,Deg_ContEditAfterChgDeg	,NULL},
   [ActChgDegWWW	] = { 554,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ChangeDegWWW		,Deg_ContEditAfterChgDeg	,NULL},
   [ActChgDegSta	] = {1207,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ChangeDegStatus		,Deg_ContEditAfterChgDeg	,NULL},

   [ActEdiCla		] = {1745,-1,TabUnk,ActSeeCla		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cla_EditClassrooms		,NULL},
   [ActNewCla		] = {1746,-1,TabUnk,ActSeeCla		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cla_RecFormNewClassroom	,Cla_ContEditAfterChgCla	,NULL},
   [ActRemCla		] = {1747,-1,TabUnk,ActSeeCla		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cla_RemoveClassroom		,Cla_ContEditAfterChgCla	,NULL},
   [ActRenClaSho	] = {1748,-1,TabUnk,ActSeeCla		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cla_RenameClassroomShort	,Cla_ContEditAfterChgCla	,NULL},
   [ActRenClaFul	] = {1749,-1,TabUnk,ActSeeCla		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cla_RenameClassroomFull	,Cla_ContEditAfterChgCla	,NULL},
   [ActChgClaMaxStd	] = {1750,-1,TabUnk,ActSeeCla		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cla_ChangeCapacity		,Cla_ContEditAfterChgCla	,NULL},
   [ActRenClaLoc	] = {1751,-1,TabUnk,ActSeeCla		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cla_ChangeClassroomLocation	,Cla_ContEditAfterChgCla	,NULL},

   // TabDeg ******************************************************************
   // Actions in menu:
   [ActSeeDegInf	] = {1149, 0,TabDeg,ActSeeDegInf	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DegCfg_ShowConfiguration		,"info"			},
   [ActSeeCrs		] = {1009, 1,TabDeg,ActSeeCrs		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_ShowCrssOfCurrentDeg	,"list-ol"		},

   // Actions not in menu:
   [ActPrnDegInf	] = {1150,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,DegCfg_PrintConfiguration		,NULL},
   [ActChgDegCtrCfg	] = {1588,-1,TabUnk,ActSeeDegInf	,    0,    0,0x300,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegCfg_ChangeDegCtrInConfig	,DegCfg_ContEditAfterChgDegInConfig,NULL},
   [ActRenDegShoCfg	] = {1598,-1,TabUnk,ActSeeDegInf	,    0,    0,0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegCfg_RenameDegreeShortInConfig	,DegCfg_ContEditAfterChgDegInConfig,NULL},
   [ActRenDegFulCfg	] = {1597,-1,TabUnk,ActSeeDegInf	,    0,    0,0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegCfg_RenameDegreeFullInConfig	,DegCfg_ContEditAfterChgDegInConfig,NULL},
   [ActChgDegWWWCfg	] = {1599,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DegCfg_ChangeDegWWWInConfig	,NULL},
   [ActReqDegLog	] = {1246,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Deg_RequestLogo		,NULL},
   [ActRecDegLog	] = { 553,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,Deg_ReceiveLogo		,DegCfg_ShowConfiguration		,NULL},
   [ActRemDegLog	] = {1343,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,Deg_RemoveLogo			,DegCfg_ShowConfiguration		,NULL},

   [ActEdiCrs		] = { 555,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_EditCourses		,NULL},
   [ActReqCrs		] = {1053,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RecFormReqCrs		,Crs_ContEditAfterChgCrs	,NULL},
   [ActNewCrs		] = { 556,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RecFormNewCrs		,Crs_ContEditAfterChgCrs	,NULL},
   [ActRemCrs		] = { 560,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RemoveCourse		,Crs_ContEditAfterChgCrs	,NULL},
   [ActChgInsCrsCod	] = {1025,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ChangeInsCrsCod		,Crs_ContEditAfterChgCrs	,NULL},
   [ActChgCrsYea	] = { 561,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ChangeCrsYear		,Crs_ContEditAfterChgCrs	,NULL},
   [ActRenCrsSho	] = { 563,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RenameCourseShort		,Crs_ContEditAfterChgCrs	,NULL},
   [ActRenCrsFul	] = { 564,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RenameCourseFull		,Crs_ContEditAfterChgCrs	,NULL},
   [ActChgCrsSta	] = {1055,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ChangeCrsStatus		,Crs_ContEditAfterChgCrs	,NULL},

   // TabCrs ******************************************************************
   // Actions in menu:
   [ActSeeCrsInf	] = { 847, 0,TabCrs,ActSeeCrsInf	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_ShowIntroduction		,"info"			},
   [ActSeeTchGui	] = { 784, 1,TabCrs,ActSeeTchGui	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"book-open"		},
   [ActSeeSyl		] = {1242, 2,TabCrs,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"list-ol"		},
   [ActSeeBib		] = {  32, 3,TabCrs,ActSeeBib		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"book"			},
   [ActSeeFAQ		] = {  54, 4,TabCrs,ActSeeFAQ		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"question"		},
   [ActSeeCrsLnk	] = {   9, 5,TabCrs,ActSeeCrsLnk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"link"			},
   [ActSeeCrsTT		] = {  25, 6,TabCrs,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TT_ShowClassTimeTable		,"clock"		},

   // Actions not in menu:
   [ActPrnCrsInf	] = {1028,-1,TabUnk,ActSeeCrsInf	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,CrsCfg_PrintConfiguration		,NULL},
   [ActChgCrsDegCfg	] = {1587,-1,TabUnk,ActSeeCrsInf	,0x380,0x380,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_ChangeCrsDegInConfig	,CrsCfg_ContEditAfterChgCrsInConfig,NULL},
   [ActRenCrsShoCfg	] = {1601,-1,TabUnk,ActSeeCrsInf	,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_RenameCourseShortInConfig	,CrsCfg_ContEditAfterChgCrsInConfig,NULL},
   [ActRenCrsFulCfg	] = {1600,-1,TabUnk,ActSeeCrsInf	,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_RenameCourseFullInConfig	,CrsCfg_ContEditAfterChgCrsInConfig,NULL},
   [ActChgInsCrsCodCfg	] = {1024,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_ChangeInsCrsCodInConfig	,CrsCfg_ContEditAfterChgCrsInConfig,NULL},
   [ActChgCrsYeaCfg	] = {1573,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_ChangeCrsYearInConfig	,CrsCfg_ContEditAfterChgCrsInConfig,NULL},
   [ActEdiCrsInf	] = { 848,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},

   [ActEdiTchGui	] = { 785,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},

   [ActSeeSylLec	] = {  28,-1,TabUnk,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,NULL},
   [ActSeeSylPra	] = {  20,-1,TabUnk,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,NULL},
   [ActEdiSylLec	] = {  44,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActEdiSylPra	] = {  74,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActDelItmSylLec	] = { 218,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RemoveItemSyllabus		,NULL},
   [ActDelItmSylPra	] = { 183,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RemoveItemSyllabus		,NULL},
   [ActUp_IteSylLec	] = { 221,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_UpItemSyllabus		,NULL},
   [ActUp_IteSylPra	] = { 213,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_UpItemSyllabus		,NULL},
   [ActDwnIteSylLec	] = { 220,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_DownItemSyllabus		,NULL},
   [ActDwnIteSylPra	] = { 212,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_DownItemSyllabus		,NULL},
   [ActRgtIteSylLec	] = { 223,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RightItemSyllabus		,NULL},
   [ActRgtIteSylPra	] = { 215,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RightItemSyllabus		,NULL},
   [ActLftIteSylLec	] = { 222,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_LeftItemSyllabus		,NULL},
   [ActLftIteSylPra	] = { 214,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_LeftItemSyllabus		,NULL},
   [ActInsIteSylLec	] = { 217,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_InsertItemSyllabus		,NULL},
   [ActInsIteSylPra	] = { 181,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_InsertItemSyllabus		,NULL},
   [ActModIteSylLec	] = { 211,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_ModifyItemSyllabus		,NULL},
   [ActModIteSylPra	] = { 216,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_ModifyItemSyllabus		,NULL},

   [ActEdiBib		] = {  76,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActEdiFAQ		] = { 109,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActEdiCrsLnk	] = {  96,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},

   [ActChgFrcReaCrsInf	] = { 877,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaTchGui	] = { 870,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaSylLec	] = { 871,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaSylPra	] = { 872,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaBib	] = { 873,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaFAQ	] = { 874,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaCrsLnk	] = { 875,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},

   [ActChgHavReaCrsInf	] = { 878,-1,TabUnk,ActSeeCrsInf	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaTchGui	] = { 886,-1,TabUnk,ActSeeTchGui	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaSylLec	] = { 880,-1,TabUnk,ActSeeSyl		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaSylPra	] = { 887,-1,TabUnk,ActSeeSyl		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaBib	] = { 884,-1,TabUnk,ActSeeBib		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaFAQ	] = { 879,-1,TabUnk,ActSeeFAQ		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaCrsLnk	] = { 885,-1,TabUnk,ActSeeCrsLnk	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},

   [ActSelInfSrcCrsInf	] = { 849,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcTchGui	] = { 789,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcSylLec	] = { 378,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcSylPra	] = { 382,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcBib	] = { 370,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcFAQ	] = { 380,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcCrsLnk	] = { 385,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},

   [ActRcvURLCrsInf	] = { 854,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLTchGui	] = { 791,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLSylLec	] = { 403,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLSylPra	] = { 402,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLBib	] = { 224,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLFAQ	] = { 234,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLCrsLnk	] = { 182,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},

   [ActRcvPagCrsInf	] = { 853,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagTchGui	] = { 788,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagSylLec	] = { 381,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagSylPra	] = { 383,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagBib	] = { 185,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagFAQ	] = { 219,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagCrsLnk	] = { 164,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},

   [ActEditorCrsInf	] = { 852,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorCourseInfo		,NULL},
   [ActEditorTchGui	] = { 786,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorTeachingGuide	,NULL},
   [ActEditorSylLec	] = { 372,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_EditSyllabus		,NULL},
   [ActEditorSylPra	] = { 371,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_EditSyllabus		,NULL},
   [ActEditorBib	] = { 376,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorBibliography		,NULL},
   [ActEditorFAQ	] = { 404,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorFAQ   		,NULL},
   [ActEditorCrsLnk	] = { 388,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorLinks		,NULL},

   [ActPlaTxtEdiCrsInf	] = { 850,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiTchGui	] = { 787,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiSylLec	] = { 379,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiSylPra	] = { 389,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiBib	] = { 377,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiFAQ	] = { 405,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo   	,NULL},
   [ActPlaTxtEdiCrsLnk] = { 400,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},

   [ActRchTxtEdiCrsInf	] = {1093,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiTchGui	] = {1094,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiSylLec	] = {1095,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiSylPra	] = {1096,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiBib	] = {1097,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiFAQ	] = {1098,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo   		,NULL},
   [ActRchTxtEdiCrsLnk	] = {1099,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},

   [ActRcvPlaTxtCrsInf	] = { 851,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtTchGui	] = { 790,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtSylLec	] = { 394,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtSylPra	] = { 396,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtBib	] = { 398,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtFAQ	] = { 406,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtCrsLnk	] = { 401,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},

   [ActRcvRchTxtCrsInf	] = {1101,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtTchGui	] = {1102,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtSylLec	] = {1103,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtSylPra	] = {1104,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtBib	] = {1105,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtFAQ	] = {1106,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtCrsLnk	] = {1107,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},

   [ActPrnCrsTT		] = { 152,-1,TabUnk,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,TT_ShowClassTimeTable		,NULL},
   [ActEdiCrsTT		] = {  45,-1,TabUnk,ActSeeCrsTT		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TT_EditCrsTimeTable		,NULL},
   [ActChgCrsTT		] = {  53,-1,TabUnk,ActSeeCrsTT		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TT_EditCrsTimeTable		,NULL},
   [ActChgCrsTT1stDay	] = {1486,-1,TabUnk,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,TT_ShowClassTimeTable		,NULL},

   // TabAss ******************************************************************
   // Actions in menu:
   [ActSeeAss		] = {  15, 0,TabAss,ActSeeAss		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"info"			},
   [ActSeeAsg		] = { 801, 1,TabAss,ActSeeAsg		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_SeeAssignments		,"edit"			},
   [ActSeePrj		] = {1674, 2,TabAss,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_SeeProjects		,"file-alt"		},
   [ActReqTst		] = { 103, 3,TabAss,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowFormAskTst		,"check"		},
   [ActSeeAllGam	] = {1649, 4,TabAss,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_SeeAllGames		,"gamepad"		},
   [ActSeeAllSvy	] = { 966, 5,TabAss,ActSeeAllSvy	,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_SeeAllSurveys		,"poll"			},
   [ActSeeAllExaAnn	] = {  85, 6,TabAss,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_ListExamAnnouncementsSee	,"bullhorn"		},

   // Actions not in menu:
   [ActEdiAss		] = {  69,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActChgFrcReaAss	] = { 883,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgHavReaAss	] = { 898,-1,TabUnk,ActSeeAss		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActSelInfSrcAss	] = { 384,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActRcvURLAss	] = { 235,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvPagAss	] = { 184,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActEditorAss	] = { 386,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorAssessment		,NULL},
   [ActPlaTxtEdiAss	] = { 387,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActRchTxtEdiAss	] = {1100,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRcvPlaTxtAss	] = { 397,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvRchTxtAss	] = {1108,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},

   [ActFrmNewAsg	] = { 812,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_RequestCreatOrEditAsg	,NULL},
   [ActEdiOneAsg	] = { 814,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_RequestCreatOrEditAsg	,NULL},
   [ActPrnOneAsg	] = {1637,-1,TabUnk,ActSeeAsg		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Asg_PrintOneAssignment		,NULL},
   [ActNewAsg		] = { 803,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_RecFormAssignment		,NULL},
   [ActChgAsg		] = { 815,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_RecFormAssignment		,NULL},
   [ActReqRemAsg	] = { 813,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_ReqRemAssignment		,NULL},
   [ActRemAsg		] = { 806,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_RemoveAssignment		,NULL},
   [ActHidAsg		] = { 964,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_HideAssignment		,NULL},
   [ActShoAsg		] = { 965,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_ShowAssignment		,NULL},

   [ActReqUsrPrj	] = {1805,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ListUsrsToSelect		,NULL},
   [ActSeeTblAllPrj	] = {1696,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Prj_ShowTableSelectedPrjs	,NULL},
   [ActCfgPrj		] = {1803,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ShowFormConfig		,NULL},
   [ActRcvCfgPrj	] = {1804,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReceiveConfigPrj		,NULL},
   [ActReqLckAllPrj	] = {1775,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqLockSelectedPrjsEdition	,NULL},
   [ActReqUnlAllPrj	] = {1776,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqUnloSelectedPrjsEdition	,NULL},
   [ActLckAllPrj	] = {1777,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_LockSelectedPrjsEdition	,NULL},
   [ActUnlAllPrj	] = {1778,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_UnloSelectedPrjsEdition	,NULL},
   [ActFrmNewPrj	] = {1675,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RequestCreatePrj		,NULL},
   [ActEdiOnePrj	] = {1676,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RequestEditPrj		,NULL},
   [ActPrnOnePrj	] = {1677,-1,TabUnk,ActSeePrj		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Prj_PrintOneProject		,NULL},
   [ActNewPrj		] = {1678,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RecFormProject		,NULL},
   [ActChgPrj		] = {1679,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RecFormProject		,NULL},
   [ActReqRemPrj	] = {1680,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemProject		,NULL},
   [ActRemPrj		] = {1681,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemoveProject		,NULL},
   [ActHidPrj		] = {1682,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_HideProject		,NULL},
   [ActShoPrj		] = {1683,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_UnhideProject		,NULL},
   [ActLckPrj		] = {1773,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,Prj_LockProjectEdition		,NULL},
   [ActUnlPrj		] = {1774,-1,TabUnk,ActSeePrj		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,Prj_UnloProjectEdition		,NULL},
   [ActReqAddStdPrj	] = {1684,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqAddStds			,NULL},
   [ActReqAddTutPrj	] = {1685,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqAddTuts			,NULL},
   [ActReqAddEvlPrj	] = {1686,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqAddEvls			,NULL},
   [ActAddStdPrj	] = {1687,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_GetSelectedUsrsAndAddStds	,NULL},
   [ActAddTutPrj	] = {1688,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_GetSelectedUsrsAndAddTuts	,NULL},
   [ActAddEvlPrj	] = {1689,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_GetSelectedUsrsAndAddEvls	,NULL},
   [ActReqRemStdPrj	] = {1690,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemStd			,NULL},
   [ActReqRemTutPrj	] = {1691,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemTut			,NULL},
   [ActReqRemEvlPrj	] = {1692,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemEvl			,NULL},
   [ActRemStdPrj	] = {1693,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemStd			,NULL},
   [ActRemTutPrj	] = {1694,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemTut			,NULL},
   [ActRemEvlPrj	] = {1695,-1,TabUnk,ActSeePrj		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemEvl			,NULL},

   [ActAdmDocPrj	] = {1697,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocPrj	] = {1698,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocPrj	] = {1699,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocPrj	] = {1700,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocPrj	] = {1701,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocPrj	] = {1702,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocPrj	] = {1703,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocPrj	] = {1704,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocPrj	] = {1705,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocPrj	] = {1706,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocPrj	] = {1707,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocPrjDZ	] = {1708,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocPrjCla	] = {1709,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpDocPrj	] = {1710,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConDocPrj	] = {1711,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPDocPrj	] = {1712,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatDocPrj	] = {1713,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatDocPrj	] = {1714,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowDocPrj	] = {1715,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmAssPrj	] = {1716,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilAssPrj	] = {1717,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilAssPrj	] = {1718,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolAssPrj	] = {1719,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopAssPrj	] = {1720,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasAssPrj	] = {1721,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreAssPrj	] = {1722,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreAssPrj	] = {1723,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolAssPrj	] = {1724,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkAssPrj	] = {1725,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolAssPrj	] = {1726,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilAssPrjDZ	] = {1727,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilAssPrjCla	] = {1728,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAssPrj	] = {1729,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAssPrj	] = {1730,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAssPrj	] = {1731,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatAssPrj	] = {1732,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAssPrj	] = {1733,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAssPrj	] = {1734,-1,TabUnk,ActSeePrj		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActSeeTst		] = {  29,-1,TabUnk,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowNewTest		,NULL},
   [ActAssTst		] = {  98,-1,TabUnk,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_AssessTest			,NULL},

   [ActEdiTstQst	] = { 104,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDates		,Tst_ShowFormAskEditTsts	,NULL},
   [ActEdiOneTstQst	] = { 105,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowFormEditOneQst		,NULL},
   [ActReqImpTstQst	] = {1007,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TsI_ShowFormImportQstsFromXML	,NULL},
   [ActImpTstQst	] = {1008,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,TsI_ImportQstsFromXML		,NULL},
   [ActLstTstQst	] = { 132,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ListQuestionsToEdit	,NULL},
   [ActRcvTstQst	] = { 126,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Tst_ReceiveQst			,NULL},
   [ActReqRemTstQst	] = {1523,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_RequestRemoveQst		,NULL},
   [ActRemTstQst	] = { 133,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_RemoveQst			,NULL},
   [ActShfTstQst	] = { 455,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ChangeShuffleQst		,NULL},

   [ActCfgTst		] = { 451,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowFormConfig		,NULL},
   [ActEnableTag	] = { 453,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_EnableTag			,NULL},
   [ActDisableTag	] = { 452,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_DisableTag			,NULL},
   [ActRenTag		] = { 143,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_RenameTag			,NULL},
   [ActRcvCfgTst	] = { 454,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ReceiveConfigTst		,NULL},

   [ActReqSeeMyTstRes	] = {1083,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDates		,Tst_SelDatesToSeeMyTstResults	,NULL},
   [ActSeeMyTstRes	] = {1084,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowMyTstResults		,NULL},
   [ActSeeOneTstResMe	] = {1085,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowOneTstResult		,NULL},
   [ActReqSeeUsrTstRes	] = {1080,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDates		,Tst_SelUsrsToViewUsrsTstResults,NULL},
   [ActSeeUsrTstRes	] = {1081,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_GetUsrsAndShowTstResults	,NULL},
   [ActSeeOneTstResOth	] = {1082,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowOneTstResult		,NULL},

   [ActSeeGam		] = {1650,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_SeeOneGame			,NULL},

   [ActReqRemMch	] = {1783,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_RequestRemoveMatch		,NULL},
   [ActRemMch		] = {1784,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_RemoveMatch		,NULL},
   [ActReqNewMch	] = {1670,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RequestNewMatch		,NULL},
   [ActNewMch		] = {1671,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mch_CreateNewMatchTch		,Mch_ResumeMatch		,NULL},
   [ActResMch		] = {1785,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mch_GetMatchBeingPlayed	,Mch_ResumeMatch		,NULL},
   [ActBckMch		] = {1790,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_BackMatch			,NULL},
   [ActPlyPauMch	] = {1789,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_PlayPauseMatch		,NULL},
   [ActFwdMch		] = {1672,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ForwardMatch		,NULL},
   [ActChgNumColMch	] = {1802,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ChangeNumColsMch		,NULL},
   [ActChgVisResMchQst	] = {1794,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ToggleVisibilResultsMchQst	,NULL},
   [ActMchCntDwn	] = {1814,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_RFRESH,Mch_GetMatchBeingPlayed	,Mch_StartCountdown		,NULL},
   [ActRefMchTch	] = {1788,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_RFRESH,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchTch		,NULL},

   [ActJoiMch		] = {1780,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		,NULL},
   [ActSeeMchAnsQstStd	] = {1808,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		,NULL},
   [ActRemMchAnsQstStd	] = {1809,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_RemoveMyQuestionAnswer	,NULL},
   [ActAnsMchQstStd	] = {1651,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ReceiveQuestionAnswer	,NULL},
   [ActRefMchStd	] = {1782,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_RFRESH,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchStd		,NULL},

   [ActSeeMyMchResCrs	] = {1796,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowMyMchResultsInCrs	,NULL},
   [ActSeeMyMchResGam	] = {1810,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowMyMchResultsInGam	,NULL},
   [ActSeeMyMchResMch	] = {1812,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowMyMchResultsInMch	,NULL},
   [ActSeeOneMchResMe	] = {1797,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowOneMchResult		,NULL},

   [ActReqSeeAllMchRes	] = {1798,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_SelUsrsToViewMchResults	,NULL},
   [ActSeeAllMchResCrs	] = {1799,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowAllMchResultsInCrs	,NULL},
   [ActSeeAllMchResGam	] = {1811,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowAllMchResultsInGam	,NULL},
   [ActSeeAllMchResMch	] = {1813,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowAllMchResultsInMch	,NULL},
   [ActSeeOneMchResOth	] = {1800,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,McR_ShowOneMchResult		,NULL},

   [ActChgVisResMchUsr	] = {1801,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_ToggleVisibilResultsMchUsr	,NULL},

   [ActFrmNewGam	] = {1652,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RequestCreatOrEditGame	,NULL},
   [ActEdiOneGam	] = {1653,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RequestCreatOrEditGame	,NULL},
   [ActNewGam		] = {1654,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RecFormGame		,NULL},
   [ActChgGam		] = {1655,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RecFormGame		,NULL},
   [ActReqRemGam	] = {1656,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_AskRemGame			,NULL},
   [ActRemGam		] = {1657,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RemoveGame			,NULL},
   [ActHidGam		] = {1660,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_HideGame			,NULL},
   [ActShoGam		] = {1661,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_UnhideGame			,NULL},
   [ActAddOneGamQst	] = {1662,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RequestNewQuestion		,NULL},
   [ActGamLstTstQst	] = {1666,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ListTstQuestionsToSelect	,NULL},
   [ActAddTstQstToGam	] = {1667,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_AddTstQuestionsToGame	,NULL},
   [ActReqRemGamQst	] = {1664,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RequestRemoveQst		,NULL},
   [ActRemGamQst	] = {1665,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RemoveQst			,NULL},
   [ActUp_GamQst	] = {1668,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_MoveUpQst			,NULL},
   [ActDwnGamQst	] = {1669,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_MoveDownQst		,NULL},

   [ActSeeSvy		] = { 982,-1,TabUnk,ActSeeAllSvy	,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_SeeOneSurvey		,NULL},
   [ActAnsSvy		] = { 983,-1,TabUnk,ActSeeAllSvy	,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReceiveSurveyAnswers	,NULL},
   [ActFrmNewSvy	] = { 973,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RequestCreatOrEditSvy	,NULL},
   [ActEdiOneSvy	] = { 974,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RequestCreatOrEditSvy	,NULL},
   [ActNewSvy		] = { 968,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RecFormSurvey		,NULL},
   [ActChgSvy		] = { 975,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RecFormSurvey		,NULL},
   [ActReqRemSvy	] = { 976,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_AskRemSurvey		,NULL},
   [ActRemSvy		] = { 969,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RemoveSurvey		,NULL},
   [ActReqRstSvy	] = { 984,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_AskResetSurvey		,NULL},
   [ActRstSvy		] = { 985,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ResetSurvey		,NULL},
   [ActHidSvy		] = { 977,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_HideSurvey			,NULL},
   [ActShoSvy		] = { 978,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_UnhideSurvey		,NULL},
   [ActEdiOneSvyQst	] = { 979,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RequestEditQuestion	,NULL},
   [ActRcvSvyQst	] = { 980,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReceiveQst			,NULL},
   [ActReqRemSvyQst	] = {1524,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RequestRemoveQst		,NULL},
   [ActRemSvyQst	] = { 981,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RemoveQst			,NULL},

   [ActSeeOneExaAnn	] = {1572,-1,TabUnk,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Exa_GetExaCodToHighlight	,Exa_ListExamAnnouncementsSee	,NULL},
   [ActSeeDatExaAnn	] = {1571,-1,TabUnk,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Exa_GetDateToHighlight		,Exa_ListExamAnnouncementsSee	,NULL},
   [ActEdiExaAnn	] = {  91,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_PutFrmEditAExamAnnouncement,NULL},
   [ActRcvExaAnn	] = { 110,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Exa_ReceiveExamAnnouncement1	,Exa_ReceiveExamAnnouncement2	,NULL},
   [ActPrnExaAnn	] = { 179,-1,TabUnk,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Exa_PrintExamAnnouncement	,NULL},
   [ActReqRemExaAnn	] = {1619,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_ReqRemoveExamAnnouncement	,NULL},
   [ActRemExaAnn	] = { 187,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Exa_RemoveExamAnnouncement1	,Exa_RemoveExamAnnouncement2	,NULL},
   [ActHidExaAnn	] = {1620,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Exa_HideExamAnnouncement	,Exa_ListExamAnnouncementsEdit	,NULL},
   [ActShoExaAnn	] = {1621,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Exa_UnhideExamAnnouncement	,Exa_ListExamAnnouncementsEdit	,NULL},

   // TabFil ******************************************************************
   // Actions in menu:
   [ActSeeAdmDocIns	] = {1249, 0,TabFil,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaIns	] = {1382, 1,TabFil,ActAdmShaIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActSeeAdmDocCtr	] = {1248, 2,TabFil,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaCtr	] = {1363, 3,TabFil,ActAdmShaCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActSeeAdmDocDeg	] = {1247, 4,TabFil,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaDeg	] = {1344, 5,TabFil,ActAdmShaDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActSeeAdmDocCrsGrp	] = {   0, 6,TabFil,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmTchCrsGrp	] = {1525, 7,TabFil,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaCrsGrp	] = { 461, 8,TabFil,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmAsgWrkUsr	] = { 792, 9,TabFil,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActReqAsgWrkCrs	] = { 899,10,TabFil,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskEditWorksCrs		,"folder-open"		},
   [ActSeeAdmMrk	] = {  17,11,TabFil,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"clipboard-list"	},
   [ActAdmBrf		] = {  23,12,TabFil,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"briefcase"		},

   // Actions not in menu:
   [ActChgToSeeDocIns	] = {1308,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocIns	] = {1309,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocIns	] = {1310,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocIns	] = {1311,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocIns	] = {1312,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocIns	] = {1313,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocIns	] = {1314,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocIns	] = {1315,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocIns	] = {1316,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocIns	] = {1317,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocIns	] = {1318,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocIns	] = {1319,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocIns	] = {1320,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocIns	] = {1321,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocIns	] = {1322,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocIns	] = {1323,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocIns	] = {1324,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocIns	] = {1325,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocIns	] = {1326,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocInsDZ	] = {1327,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocInsCla	] = {1328,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocIns	] = {1329,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocIns	] = {1330,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocIns	] = {1331,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActShoDocIns	] = {1332,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocIns	] = {1333,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocIns	] = {1334,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocIns	] = {1335,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocIns   	] = {1336,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilShaIns	] = {1383,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaIns	] = {1384,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaIns	] = {1385,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaIns	] = {1386,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaIns	] = {1387,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaIns	] = {1388,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaIns	] = {1389,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaIns	] = {1390,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaIns	] = {1391,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaIns	] = {1392,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaInsDZ	] = {1393,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaInsCla	] = {1394,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaIns	] = {1395,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaIns	] = {1396,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaIns	] = {1397,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaIns	] = {1398,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaIns	] = {1399,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaIns	] = {1400,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeDocCtr	] = {1279,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocCtr	] = {1280,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocCtr	] = {1281,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocCtr	] = {1282,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocCtr	] = {1283,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocCtr	] = {1284,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocCtr   	] = {1285,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocCtr	] = {1286,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocCtr	] = {1287,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocCtr	] = {1288,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocCtr	] = {1289,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocCtr	] = {1290,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocCtr	] = {1291,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocCtr	] = {1292,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocCtr	] = {1293,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocCtr	] = {1294,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocCtr	] = {1295,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocCtr	] = {1296,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocCtr	] = {1297,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocCtrDZ	] = {1298,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocCtrCla	] = {1299,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocCtr	] = {1300,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocCtr	] = {1301,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocCtr	] = {1302,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActShoDocCtr	] = {1303,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocCtr	] = {1304,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocCtr	] = {1305,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocCtr	] = {1306,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocCtr   	] = {1307,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilShaCtr	] = {1364,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaCtr	] = {1365,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaCtr	] = {1366,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaCtr	] = {1367,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaCtr	] = {1368,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaCtr	] = {1369,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaCtr	] = {1370,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaCtr	] = {1371,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaCtr	] = {1372,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaCtr	] = {1373,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaCtrDZ	] = {1374,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaCtrCla	] = {1375,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaCtr	] = {1376,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaCtr	] = {1377,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaCtr	] = {1378,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaCtr	] = {1379,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaCtr	] = {1380,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaCtr	] = {1381,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeDocDeg	] = {1250,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocDeg	] = {1251,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocDeg	] = {1252,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocDeg	] = {1253,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocDeg	] = {1254,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocDeg	] = {1255,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocDeg	] = {1256,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocDeg	] = {1257,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocDeg	] = {1258,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocDeg	] = {1259,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocDeg	] = {1260,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocDeg	] = {1261,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocDeg	] = {1262,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocDeg	] = {1263,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocDeg	] = {1264,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocDeg	] = {1265,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocDeg	] = {1266,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocDeg	] = {1267,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocDeg	] = {1268,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocDegDZ	] = {1269,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocDegCla	] = {1270,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocDeg	] = {1271,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocDeg	] = {1272,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocDeg	] = {1273,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActShoDocDeg	] = {1274,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocDeg	] = {1275,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocDeg	] = {1276,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocDeg	] = {1277,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocDeg	] = {1278,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilShaDeg	] = {1345,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaDeg	] = {1346,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaDeg	] = {1347,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaDeg	] = {1348,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaDeg	] = {1349,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaDeg	] = {1350,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaDeg	] = {1351,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaDeg	] = {1352,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaDeg	] = {1353,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaDeg	] = {1354,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaDegDZ	] = {1355,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaDegCla	] = {1356,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaDeg	] = {1357,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaDeg	] = {1358,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaDeg	] = {1359,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaDeg	] = {1360,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaDeg	] = {1361,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaDeg	] = {1362,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeDocCrs	] = {1195,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocCrs	] = {1078,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocCrs	] = { 462,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocCrs	] = { 476,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocCrs	] = {1124,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocCrs	] = {1033,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocCrs	] = {1111,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActSeeDocGrp	] = {1200,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocGrp	] = { 488,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocGrp	] = { 489,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocGrp	] = {1125,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocGrp	] = {1034,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocGrp	] = {1112,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocCrs	] = {1196,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocCrs	] = {  12,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocCrs	] = { 479,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocCrs	] = { 480,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocCrs	] = { 497,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocCrs	] = { 470,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocCrs	] = { 478,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocCrs	] = { 498,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocCrs	] = { 481,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocCrs	] = { 491,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocCrs	] = {1225,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocCrs	] = { 535,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocCrsDZ	] = {1214,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocCrsCla	] = { 482,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocCrs	] = { 477,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocCrs	] = { 494,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocCrs	] = {1126,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActShoDocCrs	] = { 464,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocCrs	] = { 465,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocCrs	] = {1029,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocCrs	] = { 996,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocCrs	] = {1113,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmDocGrp	] = {1201,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocGrp	] = { 473,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocGrp	] = { 474,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocGrp	] = { 484,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocGrp	] = { 472,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocGrp	] = { 471,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocGrp	] = { 485,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocGrp	] = { 468,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocGrp	] = { 469,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocGrp	] = {1231,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocGrp	] = { 490,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocGrpDZ	] = {1215,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocGrpCla	] = { 483,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocGrp	] = { 486,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocGrp	] = { 487,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocGrp	] = {1127,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActShoDocGrp	] = { 493,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocGrp	] = { 492,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocGrp	] = {1030,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocGrp	] = { 998,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocGrp	] = {1114,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmTch	] = {1526,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmTchCrs	] = {1527,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilTchCrs	] = {1528,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilTchCrs	] = {1529,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolTchCrs	] = {1530,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopTchCrs	] = {1531,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasTchCrs	] = {1532,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreTchCrs	] = {1533,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreTchCrs	] = {1534,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolTchCrs	] = {1535,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkTchCrs	] = {1536,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolTchCrs	] = {1537,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilTchCrsDZ	] = {1538,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilTchCrsCla	] = {1539,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpTchCrs	] = {1540,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConTchCrs	] = {1541,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPTchCrs	] = {1542,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatTchCrs	] = {1543,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatTchCrs	] = {1544,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowTchCrs	] = {1545,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmTchGrp	] = {1546,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilTchGrp	] = {1547,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilTchGrp	] = {1548,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolTchGrp	] = {1549,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopTchGrp	] = {1550,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasTchGrp	] = {1551,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreTchGrp	] = {1552,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreTchGrp	] = {1553,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolTchGrp	] = {1554,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkTchGrp	] = {1555,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolTchGrp	] = {1556,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilTchGrpDZ	] = {1557,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilTchGrpCla	] = {1558,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpTchGrp	] = {1559,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConTchGrp	] = {1560,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPTchGrp	] = {1561,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatTchGrp	] = {1562,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatTchGrp	] = {1563,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowTchGrp	] = {1564,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmSha	] = {1197,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmShaCrs	] = {1202,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilShaCrs	] = { 327,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaCrs	] = { 328,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaCrs	] = { 325,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaCrs	] = { 330,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaCrs	] = { 331,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaCrs	] = { 332,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaCrs	] = { 323,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaCrs	] = { 324,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaCrs	] = {1226,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaCrs	] = { 329,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaCrsDZ	] = {1216,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaCrsCla	] = { 326,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaCrs	] = { 421,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaCrs	] = { 422,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaCrs	] = {1128,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaCrs	] = {1031,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaCrs	] = {1000,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaCrs	] = {1115,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmShaGrp	] = {1203,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilShaGrp	] = { 341,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaGrp	] = { 342,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaGrp	] = { 338,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaGrp	] = { 336,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaGrp	] = { 337,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaGrp	] = { 339,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaGrp	] = { 333,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaGrp	] = { 334,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaGrp	] = {1227,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaGrp	] = { 340,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaGrpDZ	] = {1217,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaGrpCla	] = { 335,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaGrp	] = { 427,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaGrp	] = { 426,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaGrp	] = {1129,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaGrp	] = {1032,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaGrp	] = {1002,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaGrp	] = {1116,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmAsgWrkCrs	] = { 139,-1,TabUnk,ActReqAsgWrkCrs	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_GetSelectedUsrsAndShowWorks,NULL},

   [ActReqRemFilAsgUsr	] = { 834,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilAsgUsr	] = { 833,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolAsgUsr	] = { 827,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopAsgUsr	] = { 829,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasAsgUsr	] = { 830,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreAsgUsr	] = { 828,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreAsgUsr	] = { 825,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolAsgUsr	] = { 826,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkAsgUsr	] = {1232,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolAsgUsr	] = { 839,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilAsgUsrDZ	] = {1218,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilAsgUsrCla	] = { 832,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAsgUsr	] = { 824,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAsgUsr	] = { 831,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAsgUsr	] = {1130,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatAsgUsr	] = {1039,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAsgUsr	] = {1040,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAsgUsr	] = {1117,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilWrkUsr	] = { 288,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilWrkUsr	] = { 169,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolWrkUsr	] = { 228,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopWrkUsr	] = { 314,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasWrkUsr	] = { 318,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreWrkUsr	] = { 278,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreWrkUsr	] = { 150,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolWrkUsr	] = { 172,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkWrkUsr	] = {1228,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolWrkUsr	] = { 204,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilWrkUsrDZ	] = {1219,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilWrkUsrCla	] = { 148,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpWrkUsr	] = { 423,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConWrkUsr	] = { 425,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPWrkUsr	] = {1131,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatWrkUsr	] = {1041,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatWrkUsr	] = {1042,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowWrkUsr	] = {1118,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilAsgCrs	] = { 837,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilAsgCrs	] = { 838,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolAsgCrs	] = { 820,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopAsgCrs	] = { 836,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasAsgCrs	] = { 821,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreAsgCrs	] = { 822,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreAsgCrs	] = { 817,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolAsgCrs	] = { 818,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkAsgCrs	] = {1233,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolAsgCrs	] = { 823,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilAsgCrsDZ	] = {1220,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilAsgCrsCla	] = { 846,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAsgCrs	] = { 819,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAsgCrs	] = { 835,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAsgCrs	] = {1132,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatAsgCrs	] = {1043,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAsgCrs	] = {1044,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAsgCrs	] = {1119,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilWrkCrs	] = { 289,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilWrkCrs	] = { 209,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolWrkCrs	] = { 210,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopWrkCrs	] = { 312,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasWrkCrs	] = { 319,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreWrkCrs	] = { 279,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreWrkCrs	] = { 205,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolWrkCrs	] = { 206,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkWrkCrs	] = {1229,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolWrkCrs	] = { 208,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilWrkCrsDZ	] = {1221,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilWrkCrsCla	] = { 207,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpWrkCrs	] = { 416,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConWrkCrs	] = { 424,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPWrkCrs	] = {1133,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatWrkCrs	] = {1045,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatWrkCrs	] = {1046,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowWrkCrs	] = {1120,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeMrk	] = {1198,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeMrkCrs	] = {1079,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeMrkCrs	] = { 528,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeMrkCrs	] = { 527,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActReqDatSeeMrkCrs	] = {1086,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActSeeMyMrkCrs	] = { 523,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mrk_ShowMyMarks		,NULL				,NULL},

   [ActSeeMrkGrp	] = {1204,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeMrkGrp	] = { 605,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeMrkGrp	] = { 609,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActReqDatSeeMrkGrp	] = {1087,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActSeeMyMrkGrp	] = { 524,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mrk_ShowMyMarks		,NULL				,NULL},

   [ActChgToAdmMrk	] = {1199,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmMrkCrs	] = { 284,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilMrkCrs	] = { 595,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilMrkCrs	] = { 533,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolMrkCrs	] = { 530,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopMrkCrs	] = { 501,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasMrkCrs	] = { 507,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreMrkCrs	] = { 534,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreMrkCrs	] = { 596,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolMrkCrs	] = { 506,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActRenFolMrkCrs	] = { 574,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilMrkCrsDZ	] = {1222,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilMrkCrsCla	] = { 516,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmMrkCrs	] = { 607,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmMrkCrs	] = { 621,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmMrkCrs	] = {1134,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActShoMrkCrs	] = {1191,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidMrkCrs	] = {1192,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmMrkCrs	] = {1035,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmMrkCrs	] = {1036,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmMrkCrs	] = {1121,-1,TabUnk,ActSeeAdmMrk	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   [ActChgNumRowHeaCrs	] = { 503,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsHeader	,NULL},
   [ActChgNumRowFooCrs	] = { 504,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsFooter	,NULL},

   [ActAdmMrkGrp	] = {1205,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilMrkGrp	] = { 600,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilMrkGrp	] = { 509,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolMrkGrp	] = { 520,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopMrkGrp	] = { 519,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasMrkGrp	] = { 502,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreMrkGrp	] = { 521,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreMrkGrp	] = { 601,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolMrkGrp	] = { 513,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActRenFolMrkGrp	] = { 529,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilMrkGrpDZ	] = {1223,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilMrkGrpCla	] = { 514,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmMrkGrp	] = { 631,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmMrkGrp	] = { 900,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmMrkGrp	] = {1135,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActShoMrkGrp	] = {1193,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidMrkGrp	] = {1194,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmMrkGrp	] = {1037,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmMrkGrp	] = {1038,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmMrkGrp	] = {1122,-1,TabUnk,ActSeeAdmMrk	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   [ActChgNumRowHeaGrp	] = { 510,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsHeader	,NULL},
   [ActChgNumRowFooGrp	] = { 511,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsFooter	,NULL},

   [ActReqRemFilBrf	] = { 286,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilBrf	] = { 155,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolBrf	] = { 196,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopBrf		] = { 311,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasBrf		] = { 315,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreBrf	] = { 276,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreBrf	] = { 597,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolBrf	] = { 170,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkBrf	] = {1230,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolBrf	] = { 197,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilBrfDZ	] = {1224,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilBrfCla	] = { 153,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpBrf		] = { 410,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConBrf		] = { 411,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPBrf		] = {1136,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatBrf	] = {1047,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatBrf	] = {1048,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowBrf		] = {1123,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   [ActReqRemOldBrf	] = {1488,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemoveOldFiles		,NULL},
   [ActRemOldBrf	] = {1489,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemoveOldFilesBriefcase	,NULL},

   // TabUsr ******************************************************************
   // Actions in menu:
   [ActReqSelGrp	] = { 116, 0,TabUsr,ActReqSelGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqRegisterInGrps		,"sitemap"		},
   [ActLstStd		] = { 678, 1,TabUsr,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_SeeStudents		,"users"		},
   [ActLstTch		] = { 679, 2,TabUsr,ActLstTch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_SeeTeachers		,"user-tie"		},
   [ActLstOth		] = {1186, 3,TabUsr,ActLstOth		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_ListDataAdms		,"user-friends"		},
   [ActSeeAtt		] = { 861, 4,TabUsr,ActSeeAtt		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_SeeAttEvents		,"calendar-check"	},
   [ActReqSignUp	] = {1054, 5,TabUsr,ActReqSignUp	,    0,0x006,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqSignUpInCrs		,"hand-point-up"	},
   [ActSeeSignUpReq	] = {1057, 6,TabUsr,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ShowEnrolmentRequests	,"hand-point-up"	},
   [ActLstCon		] = { 995, 7,TabUsr,ActLstCon		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Con_ShowConnectedUsrs		,"user-clock"		},

   // Actions not in menu:
   [ActChgGrp		] = { 118,-1,TabUnk,ActReqSelGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMyGrpsAndShowChanges	,NULL},

   [ActReqEdiGrp	] = { 108,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqEditGroups		,NULL},

   [ActNewGrpTyp	] = { 174,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RecFormNewGrpTyp		,NULL},
   [ActReqRemGrpTyp	] = { 236,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqRemGroupType		,NULL},
   [ActRemGrpTyp	] = { 237,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RemoveGroupType		,NULL},
   [ActRenGrpTyp	] = { 304,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RenameGroupType		,NULL},
   [ActChgMdtGrpTyp	] = { 303,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMandatGrpTyp		,NULL},
   [ActChgMulGrpTyp	] = { 302,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMultiGrpTyp		,NULL},
   [ActChgTimGrpTyp	] = {1061,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeOpenTimeGrpTyp	,NULL},

   [ActNewGrp		] = { 122,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RecFormNewGrp		,NULL},
   [ActReqRemGrp	] = { 107,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqRemGroup		,NULL},
   [ActRemGrp		] = { 175,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RemoveGroup		,NULL},
   [ActOpeGrp		] = { 322,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_OpenGroup			,NULL},
   [ActCloGrp		] = { 321,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_CloseGroup			,NULL},
   [ActEnaFilZonGrp	] = { 495,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_EnableFileZonesGrp		,NULL},
   [ActDisFilZonGrp	] = { 496,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_DisableFileZonesGrp	,NULL},
   [ActChgGrpTyp	] = { 167,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeGroupType		,NULL},
   [ActRenGrp		] = { 121,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RenameGroup		,NULL},
   [ActChgGrpCla	] = {1752,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeGroupClassroom	,NULL},
   [ActChgMaxStdGrp	] = { 106,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMaxStdsGrp		,NULL},

   [ActLstGst		] = { 587,-1,TabUnk,ActLstOth		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_SeeGuests			,NULL},

   [ActPrnGstPho	] = {1190,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_SeeGstClassPhotoPrn	,NULL},
   [ActPrnStdPho	] = { 120,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_SeeStdClassPhotoPrn	,NULL},
   [ActPrnTchPho	] = { 443,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_SeeTchClassPhotoPrn	,NULL},
   [ActLstGstAll	] = {1189,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_ListAllDataGsts		,NULL},
   [ActLstStdAll	] = {  42,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_ListAllDataStds		,NULL},
   [ActLstTchAll	] = { 578,-1,TabUnk,ActLstTch		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_ListAllDataTchs 		,NULL},

   [ActSeeRecOneStd	] = {1174,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_GetUsrAndShowRecOneStdCrs	,NULL},
   [ActSeeRecOneTch	] = {1175,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_GetUsrAndShowRecOneTchCrs	,NULL},

   [ActDoActOnSevGst	] = {1753,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Usr_DoActionOnSeveralUsrs1	,Usr_DoActionOnSeveralUsrs2	,NULL},
   [ActDoActOnSevStd	] = {1754,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Usr_DoActionOnSeveralUsrs1	,Usr_DoActionOnSeveralUsrs2	,NULL},
   [ActDoActOnSevTch	] = {1755,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Usr_DoActionOnSeveralUsrs1	,Usr_DoActionOnSeveralUsrs2	,NULL},

   [ActSeeRecSevGst	] = {1187,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ListRecordsGstsShow	,NULL},
   [ActSeeRecSevStd	] = {  89,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ListRecordsStdsShow	,NULL},
   [ActSeeRecSevTch	] = {  22,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ListRecordsTchsShow	,NULL},
   [ActPrnRecSevGst	] = {1188,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Rec_ListRecordsGstsPrint	,NULL},
   [ActPrnRecSevStd	] = { 111,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Rec_ListRecordsStdsPrint	,NULL},
   [ActPrnRecSevTch	] = { 127,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Rec_ListRecordsTchsPrint	,NULL},

   [ActRcvRecOthUsr	] = { 300,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateAndShowOtherCrsRecord,NULL},
   [ActEdiRecFie	] = { 292,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ReqEditRecordFields	,NULL},
   [ActNewFie		] = { 293,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ReceiveFormField		,NULL},
   [ActReqRemFie	] = { 294,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ReqRemField		,NULL},
   [ActRemFie		] = { 295,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_RemoveField		,NULL},
   [ActRenFie		] = { 296,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_RenameField		,NULL},
   [ActChgRowFie	] = { 305,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ChangeLinesField		,NULL},
   [ActChgVisFie	] = { 297,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ChangeVisibilityField	,NULL},
   [ActRcvRecCrs	] = { 301,-1,TabUnk,ActLstStd		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateAndShowMyCrsRecord	,NULL},

   [ActFrmLogInUsrAgd	] = {1614,-1,TabUnk,ActLstTch		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_PutFormLogInToShowUsrAgenda,NULL},
   [ActLogInUsrAgd	] = {1615,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ShowOtherAgendaAfterLogIn	,NULL},
   [ActLogInUsrAgdLan	] = {1616,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ShowOtherAgendaAfterLogIn	,NULL},
   [ActSeeUsrAgd	] = {1611,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ShowUsrAgenda		,NULL},

   [ActReqEnrSevStd	] = {1426,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAdminStds		,NULL},
   [ActReqEnrSevNET	] = {1642,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAdminNonEditingTchs	,NULL},
   [ActReqEnrSevTch	] = {1427,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAdminTchs		,NULL},

   [ActReqLstUsrAtt	] = {1073,-1,TabUnk,ActSeeAtt		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ReqListUsrsAttendanceCrs	,NULL},
   [ActSeeLstMyAtt	] = {1473,-1,TabUnk,ActSeeAtt		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ListMyAttendanceCrs	,NULL},
   [ActPrnLstMyAtt	] = {1474,-1,TabUnk,ActSeeAtt		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Att_PrintMyAttendanceCrs	,NULL},
   [ActSeeLstUsrAtt	] = {1074,-1,TabUnk,ActSeeAtt		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ListUsrsAttendanceCrs	,NULL},
   [ActPrnLstUsrAtt	] = {1075,-1,TabUnk,ActSeeAtt		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Att_PrintUsrsAttendanceCrs	,NULL},
   [ActFrmNewAtt	] = {1063,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RequestCreatOrEditAttEvent	,NULL},
   [ActEdiOneAtt	] = {1064,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RequestCreatOrEditAttEvent	,NULL},
   [ActNewAtt		] = {1065,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RecFormAttEvent		,NULL},
   [ActChgAtt		] = {1066,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RecFormAttEvent		,NULL},
   [ActReqRemAtt	] = {1067,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_AskRemAttEvent		,NULL},
   [ActRemAtt		] = {1068,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_GetAndRemAttEvent		,NULL},
   [ActHidAtt		] = {1069,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_HideAttEvent		,NULL},
   [ActShoAtt		] = {1070,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ShowAttEvent		,NULL},
   [ActSeeOneAtt	] = {1071,-1,TabUnk,ActSeeAtt		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_SeeOneAttEvent		,NULL},
   [ActRecAttStd	] = {1072,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RegisterStudentsInAttEvent	,NULL},
   [ActRecAttMe		] = {1076,-1,TabUnk,ActSeeAtt		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RegisterMeAsStdInAttEvent	,NULL},

   [ActSignUp		] = {1056,-1,TabUnk,ActReqSignUp	,    0,0x006,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_SignUpInCrs		,NULL},
   [ActUpdSignUpReq	] = {1522,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_UpdateEnrolmentRequests	,NULL},
   [ActReqRejSignUp	] = {1058,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRejectSignUp		,NULL},
   [ActRejSignUp	] = {1059,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RejectSignUp		,NULL},

   [ActReqMdfOneOth	] = {1414,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRegRemOth		,NULL},
   [ActReqMdfOneStd	] = {1415,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRegRemStd		,NULL},
   [ActReqMdfOneTch	] = {1416,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRegRemTch		,NULL},

   [ActReqMdfOth	] = {1418,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherOth	,NULL},
   [ActReqMdfStd	] = {1419,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherStd	,NULL},
   [ActReqMdfNET	] = {1644,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherTch	,NULL},
   [ActReqMdfTch	] = {1420,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherTch	,NULL},

   [ActReqOthPho	] = {1432,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_SendPhotoUsr		,NULL},
   [ActReqStdPho	] = {1433,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_SendPhotoUsr		,NULL},
   [ActReqTchPho	] = {1434,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_SendPhotoUsr		,NULL},

   [ActDetOthPho	] = {1435,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   [ActDetStdPho	] = {1436,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   [ActDetTchPho	] = {1437,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},

   [ActUpdOthPho	] = {1438,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},
   [ActUpdStdPho	] = {1439,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},
   [ActUpdTchPho	] = {1440,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},

   [ActReqRemOthPho	] = {1574,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemoveUsrPhoto		,NULL},
   [ActReqRemStdPho	] = {1575,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemoveUsrPhoto		,NULL},
   [ActReqRemTchPho	] = {1576,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemoveUsrPhoto		,NULL},
   [ActRemOthPho	] = {1441,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_RemoveUsrPhoto		,NULL},
   [ActRemStdPho	] = {1442,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_RemoveUsrPhoto		,NULL},
   [ActRemTchPho	] = {1443,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_RemoveUsrPhoto		,NULL},

   [ActCreOth		] = {1444,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   [ActCreStd		] = {1445,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   [ActCreNET		] = {1645,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   [ActCreTch		] = {1446,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},

   [ActUpdOth		] = {1422,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   [ActUpdStd		] = {1423,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   [ActUpdNET		] = {1646,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   [ActUpdTch		] = {1424,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},

   [ActReqAccEnrStd	] = {1456,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   [ActReqAccEnrNET	] = {1639,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   [ActReqAccEnrTch	] = {1457,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   [ActAccEnrStd	] = {1458,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   [ActAccEnrNET	] = {1640,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   [ActAccEnrTch	] = {1459,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   [ActRemMe_Std	] = {1460,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRemMeFromCrs		,NULL},
   [ActRemMe_NET	] = {1641,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRemMeFromCrs		,NULL},
   [ActRemMe_Tch	] = {1461,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRemMeFromCrs		,NULL},

   [ActNewAdmIns	] = {1337,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AddAdmToIns		,NULL},
   [ActRemAdmIns	] = {1338,-1,TabUnk,ActLstOth		,0x300,0x300,0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RemAdmIns			,NULL},

   [ActNewAdmCtr	] = {1339,-1,TabUnk,ActLstOth		,0x300,0x300,0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AddAdmToCtr		,NULL},
   [ActRemAdmCtr	] = {1340,-1,TabUnk,ActLstOth		,0x380,0x380,0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RemAdmCtr			,NULL},

   [ActNewAdmDeg	] = { 586,-1,TabUnk,ActLstOth		,0x380,0x380,0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AddAdmToDeg		,NULL},
   [ActRemAdmDeg	] = { 584,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RemAdmDeg			,NULL},

   [ActRcvFrmEnrSevStd	] = {1428,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReceiveFormAdminStds	,NULL},
   [ActRcvFrmEnrSevNET	] = {1643,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReceiveFormAdminNonEditTchs,NULL},
   [ActRcvFrmEnrSevTch	] = {1429,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReceiveFormAdminTchs	,NULL},

   [ActCnfID_Oth	] = {1568,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ConfirmOtherUsrID		,NULL},
   [ActCnfID_Std	] = {1569,-1,TabUnk,ActLstStd		,0x3F0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ConfirmOtherUsrID		,NULL},
   [ActCnfID_Tch	] = {1570,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ConfirmOtherUsrID		,NULL},

   [ActFrmAccOth	] = {1735,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActFrmAccStd	] = {1736,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActFrmAccTch	] = {1737,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormChgOtherUsrAccount	,NULL},

   [ActRemOldNicOth	] = {1738,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveOtherUsrNick		,NULL},
   [ActRemOldNicStd	] = {1739,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveOtherUsrNick		,NULL},
   [ActRemOldNicTch	] = {1740,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveOtherUsrNick		,NULL},
   [ActChgNicOth	] = {1741,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_UpdateOtherUsrNick		,NULL},
   [ActChgNicStd	] = {1742,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_UpdateOtherUsrNick		,NULL},
   [ActChgNicTch	] = {1743,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_UpdateOtherUsrNick		,NULL},

   [ActRemID_Oth	] = {1450,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveOtherUsrID		,NULL},
   [ActRemID_Std	] = {1451,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveOtherUsrID		,NULL},
   [ActRemID_Tch	] = {1452,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveOtherUsrID		,NULL},
   [ActNewID_Oth	] = {1453,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_NewOtherUsrID		,NULL},
   [ActNewID_Std	] = {1454,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_NewOtherUsrID		,NULL},
   [ActNewID_Tch	] = {1455,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_NewOtherUsrID		,NULL},

   [ActChgPwdOth	] = {1467,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateOtherUsrPwd		,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActChgPwdStd	] = {1468,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateOtherUsrPwd		,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActChgPwdTch	] = {1469,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateOtherUsrPwd		,Acc_ShowFormChgOtherUsrAccount	,NULL},

   [ActRemMaiOth	] = {1478,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   [ActRemMaiStd	] = {1479,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   [ActRemMaiTch	] = {1480,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   [ActNewMaiOth	] = {1481,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_NewOtherUsrEmail		,NULL},
   [ActNewMaiStd	] = {1482,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_NewOtherUsrEmail		,NULL},
   [ActNewMaiTch	] = {1483,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_NewOtherUsrEmail		,NULL},

   [ActRemStdCrs	] = {1462,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,NULL},
   [ActRemNETCrs	] = {1647,-1,TabUnk,ActLstTch		,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,NULL},
   [ActRemTchCrs	] = {1463,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,NULL},

   [ActRemUsrGbl	] = {  62,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_GetUsrCodAndRemUsrGbl	,NULL},

   [ActReqRemAllStdCrs	] = {  88,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskRemAllStdsThisCrs	,NULL},
   [ActRemAllStdCrs	] = {  87,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RemAllStdsThisCrs		,NULL},

   [ActReqRemOldUsr	] = { 590,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskRemoveOldUsrs		,NULL},
   [ActRemOldUsr	] = { 773,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RemoveOldUsrs		,NULL},

   [ActLstDupUsr	] = {1578,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dup_ListDuplicateUsrs		,NULL},
   [ActLstSimUsr	] = {1579,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dup_GetUsrCodAndListSimilarUsrs,NULL},
   [ActRemDupUsr	] = {1580,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dup_RemoveUsrFromListDupUsrs	,NULL},

   [ActReqFolSevStd	] = {1756,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_RequestFollowStds		,NULL},
   [ActReqFolSevTch	] = {1757,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_RequestFollowTchs		,NULL},
   [ActReqUnfSevStd	] = {1758,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_RequestUnfollowStds	,NULL},
   [ActReqUnfSevTch	] = {1759,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_RequestUnfollowTchs	,NULL},
   [ActFolSevStd	] = {1760,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_FollowUsrs			,NULL},
   [ActFolSevTch	] = {1761,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_FollowUsrs			,NULL},
   [ActUnfSevStd	] = {1762,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_UnfollowUsrs		,NULL},
   [ActUnfSevTch	] = {1763,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_UnfollowUsrs		,NULL},

   // TabMsg ******************************************************************
   // Actions in menu:
   [ActSeeAnn		] = {1235, 0,TabMsg,ActSeeAnn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_ShowAllAnnouncements	,"bullhorn"		},
   [ActSeeAllNot	] = { 762, 1,TabMsg,ActSeeAllNot	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Not_ListFullNotices		,"sticky-note"		},
   [ActSeeFor		] = {  95, 2,TabMsg,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,"comments"		},
// [ActSeeChtRms	] = {  51, 3,TabMsg,ActSeeChtRms	,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cht_ShowChatRooms		,"comments"		},
   [ActSeeChtRms	] = {  51, 3,TabMsg,ActSeeChtRms	,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cht_ShowChatRooms		,"comments"		},
   [ActReqMsgUsr	] = {  26, 4,TabMsg,ActReqMsgUsr	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_FormMsgUsrs		,"marker"		},
   [ActSeeRcvMsg	] = {   3, 5,TabMsg,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ShowRecMsgs		,"inbox"		},
   [ActSeeSntMsg	] = {  70, 6,TabMsg,ActSeeSntMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ShowSntMsgs		,"share"		},
   [ActReqMaiUsr	] = {1772, 7,TabMsg,ActReqMaiUsr	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_ReqUsrsToListEmails	,"envelope"		},

   // Actions not in menu:
   [ActWriAnn		] = {1237,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_ShowFormAnnouncement	,NULL},
   [ActRcvAnn		] = {1238,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_ReceiveAnnouncement	,NULL},
   [ActHidAnn		] = {1470,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ann_HideActiveAnnouncement	,Ann_ShowAllAnnouncements	,NULL},
   [ActRevAnn		] = {1471,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ann_RevealHiddenAnnouncement	,Ann_ShowAllAnnouncements	,NULL},
   [ActRemAnn		] = {1236,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_RemoveAnnouncement		,NULL},

   [ActSeeOneNot	] = {1164,-1,TabUnk,ActSeeAllNot	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_GetHighLightedNotCod	,Not_ListFullNotices		,NULL},
   [ActWriNot		] = {  59,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Not_ShowFormNotice		,NULL},
   [ActRcvNot		] = {  60,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_ReceiveNotice		,Not_ListFullNotices		,NULL},
   [ActHidNot		] = { 763,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_HideActiveNotice		,Not_ListFullNotices		,NULL},
   [ActRevNot		] = { 764,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_RevealHiddenNotice		,Not_ListFullNotices		,NULL},
   [ActReqRemNot	] = {1472,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Not_RequestRemNotice		,NULL},
   [ActRemNot		] = {  73,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_RemoveNotice		,Not_ListNoticesAfterRemoval	,NULL},


   [ActSeeForCrsUsr	] = { 345,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForCrsTch	] = { 431,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForDegUsr	] = { 241,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForDegTch	] = { 243,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForCtrUsr	] = { 901,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForCtrTch	] = { 430,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForInsUsr	] = { 725,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForInsTch	] = { 724,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForGenUsr	] = { 726,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForGenTch	] = { 723,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForSWAUsr	] = { 242,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForSWATch	] = { 245,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},

   [ActSeePstForCrsUsr	] = { 346,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForCrsTch	] = { 347,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForDegUsr	] = { 255,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForDegTch	] = { 291,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForCtrUsr	] = { 348,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForCtrTch	] = { 902,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForInsUsr	] = { 730,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForInsTch	] = { 746,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForGenUsr	] = { 727,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForGenTch	] = { 731,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForSWAUsr	] = { 244,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForSWATch	] = { 246,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},

   [ActRcvThrForCrsUsr	] = { 350,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForCrsTch	] = { 754,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForDegUsr	] = { 252,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForDegTch	] = { 247,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForCtrUsr	] = { 903,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForCtrTch	] = { 904,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForInsUsr	] = { 737,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForInsTch	] = { 769,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForGenUsr	] = { 736,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForGenTch	] = { 765,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForSWAUsr	] = { 258,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForSWATch	] = { 259,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},

   [ActRcvRepForCrsUsr	] = { 599,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForCrsTch	] = { 755,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForDegUsr	] = { 606,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForDegTch	] = { 617,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForCtrUsr	] = { 905,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForCtrTch	] = { 906,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForInsUsr	] = { 740,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForInsTch	] = { 770,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForGenUsr	] = { 747,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForGenTch	] = { 816,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForSWAUsr	] = { 603,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForSWATch	] = { 622,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},

   [ActReqDelThrCrsUsr	] = { 867,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrCrsTch	] = { 869,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrDegUsr	] = { 907,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrDegTch	] = { 908,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrCtrUsr	] = { 909,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrCtrTch	] = { 910,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrInsUsr	] = { 911,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrInsTch	] = { 912,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrGenUsr	] = { 913,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrGenTch	] = { 914,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrSWAUsr	] = { 881,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},
   [ActReqDelThrSWATch	] = { 915,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RequestRemoveThread	,NULL},

   [ActDelThrForCrsUsr	] = { 868,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForCrsTch	] = { 876,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForDegUsr	] = { 916,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForDegTch	] = { 917,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForCtrUsr	] = { 918,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForCtrTch	] = { 919,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForInsUsr	] = { 920,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForInsTch	] = { 921,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForGenUsr	] = { 922,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForGenTch	] = { 923,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForSWAUsr	] = { 882,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForSWATch	] = { 924,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},

   [ActCutThrForCrsUsr	] = { 926,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForCrsTch	] = { 927,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForDegUsr	] = { 928,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForDegTch	] = { 929,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForCtrUsr	] = { 930,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForCtrTch	] = { 931,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForInsUsr	] = { 932,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForInsTch	] = { 933,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForGenUsr	] = { 934,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForGenTch	] = { 935,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForSWAUsr	] = { 890,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForSWATch	] = { 936,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},

   [ActPasThrForCrsUsr	] = { 891,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForCrsTch	] = { 937,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForDegUsr	] = { 938,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForDegTch	] = { 939,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForCtrUsr	] = { 940,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForCtrTch	] = { 941,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForInsUsr	] = { 942,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForInsTch	] = { 943,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForGenUsr	] = { 944,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForGenTch	] = { 945,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForSWAUsr	] = { 946,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForSWATch	] = { 947,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},

   [ActDelPstForCrsUsr	] = { 602,-1,TabUnk,ActSeeFor		,0x238,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForCrsTch	] = { 756,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForDegUsr	] = { 608,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForDegTch	] = { 680,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForCtrUsr	] = { 948,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForCtrTch	] = { 949,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForInsUsr	] = { 743,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForInsTch	] = { 772,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForGenUsr	] = { 735,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForGenTch	] = { 950,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForSWAUsr	] = { 613,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForSWATch	] = { 623,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},

   [ActEnbPstForCrsUsr	] = { 624,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForCrsTch	] = { 951,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForDegUsr	] = { 616,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForDegTch	] = { 619,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForCtrUsr	] = { 952,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForCtrTch	] = { 953,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForInsUsr	] = { 954,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForInsTch	] = { 955,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForGenUsr	] = { 956,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForGenTch	] = { 957,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForSWAUsr	] = { 632,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForSWATch	] = { 634,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},

   [ActDisPstForCrsUsr	] = { 610,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForCrsTch	] = { 958,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForDegUsr	] = { 615,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForDegTch	] = { 618,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForCtrUsr	] = { 959,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForCtrTch	] = { 960,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForInsUsr	] = { 961,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForInsTch	] = { 962,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForGenUsr	] = { 963,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForGenTch	] = { 925,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForSWAUsr	] = { 625,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForSWATch	] = { 635,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},

// [ActCht		] = {  52,-1,TabUnk,ActSeeChtRms	,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_NEW_TAB,Cht_OpenChatWindow		,NULL				,NULL},
   [ActCht		] = {  52,-1,TabUnk,ActSeeChtRms	,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,Cht_OpenChatWindow		,NULL				,NULL},

   [ActRcvMsgUsr	] = {  27,-1,TabUnk,ActReqMsgUsr	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Msg_RecMsgFromUsr		,NULL},
   [ActReqDelAllSntMsg	] = { 604,-1,TabUnk,ActSeeSntMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ReqDelAllSntMsgs		,NULL},
   [ActReqDelAllRcvMsg	] = { 593,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ReqDelAllRecMsgs		,NULL},
   [ActDelAllSntMsg	] = { 434,-1,TabUnk,ActSeeSntMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelAllSntMsgs		,NULL},
   [ActDelAllRcvMsg	] = { 436,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelAllRecMsgs		,NULL},
   [ActDelSntMsg	] = {  90,-1,TabUnk,ActSeeSntMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelSntMsg			,NULL},
   [ActDelRcvMsg	] = {  64,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelRecMsg			,NULL},
   [ActExpSntMsg	] = { 664,-1,TabUnk,ActSeeSntMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ExpSntMsg			,NULL},
   [ActExpRcvMsg	] = { 663,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ExpRecMsg			,NULL},
   [ActConSntMsg	] = {1020,-1,TabUnk,ActSeeSntMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ConSntMsg			,NULL},
   [ActConRcvMsg	] = {1019,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ConRecMsg			,NULL},

   [ActLstBanUsr	] = {1017,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ListBannedUsrs		,NULL},
   [ActBanUsrMsg	] = {1015,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_BanSenderWhenShowingMsgs	,NULL},
   [ActUnbUsrMsg	] = {1016,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_UnbanSenderWhenShowingMsgs	,NULL},
   [ActUnbUsrLst	] = {1018,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_UnbanSenderWhenListingUsrs	,NULL},

   [ActMaiUsr		] = { 100,-1,TabUnk,ActReqMaiUsr	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_GetSelectedUsrsAndListEmails,NULL},

   // TabAna ******************************************************************
   // Actions in menu:
   [ActReqUseGbl	] = { 761, 0,TabAna,ActReqUseGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fig_ReqShowFigures		,"chart-pie"		},
   [ActSeePhoDeg	] = { 447, 1,TabAna,ActSeePhoDeg	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ShowPhotoDegree		,"graduation-cap"	},
   [ActReqStaCrs	] = { 767, 2,TabAna,ActReqStaCrs	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ind_ReqIndicatorsCourses	,"tasks"		},
   [ActReqAccGbl	] = { 591, 3,TabAna,ActReqAccGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Sta_SetIniEndDates		,Sta_AskShowGblHits		,"chart-line"		},
   [ActReqMyUsgRep	] = {1586, 4,TabAna,ActReqMyUsgRep	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rep_ReqMyUsageReport		,"file-alt"		},
   [ActMFUAct		] = { 993, 5,TabAna,ActMFUAct		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MFU_ShowMyMFUActions		,"heart"		},

   // Actions not in menu:
   [ActSeeUseGbl	] = {  84,-1,TabUnk,ActReqUseGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fig_ShowFigures		,NULL},
   [ActPrnPhoDeg	] = { 448,-1,TabUnk,ActSeePhoDeg	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Pho_PrintPhotoDegree  		,NULL},
   [ActCalPhoDeg	] = { 444,-1,TabUnk,ActSeePhoDeg	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_CalcPhotoDegree		,NULL},
   [ActSeeAccGbl	] = {  79,-1,TabUnk,ActReqAccGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Sta_SeeGblAccesses		,NULL},
   [ActReqAccCrs	] = { 594,-1,TabUnk,ActReqAccGbl	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Sta_SetIniEndDates		,Sta_AskShowCrsHits		,NULL},
   [ActSeeAccCrs	] = { 119,-1,TabUnk,ActReqAccGbl	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Sta_SeeCrsAccesses		,NULL},
   [ActSeeAllStaCrs	] = { 768,-1,TabUnk,ActReqAccGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Ind_ShowIndicatorsCourses	,NULL},

   [ActLstClk		] = { 989,-1,TabUnk,ActReqAccGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Log_ShowLastClicks		,NULL},
   [ActRefLstClk	] = { 994,-1,TabUnk,ActRefLstClk	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RFRESH,NULL				,Lay_RefreshLastClicks		,NULL},

   [ActSeeMyUsgRep	] = {1582,-1,TabUnk,ActReqMyUsgRep	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rep_ShowMyUsageReport		,NULL},

   // TabPrf ******************************************************************
   // Actions in menu:
   [ActFrmRolSes	] = { 843, 0,TabPrf,ActFrmRolSes	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WriteFormLogout		,"power-off"		},
   [ActMyCrs		] = { 987, 1,TabPrf,ActMyCrs		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_ReqSelectOneOfMyCourses	,"sitemap"		},
   [ActSeeMyTT		] = { 408, 2,TabPrf,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TT_ShowClassTimeTable		,"clock"		},
   [ActSeeMyAgd		] = {1602, 3,TabPrf,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ShowMyAgenda		,"calendar"		},
   [ActFrmMyAcc		] = {  36, 4,TabPrf,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormMyAccount		,"at"			},
   [ActReqEdiRecSha	] = { 285, 5,TabPrf,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ShowMySharedRecordAndMore	,"card"			},
   [ActReqEdiSet	] = { 673, 6,TabPrf,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Set_EditSettings			,"cog"			},

   // Actions not in menu:
   [ActChgMyRol		] = { 589,-1,TabUnk,ActFrmRolSes	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Rol_ChangeMyRole		,Usr_ShowFormsLogoutAndRole	,NULL},

   [ActFrmNewEvtMyAgd	] = {1603,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_RequestCreatOrEditEvent	,NULL},
   [ActEdiOneEvtMyAgd	] = {1604,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_RequestCreatOrEditEvent	,NULL},
   [ActNewEvtMyAgd	] = {1605,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_RecFormEvent		,NULL},
   [ActChgEvtMyAgd	] = {1606,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_RecFormEvent		,NULL},
   [ActReqRemEvtMyAgd	] = {1607,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_AskRemEvent		,NULL},
   [ActRemEvtMyAgd	] = {1608,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_RemoveEvent		,NULL},
   [ActHidEvtMyAgd	] = {1612,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_HideEvent			,NULL},
   [ActShoEvtMyAgd	] = {1613,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_UnhideEvent		,NULL},
   [ActPrvEvtMyAgd	] = {1609,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_MakeEventPrivate		,NULL},
   [ActPubEvtMyAgd	] = {1610,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_MakeEventPublic		,NULL},
   [ActPrnAgdQR		] = {1618,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Agd_PrintAgdQRCode		,NULL},

   [ActChkUsrAcc	] = {1584,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_CheckIfEmptyAccountExists	,NULL},
   [ActCreMyAcc		] = {1787,-1,TabUnk,ActFrmMyAcc		,0x001,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormCreateMyAccount	,NULL},
   [ActCreUsrAcc	] = {1163,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_AfterCreationNewAccount	,NULL},

   [ActRemMyID		] = {1147,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveMyUsrID		,NULL},
   [ActChgMyID		] = {1148,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_NewMyUsrID			,NULL},

   [ActRemMyNck		] = {1089,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveMyNick		,NULL},
   [ActChgMyNck		] = {  37,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_UpdateMyNick		,NULL},

   [ActRemMyMai		] = {1090,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveMyUsrEmail		,NULL},
   [ActChgMyMai		] = {1088,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,May_NewMyUsrEmail		,NULL},
   [ActCnfMai		] = {1091,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_ConfirmEmail		,NULL},

   [ActChgMyPwd		] = {  35,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateMyPwd		,Acc_ShowFormChgMyAccount	,NULL},

   [ActReqRemMyAcc	] = {1430,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_AskIfRemoveMyAccount	,NULL},
   [ActRemMyAcc		] = {1431,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_RemoveMyAccount		,NULL},

   [ActChgMyData	] = { 298,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Rec_UpdateMyRecord		,Rec_ShowMySharedRecordUpd	,NULL},

   [ActReqMyPho		] = {  30,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqMyPhoto			,NULL},
   [ActDetMyPho		] = { 693,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecMyPhotoDetFaces		,NULL},
   [ActUpdMyPho		] = { 694,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_UpdateMyPhoto1		,Pho_UpdateMyPhoto2		,NULL},
   [ActReqRemMyPho	] = {1577,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemoveMyPhoto		,NULL},
   [ActRemMyPho		] = { 428,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_RemoveMyPhoto1		,Pho_RemoveMyPhoto2		,NULL},

   [ActChgCtyMyIns	] = {1166,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ChgCountryOfMyInstitution	,NULL},
   [ActChgMyIns		] = {1167,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyInstitution	,NULL},
   [ActChgMyCtr		] = {1168,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyCentre		,NULL},
   [ActChgMyDpt		] = {1169,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyDepartment		,NULL},
   [ActChgMyOff		] = {1170,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyOffice		,NULL},
   [ActChgMyOffPho	] = {1171,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyOfficePhone	,NULL},

   [ActReqEdiMyNet	] = {1172,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ShowMySharedRecordAndMore	,NULL},
   [ActChgMyNet		] = {1173,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Net_UpdateMyWebsAndSocialNets	,NULL},

   [ActChgThe		] = { 841,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,The_ChangeTheme		,Set_EditSettings			,NULL},
   [ActReqChgLan	] = { 992,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Lan_AskChangeLanguage		,NULL},
   [ActChgLan		] = { 654,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Lan_ChangeLanguage		,Set_EditSettings			,NULL},
   [ActChg1stDay	] = {1484,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Set_EditSettings			,NULL},
   [ActChgDatFmt	] = {1638,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_ChangeDateFormat		,Set_EditSettings			,NULL},
   [ActChgCol		] = { 674,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Set_ChangeSideCols		,Set_EditSettings			,NULL},
   [ActHidLftCol	] = { 668,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Set_HideLeftCol		,Set_EditSettings			,NULL},
   [ActHidRgtCol	] = { 669,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Set_HideRightCol		,Set_EditSettings			,NULL},
   [ActShoLftCol	] = { 670,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Set_ShowLeftCol		,Set_EditSettings			,NULL},
   [ActShoRgtCol	] = { 671,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Set_ShowRightCol		,Set_EditSettings			,NULL},
   [ActChgIco		] = {1092,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Ico_ChangeIconSet		,Set_EditSettings			,NULL},
   [ActChgMnu		] = {1243,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Mnu_ChangeMenu			,Set_EditSettings			,NULL},
   [ActChgPriPho	] = { 774,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ChangePhotoVisibility	,NULL},
   [ActChgBasPriPrf	] = {1404,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_ChangeBasicProfileVis	,NULL},
   [ActChgExtPriPrf	] = {1765,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_ChangeExtendedProfileVis	,NULL},
   [ActChgCooPrf	] = {1764,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Coo_ChangeMyPrefsCookies	,NULL},
   [ActChgNtfPrf	] = { 775,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Ntf_ChangeNotifyEvents		,Set_EditSettings			,NULL},

   [ActPrnUsrQR		] = {1022,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_PrintUsrQRCode		,NULL},

   [ActPrnMyTT		] = { 409,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,TT_ShowClassTimeTable		,NULL},
   [ActEdiTut		] = {  65,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TT_EditMyTutTimeTable		,NULL},
   [ActChgTut		] = {  48,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TT_EditMyTutTimeTable		,NULL},
   [ActChgMyTT1stDay	] = {1487,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C7,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,TT_ShowClassTimeTable		,NULL},
  };

Act_Action_t Act_FromActCodToAction[1 + Act_MAX_ACTION_COD] =	// Do not reuse unique action codes!
	{
	ActSeeAdmDocCrsGrp,	//  #0
	-1,			//  #1 (obsolete action)
	ActMnu,			//  #2
	ActSeeRcvMsg,		//  #3
	-1,			//  #4 (obsolete action)
	-1,			//  #5 (obsolete action)
	ActLogIn,		//  #6
	-1,			//  #7 (obsolete action)
	-1,			//  #8 (obsolete action)
	ActSeeCrsLnk,		//  #9
	ActLogOut,		// #10
	-1,			// #11 (obsolete action)
	ActAdmDocCrs,		// #12
	-1,			// #13 (obsolete action)
	-1,			// #14 (obsolete action)
	ActSeeAss,		// #15
	-1,			// #16 (obsolete action)
	ActSeeAdmMrk,		// #17
	-1,			// #18 (obsolete action)
	-1,			// #19 (obsolete action)
	ActSeeSylPra,		// #20
	-1,			// #21 (obsolete action)
	ActDoActOnSevTch,	// #22
	ActAdmBrf,		// #23
	-1,			// #24 (obsolete action)
	ActSeeCrsTT,		// #25
	ActReqMsgUsr,		// #26
	ActRcvMsgUsr,		// #27
	ActSeeSylLec,		// #28
	ActSeeTst,		// #29
	ActReqMyPho,		// #30
	-1,			// #31 (obsolete action)
	ActSeeBib,		// #32
	-1,			// #33 (obsolete action)
	-1,			// #34 (obsolete action)
	ActChgMyPwd,		// #35
	ActFrmMyAcc,		// #36
	ActChgMyNck,		// #37
	-1,			// #38 (obsolete action)
	-1,			// #39 (obsolete action)
	-1,			// #40 (obsolete action)
	-1,			// #41 (obsolete action)
	ActLstStdAll,		// #42
	-1,			// #43 (obsolete action)
	ActEdiSylLec,		// #44
	ActEdiCrsTT,	// #45
	-1,			// #46 (obsolete action)
	-1,			// #47 (obsolete action)
	ActChgTut,		// #48
	-1,			// #49 (obsolete action)
	-1,			// #50 (obsolete action)
	ActSeeChtRms,		// #51
	ActCht,			// #52
	ActChgCrsTT,	// #53
	ActSeeFAQ,		// #54
	-1,			// #55 (obsolete action)
	-1,			// #56 (obsolete action)
	-1,			// #57 (obsolete action)
	-1,			// #58 (obsolete action)
	ActWriNot,		// #59
	ActRcvNot,		// #60
	-1,			// #61 (obsolete action)
	ActRemUsrGbl,		// #62
	-1,			// #63 (obsolete action)
	ActDelRcvMsg,		// #64
	ActEdiTut,		// #65
	-1,			// #66 (obsolete action)
	-1,			// #67 (obsolete action)
	-1,			// #68 (obsolete action)
	ActEdiAss,		// #69
	ActSeeSntMsg,		// #70
	-1,			// #71 (obsolete action)
	-1,			// #72 (obsolete action)
	ActRemNot,		// #73
	ActEdiSylPra,		// #74
	-1,			// #75 (obsolete action)
	ActEdiBib,		// #76
	-1,			// #77 (obsolete action)
	-1,			// #78 (obsolete action)
	ActSeeAccGbl,		// #79
	-1,			// #80 (obsolete action)
	-1,			// #81 (obsolete action)
	-1,			// #82 (obsolete action)
	-1,			// #83 (obsolete action)
	ActSeeUseGbl,		// #84
	ActSeeAllExaAnn,		// #85
	-1,			// #86 (obsolete action)
	ActRemAllStdCrs,	// #87
	ActReqRemAllStdCrs,	// #88
	ActSeeRecSevStd,	// #89
	ActDelSntMsg,		// #90
	ActEdiExaAnn,		// #91
	-1,			// #92 (obsolete action)
	-1,			// #93 (obsolete action)
	-1,			// #94 (obsolete action)
	ActSeeFor,		// #95
	ActEdiCrsLnk,		// #96
	-1,			// #97 (obsolete action)
	ActAssTst,		// #98
	-1,			// #99 (obsolete action)
	ActMaiUsr,		// #100
	-1,			// #101 (obsolete action)
	-1,			// #102 (obsolete action)
	ActReqTst,		// #103
	ActEdiTstQst,		// #104
	ActEdiOneTstQst,	// #105
	ActChgMaxStdGrp,	// #106
	ActReqRemGrp,		// #107
	ActReqEdiGrp,		// #108
	ActEdiFAQ,		// #109
	ActRcvExaAnn,		// #110
	ActPrnRecSevStd,	// #111
	-1,			// #112 (obsolete action)
	-1,			// #113 (obsolete action)
	-1,			// #114 (obsolete action)
	-1,			// #115 (obsolete action)
	ActReqSelGrp,		// #116
	-1,			// #117 (obsolete action)
	ActChgGrp,		// #118
	ActSeeAccCrs,		// #119
	ActPrnStdPho,		// #120
	ActRenGrp,		// #121
	ActNewGrp,		// #122
	-1,			// #123 (obsolete action)
	-1,			// #124 (obsolete action)
	-1,			// #125 (obsolete action)
	ActRcvTstQst,		// #126
	ActPrnRecSevTch,	// #127
	-1,			// #128 (obsolete action)
	-1,			// #129 (obsolete action)
	-1,			// #130 (obsolete action)
	-1,			// #131 (obsolete action)
	ActLstTstQst,		// #132
	ActRemTstQst,		// #133
	-1,			// #134 (obsolete action)
	-1,			// #135 (obsolete action)
	-1,			// #136 (obsolete action)
	-1,			// #137 (obsolete action)
	-1,			// #138 (obsolete action)
	ActAdmAsgWrkCrs,	// #139
	-1,			// #140 (obsolete action)
	-1,			// #141 (obsolete action)
	-1,			// #142 (obsolete action)
	ActRenTag,		// #143
	-1,			// #144 (obsolete action)
	-1,			// #145 (obsolete action)
	-1,			// #146 (obsolete action)
	-1,			// #147 (obsolete action)
	ActRcvFilWrkUsrCla,	// #148
	-1,			// #149 (obsolete action)
	ActFrmCreWrkUsr,	// #150
	-1,			// #151 (obsolete action)
	ActPrnCrsTT,	// #152
	ActRcvFilBrfCla,	// #153
	-1,			// #154 (obsolete action)
	ActRemFilBrf,		// #155
	-1,			// #156 (obsolete action)
	-1,			// #157 (obsolete action)
	-1,			// #158 (obsolete action)
	-1,			// #159 (obsolete action)
	-1,			// #160 (obsolete action)
	-1,			// #161 (obsolete action)
	-1,			// #162 (obsolete action)
	-1,			// #163 (obsolete action)
	ActRcvPagCrsLnk,	// #164
	-1,			// #165 (obsolete action)
	-1,			// #166 (obsolete action)
	ActChgGrpTyp,		// #167
	-1,			// #168 (obsolete action)
	ActRemFilWrkUsr,	// #169
	ActCreFolBrf,		// #170
	-1,			// #171 (obsolete action)
	ActCreFolWrkUsr,	// #172
	-1,			// #173 (obsolete action)
	ActNewGrpTyp,		// #174
	ActRemGrp,		// #175
	-1,			// #176 (obsolete action)
	-1,			// #177 (obsolete action)
	-1,			// #178 (obsolete action)
	ActPrnExaAnn,		// #179
	-1,			// #180 (obsolete action)
	ActInsIteSylPra,	// #181
	ActRcvURLCrsLnk,	// #182
	ActDelItmSylPra,	// #183
	ActRcvPagAss,		// #184
	ActRcvPagBib,		// #185
	-1,			// #186 (obsolete action)
	ActRemExaAnn,		// #187
	-1,			// #188 (obsolete action)
	-1,			// #189 (obsolete action)
	-1,			// #190 (obsolete action)
	-1,			// #191 (obsolete action)
	-1,			// #192 (obsolete action)
	-1,			// #193 (obsolete action)
	ActUnk,			// #194
	-1,			// #195 (obsolete action)
	ActRemFolBrf,		// #196
	ActRenFolBrf,		// #197
	-1,			// #198 (obsolete action)
	-1,			// #199 (obsolete action)
	-1,			// #200 (obsolete action)
	-1,			// #201 (obsolete action)
	-1,			// #202 (obsolete action)
	-1,			// #203 (obsolete action)
	ActRenFolWrkUsr,	// #204
	ActFrmCreWrkCrs,	// #205
	ActCreFolWrkCrs,	// #206
	ActRcvFilWrkCrsCla,	// #207
	ActRenFolWrkCrs,	// #208
	ActRemFilWrkCrs,	// #209
	ActRemFolWrkCrs,	// #210
	ActModIteSylLec,	// #211
	ActDwnIteSylPra,	// #212
	ActUp_IteSylPra,	// #213
	ActLftIteSylPra,	// #214
	ActRgtIteSylPra,	// #215
	ActModIteSylPra,	// #216
	ActInsIteSylLec,	// #217
	ActDelItmSylLec,	// #218
	ActRcvPagFAQ,		// #219
	ActDwnIteSylLec,	// #220
	ActUp_IteSylLec,	// #221
	ActLftIteSylLec,	// #222
	ActRgtIteSylLec,	// #223
	ActRcvURLBib,		// #224
	-1,			// #225 (obsolete action)
	-1,			// #226 (obsolete action)
	-1,			// #227 (obsolete action)
	ActRemFolWrkUsr,	// #228
	-1,			// #229 (obsolete action)
	-1,			// #230 (obsolete action)
	-1,			// #231 (obsolete action)
	-1,			// #232 (obsolete action)
	-1,			// #233 (obsolete action)
	ActRcvURLFAQ,		// #234
	ActRcvURLAss,		// #235
	ActReqRemGrpTyp,	// #236
	ActRemGrpTyp,		// #237
	-1,			// #238 (obsolete action)
	-1,			// #239 (obsolete action)
	-1,			// #240 (obsolete action)
	ActSeeForDegUsr,	// #241
	ActSeeForSWAUsr,	// #242
	ActSeeForDegTch,	// #243
	ActSeePstForSWAUsr,	// #244
	ActSeeForSWATch,	// #245
	ActSeePstForSWATch,	// #246
	ActRcvThrForDegTch,	// #247
	-1,			// #248 (obsolete action)
	-1,			// #249 (obsolete action)
	-1,			// #250 (obsolete action)
	-1,			// #251 (obsolete action)
	ActRcvThrForDegUsr,	// #252
	-1,			// #253 (obsolete action)
	-1,			// #254 (obsolete action)
	ActSeePstForDegUsr,	// #255
	-1,			// #256 (obsolete action)
	-1,			// #257 (obsolete action)
	ActRcvThrForSWAUsr,	// #258
	ActRcvThrForSWATch,	// #259
	-1,			// #260 (obsolete action)
	-1,			// #261 (obsolete action)
	-1,			// #262 (obsolete action)
	-1,			// #263 (obsolete action)
	-1,			// #264 (obsolete action)
	-1,			// #265 (obsolete action)
	-1,			// #266 (obsolete action)
	-1,			// #267 (obsolete action)
	-1,			// #268 (obsolete action)
	-1,			// #269 (obsolete action)
	-1,			// #270 (obsolete action)
	-1,			// #271 (obsolete action)
	-1,			// #272 (obsolete action)
	-1,			// #273 (obsolete action)
	-1,			// #274 (obsolete action)
	-1,			// #275 (obsolete action)
	ActRemTreBrf,		// #276
	-1,			// #277 (obsolete action)
	ActRemTreWrkUsr,	// #278
	ActRemTreWrkCrs,	// #279
	-1,			// #280 (obsolete action)
	-1,			// #281 (obsolete action)
	-1,			// #282 (obsolete action)
	-1,			// #283 (obsolete action)
	ActAdmMrkCrs,		// #284
	ActReqEdiRecSha,	// #285
	ActReqRemFilBrf,	// #286
	-1,			// #287 (obsolete action)
	ActReqRemFilWrkUsr,	// #288
	ActReqRemFilWrkCrs,	// #289
	-1,			// #290 (obsolete action)
	ActSeePstForDegTch,	// #291
	ActEdiRecFie,		// #292
	ActNewFie,		// #293
	ActReqRemFie,		// #294
	ActRemFie,		// #295
	ActRenFie,		// #296
	ActChgVisFie,		// #297
	ActChgMyData,		// #298
	-1,			// #299 (obsolete action)
	ActRcvRecOthUsr,	// #300
	ActRcvRecCrs,		// #301
	ActChgMulGrpTyp,	// #302
	ActChgMdtGrpTyp,	// #303
	ActRenGrpTyp,		// #304
	ActChgRowFie,		// #305
	-1,			// #306 (obsolete action)
	-1,			// #307 (obsolete action)
	-1,			// #308 (obsolete action)
	-1,			// #309 (obsolete action)
	-1,			// #310 (obsolete action)
	ActCopBrf,		// #311
	ActCopWrkCrs,		// #312
	-1,			// #313 (obsolete action)
	ActCopWrkUsr,		// #314
	ActPasBrf,		// #315
	-1,			// #316 (obsolete action)
	-1,			// #317 (obsolete action)
	ActPasWrkUsr,		// #318
	ActPasWrkCrs,		// #319
	-1,			// #320 (obsolete action)
	ActCloGrp,		// #321
	ActOpeGrp,		// #322
	ActFrmCreShaCrs,	// #323
	ActCreFolShaCrs,	// #324
	ActRemFolShaCrs,	// #325
	ActRcvFilShaCrsCla,	// #326
	ActReqRemFilShaCrs,	// #327
	ActRemFilShaCrs,	// #328
	ActRenFolShaCrs,	// #329
	ActCopShaCrs,		// #330
	ActPasShaCrs,		// #331
	ActRemTreShaCrs,	// #332
	ActFrmCreShaGrp,	// #333
	ActCreFolShaGrp,	// #334
	ActRcvFilShaGrpCla,	// #335
	ActCopShaGrp,		// #336
	ActPasShaGrp,		// #337
	ActRemFolShaGrp,	// #338
	ActRemTreShaGrp,	// #339
	ActRenFolShaGrp,	// #340
	ActReqRemFilShaGrp,	// #341
	ActRemFilShaGrp,	// #342
	-1,			// #343 (obsolete action)
	-1,			// #344 (obsolete action)
	ActSeeForCrsUsr,	// #345
	ActSeePstForCrsUsr,	// #346
	ActSeePstForCrsTch,	// #347
	ActSeePstForCtrUsr,	// #348
	-1,			// #349 (obsolete action)
	ActRcvThrForCrsUsr,	// #350
	-1,			// #351 (obsolete action)
	-1,			// #352 (obsolete action)
	-1,			// #353 (obsolete action)
	-1,			// #354 (obsolete action)
	-1,			// #355 (obsolete action)
	-1,			// #356 (obsolete action)
	-1,			// #357 (obsolete action)
	-1,			// #358 (obsolete action)
	-1,			// #359 (obsolete action)
	-1,			// #360 (obsolete action)
	-1,			// #361 (obsolete action)
	-1,			// #362 (obsolete action)
	-1,			// #363 (obsolete action)
	-1,			// #364 (obsolete action)
	-1,			// #365 (obsolete action)
	-1,			// #366 (obsolete action)
	-1,			// #367 (obsolete action)
	-1,			// #368 (obsolete action)
	-1,			// #369 (obsolete action)
	ActSelInfSrcBib,	// #370
	ActEditorSylPra,	// #371
	ActEditorSylLec,	// #372
	-1,			// #373 (obsolete action)
	-1,			// #374 (obsolete action)
	-1,			// #375 (obsolete action)
	ActEditorBib,		// #376
	ActPlaTxtEdiBib,	// #377
	ActSelInfSrcSylLec,	// #378
	ActPlaTxtEdiSylLec,	// #379
	ActSelInfSrcFAQ,	// #380
	ActRcvPagSylLec,	// #381
	ActSelInfSrcSylPra,	// #382
	ActRcvPagSylPra,	// #383
	ActSelInfSrcAss,	// #384
	ActSelInfSrcCrsLnk,	// #385
	ActEditorAss,		// #386
	ActPlaTxtEdiAss,	// #387
	ActEditorCrsLnk,	// #388
	ActPlaTxtEdiSylPra,	// #389
	-1,			// #390 (obsolete action)
	-1,			// #391 (obsolete action)
	-1,			// #392 (obsolete action)
	-1,			// #393 (obsolete action)
	ActRcvPlaTxtSylLec,	// #394
	-1,			// #395 (obsolete action)
	ActRcvPlaTxtSylPra,	// #396
	ActRcvPlaTxtAss,	// #397
	ActRcvPlaTxtBib,	// #398
	-1,			// #399 (obsolete action)
	ActPlaTxtEdiCrsLnk,	// #400
	ActRcvPlaTxtCrsLnk,	// #401
	ActRcvURLSylPra,	// #402
	ActRcvURLSylLec,	// #403
	ActEditorFAQ,		// #404
	ActPlaTxtEdiFAQ,	// #405
	ActRcvPlaTxtFAQ,	// #406
	-1,			// #407 (obsolete action)
	ActSeeMyTT,		// #408
	ActPrnMyTT,		// #409
	ActExpBrf,		// #410
	ActConBrf,		// #411
	-1,			// #412 (obsolete action)
	-1,			// #413 (obsolete action)
	-1,			// #414 (obsolete action)
	-1,			// #415 (obsolete action)
	ActExpWrkCrs,		// #416
	-1,			// #417 (obsolete action)
	-1,			// #418 (obsolete action)
	-1,			// #419 (obsolete action)
	-1,			// #420 (obsolete action)
	ActExpShaCrs,		// #421
	ActConShaCrs,		// #422
	ActExpWrkUsr,		// #423
	ActConWrkCrs,		// #424
	ActConWrkUsr,		// #425
	ActConShaGrp,		// #426
	ActExpShaGrp,		// #427
	ActRemMyPho,		// #428
	-1,			// #429 (obsolete action)
	ActSeeForCtrTch,	// #430
	ActSeeForCrsTch,	// #431
	-1,			// #432 (obsolete action)
	-1,			// #433 (obsolete action)
	ActDelAllSntMsg,	// #434
	-1,			// #435 (obsolete action)
	ActDelAllRcvMsg,	// #436
	-1,			// #437 (obsolete action)
	-1,			// #438 (obsolete action)
	-1,			// #439 (obsolete action)
	-1,			// #440 (obsolete action)
	-1,			// #441 (obsolete action)
	-1,			// #442 (obsolete action)
	ActPrnTchPho,		// #443
	ActCalPhoDeg,		// #444
	-1,			// #445 (obsolete action)
	-1,			// #446 (obsolete action)
	ActSeePhoDeg,		// #447
	ActPrnPhoDeg,		// #448
	-1,			// #449 (obsolete action)
	-1,			// #450 (obsolete action)
	ActCfgTst,		// #451
	ActDisableTag,		// #452
	ActEnableTag,		// #453
	ActRcvCfgTst,		// #454
	ActShfTstQst,		// #455
	-1,			// #456 (obsolete action)
	-1,			// #457 (obsolete action)
	-1,			// #458 (obsolete action)
	-1,			// #459 (obsolete action)
	-1,			// #460 (obsolete action)
	ActAdmShaCrsGrp,	// #461
	ActExpSeeDocCrs,	// #462
	-1,			// #463 (obsolete action)
	ActShoDocCrs,		// #464
	ActHidDocCrs,		// #465
	-1,			// #466 (obsolete action)
	-1,			// #467 (obsolete action)
	ActFrmCreDocGrp,	// #468
	ActCreFolDocGrp,	// #469
	ActCopDocCrs,		// #470
	ActPasDocGrp,		// #471
	ActCopDocGrp,		// #472
	ActReqRemFilDocGrp,	// #473
	ActRemFilDocGrp,	// #474
	-1,			// #475 (obsolete action)
	ActConSeeDocCrs,	// #476
	ActExpAdmDocCrs,	// #477
	ActPasDocCrs,		// #478
	ActReqRemFilDocCrs,	// #479
	ActRemFilDocCrs,	// #480
	ActFrmCreDocCrs,	// #481
	ActRcvFilDocCrsCla,	// #482
	ActRcvFilDocGrpCla,	// #483
	ActRemFolDocGrp,	// #484
	ActRemTreDocGrp,	// #485
	ActExpAdmDocGrp,	// #486
	ActConAdmDocGrp,	// #487
	ActExpSeeDocGrp,	// #488
	ActConSeeDocGrp,	// #489
	ActRenFolDocGrp,	// #490
	ActCreFolDocCrs,	// #491
	ActHidDocGrp,		// #492
	ActShoDocGrp,		// #493
	ActConAdmDocCrs,	// #494
	ActEnaFilZonGrp,	// #495
	ActDisFilZonGrp,	// #496
	ActRemFolDocCrs,	// #497
	ActRemTreDocCrs,	// #498
	-1,			// #499 (obsolete action)
	-1,			// #500 (obsolete action)
	ActCopMrkCrs,		// #501
	ActPasMrkGrp,		// #502
	ActChgNumRowHeaCrs,	// #503
	ActChgNumRowFooCrs,	// #504
	-1,			// #505 (obsolete action)
	ActCreFolMrkCrs,	// #506
	ActPasMrkCrs,		// #507
	-1,			// #508 (obsolete action)
	ActRemFilMrkGrp,	// #509
	ActChgNumRowHeaGrp,	// #510
	ActChgNumRowFooGrp,	// #511
	-1,			// #512 (obsolete action)
	ActCreFolMrkGrp,	// #513
	ActRcvFilMrkGrpCla,	// #514
	-1,			// #515 (obsolete action)
	ActRcvFilMrkCrsCla,	// #516
	-1,			// #517 (obsolete action)
	-1,			// #518 (obsolete action)
	ActCopMrkGrp,		// #519
	ActRemFolMrkGrp,	// #520
	ActRemTreMrkGrp,	// #521
	-1,			// #522 (obsolete action)
	ActSeeMyMrkCrs,		// #523
	ActSeeMyMrkGrp,		// #524
	-1,			// #525 (obsolete action)
	-1,			// #526 (obsolete action)
	ActConSeeMrkCrs,	// #527
	ActExpSeeMrkCrs,	// #528
	ActRenFolMrkGrp,	// #529
	ActRemFolMrkCrs,	// #530
	-1,			// #531 (obsolete action)
	-1,			// #532 (obsolete action)
	ActRemFilMrkCrs,	// #533
	ActRemTreMrkCrs,	// #534
	ActRenFolDocCrs,	// #535
	ActEdiDeg,		// #536
	ActNewDegTyp,		// #537
	ActRenDegTyp,		// #538
	-1,			// #539 (obsolete action)
	ActNewDeg,		// #540
	-1,			// #541 (obsolete action)
	ActRemDeg,		// #542
	-1,			// #543 (obsolete action)
	ActChgDegTyp,		// #544
	ActRemDegTyp,		// #545
	ActRenDegSho,		// #546
	ActRenDegFul,		// #547
	-1,			// #548 (obsolete action)
	-1,			// #549 (obsolete action)
	-1,			// #550 (obsolete action)
	-1,			// #551 (obsolete action)
	-1,			// #552
	ActRecDegLog,		// #553
	ActChgDegWWW,		// #554
	ActEdiCrs,		// #555
	ActNewCrs,		// #556
	-1,			// #557 (obsolete action)
	-1,			// #558 (obsolete action)
	-1,			// #559 (obsolete action)
	ActRemCrs,		// #560
	ActChgCrsYea,		// #561
	-1,			// #562
	ActRenCrsSho,		// #563
	ActRenCrsFul,		// #564
	-1,			// #565 (obsolete action)
	-1,			// #566 (obsolete action)
	-1,			// #567 (obsolete action)
	-1,			// #568 (obsolete action)
	-1,			// #569 (obsolete action)
	-1,			// #570 (obsolete action)
	-1,			// #571 (obsolete action)
	-1,			// #572 (obsolete action)
	ActEdiDegTyp,		// #573
	ActRenFolMrkCrs,	// #574
	-1,			// #575 (obsolete action)
	-1,			// #576 (obsolete action)
	-1,			// #577 (obsolete action)
	ActLstTchAll,		// #578
	-1,			// #579 (obsolete action)
	-1,			// #580 (obsolete action)
	-1,			// #581 (obsolete action)
	-1,			// #582 (obsolete action)
	-1,			// #583 (obsolete action)
	ActRemAdmDeg,		// #584
	-1,			// #585 (obsolete action)
	ActNewAdmDeg,		// #586
	ActLstGst,		// #587
	-1,			// #588 (obsolete action)
	ActChgMyRol,		// #589
	ActReqRemOldUsr,	// #590
	ActReqAccGbl,		// #591
	-1,			// #592 (obsolete action)
	ActReqDelAllRcvMsg,	// #593
	ActReqAccCrs,		// #594
	ActReqRemFilMrkCrs,	// #595
	ActFrmCreMrkCrs,	// #596
	ActFrmCreBrf,		// #597
	-1,			// #598 (obsolete action)
	ActRcvRepForCrsUsr,	// #599
	ActReqRemFilMrkGrp,	// #600
	ActFrmCreMrkGrp,	// #601
	ActDelPstForCrsUsr,	// #602
	ActRcvRepForSWAUsr,	// #603
	ActReqDelAllSntMsg,	// #604
	ActExpSeeMrkGrp,	// #605
	ActRcvRepForDegUsr,	// #606
	ActExpAdmMrkCrs,	// #607
	ActDelPstForDegUsr,	// #608
	ActConSeeMrkGrp,	// #609
	ActDisPstForCrsUsr,	// #610
	-1,			// #611 (obsolete action)
	-1,			// #612 (obsolete action)
	ActDelPstForSWAUsr,	// #613
	-1,			// #614 (obsolete action)
	ActDisPstForDegUsr,	// #615
	ActEnbPstForDegUsr,	// #616
	ActRcvRepForDegTch,	// #617
	ActDisPstForDegTch,	// #618
	ActEnbPstForDegTch,	// #619
	-1,			// #620 (obsolete action)
	ActConAdmMrkCrs,	// #621
	ActRcvRepForSWATch,	// #622
	ActDelPstForSWATch,	// #623
	ActEnbPstForCrsUsr,	// #624
	ActDisPstForSWAUsr,	// #625
	-1,			// #626 (obsolete action)
	ActReqSch,		// #627
	ActSch,			// #628
	-1,			// #629 (obsolete action)
	-1,			// #630 (obsolete action)
	ActExpAdmMrkGrp,	// #631
	ActEnbPstForSWAUsr,	// #632
	ActSndNewPwd,		// #633
	ActEnbPstForSWATch,	// #634
	ActDisPstForSWATch,	// #635
	-1,			// #636 (obsolete action)
	-1,			// #637 (obsolete action)
	-1,			// #638 (obsolete action)
	-1,			// #639 (obsolete action)
	-1,			// #640 (obsolete action)
	-1,			// #641 (obsolete action)
	-1,			// #642 (obsolete action)
	-1,			// #643 (obsolete action)
	-1,			// #644 (obsolete action)
	ActAll,			// #645
	-1,			// #646 (obsolete action)
	-1,			// #647 (obsolete action)
	-1,			// #648 (obsolete action)
	-1,			// #649 (obsolete action)
	-1,			// #650 (obsolete action)
	-1,			// #651 (obsolete action)
	-1,			// #652 (obsolete action)
	-1,			// #653 (obsolete action)
	ActChgLan,		// #654
	-1,			// #655 (obsolete action)
	-1,			// #656 (obsolete action)
	-1,			// #657 (obsolete action)
	-1,			// #658 (obsolete action)
	-1,			// #659 (obsolete action)
	-1,			// #660 (obsolete action)
	-1,			// #661 (obsolete action)
	-1,			// #662 (obsolete action)
	ActExpRcvMsg,		// #663
	ActExpSntMsg,		// #664
	ActReqSndNewPwd,	// #665
	-1,			// #666 (obsolete action)
	-1,			// #667 (obsolete action)
	ActHidLftCol,		// #668
	ActHidRgtCol,		// #669
	ActShoLftCol,		// #670
	ActShoRgtCol,		// #671
	-1,			// #672 (obsolete action)
	ActReqEdiSet,		// #673
	ActChgCol,		// #674
	ActSeeDpt,		// #675
	ActSeeCtr,		// #676
	ActEdiDpt,		// #677
	ActLstStd,		// #678
	ActLstTch,		// #679
	ActDelPstForDegTch,	// #680
	ActEdiCtr,		// #681
	ActRenCtrSho,		// #682
	ActChgCtrWWW,		// #683
	ActRenCtrFul,		// #684
	ActNewCtr,		// #685
	ActRemCtr,		// #686
	ActNewDpt,		// #687
	ActRenDptSho,		// #688
	ActRenDptFul,		// #689
	ActRemDpt,		// #690
	ActChgDptWWW,		// #691
	-1,			// #692 (obsolete action)
	ActDetMyPho,		// #693
	ActUpdMyPho,		// #694
	-1,			// #695 (obsolete action)
	ActSeeIns,		// #696
	ActEdiIns,		// #697
	ActNewIns,		// #698
	ActRecInsLog,		// #699
	ActChgInsWWW,		// #700
	ActRenInsFul,		// #701
	ActRenInsSho,		// #702
	ActSeePlc,		// #703
	ActEdiPlc,		// #704
	ActNewPlc,		// #705
	ActChgCtrPlc,		// #706
	ActSeeHld,		// #707
	-1,			// #708 (obsolete action)
	-1,			// #709 (obsolete action)
	-1,			// #710 (obsolete action)
	-1,			// #711 (obsolete action)
	-1,			// #712 (obsolete action)
	ActEdiHld,		// #713
	ActNewHld,		// #714
	ActChgHldTyp,		// #715
	ActRemHld,		// #716
	ActChgHldStrDat,	// #717
	ActChgHldEndDat,	// #718
	-1,			// #719 (obsolete action)
	-1,			// #720 (obsolete action)
	ActChgDptIns,		// #721
	-1,			// #722 (obsolete action)
	ActSeeForGenTch,	// #723
	ActSeeForInsTch,	// #724
	ActSeeForInsUsr,	// #725
	ActSeeForGenUsr,	// #726
	ActSeePstForGenUsr,	// #727
	-1,			// #728 (obsolete action)
	-1,			// #729 (obsolete action)
	ActSeePstForInsUsr,	// #730
	ActSeePstForGenTch,	// #731
	-1,			// #732 (obsolete action)
	-1,			// #733 (obsolete action)
	-1,			// #734 (obsolete action)
	ActDelPstForGenUsr,	// #735
	ActRcvThrForGenUsr,	// #736
	ActRcvThrForInsUsr,	// #737
	-1,			// #738 (obsolete action)
	-1,			// #739 (obsolete action)
	ActRcvRepForInsUsr,	// #740
	-1,			// #741 (obsolete action)
	-1,			// #742 (obsolete action)
	ActDelPstForInsUsr,	// #743
	-1,			// #744 (obsolete action)
	-1,			// #745 (obsolete action)
	ActSeePstForInsTch,	// #746
	ActRcvRepForGenUsr,	// #747
	ActSeeLnk,		// #748
	ActEdiLnk,		// #749
	ActNewLnk,		// #750
	ActRenLnkFul,		// #751
	ActChgLnkWWW,		// #752
	ActRenLnkSho,		// #753
	ActRcvThrForCrsTch,	// #754
	ActRcvRepForCrsTch,	// #755
	ActDelPstForCrsTch,	// #756
	-1,			// #757 (obsolete action)
	-1,			// #758 (obsolete action)
	ActRemIns,		// #759
	-1,			// #760 (obsolete action)
	ActReqUseGbl,		// #761
	ActSeeAllNot,		// #762
	ActHidNot,		// #763
	ActRevNot,		// #764
	ActRcvThrForGenTch,	// #765
	ActRenHld,		// #766
	ActReqStaCrs,		// #767
	ActSeeAllStaCrs,	// #768
	ActRcvThrForInsTch,	// #769
	ActRcvRepForInsTch,	// #770
	-1,			// #771 (obsolete action)
	ActDelPstForInsTch,	// #772
	ActRemOldUsr,		// #773
	ActChgPriPho,		// #774
	ActChgNtfPrf,		// #775
	ActRemPlc,		// #776
	ActLstPlg,		// #777
	ActEdiPlg,		// #778
	ActNewPlg,		// #779
	ActChgPlgIP,		// #780
	ActChgPlgLog,		// #781
	ActRenPlg,		// #782
	ActChgPlgURL,		// #783
	ActSeeTchGui,		// #784
	ActEdiTchGui,		// #785
	ActEditorTchGui,	// #786
	ActPlaTxtEdiTchGui,	// #787
	ActRcvPagTchGui,	// #788
	ActSelInfSrcTchGui,	// #789
	ActRcvPlaTxtTchGui,	// #790
	ActRcvURLTchGui,	// #791
	ActAdmAsgWrkUsr,	// #792
	-1,			// #793 (obsolete action)
	-1,			// #794 (obsolete action)
	-1,			// #795 (obsolete action)
	-1,			// #796 (obsolete action)
	-1,			// #797 (obsolete action)
	-1,			// #798 (obsolete action)
	-1,			// #799 (obsolete action)
	-1,			// #800 (obsolete action)
	ActSeeAsg,		// #801
	-1,			// #802 (obsolete action)
	ActNewAsg,		// #803
	-1,			// #804 (obsolete action)
	-1,			// #805 (obsolete action)
	ActRemAsg,		// #806
	-1,			// #807 (obsolete action)
	-1,			// #808 (obsolete action)
	-1,			// #809 (obsolete action)
	-1,			// #810 (obsolete action)
	-1,			// #811 (obsolete action)
	ActFrmNewAsg,		// #812
	ActReqRemAsg,		// #813
	ActEdiOneAsg,		// #814
	ActChgAsg,		// #815
	ActRcvRepForGenTch,	// #816
	ActFrmCreAsgCrs,	// #817
	ActCreFolAsgCrs,	// #818
	ActExpAsgCrs,		// #819
	ActRemFolAsgCrs,	// #820
	ActPasAsgCrs,		// #821
	ActRemTreAsgCrs,	// #822
	ActRenFolAsgCrs,	// #823
	ActExpAsgUsr,		// #824
	ActFrmCreAsgUsr,	// #825
	ActCreFolAsgUsr,	// #826
	ActRemFolAsgUsr,	// #827
	ActRemTreAsgUsr,	// #828
	ActCopAsgUsr,		// #829
	ActPasAsgUsr,		// #830
	ActConAsgUsr,		// #831
	ActRcvFilAsgUsrCla,	// #832
	ActRemFilAsgUsr,	// #833
	ActReqRemFilAsgUsr,	// #834
	ActConAsgCrs,		// #835
	ActCopAsgCrs,		// #836
	ActReqRemFilAsgCrs,	// #837
	ActRemFilAsgCrs,	// #838
	ActRenFolAsgUsr,	// #839
	ActSetUp,		// #840
	ActChgThe,		// #841
	-1,			// #842 (obsolete action)
	ActFrmRolSes,		// #843
	-1,			// #844 (obsolete action)
	ActRefCon,		// #845
	ActRcvFilAsgCrsCla,	// #846
	ActSeeCrsInf,		// #847
	ActEdiCrsInf,		// #848
	ActSelInfSrcCrsInf,	// #849
	ActPlaTxtEdiCrsInf,	// #850
	ActRcvPlaTxtCrsInf,	// #851
	ActEditorCrsInf,	// #852
	ActRcvPagCrsInf,	// #853
	ActRcvURLCrsInf,	// #854
	ActSeeMai,		// #855
	ActEdiMai,		// #856
	ActNewMai,		// #857
	ActRenMaiSho,		// #858
	ActRenMaiFul,		// #859
	ActRemMai,		// #860
	ActSeeAtt,		// #861
	ActSeeCty,		// #862
	ActEdiCty,		// #863
	ActNewCty,		// #864
	-1,			// #865 (obsolete action)
	ActRenCty,		// #866
	ActReqDelThrCrsUsr,	// #867
	ActDelThrForCrsUsr,	// #868
	ActReqDelThrCrsTch,	// #869
	ActChgFrcReaTchGui,	// #870
	ActChgFrcReaSylLec,	// #871
	ActChgFrcReaSylPra,	// #872
	ActChgFrcReaBib,	// #873
	ActChgFrcReaFAQ,	// #874
	ActChgFrcReaCrsLnk,	// #875
	ActDelThrForCrsTch,	// #876
	ActChgFrcReaCrsInf,	// #877
	ActChgHavReaCrsInf,	// #878
	ActChgHavReaFAQ,	// #879
	ActChgHavReaSylLec,	// #880
	ActReqDelThrSWAUsr,	// #881
	ActDelThrForSWAUsr,	// #882
	ActChgFrcReaAss,	// #883
	ActChgHavReaBib,	// #884
	ActChgHavReaCrsLnk,	// #885
	ActChgHavReaTchGui,	// #886
	ActChgHavReaSylPra,	// #887
	ActChgPlgDes,		// #888
	ActRemPlg,		// #889
	ActCutThrForSWAUsr,	// #890
	ActPasThrForCrsUsr,	// #891
	ActWebSvc,		// #892
	ActRemCty,		// #893
	ActRenPlcSho,		// #894
	ActRenPlcFul,		// #895
	ActChgHldPlc,		// #896
	ActRemLnk,		// #897
	ActChgHavReaAss,	// #898
	ActReqAsgWrkCrs,	// #899
	ActConAdmMrkGrp,	// #900
	ActSeeForCtrUsr,	// #901
	ActSeePstForCtrTch,	// #902
	ActRcvThrForCtrUsr,	// #903
	ActRcvThrForCtrTch,	// #904
	ActRcvRepForCtrUsr,	// #905
	ActRcvRepForCtrTch,	// #906
	ActReqDelThrDegUsr,	// #907
	ActReqDelThrDegTch,	// #908
	ActReqDelThrCtrUsr,	// #909
	ActReqDelThrCtrTch,	// #910
	ActReqDelThrInsUsr,	// #911
	ActReqDelThrInsTch,	// #912
	ActReqDelThrGenUsr,	// #913
	ActReqDelThrGenTch,	// #914
	ActReqDelThrSWATch,	// #915
	ActDelThrForDegUsr,	// #916
	ActDelThrForDegTch,	// #917
	ActDelThrForCtrUsr,	// #918
	ActDelThrForCtrTch,	// #919
	ActDelThrForInsUsr,	// #920
	ActDelThrForInsTch,	// #921
	ActDelThrForGenUsr,	// #922
	ActDelThrForGenTch,	// #923
	ActDelThrForSWATch,	// #924
	ActDisPstForGenTch,	// #925
	ActCutThrForCrsUsr,	// #926
	ActCutThrForCrsTch,	// #927
	ActCutThrForDegUsr,	// #928
	ActCutThrForDegTch,	// #929
	ActCutThrForCtrUsr,	// #930
	ActCutThrForCtrTch,	// #931
	ActCutThrForInsUsr,	// #932
	ActCutThrForInsTch,	// #933
	ActCutThrForGenUsr,	// #934
	ActCutThrForGenTch,	// #935
	ActCutThrForSWATch,	// #936
	ActPasThrForCrsTch,	// #937
	ActPasThrForDegUsr,	// #938
	ActPasThrForDegTch,	// #939
	ActPasThrForCtrUsr,	// #940
	ActPasThrForCtrTch,	// #941
	ActPasThrForInsUsr,	// #942
	ActPasThrForInsTch,	// #943
	ActPasThrForGenUsr,	// #944
	ActPasThrForGenTch,	// #945
	ActPasThrForSWAUsr,	// #946
	ActPasThrForSWATch,	// #947
	ActDelPstForCtrUsr,	// #948
	ActDelPstForCtrTch,	// #949
	ActDelPstForGenTch,	// #950
	ActEnbPstForCrsTch,	// #951
	ActEnbPstForCtrUsr,	// #952
	ActEnbPstForCtrTch,	// #953
	ActEnbPstForInsUsr,	// #954
	ActEnbPstForInsTch,	// #955
	ActEnbPstForGenUsr,	// #956
	ActEnbPstForGenTch,	// #957
	ActDisPstForCrsTch,	// #958
	ActDisPstForCtrUsr,	// #959
	ActDisPstForCtrTch,	// #960
	ActDisPstForInsUsr,	// #961
	ActDisPstForInsTch,	// #962
	ActDisPstForGenUsr,	// #963
	ActHidAsg,		// #964
	ActShoAsg,		// #965
	ActSeeAllSvy,		// #966
	-1,			// #967 (obsolete action)
	ActNewSvy,		// #968
	ActRemSvy,		// #969
	-1,			// #970 (obsolete action)
	-1,			// #971 (obsolete action)
	-1,			// #972 (obsolete action)
	ActFrmNewSvy,		// #973
	ActEdiOneSvy,		// #974
	ActChgSvy,		// #975
	ActReqRemSvy,		// #976
	ActHidSvy,		// #977
	ActShoSvy,		// #978
	ActEdiOneSvyQst,	// #979
	ActRcvSvyQst,		// #980
	ActRemSvyQst,		// #981
	ActSeeSvy,		// #982
	ActAnsSvy,		// #983
	ActReqRstSvy,		// #984
	ActRstSvy,		// #985
	ActChgPlgAppKey,	// #986
	ActMyCrs,		// #987
	-1,			// #988 (obsolete action)
	ActLstClk,		// #989
	ActSeeNtf,		// #990
	ActSeeNewNtf,		// #991
	ActReqChgLan,		// #992
	ActMFUAct,		// #993
	ActRefLstClk,		// #994
	ActLstCon,		// #995
	ActChgDatAdmDocCrs,	// #996
	-1,			// #997 (obsolete action)
	ActChgDatAdmDocGrp,	// #998
	-1,			// #999 (obsolete action)
	ActChgDatShaCrs,	// #1000
	-1,			// #1001 (obsolete action)
	ActChgDatShaGrp,	// #1002
	-1,			// #1003 (obsolete action)
	-1,			// #1004 (obsolete action)
	-1,			// #1005 (obsolete action)
	-1,			// #1006 (obsolete action)
	ActReqImpTstQst,	// #1007
	ActImpTstQst,		// #1008
	ActSeeCrs,		// #1009
	-1,			// #1010 (obsolete action)
	ActSeeDeg,		// #1011
	-1,			// #1012 (obsolete action)
	ActSeeDegTyp,		// #1013
	-1,			// #1014 (obsolete action)
	ActBanUsrMsg,		// #1015
	ActUnbUsrMsg,		// #1016
	ActLstBanUsr,		// #1017
	ActUnbUsrLst,		// #1018
	ActConRcvMsg,		// #1019
	ActConSntMsg,		// #1020
	-1,			// #1021 (obsolete action)
	ActPrnUsrQR,		// #1022
	-1,			// #1023 (obsolete action)
	ActChgInsCrsCodCfg,	// #1024
	ActChgInsCrsCod,	// #1025
	-1,			// #1026 (obsolete action)
	-1,			// #1027 (obsolete action)
	ActPrnCrsInf,		// #1028
	ActReqDatAdmDocCrs,	// #1029
	ActReqDatAdmDocGrp,	// #1030
	ActReqDatShaCrs,	// #1031
	ActReqDatShaGrp,	// #1032
	ActReqDatSeeDocCrs,	// #1033
	ActReqDatSeeDocGrp,	// #1034
        ActReqDatAdmMrkCrs,	// #1035
        ActChgDatAdmMrkCrs,	// #1036
        ActReqDatAdmMrkGrp,	// #1037
        ActChgDatAdmMrkGrp,	// #1038
        ActReqDatAsgUsr,	// #1039
        ActChgDatAsgUsr,	// #1040
        ActReqDatWrkUsr,	// #1041
        ActChgDatWrkUsr,	// #1042
        ActReqDatAsgCrs,	// #1043
        ActChgDatAsgCrs,	// #1044
        ActReqDatWrkCrs,	// #1045
        ActChgDatWrkCrs,	// #1046
        ActReqDatBrf,		// #1047
        ActChgDatBrf,		// #1048
	-1,			// #1049 (obsolete action)
	-1,			// #1050 (obsolete action)
	ActRecCtrLog,		// #1051
	-1,			// #1052 (obsolete action)
	ActReqCrs,		// #1053
	ActReqSignUp,		// #1054
	ActChgCrsSta,		// #1055
	ActSignUp,		// #1056
	ActSeeSignUpReq,	// #1057
	ActReqRejSignUp,	// #1058
	ActRejSignUp,		// #1059
	ActSeePen,		// #1060
	ActChgTimGrpTyp,	// #1061
	-1,			// #1062 (obsolete action)
	ActFrmNewAtt,		// #1063
	ActEdiOneAtt,		// #1064
	ActNewAtt,		// #1065
	ActChgAtt,		// #1066
	ActReqRemAtt,		// #1067
	ActRemAtt,		// #1068
	ActHidAtt,		// #1069
	ActShoAtt,		// #1070
	ActSeeOneAtt,		// #1071
	ActRecAttStd,		// #1072
	ActReqLstUsrAtt,	// #1073
	ActSeeLstUsrAtt,	// #1074
	ActPrnLstUsrAtt,	// #1075
	ActRecAttMe,		// #1076
	ActLogInLan,		// #1077
	ActSeeDocCrs,		// #1078
	ActSeeMrkCrs,		// #1079
	ActReqSeeUsrTstRes,	// #1080
	ActSeeUsrTstRes,	// #1081
	ActSeeOneTstResOth,	// #1082
	ActReqSeeMyTstRes,	// #1083
	ActSeeMyTstRes,		// #1084
	ActSeeOneTstResMe,	// #1085
	ActReqDatSeeMrkCrs,	// #1086
	ActReqDatSeeMrkGrp,	// #1087
	ActChgMyMai,		// #1088
	ActRemMyNck,		// #1089
	ActRemMyMai,		// #1090
	ActCnfMai,		// #1091
	ActChgIco,		// #1092
	ActRchTxtEdiCrsInf,	// #1093
	ActRchTxtEdiTchGui,	// #1094
	ActRchTxtEdiSylLec,	// #1095
	ActRchTxtEdiSylPra,	// #1096
	ActRchTxtEdiBib,	// #1097
	ActRchTxtEdiFAQ,	// #1098
	ActRchTxtEdiCrsLnk,	// #1099
	ActRchTxtEdiAss,	// #1100
	ActRcvRchTxtCrsInf,	// #1101
	ActRcvRchTxtTchGui,	// #1102
	ActRcvRchTxtSylLec,	// #1103
	ActRcvRchTxtSylPra,	// #1104
	ActRcvRchTxtBib,	// #1105
	ActRcvRchTxtFAQ,	// #1106
	ActRcvRchTxtCrsLnk,	// #1107
	ActRcvRchTxtAss,	// #1108
	ActReqRemOldCrs,	// #1109
	ActRemOldCrs,		// #1110
	ActDowSeeDocCrs,	// #1111
	ActDowSeeDocGrp,	// #1112
	ActDowAdmDocCrs,	// #1113
	ActDowAdmDocGrp,	// #1114
	ActDowShaCrs,		// #1115
	ActDowShaGrp,		// #1116
        ActDowAsgUsr,		// #1117
        ActDowWrkUsr,		// #1118
        ActDowAsgCrs,		// #1119
        ActDowWrkCrs,		// #1120
        ActDowAdmMrkCrs,	// #1121
        ActDowAdmMrkGrp,	// #1122
        ActDowBrf,		// #1123
       	ActZIPSeeDocCrs,	// #1124
       	ActZIPSeeDocGrp,	// #1125
       	ActZIPAdmDocCrs,	// #1126
       	ActZIPAdmDocGrp,	// #1127
	ActZIPShaCrs,		// #1128
	ActZIPShaGrp,		// #1129
	ActZIPAsgUsr,		// #1130
	ActZIPWrkUsr,		// #1131
	ActZIPAsgCrs,		// #1132
	ActZIPWrkCrs,		// #1133
	ActZIPAdmMrkCrs,	// #1134
	ActZIPAdmMrkGrp,	// #1135
	ActZIPBrf,		// #1136
	ActSeeBan,		// #1137
	ActEdiBan,		// #1138
	ActNewBan,		// #1139
	ActRemBan,		// #1140
	ActRenBanSho,		// #1141
	ActRenBanFul,		// #1142
	ActChgBanWWW,		// #1143
	ActChgBanImg,		// #1144
	ActClkBan,		// #1145
	ActMrkNtfSee,		// #1146
	ActRemMyID,		// #1147
	ActChgMyID,		// #1148
	ActSeeDegInf,		// #1149
	ActPrnDegInf,		// #1150
	ActSeeCtrInf,		// #1151
	ActPrnCtrInf,		// #1152
	ActSeeInsInf,		// #1153
	ActPrnInsInf,		// #1154
	ActSeeCtyInf,		// #1155
	ActPrnCtyInf,		// #1156
	ActChgCtyWWW,		// #1157
	ActChgCtyMapAtt,	// #1158
	ActChgCtrPhoAtt,	// #1159
	ActReqCtrPho,		// #1160
	ActRecCtrPho,		// #1161
	-1,			// #1162 (obsolete action)
	ActCreUsrAcc,		// #1163
	ActSeeOneNot,		// #1164
	-1,			// #1165 (obsolete action)
	ActChgCtyMyIns,		// #1166
	ActChgMyIns,		// #1167
	ActChgMyCtr,		// #1168
	ActChgMyDpt,		// #1169
	ActChgMyOff,		// #1170
	ActChgMyOffPho,		// #1171
	ActReqEdiMyNet,		// #1172
	ActChgMyNet,		// #1173
	ActSeeRecOneStd,	// #1174
	ActSeeRecOneTch,	// #1175
	-1,			// #1176 (obsolete action)
	-1,			// #1177 (obsolete action)
	-1,			// #1178 (obsolete action)
	-1,			// #1179 (obsolete action)
	-1,			// #1180 (obsolete action)
	-1,			// #1181 (obsolete action)
	-1,			// #1182 (obsolete action)
	-1,			// #1183 (obsolete action)
	-1,			// #1184 (obsolete action)
	-1,			// #1185 (obsolete action)
	ActLstOth,		// #1186
	ActSeeRecSevGst,	// #1187
	ActPrnRecSevGst,	// #1188
	ActLstGstAll,		// #1189
	ActPrnGstPho,		// #1190
	ActShoMrkCrs,		// #1191
	ActHidMrkCrs,		// #1192
	ActShoMrkGrp,		// #1193
	ActHidMrkGrp,		// #1194
	ActChgToSeeDocCrs,	// #1195
	ActChgToAdmDocCrs,	// #1196
	ActChgToAdmSha,		// #1197
	ActChgToSeeMrk,		// #1198
	ActChgToAdmMrk,		// #1199
	ActSeeDocGrp,		// #1200
	ActAdmDocGrp,		// #1201
	ActAdmShaCrs,		// #1202
	ActAdmShaGrp,		// #1203
	ActSeeMrkGrp,		// #1204
	ActAdmMrkGrp,		// #1205
	ActReqDeg,		// #1206
	ActChgDegSta,		// #1207
	ActReqCtr,		// #1208
	ActChgCtrSta,		// #1209
	ActReqIns,		// #1210
	ActChgInsSta,		// #1211
	ActShoBan,		// #1212
	ActHidBan,		// #1213
	ActRcvFilDocCrsDZ,	// #1214
	ActRcvFilDocGrpDZ,	// #1215
	ActRcvFilShaCrsDZ,	// #1216
	ActRcvFilShaGrpDZ,	// #1217
	ActRcvFilAsgUsrDZ,	// #1218
	ActRcvFilWrkUsrDZ,	// #1219
	ActRcvFilAsgCrsDZ,	// #1220
	ActRcvFilWrkCrsDZ,	// #1221
	ActRcvFilMrkCrsDZ,	// #1222
	ActRcvFilMrkGrpDZ,	// #1223
	ActRcvFilBrfDZ,		// #1224
	ActCreLnkDocCrs,	// #1225
	ActCreLnkShaCrs,	// #1226
	ActCreLnkShaGrp,	// #1227
	ActCreLnkWrkUsr,	// #1228
	ActCreLnkWrkCrs,	// #1229
	ActCreLnkBrf,		// #1230
	ActCreLnkDocGrp,	// #1231
	ActCreLnkAsgUsr,	// #1232
	ActCreLnkAsgCrs,	// #1233
	ActAnnSee,		// #1234
	ActSeeAnn,		// #1235
	ActRemAnn,		// #1236
	ActWriAnn,		// #1237
	ActRcvAnn,		// #1238
	-1,			// #1239 (obsolete action)
	-1,			// #1240 (obsolete action)
	-1,			// #1241 (obsolete action)
	ActSeeSyl,		// #1242
	ActChgMnu,		// #1243
	ActReqCtrLog,		// #1244
	ActReqInsLog,		// #1245
	ActReqDegLog,		// #1246
	ActSeeAdmDocDeg,	// #1247
	ActSeeAdmDocCtr,	// #1248
	ActSeeAdmDocIns,	// #1249
	ActChgToSeeDocDeg,	// #1250
	ActSeeDocDeg,		// #1251
	ActExpSeeDocDeg,	// #1252
	ActConSeeDocDeg,	// #1253
	ActZIPSeeDocDeg,	// #1254
	ActReqDatSeeDocDeg,	// #1255
	ActDowSeeDocDeg,	// #1256
	ActChgToAdmDocDeg,	// #1257
	ActAdmDocDeg,		// #1258
	ActReqRemFilDocDeg,	// #1259
	ActRemFilDocDeg,	// #1260
	ActRemFolDocDeg,	// #1261
	ActCopDocDeg,		// #1262
	ActPasDocDeg,		// #1263
	ActRemTreDocDeg,	// #1264
	ActFrmCreDocDeg,	// #1265
	ActCreFolDocDeg,	// #1266
	ActCreLnkDocDeg,	// #1267
	ActRenFolDocDeg,	// #1268
	ActRcvFilDocDegDZ,	// #1269
	ActRcvFilDocDegCla,	// #1270
	ActExpAdmDocDeg,	// #1271
	ActConAdmDocDeg,	// #1272
	ActZIPAdmDocDeg,	// #1273
	ActShoDocDeg,		// #1274
	ActHidDocDeg,		// #1275
	ActReqDatAdmDocDeg,	// #1276
	ActChgDatAdmDocDeg,	// #1277
	ActDowAdmDocDeg,	// #1278
	ActChgToSeeDocCtr,	// #1279
	ActSeeDocCtr,		// #1280
	ActExpSeeDocCtr,	// #1281
	ActConSeeDocCtr,	// #1282
	ActZIPSeeDocCtr,	// #1283
	ActReqDatSeeDocCtr,	// #1284
	ActDowSeeDocCtr,	// #1285
	ActChgToAdmDocCtr,	// #1286
	ActAdmDocCtr,		// #1287
	ActReqRemFilDocCtr,	// #1288
	ActRemFilDocCtr,	// #1289
	ActRemFolDocCtr,	// #1290
	ActCopDocCtr,		// #1291
	ActPasDocCtr,		// #1292
	ActRemTreDocCtr,	// #1293
	ActFrmCreDocCtr,	// #1294
	ActCreFolDocCtr,	// #1295
	ActCreLnkDocCtr,	// #1296
	ActRenFolDocCtr,	// #1297
	ActRcvFilDocCtrDZ,	// #1298
	ActRcvFilDocCtrCla,	// #1299
	ActExpAdmDocCtr,	// #1300
	ActConAdmDocCtr,	// #1301
	ActZIPAdmDocCtr,	// #1302
	ActShoDocCtr,		// #1303
	ActHidDocCtr,		// #1304
	ActReqDatAdmDocCtr,	// #1305
	ActChgDatAdmDocCtr,	// #1306
	ActDowAdmDocCtr,	// #1307
	ActChgToSeeDocIns,	// #1308
	ActSeeDocIns,		// #1309
	ActExpSeeDocIns,	// #1310
	ActConSeeDocIns,	// #1311
	ActZIPSeeDocIns,	// #1312
	ActReqDatSeeDocIns,	// #1313
	ActDowSeeDocIns,	// #1314
	ActChgToAdmDocIns,	// #1315
	ActAdmDocIns,		// #1316
	ActReqRemFilDocIns,	// #1317
	ActRemFilDocIns,	// #1318
	ActRemFolDocIns,	// #1319
	ActCopDocIns,		// #1320
	ActPasDocIns,		// #1321
	ActRemTreDocIns,	// #1322
	ActFrmCreDocIns,	// #1323
	ActCreFolDocIns,	// #1324
	ActCreLnkDocIns,	// #1325
	ActRenFolDocIns,	// #1326
	ActRcvFilDocInsDZ,	// #1327
	ActRcvFilDocInsCla,	// #1328
	ActExpAdmDocIns,	// #1329
	ActConAdmDocIns,	// #1330
	ActZIPAdmDocIns,	// #1331
	ActShoDocIns,		// #1332
	ActHidDocIns,		// #1333
	ActReqDatAdmDocIns,	// #1334
	ActChgDatAdmDocIns,	// #1335
	ActDowAdmDocIns,	// #1336
	ActNewAdmIns,		// #1337
	ActRemAdmIns,		// #1338
	ActNewAdmCtr,		// #1339
	ActRemAdmCtr,		// #1340
	ActRemInsLog,		// #1341
	ActRemCtrLog,		// #1342
	ActRemDegLog,		// #1343
	ActAdmShaDeg,		// #1344
	ActReqRemFilShaDeg,	// #1345
	ActRemFilShaDeg,	// #1346
	ActRemFolShaDeg,	// #1347
	ActCopShaDeg,		// #1348
	ActPasShaDeg,		// #1349
	ActRemTreShaDeg,	// #1350
	ActFrmCreShaDeg,	// #1351
	ActCreFolShaDeg,	// #1352
	ActCreLnkShaDeg,	// #1353
	ActRenFolShaDeg,	// #1354
	ActRcvFilShaDegDZ,	// #1355
	ActRcvFilShaDegCla,	// #1356
	ActExpShaDeg,		// #1357
	ActConShaDeg,		// #1358
	ActZIPShaDeg,		// #1359
	ActReqDatShaDeg,	// #1360
	ActChgDatShaDeg,	// #1361
	ActDowShaDeg,		// #1362
	ActAdmShaCtr,		// #1363
	ActReqRemFilShaCtr,	// #1364
	ActRemFilShaCtr,	// #1365
	ActRemFolShaCtr,	// #1366
	ActCopShaCtr,		// #1367
	ActPasShaCtr,		// #1368
	ActRemTreShaCtr,	// #1369
	ActFrmCreShaCtr,	// #1370
	ActCreFolShaCtr,	// #1371
	ActCreLnkShaCtr,	// #1372
	ActRenFolShaCtr,	// #1373
	ActRcvFilShaCtrDZ,	// #1374
	ActRcvFilShaCtrCla,	// #1375
	ActExpShaCtr,		// #1376
	ActConShaCtr,		// #1377
	ActZIPShaCtr,		// #1378
	ActReqDatShaCtr,	// #1379
	ActChgDatShaCtr,	// #1380
	ActDowShaCtr,		// #1381
	ActAdmShaIns,		// #1382
	ActReqRemFilShaIns,	// #1383
	ActRemFilShaIns,	// #1384
	ActRemFolShaIns,	// #1385
	ActCopShaIns,		// #1386
	ActPasShaIns,		// #1387
	ActRemTreShaIns,	// #1388
	ActFrmCreShaIns,	// #1389
	ActCreFolShaIns,	// #1390
	ActCreLnkShaIns,	// #1391
	ActRenFolShaIns,	// #1392
	ActRcvFilShaInsDZ,	// #1393
	ActRcvFilShaInsCla,	// #1394
	ActExpShaIns,		// #1395
	ActConShaIns,		// #1396
	ActZIPShaIns,		// #1397
	ActReqDatShaIns,	// #1398
	ActChgDatShaIns,	// #1399
	ActDowShaIns,		// #1400
	ActReqOthPubPrf,	// #1401
	ActSeeOthPubPrf,	// #1402
	-1,			// #1403 (obsolete action)
	ActChgBasPriPrf,	// #1404
	ActCalFig,		// #1405
	-1,			// #1406 (obsolete action)
	-1,			// #1407 (obsolete action)
	-1,			// #1408 (obsolete action)
	-1,			// #1409 (obsolete action)
	ActFolUsr,		// #1410
	ActUnfUsr,		// #1411
	ActSeeFlg,		// #1412
	ActSeeFlr,		// #1413
	ActReqMdfOneOth,	// #1414
	ActReqMdfOneStd,	// #1415
	ActReqMdfOneTch,	// #1416
	-1,			// #1417 (obsolete action)
	ActReqMdfOth,		// #1418
	ActReqMdfStd,		// #1419
	ActReqMdfTch,		// #1420
	-1,			// #1421 (obsolete action)
	ActUpdOth,		// #1422
	ActUpdStd,		// #1423
	ActUpdTch,		// #1424
	-1,			// #1425 (obsolete action)
	ActReqEnrSevStd,	// #1426
	ActReqEnrSevTch,	// #1427
	ActRcvFrmEnrSevStd,	// #1428
	ActRcvFrmEnrSevTch,	// #1429
	ActReqRemMyAcc,		// #1430
	ActRemMyAcc,		// #1431
	ActReqOthPho,		// #1432
	ActReqStdPho,		// #1433
	ActReqTchPho,		// #1434
	ActDetOthPho,		// #1435
	ActDetStdPho,		// #1436
	ActDetTchPho,		// #1437
	ActUpdOthPho,		// #1438
	ActUpdStdPho,		// #1439
	ActUpdTchPho,		// #1440
	ActRemOthPho,		// #1441
	ActRemStdPho,		// #1442
	ActRemTchPho,		// #1443
	ActCreOth,		// #1444
	ActCreStd,		// #1445
	ActCreTch,		// #1446
	-1,			// #1447 (obsolete action)
	-1,			// #1448 (obsolete action)
	-1,			// #1449 (obsolete action)
	ActRemID_Oth,		// #1450
	ActRemID_Std,		// #1451
	ActRemID_Tch,		// #1452
	ActNewID_Oth,		// #1453
	ActNewID_Std,		// #1454
	ActNewID_Tch,		// #1455
	ActReqAccEnrStd,	// #1456
	ActReqAccEnrTch,	// #1457
	ActAccEnrStd,		// #1458
	ActAccEnrTch,		// #1459
	ActRemMe_Std,		// #1460
	ActRemMe_Tch,		// #1461
	ActRemStdCrs,		// #1462
	ActRemTchCrs,		// #1463
	-1,			// #1464 (obsolete action)
	-1,			// #1465 (obsolete action)
	-1,			// #1466 (obsolete action)
	ActChgPwdOth,		// #1467
	ActChgPwdStd,		// #1468
	ActChgPwdTch,		// #1469
	ActHidAnn,		// #1470
	ActRevAnn,		// #1471
	ActReqRemNot,		// #1472
	ActSeeLstMyAtt,		// #1473
	ActPrnLstMyAtt,		// #1474
	-1,			// #1475 (obsolete action)
	-1,			// #1476 (obsolete action)
	-1,			// #1477 (obsolete action)
	ActRemMaiOth,		// #1478
	ActRemMaiStd,		// #1479
	ActRemMaiTch,		// #1480
	ActNewMaiOth,		// #1481
	ActNewMaiStd,		// #1482
	ActNewMaiTch,		// #1483
	ActChg1stDay,		// #1484
	-1,			// #1485 (obsolete action)
	ActChgCrsTT1stDay,	// #1486
	ActChgMyTT1stDay,	// #1487
	ActReqRemOldBrf,	// #1488
	ActRemOldBrf,		// #1489
	ActSeeSocTmlGbl,	// #1490
	-1,			// #1491 (obsolete action)
	ActRcvSocPstGbl,	// #1492
	ActRemSocPubGbl,	// #1493
	ActReqRemSocPubGbl,	// #1494
	ActShaSocNotGbl,	// #1495
	ActUnsSocNotGbl,	// #1496
	-1,			// #1497 (obsolete action)
	ActRcvSocPstUsr,	// #1498
	ActShaSocNotUsr,	// #1499
	ActUnsSocNotUsr,	// #1500
	ActReqRemSocPubUsr,	// #1501
	ActRemSocPubUsr,	// #1502
	ActRcvSocComGbl,	// #1503
	ActRcvSocComUsr,	// #1504
	ActReqRemSocComGbl,	// #1505
	ActReqRemSocComUsr,	// #1506
	ActRemSocComGbl,	// #1507
	ActRemSocComUsr,	// #1508
	ActRefNewSocPubGbl,	// #1509
	ActRefOldSocPubGbl,	// #1510
	ActRefOldSocPubUsr,	// #1511
	ActFavSocNotGbl,	// #1512
	ActUnfSocNotGbl,	// #1513
	ActFavSocNotUsr,	// #1514
	ActUnfSocNotUsr,	// #1515
	ActFavSocComGbl,	// #1516
	ActUnfSocComGbl,	// #1517
	ActFavSocComUsr,	// #1518
	ActUnfSocComUsr,	// #1519
	ActSeeSocPrf,		// #1520
	ActFrmLogIn,		// #1521
	ActUpdSignUpReq,	// #1522
	ActReqRemTstQst,	// #1523
	ActReqRemSvyQst,	// #1524
	ActAdmTchCrsGrp,	// #1525
	ActChgToAdmTch,		// #1526
	ActAdmTchCrs,		// #1527
	ActReqRemFilTchCrs,	// #1528
	ActRemFilTchCrs,	// #1529
	ActRemFolTchCrs,	// #1530
	ActCopTchCrs,		// #1531
	ActPasTchCrs,		// #1532
	ActRemTreTchCrs,	// #1533
	ActFrmCreTchCrs,	// #1534
	ActCreFolTchCrs,	// #1535
	ActCreLnkTchCrs,	// #1536
	ActRenFolTchCrs,	// #1537
	ActRcvFilTchCrsDZ,	// #1538
	ActRcvFilTchCrsCla,	// #1539
	ActExpTchCrs,		// #1540
	ActConTchCrs,		// #1541
	ActZIPTchCrs,		// #1542
	ActReqDatTchCrs,	// #1543
	ActChgDatTchCrs,	// #1544
	ActDowTchCrs,		// #1545
	ActAdmTchGrp,		// #1546
	ActReqRemFilTchGrp,	// #1547
	ActRemFilTchGrp,	// #1548
	ActRemFolTchGrp,	// #1549
	ActCopTchGrp,		// #1550
	ActPasTchGrp,		// #1551
	ActRemTreTchGrp,	// #1552
	ActFrmCreTchGrp,	// #1553
	ActCreFolTchGrp,	// #1554
	ActCreLnkTchGrp,	// #1555
	ActRenFolTchGrp,	// #1556
	ActRcvFilTchGrpDZ,	// #1557
	ActRcvFilTchGrpCla,	// #1558
	ActExpTchGrp,		// #1559
	ActConTchGrp,		// #1560
	ActZIPTchGrp,		// #1561
	ActReqDatTchGrp,	// #1562
	ActChgDatTchGrp,	// #1563
	ActDowTchGrp,		// #1564
	-1,			// #1565 (obsolete action)
	-1,			// #1566 (obsolete action)
	-1,			// #1567 (obsolete action)
	ActCnfID_Oth,		// #1568
	ActCnfID_Std,		// #1569
	ActCnfID_Tch,		// #1570
	ActSeeDatExaAnn,	// #1571
	ActSeeOneExaAnn,	// #1572
	ActChgCrsYeaCfg,	// #1573
	ActReqRemOthPho,	// #1574
	ActReqRemStdPho,	// #1575
	ActReqRemTchPho,	// #1576
	ActReqRemMyPho,		// #1577
	ActLstDupUsr,		// #1578
	ActLstSimUsr,		// #1579
	ActRemDupUsr,		// #1580
	-1,			// #1581 (obsolete action)
	ActSeeMyUsgRep,		// #1582
	-1,			// #1583 (obsolete action)
	ActChkUsrAcc,		// #1584
	ActLogInNew,		// #1585
	ActReqMyUsgRep,		// #1586
	ActChgCrsDegCfg,	// #1587
	ActChgDegCtrCfg,	// #1588
	ActChgCtrInsCfg,	// #1589
	ActChgInsCtyCfg,	// #1590
	ActRenInsFulCfg,	// #1591
	ActRenInsShoCfg,	// #1592
	ActChgInsWWWCfg,	// #1593
	ActRenCtrFulCfg,	// #1594
	ActRenCtrShoCfg,	// #1595
	ActChgCtrWWWCfg,	// #1596
	ActRenDegFulCfg,	// #1597
	ActRenDegShoCfg,	// #1598
	ActChgDegWWWCfg,	// #1599
	ActRenCrsFulCfg,	// #1600
	ActRenCrsShoCfg,	// #1601
	ActSeeMyAgd,		// #1602
	ActFrmNewEvtMyAgd,	// #1603
	ActEdiOneEvtMyAgd,	// #1604
	ActNewEvtMyAgd,		// #1605
	ActChgEvtMyAgd,		// #1606
	ActReqRemEvtMyAgd,	// #1607
	ActRemEvtMyAgd,		// #1608
	ActPrvEvtMyAgd,		// #1609
	ActPubEvtMyAgd,		// #1610
	ActSeeUsrAgd,		// #1611
	ActHidEvtMyAgd,		// #1612
	ActShoEvtMyAgd,		// #1613
	ActFrmLogInUsrAgd,	// #1614
	ActLogInUsrAgd,		// #1615
	ActLogInUsrAgdLan,	// #1616
	-1,			// #1617 (obsolete action)
	ActPrnAgdQR,		// #1618
	ActReqRemExaAnn,	// #1619
	ActHidExaAnn,		// #1620
	ActShoExaAnn,		// #1621
	ActSeeCal,		// #1622
	ActPrnCal,		// #1623
	ActChgCal1stDay,	// #1624
	-1,			// #1625 (obsolete action)
	-1,			// #1626 (obsolete action)
	-1,			// #1627 (obsolete action)
	-1,			// #1628 (obsolete action)
	-1,			// #1629 (obsolete action)
	-1,			// #1630 (obsolete action)
	-1,			// #1631 (obsolete action)
	-1,			// #1632 (obsolete action)
	-1,			// #1633 (obsolete action)
	-1,			// #1634 (obsolete action)
	-1,			// #1635 (obsolete action)
	-1,			// #1636 (obsolete action)
	ActPrnOneAsg,		// #1637
	ActChgDatFmt,		// #1638
	ActReqAccEnrNET,	// #1639
	ActAccEnrNET,		// #1640
	ActRemMe_NET,		// #1641
	ActReqEnrSevNET,	// #1642
	ActRcvFrmEnrSevNET,	// #1643
	ActReqMdfNET,		// #1644
	ActCreNET,		// #1645
	ActUpdNET,		// #1646
	ActRemNETCrs,		// #1647
	ActChgCtrPlcCfg,	// #1648
	ActSeeAllGam,		// #1649
	ActSeeGam,		// #1650
	ActAnsMchQstStd,	// #1651
	ActFrmNewGam,		// #1652
	ActEdiOneGam,		// #1653
	ActNewGam,		// #1654
	ActChgGam,		// #1655
	ActReqRemGam,		// #1656
	ActRemGam,		// #1657
	-1,			// #1658 (obsolete action)
	-1,			// #1659 (obsolete action)
	ActHidGam,		// #1660
	ActShoGam,		// #1661
	ActAddOneGamQst,	// #1662
	-1,			// #1663 (obsolete action)
	ActReqRemGamQst,	// #1664
	ActRemGamQst,		// #1665
	ActGamLstTstQst,	// #1666
	ActAddTstQstToGam,	// #1667
	ActUp_GamQst,		// #1668
	ActDwnGamQst,		// #1669
	ActReqNewMch,		// #1670
	ActNewMch,		// #1671
	ActFwdMch,		// #1672
	-1,			// #1673 (obsolete action)
	ActSeePrj,		// #1674
	ActFrmNewPrj,		// #1675
	ActEdiOnePrj,		// #1676
	ActPrnOnePrj,		// #1677
	ActNewPrj,		// #1678
	ActChgPrj,		// #1679
	ActReqRemPrj,		// #1680
	ActRemPrj,		// #1681
	ActHidPrj,		// #1682
	ActShoPrj,		// #1683
	ActReqAddStdPrj,	// #1684
	ActReqAddTutPrj,	// #1685
	ActReqAddEvlPrj,	// #1686
	ActAddStdPrj,		// #1687
	ActAddTutPrj,		// #1688
	ActAddEvlPrj,		// #1689
	ActReqRemStdPrj,	// #1690
	ActReqRemTutPrj,	// #1691
	ActReqRemEvlPrj,	// #1692
	ActRemStdPrj,		// #1693
	ActRemTutPrj,		// #1694
	ActRemEvlPrj,		// #1695
	ActSeeTblAllPrj,	// #1696
	ActAdmDocPrj,		// #1697
	ActReqRemFilDocPrj,	// #1698
	ActRemFilDocPrj,	// #1699
	ActRemFolDocPrj,	// #1700
	ActCopDocPrj,		// #1701
	ActPasDocPrj,		// #1702
	ActRemTreDocPrj,	// #1703
	ActFrmCreDocPrj,	// #1704
	ActCreFolDocPrj,	// #1705
	ActCreLnkDocPrj,	// #1706
	ActRenFolDocPrj,	// #1707
	ActRcvFilDocPrjDZ,	// #1708
	ActRcvFilDocPrjCla,	// #1709
	ActExpDocPrj,		// #1710
	ActConDocPrj,		// #1711
	ActZIPDocPrj,		// #1712
	ActReqDatDocPrj,	// #1713
	ActChgDatDocPrj,	// #1714
	ActDowDocPrj,		// #1715
	ActAdmAssPrj,		// #1716
	ActReqRemFilAssPrj,	// #1717
	ActRemFilAssPrj,	// #1718
	ActRemFolAssPrj,	// #1719
	ActCopAssPrj,		// #1720
	ActPasAssPrj,		// #1721
	ActRemTreAssPrj,	// #1722
	ActFrmCreAssPrj,	// #1723
	ActCreFolAssPrj,	// #1724
	ActCreLnkAssPrj,	// #1725
	ActRenFolAssPrj,	// #1726
	ActRcvFilAssPrjDZ,	// #1727
	ActRcvFilAssPrjCla,	// #1728
	ActExpAssPrj,		// #1729
	ActConAssPrj,		// #1730
	ActZIPAssPrj,		// #1731
	ActReqDatAssPrj,	// #1732
	ActChgDatAssPrj,	// #1733
	ActDowAssPrj,		// #1734
	ActFrmAccOth,		// #1735
	ActFrmAccStd,		// #1736
	ActFrmAccTch,		// #1737
	ActRemOldNicOth,	// #1738
	ActRemOldNicStd,	// #1739
	ActRemOldNicTch,	// #1740
	ActChgNicOth,		// #1741
	ActChgNicStd,		// #1742
	ActChgNicTch,		// #1743
	ActSeeCla,		// #1744
	ActEdiCla,		// #1745
	ActNewCla,		// #1746
	ActRemCla,		// #1747
	ActRenClaSho,		// #1748
	ActRenClaFul,		// #1749
	ActChgClaMaxStd,	// #1750
	ActRenClaLoc,		// #1751
	ActChgGrpCla,		// #1752
	ActDoActOnSevGst,	// #1753
	ActDoActOnSevStd,	// #1754
	ActDoActOnSevTch,	// #1755
	ActReqFolSevStd,	// #1756
	ActReqFolSevTch,	// #1757
	ActReqUnfSevStd,	// #1758
	ActReqUnfSevTch,	// #1759
	ActFolSevStd,		// #1760
	ActFolSevTch,		// #1761
	ActUnfSevStd,		// #1762
	ActUnfSevTch,		// #1763
	ActChgCooPrf,		// #1764
	ActChgExtPriPrf,	// #1765
        ActAllShaSocNotGbl,	// #1766
        ActAllFavSocNotGbl,	// #1767
        ActAllFavSocComGbl,	// #1768
        ActAllShaSocNotUsr,	// #1769
        ActAllFavSocNotUsr,	// #1770
        ActAllFavSocComUsr,	// #1771
	ActReqMaiUsr,		// #1772
	ActLckPrj,		// #1773
	ActUnlPrj,		// #1774
	ActReqLckAllPrj,	// #1775
	ActReqUnlAllPrj,	// #1776
	ActLckAllPrj,		// #1777
	ActUnlAllPrj,		// #1778
	-1,			// #1779 (obsolete action)
	ActJoiMch,		// #1780
	-1,			// #1781 (obsolete action)
	ActRefMchStd,		// #1782
        ActReqRemMch,		// #1783
        ActRemMch,		// #1784
	ActResMch,		// #1785
	-1,			// #1786 (obsolete action)
	ActCreMyAcc,		// #1787
	ActRefMchTch,		// #1788
	ActPlyPauMch,		// #1789
	ActBckMch,		// #1790
	-1,			// #1791 (obsolete action)
	-1,			// #1792 (obsolete action)
	-1,			// #1793 (obsolete action)
	ActChgVisResMchQst,	// #1794
	-1,			// #1795
	ActSeeMyMchResCrs,	// #1796
	ActSeeOneMchResMe,	// #1797
	ActReqSeeAllMchRes,	// #1798
	ActSeeAllMchResCrs,	// #1799
	ActSeeOneMchResOth,	// #1800
	ActChgVisResMchUsr,	// #1801
	ActChgNumColMch,	// #1802
	ActCfgPrj,		// #1803
	ActRcvCfgPrj,		// #1804
	ActReqUsrPrj,		// #1805
	ActShoHidSocComGbl,	// #1806
	ActShoHidSocComUsr,	// #1807
        ActSeeMchAnsQstStd,	// #1808
        ActRemMchAnsQstStd,	// #1809
        ActSeeMyMchResGam,	// #1810
        ActSeeAllMchResGam,	// #1811
        ActSeeMyMchResMch,	// #1812
        ActSeeAllMchResMch,	// #1813
	ActMchCntDwn,		// #1814
	ActChgCtrLatCfg,	// #1815
	ActChgCtrLgtCfg,	// #1816
	ActChgCtrAltCfg,	// #1817
	};

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************** Get action from permanent action code ********************/
/*****************************************************************************/

Act_Action_t Act_GetActionFromActCod (long ActCod)
  {
   if (ActCod >= 0 && ActCod <= Act_MAX_ACTION_COD)
      return Act_FromActCodToAction[ActCod];

   return ActUnk;
  }

/*****************************************************************************/
/****************** Get permanent action code from action ********************/
/*****************************************************************************/

long Act_GetActCod (Act_Action_t Action)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return -1L;

   return Act_Actions[Action].ActCod;
  }

/*****************************************************************************/
/***************** Get index in menu associated to an action ******************/
/*****************************************************************************/

signed int Act_GetIndexInMenu (Act_Action_t Action)
  {
   return Act_Actions[Act_GetSuperAction (Action)].IndexInMenu;
  }

/*****************************************************************************/
/********************* Get tab associated to an action ***********************/
/*****************************************************************************/

Tab_Tab_t Act_GetTab (Act_Action_t Action)
  {
   return Act_Actions[Act_GetSuperAction (Action)].Tab;
  }

/*****************************************************************************/
/***************** Get superaction associated to an action *******************/
/*****************************************************************************/

Act_Action_t Act_GetSuperAction (Act_Action_t Action)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return ActUnk;

   return Act_Actions[Action].SuperAction;
  }

/*****************************************************************************/
/************* Check if I have permission to execute an action ***************/
/*****************************************************************************/

bool Act_CheckIfIHavePermissionToExecuteAction (Act_Action_t Action)
  {
   unsigned Permission;

   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return false;

   switch (Gbl.Hierarchy.Level)
     {
      case Hie_SYS:	// System
         Permission = Act_Actions[Action].PermissionSys;
	 break;
      case Hie_CTY:	// Country selected
         Permission = Act_Actions[Action].PermissionCty;
	 break;
      case Hie_INS:	// Institution selected
         Permission = Act_Actions[Action].PermissionIns;
	 break;
      case Hie_CTR:	// Centre selected
         Permission = Act_Actions[Action].PermissionCtr;
	 break;
      case Hie_DEG:	// Degree selected
         Permission = Act_Actions[Action].PermissionDeg;
	 break;
      case Hie_CRS:	// Course selected
	 Permission = Gbl.Usrs.Me.IBelongToCurrentCrs ? Act_Actions[Action].PermissionCrsIfIBelong :
							Act_Actions[Action].PermissionCrsIfIDontBelong;
	 break;
      default:
	 return false;
     }

   return (bool) (Permission & (1 << Gbl.Usrs.Me.Role.Logged));
  }

/*****************************************************************************/
/***************** Get content type associated to an action ******************/
/*****************************************************************************/

Act_Content_t Act_GetContentType (Act_Action_t Action)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return Act_CONT_NORM;

   return Act_Actions[Action].ContentType;
  }

/*****************************************************************************/
/****************** Get browser tab associated to an action ******************/
/*****************************************************************************/

Act_BrowserTab_t Act_GetBrowserTab (Act_Action_t Action)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return Act_BRW_UNK_TAB;

   return Act_Actions[Action].BrowserTab;
  }

/*****************************************************************************/
/********* Get pointer to function a priori associated to an action **********/
/*****************************************************************************/

void (*Act_GetFunctionPriori (Act_Action_t Action)) (void)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return NULL;

   return Act_Actions[Action].FunctionPriori;
  }

/*****************************************************************************/
/******* Get pointer to function a posteriori associated to an action ********/
/*****************************************************************************/

void (*Act_GetFunctionPosteriori (Act_Action_t Action)) (void)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return NULL;

   return Act_Actions[Action].FunctionPosteriori;
  }

/*****************************************************************************/
/********************** Get icon associated to an action *********************/
/*****************************************************************************/

const char *Act_GetIcon (Act_Action_t Action)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return NULL;

   return Ico_GetIcon (Act_Actions[Action].Icon);
  }

/*****************************************************************************/
/******************* Get the title associated to an action *******************/
/*****************************************************************************/

const char *Act_GetTitleAction (Act_Action_t Action)
  {
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];

   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return NULL;

   return Txt_MENU_TITLE[Act_GetTab (Action)][Act_GetIndexInMenu (Action)];
  }

/*****************************************************************************/
/****************** Get the subtitle associated to an action *****************/
/*****************************************************************************/

const char *Act_GetSubtitleAction (Act_Action_t Action)
  {
   extern const char *Txt_MENU_SUBTITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];

   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return NULL;

   return Txt_MENU_SUBTITLE[Act_GetTab (Action)][Act_GetIndexInMenu (Action)];
  }

/*****************************************************************************/
/********************* Get text for action from database *********************/
/*****************************************************************************/

char *Act_GetActionTextFromDB (long ActCod,
                               char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1])
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get test for an action from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get text for an action",
	               "SELECT Txt FROM actions"
	               " WHERE ActCod=%ld AND Language='%s'",
                       ActCod,Lan_STR_LANG_ID[Lan_LANGUAGE_ES]))	// !!! TODO: Replace Lan_LANGUAGE_ES by Gbl.Prefs.Language !!!
     {
      /***** Get text *****/
      row = mysql_fetch_row (mysql_res);
      Str_Copy (ActTxt,row[0],
                Act_MAX_BYTES_ACTION_TXT);
     }
   else	// ActCod-Language not found on database
      ActTxt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return ActTxt;
  }

/*****************************************************************************/
/***************** Adjust current action when no user's logged ***************/
/*****************************************************************************/

void Act_AdjustActionWhenNoUsrLogged (void)
  {
   static const Act_Action_t Actions[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = ActUnk, 	// Unknown
      [Hie_SYS] = ActFrmLogIn,	// System
      [Hie_CTY] = ActSeeCtyInf,	// Country
      [Hie_INS] = ActSeeInsInf,	// Institution
      [Hie_CTR] = ActSeeCtrInf,	// Centre
      [Hie_DEG] = ActSeeDegInf,	// Degree
      [Hie_CRS] = ActSeeCrsInf,	// Course
     };

   if (Gbl.Hierarchy.Level >= Hie_NUM_LEVELS)
      Gbl.Hierarchy.Level = ActUnk;

   Gbl.Action.Act = Actions[Gbl.Hierarchy.Level];
   Tab_SetCurrentTab ();
  }

/*****************************************************************************/
/****** Adjust current action if it's mandatory to fill any empty data *******/
/*****************************************************************************/

void Act_AdjustCurrentAction (void)
  {
   bool IAmATeacherInAnyCrs;
   bool JustAfterLogin = Gbl.Action.Act == ActLogIn    ||
	                 Gbl.Action.Act == ActLogInLan ||
	                 Gbl.Action.Act == ActLogInNew ||
			 Gbl.Action.Act == ActAnnSee;

   /***** Don't adjust anything when:
          - refreshing
          - web service *****/
   if (Gbl.Action.UsesAJAX ||
       Gbl.WebService.IsWebService)
      return;

   /***** Don't adjust anything when current action is not a menu option
          (except some actions just after login) *****/
   if (!JustAfterLogin &&					// Not just after login
       Gbl.Action.Act != Act_GetSuperAction (Gbl.Action.Act))	// It is not a menu option
      return;

   /***** Adjustment 1:
          -------------
          If I have no password in database,
          the only action possible
          is to show a form to change my account *****/
   if (!Gbl.Usrs.Me.UsrDat.Password[0])
     {
      Gbl.Action.Act = ActFrmMyAcc;
      Tab_SetCurrentTab ();
      return;
     }

   /***** Adjustment 2:
          -------------
          Just after login, check if my password seems good.
          If my password is not enough secure,
          the only action possible
          is to show a form to change my account *****/
   if (Gbl.Action.Act == ActLogIn)
      if (!Pwd_FastCheckIfPasswordSeemsGood (Gbl.Usrs.Me.LoginPlainPassword))
        {
         Gbl.Action.Act = ActFrmMyAcc;
         Tab_SetCurrentTab ();
         return;
        }

   /***** Adjustment 3:
          -------------
          If I have no nickname or email in database,
          the only action possible
          is to show a form to change my account *****/
   if (!Gbl.Usrs.Me.UsrDat.Nickname[0] ||
       !Gbl.Usrs.Me.UsrDat.Email[0])
     {
      Gbl.Action.Act = ActFrmMyAcc;
      Tab_SetCurrentTab ();
      return;
     }

   /***** Check if I am a teacher in any course *****/
   Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
   IAmATeacherInAnyCrs = (Gbl.Usrs.Me.UsrDat.Roles.InCrss & ((1 << Rol_NET) |	// I am a non-editing teacher...
	                                                     (1 << Rol_TCH)));	// ...or a teacher in any course

   /***** Adjustment 4:
          -------------
          If I haven't filled
             - my sex
             - my first name
             - my surname 1
             - my country
             - my institution
          or if I'm a teacher and I haven't filled
             - my centre
             - my department,
          the only action possible
          is to show a form to change my shared record card *****/
   if ( Gbl.Usrs.Me.UsrDat.Sex == Usr_SEX_UNKNOWN ||
       !Gbl.Usrs.Me.UsrDat.FirstName[0]           ||
       !Gbl.Usrs.Me.UsrDat.Surname1 [0]           ||
        Gbl.Usrs.Me.UsrDat.CtyCod <= 0            ||
	Gbl.Usrs.Me.UsrDat.InsCod < 0             ||
       (IAmATeacherInAnyCrs && (Gbl.Usrs.Me.UsrDat.Tch.CtrCod < 0 ||
                                Gbl.Usrs.Me.UsrDat.Tch.DptCod < 0)))
     {
      Gbl.Action.Act = ActReqEdiRecSha;
      Tab_SetCurrentTab ();
      return;
     }

   /***** Adjustment 5:
          -------------
          If any of my settings about privacy is unknown
          the only action possible
          is to show a form to change my settings *****/
   if (Gbl.Usrs.Me.UsrDat.PhotoVisibility == Pri_VISIBILITY_UNKNOWN ||
       Gbl.Usrs.Me.UsrDat.BaPrfVisibility == Pri_VISIBILITY_UNKNOWN ||
       Gbl.Usrs.Me.UsrDat.ExPrfVisibility == Pri_VISIBILITY_UNKNOWN)
     {
      Gbl.Action.Act = ActReqEdiSet;
      Tab_SetCurrentTab ();
      return;
     }

   /***** If I belong to current course *****/
   if (Gbl.Usrs.Me.IBelongToCurrentCrs)
     {
      /***** Adjustment 6:
             -------------
             If I have not accepted my enrolment in the current course,
             the only action possible
             is to show a form to ask for enrolment *****/
      if (!Gbl.Usrs.Me.UsrDat.Accepted)
	{
	 switch (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role)
	   {
	    case Rol_STD:
	       Gbl.Action.Act = ActReqAccEnrStd;
	       break;
	    case Rol_NET:
	       Gbl.Action.Act = ActReqAccEnrNET;
	       break;
	    case Rol_TCH:
	       Gbl.Action.Act = ActReqAccEnrTch;
	       break;
	    default:
	       break;
	   }
	 Tab_SetCurrentTab ();
	 return;
	}

      /***** Depending on the role I am logged in the current course... *****/
      switch (Gbl.Usrs.Me.Role.Logged)
        {
         case Rol_STD:
            /***** Adjustment 7:
		   -------------
		   If there are some group types
		   with available groups in which I must register,
		   the only action possible
		   is show a form to register in groups *****/
            if (JustAfterLogin)				// Only after login because the following query may be slow
	       if (Grp_GetIfAvailableGrpTyp (-1L))	// This query may be slow
		 {
		  Gbl.Action.Act = ActReqSelGrp;
		  Tab_SetCurrentTab ();
		  return;
		 }

	    /***** Adjustment 8:
		   -------------
		   If I have no photo
		   and current action is not available for unknown users,
		   then update number of clicks without photo.
		   If limit of clicks is reached,
		   the only action possible
		   is to show a form to send my photo *****/
	    if (!Gbl.Usrs.Me.MyPhotoExists)
	       if (!(Act_Actions[Gbl.Action.Act].PermissionCrsIfIBelong & (1 << Rol_UNK)))
		  if ((Gbl.Usrs.Me.NumAccWithoutPhoto =
		       Pho_UpdateMyClicksWithoutPhoto ()) > Pho_MAX_CLICKS_WITHOUT_PHOTO)
		    {
		     /* If limit of clicks has been reached,
			the only action possible is show a form to send my photo */
		     Gbl.Action.Act = ActReqMyPho;
		     Tab_SetCurrentTab ();
		     return;
		    }

            /***** Check if it is mandatory to read any information about course *****/
            if (Gbl.Action.Act == ActMnu)	// Do the following check sometimes, for example when the user changes the current tab
               Gbl.Crs.Info.ShowMsgMustBeRead = Inf_GetIfIMustReadAnyCrsInfoInThisCrs ();
            break;
         case Rol_NET:
            break;
         case Rol_TCH:
	    /***** Adjustment 9:
		   -------------
		   If current course has tests and pluggable is unknown,
		   the only action possible is configure tests *****/
	    if (Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown ())
	      {
	       Gbl.Action.Act = ActCfgTst;
	       Tab_SetCurrentTab ();
	       return;
	      }
            break;
         default:
            break;
        }
     }

   /***** Adjustment 10:
          --------------
          Just after login with all checks OK ==> go to default action *****/
   if (JustAfterLogin)
     {
      Gbl.Action.Act = Act_DEFAULT_ACTION_AFTER_LOGIN;
      Tab_SetCurrentTab ();
     }
  }

/*****************************************************************************/
/*** Write error message and exit when no permission to perform an action ****/
/*****************************************************************************/

void Lay_NoPermissionExit (void)
  {
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;

   Lay_ShowErrorAndExit (Txt_You_dont_have_permission_to_perform_this_action);
  }
