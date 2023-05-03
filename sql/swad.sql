CREATE DATABASE IF NOT EXISTS swad DEFAULT CHARACTER SET=latin1 DEFAULT COLLATE latin1_spanish_ci;
USE swad;
--
-- Table act_frequent: stores the recent actions more frequently made by each user
--
CREATE TABLE IF NOT EXISTS act_frequent (
	UsrCod INT NOT NULL,
	ActCod INT NOT NULL,
	Score FLOAT NOT NULL,
	LastClick DATETIME NOT NULL,
	UNIQUE INDEX(UsrCod,ActCod));
--
-- Table agendas: stores users' agendas
--
CREATE TABLE IF NOT EXISTS agd_agendas (
	AgdCod INT NOT NULL AUTO_INCREMENT,
	UsrCod INT NOT NULL,
	Public ENUM('N','Y') NOT NULL DEFAULT 'N',
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	Event VARCHAR(2047) NOT NULL,
	Location VARCHAR(2047) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(AgdCod),
	INDEX(UsrCod,Public));
--
-- Table ann_announcements: stores global announcements
--
CREATE TABLE IF NOT EXISTS ann_announcements (
	AnnCod INT NOT NULL AUTO_INCREMENT,
	Status TINYINT NOT NULL DEFAULT 0,
	Roles INT NOT NULL DEFAULT 0,
	Subject TEXT NOT NULL,
	Content TEXT NOT NULL,
	UNIQUE INDEX(AnnCod),
	INDEX(Status));
--
-- Table ann_seen: stores users who have seen global announcements
--
CREATE TABLE IF NOT EXISTS ann_seen (
	AnnCod INT NOT NULL,
	UsrCod INT NOT NULL,
	UNIQUE INDEX(AnnCod,UsrCod));
--
-- Table api_keys: stores the keys used in the API / web service
--
CREATE TABLE IF NOT EXISTS api_keys (
	WSKey CHAR(43) NOT NULL,
	UsrCod INT NOT NULL,
	PlgCod INT NOT NULL,
	LastTime DATETIME NOT NULL,
	UNIQUE INDEX(WSKey),
	INDEX(UsrCod),
	INDEX(PlgCod),
	INDEX(LastTime));
--
-- Table asg_assignments: stores the assignments proposed by the teachers to their students
--
CREATE TABLE IF NOT EXISTS asg_assignments (
	AsgCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	NumNotif INT NOT NULL DEFAULT 0,
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	Title VARCHAR(2047) NOT NULL,
	Folder VARBINARY(255) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(AsgCod),
	INDEX(CrsCod,Hidden));
--
-- Table asg_groups: stores the groups associated to assigments
--
CREATE TABLE IF NOT EXISTS asg_groups (
	AsgCod INT NOT NULL,
	GrpCod INT NOT NULL,
	UNIQUE INDEX(AsgCod,GrpCod));
--
-- Table att_events: stores events used to control attendance
--
CREATE TABLE IF NOT EXISTS att_events (
	AttCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	CommentTchVisible ENUM('N','Y') NOT NULL DEFAULT 'N',
	Title VARCHAR(2047) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(AttCod),
	INDEX(CrsCod,Hidden));
--
-- Table att_groups: stores groups associated to control attendance
--
CREATE TABLE IF NOT EXISTS att_groups (
	AttCod INT NOT NULL,
	GrpCod INT NOT NULL,
	UNIQUE INDEX(AttCod,GrpCod));
--
-- Table att_users: stores users who have attended to events
--
CREATE TABLE IF NOT EXISTS att_users (
	AttCod INT NOT NULL,
	UsrCod INT NOT NULL,
	Present ENUM('N','Y') NOT NULL DEFAULT 'Y',
	CommentStd TEXT NOT NULL,
	CommentTch TEXT NOT NULL,
	UNIQUE INDEX(AttCod,UsrCod),
	INDEX(UsrCod));
--
-- Table ban_banners: stores advertising banners
--
CREATE TABLE IF NOT EXISTS ban_banners (
	BanCod INT NOT NULL AUTO_INCREMENT,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	ShortName VARCHAR(511) NOT NULL,
	FullName VARCHAR(2047) NOT NULL,
	Img VARCHAR(255) NOT NULL,
	WWW VARCHAR(255) NOT NULL,
	UNIQUE INDEX(BanCod),
	INDEX(Hidden));
--
-- Table bld_buildings: buildings in a center
--
CREATE TABLE IF NOT EXISTS bld_buildings (
	BldCod INT NOT NULL AUTO_INCREMENT,
	CtrCod INT NOT NULL,
	ShortName VARCHAR(511) NOT NULL,
	FullName VARCHAR(2047) NOT NULL,
	Location VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(BldCod),
	INDEX(CtrCod));
--
-- Table brw_caches: stores the media private paths linked from public directories in current session 
--
CREATE TABLE IF NOT EXISTS brw_caches (
	SessionId CHAR(43) NOT NULL,
	PrivPath TEXT COLLATE latin1_bin NOT NULL,
	TmpPubDir TEXT COLLATE latin1_bin NOT NULL,
	INDEX(SessionId));
--
-- Table brw_clipboards: clipboard (paths used to copy-paste folders and files)
--
CREATE TABLE IF NOT EXISTS brw_clipboards (
	UsrCod INT NOT NULL,
	FileBrowser TINYINT NOT NULL,
	Cod INT NOT NULL DEFAULT -1,
	WorksUsrCod INT NOT NULL,
	FileType TINYINT NOT NULL DEFAULT 0,
	Path TEXT COLLATE latin1_bin NOT NULL,
	CopyTime TIMESTAMP,
	UNIQUE INDEX(UsrCod),
	INDEX(FileBrowser,Cod),
	INDEX(WorksUsrCod));
--
-- Table brw_expanded: stores the expanded folders for each user
--
CREATE TABLE IF NOT EXISTS brw_expanded (
	UsrCod INT NOT NULL,
	FileBrowser TINYINT NOT NULL,
	Cod INT NOT NULL DEFAULT -1,
	WorksUsrCod INT NOT NULL,
	Path TEXT COLLATE latin1_bin NOT NULL,
	ClickTime DATETIME NOT NULL,
	INDEX(UsrCod,FileBrowser,Cod),
	INDEX(FileBrowser,Cod),
	INDEX(WorksUsrCod));
--
-- Table brw_files: stores metadata about each file
--
CREATE TABLE IF NOT EXISTS brw_files (
	FilCod INT NOT NULL AUTO_INCREMENT,
	FileBrowser TINYINT NOT NULL,
	Cod INT NOT NULL DEFAULT -1,
	ZoneUsrCod INT NOT NULL DEFAULT -1,
	PublisherUsrCod INT NOT NULL,
	FileType TINYINT NOT NULL DEFAULT 0,
	Path TEXT COLLATE latin1_bin NOT NULL,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	Public ENUM('N','Y') NOT NULL DEFAULT 'N',
	License TINYINT NOT NULL DEFAULT 0,
	UNIQUE INDEX(FilCod),
	INDEX(FileBrowser,Cod,ZoneUsrCod),
	INDEX(ZoneUsrCod),
	INDEX(PublisherUsrCod));
--
-- Table brw_last: stores the last click of every user in each file browser zone
--
CREATE TABLE IF NOT EXISTS brw_last (
	UsrCod INT NOT NULL,
	FileBrowser TINYINT NOT NULL,
	Cod INT NOT NULL DEFAULT -1,
	LastClick DATETIME NOT NULL,
	UNIQUE INDEX(UsrCod,FileBrowser,Cod));
--
-- Table brw_sizes: stores the sizes of the file zones
--
CREATE TABLE IF NOT EXISTS brw_sizes (
	FileBrowser TINYINT NOT NULL,
	Cod INT NOT NULL DEFAULT -1,
	ZoneUsrCod INT NOT NULL DEFAULT -1,
	NumLevels INT NOT NULL,
	NumFolders INT NOT NULL,
	NumFiles INT NOT NULL,
	TotalSize BIGINT NOT NULL,
	UNIQUE INDEX(FileBrowser,Cod,ZoneUsrCod),
	INDEX(ZoneUsrCod));
--
-- Table brw_views: stores the number of times each user has seen each file
--
CREATE TABLE IF NOT EXISTS brw_views (
	FilCod INT NOT NULL,
	UsrCod INT NOT NULL,
	NumViews INT NOT NULL DEFAULT 0,
	UNIQUE INDEX(FilCod,UsrCod),INDEX(UsrCod));
--
-- Table cfe_exams: stores the calls for exams
--
CREATE TABLE IF NOT EXISTS cfe_exams (
	ExaCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	Status TINYINT NOT NULL DEFAULT 0,
	NumNotif INT NOT NULL DEFAULT 0,
	CrsFullName VARCHAR(2047) NOT NULL,
	Year TINYINT NOT NULL,
	ExamSession VARCHAR(2047) NOT NULL,
	CallDate DATETIME NOT NULL,
	ExamDate DATETIME NOT NULL,
	Duration TIME NOT NULL,
	Place TEXT NOT NULL,
	ExamMode TEXT NOT NULL,
	Structure TEXT NOT NULL,
	DocRequired TEXT NOT NULL,
	MatRequired TEXT NOT NULL,
	MatAllowed TEXT NOT NULL,
	OtherInfo TEXT NOT NULL,
	UNIQUE INDEX(ExaCod),
	INDEX(CrsCod,Status),
	INDEX(Status));
--
-- Table cht_rooms: stores number of users in each chat room (this table is not used now)
--
CREATE TABLE IF NOT EXISTS cht_rooms (
	RoomCode VARCHAR(16) NOT NULL,
	NumUsrs INT NOT NULL,
	UNIQUE INDEX(RoomCode));
