// swad_action.c: actions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
#include "swad_degree_type.h"
#include "swad_duplicate.h"
#include "swad_exam.h"
#include "swad_enrollment.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_indicator.h"
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
#include "swad_profile.h"
#include "swad_QR.h"
#include "swad_report.h"
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
1209 actions in one CGI:
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
	 12. ActSeePen			List pending institutions, centres, degrees and courses
	 13. ActReqRemOldCrs		Request the total removal of old courses
	 14. ActSeeDegTyp		List types of degrees
	 15. ActSeeMai			See mail domains
	 16. ActSeeBan			See banners
	 17. ActSeeLnk			See institutional links
	 18. ActLstPlg			List plugins
	 19. ActSetUp			Initial set up

	 20. ActSysSch			Search for courses, teachers, documents...
	 21. ActEdiDegTyp		Request edition of types of degrees
	 22. ActEdiCty			Edit countries
	 23. ActNewCty			Request the creation of a country
	 24. ActRemCty			Remove a country
	 25. ActRenCty			Change the name of a country
	 26. ActChgCtyWWW		Change web of country

	 27. ActNewDegTyp		Request the creation of a type of degree
	 28. ActRemDegTyp		Request the removal of a type of degree
	 29. ActRenDegTyp		Request renaming of a type of degree

	 30. ActRemOldCrs		Remove completely old courses
	 31. ActEdiMai			Edit mail domains
	 32. ActNewMai			Request the creation of a mail domain
	 33. ActRemMai			Request the removal of a mail domain
	 34. ActRenMaiSho		Request the change of a mail domain
	 35. ActRenMaiFul		Request the change of the info of a mail domain

	 36. ActEdiBan			Edit banners
	 37. ActNewBan			Request the creation of a banner
	 38. ActRemBan			Request the removal of a banner
	 39. ActShoBan			Show a hidden banner
	 40. ActHidBan			Hide a visible banner
	 41. ActRenBanSho		Request the change of the short name of a banner
	 42. ActRenBanFul		Request the change of the full name of a banner
	 43. ActChgBanImg		Request the change of the image of a banner
	 44. ActChgBanWWW		Request the change of the web of a banner
	 45. ActClkBan			Go to a banner when clicked

	 46. ActEdiLnk			Edit institutional links
	 47. ActNewLnk			Request the creation of an institutional link
	 48. ActRemLnk			Request the removal of an institutional link
	 49. ActRenLnkSho		Request the change of the short name of an institutional link
	 50. ActRenLnkFul		Request the change of the full name of an institutional link
	 51. ActChgLnkWWW		Request the change of the web of an institutional link
	 52. ActEdiPlg			Edit plugins
	 53. ActNewPlg			Request the creation of a plugin
	 54. ActRemPlg			Remove a plugin
	 55. ActRenPlg			Rename a plugin
	 56. ActChgPlgDes		Change the description of a plugin
	 57. ActChgPlgLog		Change the logo of a plugin
	 58. ActChgPlgAppKey		Change the application key of a plugin
	 59. ActChgPlgURL		Change the URL of a plugin
	 60. ActChgPlgIP		Change the IP address of a plugin
Country:
	 61. ActCtyReqSch		Request search in country tab

	 62. ActSeeCtyInf		Show information on the current country
	 63. ActSeeIns			List institutions

	 64. ActCtySch			Search for courses, teachers, documents...

	 65. ActPrnCtyInf		Print information on the current country
	 66. ActChgCtyMapAtt		Change map attribution of country

	 67. ActEdiIns			Edit institutions
	 68. ActReqIns			Request the creation of a request for a new institution (a teacher makes the petition to an administrator)
	 69. ActNewIns			Request the creation of an institution
	 70. ActRemIns			Remove institution
	 72. ActRenInsSho		Change short name of institution
	 73. ActRenInsFul		Change full name of institution
	 74. ActChgInsWWW		Change web of institution
	 75. ActChgInsSta		Request change of status of an institution
Institution:
	 76. ActInsReqSch		Request search in institution tab

	 77. ActSeeInsInf		Show information on the current institution
	 78. ActSeeCtr			List centres
	 79. ActSeeDpt			List departments
	 80. ActSeePlc			List places
	 81. ActSeeHld			List holidays
	 82. ActSeeAdmDocIns		Documents zone of the institution (see or admin)
	 83. ActAdmShaIns		Admin the shared files zone of the institution

	 84. ActInsSch			Search for courses, teachers, documents...

	 85. ActPrnInsInf		Print information on the current institution
	NEW. ActChgInsCtyCfg		Change country of institution in institution configuration
	 86. ActReqInsLog		Show form to send the logo of the current institution
	 87. ActRecInsLog		Receive and store the logo of the current institution
	 88. ActRemInsLog		Remove the logo of the current institution

	 89. ActEdiCtr			Edit centres
	 90. ActReqCtr			Request the creation of a request for a new centre (a teacher makes the petition to an administrator)
	 91. ActNewCtr			Request the creation of a centre
	 92. ActRemCtr			Remove centre
	 94. ActChgCtrPlc		Request change of the place of a centre
	 95. ActRenCtrSho		Change short name centre
	 96. ActRenCtrFul		Change full name centre
	 97. ActChgCtrWWW		Change web of centre
	 98. ActChgCtrSta		Request change of status of a centre

	 99. ActEdiDpt			Edit departments
	100. ActNewDpt			Request the creation of a department
	101. ActRemDpt			Remove department
	102. ActChgDptIns		Request change of the institution of a department
	103. ActRenDptSho		Change short name department
	104. ActRenDptFul		Change full name department
	105. ActChgDptWWW		Change web of department

	106. ActEdiPlc			Edit places
	107. ActNewPlc			Request the creation of a place
	108. ActRemPlc			Remove a place
	109. ActRenPlcSho		Change short name of a place
	110. ActRenPlcFul		Change full name of a place

	111. ActEdiHld			Edit holidays
	112. ActNewHld			Request the creation of a holiday
	113. ActRemHld			Remove a holiday
	114. ActChgHldPlc		Change place of a holiday
	115. ActChgHldTyp		Change type of a holiday
	116. ActChgHldStrDat		Change start date of a holiday
	117. ActChgHldEndDat		Change end date of a holiday
	118. ActRenHld			Change name of a holiday

	119. ActChgToSeeDocIns		Change to see institution documents

	120. ActSeeDocIns		See the files of the documents zone of the institution
	121. ActExpSeeDocIns		Expand a folder in institution documents
	122. ActConSeeDocIns		Contract a folder in institution documents
	123. ActZIPSeeDocIns		Compress a folder in institution documents
	124. ActReqDatSeeDocIns		Ask for metadata of a file of the documents zone of the institution
	125. ActDowSeeDocIns		Download a file in documents zone of the institution

	126. ActChgToAdmDocIns		Change to administrate institution documents

	127. ActAdmDocIns		Admin the files of the documents zone of the institution
	128. ActReqRemFilDocIns		Request removal of a file in the documents zone of the institution
	129. ActRemFilDocIns		Remove a file in the documents zone of the institution
	130. ActRemFolDocIns		Remove a folder empty the documents zone of the institution
	131. ActCopDocIns		Set source of copy in the documents zone of the institution
	132. ActPasDocIns		Paste a folder or file in the documents zone of the institution
	133. ActRemTreDocIns		Remove a folder no empty in the documents zone of the institution
	134. ActFrmCreDocIns		Form to crear a folder or file in the documents zone of the institution
	135. ActCreFolDocIns		Create a new folder in the documents zone of the institution
	136. ActCreLnkDocIns		Create a new link in the documents zone of the institution
	137. ActRenFolDocIns		Rename a folder in the documents zone of the institution
	138. ActRcvFilDocInsDZ		Receive a file in the documents zone of the institution using Dropzone.js
	139. ActRcvFilDocInsCla		Receive a file in the documents zone of the institution using the classic way
	140. ActExpAdmDocIns		Expand a folder when administrating the documents zone of the institution
	141. ActConAdmDocIns		Contract a folder when administrating the documents zone of the institution
	142. ActZIPAdmDocIns		Compress a folder when administrating the documents zone of the institution
	143. ActShoDocIns		Show hidden folder or file of the documents zone of the institution
	144. ActHidDocIns		Hide folder or file of the documents zone of the institution
	145. ActReqDatAdmDocIns		Ask for metadata of a file of the documents zone of the institution
	146. ActChgDatAdmDocIns		Change metadata of a file of the documents zone of the institution
	147. ActDowAdmDocIns		Download a file of the documents zone of the institution

	148. ActReqRemFilShaIns		Request removal of a shared file of the institution
	149. ActRemFilShaIns		Remove a shared file of the institution
	150. ActRemFolShaIns		Remove an empty shared folder of the institution
	151. ActCopShaIns		Set source of copy in shared zone of the institution
	152. ActPasShaIns		Paste a folder or file in shared zone of the institution
	153. ActRemTreShaIns		Remove a folder no empty of shared zone of the institution
	154. ActFrmCreShaIns		Form to crear a folder or file in shared zone of the institution
	155. ActCreFolShaIns		Create a new folder in shared zone of the institution
	156. ActCreLnkShaIns		Create a new link in shared zone of the institution
	157. ActRenFolShaIns		Rename a folder in shared zone of the institution
	158. ActRcvFilShaInsDZ		Receive a file in shared zone of the institution using Dropzone.js
	159. ActRcvFilShaInsCla		Receive a file in shared zone of the institution using the classic way
	160. ActExpShaIns		Expand a folder in shared zone of the institution
	161. ActConShaIns		Contract a folder in shared zone of the institution
	162. ActZIPShaIns		Compress a folder in shared zone of the institution
	163. ActReqDatShaIns		Ask for metadata of a file of the shared zone of the institution
	164. ActChgDatShaIns		Change metadata of a file of the shared zone of the institution
	165. ActDowShaIns		Download a file of the shared zone of the institution
Centre:
	166. ActCtrReqSch		Request search in centre tab

	167. ActSeeCtrInf		Show information on the current centre
	168. ActSeeDeg			List degrees of a type
	169. ActSeeAdmDocCtr		Documents zone of the centre (see or admin)
	170. ActAdmShaCtr		Admin the shared files zone of the centre

	171. ActCtrSch			Search for courses, teachers, documents...

	172. ActPrnCtrInf		Print information on the current centre
	NEW. ActChgCtrInsCfg		Request change of the institution of a centre in centre configuration
	173. ActReqCtrLog		Show form to send the logo of the current centre
	174. ActRecCtrLog		Receive and store the logo of the current centre
	175. ActRemCtrLog		Remove the logo of the current centre
	176. ActReqCtrPho		Show form to send the photo of the current centre
	177. ActRecCtrPho		Receive and store the photo of the current centre
	178. ActChgCtrPhoAtt		Change attribution of centre photo

	179. ActEdiDeg			Request edition of degrees of a type
	180. ActReqDeg			Request the creation of a request for a new degree (a teacher makes the petition to an administrator)
	181. ActNewDeg			Request the creation of a degree
	182. ActRemDeg			Request the removal of a degree
	183. ActRenDegSho		Request change of the short name of a degree
	184. ActRenDegFul		Request change of the full name of a degree
	185. ActChgDegTyp		Request change of the type of a degree
	187. ActChgDegWWW		Request change of the web of a degree
	188. ActChgDegSta		Request change of status of a degree

	189. ActChgToSeeDocCtr		Change to see centre documents

	190. ActSeeDocCtr		See the files of the documents zone of the centre
	191. ActExpSeeDocCtr		Expand a folder in centre documents
	192. ActConSeeDocCtr		Contract a folder in centre documents
	193. ActZIPSeeDocCtr		Compress a folder in centre documents
	194. ActReqDatSeeDocCtr		Ask for metadata of a file of the documents zone of the centre
	195. ActDowSeeDocCtr		Download a file in documents zone of the centre

	196. ActChgToAdmDocCtr		Change to administrate centre documents

	197. ActAdmDocCtr		Admin the files of the documents zone of the centre
	198. ActReqRemFilDocCtr		Request removal of a file in the documents zone of the centre
	199. ActRemFilDocCtr		Remove a file in the documents zone of the centre
	200. ActRemFolDocCtr		Remove a folder empty the documents zone of the centre
	201. ActCopDocCtr		Set source of copy in the documents zone of the centre
	202. ActPasDocCtr		Paste a folder or file in the documents zone of the centre
	203. ActRemTreDocCtr		Remove a folder no empty in the documents zone of the centre
	204. ActFrmCreDocCtr		Form to crear a folder or file in the documents zone of the centre
	205. ActCreFolDocCtr		Create a new folder in the documents zone of the centre
	206. ActCreLnkDocCtr		Create a new link in the documents zone of the centre
	207. ActRenFolDocCtr		Rename a folder in the documents zone of the centre
	208. ActRcvFilDocCtrDZ		Receive a file in the documents zone of the centre using Dropzone.js
	209. ActRcvFilDocCtrCla		Receive a file in the documents zone of the centre using the classic way
	210. ActExpAdmDocCtr		Expand a folder when administrating the documents zone of the centre
	211. ActConAdmDocCtr		Contract a folder when administrating the documents zone of the centre
	212. ActZIPAdmDocCtr		Compress a folder when administrating the documents zone of the centre
	213. ActShoDocCtr		Show hidden folder or file of the documents zone of the centre
	214. ActHidDocCtr		Hide folder or file of the documents zone of the centre
	215. ActReqDatAdmDocCtr		Ask for metadata of a file of the documents zone of the centre
	216. ActChgDatAdmDocCtr		Change metadata of a file of the documents zone of the centre
	217. ActDowAdmDocCtr		Download a file of the documents zone of the centre

	218. ActReqRemFilShaCtr		Request removal of a shared file of the centre
	219. ActRemFilShaCtr		Remove a shared file of the centre
	220. ActRemFolShaCtr		Remove an empty shared folder of the centre
	221. ActCopShaCtr		Set source of copy in shared zone of the centre
	222. ActPasShaCtr		Paste a folder or file in shared zone of the centre
	223. ActRemTreShaCtr		Remove a folder no empty of shared zone of the centre
	224. ActFrmCreShaCtr		Form to crear a folder or file in shared zone of the centre
	225. ActCreFolShaCtr		Create a new folder in shared zone of the centre
	226. ActCreLnkShaCtr		Create a new link in shared zone of the centre
	227. ActRenFolShaCtr		Rename a folder in shared zone of the centre
	228. ActRcvFilShaCtrDZ		Receive a file in shared zone of the centre using Dropzone.js
	229. ActRcvFilShaCtrCla		Receive a file in shared zone of the centre using the classic way
	230. ActExpShaCtr		Expand a folder in shared zone of the centre
	231. ActConShaCtr		Contract a folder in shared zone of the centre
	232. ActZIPShaCtr		Compress a folder in shared zone of the centre
	233. ActReqDatShaCtr		Ask for metadata of a file of the shared zone of the centre
	234. ActChgDatShaCtr		Change metadata of a file of the shared zone of the centre
	235. ActDowShaCtr		Download a file of the shared zone of the centre
Degree:
	236. ActDegReqSch		Request search in degree tab

	237. ActSeeDegInf		Show information on the current degree
	238. ActSeeCrs			List courses of a degree
	239. ActSeeAdmDocDeg		Documents zone of the degree (see or admin)
	240. ActAdmShaDeg		Admin the shared files zone of the degree

	241. ActDegSch			Search for courses, teachers, documents...

	242. ActPrnDegInf		Print information on the current degree
	NEW. ActChgDegCtrCfg		Request change of the centre of a degree in degree configuration
	243. ActReqDegLog		Show form to send the logo of the current degree
	244. ActRecDegLog		Receive and store the logo of the current degree
	245. ActRemDegLog		Remove the logo of the current degree

	246. ActEdiCrs			Request edition of courses of a degree
	247. ActReqCrs			Request the creation of a request for a new course (a teacher makes the petition to an administrator)
	248. ActNewCrs			Request the creation of a course
	249. ActRemCrs			Request the removal of a course
	250. ActChgInsCrsCod		Request change of institutional code of a course
	252. ActChgCrsYea		Request change of year of a course inside of its degree
	253. ActRenCrsSho		Request change of short name of a course
	254. ActRenCrsFul		Request change of full name of a course
	255. ActChgCrsSta		Request change of status of a course

	256. ActChgToSeeDocDeg		Change to see degree documents

	257. ActSeeDocDeg		See the files of the documents zone of the degree
	258. ActExpSeeDocDeg		Expand a folder in degree documents
	259. ActConSeeDocDeg		Contract a folder in degree documents
	260. ActZIPSeeDocDeg		Compress a folder in degree documents
	261. ActReqDatSeeDocDeg		Ask for metadata of a file of the documents zone of the degree
	262. ActDowSeeDocDeg		Download a file in documents zone of the degree

	263. ActChgToAdmDocDeg		Change to administrate degree documents

	264. ActAdmDocDeg		Admin the files of the documents zone of the degree
	265. ActReqRemFilDocDeg		Request removal of a file in the documents zone of the degree
	266. ActRemFilDocDeg		Remove a file in the documents zone of the degree
	267. ActRemFolDocDeg		Remove a folder empty the documents zone of the degree
	268. ActCopDocDeg		Set source of copy in the documents zone of the degree
	269. ActPasDocDeg		Paste a folder or file in the documents zone of the degree
	270. ActRemTreDocDeg		Remove a folder no empty in the documents zone of the degree
	271. ActFrmCreDocDeg		Form to crear a folder or file in the documents zone of the degree
	272. ActCreFolDocDeg		Create a new folder in the documents zone of the degree
	273. ActCreLnkDocDeg		Create a new link in the documents zone of the degree
	274. ActRenFolDocDeg		Rename a folder in the documents zone of the degree
	275. ActRcvFilDocDegDZ		Receive a file in the documents zone of the degree using Dropzone.js
	276. ActRcvFilDocDegCla		Receive a file in the documents zone of the degree using the classic way
	277. ActExpAdmDocDeg		Expand a folder when administrating the documents zone of the degree
	278. ActConAdmDocDeg		Contract a folder when administrating the documents zone of the degree
	279. ActZIPAdmDocDeg		Compress a folder when administrating the documents zone of the degree
	280. ActShoDocDeg		Show hidden folder or file of the documents zone of the degree
	281. ActHidDocDeg		Hide folder or file of the documents zone of the degree
	282. ActReqDatAdmDocDeg		Ask for metadata of a file of the documents zone of the degree
	283. ActChgDatAdmDocDeg		Change metadata of a file of the documents zone of the degree
	284. ActDowAdmDocDeg		Download a file of the documents zone of the degree

	285. ActReqRemFilShaDeg		Request removal of a shared file of the degree
	286. ActRemFilShaDeg		Remove a shared file of the degree
	287. ActRemFolShaDeg		Remove an empty shared folder of the degree
	288. ActCopShaDeg		Set source of copy in shared zone of the degree
	289. ActPasShaDeg		Paste a folder or file in shared zone of the degree
	290. ActRemTreShaDeg		Remove a folder no empty of shared zone of the degree
	291. ActFrmCreShaDeg		Form to crear a folder or file in shared zone of the degree
	292. ActCreFolShaDeg		Create a new folder in shared zone of the degree
	293. ActCreLnkShaDeg		Create a new link in shared zone of the degree
	294. ActRenFolShaDeg		Rename a folder in shared zone of the degree
	295. ActRcvFilShaDegDZ		Receive a file in shared zone of the degree using Dropzone.js
	296. ActRcvFilShaDegCla		Receive a file in shared zone of the degree using the classic way
	297. ActExpShaDeg		Expand a folder in shared zone of the degree
	298. ActConShaDeg		Contract a folder in shared zone of the degree
	299. ActZIPShaDeg		Compress a folder in shared zone of the degree
	300. ActReqDatShaDeg		Ask for metadata of a file of the shared zone of the degree
	301. ActChgDatShaDeg		Change metadata of a file of the shared zone of the degree
	302. ActDowShaDeg		Download a file of the shared zone of the degree
