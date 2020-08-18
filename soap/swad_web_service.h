// File: swad_web_service.h
//gsoap swad service name: swad
//gsoap swad service namespace: urn:swad
//gsoap swad service location: https://localhost/

/*****************************************************************************/
/******* Data structures used to return data in web service functions ********/
/*****************************************************************************/

/* createAccount */
struct swad__createAccountOutput
  {
   int userCode;
   char *wsKey;		// key used in subsequent calls to other web services
  };

/* loginBySessionKey */
struct swad__loginBySessionKeyOutput
  {
   int userCode;
   int degreeTypeCode;
   int degreeCode;
   int courseCode;
   char *wsKey;		// key used in subsequent calls to other web services
   char *userNickname;
   char *userID;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
   char *userBirthday;
   int userRole;	// 1 = guest, no courses; 2 = student in all courses; 3 = teacher in one course at least
   char *degreeTypeName;
   char *degreeName;
   char *courseName;
  };

/* loginByUserPasswordKey */
struct swad__loginByUserPasswordKeyOutput
  {
   int userCode;
   char *wsKey;		// key used in subsequent calls to other web services
   char *userNickname;
   char *userID;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
   char *userBirthday;
   int userRole;	// 1 = guest, no courses; 2 = student in all courses; 3 = teacher in one course at least
  };

/* getNewPassword */
struct swad__getNewPasswordOutput
  {
   int success;
  };

/* getCourses */
struct swad__course
  {
   int courseCode;
   char *courseShortName;
   char *courseFullName;
   int userRole;	// 2 = student, 3 = teacher
  };
struct swad__coursesArray
  {
   struct swad__course *__ptr;	// pointer to array
   int __size; 			// number of elements pointed to 
  };
struct swad__getCoursesOutput
  {
   int numCourses;
   struct swad__coursesArray coursesArray;
  };

/* getCourseInfo */
struct swad__getCourseInfoOutput
  {
   char *infoSrc;
   char *infoTxt;
  };

/* getGroupTypes */
struct swad__groupType
  {
   int groupTypeCode;
   char *groupTypeName;
   int mandatory;
   int multiple;
   long openTime;
  };
struct swad__groupTypesArray
  {
   struct swad__groupType *__ptr;	// pointer to array
   int __size; 				// number of elements pointed to
  };
struct swad__getGroupTypesOutput
  {
   int numGroupTypes;
   struct swad__groupTypesArray groupTypesArray;
  };

/* structs used in getGroups and sendMyGroups */
struct swad__group
  {
   int groupCode;
   char *groupName;
   int groupTypeCode;
   char *groupTypeName;
   int open;
   int maxStudents;
   int numStudents;
   int fileZones;
   int member;
  };
struct swad__groupsArray
  {
   struct swad__group *__ptr;	// pointer to array
   int __size; 			// number of elements pointed to 
  };

/* getGroups */
struct swad__getGroupsOutput
  {
   int numGroups;
   struct swad__groupsArray groupsArray;
  };

/* sendMyGroups */
struct swad__sendMyGroupsOutput
  {
   int success;
   int numGroups;
   struct swad__groupsArray groupsArray;
  };

/* getNotifications */
struct swad__notification
  {
   int notifCode;
   char *eventType;
   int eventCode;
   long eventTime;
   char *userNickname;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
   char *location;
   int status;
   char *summary;
   char *content;
  };
struct swad__notificationsArray
  {
   struct swad__notification *__ptr;	// pointer to array
   int __size; 				// number of elements pointed to 
  };
struct swad__getNotificationsOutput
  {
   int numNotifications;
   struct swad__notificationsArray notificationsArray;
  };

/* markNotificationsAsRead */
struct swad__markNotificationsAsReadOutput
  {
   int numNotifications;
  };

/* getTestConfig */
struct swad__getTestConfigOutput
  {
   int pluggable;
   int numQuestions;
   int minQuestions;
   int defQuestions;
   int maxQuestions;
   char *feedback;
   int visibility;
  };

/* getTests */
struct swad__tag
  {
   int tagCode;
   char *tagText;
  };
struct swad__tagsArray
  {
   struct swad__tag *__ptr;		// pointer to array
   int __size; 				// number of elements pointed to 
  };
struct swad__testQuestion
  {
   int questionCode;
   char *answerType;
   int shuffle;
   char *stem;
   char *feedback;
  };
struct swad__testQuestionsArray
  {
   struct swad__testQuestion *__ptr;	// pointer to array
   int __size; 				// number of elements pointed to 
  };
struct swad__testAnswer
  {
   int questionCode;
   int answerIndex;
   int correct;
   char *answerText;
   char *answerFeedback;
  };
struct swad__testAnswersArray
  {
   struct swad__testAnswer *__ptr;		// pointer to array
   int __size; 				// number of elements pointed to 
  };
struct swad__questionTag
  {
   int questionCode;
   int tagCode;
   int tagIndex;
  };
struct swad__questionTagsArray
  {
   struct swad__questionTag *__ptr;	// pointer to array
   int __size; 				// number of elements pointed to 
  };