--
-- Table crs_courses: stores the courses/subjects
--
CREATE TABLE IF NOT EXISTS crs_courses (
	CrsCod INT NOT NULL AUTO_INCREMENT,
	DegCod INT NOT NULL DEFAULT -1,
	Year TINYINT NOT NULL DEFAULT 0,
	InsCrsCod CHAR(7) NOT NULL,
	Status TINYINT NOT NULL DEFAULT 0,
	RequesterUsrCod INT NOT NULL DEFAULT -1,
	ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,
	FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,
	NumIndicators INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(CrsCod),
	INDEX(DegCod,Year),
	INDEX(Status));
--
-- Table grp_groups: stores the groups in courses
--
CREATE TABLE IF NOT EXISTS grp_groups (
	GrpCod INT NOT NULL AUTO_INCREMENT,
	GrpTypCod INT NOT NULL,
	GrpName VARCHAR(2047) NOT NULL,
	RooCod INT NOT NULL DEFAULT -1,
	MaxStudents INT NOT NULL,
	Open ENUM('N','Y') NOT NULL DEFAULT 'N',
	FileZones ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(GrpCod),
	INDEX(GrpTypCod),
	INDEX(RooCod));
--
-- Table grp_types: stores the types of groups in courses
--
CREATE TABLE IF NOT EXISTS grp_types (
	GrpTypCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	GrpTypName VARCHAR(2047) NOT NULL,
	Mandatory ENUM('N','Y') NOT NULL,
	Multiple ENUM('N','Y') NOT NULL,
	MustBeOpened ENUM('N','Y') NOT NULL DEFAULT 'N',
	OpenTime DATETIME NOT NULL,
	UNIQUE INDEX(GrpTypCod),
	INDEX(CrsCod));
--
-- Table grp_users: stores the users beloging to each group
--
CREATE TABLE IF NOT EXISTS grp_users (
	GrpCod INT NOT NULL,
	UsrCod INT NOT NULL,
	UNIQUE INDEX(GrpCod,UsrCod),
	INDEX(GrpCod),
	INDEX(UsrCod));
--
-- Table crs_info_read: stores the users who have read the information with mandatory reading
--
CREATE TABLE IF NOT EXISTS crs_info_read (
	UsrCod INT NOT NULL,
	CrsCod INT NOT NULL,
	InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL,
	UNIQUE INDEX(UsrCod,CrsCod,InfoType));
--
-- Table crs_info_src: stores the sources of information in courses
--
CREATE TABLE IF NOT EXISTS crs_info_src (
	CrsCod INT NOT NULL DEFAULT -1,
	InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL,
	InfoSrc ENUM('none','editor','plain_text','rich_text','page','URL') NOT NULL,
	MustBeRead ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(CrsCod,InfoType));
--
-- Table crs_info_txt: stores the text content of information in courses
--
CREATE TABLE IF NOT EXISTS crs_info_txt (
	CrsCod INT NOT NULL DEFAULT -1,
	InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL,
	InfoTxtHTML LONGTEXT NOT NULL,
	InfoTxtMD LONGTEXT NOT NULL,
	UNIQUE INDEX(CrsCod,InfoType));
--
-- Table crs_last: stores last access to courses from students or teachers
--
CREATE TABLE IF NOT EXISTS crs_last (
	CrsCod INT NOT NULL,
	LastTime DATETIME NOT NULL,
	UNIQUE INDEX(CrsCod),
	INDEX(LastTime));
--
-- Table crs_record_fields: stores the fields in the course records
--
CREATE TABLE IF NOT EXISTS crs_record_fields (
	FieldCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	FieldName VARCHAR(2047) NOT NULL,
	NumLines INT NOT NULL,
	Visibility TINYINT NOT NULL,
	UNIQUE INDEX(FieldCod),
	INDEX(CrsCod));
--
-- Table crs_records: stores the contents of course records
--
CREATE TABLE IF NOT EXISTS crs_records (
	FieldCod INT NOT NULL,
	UsrCod INT NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(FieldCod,UsrCod));
--
-- Table crs_requests: stores requests for enrollment in courses
--
CREATE TABLE IF NOT EXISTS crs_requests (
	ReqCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL,
	Role TINYINT NOT NULL DEFAULT 0,
	RequestTime DATETIME NOT NULL,
	UNIQUE INDEX(ReqCod),
	UNIQUE INDEX(CrsCod,UsrCod),
	INDEX(UsrCod));
--
-- Table crs_users: stores who users belong to what courses
--
CREATE TABLE IF NOT EXISTS crs_users (
	CrsCod INT NOT NULL,
	UsrCod INT NOT NULL,
	Role TINYINT NOT NULL DEFAULT 0,
	Accepted ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(CrsCod,UsrCod),
	UNIQUE INDEX(CrsCod,UsrCod,Role),
	UNIQUE INDEX(UsrCod,CrsCod),
	UNIQUE INDEX(UsrCod,CrsCod,Role),
	UNIQUE INDEX(Role,CrsCod,UsrCod),
	UNIQUE INDEX(Role,UsrCod,CrsCod),
	INDEX(CrsCod,Role),
	INDEX(UsrCod,Role));
--
-- Table crs_user_settings: stores last settings (preferences) of users in courses
--
CREATE TABLE IF NOT EXISTS crs_user_settings (
	UsrCod INT NOT NULL,
	CrsCod INT NOT NULL,
	LastDowGrpCod INT NOT NULL DEFAULT -1,
	LastComGrpCod INT NOT NULL DEFAULT -1,
	LastAssGrpCod INT NOT NULL DEFAULT -1,
	NumAccTst INT NOT NULL DEFAULT 0,
	LastAccTst DATETIME NOT NULL,
	NumQstsLastTst INT NOT NULL DEFAULT 0,
	UsrListType ENUM('classphoto','list') NOT NULL DEFAULT 'classphoto',
	ColsClassPhoto TINYINT NOT NULL,
	ListWithPhotos ENUM('N','Y') NOT NULL DEFAULT 'Y',
	UNIQUE INDEX(UsrCod,CrsCod));
--
-- Table ctr_centers: centers (faculties, schools...)
--
CREATE TABLE IF NOT EXISTS ctr_centers (
	CtrCod INT NOT NULL AUTO_INCREMENT,
	InsCod INT NOT NULL,
	PlcCod INT NOT NULL DEFAULT -1,
	Status TINYINT NOT NULL DEFAULT 0,
	RequesterUsrCod INT NOT NULL DEFAULT -1,
	Latitude DOUBLE PRECISION NOT NULL DEFAULT 0,
	Longitude DOUBLE PRECISION NOT NULL DEFAULT 0,
	Altitude DOUBLE PRECISION NOT NULL DEFAULT 0,
	ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,
	FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,
	WWW VARCHAR(255) NOT NULL,
	PhotoAttribution TEXT NOT NULL,
	UNIQUE INDEX(CtrCod),
	INDEX(InsCod),
	INDEX(PlcCod),
	INDEX(Status));
--
-- Table cty_countrs: stores the countries
--
CREATE TABLE IF NOT EXISTS cty_countrs (
	CtyCod INT NOT NULL,
	Alpha2 CHAR(2) NOT NULL,
	MapAttribution TEXT NOT NULL,
	Name_ca VARCHAR(767) NOT NULL,
	Name_de VARCHAR(767) NOT NULL,
	Name_en VARCHAR(767) NOT NULL,
	Name_es VARCHAR(767) NOT NULL,
	Name_fr VARCHAR(767) NOT NULL,
	Name_gn VARCHAR(767) NOT NULL,
	Name_it VARCHAR(767) NOT NULL,
	Name_pl VARCHAR(767) NOT NULL,
	Name_pt VARCHAR(767) NOT NULL,
	Name_tr VARCHAR(767) NOT NULL,
	WWW_ca VARCHAR(255) NOT NULL,
	WWW_de VARCHAR(255) NOT NULL,
	WWW_en VARCHAR(255) NOT NULL,
	WWW_es VARCHAR(255) NOT NULL,
	WWW_fr VARCHAR(255) NOT NULL,
	WWW_gn VARCHAR(255) NOT NULL,
	WWW_it VARCHAR(255) NOT NULL,
	WWW_pl VARCHAR(255) NOT NULL,
	WWW_pt VARCHAR(255) NOT NULL,
	WWW_tr VARCHAR(255) NOT NULL,
	UNIQUE INDEX(CtyCod),
	UNIQUE INDEX(Alpha2),
	INDEX(Name_ca),
	INDEX(Name_de),
	INDEX(Name_en),
	INDEX(Name_es),
	INDEX(Name_fr),
	INDEX(Name_gn),
	INDEX(Name_it),
	INDEX(Name_pl),
	INDEX(Name_pt),
	INDEX(Name_tr));
--
-- Table dbg_debug: used for debugging purposes
--
CREATE TABLE IF NOT EXISTS dbg_debug (
	DebugTime DATETIME NOT NULL,
	Txt TEXT NOT NULL,
	INDEX(DebugTime));
--
-- Table deg_degrees: stores the degrees
--
CREATE TABLE IF NOT EXISTS deg_degrees (
	DegCod INT NOT NULL AUTO_INCREMENT,
	CtrCod INT NOT NULL,
	DegTypCod INT NOT NULL,
	Status TINYINT NOT NULL DEFAULT 0,
	RequesterUsrCod INT NOT NULL DEFAULT -1,
	ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,
	FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,
	WWW VARCHAR(255) NOT NULL,
	UNIQUE INDEX(DegCod),
	INDEX(CtrCod),
	INDEX(DegTypCod),
	INDEX(Status));