Course:
	303. ActCrsReqSch		Request search in course tab

	304. ActSeeCrsInf		Show information on the current course
	305. ActSeeTchGui		Show teaching guide of the course
	306. ActSeeSyl			Show syllabus (lectures or practicals)
	307. ActSeeSylLec		Show the syllabus of lectures
	308. ActSeeSylPra		Show the syllabus of practicals
	309. ActSeeAdmDocCrsGrp		Documents zone of the course (see or admin)

	310. ActAdmTchCrsGrp		Request the administration of the files of the teachers' zone of the course or of a group
	311. ActAdmShaCrsGrp		Request the administration of the files of the shared zone of the course or of a group
	312. ActSeeCrsTT		Show the timetable
	313. ActSeeBib			Show the bibliography
	314. ActSeeFAQ			Show the FAQ
	315. ActSeeCrsLnk		Show links related to the course

	316. ActCrsSch			Search for courses, teachers, documents...

	317. ActPrnCrsInf		Print information on the course
	NEW. ActChgCrsDegCfg		Request change of degree in course configuration
	318. ActChgInsCrsCodCfg		Change institutional code in course configuration
	319. ActChgCrsYeaCfg		Change year/semester in course configuration
	320. ActEdiCrsInf		Edit general information about the course
	321. ActEdiTchGui		Edit teaching guide of the course
	322. ActPrnCrsTT		Show print view of the timetable
	323. ActEdiCrsTT		Edit the timetable
	324. ActChgCrsTT		Modify the timetable of the course
        325. ActChgCrsTT1stDay		Change first day of week and show timetable of the course
	326. ActEdiSylLec		Edit the syllabus of lectures
	327. ActEdiSylPra		Edit the syllabus of practicals
	328. ActDelItmSylLec		Remove a item from syllabus of lectures
	329. ActDelItmSylPra		Remove a item from syllabus of practicals
	330. ActUp_IteSylLec		Subir the posición of a subtree of the syllabus of lectures
	331. ActUp_IteSylPra		Subir the posición of a subtree of the syllabus of practicals
	332. ActDwnIteSylLec		Bajar the posición of a subtree of the syllabus of lectures
	333. ActDwnIteSylPra		Bajar the posición of a subtree of the syllabus of practicals
	334. ActRgtIteSylLec		Aumentar the level of a item of the syllabus of lectures
	335. ActRgtIteSylPra		Aumentar the level of a item of the syllabus of practicals
	336. ActLftIteSylLec		Disminuir the level of a item of the syllabus of lectures
	337. ActLftIteSylPra		Disminuir the level of a item of the syllabus of practicals
	338. ActInsIteSylLec		Insertar a new item in the syllabus of lectures
	339. ActInsIteSylPra		Insertar a new item in the syllabus of practicals
	340. ActModIteSylLec		Modify a item of the syllabus of lectures
	341. ActModIteSylPra		Modify a item of the syllabus of practicals

	342. ActChgToSeeDocCrs		Change to see course/group documents

	343. ActSeeDocCrs		See the files of the documents zone of the course
	344. ActExpSeeDocCrs		Expand a folder in course documents
	345. ActConSeeDocCrs		Contract a folder in course documents
	346. ActZIPSeeDocCrs		Compress a folder in course documents
	347. ActReqDatSeeDocCrs		Ask for metadata of a file of the documents zone of the course
	348. ActDowSeeDocCrs		Download a file in documents zone of the course

	349. ActSeeDocGrp		See the files of the documents zone of a group
	350. ActExpSeeDocGrp		Expand a folder in group documents
	351. ActConSeeDocGrp		Contract a folder in group documents
	352. ActZIPSeeDocGrp		Compress a folder in group documents
	353. ActReqDatSeeDocGrp		Ask for metadata of a file of the documents zone of a group
	354. ActDowSeeDocGrp		Download a file of the documents zone of a group

	355. ActChgToAdmDocCrs		Change to administrate course/group documents

	356. ActAdmDocCrs		Admin the files of the documents zone of the course
	357. ActReqRemFilDocCrs		Request removal of a file in the documents zone of the course
	358. ActRemFilDocCrs		Remove a file in the documents zone of the course
	359. ActRemFolDocCrs		Remove a folder empty the documents zone of the course
	360. ActCopDocCrs		Set source of copy in the documents zone of the course
	361. ActPasDocCrs		Paste a folder or file in the documents zone of the course
	362. ActRemTreDocCrs		Remove a folder no empty in the documents zone of the course
	363. ActFrmCreDocCrs		Form to crear a folder or file in the documents zone of the course
	364. ActCreFolDocCrs		Create a new folder in the documents zone of the course
	365. ActCreLnkDocCrs		Create a new link in the documents zone of the course
	366. ActRenFolDocCrs		Rename a folder in the documents zone of the course
	367. ActRcvFilDocCrsDZ		Receive a file in the documents zone of the course using Dropzone.js
	368. ActRcvFilDocCrsCla		Receive a file in the documents zone of the course using the classic way
	369. ActExpAdmDocCrs		Expand a folder when administrating the documents zone of the course
	370. ActConAdmDocCrs		Contract a folder when administrating the documents zone of the course
	371. ActZIPAdmDocCrs		Compress a folder when administrating the documents zone of the course
	372. ActShoDocCrs		Show hidden folder or file of the documents zone
	373. ActHidDocCrs		Hide folder or file of the documents zone
	374. ActReqDatAdmDocCrs		Ask for metadata of a file of the documents zone
	375. ActChgDatAdmDocCrs		Change metadata of a file of the documents zone
	376. ActDowAdmDocCrs		Download a file of the documents zone

	377. ActAdmDocGrp		Admin the files of the documents zone of a group
	378. ActReqRemFilDocGrp		Request removal of a file in the documents zone of a group
	379. ActRemFilDocGrp		Remove a file in the documents zone of a group
	380. ActRemFolDocGrp		Remove a folder empty in the documents zone of a group
	381. ActCopDocGrp		Set source of copy in the documents zone of a group
	382. ActPasDocGrp		Paste a folder or file in the documents zone of a group
	383. ActRemTreDocGrp		Remove a folder no empty in the documents zone of a group
	384. ActFrmCreDocGrp		Form to crear a folder or file in the documents zone of a group
	385. ActCreFolDocGrp		Create a new folder in the documents zone of a group
	386. ActCreLnkDocGrp		Create a new link in the documents zone of a group
	387. ActRenFolDocGrp		Rename a folder in the documents zone of a group
	388. ActRcvFilDocGrpDZ		Receive a file in the documents zone of a group using Dropzone.js
	389. ActRcvFilDocGrpCla		Receive a file in the documents zone of a group using the classic way
	390. ActExpAdmDocGrp		Expand a folder when administrating the documents zone of a group
	391. ActConAdmDocGrp		Contract a folder when administrating the documents zone of a group
	392. ActZIPAdmDocGrp		Compress a folder when administrating the documents zone of a group
	393. ActShoDocGrp		Show hidden folder or file of download
	394. ActHidDocGrp		Hide folder or file of download
	395. ActReqDatAdmDocGrp		Ask for metadata of a file of the documents zone of a group
	396. ActChgDatAdmDocGrp		Change metadata of a file of the documents zone of a group
	397. ActDowAdmDocGrp		Download a file of the documents zone of a group

	398. ActChgToAdmTch		Change to administrate teachers' files of the course/group

	399. ActAdmTchCrs		Admin the teachers' files zone of the course
	400. ActReqRemFilTchCrs		Request removal of a teachers' file of the course
	401. ActRemFilTchCrs		Remove a teachers' file of the course
	402. ActRemFolTchCrs		Remove an empty teachers' folder of the course
	403. ActCopTchCrs		Set source of teachers' zone of the course
	404. ActPasTchCrs		Paste a folder or file in teachers' zone of the course
	405. ActRemTreTchCrs		Remove a folder no empty of teachers' zone of the course
	406. ActFrmCreTchCrs		Form to crear a folder or file in teachers' zone of the course
	407. ActCreFolTchCrs		Create a new folder in teachers' zone of the course
	408. ActCreLnkTchCrs		Create a new link in teachers' zone of the course
	409. ActRenFolTchCrs		Rename a folder in teachers' zone of the course
	410. ActRcvFilTchCrsDZ		Receive a file in teachers' zone of the course using Dropzone.js
	411. ActRcvFilTchCrsCla		Receive a file in teachers' zone of the course using the classic way
	412. ActExpTchCrs		Expand a folder in teachers' zone of the course
	413. ActConTchCrs		Contract a folder in teachers' zone of the course
	414. ActZIPTchCrs		Compress a folder in teachers' zone of the course
	415. ActReqDatTchCrs		Ask for metadata of a file of the teachers' zone of the course
	416. ActChgDatTchCrs		Change metadata of a file of the teachers' zone of the course
	417. ActDowTchCrs		Download a file of the teachers' zone of the course

	418. ActAdmTchGrp		Admin the teachers' files zone of a group
	419. ActReqRemFilTchGrp		Request removal of a teachers' file of a group
	420. ActRemFilTchGrp		Remove a teachers' file of a group
	421. ActRemFolTchGrp		Remove an empty teachers' folder of a group
	422. ActCopTchGrp		Set source of copy in teachers' zone of a group
	423. ActPasTchGrp		Paste a folder or file in teachers' zone of a group
	424. ActRemTreTchGrp		Remove a folder no empty of teachers' zone of a group
	425. ActFrmCreTchGrp		Form to crear a folder or file in teachers' zone of a group
	426. ActCreFolTchGrp		Create a new folder in teachers' zone of a group
	427. ActCreLnkTchGrp		Create a new link in teachers' zone of a group
	428. ActRenFolTchGrp		Rename a folder in teachers' zone of a group
	429. ActRcvFilTchGrpDZ		Receive a file in teachers' zone of a group using Dropzone.js
	430. ActRcvFilTchGrpCla		Receive a file in teachers' zone of a group using the classic way
	431. ActExpTchGrp		Expand a folder in teachers' zone of a group
	432. ActConTchGrp		Contract a folder in teachers' zone of a group
	433. ActZIPTchGrp		Compress a folder in teachers' zone of a group
	434. ActReqDatTchGrp		Ask for metadata of a file of the teachers' zone of a group
	435. ActChgDatTchGrp		Change metadata of a file of the teachers' zone of a group
	436. ActDowTchGrp		Download a file of the teachers' zone of a group

	437. ActChgToAdmSha		Change to administrate shared files of the course/group

	438. ActAdmShaCrs		Admin the shared files zone of the course
	439. ActReqRemFilShaCrs		Request removal of a shared file of the course
	440. ActRemFilShaCrs		Remove a shared file of the course
	441. ActRemFolShaCrs		Remove a empty shared folder of the course
	442. ActCopShaCrs		Set source of copy in shared zone of the course
	443. ActPasShaCrs		Paste a folder or file in shared zone of the course
	444. ActRemTreShaCrs		Remove a folder no empty of shared zone of the course
	445. ActFrmCreShaCrs		Form to crear a folder or file in shared zone of the course
	446. ActCreFolShaCrs		Create a new folder in shared zone of the course
	447. ActCreLnkShaCrs		Create a new link in shared zone of the course
	448. ActRenFolShaCrs		Rename a folder in shared zone of the course
	449. ActRcvFilShaCrsDZ		Receive a file in shared zone of the course using Dropzone.js
	450. ActRcvFilShaCrsCla		Receive a file in shared zone of the course using the classic way
	451. ActExpShaCrs		Expand a folder in shared zone of the course
	452. ActConShaCrs		Contract a folder in shared zone of the course
	453. ActZIPShaCrs		Compress a folder in shared zone of the course
	454. ActReqDatShaCrs		Ask for metadata of a file of the shared zone of the course
	455. ActChgDatShaCrs		Change metadata of a file of the shared zone of the course
	456. ActDowShaCrs		Download a file of the shared zone of the course

	457. ActAdmShaGrp		Admin the shared files zone of a group
	458. ActReqRemFilShaGrp		Request removal of a shared file of a group
	459. ActRemFilShaGrp		Remove a shared file of a group
	460. ActRemFolShaGrp		Remove a folder empty común of a group
	461. ActCopShaGrp		Set source of copy in shared zone of a group
	462. ActPasShaGrp		Paste a folder or file in shared zone of a group
	463. ActRemTreShaGrp		Remove a folder no empty of shared zone of a group
	464. ActFrmCreShaGrp		Form to crear a folder or file in shared zone of a group
	465. ActCreFolShaGrp		Create a new folder in shared zone of a group
	466. ActCreLnkShaGrp		Create a new link in shared zone of a group
	467. ActRenFolShaGrp		Rename a folder in shared zone of a group
	468. ActRcvFilShaGrpDZ		Receive a file in shared zone of a group using Dropzone.js
	469. ActRcvFilShaGrpCla		Receive a file in shared zone of a group using the classic way
	470. ActExpShaGrp		Expand a folder in shared zone of a group
	471. ActConShaGrp		Contract a folder in shared zone of a group
	472. ActZIPShaGrp		Compress a folder in shared zone of a group
	473. ActReqDatShaGrp		Ask for metadata of a file of the shared zone of a group
	474. ActChgDatShaGrp		Change metadata of a file of the shared zone of a group
	475. ActDowShaGrp		Download a file of the shared zone of a group

	476. ActEdiBib			Edit the bibliography
	477. ActEdiFAQ			Edit the FAQ
	478. ActEdiCrsLnk		Edit the links relacionados with the course
	479. ActChgFrcReaCrsInf		Change force students to read course info
	480. ActChgFrcReaTchGui		Change force students to read teaching guide
	481. ActChgFrcReaSylLec		Change force students to read lectures syllabus
	482. ActChgFrcReaSylPra		Change force students to read practicals syllabus
	483. ActChgFrcReaBib		Change force students to read bibliography
	484. ActChgFrcReaFAQ		Change force students to read FAQ
	485. ActChgFrcReaCrsLnk		Change force students to read links
	486. ActChgHavReaCrsInf		Change if I have read course info
	487. ActChgHavReaTchGui		Change if I have read teaching guide
	488. ActChgHavReaSylLec		Change if I have read lectures syllabus
	489. ActChgHavReaSylPra		Change if I have read practicals syllabus
	490. ActChgHavReaBib		Change if I have read bibliography
	491. ActChgHavReaFAQ		Change if I have read FAQ
	492. ActChgHavReaCrsLnk		Change if I have read links
	493. ActSelInfSrcCrsInf		Select the type of info shown in the general information about the course
	494. ActSelInfSrcTchGui		Select the type of info shown in the teaching guide
	495. ActSelInfSrcSylLec		Select the type of info shown in the lectures syllabus
	496. ActSelInfSrcSylPra		Select the type of info shown in the practicals syllabus
	497. ActSelInfSrcBib		Select the type of info shown in the bibliography
	498. ActSelInfSrcFAQ		Select the type of info shown in the FAQ
	499. ActSelInfSrcCrsLnk		Select the type of info shown in the links
	500. ActRcvURLCrsInf		Receive a link a the general information about the course
	501. ActRcvURLTchGui		Receive a link a the teaching guide
	502. ActRcvURLSylLec		Receive a link al syllabus of lectures
	503. ActRcvURLSylPra		Receive a link al syllabus of practicals
	504. ActRcvURLBib		Receive a link a bibliography
	505. ActRcvURLFAQ		Receive a link a FAQ
	506. ActRcvURLCrsLnk		Receive a link a links
	507. ActRcvPagCrsInf		Receive a page with the general information about the course
	508. ActRcvPagTchGui		Receive a page with the teaching guide
	509. ActRcvPagSylLec		Receive a page with the syllabus of lectures
	510. ActRcvPagSylPra		Receive a page with the syllabus of practicals
	511. ActRcvPagBib		Receive a page with bibliography
	512. ActRcvPagFAQ		Receive a page with FAQ
	513. ActRcvPagCrsLnk		Receive a page with links
	514. ActEditorCrsInf		Integrated editor of the general information about the course
	515. ActEditorTchGui		Integrated editor of the teaching guide
	516. ActEditorSylLec		Integrated editor of the syllabus of lectures
	517. ActEditorSylPra		Integrated editor of the syllabus of practicals
	518. ActEditorBib		Integrated editor of bibliography
	519. ActEditorFAQ		Integrated editor of FAQ
	520. ActEditorCrsLnk		Integrated editor of links
	521. ActPlaTxtEdiCrsInf		Editor of plain text of the general information about the course
	522. ActPlaTxtEdiTchGui		Editor of plain text of the teaching guide
	523. ActPlaTxtEdiSylLec		Editor of plain text of the syllabus of lectures
	524. ActPlaTxtEdiSylPra		Editor of plain text of the syllabus of practicals
	525. ActPlaTxtEdiBib		Editor of plain text of the bibliography
	526. ActPlaTxtEdiFAQ		Editor of plain text of the FAQ
	527. ActPlaTxtEdiCrsLnk		Editor of plain text of the links
	528. ActRchTxtEdiCrsInf		Editor of plain text of the general information about the course
	529. ActRchTxtEdiTchGui		Editor of plain text of the teaching guide
	530. ActRchTxtEdiSylLec		Editor of plain text of the syllabus of lectures
	531. ActRchTxtEdiSylPra		Editor of plain text of the syllabus of practicals
	532. ActRchTxtEdiBib		Editor of plain text of the bibliography
	533. ActRchTxtEdiFAQ		Editor of plain text of the FAQ
	534. ActRchTxtEdiCrsLnk		Editor of plain text of the links
	535. ActRcvPlaTxtCrsInf		Receive and change the plain text of the general information about the course
	536. ActRcvPlaTxtTchGui		Receive and change the plain text of the teaching guide
	537. ActRcvPlaTxtSylLec		Receive and change the plain text of the syllabus of lectures
	538. ActRcvPlaTxtSylPra		Receive and change the plain text of the syllabus of practicals
	539. ActRcvPlaTxtBib		Receive and change the plain text of the bibliography
	540. ActRcvPlaTxtFAQ		Receive and change the plain text of the FAQ
	541. ActRcvPlaTxtCrsLnk		Receive and change the plain text of the links
	542. ActRcvPlaTxtCrsInf		Receive and change the rich text of the general information about the course
	543. ActRcvPlaTxtTchGui		Receive and change the rich text of the teaching guide
	544. ActRcvPlaTxtSylLec		Receive and change the rich text of the syllabus of lectures
	545. ActRcvPlaTxtSylPra		Receive and change the rich text of the syllabus of practicals
	546. ActRcvPlaTxtBib		Receive and change the rich text of the bibliography
	547. ActRcvPlaTxtFAQ		Receive and change the rich text of the FAQ
	548. ActRcvPlaTxtCrsLnk		Receive and change the rich text of the links
Assessment:
	549. ActSeeAss			Show the assessment system
	550. ActSeeAsg			Show assignments
        551. ActAdmAsgWrkUsr		One user sends works of the course
        552. ActReqAsgWrkCrs		A teacher requests edition of works sent to the course
	553. ActReqTst			Request a test of self-assesment
	554. ActSeeCal			Show the academic calendar of the course with exam announcements
	555. ActSeeAllExaAnn		Show the exam announcements
	556. ActSeeAdmMrk		Marks zone (see or admin)

	557. ActEdiAss			Edit the assessment system
	558. ActChgFrcReaAss		Change force students to read assessment system
	559. ActChgHavReaAss		Change if I have read assessment system
	560. ActSelInfSrcAss		Selec. type of assessment
	561. ActRcvURLAss		Receive a link a assessment
	562. ActRcvPagAss		Receive a page with assessment
	563. ActEditorAss		Integrated editor of assessment
	564. ActPlaTxtEdiAss		Editor of plain text of assessment
	565. ActRchTxtEdiAss		Editor of rich text of assessment
	566. ActRcvPlaTxtAss		Receive and change the plain text of the assessment system
	567. ActRcvRchTxtAss		Receive and change the rich text of the assessment system
	568. ActFrmNewAsg		Form to create a new assignment
	569. ActEdiOneAsg		Edit one assignment
	570. ActNewAsg			Create new assignment
	571. ActChgAsg			Modify data of an existing assignment
	572. ActReqRemAsg		Request the removal of an assignment
	573. ActRemAsg			Remove assignment
	574. ActHidAsg			Hide assignment
	575. ActShoAsg			Show assignment
        576. ActAdmAsgWrkCrs		Edit all the works sent to the course
	577. ActReqRemFilAsgUsr		Request removal of a file of assignments from a user
	578. ActRemFilAsgUsr		Remove a file of assignments from a user
	579. ActRemFolAsgUsr		Remove an empty folder of assignments of a user
	580. ActCopAsgUsr		Set origin of copy in assignments of a user
	581. ActPasAsgUsr		Paste a file or folder of assignments of a user
	582. ActRemTreAsgUsr		Remove a not empty folder of assignments of a user
	583. ActFrmCreAsgUsr		Form to create a new folder or file of assignments of a user
	584. ActCreFolAsgUsr		Create a new folder of assignments of a user
	585. ActCreLnkAsgUsr		Create a new link of assignments of a user
	586. ActRenFolAsgUsr		Rename a folder of assignments of a user
	587. ActRcvFilAsgUsrDZ		Receive a new file of assignments of a user using Dropzone.js
	588. ActRcvFilAsgUsrCla		Receive a new file of assignments of a user using the classic way
	589. ActExpAsgUsr		Expand a folder of assignments of a user
	590. ActConAsgUsr		Contract a folder of assignments of a user
	591. ActZIPAsgUsr		Compress a folder of assignments of a user
	592. ActReqDatAsgUsr		Ask for metadata of a file of assignments of a user
	593. ActChgDatAsgUsr		Change metadata of a file of assignments of a user
	594. ActDowAsgUsr		Download a file of assignments of a user
	595. ActReqRemFilWrkUsr		Request removal of a file of works from a user
	596. ActRemFilWrkUsr		Remove a file of works from a user
	597. ActRemFolWrkUsr		Remove an empty folder of works of a user
	598. ActCopWrkUsr		Set origin of copy in works of a user
	599. ActPasWrkUsr		Paste a file or folder of works of a user
	600. ActRemTreWrkUsr		Remove a not empty folder of works of a user
	601. ActFrmCreWrkUsr		Form to create a new folder or file of works of a user
	602. ActCreFolWrkUsr		Create a new folder of works of a user
	603. ActCreLnkWrkUsr		Create a new link of works of a user
	604. ActRenFolWrkUsr		Rename a folder of works of a user
	605. ActRcvFilWrkUsrDZ		Receive a new file of works of a user using Dropzone.js
	606. ActRcvFilWrkUsrCla		Receive a new file of works of a user using the classic way
	607. ActExpWrkUsr		Expand a folder of works of a user
	608. ActConWrkUsr		Contract a folder of works of a user
	609. ActZIPWrkUsr		Compress a folder of works of a user
	610. ActReqDatWrkUsr		Ask for metadata of a file of works of a user
	611. ActChgDatWrkUsr		Change metadata of a file of works of a user
	612. ActDowWrkUsr		Download a file of works of a user
	613. ActReqRemFilAsgCrs		Request removal of a file of assignments in the course
	614. ActRemFilAsgCrs		Remove a file of assignments in the course
	615. ActRemFolAsgCrs		Remove an empty folder of assignments in the course
	616. ActCopAsgCrs		Set origin of copy in assignments in the course
	617. ActPasAsgCrs		Paste a file or folder of assignments in the course
	618. ActRemTreAsgCrs		Remove a not empty folder of assignments in the course
	619. ActFrmCreAsgCrs		Form to create a new folder or file of assignments in the course
	620. ActCreFolAsgCrs		Create a new folder of assignments in the course
	621. ActCreLnkAsgCrs		Create a new link of assignments in the course
	622. ActRenFolAsgCrs		Rename a folder of assignments in the course
	623. ActRcvFilAsgCrsDZ		Receive a file of assignments in the course using Dropzone.js
	624. ActRcvFilAsgCrsCla		Receive a file of assignments in the course using the classic way
	625. ActExpAsgCrs		Expand a folder of assignments in a course
	626. ActConAsgCrs		Contract a folder of assignments in a course
	627. ActZIPAsgCrs		Compress a folder of assignments in a course
	628. ActReqDatAsgCrs		Ask for metadata of a file of assignments in a course
	629. ActChgDatAsgCrs		Change metadata of a file of assignments in a course
	630. ActDowAsgCrs		Download a file of assignments in a course
	631. ActReqRemFilWrkCrs		Request removal of a file of works in the course
	632. ActRemFilWrkCrs		Remove a file of works in the course
	633. ActRemFolWrkCrs		Remove an empty folder of works in the course
	634. ActCopWrkCrs		Set origin of copy in works in the course
	635. ActPasWrkCrs		Paste a file or folder of works in the course
	636. ActRemTreWrkCrs		Remove a not empty folder of works in the course
	637. ActFrmCreWrkCrs		Form to create a new folder or file of works in the course
	638. ActCreFolWrkCrs		Create a new folder of works in the course
	639. ActCreLnkWrkCrs		Create a new link of works in the course
	640. ActRenFolWrkCrs		Rename a folder of works in the course
	641. ActRcvFilWrkCrsDZ		Receive a file of works in the course using Dropzone.js
	642. ActRcvFilWrkCrsCla		Receive a file of works in the course using the classic way
	643. ActExpWrkCrs		Expand a folder of works in a course
	644. ActConWrkCrs		Contract a folder of works in a course
	645. ActZIPWrkCrs		Compress a folder of works in a course
	646. ActReqDatWrkCrs		Ask for metadata of a file of works in a course
	647. ActChgDatWrkCrs		Change metadata of a file of works in a course
	648. ActDowWrkCrs		Download a file of works in a course
	649. ActSeeTst			Show the seft-assessment test
	650. ActAssTst			Assess a self-assessment test
	651. ActEdiTstQst		Request the edition of self-assessment questions
	652. ActEdiOneTstQst		Edit one self-assesment test question
	653. ActReqImpTstQst		Put form to ask for an XML with test questions to import
	654. ActImpTstQst		Import test questions from XML file
	655. ActLstTstQst		List for edition several self-assessment test questions
	656. ActRcvTstQst		Receive a question of self-assessment
	657. ActShfTstQst		Change shuffle of of a question of self-assessment
	658. ActReqRemTstQst		Request removing a self-assesment test question
	659. ActRemTstQst		Remove a self-assesment test question
	660. ActCfgTst			Request renaming of tags of questions of self-assesment
	661. ActEnableTag		Enable a tag
	662. ActDisableTag		Disable a tag
	663. ActRenTag			Rename a tag
	664. ActRcvCfgTst		Receive configuration of test
	665. ActReqSeeMyTstExa		Select range of dates to see my results of test exams
	666. ActSeeMyTstExa		Show my test results
	667. ActReqSeeUsrTstExa		Select users and range of dates to see results of test exams
	668. ActSeeUsrTstExa		Show test results of several users
	669. ActSeeOneTstExaMe		Show one test exam of me as student
	670. ActSeeOneTstExaOth		Show one test exam of other user

	671. ActPrnCal			Show print view of the academic calendar of the course
	672. ActChgCal1stDay		Change first day of week and show academic calendar of the course

	673. ActSeeOneExaAnn		Show one exam announcement
	674. ActSeeDatExaAnn		Show exam announcements of a given date
	675. ActEdiExaAnn		Edit an exam announcement
	676. ActRcvExaAnn		Receive an exam announcement
	677. ActPrnExaAnn		Show an exam announcement ready to be printed
	678. ActRemExaAnn		Remove an exam announcement

	679. ActChgToSeeMrk		Change to see marks in course/group

	680. ActSeeMrkCrs		See marks in course
	681. ActExpSeeMrkCrs		Expand a folder in marks in course
	682. ActConSeeMrkCrs		Contract a folder in marks in course
	683. ActReqDatSeeMrkCrs		Ask for metadata of a file of the marks zone of the course
	684. ActSeeMyMrkCrs		Individualized access to marks in course

	685. ActSeeMrkGrp		See marks in group
	686. ActExpSeeMrkGrp		Expand a folder in marks in group
	687. ActConSeeMrkGrp		Contract a folder in marks in group
	688. ActReqDatSeeMrkGrp		Ask for metadata of a file of the marks zone of a group
	689. ActSeeMyMrkGrp		Individualized access to marks in group

	690. ActChgToAdmMrk		Change to admin marks in course/group

	691. ActAdmMrkCrs		Admin the files of the zone of marks in course
	692. ActReqRemFilMrkCrs		Request removal of a file of marks in course
	693. ActRemFilMrkCrs		Remove a file of marks in course
	694. ActRemFolMrkCrs		Remove a folder empty of marks in course
	695. ActCopMrkCrs		Set source of copy in marks in course
	696. ActPasMrkCrs		Paste a folder or file in marks in course
	697. ActRemTreMrkCrs		Remove a folder no empty of marks in course
	698. ActFrmCreMrkCrs		Form to crear a folder or file in marks in course
	699. ActCreFolMrkCrs		Create a new folder in marks in course
	700. ActRenFolMrkCrs		Rename a folder in marks in course
	701. ActRcvFilMrkCrsDZ		Receive a file of marks in course using Dropzone.js
	702. ActRcvFilMrkCrsCla		Receive a file of marks in course using the classic way
	703. ActExpAdmMrkCrs		Expand a folder in marks administration in course
	704. ActConAdmMrkCrs		Contract a folder in marks administration in course
	705. ActZIPAdmMrkCrs		Compress a folder in marks administration in course
	706. ActShoMrkCrs		Show hidden folder or file of the marks administration in course
	707. ActHidMrkCrs		Hide folder or file of the marks administration in course
	708. ActReqDatAdmMrkCrs		Ask for metadata of a file in marks administration in course
	709. ActChgDatAdmMrkCrs		Change metadata of a file in marks administration in course
	710. ActDowAdmMrkCrs		Download a file in marks administration in course
	711. ActChgNumRowHeaCrs		Change the number of rows of cabecera of a file of marks in course
	712. ActChgNumRowFooCrs		Change the number of rows of pie of a file of marks in course

	713. ActAdmMrkGrp		Admin the files of the zone of marks in group
	714. ActReqRemFilMrkGrp		Request removal of a file of marks in group
	715. ActRemFilMrkGrp		Remove a file of marks in group
	716. ActRemFolMrkGrp		Remove a folder empty of marks in group
	717. ActCopMrkGrp		Set source of copy in marks in group
	718. ActPasMrkGrp		Paste a folder or file in marks in group
	719. ActRemTreMrkGrp		Remove a folder no empty of marks in group
	720. ActFrmCreMrkGrp		Form to crear a folder or file in marks in group
	721. ActCreFolMrkGrp		Create a new folder in marks in group
	722. ActRenFolMrkGrp		Rename a folder in marks in group
	723. ActRcvFilMrkGrpDZ		Receive a file to marks in group using Dropzone.js
	724. ActRcvFilMrkGrpCla		Receive a file to marks in group using the classic way
	725. ActExpAdmMrkGrp		Expand a folder in marks administration in group
	726. ActConAdmMrkGrp		Contract a folder in marks administration in group
	727. ActZIPAdmMrkGrp		Compress a folder in marks administration in group
	728. ActShoMrkGrp		Show hidden folder or file of the marks administration in group
	729. ActHidMrkGrp		Hide folder or file of the marks administration in group
	730. ActReqDatAdmMrkGrp		Ask for metadata of a file in marks administration in group
	731. ActChgDatAdmMrkGrp		Change metadata of a file in marks administration in group
	732. ActDowAdmMrkGrp		Download a file in marks administration in group
	733. ActChgNumRowHeaGrp		Change the number of rows of cabecera of a file of marks in group
	734. ActChgNumRowFooGrp		Change the number of rows of pie of a file of marks in group
