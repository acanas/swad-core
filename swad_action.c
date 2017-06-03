// swad_action.c: actions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For fprintf
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_action.h"
#include "swad_agenda.h"
#include "swad_announcement.h"
#include "swad_banner.h"
#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_country.h"
#include "swad_course.h"
#include "swad_chat.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_degree_type.h"
#include "swad_duplicate.h"
#include "swad_exam.h"
#include "swad_enrolment.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_indicator.h"
#include "swad_language.h"
#include "swad_mail.h"
#include "swad_mark.h"
#include "swad_MFU.h"
#include "swad_network.h"
#include "swad_nickname.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_photo.h"
#include "swad_preference.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_QR.h"
#include "swad_report.h"
#include "swad_role.h"
#include "swad_search.h"
#include "swad_setup.h"
#include "swad_social.h"
#include "swad_tab.h"
#include "swad_test_import.h"
#include "swad_zip.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************ Internal global variables **************************/
/*****************************************************************************/
/*
1354 actions in one CGI:
	  0. ActAll			Any action (used for statistics)
	  1. ActUnk			Unknown action
	  2. ActHom			Show home menu
	  3. ActMnu			Show menu of a tab
	  4. ActRefCon			Refresh number of notifications and connected users via AJAX
	  5. ActRefLstClk		Refresh last clicks in log via AJAX
	  6. ActRefNewSocPubGbl		Refresh recent social timeline via AJAX
	  7. ActRefOldSocPubGbl		View old social timeline with users I follow via AJAX
	  8. ActRefOldSocPubUsr		View old social timeline of a user via AJAX
	  9. ActWebSvc			Call plugin function

System:
	 10. ActSysReqSch		Request search in system tab
	 11. ActSeeCty			List countries
	 12. ActSeePen			List pending institutions, centres, degrees and courses in hierarchy
	 13. ActSeeLnk			See institutional links
	 14. ActLstPlg			List plugins
	 15. ActSetUp			Initial set up
	 16. ActSeeCalSys		Show the academic calendar

	 17. ActSysSch			Search for courses, teachers, documents...

	 18. ActEdiCty			Edit countries
	 19. ActNewCty			Request the creation of a country
	 20. ActRemCty			Remove a country
	 21. ActRenCty			Change the name of a country
	 22. ActChgCtyWWW		Change web of country

	 23. ActSeeDegTyp		List types of degrees
	 24. ActEdiDegTyp		Request edition of types of degrees
	 25. ActNewDegTyp		Request the creation of a type of degree
	 26. ActRemDegTyp		Request the removal of a type of degree
	 27. ActRenDegTyp		Request renaming of a type of degree

	 28. ActReqRemOldCrs		Request the total removal of old courses
	 29. ActRemOldCrs		Remove completely old courses

	 30. ActSeeBan			See banners
	 31. ActEdiBan			Edit banners
	 32. ActNewBan			Request the creation of a banner
	 33. ActRemBan			Request the removal of a banner
	 34. ActShoBan			Show a hidden banner
	 35. ActHidBan			Hide a visible banner
	 36. ActRenBanSho		Request the change of the short name of a banner
	 37. ActRenBanFul		Request the change of the full name of a banner
	 38. ActChgBanImg		Request the change of the image of a banner
	 39. ActChgBanWWW		Request the change of the web of a banner
	 40. ActClkBan			Go to a banner when clicked

	 41. ActEdiLnk			Edit institutional links
	 42. ActNewLnk			Request the creation of an institutional link
	 43. ActRemLnk			Request the removal of an institutional link
	 44. ActRenLnkSho		Request the change of the short name of an institutional link
	 45. ActRenLnkFul		Request the change of the full name of an institutional link
	 46. ActChgLnkWWW		Request the change of the web of an institutional link
	 47. ActEdiPlg			Edit plugins
	 48. ActNewPlg			Request the creation of a plugin
	 49. ActRemPlg			Remove a plugin
	 50. ActRenPlg			Rename a plugin
	 51. ActChgPlgDes		Change the description of a plugin
	 52. ActChgPlgLog		Change the logo of a plugin
	 53. ActChgPlgAppKey		Change the application key of a plugin
	 54. ActChgPlgURL		Change the URL of a plugin
	 55. ActChgPlgIP		Change the IP address of a plugin

	 56. ActPrnSysCrs		Show print view of the academic calendar
	 57. ActChgSysCrs1stDay		Change first day of week and show academic calendar

Country:
	 58. ActCtyReqSch		Request search in country tab

	 59. ActSeeCtyInf		Show information on the current country
	 60. ActSeeIns			List institutions
	 61. ActSeeCalCty		Show the academic calendar

	 62. ActCtySch			Search for courses, teachers, documents...

	 63. ActPrnCtyInf		Print information on the current country
	 64. ActChgCtyMapAtt		Change map attribution of country

	 65. ActEdiIns			Edit institutions
	 66. ActReqIns			Request the creation of a request for a new institution (a teacher makes the petition to an administrator)
	 67. ActNewIns			Request the creation of an institution
	 68. ActRemIns			Remove institution
	 69. ActRenInsSho		Change short name of institution
	 70. ActRenInsFul		Change full name of institution
	 71. ActChgInsWWW		Change web of institution
	 72. ActChgInsSta		Request change of status of an institution

	 73. ActPrnCtyCrs		Show print view of the academic calendar
	 74. ActChgCtyCrs1stDay		Change first day of week and show academic calendar

Institution:
	 75. ActInsReqSch		Request search in institution tab
	 76. ActSeeInsInf		Show information on the current institution
	 77. ActSeeCtr			List centres
	 78. ActSeeDpt			List departments
	 79. ActSeeCalIns		Show the academic calendar

	 80. ActInsSch			Search for courses, teachers, documents...

	 81. ActPrnInsInf		Print information on the current institution
	 82. ActChgInsCtyCfg		Change country of institution in institution configuration
	 83. ActRenInsShoCfg		Change short name of institution in institution configuration
	 84. ActRenInsFulCfg		Change full name of institution in institution configuration
	 85. ActChgInsWWWCfg		Change web of institution in institution configuration
	 86. ActReqInsLog		Show form to send the logo of the current institution
	 87. ActRecInsLog		Receive and store the logo of the current institution
	 88. ActRemInsLog		Remove the logo of the current institution

	 89. ActEdiCtr			Edit centres
	 90. ActReqCtr			Request the creation of a request for a new centre (a teacher makes the petition to an administrator)
	 91. ActNewCtr			Request the creation of a centre
	 92. ActRemCtr			Remove centre
	 93. ActChgCtrPlc		Request change of the place of a centre
	 94. ActRenCtrSho		Change short name centre
	 95. ActRenCtrFul		Change full name centre
	 96. ActChgCtrWWW		Change web of centre
	 97. ActChgCtrSta		Request change of status of a centre

	 98. ActEdiDpt			Edit departments
	 99. ActNewDpt			Request the creation of a department
	100. ActRemDpt			Remove department
	101. ActChgDptIns		Request change of the institution of a department
	102. ActRenDptSho		Change short name department
	103. ActRenDptFul		Change full name department
	104. ActChgDptWWW		Change web of department

	105. ActSeePlc			List places
	106. ActEdiPlc			Edit places
	107. ActNewPlc			Request the creation of a place
	108. ActRemPlc			Remove a place
	109. ActRenPlcSho		Change short name of a place
	110. ActRenPlcFul		Change full name of a place

	111. ActSeeHld			List holidays
	112. ActEdiHld			Edit holidays
	113. ActNewHld			Request the creation of a holiday
	114. ActRemHld			Remove a holiday
	115. ActChgHldPlc		Change place of a holiday
	116. ActChgHldTyp		Change type of a holiday
	117. ActChgHldStrDat		Change start date of a holiday
	118. ActChgHldEndDat		Change end date of a holiday
	119. ActRenHld			Change name of a holiday

	120. ActPrnInsCrs		Show print view of the academic calendar
	121. ActChgInsCrs1stDay		Change first day of week and show academic calendar

Centre:
	122. ActCtrReqSch		Request search in centre tab
	123. ActSeeCtrInf		Show information on the current centre
	124. ActSeeDeg			List degrees of a type
	125. ActSeeCalCtr		Show the academic calendar

	126. ActCtrSch			Search for courses, teachers, documents...

	127. ActPrnCtrInf		Print information on the current centre
	128. ActChgCtrInsCfg		Request change of the institution of a centre in centre configuration
	129. ActRenCtrShoCfg		Change short name centre in centre configuration
	130. ActRenCtrFulCfg		Change full name centre in centre configuration
	131. ActChgCtrWWWCfg		Change web of centre in centre configuration
	132. ActReqCtrLog		Show form to send the logo of the current centre
	133. ActRecCtrLog		Receive and store the logo of the current centre
	134. ActRemCtrLog		Remove the logo of the current centre
	135. ActReqCtrPho		Show form to send the photo of the current centre
	136. ActRecCtrPho		Receive and store the photo of the current centre
	137. ActChgCtrPhoAtt		Change attribution of centre photo

	138. ActEdiDeg			Request edition of degrees of a type
	139. ActReqDeg			Request the creation of a request for a new degree (a teacher makes the petition to an administrator)
	140. ActNewDeg			Request the creation of a degree
	141. ActRemDeg			Request the removal of a degree
	142. ActRenDegSho		Request change of the short name of a degree
	143. ActRenDegFul		Request change of the full name of a degree
	144. ActChgDegTyp		Request change of the type of a degree
	145. ActChgDegWWW		Request change of the web of a degree
	146. ActChgDegSta		Request change of status of a degree

	147. ActPrnCtrCrs		Show print view of the academic calendar
	148. ActChgCtrCrs1stDay		Change first day of week and show academic calendar

Degree:
	149. ActDegReqSch		Request search in degree tab
	150. ActSeeDegInf		Show information on the current degree
	151. ActSeeCrs			List courses of a degree
	152. ActSeeCalDeg		Show the academic calendar

	153. ActDegSch			Search for courses, teachers, documents...

	154. ActPrnDegInf		Print information on the current degree
	155. ActChgDegCtrCfg		Request change of the centre of a degree in degree configuration
	156. ActRenDegShoCfg		Request change of the short name of a degree in degree configuration
	157. ActRenDegFulCfg		Request change of the full name of a degree in degree configuration
	158. ActChgDegWWWCfg		Request change of the web of a degree in degree configuration
	159. ActReqDegLog		Show form to send the logo of the current degree
	160. ActRecDegLog		Receive and store the logo of the current degree
	161. ActRemDegLog		Remove the logo of the current degree

	162. ActEdiCrs			Request edition of courses of a degree
	163. ActReqCrs			Request the creation of a request for a new course (a teacher makes the petition to an administrator)
	164. ActNewCrs			Request the creation of a course
	165. ActRemCrs			Request the removal of a course
	166. ActChgInsCrsCod		Request change of institutional code of a course
	167. ActChgCrsYea		Request change of year of a course inside of its degree
	168. ActRenCrsSho		Request change of short name of a course
	169. ActRenCrsFul		Request change of full name of a course
	170. ActChgCrsSta		Request change of status of a course

	171. ActPrnDegCrs		Show print view of the academic calendar
	172. ActChgDegCrs1stDay		Change first day of week and show academic calendar

Course:
	173. ActCrsReqSch		Request search in course tab

	174. ActSeeCrsInf		Show information on the current course
	175. ActSeeTchGui		Show teaching guide of the course
	176. ActSeeSyl			Show syllabus (lectures or practicals)
	177. ActSeeSylLec		Show the syllabus of lectures
	178. ActSeeSylPra		Show the syllabus of practicals
	179. ActSeeBib			Show the bibliography
	180. ActSeeFAQ			Show the FAQ
	181. ActSeeCrsLnk		Show links related to the course
	182. ActSeeCrsTT		Show the timetable
	183. ActSeeCalCrs		Show the academic calendar of the course with exam announcements

	184. ActCrsSch			Search for courses, teachers, documents...

	185. ActPrnCrsInf		Print information on the course
	186. ActChgCrsDegCfg		Request change of degree in course configuration
	187. ActRenCrsShoCfg		Request change of short name of a course in course configuration
	188. ActRenCrsFulCfg		Request change of full name of a course in course configuration
	189. ActChgInsCrsCodCfg		Change institutional code in course configuration
	190. ActChgCrsYeaCfg		Change year/semester in course configuration
	191. ActEdiCrsInf		Edit general information about the course
	192. ActEdiTchGui		Edit teaching guide of the course
	193. ActEdiSylLec		Edit the syllabus of lectures
	194. ActEdiSylPra		Edit the syllabus of practicals
	195. ActDelItmSylLec		Remove a item from syllabus of lectures
	196. ActDelItmSylPra		Remove a item from syllabus of practicals
	197. ActUp_IteSylLec		Subir the posición of a subtree of the syllabus of lectures
	198. ActUp_IteSylPra		Subir the posición of a subtree of the syllabus of practicals
	199. ActDwnIteSylLec		Bajar the posición of a subtree of the syllabus of lectures
	200. ActDwnIteSylPra		Bajar the posición of a subtree of the syllabus of practicals
	201. ActRgtIteSylLec		Aumentar the level of a item of the syllabus of lectures
	202. ActRgtIteSylPra		Aumentar the level of a item of the syllabus of practicals
	203. ActLftIteSylLec		Disminuir the level of a item of the syllabus of lectures
	204. ActLftIteSylPra		Disminuir the level of a item of the syllabus of practicals
	205. ActInsIteSylLec		Insertar a new item in the syllabus of lectures
	206. ActInsIteSylPra		Insertar a new item in the syllabus of practicals
	207. ActModIteSylLec		Modify a item of the syllabus of lectures
	208. ActModIteSylPra		Modify a item of the syllabus of practicals

	209. ActEdiBib			Edit the bibliography
	210. ActEdiFAQ			Edit the FAQ
	211. ActEdiCrsLnk		Edit the links relacionados with the course
	212. ActChgFrcReaCrsInf		Change force students to read course info
	213. ActChgFrcReaTchGui		Change force students to read teaching guide
	214. ActChgFrcReaSylLec		Change force students to read lectures syllabus
	215. ActChgFrcReaSylPra		Change force students to read practicals syllabus
	216. ActChgFrcReaBib		Change force students to read bibliography
	217. ActChgFrcReaFAQ		Change force students to read FAQ
	218. ActChgFrcReaCrsLnk		Change force students to read links
	219. ActChgHavReaCrsInf		Change if I have read course info
	220. ActChgHavReaTchGui		Change if I have read teaching guide
	221. ActChgHavReaSylLec		Change if I have read lectures syllabus
	222. ActChgHavReaSylPra		Change if I have read practicals syllabus
	223. ActChgHavReaBib		Change if I have read bibliography
	224. ActChgHavReaFAQ		Change if I have read FAQ
	225. ActChgHavReaCrsLnk		Change if I have read links
	226. ActSelInfSrcCrsInf		Select the type of info shown in the general information about the course
	227. ActSelInfSrcTchGui		Select the type of info shown in the teaching guide
	228. ActSelInfSrcSylLec		Select the type of info shown in the lectures syllabus
	229. ActSelInfSrcSylPra		Select the type of info shown in the practicals syllabus
	230. ActSelInfSrcBib		Select the type of info shown in the bibliography
	231. ActSelInfSrcFAQ		Select the type of info shown in the FAQ
	232. ActSelInfSrcCrsLnk		Select the type of info shown in the links
	233. ActRcvURLCrsInf		Receive a link a the general information about the course
	234. ActRcvURLTchGui		Receive a link a the teaching guide
	235. ActRcvURLSylLec		Receive a link al syllabus of lectures
	236. ActRcvURLSylPra		Receive a link al syllabus of practicals
	237. ActRcvURLBib		Receive a link a bibliography
	238. ActRcvURLFAQ		Receive a link a FAQ
	239. ActRcvURLCrsLnk		Receive a link a links
	240. ActRcvPagCrsInf		Receive a page with the general information about the course
	241. ActRcvPagTchGui		Receive a page with the teaching guide
	242. ActRcvPagSylLec		Receive a page with the syllabus of lectures
	243. ActRcvPagSylPra		Receive a page with the syllabus of practicals
	244. ActRcvPagBib		Receive a page with bibliography
	245. ActRcvPagFAQ		Receive a page with FAQ
	246. ActRcvPagCrsLnk		Receive a page with links
	247. ActEditorCrsInf		Integrated editor of the general information about the course
	248. ActEditorTchGui		Integrated editor of the teaching guide
	249. ActEditorSylLec		Integrated editor of the syllabus of lectures
	250. ActEditorSylPra		Integrated editor of the syllabus of practicals
	251. ActEditorBib		Integrated editor of bibliography
	252. ActEditorFAQ		Integrated editor of FAQ
	253. ActEditorCrsLnk		Integrated editor of links
	254. ActPlaTxtEdiCrsInf		Editor of plain text of the general information about the course
	255. ActPlaTxtEdiTchGui		Editor of plain text of the teaching guide
	256. ActPlaTxtEdiSylLec		Editor of plain text of the syllabus of lectures
	257. ActPlaTxtEdiSylPra		Editor of plain text of the syllabus of practicals
	258. ActPlaTxtEdiBib		Editor of plain text of the bibliography
	259. ActPlaTxtEdiFAQ		Editor of plain text of the FAQ
	260. ActPlaTxtEdiCrsLnk		Editor of plain text of the links
	261. ActRchTxtEdiCrsInf		Editor of plain text of the general information about the course
	262. ActRchTxtEdiTchGui		Editor of plain text of the teaching guide
	263. ActRchTxtEdiSylLec		Editor of plain text of the syllabus of lectures
	264. ActRchTxtEdiSylPra		Editor of plain text of the syllabus of practicals
	265. ActRchTxtEdiBib		Editor of plain text of the bibliography
	266. ActRchTxtEdiFAQ		Editor of plain text of the FAQ
	267. ActRchTxtEdiCrsLnk		Editor of plain text of the links
	268. ActRcvPlaTxtCrsInf		Receive and change the plain text of the general information about the course
	269. ActRcvPlaTxtTchGui		Receive and change the plain text of the teaching guide
	270. ActRcvPlaTxtSylLec		Receive and change the plain text of the syllabus of lectures
	271. ActRcvPlaTxtSylPra		Receive and change the plain text of the syllabus of practicals
	272. ActRcvPlaTxtBib		Receive and change the plain text of the bibliography
	273. ActRcvPlaTxtFAQ		Receive and change the plain text of the FAQ
	274. ActRcvPlaTxtCrsLnk		Receive and change the plain text of the links
	275. ActRcvPlaTxtCrsInf		Receive and change the rich text of the general information about the course
	276. ActRcvPlaTxtTchGui		Receive and change the rich text of the teaching guide
	277. ActRcvPlaTxtSylLec		Receive and change the rich text of the syllabus of lectures
	278. ActRcvPlaTxtSylPra		Receive and change the rich text of the syllabus of practicals
	279. ActRcvPlaTxtBib		Receive and change the rich text of the bibliography
	280. ActRcvPlaTxtFAQ		Receive and change the rich text of the FAQ
	281. ActRcvPlaTxtCrsLnk		Receive and change the rich text of the links

	282. ActPrnCrsTT		Show print view of the timetable
	283. ActEdiCrsTT		Edit the timetable
	284. ActChgCrsTT		Modify the timetable of the course
        285. ActChgCrsTT1stDay		Change first day of week and show timetable of the course

	286. ActPrnCalCrs		Show print view of the academic calendar of the course
	287. ActChgCalCrs1stDay		Change first day of week and show academic calendar of the course

Assessment:
	288. ActSeeAss			Show the assessment system
	289. ActSeeAsg			Show assignments
	290. ActReqTst			Request a test of self-assesment
        291. ActSeeAllSvy		List all surveys in pages
	292. ActSeeAllExaAnn		Show the exam announcements

	293. ActEdiAss			Edit the assessment system
	294. ActChgFrcReaAss		Change force students to read assessment system
	295. ActChgHavReaAss		Change if I have read assessment system
	296. ActSelInfSrcAss		Selec. type of assessment
	297. ActRcvURLAss		Receive a link a assessment
	298. ActRcvPagAss		Receive a page with assessment
	299. ActEditorAss		Integrated editor of assessment
	300. ActPlaTxtEdiAss		Editor of plain text of assessment
	301. ActRchTxtEdiAss		Editor of rich text of assessment
	302. ActRcvPlaTxtAss		Receive and change the plain text of the assessment system
	303. ActRcvRchTxtAss		Receive and change the rich text of the assessment system
	304. ActFrmNewAsg		Form to create a new assignment
	305. ActEdiOneAsg		Edit one assignment
	306. ActPrnOneAsg		Print one assignment
	307. ActNewAsg			Create new assignment
	308. ActChgAsg			Modify data of an existing assignment
	309. ActReqRemAsg		Request the removal of an assignment
	310. ActRemAsg			Remove assignment
	311. ActHidAsg			Hide assignment
	312. ActShoAsg			Show assignment

	313. ActSeeTst			Show the seft-assessment test
	314. ActAssTst			Assess a self-assessment test
	315. ActEdiTstQst		Request the edition of self-assessment questions
	316. ActEdiOneTstQst		Edit one self-assesment test question
	317. ActReqImpTstQst		Put form to ask for an XML with test questions to import
	318. ActImpTstQst		Import test questions from XML file
	319. ActLstTstQst		List for edition several self-assessment test questions
	320. ActRcvTstQst		Receive a question of self-assessment
	321. ActShfTstQst		Change shuffle of of a question of self-assessment
	322. ActReqRemTstQst		Request removing a self-assesment test question
	323. ActRemTstQst		Remove a self-assesment test question
	324. ActCfgTst			Request renaming of tags of questions of self-assesment
	325. ActEnableTag		Enable a tag
	326. ActDisableTag		Disable a tag
	327. ActRenTag			Rename a tag
	328. ActRcvCfgTst		Receive configuration of test
	329. ActReqSeeMyTstRes		Select range of dates to see my test results
	330. ActSeeMyTstRes		Show my test results
	331. ActReqSeeUsrTstRes		Select users and range of dates to see test results
	342. ActSeeUsrTstRes		Show test results of several users
	343. ActSeeOneTstResMe		Show one test result of me as student
	344. ActSeeOneTstResOth		Show one test result of other user

        345. ActSeeOneSvy		Show one survey
        346. ActAnsSvy			Answer a survey
        347. ActFrmNewSvy		Form to create a new survey
        348. ActEdiOneSvy		Edit one survey
        349. ActNewSvy			Create new survey
        350. ActChgSvy			Modify data of an existing survey
        351. ActReqRemSvy		Request the removal of a survey
        352. ActRemSvy			Remove survey
        353. ActReqRstSvy		Request the reset of answers of a survey
        354. ActRstSvy			Reset answers of survey
        355. ActHidSvy			Hide survey
        356. ActShoSvy			Show survey
        357. ActEdiOneSvyQst		Edit a new question for a survey
        358. ActRcvSvyQst		Receive a question of a survey
        359. ActReqRemSvyQst		Request the removal of a question of a survey
        360. ActRemSvyQst		Confirm the removal of a question of a survey

	361. ActSeeOneExaAnn		Show one exam announcement
	362. ActSeeDatExaAnn		Show exam announcements of a given date
	363. ActEdiExaAnn		Edit an exam announcement
	364. ActRcvExaAnn		Receive an exam announcement
	365. ActPrnExaAnn		Show an exam announcement ready to be printed
	366. ActReqRemExaAnn		Request the removal of an exam announcement
	367. ActRemExaAnn		Remove an exam announcement
	368. ActHidExaAnn		Hide exam announcement
	369. ActShoExaAnn		Show exam announcement
Files:
	370. ActSeeAdmDocIns		Documents zone of the institution (see or admin)
	371. ActAdmShaIns		Admin the shared files zone of the institution
	372. ActSeeAdmDocCtr		Documents zone of the centre (see or admin)
	373. ActAdmShaCtr		Admin the shared files zone of the centre
	374. ActSeeAdmDocDeg		Documents zone of the degree (see or admin)
	375. ActAdmShaDeg		Admin the shared files zone of the degree
	376. ActSeeAdmDocCrsGrp		Documents zone of the course (see or admin)
	377. ActAdmTchCrsGrp		Request the administration of the files of the teachers' zone of the course or of a group
	378. ActAdmShaCrsGrp		Request the administration of the files of the shared zone of the course or of a group
        379. ActAdmAsgWrkUsr		One user sends works of the course
        380. ActReqAsgWrkCrs		A teacher requests edition of works sent to the course
	381. ActSeeAdmMrk		Marks zone (see or admin)

	382. ActChgToSeeDocIns		Change to see institution documents

	383. ActSeeDocIns		See the files of the documents zone of the institution
	384. ActExpSeeDocIns		Expand a folder in institution documents
	385. ActConSeeDocIns		Contract a folder in institution documents
	386. ActZIPSeeDocIns		Compress a folder in institution documents
	387. ActReqDatSeeDocIns		Ask for metadata of a file of the documents zone of the institution
	388. ActDowSeeDocIns		Download a file in documents zone of the institution

	389. ActChgToAdmDocIns		Change to administrate institution documents

	390. ActAdmDocIns		Admin the files of the documents zone of the institution
	391. ActReqRemFilDocIns		Request removal of a file in the documents zone of the institution
	392. ActRemFilDocIns		Remove a file in the documents zone of the institution
	393. ActRemFolDocIns		Remove a folder empty the documents zone of the institution
	394. ActCopDocIns		Set source of copy in the documents zone of the institution
	395. ActPasDocIns		Paste a folder or file in the documents zone of the institution
	396. ActRemTreDocIns		Remove a folder no empty in the documents zone of the institution
	397. ActFrmCreDocIns		Form to crear a folder or file in the documents zone of the institution
	398. ActCreFolDocIns		Create a new folder in the documents zone of the institution
	399. ActCreLnkDocIns		Create a new link in the documents zone of the institution
	400. ActRenFolDocIns		Rename a folder in the documents zone of the institution
	401. ActRcvFilDocInsDZ		Receive a file in the documents zone of the institution using Dropzone.js
	402. ActRcvFilDocInsCla		Receive a file in the documents zone of the institution using the classic way
	403. ActExpAdmDocIns		Expand a folder when administrating the documents zone of the institution
	404. ActConAdmDocIns		Contract a folder when administrating the documents zone of the institution
	405. ActZIPAdmDocIns		Compress a folder when administrating the documents zone of the institution
	406. ActShoDocIns		Show hidden folder or file of the documents zone of the institution
	407. ActHidDocIns		Hide folder or file of the documents zone of the institution
	408. ActReqDatAdmDocIns		Ask for metadata of a file of the documents zone of the institution
	409. ActChgDatAdmDocIns		Change metadata of a file of the documents zone of the institution
	410. ActDowAdmDocIns		Download a file of the documents zone of the institution

	411. ActReqRemFilShaIns		Request removal of a shared file of the institution
	412. ActRemFilShaIns		Remove a shared file of the institution
	413. ActRemFolShaIns		Remove an empty shared folder of the institution
	414. ActCopShaIns		Set source of copy in shared zone of the institution
	415. ActPasShaIns		Paste a folder or file in shared zone of the institution
	416. ActRemTreShaIns		Remove a folder no empty of shared zone of the institution
	417. ActFrmCreShaIns		Form to crear a folder or file in shared zone of the institution
	418. ActCreFolShaIns		Create a new folder in shared zone of the institution
	419. ActCreLnkShaIns		Create a new link in shared zone of the institution
	420. ActRenFolShaIns		Rename a folder in shared zone of the institution
	421. ActRcvFilShaInsDZ		Receive a file in shared zone of the institution using Dropzone.js
	422. ActRcvFilShaInsCla		Receive a file in shared zone of the institution using the classic way
	423. ActExpShaIns		Expand a folder in shared zone of the institution
	424. ActConShaIns		Contract a folder in shared zone of the institution
	425. ActZIPShaIns		Compress a folder in shared zone of the institution
	426. ActReqDatShaIns		Ask for metadata of a file of the shared zone of the institution
	427. ActChgDatShaIns		Change metadata of a file of the shared zone of the institution
	428. ActDowShaIns		Download a file of the shared zone of the institution

	429. ActChgToSeeDocCtr		Change to see centre documents

	430. ActSeeDocCtr		See the files of the documents zone of the centre
	431. ActExpSeeDocCtr		Expand a folder in centre documents
	432. ActConSeeDocCtr		Contract a folder in centre documents
	433. ActZIPSeeDocCtr		Compress a folder in centre documents
	434. ActReqDatSeeDocCtr		Ask for metadata of a file of the documents zone of the centre
	435. ActDowSeeDocCtr		Download a file in documents zone of the centre

	436. ActChgToAdmDocCtr		Change to administrate centre documents

	437. ActAdmDocCtr		Admin the files of the documents zone of the centre
	438. ActReqRemFilDocCtr		Request removal of a file in the documents zone of the centre
	439. ActRemFilDocCtr		Remove a file in the documents zone of the centre
	440. ActRemFolDocCtr		Remove a folder empty the documents zone of the centre
	441. ActCopDocCtr		Set source of copy in the documents zone of the centre
	442. ActPasDocCtr		Paste a folder or file in the documents zone of the centre
	443. ActRemTreDocCtr		Remove a folder no empty in the documents zone of the centre
	444. ActFrmCreDocCtr		Form to crear a folder or file in the documents zone of the centre
	445. ActCreFolDocCtr		Create a new folder in the documents zone of the centre
	446. ActCreLnkDocCtr		Create a new link in the documents zone of the centre
	447. ActRenFolDocCtr		Rename a folder in the documents zone of the centre
	448. ActRcvFilDocCtrDZ		Receive a file in the documents zone of the centre using Dropzone.js
	449. ActRcvFilDocCtrCla		Receive a file in the documents zone of the centre using the classic way
	450. ActExpAdmDocCtr		Expand a folder when administrating the documents zone of the centre
	451. ActConAdmDocCtr		Contract a folder when administrating the documents zone of the centre
	452. ActZIPAdmDocCtr		Compress a folder when administrating the documents zone of the centre
	453. ActShoDocCtr		Show hidden folder or file of the documents zone of the centre
	454. ActHidDocCtr		Hide folder or file of the documents zone of the centre
	455. ActReqDatAdmDocCtr		Ask for metadata of a file of the documents zone of the centre
	456. ActChgDatAdmDocCtr		Change metadata of a file of the documents zone of the centre
	457. ActDowAdmDocCtr		Download a file of the documents zone of the centre

	458. ActReqRemFilShaCtr		Request removal of a shared file of the centre
	459. ActRemFilShaCtr		Remove a shared file of the centre
	460. ActRemFolShaCtr		Remove an empty shared folder of the centre
	461. ActCopShaCtr		Set source of copy in shared zone of the centre
	462. ActPasShaCtr		Paste a folder or file in shared zone of the centre
	463. ActRemTreShaCtr		Remove a folder no empty of shared zone of the centre
	464. ActFrmCreShaCtr		Form to crear a folder or file in shared zone of the centre
	465. ActCreFolShaCtr		Create a new folder in shared zone of the centre
	466. ActCreLnkShaCtr		Create a new link in shared zone of the centre
	467. ActRenFolShaCtr		Rename a folder in shared zone of the centre
	468. ActRcvFilShaCtrDZ		Receive a file in shared zone of the centre using Dropzone.js
	469. ActRcvFilShaCtrCla		Receive a file in shared zone of the centre using the classic way
	470. ActExpShaCtr		Expand a folder in shared zone of the centre
	471. ActConShaCtr		Contract a folder in shared zone of the centre
	472. ActZIPShaCtr		Compress a folder in shared zone of the centre
	473. ActReqDatShaCtr		Ask for metadata of a file of the shared zone of the centre
	474. ActChgDatShaCtr		Change metadata of a file of the shared zone of the centre
	475. ActDowShaCtr		Download a file of the shared zone of the centre

	476. ActChgToSeeDocDeg		Change to see degree documents

	477. ActSeeDocDeg		See the files of the documents zone of the degree
	478. ActExpSeeDocDeg		Expand a folder in degree documents
	479. ActConSeeDocDeg		Contract a folder in degree documents
	480. ActZIPSeeDocDeg		Compress a folder in degree documents
	481. ActReqDatSeeDocDeg		Ask for metadata of a file of the documents zone of the degree
	482. ActDowSeeDocDeg		Download a file in documents zone of the degree

	483. ActChgToAdmDocDeg		Change to administrate degree documents

	484. ActAdmDocDeg		Admin the files of the documents zone of the degree
	485. ActReqRemFilDocDeg		Request removal of a file in the documents zone of the degree
	486. ActRemFilDocDeg		Remove a file in the documents zone of the degree
	487. ActRemFolDocDeg		Remove a folder empty the documents zone of the degree
	488. ActCopDocDeg		Set source of copy in the documents zone of the degree
	489. ActPasDocDeg		Paste a folder or file in the documents zone of the degree
	490. ActRemTreDocDeg		Remove a folder no empty in the documents zone of the degree
	491. ActFrmCreDocDeg		Form to crear a folder or file in the documents zone of the degree
	492. ActCreFolDocDeg		Create a new folder in the documents zone of the degree
	493. ActCreLnkDocDeg		Create a new link in the documents zone of the degree
	494. ActRenFolDocDeg		Rename a folder in the documents zone of the degree
	495. ActRcvFilDocDegDZ		Receive a file in the documents zone of the degree using Dropzone.js
	496. ActRcvFilDocDegCla		Receive a file in the documents zone of the degree using the classic way
	497. ActExpAdmDocDeg		Expand a folder when administrating the documents zone of the degree
	498. ActConAdmDocDeg		Contract a folder when administrating the documents zone of the degree
	499. ActZIPAdmDocDeg		Compress a folder when administrating the documents zone of the degree
	500. ActShoDocDeg		Show hidden folder or file of the documents zone of the degree
	501. ActHidDocDeg		Hide folder or file of the documents zone of the degree
	502. ActReqDatAdmDocDeg		Ask for metadata of a file of the documents zone of the degree
	503. ActChgDatAdmDocDeg		Change metadata of a file of the documents zone of the degree
	504. ActDowAdmDocDeg		Download a file of the documents zone of the degree

	505. ActReqRemFilShaDeg		Request removal of a shared file of the degree
	506. ActRemFilShaDeg		Remove a shared file of the degree
	507. ActRemFolShaDeg		Remove an empty shared folder of the degree
	508. ActCopShaDeg		Set source of copy in shared zone of the degree
	509. ActPasShaDeg		Paste a folder or file in shared zone of the degree
	510. ActRemTreShaDeg		Remove a folder no empty of shared zone of the degree
	511. ActFrmCreShaDeg		Form to crear a folder or file in shared zone of the degree
	512. ActCreFolShaDeg		Create a new folder in shared zone of the degree
	513. ActCreLnkShaDeg		Create a new link in shared zone of the degree
	514. ActRenFolShaDeg		Rename a folder in shared zone of the degree
	515. ActRcvFilShaDegDZ		Receive a file in shared zone of the degree using Dropzone.js
	516. ActRcvFilShaDegCla		Receive a file in shared zone of the degree using the classic way
	517. ActExpShaDeg		Expand a folder in shared zone of the degree
	518. ActConShaDeg		Contract a folder in shared zone of the degree
	519. ActZIPShaDeg		Compress a folder in shared zone of the degree
	520. ActReqDatShaDeg		Ask for metadata of a file of the shared zone of the degree
	521. ActChgDatShaDeg		Change metadata of a file of the shared zone of the degree
	522. ActDowShaDeg		Download a file of the shared zone of the degree

	523. ActChgToSeeDocCrs		Change to see course/group documents

	524. ActSeeDocCrs		See the files of the documents zone of the course
	525. ActExpSeeDocCrs		Expand a folder in course documents
	526. ActConSeeDocCrs		Contract a folder in course documents
	527. ActZIPSeeDocCrs		Compress a folder in course documents
	528. ActReqDatSeeDocCrs		Ask for metadata of a file of the documents zone of the course
	529. ActDowSeeDocCrs		Download a file in documents zone of the course

	530. ActSeeDocGrp		See the files of the documents zone of a group
	531. ActExpSeeDocGrp		Expand a folder in group documents
	532. ActConSeeDocGrp		Contract a folder in group documents
	533. ActZIPSeeDocGrp		Compress a folder in group documents
	534. ActReqDatSeeDocGrp		Ask for metadata of a file of the documents zone of a group
	535. ActDowSeeDocGrp		Download a file of the documents zone of a group

	536. ActChgToAdmDocCrs		Change to administrate course/group documents

	537. ActAdmDocCrs		Admin the files of the documents zone of the course
	538. ActReqRemFilDocCrs		Request removal of a file in the documents zone of the course
	539. ActRemFilDocCrs		Remove a file in the documents zone of the course
	540. ActRemFolDocCrs		Remove a folder empty the documents zone of the course
	541. ActCopDocCrs		Set source of copy in the documents zone of the course
	542. ActPasDocCrs		Paste a folder or file in the documents zone of the course
	543. ActRemTreDocCrs		Remove a folder no empty in the documents zone of the course
	544. ActFrmCreDocCrs		Form to crear a folder or file in the documents zone of the course
	545. ActCreFolDocCrs		Create a new folder in the documents zone of the course
	546. ActCreLnkDocCrs		Create a new link in the documents zone of the course
	547. ActRenFolDocCrs		Rename a folder in the documents zone of the course
	548. ActRcvFilDocCrsDZ		Receive a file in the documents zone of the course using Dropzone.js
	549. ActRcvFilDocCrsCla		Receive a file in the documents zone of the course using the classic way
	550. ActExpAdmDocCrs		Expand a folder when administrating the documents zone of the course
	551. ActConAdmDocCrs		Contract a folder when administrating the documents zone of the course
	552. ActZIPAdmDocCrs		Compress a folder when administrating the documents zone of the course
	553. ActShoDocCrs		Show hidden folder or file of the documents zone
	554. ActHidDocCrs		Hide folder or file of the documents zone
	555. ActReqDatAdmDocCrs		Ask for metadata of a file of the documents zone
	556. ActChgDatAdmDocCrs		Change metadata of a file of the documents zone
	557. ActDowAdmDocCrs		Download a file of the documents zone

	558. ActAdmDocGrp		Admin the files of the documents zone of a group
	559. ActReqRemFilDocGrp		Request removal of a file in the documents zone of a group
	560. ActRemFilDocGrp		Remove a file in the documents zone of a group
	561. ActRemFolDocGrp		Remove a folder empty in the documents zone of a group
	562. ActCopDocGrp		Set source of copy in the documents zone of a group
	563. ActPasDocGrp		Paste a folder or file in the documents zone of a group
	564. ActRemTreDocGrp		Remove a folder no empty in the documents zone of a group
	565. ActFrmCreDocGrp		Form to crear a folder or file in the documents zone of a group
	566. ActCreFolDocGrp		Create a new folder in the documents zone of a group
	567. ActCreLnkDocGrp		Create a new link in the documents zone of a group
	568. ActRenFolDocGrp		Rename a folder in the documents zone of a group
	569. ActRcvFilDocGrpDZ		Receive a file in the documents zone of a group using Dropzone.js
	570. ActRcvFilDocGrpCla		Receive a file in the documents zone of a group using the classic way
	571. ActExpAdmDocGrp		Expand a folder when administrating the documents zone of a group
	572. ActConAdmDocGrp		Contract a folder when administrating the documents zone of a group
	573. ActZIPAdmDocGrp		Compress a folder when administrating the documents zone of a group
	574. ActShoDocGrp		Show hidden folder or file of download
	575. ActHidDocGrp		Hide folder or file of download
	576. ActReqDatAdmDocGrp		Ask for metadata of a file of the documents zone of a group
	577. ActChgDatAdmDocGrp		Change metadata of a file of the documents zone of a group
	578. ActDowAdmDocGrp		Download a file of the documents zone of a group

	579. ActChgToAdmTch		Change to administrate teachers' files of the course/group

	580. ActAdmTchCrs		Admin the teachers' files zone of the course
	581. ActReqRemFilTchCrs		Request removal of a teachers' file of the course
	582. ActRemFilTchCrs		Remove a teachers' file of the course
	583. ActRemFolTchCrs		Remove an empty teachers' folder of the course
	584. ActCopTchCrs		Set source of teachers' zone of the course
	585. ActPasTchCrs		Paste a folder or file in teachers' zone of the course
	586. ActRemTreTchCrs		Remove a folder no empty of teachers' zone of the course
	587. ActFrmCreTchCrs		Form to crear a folder or file in teachers' zone of the course
	588. ActCreFolTchCrs		Create a new folder in teachers' zone of the course
	589. ActCreLnkTchCrs		Create a new link in teachers' zone of the course
	590. ActRenFolTchCrs		Rename a folder in teachers' zone of the course
	591. ActRcvFilTchCrsDZ		Receive a file in teachers' zone of the course using Dropzone.js
	592. ActRcvFilTchCrsCla		Receive a file in teachers' zone of the course using the classic way
	593. ActExpTchCrs		Expand a folder in teachers' zone of the course
	594. ActConTchCrs		Contract a folder in teachers' zone of the course
	595. ActZIPTchCrs		Compress a folder in teachers' zone of the course
	596. ActReqDatTchCrs		Ask for metadata of a file of the teachers' zone of the course
	597. ActChgDatTchCrs		Change metadata of a file of the teachers' zone of the course
	598. ActDowTchCrs		Download a file of the teachers' zone of the course

	599. ActAdmTchGrp		Admin the teachers' files zone of a group
	600. ActReqRemFilTchGrp		Request removal of a teachers' file of a group
	601. ActRemFilTchGrp		Remove a teachers' file of a group
	602. ActRemFolTchGrp		Remove an empty teachers' folder of a group
	603. ActCopTchGrp		Set source of copy in teachers' zone of a group
	604. ActPasTchGrp		Paste a folder or file in teachers' zone of a group
	605. ActRemTreTchGrp		Remove a folder no empty of teachers' zone of a group
	606. ActFrmCreTchGrp		Form to crear a folder or file in teachers' zone of a group
	607. ActCreFolTchGrp		Create a new folder in teachers' zone of a group
	608. ActCreLnkTchGrp		Create a new link in teachers' zone of a group
	609. ActRenFolTchGrp		Rename a folder in teachers' zone of a group
	610. ActRcvFilTchGrpDZ		Receive a file in teachers' zone of a group using Dropzone.js
	611. ActRcvFilTchGrpCla		Receive a file in teachers' zone of a group using the classic way
	612. ActExpTchGrp		Expand a folder in teachers' zone of a group
	613. ActConTchGrp		Contract a folder in teachers' zone of a group
	614. ActZIPTchGrp		Compress a folder in teachers' zone of a group
	615. ActReqDatTchGrp		Ask for metadata of a file of the teachers' zone of a group
	616. ActChgDatTchGrp		Change metadata of a file of the teachers' zone of a group
	617. ActDowTchGrp		Download a file of the teachers' zone of a group

	618. ActChgToAdmSha		Change to administrate shared files of the course/group

	619. ActAdmShaCrs		Admin the shared files zone of the course
	620. ActReqRemFilShaCrs		Request removal of a shared file of the course
	621. ActRemFilShaCrs		Remove a shared file of the course
	622. ActRemFolShaCrs		Remove a empty shared folder of the course
	623. ActCopShaCrs		Set source of copy in shared zone of the course
	624. ActPasShaCrs		Paste a folder or file in shared zone of the course
	625. ActRemTreShaCrs		Remove a folder no empty of shared zone of the course
	626. ActFrmCreShaCrs		Form to crear a folder or file in shared zone of the course
	627. ActCreFolShaCrs		Create a new folder in shared zone of the course
	628. ActCreLnkShaCrs		Create a new link in shared zone of the course
	629. ActRenFolShaCrs		Rename a folder in shared zone of the course
	630. ActRcvFilShaCrsDZ		Receive a file in shared zone of the course using Dropzone.js
	631. ActRcvFilShaCrsCla		Receive a file in shared zone of the course using the classic way
	632. ActExpShaCrs		Expand a folder in shared zone of the course
	633. ActConShaCrs		Contract a folder in shared zone of the course
	634. ActZIPShaCrs		Compress a folder in shared zone of the course
	635. ActReqDatShaCrs		Ask for metadata of a file of the shared zone of the course
	636. ActChgDatShaCrs		Change metadata of a file of the shared zone of the course
	637. ActDowShaCrs		Download a file of the shared zone of the course

	638. ActAdmShaGrp		Admin the shared files zone of a group
	639. ActReqRemFilShaGrp		Request removal of a shared file of a group
	640. ActRemFilShaGrp		Remove a shared file of a group
	641. ActRemFolShaGrp		Remove a folder empty común of a group
	642. ActCopShaGrp		Set source of copy in shared zone of a group
	643. ActPasShaGrp		Paste a folder or file in shared zone of a group
	644. ActRemTreShaGrp		Remove a folder no empty of shared zone of a group
	645. ActFrmCreShaGrp		Form to crear a folder or file in shared zone of a group
	646. ActCreFolShaGrp		Create a new folder in shared zone of a group
	647. ActCreLnkShaGrp		Create a new link in shared zone of a group
	648. ActRenFolShaGrp		Rename a folder in shared zone of a group
	649. ActRcvFilShaGrpDZ		Receive a file in shared zone of a group using Dropzone.js
	650. ActRcvFilShaGrpCla		Receive a file in shared zone of a group using the classic way
	651. ActExpShaGrp		Expand a folder in shared zone of a group
	652. ActConShaGrp		Contract a folder in shared zone of a group
	653. ActZIPShaGrp		Compress a folder in shared zone of a group
	654. ActReqDatShaGrp		Ask for metadata of a file of the shared zone of a group
	655. ActChgDatShaGrp		Change metadata of a file of the shared zone of a group
	656. ActDowShaGrp		Download a file of the shared zone of a group

        657. ActAdmAsgWrkCrs		Edit all the works sent to the course
	658. ActReqRemFilAsgUsr		Request removal of a file of assignments from a user
	659. ActRemFilAsgUsr		Remove a file of assignments from a user
	660. ActRemFolAsgUsr		Remove an empty folder of assignments of a user
	661. ActCopAsgUsr		Set origin of copy in assignments of a user
	662. ActPasAsgUsr		Paste a file or folder of assignments of a user
	663. ActRemTreAsgUsr		Remove a not empty folder of assignments of a user
	664. ActFrmCreAsgUsr		Form to create a new folder or file of assignments of a user
	665. ActCreFolAsgUsr		Create a new folder of assignments of a user
	666. ActCreLnkAsgUsr		Create a new link of assignments of a user
	667. ActRenFolAsgUsr		Rename a folder of assignments of a user
	668. ActRcvFilAsgUsrDZ		Receive a new file of assignments of a user using Dropzone.js
	669. ActRcvFilAsgUsrCla		Receive a new file of assignments of a user using the classic way
	670. ActExpAsgUsr		Expand a folder of assignments of a user
	671. ActConAsgUsr		Contract a folder of assignments of a user
	672. ActZIPAsgUsr		Compress a folder of assignments of a user
	673. ActReqDatAsgUsr		Ask for metadata of a file of assignments of a user
	674. ActChgDatAsgUsr		Change metadata of a file of assignments of a user
	675. ActDowAsgUsr		Download a file of assignments of a user
	676. ActReqRemFilWrkUsr		Request removal of a file of works from a user
	677. ActRemFilWrkUsr		Remove a file of works from a user
	678. ActRemFolWrkUsr		Remove an empty folder of works of a user
	679. ActCopWrkUsr		Set origin of copy in works of a user
	680. ActPasWrkUsr		Paste a file or folder of works of a user
	681. ActRemTreWrkUsr		Remove a not empty folder of works of a user
	682. ActFrmCreWrkUsr		Form to create a new folder or file of works of a user
	683. ActCreFolWrkUsr		Create a new folder of works of a user
	684. ActCreLnkWrkUsr		Create a new link of works of a user
	685. ActRenFolWrkUsr		Rename a folder of works of a user
	686. ActRcvFilWrkUsrDZ		Receive a new file of works of a user using Dropzone.js
	687. ActRcvFilWrkUsrCla		Receive a new file of works of a user using the classic way
	688. ActExpWrkUsr		Expand a folder of works of a user
	689. ActConWrkUsr		Contract a folder of works of a user
	690. ActZIPWrkUsr		Compress a folder of works of a user
	691. ActReqDatWrkUsr		Ask for metadata of a file of works of a user
	692. ActChgDatWrkUsr		Change metadata of a file of works of a user
	693. ActDowWrkUsr		Download a file of works of a user
	694. ActReqRemFilAsgCrs		Request removal of a file of assignments in the course
	695. ActRemFilAsgCrs		Remove a file of assignments in the course
	696. ActRemFolAsgCrs		Remove an empty folder of assignments in the course
	697. ActCopAsgCrs		Set origin of copy in assignments in the course
	698. ActPasAsgCrs		Paste a file or folder of assignments in the course
	699. ActRemTreAsgCrs		Remove a not empty folder of assignments in the course
	700. ActFrmCreAsgCrs		Form to create a new folder or file of assignments in the course
	701. ActCreFolAsgCrs		Create a new folder of assignments in the course
	702. ActCreLnkAsgCrs		Create a new link of assignments in the course
	703. ActRenFolAsgCrs		Rename a folder of assignments in the course
	704. ActRcvFilAsgCrsDZ		Receive a file of assignments in the course using Dropzone.js
	705. ActRcvFilAsgCrsCla		Receive a file of assignments in the course using the classic way
	706. ActExpAsgCrs		Expand a folder of assignments in a course
	707. ActConAsgCrs		Contract a folder of assignments in a course
	708. ActZIPAsgCrs		Compress a folder of assignments in a course
	709. ActReqDatAsgCrs		Ask for metadata of a file of assignments in a course
	710. ActChgDatAsgCrs		Change metadata of a file of assignments in a course
	711. ActDowAsgCrs		Download a file of assignments in a course
	712. ActReqRemFilWrkCrs		Request removal of a file of works in the course
	713. ActRemFilWrkCrs		Remove a file of works in the course
	714. ActRemFolWrkCrs		Remove an empty folder of works in the course
	715. ActCopWrkCrs		Set origin of copy in works in the course
	716. ActPasWrkCrs		Paste a file or folder of works in the course
	717. ActRemTreWrkCrs		Remove a not empty folder of works in the course
	718. ActFrmCreWrkCrs		Form to create a new folder or file of works in the course
	719. ActCreFolWrkCrs		Create a new folder of works in the course
	720. ActCreLnkWrkCrs		Create a new link of works in the course
	721. ActRenFolWrkCrs		Rename a folder of works in the course
	722. ActRcvFilWrkCrsDZ		Receive a file of works in the course using Dropzone.js
	723. ActRcvFilWrkCrsCla		Receive a file of works in the course using the classic way
	724. ActExpWrkCrs		Expand a folder of works in a course
	725. ActConWrkCrs		Contract a folder of works in a course
	726. ActZIPWrkCrs		Compress a folder of works in a course
	727. ActReqDatWrkCrs		Ask for metadata of a file of works in a course
	728. ActChgDatWrkCrs		Change metadata of a file of works in a course
	729. ActDowWrkCrs		Download a file of works in a course

	730. ActChgToSeeMrk		Change to see marks in course/group

	731. ActSeeMrkCrs		See marks in course
	732. ActExpSeeMrkCrs		Expand a folder in marks in course
	733. ActConSeeMrkCrs		Contract a folder in marks in course
	734. ActReqDatSeeMrkCrs		Ask for metadata of a file of the marks zone of the course
	735. ActSeeMyMrkCrs		Individualized access to marks in course

	736. ActSeeMrkGrp		See marks in group
	737. ActExpSeeMrkGrp		Expand a folder in marks in group
	738. ActConSeeMrkGrp		Contract a folder in marks in group
	739. ActReqDatSeeMrkGrp		Ask for metadata of a file of the marks zone of a group
	740. ActSeeMyMrkGrp		Individualized access to marks in group

	741. ActChgToAdmMrk		Change to admin marks in course/group

	742. ActAdmMrkCrs		Admin the files of the zone of marks in course
	743. ActReqRemFilMrkCrs		Request removal of a file of marks in course
	744. ActRemFilMrkCrs		Remove a file of marks in course
	745. ActRemFolMrkCrs		Remove a folder empty of marks in course
	746. ActCopMrkCrs		Set source of copy in marks in course
	747. ActPasMrkCrs		Paste a folder or file in marks in course
	748. ActRemTreMrkCrs		Remove a folder no empty of marks in course
	749. ActFrmCreMrkCrs		Form to crear a folder or file in marks in course
	750. ActCreFolMrkCrs		Create a new folder in marks in course
	751. ActRenFolMrkCrs		Rename a folder in marks in course
	752. ActRcvFilMrkCrsDZ		Receive a file of marks in course using Dropzone.js
	753. ActRcvFilMrkCrsCla		Receive a file of marks in course using the classic way
	754. ActExpAdmMrkCrs		Expand a folder in marks administration in course
	755. ActConAdmMrkCrs		Contract a folder in marks administration in course
	756. ActZIPAdmMrkCrs		Compress a folder in marks administration in course
	757. ActShoMrkCrs		Show hidden folder or file of the marks administration in course
	758. ActHidMrkCrs		Hide folder or file of the marks administration in course
	759. ActReqDatAdmMrkCrs		Ask for metadata of a file in marks administration in course
	760. ActChgDatAdmMrkCrs		Change metadata of a file in marks administration in course
	761. ActDowAdmMrkCrs		Download a file in marks administration in course
	762. ActChgNumRowHeaCrs		Change the number of rows of cabecera of a file of marks in course
	763. ActChgNumRowFooCrs		Change the number of rows of pie of a file of marks in course

	764. ActAdmMrkGrp		Admin the files of the zone of marks in group
	765. ActReqRemFilMrkGrp		Request removal of a file of marks in group
	766. ActRemFilMrkGrp		Remove a file of marks in group
	767. ActRemFolMrkGrp		Remove a folder empty of marks in group
	768. ActCopMrkGrp		Set source of copy in marks in group
	769. ActPasMrkGrp		Paste a folder or file in marks in group
	770. ActRemTreMrkGrp		Remove a folder no empty of marks in group
	771. ActFrmCreMrkGrp		Form to crear a folder or file in marks in group
	772. ActCreFolMrkGrp		Create a new folder in marks in group
	773. ActRenFolMrkGrp		Rename a folder in marks in group
	774. ActRcvFilMrkGrpDZ		Receive a file to marks in group using Dropzone.js
	775. ActRcvFilMrkGrpCla		Receive a file to marks in group using the classic way
	776. ActExpAdmMrkGrp		Expand a folder in marks administration in group
	777. ActConAdmMrkGrp		Contract a folder in marks administration in group
	778. ActZIPAdmMrkGrp		Compress a folder in marks administration in group
	779. ActShoMrkGrp		Show hidden folder or file of the marks administration in group
	780. ActHidMrkGrp		Hide folder or file of the marks administration in group
	781. ActReqDatAdmMrkGrp		Ask for metadata of a file in marks administration in group
	782. ActChgDatAdmMrkGrp		Change metadata of a file in marks administration in group
	783. ActDowAdmMrkGrp		Download a file in marks administration in group
	784. ActChgNumRowHeaGrp		Change the number of rows of cabecera of a file of marks in group
	785. ActChgNumRowFooGrp		Change the number of rows of pie of a file of marks in group

Users:
	786. ActReqSelGrp		Request the selection of groups of students
	787. ActLstStd			Show class photo or list of students
	788. ActLstTch			Show class photo or list of teachers
	789. ActLstOth			Show class photo or list of guests
	790. ActSeeAtt			Show attendance events
	791. ActReqSignUp		Apply for my enrolment
	792. ActSeeSignUpReq		Show pending requests for inscription in the current course
	793. ActLstCon			List connected users

	794. ActChgGrp			Change my groups
	795. ActReqEdiGrp		Request the edition of groups
	796. ActNewGrpTyp		Request the creation of a type of group of students
	797. ActReqRemGrpTyp		Request the removal of a type of group of students
	798. ActRemGrpTyp		Remove a type of group of students
	799. ActRenGrpTyp		Request renaming of a type of group of students
	800. ActChgMdtGrpTyp		Request change if it is mandatory to register in groups of a type
	801. ActChgMulGrpTyp		Request change if it is possible to register in multiple groups of a type
	802. ActChgTimGrpTyp		Request change when the groups of a type will be open
	803. ActNewGrp			Request the creation of a group of students
	804. ActReqRemGrp		Request the removal of a group of students
	805. ActRemGrp			Remove a group of students
	806. ActOpeGrp			Abrir a group of students
	807. ActCloGrp			Cerrar a group of students
	808. ActEnaFilZonGrp		Enable zonas of files of a group
	809. ActDisFilZonGrp		Disable zonas of files of a group
	810. ActChgGrpTyp		Request change in the type of group of students
	811. ActRenGrp			Request renaming of a group of students
	812. ActChgMaxStdGrp		Request change in the number máximo of students of a group

	813. ActLstGst			List main data of administrators

	814. ActPrnGstPho		Show the class photo of guests ready to be printed
	815. ActPrnStdPho		Show the class photo of students ready to be printed
	816. ActPrnTchPho		Show the class photo of teachers ready to be printed
	817. ActLstGstAll		List in another window the full data of guests
	818. ActLstStdAll		List in another window the full data of students
	819. ActLstTchAll		List in another window the full data of teachers

	820. ActSeeRecOneStd		Show record of one selected student
	821. ActSeeRecOneTch		Show record of one selected teacher
	822. ActSeeRecSevGst		Show records of several selected guests
	823. ActSeeRecSevStd		Show records of several selected students
	824. ActSeeRecSevTch		Show records of several selected teachers
	825. ActPrnRecSevGst		Show records of several selected guests ready to be printed
	826. ActPrnRecSevStd		Show records of several selected students ready to be printed
	827. ActPrnRecSevTch		Show records of several selected teachers ready to be printed
	828. ActRcvRecOthUsr		Update record fields of a student in this course
	829. ActEdiRecFie		Request the edition of record fields of students
	830. ActNewFie			Request the creation of a record field
	831. ActReqRemFie		Request the removal of record field
	832. ActRemFie			Remove a record field
	833. ActRenFie			Request renaming of record field
	834. ActChgRowFie		Request change in number of lines of form of a record field
	835. ActChgVisFie		Request change in visibility of a record field
        836. ActRcvRecCrs		Receive and update fields of my record in this course

        837. ActFrmLogInUsrAgd		Put form to log in to show another user's public agenda
        838. ActLogInUsrAgd		Show another user's public agenda after log in
        839. ActLogInUsrAgdLan		Show another user's public agenda after change language
        840. ActSeeUsrAgd		Show another user's public agenda

	841. ActReqEnrSevStd		Request the enrolment/removing of several students to / from current course
	842. ActReqEnrSevNET		Request the enrolment/removing of several non-editing teachers to / from current course
	843. ActReqEnrSevTch		Request the enrolment/removing of several teachers to / from current course

	844. ActReqLstStdAtt		Request listing of attendance of several students to several events
	845. ActSeeLstMyAtt		List my attendance as student to several events
	846. ActPrnLstMyAtt		Print my attendance as student to several events
	847. ActSeeLstStdAtt		List attendance of several students to several events
	848. ActPrnLstStdAtt		Print attendance of several students to several events
	849. ActFrmNewAtt		Form to create a new attendance event
	850. ActEdiOneAtt		Edit one attendance event
	851. ActNewAtt			Create new attendance event
	852. ActChgAtt			Modify data of an existing attendance event
	853. ActReqRemAtt		Request the removal of an attendance event
	854. ActRemAtt			Remove attendance event
	855. ActHidAtt			Hide attendance event
	856. ActShoAtt			Show attendance event
	857. ActSeeOneAtt		List students who attended to an event
	858. ActRecAttStd		Save students who attended to an event and comments
	859. ActRecAttMe		Save my comments as student in an attendance event

	860. ActSignUp			Apply for my enrolment
	861. ActUpdSignUpReq		Update pending requests for inscription in the current course
	862. ActReqRejSignUp		Ask if reject the enrolment of a user in a course
	863. ActRejSignUp		Reject the enrolment of a user in a course

	864. ActReqMdfOneOth		Request a user's ID for enrolment/removing
	865. ActReqMdfOneStd		Request a student's ID for enrolment/removing
	866. ActReqMdfOneTch		Request a teacher's ID for enrolment/removing
	867. ActReqMdfOth		Request enrolment/removing of a user
	868. ActReqMdfStd		Request enrolment/removing of a student
	869. ActReqMdfNET		Request enrolment/removing of a non-editing teacher
	870. ActReqMdfTch		Request enrolment/removing of a teacher
	871. ActReqOthPho		Show form to send the photo of another user
	872. ActReqStdPho		Show form to send the photo of another user
	873. ActReqTchPho		Show form to send the photo of another user
	874. ActDetOthPho		Receive other user's photo and detect faces on it
	875. ActDetStdPho		Receive other user's photo and detect faces on it
	876. ActDetTchPho		Receive other user's photo and detect faces on it
	877. ActUpdOthPho		Update other user's photo
	878. ActUpdStdPho		Update other user's photo
	879. ActUpdTchPho		Update other user's photo
	880. ActRemOthPho		Request the removal of other user's photo
	881. ActRemStdPho		Request the removal of student's photo
	882. ActRemTchPho		Request the removal of teacher's photo
	883. ActRemOthPho		Remove other user's photo
	884. ActRemStdPho		Remove student's photo
	885. ActRemTchPho		Remove teacher's photo
	886. ActCreOth			Create a new guest
	887. ActCreStd			Create a new student
	888. ActCreNET			Create a new non.editing teacher
	889. ActCreTch			Create a new teacher
	890. ActUpdOth			Update another user's data and groups
	891. ActUpdStd			Update another student's data and groups
	892. ActUpdNET			Update another non-editing teacher's data and groups
	893. ActUpdTch			Update another teacher's data and groups
	894. ActReqAccEnrStd		Confirm acceptation / refusion of enrolment as student in current course
	895. ActReqAccEnrNET		Confirm acceptation / refusion of enrolment as non-editing teacher in current course
	896. ActReqAccEnrTch		Confirm acceptation / refusion of enrolment as teacher in current course
	897. ActAccEnrStd		Accept enrolment as student in current course
	898. ActAccEnrNET		Accept enrolment as non-editing teacher in current course
	899. ActAccEnrTch		Accept enrolment as teacher in current course
	900. ActRemMe_Std		Reject enrolment as student in current course
	901. ActRemMe_NET		Reject enrolment as non-editing teacher in current course
	902. ActRemMe_Tch		Reject enrolment as teacher in current course

	973. ActNewAdmIns		Register an administrador in this institution
	974. ActRemAdmIns		Remove an administrador of this institution
	985. ActNewAdmCtr		Register an administrador in this centre
	986. ActRemAdmCtr		Remove an administrador of this centre
	987. ActNewAdmDeg		Register an administrador in this degree
	988. ActRemAdmDeg		Remove an administrador of this degree

	989. ActRcvFrmEnrSevStd		Receive a form with IDs of students to be registered/removed to/from current course
	990. ActRcvFrmEnrSevNET		Receive a form with IDs of non-editing teachers to be registered/removed to/from current course
	991. ActRcvFrmEnrSevTch		Receive a form with IDs of teachers to be registered/removed to/from current course

	992. ActCnfID_Oth		Confirm another user's ID
	993. ActCnfID_Std		Confirm another user's ID
	994. ActCnfID_Tch		Confirm another user's ID

	995. ActFrmIDsOth		Show form to the change of the IDs of another user
	996. ActFrmIDsStd		Show form to the change of the IDs of another user
	997. ActFrmIDsTch		Show form to the change of the IDs of another user
	998. ActRemID_Oth		Remove one of the IDs of another user
	999. ActRemID_Std		Remove one of the IDs of another user
       1000. ActRemID_Tch		Remove one of the IDs of another user
       1001. ActNewID_Oth		Create a new user's ID for another user
       1002. ActNewID_Std		Create a new user's ID for another user
       1003. ActNewID_Tch		Create a new user's ID for another user

       1004. ActFrmPwdOth		Show form to change the password of another user
       1005. ActFrmPwdStd		Show form to change the password of another user
       1006. ActFrmPwdTch		Show form to change the password of another user
       1007. ActChgPwdOth		Change the password of another user
       1008. ActChgPwdStd		Change the password of another user
       1009. ActChgPwdTch		Change the password of another user

       1010. ActFrmMaiOth		Show form to the change of the email of another user
       1011. ActFrmMaiStd		Show form to the change of the email of another user
       1012. ActFrmMaiTch		Show form to the change of the email of another user
       1013. ActRemMaiOth		Remove one of the email of another user
       1014. ActRemMaiStd		Remove one of the email of another user
       1015. ActRemMaiTch		Remove one of the email of another user
       1016. ActNewMaiOth		Create a new user's email for another user
       1017. ActNewMaiStd		Create a new user's email for another user
       1018. ActNewMaiTch		Create a new user's email for another user

       1019. ActRemStdCrs		Remove a student from the current course
       1020. ActRemNETCrs		Remove a non-editing teacher from the current course
       1021. ActRemTchCrs		Remove a teacher from the current course
       1022. ActRemUsrGbl		Eliminate completely a user from the platform
       1023. ActReqRemAllStdCrs		Request the removal of all the students from the current course
       1024. ActRemAllStdCrs		Remove all the students from the current course
       1025. ActReqRemOldUsr		Request the complete elimination of old users
       1026. ActRemOldUsr		Eliminate completely old users

       1027. ActLstDupUsr		List possible duplicate users
       1028. ActLstSimUsr		List users similar to a given one (possible duplicates)
       1029. ActRemDupUsr		Remove user from list of possible duplicate users

       1030. ActLstClk			List last clicks in real time

Social:
       1031. ActSeeSocTmlGbl		Show social timeline (global)
       1032. ActSeeSocPrf		Suggest list of users to follow
       1033. ActSeeFor			Show the level superior of the forums
       1034. ActSeeChtRms		Show the chat rooms

       1035. ActRcvSocPstGbl		Receive a public social post to be displayed in the timeline (global)
       1036. ActRcvSocComGbl		Comment a social note in the timeline (global)
       1037. ActShaSocNotGbl		Share a social note in the timeline (global)
       1038. ActUnsSocNotGbl		Unshare a previously shared social note in the timeline (global)
       1039. ActFavSocNotGbl		Favourite a social note in the timeline (global)
       1040. ActUnfSocNotGbl		Unfavourite a previously favourited social note in the timeline (global)
       1041. ActFavSocComGbl		Favourite a social comment in the timeline (global)
       1042. ActUnfSocComGbl		Unfavourite a previously favourited social comment in the timeline (global)
       1043. ActReqRemSocPubGbl		Request the removal of a social publishing in the timeline (global)
       1044. ActRemSocPubGbl		Remove a social publishing in the timeline (global)
       1045. ActReqRemSocComGbl		Request the removal of a comment in a social note (global)
       1046. ActRemSocComGbl		Remove of a comment in a social note (global)

       1047. ActReqOthPubPrf		Request @nickname to show a public user's profile

       1048. ActRcvSocPstUsr		Receive a public social post to be displayed in the timeline (user)
       1049. ActRcvSocComUsr		Comment a social note in the timeline (user)
       1050. ActShaSocNotUsr		Share a social note in the timeline (user)
       1051. ActUnsSocNotUsr		Unshare a previously shared social note in the timeline (user)
       1052. ActFavSocNotUsr		Favourite a social note in the timeline (user)
       1053. ActUnfSocNotUsr		Unfavourite a previously favourited social note in the timeline (user)
       1054. ActFavSocComUsr		Favourite a social comment in the timeline (user)
       1055. ActUnfSocComUsr		Unfavourite a previously favourited social comment in the timeline (user)
       1056. ActReqRemSocPubUsr		Request the removal of a social publishing in the timeline (user)
       1057. ActRemSocPubUsr		Remove a social publishing in the timeline (user)
       1058. ActReqRemSocComUsr		Request the removal of a comment in a social note (user)
       1059. ActRemSocComUsr		Remove of a comment in a social note (user)

       1060. ActCal1stClkTim		Calculate first click time from log and store into user's figures
       1061. ActCalNumClk		Calculate number of clicks from log and store into user's figures
       1062. ActCalNumFileViews		Calculate number of file views and store into user's figures
       1063. ActCalNumForPst		Calculate number of forum posts and store into user's figures
       1064. ActCalNumMsgSnt		Calculate number of messages sent from log and store into user's figures

       1065. ActFolUsr			Follow another user
       1066. ActUnfUsr			Unfollow another user
       1067. ActSeeFlg			Show following
       1068. ActSeeFlr			Show followers

       1069. ActSeeForCrsUsr		Show top level of forum of users of the course
       1070. ActSeeForCrsTch		Show top level of forum of teachers of the course
       1071. ActSeeForDegUsr		Show top level of forum of users of the degree
       1072. ActSeeForDegTch		Show top level of forum of teachers of the degree
       1073. ActSeeForCtrUsr		Show top level of forum of users of the centre
       1074. ActSeeForCtrTch		Show top level of forum of teachers of the centre
       1075. ActSeeForInsUsr		Show top level of forum of users of the institution
       1076. ActSeeForInsTch		Show top level of forum of teachers of the institution
       1077. ActSeeForGenUsr		Show top level of forum of users general
       1078. ActSeeForGenTch		Show top level of forum of teachers general
       1079. ActSeeForSWAUsr		Show top level of forum of users of the platform
       1080. ActSeeForSWATch		Show top level of forum of teachers of the platform
       1081. ActSeePstForCrsUsr		Show the messages of a thread of the forum of users of the course
       1082. ActSeePstForCrsTch		Show the messages of a thread of the forum of teachers of the course
       1083. ActSeePstForDegUsr		Show the messages of a thread of the forum of users of the degree
       1084. ActSeePstForDegTch		Show the messages of a thread of the forum of teachers of the degree
       1085. ActSeePstForCtrUsr		Show the messages of a thread of the forum of users of the centre
       1086. ActSeePstForCtrTch		Show the messages of a thread of the forum of teachers of the centre
       1087. ActSeePstForInsUsr		Show the messages of a thread of the forum of users of the institution
       1088. ActSeePstForInsTch		Show the messages of a thread of the forum of teachers of the institution
       1089. ActSeePstForGenUsr		Show the messages of a thread of the forum of users general
       1090. ActSeePstForGenTch		Show the messages of a thread of the forum of teachers general
       1091. ActSeePstForSWAUsr		Show the messages of a thread of the forum of users of the platform
       1092. ActSeePstForSWATch		Show the messages of a thread of the forum of teachers of the platform
       1093. ActRcvThrForCrsUsr		Receive the first message of a new thread of forum of users of the course
       1094. ActRcvThrForCrsTch		Receive the first message of a new thread of forum of teachers of the course
       1095. ActRcvThrForDegUsr		Receive the first message of a new thread of forum of users of the degree
       1096. ActRcvThrForDegTch		Receive the first message of a new thread of forum of teachers of the degree
       1097. ActRcvThrForCtrUsr		Receive the first message of a new thread of forum of users of centre
       1098. ActRcvThrForCtrTch		Receive the first message of a new thread of forum of teachers of centre
       1099. ActRcvThrForInsUsr		Receive the first message of a new thread of forum of users of the institution
       1100. ActRcvThrForInsTch		Receive the first message of a new thread of forum of teachers of the institution
       1101. ActRcvThrForGenUsr		Receive the first message of a new thread of forum of users general
       1102. ActRcvThrForGenTch		Receive the first message of a new thread of forum of teachers general
       1103. ActRcvThrForSWAUsr		Receive the first message of a new thread of forum of users of the platform
       1104. ActRcvThrForSWATch		Receive the first message of a new thread of forum of teachers of the platform
       1105. ActRcvRepForCrsUsr		Receive a message of answer in a thread existente in the forum of users of the course
       1106. ActRcvRepForCrsTch		Receive a message of answer in a thread existente in the forum of teachers of the course
       1107. ActRcvRepForDegUsr		Receive a message of answer in a thread existente in the forum of users of the degree
       1108. ActRcvRepForDegTch		Receive a message of answer in a thread existente in the forum of teachers of the degree
       1109. ActRcvRepForCtrUsr		Receive a message of answer in a thread existente in the forum of users of centre
       1110. ActRcvRepForCtrTch		Receive a message of answer in a thread existente in the forum of teachers of centre
       1111. ActRcvRepForInsUsr		Receive a message of answer in a thread existente in the forum of users of the institution
       1112. ActRcvRepForInsTch		Receive a message of answer in a thread existente in the forum of teachers of the institution
       1113. ActRcvRepForGenUsr		Receive a message of answer in a thread existente in the forum of users general
       1114. ActRcvRepForGenTch		Receive a message of answer in a thread existente in the forum of teachers general
       1115. ActRcvRepForSWAUsr		Receive a message of answer in a thread existente in the forum of users of the platform
       1116. ActRcvRepForSWATch		Receive a message of answer in a thread existente in the forum of teachers of the platform
       1117. ActReqDelThrCrsUsr		Request the removal of a thread of forum of users of the course
       1118. ActReqDelThrCrsTch		Request the removal of a thread of forum of teachers of the course
       1119. ActReqDelThrDegUsr		Request the removal of a thread of forum of users of the degree
       1120. ActReqDelThrDegTch		Request the removal of a thread of forum of teachers of the degree
       1121. ActReqDelThrCtrUsr		Request the removal of a thread of forum of users of centre
       1122. ActReqDelThrCtrTch		Request the removal of a thread of forum of teachers of centre
       1123. ActReqDelThrInsUsr		Request the removal of a thread of forum of users of the institution
       1124. ActReqDelThrInsTch		Request the removal of a thread of forum of teachers of the institution
       1125. ActReqDelThrGenUsr		Request the removal of a thread of forum of users general
       1126. ActReqDelThrGenTch		Request the removal of a thread of forum of teachers general
       1127. ActReqDelThrSWAUsr		Request the removal of a thread of forum of users of the platform
       1128. ActReqDelThrSWATch		Request the removal of a thread of forum of teachers of the platform
       1129. ActDelThrForCrsUsr		Remove a thread of forum of users of the course
       1130. ActDelThrForCrsTch		Remove a thread of forum of teachers of the course
       1131. ActDelThrForDegUsr		Remove a thread of forum of users of the degree
       1132. ActDelThrForDegTch		Remove a thread of forum of teachers of the degree
       1133. ActDelThrForCtrUsr		Remove a thread of forum of users of centre
       1134. ActDelThrForCtrTch		Remove a thread of forum of teachers of centre
       1135. ActDelThrForInsUsr		Remove a thread of forum of users of the institution
       1136. ActDelThrForInsTch		Remove a thread of forum of teachers of the institution
       1137. ActDelThrForGenUsr		Remove a thread of forum of users general
       1138. ActDelThrForGenTch		Remove a thread of forum of teachers general
       1139. ActDelThrForSWAUsr		Remove a thread of forum of users of the platform
       1140. ActDelThrForSWATch		Remove a thread of forum of teachers of the platform
       1141. ActCutThrForCrsUsr		Cut a thread of forum of users of the course
       1142. ActCutThrForCrsTch		Cut a thread of forum of teachers of the course
       1143. ActCutThrForDegUsr		Cut a thread of forum of users of the degree
       1144. ActCutThrForDegTch		Cut a thread of forum of teachers of the degree
       1145. ActCutThrForCtrUsr		Cut a thread of forum of users of centre
       1146. ActCutThrForCtrTch		Cut a thread of forum of teachers of centre
       1147. ActCutThrForInsUsr		Cut a thread of forum of users of the institution
       1148. ActCutThrForInsTch		Cut a thread of forum of teachers of the institution
       1149. ActCutThrForGenUsr		Cut a thread of forum of users general
       1140. ActCutThrForGenTch		Cut a thread of forum of teachers general
       1141. ActCutThrForSWAUsr		Cut a thread of forum of users of the platform
       1142. ActCutThrForSWATch		Cut a thread of forum of teachers of the platform
       1143. ActPasThrForCrsUsr		Paste a thread of forum of users of the course
       1154. ActPasThrForCrsTch		Paste a thread of forum of teachers of the course
       1155. ActPasThrForDegUsr		Paste a thread of forum of users of the degree
       1156. ActPasThrForDegTch		Paste a thread of forum of teachers of the degree
       1157. ActPasThrForCtrUsr		Paste a thread of forum of users of centre
       1158. ActPasThrForCtrTch		Paste a thread of forum of teachers of centre
       1159. ActPasThrForInsUsr		Paste a thread of forum of users of the institution
       1150. ActPasThrForInsTch		Paste a thread of forum of teachers of the institution
       1151. ActPasThrForGenUsr		Paste a thread of forum of users general
       1152. ActPasThrForGenTch		Paste a thread of forum of teachers general
       1153. ActPasThrForSWAUsr		Paste a thread of forum of users of the platform
       1164. ActPasThrForSWATch		Paste a thread of forum of teachers of the platform
       1165. ActDelPstForCrsUsr		Remove a message of forum of users of the course
       1166. ActDelPstForCrsTch		Remove a message of forum of teachers of the course
       1167. ActDelPstForDegUsr		Remove a message of forum of users of the degree
       1168. ActDelPstForDegTch		Remove a message of forum of teachers of the degree
       1169. ActDelPstForCtrUsr		Remove a message of forum of users of centre
       1160. ActDelPstForCtrTch		Remove a message of forum of teachers of centre
       1161. ActDelPstForInsUsr		Remove a message of forum of users of the institution
       1162. ActDelPstForInsTch		Remove a message of forum of teachers of the institution
       1163. ActDelPstForGenUsr		Remove a message of forum of users general
       1174. ActDelPstForGenTch		Remove a message of forum of teachers general
       1175. ActDelPstForSWAUsr		Remove a message of forum of users of the platform
       1176. ActDelPstForSWATch		Remove a message of forum of teachers of the platform
       1177. ActEnbPstForCrsUsr		Enable a message of forum of users of the course
       1178. ActEnbPstForCrsTch		Enable a message of forum of teachers of the course
       1179. ActEnbPstForDegUsr		Enable a message of forum of users of the degree
       1170. ActEnbPstForDegTch		Enable a message of forum of teachers of the degree
       1171. ActEnbPstForCtrUsr		Enable a message of forum of users of centre
       1172. ActEnbPstForCtrTch		Enable a message of forum of teachers of centre
       1173. ActEnbPstForInsUsr		Enable a message of forum of users of the institution
       1184. ActEnbPstForInsTch		Enable a message of forum of teachers of the institution
       1185. ActEnbPstForGenUsr		Enable a message of forum of users general
       1186. ActEnbPstForGenTch		Enable a message of forum of teachers general
       1187. ActEnbPstForSWAUsr		Enable a message of forum of users of the platform
       1188. ActEnbPstForSWATch		Enable a message of forum of teachers of the platform
       1189. ActDisPstForCrsUsr		Disable a message of forum of users of the course
       1180. ActDisPstForCrsTch		Disable a message of forum of teachers of the course
       1181. ActDisPstForDegUsr		Disable a message of forum of users of the degree
       1182. ActDisPstForDegTch		Disable a message of forum of teachers of the degree
       1183. ActDisPstForCtrUsr		Disable a message of forum of users of centre
       1194. ActDisPstForCtrTch		Disable a message of forum of teachers of centre
       1195. ActDisPstForInsUsr		Disable a message of forum of users of the institution
       1196. ActDisPstForInsTch		Disable a message of forum of teachers of the institution
       1197. ActDisPstForGenUsr		Disable a message of forum of users general
       1198. ActDisPstForGenTch		Disable a message of forum of teachers general
       1199. ActDisPstForSWAUsr		Disable a message of forum of users of the platform
       1200. ActDisPstForSWATch		Disable a message of forum of teachers of the platform

       1201. ActCht			Enter in a chat room to chat

Messages:
       1202. ActSeeNtf			Show my recent notifications
       1203. ActSeeAnn			Show global announcements
       1204. ActSeeAllNot		Show all notices
       1205. ActReqMsgUsr		Write message to several users
       1206. ActSeeRcvMsg		Show the messages received from other users (link in menu)
       1207. ActSeeSntMsg		Show the messages sent to other users
       1208. ActMaiStd			Send an email to students
       1209. ActWriAnn			Show form to create a new global announcement
       1210. ActRcvAnn			Receive and create a new global announcement
       1211. ActHidAnn			Hide a global announcement that was active
       1212. ActRevAnn			Reveal a global announcement that was hidden
       1213. ActRemAnn			Remove global announcement
       1214. ActSeeOneNot		Show (expand) a notice
       1215. ActWriNot			Write a new notice
       1216. ActRcvNot			Receive and create a new notice
       1217. ActHidNot			Hide a notice that was active
       1218. ActRevNot			Reveal a notice that was hidden
       1219. ActReqRemNot		Request removal of a notice
       1220. ActRemNot			Remove a notice

       1221. ActSeeNewNtf		Show my recent notifications (link in top heading)
       1222. ActMrkNtfSee		Mark all my notifications as seen
       1223. ActSeeMai			See mail domains
       1224. ActEdiMai			Edit mail domains
       1225. ActNewMai			Request the creation of a mail domain
       1226. ActRemMai			Request the removal of a mail domain
       1227. ActRenMaiSho		Request the change of a mail domain
       1228. ActRenMaiFul		Request the change of the info of a mail domain

       1229. ActRcvMsgUsr		Sent/Receive a message of a user
       1230. ActReqDelAllSntMsg		Request the removal of todos the messages sent to other users
       1231. ActReqDelAllRcvMsg		Request the removal of todos the messages received from other users
       1232. ActDelAllSntMsg		Remove todos the messages sent to other users
       1233. ActDelAllRcvMsg		Remove todos the messages received from other users
       1234. ActDelSntMsg		Remove a message sent to other users
       1235. ActDelRcvMsg		Remove a message received from other user
       1236. ActExpSntMsg		See (expand) sent message
       1237. ActExpRcvMsg		See (expand) received message
       1238. ActConSntMsg		Hide (contract) sent message
       1239. ActConRcvMsg		Hide (contract) received message
       1240. ActLstBanUsr		List banned users
       1241. ActBanUsrMsg		Ban the sender of a message when showing received messages
       1242. ActUnbUsrMsg		Unban the sender of a message when showing received messages
       1243. ActUnbUsrLst		Unban a user when listing banned users

Statistics:
       1244. ActReqUseGbl		Request showing use of the platform
       1245. ActSeePhoDeg		Show a class photo with the average photos of the students of each degree
       1246. ActReqStaCrs		Request statistics of courses
       1247. ActReqAccGbl		Request query of clicks to the complete platform
       1248. ActReqMyUsgRep		Request my usage report
       1249. ActMFUAct			Show most frequently used actions

       1250. ActSeeUseGbl		Show use of the platform
       1251. ActPrnPhoDeg		Show vista of impresión of the class photo with the average photos of the students of each degree.
       1252. ActCalPhoDeg		Compute the average photos of the students of each degree
       1253. ActSeeAccGbl		Query clicks to the complete platform
       1254. ActReqAccCrs		Request query of clicks in the course
       1255. ActSeeAccCrs		Query clicks to current course
       1256. ActSeeAllStaCrs		Show statistics of courses
       1257. ActSeeMyUsgRep		Show my usage report

Profile:
       1258. ActFrmLogIn		Show landing page (forms to log in and to create a new account)
       1259. ActFrmRolSes		Show form to log out and to change current role in this session
       1260. ActMyCrs			Select one of my courses
       1261. ActSeeMyTT			Show the timetable of all courses of the logged user
       1262. ActSeeMyAgd		Show my full agenda (personal organizer)
       1263. ActFrmMyAcc		Show form to the creation or change of user's account
       1264. ActReqEdiRecCom		Request the edition of the record with the personal data of the user
       1265. ActEdiPrf			Show forms to edit preferences
       1266. ActAdmBrf			Show the briefcase of private archives

       1267. ActReqSndNewPwd		Show form to send a new password via email
       1268. ActSndNewPwd		Send a new password via email
       1269. ActLogOut			Close session

       1270. ActLogIn		Authentify user internally (directly from the platform)
       1271. ActLogInNew		Authentify user internally (directly from the platform, only if user has not password)
       1272. ActLogInLan		Change language to my language just after authentication
       1273. ActAnnSee			Mark announcement as seen
       1274. ActChgMyRol		Change type of logged user

       1275. ActFrmNewEvtMyAgd		Form to create a new event in my agenda
       1276. ActEdiOneEvtMyAgd		Edit one event from my agenda
       1277. ActNewEvtMyAgd		Create a new event in my agenda
       1278. ActChgEvtMyAgd		Modify data of an event from my agenda
       1279. ActReqRemEvtMyAgd		Request the removal of an event from my agenda
       1280. ActRemEvtMyAgd		Remove an event from my agenda
       1281. ActHidEvtMyAgd		Hide an event from my agenda
       1282. ActShoEvtMyAgd		Show an event from my agenda
       1283. ActPrvEvtMyAgd		Make private an event from my agenda
       1284. ActPubEvtMyAgd		Make public an event from my agenda
       1285. ActPrnAgdQR		Show agenda QR code ready to print

       1286. ActChkUsrAcc		Check if already exists a new account without password associated to a ID
       1287. ActCreUsrAcc		Create new user account
       1288. ActRemID_Me		Remove one of my user's IDs
       1289. ActNewIDMe			Create a new user's ID for me
       1290. ActRemOldNic		Remove one of my old nicknames
       1291. ActChgNic			Change my nickname
       1292. ActRemMaiMe		Remove one of my old emails
       1293. ActNewMaiMe		Change my email address
       1294. ActCnfMai			Confirm email address
       1295. ActFrmChgMyPwd		Show form to the change of the password
       1296. ActChgPwd			Change the password
       1297. ActReqRemMyAcc		Request the removal of my account
       1298. ActRemMyAcc		Remove my account

       1299. ActChgMyData		Update my personal data

       1300. ActReqMyPho		Show form to send my photo
       1301. ActDetMyPho		Receive my photo and detect faces on it
       1302. ActUpdMyPho		Update my photo
       1303. ActReqRemMyPho		Request the removal of my photo
       1304. ActRemMyPho		Remove my photo

       1305. ActEdiPri			Edit my privacy
       1306. ActChgPriPho		Change privacy of my photo
       1307. ActChgPriPrf		Change privacy of my public profile

       1308. ActReqEdiMyIns		Request the edition of my institution, centre and department
       1309. ActChgCtyMyIns		Change the country of my institution
       1310. ActChgMyIns		Change my institution
       1311. ActChgMyCtr		Change my centre
       1312. ActChgMyDpt		Change my department
       1313. ActChgMyOff		Change my office
       1314. ActChgMyOffPho		Change my office phone

       1315. ActReqEdiMyNet		Request the edition of my social networks
       1316. ActChgMyNet		Change my web and social networks

       1317. ActChgThe			Change theme
       1318. ActReqChgLan		Ask if change language
       1319. ActChgLan			Change language
       1320. ActChg1stDay		Change first day of the week
       1321. ActChgDatFmt		Change date format
       1322. ActChgCol			Change side columns
       1323. ActHidLftCol		Hide left side column
       1324. ActHidRgtCol		Hide right side column
       1325. ActShoLftCol		Show left side column
       1326. ActShoRgtCol		Show right side column
       1327. ActChgIco			Change icon set
       1328. ActChgMnu			Change menu
       1329. ActChgNtfPrf		Change whether to notify by email new messages
       1330. ActPrnUsrQR		Show my QR code ready to print

       1331. ActPrnMyTT			Show the timetable listo to impresión of all my courses
       1332. ActEdiTut			Edit the timetable of tutorías
       1333. ActChgTut			Modify the timetable of tutorías
       1334. ActChgMyTT1stDay		Change first day of week and show timetable of the course

       1335. ActReqRemFilBrf		Request removal of a file of the briefcase
       1336. ActRemFilBrf		Remove a file of the briefcase
       1337. ActRemFolBrf		Remove a folder empty of the briefcase
       1338. ActCopBrf			Set source of copy in the briefcase
       1339. ActPasBrf			Paste a folder or file in the briefcase
       1340. ActRemTreBrf		Remove a folder no empty of the briefcase
       1341. ActFrmCreBrf		Form to crear a folder or file in the briefcase
       1342. ActCreFolBrf		Create a new folder in the briefcase
       1343. ActCreLnkBrf		Create a new link in the briefcase
       1344. ActRenFolBrf		Rename a folder of the briefcase
       1345. ActRcvFilBrfDZ		Receive a file in the briefcase using Dropzone.js
       1346. ActRcvFilBrfCla		Receive a file in the briefcase using the classic way
       1347. ActExpBrf			Expand a folder in briefcase
       1348. ActConBrf			Contract a folder in briefcase
       1349. ActZIPBrf			Compress a folder in briefcase
       1350. ActReqDatBrf		Ask for metadata of a file in the briefcase
       1351. ActChgDatBrf		Change metadata of a file in the briefcase
       1352. ActDowBrf			Download a file in the briefcase
       1353. ActReqRemOldBrf		Ask for removing old files in the briefcase
       1354. ActRemOldBrf		Remove old files in the briefcase
*/