struct swad__getTestsOutput
  {
   struct swad__tagsArray tagsArray;
   struct swad__testQuestionsArray questionsArray;
   struct swad__testAnswersArray answersArray;
   struct swad__questionTagsArray questionTagsArray;
  };
struct swad__getTrivialQuestionOutput
  {
   struct swad__testQuestion question;
   struct swad__testAnswersArray answersArray;
  };

/* getGames */
struct swad__game
  {
   int gameCode;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
   int startTime;
   int endTime;
   char *title;
   char *text;
   int numQuestions;
   float maxGrade;
   int visibility;
  };
struct swad__gamesArray
  {
   struct swad__game *__ptr;	// pointer to array
   int __size; 			// number of elements pointed to
  };
struct swad__getGamesOutput
  {
   int numGames;
   struct swad__gamesArray gamesArray;
  };

/* getMatches */
struct swad__match
  {
   int matchCode;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
   int startTime;
   int endTime;
   char *title;
   int questionIndex;
   char *groups;
  };
struct swad__matchesArray
  {
   struct swad__match *__ptr;	// pointer to array
   int __size; 			// number of elements pointed to
  };
struct swad__getMatchesOutput
  {
   int numMatches;
   struct swad__matchesArray matchesArray;
  };

/* getMatchStatus */
struct swad__getMatchStatusOutput
  {
   int matchCode;
   int questionIndex;
   int numAnswers;
   int answerIndex;
  };

/* answerMatchQuestion */
struct swad__answerMatchQuestionOutput
  {
   int matchCode;
   int answerIndex;
  };

/* structs used in getUsers and sendMessage */
struct swad__user
  {
   int userCode;
   char *userNickname;
   char *userID;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
  };
struct swad__usersArray
  {
   struct swad__user *__ptr;	// pointer to array
   int __size; 			// number of elements pointed to
  };

/* getUsers */
struct swad__getUsersOutput
  {
   int numUsers;
   struct swad__usersArray usersArray;
  };

/* getAttendanceEvents */
struct swad__attendanceEvent
  {
   int attendanceEventCode;
   int hidden;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
   int startTime;
   int endTime;
   int commentsTeachersVisible;
   char *title;
   char *text;
   char *groups;
  };
struct swad__attendanceEventsArray
  {
   struct swad__attendanceEvent *__ptr;	// pointer to array
   int __size; 				// number of elements pointed to
  };
struct swad__getAttendanceEventsOutput
  {
   int numEvents;
   struct swad__attendanceEventsArray eventsArray;
  };

/* sendAttendanceEvent */
struct swad__sendAttendanceEventOutput
  {
   int attendanceEventCode;
  };

/* removeAttendanceEvent */
struct swad__removeAttendanceEventOutput
  {
   int attendanceEventCode;
  };

/* getAttendanceUsers */
struct swad__attendanceUser
  {
   int userCode;
   char *userNickname;
   char *userID;
   char *userSurname1;
   char *userSurname2;
   char *userFirstname;
   char *userPhoto;
   int present;
  };
struct swad__attendanceUsersArray
  {
   struct swad__attendanceUser *__ptr;	// pointer to array
   int __size; 				// number of elements pointed to
  };
struct swad__getAttendanceUsersOutput
  {
   int numUsers;
   struct swad__attendanceUsersArray usersArray;
  };

/* sendAttendanceUsers */
struct swad__sendAttendanceUsersOutput
  {
   int success;	// 1 ==> success; 0 ==> error
   int numUsers;
  };

/* getDirectoryTree */
struct swad__getDirectoryTreeOutput
  {
   char *tree;		// full tree in xml format
  };

/* getFile */
struct swad__getFileOutput
  {
   char *fileName;
   char *URL;
   int size;
   int time;
   char *license;
   char *publisherName;
   char *publisherPhoto;
  };

/* getMarks */
struct swad__getMarksOutput
  {
   char *content;
  };

/* sendNotice */
struct swad__sendNoticeOutput
  {
   int noticeCode;
  };

/* sendMessage */
struct swad__sendMessageOutput
  {
   int numUsers;
   struct swad__usersArray usersArray;
  };

/* location */
struct swad__location
  {
   int institutionCode;
   char *institutionShortName;
   char *institutionFullName;
   int centerCode;
   char *centerShortName;
   char *centerFullName;
   int buildingCode;
   char *buildingShortName;
   char *buildingFullName;
   int floor;
   int roomCode;
   char *roomShortName;
   char *roomFullName;
  };

/* getLocation */
struct swad__getLocationOutput
  {
   struct swad__location location;
  };

/* sendMyLocation */
struct swad__sendMyLocationOutput
  {
   int success;
  };

/* getLastLocation */
struct swad__getLastLocationOutput
  {
   struct swad__location location;
   long checkinTime;
  };

/*****************************************************************************/
/*************************** Web service functions ***************************/
/*****************************************************************************/

/* Login */
int swad__createAccount (char *userNickname,char *userEmail,char *userPassword,char *appKey,
                         struct swad__createAccountOutput *createAccountOut);