Users:
	735. ActReqSelGrp		Request the selection of groups of students
	736. ActLstStd			Show class photo or list of students
	737. ActLstTch			Show class photo or list of teachers
	738. ActLstOth			Show class photo or list of guests
	739. ActSeeAtt			Show attendance events
	740. ActReqSignUp		Apply for my enrollment
	741. ActSeeSignUpReq		Show pending requests for inscription in the current course
	742. ActLstCon			List connected users

	743. ActChgGrp			Change my groups
	744. ActReqEdiGrp		Request the edition of groups
	745. ActNewGrpTyp		Request the creation of a type of group of students
	746. ActReqRemGrpTyp		Request the removal of a type of group of students
	747. ActRemGrpTyp		Remove a type of group of students
	748. ActRenGrpTyp		Request renaming of a type of group of students
	749. ActChgMdtGrpTyp		Request change if it is mandatory to register in groups of a type
	750. ActChgMulGrpTyp		Request change if it is possible to register in multiple groups of a type
	751. ActChgTimGrpTyp		Request change when the groups of a type will be open
	752. ActNewGrp			Request the creation of a group of students
	753. ActReqRemGrp		Request the removal of a group of students
	754. ActRemGrp			Remove a group of students
	755. ActOpeGrp			Abrir a group of students
	756. ActCloGrp			Cerrar a group of students
	757. ActEnaFilZonGrp		Enable zonas of files of a group
	758. ActDisFilZonGrp		Disable zonas of files of a group
	759. ActChgGrpTyp		Request change in the type of group of students
	760. ActRenGrp			Request renaming of a group of students
	761. ActChgMaxStdGrp		Request change in the number máximo of students of a group

	763. ActLstGst			List main data of administrators

	764. ActPrnGstPho		Show the class photo of guests ready to be printed
	765. ActPrnStdPho		Show the class photo of students ready to be printed
	766. ActPrnTchPho		Show the class photo of teachers ready to be printed
	767. ActLstGstAll		List in another window the full data of guests
	768. ActLstStdAll		List in another window the full data of students
	769. ActLstTchAll		List in another window the full data of teachers

	770. ActSeeRecOneStd		Show record of one selected student
	771. ActSeeRecOneTch		Show record of one selected teacher
	772. ActSeeRecSevGst		Show records of several selected guests
	773. ActSeeRecSevStd		Show records of several selected students
	774. ActSeeRecSevTch		Show records of several selected teachers
	775. ActPrnRecSevGst		Show records of several selected guests ready to be printed
	776. ActPrnRecSevStd		Show records of several selected students ready to be printed
	777. ActPrnRecSevTch		Show records of several selected teachers ready to be printed
	778. ActRcvRecOthUsr		Update record fields of a student in this course
	779. ActEdiRecFie		Request the edition of record fields of students
	780. ActNewFie			Request the creation of a record field
	781. ActReqRemFie		Request the removal of record field
	782. ActRemFie			Remove a record field
	783. ActRenFie			Request renaming of record field
	784. ActChgRowFie		Request change in number of lines of form of a record field
	785. ActChgVisFie		Request change in visibility of a record field
        786. ActRcvRecCrs		Receive and update fields of my record in this course

	787. ActReqEnrSevStd		Request the enrollment/removing of several users to / from current course
	788. ActReqEnrSevTch		Request the enrollment/removing of several users to / from current course

	789. ActReqLstStdAtt		Request listing of attendance of several students to several events
	790. ActSeeLstMyAtt		List my attendance as student to several events
	791. ActPrnLstMyAtt		Print my attendance as student to several events
	792. ActSeeLstStdAtt		List attendance of several students to several events
	793. ActPrnLstStdAtt		Print attendance of several students to several events
	794. ActFrmNewAtt		Form to create a new attendance event
	795. ActEdiOneAtt		Edit one attendance event
	796. ActNewAtt			Create new attendance event
	797. ActChgAtt			Modify data of an existing attendance event
	798. ActReqRemAtt		Request the removal of an attendance event
	799. ActRemAtt			Remove attendance event
	800. ActHidAtt			Hide attendance event
	801. ActShoAtt			Show attendance event
	802. ActSeeOneAtt		List students who attended to an event
	803. ActRecAttStd		Save students who attended to an event and comments
	804. ActRecAttMe		Save my comments as student in an attendance event

	805. ActSignUp			Apply for my enrollment
	806. ActUpdSignUpReq		Update pending requests for inscription in the current course
	807. ActReqRejSignUp		Ask if reject the enrollment of a user in a course
	808. ActRejSignUp		Reject the enrollment of a user in a course

	809. ActReqMdfOneOth		Request a user's ID for enrollment/removing
	810. ActReqMdfOneStd		Request a user's ID for enrollment/removing
	811. ActReqMdfOneTch		Request a user's ID for enrollment/removing
	812. ActReqMdfOth		Request enrollment/removing of a user
	813. ActReqMdfStd		Request enrollment/removing of a user
	814. ActReqMdfTch		Request enrollment/removing of a user
	815. ActReqOthPho		Show form to send the photo of another user
	816. ActReqStdPho		Show form to send the photo of another user
	817. ActReqTchPho		Show form to send the photo of another user
	818. ActDetOthPho		Receive other user's photo and detect faces on it
	819. ActDetStdPho		Receive other user's photo and detect faces on it
	820. ActDetTchPho		Receive other user's photo and detect faces on it
	821. ActUpdOthPho		Update other user's photo
	822. ActUpdStdPho		Update other user's photo
	823. ActUpdTchPho		Update other user's photo
	824. ActRemOthPho		Request the removal of other user's photo
	825. ActRemStdPho		Request the removal of student's photo
	826. ActRemTchPho		Request the removal of teacher's photo
	827. ActRemOthPho		Remove other user's photo
	828. ActRemStdPho		Remove student's photo
	829. ActRemTchPho		Remove teacher's photo
	830. ActCreOth			Create a new user
	831. ActCreStd			Create a new user
	832. ActCreTch			Create a new user
	833. ActUpdOth			Update another user's data and groups
	834. ActUpdStd			Update another user's data and groups
	835. ActUpdTch			Update another user's data and groups
	836. ActReqAccEnrStd		Confirm acceptation / refusion of enrollment as student in current course
	837. ActReqAccEnrTch		Confirm acceptation / refusion of enrollment as teacher in current course
	838. ActAccEnrStd		Accept enrollment as student in current course
	839. ActAccEnrTch		Accept enrollment as teacher in current course
	840. ActRemMe_Std		Reject enrollment as student in current course
	841. ActRemMe_Tch		Reject enrollment as teacher in current course

	842. ActNewAdmIns		Register an administrador in this institution
	843. ActRemAdmIns		Remove an administrador of this institution
	844. ActNewAdmCtr		Register an administrador in this centre
	845. ActRemAdmCtr		Remove an administrador of this centre
	846. ActNewAdmDeg		Register an administrador in this degree
	847. ActRemAdmDeg		Remove an administrador of this degree

	848. ActRcvFrmEnrSevStd		Receive a form with IDs of users to be registeres/removed to/from current course
	849. ActRcvFrmEnrSevTch		Receive a form with IDs of users to be registeres/removed to/from current course

	850. ActReqCnfID_Oth		Request the confirmation of another user's ID
	851. ActReqCnfID_Std		Request the confirmation of another user's ID
	852. ActReqCnfID_Tch		Request the confirmation of another user's ID
	853. ActCnfID_Oth		Confirm another user's ID
	854. ActCnfID_Std		Confirm another user's ID
	855. ActCnfID_Tch		Confirm another user's ID

	856. ActFrmIDsOth		Show form to the change of the IDs of another user
	857. ActFrmIDsStd		Show form to the change of the IDs of another user
	858. ActFrmIDsTch		Show form to the change of the IDs of another user
	859. ActRemID_Oth		Remove one of the IDs of another user
	860. ActRemID_Std		Remove one of the IDs of another user
	861. ActRemID_Tch		Remove one of the IDs of another user
	862. ActNewID_Oth		Create a new user's ID for another user
	863. ActNewID_Std		Create a new user's ID for another user
	864. ActNewID_Tch		Create a new user's ID for another user

	865. ActFrmPwdOth		Show form to change the password of another user
	866. ActFrmPwdStd		Show form to change the password of another user
	867. ActFrmPwdTch		Show form to change the password of another user
	868. ActChgPwdOth		Change the password of another user
	869. ActChgPwdStd		Change the password of another user
	870. ActChgPwdTch		Change the password of another user

	871. ActFrmMaiOth		Show form to the change of the e-mail of another user
	872. ActFrmMaiStd		Show form to the change of the e-mail of another user
	873. ActFrmMaiTch		Show form to the change of the e-mail of another user
	874. ActRemMaiOth		Remove one of the e-mail of another user
	875. ActRemMaiStd		Remove one of the e-mail of another user
	876. ActRemMaiTch		Remove one of the e-mail of another user
	877. ActNewMaiOth		Create a new user's e-mail for another user
	878. ActNewMaiStd		Create a new user's e-mail for another user
	879. ActNewMaiTch		Create a new user's e-mail for another user

	880. ActRemStdCrs		Remove a student from the current course
	881. ActRemTchCrs		Remove a teacher from the current course
	882. ActRemUsrGbl		Eliminate completely a user from the platform
	883. ActReqRemAllStdCrs		Request the removal of all the students from the current course
	884. ActRemAllStdCrs		Remove all the students from the current course
	885. ActReqRemOldUsr		Request the complete elimination of old users
	886. ActRemOldUsr		Eliminate completely old users

	887. ActLstDupUsr		List possible duplicate users
	888. ActLstSimUsr		List users similar to a given one (possible duplicates)
	889. ActRemDupUsr		Remove user from list of possible duplicate users

        890. ActLstClk			List last clicks in real time
Social:
        891. ActSeeSocTmlGbl		Show social timeline (global)
        892. ActSeeSocPrf		Suggest list of users to follow
	893. ActSeeFor			Show the level superior of the forums
	894. ActSeeChtRms		Show the chat rooms

	895. ActRcvSocPstGbl		Receive a public social post to be displayed in the timeline (global)
	896. ActRcvSocComGbl		Comment a social note in the timeline (global)
	897. ActShaSocNotGbl		Share a social note in the timeline (global)
	898. ActUnsSocNotGbl		Unshare a previously shared social note in the timeline (global)
	899. ActFavSocNotGbl		Favourite a social note in the timeline (global)
	900. ActUnfSocNotGbl		Unfavourite a previously favourited social note in the timeline (global)
	901. ActFavSocComGbl		Favourite a social comment in the timeline (global)
	902. ActUnfSocComGbl		Unfavourite a previously favourited social comment in the timeline (global)
	903. ActReqRemSocPubGbl		Request the removal of a social publishing in the timeline (global)
	904. ActRemSocPubGbl		Remove a social publishing in the timeline (global)
	905. ActReqRemSocComGbl		Request the removal of a comment in a social note (global)
	906. ActRemSocComGbl		Remove of a comment in a social note (global)

	907. ActReqPubPrf		Request @nickname to show a public user's profile

	908. ActRcvSocPstUsr		Receive a public social post to be displayed in the timeline (user)
	909. ActRcvSocComUsr		Comment a social note in the timeline (user)
	910. ActShaSocNotUsr		Share a social note in the timeline (user)
	911. ActUnsSocNotUsr		Unshare a previously shared social note in the timeline (user)
	912. ActFavSocNotUsr		Favourite a social note in the timeline (user)
	913. ActUnfSocNotUsr		Unfavourite a previously favourited social note in the timeline (user)
	914. ActFavSocComUsr		Favourite a social comment in the timeline (user)
	915. ActUnfSocComUsr		Unfavourite a previously favourited social comment in the timeline (user)
	916. ActReqRemSocPubUsr		Request the removal of a social publishing in the timeline (user)
	917. ActRemSocPubUsr		Remove a social publishing in the timeline (user)
	918. ActReqRemSocComUsr		Request the removal of a comment in a social note (user)
	919. ActRemSocComUsr		Remove of a comment in a social note (user)

	920. ActCal1stClkTim		Calculate first click time from log and store into user's figures
	921. ActCalNumClk		Calculate number of clicks from log and store into user's figures
	922. ActCalNumFileViews		Calculate number of file views and store into user's figures
	923. ActCalNumForPst		Calculate number of forum posts and store into user's figures
	924. ActCalNumMsgSnt		Calculate number of messages sent from log and store into user's figures

	925. ActFolUsr			Follow another user
	926. ActUnfUsr			Unfollow another user
	927. ActSeeFlg			Show following
	928. ActSeeFlr			Show followers

	929. ActSeeForCrsUsr		Show top level of forum of users of the course
	930. ActSeeForCrsTch		Show top level of forum of teachers of the course
	931. ActSeeForDegUsr		Show top level of forum of users of the degree
	932. ActSeeForDegTch		Show top level of forum of teachers of the degree
	933. ActSeeForCtrUsr		Show top level of forum of users of the centre
	934. ActSeeForCtrTch		Show top level of forum of teachers of the centre
	935. ActSeeForInsUsr		Show top level of forum of users of the institution
	936. ActSeeForInsTch		Show top level of forum of teachers of the institution
	937. ActSeeForGenUsr		Show top level of forum of users general
	938. ActSeeForGenTch		Show top level of forum of teachers general
	939. ActSeeForSWAUsr		Show top level of forum of users of the platform
	940. ActSeeForSWATch		Show top level of forum of teachers of the platform
	941. ActSeePstForCrsUsr		Show the messages of a thread of the forum of users of the course
	942. ActSeePstForCrsTch		Show the messages of a thread of the forum of teachers of the course
	943. ActSeePstForDegUsr		Show the messages of a thread of the forum of users of the degree
	944. ActSeePstForDegTch		Show the messages of a thread of the forum of teachers of the degree
	945. ActSeePstForCtrUsr		Show the messages of a thread of the forum of users of the centre
	946. ActSeePstForCtrTch		Show the messages of a thread of the forum of teachers of the centre
	947. ActSeePstForInsUsr		Show the messages of a thread of the forum of users of the institution
	948. ActSeePstForInsTch		Show the messages of a thread of the forum of teachers of the institution
	949. ActSeePstForGenUsr		Show the messages of a thread of the forum of users general
	950. ActSeePstForGenTch		Show the messages of a thread of the forum of teachers general
	951. ActSeePstForSWAUsr		Show the messages of a thread of the forum of users of the platform
	952. ActSeePstForSWATch		Show the messages of a thread of the forum of teachers of the platform
	953. ActRcvThrForCrsUsr		Receive the first message of a new thread of forum of users of the course
	954. ActRcvThrForCrsTch		Receive the first message of a new thread of forum of teachers of the course
	955. ActRcvThrForDegUsr		Receive the first message of a new thread of forum of users of the degree
	956. ActRcvThrForDegTch		Receive the first message of a new thread of forum of teachers of the degree
	957. ActRcvThrForCtrUsr		Receive the first message of a new thread of forum of users of centre
	958. ActRcvThrForCtrTch		Receive the first message of a new thread of forum of teachers of centre
	959. ActRcvThrForInsUsr		Receive the first message of a new thread of forum of users of the institution
	960. ActRcvThrForInsTch		Receive the first message of a new thread of forum of teachers of the institution
	961. ActRcvThrForGenUsr		Receive the first message of a new thread of forum of users general
	962. ActRcvThrForGenTch		Receive the first message of a new thread of forum of teachers general
	963. ActRcvThrForSWAUsr		Receive the first message of a new thread of forum of users of the platform
	964. ActRcvThrForSWATch		Receive the first message of a new thread of forum of teachers of the platform
	965. ActRcvRepForCrsUsr		Receive a message of answer in a thread existente in the forum of users of the course
	966. ActRcvRepForCrsTch		Receive a message of answer in a thread existente in the forum of teachers of the course
	967. ActRcvRepForDegUsr		Receive a message of answer in a thread existente in the forum of users of the degree
	968. ActRcvRepForDegTch		Receive a message of answer in a thread existente in the forum of teachers of the degree
	969. ActRcvRepForCtrUsr		Receive a message of answer in a thread existente in the forum of users of centre
	970. ActRcvRepForCtrTch		Receive a message of answer in a thread existente in the forum of teachers of centre
	971. ActRcvRepForInsUsr		Receive a message of answer in a thread existente in the forum of users of the institution
	972. ActRcvRepForInsTch		Receive a message of answer in a thread existente in the forum of teachers of the institution
	973. ActRcvRepForGenUsr		Receive a message of answer in a thread existente in the forum of users general
	974. ActRcvRepForGenTch		Receive a message of answer in a thread existente in the forum of teachers general
	975. ActRcvRepForSWAUsr		Receive a message of answer in a thread existente in the forum of users of the platform
	976. ActRcvRepForSWATch		Receive a message of answer in a thread existente in the forum of teachers of the platform
	977. ActReqDelThrCrsUsr		Request the removal of a thread of forum of users of the course
	978. ActReqDelThrCrsTch		Request the removal of a thread of forum of teachers of the course
	979. ActReqDelThrDegUsr		Request the removal of a thread of forum of users of the degree
	980. ActReqDelThrDegTch		Request the removal of a thread of forum of teachers of the degree
	981. ActReqDelThrCtrUsr		Request the removal of a thread of forum of users of centre
	982. ActReqDelThrCtrTch		Request the removal of a thread of forum of teachers of centre
	983. ActReqDelThrInsUsr		Request the removal of a thread of forum of users of the institution
	984. ActReqDelThrInsTch		Request the removal of a thread of forum of teachers of the institution
	985. ActReqDelThrGenUsr		Request the removal of a thread of forum of users general
	986. ActReqDelThrGenTch		Request the removal of a thread of forum of teachers general
	987. ActReqDelThrSWAUsr		Request the removal of a thread of forum of users of the platform
	988. ActReqDelThrSWATch		Request the removal of a thread of forum of teachers of the platform
	989. ActDelThrForCrsUsr		Remove a thread of forum of users of the course
	990. ActDelThrForCrsTch		Remove a thread of forum of teachers of the course
	991. ActDelThrForDegUsr		Remove a thread of forum of users of the degree
	992. ActDelThrForDegTch		Remove a thread of forum of teachers of the degree
	993. ActDelThrForCtrUsr		Remove a thread of forum of users of centre
	994. ActDelThrForCtrTch		Remove a thread of forum of teachers of centre
	995. ActDelThrForInsUsr		Remove a thread of forum of users of the institution
	996. ActDelThrForInsTch		Remove a thread of forum of teachers of the institution
	997. ActDelThrForGenUsr		Remove a thread of forum of users general
	998. ActDelThrForGenTch		Remove a thread of forum of teachers general
	999. ActDelThrForSWAUsr		Remove a thread of forum of users of the platform
       1000. ActDelThrForSWATch		Remove a thread of forum of teachers of the platform
       1001. ActCutThrForCrsUsr		Cut a thread of forum of users of the course
       1002. ActCutThrForCrsTch		Cut a thread of forum of teachers of the course
       1003. ActCutThrForDegUsr		Cut a thread of forum of users of the degree
       1004. ActCutThrForDegTch		Cut a thread of forum of teachers of the degree
       1005. ActCutThrForCtrUsr		Cut a thread of forum of users of centre
       1006. ActCutThrForCtrTch		Cut a thread of forum of teachers of centre
       1007. ActCutThrForInsUsr		Cut a thread of forum of users of the institution
       1008. ActCutThrForInsTch		Cut a thread of forum of teachers of the institution
       1009. ActCutThrForGenUsr		Cut a thread of forum of users general
       1010. ActCutThrForGenTch		Cut a thread of forum of teachers general
       1001. ActCutThrForSWAUsr		Cut a thread of forum of users of the platform
       1002. ActCutThrForSWATch		Cut a thread of forum of teachers of the platform
       1003. ActPasThrForCrsUsr		Paste a thread of forum of users of the course
       1004. ActPasThrForCrsTch		Paste a thread of forum of teachers of the course
       1005. ActPasThrForDegUsr		Paste a thread of forum of users of the degree
       1006. ActPasThrForDegTch		Paste a thread of forum of teachers of the degree
       1007. ActPasThrForCtrUsr		Paste a thread of forum of users of centre
       1008. ActPasThrForCtrTch		Paste a thread of forum of teachers of centre
       1009. ActPasThrForInsUsr		Paste a thread of forum of users of the institution
       1020. ActPasThrForInsTch		Paste a thread of forum of teachers of the institution
       1021. ActPasThrForGenUsr		Paste a thread of forum of users general
       1022. ActPasThrForGenTch		Paste a thread of forum of teachers general
       1023. ActPasThrForSWAUsr		Paste a thread of forum of users of the platform
       1024. ActPasThrForSWATch		Paste a thread of forum of teachers of the platform
       1025. ActDelPstForCrsUsr		Remove a message of forum of users of the course
       1026. ActDelPstForCrsTch		Remove a message of forum of teachers of the course
       1027. ActDelPstForDegUsr		Remove a message of forum of users of the degree
       1028. ActDelPstForDegTch		Remove a message of forum of teachers of the degree
       1029. ActDelPstForCtrUsr		Remove a message of forum of users of centre
       1030. ActDelPstForCtrTch		Remove a message of forum of teachers of centre
       1031. ActDelPstForInsUsr		Remove a message of forum of users of the institution
       1032. ActDelPstForInsTch		Remove a message of forum of teachers of the institution
       1033. ActDelPstForGenUsr		Remove a message of forum of users general
       1034. ActDelPstForGenTch		Remove a message of forum of teachers general
       1035. ActDelPstForSWAUsr		Remove a message of forum of users of the platform
       1036. ActDelPstForSWATch		Remove a message of forum of teachers of the platform
       1037. ActEnbPstForCrsUsr		Enable a message of forum of users of the course
       1038. ActEnbPstForCrsTch		Enable a message of forum of teachers of the course
       1039. ActEnbPstForDegUsr		Enable a message of forum of users of the degree
       1040. ActEnbPstForDegTch		Enable a message of forum of teachers of the degree
       1041. ActEnbPstForCtrUsr		Enable a message of forum of users of centre
       1042. ActEnbPstForCtrTch		Enable a message of forum of teachers of centre
       1043. ActEnbPstForInsUsr		Enable a message of forum of users of the institution
       1044. ActEnbPstForInsTch		Enable a message of forum of teachers of the institution
       1045. ActEnbPstForGenUsr		Enable a message of forum of users general
       1046. ActEnbPstForGenTch		Enable a message of forum of teachers general
       1047. ActEnbPstForSWAUsr		Enable a message of forum of users of the platform
       1048. ActEnbPstForSWATch		Enable a message of forum of teachers of the platform
       1049. ActDisPstForCrsUsr		Disable a message of forum of users of the course
       1050. ActDisPstForCrsTch		Disable a message of forum of teachers of the course
       1051. ActDisPstForDegUsr		Disable a message of forum of users of the degree
       1052. ActDisPstForDegTch		Disable a message of forum of teachers of the degree
       1053. ActDisPstForCtrUsr		Disable a message of forum of users of centre
       1054. ActDisPstForCtrTch		Disable a message of forum of teachers of centre
       1055. ActDisPstForInsUsr		Disable a message of forum of users of the institution
       1056. ActDisPstForInsTch		Disable a message of forum of teachers of the institution
       1057. ActDisPstForGenUsr		Disable a message of forum of users general
       1058. ActDisPstForGenTch		Disable a message of forum of teachers general
       1059. ActDisPstForSWAUsr		Disable a message of forum of users of the platform
       1060. ActDisPstForSWATch		Disable a message of forum of teachers of the platform

       1061. ActCht			Enter in a chat room to chat