--
-- Table deg_types: stores the types of degree
--
CREATE TABLE IF NOT EXISTS deg_types (
	DegTypCod INT NOT NULL AUTO_INCREMENT,
	DegTypName VARCHAR(511) NOT NULL,
	UNIQUE INDEX(DegTypCod));
--
-- Table dpt_departments: stores the departments
--
CREATE TABLE IF NOT EXISTS dpt_departments (
	DptCod INT NOT NULL AUTO_INCREMENT,
	InsCod INT NOT NULL,
	ShortName VARCHAR(511) NOT NULL,
	FullName VARCHAR(2047) NOT NULL,
	WWW VARCHAR(255) NOT NULL,
	UNIQUE INDEX(DptCod),
	INDEX(InsCod));
--
-- Table exa_exams: stores the exams
--
CREATE TABLE IF NOT EXISTS exa_exams (
	ExaCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	UsrCod INT NOT NULL,
	MaxGrade DOUBLE PRECISION NOT NULL DEFAULT 1,
	Visibility INT NOT NULL DEFAULT 0x1f,
	Title VARCHAR(2047) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(ExaCod),
	INDEX(CrsCod));
--
-- Table exa_groups: stores the groups associated to each session in an exam
--
CREATE TABLE IF NOT EXISTS exa_groups (
	SesCod INT NOT NULL,
	GrpCod INT NOT NULL,
	UNIQUE INDEX(SesCod,GrpCod));
--
-- Table exa_log: stores the access log to exam prints
--
CREATE TABLE IF NOT EXISTS exa_log (
	LogCod INT NOT NULL,
	PrnCod INT NOT NULL,
	ActCod INT NOT NULL,
	QstInd INT NOT NULL DEFAULT -1,
	CanAnswer ENUM('N','Y') NOT NULL DEFAULT 'N',
	ClickTime DATETIME NOT NULL,
	IP CHAR(15) NOT NULL,
	UNIQUE INDEX(LogCod),
	UNIQUE INDEX(PrnCod,LogCod),
	INDEX(ClickTime));
--
-- Table exa_log_sessions: stores the session id fields for access log to exam prints
--
CREATE TABLE IF NOT EXISTS exa_log_sessions (
	LogCod INT NOT NULL,
	PrnCod INT NOT NULL,
	SessionId CHAR(43) NOT NULL,
	UNIQUE INDEX(LogCod),
	UNIQUE INDEX(PrnCod,LogCod));
--
-- Table exa_log_user_agents: stores the user agent fields for access log to exam prints
--
CREATE TABLE IF NOT EXISTS exa_log_user_agents (
	LogCod INT NOT NULL,
	PrnCod INT NOT NULL,
	UserAgent TEXT NOT NULL,
	UNIQUE INDEX(LogCod),
	UNIQUE INDEX(PrnCod,LogCod));
--
-- Table exa_print_questions: stores the questions and answers in exam prints made by users
--
CREATE TABLE IF NOT EXISTS exa_print_questions (
	PrnCod INT NOT NULL,
	QstCod INT NOT NULL,
	QstInd INT NOT NULL,
	SetCod INT NOT NULL,
	Score DOUBLE PRECISION NOT NULL DEFAULT 0,
	Indexes TEXT NOT NULL,
	Answers TEXT NOT NULL,
	UNIQUE INDEX(PrnCod,QstCod));
--
-- Table exa_prints: stores the exam prints of every exam session
--
CREATE TABLE IF NOT EXISTS exa_prints (
	PrnCod INT NOT NULL AUTO_INCREMENT,
	SesCod INT NOT NULL,
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	NumQsts INT NOT NULL DEFAULT 0,
	NumQstsNotBlank INT NOT NULL DEFAULT 0,
	Sent ENUM('N','Y') NOT NULL DEFAULT 'N',
	Score DOUBLE PRECISION NOT NULL DEFAULT 0,
	UNIQUE INDEX(PrnCod),
	UNIQUE INDEX(SesCod,UsrCod));
--
-- Table exa_sessions: stores the exam sessions
--
CREATE TABLE IF NOT EXISTS exa_sessions (
	SesCod INT NOT NULL AUTO_INCREMENT,
	ExaCod INT NOT NULL,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	Title VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(SesCod),
	INDEX(ExaCod));
--
-- Table exa_set_answers: stores the answers of questions in exam sets
--
CREATE TABLE IF NOT EXISTS exa_set_answers (
	QstCod INT NOT NULL,
	AnsInd TINYINT NOT NULL,
	Answer TEXT NOT NULL,
	Feedback TEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	Correct ENUM('N','Y') NOT NULL,
	UNIQUE INDEX(QstCod,AnsInd),
	INDEX(MedCod));
--
-- Table exa_set_questions: stores the questions in exam sets
--
CREATE TABLE IF NOT EXISTS exa_set_questions (
	QstCod INT NOT NULL AUTO_INCREMENT,
	SetCod INT NOT NULL,
	AnsType ENUM ('int','float','true_false','unique_choice','multiple_choice','text') NOT NULL,
	Shuffle ENUM('N','Y') NOT NULL,
	Stem TEXT NOT NULL,
	Feedback TEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(QstCod),
	UNIQUE INDEX(SetCod,QstCod),
	INDEX(MedCod));
--
-- Table exa_sets: stores the question sets in the exams
--
CREATE TABLE IF NOT EXISTS exa_sets (
	SetCod INT NOT NULL AUTO_INCREMENT,
	ExaCod INT NOT NULL,
	SetInd INT NOT NULL,
	NumQstsToPrint INT NOT NULL DEFAULT 0,
	Title VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(SetCod),
	UNIQUE INDEX(ExaCod,SetInd));