int swad__loginByUserPasswordKey (char *userID,char *userPassword,char *appKey,
                                  struct swad__loginByUserPasswordKeyOutput *loginByUserPasswordKeyOut);
int swad__loginBySessionKey (char *sessionID,char *appKey,
                             struct swad__loginBySessionKeyOutput *loginBySessionKeyOut);
int swad__getNewPassword (char *userID,char *appKey,
                          struct swad__getNewPasswordOutput *getNewPasswordOut);

/* Courses */
int swad__getCourses (char *wsKey,
                      struct swad__getCoursesOutput *getCoursesOut);
int swad__getCourseInfo (char *wsKey,int courseCode,char *infoType,
                         struct swad__getCourseInfoOutput *getCourseInfoOut);

/* Groups */
int swad__getGroupTypes (char *wsKey,int courseCode,
                         struct swad__getGroupTypesOutput *getGroupTypesOut);
int swad__getGroups (char *wsKey,int courseCode,
                     struct swad__getGroupsOutput *getGroupsOut);
int swad__sendMyGroups (char *wsKey,int courseCode,char *myGroups,
                        struct swad__sendMyGroupsOutput *sendMyGroupsOut);

/* File browsers */
int swad__getDirectoryTree (char *wsKey,int courseCode,int groupCode,int treeCode,
                            struct swad__getDirectoryTreeOutput *getDirectoryTreeOut);
int swad__getFile (char *wsKey,int fileCode,
                   struct swad__getFileOutput *getFileOut);
int swad__getMarks (char *wsKey,int fileCode,
                    struct swad__getMarksOutput *getMarksOut);

/* Self assessment tests */
int swad__getTestConfig (char *wsKey,int courseCode,
                         struct swad__getTestConfigOutput *getTestConfigOut);
int swad__getTests (char *wsKey,int courseCode,long beginTime,
                    struct swad__getTestsOutput *getTestsOut);
int swad__getTrivialQuestion (char *wsKey,char *degrees,float lowerScore,float upperScore,
                              struct swad__getTrivialQuestionOutput *getTrivialQuestionOut);

/* Games */
int swad__getGames (char *wsKey,int courseCode,
                    struct swad__getGamesOutput *getGamesOut);
int swad__getMatches (char *wsKey,int courseCode,int gameCode,
                      struct swad__getMatchesOutput *getMatchesOut);
int swad__getMatchStatus (char *wsKey,int courseCode,int gameCode,int matchCode,
                          struct swad__getMatchStatusOutput *getMatchStatusOut);
int swad__answerMatchQuestion (char *wsKey,int courseCode,int gameCode,int matchCode,int questionIndex,int answerIndex,
                               struct swad__answerMatchQuestionOutput *answerMatchQuestionOut);

/* List of users */
int swad__getUsers (char *wsKey,int courseCode,char *groups,int userRole,
                    struct swad__getUsersOutput *getUsersOut);
int swad__findUsers (char *wsKey,int courseCode,char *filter,int userRole,
                     struct swad__getUsersOutput *getUsersOut);

/* Control of attendance */
int swad__getAttendanceEvents (char *wsKey,int courseCode,
                               struct swad__getAttendanceEventsOutput *getAttendanceEventsOut);
int swad__sendAttendanceEvent (char *wsKey,int attendanceEventCode,int courseCode,int hidden,int startTime,int endTime,int commentsTeachersVisible,char *title,char *text,char *groups,
                               struct swad__sendAttendanceEventOutput *sendAttendanceEventOut);
int swad__removeAttendanceEvent (char *wsKey,int attendanceEventCode,
                                 struct swad__removeAttendanceEventOutput *removeAttendanceEventOut);
int swad__getAttendanceUsers (char *wsKey,int attendanceEventCode,
                              struct swad__getAttendanceUsersOutput *getAttendanceUsersOut);
int swad__sendAttendanceUsers (char *wsKey,int attendanceEventCode,char *users,int setOthersAsAbsent,
                               struct swad__sendAttendanceUsersOutput *sendAttendanceUsersOut);

/* Notifications */
int swad__getNotifications (char *wsKey,long beginTime,
                            struct swad__getNotificationsOutput *getNotificationsOut);
int swad__markNotificationsAsRead (char *wsKey,char *notifications,
                                   struct swad__markNotificationsAsReadOutput *markNotificationAsReadOut);

/* Notices and messages */
int swad__sendNotice (char *wsKey,int courseCode,char *body,
                      struct swad__sendNoticeOutput *sendNoticeOut);
int swad__sendMessage (char *wsKey,int messageCode,char *to,char *subject,char *body,
                       struct swad__sendMessageOutput *sendMessageOut);

/* Wi-Fi-based positioning system */
int swad__getLocation (char *wsKey,char *MAC,
                       struct swad__getLocationOutput *getLocationOut);
int swad__sendMyLocation (char *wsKey,int roomCode,
                          struct swad__sendMyLocationOutput *sendMyLocationOut);
int swad__getLastLocation (char *wsKey,int userCode,
                           struct swad__getLastLocationOutput *getLastLocationOut);