Messages:
       1062. ActSeeNtf			Show my recent notifications
       1063. ActSeeAnn			Show global announcements
       1064. ActSeeAllNot		Show all notices
       1065. ActReqMsgUsr		Write message to several users
       1066. ActSeeRcvMsg		Show the messages received from other users (link in menu)
       1067. ActSeeSntMsg		Show the messages sent to other users
       1068. ActMaiStd			Send an e-mail to students
       1069. ActWriAnn			Show form to create a new global announcement
       1070. ActRcvAnn			Receive and create a new global announcement
       1071. ActHidAnn			Hide a global announcement that was active
       1072. ActRevAnn			Reveal a global announcement that was hidden
       1073. ActRemAnn			Remove global announcement
       1074. ActSeeOneNot			Show (expand) a notice
       1075. ActWriNot			Write a new notice
       1076. ActRcvNot			Receive and create a new notice
       1077. ActHidNot			Hide a notice that was active
       1078. ActRevNot			Reveal a notice that was hidden
       1079. ActReqRemNot		Request removal of a notice
       1080. ActRemNot			Remove a notice
       1081. ActSeeNewNtf		Show my recent notifications (link in top heading)
       1082. ActMrkNtfSee		Mark all my notifications as seen
       1083. ActRcvMsgUsr		Sent/Receive a message of a user
       1084. ActReqDelAllSntMsg		Request the removal of todos the messages sent to other users
       1085. ActReqDelAllRcvMsg		Request the removal of todos the messages received from other users
       1086. ActDelAllSntMsg		Remove todos the messages sent to other users
       1087. ActDelAllRcvMsg		Remove todos the messages received from other users
       1088. ActDelSntMsg		Remove a message sent to other users
       1089. ActDelRcvMsg		Remove a message received from other user
       1090. ActExpSntMsg		See (expand) sent message
       1091. ActExpRcvMsg		See (expand) received message
       1092. ActConSntMsg		Hide (contract) sent message
       1093. ActConRcvMsg		Hide (contract) received message
       1094. ActLstBanUsr		List banned users
       1095. ActBanUsrMsg		Ban the sender of a message when showing received messages
       1096. ActUnbUsrMsg		Unban the sender of a message when showing received messages
       1097. ActUnbUsrLst		Unban a user when listing banned users
Statistics:
       1098. ActSeeAllSvy		List all surveys in pages
       1099. ActReqUseGbl		Request showing use of the platform
       1100. ActSeePhoDeg		Show a class photo with the average photos of the students of each degree
       1101. ActReqStaCrs		Request statistics of courses
       1102. ActReqAccGbl		Request query of clicks to the complete platform
        NEW. ActReqMyUsgRep		Request my usage report

       1103. ActSeeOneSvy		Show one survey
       1104. ActAnsSvy			Answer a survey
       1105. ActFrmNewSvy		Form to create a new survey
       1106. ActEdiOneSvy		Edit one survey
       1107. ActNewSvy			Create new survey
       1108. ActChgSvy			Modify data of an existing survey
       1109. ActReqRemSvy		Request the removal of a survey
       1110. ActRemSvy			Remove survey
       1111. ActReqRstSvy		Request the reset of answers of a survey
       1112. ActRstSvy			Reset answers of survey
       1113. ActHidSvy			Hide survey
       1114. ActShoSvy			Show survey
       1115. ActEdiOneSvyQst		Edit a new question for a survey
       1116. ActRcvSvyQst		Receive a question of a survey
       1117. ActReqRemSvyQst		Request the removal of a question of a survey
       1118. ActRemSvyQst		Confirm the removal of a question of a survey
       1119. ActSeeUseGbl		Show use of the platform
       1120. ActPrnPhoDeg		Show vista of impresión of the class photo with the average photos of the students of each degree.
       1121. ActCalPhoDeg		Compute the average photos of the students of each degree
       1122. ActSeeAccGbl		Query clicks to the complete platform
       1123. ActReqAccCrs		Request query of clicks in the course
       1124. ActSeeAccCrs		Query clicks to current course
       1125. ActSeeAllStaCrs		Show statistics of courses
        NEW. ActSeeMyUsgRep		Show my usage report
Profile:
       1126. ActFrmLogIn		Show landing page (forms to log in and to create a new account)
       1127. ActFrmRolSes		Show form to log out and to change current role in this session
       1128. ActMyCrs			Select one of my courses
       1129. ActSeeMyTT			Show the timetable of all courses of the logged user
        NEW. ActSeeMyAgd		Show my agenda (personal organizer)
       1130. ActFrmMyAcc		Show form to the creation or change of user's account
       1131. ActReqEdiRecCom		Request the edition of the record with the personal data of the user
       1132. ActEdiPrf			Show forms to edit preferences
       1133. ActAdmBrf			Show the briefcase of private archives
       1134. ActMFUAct			Show most frequently used actions

       1135. ActReqSndNewPwd		Show form to send a new password via e-mail
       1136. ActSndNewPwd		Send a new password via e-mail
       1137. ActLogOut			Close session

       1138. ActAutUsrInt		Authentify user internally (directly from the platform)
        NEW. ActAutUsrNew		Authentify user internally (directly from the platform, only if user has not password)
       1140. ActAutUsrChgLan		Change language to my language just after authentication
       1141. ActAnnSee			Mark announcement as seen
       1142. ActChgMyRol		Change type of logged user

        NEW. ActChkUsrAcc		Check if already exists a new account without password associated to a ID
       1143. ActCreUsrAcc		Create new user account
       1144. ActRemID_Me		Remove one of my user's IDs
       1145. ActNewIDMe			Create a new user's ID for me
       1146. ActRemOldNic		Remove one of my old nicknames
       1147. ActChgNic			Change my nickname
       1148. ActRemMaiMe		Remove one of my old e-mails
       1149. ActNewMaiMe		Change my e-mail address
       1150. ActCnfMai			Confirm e-mail address
       1151. ActFrmChgMyPwd		Show form to the change of the password
       1152. ActChgPwd			Change the password
       1153. ActReqRemMyAcc		Request the removal of my account
       1154. ActRemMyAcc		Remove my account

       1145. ActChgMyData		Update my personal data

       1156. ActReqMyPho		Show form to send my photo
       1157. ActDetMyPho		Receive my photo and detect faces on it
       1158. ActUpdMyPho		Update my photo
       1159. ActReqRemMyPho		Request the removal of my photo
       1160. ActRemMyPho		Remove my photo

       1161. ActEdiPri			Edit my privacy
       1162. ActChgPriPho		Change privacy of my photo
       1163. ActChgPriPrf		Change privacy of my public profile

       1164. ActReqEdiMyIns		Request the edition of my institution, centre and department
       1165. ActChgCtyMyIns		Change the country of my institution
       1166. ActChgMyIns		Change my institution
       1167. ActChgMyCtr		Change my centre
       1168. ActChgMyDpt		Change my department
       1169. ActChgMyOff		Change my office
       1170. ActChgMyOffPho		Change my office phone

       1171. ActReqEdiMyNet		Request the edition of my social networks
       1172. ActChgMyNet		Change my web and social networks

       1173. ActChgThe			Change theme
       1174. ActReqChgLan		Ask if change language
       1175. ActChgLan			Change language
       1176. ActChg1stDay		Change first day of the week
       1177. ActChgCol			Change side columns
       1178. ActHidLftCol		Hide left side column
       1179. ActHidRgtCol		Hide right side column
       1180. ActShoLftCol		Show left side column
       1181. ActShoRgtCol		Show right side column
       1182. ActChgIco			Change icon set
       1183. ActChgMnu			Change menu
       1184. ActChgNtfPrf		Change whether to notify by e-mail new messages
       1185. ActPrnUsrQR		Show my QR code ready to print

       1186. ActPrnMyTT			Show the timetable listo to impresión of all my courses
       1187. ActEdiTut			Edit the timetable of tutorías
       1188. ActChgTut			Modify the timetable of tutorías
       1189. ActChgMyTT1stDay		Change first day of week and show timetable of the course

       1190. ActReqRemFilBrf		Request removal of a file of the briefcase
       1191. ActRemFilBrf		Remove a file of the briefcase
       1192. ActRemFolBrf		Remove a folder empty of the briefcase
       1193. ActCopBrf			Set source of copy in the briefcase
       1194. ActPasBrf			Paste a folder or file in the briefcase
       1195. ActRemTreBrf		Remove a folder no empty of the briefcase
       1196. ActFrmCreBrf		Form to crear a folder or file in the briefcase
       1197. ActCreFolBrf		Create a new folder in the briefcase
       1198. ActCreLnkBrf		Create a new link in the briefcase
       1199. ActRenFolBrf		Rename a folder of the briefcase
       1200. ActRcvFilBrfDZ		Receive a file in the briefcase using Dropzone.js
       1201. ActRcvFilBrfCla		Receive a file in the briefcase using the classic way
       1202. ActExpBrf			Expand a folder in briefcase
       1203. ActConBrf			Contract a folder in briefcase
       1204. ActZIPBrf			Compress a folder in briefcase
       1205. ActReqDatBrf		Ask for metadata of a file in the briefcase
       1206. ActChgDatBrf		Change metadata of a file in the briefcase
       1207. ActDowBrf			Download a file in the briefcase
       1208. ActReqRemOldBrf		Ask for removing old files in the briefcase
       1209. ActRemOldBrf		Remove old files in the briefcase
*/