/*
struct Act_Actions
  {
   long ActCod;	// Unique, time-persistent numerical code for the action
   signed int IndexInMenu;
   Tab_Tab_t Tab;
   Act_Action_t SuperAction;
   unsigned PermissionCrsIfIBelong;
   unsigned PermissionCrsIfIDontBelong;
   unsigned PermissionDeg;
   unsigned PermissionCtr;
   unsigned PermissionIns;
   unsigned PermissionCty;
   unsigned PermissionSys;
   Act_Content_t ContentType;
   Act_BrowserWindow_t BrowserWindow;
   void (*FunctionPriori)();
   void (*FunctionPosteriori)();
   const char *Icon;
  };
*/
struct Act_Actions Act_Actions[Act_NUM_ACTIONS] =
  {
   // TabUnk ******************************************************************
   // Actions not in menu:
   /* ActAll		*/{ 645,-1,TabUnk,ActAll		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,NULL				,NULL},	// Used for statistics
   /* ActUnk		*/{ 194,-1,TabUnk,ActUnk		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,NULL				,NULL},
   /* ActHom		*/{ 844,-1,TabUnk,ActHom		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,NULL				,NULL},
   /* ActMnu		*/{   2,-1,TabUnk,ActMnu		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,NULL				,NULL},
   /* ActRefCon		*/{ 845,-1,TabUnk,ActRefCon		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lay_RefreshNotifsAndConnected	,NULL},
   /* ActRefLstClk	*/{ 994,-1,TabUnk,ActRefLstClk		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lay_RefreshLastClicks		,NULL},
   /* ActRefNewSocPubGbl*/{1509,-1,TabUnk,ActRefNewSocPubGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_RefreshNewTimelineGbl	,NULL},
   /* ActRefOldSocPubGbl*/{1510,-1,TabUnk,ActRefOldSocPubGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_RefreshOldTimelineGbl	,NULL},
   /* ActRefOldSocPubUsr*/{1511,-1,TabUnk,ActRefOldSocPubUsr	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_RefreshOldTimelineUsr	,NULL},
   /* ActWebSvc		*/{ 892,-1,TabUnk,ActWebSvc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_WebService			,NULL},

   // TabSys ******************************************************************
   // Actions in menu:
   /* ActSysReqSch	*/{ 627, 0,TabSys,ActSysReqSch		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sch_ReqSysSearch		,"search64x64.gif"	},
   /* ActSeeCty		*/{ 862, 1,TabSys,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Cty_ListCountries1		,Cty_ListCountries2		,"earth64x64.gif"	},
   /* ActSeePen		*/{1060, 2,TabSys,ActSeePen		,    0,    0,    0,    0,    0,    0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Hie_SeePending			,"hierarchy64x64.png"	},
   /* ActSeeLnk		*/{ 748, 3,TabSys,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lnk_SeeLinks			,"link64x64.gif"	},
   /* ActLstPlg		*/{ 777, 4,TabSys,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_ListPlugins		,"blocks64x64.gif"	},
   /* ActSetUp		*/{ 840, 5,TabSys,ActSetUp		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Set_Setup			,"lightning64x64.gif"	},
   /* ActSeeCalSys	*/{1622, 6,TabSys,ActSeeCalSys		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cal_DrawCalendarSys		,"calendar64x64.png"	},

   // Actions not in menu:
   /* ActSysSch		*/{ 628,-1,TabUnk,ActSysReqSch		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Sch_GetParamsSearch		,Sch_SysSearch			,NULL},

   /* ActEdiCty		*/{ 863,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cty_EditCountries		,NULL},
   /* ActNewCty		*/{ 864,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cty_RecFormNewCountry		,NULL},
   /* ActRemCty		*/{ 893,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cty_RemoveCountry		,NULL},
   /* ActRenCty		*/{ 866,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cty_RenameCountry		,NULL},
   /* ActChgCtyWWW	*/{1157,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cty_ChangeCtyWWW		,NULL},

   /* ActReqRemOldCrs	*/{1109,-1,TabUnk,ActSeePen		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Crs_AskRemoveOldCrss		,NULL},
   /* ActRemOldCrs	*/{1110,-1,TabUnk,ActSeePen		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Crs_RemoveOldCrss		,NULL},

   /* ActSeeBan		*/{1137,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3FF,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_SeeBanners			,NULL},
   /* ActEdiBan		*/{1138,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_EditBanners		,NULL},
   /* ActNewBan		*/{1139,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_RecFormNewBanner		,NULL},
   /* ActRemBan		*/{1140,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_RemoveBanner		,NULL},
   /* ActShoBan		*/{1212,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_ShowBanner			,NULL},
   /* ActHidBan		*/{1213,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_HideBanner			,NULL},
   /* ActRenBanSho	*/{1141,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_RenameBannerShort		,NULL},
   /* ActRenBanFul	*/{1142,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_RenameBannerFull		,NULL},
   /* ActChgBanImg	*/{1144,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_ChangeBannerImg		,NULL},
   /* ActChgBanWWW	*/{1143,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ban_ChangeBannerWWW		,NULL},
   /* ActClkBan		*/{1145,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_DOWNLD_FILE,Ban_ClickOnBanner		,NULL				,NULL},

   /* ActEdiLnk		*/{ 749,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lnk_EditLinks			,NULL},
   /* ActNewLnk		*/{ 750,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lnk_RecFormNewLink		,NULL},
   /* ActRemLnk		*/{ 897,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lnk_RemoveLink			,NULL},
   /* ActRenLnkSho	*/{ 753,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lnk_RenameLinkShort		,NULL},
   /* ActRenLnkFul	*/{ 751,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lnk_RenameLinkFull		,NULL},
   /* ActChgLnkWWW	*/{ 752,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lnk_ChangeLinkWWW		,NULL},

   /* ActEdiPlg		*/{ 778,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_EditPlugins		,NULL},
   /* ActNewPlg		*/{ 779,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_RecFormNewPlg		,NULL},
   /* ActRemPlg		*/{ 889,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_RemovePlugin		,NULL},
   /* ActRenPlg		*/{ 782,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_RenamePlugin		,NULL},
   /* ActChgPlgDes	*/{ 888,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_ChangePlgDescription	,NULL},
   /* ActChgPlgLog	*/{ 781,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_ChangePlgLogo		,NULL},
   /* ActChgPlgAppKey	*/{ 986,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_ChangePlgAppKey		,NULL},
   /* ActChgPlgURL	*/{ 783,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_ChangePlgURL		,NULL},
   /* ActChgPlgIP	*/{ 780,-1,TabUnk,ActLstPlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plg_ChangePlgIP		,NULL},

   /* ActPrnCalSys	*/{1623,-1,TabUnk,ActSeeCalSys		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Cal_PrintCalendar		,NULL},
   /* ActChgCalSys1stDay*/{1624,-1,TabUnk,ActSeeCalSys		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,Cal_DrawCalendarSys		,NULL},

   // TabCty ******************************************************************
   // Actions in menu:
   /* ActCtyReqSch	*/{1176, 0,TabCty,ActCtyReqSch		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sch_ReqCtySearch		,"search64x64.gif"	},
   /* ActSeeCtyInf	*/{1155, 1,TabCty,ActSeeCtyInf		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cty_ShowConfiguration		,"info64x64.gif"	},
   /* ActSeeIns		*/{ 696, 2,TabCty,ActSeeIns		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_ShowInssOfCurrentCty	,"institution64x64.gif"	},
   /* ActSeeCalCty	*/{1625, 3,TabCty,ActSeeCalCty		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cal_DrawCalendarCty		,"calendar64x64.png"	},

   // Actions not in menu:
   /* ActCtySch		*/{1181,-1,TabUnk,ActCtyReqSch		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_THIS_WINDOW,Sch_GetParamsSearch		,Sch_CtySearch			,NULL},

   /* ActPrnCtyInf	*/{1156,-1,TabUnk,ActSeeCtyInf		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Cty_PrintConfiguration		,NULL},
   /* ActChgCtyMapAtt	*/{1158,-1,TabUnk,ActSeeCtyInf		,    0,    0,    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cty_ChangeCtyMapAttribution	,NULL},

   /* ActEdiIns		*/{ 697,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_EditInstitutions		,NULL},
   /* ActReqIns		*/{1210,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_RecFormReqIns		,NULL},
   /* ActNewIns		*/{ 698,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_RecFormNewIns		,NULL},
   /* ActRemIns		*/{ 759,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_RemoveInstitution		,NULL},
   /* ActRenInsSho	*/{ 702,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ins_RenameInsShort		,Ins_ContEditAfterChgIns	,NULL},
   /* ActRenInsFul	*/{ 701,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ins_RenameInsFull		,Ins_ContEditAfterChgIns	,NULL},
   /* ActChgInsWWW	*/{ 700,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_ChangeInsWWW		,NULL},
   /* ActChgInsSta	*/{1211,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_ChangeInsStatus		,NULL},

   /* ActPrnCalCty	*/{1626,-1,TabUnk,ActSeeCalCty		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Cal_PrintCalendar		,NULL},
   /* ActChgCalCty1stDay*/{1627,-1,TabUnk,ActSeeCalCty		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,Cal_DrawCalendarCty		,NULL},

   // TabIns ******************************************************************
   // Actions in menu:
   /* ActInsReqSch	*/{1177, 0,TabIns,ActInsReqSch		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sch_ReqInsSearch		,"search64x64.gif"	},
   /* ActSeeInsInf	*/{1153, 1,TabIns,ActSeeInsInf		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_ShowConfiguration		,"info64x64.gif"	},
   /* ActSeeCtr		*/{ 676, 2,TabIns,ActSeeCtr		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_ShowCtrsOfCurrentIns	,"house64x64.gif"	},
   /* ActSeeDpt		*/{ 675, 3,TabIns,ActSeeDpt		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_SeeDepts			,"houseteachers64x64.gif"},
   /* ActSeeCalIns	*/{1628, 4,TabIns,ActSeeCalIns		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cal_DrawCalendarIns		,"calendar64x64.png"	},

   // Actions not in menu:
   /* ActInsSch		*/{1182,-1,TabUnk,ActInsReqSch		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Sch_GetParamsSearch		,Sch_InsSearch			,NULL},

   /* ActPrnInsInf	*/{1154,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Ins_PrintConfiguration		,NULL},
   /* ActChgInsCtyCfg	*/{1590,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ins_ChangeInsCtyInConfig	,Ins_ContEditAfterChgInsInConfig,NULL},
   /* ActRenInsShoCfg	*/{1592,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ins_RenameInsShortInConfig	,Ins_ContEditAfterChgInsInConfig,NULL},
   /* ActRenInsFulCfg	*/{1591,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ins_RenameInsFullInConfig	,Ins_ContEditAfterChgInsInConfig,NULL},
   /* ActChgInsWWWCfg	*/{1593,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_ChangeInsWWWInConfig	,NULL},
   /* ActReqInsLog	*/{1245,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ins_RequestLogo		,NULL},
   /* ActRecInsLog	*/{ 699,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,Ins_ReceiveLogo		,Ins_ShowConfiguration		,NULL},
   /* ActRemInsLog	*/{1341,-1,TabUnk,ActSeeInsInf		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,Ins_RemoveLogo			,Ins_ShowConfiguration		,NULL},

   /* ActEdiCtr		*/{ 681,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_EditCentres		,NULL},
   /* ActReqCtr		*/{1208,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_RecFormReqCtr		,NULL},
   /* ActNewCtr		*/{ 685,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_RecFormNewCtr		,NULL},
   /* ActRemCtr		*/{ 686,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_RemoveCentre		,NULL},
   /* ActChgDegPlc	*/{ 706,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_ChangeCentrePlace		,NULL},
   /* ActRenCtrSho	*/{ 682,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ctr_RenameCentreShort		,Ctr_ContEditAfterChgCtr	,NULL},
   /* ActRenCtrFul	*/{ 684,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ctr_RenameCentreFull		,Ctr_ContEditAfterChgCtr	,NULL},
   /* ActChgCtrWWW	*/{ 683,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_ChangeCtrWWW		,NULL},
   /* ActChgCtrSta	*/{1209,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_ChangeCtrStatus		,NULL},

   /* ActSeePlc		*/{ 703,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plc_SeePlaces			,NULL},
   /* ActEdiPlc		*/{ 704,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plc_EditPlaces			,NULL},
   /* ActNewPlc		*/{ 705,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plc_RecFormNewPlace		,NULL},
   /* ActRemPlc		*/{ 776,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plc_RemovePlace		,NULL},
   /* ActRenPlcSho	*/{ 894,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plc_RenamePlaceShort		,NULL},
   /* ActRenPlcFul	*/{ 895,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Plc_RenamePlaceFull		,NULL},

   /* ActEdiDpt		*/{ 677,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_EditDepartments		,NULL},
   /* ActNewDpt		*/{ 687,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_RecFormNewDpt		,NULL},
   /* ActRemDpt		*/{ 690,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_RemoveDepartment		,NULL},
   /* ActChgDptIns	*/{ 721,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_ChangeDepartIns		,NULL},
   /* ActRenDptSho	*/{ 688,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_RenameDepartShort		,NULL},
   /* ActRenDptFul	*/{ 689,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_RenameDepartFull		,NULL},
   /* ActChgDptWWW	*/{ 691,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dpt_ChangeDptWWW		,NULL},

   /* ActSeeHld		*/{ 707,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Hld_SeeHolidays		,NULL},
   /* ActEdiHld		*/{ 713,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Hld_EditHolidays		,NULL},
   /* ActNewHld		*/{ 714,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Hld_RecFormNewHoliday1		,Hld_RecFormNewHoliday2		,NULL},
   /* ActRemHld		*/{ 716,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Hld_RemoveHoliday1		,Hld_RemoveHoliday2		,NULL},
   /* ActChgHldPlc	*/{ 896,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Hld_ChangeHolidayPlace1	,Hld_ChangeHolidayPlace2	,NULL},
   /* ActChgHldTyp	*/{ 715,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Hld_ChangeHolidayType1		,Hld_ChangeHolidayType2		,NULL},
   /* ActChgHldStrDat	*/{ 717,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Hld_ChangeStartDate1		,Hld_ChangeDate2		,NULL},
   /* ActChgHldEndDat	*/{ 718,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Hld_ChangeEndDate1		,Hld_ChangeDate2		,NULL},
   /* ActRenHld		*/{ 766,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Hld_RenameHoliday1		,Hld_RenameHoliday2		,NULL},

   /* ActPrnCalIns	*/{1629,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Cal_PrintCalendar		,NULL},
   /* ActChgCalIns1stDay*/{1630,-1,TabUnk,ActSeeCalIns		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,Cal_DrawCalendarIns		,NULL},

   // TabCtr ******************************************************************
   // Actions in menu:
   /* ActCtrReqSch	*/{1178, 0,TabCtr,ActCtrReqSch		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sch_ReqCtrSearch		,"search64x64.gif"	},
   /* ActSeeCtrInf	*/{1151, 1,TabCtr,ActSeeCtrInf		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_ShowConfiguration		,"info64x64.gif"	},
   /* ActSeeDeg		*/{1011, 2,TabCtr,ActSeeDeg		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_ShowDegsOfCurrentCtr	,"deg64x64.gif"		},
   /* ActSeeCalCtr	*/{1631, 3,TabCtr,ActSeeCalCtr		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cal_DrawCalendarCtr		,"calendar64x64.png"	},

   // Actions not in menu:
   /* ActCtrSch		*/{1183,-1,TabUnk,ActCtrReqSch		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Sch_GetParamsSearch		,Sch_CtrSearch			,NULL},

   /* ActPrnCtrInf	*/{1152,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Ctr_PrintConfiguration		,NULL},
   /* ActChgCtrInsCfg	*/{1589,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ctr_ChangeCtrInsInConfig	,Ctr_ContEditAfterChgCtrInConfig,NULL},
   /* ActRenCtrShoCfg	*/{1595,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ctr_RenameCentreShortInConfig	,Ctr_ContEditAfterChgCtrInConfig,NULL},
   /* ActRenCtrFulCfg	*/{1594,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Ctr_RenameCentreFullInConfig	,Ctr_ContEditAfterChgCtrInConfig,NULL},
   /* ActChgCtrWWWCfg	*/{1596,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_ChangeCtrWWWInConfig	,NULL},
   /* ActReqCtrLog	*/{1244,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_RequestLogo		,NULL},
   /* ActRecCtrLog	*/{1051,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,Ctr_ReceiveLogo		,Ctr_ShowConfiguration		,NULL},
   /* ActRemCtrLog	*/{1342,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,Ctr_RemoveLogo			,Ctr_ShowConfiguration		,NULL},
   /* ActReqCtrPho	*/{1160,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_RequestPhoto		,NULL},
   /* ActRecCtrPho	*/{1161,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Ctr_ReceivePhoto		,NULL},
   /* ActChgCtrPhoAtt	*/{1159,-1,TabUnk,ActSeeCtrInf		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ctr_ChangeCtrPhotoAttribution	,NULL},

   /* ActSeeDegTyp	*/{1013,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,DT_SeeDegreeTypesInDegTab	,NULL},
   /* ActEdiDegTyp	*/{ 573,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,DT_EditDegreeTypes		,NULL},
   /* ActNewDegTyp	*/{ 537,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,DT_RecFormNewDegreeType	,NULL},
   /* ActRemDegTyp	*/{ 545,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,DT_RemoveDegreeType		,NULL},
   /* ActRenDegTyp	*/{ 538,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,DT_RenameDegreeType		,NULL},

   /* ActEdiDeg		*/{ 536,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_EditDegrees		,NULL},
   /* ActReqDeg		*/{1206,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_RecFormReqDeg		,NULL},
   /* ActNewDeg		*/{ 540,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_RecFormNewDeg		,NULL},
   /* ActRemDeg		*/{ 542,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_RemoveDegree		,NULL},
   /* ActRenDegSho	*/{ 546,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Deg_RenameDegreeShort		,Deg_ContEditAfterChgDeg	,NULL},
   /* ActRenDegFul	*/{ 547,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Deg_RenameDegreeFull		,Deg_ContEditAfterChgDeg	,NULL},
   /* ActChgDegTyp	*/{ 544,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,DT_ChangeDegreeType		,NULL},
   /* ActChgDegWWW	*/{ 554,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_ChangeDegWWW		,NULL},
   /* ActChgDegSta	*/{1207,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_ChangeDegStatus		,NULL},

   /* ActPrnCalCtr	*/{1632,-1,TabUnk,ActSeeCalCtr		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Cal_PrintCalendar		,NULL},
   /* ActChgCalCtr1stDay*/{1633,-1,TabUnk,ActSeeCalCtr		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,Cal_DrawCalendarCtr		,NULL},

   // TabDeg ******************************************************************
   // Actions in menu:
   /* ActDegReqSch	*/{1179, 0,TabDeg,ActDegReqSch		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sch_ReqDegSearch		,"search64x64.gif"	},
   /* ActSeeDegInf	*/{1149, 1,TabDeg,ActSeeDegInf		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_ShowConfiguration		,"info64x64.gif"	},
   /* ActSeeCrs		*/{1009, 2,TabDeg,ActSeeCrs		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Crs_ShowCrssOfCurrentDeg	,"coursesdegree64x64.gif"},
   /* ActSeeCalDeg	*/{1634, 3,TabDeg,ActSeeCalDeg		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cal_DrawCalendarDeg		,"calendar64x64.png"	},

   // Actions not in menu:
   /* ActDegSch		*/{1184,-1,TabUnk,ActDegReqSch		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Sch_GetParamsSearch		,Sch_DegSearch			,NULL},

   /* ActPrnDegInf	*/{1150,-1,TabUnk,ActSeeDegInf		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Deg_PrintConfiguration		,NULL},
   /* ActChgDegCtrCfg	*/{1588,-1,TabUnk,ActSeeDegInf		,    0,    0,0x300,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Deg_ChangeDegCtrInConfig	,Deg_ContEditAfterChgDegInConfig,NULL},
   /* ActRenDegShoCfg	*/{1598,-1,TabUnk,ActSeeDegInf		,    0,    0,0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Deg_RenameDegreeShortInConfig	,Deg_ContEditAfterChgDegInConfig,NULL},
   /* ActRenDegFulCfg	*/{1597,-1,TabUnk,ActSeeDegInf		,    0,    0,0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Deg_RenameDegreeFullInConfig	,Deg_ContEditAfterChgDegInConfig,NULL},
   /* ActChgDegWWWCfg	*/{1599,-1,TabUnk,ActSeeDegInf		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_ChangeDegWWWInConfig	,NULL},
   /* ActReqDegLog	*/{1246,-1,TabUnk,ActSeeDegInf		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Deg_RequestLogo		,NULL},
   /* ActRecDegLog	*/{ 553,-1,TabUnk,ActSeeDegInf		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,Deg_ReceiveLogo		,Deg_ShowConfiguration		,NULL},
   /* ActRemDegLog	*/{1343,-1,TabUnk,ActSeeDegInf		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,Deg_RemoveLogo			,Deg_ShowConfiguration		,NULL},

   /* ActEdiCrs		*/{ 555,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Crs_EditCourses		,NULL},
   /* ActReqCrs		*/{1053,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_RecFormReqCrs		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActNewCrs		*/{ 556,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_RecFormNewCrs		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActRemCrs		*/{ 560,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Crs_RemoveCourse		,NULL},
   /* ActChgInsCrsCod	*/{1025,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_ChangeInsCrsCod		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActChgCrsYea	*/{ 561,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_ChangeCrsYear		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActRenCrsSho	*/{ 563,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_RenameCourseShort		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActRenCrsFul	*/{ 564,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_RenameCourseFull		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActChgCrsSta	*/{1055,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_ChangeCrsStatus		,Crs_ContEditAfterChgCrs	,NULL},

   /* ActPrnCalDeg	*/{1635,-1,TabUnk,ActSeeCalDeg		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Cal_PrintCalendar		,NULL},
   /* ActChgCalDeg1stDay*/{1636,-1,TabUnk,ActSeeCalDeg		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,Cal_DrawCalendarDeg		,NULL},

   // TabCrs ******************************************************************
   // Actions in menu:
   /* ActCrsReqSch	*/{1180, 0,TabCrs,ActCrsReqSch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sch_ReqCrsSearch		,"search64x64.gif"	},
   /* ActSeeCrsInf	*/{ 847, 1,TabCrs,ActSeeCrsInf		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Crs_ShowIntroduction		,"info64x64.gif"	},
   /* ActSeeTchGui	*/{ 784, 2,TabCrs,ActSeeTchGui		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,"guide64x64.gif"	},
   /* ActSeeSyl		*/{1242, 3,TabCrs,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,"presentation64x64.gif"},
   /* ActSeeBib		*/{  32, 4,TabCrs,ActSeeBib		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,"books64x64.gif"	},
   /* ActSeeFAQ		*/{  54, 5,TabCrs,ActSeeFAQ		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,"faq64x64.gif"		},
   /* ActSeeCrsLnk	*/{   9, 6,TabCrs,ActSeeCrsLnk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,"link64x64.gif"	},
   /* ActSeeCrsTT	*/{  25, 7,TabCrs,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,TT_ShowClassTimeTable		,"clock64x64.gif"	},
   /* ActSeeCalCrs	*/{  16, 8,TabCrs,ActSeeCalCrs		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cal_DrawCalendarCrs		,"calendar64x64.png"	},

   // Actions not in menu:
   /* ActCrsSch		*/{1185,-1,TabUnk,ActCrsReqSch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Sch_GetParamsSearch		,Sch_CrsSearch			,NULL},

   /* ActPrnCrsInf	*/{1028,-1,TabUnk,ActSeeCrsInf		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Crs_PrintConfiguration		,NULL},
   /* ActChgCrsDegCfg	*/{1587,-1,TabUnk,ActSeeCrsInf		,0x380,0x380,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_ChangeCrsDegInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActRenCrsShoCfg	*/{1601,-1,TabUnk,ActSeeCrsInf		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_RenameCourseShortInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActRenCrsFulCfg	*/{1600,-1,TabUnk,ActSeeCrsInf		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_RenameCourseFullInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActChgInsCrsCodCfg*/{1024,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_ChangeInsCrsCodInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActChgCrsYeaCfg	*/{1573,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Crs_ChangeCrsYearInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActEdiCrsInf	*/{ 848,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},

   /* ActEdiTchGui	*/{ 785,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},

   /* ActSeeSylLec	*/{  28,-1,TabUnk,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,NULL},
   /* ActSeeSylPra	*/{  20,-1,TabUnk,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,NULL},
   /* ActEdiSylLec	*/{  44,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},
   /* ActEdiSylPra	*/{  74,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},
   /* ActDelItmSylLec	*/{ 218,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_RemoveItemSyllabus		,NULL},
   /* ActDelItmSylPra	*/{ 183,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_RemoveItemSyllabus		,NULL},
   /* ActUp_IteSylLec	*/{ 221,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_UpItemSyllabus		,NULL},
   /* ActUp_IteSylPra	*/{ 213,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_UpItemSyllabus		,NULL},
   /* ActDwnIteSylLec	*/{ 220,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_DownItemSyllabus		,NULL},
   /* ActDwnIteSylPra	*/{ 212,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_DownItemSyllabus		,NULL},
   /* ActRgtIteSylLec	*/{ 223,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_RightItemSyllabus		,NULL},
   /* ActRgtIteSylPra	*/{ 215,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_RightItemSyllabus		,NULL},
   /* ActLftIteSylLec	*/{ 222,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_LeftItemSyllabus		,NULL},
   /* ActLftIteSylPra	*/{ 214,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_LeftItemSyllabus		,NULL},
   /* ActInsIteSylLec	*/{ 217,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_InsertItemSyllabus		,NULL},
   /* ActInsIteSylPra	*/{ 181,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_InsertItemSyllabus		,NULL},
   /* ActModIteSylLec	*/{ 211,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_ModifyItemSyllabus		,NULL},
   /* ActModIteSylPra	*/{ 216,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_ModifyItemSyllabus		,NULL},

   /* ActEdiBib		*/{  76,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},
   /* ActEdiFAQ		*/{ 109,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},
   /* ActEdiCrsLnk	*/{  96,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},

   /* ActChgFrcReaCrsInf*/{ 877,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaTchGui*/{ 870,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaSylLec*/{ 871,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaSylPra*/{ 872,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaBib	*/{ 873,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaFAQ	*/{ 874,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaCrsLnk*/{ 875,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},

   /* ActChgHavReaCrsInf*/{ 878,-1,TabUnk,ActSeeCrsInf		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaTchGui*/{ 886,-1,TabUnk,ActSeeTchGui		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaSylLec*/{ 880,-1,TabUnk,ActSeeSyl		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaSylPra*/{ 887,-1,TabUnk,ActSeeSyl		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaBib	*/{ 884,-1,TabUnk,ActSeeBib		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaFAQ	*/{ 879,-1,TabUnk,ActSeeFAQ		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaCrsLnk*/{ 885,-1,TabUnk,ActSeeCrsLnk		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},

   /* ActSelInfSrcCrsInf*/{ 849,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcTchGui*/{ 789,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcSylLec*/{ 378,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcSylPra*/{ 382,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcBib	*/{ 370,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcFAQ	*/{ 380,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcCrsLnk*/{ 385,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},

   /* ActRcvURLCrsInf	*/{ 854,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLTchGui	*/{ 791,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLSylLec	*/{ 403,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLSylPra	*/{ 402,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLBib	*/{ 224,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLFAQ	*/{ 234,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLCrsLnk	*/{ 182,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},

   /* ActRcvPagCrsInf	*/{ 853,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagTchGui	*/{ 788,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagSylLec	*/{ 381,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagSylPra	*/{ 383,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagBib	*/{ 185,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagFAQ	*/{ 219,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagCrsLnk	*/{ 164,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},

   /* ActEditorCrsInf	*/{ 852,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditorCourseInfo		,NULL},
   /* ActEditorTchGui	*/{ 786,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditorTeachingGuide	,NULL},
   /* ActEditorSylLec	*/{ 372,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_EditSyllabus		,NULL},
   /* ActEditorSylPra	*/{ 371,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Syl_EditSyllabus		,NULL},
   /* ActEditorBib	*/{ 376,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditorBibliography		,NULL},
   /* ActEditorFAQ	*/{ 404,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditorFAQ   		,NULL},
   /* ActEditorCrsLnk	*/{ 388,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditorLinks		,NULL},

   /* ActPlaTxtEdiCrsInf*/{ 850,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiTchGui*/{ 787,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiSylLec*/{ 379,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiSylPra*/{ 389,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiBib	*/{ 377,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiFAQ	*/{ 405,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo   	,NULL},
   /* ActPlaTxtEdiCrsLnk*/{ 400,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo		,NULL},

   /* ActRchTxtEdiCrsInf*/{1093,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiTchGui*/{1094,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiSylLec*/{1095,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiSylPra*/{1096,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiBib	*/{1097,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiFAQ	*/{1098,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo   		,NULL},
   /* ActRchTxtEdiCrsLnk*/{1099,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo		,NULL},

   /* ActRcvPlaTxtCrsInf*/{ 851,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtTchGui*/{ 790,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtSylLec*/{ 394,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtSylPra*/{ 396,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtBib	*/{ 398,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtFAQ	*/{ 406,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtCrsLnk*/{ 401,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},

   /* ActRcvRchTxtCrsInf*/{1101,-1,TabUnk,ActSeeCrsInf		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtTchGui*/{1102,-1,TabUnk,ActSeeTchGui		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtSylLec*/{1103,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtSylPra*/{1104,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtBib	*/{1105,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtFAQ	*/{1106,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtCrsLnk*/{1107,-1,TabUnk,ActSeeCrsLnk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},

   /* ActPrnCrsTT	*/{ 152,-1,TabUnk,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,TT_ShowClassTimeTable		,NULL},
   /* ActEdiCrsTT	*/{  45,-1,TabUnk,ActSeeCrsTT		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,TT_EditCrsTimeTable		,NULL},
   /* ActChgCrsTT	*/{  53,-1,TabUnk,ActSeeCrsTT		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,TT_EditCrsTimeTable		,NULL},
   /* ActChgCrsTT1stDay	*/{1486,-1,TabUnk,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,TT_ShowClassTimeTable		,NULL},

   /* ActPrnCalCrs	*/{  71,-1,TabUnk,ActSeeCalCrs		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Cal_PrintCalendar		,NULL},
   /* ActChgCalCrs1stDay*/{1485,-1,TabUnk,ActSeeCalCrs		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,Cal_DrawCalendarCrs		,NULL},

   // TabAss ******************************************************************
   // Actions in menu:
   /* ActSeeAss		*/{  15, 0,TabAss,ActSeeAss		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ShowInfo			,"infogrades64x64.gif"	},
   /* ActSeeAsg		*/{ 801, 1,TabAss,ActSeeAsg		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_SeeAssignments		,"desk64x64.gif"	},
   /* ActReqTst		*/{ 103, 2,TabAss,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowFormAskTst		,"test64x64.gif"	},
   /* ActSeeAllSvy	*/{ 966, 3,TabAss,ActSeeAllSvy		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_SeeAllSurveys		,"survey64x64.gif"	},
   /* ActSeeAllExaAnn	*/{  85, 4,TabAss,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Exa_ListExamAnnouncementsSee	,"announce64x64.gif"	},

   // Actions not in menu:
   /* ActEdiAss		*/{  69,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_FormsToSelSendInfo		,NULL},
   /* ActChgFrcReaAss	*/{ 883,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgHavReaAss	*/{ 898,-1,TabUnk,ActSeeAss		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActSelInfSrcAss	*/{ 384,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_SetInfoSrc			,NULL},
   /* ActRcvURLAss	*/{ 235,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvPagAss	*/{ 184,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Inf_ReceivePagInfo		,NULL},
   /* ActEditorAss	*/{ 386,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditorAssessment		,NULL},
   /* ActPlaTxtEdiAss	*/{ 387,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditPlainTxtInfo		,NULL},
   /* ActRchTxtEdiAss	*/{1100,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_EditRichTxtInfo		,NULL},
   /* ActRcvPlaTxtAss	*/{ 397,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvRchTxtAss	*/{1108,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},

   /* ActFrmNewAsg	*/{ 812,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_RequestCreatOrEditAsg	,NULL},
   /* ActEdiOneAsg	*/{ 814,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_RequestCreatOrEditAsg	,NULL},
   /* ActPrnOneAsg	*/{1637,-1,TabUnk,ActSeeAsg		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Asg_PrintOneAssignment		,NULL},
   /* ActNewAsg		*/{ 803,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_RecFormAssignment		,NULL},
   /* ActChgAsg		*/{ 815,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_RecFormAssignment		,NULL},
   /* ActReqRemAsg	*/{ 813,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_ReqRemAssignment		,NULL},
   /* ActRemAsg		*/{ 806,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_RemoveAssignment		,NULL},
   /* ActHidAsg		*/{ 964,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_HideAssignment		,NULL},
   /* ActShoAsg		*/{ 965,-1,TabUnk,ActSeeAsg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Asg_ShowAssignment		,NULL},

   /* ActSeeTst		*/{  29,-1,TabUnk,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowNewTest		,NULL},
   /* ActAssTst		*/{  98,-1,TabUnk,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_AssessTest			,NULL},

   /* ActEdiTstQst	*/{ 104,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Tst_SetIniEndDates		,Tst_ShowFormAskEditTsts	,NULL},
   /* ActEdiOneTstQst	*/{ 105,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowFormEditOneQst		,NULL},
   /* ActReqImpTstQst	*/{1007,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,TsI_ShowFormImportQstsFromXML	,NULL},
   /* ActImpTstQst	*/{1008,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,TsI_ImportQstsFromXML		,NULL},
   /* ActLstTstQst	*/{ 132,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ListQuestionsToEdit	,NULL},
   /* ActRcvTstQst	*/{ 126,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Tst_ReceiveQst			,NULL},
   /* ActReqRemTstQst	*/{1523,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_RequestRemoveQst		,NULL},
   /* ActRemTstQst	*/{ 133,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_RemoveQst			,NULL},
   /* ActShfTstQst	*/{ 455,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ChangeShuffleQst		,NULL},

   /* ActCfgTst		*/{ 451,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowFormConfig		,NULL},
   /* ActEnableTag	*/{ 453,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_EnableTag			,NULL},
   /* ActDisableTag	*/{ 452,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_DisableTag			,NULL},
   /* ActRenTag		*/{ 143,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_RenameTag			,NULL},
   /* ActRcvCfgTst	*/{ 454,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ReceiveConfigTst		,NULL},

   /* ActReqSeeMyTstRes	*/{1083,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Tst_SetIniEndDates		,Tst_SelDatesToSeeMyTestResults	,NULL},
   /* ActSeeMyTstRes	*/{1084,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowMyTestResults		,NULL},
   /* ActSeeOneTstResMe	*/{1085,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowOneTestResult		,NULL},
   /* ActReqSeeUsrTstRes*/{1080,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Tst_SetIniEndDates		,Tst_SelUsrsToSeeUsrsTestResults,NULL},
   /* ActSeeUsrTstRes	*/{1081,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowUsrsTestResults	,NULL},
   /* ActSeeOneTstResOth*/{1082,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Tst_ShowOneTestResult		,NULL},

   /* ActSeeOneSvy	*/{ 982,-1,TabUnk,ActSeeAllSvy		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_SeeOneSurvey		,NULL},
   /* ActAnsSvy		*/{ 983,-1,TabUnk,ActSeeAllSvy		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_ReceiveSurveyAnswers	,NULL},
   /* ActFrmNewSvy	*/{ 973,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RequestCreatOrEditSvy	,NULL},
   /* ActEdiOneSvy	*/{ 974,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RequestCreatOrEditSvy	,NULL},
   /* ActNewSvy		*/{ 968,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RecFormSurvey		,NULL},
   /* ActChgSvy		*/{ 975,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RecFormSurvey		,NULL},
   /* ActReqRemSvy	*/{ 976,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_AskRemSurvey		,NULL},
   /* ActRemSvy		*/{ 969,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RemoveSurvey		,NULL},
   /* ActReqRstSvy	*/{ 984,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_AskResetSurvey		,NULL},
   /* ActRstSvy		*/{ 985,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_ResetSurvey		,NULL},
   /* ActHidSvy		*/{ 977,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_HideSurvey			,NULL},
   /* ActShoSvy		*/{ 978,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_UnhideSurvey		,NULL},
   /* ActEdiOneSvyQst	*/{ 979,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RequestEditQuestion	,NULL},
   /* ActRcvSvyQst	*/{ 980,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_ReceiveQst			,NULL},
   /* ActReqRemSvyQst	*/{1524,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RequestRemoveQst		,NULL},
   /* ActRemSvyQst	*/{ 981,-1,TabUnk,ActSeeAllSvy		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Svy_RemoveQst			,NULL},

   /* ActSeeOneExaAnn	*/{1572,-1,TabUnk,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Exa_GetExaCodToHighlight	,Exa_ListExamAnnouncementsSee	,NULL},
   /* ActSeeDatExaAnn	*/{1571,-1,TabUnk,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Exa_GetDateToHighlight		,Exa_ListExamAnnouncementsSee	,NULL},
   /* ActEdiExaAnn	*/{  91,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Exa_PutFrmEditAExamAnnouncement,NULL},
   /* ActRcvExaAnn	*/{ 110,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Exa_ReceiveExamAnnouncement1	,Exa_ReceiveExamAnnouncement2	,NULL},
   /* ActPrnExaAnn	*/{ 179,-1,TabUnk,ActSeeAllExaAnn	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Exa_PrintExamAnnouncement	,NULL},
   /* ActReqRemExaAnn	*/{1619,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Exa_ReqRemoveExamAnnouncement	,NULL},
   /* ActRemExaAnn	*/{ 187,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Exa_RemoveExamAnnouncement1	,Exa_RemoveExamAnnouncement2	,NULL},
   /* ActHidExaAnn	*/{1620,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Exa_HideExamAnnouncement1	,Exa_HideExamAnnouncement2	,NULL},
   /* ActShoExaAnn	*/{1621,-1,TabUnk,ActSeeAllExaAnn	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Exa_UnhideExamAnnouncement1	,Exa_UnhideExamAnnouncement2	,NULL},

   // TabFil ******************************************************************
   // Actions in menu:
   /* ActSeeAdmDocIns	*/{1249, 0,TabFil,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaIns	*/{1382, 1,TabFil,ActAdmShaIns		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},
   /* ActSeeAdmDocCtr	*/{1248, 2,TabFil,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaCtr	*/{1363, 3,TabFil,ActAdmShaCtr		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},
   /* ActSeeAdmDocDeg	*/{1247, 4,TabFil,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaDeg	*/{1344, 5,TabFil,ActAdmShaDeg		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},
   /* ActSeeAdmDocCrsGrp*/{   0, 6,TabFil,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmTchCrsGrp	*/{1525, 7,TabFil,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaCrsGrp	*/{ 461, 8,TabFil,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},
   /* ActAdmAsgWrkUsr	*/{ 792, 9,TabFil,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"editfolderuser64x64.gif"},
   /* ActReqAsgWrkCrs	*/{ 899,10,TabFil,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskEditWorksCrs		,"folderusers64x64.gif"	},
   /* ActSeeAdmMrk	*/{  17,11,TabFil,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"grades64x64.gif"	},

   // Actions not in menu:
   /* ActChgToSeeDocIns	*/{1308,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocIns	*/{1309,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocIns	*/{1310,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocIns	*/{1311,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocIns	*/{1312,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocIns*/{1313,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocIns   */{1314,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocIns	*/{1315,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocIns	*/{1316,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocIns*/{1317,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocIns	*/{1318,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocIns	*/{1319,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocIns	*/{1320,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocIns	*/{1321,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocIns	*/{1322,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocIns	*/{1323,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocIns	*/{1324,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocIns	*/{1325,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocIns	*/{1326,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocInsDZ	*/{1327,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilDocInsCla*/{1328,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocIns	*/{1329,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocIns	*/{1330,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocIns	*/{1331,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActShoDocIns	*/{1332,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocIns	*/{1333,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocIns*/{1334,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocIns*/{1335,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocIns   */{1336,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilShaIns*/{1383,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaIns	*/{1384,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaIns	*/{1385,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaIns	*/{1386,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaIns	*/{1387,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaIns	*/{1388,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaIns	*/{1389,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaIns	*/{1390,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaIns	*/{1391,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaIns	*/{1392,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaInsDZ	*/{1393,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilShaInsCla*/{1394,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaIns	*/{1395,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConShaIns	*/{1396,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPShaIns	*/{1397,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaIns	*/{1398,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaIns	*/{1399,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaIns      */{1400,-1,TabUnk,ActAdmShaIns		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToSeeDocCtr	*/{1279,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocCtr	*/{1280,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocCtr	*/{1281,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocCtr	*/{1282,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocCtr	*/{1283,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocCtr*/{1284,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocCtr   */{1285,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocCtr	*/{1286,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocCtr	*/{1287,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocCtr*/{1288,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocCtr	*/{1289,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocCtr	*/{1290,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocCtr	*/{1291,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocCtr	*/{1292,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocCtr	*/{1293,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocCtr	*/{1294,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocCtr	*/{1295,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocCtr	*/{1296,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocCtr	*/{1297,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocCtrDZ	*/{1298,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilDocCtrCla*/{1299,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocCtr	*/{1300,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocCtr	*/{1301,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocCtr	*/{1302,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActShoDocCtr	*/{1303,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocCtr	*/{1304,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocCtr*/{1305,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocCtr*/{1306,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocCtr   */{1307,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilShaCtr*/{1364,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaCtr	*/{1365,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaCtr	*/{1366,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaCtr	*/{1367,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaCtr	*/{1368,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaCtr	*/{1369,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaCtr	*/{1370,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaCtr	*/{1371,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaCtr	*/{1372,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaCtr	*/{1373,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaCtrDZ	*/{1374,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilShaCtrCla*/{1375,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaCtr	*/{1376,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConShaCtr	*/{1377,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPShaCtr	*/{1378,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaCtr	*/{1379,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaCtr	*/{1380,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaCtr      */{1381,-1,TabUnk,ActAdmShaCtr		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToSeeDocDeg	*/{1250,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocDeg	*/{1251,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocDeg	*/{1252,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocDeg	*/{1253,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocDeg	*/{1254,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocDeg*/{1255,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocDeg   */{1256,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocDeg	*/{1257,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocDeg	*/{1258,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocDeg*/{1259,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocDeg	*/{1260,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocDeg	*/{1261,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocDeg	*/{1262,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocDeg	*/{1263,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocDeg	*/{1264,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocDeg	*/{1265,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocDeg	*/{1266,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocDeg	*/{1267,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocDeg	*/{1268,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocDegDZ	*/{1269,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilDocDegCla*/{1270,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocDeg	*/{1271,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocDeg	*/{1272,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocDeg	*/{1273,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActShoDocDeg	*/{1274,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocDeg	*/{1275,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocDeg*/{1276,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocDeg*/{1277,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocDeg   */{1278,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilShaDeg*/{1345,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaDeg	*/{1346,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaDeg	*/{1347,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaDeg	*/{1348,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaDeg	*/{1349,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaDeg	*/{1350,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaDeg	*/{1351,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaDeg	*/{1352,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaDeg	*/{1353,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaDeg	*/{1354,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaDegDZ	*/{1355,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilShaDegCla*/{1356,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaDeg	*/{1357,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConShaDeg	*/{1358,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPShaDeg	*/{1359,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaDeg	*/{1360,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaDeg	*/{1361,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaDeg      */{1362,-1,TabUnk,ActAdmShaDeg		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToSeeDocCrs	*/{1195,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocCrs	*/{1078,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocCrs	*/{ 462,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocCrs	*/{ 476,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocCrs	*/{1124,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocCrs*/{1033,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocCrs   */{1111,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActSeeDocGrp	*/{1200,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocGrp	*/{ 488,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocGrp	*/{ 489,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocGrp	*/{1125,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocGrp*/{1034,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocGrp   */{1112,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocCrs	*/{1196,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocCrs	*/{  12,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocCrs*/{ 479,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocCrs	*/{ 480,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocCrs	*/{ 497,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocCrs	*/{ 470,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocCrs	*/{ 478,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocCrs	*/{ 498,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocCrs	*/{ 481,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocCrs	*/{ 491,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocCrs	*/{1225,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocCrs	*/{ 535,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocCrsDZ	*/{1214,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilDocCrsCla*/{ 482,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocCrs	*/{ 477,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocCrs	*/{ 494,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocCrs	*/{1126,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActShoDocCrs	*/{ 464,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocCrs	*/{ 465,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocCrs*/{1029,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocCrs*/{ 996,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocCrs   */{1113,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActAdmDocGrp	*/{1201,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocGrp*/{ 473,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocGrp	*/{ 474,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocGrp	*/{ 484,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocGrp	*/{ 472,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocGrp	*/{ 471,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocGrp	*/{ 485,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocGrp	*/{ 468,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocGrp	*/{ 469,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocGrp	*/{1231,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocGrp	*/{ 490,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocGrpDZ	*/{1215,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL			,NULL},
   /* ActRcvFilDocGrpCla*/{ 483,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocGrp	*/{ 486,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocGrp	*/{ 487,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocGrp	*/{1127,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActShoDocGrp	*/{ 493,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocGrp	*/{ 492,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocGrp*/{1030,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocGrp*/{ 998,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocGrp   */{1114,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmTch	*/{1526,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmTchCrs	*/{1527,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilTchCrs*/{1528,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilTchCrs	*/{1529,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolTchCrs	*/{1530,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopTchCrs	*/{1531,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasTchCrs	*/{1532,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreTchCrs	*/{1533,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreTchCrs	*/{1534,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolTchCrs	*/{1535,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkTchCrs	*/{1536,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolTchCrs	*/{1537,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilTchCrsDZ	*/{1538,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilTchCrsCla*/{1539,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpTchCrs	*/{1540,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConTchCrs	*/{1541,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPTchCrs	*/{1542,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatTchCrs	*/{1543,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatTchCrs	*/{1544,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowTchCrs      */{1545,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActAdmTchGrp	*/{1546,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilTchGrp*/{1547,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilTchGrp	*/{1548,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolTchGrp	*/{1549,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopTchGrp	*/{1550,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasTchGrp	*/{1551,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreTchGrp	*/{1552,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreTchGrp	*/{1553,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolTchGrp	*/{1554,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkTchGrp	*/{1555,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolTchGrp	*/{1556,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilTchGrpDZ	*/{1557,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilTchGrpCla*/{1558,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpTchGrp	*/{1559,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConTchGrp	*/{1560,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPTchGrp	*/{1561,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatTchGrp	*/{1562,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatTchGrp	*/{1563,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowTchGrp      */{1564,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmSha	*/{1197,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmShaCrs	*/{1202,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilShaCrs*/{ 327,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaCrs	*/{ 328,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaCrs	*/{ 325,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaCrs	*/{ 330,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaCrs	*/{ 331,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaCrs	*/{ 332,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaCrs	*/{ 323,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaCrs	*/{ 324,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaCrs	*/{1226,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaCrs	*/{ 329,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaCrsDZ	*/{1216,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilShaCrsCla*/{ 326,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaCrs	*/{ 421,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConShaCrs	*/{ 422,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPShaCrs	*/{1128,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaCrs	*/{1031,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaCrs	*/{1000,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaCrs      */{1115,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActAdmShaGrp	*/{1203,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilShaGrp*/{ 341,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaGrp	*/{ 342,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaGrp	*/{ 338,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaGrp	*/{ 336,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaGrp	*/{ 337,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaGrp	*/{ 339,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaGrp	*/{ 333,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaGrp	*/{ 334,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaGrp	*/{1227,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaGrp	*/{ 340,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaGrpDZ	*/{1217,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilShaGrpCla*/{ 335,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaGrp	*/{ 427,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConShaGrp	*/{ 426,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPShaGrp	*/{1129,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaGrp	*/{1032,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaGrp	*/{1002,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaGrp      */{1116,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActAdmAsgWrkCrs	*/{ 139,-1,TabUnk,ActReqAsgWrkCrs	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActReqRemFilAsgUsr*/{ 834,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilAsgUsr	*/{ 833,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolAsgUsr	*/{ 827,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopAsgUsr	*/{ 829,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasAsgUsr	*/{ 830,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreAsgUsr	*/{ 828,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreAsgUsr	*/{ 825,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolAsgUsr	*/{ 826,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkAsgUsr	*/{1232,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolAsgUsr	*/{ 839,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilAsgUsrDZ	*/{1218,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilAsgUsrCla*/{ 832,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAsgUsr	*/{ 824,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAsgUsr	*/{ 831,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAsgUsr	*/{1130,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatAsgUsr	*/{1039,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAsgUsr	*/{1040,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAsgUsr      */{1117,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilWrkUsr*/{ 288,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilWrkUsr	*/{ 169,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolWrkUsr	*/{ 228,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopWrkUsr	*/{ 314,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasWrkUsr	*/{ 318,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreWrkUsr	*/{ 278,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreWrkUsr	*/{ 150,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolWrkUsr	*/{ 172,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkWrkUsr	*/{1228,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolWrkUsr	*/{ 204,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilWrkUsrDZ	*/{1219,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilWrkUsrCla*/{ 148,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpWrkUsr	*/{ 423,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConWrkUsr	*/{ 425,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPWrkUsr	*/{1131,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatWrkUsr	*/{1041,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatWrkUsr	*/{1042,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowWrkUsr      */{1118,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilAsgCrs*/{ 837,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilAsgCrs	*/{ 838,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolAsgCrs	*/{ 820,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopAsgCrs	*/{ 836,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasAsgCrs	*/{ 821,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreAsgCrs	*/{ 822,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreAsgCrs	*/{ 817,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolAsgCrs	*/{ 818,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkAsgCrs	*/{1233,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolAsgCrs	*/{ 823,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilAsgCrsDZ	*/{1220,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilAsgCrsCla*/{ 846,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAsgCrs	*/{ 819,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAsgCrs	*/{ 835,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAsgCrs	*/{1132,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatAsgCrs	*/{1043,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAsgCrs	*/{1044,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAsgCrs      */{1119,-1,TabUnk,ActReqAsgWrkCrs	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilWrkCrs*/{ 289,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilWrkCrs	*/{ 209,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolWrkCrs	*/{ 210,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopWrkCrs	*/{ 312,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasWrkCrs	*/{ 319,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreWrkCrs	*/{ 279,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreWrkCrs	*/{ 205,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolWrkCrs	*/{ 206,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkWrkCrs	*/{1229,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolWrkCrs	*/{ 208,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilWrkCrsDZ	*/{1221,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilWrkCrsCla*/{ 207,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpWrkCrs	*/{ 416,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConWrkCrs	*/{ 424,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPWrkCrs	*/{1133,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatWrkCrs	*/{1045,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatWrkCrs	*/{1046,-1,TabUnk,ActReqAsgWrkCrs	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowWrkCrs      */{1120,-1,TabUnk,ActReqAsgWrkCrs	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToSeeMrk	*/{1198,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeMrkCrs	*/{1079,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeMrkCrs	*/{ 528,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConSeeMrkCrs	*/{ 527,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActReqDatSeeMrkCrs*/{1086,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActSeeMyMrkCrs	*/{ 523,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,Mrk_ShowMyMarks		,NULL				,NULL},

   /* ActSeeMrkGrp	*/{1204,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeMrkGrp	*/{ 605,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConSeeMrkGrp	*/{ 609,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActReqDatSeeMrkGrp*/{1087,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActSeeMyMrkGrp	*/{ 524,-1,TabUnk,ActSeeAdmMrk		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,Mrk_ShowMyMarks		,NULL				,NULL},

   /* ActChgToAdmMrk	*/{1199,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmMrkCrs	*/{ 284,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilMrkCrs*/{ 595,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilMrkCrs	*/{ 533,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolMrkCrs	*/{ 530,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopMrkCrs	*/{ 501,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasMrkCrs	*/{ 507,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreMrkCrs	*/{ 534,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreMrkCrs	*/{ 596,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolMrkCrs	*/{ 506,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActRenFolMrkCrs	*/{ 574,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilMrkCrsDZ	*/{1222,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilMrkCrsCla*/{ 516,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmMrkCrs	*/{ 607,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAdmMrkCrs	*/{ 621,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmMrkCrs	*/{1134,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActShoMrkCrs	*/{1191,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidMrkCrs	*/{1192,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmMrkCrs*/{1035,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmMrkCrs*/{1036,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmMrkCrs   */{1121,-1,TabUnk,ActSeeAdmMrk		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   /* ActChgNumRowHeaCrs*/{ 503,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mrk_ChangeNumRowsHeader	,NULL},
   /* ActChgNumRowFooCrs*/{ 504,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mrk_ChangeNumRowsFooter	,NULL},

   /* ActAdmMrkGrp	*/{1205,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilMrkGrp*/{ 600,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilMrkGrp	*/{ 509,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolMrkGrp	*/{ 520,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopMrkGrp	*/{ 519,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasMrkGrp	*/{ 502,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreMrkGrp	*/{ 521,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreMrkGrp	*/{ 601,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolMrkGrp	*/{ 513,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActRenFolMrkGrp	*/{ 529,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilMrkGrpDZ	*/{1223,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilMrkGrpCla*/{ 514,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmMrkGrp	*/{ 631,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConAdmMrkGrp	*/{ 900,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmMrkGrp	*/{1135,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActShoMrkGrp	*/{1193,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidMrkGrp	*/{1194,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmMrkGrp*/{1037,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmMrkGrp*/{1038,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmMrkGrp   */{1122,-1,TabUnk,ActSeeAdmMrk		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   /* ActChgNumRowHeaGrp*/{ 510,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mrk_ChangeNumRowsHeader	,NULL},
   /* ActChgNumRowFooGrp*/{ 511,-1,TabUnk,ActSeeAdmMrk		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mrk_ChangeNumRowsFooter	,NULL},

   // TabUsr ******************************************************************
   // Actions in menu:
   /* ActReqSelGrp	*/{ 116, 0,TabUsr,ActReqSelGrp		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ReqRegisterInGrps		,"hierarchy64x64.png"	},
   /* ActLstStd		*/{ 678, 1,TabUsr,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_SeeStudents		,"usrs64x64.gif"	},
   /* ActLstTch		*/{ 679, 2,TabUsr,ActLstTch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_SeeTeachers		,"tch64x64.gif"		},
   /* ActLstOth		*/{1186, 3,TabUsr,ActLstOth		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_ListDataAdms		,"adm64x64.gif"		},
   /* ActSeeAtt		*/{ 861, 4,TabUsr,ActSeeAtt		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_SeeAttEvents		,"rollcall64x64.png"	},
   /* ActReqSignUp	*/{1054, 5,TabUsr,ActReqSignUp		,    0,0x006,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqSignUpInCrs		,"enrollmentrequest64x64.gif"},
   /* ActSeeSignUpReq	*/{1057, 6,TabUsr,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ShowEnrolmentRequests	,"enrollmentrequest64x64.gif"},
   /* ActLstCon		*/{ 995, 7,TabUsr,ActLstCon		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Con_ShowConnectedUsrs		,"userplugged64x64.gif"	},

   // Actions not in menu:
   /* ActChgGrp		*/{ 118,-1,TabUnk,ActReqSelGrp		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ChangeMyGrpsAndShowChanges	,NULL},

   /* ActReqEdiGrp	*/{ 108,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ReqEditGroups		,NULL},

   /* ActNewGrpTyp	*/{ 174,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_RecFormNewGrpTyp		,NULL},
   /* ActReqRemGrpTyp	*/{ 236,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ReqRemGroupType		,NULL},
   /* ActRemGrpTyp	*/{ 237,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_RemoveGroupType		,NULL},
   /* ActRenGrpTyp	*/{ 304,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_RenameGroupType		,NULL},
   /* ActChgMdtGrpTyp	*/{ 303,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ChangeMandatGrpTyp		,NULL},
   /* ActChgMulGrpTyp	*/{ 302,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ChangeMultiGrpTyp		,NULL},
   /* ActChgTimGrpTyp	*/{1061,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ChangeOpenTimeGrpTyp	,NULL},

   /* ActNewGrp		*/{ 122,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_RecFormNewGrp		,NULL},
   /* ActReqRemGrp	*/{ 107,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ReqRemGroup		,NULL},
   /* ActRemGrp		*/{ 175,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_RemoveGroup		,NULL},
   /* ActOpeGrp		*/{ 322,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_OpenGroup			,NULL},
   /* ActCloGrp		*/{ 321,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_CloseGroup			,NULL},
   /* ActEnaFilZonGrp	*/{ 495,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_EnableFileZonesGrp		,NULL},
   /* ActDisFilZonGrp	*/{ 496,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_DisableFileZonesGrp	,NULL},
   /* ActChgGrpTyp	*/{ 167,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ChangeGroupType		,NULL},
   /* ActRenGrp		*/{ 121,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_RenameGroup		,NULL},
   /* ActChgMaxStdGrp	*/{ 106,-1,TabUnk,ActReqSelGrp		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Grp_ChangeMaxStdsGrp		,NULL},

   /* ActLstGst		*/{ 587,-1,TabUnk,ActLstOth		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_SeeGuests			,NULL},

   /* ActPrnGstPho	*/{1190,-1,TabUnk,ActLstStd		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_SeeGstClassPhotoPrn	,NULL},
   /* ActPrnStdPho	*/{ 120,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_SeeStdClassPhotoPrn	,NULL},
   /* ActPrnTchPho	*/{ 443,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_SeeTchClassPhotoPrn	,NULL},
   /* ActLstGstAll	*/{1189,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_ListAllDataGsts		,NULL},
   /* ActLstStdAll	*/{  42,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_ListAllDataStds		,NULL},
   /* ActLstTchAll	*/{ 578,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_ListAllDataTchs 		,NULL},

   /* ActSeeRecOneStd	*/{1174,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_GetUsrAndShowRecordOneStdCrs,NULL},
   /* ActSeeRecOneTch	*/{1175,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_GetUsrAndShowRecordOneTchCrs,NULL},

   /* ActSeeRecSevGst	*/{1187,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ListRecordsGstsShow	,NULL},
   /* ActSeeRecSevStd	*/{  89,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ListRecordsStdsShow	,NULL},
   /* ActSeeRecSevTch	*/{  22,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ListRecordsTchsShow	,NULL},
   /* ActPrnRecSevGst	*/{1188,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Rec_ListRecordsGstsPrint	,NULL},
   /* ActPrnRecSevStd	*/{ 111,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Rec_ListRecordsStdsPrint	,NULL},
   /* ActPrnRecSevTch	*/{ 127,-1,TabUnk,ActLstTch		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Rec_ListRecordsTchsPrint	,NULL},

   /* ActRcvRecOthUsr	*/{ 300,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_UpdateAndShowOtherCrsRecord,NULL},
   /* ActEdiRecFie	*/{ 292,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ReqEditRecordFields	,NULL},
   /* ActNewFie		*/{ 293,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ReceiveFormField		,NULL},
   /* ActReqRemFie	*/{ 294,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ReqRemField		,NULL},
   /* ActRemFie		*/{ 295,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_RemoveField		,NULL},
   /* ActRenFie		*/{ 296,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_RenameField		,NULL},
   /* ActChgRowFie	*/{ 305,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ChangeLinesField		,NULL},
   /* ActChgVisFie	*/{ 297,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ChangeVisibilityField	,NULL},
   /* ActRcvRecCrs	*/{ 301,-1,TabUnk,ActLstStd		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_UpdateAndShowMyCrsRecord	,NULL},

   /* ActFrmLogInUsrAgd	*/{1614,-1,TabUnk,ActLstTch		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_PutFormLogInToShowUsrAgenda,NULL},
   /* ActLogInUsrAgd	*/{1615,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_ShowOtherAgendaAfterLogIn	,NULL},
   /* ActLogInUsrAgdLan	*/{1616,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_ShowOtherAgendaAfterLogIn	,NULL},
   /* ActSeeUsrAgd	*/{1611,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_ShowUsrAgenda		,NULL},

   /* ActReqEnrSevStd	*/{1426,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqAdminStds		,NULL},
   /* ActReqEnrSevNET	*/{1642,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqAdminNonEditingTchs	,NULL},
   /* ActReqEnrSevTch	*/{1427,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqAdminTchs		,NULL},

   /* ActReqLstStdAtt	*/{1073,-1,TabUnk,ActSeeAtt		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_ReqListStdsAttendanceCrs	,NULL},
   /* ActSeeLstMyAtt	*/{1473,-1,TabUnk,ActSeeAtt		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_ListMyAttendanceCrs	,NULL},
   /* ActPrnLstMyAtt	*/{1474,-1,TabUnk,ActSeeAtt		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_PrintMyAttendanceCrs	,NULL},
   /* ActSeeLstStdAtt	*/{1074,-1,TabUnk,ActSeeAtt		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_ListStdsAttendanceCrs	,NULL},
   /* ActPrnLstStdAtt	*/{1075,-1,TabUnk,ActSeeAtt		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_PrintStdsAttendanceCrs	,NULL},
   /* ActFrmNewAtt	*/{1063,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_RequestCreatOrEditAttEvent	,NULL},
   /* ActEdiOneAtt	*/{1064,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_RequestCreatOrEditAttEvent	,NULL},
   /* ActNewAtt		*/{1065,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_RecFormAttEvent		,NULL},
   /* ActChgAtt		*/{1066,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_RecFormAttEvent		,NULL},
   /* ActReqRemAtt	*/{1067,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_AskRemAttEvent		,NULL},
   /* ActRemAtt		*/{1068,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_GetAndRemAttEvent		,NULL},
   /* ActHidAtt		*/{1069,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_HideAttEvent		,NULL},
   /* ActShoAtt		*/{1070,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_ShowAttEvent		,NULL},
   /* ActSeeOneAtt	*/{1071,-1,TabUnk,ActSeeAtt		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_SeeOneAttEvent		,NULL},
   /* ActRecAttStd	*/{1072,-1,TabUnk,ActSeeAtt		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_RegisterStudentsInAttEvent	,NULL},
   /* ActRecAttMe	*/{1076,-1,TabUnk,ActSeeAtt		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Att_RegisterMeAsStdInAttEvent	,NULL},

   /* ActSignUp		*/{1056,-1,TabUnk,ActReqSignUp		,    0,0x006,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_SignUpInCrs		,NULL},
   /* ActUpdSignUpReq	*/{1522,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_UpdateEnrolmentRequests	,NULL},
   /* ActReqRejSignUp	*/{1058,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AskIfRejectSignUp		,NULL},
   /* ActRejSignUp	*/{1059,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RejectSignUp		,NULL},

   /* ActReqMdfOneOth	*/{1414,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqRegRemOth		,NULL},
   /* ActReqMdfOneStd	*/{1415,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqRegRemStd		,NULL},
   /* ActReqMdfOneTch	*/{1416,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqRegRemTch		,NULL},

   /* ActReqMdfOth	*/{1418,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AskIfRegRemAnotherOth	,NULL},
   /* ActReqMdfStd	*/{1419,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AskIfRegRemAnotherStd	,NULL},
   /* ActReqMdfNET	*/{1644,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AskIfRegRemAnotherTch	,NULL},
   /* ActReqMdfTch	*/{1420,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AskIfRegRemAnotherTch	,NULL},

   /* ActReqOthPho	*/{1432,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_SendPhotoUsr		,NULL},
   /* ActReqStdPho	*/{1433,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_SendPhotoUsr		,NULL},
   /* ActReqTchPho	*/{1434,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_SendPhotoUsr		,NULL},

   /* ActDetOthPho	*/{1435,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   /* ActDetStdPho	*/{1436,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   /* ActDetTchPho	*/{1437,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},

   /* ActUpdOthPho	*/{1438,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},
   /* ActUpdStdPho	*/{1439,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},
   /* ActUpdTchPho	*/{1440,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},

   /* ActRemOthPho	*/{1574,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_ReqRemoveUsrPhoto		,NULL},
   /* ActRemStdPho	*/{1575,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_ReqRemoveUsrPhoto		,NULL},
   /* ActRemTchPho	*/{1576,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_ReqRemoveUsrPhoto		,NULL},
   /* ActRemOthPho	*/{1441,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_RemoveUsrPhoto		,NULL},
   /* ActRemStdPho	*/{1442,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_RemoveUsrPhoto		,NULL},
   /* ActRemTchPho	*/{1443,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_RemoveUsrPhoto		,NULL},

   /* ActCreOth		*/{1444,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   /* ActCreStd		*/{1445,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   /* ActCreNET		*/{1645,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   /* ActCreTch		*/{1446,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},

   /* ActUpdOth		*/{1422,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   /* ActUpdStd		*/{1423,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   /* ActUpdNET		*/{1646,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   /* ActUpdTch		*/{1424,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},

   /* ActReqAccEnrStd	*/{1456,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   /* ActReqAccEnrNET	*/{1639,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   /* ActReqAccEnrTch	*/{1457,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   /* ActAccEnrStd	*/{1458,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   /* ActAccEnrNET	*/{1640,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   /* ActAccEnrTch	*/{1459,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   /* ActRemMe_Std	*/{1460,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqRemMeFromCrs		,NULL},
   /* ActRemMe_NET	*/{1641,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqRemMeFromCrs		,NULL},
   /* ActRemMe_Tch	*/{1461,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReqRemMeFromCrs		,NULL},

   /* ActNewAdmIns	*/{1337,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AddAdmToIns		,NULL},
   /* ActRemAdmIns	*/{1338,-1,TabUnk,ActLstOth		,0x300,0x300,0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemAdmIns			,NULL},

   /* ActNewAdmCtr	*/{1339,-1,TabUnk,ActLstOth		,0x300,0x300,0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AddAdmToCtr		,NULL},
   /* ActRemAdmCtr	*/{1340,-1,TabUnk,ActLstOth		,0x380,0x380,0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemAdmCtr			,NULL},

   /* ActNewAdmDeg	*/{ 586,-1,TabUnk,ActLstOth		,0x380,0x380,0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AddAdmToDeg		,NULL},
   /* ActRemAdmDeg	*/{ 584,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemAdmDeg			,NULL},

   /* ActRcvFrmEnrSevStd*/{1428,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReceiveFormAdminStds	,NULL},
   /* ActRcvFrmEnrSevNET*/{1643,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReceiveFormAdminNonEditTchs,NULL},
   /* ActRcvFrmEnrSevTch*/{1429,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_ReceiveFormAdminTchs	,NULL},

   /* ActCnfID_Oth	*/{1568,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_ConfirmOtherUsrID		,NULL},
   /* ActCnfID_Std	*/{1569,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_ConfirmOtherUsrID		,NULL},
   /* ActCnfID_Tch	*/{1570,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_ConfirmOtherUsrID		,NULL},

   /* ActFrmIDsOth	*/{1447,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_ShowFormOthIDs		,NULL},
   /* ActFrmIDsStd	*/{1448,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_ShowFormOthIDs		,NULL},
   /* ActFrmIDsTch	*/{1449,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_ShowFormOthIDs		,NULL},
   /* ActRemID_Oth	*/{1450,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_RemoveOtherUsrID		,NULL},
   /* ActRemID_Std	*/{1451,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_RemoveOtherUsrID		,NULL},
   /* ActRemID_Tch	*/{1452,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_RemoveOtherUsrID		,NULL},
   /* ActNewID_Oth	*/{1453,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_NewOtherUsrID		,NULL},
   /* ActNewID_Std	*/{1454,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_NewOtherUsrID		,NULL},
   /* ActNewID_Tch	*/{1455,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_NewOtherUsrID		,NULL},

   /* ActFrmPwdOth	*/{1464,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pwd_ShowFormOthPwd		,NULL},
   /* ActFrmPwdStd	*/{1465,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pwd_ShowFormOthPwd		,NULL},
   /* ActFrmPwdTch	*/{1466,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pwd_ShowFormOthPwd		,NULL},
   /* ActChgPwdOth	*/{1467,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Pwd_UpdateOtherPwd1		,Pwd_UpdateOtherPwd2		,NULL},
   /* ActChgPwdStd	*/{1468,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Pwd_UpdateOtherPwd1		,Pwd_UpdateOtherPwd2		,NULL},
   /* ActChgPwdTch	*/{1469,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,Pwd_UpdateOtherPwd1		,Pwd_UpdateOtherPwd2		,NULL},

   /* ActFrmMaiOth	*/{1475,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_ShowFormOthEmail		,NULL},
   /* ActFrmMaiStd	*/{1476,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_ShowFormOthEmail		,NULL},
   /* ActFrmMaiTch	*/{1477,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_ShowFormOthEmail		,NULL},
   /* ActRemMaiOth	*/{1478,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   /* ActRemMaiStd	*/{1479,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   /* ActRemMaiTch	*/{1480,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   /* ActNewMaiOth	*/{1481,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_NewOtherUsrEmail		,NULL},
   /* ActNewMaiStd	*/{1482,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_NewOtherUsrEmail		,NULL},
   /* ActNewMaiTch	*/{1483,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_NewOtherUsrEmail		,NULL},

   /* ActRemStdCrs	*/{1462,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemUsrFromCrs		,NULL},
   /* ActRemNETCrs	*/{1647,-1,TabUnk,ActLstTch		,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemUsrFromCrs		,NULL},
   /* ActRemTchCrs	*/{1463,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemUsrFromCrs		,NULL},

   /* ActRemUsrGbl	*/{  62,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Acc_GetUsrCodAndRemUsrGbl	,NULL},

   /* ActReqRemAllStdCrs*/{  88,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AskRemAllStdsThisCrs	,NULL},
   /* ActRemAllStdCrs	*/{  87,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemAllStdsThisCrs		,NULL},

   /* ActReqRemOldUsr	*/{ 590,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_AskRemoveOldUsrs		,NULL},
   /* ActRemOldUsr	*/{ 773,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Enr_RemoveOldUsrs		,NULL},

   /* ActLstDupUsr	*/{1578,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dup_ListDuplicateUsrs		,NULL},
   /* ActLstSimUsr	*/{1579,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dup_GetUsrCodAndListSimilarUsrs,NULL},
   /* ActRemDupUsr	*/{1580,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Dup_RemoveUsrFromListDupUsrs	,NULL},

   /* ActLstClk		*/{ 989,-1,TabUnk,ActLstCon		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Con_ShowLastClicks		,NULL},

   // TabSoc ******************************************************************
   /* ActSeeSocTmlGbl	*/{1490, 0,TabSoc,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_ShowTimelineGbl2		,"soc64x64.png"		},
   /* ActSeeSocPrf	*/{1520, 1,TabSoc,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_SeeSocialProfiles		,"prf64x64.gif"		},
   /* ActSeeFor		*/{  95, 2,TabSoc,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,"forum64x64.gif"	},
   /* ActSeeChtRms	*/{  51, 3,TabSoc,ActSeeChtRms		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Cht_ShowChatRooms		,"chat64x64.gif"	},

   /* ActRcvSocPstGbl	*/{1492,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_ReceiveSocialPostGbl	,NULL},
   /* ActRcvSocComGbl	*/{1503,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_ReceiveCommentGbl		,NULL},
   /* ActShaSocNotGbl	*/{1495,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_ShareSocialNoteGbl		,NULL},
   /* ActUnsSocNotGbl	*/{1496,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_UnshareSocialNoteGbl	,NULL},
   /* ActFavSocNotGbl	*/{1512,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_FavSocialNoteGbl		,NULL},
   /* ActUnfSocNotGbl	*/{1513,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_UnfavSocialNoteGbl		,NULL},
   /* ActFavSocComGbl	*/{1516,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_FavSocialCommentGbl	,NULL},
   /* ActUnfSocComGbl	*/{1517,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_UnfavSocialCommentGbl	,NULL},
   /* ActReqRemSocPubGbl*/{1494,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_RequestRemSocialNoteGbl	,NULL},
   /* ActRemSocPubGbl	*/{1493,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_RemoveSocialNoteGbl	,NULL},
   /* ActReqRemSocComGbl*/{1505,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_RequestRemSocialComGbl	,NULL},
   /* ActRemSocComGbl	*/{1507,-1,TabUnk,ActSeeSocTmlGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Soc_ShowTimelineGbl1		,Soc_RemoveSocialComGbl		,NULL},

   /* ActReqOthPubPrf	*/{1401,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_RequestUserProfile		,NULL},

   /* ActRcvSocPstUsr	*/{1498,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Soc_ReceiveSocialPostUsr	,NULL},
   /* ActRcvSocComUsr	*/{1504,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Soc_ReceiveCommentUsr		,NULL},
   /* ActShaSocNotUsr	*/{1499,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_ShareSocialNoteUsr		,NULL},
   /* ActUnsSocNotUsr	*/{1500,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_UnshareSocialNoteUsr	,NULL},
   /* ActFavSocNotUsr	*/{1514,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_FavSocialNoteUsr		,NULL},
   /* ActUnfSocNotUsr	*/{1515,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_UnfavSocialNoteUsr		,NULL},
   /* ActFavSocComUsr	*/{1518,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_FavSocialCommentUsr	,NULL},
   /* ActUnfSocComUsr	*/{1519,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_UnfavSocialCommentUsr	,NULL},
   /* ActReqRemSocPubUsr*/{1501,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_RequestRemSocialNoteUsr	,NULL},
   /* ActRemSocPubUsr	*/{1502,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_RemoveSocialNoteUsr	,NULL},
   /* ActReqRemSocComGbl*/{1506,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_RequestRemSocialComUsr	,NULL},
   /* ActRemSocComGbl	*/{1508,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Soc_RemoveSocialComUsr		,NULL},

   /* ActSeeOthPubPrf	*/{1402,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_GetUsrDatAndShowUserProfile,NULL},
   /* ActCal1stClkTim	*/{1405,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_CalculateFigures		,NULL},
   /* ActCalNumClk	*/{1406,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_CalculateFigures		,NULL},
   /* ActCalNumFilVie	*/{1409,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_CalculateFigures		,NULL},
   /* ActCalNumForPst	*/{1408,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_CalculateFigures		,NULL},
   /* ActCalNumMsgSnt	*/{1407,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_CalculateFigures		,NULL},

   /* ActFolUsr		*/{1410,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Fol_FollowUsr1			,Fol_FollowUsr2			,NULL},
   /* ActUnfUsr		*/{1411,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Fol_UnfollowUsr1		,Fol_UnfollowUsr2		,NULL},
   /* ActSeeFlg		*/{1412,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Fol_ListFollowing		,NULL},
   /* ActSeeFlr		*/{1413,-1,TabUnk,ActSeeSocPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Fol_ListFollowers		,NULL},

   /* ActSeeForCrsUsr	*/{ 345,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForCrsTch	*/{ 431,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForDegUsr	*/{ 241,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForDegTch	*/{ 243,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForCtrUsr	*/{ 901,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForCtrTch	*/{ 430,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForInsUsr	*/{ 725,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForInsTch	*/{ 724,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForGenUsr	*/{ 726,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForGenTch	*/{ 723,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForSWAUsr	*/{ 242,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},
   /* ActSeeForSWATch	*/{ 245,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowForumTheads		,NULL},

   /* ActSeePstForCrsUsr*/{ 346,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForCrsTch*/{ 347,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForDegUsr*/{ 255,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForDegTch*/{ 291,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForCtrUsr*/{ 348,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForCtrTch*/{ 902,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForInsUsr*/{ 730,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForInsTch*/{ 746,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForGenUsr*/{ 727,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForGenTch*/{ 731,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForSWAUsr*/{ 244,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},
   /* ActSeePstForSWATch*/{ 246,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_ShowThreadPosts		,NULL},

   /* ActRcvThrForCrsUsr*/{ 350,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForCrsTch*/{ 754,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForDegUsr*/{ 252,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForDegTch*/{ 247,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForCtrUsr*/{ 903,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForCtrTch*/{ 904,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForInsUsr*/{ 737,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForInsTch*/{ 769,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForGenUsr*/{ 736,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForGenTch*/{ 765,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForSWAUsr*/{ 258,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvThrForSWATch*/{ 259,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},

   /* ActRcvRepForCrsUsr*/{ 599,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForCrsTch*/{ 755,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForDegUsr*/{ 606,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForDegTch*/{ 617,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForCtrUsr*/{ 905,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForCtrTch*/{ 906,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForInsUsr*/{ 740,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForInsTch*/{ 770,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForGenUsr*/{ 747,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForGenTch*/{ 816,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForSWAUsr*/{ 603,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},
   /* ActRcvRepForSWATch*/{ 622,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,For_ReceiveForumPost		,NULL},

   /* ActReqDelThrCrsUsr*/{ 867,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrCrsTch*/{ 869,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrDegUsr*/{ 907,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrDegTch*/{ 908,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrCtrUsr*/{ 909,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrCtrTch*/{ 910,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrInsUsr*/{ 911,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrInsTch*/{ 912,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrGenUsr*/{ 913,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrGenTch*/{ 914,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrSWAUsr*/{ 881,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},
   /* ActReqDelThrSWATch*/{ 915,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RequestRemoveThread	,NULL},

   /* ActDelThrForCrsUsr*/{ 868,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForCrsTch*/{ 876,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForDegUsr*/{ 916,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForDegTch*/{ 917,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForCtrUsr*/{ 918,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForCtrTch*/{ 919,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForInsUsr*/{ 920,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForInsTch*/{ 921,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForGenUsr*/{ 922,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForGenTch*/{ 923,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForSWAUsr*/{ 882,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},
   /* ActDelThrForSWATch*/{ 924,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemoveThread		,NULL},

   /* ActCutThrForCrsUsr*/{ 926,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForCrsTch*/{ 927,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForDegUsr*/{ 928,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForDegTch*/{ 929,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForCtrUsr*/{ 930,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForCtrTch*/{ 931,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForInsUsr*/{ 932,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForInsTch*/{ 933,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForGenUsr*/{ 934,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForGenTch*/{ 935,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForSWAUsr*/{ 890,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},
   /* ActCutThrForSWATch*/{ 936,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_CutThread			,NULL},

   /* ActPasThrForCrsUsr*/{ 891,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForCrsTch*/{ 937,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForDegUsr*/{ 938,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForDegTch*/{ 939,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForCtrUsr*/{ 940,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForCtrTch*/{ 941,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForInsUsr*/{ 942,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForInsTch*/{ 943,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForGenUsr*/{ 944,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForGenTch*/{ 945,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForSWAUsr*/{ 946,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},
   /* ActPasThrForSWATch*/{ 947,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_PasteThread		,NULL},

   /* ActDelPstForCrsUsr*/{ 602,-1,TabUnk,ActSeeFor		,0x238,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForCrsTch*/{ 756,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForDegUsr*/{ 608,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForDegTch*/{ 680,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForCtrUsr*/{ 948,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForCtrTch*/{ 949,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForInsUsr*/{ 743,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForInsTch*/{ 772,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForGenUsr*/{ 735,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForGenTch*/{ 950,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForSWAUsr*/{ 613,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},
   /* ActDelPstForSWATch*/{ 623,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_RemovePost			,NULL},

   /* ActEnbPstForCrsUsr*/{ 624,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForCrsTch*/{ 951,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForDegUsr*/{ 616,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForDegTch*/{ 619,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForCtrUsr*/{ 952,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForCtrTch*/{ 953,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForInsUsr*/{ 954,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForInsTch*/{ 955,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForGenUsr*/{ 956,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForGenTch*/{ 957,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForSWAUsr*/{ 632,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},
   /* ActEnbPstForSWATch*/{ 634,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_EnablePost			,NULL},

   /* ActDisPstForCrsUsr*/{ 610,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForCrsTch*/{ 958,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForDegUsr*/{ 615,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForDegTch*/{ 618,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForCtrUsr*/{ 959,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForCtrTch*/{ 960,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForInsUsr*/{ 961,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForInsTch*/{ 962,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForGenUsr*/{ 963,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForGenTch*/{ 925,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForSWAUsr*/{ 625,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},
   /* ActDisPstForSWATch*/{ 635,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,For_DisablePost		,NULL},

   /* ActCht		*/{  52,-1,TabUnk,ActSeeChtRms		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BLNK_WINDOW,Cht_OpenChatWindow		,NULL				,NULL},

   // TabMsg ******************************************************************
   // Actions in menu:
   /* ActSeeNtf         */{ 990, 0,TabMsg,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ntf_ShowMyNotifications	,"bell64x64.gif"	},
   /* ActSeeAnn		*/{1235, 1,TabMsg,ActSeeAnn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ann_ShowAllAnnouncements	,"announcement64x64.png"},
   /* ActSeeAllNot	*/{ 762, 2,TabMsg,ActSeeAllNot		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Not_ListFullNotices		,"notice64x64.png"	},
   /* ActReqMsgUsr	*/{  26, 3,TabMsg,ActReqMsgUsr		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_FormMsgUsrs		,"editnewmsg64x64.gif"	},
   /* ActSeeRcvMsg	*/{   3, 4,TabMsg,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ShowRecMsgs		,"recmsg64x64.gif"	},
   /* ActSeeSntMsg	*/{  70, 5,TabMsg,ActSeeSntMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ShowSntMsgs		,"sntmsg64x64.gif"	},
   /* ActMaiStd		*/{ 100, 6,TabMsg,ActMaiStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_ListEmails			,"email64x64.gif"	},

   // Actions not in menu:
   /* ActWriAnn		*/{1237,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ann_ShowFormAnnouncement	,NULL},
   /* ActRcvAnn		*/{1238,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ann_ReceiveAnnouncement	,NULL},
   /* ActHidAnn		*/{1470,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,Ann_HideActiveAnnouncement	,Ann_ShowAllAnnouncements	,NULL},
   /* ActRevAnn		*/{1471,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,Ann_RevealHiddenAnnouncement	,Ann_ShowAllAnnouncements	,NULL},
   /* ActRemAnn		*/{1236,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ann_RemoveAnnouncement		,NULL},

   /* ActSeeOneNot	*/{1164,-1,TabUnk,ActSeeAllNot		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Not_GetNotCodToHighlight	,Not_ListFullNotices		,NULL},
   /* ActWriNot		*/{  59,-1,TabUnk,ActSeeAllNot		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Not_ShowFormNotice		,NULL},
   /* ActRcvNot		*/{  60,-1,TabUnk,ActSeeAllNot		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Not_ReceiveNotice		,Not_ListFullNotices		,NULL},
   /* ActHidNot		*/{ 763,-1,TabUnk,ActSeeAllNot		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Not_HideActiveNotice		,Not_ListFullNotices		,NULL},
   /* ActRevNot		*/{ 764,-1,TabUnk,ActSeeAllNot		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Not_RevealHiddenNotice		,Not_ListFullNotices		,NULL},
   /* ActReqRemNot	*/{1472,-1,TabUnk,ActSeeAllNot		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Not_RequestRemNotice		,NULL},
   /* ActRemNot		*/{  73,-1,TabUnk,ActSeeAllNot		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Not_RemoveNotice		,Not_ListNoticesAfterRemoval	,NULL},

   /* ActSeeNewNtf	*/{ 991,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ntf_ShowMyNotifications	,NULL},
   /* ActMrkNtfSee	*/{1146,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ntf_MarkAllNotifAsSeen		,NULL},

   /* ActSeeMai		*/{ 855,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_SeeMailDomains		,NULL},
   /* ActEdiMai		*/{ 856,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_EditMailDomains		,NULL},
   /* ActNewMai		*/{ 857,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RecFormNewMailDomain	,NULL},
   /* ActRemMai		*/{ 860,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RemoveMailDomain		,NULL},
   /* ActRenMaiSho	*/{ 858,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RenameMailDomainShort	,NULL},
   /* ActRenMaiFul	*/{ 859,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RenameMailDomainFull	,NULL},

   /* ActRcvMsgUsr	*/{  27,-1,TabUnk,ActReqMsgUsr		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Msg_RecMsgFromUsr		,NULL},
   /* ActReqDelAllSntMsg*/{ 604,-1,TabUnk,ActSeeSntMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ReqDelAllSntMsgs		,NULL},
   /* ActReqDelAllRcvMsg*/{ 593,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ReqDelAllRecMsgs		,NULL},
   /* ActDelAllSntMsg	*/{ 434,-1,TabUnk,ActSeeSntMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_DelAllSntMsgs		,NULL},
   /* ActDelAllRcvMsg	*/{ 436,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_DelAllRecMsgs		,NULL},
   /* ActDelSntMsg	*/{  90,-1,TabUnk,ActSeeSntMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_DelSntMsg			,NULL},
   /* ActDelRcvMsg	*/{  64,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_DelRecMsg			,NULL},
   /* ActExpSntMsg	*/{ 664,-1,TabUnk,ActSeeSntMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ExpSntMsg			,NULL},
   /* ActExpRcvMsg	*/{ 663,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ExpRecMsg			,NULL},
   /* ActConSntMsg	*/{1020,-1,TabUnk,ActSeeSntMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ConSntMsg			,NULL},
   /* ActConRcvMsg	*/{1019,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ConRecMsg			,NULL},

   /* ActLstBanUsr	*/{1017,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_ListBannedUsrs		,NULL},
   /* ActBanUsrMsg	*/{1015,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_BanSenderWhenShowingMsgs	,NULL},
   /* ActUnbUsrMsg	*/{1016,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_UnbanSenderWhenShowingMsgs	,NULL},
   /* ActUnbUsrLst	*/{1018,-1,TabUnk,ActSeeRcvMsg		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Msg_UnbanSenderWhenListingUsrs	,NULL},

   // TabSta ******************************************************************
   // Actions in menu:
   /* ActReqUseGbl	*/{ 761, 0,TabSta,ActReqUseGbl		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sta_ReqShowFigures		,"pie64x64.gif"		},
   /* ActSeePhoDeg	*/{ 447, 1,TabSta,ActSeePhoDeg		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_ShowPhotoDegree		,"classphoto64x64.gif"	},
   /* ActReqStaCrs	*/{ 767, 2,TabSta,ActReqStaCrs		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ind_ReqIndicatorsCourses	,"tablestats64x64.gif"	},
   /* ActReqAccGbl	*/{ 591, 3,TabSta,ActReqAccGbl		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Sta_SetIniEndDates			,Sta_AskShowGblHits		,"stats64x64.gif"	},
   /* ActReqMyUsgRep	*/{1586, 4,TabSta,ActReqMyUsgRep	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rep_ReqMyUsageReport		,"report64x64.png"	},
   /* ActMFUAct		*/{ 993, 5,TabSta,ActMFUAct		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,MFU_ShowMyMFUActions		,"star64x64.gif"	},

   // Actions not in menu:
   /* ActSeeUseGbl	*/{  84,-1,TabUnk,ActReqUseGbl		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sta_ShowFigures		,NULL},
   /* ActPrnPhoDeg	*/{ 448,-1,TabUnk,ActSeePhoDeg		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Pho_PrintPhotoDegree  		,NULL},
   /* ActCalPhoDeg	*/{ 444,-1,TabUnk,ActSeePhoDeg		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_CalcPhotoDegree		,NULL},
   /* ActSeeAccGbl	*/{  79,-1,TabUnk,ActReqAccGbl		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sta_SeeGblAccesses		,NULL},
   /* ActReqAccCrs	*/{ 594,-1,TabUnk,ActReqAccGbl		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,Sta_SetIniEndDates		,Sta_AskShowCrsHits		,NULL},
   /* ActSeeAccCrs	*/{ 119,-1,TabUnk,ActReqAccGbl		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Sta_SeeCrsAccesses		,NULL},
   /* ActSeeAllStaCrs	*/{ 768,-1,TabUnk,ActReqAccGbl		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Ind_ShowIndicatorsCourses	,NULL},

   /* ActSeeMyUsgRep	*/{1582,-1,TabUnk,ActReqMyUsgRep	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rep_ShowMyUsageReport		,NULL},

   // TabPrf ******************************************************************
   // Actions in menu:
   /* ActFrmLogIn	*/{1521, 0,TabPrf,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_WriteLandingPage		,"keyuser64x64.gif"	},
   /* ActFrmRolSes	*/{ 843, 1,TabPrf,ActFrmRolSes		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_WriteFormLogout		,"keyuser64x64.gif"	},
   /* ActMyCrs		*/{ 987, 2,TabPrf,ActMyCrs		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Crs_ReqSelectOneOfMyCourses	,"hierarchy64x64.png"	},
   /* ActSeeMyTT	*/{ 408, 3,TabPrf,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,TT_ShowClassTimeTable		,"clock64x64.gif"	},
   /* ActSeeMyAgd	*/{1602, 4,TabPrf,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_ShowMyAgenda		,"calendar64x64.png"	},
   /* ActFrmMyAcc	*/{  36, 5,TabPrf,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Acc_ShowFormMyAccount		,"arroba64x64.gif"	},
   /* ActReqEdiRecCom	*/{ 285, 6,TabPrf,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ShowFormMySharedRecord	,"card64x64.gif"	},
   /* ActEdiPrf		*/{ 673, 7,TabPrf,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pre_EditPrefs			,"heart64x64.gif"	},
   /* ActAdmBrf		*/{  23, 8,TabPrf,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileBrowserOrWorks	,"pendrive64x64.gif"	},

   // Actions not in menu:
   /* ActReqSndNewPwd	*/{ 665,-1,TabUnk,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pwd_ShowFormSendNewPwd		,NULL},
   /* ActSndNewPwd	*/{ 633,-1,TabUnk,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pwd_ChkIdLoginAndSendNewPwd	,NULL},
   /* ActLogOut		*/{  10,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Ses_CloseSession		,Usr_Logout			,NULL},

   /* ActLogIn		*/{   6,-1,TabUnk,ActFrmRolSes		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_WelcomeUsr			,NULL},
   /* ActLogInNew	*/{1585,-1,TabUnk,ActFrmRolSes		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_WelcomeUsr			,NULL},
   /* ActLogInLan	*/{1077,-1,TabUnk,ActFrmRolSes		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Usr_WelcomeUsr			,NULL},
   /* ActAnnSee		*/{1234,-1,TabUnk,ActFrmRolSes		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Ann_MarkAnnouncementAsSeen	,NULL},
   /* ActChgMyRol	*/{ 589,-1,TabUnk,ActFrmRolSes		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Rol_ChangeMyRole		,Usr_ShowFormsLogoutAndRole	,NULL},

   /* ActFrmNewEvtMyAgd	*/{1603,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_RequestCreatOrEditEvent	,NULL},
   /* ActEdiOneEvtMyAgd	*/{1604,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_RequestCreatOrEditEvent	,NULL},
   /* ActNewEvtMyAgd	*/{1605,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_RecFormEvent		,NULL},
   /* ActChgEvtMyAgd	*/{1606,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_RecFormEvent		,NULL},
   /* ActReqRemEvtMyAgd	*/{1607,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_AskRemEvent		,NULL},
   /* ActRemEvtMyAgd	*/{1608,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_RemoveEvent		,NULL},
   /* ActHidEvtMyAgd	*/{1612,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_HideEvent			,NULL},
   /* ActShoEvtMyAgd	*/{1613,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_UnhideEvent		,NULL},
   /* ActPrvEvtMyAgd	*/{1609,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_MakeEventPrivate		,NULL},
   /* ActPubEvtMyAgd	*/{1610,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Agd_MakeEventPublic		,NULL},
   /* ActPrnAgdQR	*/{1618,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Agd_PrintAgdQRCode		,NULL},

   /* ActChkUsrAcc	*/{1584,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Acc_CheckIfEmptyAccountExists	,NULL},
   /* ActCreUsrAcc	*/{1163,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Acc_AfterCreationNewAccount	,NULL},

   /* ActRemID_Me	*/{1147,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_RemoveMyUsrID		,NULL},
   /* ActNewIDMe	*/{1148,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ID_NewMyUsrID			,NULL},

   /* ActRemOldNic	*/{1089,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Nck_RemoveNick			,NULL},
   /* ActChgNic		*/{  37,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Nck_UpdateNick			,NULL},

   /* ActRemMaiMe	*/{1090,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_RemoveMyUsrEmail		,NULL},
   /* ActNewMaiMe	*/{1088,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,May_NewMyUsrEmail		,NULL},
   /* ActCnfMai		*/{1091,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Mai_ConfirmEmail		,NULL},

   /* ActFrmChgMyPwd	*/{  34,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pwd_ShowFormChgPwd		,NULL},
   /* ActChgPwd		*/{  35,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Pwd_ActChgMyPwd1		,Pwd_ActChgMyPwd2		,NULL},

   /* ActReqRemMyAcc	*/{1430,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Acc_AskIfRemoveMyAccount	,NULL},
   /* ActRemMyAcc	*/{1431,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Acc_RemoveMyAccount		,NULL},

   /* ActChgMyData	*/{ 298,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Rec_UpdateMyRecord		,Rec_ShowMySharedRecordUpd	,NULL},

   /* ActReqMyPho	*/{  30,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_ReqMyPhotoWithContextLinks	,NULL},
   /* ActDetMyPho	*/{ 693,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Pho_RecMyPhotoDetFaces		,NULL},
   /* ActUpdMyPho	*/{ 694,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Pho_UpdateMyPhoto1		,Pho_UpdateMyPhoto2		,NULL},
   /* ActReqRemMyPho	*/{1577,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_ReqRemoveMyPhoto		,NULL},
   /* ActRemMyPho	*/{ 428,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Pho_RemoveMyPhoto1		,Pho_RemoveMyPhoto2		,NULL},

   /* ActEdiPri		*/{1403,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pri_EditMyPrivacy		,NULL},
   /* ActChgPriPho	*/{ 774,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Pho_ChangePhotoVisibility	,NULL},
   /* ActChgPriPrf	*/{1404,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Prf_ChangeProfileVisibility	,NULL},

   /* ActReqEdiMyIns	*/{1165,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ShowFormMyInsCtrDpt	,NULL},
   /* ActChgCtyMyIns	*/{1166,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_ChgCountryOfMyInstitution	,NULL},
   /* ActChgMyIns	*/{1167,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_UpdateMyInstitution	,NULL},
   /* ActChgMyCtr	*/{1168,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_UpdateMyCentre		,NULL},
   /* ActChgMyDpt	*/{1169,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_UpdateMyDepartment		,NULL},
   /* ActChgMyOff	*/{1170,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_UpdateMyOffice		,NULL},
   /* ActChgMyOffPho	*/{1171,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Rec_UpdateMyOfficePhone	,NULL},

   /* ActReqEdiMyNet	*/{1172,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Net_ShowFormMyWebsAndSocialNets,NULL},
   /* ActChgMyNet	*/{1173,-1,TabUnk,ActReqEdiRecCom	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Net_UpdateMyWebsAndSocialNets	,NULL},

   /* ActChgThe		*/{ 841,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,The_ChangeTheme		,Pre_EditPrefs			,NULL},
   /* ActReqChgLan	*/{ 992,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Lan_AskChangeLanguage		,NULL},
   /* ActChgLan		*/{ 654,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Lan_ChangeLanguage		,Pre_EditPrefs			,NULL},
   /* ActChg1stDay	*/{1484,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,Pre_EditPrefs			,NULL},
   /* ActChgDatFmt	*/{1638,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Dat_ChangeDateFormat		,Pre_EditPrefs			,NULL},
   /* ActChgCol		*/{ 674,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Pre_ChangeSideCols		,Pre_EditPrefs			,NULL},
   /* ActHidLftCol	*/{ 668,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Pre_HideLeftCol		,Pre_EditPrefs			,NULL},
   /* ActHidRgtCol	*/{ 669,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Pre_HideRightCol		,Pre_EditPrefs			,NULL},
   /* ActShoLftCol	*/{ 670,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Pre_ShowLeftCol		,Pre_EditPrefs			,NULL},
   /* ActShoRgtCol	*/{ 671,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Pre_ShowRightCol		,Pre_EditPrefs			,NULL},
   /* ActChgIco		*/{1092,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Ico_ChangeIconSet		,Pre_EditPrefs			,NULL},
   /* ActChgMnu		*/{1243,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_THIS_WINDOW,Mnu_ChangeMenu			,Pre_EditPrefs			,NULL},
   /* ActChgNtfPrf	*/{ 775,-1,TabUnk,ActEdiPrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Ntf_ChangeNotifyEvents		,Pre_EditPrefs			,NULL},

   /* ActPrnUsrQR	*/{1022,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,Usr_PrintUsrQRCode		,NULL},

   /* ActPrnMyTT	*/{ 409,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BLNK_WINDOW,NULL				,TT_ShowClassTimeTable		,NULL},
   /* ActEdiTut		*/{  65,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,TT_EditMyTutTimeTable		,NULL},
   /* ActChgTut		*/{  48,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,TT_EditMyTutTimeTable		,NULL},
   /* ActChgMyTT1stDay	*/{1487,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C7,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,Cal_ChangeFirstDayOfWeek	,TT_ShowClassTimeTable		,NULL},

   /* ActReqRemFilBrf	*/{ 286,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilBrf	*/{ 155,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFileFromTree		,NULL},
   /* ActRemFolBrf	*/{ 196,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemFolderFromTree		,NULL},
   /* ActCopBrf		*/{ 311,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasBrf		*/{ 315,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreBrf	*/{ 276,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreBrf	*/{ 597,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolBrf	*/{ 170,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkBrf	*/{1230,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolBrf	*/{ 197,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilBrfDZ	*/{1224,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   /* ActRcvFilBrfCla	*/{ 153,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_THIS_WINDOW,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpBrf		*/{ 410,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ExpandFileTree		,NULL},
   /* ActConBrf		*/{ 411,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ContractFileTree		,NULL},
   /* ActZIPBrf		*/{1136,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,ZIP_CompressFileTree		,NULL},
   /* ActReqDatBrf	*/{1047,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatBrf	*/{1048,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_ChgFileMetadata		,NULL},
   /* ActDowBrf         */{1123,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   /* ActReqRemOldBrf   */{1488,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_AskRemoveOldFiles		,NULL},
   /* ActRemOldBrf      */{1489,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_THIS_WINDOW,NULL				,Brw_RemoveOldFilesBriefcase	,NULL},
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
	ActSeeCalCrs,		// #16
	ActSeeAdmMrk,		// #17
	-1,			// #18 (obsolete action)
	-1,			// #19 (obsolete action)
	ActSeeSylPra,		// #20
	-1,			// #21 (obsolete action)
	ActSeeRecSevTch,	// #22
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
	ActFrmChgMyPwd,		// #34
	ActChgPwd,		// #35
	ActFrmMyAcc,		// #36
	ActChgNic,		// #37
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
	ActPrnCalCrs,		// #71
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
	ActMaiStd,		// #100
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
	ActReqEdiRecCom,	// #285
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
	ActSysReqSch,		// #627
	ActSysSch,		// #628
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
	ActEdiPrf,		// #673
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
	ActHom,			// #844
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
	ActSeeOneSvy,		// #982
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
	ActReqLstStdAtt,	// #1073
	ActSeeLstStdAtt,	// #1074
	ActPrnLstStdAtt,	// #1075
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
	ActNewMaiMe,		// #1088
	ActRemOldNic,		// #1089
	ActRemMaiMe,		// #1090
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
	ActRemID_Me,		// #1147
	ActNewIDMe,		// #1148
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
	ActReqEdiMyIns,		// #1165
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
	ActCtyReqSch,		// #1176
	ActInsReqSch,		// #1177
	ActCtrReqSch,		// #1178
	ActDegReqSch,		// #1179
	ActCrsReqSch,		// #1180
	ActCtySch,		// #1181
	ActInsSch,		// #1182
	ActCtrSch,		// #1183
	ActDegSch,		// #1184
	ActCrsSch,		// #1185
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
	ActEdiPri,		// #1403
	ActChgPriPrf,		// #1404
	ActCal1stClkTim,	// #1405
	ActCalNumClk,		// #1406
	ActCalNumMsgSnt,	// #1407
	ActCalNumForPst,	// #1408
	ActCalNumFilVie,	// #1409
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
	ActFrmIDsOth,		// #1447
	ActFrmIDsStd,		// #1448
	ActFrmIDsTch,		// #1449
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
	ActFrmPwdOth,		// #1464
	ActFrmPwdStd,		// #1465
	ActFrmPwdTch,		// #1466
	ActChgPwdOth,		// #1467
	ActChgPwdStd,		// #1468
	ActChgPwdTch,		// #1469
	ActHidAnn,		// #1470
	ActRevAnn,		// #1471
	ActReqRemNot,		// #1472
	ActSeeLstMyAtt,		// #1473
	ActPrnLstMyAtt,		// #1474
	ActFrmMaiOth,		// #1475
	ActFrmMaiStd,		// #1476
	ActFrmMaiTch,		// #1477
	ActRemMaiOth,		// #1478
	ActRemMaiStd,		// #1479
	ActRemMaiTch,		// #1480
	ActNewMaiOth,		// #1481
	ActNewMaiStd,		// #1482
	ActNewMaiTch,		// #1483
	ActChg1stDay,		// #1484
	ActChgCalCrs1stDay,	// #1485
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
	ActSeeCalSys,		// #1622
	ActPrnCalSys,		// #1623
	ActChgCalSys1stDay,	// #1624
	ActSeeCalCty,		// #1625
	ActPrnCalCty,		// #1626
	ActChgCalCty1stDay,	// #1627
	ActSeeCalIns,		// #1628
	ActPrnCalIns,		// #1629
	ActChgCalIns1stDay,	// #1630
	ActSeeCalCtr,		// #1631
	ActPrnCalCtr,		// #1632
	ActChgCalCtr1stDay,	// #1633
	ActSeeCalDeg,		// #1634
	ActPrnCalDeg,		// #1635
	ActChgCalDeg1stDay,	// #1636
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
	};

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Act_FormStartInternal (Act_Action_t NextAction,bool PutParameterLocationIfNoSesion,
                                   const char *Id,const char *Anchor,const char *OnSubmit);

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
/************* Check if I have permission to execute an action ***************/
/*****************************************************************************/

bool Act_CheckIfIHavePermissionToExecuteAction (Act_Action_t Action)
  {
   unsigned Permission;

   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return false;

   if (Gbl.CurrentCrs.Crs.CrsCod > 0)		// Course selected
      Permission = Gbl.Usrs.Me.IBelongToCurrentCrs ? Act_Actions[Action].PermissionCrsIfIBelong :
	                                             Act_Actions[Action].PermissionCrsIfIDontBelong;
   else if (Gbl.CurrentDeg.Deg.DegCod > 0)	// Degree selected
      Permission = Act_Actions[Action].PermissionDeg;
   else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)	// Centre selected
      Permission = Act_Actions[Action].PermissionCtr;
   else if (Gbl.CurrentIns.Ins.InsCod > 0)	// Institution selected
      Permission = Act_Actions[Action].PermissionIns;
   else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected
      Permission = Act_Actions[Action].PermissionCty;
   else
      Permission = Act_Actions[Action].PermissionSys;

   return Permission & (1 << Gbl.Usrs.Me.LoggedRole);
  }

/*****************************************************************************/
/******************* Get the title associated to an action *******************/
/*****************************************************************************/

const char *Act_GetTitleAction (Act_Action_t Action)
  {
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   Act_Action_t SuperAction;

   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return NULL;

   SuperAction = Act_Actions[Action].SuperAction;
   return Txt_MENU_TITLE[Act_Actions[SuperAction].Tab][Act_Actions[SuperAction].IndexInMenu];
  }

/*****************************************************************************/
/****************** Get the subtitle associated to an action *****************/
/*****************************************************************************/

const char *Act_GetSubtitleAction (Act_Action_t Action)
  {
   extern const char *Txt_MENU_SUBTITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   Act_Action_t SuperAction;

   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return NULL;

   SuperAction = Act_Actions[Action].SuperAction;
   return Txt_MENU_SUBTITLE[Act_Actions[SuperAction].Tab][Act_Actions[SuperAction].IndexInMenu];
  }

/*****************************************************************************/
/********************* Get text for action from database *********************/
/*****************************************************************************/

char *Act_GetActionTextFromDB (long ActCod,
                               char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1])
  {
   extern const char *Txt_STR_LANG_ID[1 + Txt_NUM_LANGUAGES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get test for an action from database *****/
   sprintf (Query,"SELECT Txt FROM actions"
	          " WHERE ActCod=%ld AND Language='%s'",
            ActCod,Txt_STR_LANG_ID[Txt_LANGUAGE_ES]);	// !!! TODO: Replace Txt_LANGUAGE_ES by Gbl.Prefs.Language !!!
   if (DB_QuerySELECT (Query,&mysql_res,"can not get text for an action"))
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
/******************************** Start a form *******************************/
/*****************************************************************************/

void Act_FormGoToStart (Act_Action_t NextAction)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   sprintf (Gbl.Form.Id,"form_%d",Gbl.Form.Num);
   Act_FormStartInternal (NextAction,false,Gbl.Form.Id,NULL,NULL);	// Do not put now parameter location
  }

void Act_FormStart (Act_Action_t NextAction)
  {
   Act_FormStartAnchorOnSubmit (NextAction,NULL,NULL);
  }

void Act_FormStartAnchor (Act_Action_t NextAction,const char *Anchor)
  {
   Act_FormStartAnchorOnSubmit (NextAction,Anchor,NULL);
  }

void Act_FormStartOnSubmit (Act_Action_t NextAction,const char *OnSubmit)
  {
   Act_FormStartAnchorOnSubmit (NextAction,NULL,OnSubmit);
  }

void Act_FormStartAnchorOnSubmit (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   sprintf (Gbl.Form.Id,"form_%d",Gbl.Form.Num);
   Act_FormStartInternal (NextAction,true,Gbl.Form.Id,Anchor,OnSubmit);	// Do put now parameter location (if no open session)
  }

void Act_FormStartUnique (Act_Action_t NextAction)
  {
   Act_FormStartUniqueAnchor (NextAction,NULL);
  }

void Act_FormStartUniqueAnchor (Act_Action_t NextAction,const char *Anchor)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   sprintf (Gbl.Form.UniqueId,"form_%s_%d",
            Gbl.UniqueNameEncrypted,Gbl.Form.Num);
   Act_FormStartInternal (NextAction,true,Gbl.Form.UniqueId,Anchor,NULL);	// Do put now parameter location (if no open session)
  }

void Act_FormStartId (Act_Action_t NextAction,const char *Id)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   Act_FormStartInternal (NextAction,true,Id,NULL,NULL);	// Do put now parameter location (if no open session)
  }

// Id can not be NULL
static void Act_FormStartInternal (Act_Action_t NextAction,bool PutParameterLocationIfNoSesion,
                                   const char *Id,const char *Anchor,const char *OnSubmit)
  {
   extern const char *Txt_STR_LANG_ID[1 + Txt_NUM_LANGUAGES];
   char ParamsStr[256 + 256 + Ses_BYTES_SESSION_ID + 256];

   if (!Gbl.Form.Inside)
     {
      /* Start form */
      fprintf (Gbl.F.Out,"<form method=\"post\" action=\"%s/%s",
	       Cfg_URL_SWAD_CGI,
	       Txt_STR_LANG_ID[Gbl.Prefs.Language]);
      if (Anchor)
	 if (Anchor[0])
            fprintf (Gbl.F.Out,"#%s",Anchor);
      fprintf (Gbl.F.Out,"\" id=\"%s\"",Id);
      if (OnSubmit)
         if (OnSubmit[0])
            fprintf (Gbl.F.Out," onsubmit=\"%s;\"",OnSubmit);
      switch (Act_Actions[NextAction].BrowserWindow)
	{
	 case Act_BLNK_WINDOW:
	 case Act_DOWNLD_FILE:
	    fprintf (Gbl.F.Out," target=\"_blank\"");
	    break;
	 default:
	    break;
	}
      if (Act_Actions[NextAction].ContentType == Act_CONT_DATA)
	 fprintf (Gbl.F.Out," enctype=\"multipart/form-data\"");
      fprintf (Gbl.F.Out," accept-charset=\"windows-1252\">");

      /* Put basic form parameters */
      Act_SetParamsForm (ParamsStr,NextAction,PutParameterLocationIfNoSesion);
      fprintf (Gbl.F.Out,"%s",ParamsStr);

      Gbl.Form.Inside = true;
     }
  }

// Params should have space for 256 + 256 + Ses_BYTES_SESSION_ID + 256 bytes
void Act_SetParamsForm (char *ParamsStr,Act_Action_t NextAction,
                        bool PutParameterLocationIfNoSesion)
  {
   char ParamAction[256];
   char ParamSession[256 + Ses_BYTES_SESSION_ID];
   char ParamLocation[256];

   ParamAction[0] = '\0';
   ParamSession[0] = '\0';
   ParamLocation[0] = '\0';

   if (NextAction != ActUnk)
      sprintf (ParamAction,"<input type=\"hidden\" name=\"act\""
	                   " value=\"%ld\" />",
	       Act_Actions[NextAction].ActCod);

   if (Gbl.Session.Id[0])
      sprintf (ParamSession,"<input type=\"hidden\" name=\"ses\""
	                    " value=\"%s\" />",
	       Gbl.Session.Id);
   else if (PutParameterLocationIfNoSesion)
      // Extra parameters necessary when there's no open session
     {
      /* If session is open, course code will be get from session data,
	 but if there is not an open session, and next action is known,
	 it is necessary to send a parameter with course code */
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	 // If course selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"crs\""
                                " value=\"%ld\" />",
                  Gbl.CurrentCrs.Crs.CrsCod);
      else if (Gbl.CurrentDeg.Deg.DegCod > 0)
	 // If no course selected, but degree selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"deg\""
                                " value=\"%ld\" />",
                  Gbl.CurrentDeg.Deg.DegCod);
      else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	 // If no degree selected, but centre selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"ctr\""
                                " value=\"%ld\" />",
                  Gbl.CurrentCtr.Ctr.CtrCod);
      else if (Gbl.CurrentIns.Ins.InsCod > 0)
	 // If no centre selected, but institution selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"ins\""
                                " value=\"%ld\" />",
                  Gbl.CurrentIns.Ins.InsCod);
      else if (Gbl.CurrentCty.Cty.CtyCod > 0)
	 // If no institution selected, but country selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"cty\""
                                " value=\"%ld\" />",
                  Gbl.CurrentCty.Cty.CtyCod);
     }

   sprintf (ParamsStr,"%s%s%s",ParamAction,ParamSession,ParamLocation);
  }

void Act_FormEnd (void)
  {
   if (Gbl.Form.Inside)
     {
      fprintf (Gbl.F.Out,"</form>");
      Gbl.Form.Inside = false;
     }
  }

/*****************************************************************************/
/******************* Anchor directive used to send a form ********************/
/*****************************************************************************/
// Requires an extern </a>

void Act_LinkFormSubmit (const char *Title,const char *LinkStyle,
                         const char *OnSubmit)
  {
   Act_LinkFormSubmitId (Title,LinkStyle,Gbl.Form.Id,OnSubmit);
  }

void Act_LinkFormSubmitUnique (const char *Title,const char *LinkStyle)
  {
   Act_LinkFormSubmitId (Title,LinkStyle,Gbl.Form.UniqueId,NULL);
  }

// Title can be NULL
// LinkStyle can be NULL
// Id can not be NULL
// OnSubmit can be NULL

void Act_LinkFormSubmitId (const char *Title,const char *LinkStyle,
                           const char *Id,const char *OnSubmit)
  {
   fprintf (Gbl.F.Out,"<a href=\"\"");
   if (Title)
      if (Title[0])
         fprintf (Gbl.F.Out," title=\"%s\"",Title);
   if (LinkStyle)
      if (LinkStyle[0])
         fprintf (Gbl.F.Out," class=\"%s\"",LinkStyle);
   fprintf (Gbl.F.Out," onclick=\"");
   if (OnSubmit)	// JavaScript function to be called
			// before submitting the form
      if (OnSubmit[0])
         fprintf (Gbl.F.Out,"%s;",OnSubmit);
   fprintf (Gbl.F.Out,"document.getElementById('%s').submit();"
		      "return false;\">",
	    Id);
  }

void Act_LinkFormSubmitAnimated (const char *Title,const char *LinkStyle,
                                 const char *OnSubmit)
  {
   fprintf (Gbl.F.Out,"<a href=\"\"");
   if (Title)
      if (Title[0])
         fprintf (Gbl.F.Out," title=\"%s\"",Title);
   if (LinkStyle)
      if (LinkStyle[0])
         fprintf (Gbl.F.Out," class=\"%s\"",LinkStyle);
   fprintf (Gbl.F.Out," onclick=\"");
   if (OnSubmit)	// JavaScript function to be called
			// before submitting the form
      if (OnSubmit[0])
         fprintf (Gbl.F.Out,"%s;",OnSubmit);
   fprintf (Gbl.F.Out,"AnimateIcon(%d);"
		      "document.getElementById('%s').submit();"
		      "return false;\">",
	    Gbl.Form.Num,
	    Gbl.Form.Id);
  }

/*****************************************************************************/
/***************************** Get unique Id *********************************/
/*****************************************************************************/

void Act_SetUniqueId (char UniqueId[Act_MAX_BYTES_ID])
  {
   static unsigned CountForThisExecution = 0;

   /***** Create Id. The id must be unique,
          the page content may be updated via AJAX.
          So, Id uses:
          - a name for this execution (Gbl.UniqueNameEncrypted)
          - a number for each element in this execution (CountForThisExecution) *****/
   sprintf (UniqueId,"id_%s_%u",
            Gbl.UniqueNameEncrypted,
            ++CountForThisExecution);
  }

/*****************************************************************************/
/***************** Adjust current action when no user's logged ***************/
/*****************************************************************************/

void Act_AdjustActionWhenNoUsrLogged (void)
  {
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)		// Course selected
      Gbl.Action.Act = ActSeeCrsInf;
   else if (Gbl.CurrentDeg.Deg.DegCod > 0)	// Degree selected
      Gbl.Action.Act = ActSeeDegInf;
   else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)	// Centre selected
      Gbl.Action.Act = ActSeeCtrInf;
   else if (Gbl.CurrentIns.Ins.InsCod > 0)	// Institution selected
      Gbl.Action.Act = ActSeeInsInf;
   else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected
      Gbl.Action.Act = ActSeeCtyInf;
   else
      Gbl.Action.Act = ActFrmLogIn;
   Tab_SetCurrentTab ();
  }

/*****************************************************************************/
/****** Adjust current action if it's mandatory to fill any empty data *******/
/*****************************************************************************/

void Act_AdjustCurrentAction (void)
  {
   bool IAmATeacher;

   /***** Don't adjust anything when current action is not a menu option *****/
   if (Gbl.Action.Act != Act_Actions[Gbl.Action.Act].SuperAction)	// It is not a menu option
      return;

   /***** Don't adjust anything when refreshing users or on a web service *****/
   if (Gbl.Action.UsesAJAX ||
       Gbl.WebService.IsWebService)
      return;

   /***** If I have no password in database,
          the only action possible is show a form to change my password *****/
   if (!Gbl.Usrs.Me.UsrDat.Password[0])
     {
      switch (Gbl.Action.Act)
        {
         case ActHom: case ActLogOut:
            break;
         default:
	    Gbl.Action.Act = ActFrmChgMyPwd;
	    Tab_SetCurrentTab ();
            return;
        }
     }

   /***** If my login password is not enough secure, the only action possible is show a form to change my password *****/
   if (Gbl.Action.Act == ActLogIn)
      if (!Pwd_FastCheckIfPasswordSeemsGood (Gbl.Usrs.Me.LoginPlainPassword))
        {
         Gbl.Action.Act = ActFrmChgMyPwd;
         Tab_SetCurrentTab ();
         return;
        }

   /***** If I have no nickname or email in database,
          the only action possible is show a form to change my account *****/
   if (!Gbl.Usrs.Me.UsrDat.Nickname[0] ||
       !Gbl.Usrs.Me.UsrDat.Email[0])
     {
      switch (Gbl.Action.Act)
        {
         case ActHom: case ActLogOut:
         case ActFrmChgMyPwd:
            break;
         default:
	    Gbl.Action.Act = ActFrmMyAcc;
	    Tab_SetCurrentTab ();
            return;
        }
     }

   /***** If I haven't filled my sex, first name, surname 1 or country,
          the only action possible is to show a form to change my common record *****/
   if ( Gbl.Usrs.Me.UsrDat.Sex == Usr_SEX_UNKNOWN ||
       !Gbl.Usrs.Me.UsrDat.FirstName[0]           ||
       !Gbl.Usrs.Me.UsrDat.Surname1[0]            ||
        Gbl.Usrs.Me.UsrDat.CtyCod <= 0)
      switch (Gbl.Action.Act)
        {
         case ActHom: case ActLogOut:
         case ActFrmMyAcc: case ActFrmChgMyPwd:
            break;
         default:
	    Gbl.Action.Act = ActReqEdiRecCom;
	    Tab_SetCurrentTab ();
            return;
        }

   /***** Check if I am a teacher *****/
   Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
   IAmATeacher = (Gbl.Usrs.Me.UsrDat.Roles & ((1 << Rol_NET) |	// I am a non-editing teacher...
	                                      (1 << Rol_TCH)));	// ...or a teacher in any course

   /***** If I haven't filled my institution,
          or if I'm a teacher and I haven't filled my centre or department,
          the only action possible is to show a form to change my common record *****/
   if (Gbl.Usrs.Me.UsrDat.InsCod < 0 ||
       (IAmATeacher && (Gbl.Usrs.Me.UsrDat.Tch.CtrCod < 0 ||
                        Gbl.Usrs.Me.UsrDat.Tch.DptCod < 0)))
      switch (Gbl.Action.Act)
        {
         case ActHom: case ActLogOut:
         case ActFrmMyAcc: case ActFrmChgMyPwd:
         case ActReqEdiRecCom:
            break;
         default:
	    Gbl.Action.Act = ActReqEdiMyIns;
	    Tab_SetCurrentTab ();
            return;
        }

   /***** If any of my preferences about privacy is unknown *****/
   if (Gbl.Usrs.Me.UsrDat.PhotoVisibility   == Pri_VISIBILITY_UNKNOWN ||
       Gbl.Usrs.Me.UsrDat.ProfileVisibility == Pri_VISIBILITY_UNKNOWN)
      switch (Gbl.Action.Act)
        {
         case ActHom: case ActLogOut:
         case ActFrmMyAcc: case ActFrmChgMyPwd:
         case ActReqEdiRecCom: case ActReqEdiMyIns:
            break;
         default:
	    Gbl.Action.Act = ActEdiPri;
	    Tab_SetCurrentTab ();
            return;
        }

   /***** If I belong to current course *****/
   if (Gbl.Usrs.Me.IBelongToCurrentCrs)
     {
      /***** If I have not accepted the enrolment,
             the only action possible is show a form to ask for enrolment *****/
      if (!Gbl.Usrs.Me.UsrDat.Accepted && Gbl.Action.Act != ActLogOut)
	{
	 switch (Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB)
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
	       return;
	   }
	 Tab_SetCurrentTab ();
	 return;
	}

      /***** Depending on the role I am logged... *****/
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_STD:
            switch (Gbl.Action.Act)
              {
               case ActLogIn:
               case ActLogInNew:
               case ActHom:
               case ActMnu:
               case ActLogOut:
               case ActFrmMyAcc:
               case ActFrmChgMyPwd:
               case ActReqEdiRecCom:
               case ActReqEdiMyIns:
               case ActEdiPri:
               case ActReqSelGrp:
               case ActReqMdfOneStd:	// A student use this action to remove him/herself from current course
               case ActSeeCrsTT:
               case ActPrnCrsTT:
               case ActChgCrsTT1stDay:
                  // These last actions are allowed in order to students could see/print timetable before register in groups
                  break;
               default:
                  /* If there are some group types with mandatory enrolment and groups in this course,
	             and I don't belong to any of those groups,
	             the the only action possible is show a form to register in groups */
                  if (Grp_NumGrpTypesMandatIDontBelongAsStd ())	// To do: if this query is too slow ==> put it only when login.
                    {
	             Gbl.Action.Act = ActReqSelGrp;
	             Tab_SetCurrentTab ();
                     return;
	            }

                  /* If I have no photo, and current action is not available for unknown users,
                     then update number of clicks without photo */
                  if (!Gbl.Usrs.Me.MyPhotoExists)
                     if (!(Act_Actions[Gbl.Action.Act].PermissionCrsIfIBelong & (1 << Rol_UNK)) &&	// If current action is not available for unknown users...
                         Gbl.Action.Act != ActReqMyPho)	// ...and current action is not ActReqMyPho...
                        if ((Gbl.Usrs.Me.NumAccWithoutPhoto = Pho_UpdateMyClicksWithoutPhoto ()) > Pho_MAX_CLICKS_WITHOUT_PHOTO)
	                  {
	                   /* If limit of clicks has been reached,
	                      the only action possible is show a form to send my photo */
	                   Gbl.Action.Act = ActReqMyPho;
	                   Tab_SetCurrentTab ();
                           return;
	                 }
                  break;
              }

            /***** Check if it is mandatory to read any information about course *****/
            if (Gbl.Action.Act == ActMnu)	// Do the following check sometimes, for example when the user changes the current tab
               Gbl.CurrentCrs.Info.ShowMsgMustBeRead = Inf_GetIfIMustReadAnyCrsInfoInThisCrs ();
            break;
         case Rol_NET:
            break;
         case Rol_TCH:
            if (Gbl.Action.Act == ActReqTst ||
                Gbl.Action.Act == ActEdiTstQst)
               /***** If current course has tests and pluggable is unknown,
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
  }