--
-- Table fig_figures: stores cached figures for quick retrieval of figures (i.e. number of students in the platform)
--
CREATE TABLE IF NOT EXISTS fig_figures (
	Figure INT NOT NULL,
	Scope ENUM('Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Sys',
	Cod INT NOT NULL DEFAULT -1,
	ValueInt INT NOT NULL DEFAULT 0,
	ValueDouble DOUBLE PRECISION NOT NULL DEFAULT 0.0,
	LastUpdate TIMESTAMP,
	UNIQUE INDEX(Figure,Scope,Cod));
--
-- Table fir_banned: stores the banned IPs in order to mitigate denial of service attacks
--
CREATE TABLE IF NOT EXISTS fir_banned (
	IP CHAR(15) NOT NULL,
	BanTime DATETIME NOT NULL,
	UnbanTime DATETIME NOT NULL,
	INDEX(IP,UnbanTime),
	INDEX(BanTime),
	INDEX(UnbanTime));
--
-- Table fir_log: stores the most recent IPs in order to mitigate denial of service attacks
--
CREATE TABLE IF NOT EXISTS fir_log (
	ClickTime DATETIME NOT NULL,
	IP CHAR(15) NOT NULL,
	UsrCod INT NOT NULL DEFAULT -1,
	INDEX(ClickTime),
	INDEX(IP),
	INDEX(UsrCod));
--
-- Table for_clipboards: stores the clipboards used to move threads from one forum to another
--
CREATE TABLE IF NOT EXISTS for_clipboards (
	ThrCod INT NOT NULL,
	UsrCod INT NOT NULL,
	TimeInsert TIMESTAMP NOT NULL,
	UNIQUE INDEX(ThrCod),
	UNIQUE INDEX(UsrCod));
--
-- Table for_disabled: stores the forum posts that have been disabled
--
CREATE TABLE IF NOT EXISTS for_disabled (
	PstCod INT NOT NULL,
	UsrCod INT NOT NULL,
	DisableTime DATETIME NOT NULL,
	UNIQUE INDEX(PstCod));
--
-- Table for_posts: stores the forum posts
--
CREATE TABLE IF NOT EXISTS ffor_posts(
	PstCod INT NOT NULL AUTO_INCREMENT,
	ThrCod INT NOT NULL,
	UsrCod INT NOT NULL,
	CreatTime DATETIME NOT NULL,
	ModifTime DATETIME NOT NULL,
	NumNotif INT NOT NULL DEFAULT 0,
	Subject TEXT NOT NULL,
	Content LONGTEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(PstCod),
	INDEX(ThrCod),
	INDEX(UsrCod),
	INDEX(CreatTime),
	INDEX(ModifTime),
	INDEX(MedCod));
--
-- Table for_read: stores the threads read by each user
--
CREATE TABLE IF NOT EXISTS for_read (
	ThrCod INT NOT NULL,
	UsrCod INT NOT NULL,
	ReadTime DATETIME NOT NULL,
	UNIQUE INDEX(ThrCod,UsrCod));
--
-- Table for_threads: stores the forum threads
--
CREATE TABLE IF NOT EXISTS for_threads (
	ThrCod INT NOT NULL AUTO_INCREMENT,
	ForumType TINYINT NOT NULL,
	HieCod INT NOT NULL DEFAULT -1,
	FirstPstCod INT NOT NULL,
	LastPstCod INT NOT NULL,
	UNIQUE INDEX(ThrCod),
	INDEX(ForumType),
	INDEX(HieCod),
	UNIQUE INDEX(FirstPstCod),
	UNIQUE INDEX(LastPstCod));
--
-- Table gam_games: stores the games
--
CREATE TABLE IF NOT EXISTS gam_games (
	GamCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	UsrCod INT NOT NULL,
	MaxGrade DOUBLE PRECISION NOT NULL DEFAULT 1,
	Visibility INT NOT NULL DEFAULT 0x1f,
	Title VARCHAR(2047) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(GamCod),
	INDEX(CrsCod));
--
-- Table gam_questions: stores the questions in the games
--
CREATE TABLE IF NOT EXISTS gam_questions (
	GamCod INT NOT NULL,
	QstInd INT NOT NULL,
	QstCod INT NOT NULL,
	UNIQUE INDEX(GamCod,QstInd),
	UNIQUE INDEX(GamCod,QstCod));
--
-- Table hld_holidays: stores the holidays in each institution
--
CREATE TABLE IF NOT EXISTS hld_holidays (
	HldCod INT NOT NULL AUTO_INCREMENT,
	InsCod INT NOT NULL,
	PlcCod INT NOT NULL DEFAULT -1,
	HldTyp TINYINT NOT NULL,
	StartDate DATE NOT NULL,
	EndDate DATE NOT NULL,
	Name VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(HldCod),
	INDEX(InsCod),
	INDEX(PlcCod));
--
-- Table institutions: stores the institutions (for example, universities)
--
CREATE TABLE IF NOT EXISTS institutions (
	InsCod INT NOT NULL AUTO_INCREMENT,
	CtyCod INT NOT NULL,
	Status TINYINT NOT NULL DEFAULT 0,
	RequesterUsrCod INT NOT NULL DEFAULT -1,
	ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,
	FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,
	WWW VARCHAR(255) NOT NULL,
	UNIQUE INDEX(InsCod),
	INDEX(CtyCod),
	INDEX(Status));
--
-- Table lnk_links: stores the global institutional links shown on right part of page
--
CREATE TABLE IF NOT EXISTS lnk_links (
	LnkCod INT NOT NULL AUTO_INCREMENT,
	ShortName VARCHAR(511) NOT NULL,
	FullName VARCHAR(2047) NOT NULL,
	WWW VARCHAR(255) NOT NULL,
	UNIQUE INDEX(LnkCod));
--
-- Table log: stores the log of all clicks
--
CREATE TABLE IF NOT EXISTS log (
	LogCod INT NOT NULL AUTO_INCREMENT,
	ActCod INT NOT NULL DEFAULT -1,
	CtyCod INT NOT NULL DEFAULT -1,
	InsCod INT NOT NULL DEFAULT -1,
	CtrCod INT NOT NULL DEFAULT -1,
	DegCod INT NOT NULL DEFAULT -1,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL DEFAULT -1,
	Role TINYINT NOT NULL,
	ClickTime DATETIME NOT NULL,
	TimeToGenerate INT NOT NULL,
	TimeToSend INT NOT NULL,
	IP CHAR(15) NOT NULL,
	PRIMARY KEY(LogCod,ClickTime),
	INDEX(ActCod),
	INDEX(CtyCod),
	INDEX(InsCod),
	INDEX(CtrCod),
	INDEX(DegCod),
	INDEX(CrsCod),
	INDEX(UsrCod),
	INDEX(ClickTime,Role)
	) ENGINE=InnoDB
	PARTITION BY RANGE (YEAR(ClickTime))
	(
	PARTITION p2004 VALUES LESS THAN (2005),
	PARTITION p2005 VALUES LESS THAN (2006),
	PARTITION p2006 VALUES LESS THAN (2007),
	PARTITION p2007 VALUES LESS THAN (2008),
	PARTITION p2008 VALUES LESS THAN (2009),
	PARTITION p2009 VALUES LESS THAN (2010),
	PARTITION p2010 VALUES LESS THAN (2011),
	PARTITION p2011 VALUES LESS THAN (2012),
	PARTITION p2012 VALUES LESS THAN (2013),
	PARTITION p2013 VALUES LESS THAN (2014),
	PARTITION p2014 VALUES LESS THAN (2015),
	PARTITION p2015 VALUES LESS THAN (2016),
	PARTITION p2016 VALUES LESS THAN (2017),
	PARTITION p2017 VALUES LESS THAN (2018),
	PARTITION p2018 VALUES LESS THAN (2019),
	PARTITION p2019 VALUES LESS THAN (2020),
	PARTITION p2020 VALUES LESS THAN (2021),
	PARTITION p2021 VALUES LESS THAN (2022),
	PARTITION p2022 VALUES LESS THAN (2023),
	PARTITION p2023 VALUES LESS THAN (2024),
	PARTITION p2024 VALUES LESS THAN (2025),
	PARTITION p2025 VALUES LESS THAN (2026),
	PARTITION p2026 VALUES LESS THAN (2027),
	PARTITION p2027 VALUES LESS THAN (2028),
	PARTITION p2028 VALUES LESS THAN (2029),
	PARTITION p2029 VALUES LESS THAN (2030),
	PARTITION p2030 VALUES LESS THAN (2031),
	PARTITION p2031 VALUES LESS THAN (2032),
	PARTITION p2032 VALUES LESS THAN (2033),
	PARTITION p2033 VALUES LESS THAN (2034),
	PARTITION p2034 VALUES LESS THAN (2035),
	PARTITION p2035 VALUES LESS THAN (2036),
	PARTITION p2036 VALUES LESS THAN (2037),
	PARTITION p2037 VALUES LESS THAN (2038),
	PARTITION p2038 VALUES LESS THAN (2039),
	PARTITION p2039 VALUES LESS THAN (2040),
	PARTITION p2040 VALUES LESS THAN (2041),
	PARTITION p2041 VALUES LESS THAN (2042),
	PARTITION p2042 VALUES LESS THAN (2043),
	PARTITION p2043 VALUES LESS THAN (2044),
	PARTITION p2044 VALUES LESS THAN (2045),
	PARTITION p2045 VALUES LESS THAN (2046),
	PARTITION p2046 VALUES LESS THAN (2047),
	PARTITION p2047 VALUES LESS THAN (2048),
	PARTITION p2048 VALUES LESS THAN (2049),
	PARTITION p2049 VALUES LESS THAN (2050),
	PARTITION p2050 VALUES LESS THAN MAXVALUE
	);
--
-- Table log_api: stores the log of calls to API (web service) from plugins
--
CREATE TABLE IF NOT EXISTS log_api (
	LogCod INT NOT NULL,
	PlgCod INT NOT NULL,
	FunCod INT NOT NULL,
	UNIQUE INDEX(LogCod),
	INDEX(PlgCod),
	INDEX(FunCod));
--
-- Table log_banners: stores the log of clicked banners
--
CREATE TABLE IF NOT EXISTS log_banners (
	LogCod INT NOT NULL,
	BanCod INT NOT NULL,
	UNIQUE INDEX(LogCod),INDEX(BanCod));
--
-- Table log_comments: stores the comments about errors associated to the log
--
CREATE TABLE IF NOT EXISTS log_comments (
	LogCod INT NOT NULL,
	Comments TEXT NOT NULL,
	UNIQUE INDEX(LogCod));
--
-- Table log_recent: stores the log of the most recent clicks, used to speed up queries related to log
--
CREATE TABLE IF NOT EXISTS log_recent (
	LogCod INT NOT NULL,
	ActCod INT NOT NULL DEFAULT -1,
	CtyCod INT NOT NULL DEFAULT -1,
	InsCod INT NOT NULL DEFAULT -1,
	CtrCod INT NOT NULL DEFAULT -1,
	DegCod INT NOT NULL DEFAULT -1,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL DEFAULT -1,
	Role TINYINT NOT NULL,
	ClickTime DATETIME NOT NULL,
	TimeToGenerate INT NOT NULL,
	TimeToSend INT NOT NULL,
	IP CHAR(15) NOT NULL,
	UNIQUE INDEX(LogCod),
	INDEX(ActCod),
	INDEX(CtyCod),
	INDEX(InsCod),
	INDEX(CtrCod),
	INDEX(DegCod),
	INDEX(CrsCod),
	INDEX(UsrCod),
	INDEX(ClickTime,Role));
--
-- Table log_search: stores the log of search strings
--
CREATE TABLE IF NOT EXISTS log_search (
	LogCod INT NOT NULL,
	SearchStr VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(LogCod));
--
-- Table mch_answers: stores the users' answers to the matches
--
CREATE TABLE IF NOT EXISTS mch_answers (
	MchCod INT NOT NULL,
	UsrCod INT NOT NULL,
	QstInd INT NOT NULL,
	NumOpt TINYINT NOT NULL,
	AnsInd TINYINT NOT NULL,
	UNIQUE INDEX(MchCod,UsrCod,QstInd));
--
-- Table mch_indexes: stores the order of answers in a match
--
CREATE TABLE IF NOT EXISTS mch_indexes (
	MchCod INT NOT NULL,
	QstInd INT NOT NULL,
	Indexes TEXT NOT NULL,
	UNIQUE INDEX(MchCod,QstInd));
--
-- Table mch_matches: stores the matches (games instances) already played
--
CREATE TABLE IF NOT EXISTS mch_matches (
	MchCod INT NOT NULL AUTO_INCREMENT,
	GamCod INT NOT NULL,
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	Title VARCHAR(2047) NOT NULL,
	QstInd INT NOT NULL DEFAULT 0,
	QstCod INT NOT NULL DEFAULT -1,
	Showing ENUM('start','stem','answers','results','end') NOT NULL DEFAULT 'start',
	Countdown INT NOT NULL DEFAULT -1,
	NumCols INT NOT NULL DEFAULT 1,
	ShowQstResults ENUM('N','Y') NOT NULL DEFAULT 'N',
	ShowUsrResults ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(MchCod),
	INDEX(GamCod));
--
-- Table mch_players: stores the current match players
--
CREATE TABLE IF NOT EXISTS mch_players (
	MchCod INT NOT NULL,
	UsrCod INT NOT NULL,
	TS TIMESTAMP,
	UNIQUE INDEX(MchCod,UsrCod));
--
-- Table mch_playing: stores the current matches being played
--
CREATE TABLE IF NOT EXISTS mch_playing (
	MchCod INT NOT NULL,
	TS TIMESTAMP,
	UNIQUE INDEX(MchCod));
--
-- Table mch_results: stores match results
--
CREATE TABLE IF NOT EXISTS mch_results (
	MchCod INT NOT NULL,
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	NumQsts INT NOT NULL DEFAULT 0,
	NumQstsNotBlank INT NOT NULL DEFAULT 0,
	Score DOUBLE PRECISION NOT NULL DEFAULT 0,
	UNIQUE INDEX(MchCod,UsrCod));
--
-- Table mch_times: stores the elapsed time in every question in every match played
--
CREATE TABLE IF NOT EXISTS mch_times (
	MchCod INT NOT NULL,
	QstInd INT NOT NULL,
	ElapsedTime TIME NOT NULL DEFAULT 0,
	UNIQUE INDEX(MchCod,QstInd));
--
-- Table med_media: stores information about media (images, videos, YouTube)
--
CREATE TABLE IF NOT EXISTS med_media (
	MedCod INT NOT NULL AUTO_INCREMENT,
	Type ENUM('none','jpg','gif','mp4','webm','ogg','youtube','embed') NOT NULL DEFAULT 'none',
	Name VARCHAR(43) NOT NULL DEFAULT '',
	URL VARCHAR(255) NOT NULL DEFAULT '',
	Title VARCHAR(2047) NOT NULL DEFAULT '',
	UNIQUE INDEX(MedCod),
	INDEX(Type));
--
-- Table mrk_marks: stores information about files of marks
--
CREATE TABLE IF NOT EXISTS mrk_marks (
	FilCod INT NOT NULL AUTO_INCREMENT,
	Header INT NOT NULL,
	Footer INT NOT NULL,
	UNIQUE INDEX(FilCod));
--
-- Table msg_banned: stores the users whose messages are banned (FromUsrCod is a recipien banned from ToUsrCod)
--
CREATE TABLE IF NOT EXISTS msg_banned (
	FromUsrCod INT NOT NULL,
	ToUsrCod INT NOT NULL,
	UNIQUE INDEX(FromUsrCod,ToUsrCod));
--
-- Table msg_content: stores the content of the sent messages
--
CREATE TABLE IF NOT EXISTS msg_content (
	MsgCod INT NOT NULL AUTO_INCREMENT,
	Subject TEXT NOT NULL,
	Content LONGTEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(MsgCod),
	FULLTEXT(Subject,Content),
	INDEX(MedCod)) ENGINE = MYISAM;
--
-- Table msg_content_deleted: stores the content of the sent messages that have been deleted
--
CREATE TABLE IF NOT EXISTS msg_content_deleted (
	MsgCod INT NOT NULL,
	Subject TEXT NOT NULL,
	Content LONGTEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(MsgCod),
	FULLTEXT(Subject,Content),
	INDEX(MedCod)) ENGINE = MYISAM;
--
-- Table msg_rcv: stores the received messages
--
CREATE TABLE IF NOT EXISTS msg_rcv (
	MsgCod INT NOT NULL,
	UsrCod INT NOT NULL,
	Notified ENUM('N','Y') NOT NULL DEFAULT 'N',
	Open ENUM('N','Y') NOT NULL DEFAULT 'N',
	Replied ENUM('N','Y') NOT NULL DEFAULT 'N',
	Expanded ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(UsrCod,MsgCod),
	INDEX(MsgCod),
	INDEX(Notified));
--
-- Table msg_rcv_deleted: stores the received messages that have been deleted
--
CREATE TABLE IF NOT EXISTS msg_rcv_deleted (
	MsgCod INT NOT NULL,
	UsrCod INT NOT NULL,
	Notified ENUM('N','Y') NOT NULL DEFAULT 'N',
	Open ENUM('N','Y') NOT NULL DEFAULT 'N',
	Replied ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(UsrCod,MsgCod),
	INDEX(MsgCod),
	INDEX(Notified));
--
-- Table msg_snt: stores the sent messages
--
CREATE TABLE IF NOT EXISTS msg_snt (
	MsgCod INT NOT NULL,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL,
	Expanded ENUM('N','Y') NOT NULL DEFAULT 'N',
	CreatTime DATETIME NOT NULL,
	UNIQUE INDEX(MsgCod),
	INDEX(CrsCod),
	INDEX(UsrCod));
--
-- Table msg_snt_deleted: stores the sent messages that have been deleted
--
CREATE TABLE IF NOT EXISTS msg_snt_deleted (
	MsgCod INT NOT NULL,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL,
	CreatTime DATETIME NOT NULL,
	UNIQUE INDEX(MsgCod),
	INDEX(CrsCod),
	INDEX(UsrCod));
--
-- Table not_deleted: stores the yellow notes (post-it) that have been deleted
--
CREATE TABLE IF NOT EXISTS not_deleted (
	NotCod INT NOT NULL,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL,
	CreatTime DATETIME NOT NULL,
	Content TEXT NOT NULL,
	NumNotif INT NOT NULL DEFAULT 0,
	UNIQUE INDEX(NotCod),
	INDEX(CrsCod),
	INDEX(UsrCod),
	INDEX(CreatTime));
--
-- Table not_notices: stores the yellow notes (post-it)
--
CREATE TABLE IF NOT EXISTS not_notices (
	NotCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL,
	CreatTime DATETIME NOT NULL,
	Content TEXT NOT NULL,
	Status TINYINT NOT NULL DEFAULT 0,
	NumNotif INT NOT NULL DEFAULT 0,
	UNIQUE INDEX(NotCod),
	INDEX(CrsCod,Status),
	INDEX(UsrCod),
	INDEX(CreatTime),
	INDEX(Status));
--
-- Table ntf_mail_domains: stores e-mail domains to which sending of notifications is allowed
--
CREATE TABLE IF NOT EXISTS ntf_mail_domains (
	MaiCod INT NOT NULL AUTO_INCREMENT,
	Domain VARCHAR(255) NOT NULL,
	Info VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(MaiCod),
	UNIQUE INDEX(Domain));
--
-- Table ntf_notifications: stores the notifications of events
--
CREATE TABLE IF NOT EXISTS ntf_notifications (
	NtfCod INT NOT NULL AUTO_INCREMENT,
	NotifyEvent TINYINT NOT NULL,
	ToUsrCod INT NOT NULL,
	FromUsrCod INT NOT NULL,
	InsCod INT NOT NULL DEFAULT -1,
	CtrCod INT NOT NULL DEFAULT -1,
	DegCod INT NOT NULL DEFAULT -1,
	CrsCod INT NOT NULL DEFAULT -1,
	Cod INT NOT NULL DEFAULT -1,
	TimeNotif DATETIME NOT NULL,
	Status TINYINT NOT NULL DEFAULT 0,
	UNIQUE INDEX(NtfCod),
	INDEX(NotifyEvent),
	INDEX(ToUsrCod),
	INDEX(TimeNotif));
--
-- Table pho_clicks_without_photo: stores the number of clicks that remains to each user before being required to submit his/her photo
--
CREATE TABLE IF NOT EXISTS pho_clicks_without_photo (
	UsrCod INT NOT NULL,
	NumClicks INT NOT NULL,
	UNIQUE INDEX(UsrCod));
--
-- Table plc_places: stores the places associated to each institution, used in holidays
--
CREATE TABLE IF NOT EXISTS plc_places (
	PlcCod INT NOT NULL AUTO_INCREMENT,
	InsCod INT NOT NULL,
	ShortName VARCHAR(511) NOT NULL,
	FullName VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(PlcCod),
	INDEX(InsCod));
--
-- Table plg_plugins: stores the plugins
--
CREATE TABLE IF NOT EXISTS plg_plugins (
	PlgCod INT NOT NULL AUTO_INCREMENT,
	Name VARCHAR(511) NOT NULL,
	Description VARCHAR(2047) NOT NULL,
	Logo VARCHAR(31) NOT NULL,
	AppKey VARCHAR(31) NOT NULL,
	URL VARCHAR(255) NOT NULL,
	IP CHAR(15) NOT NULL,
	UNIQUE INDEX(PlgCod));
--
-- Table prg_expanded: stores the items of the course program currently expanded for each user
--
CREATE TABLE IF NOT EXISTS prg_expanded (
	UsrCod INT NOT NULL,
	ItmCod INT NOT NULL,
	ClickTime DATETIME NOT NULL,
	UNIQUE INDEX(UsrCod,ItmCod),
	INDEX(ItmCod),
	INDEX(ClickTime));
--
-- Table prg_items: stores the items of the course program
--
CREATE TABLE IF NOT EXISTS prg_items (
	ItmCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	ItmInd INT NOT NULL DEFAULT 0,
	Level INT NOT NULL DEFAULT 1,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	Title VARCHAR(2047) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(ItmCod),
	UNIQUE INDEX(CrsCod,ItmInd));
--
-- Table prg_resources: stores the resources of the course program
--
CREATE TABLE IF NOT EXISTS prg_resources (
	RscCod INT NOT NULL AUTO_INCREMENT,
	ItmCod INT NOT NULL DEFAULT -1,
	RscInd INT NOT NULL DEFAULT 0,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	Type ENUM('non','asg','prj','cfe','exa','gam','rub','doc','mrk','att','for','svy') NOT NULL DEFAULT 'non',
	Title VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(RscCod),
	UNIQUE INDEX(ItmCod,RscInd));
--
-- Table prj_config: stores the configuration of projects for each course
--
CREATE TABLE IF NOT EXISTS prj_config (
	CrsCod INT NOT NULL DEFAULT -1,
	NETCanCreate ENUM('N','Y') NOT NULL DEFAULT 'Y',
	UNIQUE INDEX(CrsCod));
--
-- Table prj_projects: stores the projects proposed by the teachers to their students
--
CREATE TABLE IF NOT EXISTS prj_projects (
	PrjCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	DptCod INT NOT NULL DEFAULT -1,
	Locked ENUM('N','Y') NOT NULL DEFAULT 'N',
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	Assigned ENUM('N','Y') NOT NULL DEFAULT 'N',
	NumStds INT NOT NULL DEFAULT 1,
	Proposal ENUM('new','modified','unmodified') NOT NULL DEFAULT 'new',
	CreatTime DATETIME NOT NULL,
	ModifTime DATETIME NOT NULL,
	Title VARCHAR(4095) NOT NULL,
	Description TEXT NOT NULL,
	Knowledge TEXT NOT NULL,
	Materials TEXT NOT NULL,
	URL VARCHAR(255) NOT NULL,
	ReviewStatus ENUM('unreviewed','unapproved','approved') NOT NULL DEFAULT 'unreviewed',
	ReviewTime DATETIME NOT NULL DEFAULT '1970-01-01 01:00:00',
	ReviewTxt TEXT NOT NULL,
	UNIQUE INDEX(PrjCod),
	INDEX(CrsCod,Hidden),
	INDEX(CrsCod,CreatTime),
	INDEX(CrsCod,ModifTime),
	INDEX(CrsCod,DptCod),
	INDEX(CrsCod,ReviewStatus));
--
-- Table prj_rubrics: stores the rubrics for each project
--
CREATE TABLE IF NOT EXISTS prj_rubrics (
	CrsCod INT NOT NULL,
	Type ENUM('tut','evl','gbl') NOT NULL,
	RubCod INT NOT NULL,
	UNIQUE INDEX(CrsCod,Type,RubCod));
--
-- Table prj_scores: stores the rubric criteria scores for each project
--
CREATE TABLE IF NOT EXISTS prj_scores (
	PrjCod INT NOT NULL,
	CriCod INT NOT NULL,
	Score DOUBLE PRECISION NOT NULL DEFAULT 0,
	UNIQUE INDEX(PrjCod,CriCod));
--
-- Table prj_users: stores the users inside projects
--
CREATE TABLE IF NOT EXISTS prj_users (
	PrjCod INT NOT NULL,
	RoleInProject TINYINT NOT NULL DEFAULT 0,
	UsrCod INT NOT NULL,
	UNIQUE INDEX(PrjCod,RoleInProject,UsrCod));
--
-- Table roo_check_in: stores the history of locations of users
--
CREATE TABLE IF NOT EXISTS roo_check_in (
	ChkCod INT NOT NULL AUTO_INCREMENT,
	UsrCod INT NOT NULL,
	RooCod INT NOT NULL,
	CheckInTime DATETIME NOT NULL,
	UNIQUE INDEX(ChkCod),
	INDEX(UsrCod,CheckInTime),
	INDEX(CheckInTime));
--
-- Table roo_macs: stores the associations between rooms and MAC addresses
--
CREATE TABLE IF NOT EXISTS roo_macs (
	RooCod INT NOT NULL AUTO_INCREMENT,
	MAC BIGINT NOT NULL,
	UNIQUE INDEX(RooCod,MAC),
	UNIQUE INDEX(MAC,RooCod));
--
-- Table roo_rooms: stores the rooms associated to each center
--
CREATE TABLE IF NOT EXISTS roo_rooms (
	RooCod INT NOT NULL AUTO_INCREMENT,
	CtrCod INT NOT NULL,
	BldCod INT NOT NULL DEFAULT -1,
	Floor INT NOT NULL DEFAULT 0,
	Type ENUM('no_type','administration','auditorium','cafeteria','canteen','classroom','concierge','corridor','gym','hall','kindergarten','laboratory','library','office','outdoors','parking','pavilion','room','secretariat','seminar','shop','store','toilets','virtual','yard') NOT NULL DEFAULT 'no_type',
	ShortName VARCHAR(511) NOT NULL,
	FullName VARCHAR(2047) NOT NULL,
	Capacity INT NOT NULL,
	UNIQUE INDEX(RooCod),
	INDEX(CtrCod,BldCod,Floor));
--
-- Table rsc_clipboards: stores the clipboards for resources to be linked in program and rubrics
--
CREATE TABLE IF NOT EXISTS rsc_clipboards (
	UsrCod INT NOT NULL,
	CrsCod INT NOT NULL,
	Type ENUM('non','asg','prj','cfe','exa','gam','rub','doc','mrk','att','for','svy') NOT NULL DEFAULT 'non',
	Cod INT NOT NULL DEFAULT -1,
	CopyTime TIMESTAMP,
	UNIQUE INDEX(UsrCod,CrsCod,Type,Cod),
	INDEX(CrsCod,Type,Cod),
	INDEX(CopyTime));
--
-- Table rub_criteria: stores the criteria associated to each rubric
--
CREATE TABLE IF NOT EXISTS rub_criteria (
	CriCod INT NOT NULL AUTO_INCREMENT,
	RubCod INT NOT NULL,
	CriInd INT NOT NULL,
	Type ENUM('non','asg','prj','cfe','exa','gam','rub','doc','mrk','att','for','svy'),
	Cod INT NOT NULL DEFAULT -1,
	MinVal DOUBLE PRECISION NOT NULL DEFAULT 0,
	MaxVal DOUBLE PRECISION NOT NULL DEFAULT 1,
	Weight DOUBLE PRECISION NOT NULL DEFAULT 1,
	Title VARCHAR(2047) NOT NULL,
	UNIQUE INDEX(CriCod),
	UNIQUE INDEX(RubCod,CriInd));
--
-- Table rub_criteria: stores the assessment rubrics
--
CREATE TABLE IF NOT EXISTS rub_rubrics (
	RubCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL,
	Title VARCHAR(2047) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(RubCod),
	INDEX(CrsCod));
--
-- Table ses_params: stores some hidden parameters passed from a page to another using database instead of forms
--
CREATE TABLE IF NOT EXISTS ses_params (
	SessionId CHAR(43) NOT NULL,
	ParamName VARCHAR(255) NOT NULL,
	ParamValue LONGTEXT NOT NULL,
	INDEX(SessionId));
--
-- Table ses_sessions: stores the information of open sessions
--
CREATE TABLE IF NOT EXISTS ses_sessions (
	SessionId CHAR(43) NOT NULL,
	UsrCod INT NOT NULL,
	Password CHAR(86) COLLATE latin1_bin NOT NULL,
	Role TINYINT NOT NULL DEFAULT 0,
	CtyCod INT NOT NULL DEFAULT -1,
	InsCod INT NOT NULL DEFAULT -1,
	CtrCod INT NOT NULL DEFAULT -1,
	DegCod INT NOT NULL DEFAULT -1,
	CrsCod INT NOT NULL DEFAULT -1,
	LastTime DATETIME NOT NULL,
	LastRefresh DATETIME NOT NULL,
	FirstPubCod BIGINT NOT NULL DEFAULT 0,
	LastPubCod BIGINT NOT NULL DEFAULT 0,
	LastPageMsgRcv INT NOT NULL DEFAULT 1,
	LastPageMsgSnt INT NOT NULL DEFAULT 1,
	WhatToSearch TINYINT NOT NULL DEFAULT 0,
	SearchStr VARCHAR(2047) NOT NULL DEFAULT '',
	SideCols TINYINT NOT NULL DEFAULT 3,
	UNIQUE INDEX(SessionId),
	INDEX(UsrCod));
--
-- Table set_ip_settings: stores user's settings (preferences) for each IP address
--
CREATE TABLE IF NOT EXISTS set_ip_settings (
	IP CHAR(15) NOT NULL,
	UsrCod INT NOT NULL DEFAULT -1,
	LastChange DATETIME NOT NULL,
	FirstDayOfWeek TINYINT NOT NULL DEFAULT 0,
	DateFormat TINYINT NOT NULL DEFAULT 0,
	Theme CHAR(16) NOT NULL,
	IconSet CHAR(16) NOT NULL,
	Menu TINYINT NOT NULL DEFAULT 0,
	SideCols TINYINT NOT NULL,
	PhotoShape TINYINT NOT NULL DEFAULT 0,
	PRIMARY KEY(IP),
	INDEX(UsrCod),
	INDEX(LastChange));
--
-- Table sta_degrees: stores statistics about degrees
--
CREATE TABLE IF NOT EXISTS sta_degrees (
	DegCod INT NOT NULL DEFAULT -1,
	Sex ENUM('unknown','female','male','all') NOT NULL DEFAULT 'all',
	NumStds INT NOT NULL,
	NumStdsWithPhoto INT NOT NULL,
	TimeAvgPhoto DATETIME NOT NULL,
	TimeToComputeAvgPhoto INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(DegCod,Sex));
--
-- Table sta_notifications: stores statistics about notifications: number of notified events and number of e-mails sent
--
CREATE TABLE IF NOT EXISTS sta_notifications (
	DegCod INT NOT NULL,
	CrsCod INT NOT NULL,
	NotifyEvent TINYINT NOT NULL,
	NumEvents INT NOT NULL,
	NumMails INT NOT NULL,
	UNIQUE INDEX(DegCod,CrsCod,NotifyEvent));
--
-- Table svy_answers: stores the answers to the surveys
--
CREATE TABLE IF NOT EXISTS svy_answers (
	QstCod INT NOT NULL,
	AnsInd TINYINT NOT NULL,
	NumUsrs INT NOT NULL DEFAULT 0,
	Answer TEXT NOT NULL,
	UNIQUE INDEX(QstCod,AnsInd));
--
-- Table svy_groups: stores the groups associated to each survey
--
CREATE TABLE IF NOT EXISTS svy_groups (
	SvyCod INT NOT NULL,
	GrpCod INT NOT NULL,
	UNIQUE INDEX(SvyCod,GrpCod));
--
-- Table svy_questions: stores the questions in the surveys
--
CREATE TABLE IF NOT EXISTS svy_questions (
	QstCod INT NOT NULL AUTO_INCREMENT,
	SvyCod INT NOT NULL,
	QstInd INT NOT NULL DEFAULT 0,
	AnsType ENUM('unique_choice','multiple_choice') NOT NULL,
	Stem TEXT NOT NULL,
	UNIQUE INDEX(QstCod),
	INDEX(SvyCod));
--
-- Table svy_surveys: stores the surveys
--
CREATE TABLE IF NOT EXISTS svy_surveys (
	SvyCod INT NOT NULL AUTO_INCREMENT,
	Scope ENUM('Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Sys',
	Cod INT NOT NULL DEFAULT -1,
	Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',
	NumNotif INT NOT NULL DEFAULT 0,
	Roles INT NOT NULL DEFAULT 0,
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	Title VARCHAR(2047) NOT NULL,
	Txt TEXT NOT NULL,
	UNIQUE INDEX(SvyCod),
	INDEX(Scope,Cod));
--
-- Table svy_users: stores the users who have answer the surveys
--
CREATE TABLE IF NOT EXISTS svy_users (
	SvyCod INT NOT NULL,
	UsrCod INT NOT NULL,
	UNIQUE INDEX(SvyCod,UsrCod));
--
-- Table tmt_courses: stores the timetables of the courses
--
CREATE TABLE IF NOT EXISTS tmt_courses (
	CrsCod INT NOT NULL DEFAULT -1,
	GrpCod INT NOT NULL DEFAULT -1,
	Weekday TINYINT NOT NULL,
	StartTime TIME NOT NULL,
	Duration TIME NOT NULL,
	ClassType ENUM('free','lecture','practical') NOT NULL,
	Info VARCHAR(2047) NOT NULL DEFAULT '',
	INDEX(CrsCod,GrpCod));
--
-- Table tmt_tutoring: stores the timetables of tutoring hours of the teachers
--
CREATE TABLE IF NOT EXISTS tmt_tutoring (
	UsrCod INT NOT NULL,
	Weekday TINYINT NOT NULL,
	StartTime TIME NOT NULL,
	Duration TIME NOT NULL,
	Info VARCHAR(2047) NOT NULL DEFAULT '',
	INDEX(UsrCod));

--
-- Table tml_comments: stores the content of comments to timeline notes
--
CREATE TABLE IF NOT EXISTS tml_comments (
	PubCod BIGINT NOT NULL,
	Txt LONGTEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(PubCod),
	FULLTEXT(Txt),
	INDEX(MedCod)) ENGINE = MYISAM;
--
-- Table tml_comments_fav: stores users who marked timeline comments as favourite
--
CREATE TABLE IF NOT EXISTS tml_comments_fav (
	FavCod BIGINT AUTO_INCREMENT,
	PubCod BIGINT NOT NULL,
	UsrCod INT NOT NULL,
	TimeFav DATETIME NOT NULL,
	UNIQUE INDEX(FavCod),
	UNIQUE INDEX(PubCod,UsrCod),
	INDEX(UsrCod));
--
-- Table tml_notes: stores timeline notes
--
CREATE TABLE IF NOT EXISTS tml_notes (
	NotCod BIGINT NOT NULL AUTO_INCREMENT,
	NoteType TINYINT NOT NULL,
	Cod INT NOT NULL DEFAULT -1,
	UsrCod INT NOT NULL,
	HieCod INT NOT NULL DEFAULT -1,
	Unavailable ENUM('N','Y') NOT NULL DEFAULT 'N',
	TimeNote DATETIME NOT NULL,
	UNIQUE INDEX(NotCod),
	INDEX(NoteType,Cod),
	INDEX(UsrCod),
	INDEX(TimeNote));
--
-- Table tml_notes_fav: stores users who marked timeline notes as favourite
--
CREATE TABLE IF NOT EXISTS tml_notes_fav (
	FavCod BIGINT AUTO_INCREMENT,
	NotCod BIGINT NOT NULL,
	UsrCod INT NOT NULL,
	TimeFav DATETIME NOT NULL,
	UNIQUE INDEX(FavCod),
	UNIQUE INDEX(NotCod,UsrCod),
	INDEX(UsrCod));
--
-- Table tml_posts: stores timeline posts (public comments written by users)
--
CREATE TABLE IF NOT EXISTS tml_posts (
	PstCod INT NOT NULL AUTO_INCREMENT,
	Txt LONGTEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	UNIQUE INDEX(PstCod),
	FULLTEXT(Content),
	INDEX(MedCod)) ENGINE = MYISAM;
--
-- Table tml_pubs: stores timeline publications (original notes, sharede notes or comments)
--
CREATE TABLE IF NOT EXISTS tml_pubs (
	PubCod BIGINT NOT NULL AUTO_INCREMENT,
	NotCod BIGINT NOT NULL,
	PublisherCod INT NOT NULL,
	PubType TINYINT NOT NULL,
	TimePublish DATETIME NOT NULL,
	UNIQUE INDEX(PubCod),
	INDEX(NotCod,PublisherCod,PubType),
	INDEX(PublisherCod),
	INDEX(PubType),
	INDEX(TimePublish));
--
-- Table tml_timelines: stores notes published in timeline for every active session
--
CREATE TABLE IF NOT EXISTS tml_timelines (
	SessionId CHAR(43) NOT NULL,
	NotCod BIGINT NOT NULL,
	UNIQUE INDEX(SessionId,NotCod));
--
-- Table tst_answers: stores the answers to the questions in tests
--
CREATE TABLE IF NOT EXISTS tst_answers (
	QstCod INT NOT NULL,
	AnsInd TINYINT NOT NULL,
	Answer TEXT NOT NULL,
	Feedback TEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	Correct ENUM('N','Y') NOT NULL,
	INDEX(QstCod),
	INDEX(MedCod));
--
-- Table tst_config: stores the configuration of tests for each course
--
CREATE TABLE IF NOT EXISTS tst_config (
	CrsCod INT NOT NULL DEFAULT -1,
	Pluggable ENUM('unknown','N','Y') NOT NULL DEFAULT 'unknown',
	Min INT NOT NULL,
	Def INT NOT NULL,
	Max INT NOT NULL,
	MinTimeNxtTstPerQst INT NOT NULL DEFAULT 0,
	Visibility INT NOT NULL DEFAULT 0x1f,
	UNIQUE INDEX(CrsCod));
--
-- Table tst_exam_questions: stores the questions and answers in test prints made by users
--
CREATE TABLE IF NOT EXISTS tst_exam_questions (
	ExaCod INT NOT NULL,
	QstCod INT NOT NULL,
	QstInd INT NOT NULL,
	Score DOUBLE PRECISION NOT NULL DEFAULT 0,
	Indexes TEXT NOT NULL,
	Answers TEXT NOT NULL,
	UNIQUE INDEX(ExaCod,QstCod));
--
-- Table tst_exams: stores the test exams made by users
--
CREATE TABLE IF NOT EXISTS tst_exams (
	ExaCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL,
	UsrCod INT NOT NULL,
	StartTime DATETIME NOT NULL,
	EndTime DATETIME NOT NULL,
	NumQsts INT NOT NULL DEFAULT 0,
	NumQstsNotBlank INT NOT NULL DEFAULT 0,
	Sent ENUM('N','Y') NOT NULL DEFAULT 'N',
	AllowTeachers ENUM('N','Y') NOT NULL DEFAULT 'N',
	Score DOUBLE PRECISION NOT NULL DEFAULT 0,
	UNIQUE INDEX(ExaCod),
	INDEX(CrsCod,UsrCod));
--
-- Table tst_question_tags: stores the tags associated to each test question
--
CREATE TABLE IF NOT EXISTS tst_question_tags (
	QstCod INT NOT NULL,
	TagCod INT NOT NULL,
	TagInd TINYINT NOT NULL,
	UNIQUE INDEX(QstCod,TagCod));
--
-- Table tst_questions: stores the test questions
--
CREATE TABLE IF NOT EXISTS tst_questions (
	QstCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	EditTime DATETIME NOT NULL,
	AnsType ENUM ('int','float','true_false','unique_choice','multiple_choice','text') NOT NULL,
	Shuffle ENUM('N','Y') NOT NULL,
	Stem TEXT NOT NULL,
	Feedback TEXT NOT NULL,
	MedCod INT NOT NULL DEFAULT -1,
	NumHits INT NOT NULL DEFAULT 0,
	NumHitsNotBlank INT NOT NULL DEFAULT 0,
	Score DOUBLE PRECISION NOT NULL DEFAULT 0,
	UNIQUE INDEX(QstCod),
	INDEX(CrsCod,EditTime),
	INDEX(MedCod));
--
-- Table tst_tags: stores the tags of test questions
--
CREATE TABLE IF NOT EXISTS tst_tags (
	TagCod INT NOT NULL AUTO_INCREMENT,
	CrsCod INT NOT NULL DEFAULT -1,
	ChangeTime DATETIME NOT NULL,
	TagTxt VARCHAR(2047) NOT NULL,
	TagHidden ENUM('N','Y') NOT NULL,
	UNIQUE INDEX(TagCod),
	INDEX(CrsCod,ChangeTime));
--
-- Table usr_admins: stores users who manage degrees, centres, institutions and system
--
CREATE TABLE IF NOT EXISTS usr_admins (
	UsrCod INT NOT NULL,
	Scope ENUM('Sys','Ins','Ctr','Deg') NOT NULL,
	Cod INT NOT NULL,
	UNIQUE INDEX(UsrCod,Scope,Cod),
	INDEX(Scope,Cod));
--
-- Table usr_banned: stores users banned for ranking
--
CREATE TABLE IF NOT EXISTS usr_banned (
	UsrCod INT NOT NULL,
	UNIQUE INDEX(UsrCod));
--
-- Table usr_birthdays_today: stores birthdays already congratulated today
--
CREATE TABLE IF NOT EXISTS usr_birthdays_today (
	UsrCod INT NOT NULL,
	Today DATE NOT NULL,
	UNIQUE INDEX(UsrCod),
	INDEX(Today));
--
-- Table usr_connected: users currently connected to the platform
--
CREATE TABLE IF NOT EXISTS usr_connected (
	UsrCod INT NOT NULL,
	RoleInLastCrs TINYINT NOT NULL DEFAULT 0,
	LastCrsCod INT NOT NULL DEFAULT -1,
	LastTime DATETIME NOT NULL,
	UNIQUE INDEX(UsrCod),
	INDEX(RoleInLastCrs),
	INDEX(LastCrsCod));
--
-- Table usr_data: stores users' data
--
CREATE TABLE IF NOT EXISTS usr_data (
	UsrCod INT NOT NULL AUTO_INCREMENT,
	EncryptedUsrCod CHAR(43) NOT NULL DEFAULT '',
	Password CHAR(86) COLLATE latin1_bin NOT NULL DEFAULT '',
	Surname1 VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',
	Surname2 VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',
	FirstName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',
	Sex ENUM ('unknown','female','male') NOT NULL DEFAULT 'unknown',
	Theme CHAR(16) NOT NULL DEFAULT '',
	IconSet CHAR(16) NOT NULL DEFAULT '',
	Language CHAR(2) NOT NULL DEFAULT '',
	FirstDayOfWeek TINYINT NOT NULL DEFAULT 0,
	DateFormat TINYINT NOT NULL DEFAULT 0,
	Photo CHAR(43) NOT NULL DEFAULT '',
	PhotoVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',
	BaPrfVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',
	ExPrfVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',
	CtyCod INT NOT NULL DEFAULT -1,
	InsCtyCod INT NOT NULL DEFAULT -1,
	InsCod INT NOT NULL DEFAULT -1,
	DptCod INT NOT NULL DEFAULT -1,
	CtrCod INT NOT NULL DEFAULT -1,
	Office VARCHAR(2047) NOT NULL DEFAULT '',
	OfficePhone CHAR(16) NOT NULL DEFAULT '',
	LocalAddress VARCHAR(2047) NOT NULL DEFAULT '',
	LocalPhone CHAR(16) NOT NULL DEFAULT '',
	FamilyAddress VARCHAR(2047) NOT NULL DEFAULT '',
	FamilyPhone CHAR(16) NOT NULL DEFAULT '',
	OriginPlace VARCHAR(2047) NOT NULL DEFAULT '',
	Birthday DATE,
	Comments TEXT NOT NULL,
	Menu TINYINT NOT NULL DEFAULT 0,
	SideCols TINYINT NOT NULL DEFAULT 3,
	PhotoShape TINYINT NOT NULL DEFAULT 0,
	ThirdPartyCookies ENUM('N','Y') NOT NULL DEFAULT 'N',
	NotifNtfEvents INT NOT NULL DEFAULT 0,
	EmailNtfEvents INT NOT NULL DEFAULT 0,
	PRIMARY KEY(UsrCod),
	UNIQUE INDEX(EncryptedUsrCod),
	INDEX(Theme),
	INDEX(IconSet),
	INDEX(Language),
	INDEX(FirstDayOfWeek),
	INDEX(DateFormat),
	INDEX(PhotoVisibility),
	INDEX(BaPrfVisibility),
	INDEX(ExPrfVisibility),
	INDEX(CtyCod),
	INDEX(InsCtyCod),
	INDEX(InsCod),
	INDEX(DptCod),
	INDEX(CtrCod),
	INDEX(Menu),
	INDEX(SideCols),
	INDEX(PhotoShape),
	INDEX(ThirdPartyCookies));
--
-- Table usr_duplicated: stores informs of users possibly duplicated
--
CREATE TABLE IF NOT EXISTS usr_duplicated (
	UsrCod INT NOT NULL,
	InformerCod INT NOT NULL,
	InformTime DATETIME NOT NULL,
	UNIQUE INDEX(UsrCod,InformerCod),
	INDEX(UsrCod));
--
-- Table usr_emails: stores the users' e-mails
--
CREATE TABLE IF NOT EXISTS usr_emails (
	UsrCod INT NOT NULL,
	E_mail VARCHAR(255) COLLATE latin1_general_ci NOT NULL,
	CreatTime DATETIME NOT NULL,
	Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(UsrCod,E_mail),
	UNIQUE INDEX(E_mail));
--
-- Table usr_figures: stores some figures (numbers) related to users to show in public profile
--
CREATE TABLE IF NOT EXISTS usr_figures (
	UsrCod INT NOT NULL,
	FirstClickTime DATETIME NOT NULL,
	NumClicks INT NOT NULL DEFAULT -1,
	NumSocPub INT NOT NULL DEFAULT -1,
	NumFileViews INT NOT NULL DEFAULT -1,
	NumForPst INT NOT NULL DEFAULT -1,
	NumMsgSnt INT NOT NULL DEFAULT -1,
	PRIMARY KEY(UsrCod),
	INDEX(FirstClickTime),
	INDEX(NumClicks));
--
-- Table usr_follow: stores followers and followed
--
CREATE TABLE IF NOT EXISTS usr_follow (
	FollowerCod INT NOT NULL,
	FollowedCod INT NOT NULL,
	FollowTime DATETIME NOT NULL,
	UNIQUE INDEX (FollowerCod,FollowedCod),
	UNIQUE INDEX (FollowedCod,FollowerCod),
	INDEX (FollowTime));
--
-- Table usr_ids: stores the users' IDs
--
CREATE TABLE IF NOT EXISTS usr_ids (
	UsrCod INT NOT NULL,
	UsrID CHAR(16) NOT NULL,
	CreatTime DATETIME NOT NULL,
	Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N',
	UNIQUE INDEX(UsrCod,UsrID),
	INDEX(UsrID));
--
-- Table usr_last: stores some variable data related to users
--
CREATE TABLE IF NOT EXISTS usr_last (
	UsrCod INT NOT NULL,
	WhatToSearch TINYINT NOT NULL DEFAULT 0,
	LastSco ENUM('Unk','Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Unk',
	LastCod INT NOT NULL DEFAULT -1,
	LastAct INT NOT NULL DEFAULT -1,
	LastRole TINYINT NOT NULL DEFAULT 0,
	LastTime DATETIME NOT NULL,
	LastAccNotif DATETIME NOT NULL,
	TimelineUsrs TINYINT NOT NULL DEFAULT 0,
	UNIQUE INDEX(UsrCod),
	INDEX(LastTime));
--
-- Table usr_nicknames: stores users' nicknames
--
CREATE TABLE IF NOT EXISTS usr_nicknames (
	UsrCod INT NOT NULL,
	Nickname CHAR(16) COLLATE latin1_spanish_ci NOT NULL,
	CreatTime DATETIME NOT NULL,
	UNIQUE INDEX(UsrCod,Nickname),
	UNIQUE INDEX(Nickname));
--
-- Table usr_pending_emails: stores the emails pending of confirmation
--
CREATE TABLE IF NOT EXISTS usr_pending_emails (
	UsrCod INT NOT NULL,
	E_mail VARCHAR(255) COLLATE latin1_general_ci NOT NULL,
	MailKey CHAR(43) COLLATE latin1_bin NOT NULL,
	DateAndTime DATETIME NOT NULL,
	INDEX(UsrCod),
	UNIQUE INDEX(MailKey));
--
-- Table usr_pending_passwd: stores the passwords pending of activation, sent by e-mail when a user who have forgotten his/her password request a new one
--
CREATE TABLE IF NOT EXISTS usr_pending_passwd (
	UsrCod INT NOT NULL,
	PendingPassword CHAR(86) COLLATE latin1_bin NOT NULL,
	DateAndTime DATETIME NOT NULL,
	PRIMARY KEY(UsrCod));
--
-- Table usr_reports: stores users' usage reports
--
CREATE TABLE IF NOT EXISTS usr_reports (
	RepCod INT NOT NULL AUTO_INCREMENT,
	UsrCod INT NOT NULL,
	ReportTimeUTC DATETIME NOT NULL,
	UniqueDirL CHAR(2) NOT NULL,
	UniqueDirR CHAR(41) NOT NULL,
	Filename VARCHAR(255) NOT NULL,
	Permalink VARCHAR(255) NOT NULL,
	UNIQUE INDEX(RepCod),
	INDEX(UsrCod));
--
-- Table usr_webs: stores users' web and social networks
--
CREATE TABLE IF NOT EXISTS usr_webs (
	UsrCod INT NOT NULL,
	Web ENUM(
		'www',
		'500px',
		'delicious','deviantart','diaspora',
		'edmodo',
		'facebook','flickr','foursquare',
		'github','gnusocial','googleplus','googlescholar',
		'identica','instagram',
		'linkedin',
		'orcid',
		'paperli','pinterest',
		'researchgate','researcherid',
		'scoopit','slideshare','stackoverflow','storify',
		'tumblr','twitter',
		'wikipedia',
		'youtube') NOT NULL,
	URL VARCHAR(255) NOT NULL,
	UNIQUE INDEX(UsrCod,Web));