struct Act_Actions Act_Actions[Act_NUM_ACTIONS] =
  {
   // TabUnk ******************************************************************
   // Actions not in menu:
   /* ActAll		*/{ 645,-1,TabUnk,ActAll		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,NULL				,NULL},	// Used for statistics
   /* ActUnk		*/{ 194,-1,TabUnk,ActUnk		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,NULL				,NULL},
   /* ActHom		*/{ 844,-1,TabUnk,ActHom		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,NULL				,NULL},
   /* ActMnu		*/{   2,-1,TabUnk,ActMnu		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,NULL				,NULL},
   /* ActRefCon		*/{ 845,-1,TabUnk,ActRefCon		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lay_RefreshNotifsAndConnected	,NULL},
   /* ActRefLstClk	*/{ 994,-1,TabUnk,ActRefLstClk		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lay_RefreshLastClicks		,NULL},
   /* ActRefNewSocPubGbl*/{1509,-1,TabUnk,ActRefNewSocPubGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_RefreshNewTimelineGbl	,NULL},
   /* ActRefOldSocPubGbl*/{1510,-1,TabUnk,ActRefOldSocPubGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_RefreshOldTimelineGbl	,NULL},
   /* ActRefOldSocPubUsr*/{1511,-1,TabUnk,ActRefOldSocPubUsr	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_RefreshOldTimelineUsr	,NULL},
   /* ActWebSvc		*/{ 892,-1,TabUnk,ActWebSvc		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_WebService			,NULL},

   // TabSys ******************************************************************
   // Actions in menu:
   /* ActSysReqSch	*/{ 627, 0,TabSys,ActSysReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sch_ReqSysSearch		,"search64x64.gif"	},

   /* ActSeeCty		*/{ 862, 1,TabSys,ActSeeCty		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Cty_ListCountries1		,Cty_ListCountries2		,"earth64x64.gif"	},
   /* ActSeePen		*/{1060, 2,TabSys,ActSeePen		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_SeePending			,"mygroups64x64.gif"	},
   /* ActReqRemOldCrs	*/{1109, 3,TabSys,ActReqRemOldCrs	,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Crs_AskRemoveOldCrss		,"removeusers64x64.gif"	},
   /* ActSeeDegTyp	*/{1013, 4,TabSys,ActSeeDegTyp		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,DT_SeeDegreeTypes		,"grouptypes64x64.gif"	},
   /* ActSeeMai		*/{ 855, 5,TabSys,ActSeeMai		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_SeeMailDomains		,"email64x64.gif"	},
   /* ActSeeBan		*/{1137, 6,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_SeeBanners			,"picture64x64.gif"	},
   /* ActSeeLnk		*/{ 748, 7,TabSys,ActSeeLnk		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lnk_SeeLinks			,"link64x64.gif"	},
   /* ActLstPlg		*/{ 777, 8,TabSys,ActLstPlg		,0x110,0x110,0x110,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_ListPlugins		,"blocks64x64.gif"	},
   /* ActSetUp		*/{ 840, 9,TabSys,ActSetUp		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Set_Setup			,"lightning64x64.gif"	},

   // Actions not in menu:
   /* ActSysSch		*/{ 628,-1,TabSys,ActSysReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sch_GetParamsSearch		,Sch_SysSearch			,NULL},
   /* ActEdiDegTyp	*/{ 573,-1,TabSys,ActSeeDegTyp		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,DT_ReqEditDegreeTypes		,NULL},

   /* ActEdiCty		*/{ 863,-1,TabSys,ActSeeCty		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cty_EditCountries		,NULL},
   /* ActNewCty		*/{ 864,-1,TabSys,ActSeeCty		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cty_RecFormNewCountry		,NULL},
   /* ActRemCty		*/{ 893,-1,TabSys,ActSeeCty		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cty_RemoveCountry		,NULL},
   /* ActRenCty		*/{ 866,-1,TabSys,ActSeeCty		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cty_RenameCountry		,NULL},
   /* ActChgCtyWWW	*/{1157,-1,TabSys,ActSeeCty		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cty_ChangeCtyWWW		,NULL},

   /* ActNewDegTyp	*/{ 537,-1,TabSys,ActSeeDegTyp		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,DT_RecFormNewDegreeType		,NULL},
   /* ActRemDegTyp	*/{ 545,-1,TabSys,ActSeeDegTyp		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,DT_RemoveDegreeType		,NULL},
   /* ActRenDegTyp	*/{ 538,-1,TabSys,ActSeeDegTyp		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,DT_RenameDegreeType		,NULL},

   /* ActRemOldCrs	*/{1110,-1,TabSys,ActReqRemOldCrs	,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Crs_RemoveOldCrss		,NULL},

   /* ActEdiMai		*/{ 856,-1,TabSys,ActSeeMai		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_EditMailDomains		,NULL},
   /* ActNewMai		*/{ 857,-1,TabSys,ActSeeMai		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RecFormNewMailDomain	,NULL},
   /* ActRemMai		*/{ 860,-1,TabSys,ActSeeMai		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RemoveMailDomain		,NULL},
   /* ActRenMaiSho	*/{ 858,-1,TabSys,ActSeeMai		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RenameMailDomainShort	,NULL},
   /* ActRenMaiFul	*/{ 859,-1,TabSys,ActSeeMai		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RenameMailDomainFull	,NULL},

   /* ActEdiBan		*/{1138,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_EditBanners		,NULL},
   /* ActNewBan		*/{1139,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_RecFormNewBanner		,NULL},
   /* ActRemBan		*/{1140,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_RemoveBanner		,NULL},
   /* ActShoBan		*/{1212,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_ShowBanner			,NULL},
   /* ActHidBan		*/{1213,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_HideBanner			,NULL},
   /* ActRenBanSho	*/{1141,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_RenameBannerShort		,NULL},
   /* ActRenBanFul	*/{1142,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_RenameBannerFull		,NULL},
   /* ActChgBanImg	*/{1144,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_ChangeBannerImg		,NULL},
   /* ActChgBanWWW	*/{1143,-1,TabSys,ActSeeBan		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ban_ChangeBannerWWW		,NULL},
   /* ActClkBan		*/{1145,-1,TabSys,ActSeeBan		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_DOWNLD_FILE,Ban_ClickOnBanner		,NULL				,NULL},

   /* ActEdiLnk		*/{ 749,-1,TabSys,ActSeeLnk		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lnk_EditLinks			,NULL},
   /* ActNewLnk		*/{ 750,-1,TabSys,ActSeeLnk		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lnk_RecFormNewLink		,NULL},
   /* ActRemLnk		*/{ 897,-1,TabSys,ActSeeLnk		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lnk_RemoveLink			,NULL},
   /* ActRenLnkSho	*/{ 753,-1,TabSys,ActSeeLnk		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lnk_RenameLinkShort		,NULL},
   /* ActRenLnkFul	*/{ 751,-1,TabSys,ActSeeLnk		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lnk_RenameLinkFull		,NULL},
   /* ActChgLnkWWW	*/{ 752,-1,TabSys,ActSeeLnk		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Lnk_ChangeLinkWWW		,NULL},

   // Actions not in menu:
   /* ActEdiPlg		*/{ 778,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_EditPlugins		,NULL},
   /* ActNewPlg		*/{ 779,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_RecFormNewPlg		,NULL},
   /* ActRemPlg		*/{ 889,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_RemovePlugin		,NULL},
   /* ActRenPlg		*/{ 782,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_RenamePlugin		,NULL},
   /* ActChgPlgDes	*/{ 888,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_ChangePlgDescription	,NULL},
   /* ActChgPlgLog	*/{ 781,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_ChangePlgLogo		,NULL},
   /* ActChgPlgAppKey	*/{ 986,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_ChangePlgAppKey		,NULL},
   /* ActChgPlgURL	*/{ 783,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_ChangePlgURL		,NULL},
   /* ActChgPlgIP	*/{ 780,-1,TabSys,ActLstPlg		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plg_ChangePlgIP		,NULL},

   // TabCty ******************************************************************
   // Actions in menu:
   /* ActCtyReqSch	*/{1176, 0,TabCty,ActCtyReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sch_ReqCtySearch		,"search64x64.gif"	},

   /* ActSeeCtyInf	*/{1155, 1,TabCty,ActSeeCtyInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cty_ShowConfiguration		,"cty64x64.gif"		},
   /* ActSeeIns		*/{ 696, 2,TabCty,ActSeeIns		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_ShowInssOfCurrentCty	,"institution64x64.gif"	},

   // Actions not in menu:
   /* ActCtySch		*/{1181,-1,TabCty,ActCtyReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sch_GetParamsSearch		,Sch_CtySearch			,NULL},

   /* ActPrnCtyInf	*/{1156,-1,TabCty,ActSeeCtyInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Cty_PrintConfiguration		,NULL},
   /* ActChgCtyMapAtt	*/{1158,-1,TabCty,ActSeeCtyInf		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cty_ChangeCtyMapAttribution	,NULL},

   /* ActEdiIns		*/{ 697,-1,TabCty,ActSeeIns		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_EditInstitutions		,NULL},
   /* ActReqIns		*/{1210,-1,TabCty,ActSeeIns		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_RecFormReqIns		,NULL},
   /* ActNewIns		*/{ 698,-1,TabCty,ActSeeIns		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_RecFormNewIns		,NULL},
   /* ActRemIns		*/{ 759,-1,TabCty,ActSeeIns		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_RemoveInstitution		,NULL},
   /* ActRenInsSho	*/{ 702,-1,TabCty,ActSeeIns		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ins_RenameInsShort		,Ins_ContEditAfterChgIns	,NULL},
   /* ActRenInsFul	*/{ 701,-1,TabCty,ActSeeIns		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ins_RenameInsFull		,Ins_ContEditAfterChgIns	,NULL},
   /* ActChgInsWWW	*/{ 700,-1,TabCty,ActSeeIns		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_ChangeInsWWW		,NULL},
   /* ActChgInsSta	*/{1211,-1,TabCty,ActSeeIns		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_ChangeInsStatus		,NULL},

   // TabIns ******************************************************************
   // Actions in menu:
   /* ActInsReqSch	*/{1177, 0,TabIns,ActInsReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sch_ReqInsSearch		,"search64x64.gif"	},

   /* ActSeeInsInf	*/{1153, 1,TabIns,ActSeeInsInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_ShowConfiguration		,"ins64x64.gif"		},
   /* ActSeeCtr		*/{ 676, 2,TabIns,ActSeeCtr		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_ShowCtrsOfCurrentIns	,"house64x64.gif"	},
   /* ActSeeDpt		*/{ 675, 3,TabIns,ActSeeDpt		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_SeeDepts			,"houseteachers64x64.gif"},
   /* ActSeePlc		*/{ 703, 4,TabIns,ActSeePlc		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plc_SeePlaces			,"earth64x64.gif"	},
   /* ActSeeHld		*/{ 707, 5,TabIns,ActSeeHld		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_SeeHolidays		,"date64x64.gif"	},
   /* ActSeeAdmDocIns	*/{1249, 6,TabIns,ActSeeAdmDocIns	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaIns	*/{1382, 7,TabIns,ActAdmShaIns		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},

   // Actions not in menu:
   /* ActInsSch		*/{1182,-1,TabIns,ActInsReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sch_GetParamsSearch		,Sch_InsSearch			,NULL},

   /* ActPrnInsInf	*/{1154,-1,TabIns,ActSeeInsInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Ins_PrintConfiguration		,NULL},
   /* ActChgInsCtyCfg	*/{1590,-1,TabIns,ActSeeIns		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ins_ChangeInsCtyInConfig,Ins_ContEditAfterChgInsInConfig,NULL},
   /* ActReqInsLog	*/{1245,-1,TabIns,ActSeeInsInf		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ins_RequestLogo		,NULL},
   /* ActRecInsLog	*/{ 699,-1,TabIns,ActSeeInsInf		,0x100,0x100,0x100,Act_CONTENT_DATA,Act_MAIN_WINDOW,Ins_ReceiveLogo		,Ins_ShowConfiguration		,NULL},
   /* ActRemInsLog	*/{1341,-1,TabIns,ActSeeInsInf		,0x100,0x100,0x100,Act_CONTENT_DATA,Act_MAIN_WINDOW,Ins_RemoveLogo		,Ins_ShowConfiguration		,NULL},

   /* ActEdiCtr		*/{ 681,-1,TabIns,ActSeeCtr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_EditCentres		,NULL},
   /* ActReqCtr		*/{1208,-1,TabIns,ActSeeCtr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_RecFormReqCtr		,NULL},
   /* ActNewCtr		*/{ 685,-1,TabIns,ActSeeCtr		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_RecFormNewCtr		,NULL},
   /* ActRemCtr		*/{ 686,-1,TabIns,ActSeeCtr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_RemoveCentre		,NULL},
   /* ActChgDegPlc	*/{ 706,-1,TabIns,ActSeeCtr		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_ChangeCentrePlace		,NULL},
   /* ActRenCtrSho	*/{ 682,-1,TabIns,ActSeeCtr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ctr_RenameCentreShort	,Ctr_ContEditAfterChgCtr	,NULL},
   /* ActRenCtrFul	*/{ 684,-1,TabIns,ActSeeCtr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ctr_RenameCentreFull	,Ctr_ContEditAfterChgCtr	,NULL},
   /* ActChgCtrWWW	*/{ 683,-1,TabIns,ActSeeCtr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_ChangeCtrWWW		,NULL},
   /* ActChgCtrSta	*/{1209,-1,TabIns,ActSeeCtr		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_ChangeCtrStatus		,NULL},

   /* ActEdiDpt		*/{ 677,-1,TabIns,ActSeeDpt		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_EditDepartments		,NULL},
   /* ActNewDpt		*/{ 687,-1,TabIns,ActSeeDpt		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_RecFormNewDpt		,NULL},
   /* ActRemDpt		*/{ 690,-1,TabIns,ActSeeDpt		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_RemoveDepartment		,NULL},
   /* ActChgDptIns	*/{ 721,-1,TabIns,ActSeeDpt		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_ChangeDepartIns		,NULL},
   /* ActRenDptSho	*/{ 688,-1,TabIns,ActSeeDpt		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_RenameDepartShort		,NULL},
   /* ActRenDptFul	*/{ 689,-1,TabIns,ActSeeDpt		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_RenameDepartFull		,NULL},
   /* ActChgDptWWW	*/{ 691,-1,TabIns,ActSeeDpt		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dpt_ChangeDptWWW		,NULL},

   /* ActEdiPlc		*/{ 704,-1,TabIns,ActSeePlc		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plc_EditPlaces			,NULL},
   /* ActNewPlc		*/{ 705,-1,TabIns,ActSeePlc		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plc_RecFormNewPlace		,NULL},
   /* ActRemPlc		*/{ 776,-1,TabIns,ActSeePlc		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plc_RemovePlace		,NULL},
   /* ActRenPlcSho	*/{ 894,-1,TabIns,ActSeePlc		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plc_RenamePlaceShort		,NULL},
   /* ActRenPlcFul	*/{ 895,-1,TabIns,ActSeePlc		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Plc_RenamePlaceFull		,NULL},

   /* ActEdiHld		*/{ 713,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_EditHolidays		,NULL},
   /* ActNewHld		*/{ 714,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_RecFormNewHoliday		,NULL},
   /* ActRemHld		*/{ 716,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_RemoveHoliday		,NULL},
   /* ActChgHldPlc	*/{ 896,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_ChangeHolidayPlace		,NULL},
   /* ActChgHldTyp	*/{ 715,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_ChangeHolidayType		,NULL},
   /* ActChgHldStrDat	*/{ 717,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_ChangeStartDate		,NULL},
   /* ActChgHldEndDat	*/{ 718,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_ChangeEndDate		,NULL},
   /* ActRenHld		*/{ 766,-1,TabIns,ActSeeHld		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Hld_RenameHoliday		,NULL},

   /* ActChgToSeeDocIns	*/{1308,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocIns	*/{1309,-1,TabIns,ActSeeAdmDocIns	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocIns	*/{1310,-1,TabIns,ActSeeAdmDocIns	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocIns	*/{1311,-1,TabIns,ActSeeAdmDocIns	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocIns	*/{1312,-1,TabIns,ActSeeAdmDocIns	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocIns*/{1313,-1,TabIns,ActSeeAdmDocIns	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocIns   */{1314,-1,TabIns,ActSeeAdmDocIns	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocIns	*/{1315,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocIns	*/{1316,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocIns*/{1317,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocIns	*/{1318,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocIns	*/{1319,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocIns	*/{1320,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocIns	*/{1321,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocIns	*/{1322,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocIns	*/{1323,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocIns	*/{1324,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocIns	*/{1325,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocIns	*/{1326,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocInsDZ	*/{1327,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilDocInsCla*/{1328,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocIns	*/{1329,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocIns	*/{1330,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocIns	*/{1331,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActShoDocIns	*/{1332,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocIns	*/{1333,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocIns*/{1334,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocIns*/{1335,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocIns   */{1336,-1,TabIns,ActSeeAdmDocIns	,0x180,0x180,0x180,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilShaIns*/{1383,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaIns	*/{1384,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaIns	*/{1385,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaIns	*/{1386,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaIns	*/{1387,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaIns	*/{1388,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaIns	*/{1389,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaIns	*/{1390,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaIns	*/{1391,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaIns	*/{1392,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaInsDZ	*/{1393,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilShaInsCla*/{1394,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaIns	*/{1395,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConShaIns	*/{1396,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPShaIns	*/{1397,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaIns	*/{1398,-1,TabIns,ActAdmShaIns		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaIns	*/{1399,-1,TabIns,ActAdmShaIns		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaIns      */{1400,-1,TabIns,ActAdmShaIns		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   // TabCtr ******************************************************************
   // Actions in menu:
   /* ActCtrReqSch	*/{1178, 0,TabCtr,ActCtrReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sch_ReqCtrSearch		,"search64x64.gif"	},

   /* ActSeeCtrInf	*/{1151, 1,TabCtr,ActSeeCtrInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_ShowConfiguration		,"ctr64x64.gif"		},
   /* ActSeeDeg		*/{1011, 2,TabCtr,ActSeeDeg		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_ShowDegsOfCurrentCtr	,"deg64x64.gif"		},
   /* ActSeeAdmDocCtr	*/{1248, 3,TabCtr,ActSeeAdmDocCtr	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaCtr	*/{1363, 4,TabCtr,ActAdmShaCtr		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},

   // Actions not in menu:
   /* ActCtrSch		*/{1183,-1,TabCtr,ActCtrReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sch_GetParamsSearch		,Sch_CtrSearch			,NULL},

   /* ActPrnCtrInf	*/{1152,-1,TabCtr,ActSeeCtrInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Ctr_PrintConfiguration		,NULL},
   /* ActChgCtrInsCfg	*/{1589,-1,TabCtr,ActSeeCtrInf		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ctr_ChangeCtrInsInConfig	,Ctr_ContEditAfterChgCtrInConfig,NULL},
   /* ActReqCtrLog	*/{1244,-1,TabCtr,ActSeeCtrInf		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_RequestLogo		,NULL},
   /* ActRecCtrLog	*/{1051,-1,TabCtr,ActSeeCtrInf		,0x100,0x100,0x100,Act_CONTENT_DATA,Act_MAIN_WINDOW,Ctr_ReceiveLogo		,Ctr_ShowConfiguration		,NULL},
   /* ActRemCtrLog	*/{1342,-1,TabCtr,ActSeeCtrInf		,0x100,0x100,0x100,Act_CONTENT_DATA,Act_MAIN_WINDOW,Ctr_RemoveLogo		,Ctr_ShowConfiguration		,NULL},
   /* ActReqCtrPho	*/{1160,-1,TabCtr,ActSeeCtrInf		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_RequestPhoto		,NULL},
   /* ActRecCtrPho	*/{1161,-1,TabCtr,ActSeeCtrInf		,0x100,0x100,0x100,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Ctr_ReceivePhoto		,NULL},
   /* ActChgCtrPhoAtt	*/{1159,-1,TabCtr,ActSeeCtrInf		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ctr_ChangeCtrPhotoAttribution	,NULL},

   /* ActEdiDeg		*/{ 536,-1,TabCtr,ActSeeDeg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_EditDegrees		,NULL},
   /* ActReqDeg		*/{1206,-1,TabCtr,ActSeeDeg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_RecFormReqDeg		,NULL},
   /* ActNewDeg		*/{ 540,-1,TabCtr,ActSeeDeg		,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_RecFormNewDeg		,NULL},
   /* ActRemDeg		*/{ 542,-1,TabCtr,ActSeeDeg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_RemoveDegree		,NULL},
   /* ActRenDegSho	*/{ 546,-1,TabCtr,ActSeeDeg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Deg_RenameDegreeShort	,Deg_ContEditAfterChgDeg	,NULL},
   /* ActRenDegFul	*/{ 547,-1,TabCtr,ActSeeDeg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Deg_RenameDegreeFull	,Deg_ContEditAfterChgDeg	,NULL},
   /* ActChgDegTyp	*/{ 544,-1,TabCtr,ActSeeDeg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,DT_ChangeDegreeType		,NULL},
   /* ActChgDegWWW	*/{ 554,-1,TabCtr,ActSeeDeg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_ChangeDegWWW		,NULL},
   /* ActChgDegSta	*/{1207,-1,TabCtr,ActSeeDeg		,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_ChangeDegStatus		,NULL},

   /* ActChgToSeeDocCtr	*/{1279,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocCtr	*/{1280,-1,TabCtr,ActSeeAdmDocCtr	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocCtr	*/{1281,-1,TabCtr,ActSeeAdmDocCtr	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocCtr	*/{1282,-1,TabCtr,ActSeeAdmDocCtr	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocCtr	*/{1283,-1,TabCtr,ActSeeAdmDocCtr	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocCtr*/{1284,-1,TabCtr,ActSeeAdmDocCtr	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocCtr   */{1285,-1,TabCtr,ActSeeAdmDocCtr	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocCtr	*/{1286,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocCtr	*/{1287,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocCtr*/{1288,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocCtr	*/{1289,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocCtr	*/{1290,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocCtr	*/{1291,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocCtr	*/{1292,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocCtr	*/{1293,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocCtr	*/{1294,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocCtr	*/{1295,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocCtr	*/{1296,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocCtr	*/{1297,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocCtrDZ	*/{1298,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilDocCtrCla*/{1299,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocCtr	*/{1300,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocCtr	*/{1301,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocCtr	*/{1302,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActShoDocCtr	*/{1303,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocCtr	*/{1304,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocCtr*/{1305,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocCtr*/{1306,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocCtr   */{1307,-1,TabCtr,ActSeeAdmDocCtr	,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilShaCtr*/{1364,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaCtr	*/{1365,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaCtr	*/{1366,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaCtr	*/{1367,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaCtr	*/{1368,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaCtr	*/{1369,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaCtr	*/{1370,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaCtr	*/{1371,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaCtr	*/{1372,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaCtr	*/{1373,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaCtrDZ	*/{1374,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilShaCtrCla*/{1375,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaCtr	*/{1376,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConShaCtr	*/{1377,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPShaCtr	*/{1378,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaCtr	*/{1379,-1,TabCtr,ActAdmShaCtr		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaCtr	*/{1380,-1,TabCtr,ActAdmShaCtr		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaCtr      */{1381,-1,TabCtr,ActAdmShaCtr		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   // TabDeg ******************************************************************
   // Actions in menu:
   /* ActDegReqSch	*/{1179, 0,TabDeg,ActDegReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sch_ReqDegSearch		,"search64x64.gif"	},

   /* ActSeeDegInf	*/{1149, 1,TabDeg,ActSeeDegInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_ShowConfiguration		,"deg64x64.gif"		},
   /* ActSeeCrs		*/{1009, 2,TabDeg,ActSeeCrs		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Crs_ShowCrssOfCurrentDeg	,"coursesdegree64x64.gif"},
   /* ActSeeAdmDocDeg	*/{1247, 3,TabDeg,ActSeeAdmDocDeg	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaDeg	*/{1344, 4,TabDeg,ActAdmShaDeg		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},

   // Actions not in menu:
   /* ActDegSch		*/{1184,-1,TabDeg,ActDegReqSch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sch_GetParamsSearch		,Sch_DegSearch			,NULL},

   /* ActPrnDegInf	*/{1150,-1,TabDeg,ActSeeDegInf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Deg_PrintConfiguration		,NULL},
   /* ActChgDegCtrCfg	*/{1588,-1,TabDeg,ActSeeDegInf		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,Deg_ChangeDegCtrInConfig	,Deg_ContEditAfterChgDegInConfig,NULL},
   /* ActReqDegLog	*/{1246,-1,TabDeg,ActSeeDegInf		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Deg_RequestLogo		,NULL},
   /* ActRecDegLog	*/{ 553,-1,TabDeg,ActSeeDegInf		,0x100,0x100,0x100,Act_CONTENT_DATA,Act_MAIN_WINDOW,Deg_ReceiveLogo		,Deg_ShowConfiguration		,NULL},
   /* ActRemDegLog	*/{1343,-1,TabDeg,ActSeeDegInf		,0x100,0x100,0x100,Act_CONTENT_DATA,Act_MAIN_WINDOW,Deg_RemoveLogo		,Deg_ShowConfiguration		,NULL},

   /* ActEdiCrs		*/{ 555,-1,TabDeg,ActSeeCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Crs_ReqEditCourses		,NULL},
   /* ActReqCrs		*/{1053,-1,TabDeg,ActSeeCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_RecFormReqCrs		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActNewCrs		*/{ 556,-1,TabDeg,ActSeeCrs		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_RecFormNewCrs		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActRemCrs		*/{ 560,-1,TabDeg,ActSeeCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Crs_RemoveCourse		,NULL},
   /* ActChgInsCrsCod	*/{1025,-1,TabDeg,ActSeeCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_ChangeInsCrsCod		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActChgCrsYea	*/{ 561,-1,TabDeg,ActSeeCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_ChangeCrsYear		,Crs_ContEditAfterChgCrs	,NULL},
   /* ActRenCrsSho	*/{ 563,-1,TabDeg,ActSeeCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_RenameCourseShort	,Crs_ContEditAfterChgCrs	,NULL},
   /* ActRenCrsFul	*/{ 564,-1,TabDeg,ActSeeCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_RenameCourseFull	,Crs_ContEditAfterChgCrs	,NULL},
   /* ActChgCrsSta	*/{1055,-1,TabDeg,ActSeeCrs		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_ChangeCrsStatus		,Crs_ContEditAfterChgCrs	,NULL},

   /* ActChgToSeeDocDeg	*/{1250,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocDeg	*/{1251,-1,TabDeg,ActSeeAdmDocDeg	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocDeg	*/{1252,-1,TabDeg,ActSeeAdmDocDeg	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocDeg	*/{1253,-1,TabDeg,ActSeeAdmDocDeg	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocDeg	*/{1254,-1,TabDeg,ActSeeAdmDocDeg	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocDeg*/{1255,-1,TabDeg,ActSeeAdmDocDeg	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocDeg   */{1256,-1,TabDeg,ActSeeAdmDocDeg	,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocDeg	*/{1257,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocDeg	*/{1258,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocDeg*/{1259,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocDeg	*/{1260,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocDeg	*/{1261,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocDeg	*/{1262,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocDeg	*/{1263,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocDeg	*/{1264,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocDeg	*/{1265,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocDeg	*/{1266,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocDeg	*/{1267,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocDeg	*/{1268,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocDegDZ	*/{1269,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilDocDegCla*/{1270,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocDeg	*/{1271,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocDeg	*/{1272,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocDeg	*/{1273,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActShoDocDeg	*/{1274,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocDeg	*/{1275,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocDeg*/{1276,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocDeg*/{1277,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocDeg   */{1278,-1,TabDeg,ActSeeAdmDocDeg	,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilShaDeg*/{1345,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaDeg	*/{1346,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaDeg	*/{1347,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaDeg	*/{1348,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaDeg	*/{1349,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaDeg	*/{1350,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaDeg	*/{1351,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaDeg	*/{1352,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaDeg	*/{1353,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaDeg	*/{1354,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaDegDZ	*/{1355,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilShaDegCla*/{1356,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaDeg	*/{1357,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConShaDeg	*/{1358,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPShaDeg	*/{1359,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaDeg	*/{1360,-1,TabDeg,ActAdmShaDeg		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaDeg	*/{1361,-1,TabDeg,ActAdmShaDeg		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaDeg      */{1362,-1,TabDeg,ActAdmShaDeg		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   // TabCrs ******************************************************************
   // Actions in menu:
   /* ActCrsReqSch	*/{1180, 0,TabCrs,ActCrsReqSch		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sch_ReqCrsSearch		,"search64x64.gif"	},

   /* ActSeeCrsInf	*/{ 847, 1,TabCrs,ActSeeCrsInf		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Crs_ShowIntroduction		,"info64x64.gif"	},
   /* ActSeeTchGui	*/{ 784, 2,TabCrs,ActSeeTchGui		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,"guide64x64.gif"	},
   /* ActSeeSyl		*/{1242, 3,TabCrs,ActSeeSyl		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,"presentation64x64.gif"},
   /* ActSeeAdmDocCrsGrp*/{   0, 4,TabCrs,ActSeeAdmDocCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmTchCrsGrp	*/{1525, 5,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folder64x64.gif"	},
   /* ActAdmShaCrsGrp	*/{ 461, 6,TabCrs,ActAdmShaCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"folderusers64x64.gif"	},
   /* ActSeeCrsTT	*/{  25, 7,TabCrs,ActSeeCrsTT		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,TT_ShowClassTimeTable		,"clock64x64.gif"	},
   /* ActSeeBib		*/{  32, 8,TabCrs,ActSeeBib		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,"books64x64.gif"	},
   /* ActSeeFAQ		*/{  54, 9,TabCrs,ActSeeFAQ		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,"faq64x64.gif"		},
   /* ActSeeCrsLnk	*/{   9,10,TabCrs,ActSeeCrsLnk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,"link64x64.gif"	},

   // Actions not in menu:
   /* ActDegSch		*/{1185,-1,TabCrs,ActCrsReqSch		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sch_GetParamsSearch		,Sch_CrsSearch			,NULL},

   /* ActPrnCrsInf	*/{1028,-1,TabCrs,ActSeeCrsInf		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Crs_PrintConfiguration		,NULL},
   /* ActChgCrsDegCfg	*/{1587,-1,TabCrs,ActSeeCrsInf		,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_ChangeCrsDegInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActChgInsCrsCodCfg*/{1024,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_ChangeInsCrsCodInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActChgCrsYeaCfg	*/{1573,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Crs_ChangeCrsYearInConfig	,Crs_ContEditAfterChgCrsInConfig,NULL},
   /* ActEdiCrsInf	*/{ 848,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},

   /* ActEdiTchGui	*/{ 785,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},

   /* ActPrnCrsTT	*/{ 152,-1,TabCrs,ActSeeCrsTT		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,TT_ShowClassTimeTable		,NULL},
   /* ActEdiCrsTT	*/{  45,-1,TabCrs,ActSeeCrsTT		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,TT_EditCrsTimeTable		,NULL},
   /* ActChgCrsTT	*/{  53,-1,TabCrs,ActSeeCrsTT		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,TT_EditCrsTimeTable		,NULL},
   /* ActChgCrsTT1stDay	*/{1486,-1,TabCrs,ActSeeCrsTT		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Cal_ChangeFirstDayOfWeek	,TT_ShowClassTimeTable		,NULL},

   /* ActSeeSylLec	*/{  28,-1,TabCrs,ActSeeSyl		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,NULL},
   /* ActSeeSylPra	*/{  20,-1,TabCrs,ActSeeSyl		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,NULL},
   /* ActEdiSylLec	*/{  44,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},
   /* ActEdiSylPra	*/{  74,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},
   /* ActDelItmSylLec	*/{ 218,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_RemoveItemSyllabus		,NULL},
   /* ActDelItmSylPra	*/{ 183,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_RemoveItemSyllabus		,NULL},
   /* ActUp_IteSylLec	*/{ 221,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_UpItemSyllabus		,NULL},
   /* ActUp_IteSylPra	*/{ 213,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_UpItemSyllabus		,NULL},
   /* ActDwnIteSylLec	*/{ 220,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_DownItemSyllabus		,NULL},
   /* ActDwnIteSylPra	*/{ 212,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_DownItemSyllabus		,NULL},
   /* ActRgtIteSylLec	*/{ 223,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_RightItemSyllabus		,NULL},
   /* ActRgtIteSylPra	*/{ 215,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_RightItemSyllabus		,NULL},
   /* ActLftIteSylLec	*/{ 222,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_LeftItemSyllabus		,NULL},
   /* ActLftIteSylPra	*/{ 214,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_LeftItemSyllabus		,NULL},
   /* ActInsIteSylLec	*/{ 217,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_InsertItemSyllabus		,NULL},
   /* ActInsIteSylPra	*/{ 181,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_InsertItemSyllabus		,NULL},
   /* ActModIteSylLec	*/{ 211,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_ModifyItemSyllabus		,NULL},
   /* ActModIteSylPra	*/{ 216,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_ModifyItemSyllabus		,NULL},

   /* ActChgToSeeDocCrs	*/{1195,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeDocCrs	*/{1078,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocCrs	*/{ 462,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocCrs	*/{ 476,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocCrs	*/{1124,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocCrs*/{1033,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocCrs   */{1111,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActSeeDocGrp	*/{1200,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeDocGrp	*/{ 488,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConSeeDocGrp	*/{ 489,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPSeeDocGrp	*/{1125,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatSeeDocGrp*/{1034,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActDowSeeDocGrp   */{1112,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmDocCrs	*/{1196,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmDocCrs	*/{  12,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocCrs*/{ 479,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocCrs	*/{ 480,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocCrs	*/{ 497,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocCrs	*/{ 470,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocCrs	*/{ 478,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocCrs	*/{ 498,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocCrs	*/{ 481,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocCrs	*/{ 491,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocCrs	*/{1225,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocCrs	*/{ 535,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocCrsDZ	*/{1214,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilDocCrsCla*/{ 482,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocCrs	*/{ 477,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocCrs	*/{ 494,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocCrs	*/{1126,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActShoDocCrs	*/{ 464,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocCrs	*/{ 465,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocCrs*/{1029,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocCrs*/{ 996,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocCrs   */{1113,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActAdmDocGrp	*/{1201,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilDocGrp*/{ 473,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilDocGrp	*/{ 474,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolDocGrp	*/{ 484,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopDocGrp	*/{ 472,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasDocGrp	*/{ 471,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreDocGrp	*/{ 485,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreDocGrp	*/{ 468,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolDocGrp	*/{ 469,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkDocGrp	*/{1231,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolDocGrp	*/{ 490,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilDocGrpDZ	*/{1215,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilDocGrpCla*/{ 483,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmDocGrp	*/{ 486,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAdmDocGrp	*/{ 487,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmDocGrp	*/{1127,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActShoDocGrp	*/{ 493,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidDocGrp	*/{ 492,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmDocGrp*/{1030,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmDocGrp*/{ 998,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmDocGrp   */{1114,-1,TabCrs,ActSeeAdmDocCrsGrp	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmTch	*/{1526,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmTchCrs	*/{1527,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilTchCrs*/{1528,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilTchCrs	*/{1529,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolTchCrs	*/{1530,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopTchCrs	*/{1531,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasTchCrs	*/{1532,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreTchCrs	*/{1533,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreTchCrs	*/{1534,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolTchCrs	*/{1535,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkTchCrs	*/{1536,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolTchCrs	*/{1537,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilTchCrsDZ	*/{1538,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilTchCrsCla*/{1539,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpTchCrs	*/{1540,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConTchCrs	*/{1541,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPTchCrs	*/{1542,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatTchCrs	*/{1543,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatTchCrs	*/{1544,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowTchCrs      */{1545,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActAdmTchGrp	*/{1546,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilTchGrp*/{1547,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilTchGrp	*/{1548,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolTchGrp	*/{1549,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopTchGrp	*/{1550,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasTchGrp	*/{1551,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreTchGrp	*/{1552,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreTchGrp	*/{1553,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolTchGrp	*/{1554,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkTchGrp	*/{1555,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolTchGrp	*/{1556,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilTchGrpDZ	*/{1557,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilTchGrpCla*/{1558,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpTchGrp	*/{1559,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConTchGrp	*/{1560,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPTchGrp	*/{1561,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatTchGrp	*/{1562,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatTchGrp	*/{1563,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowTchGrp      */{1564,-1,TabCrs,ActAdmTchCrsGrp	,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActChgToAdmSha	*/{1197,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmShaCrs	*/{1202,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilShaCrs*/{ 327,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaCrs	*/{ 328,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaCrs	*/{ 325,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaCrs	*/{ 330,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaCrs	*/{ 331,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaCrs	*/{ 332,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaCrs	*/{ 323,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaCrs	*/{ 324,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaCrs	*/{1226,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaCrs	*/{ 329,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaCrsDZ	*/{1216,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilShaCrsCla*/{ 326,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaCrs	*/{ 421,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConShaCrs	*/{ 422,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPShaCrs	*/{1128,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaCrs	*/{1031,-1,TabCrs,ActAdmShaCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaCrs	*/{1000,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaCrs      */{1115,-1,TabCrs,ActAdmShaCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActAdmShaGrp	*/{1203,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilShaGrp*/{ 341,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilShaGrp	*/{ 342,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolShaGrp	*/{ 338,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopShaGrp	*/{ 336,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasShaGrp	*/{ 337,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreShaGrp	*/{ 339,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreShaGrp	*/{ 333,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolShaGrp	*/{ 334,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkShaGrp	*/{1227,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolShaGrp	*/{ 340,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilShaGrpDZ	*/{1217,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilShaGrpCla*/{ 335,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpShaGrp	*/{ 427,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConShaGrp	*/{ 426,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPShaGrp	*/{1129,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatShaGrp	*/{1032,-1,TabCrs,ActAdmShaCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatShaGrp	*/{1002,-1,TabCrs,ActAdmShaCrsGrp	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowShaGrp      */{1116,-1,TabCrs,ActAdmShaCrsGrp	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActEdiBib		*/{  76,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},
   /* ActEdiFAQ		*/{ 109,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},
   /* ActEdiCrsLnk	*/{  96,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},

   /* ActChgFrcReaCrsInf*/{ 877,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaTchGui*/{ 870,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaSylLec*/{ 871,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaSylPra*/{ 872,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaBib	*/{ 873,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaFAQ	*/{ 874,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgFrcReaCrsLnk*/{ 875,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},

   /* ActChgHavReaCrsInf*/{ 878,-1,TabCrs,ActSeeCrsInf		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaTchGui*/{ 886,-1,TabCrs,ActSeeTchGui		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaSylLec*/{ 880,-1,TabCrs,ActSeeSyl		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaSylPra*/{ 887,-1,TabCrs,ActSeeSyl		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaBib	*/{ 884,-1,TabCrs,ActSeeBib		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaFAQ	*/{ 879,-1,TabCrs,ActSeeFAQ		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActChgHavReaCrsLnk*/{ 885,-1,TabCrs,ActSeeCrsLnk		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},

   /* ActSelInfSrcCrsInf*/{ 849,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcTchGui*/{ 789,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcSylLec*/{ 378,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcSylPra*/{ 382,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcBib	*/{ 370,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcFAQ	*/{ 380,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},
   /* ActSelInfSrcCrsLnk*/{ 385,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},

   /* ActRcvURLCrsInf	*/{ 854,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLTchGui	*/{ 791,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLSylLec	*/{ 403,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLSylPra	*/{ 402,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLBib	*/{ 224,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLFAQ	*/{ 234,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvURLCrsLnk	*/{ 182,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},

   /* ActRcvPagCrsInf	*/{ 853,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagTchGui	*/{ 788,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagSylLec	*/{ 381,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagSylPra	*/{ 383,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagBib	*/{ 185,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagFAQ	*/{ 219,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},
   /* ActRcvPagCrsLnk	*/{ 164,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},

   /* ActEditorCrsInf	*/{ 852,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditorCourseInfo		,NULL},
   /* ActEditorTchGui	*/{ 786,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditorTeachingGuide	,NULL},
   /* ActEditorSylLec	*/{ 372,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_EditSyllabus		,NULL},
   /* ActEditorSylPra	*/{ 371,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Syl_EditSyllabus		,NULL},
   /* ActEditorBib	*/{ 376,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditorBibliography		,NULL},
   /* ActEditorFAQ	*/{ 404,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditorFAQ   		,NULL},
   /* ActEditorCrsLnk	*/{ 388,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditorLinks		,NULL},

   /* ActPlaTxtEdiCrsInf*/{ 850,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiTchGui*/{ 787,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiSylLec*/{ 379,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiSylPra*/{ 389,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiBib	*/{ 377,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo		,NULL},
   /* ActPlaTxtEdiFAQ	*/{ 405,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo   	,NULL},
   /* ActPlaTxtEdiCrsLnk*/{ 400,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo		,NULL},

   /* ActRchTxtEdiCrsInf*/{1093,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiTchGui*/{1094,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiSylLec*/{1095,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiSylPra*/{1096,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiBib	*/{1097,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo		,NULL},
   /* ActRchTxtEdiFAQ	*/{1098,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo   		,NULL},
   /* ActRchTxtEdiCrsLnk*/{1099,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo		,NULL},

   /* ActRcvPlaTxtCrsInf*/{ 851,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtTchGui*/{ 790,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtSylLec*/{ 394,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtSylPra*/{ 396,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtBib	*/{ 398,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtFAQ	*/{ 406,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvPlaTxtCrsLnk*/{ 401,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},

   /* ActRcvRchTxtCrsInf*/{1101,-1,TabCrs,ActSeeCrsInf		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtTchGui*/{1102,-1,TabCrs,ActSeeTchGui		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtSylLec*/{1103,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtSylPra*/{1104,-1,TabCrs,ActSeeSyl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtBib	*/{1105,-1,TabCrs,ActSeeBib		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtFAQ	*/{1106,-1,TabCrs,ActSeeFAQ		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},
   /* ActRcvRchTxtCrsLnk*/{1107,-1,TabCrs,ActSeeCrsLnk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},

   // TabAss ******************************************************************
   // Actions in menu:
   /* ActSeeAss		*/{  15, 0,TabAss,ActSeeAss		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ShowInfo			,"infogrades64x64.gif"	},
   /* ActSeeAsg		*/{ 801, 1,TabAss,ActSeeAsg		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_SeeAssignments		,"desk64x64.gif"	},
   /* ActAdmAsgWrkUsr	*/{ 792, 2,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"editfolderuser64x64.gif"},
   /* ActReqAsgWrkCrs	*/{ 899, 3,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskEditWorksCrs		,"folderusers64x64.gif"	},
   /* ActReqTst		*/{ 103, 4,TabAss,ActReqTst		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowFormAskTst		,"test64x64.gif"	},
   /* ActSeeCal		*/{  16, 5,TabAss,ActSeeCal		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cal_DrawCalendar		,"date64x64.gif"	},
   /* ActSeeAllExaAnn	*/{  85, 6,TabAss,ActSeeAllExaAnn	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Exa_ListExamAnnouncementsSee	,"announce64x64.gif"	},
   /* ActSeeAdmMrk	*/{  17, 7,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"grades64x64.gif"	},

   // Actions not in menu:
   /* ActEdiAss		*/{  69,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_FormsToSelSendInfo		,NULL},
   /* ActChgFrcReaAss	*/{ 883,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeForceReadInfo	,NULL},
   /* ActChgHavReaAss	*/{ 898,-1,TabAss,ActSeeAss		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ChangeIHaveReadInfo	,NULL},
   /* ActSelInfSrcAss	*/{ 384,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_SetInfoSrc			,NULL},
   /* ActRcvURLAss	*/{ 235,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_ReceiveURLInfo		,NULL},
   /* ActRcvPagAss	*/{ 184,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Inf_ReceivePagInfo		,NULL},
   /* ActEditorAss	*/{ 386,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditorAssessment		,NULL},
   /* ActPlaTxtEdiAss	*/{ 387,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditPlainTxtInfo		,NULL},
   /* ActRchTxtEdiAss	*/{1100,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_EditRichTxtInfo		,NULL},
   /* ActRcvPlaTxtAss	*/{ 397,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangePlainTxtInfo	,NULL},
   /* ActRcvRchTxtAss	*/{1108,-1,TabAss,ActSeeAss		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Inf_RecAndChangeRichTxtInfo	,NULL},

   /* ActFrmNewAsg	*/{ 812,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_RequestCreatOrEditAsg	,NULL},
   /* ActEdiOneAsg	*/{ 814,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_RequestCreatOrEditAsg	,NULL},
   /* ActNewAsg		*/{ 803,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_RecFormAssignment		,NULL},
   /* ActChgAsg		*/{ 815,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_RecFormAssignment		,NULL},
   /* ActReqRemAsg	*/{ 813,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_AskRemAssignment		,NULL},
   /* ActRemAsg		*/{ 806,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_RemoveAssignment		,NULL},
   /* ActHidAsg		*/{ 964,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_HideAssignment		,NULL},
   /* ActShoAsg		*/{ 965,-1,TabAss,ActSeeAsg		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Asg_ShowAssignment		,NULL},

   /* ActAdmAsgWrkCrs	*/{ 139,-1,TabAss,ActReqAsgWrkCrs	,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActReqRemFilAsgUsr*/{ 834,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilAsgUsr	*/{ 833,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolAsgUsr	*/{ 827,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopAsgUsr	*/{ 829,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasAsgUsr	*/{ 830,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreAsgUsr	*/{ 828,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreAsgUsr	*/{ 825,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolAsgUsr	*/{ 826,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkAsgUsr	*/{1232,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolAsgUsr	*/{ 839,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilAsgUsrDZ	*/{1218,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilAsgUsrCla*/{ 832,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAsgUsr	*/{ 824,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAsgUsr	*/{ 831,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAsgUsr	*/{1130,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatAsgUsr	*/{1039,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAsgUsr	*/{1040,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAsgUsr      */{1117,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilWrkUsr*/{ 288,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilWrkUsr	*/{ 169,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolWrkUsr	*/{ 228,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopWrkUsr	*/{ 314,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasWrkUsr	*/{ 318,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreWrkUsr	*/{ 278,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreWrkUsr	*/{ 150,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolWrkUsr	*/{ 172,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkWrkUsr	*/{1228,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolWrkUsr	*/{ 204,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilWrkUsrDZ	*/{1219,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilWrkUsrCla*/{ 148,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpWrkUsr	*/{ 423,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConWrkUsr	*/{ 425,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPWrkUsr	*/{1131,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatWrkUsr	*/{1041,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatWrkUsr	*/{1042,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowWrkUsr      */{1118,-1,TabAss,ActAdmAsgWrkUsr	,0x008,0x000,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilAsgCrs*/{ 837,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilAsgCrs	*/{ 838,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolAsgCrs	*/{ 820,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopAsgCrs	*/{ 836,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasAsgCrs	*/{ 821,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreAsgCrs	*/{ 822,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreAsgCrs	*/{ 817,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolAsgCrs	*/{ 818,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkAsgCrs	*/{1233,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolAsgCrs	*/{ 823,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilAsgCrsDZ	*/{1220,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilAsgCrsCla*/{ 846,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAsgCrs	*/{ 819,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAsgCrs	*/{ 835,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAsgCrs	*/{1132,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatAsgCrs	*/{1043,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAsgCrs	*/{1044,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAsgCrs      */{1119,-1,TabAss,ActReqAsgWrkCrs	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActReqRemFilWrkCrs*/{ 289,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilWrkCrs	*/{ 209,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolWrkCrs	*/{ 210,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopWrkCrs	*/{ 312,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasWrkCrs	*/{ 319,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreWrkCrs	*/{ 279,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreWrkCrs	*/{ 205,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolWrkCrs	*/{ 206,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkWrkCrs	*/{1229,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolWrkCrs	*/{ 208,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilWrkCrsDZ	*/{1221,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilWrkCrsCla*/{ 207,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpWrkCrs	*/{ 416,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConWrkCrs	*/{ 424,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPWrkCrs	*/{1133,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatWrkCrs	*/{1045,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatWrkCrs	*/{1046,-1,TabAss,ActReqAsgWrkCrs	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowWrkCrs      */{1120,-1,TabAss,ActReqAsgWrkCrs	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   /* ActSeeTst		*/{  29,-1,TabAss,ActReqTst		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowNewTestExam		,NULL},
   /* ActAssTst		*/{  98,-1,TabAss,ActReqTst		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_AssessTestExam		,NULL},

   /* ActEdiTstQst	*/{ 104,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Tst_SetIniEndDates		,Tst_ShowFormAskEditTsts	,NULL},
   /* ActEdiOneTstQst	*/{ 105,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowFormEditOneQst		,NULL},
   /* ActReqImpTstQst	*/{1007,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,TsI_ShowFormImportQstsFromXML	,NULL},
   /* ActImpTstQst	*/{1008,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,TsI_ImportQstsFromXML		,NULL},
   /* ActLstTstQst	*/{ 132,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ListQuestionsToEdit	,NULL},
   /* ActRcvTstQst	*/{ 126,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Tst_ReceiveQst			,NULL},
   /* ActReqRemTstQst	*/{1523,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_RequestRemoveQst		,NULL},
   /* ActRemTstQst	*/{ 133,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_RemoveQst			,NULL},
   /* ActShfTstQst	*/{ 455,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ChangeShuffleQst		,NULL},

   /* ActCfgTst		*/{ 451,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowFormConfig		,NULL},
   /* ActEnableTag	*/{ 453,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_EnableTag			,NULL},
   /* ActDisableTag	*/{ 452,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_DisableTag			,NULL},
   /* ActRenTag		*/{ 143,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_RenameTag			,NULL},
   /* ActRcvCfgTst	*/{ 454,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ReceiveConfigTst		,NULL},

   /* ActReqSeeMyTstExa	*/{1083,-1,TabAss,ActReqTst		,0x108,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Tst_SetIniEndDates		,Tst_SelDatesToSeeMyExams	,NULL},
   /* ActSeeMyTstExa	*/{1084,-1,TabAss,ActReqTst		,0x108,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowMyExams		,NULL},
   /* ActSeeOneTstExaMe	*/{1085,-1,TabAss,ActReqTst		,0x108,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowOneExam		,NULL},
   /* ActReqSeeUsrTstExa*/{1080,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Tst_SetIniEndDates		,Tst_SelUsrsToSeeUsrsExams	,NULL},
   /* ActSeeUsrTstExa	*/{1081,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowUsrsExams		,NULL},
   /* ActSeeOneTstExaOth*/{1082,-1,TabAss,ActReqTst		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Tst_ShowOneExam		,NULL},

   /* ActPrnCal		*/{  71,-1,TabAss,ActSeeCal		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Cal_DrawCalendar		,NULL},
   /* ActChgCal1stDay	*/{1485,-1,TabAss,ActSeeCal		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Cal_ChangeFirstDayOfWeek	,Cal_DrawCalendar		,NULL},

   /* ActSeeOneExaAnn	*/{1572,-1,TabAss,ActSeeAllExaAnn	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Exa_GetExaCodToHighlight	,Exa_ListExamAnnouncementsSee	,NULL},
   /* ActSeeDatExaAnn	*/{1571,-1,TabAss,ActSeeAllExaAnn	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Exa_GetDateToHighlight	,Exa_ListExamAnnouncementsSee	,NULL},
   /* ActEdiExaAnn	*/{  91,-1,TabAss,ActSeeAllExaAnn	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Exa_PutFrmEditAExamAnnouncement,NULL},
   /* ActRcvExaAnn	*/{ 110,-1,TabAss,ActSeeAllExaAnn	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Exa_ReceiveExamAnnouncement	,NULL},
   /* ActPrnExaAnn	*/{ 179,-1,TabAss,ActSeeAllExaAnn	,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Exa_PrintExamAnnouncement	,NULL},
   /* ActRemExaAnn	*/{ 187,-1,TabAss,ActSeeAllExaAnn	,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Exa_RemoveExamAnnouncement	,NULL},

   /* ActChgToSeeMrk	*/{1198,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActSeeMrkCrs	*/{1079,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeMrkCrs	*/{ 528,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConSeeMrkCrs	*/{ 527,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActReqDatSeeMrkCrs*/{1086,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActSeeMyMrkCrs	*/{ 523,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,Mrk_ShowMyMarks		,NULL				,NULL},

   /* ActSeeMrkGrp	*/{1204,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActExpSeeMrkGrp	*/{ 605,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConSeeMrkGrp	*/{ 609,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActReqDatSeeMrkGrp*/{1087,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActSeeMyMrkGrp	*/{ 524,-1,TabAss,ActSeeAdmMrk		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,Mrk_ShowMyMarks		,NULL				,NULL},

   /* ActChgToAdmMrk	*/{1199,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},

   /* ActAdmMrkCrs	*/{ 284,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilMrkCrs*/{ 595,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilMrkCrs	*/{ 533,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolMrkCrs	*/{ 530,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopMrkCrs	*/{ 501,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasMrkCrs	*/{ 507,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreMrkCrs	*/{ 534,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreMrkCrs	*/{ 596,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolMrkCrs	*/{ 506,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActRenFolMrkCrs	*/{ 574,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilMrkCrsDZ	*/{1222,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilMrkCrsCla*/{ 516,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmMrkCrs	*/{ 607,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAdmMrkCrs	*/{ 621,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmMrkCrs	*/{1134,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActShoMrkCrs	*/{1191,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidMrkCrs	*/{1192,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmMrkCrs*/{1035,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmMrkCrs*/{1036,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmMrkCrs   */{1121,-1,TabAss,ActSeeAdmMrk		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   /* ActChgNumRowHeaCrs*/{ 503,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mrk_ChangeNumRowsHeader	,NULL},
   /* ActChgNumRowFooCrs*/{ 504,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mrk_ChangeNumRowsFooter	,NULL},

   /* ActAdmMrkGrp	*/{1205,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,NULL},
   /* ActReqRemFilMrkGrp*/{ 600,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilMrkGrp	*/{ 509,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolMrkGrp	*/{ 520,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopMrkGrp	*/{ 519,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasMrkGrp	*/{ 502,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreMrkGrp	*/{ 521,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreMrkGrp	*/{ 601,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolMrkGrp	*/{ 513,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActRenFolMrkGrp	*/{ 529,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilMrkGrpDZ	*/{1223,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilMrkGrpCla*/{ 514,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpAdmMrkGrp	*/{ 631,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConAdmMrkGrp	*/{ 900,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPAdmMrkGrp	*/{1135,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActShoMrkGrp	*/{1193,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsVisible	,NULL},
   /* ActHidMrkGrp	*/{1194,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_SetDocumentAsHidden	,NULL},
   /* ActReqDatAdmMrkGrp*/{1037,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatAdmMrkGrp*/{1038,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowAdmMrkGrp   */{1122,-1,TabAss,ActSeeAdmMrk		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   /* ActChgNumRowHeaGrp*/{ 510,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mrk_ChangeNumRowsHeader	,NULL},
   /* ActChgNumRowFooGrp*/{ 511,-1,TabAss,ActSeeAdmMrk		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mrk_ChangeNumRowsFooter	,NULL},

   // TabUsr ******************************************************************
   // Actions in menu:
   /* ActReqSelGrp	*/{ 116, 0,TabUsr,ActReqSelGrp		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ReqRegisterInGrps		,"mygroups64x64.gif"	},
   /* ActLstStd		*/{ 678, 1,TabUsr,ActLstStd		,0x1F8,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_SeeStudents		,"usrs64x64.gif"	},
   /* ActLstTch		*/{ 679, 2,TabUsr,ActLstTch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_SeeTeachers		,"tch64x64.gif"		},
   /* ActLstOth		*/{1186, 3,TabUsr,ActLstOth		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_ListDataAdms		,"adm64x64.gif"		},
   /* ActSeeAtt		*/{ 861, 4,TabUsr,ActSeeAtt		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_SeeAttEvents		,"rollcall64x64.gif"	},
   /* ActReqSignUp	*/{1054, 5,TabUsr,ActReqSignUp		,0x000,0x006,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqSignUpInCrs		,"enrollmentrequest64x64.gif"},
   /* ActSeeSignUpReq	*/{1057, 6,TabUsr,ActSeeSignUpReq	,0x1F0,0x1F0,0x1F0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ShowEnrollmentRequests	,"enrollmentrequest64x64.gif"},
   /* ActLstCon		*/{ 995, 7,TabUsr,ActLstCon		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Con_ShowConnectedUsrs		,"userplugged64x64.gif"	},

   // Actions not in menu:
   /* ActChgGrp		*/{ 118,-1,TabUsr,ActReqSelGrp		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ChangeMyGrpsAndShowChanges	,NULL},

   /* ActReqEdiGrp	*/{ 108,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ReqEditGroups		,NULL},

   /* ActNewGrpTyp	*/{ 174,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_RecFormNewGrpTyp		,NULL},
   /* ActReqRemGrpTyp	*/{ 236,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ReqRemGroupType		,NULL},
   /* ActRemGrpTyp	*/{ 237,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_RemoveGroupType		,NULL},
   /* ActRenGrpTyp	*/{ 304,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_RenameGroupType		,NULL},
   /* ActChgMdtGrpTyp	*/{ 303,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ChangeMandatGrpTyp		,NULL},
   /* ActChgMulGrpTyp	*/{ 302,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ChangeMultiGrpTyp		,NULL},
   /* ActChgTimGrpTyp	*/{1061,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ChangeOpenTimeGrpTyp	,NULL},

   /* ActNewGrp		*/{ 122,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_RecFormNewGrp		,NULL},
   /* ActReqRemGrp	*/{ 107,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ReqRemGroup		,NULL},
   /* ActRemGrp		*/{ 175,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_RemoveGroup		,NULL},
   /* ActOpeGrp		*/{ 322,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_OpenGroup			,NULL},
   /* ActCloGrp		*/{ 321,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_CloseGroup			,NULL},
   /* ActEnaFilZonGrp	*/{ 495,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_EnableFileZonesGrp		,NULL},
   /* ActDisFilZonGrp	*/{ 496,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_DisableFileZonesGrp	,NULL},
   /* ActChgGrpTyp	*/{ 167,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ChangeGroupType		,NULL},
   /* ActRenGrp		*/{ 121,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_RenameGroup		,NULL},
   /* ActChgMaxStdGrp	*/{ 106,-1,TabUsr,ActReqSelGrp		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Grp_ChangeMaxStdsGrp		,NULL},

   /* ActLstGst		*/{ 587,-1,TabUsr,ActLstOth		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_SeeGuests			,NULL},

   /* ActPrnGstPho	*/{1190,-1,TabUsr,ActLstStd		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_SeeGstClassPhotoPrn	,NULL},
   /* ActPrnStdPho	*/{ 120,-1,TabUsr,ActLstStd		,0x1F8,0x1E0,0x1E0,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_SeeStdClassPhotoPrn	,NULL},
   /* ActPrnTchPho	*/{ 443,-1,TabUsr,ActLstTch		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_SeeTchClassPhotoPrn	,NULL},
   /* ActLstGstAll	*/{1189,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_ListAllDataGsts		,NULL},
   /* ActLstStdAll	*/{  42,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_ListAllDataStds		,NULL},
   /* ActLstTchAll	*/{ 578,-1,TabUsr,ActLstTch		,0x1F0,0x1F0,0x1E0,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_ListAllDataTchs 		,NULL},

   /* ActSeeRecOneStd	*/{1174,-1,TabUsr,ActLstStd		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_GetUsrAndShowRecordOneStdCrs,NULL},
   /* ActSeeRecOneTch	*/{1175,-1,TabUsr,ActLstTch		,0x1FE,0x1FE,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_GetUsrAndShowRecordOneTchCrs,NULL},

   /* ActSeeRecSevGst	*/{1187,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ListRecordsGsts		,NULL},
   /* ActSeeRecSevStd	*/{  89,-1,TabUsr,ActLstStd		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ListRecordsStdsForEdit	,NULL},
   /* ActSeeRecSevTch	*/{  22,-1,TabUsr,ActLstTch		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ListRecordsTchs		,NULL},
   /* ActPrnRecSevGst	*/{1188,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Rec_ListRecordsGsts		,NULL},
   /* ActPrnRecSevStd	*/{ 111,-1,TabUsr,ActLstStd		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Rec_ListRecordsStdsForPrint	,NULL},
   /* ActPrnRecSevTch	*/{ 127,-1,TabUsr,ActLstTch		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Rec_ListRecordsTchs		,NULL},

   /* ActRcvRecOthUsr	*/{ 300,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_UpdateAndShowOtherCrsRecord,NULL},
   /* ActEdiRecFie	*/{ 292,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ReqEditRecordFields	,NULL},
   /* ActNewFie		*/{ 293,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ReceiveFormField		,NULL},
   /* ActReqRemFie	*/{ 294,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ReqRemField		,NULL},
   /* ActRemFie		*/{ 295,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_RemoveField		,NULL},
   /* ActRenFie		*/{ 296,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_RenameField		,NULL},
   /* ActChgRowFie	*/{ 305,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ChangeLinesField		,NULL},
   /* ActChgVisFie	*/{ 297,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ChangeVisibilityField	,NULL},
   /* ActRcvRecCrs	*/{ 301,-1,TabUsr,ActLstStd		,0x108,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_UpdateAndShowMyCrsRecord	,NULL},

   /* ActReqEnrSevStd	*/{1426,-1,TabUsr,ActLstStd		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqAdminStds		,NULL},
   /* ActReqEnrSevTch	*/{1427,-1,TabUsr,ActLstTch		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqAdminTchs		,NULL},

   /* ActReqLstStdAtt	*/{1073,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_ReqListStdsAttendanceCrs	,NULL},
   /* ActSeeLstMyAtt	*/{1473,-1,TabUsr,ActSeeAtt		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_ListMyAttendanceCrs	,NULL},
   /* ActPrnLstMyAtt	*/{1474,-1,TabUsr,ActSeeAtt		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_PrintMyAttendanceCrs	,NULL},
   /* ActSeeLstStdAtt	*/{1074,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_ListStdsAttendanceCrs	,NULL},
   /* ActPrnLstStdAtt	*/{1075,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Usr_PrintStdsAttendanceCrs	,NULL},
   /* ActFrmNewAtt	*/{1063,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_RequestCreatOrEditAttEvent	,NULL},
   /* ActEdiOneAtt	*/{1064,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_RequestCreatOrEditAttEvent	,NULL},
   /* ActNewAtt		*/{1065,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_RecFormAttEvent		,NULL},
   /* ActChgAtt		*/{1066,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_RecFormAttEvent		,NULL},
   /* ActReqRemAtt	*/{1067,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_AskRemAttEvent		,NULL},
   /* ActRemAtt		*/{1068,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_GetAndRemAttEvent		,NULL},
   /* ActHidAtt		*/{1069,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_HideAttEvent		,NULL},
   /* ActShoAtt		*/{1070,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_ShowAttEvent		,NULL},
   /* ActSeeOneAtt	*/{1071,-1,TabUsr,ActSeeAtt		,0x118,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_SeeOneAttEvent		,NULL},
   /* ActRecAttStd	*/{1072,-1,TabUsr,ActSeeAtt		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_RegisterStudentsInAttEvent	,NULL},
   /* ActRecAttMe	*/{1076,-1,TabUsr,ActSeeAtt		,0x008,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Att_RegisterMeAsStdInAttEvent	,NULL},

   /* ActSignUp		*/{1056,-1,TabUsr,ActReqSignUp		,0x000,0x006,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_SignUpInCrs		,NULL},
   /* ActUpdSignUpReq	*/{1522,-1,TabUsr,ActSeeSignUpReq	,0x1F0,0x1F0,0x1F0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_UpdateEnrollmentRequests	,NULL},
   /* ActReqRejSignUp	*/{1058,-1,TabUsr,ActSeeSignUpReq	,0x1F0,0x1F0,0x1F0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AskIfRejectSignUp		,NULL},
   /* ActRejSignUp	*/{1059,-1,TabUsr,ActSeeSignUpReq	,0x1F0,0x1F0,0x1F0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RejectSignUp		,NULL},

   /* ActReqMdfOneOth	*/{1414,-1,TabUsr,ActLstOth		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqRegRemOth		,NULL},
   /* ActReqMdfOneStd	*/{1415,-1,TabUsr,ActLstStd		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqRegRemStd		,NULL},
   /* ActReqMdfOneTch	*/{1416,-1,TabUsr,ActLstTch		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqRegRemTch		,NULL},

   /* ActReqMdfOth	*/{1418,-1,TabUsr,ActLstOth		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AskIfRegRemAnotherOth	,NULL},
   /* ActReqMdfStd	*/{1419,-1,TabUsr,ActLstStd		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AskIfRegRemAnotherStd	,NULL},
   /* ActReqMdfTch	*/{1420,-1,TabUsr,ActLstTch		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AskIfRegRemAnotherTch	,NULL},

   /* ActReqOthPho	*/{1432,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_SendPhotoUsr		,NULL},
   /* ActReqStdPho	*/{1433,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_SendPhotoUsr		,NULL},
   /* ActReqTchPho	*/{1434,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_SendPhotoUsr		,NULL},

   /* ActDetOthPho	*/{1435,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   /* ActDetStdPho	*/{1436,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   /* ActDetTchPho	*/{1437,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Pho_RecOtherUsrPhotoDetFaces	,NULL},

   /* ActUpdOthPho	*/{1438,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},
   /* ActUpdStdPho	*/{1439,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},
   /* ActUpdTchPho	*/{1440,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pho_UpdateUsrPhoto1		,Pho_UpdateUsrPhoto2		,NULL},

   /* ActRemOthPho	*/{1574,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_ReqRemoveUsrPhoto		,NULL},
   /* ActRemStdPho	*/{1575,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_ReqRemoveUsrPhoto		,NULL},
   /* ActRemTchPho	*/{1576,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_ReqRemoveUsrPhoto		,NULL},
   /* ActRemOthPho	*/{1441,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_RemoveUsrPhoto		,NULL},
   /* ActRemStdPho	*/{1442,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_RemoveUsrPhoto		,NULL},
   /* ActRemTchPho	*/{1443,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_RemoveUsrPhoto		,NULL},

   /* ActCreOth		*/{1444,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_CreateNewUsr		,NULL},
   /* ActCreStd		*/{1445,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_CreateNewUsr		,NULL},
   /* ActCreTch		*/{1446,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_CreateNewUsr		,NULL},

   /* ActUpdOth		*/{1422,-1,TabUsr,ActLstOth		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ModifyUsr			,NULL},
   /* ActUpdStd		*/{1423,-1,TabUsr,ActLstStd		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ModifyUsr			,NULL},
   /* ActUpdTch		*/{1424,-1,TabUsr,ActLstTch		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ModifyUsr			,NULL},

   /* ActReqAccEnrStd	*/{1456,-1,TabUsr,ActLstStd		,0x1E8,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqAcceptRegisterInCrs	,NULL},
   /* ActReqAccEnrTch	*/{1457,-1,TabUsr,ActLstTch		,0x1F0,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqAcceptRegisterInCrs	,NULL},
   /* ActAccEnrStd	*/{1458,-1,TabUsr,ActLstStd		,0x1E8,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AcceptRegisterMeInCrs	,NULL},
   /* ActAccEnrTch	*/{1459,-1,TabUsr,ActLstTch		,0x1F0,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AcceptRegisterMeInCrs	,NULL},
   /* ActRemMe_Std	*/{1460,-1,TabUsr,ActLstStd		,0x1E8,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqRemMeFromCrs		,NULL},
   /* ActRemMe_Tch	*/{1461,-1,TabUsr,ActLstTch		,0x1F0,0x000,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReqRemMeFromCrs		,NULL},

   /* ActNewAdmIns	*/{1337,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AddAdmToIns		,NULL},
   /* ActRemAdmIns	*/{1338,-1,TabUsr,ActLstOth		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RemAdmIns			,NULL},

   /* ActNewAdmCtr	*/{1339,-1,TabUsr,ActLstOth		,0x180,0x180,0x180,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AddAdmToCtr		,NULL},
   /* ActRemAdmCtr	*/{1340,-1,TabUsr,ActLstOth		,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RemAdmCtr			,NULL},

   /* ActNewAdmDeg	*/{ 586,-1,TabUsr,ActLstOth		,0x1C0,0x1C0,0x1C0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AddAdmToDeg		,NULL},
   /* ActRemAdmDeg	*/{ 584,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RemAdmDeg			,NULL},

   /* ActRcvFrmEnrSevStd*/{1428,-1,TabUsr,ActLstStd		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReceiveFormAdminStds	,NULL},
   /* ActRcvFrmEnrSevTch*/{1429,-1,TabUsr,ActLstTch		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_ReceiveFormAdminTchs	,NULL},

   /* ActReqCnfID_Oth	*/{1565,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_RequestConfirmOtherUsrID	,NULL},
   /* ActReqCnfID_Std	*/{1566,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_RequestConfirmOtherUsrID	,NULL},
   /* ActReqCnfID_Tch	*/{1567,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_RequestConfirmOtherUsrID	,NULL},
   /* ActCnfID_Oth	*/{1568,-1,TabUsr,ActLstOth		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_ConfirmOtherUsrID		,NULL},
   /* ActCnfID_Std	*/{1569,-1,TabUsr,ActLstStd		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_ConfirmOtherUsrID		,NULL},
   /* ActCnfID_Tch	*/{1570,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_ConfirmOtherUsrID		,NULL},

   /* ActFrmIDsOth	*/{1447,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_ShowFormOthIDs		,NULL},
   /* ActFrmIDsStd	*/{1448,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_ShowFormOthIDs		,NULL},
   /* ActFrmIDsTch	*/{1449,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_ShowFormOthIDs		,NULL},
   /* ActRemID_Oth	*/{1450,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_RemoveOtherUsrID		,NULL},
   /* ActRemID_Std	*/{1451,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_RemoveOtherUsrID		,NULL},
   /* ActRemID_Tch	*/{1452,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_RemoveOtherUsrID		,NULL},
   /* ActNewID_Oth	*/{1453,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_NewOtherUsrID		,NULL},
   /* ActNewID_Std	*/{1454,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_NewOtherUsrID		,NULL},
   /* ActNewID_Tch	*/{1455,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_NewOtherUsrID		,NULL},

   /* ActFrmPwdOth	*/{1464,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pwd_ShowFormOthPwd		,NULL},
   /* ActFrmPwdStd	*/{1465,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pwd_ShowFormOthPwd		,NULL},
   /* ActFrmPwdTch	*/{1466,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pwd_ShowFormOthPwd		,NULL},
   /* ActChgPwdOth	*/{1467,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pwd_UpdateOtherPwd1		,Pwd_UpdateOtherPwd2		,NULL},
   /* ActChgPwdStd	*/{1468,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pwd_UpdateOtherPwd1		,Pwd_UpdateOtherPwd2		,NULL},
   /* ActChgPwdTch	*/{1469,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pwd_UpdateOtherPwd1		,Pwd_UpdateOtherPwd2		,NULL},

   /* ActFrmMaiOth	*/{1475,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_ShowFormOthEmail		,NULL},
   /* ActFrmMaiStd	*/{1476,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_ShowFormOthEmail		,NULL},
   /* ActFrmMaiTch	*/{1477,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_ShowFormOthEmail		,NULL},
   /* ActRemMaiOth	*/{1478,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RemoveOtherUsrEmail	,NULL},
   /* ActRemMaiStd	*/{1479,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RemoveOtherUsrEmail	,NULL},
   /* ActRemMaiTch	*/{1480,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RemoveOtherUsrEmail	,NULL},
   /* ActNewMaiOth	*/{1481,-1,TabUsr,ActLstOth		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_NewOtherUsrEmail		,NULL},
   /* ActNewMaiStd	*/{1482,-1,TabUsr,ActLstStd		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_NewOtherUsrEmail		,NULL},
   /* ActNewMaiTch	*/{1483,-1,TabUsr,ActLstTch		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_NewOtherUsrEmail		,NULL},

   /* ActRemStdCrs	*/{1462,-1,TabUsr,ActLstStd		,0x1F8,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RemUsrFromCrs		,NULL},
   /* ActRemTchCrs	*/{1463,-1,TabUsr,ActLstTch		,0x1F0,0x1E0,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RemUsrFromCrs		,NULL},

   /* ActRemUsrGbl	*/{  62,-1,TabUsr,ActLstOth		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Acc_GetUsrCodAndRemUsrGbl	,NULL},

   /* ActReqRemAllStdCrs*/{  88,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AskRemAllStdsThisCrs	,NULL},
   /* ActRemAllStdCrs	*/{  87,-1,TabUsr,ActLstStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RemAllStdsThisCrs		,NULL},

   /* ActReqRemOldUsr	*/{ 590,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_AskRemoveOldUsrs		,NULL},
   /* ActRemOldUsr	*/{ 773,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Enr_RemoveOldUsrs		,NULL},

   /* ActLstDupUsr	*/{1578,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dup_ListDuplicateUsrs		,NULL},
   /* ActLstSimUsr	*/{1579,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dup_GetUsrCodAndListSimilarUsrs,NULL},
   /* ActRemDupUsr	*/{1580,-1,TabUsr,ActLstOth		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Dup_RemoveUsrFromListDupUsrs	,NULL},

   /* ActLstClk		*/{ 989,-1,TabUsr,ActLstCon		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Con_ShowLastClicks		,NULL},

   // TabSoc ******************************************************************
   /* ActSeeSocTmlGbl	*/{1490, 0,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_ShowTimelineGbl		,"soc64x64.png"		},
   /* ActSeeSocPrf	*/{1520, 1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_SeeSocialProfiles		,"prf64x64.gif"		},
   /* ActSeeFor		*/{  95, 2,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumList		,"forum64x64.gif"	},
   /* ActSeeChtRms	*/{  51, 3,TabSoc,ActSeeChtRms		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Cht_ShowChatRooms		,"chat64x64.gif"	},

   /* ActRcvSocPstGbl	*/{1492,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_ReceiveSocialPostGbl	,NULL},
   /* ActRcvSocComGbl	*/{1503,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_ReceiveCommentGbl		,NULL},
   /* ActShaSocNotGbl	*/{1495,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_ShareSocialNoteGbl		,NULL},
   /* ActUnsSocNotGbl	*/{1496,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_UnshareSocialNoteGbl	,NULL},
   /* ActFavSocNotGbl	*/{1512,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_FavSocialNoteGbl		,NULL},
   /* ActUnfSocNotGbl	*/{1513,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_UnfavSocialNoteGbl		,NULL},
   /* ActFavSocComGbl	*/{1516,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_FavSocialCommentGbl	,NULL},
   /* ActUnfSocComGbl	*/{1517,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_UnfavSocialCommentGbl	,NULL},
   /* ActReqRemSocPubGbl*/{1494,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_RequestRemSocialNoteGbl	,NULL},
   /* ActRemSocPubGbl	*/{1493,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_RemoveSocialNoteGbl	,NULL},
   /* ActReqRemSocComGbl*/{1505,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_RequestRemSocialComGbl	,NULL},
   /* ActRemSocComGbl	*/{1507,-1,TabSoc,ActSeeSocTmlGbl	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Soc_MarkMyNotifAsSeen	,Soc_RemoveSocialComGbl		,NULL},

   /* ActReqPubPrf	*/{1401,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_RequestUserProfile		,NULL},

   /* ActRcvSocPstUsr	*/{1498,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Soc_ReceiveSocialPostUsr	,NULL},
   /* ActRcvSocComUsr	*/{1504,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Soc_ReceiveCommentUsr		,NULL},
   /* ActShaSocNotUsr	*/{1499,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_ShareSocialNoteUsr		,NULL},
   /* ActUnsSocNotUsr	*/{1500,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_UnshareSocialNoteUsr	,NULL},
   /* ActFavSocNotUsr	*/{1514,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_FavSocialNoteUsr		,NULL},
   /* ActUnfSocNotUsr	*/{1515,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_UnfavSocialNoteUsr		,NULL},
   /* ActFavSocComUsr	*/{1518,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_FavSocialCommentUsr	,NULL},
   /* ActUnfSocComUsr	*/{1519,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_UnfavSocialCommentUsr	,NULL},
   /* ActReqRemSocPubUsr*/{1501,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_RequestRemSocialNoteUsr	,NULL},
   /* ActRemSocPubUsr	*/{1502,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_RemoveSocialNoteUsr	,NULL},
   /* ActReqRemSocComGbl*/{1506,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_RequestRemSocialComUsr	,NULL},
   /* ActRemSocComGbl	*/{1508,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Soc_RemoveSocialComUsr		,NULL},

   /* ActSeePubPrf	*/{1402,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_GetUsrDatAndShowUserProfile,NULL},
   /* ActCal1stClkTim	*/{1405,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_CalculateFigures		,NULL},
   /* ActCalNumClk	*/{1406,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_CalculateFigures		,NULL},
   /* ActCalNumFilVie	*/{1409,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_CalculateFigures		,NULL},
   /* ActCalNumForPst	*/{1408,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_CalculateFigures		,NULL},
   /* ActCalNumMsgSnt	*/{1407,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_CalculateFigures		,NULL},

   /* ActFolUsr		*/{1410,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Fol_FollowUsr			,NULL},
   /* ActUnfUsr		*/{1411,-1,TabSoc,ActSeeSocPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Fol_UnfollowUsr		,NULL},
   /* ActSeeFlg		*/{1412,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Fol_ListFollowing		,NULL},
   /* ActSeeFlr		*/{1413,-1,TabSoc,ActSeeSocPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Fol_ListFollowers		,NULL},

   /* ActSeeForCrsUsr	*/{ 345,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForCrsTch	*/{ 431,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForDegUsr	*/{ 241,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForDegTch	*/{ 243,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForCtrUsr	*/{ 901,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForCtrTch	*/{ 430,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForInsUsr	*/{ 725,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForInsTch	*/{ 724,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForGenUsr	*/{ 726,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForGenTch	*/{ 723,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForSWAUsr	*/{ 242,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},
   /* ActSeeForSWATch	*/{ 245,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowForumThrs		,NULL},

   /* ActSeePstForCrsUsr*/{ 346,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForCrsTch*/{ 347,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForDegUsr*/{ 255,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForDegTch*/{ 291,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForCtrUsr*/{ 348,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForCtrTch*/{ 902,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForInsUsr*/{ 730,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForInsTch*/{ 746,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForGenUsr*/{ 727,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForGenTch*/{ 731,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForSWAUsr*/{ 244,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},
   /* ActSeePstForSWATch*/{ 246,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ShowThrPsts		,NULL},

   /* ActRcvThrForCrsUsr*/{ 350,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForCrsTch*/{ 754,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForDegUsr*/{ 252,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForDegTch*/{ 247,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForCtrUsr*/{ 903,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForCtrTch*/{ 904,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForInsUsr*/{ 737,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForInsTch*/{ 769,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForGenUsr*/{ 736,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForGenTch*/{ 765,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForSWAUsr*/{ 258,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvThrForSWATch*/{ 259,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},

   /* ActRcvRepForCrsUsr*/{ 599,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForCrsTch*/{ 755,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForDegUsr*/{ 606,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForDegTch*/{ 617,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForCtrUsr*/{ 905,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForCtrTch*/{ 906,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForInsUsr*/{ 740,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForInsTch*/{ 770,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForGenUsr*/{ 747,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForGenTch*/{ 816,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForSWAUsr*/{ 603,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},
   /* ActRcvRepForSWATch*/{ 622,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,For_RecForumPst		,NULL},

   /* ActReqDelThrCrsUsr*/{ 867,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrCrsTch*/{ 869,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrDegUsr*/{ 907,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrDegTch*/{ 908,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrCtrUsr*/{ 909,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrCtrTch*/{ 910,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrInsUsr*/{ 911,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrInsTch*/{ 912,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrGenUsr*/{ 913,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrGenTch*/{ 914,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrSWAUsr*/{ 881,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},
   /* ActReqDelThrSWATch*/{ 915,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_ReqDelThr			,NULL},

   /* ActDelThrForCrsUsr*/{ 868,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForCrsTch*/{ 876,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForDegUsr*/{ 916,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForDegTch*/{ 917,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForCtrUsr*/{ 918,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForCtrTch*/{ 919,-1,TabSoc,ActSeeFor		,0x1E4,0x1E4,0x1E4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForInsUsr*/{ 920,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForInsTch*/{ 921,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForGenUsr*/{ 922,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForGenTch*/{ 923,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForSWAUsr*/{ 882,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},
   /* ActDelThrForSWATch*/{ 924,-1,TabSoc,ActSeeFor		,0x104,0x104,0x104,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelThr			,NULL},

   /* ActCutThrForCrsUsr*/{ 926,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForCrsTch*/{ 927,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForDegUsr*/{ 928,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForDegTch*/{ 929,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForCtrUsr*/{ 930,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForCtrTch*/{ 931,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForInsUsr*/{ 932,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForInsTch*/{ 933,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForGenUsr*/{ 934,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForGenTch*/{ 935,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForSWAUsr*/{ 890,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},
   /* ActCutThrForSWATch*/{ 936,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_CutThr			,NULL},

   /* ActPasThrForCrsUsr*/{ 891,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForCrsTch*/{ 937,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForDegUsr*/{ 938,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForDegTch*/{ 939,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForCtrUsr*/{ 940,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForCtrTch*/{ 941,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForInsUsr*/{ 942,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForInsTch*/{ 943,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForGenUsr*/{ 944,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForGenTch*/{ 945,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForSWAUsr*/{ 946,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},
   /* ActPasThrForSWATch*/{ 947,-1,TabSoc,ActSeeFor		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_PasteThr			,NULL},

   /* ActDelPstForCrsUsr*/{ 602,-1,TabSoc,ActSeeFor		,0x11C,0x11C,0x11C,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForCrsTch*/{ 756,-1,TabSoc,ActSeeFor		,0x114,0x114,0x114,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForDegUsr*/{ 608,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForDegTch*/{ 680,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForCtrUsr*/{ 948,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForCtrTch*/{ 949,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForInsUsr*/{ 743,-1,TabSoc,ActSeeFor		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForInsTch*/{ 772,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForGenUsr*/{ 735,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForGenTch*/{ 950,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForSWAUsr*/{ 613,-1,TabSoc,ActSeeFor		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},
   /* ActDelPstForSWATch*/{ 623,-1,TabSoc,ActSeeFor		,0x1F4,0x1F4,0x1F4,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DelPst			,NULL},

   /* ActEnbPstForCrsUsr*/{ 624,-1,TabSoc,ActSeeFor		,0x110,0x110,0x110,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForCrsTch*/{ 951,-1,TabSoc,ActSeeFor		,0x110,0x110,0x110,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForDegUsr*/{ 616,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForDegTch*/{ 619,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForCtrUsr*/{ 952,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForCtrTch*/{ 953,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForInsUsr*/{ 954,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForInsTch*/{ 955,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForGenUsr*/{ 956,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForGenTch*/{ 957,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForSWAUsr*/{ 632,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},
   /* ActEnbPstForSWATch*/{ 634,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_EnbPst			,NULL},

   /* ActDisPstForCrsUsr*/{ 610,-1,TabSoc,ActSeeFor		,0x110,0x110,0x110,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForCrsTch*/{ 958,-1,TabSoc,ActSeeFor		,0x110,0x110,0x110,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForDegUsr*/{ 615,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForDegTch*/{ 618,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForCtrUsr*/{ 959,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForCtrTch*/{ 960,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForInsUsr*/{ 961,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForInsTch*/{ 962,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForGenUsr*/{ 963,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForGenTch*/{ 925,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForSWAUsr*/{ 625,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},
   /* ActDisPstForSWATch*/{ 635,-1,TabSoc,ActSeeFor		,0x1E0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,For_DisPst			,NULL},

   /* ActCht		*/{  52,-1,TabSoc,ActSeeChtRms		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_NEW_WINDOW ,Cht_OpenChatWindow		,NULL				,NULL},

   // TabMsg ******************************************************************
   // Actions in menu:
   /* ActSeeNtf         */{ 990, 0,TabMsg,ActSeeNtf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ntf_ShowMyNotifications	,"bell64x64.gif"	},
   /* ActSeeAnn		*/{1235, 1,TabMsg,ActSeeAnn		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ann_ShowAllAnnouncements	,"note64x64.gif"	},
   /* ActSeeAllNot	*/{ 762, 2,TabMsg,ActSeeAllNot		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Not_ListFullNotices		,"note64x64.gif"	},
   /* ActReqMsgUsr	*/{  26, 3,TabMsg,ActReqMsgUsr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_FormMsgUsrs		,"editnewmsg64x64.gif"	},
   /* ActSeeRcvMsg	*/{   3, 4,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ShowRecMsgs		,"recmsg64x64.gif"	},
   /* ActSeeSntMsg	*/{  70, 5,TabMsg,ActSeeSntMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ShowSntMsgs		,"sntmsg64x64.gif"	},
   /* ActMaiStd		*/{ 100, 6,TabMsg,ActMaiStd		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_ListEMails			,"email64x64.gif"	},

   // Actions not in menu:
   /* ActWriAnn		*/{1237,-1,TabMsg,ActSeeAnn		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ann_ShowFormAnnouncement	,NULL},
   /* ActRcvAnn		*/{1238,-1,TabMsg,ActSeeAnn		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ann_ReceiveAnnouncement	,NULL},
   /* ActHidAnn		*/{1470,-1,TabMsg,ActSeeAnn		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ann_HideActiveAnnouncement	,Ann_ShowAllAnnouncements	,NULL},
   /* ActRevAnn		*/{1471,-1,TabMsg,ActSeeAnn		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ann_RevealHiddenAnnouncement,Ann_ShowAllAnnouncements	,NULL},
   /* ActRemAnn		*/{1236,-1,TabMsg,ActSeeAnn		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ann_RemoveAnnouncement		,NULL},

   /* ActSeeOneNot	*/{1164,-1,TabMsg,ActSeeAllNot		,0x1FF,0x1FF,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Not_GetNotCodToHighlight	,Not_ListFullNotices		,NULL},
   /* ActWriNot		*/{  59,-1,TabMsg,ActSeeAllNot		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Not_ShowFormNotice		,NULL},
   /* ActRcvNot		*/{  60,-1,TabMsg,ActSeeAllNot		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Not_ReceiveNotice		,Not_ListFullNotices		,NULL},
   /* ActHidNot		*/{ 763,-1,TabMsg,ActSeeAllNot		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Not_HideActiveNotice	,Not_ListFullNotices		,NULL},
   /* ActRevNot		*/{ 764,-1,TabMsg,ActSeeAllNot		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Not_RevealHiddenNotice	,Not_ListFullNotices		,NULL},
   /* ActReqRemNot	*/{1472,-1,TabMsg,ActSeeAllNot		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Not_RequestRemNotice		,NULL},
   /* ActRemNot		*/{  73,-1,TabMsg,ActSeeAllNot		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Not_RemoveNotice		,Not_ListNoticesAfterRemoval	,NULL},

   /* ActSeeNewNtf	*/{ 991,-1,TabMsg,ActSeeNtf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ntf_ShowMyNotifications	,NULL},
   /* ActMrkNtfSee	*/{1146,-1,TabMsg,ActSeeNtf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ntf_MarkAllNotifAsSeen		,NULL},

   /* ActRcvMsgUsr	*/{  27,-1,TabMsg,ActReqMsgUsr		,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Msg_RecMsgFromUsr		,NULL},
   /* ActReqDelAllSntMsg*/{ 604,-1,TabMsg,ActSeeSntMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ReqDelAllSntMsgs		,NULL},
   /* ActReqDelAllRcvMsg*/{ 593,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ReqDelAllRecMsgs		,NULL},
   /* ActDelAllSntMsg	*/{ 434,-1,TabMsg,ActSeeSntMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_DelAllSntMsgs		,NULL},
   /* ActDelAllRcvMsg	*/{ 436,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_DelAllRecMsgs		,NULL},
   /* ActDelSntMsg	*/{  90,-1,TabMsg,ActSeeSntMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_DelSntMsg			,NULL},
   /* ActDelRcvMsg	*/{  64,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_DelRecMsg			,NULL},
   /* ActExpSntMsg	*/{ 664,-1,TabMsg,ActSeeSntMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ExpSntMsg			,NULL},
   /* ActExpRcvMsg	*/{ 663,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ExpRecMsg			,NULL},
   /* ActConSntMsg	*/{1020,-1,TabMsg,ActSeeSntMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ConSntMsg			,NULL},
   /* ActConRcvMsg	*/{1019,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ConRecMsg			,NULL},

   /* ActLstBanUsr	*/{1017,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_ListBannedUsrs		,NULL},
   /* ActBanUsrMsg	*/{1015,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_BanSenderWhenShowingMsgs	,NULL},
   /* ActUnbUsrMsg	*/{1016,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_UnbanSenderWhenShowingMsgs	,NULL},
   /* ActUnbUsrLst	*/{1018,-1,TabMsg,ActSeeRcvMsg		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Msg_UnbanSenderWhenListingUsrs	,NULL},

   // TabSta ******************************************************************
   // Actions in menu:
   /* ActSeeAllSvy	*/{ 966, 0,TabSta,ActSeeAllSvy		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_SeeAllSurveys		,"survey64x64.gif"	},
   /* ActReqUseGbl	*/{ 761, 1,TabSta,ActReqUseGbl		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sta_ReqUseOfPlatform		,"pie64x64.gif"		},
   /* ActSeePhoDeg	*/{ 447, 2,TabSta,ActSeePhoDeg		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_ShowPhotoDegree		,"classphoto64x64.gif"	},
   /* ActReqStaCrs	*/{ 767, 3,TabSta,ActReqStaCrs		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ind_ReqIndicatorsCourses	,"tablestats64x64.gif"	},
   /* ActReqAccGbl	*/{ 591, 4,TabSta,ActReqAccGbl		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sta_SetIniEndDates		,Sta_AskShowGblHits		,"stats64x64.gif"	},
   /* ActReqMyUsgRep	*/{1586, 5,TabSta,ActReqMyUsgRep	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rep_ReqMyUsageReport		,"report64x64.png"	},

   // Actions not in menu:
   /* ActSeeOneSvy	*/{ 982,-1,TabSta,ActSeeAllSvy		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_SeeOneSurvey		,NULL},
   /* ActAnsSvy		*/{ 983,-1,TabSta,ActSeeAllSvy		,0x1F8,0x1F8,0x1F8,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_ReceiveSurveyAnswers	,NULL},
   /* ActFrmNewSvy	*/{ 973,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RequestCreatOrEditSvy	,NULL},
   /* ActEdiOneSvy	*/{ 974,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RequestCreatOrEditSvy	,NULL},
   /* ActNewSvy		*/{ 968,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RecFormSurvey		,NULL},
   /* ActChgSvy		*/{ 975,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RecFormSurvey		,NULL},
   /* ActReqRemSvy	*/{ 976,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_AskRemSurvey		,NULL},
   /* ActRemSvy		*/{ 969,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RemoveSurvey		,NULL},

   /* ActReqRstSvy	*/{ 984,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_AskResetSurvey		,NULL},
   /* ActRstSvy		*/{ 985,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_ResetSurvey		,NULL},

   /* ActHidSvy		*/{ 977,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_HideSurvey			,NULL},
   /* ActShoSvy		*/{ 978,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_UnhideSurvey		,NULL},
   /* ActEdiOneSvyQst	*/{ 979,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RequestEditQuestion	,NULL},
   /* ActRcvSvyQst	*/{ 980,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_ReceiveQst			,NULL},
   /* ActReqRemSvyQst	*/{1524,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RequestRemoveQst		,NULL},
   /* ActRemSvyQst	*/{ 981,-1,TabSta,ActSeeAllSvy		,0x1F0,0x1E0,0x1E0,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Svy_RemoveQst			,NULL},

   /* ActSeeUseGbl	*/{  84,-1,TabSta,ActReqUseGbl		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sta_ShowUseOfPlatform		,NULL},
   /* ActPrnPhoDeg	*/{ 448,-1,TabSta,ActSeePhoDeg		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Pho_PrintPhotoDegree  		,NULL},
   /* ActCalPhoDeg	*/{ 444,-1,TabSta,ActSeePhoDeg		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_CalcPhotoDegree		,NULL},
   /* ActSeeAccGbl	*/{  79,-1,TabSta,ActReqAccGbl		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sta_SeeGblAccesses		,NULL},
   /* ActReqAccCrs	*/{ 594,-1,TabSta,ActReqAccGbl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,Sta_SetIniEndDates		,Sta_AskShowCrsHits		,NULL},
   /* ActSeeAccCrs	*/{ 119,-1,TabSta,ActReqAccGbl		,0x110,0x100,0x000,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Sta_SeeCrsAccesses		,NULL},
   /* ActSeeAllStaCrs	*/{ 768,-1,TabSta,ActReqAccGbl		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,Ind_ShowIndicatorsCourses	,NULL},

   /* ActSeeMyUsgRep	*/{1582,-1,TabSta,ActReqMyUsgRep	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rep_ShowMyUsageReport		,NULL},

   // TabPrf ******************************************************************
   // Actions in menu:
   /* ActFrmLogIn	*/{1521, 0,TabPrf,ActFrmLogIn		,0x001,0x001,0x001,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_WriteLandingPage		,"keyuser64x64.gif"	},
   /* ActFrmRolSes	*/{ 843, 1,TabPrf,ActFrmRolSes		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_WriteFormLogout		,"keyuser64x64.gif"	},
   /* ActMyCrs		*/{ 987, 2,TabPrf,ActMyCrs		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Crs_ReqSelectOneOfMyCourses	,"mygroups64x64.gif"	},
   /* ActSeeMyTT	*/{ 408, 3,TabPrf,ActSeeMyTT		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,TT_ShowClassTimeTable		,"clock64x64.gif"	},
   /* ActSeeMyAgd	*/{1581, 4,TabPrf,ActSeeMyAgd		,0x100,0x100,0x100,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Agd_ShowMyAgenda		,"date64x64.gif"	},
   /* ActFrmMyAcc	*/{  36, 5,TabPrf,ActFrmMyAcc		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Acc_ShowFormMyAccount		,"arroba64x64.gif"	},
   /* ActReqEdiRecCom	*/{ 285, 6,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ShowFormMyCommRecord	,"card64x64.gif"	},
   /* ActEdiPrf		*/{ 673, 7,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pre_EditPrefs			,"heart64x64.gif"	},
   /* ActAdmBrf		*/{  23, 8,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileBrowserOrWorks	,"pendrive64x64.gif"	},
   /* ActMFUAct		*/{ 993, 9,TabPrf,ActMFUAct		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,MFU_ShowMyMFUActions		,"star64x64.gif"	},

   // Actions not in menu:
   /* ActReqSndNewPwd	*/{ 665,-1,TabPrf,ActFrmLogIn		,0x000,0x001,0x001,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pwd_ShowFormSendNewPwd		,NULL},
   /* ActSndNewPwd	*/{ 633,-1,TabPrf,ActFrmLogIn		,0x000,0x001,0x001,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pwd_ChkIdLoginAndSendNewPwd	,NULL},
   /* ActLogOut		*/{  10,-1,TabPrf,ActFrmLogIn		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ses_CloseSession		,Usr_Logout			,NULL},

   /* ActAutUsrInt	*/{   6,-1,TabPrf,ActFrmRolSes		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_WelcomeUsr			,NULL},
   /* ActAutUsrNew	*/{1585,-1,TabPrf,ActFrmRolSes		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_WelcomeUsr			,NULL},
   /* ActAutUsrChgLan	*/{1077,-1,TabPrf,ActFrmRolSes		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Usr_WelcomeUsr			,NULL},
   /* ActAnnSee		*/{1234,-1,TabPrf,ActFrmRolSes		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Ann_MarkAnnouncementAsSeen	,NULL},
   /* ActChgMyRol	*/{ 589,-1,TabPrf,ActFrmRolSes		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Rol_ChangeMyRole		,Usr_ShowFormsLogoutAndRole	,NULL},

   /* ActChkUsrAcc	*/{1584,-1,TabPrf,ActFrmMyAcc		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Acc_CheckIfEmptyAccountExists	,NULL},
   /* ActCreUsrAcc	*/{1163,-1,TabPrf,ActFrmMyAcc		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Acc_AfterCreationNewAccount	,NULL},

   /* ActRemID_Me	*/{1147,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_RemoveMyUsrID		,NULL},
   /* ActNewIDMe	*/{1148,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ID_NewMyUsrID			,NULL},

   /* ActRemOldNic	*/{1089,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Nck_RemoveNick			,NULL},
   /* ActChgNic		*/{  37,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Nck_UpdateNick			,NULL},

   /* ActRemMaiMe	*/{1090,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_RemoveMyUsrEmail		,NULL},
   /* ActNewMaiMe	*/{1088,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,May_NewMyUsrEmail		,NULL},
   /* ActCnfMai		*/{1091,-1,TabPrf,ActFrmMyAcc		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Mai_ConfirmEmail		,NULL},

   /* ActFrmChgMyPwd	*/{  34,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pwd_ShowFormChgPwd		,NULL},
   /* ActChgPwd		*/{  35,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pwd_ActChgMyPwd1		,Pwd_ActChgMyPwd2		,NULL},

   /* ActReqRemMyAcc	*/{1430,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Acc_AskIfRemoveMyAccount	,NULL},
   /* ActRemMyAcc	*/{1431,-1,TabPrf,ActFrmMyAcc		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Acc_RemoveMyAccount		,NULL},

   /* ActChgMyData	*/{ 298,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Rec_UpdateMyRecord		,Rec_ShowMyCommonRecordUpd	,NULL},

   /* ActReqMyPho	*/{  30,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_ReqMyPhotoWithContextLinks	,NULL},
   /* ActDetMyPho	*/{ 693,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Pho_RecMyPhotoDetFaces		,NULL},
   /* ActUpdMyPho	*/{ 694,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pho_UpdateMyPhoto1		,Pho_UpdateMyPhoto2		,NULL},
   /* ActReqRemMyPho	*/{1577,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_ReqRemoveMyPhoto		,NULL},
   /* ActRemMyPho	*/{ 428,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pho_RemoveMyPhoto1		,Pho_RemoveMyPhoto2		,NULL},

   /* ActEdiPri		*/{1403,-1,TabPrf,ActEdiPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pri_EditMyPrivacy		,NULL},
   /* ActChgPriPho	*/{ 774,-1,TabPrf,ActEdiPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pho_ChangePhotoVisibility	,NULL},
   /* ActChgPriPrf	*/{1404,-1,TabPrf,ActEdiPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Prf_ChangeProfileVisibility	,NULL},

   /* ActReqEdiMyIns	*/{1165,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ShowFormMyInsCtrDpt	,NULL},
   /* ActChgCtyMyIns	*/{1166,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_ChgCountryOfMyInstitution	,NULL},
   /* ActChgMyIns	*/{1167,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_UpdateMyInstitution	,NULL},
   /* ActChgMyCtr	*/{1168,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_UpdateMyCentre		,NULL},
   /* ActChgMyDpt	*/{1169,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_UpdateMyDepartment		,NULL},
   /* ActChgMyOff	*/{1170,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_UpdateMyOffice		,NULL},
   /* ActChgMyOffPho	*/{1171,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Rec_UpdateMyOfficePhone	,NULL},

   /* ActReqEdiMyNet	*/{1172,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Net_ShowFormMyWebsAndSocialNets,NULL},
   /* ActChgMyNet	*/{1173,-1,TabPrf,ActReqEdiRecCom	,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Net_UpdateMyWebsAndSocialNets	,NULL},

   /* ActChgThe		*/{ 841,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,The_ChangeTheme		,Pre_EditPrefs			,NULL},
   /* ActReqChgLan	*/{ 992,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Pre_AskChangeLanguage		,NULL},
   /* ActChgLan		*/{ 654,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pre_ChangeLanguage		,Pre_EditPrefs			,NULL},
   /* ActChg1stDay	*/{1484,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Cal_ChangeFirstDayOfWeek	,Pre_EditPrefs			,NULL},
   /* ActChgCol		*/{ 674,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pre_ChangeSideCols		,Pre_EditPrefs			,NULL},
   /* ActHidLftCol	*/{ 668,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pre_HideLeftCol		,Pre_EditPrefs			,NULL},
   /* ActHidRgtCol	*/{ 669,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pre_HideRightCol		,Pre_EditPrefs			,NULL},
   /* ActShoLftCol	*/{ 670,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pre_ShowLeftCol		,Pre_EditPrefs			,NULL},
   /* ActShoRgtCol	*/{ 671,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Pre_ShowRightCol		,Pre_EditPrefs			,NULL},
   /* ActChgIco		*/{1092,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ico_ChangeIconSet		,Pre_EditPrefs			,NULL},
   /* ActChgMnu		*/{1243,-1,TabPrf,ActEdiPrf		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_MAIN_WINDOW,Mnu_ChangeMenu		,Pre_EditPrefs			,NULL},
   /* ActChgNtfPrf	*/{ 775,-1,TabPrf,ActEdiPrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Ntf_ChangeNotifyEvents	,Pre_EditPrefs			,NULL},

   /* ActPrnUsrQR	*/{1022,-1,TabPrf,ActFrmMyAcc		,0x1FF,0x1FF,0x1FF,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,QR_PrintQRCode			,NULL},

   /* ActPrnMyTT	*/{ 409,-1,TabPrf,ActSeeMyTT		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_NEW_WINDOW ,NULL			,TT_ShowClassTimeTable		,NULL},
   /* ActEdiTut		*/{  65,-1,TabPrf,ActSeeMyTT		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,TT_ShowMyTutTimeTable		,NULL},
   /* ActChgTut		*/{  48,-1,TabPrf,ActSeeMyTT		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,TT_ShowMyTutTimeTable		,NULL},
   /* ActChgMyTT1stDay	*/{1487,-1,TabPrf,ActSeeMyTT		,0x1FF,0x1FF,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,Cal_ChangeFirstDayOfWeek	,TT_ShowClassTimeTable		,NULL},

   /* ActReqRemFilBrf	*/{ 286,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemFileFromTree		,NULL},
   /* ActRemFilBrf	*/{ 155,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFileFromTree		,NULL},
   /* ActRemFolBrf	*/{ 196,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemFolderFromTree		,NULL},
   /* ActCopBrf		*/{ 311,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_CopyFromFileBrowser	,NULL},
   /* ActPasBrf		*/{ 315,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_PasteIntoFileBrowser	,NULL},
   /* ActRemTreBrf	*/{ 276,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemSubtreeInFileBrowser	,NULL},
   /* ActFrmCreBrf	*/{ 597,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFormFileBrowser	,NULL},
   /* ActCreFolBrf	*/{ 170,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecFolderFileBrowser	,NULL},
   /* ActCreLnkBrf	*/{1230,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RecLinkFileBrowser		,NULL},
   /* ActRenFolBrf	*/{ 197,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RenFolderFileBrowser	,NULL},
   /* ActRcvFilBrfDZ	*/{1224,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone,NULL				,NULL},
   /* ActRcvFilBrfCla	*/{ 153,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_DATA,Act_MAIN_WINDOW,NULL			,Brw_RcvFileInFileBrwClassic	,NULL},
   /* ActExpBrf		*/{ 410,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ExpandFileTree		,NULL},
   /* ActConBrf		*/{ 411,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ContractFileTree		,NULL},
   /* ActZIPBrf		*/{1136,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,ZIP_CompressFileTree		,NULL},
   /* ActReqDatBrf	*/{1047,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ShowFileMetadata		,NULL},
   /* ActChgDatBrf	*/{1048,-1,TabPrf,ActAdmBrf		,0x1FC,0x1FC,0x1FC,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_ChgFileMetadata		,NULL},
   /* ActDowBrf         */{1123,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   /* ActReqRemOldBrf   */{1488,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_AskRemoveOldFiles		,NULL},
   /* ActRemOldBrf      */{1489,-1,TabPrf,ActAdmBrf		,0x1FE,0x1FE,0x1FE,Act_CONTENT_NORM,Act_MAIN_WINDOW,NULL			,Brw_RemoveOldFilesBriefcase	,NULL},
  };

Act_Action_t Act_FromActCodToAction[1+Act_MAX_ACTION_COD] =	// Do not reuse unique action codes!
	{
	ActSeeAdmDocCrsGrp,	//  #0
	-1,			//  #1 (obsolete action)
	ActMnu,			//  #2
	ActSeeRcvMsg,		//  #3
	-1,			//  #4 (obsolete action)
	-1,			//  #5 (obsolete action)
	ActAutUsrInt,		//  #6
	-1,			//  #7 (obsolete action)
	-1,			//  #8 (obsolete action)
	ActSeeCrsLnk,		//  #9
	ActLogOut,		// #10
	-1,			// #11 (obsolete action)
	ActAdmDocCrs,		// #12
	-1,			// #13 (obsolete action)
	-1,			// #14 (obsolete action)
	ActSeeAss,		// #15
	ActSeeCal,		// #16
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
	ActPrnCal,		// #71
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
	ActAutUsrChgLan,	// #1077
	ActSeeDocCrs,		// #1078
	ActSeeMrkCrs,		// #1079
	ActReqSeeUsrTstExa,	// #1080
	ActSeeUsrTstExa,	// #1081
	ActSeeOneTstExaOth,	// #1082
	ActReqSeeMyTstExa,	// #1083
	ActSeeMyTstExa,		// #1084
	ActSeeOneTstExaMe,	// #1085
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
	ActReqPubPrf,		// #1401
	ActSeePubPrf,		// #1402
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
	ActChgCal1stDay,	// #1485
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
	ActReqCnfID_Oth,	// #1565
	ActReqCnfID_Std,	// #1566
	ActReqCnfID_Tch,	// #1567
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
	ActSeeMyAgd,		// #1581
	ActSeeMyUsgRep,		// #1582
	-1,			// #1583 (obsolete action)
	ActChkUsrAcc,		// #1584
	ActAutUsrNew,		// #1585
	ActReqMyUsgRep,		// #1586
	ActChgCrsDegCfg,	// #1587
	ActChgDegCtrCfg,	// #1588
	ActChgCtrInsCfg,	// #1589
	ActChgInsCtyCfg,	// #1590
	};

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Act_FormStartInternal (Act_Action_t NextAction,bool PutParameterLocationIfNoSesion,
                                   const char *Id,const char *Anchor,const char *OnSubmit);

/*****************************************************************************/
/************* Check if I have permission to execute an action ***************/
/*****************************************************************************/

bool Act_CheckIfIHavePermissionToExecuteAction (Act_Action_t Action)
  {
   if (Action < 0 || Action >= Act_NUM_ACTIONS)
      return false;

   return ((Gbl.CurrentCrs.Crs.CrsCod >= 0 && (( Gbl.Usrs.Me.IBelongToCurrentCrs && (Act_Actions[Action].PermisIfIBelongToCrs     & (1 << Gbl.Usrs.Me.LoggedRole)))   ||
                                               (!Gbl.Usrs.Me.IBelongToCurrentCrs && (Act_Actions[Action].PermisIfIDontBelongToCrs & (1 << Gbl.Usrs.Me.LoggedRole))))) ||
           (Gbl.CurrentCrs.Crs.CrsCod  < 0 &&                                       (Act_Actions[Action].PermisIfNoCrsSelected    & (1 << Gbl.Usrs.Me.LoggedRole))));
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

char *Act_GetActionTextFromDB (long ActCod,char *Txt)
  {
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get test for an action from database *****/
   sprintf (Query,"SELECT Txt FROM actions"
	          " WHERE ActCod='%ld' AND Language='%s'",
            ActCod,Txt_STR_LANG_ID[Txt_LANGUAGE_ES]);	// !!! TODO: Replace Txt_LANGUAGE_ES by Gbl.Prefs.Language !!!
   if (DB_QuerySELECT (Query,&mysql_res,"can not get text for an action"))
     {
      /***** Get text *****/
      row = mysql_fetch_row (mysql_res);
      strncpy (Txt,row[0],Act_MAX_LENGTH_ACTION_TXT);
      Txt[Act_MAX_LENGTH_ACTION_TXT] = '\0';
     }
   else	// ActCod-Language not found on database
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Txt;
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
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   sprintf (Gbl.Form.Id,"form_%d",Gbl.Form.Num);
   Act_FormStartInternal (NextAction,true,Gbl.Form.Id,NULL,NULL);	// Do put now parameter location (if no open session)
  }

void Act_FormStartOnSubmit (Act_Action_t NextAction,const char *OnSubmit)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   sprintf (Gbl.Form.Id,"form_%d",Gbl.Form.Num);
   Act_FormStartInternal (NextAction,true,Gbl.Form.Id,NULL,OnSubmit);	// Do put now parameter location (if no open session)
  }

void Act_FormStartUnique (Act_Action_t NextAction)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   sprintf (Gbl.Form.UniqueId,"form_%s_%d",
            Gbl.UniqueNameEncrypted,Gbl.Form.Num);
   Act_FormStartInternal (NextAction,true,Gbl.Form.UniqueId,NULL,NULL);	// Do put now parameter location (if no open session)
  }

void Act_FormStartAnchor (Act_Action_t NextAction,const char *Anchor)
  {
   Gbl.Form.Num++; // Initialized to -1. The first time it is incremented, it will be equal to 0
   sprintf (Gbl.Form.Id,"form_%d",Gbl.Form.Num);
   Act_FormStartInternal (NextAction,true,Gbl.Form.Id,Anchor,NULL);	// Do put now parameter location (if no open session)
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
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char ParamsStr[256+256+Ses_LENGTH_SESSION_ID+256];

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
	 case Act_NEW_WINDOW:
	 case Act_DOWNLD_FILE:
	    fprintf (Gbl.F.Out," target=\"_blank\"");
	    break;
	 default:
	    break;
	}
      if (Act_Actions[NextAction].ContentType == Act_CONTENT_DATA)
	 fprintf (Gbl.F.Out," enctype=\"multipart/form-data\"");
      fprintf (Gbl.F.Out," accept-charset=\"windows-1252\">");

      /* Put basic form parameters */
      Act_SetParamsForm (ParamsStr,NextAction,PutParameterLocationIfNoSesion);
      fprintf (Gbl.F.Out,"%s",ParamsStr);

      Gbl.Form.Inside = true;
     }
  }

// Params should have space for 256+256+Ses_LENGTH_SESSION_ID+256 bytes
void Act_SetParamsForm (char *ParamsStr,Act_Action_t NextAction,bool PutParameterLocationIfNoSesion)
  {
   char ParamAction[256];
   char ParamSession[256+Ses_LENGTH_SESSION_ID];
   char ParamLocation[256];

   ParamAction[0] = '\0';
   ParamSession[0] = '\0';
   ParamLocation[0] = '\0';

   if (NextAction != ActUnk)
      sprintf (ParamAction,"<input type=\"hidden\" name=\"act\" value=\"%ld\" />",
	       Act_Actions[NextAction].ActCod);

   if (Gbl.Session.Id[0])
      sprintf (ParamSession,"<input type=\"hidden\" name=\"ses\" value=\"%s\" />",
	       Gbl.Session.Id);
   else if (PutParameterLocationIfNoSesion)	// Extra parameters necessary when there's no open session
     {
      /* If session is open, course code will be get from session data,
	 but if there is not an open session, and next action is known, it is necessary to send a parameter with course code */
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)		// If course selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"crs\" value=\"%ld\" />",
                  Gbl.CurrentCrs.Crs.CrsCod);
      else if (Gbl.CurrentDeg.Deg.DegCod > 0)		// If no course selected, but degree selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"deg\" value=\"%ld\" />",
                  Gbl.CurrentDeg.Deg.DegCod);
      else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// If no degree selected, but centre selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"ctr\" value=\"%ld\" />",
                  Gbl.CurrentCtr.Ctr.CtrCod);
      else if (Gbl.CurrentIns.Ins.InsCod > 0)		// If no centre selected, but institution selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"ins\" value=\"%ld\" />",
                  Gbl.CurrentIns.Ins.InsCod);
      else if (Gbl.CurrentCty.Cty.CtyCod > 0)		// If no institution selected, but country selected...
         sprintf (ParamLocation,"<input type=\"hidden\" name=\"cty\" value=\"%ld\" />",
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

void Act_SetUniqueId (char UniqueId[Act_MAX_LENGTH_ID])
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
   bool IAmATeacher = (Gbl.Usrs.Me.UsrDat.Roles & (1 << Rol_TEACHER));

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
   if (Gbl.Action.Act == ActAutUsrInt)
      if (!Pwd_FastCheckIfPasswordSeemsGood (Gbl.Usrs.Me.LoginPlainPassword))
        {
         Gbl.Action.Act = ActFrmChgMyPwd;
         Tab_SetCurrentTab ();
         return;
        }

   /***** If I have no nickname or e-mail in database,
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
      /***** If I have not accepted the enrollment,
             the only action possible is show a form to ask for enrollment *****/
      if (!Gbl.Usrs.Me.UsrDat.Accepted && Gbl.Action.Act != ActLogOut)
	{
	 Gbl.Action.Act = (Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT) ? ActReqAccEnrStd :
	                                                                           ActReqAccEnrTch;
	 Tab_SetCurrentTab ();
	 return;
	}

      /***** Depending on the role I am logged... *****/
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_STUDENT:
            switch (Gbl.Action.Act)
              {
               case ActAutUsrInt:
               case ActAutUsrNew:
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
                  /* If there are some group types with mandatory enrollment and groups in this course,
	             and I don't belong to any of those groups,
	             the the only action possible is show a form to register in groups */
                  if (Grp_NumGrpTypesMandatIDontBelong ())	// To do: if this query is too slow ==> put it only when login.
                    {
	             Gbl.Action.Act = ActReqSelGrp;
	             Tab_SetCurrentTab ();
                     return;
	            }

                  /* If I have no photo, and current action is not available for unknown users,
                     then update number of clicks without photo */
                  if (!Gbl.Usrs.Me.MyPhotoExists)
                     if (!(Act_Actions[Gbl.Action.Act].PermisIfIBelongToCrs & (1 << Rol_UNKNOWN)) &&	// If current action is not available for unknown users...
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
         case Rol_TEACHER:
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
