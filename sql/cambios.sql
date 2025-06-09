RENAME TABLE tbl_name TO new_tbl_name;

RENAME TABLE accesos TO log;
ALTER TABLE log CHANGE CodTit DegCod VARCHAR(16) NOT NULL;

CREATE TABLE deg_types (DegTypCod INT NOT NULL AUTO_INCREMENT, DegTypName VARCHAR(255) NOT NULL, UNIQUE INDEX(DegTypCod));
DESCRIBE deg_types;
CREATE TABLE degrees (DegCod VARCHAR(16) NOT NULL, DegTypCod INT NOT NULL, DegName VARCHAR(255) NOT NULL, UNIQUE INDEX(DegCod), INDEX(DegTypCod));
DESCRIBE degrees;

ALTER TABLE degrees CHANGE DegName FullName VARCHAR(255) NOT NULL;
ALTER TABLE degrees ADD COLUMN ShortName VARCHAR(32) NOT NULL AFTER DegTypCod;
ALTER TABLE degrees ADD COLUMN CampusCod INT NOT NULL AFTER DegTypCod;
ALTER TABLE degrees ADD COLUMN FirstYear TINYINT NOT NULL DEFAULT 0 AFTER FullName;
ALTER TABLE degrees ADD COLUMN LastYear TINYINT NOT NULL DEFAULT 0 AFTER FirstYear;
ALTER TABLE degrees ADD COLUMN OptYear ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER LastYear;
ALTER TABLE degrees ADD COLUMN Logo VARCHAR(16) NOT NULL AFTER OptYear;
ALTER TABLE degrees ADD COLUMN WWW VARCHAR(255) NOT NULL AFTER Logo;

SELECT messages.MsgCod,DATE_FORMAT(messages.CreatTime,'%Y%m%d%H%i%S') AS F,messages.AuthorDNI,messages.Location,messages.Subject,messages.Content,msg_received.Replied FROM msg_received,messages WHERE msg_received.DstDNI='24243619' AND msg_received.MsgCod=messages.MsgCod ORDER BY F DESC;

INSERT INTO apodos (DNI,Apodo) VALUES ('74641103','mataran');

CREATE TABLE courses (CrsCod INT NOT NULL AUTO_INCREMENT, DegCod INT NOT NULL DEFAULT -1, Semester TINYINT NOT NULL DEFAULT 0, ShortName VARCHAR(32) NOT NULL, FullName VARCHAR(255) NOT NULL, UNIQUE INDEX(CrsCod), INDEX(DegCod), INDEX(Semester));

SELECT DISTINCT(messages.Location) FROM messages,msg_received WHERE msg_received.DstDNI='27518991' AND msg_received.MsgCod=messages.MsgCod ORDER BY messages.Location;

SELECT msg_received.MsgCod,messages.CreatTime AS F FROM msg_received,messages WHERE msg_received.DstDNI='24243619' AND msg_received.MsgCod=messages.MsgCod ORDER BY F DESC;

DROP INDEX DstDNI ON msg_received;
CREATE INDEX DstDNI_MsgCod ON msg_received (DstDNI,MsgCod);

SELECT messages.MsgCod AS M FROM messages,msg_sent WHERE messages.AuthorDNI='24243619' AND messages.MsgCod=msg_sent.MsgCod GROUP BY M ORDER BY M DESC;

SELECT DISTINCT(asg_usr.DNI,asg_usr.Accepted) FROM asg_usr,fichas_profesor WHERE asg_usr.DNI=fichas_profesor.DNI;

ALTER TABLE courses ADD COLUMN NumTeachers INT NOT NULL DEFAULT 0 AFTER FullName;
ALTER TABLE courses ADD COLUMN NumStudents INT NOT NULL DEFAULT 0 AFTER NumTeachers;

SELECT CrsCod,Year,Semester,ShortName,FullName,NumTeachers,NumStudents FROM courses WHERE DegCod='11' ORDER BY Year,ShortName;

ALTER TABLE sessions CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE sessions ADD COLUMN CrsCod INT NOT NULL AFTER Password;
ALTER TABLE sessions CHANGE CrsCod CrsCod INT NOT NULL DEFAULT -1;

ALTER TABLE connected CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE connected ADD COLUMN CrsCod INT NOT NULL AFTER DNI;
ALTER TABLE connected CHANGE CrsCod CrsCod INT NOT NULL DEFAULT -1;
ALTER TABLE connected ADD INDEX (CrsCod);

ALTER TABLE log CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE log ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER DegCod;
ALTER TABLE log ADD INDEX (CrsCod);
ALTER TABLE log CHANGE DegCod DegCod_old CHAR(16) NOT NULL;

ALTER TABLE crs_info_src CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE crs_info_src ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE crs_info_src ADD INDEX (CrsCod);

ALTER TABLE crs_info_txt CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE crs_info_txt ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE crs_info_txt ADD INDEX (CrsCod);

ALTER TABLE asg_usr CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE asg_usr ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE asg_usr ADD INDEX (CrsCod,DNI);

ALTER TABLE crs_grp_types CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE crs_grp_types ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER GrpTypCod;
ALTER TABLE crs_grp_types ADD INDEX (CrsCod);

ALTER TABLE crs_card_fields CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE crs_card_fields ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER CodCampo;
ALTER TABLE crs_card_fields ADD INDEX (CrsCod);

ALTER TABLE messages CHANGE Location Location_old CHAR(16) NOT NULL;
ALTER TABLE messages ADD COLUMN Location INT NOT NULL DEFAULT -1 AFTER MsgCod;
ALTER TABLE messages ADD INDEX (Location);

ALTER TABLE msg_notices CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE msg_notices ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER MsgCod;
ALTER TABLE msg_notices ADD INDEX (CrsCod);

ALTER TABLE forum_thread CHANGE Location Location_old CHAR(16) NOT NULL;
ALTER TABLE forum_thread ADD COLUMN Location INT NOT NULL DEFAULT -1 AFTER ForumType;
ALTER TABLE forum_thread ADD INDEX (Location);

ALTER TABLE crs_common_files CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE crs_common_files ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE crs_common_files ADD INDEX (CrsCod,CodGrupo);

ALTER TABLE calificaciones CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE calificaciones ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE calificaciones ADD INDEX (CrsCod,GrpCod);

ALTER TABLE convocatorias CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE convocatorias ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER CodConv;
ALTER TABLE convocatorias ADD INDEX (CrsCod);

ALTER TABLE hidden_downloads CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE hidden_downloads ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE hidden_downloads ADD INDEX (CrsCod,GrpCod);

ALTER TABLE expanded_folders CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE expanded_folders ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER TreeType;
ALTER TABLE expanded_folders ADD INDEX (CrsCod);

ALTER TABLE timetable_crs CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE timetable_crs ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE timetable_crs ADD INDEX (CrsCod,GrpCod);

ALTER TABLE portapapeles CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE portapapeles ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER Tipo;
ALTER TABLE portapapeles ADD INDEX (CrsCod);

ALTER TABLE tst_config CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE tst_config ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 FIRST;
ALTER TABLE tst_config ADD INDEX (CrsCod);

ALTER TABLE tst_questions CHANGE CodAsg FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE tst_questions ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER CodPrg;
ALTER TABLE tst_questions ADD INDEX (CrsCod);

ALTER TABLE tst_tags CHANGE CrsCod FullCrsCod_old CHAR(16) NOT NULL;
ALTER TABLE tst_tags ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER TagCod;
ALTER TABLE tst_tags ADD INDEX (CrsCod);

ALTER TABLE connected ADD COLUMN UsrType TINYINT NOT NULL AFTER DNI;

SELECT DISTINCT(connected.DNI,connected.UsrType) FROM connected,asg_usr WHERE asg_usr.FullCrsCod_old='PID00SWAD' AND asg_usr.DNI=connected.DNI;

ALTER TABLE courses ADD COLUMN FullCrsCod_old CHAR(16) NOT NULL AFTER CrsCod;

CREATE UNIQUE INDEX FullCrsCod_old ON courses (FullCrsCod_old);

CREATE UNIQUE INDEX DegCod_old ON degrees (DegCod_old);

ALTER TABLE sessions DROP COLUMN FullCrsCod_old;

SELECT sessions.DNI,sessions.Password,sessions.CrsCod,courses.FullCrsCod_old FROM sessions,courses WHERE sessions.IdSession='bbPI3AhWYE05FWN82-sOnw' AND sessions.CrsCod=courses.CrsCod;

ALTER TABLE connected DROP COLUMN FullCrsCod_old;
ALTER TABLE portapapeles DROP COLUMN FullCrsCod_old;

UPDATE timetable_crs,courses SET timetable_crs.CrsCod=courses.CrsCod WHERE timetable_crs.FullCrsCod_old=courses.FullCrsCod_old;
ALTER TABLE timetable_crs DROP COLUMN FullCrsCod_old;

UPDATE asg_usr,courses SET asg_usr.CrsCod=courses.CrsCod WHERE asg_usr.FullCrsCod_old=courses.FullCrsCod_old;

SELECT COUNT(DISTINCT(asg_usr.DNI)) FROM degrees,asg_usr,fichas_alumno WHERE degrees.DegCod_old='II' AND degrees.DegCod=courses.DegCod AND courses.CrsCod=asg_usr.CrsCod AND asg_usr.DNI=fichas_alumno.DNI;
SELECT COUNT(DISTINCT(asg_usr.DNI)) FROM courses,asg_usr,fichas_alumno WHERE courses.DegCod='57' AND courses.CrsCod=asg_usr.CrsCod AND asg_usr.DNI=fichas_alumno.DNI;
SELECT DISTINCT(asg_usr.DNI),'Y' FROM courses,asg_usr,fichas_profesor WHERE courses.DegCod='57' AND courses.CrsCod=asg_usr.CrsCod AND asg_usr.Accepted='Y' AND asg_usr.DNI=fichas_profesor.DNI;
SELECT DISTINCT(asg_usr.DNI) FROM courses,asg_usr,fichas_alumno WHERE courses.DegCod='57' AND courses.CrsCod=asg_usr.CrsCod AND asg_usr.DNI=fichas_alumno.DNI;

ALTER TABLE asg_usr DROP COLUMN FullCrsCod_old;

UPDATE crs_common_files,courses SET crs_common_files.CrsCod=courses.CrsCod WHERE crs_common_files.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE crs_common_files ADD COLUMN Path2 TEXT COLLATE latin1_bin NOT NULL AFTER Path;
ALTER TABLE crs_common_files CHANGE Path Path TEXT COLLATE latin1_bin NOT NULL;
ALTER TABLE crs_common_files DROP COLUMN Path2;

ALTER TABLE portapapeles CHANGE Path Path TEXT COLLATE latin1_bin NOT NULL;
ALTER TABLE expanded_folders CHANGE Path Path TEXT COLLATE latin1_bin NOT NULL;
ALTER TABLE hidden_downloads CHANGE Path Path TEXT COLLATE latin1_bin NOT NULL;
ALTER TABLE calificaciones CHANGE Path Path TEXT COLLATE latin1_bin NOT NULL;

UPDATE crs_grp_types,courses SET crs_grp_types.CrsCod=courses.CrsCod WHERE crs_grp_types.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE crs_common_files DROP COLUMN FullCrsCod_old;
ALTER TABLE crs_grp_types DROP COLUMN FullCrsCod_old;

UPDATE calificaciones,courses SET calificaciones.CrsCod=courses.CrsCod WHERE calificaciones.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE calificaciones DROP COLUMN FullCrsCod_old;

UPDATE crs_card_fields,courses SET crs_card_fields.CrsCod=courses.CrsCod WHERE crs_card_fields.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE crs_card_fields DROP COLUMN FullCrsCod_old;

UPDATE expanded_folders,courses SET expanded_folders.CrsCod=courses.CrsCod WHERE expanded_folders.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE expanded_folders DROP COLUMN FullCrsCod_old;

UPDATE hidden_downloads,courses SET hidden_downloads.CrsCod=courses.CrsCod WHERE hidden_downloads.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE hidden_downloads DROP COLUMN FullCrsCod_old;

UPDATE tst_config,courses SET tst_config.CrsCod=courses.CrsCod WHERE tst_config.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE tst_config DROP COLUMN FullCrsCod_old;

UPDATE tst_questions,courses SET tst_questions.CrsCod=courses.CrsCod WHERE tst_questions.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE tst_questions DROP COLUMN FullCrsCod_old;

UPDATE tst_tags,courses SET tst_tags.CrsCod=courses.CrsCod WHERE tst_tags.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE tst_tags DROP COLUMN FullCrsCod_old;

UPDATE convocatorias,courses SET convocatorias.CrsCod=courses.CrsCod WHERE convocatorias.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE convocatorias DROP COLUMN FullCrsCod_old;

UPDATE crs_info_src,courses SET crs_info_src.CrsCod=courses.CrsCod WHERE crs_info_src.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE crs_info_src DROP COLUMN FullCrsCod_old;

UPDATE crs_info_txt,courses SET crs_info_txt.CrsCod=courses.CrsCod WHERE crs_info_txt.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE crs_info_txt DROP COLUMN FullCrsCod_old;

ALTER TABLE messages ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER Location;

ALTER TABLE messages ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER Location;
ALTER TABLE messages ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER DegCod;
UPDATE messages,courses SET messages.CrsCod=courses.CrsCod WHERE messages.Location_old=courses.FullCrsCod_old;

ALTER TABLE messages DROP COLUMN Location;
ALTER TABLE messages DROP COLUMN DegCod;
ALTER TABLE messages DROP COLUMN Location_old;

UPDATE msg_notices,courses SET msg_notices.CrsCod=courses.CrsCod WHERE msg_notices.FullCrsCod_old=courses.FullCrsCod_old;

ALTER TABLE msg_notices DROP COLUMN FullCrsCod_old;

SELECT DISTINCT(messages.CrsCod,courses.ShortName) FROM messages,msg_sent,courses WHERE messages.AuthorDNI='24243619' AND messages.MsgCod=msg_sent.MsgCod AND messages.CrsCod=courses.CrsCod ORDER BY courses.ShortName;

ALTER TABLE forum_thread ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER Location;
ALTER TABLE forum_thread ADD COLUMN CrsCod INT NOT NULL DEFAULT -1 AFTER DegCod;

UPDATE forum_thread,courses SET forum_thread.CrsCod=courses.CrsCod WHERE forum_thread.Location_old=courses.FullCrsCod_old;
UPDATE forum_thread,degrees SET forum_thread.DegCod=degrees.DegCod WHERE forum_thread.Location_old=degrees.DegCod_old;
UPDATE forum_thread,courses SET forum_thread.Location=courses.CrsCod WHERE forum_thread.Location_old=courses.FullCrsCod_old;
UPDATE forum_thread,degrees SET forum_thread.Location=degrees.DegCod WHERE forum_thread.Location_old=degrees.DegCod_old;

ALTER TABLE forum_thread DROP COLUMN DegCod;
ALTER TABLE forum_thread DROP COLUMN CrsCod;
ALTER TABLE forum_thread DROP COLUMN Location_old;

ALTER TABLE log ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER DegCod_old;
CREATE INDEX DegCod ON log (DegCod);





UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FullCrsCod_old=courses.FullCrsCod_old;

UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20071201000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20071101000000' AND log.FechaHora<='20071202000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20071001000000' AND log.FechaHora<='20071102000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070901000000' AND log.FechaHora<='20071002000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070801000000' AND log.FechaHora<='20070902000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070701000000' AND log.FechaHora<='20070802000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070601000000' AND log.FechaHora<='20070702000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070501000000' AND log.FechaHora<='20070602000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070401000000' AND log.FechaHora<='20070502000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070301000000' AND log.FechaHora<='20070402000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070201000000' AND log.FechaHora<='20070302000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20070101000000' AND log.FechaHora<='20070202000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;

UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20061201000000' AND log.FechaHora<='20070102000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20061101000000' AND log.FechaHora<='20061202000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20061001000000' AND log.FechaHora<='20061102000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060901000000' AND log.FechaHora<='20061002000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060801000000' AND log.FechaHora<='20060902000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060701000000' AND log.FechaHora<='20060802000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060601000000' AND log.FechaHora<='20060702000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060501000000' AND log.FechaHora<='20060602000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060401000000' AND log.FechaHora<='20060502000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060301000000' AND log.FechaHora<='20060402000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060201000000' AND log.FechaHora<='20060302000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20060101000000' AND log.FechaHora<='20060202000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;

UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20051201000000' AND log.FechaHora<='20060102000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20051101000000' AND log.FechaHora<='20051202000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20051001000000' AND log.FechaHora<='20051102000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050901000000' AND log.FechaHora<='20051002000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050801000000' AND log.FechaHora<='20050902000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050701000000' AND log.FechaHora<='20050802000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050601000000' AND log.FechaHora<='20050702000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050501000000' AND log.FechaHora<='20050602000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050401000000' AND log.FechaHora<='20050502000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050301000000' AND log.FechaHora<='20050402000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20050201000000' AND log.FechaHora<='20050302000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora<='20050202000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;






UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.DegCod_old=degrees.DegCod_old;

UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20071201000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20071101000000' AND log.FechaHora<='20071202000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20071001000000' AND log.FechaHora<='20071102000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070901000000' AND log.FechaHora<='20071002000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070801000000' AND log.FechaHora<='20070902000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070701000000' AND log.FechaHora<='20070802000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070601000000' AND log.FechaHora<='20070702000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070501000000' AND log.FechaHora<='20070602000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070401000000' AND log.FechaHora<='20070502000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070301000000' AND log.FechaHora<='20070402000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070201000000' AND log.FechaHora<='20070302000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20070101000000' AND log.FechaHora<='20070202000000' AND log.DegCod_old=degrees.DegCod_old;

UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20061201000000' AND log.FechaHora<='20070102000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20061101000000' AND log.FechaHora<='20061202000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20061001000000' AND log.FechaHora<='20061102000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060901000000' AND log.FechaHora<='20061002000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060801000000' AND log.FechaHora<='20060902000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060701000000' AND log.FechaHora<='20060802000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060601000000' AND log.FechaHora<='20060702000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060501000000' AND log.FechaHora<='20060602000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060401000000' AND log.FechaHora<='20060502000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060301000000' AND log.FechaHora<='20060402000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060201000000' AND log.FechaHora<='20060302000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20060101000000' AND log.FechaHora<='20060202000000' AND log.DegCod_old=degrees.DegCod_old;

UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20051201000000' AND log.FechaHora<='20060102000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20051101000000' AND log.FechaHora<='20051202000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20051001000000' AND log.FechaHora<='20051102000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050901000000' AND log.FechaHora<='20051002000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050801000000' AND log.FechaHora<='20050902000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050701000000' AND log.FechaHora<='20050802000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050601000000' AND log.FechaHora<='20050702000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050501000000' AND log.FechaHora<='20050602000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050401000000' AND log.FechaHora<='20050502000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050301000000' AND log.FechaHora<='20050402000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20050201000000' AND log.FechaHora<='20050302000000' AND log.DegCod_old=degrees.DegCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora<='20050202000000' AND log.DegCod_old=degrees.DegCod_old;



UPDATE log,courses SET log.CrsCod=courses.CrsCod WHERE log.FechaHora>='20071201000000' AND log.FullCrsCod_old=courses.FullCrsCod_old;
UPDATE log,degrees SET log.DegCod=degrees.DegCod WHERE log.FechaHora>='20071201000000' AND log.DegCod_old=degrees.DegCod_old;

SELECT FullCrsCod_old,CrsCod_old,DegCod,Year,Semester,ShortName,FullName,NumTeachers,NumStudents FROM courses WHERE CrsCod='-1';

DROP INDEX DegCod ON sta_degrees;
ALTER TABLE sta_degrees CHANGE DegCod DegCod_old CHAR(16) NOT NULL;
ALTER TABLE sta_degrees ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER DegCod_old;
UPDATE sta_degrees,degrees SET sta_degrees.DegCod=degrees.DegCod WHERE sta_degrees.DegCod_old=degrees.DegCod_old;
ALTER TABLE sta_degrees ADD UNIQUE INDEX (DegCod);
ALTER TABLE sta_degrees DROP COLUMN DegCod_old;

SELECT asg_usr.CrsCod,courses.ShortName AS Name FROM asg_usr,courses WHERE asg_usr.DNI='24243619' AND asg_usr.CrsCod=courses.CrsCod ORDER BY Name;

SELECT COUNT(DISTINCT(courses.DegCod)) FROM asg_usr,courses WHERE asg_usr.DNI='24243619' AND courses.DegCod='57' AND asg_usr.CrsCod=courses.CrsCod;

ALTER TABLE log DROP COLUMN DegCod_old;
ALTER TABLE log DROP COLUMN FullCrsCod_old;

mv /var/www/swad/asg/ARQ/0/2/AP /var/www/swad/crs/1
mv /var/www/html/asg/ARQ/0/2/AP /var/www/html/crs/1

mv /var/www/swad/asg/ARQ/0/0/DAO /var/www/swad/crs/2
mv /var/www/html/asg/ARQ/0/0/DAO /var/www/html/crs/2

mv /var/www/swad/asg/ARQ/1/0/FMA /var/www/swad/crs/3
mv /var/www/html/asg/ARQ/1/0/FMA /var/www/html/crs/3

mv /var/www/swad/asg/ARQ/2/2/AF /var/www/swad/crs/4
mv /var/www/html/asg/ARQ/2/2/AF /var/www/html/crs/4

mv /var/www/swad/asg/ARQ/3/0/P3 /var/www/swad/crs/5
mv /var/www/html/asg/ARQ/3/0/P3 /var/www/html/crs/5

mv /var/www/swad/asg/ARQ/5/0/APFC /var/www/swad/crs/6
mv /var/www/html/asg/ARQ/5/0/APFC /var/www/html/crs/6

mv /var/www/swad/asg/ARQ/5/1/E4 /var/www/swad/crs/7
mv /var/www/html/asg/ARQ/5/1/E4 /var/www/html/crs/7

mv /var/www/swad/asg/ARQ/5/2/LA /var/www/swad/crs/8
mv /var/www/html/asg/ARQ/5/2/LA /var/www/html/crs/8

mv /var/www/swad/asg/ARQ/5/2/LB /var/www/swad/crs/9
mv /var/www/html/asg/ARQ/5/2/LB /var/www/html/crs/9

mv /var/www/swad/asg/AT/1/0/F /var/www/swad/crs/10
mv /var/www/html/asg/AT/1/0/F /var/www/html/crs/10

mv /var/www/swad/asg/AT/2/0/I1AB /var/www/swad/crs/11
mv /var/www/html/asg/AT/2/0/I1AB /var/www/html/crs/11

mv /var/www/swad/asg/AT/3/0/I2AB /var/www/swad/crs/12
mv /var/www/html/asg/AT/3/0/I2AB /var/www/html/crs/12

mv /var/www/swad/asg/AT/3/0/MPVB /var/www/swad/crs/13
mv /var/www/html/asg/AT/3/0/MPVB /var/www/html/crs/13

mv /var/www/swad/asg/AA/0/0/PF82 /var/www/swad/crs/14
mv /var/www/html/asg/AA/0/0/PF82 /var/www/html/crs/14

mv /var/www/swad/asg/CAP/1/0/EEAFLE /var/www/swad/crs/15
mv /var/www/html/asg/CAP/1/0/EEAFLE /var/www/html/crs/15

mv /var/www/swad/asg/CAP/1/1/ECD04 /var/www/swad/crs/16
mv /var/www/html/asg/CAP/1/1/ECD04 /var/www/html/crs/16

mv /var/www/swad/asg/CEH/0/2/ASPE /var/www/swad/crs/17
mv /var/www/html/asg/CEH/0/2/ASPE /var/www/html/crs/17

mv /var/www/swad/asg/CEH/0/1/PGE /var/www/swad/crs/18
mv /var/www/html/asg/CEH/0/1/PGE /var/www/html/crs/18

mv /var/www/swad/asg/CLCE/0/0/CCH /var/www/swad/crs/19
mv /var/www/html/asg/CLCE/0/0/CCH /var/www/html/crs/19

mv /var/www/swad/asg/CFC/0/0/MCDAPC /var/www/swad/crs/20
mv /var/www/html/asg/CFC/0/0/MCDAPC /var/www/html/crs/20

mv /var/www/swad/asg/CFC/0/0/DJANGO /var/www/swad/crs/21
mv /var/www/html/asg/CFC/0/0/DJANGO /var/www/html/crs/21

mv /var/www/swad/asg/DBD/1/0/HCDCA /var/www/swad/crs/22
mv /var/www/html/asg/DBD/1/0/HCDCA /var/www/html/crs/22

mv /var/www/swad/asg/DCE/0/2/RFEA /var/www/swad/crs/23
mv /var/www/html/asg/DCE/0/2/RFEA /var/www/html/crs/23

mv /var/www/swad/asg/DCE/1/1/AD /var/www/swad/crs/24
mv /var/www/html/asg/DCE/1/1/AD /var/www/html/crs/24

mv /var/www/swad/asg/DCE/1/2/CF1 /var/www/swad/crs/25
mv /var/www/html/asg/DCE/1/2/CF1 /var/www/html/crs/25

mv /var/www/swad/asg/DCE/1/1/CG /var/www/swad/crs/26
mv /var/www/html/asg/DCE/1/1/CG /var/www/html/crs/26

mv /var/www/swad/asg/DCE/1/2/DMB /var/www/swad/crs/27
mv /var/www/html/asg/DCE/1/2/DMB /var/www/html/crs/27

mv /var/www/swad/asg/DCE/1/1/EP /var/www/swad/crs/28
mv /var/www/html/asg/DCE/1/1/EP /var/www/html/crs/28

mv /var/www/swad/asg/DCE/1/1/DC /var/www/swad/crs/29
mv /var/www/html/asg/DCE/1/1/DC /var/www/html/crs/29

mv /var/www/swad/asg/DCE/1/1/DCCD /var/www/swad/crs/30
mv /var/www/html/asg/DCE/1/1/DCCD /var/www/html/crs/30

mv /var/www/swad/asg/DCE/1/1/DCF /var/www/swad/crs/31
mv /var/www/html/asg/DCE/1/1/DCF /var/www/html/crs/31

mv /var/www/swad/asg/DCE/1/2/INF /var/www/swad/crs/32
mv /var/www/html/asg/DCE/1/2/INF /var/www/html/crs/32

mv /var/www/swad/asg/DCE/1/1/ME1 /var/www/swad/crs/33
mv /var/www/html/asg/DCE/1/1/ME1 /var/www/html/crs/33

mv /var/www/swad/asg/DCE/1/2/ME2 /var/www/swad/crs/34
mv /var/www/html/asg/DCE/1/2/ME2 /var/www/html/crs/34

mv /var/www/swad/asg/DCE/1/2/MC /var/www/swad/crs/35
mv /var/www/html/asg/DCE/1/2/MC /var/www/html/crs/35

mv /var/www/swad/asg/DCE/1/2/MI /var/www/swad/crs/36
mv /var/www/html/asg/DCE/1/2/MI /var/www/html/crs/36

mv /var/www/swad/asg/DCE/2/1/COS1 /var/www/swad/crs/37
mv /var/www/html/asg/DCE/2/1/COS1 /var/www/html/crs/37

mv /var/www/swad/asg/DCE/2/2/COS2 /var/www/swad/crs/38
mv /var/www/html/asg/DCE/2/2/COS2 /var/www/html/crs/38

mv /var/www/swad/asg/DCE/2/1/CF2 /var/www/swad/crs/39
mv /var/www/html/asg/DCE/2/1/CF2 /var/www/html/crs/39

mv /var/www/swad/asg/DCE/2/2/DFAB /var/www/swad/crs/40
mv /var/www/html/asg/DCE/2/2/DFAB /var/www/html/crs/40

mv /var/www/swad/asg/DCE/2/2/DFDE /var/www/swad/crs/41
mv /var/www/html/asg/DCE/2/2/DFDE /var/www/html/crs/41

mv /var/www/swad/asg/DCE/2/1/DTS /var/www/swad/crs/42
mv /var/www/html/asg/DCE/2/1/DTS /var/www/html/crs/42

mv /var/www/swad/asg/DCE/2/1/EE1 /var/www/swad/crs/43
mv /var/www/html/asg/DCE/2/1/EE1 /var/www/html/crs/43

mv /var/www/swad/asg/DCE/2/2/EE2 /var/www/swad/crs/44
mv /var/www/html/asg/DCE/2/2/EE2 /var/www/html/crs/44

mv /var/www/swad/asg/DCE/2/0/EEM /var/www/swad/crs/45
mv /var/www/html/asg/DCE/2/0/EEM /var/www/html/crs/45

mv /var/www/swad/asg/DCE/2/2/EC /var/www/swad/crs/46
mv /var/www/html/asg/DCE/2/2/EC /var/www/html/crs/46

mv /var/www/swad/asg/DCE/2/1/MARK /var/www/swad/crs/47
mv /var/www/html/asg/DCE/2/1/MARK /var/www/html/crs/47

mv /var/www/swad/asg/DCE/2/2/MF /var/www/swad/crs/48
mv /var/www/html/asg/DCE/2/2/MF /var/www/html/crs/48

mv /var/www/swad/asg/DCE/3/2/AC /var/www/swad/crs/49
mv /var/www/html/asg/DCE/3/2/AC /var/www/html/crs/49

mv /var/www/swad/asg/DCE/3/1/AOF /var/www/swad/crs/50
mv /var/www/html/asg/DCE/3/1/AOF /var/www/html/crs/50

mv /var/www/swad/asg/DCE/3/2/AEF /var/www/swad/crs/51
mv /var/www/html/asg/DCE/3/2/AEF /var/www/html/crs/51

mv /var/www/swad/asg/DCE/3/1/CE /var/www/swad/crs/52
mv /var/www/html/asg/DCE/3/1/CE /var/www/html/crs/52

mv /var/www/swad/asg/DCE/3/2/CC /var/www/swad/crs/53
mv /var/www/html/asg/DCE/3/2/CC /var/www/html/crs/53

mv /var/www/swad/asg/DCE/3/1/CAP /var/www/swad/crs/54
mv /var/www/html/asg/DCE/3/1/CAP /var/www/html/crs/54

mv /var/www/swad/asg/DCE/3/1/CSOE /var/www/swad/crs/55
mv /var/www/html/asg/DCE/3/1/CSOE /var/www/html/crs/55

mv /var/www/swad/asg/DCE/3/1/CMTV /var/www/swad/crs/56
mv /var/www/html/asg/DCE/3/1/CMTV /var/www/html/crs/56

mv /var/www/swad/asg/DCE/3/2/CG /var/www/swad/crs/57
mv /var/www/html/asg/DCE/3/2/CG /var/www/html/crs/57

mv /var/www/swad/asg/DCE/3/2/DSS /var/www/swad/crs/58
mv /var/www/html/asg/DCE/3/2/DSS /var/www/html/crs/58

mv /var/www/swad/asg/DCE/3/1/DO /var/www/swad/crs/59
mv /var/www/html/asg/DCE/3/1/DO /var/www/html/crs/59

mv /var/www/swad/asg/DCE/3/1/DV /var/www/swad/crs/60
mv /var/www/html/asg/DCE/3/1/DV /var/www/html/crs/60

mv /var/www/swad/asg/DCE/3/1/EGE /var/www/swad/crs/61
mv /var/www/html/asg/DCE/3/1/EGE /var/www/html/crs/61

mv /var/www/swad/asg/DCE/3/2/GRH /var/www/swad/crs/62
mv /var/www/html/asg/DCE/3/2/GRH /var/www/html/crs/62

mv /var/www/swad/asg/DCE/3/1/GF /var/www/swad/crs/63
mv /var/www/html/asg/DCE/3/1/GF /var/www/html/crs/63

mv /var/www/swad/asg/DCE/3/2/HECOA /var/www/swad/crs/64
mv /var/www/html/asg/DCE/3/2/HECOA /var/www/html/crs/64

mv /var/www/swad/asg/DCE/3/2/HECOB /var/www/swad/crs/65
mv /var/www/html/asg/DCE/3/2/HECOB /var/www/html/crs/65

mv /var/www/swad/asg/DCE/3/1/MA1 /var/www/swad/crs/66
mv /var/www/html/asg/DCE/3/1/MA1 /var/www/html/crs/66

mv /var/www/swad/asg/DCE/3/2/MA2 /var/www/swad/crs/67
mv /var/www/html/asg/DCE/3/2/MA2 /var/www/html/crs/67

mv /var/www/swad/asg/DCE/3/2/OE /var/www/swad/crs/68
mv /var/www/html/asg/DCE/3/2/OE /var/www/html/crs/68

mv /var/www/swad/asg/DCE/3/2/OAE /var/www/swad/crs/69
mv /var/www/html/asg/DCE/3/2/OAE /var/www/html/crs/69

mv /var/www/swad/asg/DCE/3/2/RL /var/www/swad/crs/70
mv /var/www/html/asg/DCE/3/2/RL /var/www/html/crs/70

mv /var/www/swad/asg/DCE/3/2/TGBI /var/www/swad/crs/71
mv /var/www/html/asg/DCE/3/2/TGBI /var/www/html/crs/71

mv /var/www/swad/asg/DCEC/2/1/MARK1 /var/www/swad/crs/72
mv /var/www/html/asg/DCEC/2/1/MARK1 /var/www/html/crs/72

mv /var/www/swad/asg/DCEC/2/2/MARK2 /var/www/swad/crs/73
mv /var/www/html/asg/DCEC/2/2/MARK2 /var/www/html/crs/73

mv /var/www/swad/asg/DCEC/3/1/CC /var/www/swad/crs/74
mv /var/www/html/asg/DCEC/3/1/CC /var/www/html/crs/74

mv /var/www/swad/asg/DCEC/3/2/CC /var/www/swad/crs/75
mv /var/www/html/asg/DCEC/3/2/CC /var/www/html/crs/75

mv /var/www/swad/asg/DES/0/1/ISAF /var/www/swad/crs/76
mv /var/www/html/asg/DES/0/1/ISAF /var/www/html/crs/76

mv /var/www/swad/asg/DES/0/2/SSAM /var/www/swad/crs/77
mv /var/www/html/asg/DES/0/2/SSAM /var/www/html/crs/77

mv /var/www/swad/asg/DES/1/1/PLES /var/www/swad/crs/78
mv /var/www/html/asg/DES/1/1/PLES /var/www/html/crs/78

mv /var/www/swad/asg/DES/2/1/CSRG /var/www/swad/crs/79
mv /var/www/html/asg/DES/2/1/CSRG /var/www/html/crs/79

mv /var/www/swad/asg/DES/2/1/DOAENF /var/www/swad/crs/80
mv /var/www/html/asg/DES/2/1/DOAENF /var/www/html/crs/80

mv /var/www/swad/asg/DES/2/1/DEPS /var/www/swad/crs/81
mv /var/www/html/asg/DES/2/1/DEPS /var/www/html/crs/81

mv /var/www/swad/asg/DES/2/1/ESPIP /var/www/swad/crs/82
mv /var/www/html/asg/DES/2/1/ESPIP /var/www/html/crs/82

mv /var/www/swad/asg/DES/2/1/EP /var/www/swad/crs/83
mv /var/www/html/asg/DES/2/1/EP /var/www/html/crs/83

mv /var/www/swad/asg/DES/2/0/IEPFDS /var/www/swad/crs/84
mv /var/www/html/asg/DES/2/0/IEPFDS /var/www/html/crs/84

mv /var/www/swad/asg/DES/2/0/IEPFD3 /var/www/swad/crs/85
mv /var/www/html/asg/DES/2/0/IEPFD3 /var/www/html/crs/85

mv /var/www/swad/asg/DES/2/2/PEPM /var/www/swad/crs/86
mv /var/www/html/asg/DES/2/2/PEPM /var/www/html/crs/86

mv /var/www/swad/asg/DES/3/1/P /var/www/swad/crs/87
mv /var/www/html/asg/DES/3/1/P /var/www/html/crs/87

mv /var/www/swad/asg/DES/3/2/NTAE /var/www/swad/crs/88
mv /var/www/html/asg/DES/3/2/NTAE /var/www/html/crs/88

mv /var/www/swad/asg/DENF/1/2/B /var/www/swad/crs/89
mv /var/www/html/asg/DENF/1/2/B /var/www/html/crs/89

mv /var/www/swad/asg/DENF/1/0/FE /var/www/swad/crs/90
mv /var/www/html/asg/DENF/1/0/FE /var/www/html/crs/90

mv /var/www/swad/asg/DENF/2/0/EM1 /var/www/swad/crs/91
mv /var/www/html/asg/DENF/2/0/EM1 /var/www/html/crs/91

mv /var/www/swad/asg/DENF/2/0/EQ1 /var/www/swad/crs/92
mv /var/www/html/asg/DENF/2/0/EQ1 /var/www/html/crs/92

mv /var/www/swad/asg/DENF/3/0/ASE /var/www/swad/crs/93
mv /var/www/html/asg/DENF/3/0/ASE /var/www/html/crs/93

mv /var/www/swad/asg/DENF/3/0/ER /var/www/swad/crs/94
mv /var/www/html/asg/DENF/3/0/ER /var/www/html/crs/94

mv /var/www/swad/asg/DENF/3/2/FEJFG /var/www/swad/crs/95
mv /var/www/html/asg/DENF/3/2/FEJFG /var/www/html/crs/95

mv /var/www/swad/asg/DENFM/2/0/EMQ2 /var/www/swad/crs/96
mv /var/www/html/asg/DENFM/2/0/EMQ2 /var/www/html/crs/96

mv /var/www/swad/asg/DENFM/3/0/EMQ4 /var/www/swad/crs/97
mv /var/www/html/asg/DENFM/3/0/EMQ4 /var/www/html/crs/97

mv /var/www/swad/asg/DE/1/2/ACP /var/www/swad/crs/98
mv /var/www/html/asg/DE/1/2/ACP /var/www/html/crs/98

mv /var/www/swad/asg/DE/1/1/CP /var/www/swad/crs/99
mv /var/www/html/asg/DE/1/1/CP /var/www/html/crs/99

mv /var/www/swad/asg/DE/1/1/ED /var/www/swad/crs/100
mv /var/www/html/asg/DE/1/1/ED /var/www/html/crs/100

mv /var/www/swad/asg/DE/1/1/FI /var/www/swad/crs/101
mv /var/www/html/asg/DE/1/1/FI /var/www/html/crs/101

mv /var/www/swad/asg/DE/2/2/EP /var/www/swad/crs/102
mv /var/www/html/asg/DE/2/2/EP /var/www/html/crs/102

mv /var/www/swad/asg/DE/2/1/M1 /var/www/swad/crs/103
mv /var/www/html/asg/DE/2/1/M1 /var/www/html/crs/103

mv /var/www/swad/asg/DE/3/1/SC /var/www/swad/crs/104
mv /var/www/html/asg/DE/3/1/SC /var/www/html/crs/104

mv /var/www/swad/asg/DE/3/1/TEE /var/www/swad/crs/105
mv /var/www/html/asg/DE/3/1/TEE /var/www/html/crs/105

mv /var/www/swad/asg/DF/0/2/FD /var/www/swad/crs/106
mv /var/www/html/asg/DF/0/2/FD /var/www/html/crs/106

mv /var/www/swad/asg/DF/0/1/TACM /var/www/swad/crs/107
mv /var/www/html/asg/DF/0/1/TACM /var/www/html/crs/107

mv /var/www/swad/asg/DF/0/1/TAMJF /var/www/swad/crs/108
mv /var/www/html/asg/DF/0/1/TAMJF /var/www/html/crs/108

mv /var/www/swad/asg/DF/1/2/EF /var/www/swad/crs/109
mv /var/www/html/asg/DF/1/2/EF /var/www/html/crs/109

mv /var/www/swad/asg/DF/1/0/FGAM /var/www/swad/crs/110
mv /var/www/html/asg/DF/1/0/FGAM /var/www/html/crs/110

mv /var/www/swad/asg/DF/1/0/FGCV /var/www/swad/crs/111
mv /var/www/html/asg/DF/1/0/FGCV /var/www/html/crs/111

mv /var/www/swad/asg/DF/1/0/FGEC /var/www/swad/crs/112
mv /var/www/html/asg/DF/1/0/FGEC /var/www/html/crs/112

mv /var/www/swad/asg/DF/1/0/FGJFG /var/www/swad/crs/113
mv /var/www/html/asg/DF/1/0/FGJFG /var/www/html/crs/113

mv /var/www/swad/asg/DF/2/0/BAM /var/www/swad/crs/114
mv /var/www/html/asg/DF/2/0/BAM /var/www/html/crs/114

mv /var/www/swad/asg/DF/2/0/BCV /var/www/swad/crs/115
mv /var/www/html/asg/DF/2/0/BCV /var/www/html/crs/115

mv /var/www/swad/asg/DF/2/0/BGV /var/www/swad/crs/116
mv /var/www/html/asg/DF/2/0/BGV /var/www/html/crs/116

mv /var/www/swad/asg/DF/2/0/BT /var/www/swad/crs/117
mv /var/www/html/asg/DF/2/0/BT /var/www/html/crs/117

mv /var/www/swad/asg/DF/2/0/FE1BE /var/www/swad/crs/118
mv /var/www/html/asg/DF/2/0/FE1BE /var/www/html/crs/118

mv /var/www/swad/asg/DF/2/0/FE1CM /var/www/swad/crs/119
mv /var/www/html/asg/DF/2/0/FE1CM /var/www/html/crs/119

mv /var/www/swad/asg/DF/2/0/FE1MJF /var/www/swad/crs/120
mv /var/www/html/asg/DF/2/0/FE1MJF /var/www/html/crs/120

mv /var/www/swad/asg/DF/3/0/FE2A /var/www/swad/crs/121
mv /var/www/html/asg/DF/3/0/FE2A /var/www/html/crs/121

mv /var/www/swad/asg/DF/3/0/FE2E /var/www/swad/crs/122
mv /var/www/html/asg/DF/3/0/FE2E /var/www/html/crs/122

mv /var/www/swad/asg/DF/3/1/NS /var/www/swad/crs/123
mv /var/www/html/asg/DF/3/1/NS /var/www/html/crs/123

mv /var/www/swad/asg/DF/3/0/SPLS /var/www/swad/crs/124
mv /var/www/html/asg/DF/3/0/SPLS /var/www/html/crs/124

mv /var/www/swad/asg/DL/1/2/FOAL /var/www/swad/crs/125
mv /var/www/html/asg/DL/1/2/FOAL /var/www/html/crs/125

mv /var/www/swad/asg/DL/1/2/PPDHP /var/www/swad/crs/126
mv /var/www/html/asg/DL/1/2/PPDHP /var/www/html/crs/126

mv /var/www/swad/asg/DNHD/0/1/BMAN /var/www/swad/crs/127
mv /var/www/html/asg/DNHD/0/1/BMAN /var/www/html/crs/127

mv /var/www/swad/asg/DNHD/1/1/AC /var/www/swad/crs/128
mv /var/www/html/asg/DNHD/1/1/AC /var/www/html/crs/128

mv /var/www/swad/asg/DNHD/1/0/B /var/www/swad/crs/129
mv /var/www/html/asg/DNHD/1/0/B /var/www/html/crs/129

mv /var/www/swad/asg/DNHD/1/1/BR /var/www/swad/crs/130
mv /var/www/html/asg/DNHD/1/1/BR /var/www/html/crs/130

mv /var/www/swad/asg/DNHD/1/2/FA /var/www/swad/crs/131
mv /var/www/html/asg/DNHD/1/2/FA /var/www/html/crs/131

mv /var/www/swad/asg/DNHD/1/1/QOA /var/www/swad/crs/132
mv /var/www/html/asg/DNHD/1/1/QOA /var/www/html/crs/132

mv /var/www/swad/asg/DNHD/1/2/TAA /var/www/swad/crs/133
mv /var/www/html/asg/DNHD/1/2/TAA /var/www/html/crs/133

mv /var/www/swad/asg/DNHD/1/2/TC /var/www/swad/crs/134
mv /var/www/html/asg/DNHD/1/2/TC /var/www/html/crs/134

mv /var/www/swad/asg/DNHD/2/1/BN /var/www/swad/crs/135
mv /var/www/html/asg/DNHD/2/1/BN /var/www/html/crs/135

mv /var/www/swad/asg/DNHD/2/0/D /var/www/swad/crs/136
mv /var/www/html/asg/DNHD/2/0/D /var/www/html/crs/136

mv /var/www/swad/asg/DNHD/2/1/ME /var/www/swad/crs/137
mv /var/www/html/asg/DNHD/2/1/ME /var/www/html/crs/137

mv /var/www/swad/asg/DNHD/2/0/MPCA /var/www/swad/crs/138
mv /var/www/html/asg/DNHD/2/0/MPCA /var/www/html/crs/138

mv /var/www/swad/asg/DNHD/2/0/MPCAC /var/www/swad/crs/139
mv /var/www/html/asg/DNHD/2/0/MPCAC /var/www/html/crs/139

mv /var/www/swad/asg/DNHD/3/1/BN /var/www/swad/crs/140
mv /var/www/html/asg/DNHD/3/1/BN /var/www/html/crs/140

mv /var/www/swad/asg/DOO/0/1/SVD /var/www/swad/crs/141
mv /var/www/html/asg/DOO/0/1/SVD /var/www/html/crs/141

mv /var/www/swad/asg/DOO/1/0/EFSV1 /var/www/swad/crs/142
mv /var/www/html/asg/DOO/1/0/EFSV1 /var/www/html/crs/142

mv /var/www/swad/asg/DOO/1/0/F /var/www/swad/crs/143
mv /var/www/html/asg/DOO/1/0/F /var/www/html/crs/143

mv /var/www/swad/asg/DOO/1/0/FP /var/www/swad/crs/144
mv /var/www/html/asg/DOO/1/0/FP /var/www/html/crs/144

mv /var/www/swad/asg/DOO/1/0/O1A /var/www/swad/crs/145
mv /var/www/html/asg/DOO/1/0/O1A /var/www/html/crs/145

mv /var/www/swad/asg/DOO/1/0/O1B /var/www/swad/crs/146
mv /var/www/html/asg/DOO/1/0/O1B /var/www/html/crs/146

mv /var/www/swad/asg/DOO/1/0/O1P /var/www/swad/crs/147
mv /var/www/html/asg/DOO/1/0/O1P /var/www/html/crs/147

mv /var/www/swad/asg/DOO/1/2/TO1T /var/www/swad/crs/148
mv /var/www/html/asg/DOO/1/2/TO1T /var/www/html/crs/148

mv /var/www/swad/asg/DOO/1/2/TO1P /var/www/swad/crs/149
mv /var/www/html/asg/DOO/1/2/TO1P /var/www/html/crs/149

mv /var/www/swad/asg/DOO/2/2/AAP /var/www/swad/crs/150
mv /var/www/html/asg/DOO/2/2/AAP /var/www/html/crs/150

mv /var/www/swad/asg/DOO/2/0/EFSV2 /var/www/swad/crs/151
mv /var/www/html/asg/DOO/2/0/EFSV2 /var/www/html/crs/151

mv /var/www/swad/asg/DOO/2/0/O2 /var/www/swad/crs/152
mv /var/www/html/asg/DOO/2/0/O2 /var/www/html/crs/152

mv /var/www/swad/asg/DOO/2/0/O2P /var/www/swad/crs/153
mv /var/www/html/asg/DOO/2/0/O2P /var/www/html/crs/153

mv /var/www/swad/asg/DOO/2/1/OC1 /var/www/swad/crs/154
mv /var/www/html/asg/DOO/2/1/OC1 /var/www/html/crs/154

mv /var/www/swad/asg/DOO/2/2/OC2 /var/www/swad/crs/155
mv /var/www/html/asg/DOO/2/2/OC2 /var/www/html/crs/155

mv /var/www/swad/asg/DOO/3/2/OC3 /var/www/swad/crs/156
mv /var/www/html/asg/DOO/3/2/OC3 /var/www/html/crs/156

mv /var/www/swad/asg/DOO/3/1/RFC /var/www/swad/crs/157
mv /var/www/html/asg/DOO/3/1/RFC /var/www/html/crs/157

mv /var/www/swad/asg/DRL/0/1/PCEP /var/www/swad/crs/158
mv /var/www/html/asg/DRL/0/1/PCEP /var/www/html/crs/158

mv /var/www/swad/asg/DRL/1/0/DGP /var/www/swad/crs/159
mv /var/www/html/asg/DRL/1/0/DGP /var/www/html/crs/159

mv /var/www/swad/asg/DRL/1/1/E /var/www/swad/crs/160
mv /var/www/html/asg/DRL/1/1/E /var/www/html/crs/160

mv /var/www/swad/asg/DRL/1/1/ESD /var/www/swad/crs/161
mv /var/www/html/asg/DRL/1/1/ESD /var/www/html/crs/161

mv /var/www/swad/asg/DRL/1/1/ESE /var/www/swad/crs/162
mv /var/www/html/asg/DRL/1/1/ESE /var/www/html/crs/162

mv /var/www/swad/asg/DRL/1/1/ESF /var/www/swad/crs/163
mv /var/www/html/asg/DRL/1/1/ESF /var/www/html/crs/163

mv /var/www/swad/asg/DRL/1/2/EAO /var/www/swad/crs/164
mv /var/www/html/asg/DRL/1/2/EAO /var/www/html/crs/164

mv /var/www/swad/asg/DRL/1/2/EAOC2 /var/www/swad/crs/165
mv /var/www/html/asg/DRL/1/2/EAOC2 /var/www/html/crs/165

mv /var/www/swad/asg/DRL/2/1/ADPP /var/www/swad/crs/166
mv /var/www/html/asg/DRL/2/1/ADPP /var/www/html/crs/166

mv /var/www/swad/asg/DRL/2/0/DSS /var/www/swad/crs/167
mv /var/www/html/asg/DRL/2/0/DSS /var/www/html/crs/167

mv /var/www/swad/asg/DRL/2/0/DT /var/www/swad/crs/168
mv /var/www/html/asg/DRL/2/0/DT /var/www/html/crs/168

mv /var/www/swad/asg/DRL/2/2/GERH /var/www/swad/crs/169
mv /var/www/html/asg/DRL/2/2/GERH /var/www/html/crs/169

mv /var/www/swad/asg/DRL/2/1/IDP /var/www/swad/crs/170
mv /var/www/html/asg/DRL/2/1/IDP /var/www/html/crs/170

mv /var/www/swad/asg/DRL/2/0/OMT /var/www/swad/crs/171
mv /var/www/html/asg/DRL/2/0/OMT /var/www/html/crs/171

mv /var/www/swad/asg/DRL/2/2/PI1 /var/www/swad/crs/172
mv /var/www/html/asg/DRL/2/2/PI1 /var/www/html/crs/172

mv /var/www/swad/asg/DRL/2/2/RFEA /var/www/swad/crs/173
mv /var/www/html/asg/DRL/2/2/RFEA /var/www/html/crs/173

mv /var/www/swad/asg/DRL/2/2/RFED /var/www/swad/crs/174
mv /var/www/html/asg/DRL/2/2/RFED /var/www/html/crs/174

mv /var/www/swad/asg/DRL/2/2/RFEE /var/www/swad/crs/175
mv /var/www/html/asg/DRL/2/2/RFEE /var/www/html/crs/175

mv /var/www/swad/asg/DRL/2/1/ST /var/www/swad/crs/176
mv /var/www/html/asg/DRL/2/1/ST /var/www/html/crs/176

mv /var/www/swad/asg/DRL/3/1/CGCD /var/www/swad/crs/177
mv /var/www/html/asg/DRL/3/1/CGCD /var/www/html/crs/177

mv /var/www/swad/asg/DRL/3/0/PTA /var/www/swad/crs/178
mv /var/www/html/asg/DRL/3/0/PTA /var/www/html/crs/178

mv /var/www/swad/asg/DTO/0/2/TFTO /var/www/swad/crs/179
mv /var/www/html/asg/DTO/0/2/TFTO /var/www/html/crs/179

mv /var/www/swad/asg/DTO/1/0/ECH /var/www/swad/crs/180
mv /var/www/html/asg/DTO/1/0/ECH /var/www/html/crs/180

mv /var/www/swad/asg/DTO/1/1/PAA /var/www/swad/crs/181
mv /var/www/html/asg/DTO/1/1/PAA /var/www/html/crs/181

mv /var/www/swad/asg/DTO/2/0/AMQ1 /var/www/swad/crs/182
mv /var/www/html/asg/DTO/2/0/AMQ1 /var/www/html/crs/182

mv /var/www/swad/asg/DTO/3/0/AMQ2 /var/www/swad/crs/183
mv /var/www/html/asg/DTO/3/0/AMQ2 /var/www/html/crs/183

mv /var/www/swad/asg/DTO/3/0/G /var/www/swad/crs/184
mv /var/www/html/asg/DTO/3/0/G /var/www/html/crs/184

mv /var/www/swad/asg/DTO/3/0/TOA2I /var/www/swad/crs/185
mv /var/www/html/asg/DTO/3/0/TOA2I /var/www/html/crs/185

mv /var/www/swad/asg/DTS/0/2/ETS /var/www/swad/crs/186
mv /var/www/html/asg/DTS/0/2/ETS /var/www/html/crs/186

mv /var/www/swad/asg/DTS/0/2/EATS /var/www/swad/crs/187
mv /var/www/html/asg/DTS/0/2/EATS /var/www/html/crs/187

mv /var/www/swad/asg/DTS/0/1/GTS /var/www/swad/crs/188
mv /var/www/html/asg/DTS/0/1/GTS /var/www/html/crs/188

mv /var/www/swad/asg/DTS/0/1/PDTS /var/www/swad/crs/189
mv /var/www/html/asg/DTS/0/1/PDTS /var/www/html/crs/189

mv /var/www/swad/asg/DTS/0/1/PSEUE /var/www/swad/crs/190
mv /var/www/html/asg/DTS/0/1/PSEUE /var/www/html/crs/190

mv /var/www/swad/asg/DTS/0/1/SPTS /var/www/swad/crs/191
mv /var/www/html/asg/DTS/0/1/SPTS /var/www/html/crs/191

mv /var/www/swad/asg/DTS/0/1/TSAJ /var/www/swad/crs/192
mv /var/www/html/asg/DTS/0/1/TSAJ /var/www/html/crs/192

mv /var/www/swad/asg/DTS/0/2/TSAPS /var/www/swad/crs/193
mv /var/www/html/asg/DTS/0/2/TSAPS /var/www/html/crs/193

mv /var/www/swad/asg/DTS/0/1/TSD /var/www/swad/crs/194
mv /var/www/html/asg/DTS/0/1/TSD /var/www/html/crs/194

mv /var/www/swad/asg/DTS/0/2/TSG /var/www/swad/crs/195
mv /var/www/html/asg/DTS/0/2/TSG /var/www/html/crs/195

mv /var/www/swad/asg/DTS/0/2/TSS /var/www/swad/crs/196
mv /var/www/html/asg/DTS/0/2/TSS /var/www/html/crs/196

mv /var/www/swad/asg/DTS/0/1/TSSSI /var/www/swad/crs/197
mv /var/www/html/asg/DTS/0/1/TSSSI /var/www/html/crs/197

mv /var/www/swad/asg/DTS/1/0/IFMTSA /var/www/swad/crs/198
mv /var/www/html/asg/DTS/1/0/IFMTSA /var/www/html/crs/198

mv /var/www/swad/asg/DTS/1/0/IFMTSB /var/www/swad/crs/199
mv /var/www/html/asg/DTS/1/0/IFMTSB /var/www/html/crs/199

mv /var/www/swad/asg/DTS/1/0/SS1 /var/www/swad/crs/200
mv /var/www/html/asg/DTS/1/0/SS1 /var/www/html/crs/200

mv /var/www/swad/asg/DTS/2/1/IFE1 /var/www/swad/crs/201
mv /var/www/html/asg/DTS/2/1/IFE1 /var/www/html/crs/201

mv /var/www/swad/asg/DTS/2/2/IFE2 /var/www/swad/crs/202
mv /var/www/html/asg/DTS/2/2/IFE2 /var/www/html/crs/202

mv /var/www/swad/asg/DTS/2/0/MCATS /var/www/swad/crs/203
mv /var/www/html/asg/DTS/2/0/MCATS /var/www/html/crs/203

mv /var/www/swad/asg/DTS/2/2/PS /var/www/swad/crs/204
mv /var/www/html/asg/DTS/2/2/PS /var/www/html/crs/204

mv /var/www/swad/asg/DTS/2/0/SS2 /var/www/swad/crs/205
mv /var/www/html/asg/DTS/2/0/SS2 /var/www/html/crs/205

mv /var/www/swad/asg/DTS/2/0/TSC /var/www/swad/crs/206
mv /var/www/html/asg/DTS/2/0/TSC /var/www/html/crs/206

mv /var/www/swad/asg/DTS/2/1/TSIFG /var/www/swad/crs/207
mv /var/www/html/asg/DTS/2/1/TSIFG /var/www/html/crs/207

mv /var/www/swad/asg/DTUR/0/1/SFP /var/www/swad/crs/208
mv /var/www/html/asg/DTUR/0/1/SFP /var/www/html/crs/208

mv /var/www/swad/asg/DTUR/1/1/ACSTA /var/www/swad/crs/209
mv /var/www/html/asg/DTUR/1/1/ACSTA /var/www/html/crs/209

mv /var/www/swad/asg/DTUR/1/1/ACST /var/www/swad/crs/210
mv /var/www/html/asg/DTUR/1/1/ACST /var/www/html/crs/210

mv /var/www/swad/asg/DTUR/1/1/DL1 /var/www/swad/crs/211
mv /var/www/html/asg/DTUR/1/1/DL1 /var/www/html/crs/211

mv /var/www/swad/asg/DTUR/1/0/IE /var/www/swad/crs/212
mv /var/www/html/asg/DTUR/1/0/IE /var/www/html/crs/212

mv /var/www/swad/asg/DTUR/3/2/ETC /var/www/swad/crs/213
mv /var/www/html/asg/DTUR/3/2/ETC /var/www/html/crs/213

mv /var/www/swad/asg/DAFESC/0/0/AFAD /var/www/swad/crs/214
mv /var/www/html/asg/DAFESC/0/0/AFAD /var/www/html/crs/214

mv /var/www/swad/asg/DAFESC/0/0/AFESC /var/www/swad/crs/215
mv /var/www/html/asg/DAFESC/0/0/AFESC /var/www/html/crs/215

mv /var/www/swad/asg/DAFESC/0/0/CRVAF /var/www/swad/crs/216
mv /var/www/html/asg/DAFESC/0/0/CRVAF /var/www/html/crs/216

mv /var/www/swad/asg/DAFESC/0/0/CCM /var/www/swad/crs/217
mv /var/www/html/asg/DAFESC/0/0/CCM /var/www/html/crs/217

mv /var/www/swad/asg/DAFESC/0/0/DISAAF /var/www/swad/crs/218
mv /var/www/html/asg/DAFESC/0/0/DISAAF /var/www/html/crs/218

mv /var/www/swad/asg/DAFESC/0/0/ECEC /var/www/swad/crs/219
mv /var/www/html/asg/DAFESC/0/0/ECEC /var/www/html/crs/219

mv /var/www/swad/asg/DAFESC/0/0/ICAFD /var/www/swad/crs/220
mv /var/www/html/asg/DAFESC/0/0/ICAFD /var/www/html/crs/220

mv /var/www/swad/asg/DAFESC/0/0/NPIEI /var/www/swad/crs/221
mv /var/www/html/asg/DAFESC/0/0/NPIEI /var/www/html/crs/221

mv /var/www/swad/asg/DAFESC/0/0/PISEF /var/www/swad/crs/222
mv /var/www/html/asg/DAFESC/0/0/PISEF /var/www/html/crs/222

mv /var/www/swad/asg/DAFESC/0/0/RAFSC /var/www/swad/crs/223
mv /var/www/html/asg/DAFESC/0/0/RAFSC /var/www/html/crs/223

mv /var/www/swad/asg/DAFESC/0/0/TCCIS /var/www/swad/crs/224
mv /var/www/html/asg/DAFESC/0/0/TCCIS /var/www/html/crs/224

mv /var/www/swad/asg/DAFESC/0/0/TICAEF /var/www/swad/crs/225
mv /var/www/html/asg/DAFESC/0/0/TICAEF /var/www/html/crs/225

mv /var/www/swad/asg/DAFESC/0/0/USIEF /var/www/swad/crs/226
mv /var/www/html/asg/DAFESC/0/0/USIEF /var/www/html/crs/226

mv /var/www/swad/asg/DAGOT/0/0/ET /var/www/swad/crs/227
mv /var/www/html/asg/DAGOT/0/0/ET /var/www/html/crs/227

mv /var/www/swad/asg/DASDC/0/0/EIMC /var/www/swad/crs/228
mv /var/www/html/asg/DASDC/0/0/EIMC /var/www/html/crs/228

mv /var/www/swad/asg/DAVE/0/0/EAVOL /var/www/swad/crs/229
mv /var/www/html/asg/DAVE/0/0/EAVOL /var/www/html/crs/229

mv /var/www/swad/asg/DCPIEC/1/0/NTII /var/www/swad/crs/230
mv /var/www/html/asg/DCPIEC/1/0/NTII /var/www/html/crs/230

mv /var/www/swad/asg/DDC/0/0/CNCA /var/www/swad/crs/231
mv /var/www/html/asg/DDC/0/0/CNCA /var/www/html/crs/231

mv /var/www/swad/asg/DDCE/0/0/SIDLFT /var/www/swad/crs/232
mv /var/www/html/asg/DDCE/0/0/SIDLFT /var/www/html/crs/232

mv /var/www/swad/asg/DDMPR/0/0/PIRRJ /var/www/swad/crs/233
mv /var/www/html/asg/DDMPR/0/0/PIRRJ /var/www/html/crs/233

mv /var/www/swad/asg/DDMPR/0/0/NFGTM /var/www/swad/crs/234
mv /var/www/html/asg/DDMPR/0/0/NFGTM /var/www/html/crs/234

mv /var/www/swad/asg/DDM/0/2/TEDE /var/www/swad/crs/235
mv /var/www/html/asg/DDM/0/2/TEDE /var/www/html/crs/235

mv /var/www/swad/asg/DDNT/0/0/SMTBA /var/www/swad/crs/236
mv /var/www/html/asg/DDNT/0/0/SMTBA /var/www/html/crs/236

mv /var/www/swad/asg/DDLL/0/1/ATELE /var/www/swad/crs/237
mv /var/www/html/asg/DDLL/0/1/ATELE /var/www/html/crs/237

mv /var/www/swad/asg/DDLL/0/2/CLRLP /var/www/swad/crs/238
mv /var/www/html/asg/DDLL/0/2/CLRLP /var/www/html/crs/238

mv /var/www/swad/asg/DESFM/0/0/VFMIS /var/www/swad/crs/239
mv /var/www/html/asg/DESFM/0/0/VFMIS /var/www/html/crs/239

mv /var/www/swad/asg/DEMG/0/2/EFAD /var/www/swad/crs/240
mv /var/www/html/asg/DEMG/0/2/EFAD /var/www/html/crs/240

mv /var/www/swad/asg/DEMG/0/0/GSMH /var/www/swad/crs/241
mv /var/www/html/asg/DEMG/0/0/GSMH /var/www/html/crs/241

mv /var/www/swad/asg/DEMG/0/0/RTLTF /var/www/swad/crs/242
mv /var/www/html/asg/DEMG/0/0/RTLTF /var/www/html/crs/242

mv /var/www/swad/asg/DEMG/0/2/RPMDP /var/www/swad/crs/243
mv /var/www/html/asg/DEMG/0/2/RPMDP /var/www/html/crs/243

mv /var/www/swad/asg/DESFI/0/0/ALL /var/www/swad/crs/244
mv /var/www/html/asg/DESFI/0/0/ALL /var/www/html/crs/244

mv /var/www/swad/asg/DESFI/0/0/AACIEP /var/www/swad/crs/245
mv /var/www/html/asg/DESFI/0/0/AACIEP /var/www/html/crs/245

mv /var/www/swad/asg/DESFI/0/0/FSALI /var/www/swad/crs/246
mv /var/www/html/asg/DESFI/0/0/FSALI /var/www/html/crs/246

mv /var/www/swad/asg/DESFI/0/0/PGTLI /var/www/swad/crs/247
mv /var/www/html/asg/DESFI/0/0/PGTLI /var/www/html/crs/247

mv /var/www/swad/asg/DIPP/0/0/TIAIP /var/www/swad/crs/248
mv /var/www/html/asg/DIPP/0/0/TIAIP /var/www/html/crs/248

mv /var/www/swad/asg/DICPA/0/0/MBC /var/www/swad/crs/249
mv /var/www/html/asg/DICPA/0/0/MBC /var/www/html/crs/249

mv /var/www/swad/asg/DLPAC/0/0/CLPVAC /var/www/swad/crs/250
mv /var/www/html/asg/DLPAC/0/0/CLPVAC /var/www/html/crs/250

mv /var/www/swad/asg/DLPAC/0/0/CPAEA /var/www/swad/crs/251
mv /var/www/html/asg/DLPAC/0/0/CPAEA /var/www/html/crs/251

mv /var/www/swad/asg/DLPAC/0/0/EAAE /var/www/swad/crs/252
mv /var/www/html/asg/DLPAC/0/0/EAAE /var/www/html/crs/252

mv /var/www/swad/asg/DLPAC/0/0/ECN /var/www/swad/crs/253
mv /var/www/html/asg/DLPAC/0/0/ECN /var/www/html/crs/253

mv /var/www/swad/asg/DLPAC/0/0/FIES /var/www/swad/crs/254
mv /var/www/html/asg/DLPAC/0/0/FIES /var/www/html/crs/254

mv /var/www/swad/asg/DLPAC/0/0/HLRAC /var/www/swad/crs/255
mv /var/www/html/asg/DLPAC/0/0/HLRAC /var/www/html/crs/255

mv /var/www/swad/asg/DLPAC/0/0/CAH /var/www/swad/crs/256
mv /var/www/html/asg/DLPAC/0/0/CAH /var/www/html/crs/256

mv /var/www/swad/asg/DLPAC/0/0/OLES /var/www/swad/crs/257
mv /var/www/html/asg/DLPAC/0/0/OLES /var/www/html/crs/257

mv /var/www/swad/asg/DLPAC/0/0/RE /var/www/swad/crs/258
mv /var/www/html/asg/DLPAC/0/0/RE /var/www/html/crs/258

mv /var/www/swad/asg/DLPAC/0/0/NCIOA /var/www/swad/crs/259
mv /var/www/html/asg/DLPAC/0/0/NCIOA /var/www/html/crs/259

mv /var/www/swad/asg/DLPAC/0/0/PATC /var/www/swad/crs/260
mv /var/www/html/asg/DLPAC/0/0/PATC /var/www/html/crs/260

mv /var/www/swad/asg/DLPAC/0/0/TEUC /var/www/swad/crs/261
mv /var/www/html/asg/DLPAC/0/0/TEUC /var/www/html/crs/261

mv /var/www/swad/asg/DMEA/1/0/ICA /var/www/swad/crs/262
mv /var/www/html/asg/DMEA/1/0/ICA /var/www/html/crs/262

mv /var/www/swad/asg/DMEA/1/0/MEEA /var/www/swad/crs/263
mv /var/www/html/asg/DMEA/1/0/MEEA /var/www/html/crs/263

mv /var/www/swad/asg/DPRL/0/0/PRP /var/www/swad/crs/264
mv /var/www/html/asg/DPRL/0/0/PRP /var/www/html/crs/264

mv /var/www/swad/asg/DQ/1/2/APCLAR /var/www/swad/crs/265
mv /var/www/html/asg/DQ/1/2/APCLAR /var/www/html/crs/265

mv /var/www/swad/asg/DSAH/0/2/PSGHE /var/www/swad/crs/266
mv /var/www/html/asg/DSAH/0/2/PSGHE /var/www/html/crs/266

mv /var/www/swad/asg/DTLA/1/0/SLSC /var/www/swad/crs/267
mv /var/www/html/asg/DTLA/1/0/SLSC /var/www/html/crs/267

mv /var/www/swad/asg/DTSC/0/0/TAT /var/www/swad/crs/268
mv /var/www/html/asg/DTSC/0/0/TAT /var/www/html/crs/268

mv /var/www/swad/asg/DS/1/0/AIG /var/www/swad/crs/269
mv /var/www/html/asg/DS/1/0/AIG /var/www/html/crs/269

mv /var/www/swad/asg/DS/1/0/OAC /var/www/swad/crs/270
mv /var/www/html/asg/DS/1/0/OAC /var/www/html/crs/270

mv /var/www/swad/asg/ECSP/3/0/PO /var/www/swad/crs/271
mv /var/www/html/asg/ECSP/3/0/PO /var/www/html/crs/271

mv /var/www/swad/asg/EEA/1/0/M /var/www/swad/crs/272
mv /var/www/html/asg/EEA/1/0/M /var/www/html/crs/272

mv /var/www/swad/asg/EGENL/1/0/ENLATS /var/www/swad/crs/273
mv /var/www/html/asg/EGENL/1/0/ENLATS /var/www/html/crs/273

mv /var/www/swad/asg/EGENL/1/0/CFFF /var/www/swad/crs/274
mv /var/www/html/asg/EGENL/1/0/CFFF /var/www/html/crs/274

mv /var/www/swad/asg/EGENL/1/0/GENL /var/www/swad/crs/275
mv /var/www/html/asg/EGENL/1/0/GENL /var/www/html/crs/275

mv /var/www/swad/asg/EGENL/1/0/FPENL /var/www/swad/crs/276
mv /var/www/html/asg/EGENL/1/0/FPENL /var/www/html/crs/276

mv /var/www/swad/asg/EGENL/1/0/ICENL /var/www/swad/crs/277
mv /var/www/html/asg/EGENL/1/0/ICENL /var/www/html/crs/277

mv /var/www/swad/asg/EGENL/1/0/ODENL /var/www/swad/crs/278
mv /var/www/html/asg/EGENL/1/0/ODENL /var/www/html/crs/278

mv /var/www/swad/asg/EUSA/1/0/EUSA /var/www/swad/crs/279
mv /var/www/html/asg/EUSA/1/0/EUSA /var/www/html/crs/279

mv /var/www/swad/asg/EUSA/2/0/EUSA /var/www/swad/crs/280
mv /var/www/html/asg/EUSA/2/0/EUSA /var/www/html/crs/280

mv /var/www/swad/asg/GMPDF/0/0/MPDF /var/www/swad/crs/281
mv /var/www/html/asg/GMPDF/0/0/MPDF /var/www/html/crs/281

mv /var/www/swad/asg/GCIIA/0/0/IIBA /var/www/swad/crs/282
mv /var/www/html/asg/GCIIA/0/0/IIBA /var/www/html/crs/282

mv /var/www/swad/asg/GCIIA/0/0/PI /var/www/swad/crs/283
mv /var/www/html/asg/GCIIA/0/0/PI /var/www/html/crs/283

mv /var/www/swad/asg/ICCP/0/2/GAO /var/www/swad/crs/284
mv /var/www/html/asg/ICCP/0/2/GAO /var/www/html/crs/284

mv /var/www/swad/asg/ICCP/0/2/IGMT /var/www/swad/crs/285
mv /var/www/html/asg/ICCP/0/2/IGMT /var/www/html/crs/285

mv /var/www/swad/asg/ICCP/0/2/SHOT /var/www/swad/crs/286
mv /var/www/html/asg/ICCP/0/2/SHOT /var/www/html/crs/286

mv /var/www/swad/asg/ICCP/0/1/TP /var/www/swad/crs/287
mv /var/www/html/asg/ICCP/0/1/TP /var/www/html/crs/287

mv /var/www/swad/asg/ICCP/1/2/CTM /var/www/swad/crs/288
mv /var/www/html/asg/ICCP/1/2/CTM /var/www/html/crs/288

mv /var/www/swad/asg/ICCP/1/2/EDO /var/www/swad/crs/289
mv /var/www/html/asg/ICCP/1/2/EDO /var/www/html/crs/289

mv /var/www/swad/asg/ICCP/1/2/E /var/www/swad/crs/290
mv /var/www/html/asg/ICCP/1/2/E /var/www/html/crs/290

mv /var/www/swad/asg/ICCP/1/2/EIC /var/www/swad/crs/291
mv /var/www/html/asg/ICCP/1/2/EIC /var/www/html/crs/291

mv /var/www/swad/asg/ICCP/1/1/F /var/www/swad/crs/292
mv /var/www/html/asg/ICCP/1/1/F /var/www/html/crs/292

mv /var/www/swad/asg/ICCP/1/1/FII /var/www/swad/crs/293
mv /var/www/html/asg/ICCP/1/1/FII /var/www/html/crs/293

mv /var/www/swad/asg/ICCP/1/1/GG /var/www/swad/crs/294
mv /var/www/html/asg/ICCP/1/1/GG /var/www/html/crs/294

mv /var/www/swad/asg/ICCP/1/2/GGAA /var/www/swad/crs/295
mv /var/www/html/asg/ICCP/1/2/GGAA /var/www/html/crs/295

mv /var/www/swad/asg/ICCP/1/2/GGAB /var/www/swad/crs/296
mv /var/www/html/asg/ICCP/1/2/GGAB /var/www/html/crs/296

mv /var/www/swad/asg/ICCP/1/2/GGAC /var/www/swad/crs/297
mv /var/www/html/asg/ICCP/1/2/GGAC /var/www/html/crs/297

mv /var/www/swad/asg/ICCP/1/1/M1 /var/www/swad/crs/298
mv /var/www/html/asg/ICCP/1/1/M1 /var/www/html/crs/298

mv /var/www/swad/asg/ICCP/1/1/M2 /var/www/swad/crs/299
mv /var/www/html/asg/ICCP/1/1/M2 /var/www/html/crs/299

mv /var/www/swad/asg/ICCP/1/2/MA /var/www/swad/crs/300
mv /var/www/html/asg/ICCP/1/2/MA /var/www/html/crs/300

mv /var/www/swad/asg/ICCP/1/1/TR /var/www/swad/crs/301
mv /var/www/html/asg/ICCP/1/1/TR /var/www/html/crs/301

mv /var/www/swad/asg/ICCP/2/1/CA /var/www/swad/crs/302
mv /var/www/html/asg/ICCP/2/1/CA /var/www/html/crs/302

mv /var/www/swad/asg/ICCP/2/1/E /var/www/swad/crs/303
mv /var/www/html/asg/ICCP/2/1/E /var/www/html/crs/303

mv /var/www/swad/asg/ICCP/2/2/GA /var/www/swad/crs/304
mv /var/www/html/asg/ICCP/2/2/GA /var/www/html/crs/304

mv /var/www/swad/asg/ICCP/2/2/IHH /var/www/swad/crs/305
mv /var/www/html/asg/ICCP/2/2/IHH /var/www/html/crs/305

mv /var/www/swad/asg/ICCP/2/1/IT /var/www/swad/crs/306
mv /var/www/html/asg/ICCP/2/1/IT /var/www/html/crs/306

mv /var/www/swad/asg/ICCP/2/2/LAPU /var/www/swad/crs/307
mv /var/www/html/asg/ICCP/2/2/LAPU /var/www/html/crs/307

mv /var/www/swad/asg/ICCP/2/1/MSR /var/www/swad/crs/308
mv /var/www/html/asg/ICCP/2/1/MSR /var/www/html/crs/308

mv /var/www/swad/asg/ICCP/2/2/MART /var/www/swad/crs/309
mv /var/www/html/asg/ICCP/2/2/MART /var/www/html/crs/309

mv /var/www/swad/asg/ICCP/2/1/MMT /var/www/swad/crs/310
mv /var/www/html/asg/ICCP/2/1/MMT /var/www/html/crs/310

mv /var/www/swad/asg/ICCP/2/1/TAEIC /var/www/swad/crs/311
mv /var/www/html/asg/ICCP/2/1/TAEIC /var/www/html/crs/311

mv /var/www/swad/asg/ICCP/2/1/TIIC /var/www/swad/crs/312
mv /var/www/html/asg/ICCP/2/1/TIIC /var/www/html/crs/312

mv /var/www/swad/asg/ICCP/2/0/TE /var/www/swad/crs/313
mv /var/www/html/asg/ICCP/2/0/TE /var/www/html/crs/313

mv /var/www/swad/asg/ICCP/2/2/TF /var/www/swad/crs/314
mv /var/www/html/asg/ICCP/2/2/TF /var/www/html/crs/314

mv /var/www/swad/asg/ICCP/2/1/T /var/www/swad/crs/315
mv /var/www/html/asg/ICCP/2/1/T /var/www/html/crs/315

mv /var/www/swad/asg/ICCP/3/2/AE1 /var/www/swad/crs/316
mv /var/www/html/asg/ICCP/3/2/AE1 /var/www/html/crs/316

mv /var/www/swad/asg/ICCP/3/2/ANA /var/www/swad/crs/317
mv /var/www/html/asg/ICCP/3/2/ANA /var/www/html/crs/317

mv /var/www/swad/asg/ICCP/3/2/ANB /var/www/swad/crs/318
mv /var/www/html/asg/ICCP/3/2/ANB /var/www/html/crs/318

mv /var/www/swad/asg/ICCP/3/1/CA /var/www/swad/crs/319
mv /var/www/html/asg/ICCP/3/1/CA /var/www/html/crs/319

mv /var/www/swad/asg/ICCP/3/1/EDP /var/www/swad/crs/320
mv /var/www/html/asg/ICCP/3/1/EDP /var/www/html/crs/320

mv /var/www/swad/asg/ICCP/3/1/E /var/www/swad/crs/321
mv /var/www/html/asg/ICCP/3/1/E /var/www/html/crs/321

mv /var/www/swad/asg/ICCP/3/2/GC /var/www/swad/crs/322
mv /var/www/html/asg/ICCP/3/2/GC /var/www/html/crs/322

mv /var/www/swad/asg/ICCP/3/2/HSS /var/www/swad/crs/323
mv /var/www/html/asg/ICCP/3/2/HSS /var/www/html/crs/323

mv /var/www/swad/asg/ICCP/3/1/MMC /var/www/swad/crs/324
mv /var/www/html/asg/ICCP/3/1/MMC /var/www/html/crs/324

mv /var/www/swad/asg/ICCP/3/2/OAHE /var/www/swad/crs/325
mv /var/www/html/asg/ICCP/3/2/OAHE /var/www/html/crs/325

mv /var/www/swad/asg/ICCP/3/1/SC /var/www/swad/crs/326
mv /var/www/html/asg/ICCP/3/1/SC /var/www/html/crs/326

mv /var/www/swad/asg/ICCP/3/2/U /var/www/swad/crs/327
mv /var/www/html/asg/ICCP/3/2/U /var/www/html/crs/327

mv /var/www/swad/asg/ICCP/3/1/UOT /var/www/swad/crs/328
mv /var/www/html/asg/ICCP/3/1/UOT /var/www/html/crs/328

mv /var/www/swad/asg/ICCP/4/1/AE2 /var/www/swad/crs/329
mv /var/www/html/asg/ICCP/4/1/AE2 /var/www/html/crs/329

mv /var/www/swad/asg/ICCP/4/1/DGOLIC /var/www/swad/crs/330
mv /var/www/html/asg/ICCP/4/1/DGOLIC /var/www/html/crs/330

mv /var/www/swad/asg/ICCP/4/1/F /var/www/swad/crs/331
mv /var/www/html/asg/ICCP/4/1/F /var/www/html/crs/331

mv /var/www/swad/asg/ICCP/4/1/HF /var/www/swad/crs/332
mv /var/www/html/asg/ICCP/4/1/HF /var/www/html/crs/332

mv /var/www/swad/asg/ICCP/4/2/HAPB /var/www/swad/crs/333
mv /var/www/html/asg/ICCP/4/2/HAPB /var/www/html/crs/333

mv /var/www/swad/asg/ICCP/4/2/IAOP /var/www/swad/crs/334
mv /var/www/html/asg/ICCP/4/2/IAOP /var/www/html/crs/334

mv /var/www/swad/asg/ICCP/4/1/IMC /var/www/swad/crs/335
mv /var/www/html/asg/ICCP/4/1/IMC /var/www/html/crs/335

mv /var/www/swad/asg/ICCP/4/1/ISMA /var/www/swad/crs/336
mv /var/www/html/asg/ICCP/4/1/ISMA /var/www/html/crs/336

mv /var/www/swad/asg/ICCP/4/2/ISE /var/www/swad/crs/337
mv /var/www/html/asg/ICCP/4/2/ISE /var/www/html/crs/337

mv /var/www/swad/asg/ICCP/4/2/MF /var/www/swad/crs/338
mv /var/www/html/asg/ICCP/4/2/MF /var/www/html/crs/338

mv /var/www/swad/asg/ICCP/4/2/PETT /var/www/swad/crs/339
mv /var/www/html/asg/ICCP/4/2/PETT /var/www/html/crs/339

mv /var/www/swad/asg/ICCP/4/1/PC1 /var/www/swad/crs/340
mv /var/www/html/asg/ICCP/4/1/PC1 /var/www/html/crs/340

mv /var/www/swad/asg/ICCP/4/2/PC2 /var/www/swad/crs/341
mv /var/www/html/asg/ICCP/4/2/PC2 /var/www/html/crs/341

mv /var/www/swad/asg/ICCP/5/0/AC /var/www/swad/crs/342
mv /var/www/html/asg/ICCP/5/0/AC /var/www/html/crs/342

mv /var/www/swad/asg/ICCP/5/1/AH /var/www/swad/crs/343
mv /var/www/html/asg/ICCP/5/1/AH /var/www/html/crs/343

mv /var/www/swad/asg/ICCP/5/1/AAE /var/www/swad/crs/344
mv /var/www/html/asg/ICCP/5/1/AAE /var/www/html/crs/344

mv /var/www/swad/asg/ICCP/5/1/ANEE /var/www/swad/crs/345
mv /var/www/html/asg/ICCP/5/1/ANEE /var/www/html/crs/345

mv /var/www/swad/asg/ICCP/5/1/DSR /var/www/swad/crs/346
mv /var/www/html/asg/ICCP/5/1/DSR /var/www/html/crs/346

mv /var/www/swad/asg/ICCP/5/2/DEPE /var/www/swad/crs/347
mv /var/www/html/asg/ICCP/5/2/DEPE /var/www/html/crs/347

mv /var/www/swad/asg/ICCP/5/1/EP /var/www/swad/crs/348
mv /var/www/html/asg/ICCP/5/1/EP /var/www/html/crs/348

mv /var/www/swad/asg/ICCP/5/2/ECC /var/www/swad/crs/349
mv /var/www/html/asg/ICCP/5/2/ECC /var/www/html/crs/349

mv /var/www/swad/asg/ICCP/5/1/EMM /var/www/swad/crs/350
mv /var/www/html/asg/ICCP/5/1/EMM /var/www/html/crs/350

mv /var/www/swad/asg/ICCP/5/2/EP /var/www/swad/crs/351
mv /var/www/html/asg/ICCP/5/2/EP /var/www/html/crs/351

mv /var/www/swad/asg/ICCP/5/2/HSAS /var/www/swad/crs/352
mv /var/www/html/asg/ICCP/5/2/HSAS /var/www/html/crs/352

mv /var/www/swad/asg/ICCP/5/1/IA /var/www/swad/crs/353
mv /var/www/html/asg/ICCP/5/1/IA /var/www/html/crs/353

mv /var/www/swad/asg/ICCP/5/2/IT /var/www/swad/crs/354
mv /var/www/html/asg/ICCP/5/2/IT /var/www/html/crs/354

mv /var/www/swad/asg/ICCP/5/2/IO /var/www/swad/crs/355
mv /var/www/html/asg/ICCP/5/2/IO /var/www/html/crs/355

mv /var/www/swad/asg/ICCP/5/1/IS /var/www/swad/crs/356
mv /var/www/html/asg/ICCP/5/1/IS /var/www/html/crs/356

mv /var/www/swad/asg/ICCP/5/2/L /var/www/swad/crs/357
mv /var/www/html/asg/ICCP/5/2/L /var/www/html/crs/357

mv /var/www/swad/asg/ICCP/5/2/MR /var/www/swad/crs/358
mv /var/www/html/asg/ICCP/5/2/MR /var/www/html/crs/358

mv /var/www/swad/asg/ICCP/5/2/IMM /var/www/swad/crs/359
mv /var/www/html/asg/ICCP/5/2/IMM /var/www/html/crs/359

mv /var/www/swad/asg/ICCP/5/2/OST /var/www/swad/crs/360
mv /var/www/html/asg/ICCP/5/2/OST /var/www/html/crs/360

mv /var/www/swad/asg/ICCP/5/2/OGE /var/www/swad/crs/361
mv /var/www/html/asg/ICCP/5/2/OGE /var/www/html/crs/361

mv /var/www/swad/asg/ICCP/5/1/OGPO /var/www/swad/crs/362
mv /var/www/html/asg/ICCP/5/1/OGPO /var/www/html/crs/362

mv /var/www/swad/asg/ICCP/5/1/PAH /var/www/swad/crs/363
mv /var/www/html/asg/ICCP/5/1/PAH /var/www/html/crs/363

mv /var/www/swad/asg/ICCP/5/1/PET /var/www/swad/crs/364
mv /var/www/html/asg/ICCP/5/1/PET /var/www/html/crs/364

mv /var/www/swad/asg/ICCP/5/1/PFC /var/www/swad/crs/365
mv /var/www/html/asg/ICCP/5/1/PFC /var/www/html/crs/365

mv /var/www/swad/asg/ICCP/5/2/PFC /var/www/swad/crs/366
mv /var/www/html/asg/ICCP/5/2/PFC /var/www/html/crs/366

mv /var/www/swad/asg/ICCP/5/0/P /var/www/swad/crs/367
mv /var/www/html/asg/ICCP/5/0/P /var/www/html/crs/367

mv /var/www/swad/asg/ICCP/5/1/P /var/www/swad/crs/368
mv /var/www/html/asg/ICCP/5/1/P /var/www/html/crs/368

mv /var/www/swad/asg/ICCP/5/0/SE /var/www/swad/crs/369
mv /var/www/html/asg/ICCP/5/0/SE /var/www/html/crs/369

mv /var/www/swad/asg/ICCP/5/1/TA /var/www/swad/crs/370
mv /var/www/html/asg/ICCP/5/1/TA /var/www/html/crs/370

mv /var/www/swad/asg/ICCP/5/1/TC /var/www/swad/crs/371
mv /var/www/html/asg/ICCP/5/1/TC /var/www/html/crs/371

mv /var/www/swad/asg/ICCP/5/1/TT /var/www/swad/crs/372
mv /var/www/html/asg/ICCP/5/1/TT /var/www/html/crs/372

mv /var/www/swad/asg/ICCP/5/0/TSU /var/www/swad/crs/373
mv /var/www/html/asg/ICCP/5/0/TSU /var/www/html/crs/373

mv /var/www/swad/asg/ICCP/5/0/U /var/www/swad/crs/374
mv /var/www/html/asg/ICCP/5/0/U /var/www/html/crs/374

mv /var/www/swad/asg/IE/1/2/C /var/www/swad/crs/375
mv /var/www/html/asg/IE/1/2/C /var/www/html/crs/375

mv /var/www/swad/asg/IE/1/1/DCSE /var/www/swad/crs/376
mv /var/www/html/asg/IE/1/1/DCSE /var/www/html/crs/376

mv /var/www/swad/asg/IE/1/2/DEF /var/www/swad/crs/377
mv /var/www/html/asg/IE/1/2/DEF /var/www/html/crs/377

mv /var/www/swad/asg/IE/1/1/EC /var/www/swad/crs/378
mv /var/www/html/asg/IE/1/1/EC /var/www/html/crs/378

mv /var/www/swad/asg/IE/1/1/PI /var/www/swad/crs/379
mv /var/www/html/asg/IE/1/1/PI /var/www/html/crs/379

mv /var/www/swad/asg/IE/1/0/SAI /var/www/swad/crs/380
mv /var/www/html/asg/IE/1/0/SAI /var/www/html/crs/380

mv /var/www/swad/asg/IE/1/2/SM /var/www/swad/crs/381
mv /var/www/html/asg/IE/1/2/SM /var/www/html/crs/381

mv /var/www/swad/asg/IE/1/2/TD /var/www/swad/crs/382
mv /var/www/html/asg/IE/1/2/TD /var/www/html/crs/382

mv /var/www/swad/asg/IE/1/0/TTS /var/www/swad/crs/383
mv /var/www/html/asg/IE/1/0/TTS /var/www/html/crs/383

mv /var/www/swad/asg/IE/2/2/CE /var/www/swad/crs/384
mv /var/www/html/asg/IE/2/2/CE /var/www/html/crs/384

mv /var/www/swad/asg/IE/2/1/CL /var/www/swad/crs/385
mv /var/www/html/asg/IE/2/1/CL /var/www/html/crs/385

mv /var/www/swad/asg/IE/2/1/DCI /var/www/swad/crs/386
mv /var/www/html/asg/IE/2/1/DCI /var/www/html/crs/386

mv /var/www/swad/asg/IE/2/2/DSE /var/www/swad/crs/387
mv /var/www/html/asg/IE/2/2/DSE /var/www/html/crs/387

mv /var/www/swad/asg/IE/2/2/DCIM /var/www/swad/crs/388
mv /var/www/html/asg/IE/2/2/DCIM /var/www/html/crs/388

mv /var/www/swad/asg/IE/2/1/EA /var/www/swad/crs/389
mv /var/www/html/asg/IE/2/1/EA /var/www/html/crs/389

mv /var/www/swad/asg/IE/2/1/EETS /var/www/swad/crs/390
mv /var/www/html/asg/IE/2/1/EETS /var/www/html/crs/390

mv /var/www/swad/asg/IE/2/1/FOEOI /var/www/swad/crs/391
mv /var/www/html/asg/IE/2/1/FOEOI /var/www/html/crs/391

mv /var/www/swad/asg/IE/2/1/IACA /var/www/swad/crs/392
mv /var/www/html/asg/IE/2/1/IACA /var/www/html/crs/392

mv /var/www/swad/asg/IE/2/2/LPSC /var/www/swad/crs/393
mv /var/www/html/asg/IE/2/2/LPSC /var/www/html/crs/393

mv /var/www/swad/asg/IE/2/1/P /var/www/swad/crs/394
mv /var/www/html/asg/IE/2/1/P /var/www/html/crs/394

mv /var/www/swad/asg/IE/2/2/SACC /var/www/swad/crs/395
mv /var/www/html/asg/IE/2/2/SACC /var/www/html/crs/395

mv /var/www/swad/asg/IE/2/0/ST /var/www/swad/crs/396
mv /var/www/html/asg/IE/2/0/ST /var/www/html/crs/396

mv /var/www/swad/asg/IE/2/2/TEEM /var/www/swad/crs/397
mv /var/www/html/asg/IE/2/2/TEEM /var/www/html/crs/397

mv /var/www/swad/asg/IE/2/2/TEPDS /var/www/swad/crs/398
mv /var/www/html/asg/IE/2/2/TEPDS /var/www/html/crs/398

mv /var/www/swad/asg/IE/2/2/TMSE /var/www/swad/crs/399
mv /var/www/html/asg/IE/2/2/TMSE /var/www/html/crs/399

mv /var/www/swad/asg/IE/2/0/TFC /var/www/swad/crs/400
mv /var/www/html/asg/IE/2/0/TFC /var/www/html/crs/400

mv /var/www/swad/asg/II/0/2/IFE2 /var/www/swad/crs/401
mv /var/www/html/asg/II/0/2/IFE2 /var/www/html/crs/401

mv /var/www/swad/asg/II/1/1/AEF /var/www/swad/crs/402
mv /var/www/html/asg/II/1/1/AEF /var/www/html/crs/402

mv /var/www/swad/asg/II/1/1/AM /var/www/swad/crs/403
mv /var/www/html/asg/II/1/1/AM /var/www/html/crs/403

mv /var/www/swad/asg/II/1/2/EA /var/www/swad/crs/404
mv /var/www/html/asg/II/1/2/EA /var/www/html/crs/404

mv /var/www/swad/asg/II/1/2/EB /var/www/swad/crs/405
mv /var/www/html/asg/II/1/2/EB /var/www/html/crs/405

mv /var/www/swad/asg/II/1/2/ED /var/www/swad/crs/406
mv /var/www/html/asg/II/1/2/ED /var/www/html/crs/406

mv /var/www/swad/asg/II/1/2/FLPA /var/www/swad/crs/407
mv /var/www/html/asg/II/1/2/FLPA /var/www/html/crs/407

mv /var/www/swad/asg/II/1/2/FLPB /var/www/swad/crs/408
mv /var/www/html/asg/II/1/2/FLPB /var/www/html/crs/408

mv /var/www/swad/asg/II/1/1/FFC /var/www/swad/crs/409
mv /var/www/html/asg/II/1/1/FFC /var/www/html/crs/409

mv /var/www/swad/asg/II/1/2/FTC /var/www/swad/crs/410
mv /var/www/html/asg/II/1/2/FTC /var/www/html/crs/410

mv /var/www/swad/asg/II/1/1/ICA /var/www/swad/crs/411
mv /var/www/html/asg/II/1/1/ICA /var/www/html/crs/411

mv /var/www/swad/asg/II/1/1/IC /var/www/swad/crs/412
mv /var/www/html/asg/II/1/1/IC /var/www/html/crs/412

mv /var/www/swad/asg/II/1/1/MD /var/www/swad/crs/413
mv /var/www/html/asg/II/1/1/MD /var/www/html/crs/413

mv /var/www/swad/asg/II/1/1/MP1 /var/www/swad/crs/414
mv /var/www/html/asg/II/1/1/MP1 /var/www/html/crs/414

mv /var/www/swad/asg/II/1/2/MP2 /var/www/swad/crs/415
mv /var/www/html/asg/II/1/2/MP2 /var/www/html/crs/415

mv /var/www/swad/asg/II/1/2/TC1A /var/www/swad/crs/416
mv /var/www/html/asg/II/1/2/TC1A /var/www/html/crs/416

mv /var/www/swad/asg/II/1/2/TC1B /var/www/swad/crs/417
mv /var/www/html/asg/II/1/2/TC1B /var/www/html/crs/417

mv /var/www/swad/asg/II/2/1/BD1 /var/www/swad/crs/418
mv /var/www/html/asg/II/2/1/BD1 /var/www/html/crs/418

mv /var/www/swad/asg/II/2/2/BD2 /var/www/swad/crs/419
mv /var/www/html/asg/II/2/2/BD2 /var/www/html/crs/419

mv /var/www/swad/asg/II/2/2/CN /var/www/swad/crs/420
mv /var/www/html/asg/II/2/2/CN /var/www/html/crs/420

mv /var/www/swad/asg/II/2/1/EC1 /var/www/swad/crs/421
mv /var/www/html/asg/II/2/1/EC1 /var/www/html/crs/421

mv /var/www/swad/asg/II/2/2/EC2 /var/www/swad/crs/422
mv /var/www/html/asg/II/2/2/EC2 /var/www/html/crs/422

mv /var/www/swad/asg/II/2/1/MC1 /var/www/swad/crs/423
mv /var/www/html/asg/II/2/1/MC1 /var/www/html/crs/423

mv /var/www/swad/asg/II/2/2/MC2 /var/www/swad/crs/424
mv /var/www/html/asg/II/2/2/MC2 /var/www/html/crs/424

mv /var/www/swad/asg/II/2/1/SO1 /var/www/swad/crs/425
mv /var/www/html/asg/II/2/1/SO1 /var/www/html/crs/425

mv /var/www/swad/asg/II/2/2/SO2 /var/www/swad/crs/426
mv /var/www/html/asg/II/2/2/SO2 /var/www/html/crs/426

mv /var/www/swad/asg/II/2/1/TC2A /var/www/swad/crs/427
mv /var/www/html/asg/II/2/1/TC2A /var/www/html/crs/427

mv /var/www/swad/asg/II/2/1/TC2B /var/www/swad/crs/428
mv /var/www/html/asg/II/2/1/TC2B /var/www/html/crs/428

mv /var/www/swad/asg/II/2/1/TA /var/www/swad/crs/429
mv /var/www/html/asg/II/2/1/TA /var/www/html/crs/429

mv /var/www/swad/asg/II/3/2/AN /var/www/swad/crs/430
mv /var/www/html/asg/II/3/2/AN /var/www/html/crs/430

mv /var/www/swad/asg/II/3/1/CA /var/www/swad/crs/431
mv /var/www/html/asg/II/3/1/CA /var/www/html/crs/431

mv /var/www/swad/asg/II/3/1/DLP /var/www/swad/crs/432
mv /var/www/html/asg/II/3/1/DLP /var/www/html/crs/432

mv /var/www/swad/asg/II/3/2/DCM /var/www/swad/crs/433
mv /var/www/html/asg/II/3/2/DCM /var/www/html/crs/433

mv /var/www/swad/asg/II/3/1/DCM /var/www/swad/crs/434
mv /var/www/html/asg/II/3/1/DCM /var/www/html/crs/434

mv /var/www/swad/asg/II/3/2/GBD /var/www/swad/crs/435
mv /var/www/html/asg/II/3/2/GBD /var/www/html/crs/435

mv /var/www/swad/asg/II/3/2/FATC /var/www/swad/crs/436
mv /var/www/html/asg/II/3/2/FATC /var/www/html/crs/436

mv /var/www/swad/asg/II/3/1/IHA /var/www/swad/crs/437
mv /var/www/html/asg/II/3/1/IHA /var/www/html/crs/437

mv /var/www/swad/asg/II/3/2/IG /var/www/swad/crs/438
mv /var/www/html/asg/II/3/2/IG /var/www/html/crs/438

mv /var/www/swad/asg/II/3/2/IC /var/www/swad/crs/439
mv /var/www/html/asg/II/3/2/IC /var/www/html/crs/439

mv /var/www/swad/asg/II/3/1/IS1 /var/www/swad/crs/440
mv /var/www/html/asg/II/3/1/IS1 /var/www/html/crs/440

mv /var/www/swad/asg/II/3/1/LI /var/www/swad/crs/441
mv /var/www/html/asg/II/3/1/LI /var/www/html/crs/441

mv /var/www/swad/asg/II/3/2/MIT /var/www/swad/crs/442
mv /var/www/html/asg/II/3/2/MIT /var/www/html/crs/442

mv /var/www/swad/asg/II/3/2/PC /var/www/swad/crs/443
mv /var/www/html/asg/II/3/2/PC /var/www/html/crs/443

mv /var/www/swad/asg/II/3/1/PD /var/www/swad/crs/444
mv /var/www/html/asg/II/3/1/PD /var/www/html/crs/444

mv /var/www/swad/asg/II/3/1/PDO /var/www/swad/crs/445
mv /var/www/html/asg/II/3/1/PDO /var/www/html/crs/445

mv /var/www/swad/asg/II/3/2/TIC /var/www/swad/crs/446
mv /var/www/html/asg/II/3/2/TIC /var/www/html/crs/446

mv /var/www/swad/asg/II/4/1/A /var/www/swad/crs/447
mv /var/www/html/asg/II/4/1/A /var/www/html/crs/447

mv /var/www/swad/asg/II/4/1/CLP /var/www/swad/crs/448
mv /var/www/html/asg/II/4/1/CLP /var/www/html/crs/448

mv /var/www/swad/asg/II/4/2/C /var/www/swad/crs/449
mv /var/www/html/asg/II/4/2/C /var/www/html/crs/449

mv /var/www/swad/asg/II/4/1/DAC /var/www/swad/crs/450
mv /var/www/html/asg/II/4/1/DAC /var/www/html/crs/450

mv /var/www/swad/asg/II/4/1/IS2 /var/www/swad/crs/451
mv /var/www/html/asg/II/4/1/IS2 /var/www/html/crs/451

mv /var/www/swad/asg/II/4/2/IS3 /var/www/swad/crs/452
mv /var/www/html/asg/II/4/2/IS3 /var/www/html/crs/452

mv /var/www/swad/asg/II/4/1/IAIC /var/www/swad/crs/453
mv /var/www/html/asg/II/4/1/IAIC /var/www/html/crs/453

mv /var/www/swad/asg/II/4/2/MABD /var/www/swad/crs/454
mv /var/www/html/asg/II/4/2/MABD /var/www/html/crs/454

mv /var/www/swad/asg/II/4/2/PL /var/www/swad/crs/455
mv /var/www/html/asg/II/4/2/PL /var/www/html/crs/455

mv /var/www/swad/asg/II/4/1/PDP /var/www/swad/crs/456
mv /var/www/html/asg/II/4/1/PDP /var/www/html/crs/456

mv /var/www/swad/asg/II/4/2/RI /var/www/swad/crs/457
mv /var/www/html/asg/II/4/2/RI /var/www/html/crs/457

mv /var/www/swad/asg/II/4/1/SCD /var/www/swad/crs/458
mv /var/www/html/asg/II/4/1/SCD /var/www/html/crs/458

mv /var/www/swad/asg/II/4/2/SAAV /var/www/swad/crs/459
mv /var/www/html/asg/II/4/2/SAAV /var/www/html/crs/459

mv /var/www/swad/asg/II/4/1/TDRC1 /var/www/swad/crs/460
mv /var/www/html/asg/II/4/1/TDRC1 /var/www/html/crs/460

mv /var/www/swad/asg/II/4/2/TDRC2 /var/www/swad/crs/461
mv /var/www/html/asg/II/4/2/TDRC2 /var/www/html/crs/461

mv /var/www/swad/asg/II/4/1/VA /var/www/swad/crs/462
mv /var/www/html/asg/II/4/1/VA /var/www/html/crs/462

mv /var/www/swad/asg/II/4/2/VR /var/www/swad/crs/463
mv /var/www/html/asg/II/4/2/VR /var/www/html/crs/463

mv /var/www/swad/asg/II/5/1/AC1 /var/www/swad/crs/464
mv /var/www/html/asg/II/5/1/AC1 /var/www/html/crs/464

mv /var/www/swad/asg/II/5/2/AC2 /var/www/swad/crs/465
mv /var/www/html/asg/II/5/2/AC2 /var/www/html/crs/465

mv /var/www/swad/asg/II/5/2/AE /var/www/swad/crs/466
mv /var/www/html/asg/II/5/2/AE /var/www/html/crs/466

mv /var/www/swad/asg/II/5/2/B /var/www/swad/crs/467
mv /var/www/html/asg/II/5/2/B /var/www/html/crs/467

mv /var/www/swad/asg/II/5/1/CS /var/www/swad/crs/468
mv /var/www/html/asg/II/5/1/CS /var/www/html/crs/468

mv /var/www/swad/asg/II/5/2/DI /var/www/swad/crs/469
mv /var/www/html/asg/II/5/2/DI /var/www/html/crs/469

mv /var/www/swad/asg/II/5/1/DASD /var/www/swad/crs/470
mv /var/www/html/asg/II/5/1/DASD /var/www/html/crs/470

mv /var/www/swad/asg/II/5/2/DS0 /var/www/swad/crs/471
mv /var/www/html/asg/II/5/2/DS0 /var/www/html/crs/471

mv /var/www/swad/asg/II/5/1/ICA /var/www/swad/crs/472
mv /var/www/html/asg/II/5/1/ICA /var/www/html/crs/472

mv /var/www/swad/asg/II/5/2/ISLN /var/www/swad/crs/473
mv /var/www/html/asg/II/5/2/ISLN /var/www/html/crs/473

mv /var/www/swad/asg/II/5/1/MIA /var/www/swad/crs/474
mv /var/www/html/asg/II/5/1/MIA /var/www/html/crs/474

mv /var/www/swad/asg/II/5/2/N /var/www/swad/crs/475
mv /var/www/html/asg/II/5/2/N /var/www/html/crs/475

mv /var/www/swad/asg/II/5/2/PV /var/www/swad/crs/476
mv /var/www/html/asg/II/5/2/PV /var/www/html/crs/476

mv /var/www/swad/asg/II/5/1/PI1 /var/www/swad/crs/477
mv /var/www/html/asg/II/5/1/PI1 /var/www/html/crs/477

mv /var/www/swad/asg/II/5/2/PI2 /var/www/swad/crs/478
mv /var/www/html/asg/II/5/2/PI2 /var/www/html/crs/478

mv /var/www/swad/asg/II/5/1/RF /var/www/swad/crs/479
mv /var/www/html/asg/II/5/1/RF /var/www/html/crs/479

mv /var/www/swad/asg/II/5/1/SLP /var/www/swad/crs/480
mv /var/www/html/asg/II/5/1/SLP /var/www/html/crs/480

mv /var/www/swad/asg/II/5/1/SI /var/www/swad/crs/481
mv /var/www/html/asg/II/5/1/SI /var/www/html/crs/481

mv /var/www/swad/asg/II/5/1/TSC /var/www/swad/crs/482
mv /var/www/html/asg/II/5/1/TSC /var/www/html/crs/482

mv /var/www/swad/asg/IQ/1/1/FIQ /var/www/swad/crs/483
mv /var/www/html/asg/IQ/1/1/FIQ /var/www/html/crs/483

mv /var/www/swad/asg/IQ/1/0/FFI /var/www/swad/crs/484
mv /var/www/html/asg/IQ/1/0/FFI /var/www/html/crs/484

mv /var/www/swad/asg/IQ/1/2/OBFFA /var/www/swad/crs/485
mv /var/www/html/asg/IQ/1/2/OBFFA /var/www/html/crs/485

mv /var/www/swad/asg/IQ/1/0/QAA /var/www/swad/crs/486
mv /var/www/html/asg/IQ/1/0/QAA /var/www/html/crs/486

mv /var/www/swad/asg/IQ/1/0/QAB /var/www/swad/crs/487
mv /var/www/html/asg/IQ/1/0/QAB /var/www/html/crs/487

mv /var/www/swad/asg/IQ/2/1/M2 /var/www/swad/crs/488
mv /var/www/html/asg/IQ/2/1/M2 /var/www/html/crs/488

mv /var/www/swad/asg/IQ/2/1/QF /var/www/swad/crs/489
mv /var/www/html/asg/IQ/2/1/QF /var/www/html/crs/489

mv /var/www/swad/asg/IQ/2/2/TSAC /var/www/swad/crs/490
mv /var/www/html/asg/IQ/2/2/TSAC /var/www/html/crs/490

mv /var/www/swad/asg/IQ/2/2/TQA /var/www/swad/crs/491
mv /var/www/html/asg/IQ/2/2/TQA /var/www/html/crs/491

mv /var/www/swad/asg/IQ/3/1/CQA /var/www/swad/crs/492
mv /var/www/html/asg/IQ/3/1/CQA /var/www/html/crs/492

mv /var/www/swad/asg/IQ/3/2/MEAI /var/www/swad/crs/493
mv /var/www/html/asg/IQ/3/2/MEAI /var/www/html/crs/493

mv /var/www/swad/asg/IQ/3/2/TEA /var/www/swad/crs/494
mv /var/www/html/asg/IQ/3/2/TEA /var/www/html/crs/494

mv /var/www/swad/asg/IQ/4/2/RQ /var/www/swad/crs/495
mv /var/www/html/asg/IQ/4/2/RQ /var/www/html/crs/495

mv /var/www/swad/asg/IQ/4/2/TMA /var/www/swad/crs/496
mv /var/www/html/asg/IQ/4/2/TMA /var/www/html/crs/496

mv /var/www/swad/asg/IQ/4/1/T /var/www/swad/crs/497
mv /var/www/html/asg/IQ/4/1/T /var/www/html/crs/497

mv /var/www/swad/asg/IQ/5/1/B /var/www/swad/crs/498
mv /var/www/html/asg/IQ/5/1/B /var/www/html/crs/498

mv /var/www/swad/asg/IQ/5/1/CIPQ /var/www/swad/crs/499
mv /var/www/html/asg/IQ/5/1/CIPQ /var/www/html/crs/499

mv /var/www/swad/asg/IQ/5/2/DEG /var/www/swad/crs/500
mv /var/www/html/asg/IQ/5/2/DEG /var/www/html/crs/500

mv /var/www/swad/asg/IQ/5/1/EIQ3 /var/www/swad/crs/501
mv /var/www/html/asg/IQ/5/1/EIQ3 /var/www/html/crs/501

mv /var/www/swad/asg/IQ/5/2/OBIA /var/www/swad/crs/502
mv /var/www/html/asg/IQ/5/2/OBIA /var/www/html/crs/502

mv /var/www/swad/asg/IT/1/1/AL /var/www/swad/crs/503
mv /var/www/html/asg/IT/1/1/AL /var/www/html/crs/503

mv /var/www/swad/asg/IT/1/1/ACA /var/www/swad/crs/504
mv /var/www/html/asg/IT/1/1/ACA /var/www/html/crs/504

mv /var/www/swad/asg/IT/1/0/C /var/www/swad/crs/505
mv /var/www/html/asg/IT/1/0/C /var/www/html/crs/505

mv /var/www/swad/asg/IT/1/2/DE1 /var/www/swad/crs/506
mv /var/www/html/asg/IT/1/2/DE1 /var/www/html/crs/506

mv /var/www/swad/asg/IT/1/2/ED /var/www/swad/crs/507
mv /var/www/html/asg/IT/1/2/ED /var/www/html/crs/507

mv /var/www/swad/asg/IT/1/0/FP /var/www/swad/crs/508
mv /var/www/html/asg/IT/1/0/FP /var/www/html/crs/508

mv /var/www/swad/asg/IT/1/0/FFI /var/www/swad/crs/509
mv /var/www/html/asg/IT/1/0/FFI /var/www/html/crs/509

mv /var/www/swad/asg/IT/2/0/CE /var/www/swad/crs/510
mv /var/www/html/asg/IT/2/0/CE /var/www/html/crs/510

mv /var/www/swad/asg/IT/2/1/DE2 /var/www/swad/crs/511
mv /var/www/html/asg/IT/2/1/DE2 /var/www/html/crs/511

mv /var/www/swad/asg/IT/2/0/EA /var/www/swad/crs/512
mv /var/www/html/asg/IT/2/0/EA /var/www/html/crs/512

mv /var/www/swad/asg/IT/2/2/FC /var/www/swad/crs/513
mv /var/www/html/asg/IT/2/2/FC /var/www/html/crs/513

mv /var/www/swad/asg/IT/2/1/FM1 /var/www/swad/crs/514
mv /var/www/html/asg/IT/2/1/FM1 /var/www/html/crs/514

mv /var/www/swad/asg/IT/2/2/FM2 /var/www/swad/crs/515
mv /var/www/html/asg/IT/2/2/FM2 /var/www/html/crs/515

mv /var/www/swad/asg/IT/2/2/SA /var/www/swad/crs/516
mv /var/www/html/asg/IT/2/2/SA /var/www/html/crs/516

mv /var/www/swad/asg/IT/2/1/SA /var/www/swad/crs/517
mv /var/www/html/asg/IT/2/1/SA /var/www/html/crs/517

mv /var/www/swad/asg/IT/2/2/SD /var/www/swad/crs/518
mv /var/www/html/asg/IT/2/2/SD /var/www/html/crs/518

mv /var/www/swad/asg/IT/3/1/CD /var/www/swad/crs/519
mv /var/www/html/asg/IT/3/1/CD /var/www/html/crs/519

mv /var/www/swad/asg/IT/3/1/DAI /var/www/swad/crs/520
mv /var/www/html/asg/IT/3/1/DAI /var/www/html/crs/520

mv /var/www/swad/asg/IT/3/2/IA /var/www/swad/crs/521
mv /var/www/html/asg/IT/3/2/IA /var/www/html/crs/521

mv /var/www/swad/asg/IT/3/1/SS /var/www/swad/crs/522
mv /var/www/html/asg/IT/3/1/SS /var/www/html/crs/522

mv /var/www/swad/asg/IT/3/0/SED /var/www/swad/crs/523
mv /var/www/html/asg/IT/3/0/SED /var/www/html/crs/523

mv /var/www/swad/asg/IT/4/1/AC /var/www/swad/crs/524
mv /var/www/html/asg/IT/4/1/AC /var/www/html/crs/524

mv /var/www/swad/asg/IT/4/1/TH /var/www/swad/crs/525
mv /var/www/html/asg/IT/4/1/TH /var/www/html/crs/525

mv /var/www/swad/asg/IT/4/1/TDSA /var/www/swad/crs/526
mv /var/www/html/asg/IT/4/1/TDSA /var/www/html/crs/526

mv /var/www/swad/asg/IT/4/1/TDSB /var/www/swad/crs/527
mv /var/www/html/asg/IT/4/1/TDSB /var/www/html/crs/527

mv /var/www/swad/asg/IT/4/1/TDSP /var/www/swad/crs/528
mv /var/www/html/asg/IT/4/1/TDSP /var/www/html/crs/528

mv /var/www/swad/asg/IT/5/2/CO /var/www/swad/crs/529
mv /var/www/html/asg/IT/5/2/CO /var/www/html/crs/529

mv /var/www/swad/asg/IT/5/2/ICT /var/www/swad/crs/530
mv /var/www/html/asg/IT/5/2/ICT /var/www/html/crs/530

mv /var/www/swad/asg/IT/5/2/TT /var/www/swad/crs/531
mv /var/www/html/asg/IT/5/2/TT /var/www/html/crs/531

mv /var/www/swad/asg/ITIG/1/1/AED /var/www/swad/crs/532
mv /var/www/html/asg/ITIG/1/1/AED /var/www/html/crs/532

mv /var/www/swad/asg/ITIG/1/1/AM /var/www/swad/crs/533
mv /var/www/html/asg/ITIG/1/1/AM /var/www/html/crs/533

mv /var/www/swad/asg/ITIG/1/2/EB /var/www/swad/crs/534
mv /var/www/html/asg/ITIG/1/2/EB /var/www/html/crs/534

mv /var/www/swad/asg/ITIG/1/2/EDA /var/www/swad/crs/535
mv /var/www/html/asg/ITIG/1/2/EDA /var/www/html/crs/535

mv /var/www/swad/asg/ITIG/1/2/FLPA /var/www/swad/crs/536
mv /var/www/html/asg/ITIG/1/2/FLPA /var/www/html/crs/536

mv /var/www/swad/asg/ITIG/1/2/FLPB /var/www/swad/crs/537
mv /var/www/html/asg/ITIG/1/2/FLPB /var/www/html/crs/537

mv /var/www/swad/asg/ITIG/1/1/FTC /var/www/swad/crs/538
mv /var/www/html/asg/ITIG/1/1/FTC /var/www/html/crs/538

mv /var/www/swad/asg/ITIG/1/1/ICA /var/www/swad/crs/539
mv /var/www/html/asg/ITIG/1/1/ICA /var/www/html/crs/539

mv /var/www/swad/asg/ITIG/1/1/ICB /var/www/swad/crs/540
mv /var/www/html/asg/ITIG/1/1/ICB /var/www/html/crs/540

mv /var/www/swad/asg/ITIG/1/1/MDA /var/www/swad/crs/541
mv /var/www/html/asg/ITIG/1/1/MDA /var/www/html/crs/541

mv /var/www/swad/asg/ITIG/1/1/MDB /var/www/swad/crs/542
mv /var/www/html/asg/ITIG/1/1/MDB /var/www/html/crs/542

mv /var/www/swad/asg/ITIG/1/1/MP1 /var/www/swad/crs/543
mv /var/www/html/asg/ITIG/1/1/MP1 /var/www/html/crs/543

mv /var/www/swad/asg/ITIG/1/2/MP2 /var/www/swad/crs/544
mv /var/www/html/asg/ITIG/1/2/MP2 /var/www/html/crs/544

mv /var/www/swad/asg/ITIG/1/2/TCA /var/www/swad/crs/545
mv /var/www/html/asg/ITIG/1/2/TCA /var/www/html/crs/545

mv /var/www/swad/asg/ITIG/1/2/TCB /var/www/swad/crs/546
mv /var/www/html/asg/ITIG/1/2/TCB /var/www/html/crs/546

mv /var/www/swad/asg/ITIG/2/1/BD /var/www/swad/crs/547
mv /var/www/html/asg/ITIG/2/1/BD /var/www/html/crs/547

mv /var/www/swad/asg/ITIG/2/2/CN /var/www/swad/crs/548
mv /var/www/html/asg/ITIG/2/2/CN /var/www/html/crs/548

mv /var/www/swad/asg/ITIG/2/1/EC /var/www/swad/crs/549
mv /var/www/html/asg/ITIG/2/1/EC /var/www/html/crs/549

mv /var/www/swad/asg/ITIG/2/1/IS1 /var/www/swad/crs/550
mv /var/www/html/asg/ITIG/2/1/IS1 /var/www/html/crs/550

mv /var/www/swad/asg/ITIG/2/2/IS2 /var/www/swad/crs/551
mv /var/www/html/asg/ITIG/2/2/IS2 /var/www/html/crs/551

mv /var/www/swad/asg/ITIG/2/2/MC /var/www/swad/crs/552
mv /var/www/html/asg/ITIG/2/2/MC /var/www/html/crs/552

mv /var/www/swad/asg/ITIG/2/2/PBD /var/www/swad/crs/553
mv /var/www/html/asg/ITIG/2/2/PBD /var/www/html/crs/553

mv /var/www/swad/asg/ITIG/2/1/SO1 /var/www/swad/crs/554
mv /var/www/html/asg/ITIG/2/1/SO1 /var/www/html/crs/554

mv /var/www/swad/asg/ITIG/2/2/SO2 /var/www/swad/crs/555
mv /var/www/html/asg/ITIG/2/2/SO2 /var/www/html/crs/555

mv /var/www/swad/asg/ITIG/2/1/TA /var/www/swad/crs/556
mv /var/www/html/asg/ITIG/2/1/TA /var/www/html/crs/556

mv /var/www/swad/asg/ITIG/3/1/C /var/www/swad/crs/557
mv /var/www/html/asg/ITIG/3/1/C /var/www/html/crs/557

mv /var/www/swad/asg/ITIG/3/2/EE /var/www/swad/crs/558
mv /var/www/html/asg/ITIG/3/2/EE /var/www/html/crs/558

mv /var/www/swad/asg/ITIG/3/1/RC /var/www/swad/crs/559
mv /var/www/html/asg/ITIG/3/1/RC /var/www/html/crs/559

mv /var/www/swad/asg/ITIS/1/1/AED /var/www/swad/crs/560
mv /var/www/html/asg/ITIS/1/1/AED /var/www/html/crs/560

mv /var/www/swad/asg/ITIS/1/1/AEDB /var/www/swad/crs/561
mv /var/www/html/asg/ITIS/1/1/AEDB /var/www/html/crs/561

mv /var/www/swad/asg/ITIS/1/1/AMA /var/www/swad/crs/562
mv /var/www/html/asg/ITIS/1/1/AMA /var/www/html/crs/562

mv /var/www/swad/asg/ITIS/1/1/AM /var/www/swad/crs/563
mv /var/www/html/asg/ITIS/1/1/AM /var/www/html/crs/563

mv /var/www/swad/asg/ITIS/1/2/EB /var/www/swad/crs/564
mv /var/www/html/asg/ITIS/1/2/EB /var/www/html/crs/564

mv /var/www/swad/asg/ITIS/1/2/ED /var/www/swad/crs/565
mv /var/www/html/asg/ITIS/1/2/ED /var/www/html/crs/565

mv /var/www/swad/asg/ITIS/1/2/FLPB /var/www/swad/crs/566
mv /var/www/html/asg/ITIS/1/2/FLPB /var/www/html/crs/566

mv /var/www/swad/asg/ITIS/1/1/FFI /var/www/swad/crs/567
mv /var/www/html/asg/ITIS/1/1/FFI /var/www/html/crs/567

mv /var/www/swad/asg/ITIS/1/2/FTC /var/www/swad/crs/568
mv /var/www/html/asg/ITIS/1/2/FTC /var/www/html/crs/568

mv /var/www/swad/asg/ITIS/1/1/ICA /var/www/swad/crs/569
mv /var/www/html/asg/ITIS/1/1/ICA /var/www/html/crs/569

mv /var/www/swad/asg/ITIS/1/1/ICB /var/www/swad/crs/570
mv /var/www/html/asg/ITIS/1/1/ICB /var/www/html/crs/570

mv /var/www/swad/asg/ITIS/1/1/MD /var/www/swad/crs/571
mv /var/www/html/asg/ITIS/1/1/MD /var/www/html/crs/571

mv /var/www/swad/asg/ITIS/1/1/MP1 /var/www/swad/crs/572
mv /var/www/html/asg/ITIS/1/1/MP1 /var/www/html/crs/572

mv /var/www/swad/asg/ITIS/1/2/MP2A /var/www/swad/crs/573
mv /var/www/html/asg/ITIS/1/2/MP2A /var/www/html/crs/573

mv /var/www/swad/asg/ITIS/1/2/MP2B /var/www/swad/crs/574
mv /var/www/html/asg/ITIS/1/2/MP2B /var/www/html/crs/574

mv /var/www/swad/asg/ITIS/1/2/TCA /var/www/swad/crs/575
mv /var/www/html/asg/ITIS/1/2/TCA /var/www/html/crs/575

mv /var/www/swad/asg/ITIS/1/2/TCB /var/www/swad/crs/576
mv /var/www/html/asg/ITIS/1/2/TCB /var/www/html/crs/576

mv /var/www/swad/asg/ITIS/2/1/BD /var/www/swad/crs/577
mv /var/www/html/asg/ITIS/2/1/BD /var/www/html/crs/577

mv /var/www/swad/asg/ITIS/2/1/CN /var/www/swad/crs/578
mv /var/www/html/asg/ITIS/2/1/CN /var/www/html/crs/578

mv /var/www/swad/asg/ITIS/2/1/EC /var/www/swad/crs/579
mv /var/www/html/asg/ITIS/2/1/EC /var/www/html/crs/579

mv /var/www/swad/asg/ITIS/2/1/ECP /var/www/swad/crs/580
mv /var/www/html/asg/ITIS/2/1/ECP /var/www/html/crs/580

mv /var/www/swad/asg/ITIS/2/2/IS /var/www/swad/crs/581
mv /var/www/html/asg/ITIS/2/2/IS /var/www/html/crs/581

mv /var/www/swad/asg/ITIS/2/2/MC /var/www/swad/crs/582
mv /var/www/html/asg/ITIS/2/2/MC /var/www/html/crs/582

mv /var/www/swad/asg/ITIS/2/2/SBD /var/www/swad/crs/583
mv /var/www/html/asg/ITIS/2/2/SBD /var/www/html/crs/583

mv /var/www/swad/asg/ITIS/2/1/SO1 /var/www/swad/crs/584
mv /var/www/html/asg/ITIS/2/1/SO1 /var/www/html/crs/584

mv /var/www/swad/asg/ITIS/2/2/SO2 /var/www/swad/crs/585
mv /var/www/html/asg/ITIS/2/2/SO2 /var/www/html/crs/585

mv /var/www/swad/asg/ITIS/2/1/TA /var/www/swad/crs/586
mv /var/www/html/asg/ITIS/2/1/TA /var/www/html/crs/586

mv /var/www/swad/asg/ITIS/3/1/IDC /var/www/swad/crs/587
mv /var/www/html/asg/ITIS/3/1/IDC /var/www/html/crs/587

mv /var/www/swad/asg/ITIS/3/1/R /var/www/swad/crs/588
mv /var/www/html/asg/ITIS/3/1/R /var/www/html/crs/588

mv /var/www/swad/asg/ITIS/3/2/T /var/www/swad/crs/589
mv /var/www/html/asg/ITIS/3/2/T /var/www/html/crs/589

mv /var/www/swad/asg/ITIO/3/2/AIS /var/www/swad/crs/590
mv /var/www/html/asg/ITIO/3/2/AIS /var/www/html/crs/590

mv /var/www/swad/asg/ITIO/3/2/ASBM /var/www/swad/crs/591
mv /var/www/html/asg/ITIO/3/2/ASBM /var/www/html/crs/591

mv /var/www/swad/asg/ITIO/3/1/AI /var/www/swad/crs/592
mv /var/www/html/asg/ITIO/3/1/AI /var/www/html/crs/592

mv /var/www/swad/asg/ITIO/3/2/BDD /var/www/swad/crs/593
mv /var/www/html/asg/ITIO/3/2/BDD /var/www/html/crs/593

mv /var/www/swad/asg/ITIO/3/2/CFG /var/www/swad/crs/594
mv /var/www/html/asg/ITIO/3/2/CFG /var/www/html/crs/594

mv /var/www/swad/asg/ITIO/3/2/DSTR /var/www/swad/crs/595
mv /var/www/html/asg/ITIO/3/2/DSTR /var/www/html/crs/595

mv /var/www/swad/asg/ITIO/3/2/DSDO /var/www/swad/crs/596
mv /var/www/html/asg/ITIO/3/2/DSDO /var/www/html/crs/596

mv /var/www/swad/asg/ITIO/3/1/DIU /var/www/swad/crs/597
mv /var/www/html/asg/ITIO/3/1/DIU /var/www/html/crs/597

mv /var/www/swad/asg/ITIO/3/2/DEC /var/www/swad/crs/598
mv /var/www/html/asg/ITIO/3/2/DEC /var/www/html/crs/598

mv /var/www/swad/asg/ITIO/3/1/EDS /var/www/swad/crs/599
mv /var/www/html/asg/ITIO/3/1/EDS /var/www/html/crs/599

mv /var/www/swad/asg/ITIO/3/2/FIG /var/www/swad/crs/600
mv /var/www/html/asg/ITIO/3/2/FIG /var/www/html/crs/600

mv /var/www/swad/asg/ITIO/3/1/FFC /var/www/swad/crs/601
mv /var/www/html/asg/ITIO/3/1/FFC /var/www/html/crs/601

mv /var/www/swad/asg/ITIO/3/2/GE /var/www/swad/crs/602
mv /var/www/html/asg/ITIO/3/2/GE /var/www/html/crs/602

mv /var/www/swad/asg/ITIO/3/1/GSI /var/www/swad/crs/603
mv /var/www/html/asg/ITIO/3/1/GSI /var/www/html/crs/603

mv /var/www/swad/asg/ITIO/3/1/ID /var/www/swad/crs/604
mv /var/www/html/asg/ITIO/3/1/ID /var/www/html/crs/604

mv /var/www/swad/asg/ITIO/3/2/II /var/www/swad/crs/605
mv /var/www/html/asg/ITIO/3/2/II /var/www/html/crs/605

mv /var/www/swad/asg/ITIO/3/1/LTDR /var/www/swad/crs/606
mv /var/www/html/asg/ITIO/3/1/LTDR /var/www/html/crs/606

mv /var/www/swad/asg/ITIO/3/2/LTDR /var/www/swad/crs/607
mv /var/www/html/asg/ITIO/3/2/LTDR /var/www/html/crs/607

mv /var/www/swad/asg/ITIO/3/1/LP /var/www/swad/crs/608
mv /var/www/html/asg/ITIO/3/1/LP /var/www/html/crs/608

mv /var/www/swad/asg/ITIO/3/2/MEI /var/www/swad/crs/609
mv /var/www/html/asg/ITIO/3/2/MEI /var/www/html/crs/609

mv /var/www/swad/asg/ITIO/3/1/MIA /var/www/swad/crs/610
mv /var/www/html/asg/ITIO/3/1/MIA /var/www/html/crs/610

mv /var/www/swad/asg/ITIO/3/1/NTP /var/www/swad/crs/611
mv /var/www/html/asg/ITIO/3/1/NTP /var/www/html/crs/611

mv /var/www/swad/asg/ITIO/3/1/O /var/www/swad/crs/612
mv /var/www/html/asg/ITIO/3/1/O /var/www/html/crs/612

mv /var/www/swad/asg/ITIO/3/2/P /var/www/swad/crs/613
mv /var/www/html/asg/ITIO/3/2/P /var/www/html/crs/613

mv /var/www/swad/asg/ITIO/3/1/PDS /var/www/swad/crs/614
mv /var/www/html/asg/ITIO/3/1/PDS /var/www/html/crs/614

mv /var/www/swad/asg/ITIO/3/1/PC /var/www/swad/crs/615
mv /var/www/html/asg/ITIO/3/1/PC /var/www/html/crs/615

mv /var/www/swad/asg/ITIO/3/2/PBD /var/www/swad/crs/616
mv /var/www/html/asg/ITIO/3/2/PBD /var/www/html/crs/616

mv /var/www/swad/asg/ITIO/3/1/SPSI /var/www/swad/crs/617
mv /var/www/html/asg/ITIO/3/1/SPSI /var/www/html/crs/617

mv /var/www/swad/asg/ITIO/3/2/SAD /var/www/swad/crs/618
mv /var/www/html/asg/ITIO/3/2/SAD /var/www/html/crs/618

mv /var/www/swad/asg/ITIO/3/2/SBD /var/www/swad/crs/619
mv /var/www/html/asg/ITIO/3/2/SBD /var/www/html/crs/619

mv /var/www/swad/asg/ITIO/3/2/SIE /var/www/swad/crs/620
mv /var/www/html/asg/ITIO/3/2/SIE /var/www/html/crs/620

mv /var/www/swad/asg/ITIO/3/2/GIS /var/www/swad/crs/621
mv /var/www/html/asg/ITIO/3/2/GIS /var/www/html/crs/621

mv /var/www/swad/asg/ITIO/3/1/SID /var/www/swad/crs/622
mv /var/www/html/asg/ITIO/3/1/SID /var/www/html/crs/622

mv /var/www/swad/asg/ITIO/3/2/SIGes /var/www/swad/crs/623
mv /var/www/html/asg/ITIO/3/2/SIGes /var/www/html/crs/623

mv /var/www/swad/asg/ITIO/3/1/SM /var/www/swad/crs/624
mv /var/www/html/asg/ITIO/3/1/SM /var/www/html/crs/624

mv /var/www/swad/asg/ITIO/3/1/TMS /var/www/swad/crs/625
mv /var/www/html/asg/ITIO/3/1/TMS /var/www/html/crs/625

mv /var/www/swad/asg/ITIO/3/1/TCC /var/www/swad/crs/626
mv /var/www/html/asg/ITIO/3/1/TCC /var/www/html/crs/626

mv /var/www/swad/asg/ITIO/3/1/T /var/www/swad/crs/627
mv /var/www/html/asg/ITIO/3/1/T /var/www/html/crs/627

mv /var/www/swad/asg/ITIGC/1/1/AED /var/www/swad/crs/628
mv /var/www/html/asg/ITIGC/1/1/AED /var/www/html/crs/628

mv /var/www/swad/asg/ITIGC/1/1/AM /var/www/swad/crs/629
mv /var/www/html/asg/ITIGC/1/1/AM /var/www/html/crs/629

mv /var/www/swad/asg/ITIGC/1/2/E /var/www/swad/crs/630
mv /var/www/html/asg/ITIGC/1/2/E /var/www/html/crs/630

mv /var/www/swad/asg/ITIGC/1/2/FLP /var/www/swad/crs/631
mv /var/www/html/asg/ITIGC/1/2/FLP /var/www/html/crs/631

mv /var/www/swad/asg/ITIGC/1/1/FTC /var/www/swad/crs/632
mv /var/www/html/asg/ITIGC/1/1/FTC /var/www/html/crs/632

mv /var/www/swad/asg/ITIGC/1/1/IC /var/www/swad/crs/633
mv /var/www/html/asg/ITIGC/1/1/IC /var/www/html/crs/633

mv /var/www/swad/asg/ITIGC/1/2/MD /var/www/swad/crs/634
mv /var/www/html/asg/ITIGC/1/2/MD /var/www/html/crs/634

mv /var/www/swad/asg/ITIGC/1/1/MP1 /var/www/swad/crs/635
mv /var/www/html/asg/ITIGC/1/1/MP1 /var/www/html/crs/635

mv /var/www/swad/asg/ITIGC/1/2/MP2 /var/www/swad/crs/636
mv /var/www/html/asg/ITIGC/1/2/MP2 /var/www/html/crs/636

mv /var/www/swad/asg/ITIGC/1/2/TC /var/www/swad/crs/637
mv /var/www/html/asg/ITIGC/1/2/TC /var/www/html/crs/637

mv /var/www/swad/asg/ITIGC/2/1/BD /var/www/swad/crs/638
mv /var/www/html/asg/ITIGC/2/1/BD /var/www/html/crs/638

mv /var/www/swad/asg/ITIGC/2/2/CN /var/www/swad/crs/639
mv /var/www/html/asg/ITIGC/2/2/CN /var/www/html/crs/639

mv /var/www/swad/asg/ITIGC/2/1/ED /var/www/swad/crs/640
mv /var/www/html/asg/ITIGC/2/1/ED /var/www/html/crs/640

mv /var/www/swad/asg/ITIGC/2/1/EC /var/www/swad/crs/641
mv /var/www/html/asg/ITIGC/2/1/EC /var/www/html/crs/641

mv /var/www/swad/asg/ITIGC/2/1/IS1 /var/www/swad/crs/642
mv /var/www/html/asg/ITIGC/2/1/IS1 /var/www/html/crs/642

mv /var/www/swad/asg/ITIGC/2/2/IS2 /var/www/swad/crs/643
mv /var/www/html/asg/ITIGC/2/2/IS2 /var/www/html/crs/643

mv /var/www/swad/asg/ITIGC/2/2/MC /var/www/swad/crs/644
mv /var/www/html/asg/ITIGC/2/2/MC /var/www/html/crs/644

mv /var/www/swad/asg/ITIGC/2/2/PBD /var/www/swad/crs/645
mv /var/www/html/asg/ITIGC/2/2/PBD /var/www/html/crs/645

mv /var/www/swad/asg/ITIGC/2/1/SO1 /var/www/swad/crs/646
mv /var/www/html/asg/ITIGC/2/1/SO1 /var/www/html/crs/646

mv /var/www/swad/asg/ITIGC/2/2/SO2 /var/www/swad/crs/647
mv /var/www/html/asg/ITIGC/2/2/SO2 /var/www/html/crs/647

mv /var/www/swad/asg/ITIGC/2/1/TA /var/www/swad/crs/648
mv /var/www/html/asg/ITIGC/2/1/TA /var/www/html/crs/648

mv /var/www/swad/asg/ITIGC/3/1/C /var/www/swad/crs/649
mv /var/www/html/asg/ITIGC/3/1/C /var/www/html/crs/649

mv /var/www/swad/asg/ITIGC/3/2/EE /var/www/swad/crs/650
mv /var/www/html/asg/ITIGC/3/2/EE /var/www/html/crs/650

mv /var/www/swad/asg/ITIGC/3/2/DIU /var/www/swad/crs/651
mv /var/www/html/asg/ITIGC/3/2/DIU /var/www/html/crs/651

mv /var/www/swad/asg/ITIGC/3/1/P /var/www/swad/crs/652
mv /var/www/html/asg/ITIGC/3/1/P /var/www/html/crs/652

mv /var/www/swad/asg/ITIGC/3/1/RC /var/www/swad/crs/653
mv /var/www/html/asg/ITIGC/3/1/RC /var/www/html/crs/653

mv /var/www/swad/asg/ITIGC/3/2/TDR /var/www/swad/crs/654
mv /var/www/html/asg/ITIGC/3/2/TDR /var/www/html/crs/654

mv /var/www/swad/asg/LADE/0/2/AMAA /var/www/swad/crs/655
mv /var/www/html/asg/LADE/0/2/AMAA /var/www/html/crs/655

mv /var/www/swad/asg/LADE/0/2/AC /var/www/swad/crs/656
mv /var/www/html/asg/LADE/0/2/AC /var/www/html/crs/656

mv /var/www/swad/asg/LADE/0/1/CE /var/www/swad/crs/657
mv /var/www/html/asg/LADE/0/1/CE /var/www/html/crs/657

mv /var/www/swad/asg/LADE/0/2/CDC /var/www/swad/crs/658
mv /var/www/html/asg/LADE/0/2/CDC /var/www/html/crs/658

mv /var/www/swad/asg/LADE/0/2/CC /var/www/swad/crs/659
mv /var/www/html/asg/LADE/0/2/CC /var/www/html/crs/659

mv /var/www/swad/asg/LADE/0/1/CG /var/www/swad/crs/660
mv /var/www/html/asg/LADE/0/1/CG /var/www/html/crs/660

mv /var/www/swad/asg/LADE/0/1/CSEE /var/www/swad/crs/661
mv /var/www/html/asg/LADE/0/1/CSEE /var/www/html/crs/661

mv /var/www/swad/asg/LADE/0/1/CP /var/www/swad/crs/662
mv /var/www/html/asg/LADE/0/1/CP /var/www/html/crs/662

mv /var/www/swad/asg/LADE/0/1/CMTV /var/www/swad/crs/663
mv /var/www/html/asg/LADE/0/1/CMTV /var/www/html/crs/663

mv /var/www/swad/asg/LADE/0/2/CG /var/www/swad/crs/664
mv /var/www/html/asg/LADE/0/2/CG /var/www/html/crs/664

mv /var/www/swad/asg/LADE/0/1/CEC /var/www/swad/crs/665
mv /var/www/html/asg/LADE/0/1/CEC /var/www/html/crs/665

mv /var/www/swad/asg/LADE/0/2/DSS /var/www/swad/crs/666
mv /var/www/html/asg/LADE/0/2/DSS /var/www/html/crs/666

mv /var/www/swad/asg/LADE/0/1/DO1 /var/www/swad/crs/667
mv /var/www/html/asg/LADE/0/1/DO1 /var/www/html/crs/667

mv /var/www/swad/asg/LADE/0/1/DO2 /var/www/swad/crs/668
mv /var/www/html/asg/LADE/0/1/DO2 /var/www/html/crs/668

mv /var/www/swad/asg/LADE/0/2/DRH /var/www/swad/crs/669
mv /var/www/html/asg/LADE/0/2/DRH /var/www/html/crs/669

mv /var/www/swad/asg/LADE/0/1/DCB /var/www/swad/crs/670
mv /var/www/html/asg/LADE/0/1/DCB /var/www/html/crs/670

mv /var/www/swad/asg/LADE/0/2/FEJ /var/www/swad/crs/671
mv /var/www/html/asg/LADE/0/2/FEJ /var/www/html/crs/671

mv /var/www/swad/asg/LADE/0/1/FE /var/www/swad/crs/672
mv /var/www/html/asg/LADE/0/1/FE /var/www/html/crs/672

mv /var/www/swad/asg/LADE/0/1/GCE /var/www/swad/crs/673
mv /var/www/html/asg/LADE/0/1/GCE /var/www/html/crs/673

mv /var/www/swad/asg/LADE/0/2/GRI /var/www/swad/crs/674
mv /var/www/html/asg/LADE/0/2/GRI /var/www/html/crs/674

mv /var/www/swad/asg/LADE/0/1/HE /var/www/swad/crs/675
mv /var/www/html/asg/LADE/0/1/HE /var/www/html/crs/675

mv /var/www/swad/asg/LADE/0/1/IC1 /var/www/swad/crs/676
mv /var/www/html/asg/LADE/0/1/IC1 /var/www/html/crs/676

mv /var/www/swad/asg/LADE/0/2/ICCD /var/www/swad/crs/677
mv /var/www/html/asg/LADE/0/2/ICCD /var/www/html/crs/677

mv /var/www/swad/asg/LADE/0/2/IDPFA /var/www/swad/crs/678
mv /var/www/html/asg/LADE/0/2/IDPFA /var/www/html/crs/678

mv /var/www/swad/asg/LADE/0/2/II /var/www/swad/crs/679
mv /var/www/html/asg/LADE/0/2/II /var/www/html/crs/679

mv /var/www/swad/asg/LADE/0/1/ISA /var/www/swad/crs/680
mv /var/www/html/asg/LADE/0/1/ISA /var/www/html/crs/680

mv /var/www/swad/asg/LADE/0/1/ISB /var/www/swad/crs/681
mv /var/www/html/asg/LADE/0/1/ISB /var/www/html/crs/681

mv /var/www/swad/asg/LADE/0/2/IMF /var/www/swad/crs/682
mv /var/www/html/asg/LADE/0/2/IMF /var/www/html/crs/682

mv /var/www/swad/asg/LADE/0/2/IM /var/www/swad/crs/683
mv /var/www/html/asg/LADE/0/2/IM /var/www/html/crs/683

mv /var/www/swad/asg/LADE/0/1/IO /var/www/swad/crs/684
mv /var/www/html/asg/LADE/0/1/IO /var/www/html/crs/684

mv /var/www/swad/asg/LADE/0/2/MCABC /var/www/swad/crs/685
mv /var/www/html/asg/LADE/0/2/MCABC /var/www/html/crs/685

mv /var/www/swad/asg/LADE/0/2/NCI /var/www/swad/crs/686
mv /var/www/html/asg/LADE/0/2/NCI /var/www/html/crs/686

mv /var/www/swad/asg/LADE/0/2/O /var/www/swad/crs/687
mv /var/www/html/asg/LADE/0/2/O /var/www/html/crs/687

mv /var/www/swad/asg/LADE/0/2/OE /var/www/swad/crs/688
mv /var/www/html/asg/LADE/0/2/OE /var/www/html/crs/688

mv /var/www/swad/asg/LADE/0/2/PC /var/www/swad/crs/689
mv /var/www/html/asg/LADE/0/2/PC /var/www/html/crs/689

mv /var/www/swad/asg/LADE/0/1/PP /var/www/swad/crs/690
mv /var/www/html/asg/LADE/0/1/PP /var/www/html/crs/690

mv /var/www/swad/asg/LADE/0/2/RL /var/www/swad/crs/691
mv /var/www/html/asg/LADE/0/2/RL /var/www/html/crs/691

mv /var/www/swad/asg/LADE/0/2/SF /var/www/swad/crs/692
mv /var/www/html/asg/LADE/0/2/SF /var/www/html/crs/692

mv /var/www/swad/asg/LADE/0/2/TAM /var/www/swad/crs/693
mv /var/www/html/asg/LADE/0/2/TAM /var/www/html/crs/693

mv /var/www/swad/asg/LADE/0/2/TGBI /var/www/swad/crs/694
mv /var/www/html/asg/LADE/0/2/TGBI /var/www/html/crs/694

mv /var/www/swad/asg/LADE/1/2/CF /var/www/swad/crs/695
mv /var/www/html/asg/LADE/1/2/CF /var/www/html/crs/695

mv /var/www/swad/asg/LADE/1/1/DCB /var/www/swad/crs/696
mv /var/www/html/asg/LADE/1/1/DCB /var/www/html/crs/696

mv /var/www/swad/asg/LADE/1/1/DCCDE /var/www/swad/crs/697
mv /var/www/html/asg/LADE/1/1/DCCDE /var/www/html/crs/697

mv /var/www/swad/asg/LADE/1/1/MI1 /var/www/swad/crs/698
mv /var/www/html/asg/LADE/1/1/MI1 /var/www/html/crs/698

mv /var/www/swad/asg/LADE/2/1/DTSSB /var/www/swad/crs/699
mv /var/www/html/asg/LADE/2/1/DTSSB /var/www/html/crs/699

mv /var/www/swad/asg/LADER/1/0/DCO1B /var/www/swad/crs/700
mv /var/www/html/asg/LADER/1/0/DCO1B /var/www/html/crs/700

mv /var/www/swad/asg/LADER/1/2/IDPB /var/www/swad/crs/701
mv /var/www/html/asg/LADER/1/2/IDPB /var/www/html/crs/701

mv /var/www/swad/asg/LADER/1/2/MA1AB /var/www/swad/crs/702
mv /var/www/html/asg/LADER/1/2/MA1AB /var/www/html/crs/702

mv /var/www/swad/asg/LADER/1/2/TCE1 /var/www/swad/crs/703
mv /var/www/html/asg/LADER/1/2/TCE1 /var/www/html/crs/703

mv /var/www/swad/asg/LADER/2/0/DC2 /var/www/swad/crs/704
mv /var/www/html/asg/LADER/2/0/DC2 /var/www/html/crs/704

mv /var/www/swad/asg/LADER/2/1/DCO2B /var/www/swad/crs/705
mv /var/www/html/asg/LADER/2/1/DCO2B /var/www/html/crs/705

mv /var/www/swad/asg/LADER/5/0/DFT1A /var/www/swad/crs/706
mv /var/www/html/asg/LADER/5/0/DFT1A /var/www/html/crs/706

mv /var/www/swad/asg/LADER/5/0/DM1 /var/www/swad/crs/707
mv /var/www/html/asg/LADER/5/0/DM1 /var/www/html/crs/707

mv /var/www/swad/asg/LADER/6/0/DF2A /var/www/swad/crs/708
mv /var/www/html/asg/LADER/6/0/DF2A /var/www/html/crs/708

mv /var/www/swad/asg/LADER/6/0/DIPA /var/www/swad/crs/709
mv /var/www/html/asg/LADER/6/0/DIPA /var/www/html/crs/709

mv /var/www/swad/asg/LADER/6/1/DM2 /var/www/swad/crs/710
mv /var/www/html/asg/LADER/6/1/DM2 /var/www/html/crs/710

mv /var/www/swad/asg/LAS/0/0/CM /var/www/swad/crs/711
mv /var/www/html/asg/LAS/0/0/CM /var/www/html/crs/711

mv /var/www/swad/asg/LAS/0/2/HSC /var/www/swad/crs/712
mv /var/www/html/asg/LAS/0/2/HSC /var/www/html/crs/712

mv /var/www/swad/asg/LAS/4/2/AA /var/www/swad/crs/713
mv /var/www/html/asg/LAS/4/2/AA /var/www/html/crs/713

mv /var/www/swad/asg/LAS/4/2/AE /var/www/swad/crs/714
mv /var/www/html/asg/LAS/4/2/AE /var/www/html/crs/714

mv /var/www/swad/asg/LAS/4/1/AR /var/www/swad/crs/715
mv /var/www/html/asg/LAS/4/1/AR /var/www/html/crs/715

mv /var/www/swad/asg/LAS/4/2/AS /var/www/swad/crs/716
mv /var/www/html/asg/LAS/4/2/AS /var/www/html/crs/716

mv /var/www/swad/asg/LAS/4/1/AD /var/www/swad/crs/717
mv /var/www/html/asg/LAS/4/1/AD /var/www/html/crs/717

mv /var/www/swad/asg/LAS/4/2/AG /var/www/swad/crs/718
mv /var/www/html/asg/LAS/4/2/AG /var/www/html/crs/718

mv /var/www/swad/asg/LAS/4/2/AP /var/www/swad/crs/719
mv /var/www/html/asg/LAS/4/2/AP /var/www/html/crs/719

mv /var/www/swad/asg/LAS/4/1/ABS /var/www/swad/crs/720
mv /var/www/html/asg/LAS/4/1/ABS /var/www/html/crs/720

mv /var/www/swad/asg/LAS/4/1/EACSB /var/www/swad/crs/721
mv /var/www/html/asg/LAS/4/1/EACSB /var/www/html/crs/721

mv /var/www/swad/asg/LAS/4/2/ER /var/www/swad/crs/722
mv /var/www/html/asg/LAS/4/2/ER /var/www/html/crs/722

mv /var/www/swad/asg/LAS/4/1/FSH /var/www/swad/crs/723
mv /var/www/html/asg/LAS/4/1/FSH /var/www/html/crs/723

mv /var/www/swad/asg/LAS/4/1/FAS1 /var/www/swad/crs/724
mv /var/www/html/asg/LAS/4/1/FAS1 /var/www/html/crs/724

mv /var/www/swad/asg/LAS/4/2/FAS2 /var/www/swad/crs/725
mv /var/www/html/asg/LAS/4/2/FAS2 /var/www/html/crs/725

mv /var/www/swad/asg/LAS/4/1/GHD /var/www/swad/crs/726
mv /var/www/html/asg/LAS/4/1/GHD /var/www/html/crs/726

mv /var/www/swad/asg/LAS/4/2/HEC /var/www/swad/crs/727
mv /var/www/html/asg/LAS/4/2/HEC /var/www/html/crs/727

mv /var/www/swad/asg/LAS/4/2/HSC /var/www/swad/crs/728
mv /var/www/html/asg/LAS/4/2/HSC /var/www/html/crs/728

mv /var/www/swad/asg/LAS/4/0/MTIE /var/www/swad/crs/729
mv /var/www/html/asg/LAS/4/0/MTIE /var/www/html/crs/729

mv /var/www/swad/asg/LAS/4/0/MTIE2 /var/www/swad/crs/730
mv /var/www/html/asg/LAS/4/0/MTIE2 /var/www/html/crs/730

mv /var/www/swad/asg/LAS/4/1/NE /var/www/swad/crs/731
mv /var/www/html/asg/LAS/4/1/NE /var/www/html/crs/731

mv /var/www/swad/asg/LAS/5/1/AE /var/www/swad/crs/732
mv /var/www/html/asg/LAS/5/1/AE /var/www/html/crs/732

mv /var/www/swad/asg/LAS/5/1/ACS /var/www/swad/crs/733
mv /var/www/html/asg/LAS/5/1/ACS /var/www/html/crs/733

mv /var/www/swad/asg/LAS/5/2/AP /var/www/swad/crs/734
mv /var/www/html/asg/LAS/5/2/AP /var/www/html/crs/734

mv /var/www/swad/asg/LAS/5/1/ECSE /var/www/swad/crs/735
mv /var/www/html/asg/LAS/5/1/ECSE /var/www/html/crs/735

mv /var/www/swad/asg/LAS/5/2/TCA /var/www/swad/crs/736
mv /var/www/html/asg/LAS/5/2/TCA /var/www/html/crs/736

mv /var/www/swad/asg/LAS/5/2/TCAC /var/www/swad/crs/737
mv /var/www/html/asg/LAS/5/2/TCAC /var/www/html/crs/737

mv /var/www/swad/asg/LBA/0/0/AA /var/www/swad/crs/738
mv /var/www/html/asg/LBA/0/0/AA /var/www/html/crs/738

mv /var/www/swad/asg/LBA/0/2/ATEOA /var/www/swad/crs/739
mv /var/www/html/asg/LBA/0/2/ATEOA /var/www/html/crs/739

mv /var/www/swad/asg/LBA/0/1/CRR /var/www/swad/crs/740
mv /var/www/html/asg/LBA/0/1/CRR /var/www/html/crs/740

mv /var/www/swad/asg/LBA/0/1/EC /var/www/swad/crs/741
mv /var/www/html/asg/LBA/0/1/EC /var/www/html/crs/741

mv /var/www/swad/asg/LBA/0/1/NQMR /var/www/swad/crs/742
mv /var/www/html/asg/LBA/0/1/NQMR /var/www/html/crs/742

mv /var/www/swad/asg/LBA/0/1/PABC /var/www/swad/crs/743
mv /var/www/html/asg/LBA/0/1/PABC /var/www/html/crs/743

mv /var/www/swad/asg/LBA/0/2/SCAD /var/www/swad/crs/744
mv /var/www/html/asg/LBA/0/2/SCAD /var/www/html/crs/744

mv /var/www/swad/asg/LBA/1/0/D1A /var/www/swad/crs/745
mv /var/www/html/asg/LBA/1/0/D1A /var/www/html/crs/745

mv /var/www/swad/asg/LBA/1/0/D1B /var/www/swad/crs/746
mv /var/www/html/asg/LBA/1/0/D1B /var/www/html/crs/746

mv /var/www/swad/asg/LBA/1/0/D1C /var/www/swad/crs/747
mv /var/www/html/asg/LBA/1/0/D1C /var/www/html/crs/747

mv /var/www/swad/asg/LBA/1/0/THGAAB /var/www/swad/crs/748
mv /var/www/html/asg/LBA/1/0/THGAAB /var/www/html/crs/748

mv /var/www/swad/asg/LBA/2/0/ALA /var/www/swad/crs/749
mv /var/www/html/asg/LBA/2/0/ALA /var/www/html/crs/749

mv /var/www/swad/asg/LBA/2/0/D2D /var/www/swad/crs/750
mv /var/www/html/asg/LBA/2/0/D2D /var/www/html/crs/750

mv /var/www/swad/asg/LBA/2/0/HACC /var/www/swad/crs/751
mv /var/www/html/asg/LBA/2/0/HACC /var/www/html/crs/751

mv /var/www/swad/asg/LBA/3/0/MLPCAC /var/www/swad/crs/752
mv /var/www/html/asg/LBA/3/0/MLPCAC /var/www/html/crs/752

mv /var/www/swad/asg/LBA/3/0/MLTCDC /var/www/swad/crs/753
mv /var/www/html/asg/LBA/3/0/MLTCDC /var/www/html/crs/753

mv /var/www/swad/asg/LBA/3/0/MLTOEB /var/www/swad/crs/754
mv /var/www/html/asg/LBA/3/0/MLTOEB /var/www/html/crs/754

mv /var/www/swad/asg/LBA/4/1/ADAM /var/www/swad/crs/755
mv /var/www/html/asg/LBA/4/1/ADAM /var/www/html/crs/755

mv /var/www/swad/asg/LBA/4/2/DEEA /var/www/swad/crs/756
mv /var/www/html/asg/LBA/4/2/DEEA /var/www/html/crs/756

mv /var/www/swad/asg/LBA/4/2/DEEB /var/www/swad/crs/757
mv /var/www/html/asg/LBA/4/2/DEEB /var/www/html/crs/757

mv /var/www/swad/asg/LBA/4/2/DEEC /var/www/swad/crs/758
mv /var/www/html/asg/LBA/4/2/DEEC /var/www/html/crs/758

mv /var/www/swad/asg/LBA/4/0/IC1D /var/www/swad/crs/759
mv /var/www/html/asg/LBA/4/0/IC1D /var/www/html/crs/759

mv /var/www/swad/asg/LB/0/2/HCV /var/www/swad/crs/760
mv /var/www/html/asg/LB/0/2/HCV /var/www/html/crs/760

mv /var/www/swad/asg/LB/1/2/B /var/www/swad/crs/761
mv /var/www/html/asg/LB/1/2/B /var/www/html/crs/761

mv /var/www/swad/asg/LB/1/2/FPB /var/www/swad/crs/762
mv /var/www/html/asg/LB/1/2/FPB /var/www/html/crs/762

mv /var/www/swad/asg/LB/1/0/PG /var/www/swad/crs/763
mv /var/www/html/asg/LB/1/0/PG /var/www/html/crs/763

mv /var/www/swad/asg/LB/1/1/QB /var/www/swad/crs/764
mv /var/www/html/asg/LB/1/1/QB /var/www/html/crs/764

mv /var/www/swad/asg/LB/1/0/ZGA /var/www/swad/crs/765
mv /var/www/html/asg/LB/1/0/ZGA /var/www/html/crs/765

mv /var/www/swad/asg/LB/2/0/CHVA /var/www/swad/crs/766
mv /var/www/html/asg/LB/2/0/CHVA /var/www/html/crs/766

mv /var/www/swad/asg/LB/3/0/FA /var/www/swad/crs/767
mv /var/www/html/asg/LB/3/0/FA /var/www/html/crs/767

mv /var/www/swad/asg/LB/3/0/FAC /var/www/swad/crs/768
mv /var/www/html/asg/LB/3/0/FAC /var/www/html/crs/768

mv /var/www/swad/asg/LB/3/0/FAD1 /var/www/swad/crs/769
mv /var/www/html/asg/LB/3/0/FAD1 /var/www/html/crs/769

mv /var/www/swad/asg/LB/3/0/FAD /var/www/swad/crs/770
mv /var/www/html/asg/LB/3/0/FAD /var/www/html/crs/770

mv /var/www/swad/asg/LB/3/1/M /var/www/swad/crs/771
mv /var/www/html/asg/LB/3/1/M /var/www/html/crs/771

mv /var/www/swad/asg/LB/3/0/OMA /var/www/swad/crs/772
mv /var/www/html/asg/LB/3/0/OMA /var/www/html/crs/772

mv /var/www/swad/asg/LB/3/2/ZM /var/www/swad/crs/773
mv /var/www/html/asg/LB/3/2/ZM /var/www/html/crs/773

mv /var/www/swad/asg/LB/4/0/FAA1 /var/www/swad/crs/774
mv /var/www/html/asg/LB/4/0/FAA1 /var/www/html/crs/774

mv /var/www/swad/asg/LB/4/0/FAA /var/www/swad/crs/775
mv /var/www/html/asg/LB/4/0/FAA /var/www/html/crs/775

mv /var/www/swad/asg/LB/4/2/FAA /var/www/swad/crs/776
mv /var/www/html/asg/LB/4/2/FAA /var/www/html/crs/776

mv /var/www/swad/asg/LB/4/2/BCA /var/www/swad/crs/777
mv /var/www/html/asg/LB/4/2/BCA /var/www/html/crs/777

mv /var/www/swad/asg/LB/5/0/FBA4I /var/www/swad/crs/778
mv /var/www/html/asg/LB/5/0/FBA4I /var/www/html/crs/778

mv /var/www/swad/asg/LB/5/1/FBA4IC /var/www/swad/crs/779
mv /var/www/html/asg/LB/5/1/FBA4IC /var/www/html/crs/779

mv /var/www/swad/asg/LB/5/0/G /var/www/swad/crs/780
mv /var/www/html/asg/LB/5/0/G /var/www/html/crs/780

mv /var/www/swad/asg/LBQ/1/1/BC /var/www/swad/crs/781
mv /var/www/html/asg/LBQ/1/1/BC /var/www/html/crs/781

mv /var/www/swad/asg/LBQ/1/1/EMR /var/www/swad/crs/782
mv /var/www/html/asg/LBQ/1/1/EMR /var/www/html/crs/782

mv /var/www/swad/asg/LBQ/1/2/QB /var/www/swad/crs/783
mv /var/www/html/asg/LBQ/1/2/QB /var/www/html/crs/783

mv /var/www/swad/asg/LBQ/2/1/I /var/www/swad/crs/784
mv /var/www/html/asg/LBQ/2/1/I /var/www/html/crs/784

mv /var/www/swad/asg/LBQ/2/2/IC /var/www/swad/crs/785
mv /var/www/html/asg/LBQ/2/2/IC /var/www/html/crs/785

mv /var/www/swad/asg/LBQ/2/0/MEB2 /var/www/swad/crs/786
mv /var/www/html/asg/LBQ/2/0/MEB2 /var/www/html/crs/786

mv /var/www/swad/asg/LCTA/0/1/MCF /var/www/swad/crs/787
mv /var/www/html/asg/LCTA/0/1/MCF /var/www/html/crs/787

mv /var/www/swad/asg/LCTA/1/1/AC /var/www/swad/crs/788
mv /var/www/html/asg/LCTA/1/1/AC /var/www/html/crs/788

mv /var/www/swad/asg/LCTA/1/1/BMAAC /var/www/swad/crs/789
mv /var/www/html/asg/LCTA/1/1/BMAAC /var/www/html/crs/789

mv /var/www/swad/asg/LCTA/1/0/BR /var/www/swad/crs/790
mv /var/www/html/asg/LCTA/1/0/BR /var/www/html/crs/790

mv /var/www/swad/asg/LCTA/1/2/PA /var/www/swad/crs/791
mv /var/www/html/asg/LCTA/1/2/PA /var/www/html/crs/791

mv /var/www/swad/asg/LCTA/1/0/QBA /var/www/swad/crs/792
mv /var/www/html/asg/LCTA/1/0/QBA /var/www/html/crs/792

mv /var/www/swad/asg/LCTA/2/2/CGLA /var/www/swad/crs/793
mv /var/www/html/asg/LCTA/2/2/CGLA /var/www/html/crs/793

mv /var/www/swad/asg/LCTA/2/0/DN /var/www/swad/crs/794
mv /var/www/html/asg/LCTA/2/0/DN /var/www/html/crs/794

mv /var/www/swad/asg/LCTA/2/2/DPIPA /var/www/swad/crs/795
mv /var/www/html/asg/LCTA/2/2/DPIPA /var/www/html/crs/795

mv /var/www/swad/asg/LCTA/2/1/MI /var/www/swad/crs/796
mv /var/www/html/asg/LCTA/2/1/MI /var/www/html/crs/796

mv /var/www/swad/asg/LCAFD/0/2/FEC /var/www/swad/crs/797
mv /var/www/html/asg/LCAFD/0/2/FEC /var/www/html/crs/797

mv /var/www/swad/asg/LCAFD/0/0/NT /var/www/swad/crs/798
mv /var/www/html/asg/LCAFD/0/0/NT /var/www/html/crs/798

mv /var/www/swad/asg/LCAFD/0/2/NAFD /var/www/swad/crs/799
mv /var/www/html/asg/LCAFD/0/2/NAFD /var/www/html/crs/799

mv /var/www/swad/asg/LCAFD/1/1/AF /var/www/swad/crs/800
mv /var/www/html/asg/LCAFD/1/1/AF /var/www/html/crs/800

mv /var/www/swad/asg/LCAFD/1/0/EC /var/www/swad/crs/801
mv /var/www/html/asg/LCAFD/1/0/EC /var/www/html/crs/801

mv /var/www/swad/asg/LCAFD/1/2/FA /var/www/swad/crs/802
mv /var/www/html/asg/LCAFD/1/2/FA /var/www/html/crs/802

mv /var/www/swad/asg/LCAFD/1/1/FDI /var/www/swad/crs/803
mv /var/www/html/asg/LCAFD/1/1/FDI /var/www/html/crs/803

mv /var/www/swad/asg/LCAFD/1/0/FEN /var/www/swad/crs/804
mv /var/www/html/asg/LCAFD/1/0/FEN /var/www/html/crs/804

mv /var/www/swad/asg/LCAFD/1/2/FEN2 /var/www/swad/crs/805
mv /var/www/html/asg/LCAFD/1/2/FEN2 /var/www/html/crs/805

mv /var/www/swad/asg/LCAFD/1/0/FHR /var/www/swad/crs/806
mv /var/www/html/asg/LCAFD/1/0/FHR /var/www/html/crs/806

mv /var/www/swad/asg/LCAFD/1/1/FHDEV /var/www/swad/crs/807
mv /var/www/html/asg/LCAFD/1/1/FHDEV /var/www/html/crs/807

mv /var/www/swad/asg/LCAFD/2/1/BAFAB /var/www/swad/crs/808
mv /var/www/html/asg/LCAFD/2/1/BAFAB /var/www/html/crs/808

mv /var/www/swad/asg/LCAFD/2/2/FHA /var/www/swad/crs/809
mv /var/www/html/asg/LCAFD/2/2/FHA /var/www/html/crs/809

mv /var/www/swad/asg/LCAFD/2/2/FHB /var/www/swad/crs/810
mv /var/www/html/asg/LCAFD/2/2/FHB /var/www/html/crs/810

mv /var/www/swad/asg/LCAFD/2/2/FHC /var/www/swad/crs/811
mv /var/www/html/asg/LCAFD/2/2/FHC /var/www/html/crs/811

mv /var/www/swad/asg/LCAFD/2/2/FETD /var/www/swad/crs/812
mv /var/www/html/asg/LCAFD/2/2/FETD /var/www/html/crs/812

mv /var/www/swad/asg/LCAFD/2/0/FHDEBC /var/www/swad/crs/813
mv /var/www/html/asg/LCAFD/2/0/FHDEBC /var/www/html/crs/813

mv /var/www/swad/asg/LCAFD/2/0/FHDEB /var/www/swad/crs/814
mv /var/www/html/asg/LCAFD/2/0/FHDEB /var/www/html/crs/814

mv /var/www/swad/asg/LCAFD/2/2/FHDEF /var/www/swad/crs/815
mv /var/www/html/asg/LCAFD/2/2/FHDEF /var/www/html/crs/815

mv /var/www/swad/asg/LCAFD/2/0/FHDL /var/www/swad/crs/816
mv /var/www/html/asg/LCAFD/2/0/FHDL /var/www/html/crs/816

mv /var/www/swad/asg/LCAFD/2/2/JM /var/www/swad/crs/817
mv /var/www/html/asg/LCAFD/2/2/JM /var/www/html/crs/817

mv /var/www/swad/asg/LCAFD/2/1/SD /var/www/swad/crs/818
mv /var/www/html/asg/LCAFD/2/1/SD /var/www/html/crs/818

mv /var/www/swad/asg/LCAFD/2/0/THD /var/www/swad/crs/819
mv /var/www/html/asg/LCAFD/2/0/THD /var/www/html/crs/819

mv /var/www/swad/asg/LCAFD/3/2/BCECD /var/www/swad/crs/820
mv /var/www/html/asg/LCAFD/3/2/BCECD /var/www/html/crs/820

mv /var/www/swad/asg/LCAFD/3/1/BED /var/www/swad/crs/821
mv /var/www/html/asg/LCAFD/3/1/BED /var/www/html/crs/821

mv /var/www/swad/asg/LCAFD/3/1/BENMAM /var/www/swad/crs/822
mv /var/www/html/asg/LCAFD/3/1/BENMAM /var/www/html/crs/822

mv /var/www/swad/asg/LCAFD/3/1/EDAA /var/www/swad/crs/823
mv /var/www/html/asg/LCAFD/3/1/EDAA /var/www/html/crs/823

mv /var/www/swad/asg/LCAFD/3/2/EDA /var/www/swad/crs/824
mv /var/www/html/asg/LCAFD/3/2/EDA /var/www/html/crs/824

mv /var/www/swad/asg/LCAFD/3/2/EDB /var/www/swad/crs/825
mv /var/www/html/asg/LCAFD/3/2/EDB /var/www/html/crs/825

mv /var/www/swad/asg/LCAFD/3/1/EDI /var/www/swad/crs/826
mv /var/www/html/asg/LCAFD/3/1/EDI /var/www/html/crs/826

mv /var/www/swad/asg/LCAFD/3/0/JDDTA /var/www/swad/crs/827
mv /var/www/html/asg/LCAFD/3/0/JDDTA /var/www/html/crs/827

mv /var/www/swad/asg/LCAFD/3/1/PA /var/www/swad/crs/828
mv /var/www/html/asg/LCAFD/3/1/PA /var/www/html/crs/828

mv /var/www/swad/asg/LCAFD/4/0/ARDDI /var/www/swad/crs/829
mv /var/www/html/asg/LCAFD/4/0/ARDDI /var/www/html/crs/829

mv /var/www/swad/asg/LCAFD/4/1/AEEFD /var/www/swad/crs/830
mv /var/www/html/asg/LCAFD/4/1/AEEFD /var/www/html/crs/830

mv /var/www/swad/asg/LCAFD/4/1/BGPGD /var/www/swad/crs/831
mv /var/www/html/asg/LCAFD/4/1/BGPGD /var/www/html/crs/831

mv /var/www/swad/asg/LCAFD/4/0/EAFDA /var/www/swad/crs/832
mv /var/www/html/asg/LCAFD/4/0/EAFDA /var/www/html/crs/832

mv /var/www/swad/asg/LCAFD/4/0/EAFDB /var/www/swad/crs/833
mv /var/www/html/asg/LCAFD/4/0/EAFDB /var/www/html/crs/833

mv /var/www/swad/asg/LCAFD/4/0/EAFDC /var/www/swad/crs/834
mv /var/www/html/asg/LCAFD/4/0/EAFDC /var/www/html/crs/834

mv /var/www/swad/asg/LCAFD/4/2/E /var/www/swad/crs/835
mv /var/www/html/asg/LCAFD/4/2/E /var/www/html/crs/835

mv /var/www/swad/asg/LCAFD/4/2/FEAC /var/www/swad/crs/836
mv /var/www/html/asg/LCAFD/4/2/FEAC /var/www/html/crs/836

mv /var/www/swad/asg/LCAFD/4/1/FEEC /var/www/swad/crs/837
mv /var/www/html/asg/LCAFD/4/1/FEEC /var/www/html/crs/837

mv /var/www/swad/asg/LCAFD/4/1/OARATL /var/www/swad/crs/838
mv /var/www/html/asg/LCAFD/4/1/OARATL /var/www/html/crs/838

mv /var/www/swad/asg/LCAFD/4/2/OED /var/www/swad/crs/839
mv /var/www/html/asg/LCAFD/4/2/OED /var/www/html/crs/839

mv /var/www/swad/asg/LCAFD/5/1/AFSA /var/www/swad/crs/840
mv /var/www/html/asg/LCAFD/5/1/AFSA /var/www/html/crs/840

mv /var/www/swad/asg/LCAFD/5/1/AFSB /var/www/swad/crs/841
mv /var/www/html/asg/LCAFD/5/1/AFSB /var/www/html/crs/841

mv /var/www/swad/asg/LCAFD/5/1/AFSC /var/www/swad/crs/842
mv /var/www/html/asg/LCAFD/5/1/AFSC /var/www/html/crs/842

mv /var/www/swad/asg/LCAFD/5/2/AMN /var/www/swad/crs/843
mv /var/www/html/asg/LCAFD/5/2/AMN /var/www/html/crs/843

mv /var/www/swad/asg/LCAFD/5/2/BTD /var/www/swad/crs/844
mv /var/www/html/asg/LCAFD/5/2/BTD /var/www/html/crs/844

mv /var/www/swad/asg/LCAFD/5/2/BED /var/www/swad/crs/845
mv /var/www/html/asg/LCAFD/5/2/BED /var/www/html/crs/845

mv /var/www/swad/asg/LCAFD/5/2/CSIPD /var/www/swad/crs/846
mv /var/www/html/asg/LCAFD/5/2/CSIPD /var/www/html/crs/846

mv /var/www/swad/asg/LCAFD/5/1/EAAF /var/www/swad/crs/847
mv /var/www/html/asg/LCAFD/5/1/EAAF /var/www/html/crs/847

mv /var/www/swad/asg/LCAFD/5/1/EFDAN /var/www/swad/crs/848
mv /var/www/html/asg/LCAFD/5/1/EFDAN /var/www/html/crs/848

mv /var/www/swad/asg/LCAFD/5/0/PARA /var/www/swad/crs/849
mv /var/www/html/asg/LCAFD/5/0/PARA /var/www/html/crs/849

mv /var/www/swad/asg/LCAFD/5/0/PE /var/www/swad/crs/850
mv /var/www/html/asg/LCAFD/5/0/PE /var/www/html/crs/850

mv /var/www/swad/asg/LCAFD/5/0/PEAD /var/www/swad/crs/851
mv /var/www/html/asg/LCAFD/5/0/PEAD /var/www/html/crs/851

mv /var/www/swad/asg/LCAFD/5/2/SPD /var/www/swad/crs/852
mv /var/www/html/asg/LCAFD/5/2/SPD /var/www/html/crs/852

mv /var/www/swad/asg/LCA/0/1/B /var/www/swad/crs/853
mv /var/www/html/asg/LCA/0/1/B /var/www/html/crs/853

mv /var/www/swad/asg/LCA/0/2/P /var/www/swad/crs/854
mv /var/www/html/asg/LCA/0/2/P /var/www/html/crs/854

mv /var/www/swad/asg/LCA/0/1/RCGR /var/www/swad/crs/855
mv /var/www/html/asg/LCA/0/1/RCGR /var/www/html/crs/855

mv /var/www/swad/asg/LCA/0/2/TMRRU /var/www/swad/crs/856
mv /var/www/html/asg/LCA/0/2/TMRRU /var/www/html/crs/856

mv /var/www/swad/asg/LCA/1/2/BIA /var/www/swad/crs/857
mv /var/www/html/asg/LCA/1/2/BIA /var/www/html/crs/857

mv /var/www/swad/asg/LCA/1/1/BQMAA /var/www/swad/crs/858
mv /var/www/html/asg/LCA/1/1/BQMAA /var/www/html/crs/858

mv /var/www/swad/asg/LCA/1/1/BQMAAB /var/www/swad/crs/859
mv /var/www/html/asg/LCA/1/1/BQMAAB /var/www/html/crs/859

mv /var/www/swad/asg/LCA/1/1/BQMAC /var/www/swad/crs/860
mv /var/www/html/asg/LCA/1/1/BQMAC /var/www/html/crs/860

mv /var/www/swad/asg/LCA/1/1/BQMAD /var/www/swad/crs/861
mv /var/www/html/asg/LCA/1/1/BQMAD /var/www/html/crs/861

mv /var/www/swad/asg/LCA/1/0/B /var/www/swad/crs/862
mv /var/www/html/asg/LCA/1/0/B /var/www/html/crs/862

mv /var/www/swad/asg/LCA/1/0/MFC /var/www/swad/crs/863
mv /var/www/html/asg/LCA/1/0/MFC /var/www/html/crs/863

mv /var/www/swad/asg/LCA/2/2/ALA /var/www/swad/crs/864
mv /var/www/html/asg/LCA/2/2/ALA /var/www/html/crs/864

mv /var/www/swad/asg/LCA/2/2/B /var/www/swad/crs/865
mv /var/www/html/asg/LCA/2/2/B /var/www/html/crs/865

mv /var/www/swad/asg/LCA/2/2/FMA /var/www/swad/crs/866
mv /var/www/html/asg/LCA/2/2/FMA /var/www/html/crs/866

mv /var/www/swad/asg/LCA/2/2/QMA /var/www/swad/crs/867
mv /var/www/html/asg/LCA/2/2/QMA /var/www/html/crs/867

mv /var/www/swad/asg/LCA/2/1/PTC /var/www/swad/crs/868
mv /var/www/html/asg/LCA/2/1/PTC /var/www/html/crs/868

mv /var/www/swad/asg/LCA/4/1/CA /var/www/swad/crs/869
mv /var/www/html/asg/LCA/4/1/CA /var/www/html/crs/869

mv /var/www/swad/asg/LCA/4/2/EMA /var/www/swad/crs/870
mv /var/www/html/asg/LCA/4/2/EMA /var/www/html/crs/870

mv /var/www/swad/asg/LCA/4/0/EIA /var/www/swad/crs/871
mv /var/www/html/asg/LCA/4/0/EIA /var/www/html/crs/871

mv /var/www/swad/asg/LCA/4/2/GCFF /var/www/swad/crs/872
mv /var/www/html/asg/LCA/4/2/GCFF /var/www/html/crs/872

mv /var/www/swad/asg/LCA/4/0/OTMA /var/www/swad/crs/873
mv /var/www/html/asg/LCA/4/0/OTMA /var/www/html/crs/873

mv /var/www/swad/asg/LCA/4/1/RMMA /var/www/swad/crs/874
mv /var/www/html/asg/LCA/4/1/RMMA /var/www/html/crs/874

mv /var/www/swad/asg/LCPA/0/1/CP /var/www/swad/crs/875
mv /var/www/html/asg/LCPA/0/1/CP /var/www/html/crs/875

mv /var/www/swad/asg/LCPA/0/2/PGA /var/www/swad/crs/876
mv /var/www/html/asg/LCPA/0/2/PGA /var/www/html/crs/876

mv /var/www/swad/asg/LCPA/0/1/SICAP /var/www/swad/crs/877
mv /var/www/html/asg/LCPA/0/1/SICAP /var/www/html/crs/877

mv /var/www/swad/asg/LCPA/1/0/EP /var/www/swad/crs/878
mv /var/www/html/asg/LCPA/1/0/EP /var/www/html/crs/878

mv /var/www/swad/asg/LCPA/1/0/EPB /var/www/swad/crs/879
mv /var/www/html/asg/LCPA/1/0/EPB /var/www/html/crs/879

mv /var/www/swad/asg/LCPA/1/0/FCP /var/www/swad/crs/880
mv /var/www/html/asg/LCPA/1/0/FCP /var/www/html/crs/880

mv /var/www/swad/asg/LCPA/1/0/HSPC /var/www/swad/crs/881
mv /var/www/html/asg/LCPA/1/0/HSPC /var/www/html/crs/881

mv /var/www/swad/asg/LCPA/1/0/SG /var/www/swad/crs/882
mv /var/www/html/asg/LCPA/1/0/SG /var/www/html/crs/882

mv /var/www/swad/asg/LCPA/2/0/CAB /var/www/swad/crs/883
mv /var/www/html/asg/LCPA/2/0/CAB /var/www/html/crs/883

mv /var/www/swad/asg/LCPA/2/1/RPSE /var/www/swad/crs/884
mv /var/www/html/asg/LCPA/2/1/RPSE /var/www/html/crs/884

mv /var/www/swad/asg/LCPA/2/0/TEA /var/www/swad/crs/885
mv /var/www/html/asg/LCPA/2/0/TEA /var/www/html/crs/885

mv /var/www/swad/asg/LCPA/2/0/TEB /var/www/swad/crs/886
mv /var/www/html/asg/LCPA/2/0/TEB /var/www/html/crs/886

mv /var/www/swad/asg/LCPA/4/1/DCEA /var/www/swad/crs/887
mv /var/www/html/asg/LCPA/4/1/DCEA /var/www/html/crs/887

mv /var/www/swad/asg/LCPA/4/1/PGAA /var/www/swad/crs/888
mv /var/www/html/asg/LCPA/4/1/PGAA /var/www/html/crs/888

mv /var/www/swad/asg/LCPA/5/1/IDUE /var/www/swad/crs/889
mv /var/www/html/asg/LCPA/5/1/IDUE /var/www/html/crs/889

mv /var/www/swad/asg/LCPA/5/1/PA /var/www/swad/crs/890
mv /var/www/html/asg/LCPA/5/1/PA /var/www/html/crs/890

mv /var/www/swad/asg/LCPA/5/1/PPEA /var/www/swad/crs/891
mv /var/www/html/asg/LCPA/5/1/PPEA /var/www/html/crs/891

mv /var/www/swad/asg/LCPSS/0/0/IFE /var/www/swad/crs/892
mv /var/www/html/asg/LCPSS/0/0/IFE /var/www/html/crs/892

mv /var/www/swad/asg/LCPSS/5/0/PS /var/www/swad/crs/893
mv /var/www/html/asg/LCPSS/5/0/PS /var/www/html/crs/893

mv /var/www/swad/asg/LCTE/1/1/ATD1 /var/www/swad/crs/894
mv /var/www/html/asg/LCTE/1/1/ATD1 /var/www/html/crs/894

mv /var/www/swad/asg/LCTE/2/1/ATD2 /var/www/swad/crs/895
mv /var/www/html/asg/LCTE/2/1/ATD2 /var/www/html/crs/895

mv /var/www/swad/asg/LCTE/2/2/MR /var/www/swad/crs/896
mv /var/www/html/asg/LCTE/2/2/MR /var/www/html/crs/896

mv /var/www/swad/asg/LCTE/2/0/MEIO /var/www/swad/crs/897
mv /var/www/html/asg/LCTE/2/0/MEIO /var/www/html/crs/897

mv /var/www/swad/asg/LCTE/2/2/TAM /var/www/swad/crs/898
mv /var/www/html/asg/LCTE/2/2/TAM /var/www/html/crs/898

mv /var/www/swad/asg/LCT/0/1/DET /var/www/swad/crs/899
mv /var/www/html/asg/LCT/0/1/DET /var/www/html/crs/899

mv /var/www/swad/asg/LCT/0/1/GC /var/www/swad/crs/900
mv /var/www/html/asg/LCT/0/1/GC /var/www/html/crs/900

mv /var/www/swad/asg/LCT/0/0/IFE /var/www/swad/crs/901
mv /var/www/html/asg/LCT/0/0/IFE /var/www/html/crs/901

mv /var/www/swad/asg/LCT/0/1/SFP /var/www/swad/crs/902
mv /var/www/html/asg/LCT/0/1/SFP /var/www/html/crs/902

mv /var/www/swad/asg/LCT/1/1/CHOA /var/www/swad/crs/903
mv /var/www/html/asg/LCT/1/1/CHOA /var/www/html/crs/903

mv /var/www/swad/asg/LCT/1/1/DPT /var/www/swad/crs/904
mv /var/www/html/asg/LCT/1/1/DPT /var/www/html/crs/904

mv /var/www/swad/asg/LCT/1/1/EDPP1F /var/www/swad/crs/905
mv /var/www/html/asg/LCT/1/1/EDPP1F /var/www/html/crs/905

mv /var/www/swad/asg/LCT/2/1/PGRH /var/www/swad/crs/906
mv /var/www/html/asg/LCT/2/1/PGRH /var/www/html/crs/906

mv /var/www/swad/asg/LCAV/1/2/CPPE /var/www/swad/crs/907
mv /var/www/html/asg/LCAV/1/2/CPPE /var/www/html/crs/907

mv /var/www/swad/asg/LCAV/4/1/DI /var/www/swad/crs/908
mv /var/www/html/asg/LCAV/4/1/DI /var/www/html/crs/908

mv /var/www/swad/asg/LCAV/4/1/GA /var/www/swad/crs/909
mv /var/www/html/asg/LCAV/4/1/GA /var/www/html/crs/909

mv /var/www/swad/asg/LCAV/4/1/TCA /var/www/swad/crs/910
mv /var/www/html/asg/LCAV/4/1/TCA /var/www/html/crs/910

mv /var/www/swad/asg/LCAV/4/2/TPA /var/www/swad/crs/911
mv /var/www/html/asg/LCAV/4/2/TPA /var/www/html/crs/911

mv /var/www/swad/asg/LD/0/1/AJVAF /var/www/swad/crs/912
mv /var/www/html/asg/LD/0/1/AJVAF /var/www/html/crs/912

mv /var/www/swad/asg/LD/0/1/AJDCA /var/www/swad/crs/913
mv /var/www/html/asg/LD/0/1/AJDCA /var/www/html/crs/913

mv /var/www/swad/asg/LD/0/2/ASASL /var/www/swad/crs/914
mv /var/www/html/asg/LD/0/2/ASASL /var/www/html/crs/914

mv /var/www/swad/asg/LD/0/1/AJA /var/www/swad/crs/915
mv /var/www/html/asg/LD/0/1/AJA /var/www/html/crs/915

mv /var/www/swad/asg/LD/0/1/AJMCE /var/www/swad/crs/916
mv /var/www/html/asg/LD/0/1/AJMCE /var/www/html/crs/916

mv /var/www/swad/asg/LD/0/1/CMPC /var/www/swad/crs/917
mv /var/www/html/asg/LD/0/1/CMPC /var/www/html/crs/917

mv /var/www/swad/asg/LD/0/1/CMPCB /var/www/swad/crs/918
mv /var/www/html/asg/LD/0/1/CMPCB /var/www/html/crs/918

mv /var/www/swad/asg/LD/0/0/CFACJF /var/www/swad/crs/919
mv /var/www/html/asg/LD/0/0/CFACJF /var/www/html/crs/919

mv /var/www/swad/asg/LD/0/2/DAPAJ /var/www/swad/crs/920
mv /var/www/html/asg/LD/0/2/DAPAJ /var/www/html/crs/920

mv /var/www/swad/asg/LD/0/2/DSCMA /var/www/swad/crs/921
mv /var/www/html/asg/LD/0/2/DSCMA /var/www/html/crs/921

mv /var/www/swad/asg/LD/0/2/DCI /var/www/swad/crs/922
mv /var/www/html/asg/LD/0/2/DCI /var/www/html/crs/922

mv /var/www/swad/asg/LD/0/1/DDB /var/www/swad/crs/923
mv /var/www/html/asg/LD/0/1/DDB /var/www/html/crs/923

mv /var/www/swad/asg/LD/0/2/DEPA /var/www/swad/crs/924
mv /var/www/html/asg/LD/0/2/DEPA /var/www/html/crs/924

mv /var/www/swad/asg/LD/0/2/DEPB /var/www/swad/crs/925
mv /var/www/html/asg/LD/0/2/DEPB /var/www/html/crs/925

mv /var/www/swad/asg/LD/0/2/DMC /var/www/swad/crs/926
mv /var/www/html/asg/LD/0/2/DMC /var/www/html/crs/926

mv /var/www/swad/asg/LD/0/2/DMSB /var/www/swad/crs/927
mv /var/www/html/asg/LD/0/2/DMSB /var/www/html/crs/927

mv /var/www/swad/asg/LD/0/2/DPUE /var/www/swad/crs/928
mv /var/www/html/asg/LD/0/2/DPUE /var/www/html/crs/928

mv /var/www/swad/asg/LD/0/2/DPNJ /var/www/swad/crs/929
mv /var/www/html/asg/LD/0/2/DPNJ /var/www/html/crs/929

mv /var/www/swad/asg/LD/0/1/DPSI /var/www/swad/crs/930
mv /var/www/html/asg/LD/0/1/DPSI /var/www/html/crs/930

mv /var/www/swad/asg/LD/0/0/EP /var/www/swad/crs/931
mv /var/www/html/asg/LD/0/0/EP /var/www/html/crs/931

mv /var/www/swad/asg/LD/0/1/ERC /var/www/swad/crs/932
mv /var/www/html/asg/LD/0/1/ERC /var/www/html/crs/932

mv /var/www/swad/asg/LD/0/1/JPI /var/www/swad/crs/933
mv /var/www/html/asg/LD/0/1/JPI /var/www/html/crs/933

mv /var/www/swad/asg/LD/0/1/FI /var/www/swad/crs/934
mv /var/www/html/asg/LD/0/1/FI /var/www/html/crs/934

mv /var/www/swad/asg/LD/0/1/OCDEC /var/www/swad/crs/935
mv /var/www/html/asg/LD/0/1/OCDEC /var/www/html/crs/935

mv /var/www/swad/asg/LD/0/1/MRDE /var/www/swad/crs/936
mv /var/www/html/asg/LD/0/1/MRDE /var/www/html/crs/936

mv /var/www/swad/asg/LD/0/1/NE /var/www/swad/crs/937
mv /var/www/html/asg/LD/0/1/NE /var/www/html/crs/937

mv /var/www/swad/asg/LD/0/2/RJFCE /var/www/swad/crs/938
mv /var/www/html/asg/LD/0/2/RJFCE /var/www/html/crs/938

mv /var/www/swad/asg/LD/0/1/SFR /var/www/swad/crs/939
mv /var/www/html/asg/LD/0/1/SFR /var/www/html/crs/939

mv /var/www/swad/asg/LD/0/0/STL /var/www/swad/crs/940
mv /var/www/html/asg/LD/0/0/STL /var/www/html/crs/940

mv /var/www/swad/asg/LD/0/2/TEEJP /var/www/swad/crs/941
mv /var/www/html/asg/LD/0/2/TEEJP /var/www/html/crs/941

mv /var/www/swad/asg/LD/1/2/DC1AB /var/www/swad/crs/942
mv /var/www/html/asg/LD/1/2/DC1AB /var/www/html/crs/942

mv /var/www/swad/asg/LD/1/2/DC1D /var/www/swad/crs/943
mv /var/www/html/asg/LD/1/2/DC1D /var/www/html/crs/943

mv /var/www/swad/asg/LD/1/0/DCO1C /var/www/swad/crs/944
mv /var/www/html/asg/LD/1/0/DCO1C /var/www/html/crs/944

mv /var/www/swad/asg/LD/1/0/DCO1D /var/www/swad/crs/945
mv /var/www/html/asg/LD/1/0/DCO1D /var/www/html/crs/945

mv /var/www/swad/asg/LD/1/0/DCO1G /var/www/swad/crs/946
mv /var/www/html/asg/LD/1/0/DCO1G /var/www/html/crs/946

mv /var/www/swad/asg/LD/1/2/IDPC /var/www/swad/crs/947
mv /var/www/html/asg/LD/1/2/IDPC /var/www/html/crs/947

mv /var/www/swad/asg/LD/1/2/IDPD /var/www/swad/crs/948
mv /var/www/html/asg/LD/1/2/IDPD /var/www/html/crs/948

mv /var/www/swad/asg/LD/1/2/IDPE /var/www/swad/crs/949
mv /var/www/html/asg/LD/1/2/IDPE /var/www/html/crs/949

mv /var/www/swad/asg/LD/1/2/IDPG /var/www/swad/crs/950
mv /var/www/html/asg/LD/1/2/IDPG /var/www/html/crs/950

mv /var/www/swad/asg/LD/1/2/SFA /var/www/swad/crs/951
mv /var/www/html/asg/LD/1/2/SFA /var/www/html/crs/951

mv /var/www/swad/asg/LD/1/2/SFB /var/www/swad/crs/952
mv /var/www/html/asg/LD/1/2/SFB /var/www/html/crs/952

mv /var/www/swad/asg/LD/1/1/TDA /var/www/swad/crs/953
mv /var/www/html/asg/LD/1/1/TDA /var/www/html/crs/953

mv /var/www/swad/asg/LD/1/1/TDS /var/www/swad/crs/954
mv /var/www/html/asg/LD/1/1/TDS /var/www/html/crs/954

mv /var/www/swad/asg/LD/2/0/DC2A /var/www/swad/crs/955
mv /var/www/html/asg/LD/2/0/DC2A /var/www/html/crs/955

mv /var/www/swad/asg/LD/2/0/DC2BC /var/www/swad/crs/956
mv /var/www/html/asg/LD/2/0/DC2BC /var/www/html/crs/956

mv /var/www/swad/asg/LD/2/0/DC2C /var/www/swad/crs/957
mv /var/www/html/asg/LD/2/0/DC2C /var/www/html/crs/957

mv /var/www/swad/asg/LD/2/0/DC2CF /var/www/swad/crs/958
mv /var/www/html/asg/LD/2/0/DC2CF /var/www/html/crs/958

mv /var/www/swad/asg/LD/2/0/DC2D /var/www/swad/crs/959
mv /var/www/html/asg/LD/2/0/DC2D /var/www/html/crs/959

mv /var/www/swad/asg/LD/2/1/DCO2A /var/www/swad/crs/960
mv /var/www/html/asg/LD/2/1/DCO2A /var/www/html/crs/960

mv /var/www/swad/asg/LD/2/2/DEPB /var/www/swad/crs/961
mv /var/www/html/asg/LD/2/2/DEPB /var/www/html/crs/961

mv /var/www/swad/asg/LD/2/0/EPHPD /var/www/swad/crs/962
mv /var/www/html/asg/LD/2/0/EPHPD /var/www/html/crs/962

mv /var/www/swad/asg/LD/2/0/EPHPEF /var/www/swad/crs/963
mv /var/www/html/asg/LD/2/0/EPHPEF /var/www/html/crs/963

mv /var/www/swad/asg/LD/2/2/SGDL /var/www/swad/crs/964
mv /var/www/html/asg/LD/2/2/SGDL /var/www/html/crs/964

mv /var/www/swad/asg/LD/3/2/DC3A /var/www/swad/crs/965
mv /var/www/html/asg/LD/3/2/DC3A /var/www/html/crs/965

mv /var/www/swad/asg/LD/3/2/DC3BC /var/www/swad/crs/966
mv /var/www/html/asg/LD/3/2/DC3BC /var/www/html/crs/966

mv /var/www/swad/asg/LD/3/2/DC3C /var/www/swad/crs/967
mv /var/www/html/asg/LD/3/2/DC3C /var/www/html/crs/967

mv /var/www/swad/asg/LD/3/2/DC3E /var/www/swad/crs/968
mv /var/www/html/asg/LD/3/2/DC3E /var/www/html/crs/968

mv /var/www/swad/asg/LD/3/2/DC3F /var/www/swad/crs/969
mv /var/www/html/asg/LD/3/2/DC3F /var/www/html/crs/969

mv /var/www/swad/asg/LD/3/2/DGIUB /var/www/swad/crs/970
mv /var/www/html/asg/LD/3/2/DGIUB /var/www/html/crs/970

mv /var/www/swad/asg/LD/3/2/IDCA /var/www/swad/crs/971
mv /var/www/html/asg/LD/3/2/IDCA /var/www/html/crs/971

mv /var/www/swad/asg/LD/4/0/DADG /var/www/swad/crs/972
mv /var/www/html/asg/LD/4/0/DADG /var/www/html/crs/972

mv /var/www/swad/asg/LD/4/0/DC4BD /var/www/swad/crs/973
mv /var/www/html/asg/LD/4/0/DC4BD /var/www/html/crs/973

mv /var/www/swad/asg/LD/4/0/DC4C /var/www/swad/crs/974
mv /var/www/html/asg/LD/4/0/DC4C /var/www/html/crs/974

mv /var/www/swad/asg/LD/4/0/DC4D /var/www/swad/crs/975
mv /var/www/html/asg/LD/4/0/DC4D /var/www/html/crs/975

mv /var/www/swad/asg/LD/4/0/DFT1A /var/www/swad/crs/976
mv /var/www/html/asg/LD/4/0/DFT1A /var/www/html/crs/976

mv /var/www/swad/asg/LD/4/0/DFT1B /var/www/swad/crs/977
mv /var/www/html/asg/LD/4/0/DFT1B /var/www/html/crs/977

mv /var/www/swad/asg/LD/4/0/DFT1DF /var/www/swad/crs/978
mv /var/www/html/asg/LD/4/0/DFT1DF /var/www/html/crs/978

mv /var/www/swad/asg/LD/4/0/DFT1G /var/www/swad/crs/979
mv /var/www/html/asg/LD/4/0/DFT1G /var/www/html/crs/979

mv /var/www/swad/asg/LD/4/0/DM1 /var/www/swad/crs/980
mv /var/www/html/asg/LD/4/0/DM1 /var/www/html/crs/980

mv /var/www/swad/asg/LD/4/0/DM1D /var/www/swad/crs/981
mv /var/www/html/asg/LD/4/0/DM1D /var/www/html/crs/981

mv /var/www/swad/asg/LD/4/0/DM1E /var/www/swad/crs/982
mv /var/www/html/asg/LD/4/0/DM1E /var/www/html/crs/982

mv /var/www/swad/asg/LD/4/0/HTA /var/www/swad/crs/983
mv /var/www/html/asg/LD/4/0/HTA /var/www/html/crs/983

mv /var/www/swad/asg/LD/4/0/HTB /var/www/swad/crs/984
mv /var/www/html/asg/LD/4/0/HTB /var/www/html/crs/984

mv /var/www/swad/asg/LD/5/1/DEEA /var/www/swad/crs/985
mv /var/www/html/asg/LD/5/1/DEEA /var/www/html/crs/985

mv /var/www/swad/asg/LD/5/1/DEEB /var/www/swad/crs/986
mv /var/www/html/asg/LD/5/1/DEEB /var/www/html/crs/986

mv /var/www/swad/asg/LD/5/1/DEEC /var/www/swad/crs/987
mv /var/www/html/asg/LD/5/1/DEEC /var/www/html/crs/987

mv /var/www/swad/asg/LD/5/1/DEED /var/www/swad/crs/988
mv /var/www/html/asg/LD/5/1/DEED /var/www/html/crs/988

mv /var/www/swad/asg/LD/5/0/DFT2C /var/www/swad/crs/989
mv /var/www/html/asg/LD/5/0/DFT2C /var/www/html/crs/989

mv /var/www/swad/asg/LD/5/0/DFT2D /var/www/swad/crs/990
mv /var/www/html/asg/LD/5/0/DFT2D /var/www/html/crs/990

mv /var/www/swad/asg/LD/5/0/DFT2B /var/www/swad/crs/991
mv /var/www/html/asg/LD/5/0/DFT2B /var/www/html/crs/991

mv /var/www/swad/asg/LD/5/0/DIPA /var/www/swad/crs/992
mv /var/www/html/asg/LD/5/0/DIPA /var/www/html/crs/992

mv /var/www/swad/asg/LD/5/0/DIP /var/www/swad/crs/993
mv /var/www/html/asg/LD/5/0/DIP /var/www/html/crs/993

mv /var/www/swad/asg/LD/5/0/DIPC /var/www/swad/crs/994
mv /var/www/html/asg/LD/5/0/DIPC /var/www/html/crs/994

mv /var/www/swad/asg/LD/5/0/DIPEF /var/www/swad/crs/995
mv /var/www/html/asg/LD/5/0/DIPEF /var/www/html/crs/995

mv /var/www/swad/asg/LD/5/1/DM2A /var/www/swad/crs/996
mv /var/www/html/asg/LD/5/1/DM2A /var/www/html/crs/996

mv /var/www/swad/asg/LD/5/1/DM2C /var/www/swad/crs/997
mv /var/www/html/asg/LD/5/1/DM2C /var/www/html/crs/997

mv /var/www/swad/asg/LD/5/1/DM2E /var/www/swad/crs/998
mv /var/www/html/asg/LD/5/1/DM2E /var/www/html/crs/998

mv /var/www/swad/asg/LD/5/1/FDA /var/www/swad/crs/999
mv /var/www/html/asg/LD/5/1/FDA /var/www/html/crs/999

mv /var/www/swad/asg/LD/5/1/FDC /var/www/swad/crs/1000
mv /var/www/html/asg/LD/5/1/FDC /var/www/html/crs/1000

mv /var/www/swad/asg/LD/5/1/FDD /var/www/swad/crs/1001
mv /var/www/html/asg/LD/5/1/FDD /var/www/html/crs/1001

mv /var/www/swad/asg/LD/5/1/FDDE /var/www/swad/crs/1002
mv /var/www/html/asg/LD/5/1/FDDE /var/www/html/crs/1002

mv /var/www/swad/asg/LD/5/2/P /var/www/swad/crs/1003
mv /var/www/html/asg/LD/5/2/P /var/www/html/crs/1003

mv /var/www/swad/asg/LD/5/2/PA /var/www/swad/crs/1004
mv /var/www/html/asg/LD/5/2/PA /var/www/html/crs/1004

mv /var/www/swad/asg/LDCPA/3/0/EP /var/www/swad/crs/1005
mv /var/www/html/asg/LDCPA/3/0/EP /var/www/html/crs/1005

mv /var/www/swad/asg/LDCPA/4/0/DF1 /var/www/swad/crs/1006
mv /var/www/html/asg/LDCPA/4/0/DF1 /var/www/html/crs/1006

mv /var/www/swad/asg/LDCPA/5/1/PC /var/www/swad/crs/1007
mv /var/www/html/asg/LDCPA/5/1/PC /var/www/html/crs/1007

mv /var/www/swad/asg/LDCPA/5/1/PP /var/www/swad/crs/1008
mv /var/www/html/asg/LDCPA/5/1/PP /var/www/html/crs/1008

mv /var/www/swad/asg/LDOC/0/2/LNCT /var/www/swad/crs/1009
mv /var/www/html/asg/LDOC/0/2/LNCT /var/www/html/crs/1009

mv /var/www/swad/asg/LDOC/1/0/ARUI /var/www/swad/crs/1010
mv /var/www/html/asg/LDOC/1/0/ARUI /var/www/html/crs/1010

mv /var/www/swad/asg/LDOC/1/1/E /var/www/swad/crs/1011
mv /var/www/html/asg/LDOC/1/1/E /var/www/html/crs/1011

mv /var/www/swad/asg/LDOC/1/1/EB /var/www/swad/crs/1012
mv /var/www/html/asg/LDOC/1/1/EB /var/www/html/crs/1012

mv /var/www/swad/asg/LDOC/1/1/EBC /var/www/swad/crs/1013
mv /var/www/html/asg/LDOC/1/1/EBC /var/www/html/crs/1013

mv /var/www/swad/asg/LDOC/1/1/SI /var/www/swad/crs/1014
mv /var/www/html/asg/LDOC/1/1/SI /var/www/html/crs/1014

mv /var/www/swad/asg/LE/0/2/CF /var/www/swad/crs/1015
mv /var/www/html/asg/LE/0/2/CF /var/www/html/crs/1015

mv /var/www/swad/asg/LE/0/2/CE /var/www/swad/crs/1016
mv /var/www/html/asg/LE/0/2/CE /var/www/html/crs/1016

mv /var/www/swad/asg/LE/0/2/DAE /var/www/swad/crs/1017
mv /var/www/html/asg/LE/0/2/DAE /var/www/html/crs/1017

mv /var/www/swad/asg/LE/0/2/EUE /var/www/swad/crs/1018
mv /var/www/html/asg/LE/0/2/EUE /var/www/html/crs/1018

mv /var/www/swad/asg/LE/0/1/EMARN /var/www/swad/crs/1019
mv /var/www/html/asg/LE/0/1/EMARN /var/www/html/crs/1019

mv /var/www/swad/asg/LE/0/1/ET /var/www/swad/crs/1020
mv /var/www/html/asg/LE/0/1/ET /var/www/html/crs/1020

mv /var/www/swad/asg/LE/0/1/EGP /var/www/swad/crs/1021
mv /var/www/html/asg/LE/0/1/EGP /var/www/html/crs/1021

mv /var/www/swad/asg/LE/0/2/EIND /var/www/swad/crs/1022
mv /var/www/html/asg/LE/0/2/EIND /var/www/html/crs/1022

mv /var/www/swad/asg/LE/0/2/EINT /var/www/swad/crs/1023
mv /var/www/html/asg/LE/0/2/EINT /var/www/html/crs/1023

mv /var/www/swad/asg/LE/0/2/EU /var/www/swad/crs/1024
mv /var/www/html/asg/LE/0/2/EU /var/www/html/crs/1024

mv /var/www/swad/asg/LE/0/1/EPR /var/www/swad/crs/1025
mv /var/www/html/asg/LE/0/1/EPR /var/www/html/crs/1025

mv /var/www/swad/asg/LE/0/1/GEMI /var/www/swad/crs/1026
mv /var/www/html/asg/LE/0/1/GEMI /var/www/html/crs/1026

mv /var/www/swad/asg/LE/0/1/IC /var/www/swad/crs/1027
mv /var/www/html/asg/LE/0/1/IC /var/www/html/crs/1027

mv /var/www/swad/asg/LE/0/1/ITDE /var/www/swad/crs/1028
mv /var/www/html/asg/LE/0/1/ITDE /var/www/html/crs/1028

mv /var/www/swad/asg/LE/0/2/IEI /var/www/swad/crs/1029
mv /var/www/html/asg/LE/0/2/IEI /var/www/html/crs/1029

mv /var/www/swad/asg/LE/0/1/MARKI /var/www/swad/crs/1030
mv /var/www/html/asg/LE/0/1/MARKI /var/www/html/crs/1030

mv /var/www/swad/asg/LE/0/2/MARKA /var/www/swad/crs/1031
mv /var/www/html/asg/LE/0/2/MARKA /var/www/html/crs/1031

mv /var/www/swad/asg/LE/0/1/PEE /var/www/swad/crs/1032
mv /var/www/html/asg/LE/0/1/PEE /var/www/html/crs/1032

mv /var/www/swad/asg/LE/0/2/OM /var/www/swad/crs/1033
mv /var/www/html/asg/LE/0/2/OM /var/www/html/crs/1033

mv /var/www/swad/asg/LE/0/2/PEE /var/www/swad/crs/1034
mv /var/www/html/asg/LE/0/2/PEE /var/www/html/crs/1034

mv /var/www/swad/asg/LE/0/1/SD /var/www/swad/crs/1035
mv /var/www/html/asg/LE/0/1/SD /var/www/html/crs/1035

mv /var/www/swad/asg/LE/0/2/SOC /var/www/swad/crs/1036
mv /var/www/html/asg/LE/0/2/SOC /var/www/html/crs/1036

mv /var/www/swad/asg/LE/0/2/THE1 /var/www/swad/crs/1037
mv /var/www/html/asg/LE/0/2/THE1 /var/www/html/crs/1037

mv /var/www/swad/asg/LE/0/1/THE2 /var/www/swad/crs/1038
mv /var/www/html/asg/LE/0/1/THE2 /var/www/html/crs/1038

mv /var/www/swad/asg/LE/1/1/C1 /var/www/swad/crs/1039
mv /var/www/html/asg/LE/1/1/C1 /var/www/html/crs/1039

mv /var/www/swad/asg/LE/1/2/C2 /var/www/swad/crs/1040
mv /var/www/html/asg/LE/1/2/C2 /var/www/html/crs/1040

mv /var/www/swad/asg/LE/1/2/DCB /var/www/swad/crs/1041
mv /var/www/html/asg/LE/1/2/DCB /var/www/html/crs/1041

mv /var/www/swad/asg/LE/1/0/ECONL /var/www/swad/crs/1042
mv /var/www/html/asg/LE/1/0/ECONL /var/www/html/crs/1042

mv /var/www/swad/asg/LE/1/0/ECONLE /var/www/swad/crs/1043
mv /var/www/html/asg/LE/1/0/ECONLE /var/www/html/crs/1043

mv /var/www/swad/asg/LE/1/1/HE1 /var/www/swad/crs/1044
mv /var/www/html/asg/LE/1/1/HE1 /var/www/html/crs/1044

mv /var/www/swad/asg/LE/1/2/HE2A /var/www/swad/crs/1045
mv /var/www/html/asg/LE/1/2/HE2A /var/www/html/crs/1045

mv /var/www/swad/asg/LE/1/2/HE2B /var/www/swad/crs/1046
mv /var/www/html/asg/LE/1/2/HE2B /var/www/html/crs/1046

mv /var/www/swad/asg/LE/1/2/HE2C /var/www/swad/crs/1047
mv /var/www/html/asg/LE/1/2/HE2C /var/www/html/crs/1047

mv /var/www/swad/asg/LE/1/2/HE2D /var/www/swad/crs/1048
mv /var/www/html/asg/LE/1/2/HE2D /var/www/html/crs/1048

mv /var/www/swad/asg/LE/1/2/IDDCEA /var/www/swad/crs/1049
mv /var/www/html/asg/LE/1/2/IDDCEA /var/www/html/crs/1049

mv /var/www/swad/asg/LE/1/2/IDABD /var/www/swad/crs/1050
mv /var/www/html/asg/LE/1/2/IDABD /var/www/html/crs/1050

mv /var/www/swad/asg/LE/1/2/IDC /var/www/swad/crs/1051
mv /var/www/html/asg/LE/1/2/IDC /var/www/html/crs/1051

mv /var/www/swad/asg/LE/1/1/IE /var/www/swad/crs/1052
mv /var/www/html/asg/LE/1/1/IE /var/www/html/crs/1052

mv /var/www/swad/asg/LE/1/2/IEA /var/www/swad/crs/1053
mv /var/www/html/asg/LE/1/2/IEA /var/www/html/crs/1053

mv /var/www/swad/asg/LE/1/1/M1 /var/www/swad/crs/1054
mv /var/www/html/asg/LE/1/1/M1 /var/www/html/crs/1054

mv /var/www/swad/asg/LE/1/2/M2 /var/www/swad/crs/1055
mv /var/www/html/asg/LE/1/2/M2 /var/www/html/crs/1055

mv /var/www/swad/asg/LE/1/2/MI1 /var/www/swad/crs/1056
mv /var/www/html/asg/LE/1/2/MI1 /var/www/html/crs/1056

mv /var/www/swad/asg/LE/1/2/MI1A /var/www/swad/crs/1057
mv /var/www/html/asg/LE/1/2/MI1A /var/www/html/crs/1057

mv /var/www/swad/asg/LE/1/1/TC1 /var/www/swad/crs/1058
mv /var/www/html/asg/LE/1/1/TC1 /var/www/html/crs/1058

mv /var/www/swad/asg/LE/1/2/TC2 /var/www/swad/crs/1059
mv /var/www/html/asg/LE/1/2/TC2 /var/www/html/crs/1059

mv /var/www/swad/asg/LE/2/2/ATC /var/www/swad/crs/1060
mv /var/www/html/asg/LE/2/2/ATC /var/www/html/crs/1060

mv /var/www/swad/asg/LE/2/1/C3C /var/www/swad/crs/1061
mv /var/www/html/asg/LE/2/1/C3C /var/www/html/crs/1061

mv /var/www/swad/asg/LE/2/1/DM /var/www/swad/crs/1062
mv /var/www/html/asg/LE/2/1/DM /var/www/html/crs/1062

mv /var/www/swad/asg/LE/2/2/EE /var/www/swad/crs/1063
mv /var/www/html/asg/LE/2/2/EE /var/www/html/crs/1063

mv /var/www/swad/asg/LE/2/1/EM /var/www/swad/crs/1064
mv /var/www/html/asg/LE/2/1/EM /var/www/html/crs/1064

mv /var/www/swad/asg/LE/2/2/HE3A /var/www/swad/crs/1065
mv /var/www/html/asg/LE/2/2/HE3A /var/www/html/crs/1065

mv /var/www/swad/asg/LE/2/1/MA1 /var/www/swad/crs/1066
mv /var/www/html/asg/LE/2/1/MA1 /var/www/html/crs/1066

mv /var/www/swad/asg/LE/2/2/MA2 /var/www/swad/crs/1067
mv /var/www/html/asg/LE/2/2/MA2 /var/www/html/crs/1067

mv /var/www/swad/asg/LE/2/1/MI2 /var/www/swad/crs/1068
mv /var/www/html/asg/LE/2/1/MI2 /var/www/html/crs/1068

mv /var/www/swad/asg/LE/2/1/M3 /var/www/swad/crs/1069
mv /var/www/html/asg/LE/2/1/M3 /var/www/html/crs/1069

mv /var/www/swad/asg/LE/3/1/ECO1 /var/www/swad/crs/1070
mv /var/www/html/asg/LE/3/1/ECO1 /var/www/html/crs/1070

mv /var/www/swad/asg/LE/3/2/ECO2 /var/www/swad/crs/1071
mv /var/www/html/asg/LE/3/2/ECO2 /var/www/html/crs/1071

mv /var/www/swad/asg/LE/3/2/EE /var/www/swad/crs/1072
mv /var/www/html/asg/LE/3/2/EE /var/www/html/crs/1072

mv /var/www/swad/asg/LE/3/1/ESP1 /var/www/swad/crs/1073
mv /var/www/html/asg/LE/3/1/ESP1 /var/www/html/crs/1073

mv /var/www/swad/asg/LE/3/2/ESP2 /var/www/swad/crs/1074
mv /var/www/html/asg/LE/3/2/ESP2 /var/www/html/crs/1074

mv /var/www/swad/asg/LE/3/1/MA3 /var/www/swad/crs/1075
mv /var/www/html/asg/LE/3/1/MA3 /var/www/html/crs/1075

mv /var/www/swad/asg/LE/3/2/MA4 /var/www/swad/crs/1076
mv /var/www/html/asg/LE/3/2/MA4 /var/www/html/crs/1076

mv /var/www/swad/asg/LE/3/1/MARK /var/www/swad/crs/1077
mv /var/www/html/asg/LE/3/1/MARK /var/www/html/crs/1077

mv /var/www/swad/asg/LE/3/2/OEI /var/www/swad/crs/1078
mv /var/www/html/asg/LE/3/2/OEI /var/www/html/crs/1078

mv /var/www/swad/asg/LE/3/1/PE /var/www/swad/crs/1079
mv /var/www/html/asg/LE/3/1/PE /var/www/html/crs/1079

mv /var/www/swad/asg/LE/4/1/EP /var/www/swad/crs/1080
mv /var/www/html/asg/LE/4/1/EP /var/www/html/crs/1080

mv /var/www/swad/asg/LE/4/2/HIAE /var/www/swad/crs/1081
mv /var/www/html/asg/LE/4/2/HIAE /var/www/html/crs/1081

mv /var/www/swad/asg/LE/4/1/MI3AB /var/www/swad/crs/1082
mv /var/www/html/asg/LE/4/1/MI3AB /var/www/html/crs/1082

mv /var/www/swad/asg/LF/0/1/ACF /var/www/swad/crs/1083
mv /var/www/html/asg/LF/0/1/ACF /var/www/html/crs/1083

mv /var/www/swad/asg/LF/0/0/BH /var/www/swad/crs/1084
mv /var/www/html/asg/LF/0/0/BH /var/www/html/crs/1084

mv /var/www/swad/asg/LF/0/1/BA /var/www/swad/crs/1085
mv /var/www/html/asg/LF/0/1/BA /var/www/html/crs/1085

mv /var/www/swad/asg/LF/0/0/DBF /var/www/swad/crs/1086
mv /var/www/html/asg/LF/0/0/DBF /var/www/html/crs/1086

mv /var/www/swad/asg/LF/0/1/FCS1 /var/www/swad/crs/1087
mv /var/www/html/asg/LF/0/1/FCS1 /var/www/html/crs/1087

mv /var/www/swad/asg/LF/0/1/FCS2 /var/www/swad/crs/1088
mv /var/www/html/asg/LF/0/1/FCS2 /var/www/html/crs/1088

mv /var/www/swad/asg/LF/0/1/IGADM /var/www/swad/crs/1089
mv /var/www/html/asg/LF/0/1/IGADM /var/www/html/crs/1089

mv /var/www/swad/asg/LF/0/2/I1ACE /var/www/swad/crs/1090
mv /var/www/html/asg/LF/0/2/I1ACE /var/www/html/crs/1090

mv /var/www/swad/asg/LF/0/1/I2AC /var/www/swad/crs/1091
mv /var/www/html/asg/LF/0/1/I2AC /var/www/html/crs/1091

mv /var/www/swad/asg/LF/0/1/IB /var/www/swad/crs/1092
mv /var/www/html/asg/LF/0/1/IB /var/www/html/crs/1092

mv /var/www/swad/asg/LF/0/2/PFMCF /var/www/swad/crs/1093
mv /var/www/html/asg/LF/0/2/PFMCF /var/www/html/crs/1093

mv /var/www/swad/asg/LF/0/1/PFUE /var/www/swad/crs/1094
mv /var/www/html/asg/LF/0/1/PFUE /var/www/html/crs/1094

mv /var/www/swad/asg/LF/0/1/RAAC /var/www/swad/crs/1095
mv /var/www/html/asg/LF/0/1/RAAC /var/www/html/crs/1095

mv /var/www/swad/asg/LF/1/0/BQE /var/www/swad/crs/1096
mv /var/www/html/asg/LF/1/0/BQE /var/www/html/crs/1096

mv /var/www/swad/asg/LF/1/0/BQD /var/www/swad/crs/1097
mv /var/www/html/asg/LF/1/0/BQD /var/www/html/crs/1097

mv /var/www/swad/asg/LF/1/0/BQB /var/www/swad/crs/1098
mv /var/www/html/asg/LF/1/0/BQB /var/www/html/crs/1098

mv /var/www/swad/asg/LF/1/0/BQA /var/www/swad/crs/1099
mv /var/www/html/asg/LF/1/0/BQA /var/www/html/crs/1099

mv /var/www/swad/asg/LF/1/0/BQF /var/www/swad/crs/1100
mv /var/www/html/asg/LF/1/0/BQF /var/www/html/crs/1100

mv /var/www/swad/asg/LF/1/0/BQC /var/www/swad/crs/1101
mv /var/www/html/asg/LF/1/0/BQC /var/www/html/crs/1101

mv /var/www/swad/asg/LF/1/0/BQT /var/www/swad/crs/1102
mv /var/www/html/asg/LF/1/0/BQT /var/www/html/crs/1102

mv /var/www/swad/asg/LF/1/2/BFAB /var/www/swad/crs/1103
mv /var/www/html/asg/LF/1/2/BFAB /var/www/html/crs/1103

mv /var/www/swad/asg/LF/1/2/BFD /var/www/swad/crs/1104
mv /var/www/html/asg/LF/1/2/BFD /var/www/html/crs/1104

mv /var/www/swad/asg/LF/1/2/BFC /var/www/swad/crs/1105
mv /var/www/html/asg/LF/1/2/BFC /var/www/html/crs/1105

mv /var/www/swad/asg/LF/1/2/BFF /var/www/swad/crs/1106
mv /var/www/html/asg/LF/1/2/BFF /var/www/html/crs/1106

mv /var/www/swad/asg/LF/1/0/FAFA /var/www/swad/crs/1107
mv /var/www/html/asg/LF/1/0/FAFA /var/www/html/crs/1107

mv /var/www/swad/asg/LF/1/0/FAFCF /var/www/swad/crs/1108
mv /var/www/html/asg/LF/1/0/FAFCF /var/www/html/crs/1108

mv /var/www/swad/asg/LF/1/0/FAFE /var/www/swad/crs/1109
mv /var/www/html/asg/LF/1/0/FAFE /var/www/html/crs/1109

mv /var/www/swad/asg/LF/1/1/GAFB /var/www/swad/crs/1110
mv /var/www/html/asg/LF/1/1/GAFB /var/www/html/crs/1110

mv /var/www/swad/asg/LF/1/1/GAFD /var/www/swad/crs/1111
mv /var/www/html/asg/LF/1/1/GAFD /var/www/html/crs/1111

mv /var/www/swad/asg/LF/1/2/HFD /var/www/swad/crs/1112
mv /var/www/html/asg/LF/1/2/HFD /var/www/html/crs/1112

mv /var/www/swad/asg/LF/1/0/MAB /var/www/swad/crs/1113
mv /var/www/html/asg/LF/1/0/MAB /var/www/html/crs/1113

mv /var/www/swad/asg/LF/1/0/MAAC /var/www/swad/crs/1114
mv /var/www/html/asg/LF/1/0/MAAC /var/www/html/crs/1114

mv /var/www/swad/asg/LF/1/0/MAC /var/www/swad/crs/1115
mv /var/www/html/asg/LF/1/0/MAC /var/www/html/crs/1115

mv /var/www/swad/asg/LF/1/0/MAF /var/www/swad/crs/1116
mv /var/www/html/asg/LF/1/0/MAF /var/www/html/crs/1116

mv /var/www/swad/asg/LF/1/0/MAE /var/www/swad/crs/1117
mv /var/www/html/asg/LF/1/0/MAE /var/www/html/crs/1117

mv /var/www/swad/asg/LF/1/0/PMAAA /var/www/swad/crs/1118
mv /var/www/html/asg/LF/1/0/PMAAA /var/www/html/crs/1118

mv /var/www/swad/asg/LF/1/0/QIB /var/www/swad/crs/1119
mv /var/www/html/asg/LF/1/0/QIB /var/www/html/crs/1119

mv /var/www/swad/asg/LF/1/0/QIA /var/www/swad/crs/1120
mv /var/www/html/asg/LF/1/0/QIA /var/www/html/crs/1120

mv /var/www/swad/asg/LF/1/0/QIC /var/www/swad/crs/1121
mv /var/www/html/asg/LF/1/0/QIC /var/www/html/crs/1121

mv /var/www/swad/asg/LF/1/0/QID /var/www/swad/crs/1122
mv /var/www/html/asg/LF/1/0/QID /var/www/html/crs/1122

mv /var/www/swad/asg/LF/1/0/QIE /var/www/swad/crs/1123
mv /var/www/html/asg/LF/1/0/QIE /var/www/html/crs/1123

mv /var/www/swad/asg/LF/1/0/QIF /var/www/swad/crs/1124
mv /var/www/html/asg/LF/1/0/QIF /var/www/html/crs/1124

mv /var/www/swad/asg/LF/2/2/AQBD /var/www/swad/crs/1125
mv /var/www/html/asg/LF/2/2/AQBD /var/www/html/crs/1125

mv /var/www/swad/asg/LF/2/2/AQF /var/www/swad/crs/1126
mv /var/www/html/asg/LF/2/2/AQF /var/www/html/crs/1126

mv /var/www/swad/asg/LF/2/2/AQE /var/www/swad/crs/1127
mv /var/www/html/asg/LF/2/2/AQE /var/www/html/crs/1127

mv /var/www/swad/asg/LF/2/0/FCHBD /var/www/swad/crs/1128
mv /var/www/html/asg/LF/2/0/FCHBD /var/www/html/crs/1128

mv /var/www/swad/asg/LF/2/0/FCHD /var/www/swad/crs/1129
mv /var/www/html/asg/LF/2/0/FCHD /var/www/html/crs/1129

mv /var/www/swad/asg/LF/2/0/FCHE /var/www/swad/crs/1130
mv /var/www/html/asg/LF/2/0/FCHE /var/www/html/crs/1130

mv /var/www/swad/asg/LF/2/2/FVAC /var/www/swad/crs/1131
mv /var/www/html/asg/LF/2/2/FVAC /var/www/html/crs/1131

mv /var/www/swad/asg/LF/2/2/FVB /var/www/swad/crs/1132
mv /var/www/html/asg/LF/2/2/FVB /var/www/html/crs/1132

mv /var/www/swad/asg/LF/2/2/FVD /var/www/swad/crs/1133
mv /var/www/html/asg/LF/2/2/FVD /var/www/html/crs/1133

mv /var/www/swad/asg/LF/2/2/FVE /var/www/swad/crs/1134
mv /var/www/html/asg/LF/2/2/FVE /var/www/html/crs/1134

mv /var/www/swad/asg/LF/2/2/FVF /var/www/swad/crs/1135
mv /var/www/html/asg/LF/2/2/FVF /var/www/html/crs/1135

mv /var/www/swad/asg/LF/2/0/MB /var/www/swad/crs/1136
mv /var/www/html/asg/LF/2/0/MB /var/www/html/crs/1136

mv /var/www/swad/asg/LF/2/0/MA /var/www/swad/crs/1137
mv /var/www/html/asg/LF/2/0/MA /var/www/html/crs/1137

mv /var/www/swad/asg/LF/2/0/MC /var/www/swad/crs/1138
mv /var/www/html/asg/LF/2/0/MC /var/www/html/crs/1138

mv /var/www/swad/asg/LF/2/0/MBC /var/www/swad/crs/1139
mv /var/www/html/asg/LF/2/0/MBC /var/www/html/crs/1139

mv /var/www/swad/asg/LF/2/0/MD /var/www/swad/crs/1140
mv /var/www/html/asg/LF/2/0/MD /var/www/html/crs/1140

mv /var/www/swad/asg/LF/2/0/MBE /var/www/swad/crs/1141
mv /var/www/html/asg/LF/2/0/MBE /var/www/html/crs/1141

mv /var/www/swad/asg/LF/2/0/MF /var/www/swad/crs/1142
mv /var/www/html/asg/LF/2/0/MF /var/www/html/crs/1142

mv /var/www/swad/asg/LF/2/0/MT /var/www/swad/crs/1143
mv /var/www/html/asg/LF/2/0/MT /var/www/html/crs/1143

mv /var/www/swad/asg/LF/2/1/PA /var/www/swad/crs/1144
mv /var/www/html/asg/LF/2/1/PA /var/www/html/crs/1144

mv /var/www/swad/asg/LF/2/1/PB /var/www/swad/crs/1145
mv /var/www/html/asg/LF/2/1/PB /var/www/html/crs/1145

mv /var/www/swad/asg/LF/2/1/PCE /var/www/swad/crs/1146
mv /var/www/html/asg/LF/2/1/PCE /var/www/html/crs/1146

mv /var/www/swad/asg/LF/2/1/PE /var/www/swad/crs/1147
mv /var/www/html/asg/LF/2/1/PE /var/www/html/crs/1147

mv /var/www/swad/asg/LF/2/1/PF /var/www/swad/crs/1148
mv /var/www/html/asg/LF/2/1/PF /var/www/html/crs/1148

mv /var/www/swad/asg/LF/2/0/QOA /var/www/swad/crs/1149
mv /var/www/html/asg/LF/2/0/QOA /var/www/html/crs/1149

mv /var/www/swad/asg/LF/2/0/QOD /var/www/swad/crs/1150
mv /var/www/html/asg/LF/2/0/QOD /var/www/html/crs/1150

mv /var/www/swad/asg/LF/2/1/TID /var/www/swad/crs/1151
mv /var/www/html/asg/LF/2/1/TID /var/www/html/crs/1151

mv /var/www/swad/asg/LF/2/1/TIC /var/www/swad/crs/1152
mv /var/www/html/asg/LF/2/1/TIC /var/www/html/crs/1152

mv /var/www/swad/asg/LF/2/1/TIF /var/www/swad/crs/1153
mv /var/www/html/asg/LF/2/1/TIF /var/www/html/crs/1153

mv /var/www/swad/asg/LF/3/0/ABDLC /var/www/swad/crs/1154
mv /var/www/html/asg/LF/3/0/ABDLC /var/www/html/crs/1154

mv /var/www/swad/asg/LF/3/2/BFA /var/www/swad/crs/1155
mv /var/www/html/asg/LF/3/2/BFA /var/www/html/crs/1155

mv /var/www/swad/asg/LF/3/0/FBD /var/www/swad/crs/1156
mv /var/www/html/asg/LF/3/0/FBD /var/www/html/crs/1156

mv /var/www/swad/asg/LF/3/1/F1A /var/www/swad/crs/1157
mv /var/www/html/asg/LF/3/1/F1A /var/www/html/crs/1157

mv /var/www/swad/asg/LF/3/1/F1B /var/www/swad/crs/1158
mv /var/www/html/asg/LF/3/1/F1B /var/www/html/crs/1158

mv /var/www/swad/asg/LF/3/1/F1D /var/www/swad/crs/1159
mv /var/www/html/asg/LF/3/1/F1D /var/www/html/crs/1159

mv /var/www/swad/asg/LF/3/0/FPD /var/www/swad/crs/1160
mv /var/www/html/asg/LF/3/0/FPD /var/www/html/crs/1160

mv /var/www/swad/asg/LF/3/0/F /var/www/swad/crs/1161
mv /var/www/html/asg/LF/3/0/F /var/www/html/crs/1161

mv /var/www/swad/asg/LF/3/0/FA /var/www/swad/crs/1162
mv /var/www/html/asg/LF/3/0/FA /var/www/html/crs/1162

mv /var/www/swad/asg/LF/3/0/NBC /var/www/swad/crs/1163
mv /var/www/html/asg/LF/3/0/NBC /var/www/html/crs/1163

mv /var/www/swad/asg/LF/3/0/NBF /var/www/swad/crs/1164
mv /var/www/html/asg/LF/3/0/NBF /var/www/html/crs/1164

mv /var/www/swad/asg/LF/3/0/QFB /var/www/swad/crs/1165
mv /var/www/html/asg/LF/3/0/QFB /var/www/html/crs/1165

mv /var/www/swad/asg/LF/3/0/QFE /var/www/swad/crs/1166
mv /var/www/html/asg/LF/3/0/QFE /var/www/html/crs/1166

mv /var/www/swad/asg/LF/3/0/QFF /var/www/swad/crs/1167
mv /var/www/html/asg/LF/3/0/QFF /var/www/html/crs/1167

mv /var/www/swad/asg/LF/3/0/TFGD /var/www/swad/crs/1168
mv /var/www/html/asg/LF/3/0/TFGD /var/www/html/crs/1168

mv /var/www/swad/asg/LF/4/2/AOHPF /var/www/swad/crs/1169
mv /var/www/html/asg/LF/4/2/AOHPF /var/www/html/crs/1169

mv /var/www/swad/asg/LF/4/0/ABDLB /var/www/swad/crs/1170
mv /var/www/html/asg/LF/4/0/ABDLB /var/www/html/crs/1170

mv /var/www/swad/asg/LF/4/2/ABDLP /var/www/swad/crs/1171
mv /var/www/html/asg/LF/4/2/ABDLP /var/www/html/crs/1171

mv /var/www/swad/asg/LF/4/2/AQBF /var/www/swad/crs/1172
mv /var/www/html/asg/LF/4/2/AQBF /var/www/html/crs/1172

mv /var/www/swad/asg/LF/4/1/AFBM /var/www/swad/crs/1173
mv /var/www/html/asg/LF/4/1/AFBM /var/www/html/crs/1173

mv /var/www/swad/asg/LF/4/0/EF /var/www/swad/crs/1174
mv /var/www/html/asg/LF/4/0/EF /var/www/html/crs/1174

mv /var/www/swad/asg/LF/4/2/FP /var/www/swad/crs/1175
mv /var/www/html/asg/LF/4/2/FP /var/www/html/crs/1175

mv /var/www/swad/asg/LF/4/0/F2C /var/www/swad/crs/1176
mv /var/www/html/asg/LF/4/0/F2C /var/www/html/crs/1176

mv /var/www/swad/asg/LF/4/1/I /var/www/swad/crs/1177
mv /var/www/html/asg/LF/4/1/I /var/www/html/crs/1177

mv /var/www/swad/asg/LF/4/2/PMM /var/www/swad/crs/1178
mv /var/www/html/asg/LF/4/2/PMM /var/www/html/crs/1178

mv /var/www/swad/asg/LF/4/1/SOC /var/www/swad/crs/1179
mv /var/www/html/asg/LF/4/1/SOC /var/www/html/crs/1179

mv /var/www/swad/asg/LF/4/0/TFEB /var/www/swad/crs/1180
mv /var/www/html/asg/LF/4/0/TFEB /var/www/html/crs/1180

mv /var/www/swad/asg/LF/5/1/BVAFA /var/www/swad/crs/1181
mv /var/www/html/asg/LF/5/1/BVAFA /var/www/html/crs/1181

mv /var/www/swad/asg/LF/5/1/FCBC /var/www/swad/crs/1182
mv /var/www/html/asg/LF/5/1/FCBC /var/www/html/crs/1182

mv /var/www/swad/asg/LF/5/1/FA /var/www/swad/crs/1183
mv /var/www/html/asg/LF/5/1/FA /var/www/html/crs/1183

mv /var/www/swad/asg/LF/5/1/GCRVIF /var/www/swad/crs/1184
mv /var/www/html/asg/LF/5/1/GCRVIF /var/www/html/crs/1184

mv /var/www/swad/asg/LF/5/1/GPFB /var/www/swad/crs/1185
mv /var/www/html/asg/LF/5/1/GPFB /var/www/html/crs/1185

mv /var/www/swad/asg/LF/5/1/GPFAC /var/www/swad/crs/1186
mv /var/www/html/asg/LF/5/1/GPFAC /var/www/html/crs/1186

mv /var/www/swad/asg/LF/5/1/LDFAC /var/www/swad/crs/1187
mv /var/www/html/asg/LF/5/1/LDFAC /var/www/html/crs/1187

mv /var/www/swad/asg/LF/5/1/LDF /var/www/swad/crs/1188
mv /var/www/html/asg/LF/5/1/LDF /var/www/html/crs/1188

mv /var/www/swad/asg/LF/5/1/PM /var/www/swad/crs/1189
mv /var/www/html/asg/LF/5/1/PM /var/www/html/crs/1189

mv /var/www/swad/asg/LFIL/1/0/SLL /var/www/swad/crs/1190
mv /var/www/html/asg/LFIL/1/0/SLL /var/www/html/crs/1190

mv /var/www/swad/asg/LFIL/2/2/TL /var/www/swad/crs/1191
mv /var/www/html/asg/LFIL/2/2/TL /var/www/html/crs/1191

mv /var/www/swad/asg/LFCL/0/1/EL /var/www/swad/crs/1192
mv /var/www/html/asg/LFCL/0/1/EL /var/www/html/crs/1192

mv /var/www/swad/asg/LFCL/0/1/EMG1 /var/www/swad/crs/1193
mv /var/www/html/asg/LFCL/0/1/EMG1 /var/www/html/crs/1193

mv /var/www/swad/asg/LFCL/0/1/EMG2 /var/www/swad/crs/1194
mv /var/www/html/asg/LFCL/0/1/EMG2 /var/www/html/crs/1194

mv /var/www/swad/asg/LFCL/0/2/MGAP /var/www/swad/crs/1195
mv /var/www/html/asg/LFCL/0/2/MGAP /var/www/html/crs/1195

mv /var/www/swad/asg/LFCL/4/1/HCC1 /var/www/swad/crs/1196
mv /var/www/html/asg/LFCL/4/1/HCC1 /var/www/html/crs/1196

mv /var/www/swad/asg/LFCL/5/2/HCC2 /var/www/swad/crs/1197
mv /var/www/html/asg/LFCL/5/2/HCC2 /var/www/html/crs/1197

mv /var/www/swad/asg/LFES/0/0/GCLE /var/www/swad/crs/1198
mv /var/www/html/asg/LFES/0/0/GCLE /var/www/html/crs/1198

mv /var/www/swad/asg/LFES/5/0/HCPE /var/www/swad/crs/1199
mv /var/www/html/asg/LFES/5/0/HCPE /var/www/html/crs/1199

mv /var/www/swad/asg/LFFR/0/1/EF1 /var/www/swad/crs/1200
mv /var/www/html/asg/LFFR/0/1/EF1 /var/www/html/crs/1200

mv /var/www/swad/asg/LFFR/0/2/EF2 /var/www/swad/crs/1201
mv /var/www/html/asg/LFFR/0/2/EF2 /var/www/html/crs/1201

mv /var/www/swad/asg/LFFR/0/0/ELF /var/www/swad/crs/1202
mv /var/www/html/asg/LFFR/0/0/ELF /var/www/html/crs/1202

mv /var/www/swad/asg/LFFR/3/2/IIAF /var/www/swad/crs/1203
mv /var/www/html/asg/LFFR/3/2/IIAF /var/www/html/crs/1203

mv /var/www/swad/asg/LFFR/4/0/HLF /var/www/swad/crs/1204
mv /var/www/html/asg/LFFR/4/0/HLF /var/www/html/crs/1204

mv /var/www/swad/asg/LFHI/0/2/FSBM /var/www/swad/crs/1205
mv /var/www/html/asg/LFHI/0/2/FSBM /var/www/html/crs/1205

mv /var/www/swad/asg/LFHI/0/1/HELE /var/www/swad/crs/1206
mv /var/www/html/asg/LFHI/0/1/HELE /var/www/html/crs/1206

mv /var/www/swad/asg/LFHI/0/1/LESO /var/www/swad/crs/1207
mv /var/www/html/asg/LFHI/0/1/LESO /var/www/html/crs/1207

mv /var/www/swad/asg/LFHI/0/2/LSXIXH /var/www/swad/crs/1208
mv /var/www/html/asg/LFHI/0/2/LSXIXH /var/www/html/crs/1208

mv /var/www/swad/asg/LFHI/0/1/NHSXX /var/www/swad/crs/1209
mv /var/www/html/asg/LFHI/0/1/NHSXX /var/www/html/crs/1209

mv /var/www/swad/asg/LFHI/1/0/MSEB /var/www/swad/crs/1210
mv /var/www/html/asg/LFHI/1/0/MSEB /var/www/html/crs/1210

mv /var/www/swad/asg/LFHI/1/2/NC /var/www/swad/crs/1211
mv /var/www/html/asg/LFHI/1/2/NC /var/www/html/crs/1211

mv /var/www/swad/asg/LFHI/2/0/LSLEB /var/www/swad/crs/1212
mv /var/www/html/asg/LFHI/2/0/LSLEB /var/www/html/crs/1212

mv /var/www/swad/asg/LFHI/2/0/LE2B /var/www/swad/crs/1213
mv /var/www/html/asg/LFHI/2/0/LE2B /var/www/html/crs/1213

mv /var/www/swad/asg/LFHI/2/2/TLB /var/www/swad/crs/1214
mv /var/www/html/asg/LFHI/2/2/TLB /var/www/html/crs/1214

mv /var/www/swad/asg/LFHI/4/2/DHA /var/www/swad/crs/1215
mv /var/www/html/asg/LFHI/4/2/DHA /var/www/html/crs/1215

mv /var/www/swad/asg/LFHI/4/2/DEB /var/www/swad/crs/1216
mv /var/www/html/asg/LFHI/4/2/DEB /var/www/html/crs/1216

mv /var/www/swad/asg/LFIN/0/0/CLALLI /var/www/swad/crs/1217
mv /var/www/html/asg/LFIN/0/0/CLALLI /var/www/html/crs/1217

mv /var/www/swad/asg/LFIN/0/2/ESLEU /var/www/swad/crs/1218
mv /var/www/html/asg/LFIN/0/2/ESLEU /var/www/html/crs/1218

mv /var/www/swad/asg/LFIN/0/1/IFE1 /var/www/swad/crs/1219
mv /var/www/html/asg/LFIN/0/1/IFE1 /var/www/html/crs/1219

mv /var/www/swad/asg/LFIN/0/2/IFE2 /var/www/swad/crs/1220
mv /var/www/html/asg/LFIN/0/2/IFE2 /var/www/html/crs/1220

mv /var/www/swad/asg/LFIN/0/2/LCIE /var/www/swad/crs/1221
mv /var/www/html/asg/LFIN/0/2/LCIE /var/www/html/crs/1221

mv /var/www/swad/asg/LFIN/0/1/MEI2A /var/www/swad/crs/1222
mv /var/www/html/asg/LFIN/0/1/MEI2A /var/www/html/crs/1222

mv /var/www/swad/asg/LFIN/0/1/MEI4A /var/www/swad/crs/1223
mv /var/www/html/asg/LFIN/0/1/MEI4A /var/www/html/crs/1223

mv /var/www/swad/asg/LFIN/0/2/PI /var/www/swad/crs/1224
mv /var/www/html/asg/LFIN/0/2/PI /var/www/html/crs/1224

mv /var/www/swad/asg/LFIN/0/2/RF /var/www/swad/crs/1225
mv /var/www/html/asg/LFIN/0/2/RF /var/www/html/crs/1225

mv /var/www/swad/asg/LFIN/0/1/TDLI1 /var/www/swad/crs/1226
mv /var/www/html/asg/LFIN/0/1/TDLI1 /var/www/html/crs/1226

mv /var/www/swad/asg/LFIN/0/2/TDLI2 /var/www/swad/crs/1227
mv /var/www/html/asg/LFIN/0/2/TDLI2 /var/www/html/crs/1227

mv /var/www/swad/asg/LFIN/1/0/II1C /var/www/swad/crs/1228
mv /var/www/html/asg/LFIN/1/0/II1C /var/www/html/crs/1228

mv /var/www/swad/asg/LFIN/1/0/II2 /var/www/swad/crs/1229
mv /var/www/html/asg/LFIN/1/0/II2 /var/www/html/crs/1229

mv /var/www/swad/asg/LFIN/1/0/II2C /var/www/swad/crs/1230
mv /var/www/html/asg/LFIN/1/0/II2C /var/www/html/crs/1230

mv /var/www/swad/asg/LFIN/1/0/LEA /var/www/swad/crs/1231
mv /var/www/html/asg/LFIN/1/0/LEA /var/www/html/crs/1231

mv /var/www/swad/asg/LFIN/1/0/LEB /var/www/swad/crs/1232
mv /var/www/html/asg/LFIN/1/0/LEB /var/www/html/crs/1232

mv /var/www/swad/asg/LFIN/1/0/LI1 /var/www/swad/crs/1233
mv /var/www/html/asg/LFIN/1/0/LI1 /var/www/html/crs/1233

mv /var/www/swad/asg/LFIN/1/1/TELIAB /var/www/swad/crs/1234
mv /var/www/html/asg/LFIN/1/1/TELIAB /var/www/html/crs/1234

mv /var/www/swad/asg/LFIN/1/1/TELIC /var/www/swad/crs/1235
mv /var/www/html/asg/LFIN/1/1/TELIC /var/www/html/crs/1235

mv /var/www/swad/asg/LFIN/2/1/II3AB /var/www/swad/crs/1236
mv /var/www/html/asg/LFIN/2/1/II3AB /var/www/html/crs/1236

mv /var/www/swad/asg/LFIN/2/0/II4AB /var/www/swad/crs/1237
mv /var/www/html/asg/LFIN/2/0/II4AB /var/www/html/crs/1237

mv /var/www/swad/asg/LFIN/2/0/II4C /var/www/swad/crs/1238
mv /var/www/html/asg/LFIN/2/0/II4C /var/www/html/crs/1238

mv /var/www/swad/asg/LFIN/2/1/LAEIAB /var/www/swad/crs/1239
mv /var/www/html/asg/LFIN/2/1/LAEIAB /var/www/html/crs/1239

mv /var/www/swad/asg/LFIN/2/0/LI1 /var/www/swad/crs/1240
mv /var/www/html/asg/LFIN/2/0/LI1 /var/www/html/crs/1240

mv /var/www/swad/asg/LFIN/2/1/LI1BC /var/www/swad/crs/1241
mv /var/www/html/asg/LFIN/2/1/LI1BC /var/www/html/crs/1241

mv /var/www/swad/asg/LFIN/2/0/LI1C /var/www/swad/crs/1242
mv /var/www/html/asg/LFIN/2/0/LI1C /var/www/html/crs/1242

mv /var/www/swad/asg/LFIN/2/2/TLA /var/www/swad/crs/1243
mv /var/www/html/asg/LFIN/2/2/TLA /var/www/html/crs/1243

mv /var/www/swad/asg/LFIN/3/0/MGI /var/www/swad/crs/1244
mv /var/www/html/asg/LFIN/3/0/MGI /var/www/html/crs/1244

mv /var/www/swad/asg/LFIN/4/0/HLI /var/www/swad/crs/1245
mv /var/www/html/asg/LFIN/4/0/HLI /var/www/html/crs/1245

mv /var/www/swad/asg/LFIN/5/0/GI /var/www/swad/crs/1246
mv /var/www/html/asg/LFIN/5/0/GI /var/www/html/crs/1246

mv /var/www/swad/asg/LFIN/5/0/LI2B /var/www/swad/crs/1247
mv /var/www/html/asg/LFIN/5/0/LI2B /var/www/html/crs/1247

mv /var/www/swad/asg/LFI/0/1/FP /var/www/swad/crs/1248
mv /var/www/html/asg/LFI/0/1/FP /var/www/html/crs/1248

mv /var/www/swad/asg/LFI/0/0/IRFM /var/www/swad/crs/1249
mv /var/www/html/asg/LFI/0/0/IRFM /var/www/html/crs/1249

mv /var/www/swad/asg/LFI/0/0/TMCF /var/www/swad/crs/1250
mv /var/www/html/asg/LFI/0/0/TMCF /var/www/html/crs/1250

mv /var/www/swad/asg/LFI/1/2/RAAC /var/www/swad/crs/1251
mv /var/www/html/asg/LFI/1/2/RAAC /var/www/html/crs/1251

mv /var/www/swad/asg/LFI/1/2/RAB /var/www/swad/crs/1252
mv /var/www/html/asg/LFI/1/2/RAB /var/www/html/crs/1252

mv /var/www/swad/asg/LFI/2/1/FM1A /var/www/swad/crs/1253
mv /var/www/html/asg/LFI/2/1/FM1A /var/www/html/crs/1253

mv /var/www/swad/asg/LFI/2/1/FC2 /var/www/swad/crs/1254
mv /var/www/html/asg/LFI/2/1/FC2 /var/www/html/crs/1254

mv /var/www/swad/asg/LFI/2/2/IA /var/www/swad/crs/1255
mv /var/www/html/asg/LFI/2/2/IA /var/www/html/crs/1255

mv /var/www/swad/asg/LFI/2/2/IMNFA /var/www/swad/crs/1256
mv /var/www/html/asg/LFI/2/2/IMNFA /var/www/html/crs/1256

mv /var/www/swad/asg/LFI/2/0/MMF4 /var/www/swad/crs/1257
mv /var/www/html/asg/LFI/2/0/MMF4 /var/www/html/crs/1257

mv /var/www/swad/asg/LFI/2/0/MOC /var/www/swad/crs/1258
mv /var/www/html/asg/LFI/2/0/MOC /var/www/html/crs/1258

mv /var/www/swad/asg/LFI/2/0/TEF1MT /var/www/swad/crs/1259
mv /var/www/html/asg/LFI/2/0/TEF1MT /var/www/html/crs/1259

mv /var/www/swad/asg/LFI/3/1/FMA /var/www/swad/crs/1260
mv /var/www/html/asg/LFI/3/1/FMA /var/www/html/crs/1260

mv /var/www/swad/asg/LFI/3/0/O /var/www/swad/crs/1261
mv /var/www/html/asg/LFI/3/0/O /var/www/html/crs/1261

mv /var/www/swad/asg/LFI/3/0/OB /var/www/swad/crs/1262
mv /var/www/html/asg/LFI/3/0/OB /var/www/html/crs/1262

mv /var/www/swad/asg/LFI/4/0/FE /var/www/swad/crs/1263
mv /var/www/html/asg/LFI/4/0/FE /var/www/html/crs/1263

mv /var/www/swad/asg/LFI/4/1/FPOI /var/www/swad/crs/1264
mv /var/www/html/asg/LFI/4/1/FPOI /var/www/html/crs/1264

mv /var/www/swad/asg/LFI/5/1/AG /var/www/swad/crs/1265
mv /var/www/html/asg/LFI/5/1/AG /var/www/html/crs/1265

mv /var/www/swad/asg/LFI/5/2/EN /var/www/swad/crs/1266
mv /var/www/html/asg/LFI/5/2/EN /var/www/html/crs/1266

mv /var/www/swad/asg/LFI/5/2/FP /var/www/swad/crs/1267
mv /var/www/html/asg/LFI/5/2/FP /var/www/html/crs/1267

mv /var/www/swad/asg/LFI/5/2/FVT /var/www/swad/crs/1268
mv /var/www/html/asg/LFI/5/2/FVT /var/www/html/crs/1268

mv /var/www/swad/asg/LFI/5/2/FVP /var/www/swad/crs/1269
mv /var/www/html/asg/LFI/5/2/FVP /var/www/html/crs/1269

mv /var/www/swad/asg/LFI/5/1/MC2 /var/www/swad/crs/1270
mv /var/www/html/asg/LFI/5/1/MC2 /var/www/html/crs/1270

mv /var/www/swad/asg/LGG/0/2/DIRR /var/www/swad/crs/1271
mv /var/www/html/asg/LGG/0/2/DIRR /var/www/html/crs/1271

mv /var/www/swad/asg/LGG/0/2/HEMC /var/www/swad/crs/1272
mv /var/www/html/asg/LGG/0/2/HEMC /var/www/html/crs/1272

mv /var/www/swad/asg/LGG/1/0/TGCFA /var/www/swad/crs/1273
mv /var/www/html/asg/LGG/1/0/TGCFA /var/www/html/crs/1273

mv /var/www/swad/asg/LGG/5/0/OT /var/www/swad/crs/1274
mv /var/www/html/asg/LGG/5/0/OT /var/www/html/crs/1274

mv /var/www/swad/asg/LG/0/1/GC /var/www/swad/crs/1275
mv /var/www/html/asg/LG/0/1/GC /var/www/html/crs/1275

mv /var/www/swad/asg/LG/0/1/PG /var/www/swad/crs/1276
mv /var/www/html/asg/LG/0/1/PG /var/www/html/crs/1276

mv /var/www/swad/asg/LG/0/0/PE /var/www/swad/crs/1277
mv /var/www/html/asg/LG/0/0/PE /var/www/html/crs/1277

mv /var/www/swad/asg/LG/0/1/TPM /var/www/swad/crs/1278
mv /var/www/html/asg/LG/0/1/TPM /var/www/html/crs/1278

mv /var/www/swad/asg/LG/1/2/EA /var/www/swad/crs/1279
mv /var/www/html/asg/LG/1/2/EA /var/www/html/crs/1279

mv /var/www/swad/asg/LG/1/2/EB /var/www/swad/crs/1280
mv /var/www/html/asg/LG/1/2/EB /var/www/html/crs/1280

mv /var/www/swad/asg/LG/2/2/M2A /var/www/swad/crs/1281
mv /var/www/html/asg/LG/2/2/M2A /var/www/html/crs/1281

mv /var/www/swad/asg/LG/2/2/M2B /var/www/swad/crs/1282
mv /var/www/html/asg/LG/2/2/M2B /var/www/html/crs/1282

mv /var/www/swad/asg/LG/2/2/P2 /var/www/swad/crs/1283
mv /var/www/html/asg/LG/2/2/P2 /var/www/html/crs/1283

mv /var/www/swad/asg/LG/4/1/AE /var/www/swad/crs/1284
mv /var/www/html/asg/LG/4/1/AE /var/www/html/crs/1284

mv /var/www/swad/asg/LG/4/0/GR /var/www/swad/crs/1285
mv /var/www/html/asg/LG/4/0/GR /var/www/html/crs/1285

mv /var/www/swad/asg/LG/4/1/PI /var/www/swad/crs/1286
mv /var/www/html/asg/LG/4/1/PI /var/www/html/crs/1286

mv /var/www/swad/asg/LG/5/1/OE /var/www/swad/crs/1287
mv /var/www/html/asg/LG/5/1/OE /var/www/html/crs/1287

mv /var/www/swad/asg/LG/5/2/RE /var/www/swad/crs/1288
mv /var/www/html/asg/LG/5/2/RE /var/www/html/crs/1288

mv /var/www/swad/asg/LG/5/2/V /var/www/swad/crs/1289
mv /var/www/html/asg/LG/5/2/V /var/www/html/crs/1289

mv /var/www/swad/asg/LH/0/1/AT /var/www/swad/crs/1290
mv /var/www/html/asg/LH/0/1/AT /var/www/html/crs/1290

mv /var/www/swad/asg/LH/0/2/AA /var/www/swad/crs/1291
mv /var/www/html/asg/LH/0/2/AA /var/www/html/crs/1291

mv /var/www/swad/asg/LH/0/2/AI /var/www/swad/crs/1292
mv /var/www/html/asg/LH/0/2/AI /var/www/html/crs/1292

mv /var/www/swad/asg/LH/0/1/CMA /var/www/swad/crs/1293
mv /var/www/html/asg/LH/0/1/CMA /var/www/html/crs/1293

mv /var/www/swad/asg/LH/0/2/CPA /var/www/swad/crs/1294
mv /var/www/html/asg/LH/0/2/CPA /var/www/html/crs/1294

mv /var/www/swad/asg/LH/0/1/EN /var/www/swad/crs/1295
mv /var/www/html/asg/LH/0/1/EN /var/www/html/crs/1295

mv /var/www/swad/asg/LH/0/0/HEPOA /var/www/swad/crs/1296
mv /var/www/html/asg/LH/0/0/HEPOA /var/www/html/crs/1296

mv /var/www/swad/asg/LH/0/1/HG /var/www/swad/crs/1297
mv /var/www/html/asg/LH/0/1/HG /var/www/html/crs/1297

mv /var/www/swad/asg/LH/0/1/HSVCEM /var/www/swad/crs/1298
mv /var/www/html/asg/LH/0/1/HSVCEM /var/www/html/crs/1298

mv /var/www/swad/asg/LH/0/0/HMA /var/www/swad/crs/1299
mv /var/www/html/asg/LH/0/0/HMA /var/www/html/crs/1299

mv /var/www/swad/asg/LH/0/1/HM /var/www/swad/crs/1300
mv /var/www/html/asg/LH/0/1/HM /var/www/html/crs/1300

mv /var/www/swad/asg/LH/0/2/HMM /var/www/swad/crs/1301
mv /var/www/html/asg/LH/0/2/HMM /var/www/html/crs/1301

mv /var/www/swad/asg/LH/0/0/HEU /var/www/swad/crs/1302
mv /var/www/html/asg/LH/0/0/HEU /var/www/html/crs/1302

mv /var/www/swad/asg/LH/0/1/HR /var/www/swad/crs/1303
mv /var/www/html/asg/LH/0/1/HR /var/www/html/crs/1303

mv /var/www/swad/asg/LH/0/0/IEJE /var/www/swad/crs/1304
mv /var/www/html/asg/LH/0/0/IEJE /var/www/html/crs/1304

mv /var/www/swad/asg/LH/0/2/JC /var/www/swad/crs/1305
mv /var/www/html/asg/LH/0/2/JC /var/www/html/crs/1305

mv /var/www/swad/asg/LH/0/1/OPEM /var/www/swad/crs/1306
mv /var/www/html/asg/LH/0/1/OPEM /var/www/html/crs/1306

mv /var/www/swad/asg/LH/1/0/HAUA /var/www/swad/crs/1307
mv /var/www/html/asg/LH/1/0/HAUA /var/www/html/crs/1307

mv /var/www/swad/asg/LH/1/0/HAUB /var/www/swad/crs/1308
mv /var/www/html/asg/LH/1/0/HAUB /var/www/html/crs/1308

mv /var/www/swad/asg/LH/1/1/HEAB /var/www/swad/crs/1309
mv /var/www/html/asg/LH/1/1/HEAB /var/www/html/crs/1309

mv /var/www/swad/asg/LH/1/1/HEAA /var/www/swad/crs/1310
mv /var/www/html/asg/LH/1/1/HEAA /var/www/html/crs/1310

mv /var/www/swad/asg/LH/1/1/P1A /var/www/swad/crs/1311
mv /var/www/html/asg/LH/1/1/P1A /var/www/html/crs/1311

mv /var/www/swad/asg/LH/1/2/P2A /var/www/swad/crs/1312
mv /var/www/html/asg/LH/1/2/P2A /var/www/html/crs/1312

mv /var/www/swad/asg/LH/2/0/HCE /var/www/swad/crs/1313
mv /var/www/html/asg/LH/2/0/HCE /var/www/html/crs/1313

mv /var/www/swad/asg/LH/4/2/HCT /var/www/swad/crs/1314
mv /var/www/html/asg/LH/4/2/HCT /var/www/html/crs/1314

mv /var/www/swad/asg/LH/4/1/HEA /var/www/swad/crs/1315
mv /var/www/html/asg/LH/4/1/HEA /var/www/html/crs/1315

mv /var/www/swad/asg/LH/4/0/THA /var/www/swad/crs/1316
mv /var/www/html/asg/LH/4/0/THA /var/www/html/crs/1316

mv /var/www/swad/asg/LHA/0/1/CEA /var/www/swad/crs/1317
mv /var/www/html/asg/LHA/0/1/CEA /var/www/html/crs/1317

mv /var/www/swad/asg/LHA/0/2/HC /var/www/swad/crs/1318
mv /var/www/html/asg/LHA/0/2/HC /var/www/html/crs/1318

mv /var/www/swad/asg/LHA/0/1/IMT /var/www/swad/crs/1319
mv /var/www/html/asg/LHA/0/1/IMT /var/www/html/crs/1319

mv /var/www/swad/asg/LHA/1/1/HAM /var/www/swad/crs/1320
mv /var/www/html/asg/LHA/1/1/HAM /var/www/html/crs/1320

mv /var/www/swad/asg/LHA/2/0/TACBCA /var/www/swad/crs/1321
mv /var/www/html/asg/LHA/2/0/TACBCA /var/www/html/crs/1321

mv /var/www/swad/asg/LHA/2/0/TACBCE /var/www/swad/crs/1322
mv /var/www/html/asg/LHA/2/0/TACBCE /var/www/html/crs/1322

mv /var/www/swad/asg/LHA/3/1/CA1 /var/www/swad/crs/1323
mv /var/www/html/asg/LHA/3/1/CA1 /var/www/html/crs/1323

mv /var/www/swad/asg/LHA/3/2/CA2 /var/www/swad/crs/1324
mv /var/www/html/asg/LHA/3/2/CA2 /var/www/html/crs/1324

mv /var/www/swad/asg/LHCM/0/2/FMM /var/www/swad/crs/1325
mv /var/www/html/asg/LHCM/0/2/FMM /var/www/html/crs/1325

mv /var/www/swad/asg/LHCM/5/0/EM /var/www/swad/crs/1326
mv /var/www/html/asg/LHCM/5/0/EM /var/www/html/crs/1326

mv /var/www/swad/asg/LITM/0/1/MARKI /var/www/swad/crs/1327
mv /var/www/html/asg/LITM/0/1/MARKI /var/www/html/crs/1327

mv /var/www/swad/asg/LITM/1/1/IM1 /var/www/swad/crs/1328
mv /var/www/html/asg/LITM/1/1/IM1 /var/www/html/crs/1328

mv /var/www/swad/asg/LITM/1/2/IM2 /var/www/swad/crs/1329
mv /var/www/html/asg/LITM/1/2/IM2 /var/www/html/crs/1329

mv /var/www/swad/asg/LITM/1/1/PC /var/www/swad/crs/1330
mv /var/www/html/asg/LITM/1/1/PC /var/www/html/crs/1330

mv /var/www/swad/asg/LMA/1/0/ABC /var/www/swad/crs/1331
mv /var/www/html/asg/LMA/1/0/ABC /var/www/html/crs/1331

mv /var/www/swad/asg/LMA/3/0/AEB /var/www/swad/crs/1332
mv /var/www/html/asg/LMA/3/0/AEB /var/www/html/crs/1332

mv /var/www/swad/asg/LMA/3/0/EDB /var/www/swad/crs/1333
mv /var/www/html/asg/LMA/3/0/EDB /var/www/html/crs/1333

mv /var/www/swad/asg/LMA/5/1/MV /var/www/swad/crs/1334
mv /var/www/html/asg/LMA/5/1/MV /var/www/html/crs/1334

mv /var/www/swad/asg/LMA/5/1/PVEF /var/www/swad/crs/1335
mv /var/www/html/asg/LMA/5/1/PVEF /var/www/html/crs/1335

mv /var/www/swad/asg/LM/0/2/AEODM /var/www/swad/crs/1336
mv /var/www/html/asg/LM/0/2/AEODM /var/www/html/crs/1336

mv /var/www/swad/asg/LM/0/1/EPSH /var/www/swad/crs/1337
mv /var/www/html/asg/LM/0/1/EPSH /var/www/html/crs/1337

mv /var/www/swad/asg/LM/0/2/HEM /var/www/swad/crs/1338
mv /var/www/html/asg/LM/0/2/HEM /var/www/html/crs/1338

mv /var/www/swad/asg/LM/0/1/IFE /var/www/swad/crs/1339
mv /var/www/html/asg/LM/0/1/IFE /var/www/html/crs/1339

mv /var/www/swad/asg/LM/2/1/BPAB /var/www/swad/crs/1340
mv /var/www/html/asg/LM/2/1/BPAB /var/www/html/crs/1340

mv /var/www/swad/asg/LM/2/0/AH2 /var/www/swad/crs/1341
mv /var/www/html/asg/LM/2/0/AH2 /var/www/html/crs/1341

mv /var/www/swad/asg/LM/2/1/HM /var/www/swad/crs/1342
mv /var/www/html/asg/LM/2/1/HM /var/www/html/crs/1342

mv /var/www/swad/asg/LM/2/1/HMA /var/www/swad/crs/1343
mv /var/www/html/asg/LM/2/1/HMA /var/www/html/crs/1343

mv /var/www/swad/asg/LM/2/1/HMB /var/www/swad/crs/1344
mv /var/www/html/asg/LM/2/1/HMB /var/www/html/crs/1344

mv /var/www/swad/asg/LM/2/2/HM1 /var/www/swad/crs/1345
mv /var/www/html/asg/LM/2/2/HM1 /var/www/html/crs/1345

mv /var/www/swad/asg/LM/2/2/HM2 /var/www/swad/crs/1346
mv /var/www/html/asg/LM/2/2/HM2 /var/www/html/crs/1346

mv /var/www/swad/asg/LM/3/1/MPM /var/www/swad/crs/1347
mv /var/www/html/asg/LM/3/1/MPM /var/www/html/crs/1347

mv /var/www/swad/asg/LM/5/0/PQ2 /var/www/swad/crs/1348
mv /var/www/html/asg/LM/5/0/PQ2 /var/www/html/crs/1348

mv /var/www/swad/asg/LM/6/0/MPSPC /var/www/swad/crs/1349
mv /var/www/html/asg/LM/6/0/MPSPC /var/www/html/crs/1349

mv /var/www/swad/asg/LM/6/0/PM3PCN /var/www/swad/crs/1350
mv /var/www/html/asg/LM/6/0/PM3PCN /var/www/html/crs/1350

mv /var/www/swad/asg/LO/1/2/HOMCO /var/www/swad/crs/1351
mv /var/www/html/asg/LO/1/2/HOMCO /var/www/html/crs/1351

mv /var/www/swad/asg/LPED/0/2/MEPMC /var/www/swad/crs/1352
mv /var/www/html/asg/LPED/0/2/MEPMC /var/www/html/crs/1352

mv /var/www/swad/asg/LPED/1/0/BMIE /var/www/swad/crs/1353
mv /var/www/html/asg/LPED/1/0/BMIE /var/www/html/crs/1353

mv /var/www/swad/asg/LPED/1/0/HE /var/www/swad/crs/1354
mv /var/www/html/asg/LPED/1/0/HE /var/www/html/crs/1354

mv /var/www/swad/asg/LPED/1/1/TC /var/www/swad/crs/1355
mv /var/www/html/asg/LPED/1/1/TC /var/www/html/crs/1355

mv /var/www/swad/asg/LPED/1/0/TE /var/www/swad/crs/1356
mv /var/www/html/asg/LPED/1/0/TE /var/www/html/crs/1356

mv /var/www/swad/asg/LPED/2/0/DG /var/www/swad/crs/1357
mv /var/www/html/asg/LPED/2/0/DG /var/www/html/crs/1357

mv /var/www/swad/asg/LPED/2/0/OGCE /var/www/swad/crs/1358
mv /var/www/html/asg/LPED/2/0/OGCE /var/www/html/crs/1358

mv /var/www/swad/asg/LPED/2/0/SEA /var/www/swad/crs/1359
mv /var/www/html/asg/LPED/2/0/SEA /var/www/html/crs/1359

mv /var/www/swad/asg/LPED/2/0/SE /var/www/swad/crs/1360
mv /var/www/html/asg/LPED/2/0/SE /var/www/html/crs/1360

mv /var/www/swad/asg/LPED/2/1/TEC /var/www/swad/crs/1361
mv /var/www/html/asg/LPED/2/1/TEC /var/www/html/crs/1361

mv /var/www/swad/asg/LPED/3/2/EMC /var/www/swad/crs/1362
mv /var/www/html/asg/LPED/3/2/EMC /var/www/html/crs/1362

mv /var/www/swad/asg/LPED/3/2/FAEE /var/www/swad/crs/1363
mv /var/www/html/asg/LPED/3/2/FAEE /var/www/html/crs/1363

mv /var/www/swad/asg/LPED/3/1/OET /var/www/swad/crs/1364
mv /var/www/html/asg/LPED/3/1/OET /var/www/html/crs/1364

mv /var/www/swad/asg/LPED/4/1/ACM /var/www/swad/crs/1365
mv /var/www/html/asg/LPED/4/1/ACM /var/www/html/crs/1365

mv /var/www/swad/asg/LPED/4/2/DEE /var/www/swad/crs/1366
mv /var/www/html/asg/LPED/4/2/DEE /var/www/html/crs/1366

mv /var/www/swad/asg/LPED/4/1/DDIC /var/www/swad/crs/1367
mv /var/www/html/asg/LPED/4/1/DDIC /var/www/html/crs/1367

mv /var/www/swad/asg/LPED/4/2/EE /var/www/swad/crs/1368
mv /var/www/html/asg/LPED/4/2/EE /var/www/html/crs/1368

mv /var/www/swad/asg/LPED/4/1/PSC /var/www/swad/crs/1369
mv /var/www/html/asg/LPED/4/1/PSC /var/www/html/crs/1369

mv /var/www/swad/asg/LPED/4/2/POOE /var/www/swad/crs/1370
mv /var/www/html/asg/LPED/4/2/POOE /var/www/html/crs/1370

mv /var/www/swad/asg/LPED/5/2/DPDA /var/www/swad/crs/1371
mv /var/www/html/asg/LPED/5/2/DPDA /var/www/html/crs/1371

mv /var/www/swad/asg/LPED/5/2/DPDB /var/www/swad/crs/1372
mv /var/www/html/asg/LPED/5/2/DPDB /var/www/html/crs/1372

mv /var/www/swad/asg/LPED/5/2/DPDC /var/www/swad/crs/1373
mv /var/www/html/asg/LPED/5/2/DPDC /var/www/html/crs/1373

mv /var/www/swad/asg/LPED/5/2/EFDP /var/www/swad/crs/1374
mv /var/www/html/asg/LPED/5/2/EFDP /var/www/html/crs/1374

mv /var/www/swad/asg/LPED/5/2/EPCP /var/www/swad/crs/1375
mv /var/www/html/asg/LPED/5/2/EPCP /var/www/html/crs/1375

mv /var/www/swad/asg/LPED/5/1/EGI /var/www/swad/crs/1376
mv /var/www/html/asg/LPED/5/1/EGI /var/www/html/crs/1376

mv /var/www/swad/asg/LPED/5/1/FAFPAB /var/www/swad/crs/1377
mv /var/www/html/asg/LPED/5/1/FAFPAB /var/www/html/crs/1377

mv /var/www/swad/asg/LPED/5/2/PFE /var/www/swad/crs/1378
mv /var/www/html/asg/LPED/5/2/PFE /var/www/html/crs/1378

mv /var/www/swad/asg/LPED/5/2/P2 /var/www/swad/crs/1379
mv /var/www/html/asg/LPED/5/2/P2 /var/www/html/crs/1379

mv /var/www/swad/asg/LPED/5/2/SMCMSE /var/www/swad/crs/1380
mv /var/www/html/asg/LPED/5/2/SMCMSE /var/www/html/crs/1380

mv /var/www/swad/asg/LPSP/0/1/ESEA /var/www/swad/crs/1381
mv /var/www/html/asg/LPSP/0/1/ESEA /var/www/html/crs/1381

mv /var/www/swad/asg/LPSP/0/2/OEE /var/www/swad/crs/1382
mv /var/www/html/asg/LPSP/0/2/OEE /var/www/html/crs/1382

mv /var/www/swad/asg/LPSP/1/2/ACA /var/www/swad/crs/1383
mv /var/www/html/asg/LPSP/1/2/ACA /var/www/html/crs/1383

mv /var/www/swad/asg/LPSP/1/2/DPP /var/www/swad/crs/1384
mv /var/www/html/asg/LPSP/1/2/DPP /var/www/html/crs/1384

mv /var/www/swad/asg/LPSP/1/1/DE /var/www/swad/crs/1385
mv /var/www/html/asg/LPSP/1/1/DE /var/www/html/crs/1385

mv /var/www/swad/asg/LPSP/1/2/DAIP /var/www/swad/crs/1386
mv /var/www/html/asg/LPSP/1/2/DAIP /var/www/html/crs/1386

mv /var/www/swad/asg/LPSP/1/2/DDIC /var/www/swad/crs/1387
mv /var/www/html/asg/LPSP/1/2/DDIC /var/www/html/crs/1387

mv /var/www/swad/asg/LPSP/1/2/EEA /var/www/swad/crs/1388
mv /var/www/html/asg/LPSP/1/2/EEA /var/www/html/crs/1388

mv /var/www/swad/asg/LPSP/1/2/EED /var/www/swad/crs/1389
mv /var/www/html/asg/LPSP/1/2/EED /var/www/html/crs/1389

mv /var/www/swad/asg/LPSP/1/1/ESE /var/www/swad/crs/1390
mv /var/www/html/asg/LPSP/1/1/ESE /var/www/html/crs/1390

mv /var/www/swad/asg/LPSP/1/2/EPCE /var/www/swad/crs/1391
mv /var/www/html/asg/LPSP/1/2/EPCE /var/www/html/crs/1391

mv /var/www/swad/asg/LPSP/1/1/MOE /var/www/swad/crs/1392
mv /var/www/html/asg/LPSP/1/1/MOE /var/www/html/crs/1392

mv /var/www/swad/asg/LPSP/1/1/MIE /var/www/swad/crs/1393
mv /var/www/html/asg/LPSP/1/1/MIE /var/www/html/crs/1393

mv /var/www/swad/asg/LPSP/1/1/MOIP /var/www/swad/crs/1394
mv /var/www/html/asg/LPSP/1/1/MOIP /var/www/html/crs/1394

mv /var/www/swad/asg/LPSP/1/1/MOIPD /var/www/swad/crs/1395
mv /var/www/html/asg/LPSP/1/1/MOIPD /var/www/html/crs/1395

mv /var/www/swad/asg/LPSP/1/1/PPRSEE /var/www/swad/crs/1396
mv /var/www/html/asg/LPSP/1/1/PPRSEE /var/www/html/crs/1396

mv /var/www/swad/asg/LPSP/1/2/PIP /var/www/swad/crs/1397
mv /var/www/html/asg/LPSP/1/2/PIP /var/www/html/crs/1397

mv /var/www/swad/asg/LPSP/1/1/PI /var/www/swad/crs/1398
mv /var/www/html/asg/LPSP/1/1/PI /var/www/html/crs/1398

mv /var/www/swad/asg/LPSP/1/1/PE /var/www/swad/crs/1399
mv /var/www/html/asg/LPSP/1/1/PE /var/www/html/crs/1399

mv /var/www/swad/asg/LPSP/2/1/AEM /var/www/swad/crs/1400
mv /var/www/html/asg/LPSP/2/1/AEM /var/www/html/crs/1400

mv /var/www/swad/asg/LPSP/2/1/AMC /var/www/swad/crs/1401
mv /var/www/html/asg/LPSP/2/1/AMC /var/www/html/crs/1401

mv /var/www/swad/asg/LPSP/2/1/IPTD /var/www/swad/crs/1402
mv /var/www/html/asg/LPSP/2/1/IPTD /var/www/html/crs/1402

mv /var/www/swad/asg/LPSP/2/2/DSMS /var/www/swad/crs/1403
mv /var/www/html/asg/LPSP/2/2/DSMS /var/www/html/crs/1403

mv /var/www/swad/asg/LPSP/2/1/ELDA /var/www/swad/crs/1404
mv /var/www/html/asg/LPSP/2/1/ELDA /var/www/html/crs/1404

mv /var/www/swad/asg/LPSP/2/1/EDACE /var/www/swad/crs/1405
mv /var/www/html/asg/LPSP/2/1/EDACE /var/www/html/crs/1405

mv /var/www/swad/asg/LPSP/2/2/EFEDEA /var/www/swad/crs/1406
mv /var/www/html/asg/LPSP/2/2/EFEDEA /var/www/html/crs/1406

mv /var/www/swad/asg/LPSP/2/1/EMDEA /var/www/swad/crs/1407
mv /var/www/html/asg/LPSP/2/1/EMDEA /var/www/html/crs/1407

mv /var/www/swad/asg/LPSP/2/2/EPDEA /var/www/swad/crs/1408
mv /var/www/html/asg/LPSP/2/2/EPDEA /var/www/html/crs/1408

mv /var/www/swad/asg/LPSP/2/1/IDEE /var/www/swad/crs/1409
mv /var/www/html/asg/LPSP/2/1/IDEE /var/www/html/crs/1409

mv /var/www/swad/asg/LPSP/2/2/MDTIP /var/www/swad/crs/1410
mv /var/www/html/asg/LPSP/2/2/MDTIP /var/www/html/crs/1410

mv /var/www/swad/asg/LPSP/2/2/OEEE /var/www/swad/crs/1411
mv /var/www/html/asg/LPSP/2/2/OEEE /var/www/html/crs/1411

mv /var/www/swad/asg/LPSP/2/1/OP /var/www/swad/crs/1412
mv /var/www/html/asg/LPSP/2/1/OP /var/www/html/crs/1412

mv /var/www/swad/asg/LPSP/2/0/P /var/www/swad/crs/1413
mv /var/www/html/asg/LPSP/2/0/P /var/www/html/crs/1413

mv /var/www/swad/asg/LPSP/2/2/PG /var/www/swad/crs/1414
mv /var/www/html/asg/LPSP/2/2/PG /var/www/html/crs/1414

mv /var/www/swad/asg/LPSP/2/2/PS /var/www/swad/crs/1415
mv /var/www/html/asg/LPSP/2/2/PS /var/www/html/crs/1415

mv /var/www/swad/asg/LPSP/2/1/PTL /var/www/swad/crs/1416
mv /var/www/html/asg/LPSP/2/1/PTL /var/www/html/crs/1416

mv /var/www/swad/asg/LPSP/2/1/PDI /var/www/swad/crs/1417
mv /var/www/html/asg/LPSP/2/1/PDI /var/www/html/crs/1417

mv /var/www/swad/asg/LPSP/2/2/TPP /var/www/swad/crs/1418
mv /var/www/html/asg/LPSP/2/2/TPP /var/www/html/crs/1418

mv /var/www/swad/asg/LPC/0/2/MOE /var/www/swad/crs/1419
mv /var/www/html/asg/LPC/0/2/MOE /var/www/html/crs/1419

mv /var/www/swad/asg/LPC/1/2/DPP /var/www/swad/crs/1420
mv /var/www/html/asg/LPC/1/2/DPP /var/www/html/crs/1420

mv /var/www/swad/asg/LPC/1/2/DE /var/www/swad/crs/1421
mv /var/www/html/asg/LPC/1/2/DE /var/www/html/crs/1421

mv /var/www/swad/asg/LPC/1/1/DAIP /var/www/swad/crs/1422
mv /var/www/html/asg/LPC/1/1/DAIP /var/www/html/crs/1422

mv /var/www/swad/asg/LPC/1/1/DDIC /var/www/swad/crs/1423
mv /var/www/html/asg/LPC/1/1/DDIC /var/www/html/crs/1423

mv /var/www/swad/asg/LPC/1/2/EE /var/www/swad/crs/1424
mv /var/www/html/asg/LPC/1/2/EE /var/www/html/crs/1424

mv /var/www/swad/asg/LPC/1/2/EPCE /var/www/swad/crs/1425
mv /var/www/html/asg/LPC/1/2/EPCE /var/www/html/crs/1425

mv /var/www/swad/asg/LPC/1/1/MIE /var/www/swad/crs/1426
mv /var/www/html/asg/LPC/1/1/MIE /var/www/html/crs/1426

mv /var/www/swad/asg/LPC/1/1/MOIP /var/www/swad/crs/1427
mv /var/www/html/asg/LPC/1/1/MOIP /var/www/html/crs/1427

mv /var/www/swad/asg/LPC/1/2/PIP /var/www/swad/crs/1428
mv /var/www/html/asg/LPC/1/2/PIP /var/www/html/crs/1428

mv /var/www/swad/asg/LPC/1/2/PE /var/www/swad/crs/1429
mv /var/www/html/asg/LPC/1/2/PE /var/www/html/crs/1429

mv /var/www/swad/asg/LPC/1/2/PI /var/www/swad/crs/1430
mv /var/www/html/asg/LPC/1/2/PI /var/www/html/crs/1430

mv /var/www/swad/asg/LPC/1/1/PI /var/www/swad/crs/1431
mv /var/www/html/asg/LPC/1/1/PI /var/www/html/crs/1431

mv /var/www/swad/asg/LPC/1/2/THIEE /var/www/swad/crs/1432
mv /var/www/html/asg/LPC/1/2/THIEE /var/www/html/crs/1432

mv /var/www/swad/asg/LPC/2/2/AC /var/www/swad/crs/1433
mv /var/www/html/asg/LPC/2/2/AC /var/www/html/crs/1433

mv /var/www/swad/asg/LPC/2/2/AMC /var/www/swad/crs/1434
mv /var/www/html/asg/LPC/2/2/AMC /var/www/html/crs/1434

mv /var/www/swad/asg/LPC/2/1/AEM /var/www/swad/crs/1435
mv /var/www/html/asg/LPC/2/1/AEM /var/www/html/crs/1435

mv /var/www/swad/asg/LPC/2/1/DSMS /var/www/swad/crs/1436
mv /var/www/html/asg/LPC/2/1/DSMS /var/www/html/crs/1436

mv /var/www/swad/asg/LPC/2/2/EACDEA /var/www/swad/crs/1437
mv /var/www/html/asg/LPC/2/2/EACDEA /var/www/html/crs/1437

mv /var/www/swad/asg/LPC/2/2/ELMDA /var/www/swad/crs/1438
mv /var/www/html/asg/LPC/2/2/ELMDA /var/www/html/crs/1438

mv /var/www/swad/asg/LPC/2/1/EDACSE /var/www/swad/crs/1439
mv /var/www/html/asg/LPC/2/1/EDACSE /var/www/html/crs/1439

mv /var/www/swad/asg/LPC/2/2/IDEE /var/www/swad/crs/1440
mv /var/www/html/asg/LPC/2/2/IDEE /var/www/html/crs/1440

mv /var/www/swad/asg/LPC/2/1/IPTD /var/www/swad/crs/1441
mv /var/www/html/asg/LPC/2/1/IPTD /var/www/html/crs/1441

mv /var/www/swad/asg/LPC/2/2/OEEE /var/www/swad/crs/1442
mv /var/www/html/asg/LPC/2/2/OEEE /var/www/html/crs/1442

mv /var/www/swad/asg/LPC/2/2/OP /var/www/swad/crs/1443
mv /var/www/html/asg/LPC/2/2/OP /var/www/html/crs/1443

mv /var/www/swad/asg/LPC/2/0/P /var/www/swad/crs/1444
mv /var/www/html/asg/LPC/2/0/P /var/www/html/crs/1444

mv /var/www/swad/asg/LPC/2/1/PG /var/www/swad/crs/1445
mv /var/www/html/asg/LPC/2/1/PG /var/www/html/crs/1445

mv /var/www/swad/asg/LPC/2/2/PTL /var/www/swad/crs/1446
mv /var/www/html/asg/LPC/2/2/PTL /var/www/html/crs/1446

mv /var/www/swad/asg/LPC/2/1/PIP /var/www/swad/crs/1447
mv /var/www/html/asg/LPC/2/1/PIP /var/www/html/crs/1447

mv /var/www/swad/asg/LPC/2/1/TPP /var/www/swad/crs/1448
mv /var/www/html/asg/LPC/2/1/TPP /var/www/html/crs/1448

mv /var/www/swad/asg/LPM/0/1/AALE /var/www/swad/crs/1449
mv /var/www/html/asg/LPM/0/1/AALE /var/www/html/crs/1449

mv /var/www/swad/asg/LPM/2/2/EMIA /var/www/swad/crs/1450
mv /var/www/html/asg/LPM/2/2/EMIA /var/www/html/crs/1450

mv /var/www/swad/asg/LQ/0/2/CQI /var/www/swad/crs/1451
mv /var/www/html/asg/LQ/0/2/CQI /var/www/html/crs/1451

mv /var/www/swad/asg/LQ/0/2/EEPMA /var/www/swad/crs/1452
mv /var/www/html/asg/LQ/0/2/EEPMA /var/www/html/crs/1452

mv /var/www/swad/asg/LQ/1/2/CA /var/www/swad/crs/1453
mv /var/www/html/asg/LQ/1/2/CA /var/www/html/crs/1453

mv /var/www/swad/asg/LQ/1/2/CB /var/www/swad/crs/1454
mv /var/www/html/asg/LQ/1/2/CB /var/www/html/crs/1454

mv /var/www/swad/asg/LQ/1/2/CC /var/www/swad/crs/1455
mv /var/www/html/asg/LQ/1/2/CC /var/www/html/crs/1455

mv /var/www/swad/asg/LQ/1/2/CD /var/www/swad/crs/1456
mv /var/www/html/asg/LQ/1/2/CD /var/www/html/crs/1456

mv /var/www/swad/asg/LQ/1/0/MA /var/www/swad/crs/1457
mv /var/www/html/asg/LQ/1/0/MA /var/www/html/crs/1457

mv /var/www/swad/asg/LQ/1/0/MB /var/www/swad/crs/1458
mv /var/www/html/asg/LQ/1/0/MB /var/www/html/crs/1458

mv /var/www/swad/asg/LQ/1/0/QAA /var/www/swad/crs/1459
mv /var/www/html/asg/LQ/1/0/QAA /var/www/html/crs/1459

mv /var/www/swad/asg/LQ/1/0/QAB /var/www/swad/crs/1460
mv /var/www/html/asg/LQ/1/0/QAB /var/www/html/crs/1460

mv /var/www/swad/asg/LQ/1/0/QAC /var/www/swad/crs/1461
mv /var/www/html/asg/LQ/1/0/QAC /var/www/html/crs/1461

mv /var/www/swad/asg/LQ/1/0/QO1 /var/www/swad/crs/1462
mv /var/www/html/asg/LQ/1/0/QO1 /var/www/html/crs/1462

mv /var/www/swad/asg/LQ/2/1/ESO /var/www/swad/crs/1463
mv /var/www/html/asg/LQ/2/1/ESO /var/www/html/crs/1463

mv /var/www/swad/asg/LQ/2/1/IUC /var/www/swad/crs/1464
mv /var/www/html/asg/LQ/2/1/IUC /var/www/html/crs/1464

mv /var/www/swad/asg/LQ/2/1/MA /var/www/swad/crs/1465
mv /var/www/html/asg/LQ/2/1/MA /var/www/html/crs/1465

mv /var/www/swad/asg/LQ/2/0/QAIA /var/www/swad/crs/1466
mv /var/www/html/asg/LQ/2/0/QAIA /var/www/html/crs/1466

mv /var/www/swad/asg/LQ/2/0/QAIB /var/www/swad/crs/1467
mv /var/www/html/asg/LQ/2/0/QAIB /var/www/html/crs/1467

mv /var/www/swad/asg/LQ/2/0/QAIC /var/www/swad/crs/1468
mv /var/www/html/asg/LQ/2/0/QAIC /var/www/html/crs/1468

mv /var/www/swad/asg/LQ/2/0/QIC /var/www/swad/crs/1469
mv /var/www/html/asg/LQ/2/0/QIC /var/www/html/crs/1469

mv /var/www/swad/asg/LQ/3/0/IQ /var/www/swad/crs/1470
mv /var/www/html/asg/LQ/3/0/IQ /var/www/html/crs/1470

mv /var/www/swad/asg/LQ/3/1/IEQA /var/www/swad/crs/1471
mv /var/www/html/asg/LQ/3/1/IEQA /var/www/html/crs/1471

mv /var/www/swad/asg/LQ/4/1/EQA1A /var/www/swad/crs/1472
mv /var/www/html/asg/LQ/4/1/EQA1A /var/www/html/crs/1472

mv /var/www/swad/asg/LQ/4/1/EQA1B /var/www/swad/crs/1473
mv /var/www/html/asg/LQ/4/1/EQA1B /var/www/html/crs/1473

mv /var/www/swad/asg/LQ/5/2/AS /var/www/swad/crs/1474
mv /var/www/html/asg/LQ/5/2/AS /var/www/html/crs/1474

mv /var/www/swad/asg/LQ/5/1/AQBC /var/www/swad/crs/1475
mv /var/www/html/asg/LQ/5/1/AQBC /var/www/html/crs/1475

mv /var/www/swad/asg/LQ/5/1/MAA /var/www/swad/crs/1476
mv /var/www/html/asg/LQ/5/1/MAA /var/www/html/crs/1476

mv /var/www/swad/asg/LQ/5/1/TQCCQA /var/www/swad/crs/1477
mv /var/www/html/asg/LQ/5/1/TQCCQA /var/www/html/crs/1477

mv /var/www/swad/asg/LSOC/0/2/AM /var/www/swad/crs/1478
mv /var/www/html/asg/LSOC/0/2/AM /var/www/html/crs/1478

mv /var/www/swad/asg/LSOC/1/0/EP /var/www/swad/crs/1479
mv /var/www/html/asg/LSOC/1/0/EP /var/www/html/crs/1479

mv /var/www/swad/asg/LSOC/1/0/EPB /var/www/swad/crs/1480
mv /var/www/html/asg/LSOC/1/0/EPB /var/www/html/crs/1480

mv /var/www/swad/asg/LSOC/2/0/HTS /var/www/swad/crs/1481
mv /var/www/html/asg/LSOC/2/0/HTS /var/www/html/crs/1481

mv /var/www/swad/asg/LSOC/2/0/HTSB /var/www/swad/crs/1482
mv /var/www/html/asg/LSOC/2/0/HTSB /var/www/html/crs/1482

mv /var/www/swad/asg/LSOC/2/0/TEA /var/www/swad/crs/1483
mv /var/www/html/asg/LSOC/2/0/TEA /var/www/html/crs/1483

mv /var/www/swad/asg/LSOC/2/0/TEB /var/www/swad/crs/1484
mv /var/www/html/asg/LSOC/2/0/TEB /var/www/html/crs/1484

mv /var/www/swad/asg/LSOC/3/0/CAB /var/www/swad/crs/1485
mv /var/www/html/asg/LSOC/3/0/CAB /var/www/html/crs/1485

mv /var/www/swad/asg/LSOC/4/0/SIIB /var/www/swad/crs/1486
mv /var/www/html/asg/LSOC/4/0/SIIB /var/www/html/crs/1486

mv /var/www/swad/asg/LSOC/4/2/TAISC /var/www/swad/crs/1487
mv /var/www/html/asg/LSOC/4/2/TAISC /var/www/html/crs/1487

mv /var/www/swad/asg/LSOC/5/1/SC /var/www/swad/crs/1488
mv /var/www/html/asg/LSOC/5/1/SC /var/www/html/crs/1488

mv /var/www/swad/asg/LSOC/5/1/SU /var/www/swad/crs/1489
mv /var/www/html/asg/LSOC/5/1/SU /var/www/html/crs/1489

mv /var/www/swad/asg/LSOC/5/1/TIAC /var/www/swad/crs/1490
mv /var/www/html/asg/LSOC/5/1/TIAC /var/www/html/crs/1490

mv /var/www/swad/asg/LSOC/5/1/TMCS /var/www/swad/crs/1491
mv /var/www/html/asg/LSOC/5/1/TMCS /var/www/html/crs/1491

mv /var/www/swad/asg/LTLLC/0/2/CC /var/www/swad/crs/1492
mv /var/www/html/asg/LTLLC/0/2/CC /var/www/html/crs/1492

mv /var/www/swad/asg/LTLLC/1/2/CL2 /var/www/swad/crs/1493
mv /var/www/html/asg/LTLLC/1/2/CL2 /var/www/html/crs/1493

mv /var/www/swad/asg/LTLLC/1/1/HPLE /var/www/swad/crs/1494
mv /var/www/html/asg/LTLLC/1/1/HPLE /var/www/html/crs/1494

mv /var/www/swad/asg/LTI/0/1/AIDDC /var/www/swad/crs/1495
mv /var/www/html/asg/LTI/0/1/AIDDC /var/www/html/crs/1495

mv /var/www/swad/asg/LTI/0/2/CTS /var/www/swad/crs/1496
mv /var/www/html/asg/LTI/0/2/CTS /var/www/html/crs/1496

mv /var/www/swad/asg/LTI/0/2/CE2 /var/www/swad/crs/1497
mv /var/www/html/asg/LTI/0/2/CE2 /var/www/html/crs/1497

mv /var/www/swad/asg/LTI/0/1/DE /var/www/swad/crs/1498
mv /var/www/html/asg/LTI/0/1/DE /var/www/html/crs/1498

mv /var/www/swad/asg/LTI/0/1/IBCI /var/www/swad/crs/1499
mv /var/www/html/asg/LTI/0/1/IBCI /var/www/html/crs/1499

mv /var/www/swad/asg/LTI/0/0/LE3 /var/www/swad/crs/1500
mv /var/www/html/asg/LTI/0/0/LE3 /var/www/html/crs/1500

mv /var/www/swad/asg/LTI/0/1/MM /var/www/swad/crs/1501
mv /var/www/html/asg/LTI/0/1/MM /var/www/html/crs/1501

mv /var/www/swad/asg/LTI/0/2/TCM /var/www/swad/crs/1502
mv /var/www/html/asg/LTI/0/2/TCM /var/www/html/crs/1502

mv /var/www/swad/asg/LTI/0/1/TAA /var/www/swad/crs/1503
mv /var/www/html/asg/LTI/0/1/TAA /var/www/html/crs/1503

mv /var/www/swad/asg/LTI/0/1/T14FE /var/www/swad/crs/1504
mv /var/www/html/asg/LTI/0/1/T14FE /var/www/html/crs/1504

mv /var/www/swad/asg/LTI/0/2/VIDDA /var/www/swad/crs/1505
mv /var/www/html/asg/LTI/0/2/VIDDA /var/www/html/crs/1505

mv /var/www/swad/asg/LTI/1/1/LE1CD /var/www/swad/crs/1506
mv /var/www/html/asg/LTI/1/1/LE1CD /var/www/html/crs/1506

mv /var/www/swad/asg/LTI/2/2/CR /var/www/swad/crs/1507
mv /var/www/html/asg/LTI/2/2/CR /var/www/html/crs/1507

mv /var/www/swad/asg/LTI/2/1/DAT /var/www/swad/crs/1508
mv /var/www/html/asg/LTI/2/1/DAT /var/www/html/crs/1508

mv /var/www/swad/asg/LTI/2/2/T2BAF /var/www/swad/crs/1509
mv /var/www/html/asg/LTI/2/2/T2BAF /var/www/html/crs/1509

mv /var/www/swad/asg/LTI/2/2/T2I /var/www/swad/crs/1510
mv /var/www/html/asg/LTI/2/2/T2I /var/www/html/crs/1510

mv /var/www/swad/asg/LTI/2/2/T3A /var/www/swad/crs/1511
mv /var/www/html/asg/LTI/2/2/T3A /var/www/html/crs/1511

mv /var/www/swad/asg/LTI/2/2/T3ABC /var/www/swad/crs/1512
mv /var/www/html/asg/LTI/2/2/T3ABC /var/www/html/crs/1512

mv /var/www/swad/asg/LTI/2/2/T3ABIB /var/www/swad/crs/1513
mv /var/www/html/asg/LTI/2/2/T3ABIB /var/www/html/crs/1513

mv /var/www/swad/asg/LTI/2/2/T3ABIC /var/www/swad/crs/1514
mv /var/www/html/asg/LTI/2/2/T3ABIC /var/www/html/crs/1514

mv /var/www/swad/asg/LTI/2/1/TPTA /var/www/swad/crs/1515
mv /var/www/html/asg/LTI/2/1/TPTA /var/www/html/crs/1515

mv /var/www/swad/asg/LTI/2/1/TPTF /var/www/swad/crs/1516
mv /var/www/html/asg/LTI/2/1/TPTF /var/www/html/crs/1516

mv /var/www/swad/asg/LTI/3/2/IAT /var/www/swad/crs/1517
mv /var/www/html/asg/LTI/3/2/IAT /var/www/html/crs/1517

mv /var/www/swad/asg/LTI/3/2/IATBDF /var/www/swad/crs/1518
mv /var/www/html/asg/LTI/3/2/IATBDF /var/www/html/crs/1518

mv /var/www/swad/asg/LTI/3/2/IATCE /var/www/swad/crs/1519
mv /var/www/html/asg/LTI/3/2/IATCE /var/www/html/crs/1519

mv /var/www/swad/asg/LTI/3/2/PTB /var/www/swad/crs/1520
mv /var/www/html/asg/LTI/3/2/PTB /var/www/html/crs/1520

mv /var/www/swad/asg/LTI/3/2/TISA /var/www/swad/crs/1521
mv /var/www/html/asg/LTI/3/2/TISA /var/www/html/crs/1521

mv /var/www/swad/asg/LTI/3/1/T1CAA /var/www/swad/crs/1522
mv /var/www/html/asg/LTI/3/1/T1CAA /var/www/html/crs/1522

mv /var/www/swad/asg/LTI/3/1/T4BAA /var/www/swad/crs/1523
mv /var/www/html/asg/LTI/3/1/T4BAA /var/www/html/crs/1523

mv /var/www/swad/asg/LTI/3/1/T4BAF /var/www/swad/crs/1524
mv /var/www/html/asg/LTI/3/1/T4BAF /var/www/html/crs/1524

mv /var/www/swad/asg/LTI/3/1/T5A /var/www/swad/crs/1525
mv /var/www/html/asg/LTI/3/1/T5A /var/www/html/crs/1525

mv /var/www/swad/asg/LTI/3/1/T5ABF /var/www/swad/crs/1526
mv /var/www/html/asg/LTI/3/1/T5ABF /var/www/html/crs/1526

mv /var/www/swad/asg/LTI/3/2/T5ABFC /var/www/swad/crs/1527
mv /var/www/html/asg/LTI/3/2/T5ABFC /var/www/html/crs/1527

mv /var/www/swad/asg/LTI/3/2/T7EA /var/www/swad/crs/1528
mv /var/www/html/asg/LTI/3/2/T7EA /var/www/html/crs/1528

mv /var/www/swad/asg/LTI/4/0/ICA /var/www/swad/crs/1529
mv /var/www/html/asg/LTI/4/0/ICA /var/www/html/crs/1529

mv /var/www/swad/asg/LTI/4/1/T4B /var/www/swad/crs/1530
mv /var/www/html/asg/LTI/4/1/T4B /var/www/html/crs/1530

mv /var/www/swad/asg/LTI/4/1/T4CAA /var/www/swad/crs/1531
mv /var/www/html/asg/LTI/4/1/T4CAA /var/www/html/crs/1531

mv /var/www/swad/asg/LTI/4/1/T8B /var/www/swad/crs/1532
mv /var/www/html/asg/LTI/4/1/T8B /var/www/html/crs/1532

mv /var/www/swad/asg/LTI/4/1/T8BAA /var/www/swad/crs/1533
mv /var/www/html/asg/LTI/4/1/T8BAA /var/www/html/crs/1533

mv /var/www/swad/asg/LTI/4/1/T8I /var/www/swad/crs/1534
mv /var/www/html/asg/LTI/4/1/T8I /var/www/html/crs/1534

mv /var/www/swad/asg/LTI/4/1/T9A /var/www/swad/crs/1535
mv /var/www/html/asg/LTI/4/1/T9A /var/www/html/crs/1535

mv /var/www/swad/asg/LTI/4/2/T9ABFL /var/www/swad/crs/1536
mv /var/www/html/asg/LTI/4/2/T9ABFL /var/www/html/crs/1536

mv /var/www/swad/asg/LTI/4/2/T11EA /var/www/swad/crs/1537
mv /var/www/html/asg/LTI/4/2/T11EA /var/www/html/crs/1537

mv /var/www/swad/asg/LTI/4/1/T12 /var/www/swad/crs/1538
mv /var/www/html/asg/LTI/4/1/T12 /var/www/html/crs/1538

mv /var/www/swad/asg/LTI/4/2/T13EA /var/www/swad/crs/1539
mv /var/www/html/asg/LTI/4/2/T13EA /var/www/html/crs/1539

mv /var/www/swad/asg/M/0/2/LD /var/www/swad/crs/1540
mv /var/www/html/asg/M/0/2/LD /var/www/html/crs/1540

mv /var/www/swad/asg/M/0/2/NSAFC /var/www/swad/crs/1541
mv /var/www/html/asg/M/0/2/NSAFC /var/www/html/crs/1541

mv /var/www/swad/asg/M/3/2/NTAE /var/www/swad/crs/1542
mv /var/www/html/asg/M/3/2/NTAE /var/www/html/crs/1542

mv /var/www/swad/asg/M/3/2/OET /var/www/swad/crs/1543
mv /var/www/html/asg/M/3/2/OET /var/www/html/crs/1543

mv /var/www/swad/asg/M/3/1/PM1 /var/www/swad/crs/1544
mv /var/www/html/asg/M/3/1/PM1 /var/www/html/crs/1544

mv /var/www/swad/asg/M/3/0/SE /var/www/swad/crs/1545
mv /var/www/html/asg/M/3/0/SE /var/www/html/crs/1545

mv /var/www/swad/asg/MC/0/2/TMEE /var/www/swad/crs/1546
mv /var/www/html/asg/MC/0/2/TMEE /var/www/html/crs/1546

mv /var/www/swad/asg/MC/1/0/PEDEE /var/www/swad/crs/1547
mv /var/www/html/asg/MC/1/0/PEDEE /var/www/html/crs/1547

mv /var/www/swad/asg/MC/3/1/OCE /var/www/swad/crs/1548
mv /var/www/html/asg/MC/3/1/OCE /var/www/html/crs/1548

mv /var/www/swad/asg/MML/1/1/IEDI /var/www/swad/crs/1549
mv /var/www/html/asg/MML/1/1/IEDI /var/www/html/crs/1549

mv /var/www/swad/asg/MAL/1/1/DIL /var/www/swad/crs/1550
mv /var/www/html/asg/MAL/1/1/DIL /var/www/html/crs/1550

mv /var/www/swad/asg/MAL/3/1/NTAE /var/www/swad/crs/1551
mv /var/www/html/asg/MAL/3/1/NTAE /var/www/html/crs/1551

mv /var/www/swad/asg/MAL/3/1/NTAEA /var/www/swad/crs/1552
mv /var/www/html/asg/MAL/3/1/NTAEA /var/www/html/crs/1552

mv /var/www/swad/asg/MAL/3/2/PM2 /var/www/swad/crs/1553
mv /var/www/html/asg/MAL/3/2/PM2 /var/www/html/crs/1553

mv /var/www/swad/asg/MAL/3/1/TETLOE /var/www/swad/crs/1554
mv /var/www/html/asg/MAL/3/1/TETLOE /var/www/html/crs/1554

mv /var/www/swad/asg/MALC/1/1/AFNL /var/www/swad/crs/1555
mv /var/www/html/asg/MALC/1/1/AFNL /var/www/html/crs/1555

mv /var/www/swad/asg/MALC/1/1/CLC /var/www/swad/crs/1556
mv /var/www/html/asg/MALC/1/1/CLC /var/www/html/crs/1556

mv /var/www/swad/asg/MALC/1/0/DG /var/www/swad/crs/1557
mv /var/www/html/asg/MALC/1/0/DG /var/www/html/crs/1557

mv /var/www/swad/asg/MALC/1/2/EMSC /var/www/swad/crs/1558
mv /var/www/html/asg/MALC/1/2/EMSC /var/www/html/crs/1558

mv /var/www/swad/asg/MALC/1/2/EPSC /var/www/swad/crs/1559
mv /var/www/html/asg/MALC/1/2/EPSC /var/www/html/crs/1559

mv /var/www/swad/asg/MALC/1/1/LI /var/www/swad/crs/1560
mv /var/www/html/asg/MALC/1/1/LI /var/www/html/crs/1560

mv /var/www/swad/asg/MALC/1/0/L /var/www/swad/crs/1561
mv /var/www/html/asg/MALC/1/0/L /var/www/html/crs/1561

mv /var/www/swad/asg/MALC/1/0/PEDEE /var/www/swad/crs/1562
mv /var/www/html/asg/MALC/1/0/PEDEE /var/www/html/crs/1562

mv /var/www/swad/asg/MALC/1/2/PL /var/www/swad/crs/1563
mv /var/www/html/asg/MALC/1/2/PL /var/www/html/crs/1563

mv /var/www/swad/asg/MALC/1/1/SE /var/www/swad/crs/1564
mv /var/www/html/asg/MALC/1/1/SE /var/www/html/crs/1564

mv /var/www/swad/asg/MALC/1/2/TICE /var/www/swad/crs/1565
mv /var/www/html/asg/MALC/1/2/TICE /var/www/html/crs/1565

mv /var/www/swad/asg/MALC/2/1/ANL /var/www/swad/crs/1566
mv /var/www/html/asg/MALC/2/1/ANL /var/www/html/crs/1566

mv /var/www/swad/asg/MALC/2/2/BPEE /var/www/swad/crs/1567
mv /var/www/html/asg/MALC/2/2/BPEE /var/www/html/crs/1567

mv /var/www/swad/asg/MALC/2/1/BPEE /var/www/swad/crs/1568
mv /var/www/html/asg/MALC/2/1/BPEE /var/www/html/crs/1568

mv /var/www/swad/asg/MALC/2/0/DHL /var/www/swad/crs/1569
mv /var/www/html/asg/MALC/2/0/DHL /var/www/html/crs/1569

mv /var/www/swad/asg/MALC/2/1/DIL /var/www/swad/crs/1570
mv /var/www/html/asg/MALC/2/1/DIL /var/www/html/crs/1570

mv /var/www/swad/asg/MALC/2/2/EA /var/www/swad/crs/1571
mv /var/www/html/asg/MALC/2/2/EA /var/www/html/crs/1571

mv /var/www/swad/asg/MALC/2/2/LD /var/www/swad/crs/1572
mv /var/www/html/asg/MALC/2/2/LD /var/www/html/crs/1572

mv /var/www/swad/asg/MALC/2/1/NTAE /var/www/swad/crs/1573
mv /var/www/html/asg/MALC/2/1/NTAE /var/www/html/crs/1573

mv /var/www/swad/asg/MALC/2/0/PAL /var/www/swad/crs/1574
mv /var/www/html/asg/MALC/2/0/PAL /var/www/html/crs/1574

mv /var/www/swad/asg/MALC/2/2/TAIL /var/www/swad/crs/1575
mv /var/www/html/asg/MALC/2/2/TAIL /var/www/html/crs/1575

mv /var/www/swad/asg/MALC/2/0/TETAL /var/www/swad/crs/1576
mv /var/www/html/asg/MALC/2/0/TETAL /var/www/html/crs/1576

mv /var/www/swad/asg/MALC/2/0/TETLOE /var/www/swad/crs/1577
mv /var/www/html/asg/MALC/2/0/TETLOE /var/www/html/crs/1577

mv /var/www/swad/asg/MALC/3/1/AEPL /var/www/swad/crs/1578
mv /var/www/html/asg/MALC/3/1/AEPL /var/www/html/crs/1578

mv /var/www/swad/asg/MALC/3/1/AVT /var/www/swad/crs/1579
mv /var/www/html/asg/MALC/3/1/AVT /var/www/html/crs/1579

mv /var/www/swad/asg/MALC/3/1/EL /var/www/swad/crs/1580
mv /var/www/html/asg/MALC/3/1/EL /var/www/html/crs/1580

mv /var/www/swad/asg/MALC/3/1/FPAA /var/www/swad/crs/1581
mv /var/www/html/asg/MALC/3/1/FPAA /var/www/html/crs/1581

mv /var/www/swad/asg/MALC/3/1/OCE /var/www/swad/crs/1582
mv /var/www/html/asg/MALC/3/1/OCE /var/www/html/crs/1582

mv /var/www/swad/asg/MALC/3/2/P /var/www/swad/crs/1583
mv /var/www/html/asg/MALC/3/2/P /var/www/html/crs/1583

mv /var/www/swad/asg/MALC/3/2/PITAL /var/www/swad/crs/1584
mv /var/www/html/asg/MALC/3/2/PITAL /var/www/html/crs/1584

mv /var/www/swad/asg/MALC/3/1/SAC /var/www/swad/crs/1585
mv /var/www/html/asg/MALC/3/1/SAC /var/www/html/crs/1585

mv /var/www/swad/asg/MALC/3/2/SL /var/www/swad/crs/1586
mv /var/www/html/asg/MALC/3/2/SL /var/www/html/crs/1586

mv /var/www/swad/asg/MALC/3/1/TTPAL /var/www/swad/crs/1587
mv /var/www/html/asg/MALC/3/1/TTPAL /var/www/html/crs/1587

mv /var/www/swad/asg/MEE/1/2/MEEA /var/www/swad/crs/1588
mv /var/www/html/asg/MEE/1/2/MEEA /var/www/html/crs/1588

mv /var/www/swad/asg/MEE/1/2/MEEC /var/www/swad/crs/1589
mv /var/www/html/asg/MEE/1/2/MEEC /var/www/html/crs/1589

mv /var/www/swad/asg/MEE/2/1/AEEDA /var/www/swad/crs/1590
mv /var/www/html/asg/MEE/2/1/AEEDA /var/www/html/crs/1590

mv /var/www/swad/asg/MEE/2/1/AEEDM /var/www/swad/crs/1591
mv /var/www/html/asg/MEE/2/1/AEEDM /var/www/html/crs/1591

mv /var/www/swad/asg/MEE/3/2/ADOEE /var/www/swad/crs/1592
mv /var/www/html/asg/MEE/3/2/ADOEE /var/www/html/crs/1592

mv /var/www/swad/asg/MEE/3/2/MNEE /var/www/swad/crs/1593
mv /var/www/html/asg/MEE/3/2/MNEE /var/www/html/crs/1593

mv /var/www/swad/asg/MEE/3/2/NTAE /var/www/swad/crs/1594
mv /var/www/html/asg/MEE/3/2/NTAE /var/www/html/crs/1594

mv /var/www/swad/asg/MEE/3/1/P1 /var/www/swad/crs/1595
mv /var/www/html/asg/MEE/3/1/P1 /var/www/html/crs/1595

mv /var/www/swad/asg/MEE/3/2/SPM2 /var/www/swad/crs/1596
mv /var/www/html/asg/MEE/3/2/SPM2 /var/www/html/crs/1596

mv /var/www/swad/asg/MEEC/1/0/AEEDM /var/www/swad/crs/1597
mv /var/www/html/asg/MEEC/1/0/AEEDM /var/www/html/crs/1597

mv /var/www/swad/asg/MEEC/1/2/AEEDV /var/www/swad/crs/1598
mv /var/www/html/asg/MEEC/1/2/AEEDV /var/www/html/crs/1598

mv /var/www/swad/asg/MEEC/1/0/DG /var/www/swad/crs/1599
mv /var/www/html/asg/MEEC/1/0/DG /var/www/html/crs/1599

mv /var/www/swad/asg/MEEC/1/2/IAPV /var/www/swad/crs/1600
mv /var/www/html/asg/MEEC/1/2/IAPV /var/www/html/crs/1600

mv /var/www/swad/asg/MEEC/1/1/JMAEE /var/www/swad/crs/1601
mv /var/www/html/asg/MEEC/1/1/JMAEE /var/www/html/crs/1601

mv /var/www/swad/asg/MEEC/1/0/PEDEE /var/www/swad/crs/1602
mv /var/www/html/asg/MEEC/1/0/PEDEE /var/www/html/crs/1602

mv /var/www/swad/asg/MEEC/1/1/SE /var/www/swad/crs/1603
mv /var/www/html/asg/MEEC/1/1/SE /var/www/html/crs/1603

mv /var/www/swad/asg/MEEC/1/2/TICE /var/www/swad/crs/1604
mv /var/www/html/asg/MEEC/1/2/TICE /var/www/html/crs/1604

mv /var/www/swad/asg/MEEC/1/1/TCP /var/www/swad/crs/1605
mv /var/www/html/asg/MEEC/1/1/TCP /var/www/html/crs/1605

mv /var/www/swad/asg/MEEC/2/1/AEEDA /var/www/swad/crs/1606
mv /var/www/html/asg/MEEC/2/1/AEEDA /var/www/html/crs/1606

mv /var/www/swad/asg/MEEC/2/1/AEEDM /var/www/swad/crs/1607
mv /var/www/html/asg/MEEC/2/1/AEEDM /var/www/html/crs/1607

mv /var/www/swad/asg/MEEC/2/2/BPEE /var/www/swad/crs/1608
mv /var/www/html/asg/MEEC/2/2/BPEE /var/www/html/crs/1608

mv /var/www/swad/asg/MEEC/2/1/BPEE /var/www/swad/crs/1609
mv /var/www/html/asg/MEEC/2/1/BPEE /var/www/html/crs/1609

mv /var/www/swad/asg/MEEC/2/2/EFANEE /var/www/swad/crs/1610
mv /var/www/html/asg/MEEC/2/2/EFANEE /var/www/html/crs/1610

mv /var/www/swad/asg/MEEC/2/2/EPM /var/www/swad/crs/1611
mv /var/www/html/asg/MEEC/2/2/EPM /var/www/html/crs/1611

mv /var/www/swad/asg/MEEC/2/2/IPEP /var/www/swad/crs/1612
mv /var/www/html/asg/MEEC/2/2/IPEP /var/www/html/crs/1612

mv /var/www/swad/asg/MEEC/2/1/NTAE /var/www/swad/crs/1613
mv /var/www/html/asg/MEEC/2/1/NTAE /var/www/html/crs/1613

mv /var/www/swad/asg/MEEC/2/1/TMEE /var/www/swad/crs/1614
mv /var/www/html/asg/MEEC/2/1/TMEE /var/www/html/crs/1614

mv /var/www/swad/asg/MEEC/2/0/TETLE /var/www/swad/crs/1615
mv /var/www/html/asg/MEEC/2/0/TETLE /var/www/html/crs/1615

mv /var/www/swad/asg/MEEC/3/1/ADOEE /var/www/swad/crs/1616
mv /var/www/html/asg/MEEC/3/1/ADOEE /var/www/html/crs/1616

mv /var/www/swad/asg/MEEC/3/2/BPEII /var/www/swad/crs/1617
mv /var/www/html/asg/MEEC/3/2/BPEII /var/www/html/crs/1617

mv /var/www/swad/asg/MEEC/3/1/ELANEE /var/www/swad/crs/1618
mv /var/www/html/asg/MEEC/3/1/ELANEE /var/www/html/crs/1618

mv /var/www/swad/asg/MEEC/3/1/EMANEE /var/www/swad/crs/1619
mv /var/www/html/asg/MEEC/3/1/EMANEE /var/www/html/crs/1619

mv /var/www/swad/asg/MEEC/3/1/MEE /var/www/swad/crs/1620
mv /var/www/html/asg/MEEC/3/1/MEE /var/www/html/crs/1620

mv /var/www/swad/asg/MEEC/3/1/OCE /var/www/swad/crs/1621
mv /var/www/html/asg/MEEC/3/1/OCE /var/www/html/crs/1621

mv /var/www/swad/asg/MEEC/3/1/OET /var/www/swad/crs/1622
mv /var/www/html/asg/MEEC/3/1/OET /var/www/html/crs/1622

mv /var/www/swad/asg/MEEC/3/2/P /var/www/swad/crs/1623
mv /var/www/html/asg/MEEC/3/2/P /var/www/html/crs/1623

mv /var/www/swad/asg/MEF/0/2/BMAF /var/www/swad/crs/1624
mv /var/www/html/asg/MEF/0/2/BMAF /var/www/html/crs/1624

mv /var/www/swad/asg/MEF/0/1/DDI /var/www/swad/crs/1625
mv /var/www/html/asg/MEF/0/1/DDI /var/www/html/crs/1625

mv /var/www/swad/asg/MEF/0/1/FEB /var/www/swad/crs/1626
mv /var/www/html/asg/MEF/0/1/FEB /var/www/html/crs/1626

mv /var/www/swad/asg/MEF/1/1/EADC /var/www/swad/crs/1627
mv /var/www/html/asg/MEF/1/1/EADC /var/www/html/crs/1627

mv /var/www/swad/asg/MEF/1/1/EFB /var/www/swad/crs/1628
mv /var/www/html/asg/MEF/1/1/EFB /var/www/html/crs/1628

mv /var/www/swad/asg/MEF/1/2/EFD1A /var/www/swad/crs/1629
mv /var/www/html/asg/MEF/1/2/EFD1A /var/www/html/crs/1629

mv /var/www/swad/asg/MEF/1/2/EFD1C /var/www/swad/crs/1630
mv /var/www/html/asg/MEF/1/2/EFD1C /var/www/html/crs/1630

mv /var/www/swad/asg/MEF/1/1/BBFM /var/www/swad/crs/1631
mv /var/www/html/asg/MEF/1/1/BBFM /var/www/html/crs/1631

mv /var/www/swad/asg/MEF/2/1/EFD2 /var/www/swad/crs/1632
mv /var/www/html/asg/MEF/2/1/EFD2 /var/www/html/crs/1632

mv /var/www/swad/asg/MEF/2/1/DJP /var/www/swad/crs/1633
mv /var/www/html/asg/MEF/2/1/DJP /var/www/html/crs/1633

mv /var/www/swad/asg/MEF/2/2/DD /var/www/swad/crs/1634
mv /var/www/html/asg/MEF/2/2/DD /var/www/html/crs/1634

mv /var/www/swad/asg/MEF/2/1/OCEC /var/www/swad/crs/1635
mv /var/www/html/asg/MEF/2/1/OCEC /var/www/html/crs/1635

mv /var/www/swad/asg/MEF/2/2/TPAFAC /var/www/swad/crs/1636
mv /var/www/html/asg/MEF/2/2/TPAFAC /var/www/html/crs/1636

mv /var/www/swad/asg/MEF/2/2/DLL /var/www/swad/crs/1637
mv /var/www/html/asg/MEF/2/2/DLL /var/www/html/crs/1637

mv /var/www/swad/asg/MEF/3/1/EFD3 /var/www/swad/crs/1638
mv /var/www/html/asg/MEF/3/1/EFD3 /var/www/html/crs/1638

mv /var/www/swad/asg/MEF/3/1/ECD /var/www/swad/crs/1639
mv /var/www/html/asg/MEF/3/1/ECD /var/www/html/crs/1639

mv /var/www/swad/asg/MEF/3/1/IEDIAC /var/www/swad/crs/1640
mv /var/www/html/asg/MEF/3/1/IEDIAC /var/www/html/crs/1640

mv /var/www/swad/asg/MEF/3/1/NTAE /var/www/swad/crs/1641
mv /var/www/html/asg/MEF/3/1/NTAE /var/www/html/crs/1641

mv /var/www/swad/asg/MEF/3/1/NTAEC /var/www/swad/crs/1642
mv /var/www/html/asg/MEF/3/1/NTAEC /var/www/html/crs/1642

mv /var/www/swad/asg/MEF/3/2/PMI3 /var/www/swad/crs/1643
mv /var/www/html/asg/MEF/3/2/PMI3 /var/www/html/crs/1643

mv /var/www/swad/asg/MEF/3/2/PEFG5 /var/www/swad/crs/1644
mv /var/www/html/asg/MEF/3/2/PEFG5 /var/www/html/crs/1644

mv /var/www/swad/asg/MEF/3/2/PM2EFV /var/www/swad/crs/1645
mv /var/www/html/asg/MEF/3/2/PM2EFV /var/www/html/crs/1645

mv /var/www/swad/asg/MEF/3/2/P2 /var/www/swad/crs/1646
mv /var/www/html/asg/MEF/3/2/P2 /var/www/html/crs/1646

mv /var/www/swad/asg/MEFC/1/1/IDI /var/www/swad/crs/1647
mv /var/www/html/asg/MEFC/1/1/IDI /var/www/html/crs/1647

mv /var/www/swad/asg/MEFC/1/2/MEF /var/www/swad/crs/1648
mv /var/www/html/asg/MEFC/1/2/MEF /var/www/html/crs/1648

mv /var/www/swad/asg/MEFC/2/2/EFB /var/www/swad/crs/1649
mv /var/www/html/asg/MEFC/2/2/EFB /var/www/html/crs/1649

mv /var/www/swad/asg/MEFC/3/1/IDC /var/www/swad/crs/1650
mv /var/www/html/asg/MEFC/3/1/IDC /var/www/html/crs/1650

mv /var/www/swad/asg/MEFM/0/1/AFMA /var/www/swad/crs/1651
mv /var/www/html/asg/MEFM/0/1/AFMA /var/www/html/crs/1651

mv /var/www/swad/asg/MEFM/0/2/AFRO /var/www/swad/crs/1652
mv /var/www/html/asg/MEFM/0/2/AFRO /var/www/html/crs/1652

mv /var/www/swad/asg/MEFM/0/2/DCEFEP /var/www/swad/crs/1653
mv /var/www/html/asg/MEFM/0/2/DCEFEP /var/www/html/crs/1653

mv /var/www/swad/asg/MEFM/0/2/HAF /var/www/swad/crs/1654
mv /var/www/html/asg/MEFM/0/2/HAF /var/www/html/crs/1654

mv /var/www/swad/asg/MEFM/0/2/PREFD /var/www/swad/crs/1655
mv /var/www/html/asg/MEFM/0/2/PREFD /var/www/html/crs/1655

mv /var/www/swad/asg/MEFM/2/0/DEF /var/www/swad/crs/1656
mv /var/www/html/asg/MEFM/2/0/DEF /var/www/html/crs/1656

mv /var/www/swad/asg/MEFM/2/1/ECC /var/www/swad/crs/1657
mv /var/www/html/asg/MEFM/2/1/ECC /var/www/html/crs/1657

mv /var/www/swad/asg/MEFM/2/2/TPAF /var/www/swad/crs/1658
mv /var/www/html/asg/MEFM/2/2/TPAF /var/www/html/crs/1658

mv /var/www/swad/asg/MEFM/3/1/JMEF /var/www/swad/crs/1659
mv /var/www/html/asg/MEFM/3/1/JMEF /var/www/html/crs/1659

mv /var/www/swad/asg/MEFM/3/2/P /var/www/swad/crs/1660
mv /var/www/html/asg/MEFM/3/2/P /var/www/html/crs/1660

mv /var/www/swad/asg/MEFM/3/2/PE /var/www/swad/crs/1661
mv /var/www/html/asg/MEFM/3/2/PE /var/www/html/crs/1661

mv /var/www/swad/asg/MEI/1/0/DGA /var/www/swad/crs/1662
mv /var/www/html/asg/MEI/1/0/DGA /var/www/html/crs/1662

mv /var/www/swad/asg/MEI/1/2/EMIC /var/www/swad/crs/1663
mv /var/www/html/asg/MEI/1/2/EMIC /var/www/html/crs/1663

mv /var/www/swad/asg/MEI/2/0/BPEEA /var/www/swad/crs/1664
mv /var/www/html/asg/MEI/2/0/BPEEA /var/www/html/crs/1664

mv /var/www/swad/asg/MEI/2/1/DEMDA /var/www/swad/crs/1665
mv /var/www/html/asg/MEI/2/1/DEMDA /var/www/html/crs/1665

mv /var/www/swad/asg/MEI/2/2/EMIT /var/www/swad/crs/1666
mv /var/www/html/asg/MEI/2/2/EMIT /var/www/html/crs/1666

mv /var/www/swad/asg/MEI/2/1/FDEEI /var/www/swad/crs/1667
mv /var/www/html/asg/MEI/2/1/FDEEI /var/www/html/crs/1667

mv /var/www/swad/asg/MEI/2/1/HIE /var/www/swad/crs/1668
mv /var/www/html/asg/MEI/2/1/HIE /var/www/html/crs/1668

mv /var/www/swad/asg/MEI/3/1/LI /var/www/swad/crs/1669
mv /var/www/html/asg/MEI/3/1/LI /var/www/html/crs/1669

mv /var/www/swad/asg/MEI/3/1/NTAE /var/www/swad/crs/1670
mv /var/www/html/asg/MEI/3/1/NTAE /var/www/html/crs/1670

mv /var/www/swad/asg/MEI/3/1/NTAEA /var/www/swad/crs/1671
mv /var/www/html/asg/MEI/3/1/NTAEA /var/www/html/crs/1671

mv /var/www/swad/asg/MEI/3/1/NTAEC /var/www/swad/crs/1672
mv /var/www/html/asg/MEI/3/1/NTAEC /var/www/html/crs/1672

mv /var/www/swad/asg/MEI/3/2/P1 /var/www/swad/crs/1673
mv /var/www/html/asg/MEI/3/2/P1 /var/www/html/crs/1673

mv /var/www/swad/asg/MEI/3/2/PM2 /var/www/swad/crs/1674
mv /var/www/html/asg/MEI/3/2/PM2 /var/www/html/crs/1674

mv /var/www/swad/asg/MEI/3/1/SEA /var/www/swad/crs/1675
mv /var/www/html/asg/MEI/3/1/SEA /var/www/html/crs/1675

mv /var/www/swad/asg/MEI/3/1/SE /var/www/swad/crs/1676
mv /var/www/html/asg/MEI/3/1/SE /var/www/html/crs/1676

mv /var/www/swad/asg/MEI/3/1/TI /var/www/swad/crs/1677
mv /var/www/html/asg/MEI/3/1/TI /var/www/html/crs/1677

mv /var/www/swad/asg/MEIC/1/2/LI /var/www/swad/crs/1678
mv /var/www/html/asg/MEIC/1/2/LI /var/www/html/crs/1678

mv /var/www/swad/asg/MEIC/2/0/DHLD /var/www/swad/crs/1679
mv /var/www/html/asg/MEIC/2/0/DHLD /var/www/html/crs/1679

mv /var/www/swad/asg/MEIC/3/1/JDTI /var/www/swad/crs/1680
mv /var/www/html/asg/MEIC/3/1/JDTI /var/www/html/crs/1680

mv /var/www/swad/asg/MEIM/3/1/EIEEI /var/www/swad/crs/1681
mv /var/www/html/asg/MEIM/3/1/EIEEI /var/www/html/crs/1681

mv /var/www/swad/asg/MEM/0/1/AEM /var/www/swad/crs/1682
mv /var/www/html/asg/MEM/0/1/AEM /var/www/html/crs/1682

mv /var/www/swad/asg/MEM/1/0/DGA /var/www/swad/crs/1683
mv /var/www/html/asg/MEM/1/0/DGA /var/www/html/crs/1683

mv /var/www/swad/asg/MEM/1/0/FI /var/www/swad/crs/1684
mv /var/www/html/asg/MEM/1/0/FI /var/www/html/crs/1684

mv /var/www/swad/asg/MEM/1/2/FRD /var/www/swad/crs/1685
mv /var/www/html/asg/MEM/1/2/FRD /var/www/html/crs/1685

mv /var/www/swad/asg/MEM/1/2/MRMDM /var/www/swad/crs/1686
mv /var/www/html/asg/MEM/1/2/MRMDM /var/www/html/crs/1686

mv /var/www/swad/asg/MEM/2/0/AI /var/www/swad/crs/1687
mv /var/www/html/asg/MEM/2/0/AI /var/www/html/crs/1687

mv /var/www/swad/asg/MEM/2/1/LLD /var/www/swad/crs/1688
mv /var/www/html/asg/MEM/2/1/LLD /var/www/html/crs/1688

mv /var/www/swad/asg/MEM/2/2/MICMNT /var/www/swad/crs/1689
mv /var/www/html/asg/MEM/2/2/MICMNT /var/www/html/crs/1689

mv /var/www/swad/asg/MEM/3/1/DEMM /var/www/swad/crs/1690
mv /var/www/html/asg/MEM/3/1/DEMM /var/www/html/crs/1690

mv /var/www/swad/asg/MEM/3/1/NTAE /var/www/swad/crs/1691
mv /var/www/html/asg/MEM/3/1/NTAE /var/www/html/crs/1691

mv /var/www/swad/asg/MEM/3/2/P2A /var/www/swad/crs/1692
mv /var/www/html/asg/MEM/3/2/P2A /var/www/html/crs/1692

mv /var/www/swad/asg/MEM/3/2/PM2 /var/www/swad/crs/1693
mv /var/www/html/asg/MEM/3/2/PM2 /var/www/html/crs/1693

mv /var/www/swad/asg/MEMM/2/2/EFD /var/www/swad/crs/1694
mv /var/www/html/asg/MEMM/2/2/EFD /var/www/html/crs/1694

mv /var/www/swad/asg/MEP/1/1/EADB /var/www/swad/crs/1695
mv /var/www/html/asg/MEP/1/1/EADB /var/www/html/crs/1695

mv /var/www/swad/asg/MEP/1/1/EADC /var/www/swad/crs/1696
mv /var/www/html/asg/MEP/1/1/EADC /var/www/html/crs/1696

mv /var/www/swad/asg/MEP/1/1/EADD /var/www/swad/crs/1697
mv /var/www/html/asg/MEP/1/1/EADD /var/www/html/crs/1697

mv /var/www/swad/asg/MEP/2/0/BPEEA /var/www/swad/crs/1698
mv /var/www/html/asg/MEP/2/0/BPEEA /var/www/html/crs/1698

mv /var/www/swad/asg/MEP/2/0/BPEEC /var/www/swad/crs/1699
mv /var/www/html/asg/MEP/2/0/BPEEC /var/www/html/crs/1699

mv /var/www/swad/asg/MEP/2/0/BPEE /var/www/swad/crs/1700
mv /var/www/html/asg/MEP/2/0/BPEE /var/www/html/crs/1700

mv /var/www/swad/asg/MEP/2/0/LLDC /var/www/swad/crs/1701
mv /var/www/html/asg/MEP/2/0/LLDC /var/www/html/crs/1701

mv /var/www/swad/asg/MEP/2/1/OCEC /var/www/swad/crs/1702
mv /var/www/html/asg/MEP/2/1/OCEC /var/www/html/crs/1702

mv /var/www/swad/asg/MEP/2/1/OGCE /var/www/swad/crs/1703
mv /var/www/html/asg/MEP/2/1/OGCE /var/www/html/crs/1703

mv /var/www/swad/asg/MEP/3/2/IEDIA /var/www/swad/crs/1704
mv /var/www/html/asg/MEP/3/2/IEDIA /var/www/html/crs/1704

mv /var/www/swad/asg/MEP/3/2/NTAE /var/www/swad/crs/1705
mv /var/www/html/asg/MEP/3/2/NTAE /var/www/html/crs/1705

mv /var/www/swad/asg/MEP/3/2/NTAEA /var/www/swad/crs/1706
mv /var/www/html/asg/MEP/3/2/NTAEA /var/www/html/crs/1706

mv /var/www/swad/asg/MEP/3/2/NTAEC /var/www/swad/crs/1707
mv /var/www/html/asg/MEP/3/2/NTAEC /var/www/html/crs/1707

mv /var/www/swad/asg/MEP/3/2/NTAED /var/www/swad/crs/1708
mv /var/www/html/asg/MEP/3/2/NTAED /var/www/html/crs/1708

mv /var/www/swad/asg/MEP/3/2/PM2A /var/www/swad/crs/1709
mv /var/www/html/asg/MEP/3/2/PM2A /var/www/html/crs/1709

mv /var/www/swad/asg/MEP/3/2/PM2B /var/www/swad/crs/1710
mv /var/www/html/asg/MEP/3/2/PM2B /var/www/html/crs/1710

mv /var/www/swad/asg/MEP/3/2/PM2C /var/www/swad/crs/1711
mv /var/www/html/asg/MEP/3/2/PM2C /var/www/html/crs/1711

mv /var/www/swad/asg/MEP/3/2/PM2D /var/www/swad/crs/1712
mv /var/www/html/asg/MEP/3/2/PM2D /var/www/html/crs/1712

mv /var/www/swad/asg/MEP/3/2/PM2E /var/www/swad/crs/1713
mv /var/www/html/asg/MEP/3/2/PM2E /var/www/html/crs/1713

mv /var/www/swad/asg/MEP/3/2/PM2F /var/www/swad/crs/1714
mv /var/www/html/asg/MEP/3/2/PM2F /var/www/html/crs/1714

mv /var/www/swad/asg/MEP/3/2/PM2G /var/www/swad/crs/1715
mv /var/www/html/asg/MEP/3/2/PM2G /var/www/html/crs/1715

mv /var/www/swad/asg/MEP/3/2/PM2H /var/www/swad/crs/1716
mv /var/www/html/asg/MEP/3/2/PM2H /var/www/html/crs/1716

mv /var/www/swad/asg/MEP/3/2/PM2I /var/www/swad/crs/1717
mv /var/www/html/asg/MEP/3/2/PM2I /var/www/html/crs/1717

mv /var/www/swad/asg/MEP/3/2/SE /var/www/swad/crs/1718
mv /var/www/html/asg/MEP/3/2/SE /var/www/html/crs/1718

mv /var/www/swad/asg/MEPM/2/2/EFD /var/www/swad/crs/1719
mv /var/www/html/asg/MEPM/2/2/EFD /var/www/html/crs/1719

mv /var/www/swad/asg/MLE/1/1/CA /var/www/swad/crs/1720
mv /var/www/html/asg/MLE/1/1/CA /var/www/html/crs/1720

mv /var/www/swad/asg/MLE/1/1/EADA /var/www/swad/crs/1721
mv /var/www/html/asg/MLE/1/1/EADA /var/www/html/crs/1721

mv /var/www/swad/asg/MLE/1/1/LEC /var/www/swad/crs/1722
mv /var/www/html/asg/MLE/1/1/LEC /var/www/html/crs/1722

mv /var/www/swad/asg/MLE/1/2/IEIC /var/www/swad/crs/1723
mv /var/www/html/asg/MLE/1/2/IEIC /var/www/html/crs/1723

mv /var/www/swad/asg/MLE/1/2/IEIT /var/www/swad/crs/1724
mv /var/www/html/asg/MLE/1/2/IEIT /var/www/html/crs/1724

mv /var/www/swad/asg/MLE/1/0/PEDEE /var/www/swad/crs/1725
mv /var/www/html/asg/MLE/1/0/PEDEE /var/www/html/crs/1725

mv /var/www/swad/asg/MLE/2/0/BPEEA /var/www/swad/crs/1726
mv /var/www/html/asg/MLE/2/0/BPEEA /var/www/html/crs/1726

mv /var/www/swad/asg/MLE/2/2/DIEF /var/www/swad/crs/1727
mv /var/www/html/asg/MLE/2/2/DIEF /var/www/html/crs/1727

mv /var/www/swad/asg/MLE/2/1/FFA /var/www/swad/crs/1728
mv /var/www/html/asg/MLE/2/1/FFA /var/www/html/crs/1728

mv /var/www/swad/asg/MLE/2/1/FIAT /var/www/swad/crs/1729
mv /var/www/html/asg/MLE/2/1/FIAT /var/www/html/crs/1729

mv /var/www/swad/asg/MLE/2/1/FIC /var/www/swad/crs/1730
mv /var/www/html/asg/MLE/2/1/FIC /var/www/html/crs/1730

mv /var/www/swad/asg/MLE/2/1/MSFA /var/www/swad/crs/1731
mv /var/www/html/asg/MLE/2/1/MSFA /var/www/html/crs/1731

mv /var/www/swad/asg/MLE/2/1/OCEA /var/www/swad/crs/1732
mv /var/www/html/asg/MLE/2/1/OCEA /var/www/html/crs/1732

mv /var/www/swad/asg/MLE/2/1/OCEC /var/www/swad/crs/1733
mv /var/www/html/asg/MLE/2/1/OCEC /var/www/html/crs/1733

mv /var/www/swad/asg/MLE/3/2/DLF /var/www/swad/crs/1734
mv /var/www/html/asg/MLE/3/2/DLF /var/www/html/crs/1734

mv /var/www/swad/asg/MLE/3/2/NTAEC /var/www/swad/crs/1735
mv /var/www/html/asg/MLE/3/2/NTAEC /var/www/html/crs/1735

mv /var/www/swad/asg/MLE/3/2/NTAE /var/www/swad/crs/1736
mv /var/www/html/asg/MLE/3/2/NTAE /var/www/html/crs/1736

mv /var/www/swad/asg/MLEM/2/2/EFD /var/www/swad/crs/1737
mv /var/www/html/asg/MLEM/2/2/EFD /var/www/html/crs/1737

mv /var/www/swad/asg/MLEM/3/1/LII /var/www/swad/crs/1738
mv /var/www/html/asg/MLEM/3/1/LII /var/www/html/crs/1738

mv /var/www/swad/asg/MAC/1/2/B /var/www/swad/crs/1739
mv /var/www/html/asg/MAC/1/2/B /var/www/html/crs/1739

mv /var/www/swad/asg/MAT/1/0/MAT /var/www/swad/crs/1740
mv /var/www/html/asg/MAT/1/0/MAT /var/www/html/crs/1740

mv /var/www/swad/asg/MDAUA/0/0/EDUA /var/www/swad/crs/1741
mv /var/www/html/asg/MDAUA/0/0/EDUA /var/www/html/crs/1741

mv /var/www/swad/asg/MDCE/1/0/UEDF /var/www/swad/crs/1742
mv /var/www/html/asg/MDCE/1/0/UEDF /var/www/html/crs/1742

mv /var/www/swad/asg/MDC/1/0/MDC /var/www/swad/crs/1743
mv /var/www/html/asg/MDC/1/0/MDC /var/www/html/crs/1743

mv /var/www/swad/asg/MDC/1/0/MDC2 /var/www/swad/crs/1744
mv /var/www/html/asg/MDC/1/0/MDC2 /var/www/html/crs/1744

mv /var/www/swad/asg/MDC/1/0/MDC3 /var/www/swad/crs/1745
mv /var/www/html/asg/MDC/1/0/MDC3 /var/www/html/crs/1745

mv /var/www/swad/asg/MEO/1/0/MEO /var/www/swad/crs/1746
mv /var/www/html/asg/MEO/1/0/MEO /var/www/html/crs/1746

mv /var/www/swad/asg/MEMDIS/0/0/ANAPS /var/www/swad/crs/1747
mv /var/www/html/asg/MEMDIS/0/0/ANAPS /var/www/html/crs/1747

mv /var/www/swad/asg/MEMDIS/0/0/ANTPS /var/www/swad/crs/1748
mv /var/www/html/asg/MEMDIS/0/0/ANTPS /var/www/html/crs/1748

mv /var/www/swad/asg/MEMDIS/0/0/AR /var/www/swad/crs/1749
mv /var/www/html/asg/MEMDIS/0/0/AR /var/www/html/crs/1749

mv /var/www/swad/asg/MEMDIS/0/0/DI /var/www/swad/crs/1750
mv /var/www/html/asg/MEMDIS/0/0/DI /var/www/html/crs/1750

mv /var/www/swad/asg/MEMDIS/0/0/DCDA /var/www/swad/crs/1751
mv /var/www/html/asg/MEMDIS/0/0/DCDA /var/www/html/crs/1751

mv /var/www/swad/asg/MEMDIS/0/0/DGIS /var/www/swad/crs/1752
mv /var/www/html/asg/MEMDIS/0/0/DGIS /var/www/html/crs/1752

mv /var/www/swad/asg/MEMDIS/0/0/DSA /var/www/swad/crs/1753
mv /var/www/html/asg/MEMDIS/0/0/DSA /var/www/html/crs/1753

mv /var/www/swad/asg/MEMDIS/0/0/DDE /var/www/swad/crs/1754
mv /var/www/html/asg/MEMDIS/0/0/DDE /var/www/html/crs/1754

mv /var/www/swad/asg/MEMDIS/0/0/DGTS /var/www/swad/crs/1755
mv /var/www/html/asg/MEMDIS/0/0/DGTS /var/www/html/crs/1755

mv /var/www/swad/asg/MEMDIS/0/0/EBPMPM /var/www/swad/crs/1756
mv /var/www/html/asg/MEMDIS/0/0/EBPMPM /var/www/html/crs/1756

mv /var/www/swad/asg/MEMDIS/0/0/EIMPC /var/www/swad/crs/1757
mv /var/www/html/asg/MEMDIS/0/0/EIMPC /var/www/html/crs/1757

mv /var/www/swad/asg/MEMDIS/0/0/EIS /var/www/swad/crs/1758
mv /var/www/html/asg/MEMDIS/0/0/EIS /var/www/html/crs/1758

mv /var/www/swad/asg/MEMDIS/0/0/EARPAL /var/www/swad/crs/1759
mv /var/www/html/asg/MEMDIS/0/0/EARPAL /var/www/html/crs/1759

mv /var/www/swad/asg/MEMDIS/0/0/EPPE /var/www/swad/crs/1760
mv /var/www/html/asg/MEMDIS/0/0/EPPE /var/www/html/crs/1760

mv /var/www/swad/asg/MEMDIS/0/0/FME /var/www/swad/crs/1761
mv /var/www/html/asg/MEMDIS/0/0/FME /var/www/html/crs/1761

mv /var/www/swad/asg/MEMDIS/0/0/GMJ /var/www/swad/crs/1762
mv /var/www/html/asg/MEMDIS/0/0/GMJ /var/www/html/crs/1762

mv /var/www/swad/asg/MEMDIS/0/0/IDEEG /var/www/swad/crs/1763
mv /var/www/html/asg/MEMDIS/0/0/IDEEG /var/www/html/crs/1763

mv /var/www/swad/asg/MEMDIS/0/0/IISE /var/www/swad/crs/1764
mv /var/www/html/asg/MEMDIS/0/0/IISE /var/www/html/crs/1764

mv /var/www/swad/asg/MEMDIS/0/0/IED /var/www/swad/crs/1765
mv /var/www/html/asg/MEMDIS/0/0/IED /var/www/html/crs/1765

mv /var/www/swad/asg/MEMDIS/0/0/IEM /var/www/swad/crs/1766
mv /var/www/html/asg/MEMDIS/0/0/IEM /var/www/html/crs/1766

mv /var/www/swad/asg/MEMDIS/0/0/MFTS /var/www/swad/crs/1767
mv /var/www/html/asg/MEMDIS/0/0/MFTS /var/www/html/crs/1767

mv /var/www/swad/asg/MEMDIS/0/0/SSPSPI /var/www/swad/crs/1768
mv /var/www/html/asg/MEMDIS/0/0/SSPSPI /var/www/html/crs/1768

mv /var/www/swad/asg/MEMDIS/0/0/SPIS /var/www/swad/crs/1769
mv /var/www/html/asg/MEMDIS/0/0/SPIS /var/www/html/crs/1769

mv /var/www/swad/asg/MEMDIS/0/0/TSA /var/www/swad/crs/1770
mv /var/www/html/asg/MEMDIS/0/0/TSA /var/www/html/crs/1770

mv /var/www/swad/asg/MGEMMA/0/2/EFAD /var/www/swad/crs/1771
mv /var/www/html/asg/MGEMMA/0/2/EFAD /var/www/html/crs/1771

mv /var/www/swad/asg/MGEMMA/0/1/GCRAC /var/www/swad/crs/1772
mv /var/www/html/asg/MGEMMA/0/1/GCRAC /var/www/html/crs/1772

mv /var/www/swad/asg/MGEMMA/0/1/HF /var/www/swad/crs/1773
mv /var/www/html/asg/MGEMMA/0/1/HF /var/www/html/crs/1773

mv /var/www/swad/asg/MGEMMA/0/1/MF /var/www/swad/crs/1774
mv /var/www/html/asg/MGEMMA/0/1/MF /var/www/html/crs/1774

mv /var/www/swad/asg/MGEMMA/0/1/PFAS /var/www/swad/crs/1775
mv /var/www/html/asg/MGEMMA/0/1/PFAS /var/www/html/crs/1775

mv /var/www/swad/asg/MGEMMA/0/1/TF /var/www/swad/crs/1776
mv /var/www/html/asg/MGEMMA/0/1/TF /var/www/html/crs/1776

mv /var/www/swad/asg/MGEO/0/0/FMSPST /var/www/swad/crs/1777
mv /var/www/html/asg/MGEO/0/0/FMSPST /var/www/html/crs/1777

mv /var/www/swad/asg/MIC/1/0/FCEIC /var/www/swad/crs/1778
mv /var/www/html/asg/MIC/1/0/FCEIC /var/www/html/crs/1778

mv /var/www/swad/asg/MIA/1/0/CSV /var/www/swad/crs/1779
mv /var/www/html/asg/MIA/1/0/CSV /var/www/html/crs/1779

mv /var/www/swad/asg/MICR/0/0/AC /var/www/swad/crs/1780
mv /var/www/html/asg/MICR/0/0/AC /var/www/html/crs/1780

mv /var/www/swad/asg/MICR/0/0/ACAP /var/www/swad/crs/1781
mv /var/www/html/asg/MICR/0/0/ACAP /var/www/html/crs/1781

mv /var/www/swad/asg/MICR/0/0/AIAV /var/www/swad/crs/1782
mv /var/www/html/asg/MICR/0/0/AIAV /var/www/html/crs/1782

mv /var/www/swad/asg/MICR/0/0/APCC /var/www/swad/crs/1783
mv /var/www/html/asg/MICR/0/0/APCC /var/www/html/crs/1783

mv /var/www/swad/asg/MICR/0/0/CAPB /var/www/swad/crs/1784
mv /var/www/html/asg/MICR/0/0/CAPB /var/www/html/crs/1784

mv /var/www/swad/asg/MICR/0/0/CAPOP /var/www/swad/crs/1785
mv /var/www/html/asg/MICR/0/0/CAPOP /var/www/html/crs/1785

mv /var/www/swad/asg/MICR/0/0/CI /var/www/swad/crs/1786
mv /var/www/html/asg/MICR/0/0/CI /var/www/html/crs/1786

mv /var/www/swad/asg/MICR/0/0/HCDHR /var/www/swad/crs/1787
mv /var/www/html/asg/MICR/0/0/HCDHR /var/www/html/crs/1787

mv /var/www/swad/asg/MICR/0/0/HMM /var/www/swad/crs/1788
mv /var/www/html/asg/MICR/0/0/HMM /var/www/html/crs/1788

mv /var/www/swad/asg/MICR/0/0/IN /var/www/swad/crs/1789
mv /var/www/html/asg/MICR/0/0/IN /var/www/html/crs/1789

mv /var/www/swad/asg/MICR/0/0/ISO /var/www/swad/crs/1790
mv /var/www/html/asg/MICR/0/0/ISO /var/www/html/crs/1790

mv /var/www/swad/asg/MICR/0/0/OCSAP /var/www/swad/crs/1791
mv /var/www/html/asg/MICR/0/0/OCSAP /var/www/html/crs/1791

mv /var/www/swad/asg/MICR/0/0/PAIPC /var/www/swad/crs/1792
mv /var/www/html/asg/MICR/0/0/PAIPC /var/www/html/crs/1792

mv /var/www/swad/asg/MICR/0/0/RS /var/www/swad/crs/1793
mv /var/www/html/asg/MICR/0/0/RS /var/www/html/crs/1793

mv /var/www/swad/asg/MICR/0/0/SD /var/www/swad/crs/1794
mv /var/www/html/asg/MICR/0/0/SD /var/www/html/crs/1794

mv /var/www/swad/asg/MICR/0/0/SS /var/www/swad/crs/1795
mv /var/www/html/asg/MICR/0/0/SS /var/www/html/crs/1795

mv /var/www/swad/asg/MMP/0/0/M /var/www/swad/crs/1796
mv /var/www/html/asg/MMP/0/0/M /var/www/html/crs/1796

mv /var/www/swad/asg/MMP/0/0/EP /var/www/swad/crs/1797
mv /var/www/html/asg/MMP/0/0/EP /var/www/html/crs/1797

mv /var/www/swad/asg/MMP/0/0/E /var/www/swad/crs/1798
mv /var/www/html/asg/MMP/0/0/E /var/www/html/crs/1798

mv /var/www/swad/asg/MMP/0/0/EE /var/www/swad/crs/1799
mv /var/www/html/asg/MMP/0/0/EE /var/www/html/crs/1799

mv /var/www/swad/asg/MMP/0/0/AP /var/www/swad/crs/1800
mv /var/www/html/asg/MMP/0/0/AP /var/www/html/crs/1800

mv /var/www/swad/asg/MMP/0/0/CE /var/www/swad/crs/1801
mv /var/www/html/asg/MMP/0/0/CE /var/www/html/crs/1801

mv /var/www/swad/asg/MMP/0/0/PI /var/www/swad/crs/1802
mv /var/www/html/asg/MMP/0/0/PI /var/www/html/crs/1802

mv /var/www/swad/asg/MMP/0/0/OC /var/www/swad/crs/1803
mv /var/www/html/asg/MMP/0/0/OC /var/www/html/crs/1803

mv /var/www/swad/asg/MMP/0/0/DP /var/www/swad/crs/1804
mv /var/www/html/asg/MMP/0/0/DP /var/www/html/crs/1804

mv /var/www/swad/asg/MMP/0/0/PMC /var/www/swad/crs/1805
mv /var/www/html/asg/MMP/0/0/PMC /var/www/html/crs/1805

mv /var/www/swad/asg/MMP/0/0/GR /var/www/swad/crs/1806
mv /var/www/html/asg/MMP/0/0/GR /var/www/html/crs/1806

mv /var/www/swad/asg/MMP/0/0/GCGL /var/www/swad/crs/1807
mv /var/www/html/asg/MMP/0/0/GCGL /var/www/html/crs/1807

mv /var/www/swad/asg/MLLI/0/0/ALL /var/www/swad/crs/1808
mv /var/www/html/asg/MLLI/0/0/ALL /var/www/html/crs/1808

mv /var/www/swad/asg/MLLI/0/0/EAI /var/www/swad/crs/1809
mv /var/www/html/asg/MLLI/0/0/EAI /var/www/html/crs/1809

mv /var/www/swad/asg/MLLI/0/0/AACIEP /var/www/swad/crs/1810
mv /var/www/html/asg/MLLI/0/0/AACIEP /var/www/html/crs/1810

mv /var/www/swad/asg/MLLI/0/0/CPAEL /var/www/swad/crs/1811
mv /var/www/html/asg/MLLI/0/0/CPAEL /var/www/html/crs/1811

mv /var/www/swad/asg/MLLI/0/0/CLIA30 /var/www/swad/crs/1812
mv /var/www/html/asg/MLLI/0/0/CLIA30 /var/www/html/crs/1812

mv /var/www/swad/asg/MLLI/0/0/EFAD /var/www/swad/crs/1813
mv /var/www/html/asg/MLLI/0/0/EFAD /var/www/html/crs/1813

mv /var/www/swad/asg/MLLI/0/0/EAL /var/www/swad/crs/1814
mv /var/www/html/asg/MLLI/0/0/EAL /var/www/html/crs/1814

mv /var/www/swad/asg/MLLI/0/0/EINOH /var/www/swad/crs/1815
mv /var/www/html/asg/MLLI/0/0/EINOH /var/www/html/crs/1815

mv /var/www/swad/asg/MLLI/0/0/FSALI /var/www/swad/crs/1816
mv /var/www/html/asg/MLLI/0/0/FSALI /var/www/html/crs/1816

mv /var/www/swad/asg/MLLI/0/0/IT /var/www/swad/crs/1817
mv /var/www/html/asg/MLLI/0/0/IT /var/www/html/crs/1817

mv /var/www/swad/asg/MLLI/0/0/IAD /var/www/swad/crs/1818
mv /var/www/html/asg/MLLI/0/0/IAD /var/www/html/crs/1818

mv /var/www/swad/asg/MLLI/0/0/LGAC /var/www/swad/crs/1819
mv /var/www/html/asg/MLLI/0/0/LGAC /var/www/html/crs/1819

mv /var/www/swad/asg/MLLI/0/0/MIALA /var/www/swad/crs/1820
mv /var/www/html/asg/MLLI/0/0/MIALA /var/www/html/crs/1820

mv /var/www/swad/asg/MLLI/0/0/NP /var/www/swad/crs/1821
mv /var/www/html/asg/MLLI/0/0/NP /var/www/html/crs/1821

mv /var/www/swad/asg/MLLI/0/0/PDU /var/www/swad/crs/1822
mv /var/www/html/asg/MLLI/0/0/PDU /var/www/html/crs/1822

mv /var/www/swad/asg/MMPSP/0/0/AS /var/www/swad/crs/1823
mv /var/www/html/asg/MMPSP/0/0/AS /var/www/html/crs/1823

mv /var/www/swad/asg/MMPSP/0/0/EPET /var/www/swad/crs/1824
mv /var/www/html/asg/MMPSP/0/0/EPET /var/www/html/crs/1824

mv /var/www/swad/asg/MMPSP/0/0/PS /var/www/swad/crs/1825
mv /var/www/html/asg/MMPSP/0/0/PS /var/www/html/crs/1825

mv /var/www/swad/asg/MMTAF/0/0/DEADT /var/www/swad/crs/1826
mv /var/www/html/asg/MMTAF/0/0/DEADT /var/www/html/crs/1826

mv /var/www/swad/asg/MM/0/0/ET /var/www/swad/crs/1827
mv /var/www/html/asg/MM/0/0/ET /var/www/html/crs/1827

mv /var/www/swad/asg/MM/0/0/MAC /var/www/swad/crs/1828
mv /var/www/html/asg/MM/0/0/MAC /var/www/html/crs/1828

mv /var/www/swad/asg/MM/0/0/MAI /var/www/swad/crs/1829
mv /var/www/html/asg/MM/0/0/MAI /var/www/html/crs/1829

mv /var/www/swad/asg/MM/0/0/MCM /var/www/swad/crs/1830
mv /var/www/html/asg/MM/0/0/MCM /var/www/html/crs/1830

mv /var/www/swad/asg/MM/0/0/MM /var/www/swad/crs/1831
mv /var/www/html/asg/MM/0/0/MM /var/www/html/crs/1831

mv /var/www/swad/asg/MM/0/0/MSF /var/www/swad/crs/1832
mv /var/www/html/asg/MM/0/0/MSF /var/www/html/crs/1832

mv /var/www/swad/asg/MM/0/0/ME /var/www/swad/crs/1833
mv /var/www/html/asg/MM/0/0/ME /var/www/html/crs/1833

mv /var/www/swad/asg/MM/1/0/CFHM /var/www/swad/crs/1834
mv /var/www/html/asg/MM/1/0/CFHM /var/www/html/crs/1834

mv /var/www/swad/asg/MM/1/0/LM /var/www/swad/crs/1835
mv /var/www/html/asg/MM/1/0/LM /var/www/html/crs/1835

mv /var/www/swad/asg/MM/1/0/TMAM /var/www/swad/crs/1836
mv /var/www/html/asg/MM/1/0/TMAM /var/www/html/crs/1836

mv /var/www/swad/asg/MM/1/0/PM /var/www/swad/crs/1837
mv /var/www/html/asg/MM/1/0/PM /var/www/html/crs/1837

mv /var/www/swad/asg/MM/1/0/AM /var/www/swad/crs/1838
mv /var/www/html/asg/MM/1/0/AM /var/www/html/crs/1838

mv /var/www/swad/asg/MM/1/0/D /var/www/swad/crs/1839
mv /var/www/html/asg/MM/1/0/D /var/www/html/crs/1839

mv /var/www/swad/asg/MM/1/0/C /var/www/swad/crs/1840
mv /var/www/html/asg/MM/1/0/C /var/www/html/crs/1840

mv /var/www/swad/asg/MM/2/0/CE /var/www/swad/crs/1841
mv /var/www/html/asg/MM/2/0/CE /var/www/html/crs/1841

mv /var/www/swad/asg/MM/2/0/SE /var/www/swad/crs/1842
mv /var/www/html/asg/MM/2/0/SE /var/www/html/crs/1842

mv /var/www/swad/asg/MM/2/0/SI /var/www/swad/crs/1843
mv /var/www/html/asg/MM/2/0/SI /var/www/html/crs/1843

mv /var/www/swad/asg/MM/2/0/GM /var/www/swad/crs/1844
mv /var/www/html/asg/MM/2/0/GM /var/www/html/crs/1844

mv /var/www/swad/asg/MM/2/0/AGREH /var/www/swad/crs/1845
mv /var/www/html/asg/MM/2/0/AGREH /var/www/html/crs/1845

mv /var/www/swad/asg/MNH/0/0/E /var/www/swad/crs/1846
mv /var/www/html/asg/MNH/0/0/E /var/www/html/crs/1846

mv /var/www/swad/asg/MPC/1/0/PE /var/www/swad/crs/1847
mv /var/www/html/asg/MPC/1/0/PE /var/www/html/crs/1847

mv /var/www/swad/asg/MPS/0/0/DSC /var/www/swad/crs/1848
mv /var/www/html/asg/MPS/0/0/DSC /var/www/html/crs/1848

mv /var/www/swad/asg/MQ/1/0/EAA /var/www/swad/crs/1849
mv /var/www/html/asg/MQ/1/0/EAA /var/www/html/crs/1849

mv /var/www/swad/asg/MTI/1/0/GT /var/www/swad/crs/1850
mv /var/www/html/asg/MTI/1/0/GT /var/www/html/crs/1850

mv /var/www/swad/asg/MTI/1/1/HIITI /var/www/swad/crs/1851
mv /var/www/html/asg/MTI/1/1/HIITI /var/www/html/crs/1851

mv /var/www/swad/asg/MTI/1/0/ICEFDC /var/www/swad/crs/1852
mv /var/www/html/asg/MTI/1/0/ICEFDC /var/www/html/crs/1852

mv /var/www/swad/asg/MTI/1/1/TCTI /var/www/swad/crs/1853
mv /var/www/html/asg/MTI/1/1/TCTI /var/www/html/crs/1853

mv /var/www/swad/asg/MTI/1/0/TCT /var/www/swad/crs/1854
mv /var/www/html/asg/MTI/1/0/TCT /var/www/html/crs/1854

mv /var/www/swad/asg/MEIG/1/0/CG /var/www/swad/crs/1855
mv /var/www/html/asg/MEIG/1/0/CG /var/www/html/crs/1855

mv /var/www/swad/asg/MEIG/1/0/MOC /var/www/swad/crs/1856
mv /var/www/html/asg/MEIG/1/0/MOC /var/www/html/crs/1856

mv /var/www/swad/asg/PDDH/1/2/M1 /var/www/swad/crs/1857
mv /var/www/html/asg/PDDH/1/2/M1 /var/www/html/crs/1857

mv /var/www/swad/asg/PATC/0/0/FGM /var/www/swad/crs/1858
mv /var/www/html/asg/PATC/0/0/FGM /var/www/html/crs/1858

mv /var/www/swad/asg/PID/0/0/SWAD /var/www/swad/crs/1859
mv /var/www/html/asg/PID/0/0/SWAD /var/www/html/crs/1859

mv /var/www/swad/asg/PID/0/0/SECPE /var/www/swad/crs/1860
mv /var/www/html/asg/PID/0/0/SECPE /var/www/html/crs/1860

mv /var/www/swad/asg/PID/0/0/TAEVS /var/www/swad/crs/1861
mv /var/www/html/asg/PID/0/0/TAEVS /var/www/html/crs/1861

mv /var/www/swad/asg/GIBH/0/0/FIPPD /var/www/swad/crs/1862
mv /var/www/html/asg/GIBH/0/0/FIPPD /var/www/html/crs/1862

mv /var/www/swad/asg/GIBH/0/0/IBH /var/www/swad/crs/1863
mv /var/www/html/asg/GIBH/0/0/IBH /var/www/html/crs/1863

mv /var/www/swad/asg/PV/0/0/PV /var/www/swad/crs/1864
mv /var/www/html/asg/PV/0/0/PV /var/www/html/crs/1864

mv /var/www/swad/asg/DEMO/1/0/DEMO00 /var/www/swad/crs/1865
mv /var/www/html/asg/DEMO/1/0/DEMO00 /var/www/html/crs/1865

mv /var/www/swad/asg/DEMO/1/0/DEMO01 /var/www/swad/crs/1866
mv /var/www/html/asg/DEMO/1/0/DEMO01 /var/www/html/crs/1866

mv /var/www/swad/asg/DEMO/1/0/DEMO02 /var/www/swad/crs/1867
mv /var/www/html/asg/DEMO/1/0/DEMO02 /var/www/html/crs/1867

mv /var/www/swad/asg/DEMO/1/0/DEMO03 /var/www/swad/crs/1868
mv /var/www/html/asg/DEMO/1/0/DEMO03 /var/www/html/crs/1868

mv /var/www/swad/asg/DEMO/1/0/DEMO04 /var/www/swad/crs/1869
mv /var/www/html/asg/DEMO/1/0/DEMO04 /var/www/html/crs/1869

mv /var/www/swad/asg/DEMO/1/0/DEMO05 /var/www/swad/crs/1870
mv /var/www/html/asg/DEMO/1/0/DEMO05 /var/www/html/crs/1870

mv /var/www/swad/asg/DEMO/1/0/DEMO06 /var/www/swad/crs/1871
mv /var/www/html/asg/DEMO/1/0/DEMO06 /var/www/html/crs/1871

mv /var/www/swad/asg/DEMO/1/0/DEMO07 /var/www/swad/crs/1872
mv /var/www/html/asg/DEMO/1/0/DEMO07 /var/www/html/crs/1872

mv /var/www/swad/asg/DEMO/1/0/DEMO08 /var/www/swad/crs/1873
mv /var/www/html/asg/DEMO/1/0/DEMO08 /var/www/html/crs/1873

mv /var/www/swad/asg/DEMO/1/0/DEMO09 /var/www/swad/crs/1874
mv /var/www/html/asg/DEMO/1/0/DEMO09 /var/www/html/crs/1874

mv /var/www/swad/asg/DEMO/1/0/DEMO10 /var/www/swad/crs/1875
mv /var/www/html/asg/DEMO/1/0/DEMO10 /var/www/html/crs/1875

mv /var/www/swad/asg/DEMO/1/0/DEMO11 /var/www/swad/crs/1876
mv /var/www/html/asg/DEMO/1/0/DEMO11 /var/www/html/crs/1876

mv /var/www/swad/asg/DEMO/1/0/DEMO12 /var/www/swad/crs/1877
mv /var/www/html/asg/DEMO/1/0/DEMO12 /var/www/html/crs/1877

mv /var/www/swad/asg/DEMO/1/0/DEMO13 /var/www/swad/crs/1878
mv /var/www/html/asg/DEMO/1/0/DEMO13 /var/www/html/crs/1878

mv /var/www/swad/asg/DEMO/1/0/DEMO14 /var/www/swad/crs/1879
mv /var/www/html/asg/DEMO/1/0/DEMO14 /var/www/html/crs/1879

mv /var/www/swad/asg/DEMO/1/0/DEMO15 /var/www/swad/crs/1880
mv /var/www/html/asg/DEMO/1/0/DEMO15 /var/www/html/crs/1880

mv /var/www/swad/asg/DEMO/1/0/DEMO16 /var/www/swad/crs/1881
mv /var/www/html/asg/DEMO/1/0/DEMO16 /var/www/html/crs/1881

mv /var/www/swad/asg/DEMO/1/0/DEMO17 /var/www/swad/crs/1882
mv /var/www/html/asg/DEMO/1/0/DEMO17 /var/www/html/crs/1882

mv /var/www/swad/asg/DEMO/1/0/DEMO18 /var/www/swad/crs/1883
mv /var/www/html/asg/DEMO/1/0/DEMO18 /var/www/html/crs/1883

mv /var/www/swad/asg/DEMO/1/0/DEMO19 /var/www/swad/crs/1884
mv /var/www/html/asg/DEMO/1/0/DEMO19 /var/www/html/crs/1884

mv /var/www/swad/asg/DEMO/2/0/DEMO20 /var/www/swad/crs/1885
mv /var/www/html/asg/DEMO/2/0/DEMO20 /var/www/html/crs/1885

mv /var/www/swad/asg/DEMO/2/0/DEMO21 /var/www/swad/crs/1886
mv /var/www/html/asg/DEMO/2/0/DEMO21 /var/www/html/crs/1886

mv /var/www/swad/asg/DEMO/2/0/DEMO22 /var/www/swad/crs/1887
mv /var/www/html/asg/DEMO/2/0/DEMO22 /var/www/html/crs/1887

mv /var/www/swad/asg/DEMO/2/0/DEMO23 /var/www/swad/crs/1888
mv /var/www/html/asg/DEMO/2/0/DEMO23 /var/www/html/crs/1888

mv /var/www/swad/asg/DEMO/2/0/DEMO24 /var/www/swad/crs/1889
mv /var/www/html/asg/DEMO/2/0/DEMO24 /var/www/html/crs/1889

mv /var/www/swad/asg/DEMO/2/0/DEMO25 /var/www/swad/crs/1890
mv /var/www/html/asg/DEMO/2/0/DEMO25 /var/www/html/crs/1890

mv /var/www/swad/asg/DEMO/2/0/DEMO26 /var/www/swad/crs/1891
mv /var/www/html/asg/DEMO/2/0/DEMO26 /var/www/html/crs/1891

mv /var/www/swad/asg/DEMO/2/0/DEMO27 /var/www/swad/crs/1892
mv /var/www/html/asg/DEMO/2/0/DEMO27 /var/www/html/crs/1892

mv /var/www/swad/asg/DEMO/2/0/DEMO28 /var/www/swad/crs/1893
mv /var/www/html/asg/DEMO/2/0/DEMO28 /var/www/html/crs/1893

mv /var/www/swad/asg/DEMO/2/0/DEMO29 /var/www/swad/crs/1894
mv /var/www/html/asg/DEMO/2/0/DEMO29 /var/www/html/crs/1894

mv /var/www/swad/asg/DEMO/2/0/DEMO30 /var/www/swad/crs/1895
mv /var/www/html/asg/DEMO/2/0/DEMO30 /var/www/html/crs/1895

mv /var/www/swad/asg/DEMO/2/0/DEMO31 /var/www/swad/crs/1896
mv /var/www/html/asg/DEMO/2/0/DEMO31 /var/www/html/crs/1896

mv /var/www/swad/asg/DEMO/2/0/DEMO32 /var/www/swad/crs/1897
mv /var/www/html/asg/DEMO/2/0/DEMO32 /var/www/html/crs/1897

mv /var/www/swad/asg/DEMO/2/0/DEMO33 /var/www/swad/crs/1898
mv /var/www/html/asg/DEMO/2/0/DEMO33 /var/www/html/crs/1898

mv /var/www/swad/asg/DEMO/2/0/DEMO34 /var/www/swad/crs/1899
mv /var/www/html/asg/DEMO/2/0/DEMO34 /var/www/html/crs/1899

mv /var/www/swad/asg/DEMO/2/0/DEMO35 /var/www/swad/crs/1900
mv /var/www/html/asg/DEMO/2/0/DEMO35 /var/www/html/crs/1900

mv /var/www/swad/asg/DEMO/2/0/DEMO36 /var/www/swad/crs/1901
mv /var/www/html/asg/DEMO/2/0/DEMO36 /var/www/html/crs/1901

mv /var/www/swad/asg/DEMO/2/0/DEMO37 /var/www/swad/crs/1902
mv /var/www/html/asg/DEMO/2/0/DEMO37 /var/www/html/crs/1902

mv /var/www/swad/asg/DEMO/2/0/DEMO38 /var/www/swad/crs/1903
mv /var/www/html/asg/DEMO/2/0/DEMO38 /var/www/html/crs/1903

mv /var/www/swad/asg/DEMO/2/0/DEMO39 /var/www/swad/crs/1904
mv /var/www/html/asg/DEMO/2/0/DEMO39 /var/www/html/crs/1904

mv /var/www/swad/asg/DEMO/3/0/DEMO40 /var/www/swad/crs/1905
mv /var/www/html/asg/DEMO/3/0/DEMO40 /var/www/html/crs/1905

mv /var/www/swad/asg/DEMO/3/0/DEMO41 /var/www/swad/crs/1906
mv /var/www/html/asg/DEMO/3/0/DEMO41 /var/www/html/crs/1906

mv /var/www/swad/asg/DEMO/3/0/DEMO42 /var/www/swad/crs/1907
mv /var/www/html/asg/DEMO/3/0/DEMO42 /var/www/html/crs/1907

mv /var/www/swad/asg/DEMO/3/0/DEMO43 /var/www/swad/crs/1908
mv /var/www/html/asg/DEMO/3/0/DEMO43 /var/www/html/crs/1908

mv /var/www/swad/asg/DEMO/3/0/DEMO44 /var/www/swad/crs/1909
mv /var/www/html/asg/DEMO/3/0/DEMO44 /var/www/html/crs/1909

mv /var/www/swad/asg/DEMO/3/0/DEMO45 /var/www/swad/crs/1910
mv /var/www/html/asg/DEMO/3/0/DEMO45 /var/www/html/crs/1910

mv /var/www/swad/asg/DEMO/3/0/DEMO46 /var/www/swad/crs/1911
mv /var/www/html/asg/DEMO/3/0/DEMO46 /var/www/html/crs/1911

mv /var/www/swad/asg/DEMO/3/0/DEMO47 /var/www/swad/crs/1912
mv /var/www/html/asg/DEMO/3/0/DEMO47 /var/www/html/crs/1912

mv /var/www/swad/asg/DEMO/3/0/DEMO48 /var/www/swad/crs/1913
mv /var/www/html/asg/DEMO/3/0/DEMO48 /var/www/html/crs/1913

mv /var/www/swad/asg/DEMO/3/0/DEMO49 /var/www/swad/crs/1914
mv /var/www/html/asg/DEMO/3/0/DEMO49 /var/www/html/crs/1914

mv /var/www/swad/asg/DEMO/3/0/DEMO50 /var/www/swad/crs/1915
mv /var/www/html/asg/DEMO/3/0/DEMO50 /var/www/html/crs/1915

mv /var/www/swad/asg/DEMO/3/0/DEMO51 /var/www/swad/crs/1916
mv /var/www/html/asg/DEMO/3/0/DEMO51 /var/www/html/crs/1916

mv /var/www/swad/asg/DEMO/3/0/DEMO52 /var/www/swad/crs/1917
mv /var/www/html/asg/DEMO/3/0/DEMO52 /var/www/html/crs/1917

mv /var/www/swad/asg/DEMO/3/0/DEMO53 /var/www/swad/crs/1918
mv /var/www/html/asg/DEMO/3/0/DEMO53 /var/www/html/crs/1918

mv /var/www/swad/asg/DEMO/3/0/DEMO54 /var/www/swad/crs/1919
mv /var/www/html/asg/DEMO/3/0/DEMO54 /var/www/html/crs/1919

mv /var/www/swad/asg/DEMO/3/0/DEMO55 /var/www/swad/crs/1920
mv /var/www/html/asg/DEMO/3/0/DEMO55 /var/www/html/crs/1920

mv /var/www/swad/asg/DEMO/3/0/DEMO56 /var/www/swad/crs/1921
mv /var/www/html/asg/DEMO/3/0/DEMO56 /var/www/html/crs/1921

mv /var/www/swad/asg/DEMO/3/0/DEMO57 /var/www/swad/crs/1922
mv /var/www/html/asg/DEMO/3/0/DEMO57 /var/www/html/crs/1922

mv /var/www/swad/asg/DEMO/3/0/DEMO58 /var/www/swad/crs/1923
mv /var/www/html/asg/DEMO/3/0/DEMO58 /var/www/html/crs/1923

mv /var/www/swad/asg/DEMO/3/0/DEMO59 /var/www/swad/crs/1924
mv /var/www/html/asg/DEMO/3/0/DEMO59 /var/www/html/crs/1924

mv /var/www/swad/asg/DEMO/4/0/DEMO60 /var/www/swad/crs/1925
mv /var/www/html/asg/DEMO/4/0/DEMO60 /var/www/html/crs/1925

mv /var/www/swad/asg/DEMO/4/0/DEMO61 /var/www/swad/crs/1926
mv /var/www/html/asg/DEMO/4/0/DEMO61 /var/www/html/crs/1926

mv /var/www/swad/asg/DEMO/4/0/DEMO62 /var/www/swad/crs/1927
mv /var/www/html/asg/DEMO/4/0/DEMO62 /var/www/html/crs/1927

mv /var/www/swad/asg/DEMO/4/0/DEMO63 /var/www/swad/crs/1928
mv /var/www/html/asg/DEMO/4/0/DEMO63 /var/www/html/crs/1928

mv /var/www/swad/asg/DEMO/4/0/DEMO64 /var/www/swad/crs/1929
mv /var/www/html/asg/DEMO/4/0/DEMO64 /var/www/html/crs/1929

mv /var/www/swad/asg/DEMO/4/0/DEMO65 /var/www/swad/crs/1930
mv /var/www/html/asg/DEMO/4/0/DEMO65 /var/www/html/crs/1930

mv /var/www/swad/asg/DEMO/4/0/DEMO66 /var/www/swad/crs/1931
mv /var/www/html/asg/DEMO/4/0/DEMO66 /var/www/html/crs/1931

mv /var/www/swad/asg/DEMO/4/0/DEMO67 /var/www/swad/crs/1932
mv /var/www/html/asg/DEMO/4/0/DEMO67 /var/www/html/crs/1932

mv /var/www/swad/asg/DEMO/4/0/DEMO68 /var/www/swad/crs/1933
mv /var/www/html/asg/DEMO/4/0/DEMO68 /var/www/html/crs/1933

mv /var/www/swad/asg/DEMO/4/0/DEMO69 /var/www/swad/crs/1934
mv /var/www/html/asg/DEMO/4/0/DEMO69 /var/www/html/crs/1934

mv /var/www/swad/asg/DEMO/4/0/DEMO70 /var/www/swad/crs/1935
mv /var/www/html/asg/DEMO/4/0/DEMO70 /var/www/html/crs/1935

mv /var/www/swad/asg/DEMO/4/0/DEMO71 /var/www/swad/crs/1936
mv /var/www/html/asg/DEMO/4/0/DEMO71 /var/www/html/crs/1936

mv /var/www/swad/asg/DEMO/4/0/DEMO72 /var/www/swad/crs/1937
mv /var/www/html/asg/DEMO/4/0/DEMO72 /var/www/html/crs/1937

mv /var/www/swad/asg/DEMO/4/0/DEMO73 /var/www/swad/crs/1938
mv /var/www/html/asg/DEMO/4/0/DEMO73 /var/www/html/crs/1938

mv /var/www/swad/asg/DEMO/4/0/DEMO74 /var/www/swad/crs/1939
mv /var/www/html/asg/DEMO/4/0/DEMO74 /var/www/html/crs/1939

mv /var/www/swad/asg/DEMO/4/0/DEMO75 /var/www/swad/crs/1940
mv /var/www/html/asg/DEMO/4/0/DEMO75 /var/www/html/crs/1940

mv /var/www/swad/asg/DEMO/4/0/DEMO76 /var/www/swad/crs/1941
mv /var/www/html/asg/DEMO/4/0/DEMO76 /var/www/html/crs/1941

mv /var/www/swad/asg/DEMO/4/0/DEMO77 /var/www/swad/crs/1942
mv /var/www/html/asg/DEMO/4/0/DEMO77 /var/www/html/crs/1942

mv /var/www/swad/asg/DEMO/4/0/DEMO78 /var/www/swad/crs/1943
mv /var/www/html/asg/DEMO/4/0/DEMO78 /var/www/html/crs/1943

mv /var/www/swad/asg/DEMO/4/0/DEMO79 /var/www/swad/crs/1944
mv /var/www/html/asg/DEMO/4/0/DEMO79 /var/www/html/crs/1944

mv /var/www/swad/asg/DEMO/5/0/DEMO80 /var/www/swad/crs/1945
mv /var/www/html/asg/DEMO/5/0/DEMO80 /var/www/html/crs/1945

mv /var/www/swad/asg/DEMO/5/0/DEMO81 /var/www/swad/crs/1946
mv /var/www/html/asg/DEMO/5/0/DEMO81 /var/www/html/crs/1946

mv /var/www/swad/asg/DEMO/5/0/DEMO82 /var/www/swad/crs/1947
mv /var/www/html/asg/DEMO/5/0/DEMO82 /var/www/html/crs/1947

mv /var/www/swad/asg/DEMO/5/0/DEMO83 /var/www/swad/crs/1948
mv /var/www/html/asg/DEMO/5/0/DEMO83 /var/www/html/crs/1948

mv /var/www/swad/asg/DEMO/5/0/DEMO84 /var/www/swad/crs/1949
mv /var/www/html/asg/DEMO/5/0/DEMO84 /var/www/html/crs/1949

mv /var/www/swad/asg/DEMO/5/0/DEMO85 /var/www/swad/crs/1950
mv /var/www/html/asg/DEMO/5/0/DEMO85 /var/www/html/crs/1950

mv /var/www/swad/asg/DEMO/5/0/DEMO86 /var/www/swad/crs/1951
mv /var/www/html/asg/DEMO/5/0/DEMO86 /var/www/html/crs/1951

mv /var/www/swad/asg/DEMO/5/0/DEMO87 /var/www/swad/crs/1952
mv /var/www/html/asg/DEMO/5/0/DEMO87 /var/www/html/crs/1952

mv /var/www/swad/asg/DEMO/5/0/DEMO88 /var/www/swad/crs/1953
mv /var/www/html/asg/DEMO/5/0/DEMO88 /var/www/html/crs/1953

mv /var/www/swad/asg/DEMO/5/0/DEMO89 /var/www/swad/crs/1954
mv /var/www/html/asg/DEMO/5/0/DEMO89 /var/www/html/crs/1954

mv /var/www/swad/asg/DEMO/5/0/DEMO90 /var/www/swad/crs/1955
mv /var/www/html/asg/DEMO/5/0/DEMO90 /var/www/html/crs/1955

mv /var/www/swad/asg/DEMO/5/0/DEMO91 /var/www/swad/crs/1956
mv /var/www/html/asg/DEMO/5/0/DEMO91 /var/www/html/crs/1956

mv /var/www/swad/asg/DEMO/5/0/DEMO92 /var/www/swad/crs/1957
mv /var/www/html/asg/DEMO/5/0/DEMO92 /var/www/html/crs/1957

mv /var/www/swad/asg/DEMO/5/0/DEMO93 /var/www/swad/crs/1958
mv /var/www/html/asg/DEMO/5/0/DEMO93 /var/www/html/crs/1958

mv /var/www/swad/asg/DEMO/5/0/DEMO94 /var/www/swad/crs/1959
mv /var/www/html/asg/DEMO/5/0/DEMO94 /var/www/html/crs/1959

mv /var/www/swad/asg/DEMO/5/0/DEMO95 /var/www/swad/crs/1960
mv /var/www/html/asg/DEMO/5/0/DEMO95 /var/www/html/crs/1960

mv /var/www/swad/asg/DEMO/5/0/DEMO96 /var/www/swad/crs/1961
mv /var/www/html/asg/DEMO/5/0/DEMO96 /var/www/html/crs/1961

mv /var/www/swad/asg/DEMO/5/0/DEMO97 /var/www/swad/crs/1962
mv /var/www/html/asg/DEMO/5/0/DEMO97 /var/www/html/crs/1962

mv /var/www/swad/asg/DEMO/5/0/DEMO98 /var/www/swad/crs/1963
mv /var/www/html/asg/DEMO/5/0/DEMO98 /var/www/html/crs/1963

mv /var/www/swad/asg/DEMO/5/0/DEMO99 /var/www/swad/crs/1964
mv /var/www/html/asg/DEMO/5/0/DEMO99 /var/www/html/crs/1964

mv /var/www/swad/asg/DEMO/6/0/DEM100 /var/www/swad/crs/1965
mv /var/www/html/asg/DEMO/6/0/DEM100 /var/www/html/crs/1965

mv /var/www/swad/asg/DEMO/6/0/DEM101 /var/www/swad/crs/1966
mv /var/www/html/asg/DEMO/6/0/DEM101 /var/www/html/crs/1966

mv /var/www/swad/asg/DEMO/6/0/DEM102 /var/www/swad/crs/1967
mv /var/www/html/asg/DEMO/6/0/DEM102 /var/www/html/crs/1967

mv /var/www/swad/asg/DEMO/6/0/DEM103 /var/www/swad/crs/1968
mv /var/www/html/asg/DEMO/6/0/DEM103 /var/www/html/crs/1968

mv /var/www/swad/asg/DEMO/6/0/DEM104 /var/www/swad/crs/1969
mv /var/www/html/asg/DEMO/6/0/DEM104 /var/www/html/crs/1969

mv /var/www/swad/asg/DEMO/6/0/DEM105 /var/www/swad/crs/1970
mv /var/www/html/asg/DEMO/6/0/DEM105 /var/www/html/crs/1970

mv /var/www/swad/asg/DEMO/6/0/DEM106 /var/www/swad/crs/1971
mv /var/www/html/asg/DEMO/6/0/DEM106 /var/www/html/crs/1971

mv /var/www/swad/asg/DEMO/6/0/DEM107 /var/www/swad/crs/1972
mv /var/www/html/asg/DEMO/6/0/DEM107 /var/www/html/crs/1972

mv /var/www/swad/asg/DEMO/6/0/DEM108 /var/www/swad/crs/1973
mv /var/www/html/asg/DEMO/6/0/DEM108 /var/www/html/crs/1973

mv /var/www/swad/asg/DEMO/6/0/DEM109 /var/www/swad/crs/1974
mv /var/www/html/asg/DEMO/6/0/DEM109 /var/www/html/crs/1974

mv /var/www/swad/asg/DEMO/6/0/DEM110 /var/www/swad/crs/1975
mv /var/www/html/asg/DEMO/6/0/DEM110 /var/www/html/crs/1975

mv /var/www/swad/asg/DEMO/6/0/DEM111 /var/www/swad/crs/1976
mv /var/www/html/asg/DEMO/6/0/DEM111 /var/www/html/crs/1976

mv /var/www/swad/asg/DEMO/6/0/DEM112 /var/www/swad/crs/1977
mv /var/www/html/asg/DEMO/6/0/DEM112 /var/www/html/crs/1977

mv /var/www/swad/asg/DEMO/6/0/DEM113 /var/www/swad/crs/1978
mv /var/www/html/asg/DEMO/6/0/DEM113 /var/www/html/crs/1978

mv /var/www/swad/asg/DEMO/6/0/DEM114 /var/www/swad/crs/1979
mv /var/www/html/asg/DEMO/6/0/DEM114 /var/www/html/crs/1979

mv /var/www/swad/asg/DEMO/6/0/DEM115 /var/www/swad/crs/1980
mv /var/www/html/asg/DEMO/6/0/DEM115 /var/www/html/crs/1980

mv /var/www/swad/asg/DEMO/6/0/DEM116 /var/www/swad/crs/1981
mv /var/www/html/asg/DEMO/6/0/DEM116 /var/www/html/crs/1981

mv /var/www/swad/asg/DEMO/6/0/DEM117 /var/www/swad/crs/1982
mv /var/www/html/asg/DEMO/6/0/DEM117 /var/www/html/crs/1982

mv /var/www/swad/asg/DEMO/6/0/DEM118 /var/www/swad/crs/1983
mv /var/www/html/asg/DEMO/6/0/DEM118 /var/www/html/crs/1983

mv /var/www/swad/asg/DEMO/6/0/DEM119 /var/www/swad/crs/1984
mv /var/www/html/asg/DEMO/6/0/DEM119 /var/www/html/crs/1984

alter table usr_data add column Gender ENUM ('unknown','female','male') DEFAULT 'unknown' NOT NULL after FirstName;

alter table usr_data change LastAccBriefcase LastAccBriefcase DATETIME NOT NULL;

ALTER TABLE log CHANGE TipoUsrNum UsrType TINYINT NOT NULL;

UPDATE usr_data SET UsrType='5' where UsrType='4';
UPDATE connected SET UsrType='5' where UsrType='4';
UPDATE log SET UsrType='5' where UsrType='4';

ALTER TABLE sessions ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER Password;

show index from courses;
DROP INDEX FullCrsCod_old ON courses;
CREATE INDEX FullCrsCod_old ON courses (FullCrsCod_old);

ALTER TABLE asg_usr CHANGE NumAccTst NumAccTst INT NOT NULL DEFAULT 0;
ALTER TABLE asg_usr ADD COLUMN NextAccTst DATETIME AFTER NumAccTst;

ALTER TABLE tst_config ADD COLUMN MinTimeNxtTstPerQst INT NOT NULL DEFAULT 0 AFTER Max;

SELECT degrees.FullName,courses.CrsCod,courses.Year,courses.Semester,courses.ShortName,courses.FullName,courses.NumTeachers,courses.NumStudents FROM courses,degrees WHERE courses.FullName LIKE '%estructura%' AND courses.DegCod=degrees.DegCod ORDER BY degrees.FullName,courses.FullName,courses.Year,courses.Semester;

SELECT degrees.DegCod,courses.CrsCod,degrees.Logo,degrees.ShortName,degrees.FullName,courses.Year,courses.Semester,courses.FullName,courses.NumTeachers,courses.NumStudents FROM asg_usr,courses,degrees WHERE asg_usr.DNI IN (SELECT DNI FROM usr_data WHERE UsrType='3' AND CONCAT_WS(' ',FirstName,Surname1,Surname2) LIKE '%eva%' AND CONCAT_WS(' ',FirstName,Surname1,Surname2) LIKE '%martinez%' AND CONCAT_WS(' ',FirstName,Surname1,Surname2) LIKE '%ortigosa%') AND asg_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod ORDER BY degrees.FullName,courses.Year,courses.Semester,courses.FullName;

SELECT degrees.DegCod,courses.CrsCod,degrees.Logo,degrees.ShortName,degrees.FullName,courses.Year,courses.Semester,courses.FullName,courses.NumTeachers,courses.NumStudents
FROM (SELECT DNI FROM usr_data WHERE UsrType='3' AND CONCAT_WS(' ',FirstName,Surname1,Surname2) LIKE '%eva%' AND CONCAT_WS(' ',FirstName,Surname1,Surname2) LIKE '%martinez%' AND CONCAT_WS(' ',FirstName,Surname1,Surname2) LIKE '%ortigosa%') AS teachers,asg_usr,courses,degrees WHERE teachers.DNI=asg_usr.DNI AND asg_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod ORDER BY degrees.FullName,courses.Year,courses.Semester,courses.FullName;

CREATE TABLE tst_status (CrsCod INT NOT NULL, IdSession CHAR(22) NOT NULL, Status TINYINT NOT NULL, UNIQUE INDEX(CrsCod,IdSession));
CREATE TABLE tst_status (IdSession CHAR(22) NOT NULL, CrsCod INT NOT NULL, NumAccTst INT NOT NULL, Status TINYINT NOT NULL, UNIQUE INDEX(IdSession,CrsCod,NumAccTst));
CREATE TABLE tst_status (IdSession CHAR(22) NOT NULL, CrsCod INT NOT NULL, NumTst INT NOT NULL, Status TINYINT NOT NULL, UNIQUE INDEX(IdSession,CrsCod,NumTst));

ALTER TABLE courses DROP COLUMN CrsCod_old;
ALTER TABLE degrees DROP COLUMN DegCod_old;

SELECT COUNT(*) FROM asg_usr,usr_data WHERE asg_usr.CrsCod='80' AND usr_data.UsrType='2' AND asg_usr.DNI=usr_data.DNI;

CREATE INDEX UsrType ON usr_data (UsrType);

SELECT COUNT(*) FROM asg_usr,usr_data WHERE asg_usr.CrsCod='421' AND asg_usr.DNI=usr_data.DNI AND usr_data.UsrType='2';

ALTER TABLE courses DROP COLUMN NumTeachers;
ALTER TABLE courses DROP COLUMN NumStudents;

ALTER TABLE sessions ADD COLUMN DegTypCod INT NOT NULL DEFAULT -1 AFTER Password;

CREATE TABLE deg_admin (DNI CHAR(16) NOT NULL, DegCod INT NOT NULL, UNIQUE INDEX(DNI,DegCod));

SELECT asg_usr.DNI,asg_usr.Accepted FROM asg_usr,fichas_profesor WHERE asg_usr.DNI='24243619' AND asg_usr.DNI=fichas_profesor.DNI;

REPLACE INTO deg_admin (DNI,DegCod) VALUES ('24243619','-1');

ALTER TABLE sessions ADD COLUMN UsrType TINYINT NOT NULL AFTER Password;

connected
usr_data
log
sessions

select count(*) from connected where UsrType='5';
select count(*) from usr_data where UsrType='5';
select count(*) from log where UsrType='5';
select count(*) from sessions where UsrType='5';

select * from connected where UsrType='5';
select * from usr_data where UsrType='5';
select * from log where UsrType='5';
select * from sessions where UsrType='5';

update usr_data set UsrType='3' where UsrType='5';

ALTER TABLE degrees DROP COLUMN NumCourses;

SELECT degrees.DegCod,degrees.ShortName,degrees.FullName,degrees.Logo FROM deg_admin,degrees WHERE deg_admin.DNI='24243619' AND deg_admin.DegCod=degrees.DegCod ORDER BY degrees.ShortName;

SELECT degrees.DegCod,degrees.CampusCod,degrees.ShortName,degrees.FullName,degrees.FirstYear,degrees.LastYear,degrees.OptYear,degrees.Logo,degrees.WWW FROM deg_admin,degrees WHERE deg_admin.DNI='24243619' AND deg_admin.DegCod='-1' OR (deg_admin.DegCod<>'-1' AND deg_admin.DegCod=degrees.DegCod) ORDER BY degrees.ShortName;

SELECT DATE_FORMAT(FechaHora,'%Y%m%d') AS Dia,COUNT(*) FROM log WHERE FechaHora >= '20071201' AND FechaHora <= '20080131235959' GROUP BY Dia DESC;
SELECT DATE_FORMAT(FechaHora,'%x%v') AS Week,COUNT(*) FROM log WHERE FechaHora >= '20071201' AND FechaHora <= '20080131235959' GROUP BY Week DESC;

SELECT asg_usr.DNI,COUNT(asg_usr.Accepted='Y') FROM asg_usr,usr_data WHERE usr_data.UsrType='%u' AND asg_usr.DNI=usr_data.DNI GROUP BY asg_usr.DNI;

SELECT DISTINCT(messages.CrsCod),courses.ShortName FROM messages,msg_received,courses WHERE msg_received.DstDNI='24243619' AND msg_received.MsgCod=messages.MsgCod AND messages.CrsCod=courses.CrsCod ORDER BY courses.ShortName;


SELECT CodPrg,AnsType,Shuffle,Enunciado FROM tst_questions WHERE CrsCod='1859' AND CodPrg NOT IN (SELECT tst_questions.CodPrg FROM tst_questions,tst_question_tags,tst_tags WHERE tst_questions.CrsCod='1859' AND tst_questions.CodPrg=tst_question_tags.CodPrg AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='1859' AND tst_tags.TagHidden='Y');

SELECT tst_questions.CodPrg FROM tst_questions,tst_question_tags,tst_tags WHERE tst_questions.CrsCod='421' AND tst_questions.CodPrg=tst_question_tags.CodPrg AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='421' AND tst_tags.TagHidden='Y'

SELECT q.CodPrg FROM tst_questions AS q,tst_question_tags AS qt,tst_tags AS t WHERE q.CrsCod='421' AND q.CodPrg=qt.CodPrg AND qt.TagCod=t.TagCod AND t.CrsCod='421' AND t.TagHidden='Y';

SELECT COUNT(forum_thread.ThrCod),COUNT(forum_post.MsgCod) FROM forum_thread,forum_post WHERE forum_thread.ForumType='6' AND forum_thread.ThrCod=forum_post.ThrCod;
SELECT forum_thread.ThrCod,forum_thread.ThrInd,COUNT(forum_post.MsgCod),m0.MsgCod,m1.MsgCod,m0.AuthorDNI,m1.AuthorDNI,DATE_FORMAT(m0.CreatTime,'%%Y%%m%%d%%H%%i%%S'),DATE_FORMAT(m1.CreatTime,'%%Y%%m%%d%%H%%i%%S'),m0.Subject FROM forum_thread,forum_post,messages AS m0,messages AS m1 WHERE forum_thread.ForumType='6' AND forum_thread.ThrCod=forum_post.ThrCod AND forum_thread.FirstPstCod=m0.MsgCod AND forum_thread.LastPstCod=m1.MsgCod GROUP BY forum_post.ThrCod ORDER BY m%u.CreatTime DESC;
SELECT COUNT(*) FROM forum_thread,forum_post,messages WHERE forum_thread.ForumType='6' AND forum_thread.ThrCod=forum_post.ThrCod AND forum_post.MsgCod=messages.MsgCod AND messages.AuthorDNI='24243619';


DROP INDEX MsgCod ON MsgCod;
CREATE UNIQUE INDEX MsgCod ON forum_post (MsgCod);

ALTER TABLE forum_post ADD COLUMN ThrCod2 INT NOT NULL AFTER MsgCod;
UPDATE forum_post SET ThrCod2=ThrCod;
ALTER TABLE forum_post DROP COLUMN ThrCod;
ALTER TABLE forum_post CHANGE ThrCod2 ThrCod INT NOT NULL;
CREATE INDEX ThrCod ON forum_post (ThrCod);

ALTER TABLE forum_thread DROP COLUMN ThrInd;


SELECT SQL_NO_CACHE CrsCod,ShortName FROM courses WHERE CrsCod IN (SELECT SQL_NO_CACHE DISTINCT messages.CrsCod FROM msg_received,messages WHERE msg_received.DstDNI='24243619' AND msg_received.MsgCod=messages.MsgCod) ORDER BY ShortName;

ALTER TABLE fichas_profesor CHANGE Tlf Tlf CHAR(16) NOT NULL;
ALTER TABLE fichas_alumno CHANGE LocalPhone LocalPhone CHAR(16) NOT NULL;
ALTER TABLE fichas_alumno CHANGE FamilyPhone FamilyPhone CHAR(16) NOT NULL;

mysql> CREATE TABLE forum_post (MsgCod INT NOT NULL,
 ThrCod INT NOT NULL,
 AuthorDNI CHAR(16) NOT NULL,
 CreatTime DATETIME NOT NULL,
 ModifTime DATETIME NOT NULL,
 Subject TEXT NOT NULL,
 Content LONGTEXT NOT NULL;
 		 UNIQUE INDEX(MsgCod),INDEX(ThrCod),INDEX(AuthorDNI),INDEX(CreatTime));

mysql> DESCRIBE forum_post;
+-----------+----------+------+-----+---------------------+-------+
| Field | Type | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------------------+-------+
| MsgCod | int(11) | NO | PRI | 0 | |
| ThrCod | int(11) | NO | MUL | | |
| ModifTime | datetime | NO | | 0000-00-00 00:00:00 | |
+-----------+----------+------+-----+---------------------+-------+
3 rows in set (0.00 sec)

mysql> CREATE TABLE messages (MsgCod INT NOT NULL AUTO_INCREMENT,
 CrsCod INT NOT NULL DEFAULT -1,
			 AuthorDNI CHAR(16) NOT NULL,
 CreatTime DATETIME NOT NULL,
			 Subject TEXT NOT NULL,
			 Content LONGTEXT NOT NULL,
			 UNIQUE INDEX(MsgCod), INDEX(CrsCod), INDEX(AuthorDNI));
mysql> DESCRIBE messages;
+-----------+----------+------+-----+---------------------+----------------+
| Field | Type | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------------------+----------------+
| MsgCod | int(11) | NO | PRI | NULL | auto_increment |
| CrsCod | int(11) | NO | MUL | -1 | |
| AuthorDNI | char(16) | NO | MUL | | |
| CreatTime | datetime | NO | | 0000-00-00 00:00:00 | |
| Subject | text | NO | | | |
| Content | longtext | NO | | | |
+-----------+----------+------+-----+---------------------+----------------+
6 rows in set (0.00 sec)

CREATE INDEX CreatTime ON messages (CreatTime);
-------------------

BACKUP TABLE forum_post TO '/home/acanas/';
ALTER TABLE forum_post CHANGE MsgCod MsgCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE forum_post ADD COLUMN AuthorDNI CHAR(16) NOT NULL AFTER ThrCod;
ALTER TABLE forum_post ADD COLUMN CreatTime DATETIME NOT NULL AFTER AuthorDNI;
ALTER TABLE forum_post ADD COLUMN Subject TEXT NOT NULL AFTER ModifTime;
ALTER TABLE forum_post ADD COLUMN Content LONGTEXT NOT NULL AFTER Subject;
CREATE INDEX AuthorDNI ON forum_post (AuthorDNI);
CREATE INDEX CreatTime ON forum_post (CreatTime);
CREATE INDEX ModifTime ON forum_post (ModifTime);
UPDATE forum_post,messages SET forum_post.AuthorDNI=messages.AuthorDNI WHERE forum_post.MsgCod=messages.MsgCod;
UPDATE forum_post,messages SET forum_post.CreatTime=messages.CreatTime WHERE forum_post.MsgCod=messages.MsgCod;
UPDATE forum_post,messages SET forum_post.Subject=messages.Subject WHERE forum_post.MsgCod=messages.MsgCod;
UPDATE forum_post,messages SET forum_post.Content=messages.Content WHERE forum_post.MsgCod=messages.MsgCod;

DELETE FROM messages WHERE MsgCod IN (SELECT MsgCod FROM forum_post) AND MsgCod NOT IN (SELECT MsgCod FROM msg_sent) AND MsgCod NOT IN (SELECT MsgCod FROM msg_received) AND MsgCod NOT IN (SELECT MsgCod FROM msg_notices);

------------------------------------


BACKUP TABLE msg_notices TO '/home/acanas/';
ALTER TABLE msg_notices CHANGE MsgCod MsgCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE msg_notices ADD COLUMN AuthorDNI CHAR(16) NOT NULL AFTER CrsCod;
ALTER TABLE msg_notices ADD COLUMN CreatTime DATETIME NOT NULL AFTER AuthorDNI;
ALTER TABLE msg_notices ADD COLUMN Content LONGTEXT NOT NULL AFTER CreatTime;
CREATE INDEX AuthorDNI ON msg_notices (AuthorDNI);
CREATE INDEX CreatTime ON msg_notices (CreatTime);
UPDATE msg_notices,messages SET msg_notices.AuthorDNI=messages.AuthorDNI WHERE msg_notices.MsgCod=messages.MsgCod;
UPDATE msg_notices,messages SET msg_notices.CreatTime=messages.CreatTime WHERE msg_notices.MsgCod=messages.MsgCod;
UPDATE msg_notices,messages SET msg_notices.Content=messages.Content WHERE msg_notices.MsgCod=messages.MsgCod;

DELETE FROM messages WHERE MsgCod IN (SELECT MsgCod FROM msg_notices) AND MsgCod NOT IN (SELECT MsgCod FROM msg_sent) AND MsgCod NOT IN (SELECT MsgCod FROM msg_received);

-----

ALTER TABLE courses DROP COLUMN FullCrsCod_old;

SELECT COUNT(*) FROM forum_thr_read WHERE ThrCod='4955';

SELECT COUNT(DISTINCT AuthorDNI) FROM forum_post WHERE ThrCod='4955';

SELECT COUNT(*) FROM forum_post WHERE ThrCod='4955';

CREATE TABLE forum_thr_clip (ThrCod INT NOT NULL,DNI CHAR(16) NOT NULL,UNIQUE INDEX(ThrCod),UNIQUE INDEX (DNI));
ALTER TABLE forum_thr_clip ADD COLUMN TimeInsert TIMESTAMP NOT NULL AFTER DNI;

SELECT CrsCod,AuthorDNI,DATE_FORMAT(CreatTime,'%Y%m%d%H%i%S'),Subject,Content FROM messages WHERE MsgCod='484624';


SELECT COUNT(forum_post.MsgCod),m0.MsgCod,m1.MsgCod,m0.AuthorDNI,m1.AuthorDNI,DATE_FORMAT(m0.CreatTime,'%Y%m%d%H%i%S'),DATE_FORMAT(m1.CreatTime,'%Y%m%d%H%i%S'),m0.Subject FROM forum_thread,forum_post,forum_post AS m0,forum_post AS m1 WHERE forum_thread.ThrCod='5004' AND forum_thread.ThrCod=forum_post.ThrCod AND forum_thread.FirstPstCod=m0.MsgCod AND forum_thread.LastPstCod=m1.MsgCod GROUP BY forum_post.ThrCod;

SELECT m0.MsgCod,m1.MsgCod,m0.AuthorDNI,m1.AuthorDNI,DATE_FORMAT(m0.CreatTime,'%Y%m%d%H%i%S'),DATE_FORMAT(m1.CreatTime,'%Y%m%d%H%i%S'),m0.Subject FROM forum_thread,forum_post AS m0,forum_post AS m1 WHERE forum_thread.ThrCod='5004' AND forum_thread.FirstPstCod=m0.MsgCod AND forum_thread.LastPstCod=m1.MsgCod;

----

ALTER TABLE usr_data ADD COLUMN Nickname CHAR(16) NOT NULL AFTER DNI;
CREATE INDEX Nickname ON usr_data (Nickname);
UPDATE usr_data,apodos SET usr_data.Nickname=apodos.apodo WHERE usr_data.DNI=apodos.DNI;

----

ALTER TABLE usr_data ADD COLUMN Dpto VARCHAR(255) NOT NULL AFTER PublicPhoto;
ALTER TABLE usr_data ADD COLUMN Centro VARCHAR(255) NOT NULL AFTER Dpto;
ALTER TABLE usr_data ADD COLUMN Despacho VARCHAR(255) NOT NULL AFTER Centro;
ALTER TABLE usr_data ADD COLUMN Tlf CHAR(16) NOT NULL AFTER Despacho;
UPDATE usr_data,fichas_profesor SET usr_data.Dpto=fichas_profesor.Dpto WHERE usr_data.DNI=fichas_profesor.DNI;
UPDATE usr_data,fichas_profesor SET usr_data.Centro=fichas_profesor.Centro WHERE usr_data.DNI=fichas_profesor.DNI;
UPDATE usr_data,fichas_profesor SET usr_data.Despacho=fichas_profesor.Despacho WHERE usr_data.DNI=fichas_profesor.DNI;
UPDATE usr_data,fichas_profesor SET usr_data.Tlf=fichas_profesor.Tlf WHERE usr_data.DNI=fichas_profesor.DNI;

-----

ALTER TABLE usr_data ADD COLUMN LocalAddress VARCHAR(255) NOT NULL AFTER Tlf;
ALTER TABLE usr_data ADD COLUMN LocalPhone CHAR(16) NOT NULL AFTER LocalAddress;
ALTER TABLE usr_data ADD COLUMN FamilyAddress VARCHAR(255) NOT NULL AFTER LocalPhone;
ALTER TABLE usr_data ADD COLUMN FamilyPhone CHAR(16) NOT NULL AFTER FamilyAddress;
ALTER TABLE usr_data ADD COLUMN OriginPlace VARCHAR(255) NOT NULL AFTER FamilyPhone;
ALTER TABLE usr_data ADD COLUMN Birthday DATE NOT NULL AFTER OriginPlace;
ALTER TABLE usr_data ADD COLUMN Observaciones TEXT NOT NULL AFTER Birthday;
UPDATE usr_data,fichas_alumno SET usr_data.LocalAddress=fichas_alumno.LocalAddress WHERE usr_data.DNI=fichas_alumno.DNI;
UPDATE usr_data,fichas_alumno SET usr_data.LocalPhone=fichas_alumno.LocalPhone WHERE usr_data.DNI=fichas_alumno.DNI;
UPDATE usr_data,fichas_alumno SET usr_data.FamilyAddress=fichas_alumno.FamilyAddress WHERE usr_data.DNI=fichas_alumno.DNI;
UPDATE usr_data,fichas_alumno SET usr_data.FamilyPhone=fichas_alumno.FamilyPhone WHERE usr_data.DNI=fichas_alumno.DNI;
UPDATE usr_data,fichas_alumno SET usr_data.OriginPlace=fichas_alumno.OriginPlace WHERE usr_data.DNI=fichas_alumno.DNI;
UPDATE usr_data,fichas_alumno SET usr_data.Birthday=fichas_alumno.Birthday WHERE usr_data.DNI=fichas_alumno.DNI;
UPDATE usr_data,fichas_alumno SET usr_data.Observaciones=fichas_alumno.Observaciones WHERE usr_data.DNI=fichas_alumno.DNI;

----

RENAME TABLE fichas_comun TO usr_data;
ALTER TABLE usr_data CHANGE Dpto Dept VARCHAR(255) NOT NULL;
ALTER TABLE usr_data CHANGE Centro Centre VARCHAR(255) NOT NULL;
ALTER TABLE usr_data CHANGE Despacho Office VARCHAR(255) NOT NULL;
ALTER TABLE usr_data CHANGE Tlf OfficePhone CHAR(16) NOT NULL;
ALTER TABLE usr_data CHANGE DirLocal LocalAddress VARCHAR(255) NOT NULL;
ALTER TABLE usr_data CHANGE DirFamilia FamilyAddress VARCHAR(255) NOT NULL;
ALTER TABLE usr_data CHANGE TlfLocal LocalPhone CHAR(16) NOT NULL;
ALTER TABLE usr_data CHANGE TlfFamilia FamilyPhone CHAR(16) NOT NULL;
ALTER TABLE usr_data CHANGE LugarOrg OriginPlace VARCHAR(255) NOT NULL;
ALTER TABLE usr_data CHANGE FechaNac Birthday DATE NOT NULL;
ALTER TABLE usr_data CHANGE Observaciones Comments TEXT NOT NULL;

----

ALTER TABLE usr_data ADD COLUMN Language CHAR(2) NOT NULL AFTER Gender;

----

mysql> describe asg_usr;
+----------------+---------------+------+-----+---------+-------+
| Field | Type | Null | Key | Default | Extra |
+----------------+---------------+------+-----+---------+-------+
| CrsCod | int(11) | NO | PRI | -1 | |
| DNI | char(16) | NO | PRI | | |
| Accepted | enum('N','Y') | NO | | N | |
| UltPst | tinyint(4) | YES | | NULL | |
| UltAccDescarga | datetime | YES | | NULL | |
| UltAccComunAsg | datetime | YES | | NULL | |
| UltAccMisTrbjs | datetime | YES | | NULL | |
| UltAccTrbjsAsg | datetime | YES | | NULL | |
| UltAccCalif | datetime | YES | | NULL | |
| LastDowGrpCod | int(11) | NO | | -1 | |
| LastComGrpCod | int(11) | NO | | -1 | |
| LastAssGrpCod | int(11) | NO | | -1 | |
| NumAccTst | int(11) | NO | | 0 | |
| NextAccTst | datetime | YES | | NULL | |
| ColsOrla | tinyint(4) | YES | | NULL | |
+----------------+---------------+------+-----+---------+-------+
15 rows in set (0.12 sec)

RENAME TABLE asg_usr TO crs_usr;
ALTER TABLE crs_usr CHANGE UltPst LastTab TINYINT NOT NULL;
ALTER TABLE crs_usr CHANGE UltAccDescarga LastAccDownload DATETIME NOT NULL;
ALTER TABLE crs_usr CHANGE UltAccComunAsg LastAccCommonCrs DATETIME NOT NULL;
ALTER TABLE crs_usr CHANGE UltAccMisTrbjs LastAccMyWorks DATETIME NOT NULL;
ALTER TABLE crs_usr CHANGE UltAccTrbjsAsg LastAccCrsWorks DATETIME NOT NULL;
ALTER TABLE crs_usr CHANGE UltAccCalif LastAccGrades DATETIME NOT NULL;
ALTER TABLE crs_usr CHANGE ColsOrla ColsClassPhoto TINYINT NOT NULL;

ALTER TABLE crs_usr CHANGE NextAccTst NextAccTst DATETIME NOT NULL;

-----------

ALTER TABLE usr_data ADD COLUMN LastTab TINYINT NOT NULL AFTER Comments;

ALTER TABLE crs_usr DROP COLUMN LastTab;

-----

RENAME TABLE asg_tipos_grp TO crs_grp_types;

RENAME TABLE degree_types TO deg_types;
RENAME TABLE asg_grupos TO crs_grp;

RENAME TABLE asg_grp_usr TO crs_grp_usr;

ALTER TABLE crs_grp_types ADD COLUMN GrpTypCod INT NOT NULL AFTER CodTipoGrupo;
UPDATE crs_grp_types SET GrpTypCod=CodTipoGrupo;
ALTER TABLE crs_grp_types DROP COLUMN CodTipoGrupo;
CREATE UNIQUE INDEX GrpTypCod ON crs_grp_types (GrpTypCod);
ALTER TABLE crs_grp_types CHANGE GrpTypCod GrpTypCod INT NOT NULL AUTO_INCREMENT;

ALTER TABLE crs_grp CHANGE CodTipoGrupo GrpTypCod INT NOT NULL;

ALTER TABLE crs_grp_types CHANGE TipoGrupo GrpTypName VARCHAR(255) NOT NULL;
ALTER TABLE crs_grp_types CHANGE Obligatorio Obligatory ENUM('N','S') NOT NULL;

ALTER TABLE crs_grp_types ADD COLUMN Obligatory2 ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Obligatory;
UPDATE crs_grp_types SET Obligatory2='N' WHERE Obligatory='N';
UPDATE crs_grp_types SET Obligatory2='Y' WHERE Obligatory='S';
ALTER TABLE crs_grp_types DROP COLUMN Obligatory;
ALTER TABLE crs_grp_types CHANGE Obligatory2 Obligatory ENUM('N','Y') NOT NULL DEFAULT 'N';

ALTER TABLE crs_grp_types ADD COLUMN Multiple2 ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Multiple;
UPDATE crs_grp_types SET Multiple2='N' WHERE Multiple='N';
UPDATE crs_grp_types SET Multiple2='Y' WHERE Multiple='S';
ALTER TABLE crs_grp_types DROP COLUMN Multiple;
ALTER TABLE crs_grp_types CHANGE Multiple2 Multiple ENUM('N','Y') NOT NULL DEFAULT 'N';

CREATE TABLE prueba (CodGrupo INT NOT NULL AUTO_INCREMENT, GrpTypCod INT NOT NULL, UNIQUE INDEX(CodGrupo), INDEX(GrpTypCod));

ALTER TABLE prueba CHANGE CodGrupo GrpCod INT NOT NULL AUTO_INCREMENT;

-----

ALTER TABLE crs_grp CHANGE CodGrupo GrpCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE crs_grp_usr CHANGE CodGrupo GrpCod INT NOT NULL;
ALTER TABLE portapapeles CHANGE CodGrupo GrpCod INT NOT NULL;
ALTER TABLE crs_common_files CHANGE CodGrupo GrpCod INT NOT NULL;

DROP TABLE prueba;
DROP TABLE apodos;

RENAME TABLE asg_arch_comunes TO crs_common_files;

RENAME TABLE asg_fichas TO crs_cards;
RENAME TABLE campos_fichas TO crs_card_fields;

RENAME TABLE calificaciones TO grades;
RENAME TABLE portapapeles TO clipboard;
---

RENAME TABLE param_ocultos TO hidden_params;
RENAME TABLE descargas_ocultas TO hidden_downloads;


RENAME TABLE convocatorias TO calls_for_exams;

RENAME TABLE horario_asg TO timetable_crs;
RENAME TABLE horario_tut TO timetable_tut;

RENAME TABLE num_accesos_sin_foto TO clicks_without_photo;
RENAME TABLE num_clicks_without_photo TO clicks_without_photo;

----

SHOW INDEX FROM usr_data;
DROP INDEX Nickname ON usr_data;
CREATE INDEX Nickname ON usr_data (Nickname);
SHOW INDEX FROM usr_data;

----

ALTER TABLE usr_data ADD COLUMN UsrCod INT NOT NULL FIRST;
CREATE UNIQUE INDEX UsrCod ON usr_data (UsrCod);
ALTER TABLE usr_data CHANGE UsrCod UsrCod INT NOT NULL AUTO_INCREMENT;

-----------------------------------------------------------------------------------------------------------------------------------

ALTER TABLE usr_data DROP COLUMN UsrCod;
ALTER TABLE usr_data ADD UsrCod INT NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (UsrCod);

---

*ALTER TABLE msg_new ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add unique index!
*UPDATE msg_new,usr_data SET msg_new.UsrCod=usr_data.UsrCod WHERE msg_new.DstDNI=usr_data.DNI;
*DROP INDEX DstDNI ON msg_new;
*CREATE UNIQUE INDEX UsrCod ON msg_new (UsrCod);
*ALTER TABLE msg_new DROP COLUMN DstDNI;

*ALTER TABLE msg_received ADD COLUMN UsrCod INT NOT NULL AFTER MsgCod;	// Add multiple index!
*UPDATE msg_received,usr_data SET msg_received.UsrCod=usr_data.UsrCod WHERE msg_received.DstDNI=usr_data.DNI;
*CREATE INDEX UsrCod ON msg_received (UsrCod);
*ALTER TABLE msg_received DROP COLUMN DstDNI;

*ALTER TABLE msg_sent ADD COLUMN UsrCod INT NOT NULL AFTER MsgCod;	// Add multiple index!
*UPDATE msg_sent,usr_data SET msg_sent.UsrCod=usr_data.UsrCod WHERE msg_sent.DstDNI=usr_data.DNI;
*CREATE INDEX UsrCod ON msg_sent (UsrCod);
*ALTER TABLE msg_sent DROP COLUMN DstDNI;

*ALTER TABLE clicks_without_photo ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add unique index!
*UPDATE clicks_without_photo,usr_data SET clicks_without_photo.UsrCod=usr_data.UsrCod WHERE clicks_without_photo.DNI=usr_data.DNI;
*ALTER TABLE clicks_without_photo CHANGE NumAccesos NumClicks INT NOT NULL;
*ALTER TABLE clicks_without_photo DROP COLUMN DNI;
*CREATE UNIQUE INDEX UsrCod ON clicks_without_photo (UsrCod);

*ALTER TABLE clipboard ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add unique index
*UPDATE clipboard,usr_data SET clipboard.UsrCod=usr_data.UsrCod,clipboard.Fecha=clipboard.Fecha WHERE clipboard.DNI=usr_data.DNI;
*ALTER TABLE clipboard DROP COLUMN DNI;
*CREATE UNIQUE INDEX UsrCod ON clipboard (UsrCod);
*ALTER TABLE clipboard ADD COLUMN WorksUsrCod INT NOT NULL AFTER GrpCod;
*UPDATE clipboard,usr_data SET clipboard.WorksUsrCod=usr_data.UsrCod,clipboard.Fecha=clipboard.Fecha WHERE clipboard.DNITrabajos=usr_data.DNI;
*ALTER TABLE clipboard DROP COLUMN DNITrabajos;

*ALTER TABLE connected ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add unique index!

*ALTER TABLE sessions ADD COLUMN UsrCod INT NOT NULL AFTER IdSession;	// Add multiple index!

*ALTER TABLE crs_usr ADD COLUMN UsrCod INT NOT NULL AFTER CrsCod;	// Add UNIQUE INDEX(CrsCod,UsrCod)!

*ALTER TABLE crs_grp_usr ADD COLUMN UsrCod INT NOT NULL AFTER GrpCod;	// Add multiple index!

*UPDATE connected,usr_data SET connected.UsrCod=usr_data.UsrCod WHERE connected.DNI=usr_data.DNI;
*UPDATE sessions,usr_data SET sessions.UsrCod=usr_data.UsrCod WHERE sessions.DNI=usr_data.DNI;

*UPDATE crs_usr,usr_data SET crs_usr.UsrCod=usr_data.UsrCod WHERE crs_usr.DNI=usr_data.DNI;
*UPDATE crs_grp_usr,usr_data SET crs_grp_usr.UsrCod=usr_data.UsrCod WHERE crs_grp_usr.DNI=usr_data.DNI;

*ALTER TABLE crs_usr DROP COLUMN DNI;
*ALTER TABLE crs_grp_usr DROP COLUMN DNI;

*DROP INDEX CrsCod_DNI ON crs_usr;
*CREATE UNIQUE INDEX CrsCod_UsrCod ON crs_usr (CrsCod,UsrCod);
*CREATE INDEX UsrCod ON crs_grp_usr (UsrCod);

*CREATE UNIQUE INDEX GrpCod_UsrCod ON crs_grp_usr (GrpCod,UsrCod);

*ALTER TABLE sessions DROP COLUMN DNI;
*CREATE INDEX UsrCod ON sessions (UsrCod);

*ALTER TABLE connected DROP COLUMN DNI;
*CREATE UNIQUE INDEX UsrCod ON connected (UsrCod);

*ALTER TABLE crs_common_files ADD COLUMN UsrCod INT NOT NULL AFTER GrpCod;
*UPDATE crs_common_files,usr_data SET crs_common_files.UsrCod=usr_data.UsrCod WHERE crs_common_files.DNI=usr_data.DNI;
*ALTER TABLE crs_common_files DROP COLUMN DNI;
*CREATE INDEX UsrCod ON crs_common_files (UsrCod);
*DROP INDEX CodAsg ON crs_common_files;

*ALTER TABLE crs_cards ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add unique index!
*UPDATE crs_cards,usr_data SET crs_cards.UsrCod=usr_data.UsrCod WHERE crs_cards.DNI=usr_data.DNI;

*DROP INDEX CodCampo ON crs_cards;
*DROP INDEX DNI ON crs_cards;
*ALTER TABLE crs_cards DROP COLUMN DNI;
*CREATE UNIQUE INDEX FldCod_UsrCod ON crs_cards (CodCampo,UsrCod);

*ALTER TABLE deg_admin ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add UNIQUE INDEX(UsrCod,DegCod)!
*UPDATE deg_admin,usr_data SET deg_admin.UsrCod=usr_data.UsrCod WHERE deg_admin.DNI=usr_data.DNI;

*DROP INDEX DNI ON deg_admin;
*ALTER TABLE deg_admin DROP COLUMN DNI;
*CREATE UNIQUE INDEX UsrCod_DegCod ON deg_admin (UsrCod,DegCod);

*ALTER TABLE expanded_folders ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add multiple index!
*UPDATE expanded_folders,usr_data SET expanded_folders.UsrCod=usr_data.UsrCod WHERE expanded_folders.DNI=usr_data.DNI;
*DROP INDEX DNI ON expanded_folders;
*DROP INDEX CrsCod_2 ON expanded_folders;
*ALTER TABLE expanded_folders DROP COLUMN DNI;
*CREATE INDEX UsrCod_TreeType ON expanded_folders (UsrCod,TreeType);
*CREATE INDEX CrsCod ON expanded_folders (CrsCod);
*ALTER TABLE expanded_folders DROP COLUMN DNIWorks;

*ALTER TABLE forum_disabled_post ADD COLUMN UsrCod INT NOT NULL AFTER MsgCod;
*UPDATE forum_disabled_post,usr_data SET forum_disabled_post.UsrCod=usr_data.UsrCod WHERE forum_disabled_post.DNI=usr_data.DNI;
*ALTER TABLE forum_disabled_post DROP COLUMN DNI;

*ALTER TABLE forum_thr_read ADD COLUMN UsrCod INT NOT NULL AFTER ThrCod;	// Add UNIQUE INDEX(ThrCod,DNI)!
*UPDATE forum_thr_read,usr_data SET forum_thr_read.UsrCod=usr_data.UsrCod WHERE forum_thr_read.DNI=usr_data.DNI;
*DROP INDEX ThrCod ON forum_thr_read;
*CREATE UNIQUE INDEX ThrCod_UsrCod ON forum_thr_read (ThrCod,UsrCod);
*ALTER TABLE forum_thr_read DROP COLUMN DNI;

*ALTER TABLE forum_thr_clip ADD COLUMN UsrCod INT NOT NULL AFTER ThrCod;	// Add unique index!
*UPDATE forum_thr_clip,usr_data SET forum_thr_clip.UsrCod=usr_data.UsrCod WHERE forum_thr_clip.DNI=usr_data.DNI;
*ALTER TABLE forum_thr_clip DROP COLUMN DNI;
*CREATE UNIQUE INDEX UsrCod ON forum_thr_clip (UsrCod);

*ALTER TABLE timetable_tut ADD COLUMN UsrCod INT NOT NULL FIRST;	// Add multiple index!
*UPDATE timetable_tut,usr_data SET timetable_tut.UsrCod=usr_data.UsrCod WHERE timetable_tut.DNI=usr_data.DNI;
*ALTER TABLE timetable_tut DROP COLUMN DNI;
*CREATE INDEX UsrCod ON timetable_tut (UsrCod);

CREATE INDEX UsrCod ON crs_usr (UsrCod);
CREATE INDEX UsrCod ON crs_grp_usr (UsrCod);

SELECT COUNT(DISTINCT usr_data.UsrCod) FROM usr_data,crs_usr WHERE usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod;

-----

ALTER TABLE usr_data ADD COLUMN NumNewMsgs INT NOT NULL AFTER LastTab;
UPDATE usr_data,msg_new SET usr_data.NumNewMsgs=msg_new.NumNewMsgs WHERE usr_data.UsrCod=msg_new.UsrCod;

-----

ALTER TABLE msg_received ADD COLUMN Replied2 ENUM('N','Y') NOT NULL AFTER Replied;
UPDATE msg_received SET Replied2='Y' WHERE Replied<>'N';
ALTER TABLE msg_sent ADD COLUMN ReadByDst2 ENUM('N','Y') NOT NULL AFTER ReadByDst;
UPDATE msg_sent SET ReadByDst2='Y' WHERE ReadByDst<>'N';

UPDATE msg_received SET Replied2='Y' WHERE Replied<>'N';
UPDATE msg_sent SET ReadByDst2='Y' WHERE ReadByDst<>'N';
ALTER TABLE msg_received DROP COLUMN Replied;
ALTER TABLE msg_received CHANGE Replied2 Replied ENUM('N','Y') NOT NULL;
ALTER TABLE msg_sent DROP COLUMN ReadByDst;
ALTER TABLE msg_sent CHANGE ReadByDst2 ReadByDst ENUM('N','Y') NOT NULL;

CREATE UNIQUE INDEX UsrCod_MsgCod ON msg_sent (UsrCod,MsgCod);
CREATE UNIQUE INDEX UsrCod_MsgCod ON msg_received (UsrCod,MsgCod);

ALTER TABLE msg_received CHANGE Replied Replied ENUM('N','Y') NOT NULL DEFAULT 'N';
ALTER TABLE msg_sent CHANGE ReadByDst ReadByDst ENUM('N','Y') NOT NULL DEFAULT 'N';

ALTER TABLE msg_received ADD COLUMN Expanded ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Replied;

ALTER TABLE msg_received ADD COLUMN ReadByMe ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER UsrCod;

UPDATE msg_received SET ReadByMe='Y';
UPDATE msg_sent SET ReadByDst='Y';

----

ALTER TABLE messages ADD COLUMN Deleted ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER UsrCod;

ALTER TABLE msg_received ADD COLUMN Deleted ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER UsrCod;


SELECT MsgCod FROM messages WHERE UsrCod='1346' AND Deleted='N' ORDER BY MsgCod DESC;

SELECT COUNT(*) FROM msg_sent WHERE (MsgCod,UsrCod) NOT IN (SELECT DISTINCTROW MsgCod,UsrCod FROM msg_received);

SELECT MsgCod,UsrCod,'Y',ReadByDst,'N','N' FROM msg_sent WHERE (MsgCod,UsrCod) NOT IN (SELECT MsgCod,UsrCod FROM msg_received);

INSERT INTO msg_received (MsgCod,UsrCod,Deleted,ReadByMe,Replied,Expanded) SELECT MsgCod,UsrCod,'Y',ReadByDst,'N','N' FROM msg_sent WHERE (MsgCod,UsrCod) NOT IN (SELECT MsgCod,UsrCod FROM msg_received);

ALTER TABLE msg_received ADD COLUMN OldLstDst TEXT AFTER Expanded;

UPDATE msg_received,msg_sent SET msg_received.OldLstDst=msg_sent.OldLstDst WHERE msg_received.UsrCod='-1' AND msg_received.MsgCod=msg_sent.MsgCod;
----

ALTER TABLE messages ADD COLUMN Expanded ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Deleted;

---
ALTER TABLE msg_received CHANGE ReadByMe Open ENUM('N','Y') NOT NULL DEFAULT 'N';

---

ALTER TABLE log ADD COLUMN Addr CHAR(40) NOT NULL AFTER Accion;

ALTER TABLE log ADD COLUMN Host VARCHAR(255) NOT NULL AFTER Addr;

ALTER TABLE log DROP COLUMN Host;

----

ALTER TABLE usr_data ADD COLUMN PendingPassword CHAR(22) NOT NULL AFTER Password;

----

ALTER TABLE usr_data DROP COLUMN PendingPassword;
CREATE TABLE pending_passwd (UsrCod INT NOT NULL,PendingPassword CHAR(22) NOT NULL,DateAndTime DATETIME NOT NULL,PRIMARY KEY (UsrCod));

-----

ALTER TABLE usr_data DROP COLUMN Question;
ALTER TABLE usr_data DROP COLUMN SecretAnswer;

UPDATE usr_data SET Password='' WHERE Password='kbG7BYtb_ghI57nsp2EMMw';

-----

ALTER TABLE sessions DROP COLUMN PasswordSHA512;
ALTER TABLE sessions ADD COLUMN PasswordSHA512 CHAR(86) NOT NULL AFTER Password;
ALTER TABLE usr_data ADD COLUMN PasswordSHA512 CHAR(86) NOT NULL AFTER Password;
ALTER TABLE pending_passwd ADD COLUMN PendingPasswordSHA512 CHAR(86) NOT NULL AFTER PendingPassword;

----
ALTER TABLE sessions DROP COLUMN Password;
ALTER TABLE pending_passwd DROP COLUMN PendingPassword;
----
ALTER TABLE usr_data DROP COLUMN Question;
ALTER TABLE usr_data DROP COLUMN SecretAnswer;
ALTER TABLE usr_data DROP COLUMN Password;
----
ALTER TABLE pending_passwd CHANGE PendingPasswordSHA512 PendingPassword CHAR(86) NOT NULL;
ALTER TABLE sessions CHANGE PasswordSHA512 Password CHAR(86) NOT NULL;
ALTER TABLE usr_data CHANGE PasswordSHA512 Password CHAR(86) NOT NULL;
----

ALTER TABLE sessions CHANGE IdSession IdSession CHAR(43) NOT NULL;
ALTER TABLE tst_status CHANGE IdSession IdSession CHAR(43) NOT NULL;
ALTER TABLE hidden_params CHANGE IdSesion IdSesion CHAR(43) NOT NULL;
ALTER TABLE usr_data CHANGE Photo Photo CHAR(43) NOT NULL;
----
ALTER TABLE usr_data ADD COLUMN HideSideCols ENUM('N','Y') NOT NULL AFTER NumNewMsgs;

ALTER TABLE usr_data CHANGE HideSideCols HideSideCols ENUM('N','Y') NOT NULL DEFAULT 'N';
ALTER TABLE usr_data CHANGE PublicPhoto PublicPhoto ENUM('N','Y') NOT NULL DEFAULT 'N';

ALTER TABLE sessions ADD COLUMN HideSideCols ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER LastRefresh;

---

ALTER TABLE usr_data ADD COLUMN SideCols TINYINT NOT NULL DEFAULT 3 AFTER HideSideCols;
ALTER TABLE sessions ADD COLUMN SideCols TINYINT NOT NULL DEFAULT 3 AFTER HideSideCols;

ALTER TABLE usr_data DROP COLUMN HideSideCols;
ALTER TABLE sessions DROP COLUMN HideSideCols;

----

CREATE TABLE log_recent (DegCod INT NOT NULL DEFAULT -1,CrsCod INT NOT NULL DEFAULT -1,DNI CHAR(16) NOT NULL,UsrType TINYINT NOT NULL,ClickTime DATETIME NOT NULL,TimeToGenerate INT NOT NULL,TimeToSend INT NOT NULL,Action VARCHAR(32) NOT NULL,IP CHAR(40) NOT NULL,Comments VARCHAR(255),INDEX (DegCod),INDEX(CrsCod),INDEX(DNI),INDEX(ClickTime,UsrType));

DELETE FROM log_recent;
INSERT log_recent SELECT * FROM log WHERE FechaHora>='20080501';

----

RENAME TABLE msg_sent TO msg_sent_old;
CREATE TABLE msg_received_old LIKE msg_received;
INSERT msg_received_old SELECT * FROM msg_received;
ALTER TABLE msg_received DROP COLUMN OldLstDst;

nice -n 19 rm -Rf swad-copia-2008-05-17; mkdir swad-copia-2008-06-09; chmod go-rwx swad-copia-2008-06-09; nice -n 19 cp -Rp /home swad html chatserver swad-copia-2008-06-09; df; ls -l

----

SELECT DATE_FORMAT(FechaHora,'%Y%m') AS Month,COUNT(*) FROM log WHERE FechaHora >= '20050101' AND FechaHora < '20080601' GROUP BY Month DESC;

SELECT DATE_FORMAT(FechaHora,'%Y%m') AS Month,COUNT(DISTINCT(DNI)) FROM log WHERE FechaHora >= '20050101' AND FechaHora < '20080601' AND UsrType<>'0' GROUP BY Month DESC;

-----

ALTER TABLE usr_data ADD COLUMN Theme CHAR(16) NOT NULL AFTER Gender;

ALTER TABLE hidden_params CHANGE ValorParam ParamValue TEXT;

ALTER TABLE usr_data ADD INDEX (Theme);
ALTER TABLE usr_data ADD INDEX (Language);
ALTER TABLE usr_data ADD INDEX (SideCols);

----

CREATE TABLE IP_prefs (IP CHAR(40) NOT NULL,LastChange DATETIME NOT NULL,Theme CHAR(16) NOT NULL,Language CHAR(2) NOT NULL,SideCols TINYINT NOT NULL,PRIMARY KEY (IP),INDEX(LastChange));

ALTER TABLE IP_prefs ADD COLUMN UsrCod INT NOT NULL DEFAULT -1 AFTER IP;
ALTER TABLE IP_prefs ADD INDEX (UsrCod);

----

ALTER TABLE usr_data CHANGE ShowSideCols SideCols TINYINT NOT NULL DEFAULT 3;
ALTER TABLE sessions CHANGE ShowSideCols SideCols TINYINT NOT NULL DEFAULT 3;
ALTER TABLE IP_prefs CHANGE ShowSideCols SideCols TINYINT NOT NULL DEFAULT 3;

ALTER TABLE sessions DROP COLUMN SideCols;

-----

ALTER TABLE usr_data DROP COLUMN LastPageMsgSnt;
ALTER TABLE usr_data DROP COLUMN LastPageMsgRec;
ALTER TABLE usr_data ADD COLUMN LastPageMsgRec INT NOT NULL DEFAULT 1 AFTER LastTab;
ALTER TABLE usr_data ADD COLUMN LastPageMsgSnt INT NOT NULL DEFAULT 1 AFTER LastPageMsgRec;

---

ALTER TABLE sessions ADD COLUMN LastPageMsgRec INT NOT NULL DEFAULT 1 AFTER LastRefresh;
ALTER TABLE sessions ADD COLUMN LastPageMsgSnt INT NOT NULL DEFAULT 1 AFTER LastPageMsgRec;
ALTER TABLE usr_data DROP COLUMN LastPageMsgSnt;
ALTER TABLE usr_data DROP COLUMN LastPageMsgRec;

----

(SELECT DegCod,'','' AS SN,'' FROM deg_admin WHERE UsrCod='12040' AND DegCod<'0') UNION (SELECT degrees.DegCod,degrees.ShortName AS SN,degrees.FullName,degrees.Logo FROM deg_admin,degrees WHERE deg_admin.UsrCod='12040' AND deg_admin.DegCod>='0' AND deg_admin.DegCod=degrees.DegCod) ORDER BY SN;

-----

ALTER TABLE usr_data ADD INDEX (Dept);
ALTER TABLE usr_data ADD INDEX (Centre);

----

SELECT * FROM usr_data WHERE MATCH (FirstName,Surname1,Surname2) AGAINST ('Ca�as');

SELECT FirstName,Surname1,Surname2,MATCH (FirstName,Surname1,Surname2) AGAINST ('Ca�as') FROM usr_data WHERE MATCH (FirstName,Surname1,Surname2) AGAINST ('Ca�as');

CREATE FULLTEXT INDEX Name ON usr_data (FirstName,Surname1,Surname2);

DROP INDEX Name ON usr_data;

CREATE FULLTEXT INDEX SubjectContent ON messages (Subject,Content);

 mysqldump -l -h swad.ugr.es -u swad -p swad > copia_base_datos_swad_2008_07_01.sql

SELECT MsgCod,Subject FROM messages WHERE UsrCod='1346' AND Deleted='N' ORDER BY MsgCod DESC;

----

ALTER TABLE DROP COLUMN crs_usr;
ALTER TABLE crs_usr ADD COLUMN UsrListType ENUM('classphoto','list') DEFAULT 'classphoto' NOT NULL AFTER ColsClassPhoto;


---

ALTER TABLE crs_usr ADD COLUMN UsrListType2 ENUM('classphoto','list') NOT NULL DEFAULT 'classphoto' AFTER NextAccTst;
UPDATE crs_usr SET UsrListType2=UsrListType;
ALTER TABLE crs_usr DROP COLUMN UsrListType;
ALTER TABLE crs_usr CHANGE UsrListType2 UsrListType ENUM('classphoto','list') NOT NULL DEFAULT 'classphoto';

----

ALTER TABLE crs_usr ADD COLUMN ListWithPhotos ENUM('N','Y') NOT NULL DEFAULT 'Y' AFTER ColsClassPhoto;

----

ALTER TABLE sta_degrees ADD COLUMN Gender ENUM ('unknown','female','male','all') NOT NULL DEFAULT 'all' AFTER DegCod;
DROP INDEX DegCod ON sta_degrees;
CREATE UNIQUE INDEX DegCod_Gender ON sta_degrees (DegCod,Gender);


SELECT degrees.DegCod FROM degrees,sta_degrees WHERE sta_degrees.Gender='all' AND sta_degrees.NumStds>'0' AND degrees.DegCod=sta_degrees.DegCod ORDER BY degrees.ShortName;

SELECT degrees.DegCod FROM degrees,sta_degrees WHERE sta_degrees.Gender='all' AND sta_degrees.NumStds>'0' AND degrees.DegCod=sta_degrees.DegCod ORDER BY sta_degrees.NumStdsWithPhoto DESC,sta_degrees.NumStds DESC,degrees.ShortName;

----

CREATE TABLE centres (CtrCod INT NOT NULL AUTO_INCREMENT,ShortName VARCHAR(32) NOT NULL,FullName VARCHAR(255) NOT NULL,WWW VARCHAR(255) NOT NULL,UNIQUE INDEX(CtrCod));

----

INSERT INTO centres (ShortName,FullName,WWW) VALUES ('ETS Arquitectura' ,'E. T. S. de Arquitectura' ,'http://www.ugr.es/~etsarqui/' ),('ETSI Inf. y Teleco' ,'E. T. S. de Ingenier�as Inform�tica y de Telecomunicaci�n' ,'http://etsiit.ugr.es/' ),('ETSI Caminos...' ,'E. T. S. de Ingenieros de Caminos, Canales y Puertos' ,'http://www.ugr.es/~ecaminos/' ),('EU Arq. T�cnica' ,'E. U. de Arquitectura T�cnica' ,'http://arqtec.ugr.es/' ),('EU Ciencias Salud' ,'E. U. de Ciencias de la Salud' ,'http://www.ugr.es/~ccsalud/' ),('EU Enf.(Ceuta)' ,'E. U. de Enfermer�a (Cruz Roja) F. Gonz�lez Azcune (Ceuta)' ,'http://www.ugr.es/~eueceuta/eueceuta.html'),('EU Enf.(Mel.)' ,'E. U. de Enfermer�a (Cruz Roja) M. Olmedo Jim�nez (Melilla)','http://www.ugr.es/facultades.htm'),('EU Enf. V. Nieves' ,'E. U. de Enfermer�a (S.A.S.) Virgen de las Nieves' ,'http://www.ugr.es/facultades.htm'),('EU Empres. (Mel.)' ,'E. U. de Estudios Empresariales (Melilla)' ,'http://www.ugr.es/~eues/' ),('EU Trabajo Social' ,'E. U. de Trabajo Social' ,'http://www.ugr.es/~eutrasoc/' ),('EU EGB Inmac.' ,'E. U. de Profesorado de E.G.B. La Inmaculada' ,'http://www.eulainmaculada.com/' ),('F Bellas Artes' ,'Facultad de Bellas Artes' ,'http://www.ugr.es/~fbbaa/' ),('F Biblioteconom...' ,'Facultad de Biblioteconom�a y Documentaci�n' ,'http://www.ugr.es/~fbd/' ),('F Ciencias' ,'Facultad de Ciencias' ,'http://www.ugr.es/~decacien/' ),('F C. A. F�s. Depor.','Facultad de Ciencias de la Actividad F�sica y el Deporte' ,'http://deporte.ugr.es/' ),('F C. Educaci�n' ,'Facultad de Ciencias de la Educaci�n' ,'http://www.ugr.es/~dcceduc/' ),('F C. del Trabajo' ,'Facultad de Ciencias del Trabajo' ,'http://www.ugr.es/~citrab/' ),('F C. Eco. Empres.' ,'Facultad de Ciencias Econ�micas y Empresariales' ,'http://www.ugr.es/~fccee/' ),('F C. Pol�t. Sociol.','Facultad de Ciencias Pol�ticas y Sociolog�a' ,'http://www.ugr.es/~ccpolsoc/' ),('F Derecho' ,'Facultad de Derecho' ,'http://www.ugr.es/~wderecho/' ),('F Ed. Hum.(Ceuta)' ,'Facultad de Educaci�n y Humanidades (Ceuta)' ,'http://www.ugr.es/~w3ceuta/' ),('F Ed. Hum.(Mel.)' ,'Facultad de Educaci�n y Humanidades (Melilla)' ,'http://www.ugr.es/~faedumel/' ),('F Farmacia' ,'Facultad de Farmacia' ,'http://farmacia.ugr.es/' ),('F Filos. y Letras' ,'Facultad de Filosof�a y Letras' ,'http://www.ugr.es/~letras/' ),('F Medicina' ,'Facultad de Medicina' ,'http://www.ugr.es/~facmed/' ),('F Odontolog�a' ,'Facultad de Odontolog�a' ,'http://www.ugr.es/~odonto/' ),('F Psicolog�a' ,'Facultad de Psicolog�a' ,'http://www.ugr.es/~psicolo/' ),('F Traduc. Interp.' ,'Facultad de Traducci�n e Interpretaci�n' ,'http://www.ugr.es/~factrad/' );

ALTER TABLE usr_data ADD COLUMN CtrCod INT NOT NULL DEFAULT -1 AFTER Centre;
CREATE INDEX CtrCod ON usr_data (CtrCod);
UPDATE usr_data,centres SET usr_data.CtrCod=centres.CtrCod WHERE usr_data.UsrType>='3' AND usr_data.Centre=centres.FullName;
ALTER TABLE usr_data DROP COLUMN Centre;

-----

CREATE TABLE departments (DptCod INT NOT NULL AUTO_INCREMENT,ShortName VARCHAR(32) NOT NULL,FullName VARCHAR(255) NOT NULL,WWW VARCHAR(255) NOT NULL,UNIQUE INDEX(DptCod));
INSERT INTO departments (ShortName,FullName,WWW) VALUES ('�lgebra','�lgebra' ,'http://www.ugr.es/~algebra/' ), ('An�lisis Geogr�...' ,'An�lisis Geogr�fico Regional y Geograf�a F�sica' ,'http://www.ugr.es/~geofireg/' ), ('An�lisis Mat.' ,'An�lisis Matem�tico' ,'http://www.ugr.es/~dpto_am/' ), ('Anat. Patol�gica...','Anatom�a Patol�gica e Historia de la Ciencia' ,'http://www.ugr.es/~hciencia/' ), ('Anat. y Embr...' ,'Anatom�a y Embriolog�a Humana' ,'http://www.ugr.es/~cmorfolo/' ), ('Antr. Social' ,'Antropolog�a Social' ,'http://www.ugr.es/~antrosoc/' ), ('Arq. Tec. Comp.' ,'Arquitectura y Tecnolog�a de Computadores' ,'http://atc.ugr.es/' ), ('Bibliot. y Docum.' ,'Biblioteconom�a y Documentaci�n' ,'http://www.ugr.es/~dbibliot/' 	 ), ('Biolog�a Animal' ,'Biolog�a Animal' ,'http://www.ugr.es/~dptobae/' ), ('Biolog�a Celular' ,'Biolog�a Celular' ,'http://www.ugr.es/~celular/' ), ('Bioqu�mica I...' ,'Bioqu�mica y Biolog�a Molecular I (Fac. Ciencias)' ,'http://www.ugr.es/~bbm1/' ), ('Bioqu�mica II...' ,'Bioqu�mica y Biolog�a Molecular II (Fac. Farmacia)' ,'http://www.ugr.es/~gebqmed/' ), ('Bioqu�mica III...' ,'Bioqu�mica y Biolog�a Molecular III e Inmunolog�a (Fac. Medicina)' ,'http://www.ugr.es/~gebqmed/' ), ('Bot�nica' ,'Bot�nica' ,'http://www.ugr.es/~botanica/' ), ('Ciencia Pol�tica...','Ciencia Pol�tica y de la Administraci�n' ,'http://www.ugr.es/~webptca/' ), ('C. Comput. e I. A.' ,'Ciencias de la Computaci�n e Inteligencia Artificial' ,'http://decsai.ugr.es/' ), ('Cirug�a....' ,'Cirug�a y sus especialidades' ,'http://www.ugr.es/~cirugia/index.htm' ), ('Comerc. e Inv...' ,'Comercializaci�n e Investigaci�n de Mercados' ,'http://www.ugr.es/~fccee/doc/Departamentos/depComercializacion.htm'), ('Constr. Arquitect.' ,'Construcciones Arquitect�nicas' ,'http://www.ugr.es/~consarq/' ), ('Dcho. Administr.' ,'Derecho Administrativo' ,'http://www.ugr.es/~dereadmi/' ), ('Dcho. Civil' ,'Derecho Civil' ,'http://www.ugr.es/~dpto_dc/' ), ('Dcho. Constituc.' ,'Derecho Constitucional' ,'http://www.ugr.es/~pwdconst/' ), ('Dcho. Trabajo...' ,'Derecho del Trabajo y de la Seguridad Social' ,'http://www.ugr.es/~deretrab/' ), ('Dcho. Financ...' ,'Derecho Financiero y Tributario' ,'http://www.ugr.es/departamentos.htm' ), ('Dcho. I. Privado...','Derecho Internacional Privado e Historia del Derecho' ,'http://www.ugr.es/~dintpriv/' ), ('Dcho. I. P�blico...','Derecho Internacional P�blico y Relaciones Internacionales' ,'http://www.ugr.es/~deintpub/' ), ('Dcho. Mercantil...' ,'Derecho Mercantil y Derecho Romano' ,'http://www.ugr.es/~dmeroma/' ), ('Dcho. Penal' ,'Derecho Penal' ,'http://www.ugr.es/~deppenal/' ), ('Dcho. Procesal...' ,'Derecho Procesal y Eclesi�stico' ,'http://www.ugr.es/departamentos.htm' ), ('Dibujo' ,'Dibujo' ,'http://www.ugr.es/departamentos.htm' ), ('Did. Expresi�n...' ,'Did�ctica de la Expresi�n Musical, Pl�stica y Corporal' ,'http://www.ugr.es/~demuplac/' ), ('Did. Lengua Liter.' ,'Did�ctica de la Lengua y la Literatura' ,'http://www.ugr.es/~didlen/' ), ('Did. Matem�tica' ,'Did�ctica de la Matem�tica' ,'http://www.ugr.es/~dpto_did/' ), ('Did. Ciencias Exp.' ,'Did�ctica de las Ciencias Experimentales' ,'http://www.ugr.es/~diccexp/' ), ('Did. Ciencias Soc.' ,'Did�ctica de las Ciencias Sociales' ,'http://www.ugr.es/departamentos.htm' ), ('Did. Org. Escolar' ,'Did�ctica y Organizaci�n Escolar' ,'http://www.ugr.es/~didacoe/' ), ('Ecolog�a' ,'Ecolog�a' ,'http://www.ugr.es/ugr/index.php?page=departamentos_institutos/departamentos/fichas/ecologia'), ('Econ. Aplicada' ,'Econom�a Aplicada' ,'http://www.ugr.es/~ecoapli/' ), ('Econ. Financ...' ,'Econom�a Financiera y Contabilidad' ,'http://www.ugr.es/~aedem/aedem15.htm' ), ('Econ. Internac...' ,'Econom�a Internacional y de Espa�a' ,'http://www.ugr.es/departamentos.htm' ), ('Edafolog�a y Q.A.' ,'Edafolog�a y Qu�mica Agr�cola' ,'http://www.ugr.es/departamentos.htm' ), ('Edu. F�sica...' ,'Educaci�n F�sica y Deportiva' ,'http://www.ugr.es/departamentos.htm' ), ('Electromagnet...' ,'Electromagnetismo y F�sica de la Materia' ,'http://ergodic.ugr.es/efm/' );
INSERT INTO departments (ShortName,FullName,WWW) VALUES ('Electr�nica y T.C.' ,'Electr�nica y Tecnolog�a de Computadores' ,'http://ceres.ugr.es/dpto/' ), ('Enfermer�a' ,'Enfermer�a' ,'http://www.ugr.es/~enfermer/' ), ('Escultura' ,'Escultura' ,'http://www.ugr.es/~escultura/' ), ('Estad�stica e IO' ,'Estad�stica e Investigaci�n Operativa' ,'http://www.ugr.es/~udocente/' ), ('Estomatolog�a' ,'Estomatolog�a' ,'http://www.ugr.es/~estomato/' ), ('Estratigr. y Pal.' ,'Estratigraf�a y Paleontolog�a' ,'http://www.ugr.es/~estratig/' ), ('Estudios Sem�t.' ,'Estudios Sem�ticos' ,'http://www.ugr.es/~estsemi/' ), ('Expr. Gr�f. Arq...' ,'Expresi�n Gr�fica Arquitect�nica y en la Ingenier�a' ,'http://www.ugr.es/~expregra/' ), ('Farmacia y T. F.' ,'Farmacia y Tecnolog�a Farmac�utica' ,'http://www.ugr.es/departamentos.htm' ), ('Farmacolog�a' ,'Farmacolog�a' ,'http://www.ugr.es/~farmaco/' ), ('Filol. Francesa' ,'Filolog�a Francesa' ,'http://www.ugr.es/departamentos.htm' ), ('Filol. Griega' ,'Filolog�a Griega' ,'http://www.ugr.es/~odiseo/' ), ('Filol. Ingl. Alem.' ,'Filolog�as Inglesa y Alemana' ,'http://dpingles.ugr.es/' ), ('Filol. Latina' ,'Filolog�a Latina' ,'http://www.ugr.es/~filatina/'), ('Filol. Rom�nica...' ,'Filolog�as Rom�nica, Italiana, Gallego-Portuguesa y Catalana' ,'http://www.ugr.es/~frigpc/' ), ('Filosof�a' ,'Filosof�a' ,'http://www.ugr.es/~filosofi/' ), ('Filosof�a Dcho.' ,'Filosof�a del Derecho' ,'http://www.ugr.es/~filode/' ), ('F�sica Aplicada' ,'F�sica Aplicada' ,'http://www.ugr.es/txt/es/fisiapli.html' ), ('F�sica At�mica...' ,'F�sica At�mica, Molecular y Nuclear','http://www.ugr.es/~amaro/depart.html' ), ('F�sica T. Cosmos' ,'F�sica Te�rica y del Cosmos' ,'http://www.ugr.es/~fteorica/' ), ('Fisiolog�a' ,'Fisiolog�a' ,'http://www.ugr.es/departamentos.htm' ), ('Fisiolog�a Vegetal' ,'Fisiolog�a Vegetal' ,'http://www.ugr.es/~fisioveg/' ), ('Fisioterapia' ,'Fisioterapia' ,'http://www.ugr.es/departamentos.htm' ), ('Gen�tica' ,'Gen�tica' ,'http://www.ugr.es/~dpto_gen/' ), ('Geodin�mica' ,'Geodin�mica' ,'http://www.ugr.es/~geodina/departamento/'), ('Geogr. Humana' ,'Geograf�a Humana' ,'http://www.ugr.es/~geohum/' ), ('Geometr�a y Top.' ,'Geometr�a y Topolog�a' ,'http://www.ugr.es/~geometry/' ), ('Histolog�a' ,'Histolog�a' ,'http://histolii.ugr.es/' ), ('Historia Antigua' ,'Historia Antigua' ,'http://www.ugr.es/~hantigua/' ), ('Historia Contemp.' ,'Historia Contempor�nea' ,'http://www.ugr.es/~histocon/' );
INSERT INTO departments (ShortName,FullName,WWW) VALUES ('Historia del Arte' ,'Historia del Arte' ,'http://www.ugr.es/~histarte/' ), ('H. Medieval...' ,'Historia Medieval y Ciencias y T�cnicas Historiogr�ficas' ,'http://www.ugr.es/~medieval/' ), ('H. Moderna...' ,'Historia Moderna y de Am�rica' ,'http://www.ugr.es/~modeamer/' ), ('Ing. Civil' ,'Ingenier�a Civil' ,'http://www.ugr.es/~ingcivil/' ), ('Ing. Qu�mica' ,'Ingenier�a Qu�mica' ,'http://www.ugr.es/~iquimica/'), ('Lengua Espa�ola' ,'Lengua Espa�ola' ,'http://www.ugr.es/~lenguaes/' ), ('Leng. y Sis. Inf.' ,'Lenguajes y Sistemas Inform�ticos' ,'http://lsi.ugr.es/' ), ('Lin.Gen. Teo.Lit.' ,'Ling��stica General y Teor�a de la Literatura' ,'http://www.ugr.es/~teorial/' ), ('Literatura Espa�.' ,'Literatura Espa�ola' ,'http://www.ugr.es/~litespa/' ), ('Matem. Aplicada' ,'Matem�tica Aplicada' ,'http://www.ugr.es/~mateapli/' ), ('Medicina' ,'Medicina' ,'http://www.ugr.es/~medicina/' ), ('Med. Legal y Psiq.' ,'Medicina Legal y Psiquiatr�a' ,'http://www.ugr.es/~dpto_mlp/' ), ('Med.Pr. Salud Pbl.' ,'Medicina Preventiva y Salud P�blica' ,'http://www.ugr.es/~dpto_prev/' ), ('Mec�n. Medios...' ,'Mec�nica de Medios Continuos y Teor�a de Estructuras' ,'http://www.ugr.es/~estruct/' ), ('M�todos C. Eco...' ,'M�todos Cuantitativos para la Econom�a y la Empresa' ,'http://www.ugr.es/~metcuant/' ), ('M�todos Invest...' ,'M�todos de Investigaci�n y Diagn�stico en Educaci�n' ,'http://www.ugr.es/~mide/' ), ('Microbiolog�a' ,'Microbiolog�a' ,'http://www.ugr.es/~dptomic/' ), ('Mineralog�a y Petr.','Mineralog�a y Petrolog�a' ,'http://www.ugr.es/~minpet/' );
INSERT INTO departments (ShortName,FullName,WWW) VALUES ('Nutrici�n y Brom.' ,'Nutrici�n y Bromatolog�a' ,'http://www.ugr.es/~mdruiz/' ), ('Obstetricia y Gin.' ,'Obstetricia y Ginecolog�a' ,'http://www.ugr.es/~obstetri/' ), ('�ptica' ,'�ptica' ,'http://www.ugr.es/~geoptica/' ), ('Org. Empresas' ,'Organizaci�n de Empresas' ,'http://www.ugr.es/~orgaemp/' ), ('Parasitolog�a' ,'Parasitolog�a' ,'http://www.ugr.es/~parasito/' ), ('Pedagog�a' ,'Pedagog�a' ,'http://www.ugr.es/~dpto_ped/' ), ('Pediatr�a' ,'Pediatr�a' ,'http://www.ugr.es/~pwpedia/' ), ('Person., Ev. y Tr.' ,'Personalidad, Evaluaci�n y Tratamiento' ,'http://www.ugr.es/departamentos.htm' ), ('Pintura' ,'Pintura' ,'http://www.ugr.es/~pwpint/' ), ('Prehistoria y Arq.' ,'Prehistoria y Arqueolog�a' ,'http://www.ugr.es/~arqueol/' ), ('Psi. Evolutiva...' ,'Psicolog�a Evolutiva y de la Educaci�n' ,'http://www.ugr.es/~psicoevo/' ), ('Psi. Experiment...' ,'Psicolog�a Experimental y Fisiolog�a' ,'http://www.ugr.es/departamentos.htm' ), ('Psi. Social...' ,'Psicolog�a Social y Metodolog�a de las Ciencias del Comportamiento','http://www.ugr.es/departamentos.htm' ), ('Qu�mica Anal�tica' ,'Qu�mica Anal�tica' ,'http://www.ugr.es/~qanaliti/' ), ('Qu�mica F�sica' ,'Qu�mica F�sica' ,'http://www.ugr.es/~qmfisica/' ), ('Qu�mica Inorg.' ,'Qu�mica Inorg�nica' ,'http://www.ugr.es/~mquiros/departa.htm' ), ('Qu�mica Far. y Org.','Qu�mica Farmac�utica y Org�nica' ,'http://www.ugr.es/departamentos.htm' ), ('Qu�mica Org�nica' ,'Qu�mica Org�nica' ,'http://www.ugr.es/~qorgani/' ), ('Radiolog. Med.F�s.' ,'Radiolog�a y Medicina F�sica' ,'http://www.ugr.es/~dptorad/' ), ('Sociolog�a' ,'Sociolog�a' ,'http://www.departamentosociologiagranada.org/'), ('Trabajo Social...' ,'Trabajo Social y Servicios Sociales' ,'http://www.ugr.es/~tsocial/' ), ('Traducci�n e Int.' ,'Traducci�n e Interpretaci�n' ,'http://www.ugr.es/~dpto_ti/' ), ('Teor�a Se�al,...' ,'Teor�a de la Se�al, Telem�tica y Comunicaciones' ,'http://www.ugr.es/~tstc/' ), ('Teor�a e H. Econ.' ,'Teor�a e Historia Econ�mica' ,'http://www.ugr.es/~fccee/doc/Departamentos/depHistEcon.htm');

----

ALTER TABLE usr_data ADD COLUMN DptCod INT NOT NULL DEFAULT -1 AFTER Dept;
CREATE INDEX DptCod ON usr_data (DptCod);
UPDATE usr_data,departments SET usr_data.DptCod=departments.DptCod WHERE usr_data.UsrType>='3' AND usr_data.Dept=departments.FullName;
ALTER TABLE usr_data DROP COLUMN Dept;

----

CREATE INDEX DptCod ON usr_data (DptCod);

CREATE UNIQUE INDEX FirstPstCod ON forum_thread (FirstPstCod);
CREATE UNIQUE INDEX LastPstCod ON forum_thread (LastPstCod);

-----

SELECT COUNT(DISTINCT forum_thread.ThrCod),COUNT(*) FROM forum_thread,forum_post WHERE forum_thread.ForumType='0' AND forum_thread.ThrCod=forum_post.ThrCod;

SELECT COUNT(*) FROM forum_thread,forum_post WHERE forum_thread.ForumType='8' AND forum_thread.LastPstCod=forum_post.MsgCod AND forum_post.ModifTime>'20081000';

SELECT COUNT(UsrCod) FROM usr_data WHERE UsrCod NOT IN (SELECT DISTINCT UsrCod FROM crs_usr);
SELECT UsrCod FROM usr_data WHERE UsrCod NOT IN (SELECT DISTINCT UsrCod FROM crs_usr);

SELECT UsrCod FROM usr_data WHERE DNI NOT IN (SELECT DISTINCT DNI FROM log WHERE UNIX_TIMESTAMP(FechaHora) > UNIX_TIMESTAMP()-31104000)

SELECT COUNT(*) FROM log WHERE DNI='24243620' AND (UNIX_TIMESTAMP(FechaHora) > UNIX_TIMESTAMP()-31104000);

SELECT column1 FROM t1 WHERE EXISTS (SELECT DNI FROM log WHERE UNIX_TIMESTAMP(FechaHora) > UNIX_TIMESTAMP()-31104000);

SELECT UsrCod FROM usr_data WHERE NOT EXISTS (SELECT * FROM log WHERE log.DNI = usr_data.DNI AND UNIX_TIMESTAMP(FechaHora) > UNIX_TIMESTAMP()-20);
SELECT UsrCod FROM usr_data WHERE NOT EXISTS (SELECT * FROM log WHERE (UNIX_TIMESTAMP(FechaHora) > UNIX_TIMESTAMP()-20) AND log.DNI = usr_data.DNI);

SELECT * FROM log WHERE DNI = '24243619' AND (UNIX_TIMESTAMP(FechaHora) > UNIX_TIMESTAMP()-20);
----

ALTER TABLE usr_data ADD COLUMN LastTab2 TINYINT NOT NULL AFTER SideCols;
UPDATE usr_data SET LastTab2=LastTab;
ALTER TABLE usr_data DROP COLUMN LastTab;
ALTER TABLE usr_data CHANGE LastTab2 LastTab TINYINT NOT NULL;

ALTER TABLE usr_data ADD COLUMN LastTime DATETIME NOT NULL AFTER LastTab;

UPDATE usr_data,log SET usr_data.LastTime=MAX(log.FechaHora) WHERE usr_data.DNI=log.DNI;
UPDATE usr_data,log SET usr_data.LastTime=log.FechaHora WHERE log.DNI LIKE '242436%' AND log.FechaHora=(SELECT MAX(FechaHora) FROM log WHERE DNI='24243620') AND usr_data.DNI=log.DNI;

SELECT DNI,MAX(FechaHora) FROM log WHERE DNI LIKE '242436%' GROUP BY DNI;

SELECT DNI,MAX(FechaHora) AS FH FROM log WHERE DNI LIKE '242436%' GROUP BY DNI;

DROP TABLE IF EXISTS log_tmp;
CREATE TEMPORARY TABLE log_tmp (DNI CHAR(16) NOT NULL,LastTime DATETIME NOT NULL,UNIQUE INDEX (DNI),INDEX(LastTime)) SELECT DNI,MAX(FechaHora) AS LastTime FROM log GROUP BY DNI;
UPDATE usr_data,log_tmp SET usr_data.LastTime=log_tmp.LastTime WHERE usr_data.DNI=log_tmp.DNI;

CREATE INDEX LastTime ON usr_data (LastTime);

SELECT DNI,LastTime FROM usr_data WHERE UNIX_TIMESTAMP(LastTime) < UNIX_TIMESTAMP()-(12*30*24*60*60) AND UsrCod NOT IN (SELECT DISTINCT UsrCod FROM crs_usr);

------------------

(SELECT centres.CtrCod,centres.ShortName,centres.FullName,centres.WWW,COUNT(*) FROM centres,usr_data WHERE usr_data.UsrType='3' AND usr_data.CtrCod=centres.CtrCod GROUP BY usr_data.CtrCod UNION SELECT CtrCod,ShortName,FullName,WWW,'0' FROM centres WHERE CtrCod NOT IN (SELECT DISTINCT CtrCod FROM usr_data WHERE UsrType='3')) ORDER BY centres.FullName;


(SELECT centres.CtrCod,centres.ShortName,centres.FullName,centres.WWW,COUNT(*) AS NumTchs FROM centres,usr_data WHERE usr_data.UsrType='3' AND usr_data.CtrCod=centres.CtrCod GROUP BY centres.CtrCod) UNION (SELECT CtrCod,ShortName,FullName,WWW,'0' FROM centres WHERE CtrCod NOT IN (SELECT DISTINCT CtrCod FROM usr_data WHERE UsrType='3')) ORDER BY NumTchs DESC;


------

SELECT COUNT(*) FROM crs_usr WHERE UsrCod='24243619' AND CrsCod IN (SELECT courses.CrsCod FROM deg_admin,courses WHERE deg_admin.UsrCod='34853319' and (deg_admin.DegCod<0 or deg_admin.DegCod=courses.DegCod));

-------

DROP TABLE institution;
CREATE TABLE institutions (InsCod INT NOT NULL AUTO_INCREMENT,ShortName VARCHAR(32) NOT NULL,FullName VARCHAR(255) NOT NULL,Logo VARCHAR(16) NOT NULL,WWW VARCHAR(255) NOT NULL,UNIQUE INDEX(InsCod));

ALTER TABLE usr_data ADD COLUMN InsCod INT NOT NULL DEFAULT -1 AFTER PublicPhoto;
CREATE INDEX InsCod ON usr_data (InsCod);

update usr_data set CtrCod='30' where DNI='74660014' OR DNI='74648378' OR DNI='24259704' OR DNI='24181974' OR DNI='74641868' OR DNI='P74665785' OR DNI='X2402791' OR DNI='52565549' OR DNI='44283270';
update usr_data set InsCod='1' where DNI='24219870' OR DNI='74660014' OR DNI='74648378' OR DNI='24259704' OR DNI='24181974' OR DNI='74641868' OR DNI='P74665785' OR DNI='X2402791' OR DNI='52565549' OR DNI='44283270';

ALTER TABLE centres ADD COLUMN InsCod INT NOT NULL AFTER CtrCod;
CREATE INDEX InsCod ON centres (InsCod);
ALTER TABLE departments ADD COLUMN InsCod INT NOT NULL AFTER DptCod;
CREATE INDEX InsCod ON departments (InsCod);

----

SELECT centres.ShortName,centres.FullName,centres.WWW,COUNT(*) FROM centres,usr_data WHERE centres.CtrCod='%ld' AND usr_data.UsrType='%u' AND centres.CtrCod=usr_data.CtrCod GROUP BY centres.CtrCod;

-----


ALTER TABLE degrees ADD COLUMN PlcCod INT NOT NULL AFTER CampusCod;
CREATE INDEX PlcCod ON degrees (PlcCod);

CREATE TABLE places (PlcCod INT NOT NULL AUTO_INCREMENT,ShortName VARCHAR(32) NOT NULL,FullName VARCHAR(255) NOT NULL,UNIQUE INDEX(PlcCod));

------

CREATE TABLE holidays (HldCod INT NOT NULL AUTO_INCREMENT,PlcCod INT NOT NULL DEFAULT -1,Holiday DATE NOT NULL,Name VARCHAR(255) NOT NULL,UNIQUE INDEX (HldCod),INDEX(PlcCod));

------

ALTER TABLE degrees DROP COLUMN CampusCod;

-----

ALTER TABLE holidays CHANGE Holiday HldDate DATE NOT NULL;
ALTER TABLE holidays CHANGE HldDate StartDate DATE NOT NULL;

ALTER TABLE holidays ADD COLUMN Type ENUM('holiday','non_school') NOT NULL AFTER PlcCod;
ALTER TABLE holidays ADD COLUMN EndDate DATE NOT NULL AFTER StartDate;
DESCRIBE holidays;

ALTER TABLE holidays CHANGE HldDate StartDate DATE NOT NULL;

ALTER TABLE holidays CHANGE Type Type TINYINT NOT NULL;
ALTER TABLE holidays CHANGE Type HldTyp TINYINT NOT NULL;

------

SELECT institutions.InsCod,institutions.ShortName,institutions.FullName,institutions.Logo,institutions.WWW,COUNT(*) AS NumUsrs FROM institutions,usr_data WHERE usr_data.InsCod=institutions.InsCod GROUP BY institutions.InsCod;


SELECT T1.InsCod,institutions.ShortName,T1.NumStds,T2.NumTchs FROM
(SELECT institutions.InsCod,COUNT(*) AS NumStds FROM institutions,usr_data WHERE usr_data.UsrType='2' AND usr_data.InsCod=institutions.InsCod GROUP BY institutions.InsCod) AS T1,
(SELECT institutions.InsCod,COUNT(*) AS NumTchs FROM institutions,usr_data WHERE usr_data.UsrType='3' AND usr_data.InsCod=institutions.InsCod GROUP BY institutions.InsCod) AS T2,
institutions
WHERE T1.InsCod=T2.InsCod;

) AS T
GROUP BY InsCod;

SELECT institutions.InsCod,institutions.ShortName,institutions.FullName,institutions.Logo,institutions.WWW,COUNT(DISTINCT usr_data.UsrCod,usr_data.DNI) FROM institutions,usr_data WHERE usr_data.InsCod=institutions.InsCod GROUP BY institutions.InsCod;


SELECT institutions.InsCod,institutions.ShortName,COUNT(DISTINCT usr_data.UsrCod,usr_data.DNI) FROM institutions,usr_data WHERE usr_data.InsCod=institutions.InsCod GROUP BY institutions.InsCod;

-----


ALTER TABLE crs_grp CHANGE MaxAlumnos MaxStudents INT NOT NULL;
ALTER TABLE crs_grp CHANGE NumAlumnos NumStudents INT NOT NULL;

----

ALTER TABLE degrees ADD COLUMN InsCod INT NOT NULL AFTER DegTypCod;
CREATE INDEX InsCod ON degrees (InsCod);

-----

ALTER TABLE calls_for_exams CHANGE CodConv CalCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE calls_for_exams CHANGE Asignatura CrsFullName VARCHAR(255) NOT NULL;
ALTER TABLE calls_for_exams CHANGE Curso Year TINYINT NOT NULL;
ALTER TABLE calls_for_exams CHANGE Tipo CallType VARCHAR(255) NOT NULL;
ALTER TABLE calls_for_exams CHANGE FechaConv CallDate DATETIME NOT NULL;
ALTER TABLE calls_for_exams CHANGE FechaExamen ExamDate DATETIME NOT NULL;
ALTER TABLE calls_for_exams CHANGE Duracion ExamLength TIME NOT NULL;
ALTER TABLE calls_for_exams CHANGE Lugar Place TEXT NOT NULL;
ALTER TABLE calls_for_exams CHANGE Modalidad Modality TEXT NOT NULL;
ALTER TABLE calls_for_exams CHANGE Estructura Structure TEXT NOT NULL;
ALTER TABLE calls_for_exams CHANGE DocExigida DocRequired TEXT NOT NULL;
ALTER TABLE calls_for_exams CHANGE MatObligado MatRequired TEXT NOT NULL;
ALTER TABLE calls_for_exams CHANGE MatPermitido MatAllowed TEXT NOT NULL;
ALTER TABLE calls_for_exams CHANGE Indicaciones Indications TEXT NOT NULL;

-----

/*
typedef enum {FORUM_COURSE_USRS     = 0 --> 0,
	      FORUM_COURSE_TCHS = 1 --> 1,
              FORUM_DEGREE_USRS     = 2 --> 2,
              FORUM_DEGREE_TCHS = 3 --> 3,
              FORUM_CENTER_USRS     = 4 --> 4,	// Not used
              FORUM_CENTER_TCHS = 5 --> 5,	// Not used
              FORUM_GLOBAL_USRS     = 6 --> 8,
              FORUM_GLOBAL_TCHS = 7 --> 9,
              FORUM_SWAD_USRS       = 8 --> 10,
              FORUM_SWAD_TCHS   = 9 --> 11} OLD_ForumType_t;	// Don't change numbers (used in database)
typedef enum {FORUM_COURSE_USRS          =  0,
	      FORUM_COURSE_TCHS      =  1,
              FORUM_DEGREE_USRS          =  2,
              FORUM_DEGREE_TCHS      =  3,
              FORUM_CENTER_USRS          =  4,	// Not used
              FORUM_CENTER_TCHS      =  5,	// Not used
              FORUM_INSTITUTION_USRS     =  6,
              FORUM_INSTITUTION_TCHS =  7,
              FORUM_GLOBAL_USRS          =  8,
              FORUM_GLOBAL_TCHS      =  9,
              FORUM_SWAD_USRS            = 10,
              FORUM_SWAD_TCHS        = 11} For_ForumType_t;	// Don't change numbers (used in database)
*/
UPDATE forum_thread SET ForumType='11' WHERE ForumType='9';
UPDATE forum_thread SET ForumType='10' WHERE ForumType='8';
UPDATE forum_thread SET ForumType='9' WHERE ForumType='7';
UPDATE forum_thread SET ForumType='8' WHERE ForumType='6';

----

ALTER TABLE sta_degrees ADD COLUMN TimeAvgPhoto DATETIME NOT NULL AFTER NumStdsWithPhoto;
CREATE INDEX TimeAvgPhoto ON sta_degrees (TimeAvgPhoto);

-----

(SELECT DegCod,'0000-00-00' AS TimeAvgPhoto FROM degrees WHERE DegCod NOT IN (SELECT DISTINCT DegCod FROM sta_degrees)) UNION (SELECT DegCod,TimeAvgPhoto FROM sta_degrees WHERE UNIX_TIMESTAMP(TimeAvgPhoto)<UNIX_TIMESTAMP()-60) ORDER BY TimeAvgPhoto LIMIT 1;

ALTER TABLE sta_degrees ADD COLUMN TimeToComputeAvgPhoto INT NOT NULL DEFAULT 0 AFTER TimeAvgPhoto;
ALTER TABLE sta_degrees DROP COLUMN TimeToComputeAvgPhoto;
ALTER TABLE sta_degrees ADD COLUMN TimeToComputeAvgPhoto INT NOT NULL DEFAULT -1 AFTER TimeAvgPhoto;

----

SELECT DegCod FROM (SELECT DegCod FROM sta_degrees WHERE UNIX_TIMESTAMP(TimeAvgPhoto)<UNIX_TIMESTAMP()-86400 ORDER BY TimeAvgPhoto LIMIT 10) AS D ORDER BY RAND() LIMIT 10;

----

SELECT COUNT(*) FROM courses,crs_usr,usr_data WHERE courses.DegCod='16' AND courses.CrsCod=crs_usr.CrsCod AND crs_usr.UsrCod=usr_data.UsrCod AND usr_data.Language='es';
SELECT * FROM courses,crs_usr,usr_data WHERE courses.DegCod='16' AND courses.CrsCod=crs_usr.CrsCod AND crs_usr.UsrCod=usr_data.UsrCod AND usr_data.Language='es';

-----


CREATE TABLE links (LnkCod INT NOT NULL AUTO_INCREMENT,ShortName VARCHAR(32) NOT NULL,FullName VARCHAR(255) NOT NULL,UNIQUE INDEX(LnkCod));
ALTER TABLE links ADD COLUMN WWW VARCHAR(255) NOT NULL AFTER FullName;

----

CREATE INDEX Deleted ON messages (Deleted);
CREATE INDEX Deleted ON msg_received (Deleted);

-----


DROP TABLE exp_tree_size;
CREATE TABLE exp_tree_size (TreeType TINYINT NOT NULL,CrsCod INT NOT NULL DEFAULT -1,GrpCod INT NOT NULL DEFAULT -1,UsrCod INT NOT NULL DEFAULT -1,NumLevels INT NOT NULL,NumFolders INT NOT NULL,NumFiles INT NOT NULL,TotalSize BIGINT NOT NULL,UNIQUE INDEX(TreeType,CrsCod,GrpCod,UsrCod),INDEX(CrsCod),INDEX(GrpCod),INDEX(UsrCod));

select CrsCod from courses where DegCod in (select DegCod from degrees where DegTypCod in ('2','4','7','9','16','17'));

----


SELECT SUM(NumLevels),SUM(NumFolders),SUM(NumFiles),SUM(TotalSize) FROM exp_tree_size WHERE TreeType='3';

SELECT MAX(exp_tree_size.NumLevels),SUM(exp_tree_size.NumFolders),SUM(exp_tree_size.NumFiles),SUM(exp_tree_size.TotalSize) FROM exp_tree_size,crs_usr WHERE exp_tree_size.TreeType='9' AND crs_usr.CrsCod='1859' AND exp_tree_size.UsrCod=crs_usr.UsrCod;

DELETE FROM exp_tree_size WHERE TreeType='6';

SELECT COUNT(CrsCod),MAX(NumLevels),SUM(NumFolders),SUM(NumFiles),SUM(TotalSize) FROM exp_tree_size WHERE TreeType='7';

SELECT COUNT(DISTINCT CrsCod),COUNT(DISTINCT UsrCod),MAX(NumLevels),SUM(NumFolders),SUM(NumFiles),SUM(TotalSize) FROM exp_tree_size;


-----

ALTER TABLE IP_prefs ADD COLUMN SecureIcons ENUM('N','Y') NOT NULL DEFAULT 'Y' AFTER SideCols;
ALTER TABLE usr_data ADD COLUMN SecureIcons ENUM('N','Y') NOT NULL DEFAULT 'Y' AFTER SideCols;
ALTER TABLE usr_data ADD INDEX (SecureIcons);

-----


ALTER TABLE IP_prefs DROP COLUMN SecureIcons;


-----

select tst_answers.Respuesta from tst_answers,tst_questions where tst_questions.AnsType='T/F' and tst_questions.CodPrg=tst_answers.CodPrg;

UPDATE tst_answers,tst_questions SET tst_answers.Respuesta='T' WHERE tst_questions.AnsType='T/F' AND tst_questions.CodPrg=tst_answers.CodPrg AND tst_answers.Respuesta='V';

----

ALTER TABLE tst_questions CHANGE CodPrg QstCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE tst_question_tags CHANGE CodPrg QstCod INT NOT NULL;
ALTER TABLE tst_answers CHANGE CodPrg QstCod INT NOT NULL;

----

ALTER TABLE tst_answers CHANGE IndRes AnsInd TINYINT NOT NULL;

-----

ALTER TABLE tst_answers CHANGE Respuesta Answer TEXT NOT NULL;

----


ALTER TABLE tst_answers ADD COLUMN Correct ENUM('N','Y') NOT NULL AFTER Correcta;
UPDATE tst_answers SET Correct='Y' WHERE Correcta='correcta';
UPDATE tst_answers SET Correct='N' WHERE Correcta='incorrecta';
ALTER TABLE tst_answers DROP COLUMN Correcta;

----

ALTER TABLE tst_questions CHANGE FechaHora EditTime DATETIME NOT NULL;

----

ALTER TABLE tst_questions CHANGE Enunciado Stem TEXT NOT NULL;
ALTER TABLE tst_questions CHANGE NumAccesos NumHits INT NOT NULL;

ALTER TABLE tst_questions CHANGE Puntuacion Score DOUBLE PRECISION NOT NULL;

----

ALTER TABLE msg_notices ADD COLUMN Hidden ENUM('N','Y') NOT NULL AFTER Content;

DROP INDEX CrsCod ON msg_notices;
CREATE INDEX CrsCod_Hidden ON msg_notices (CrsCod,Hidden);

-----

DROP INDEX CrsCod_Hidden ON msg_notices;
ALTER TABLE msg_notices CHANGE COLUMN Hidden Obsolete ENUM('N','Y') NOT NULL DEFAULT 'N';
CREATE INDEX CrsCod_Obsolete ON msg_notices (CrsCod,Obsolete);

-----

ALTER TABLE usr_data ADD COLUMN EncryptedUsrCod CHAR(43) NOT NULL AFTER UsrCod;
CREATE UNIQUE INDEX EncryptedUsrCod ON usr_data (EncryptedUsrCod);

------

ALTER TABLE tst_config CHANGE COLUMN Feedback Feedback ENUM('nothing','total_result','each_result','each_good_bad') NOT NULL;

-------

ALTER TABLE crs_card_fields CHANGE CodCampo FieldCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE crs_cards CHANGE CodCampo FieldCod INT NOT NULL;
ALTER TABLE crs_card_fields CHANGE NombreCampo FieldName VARCHAR(255) NOT NULL;
ALTER TABLE crs_card_fields CHANGE NumLineas NumLines INT NOT NULL;
ALTER TABLE crs_card_fields CHANGE Visibilidad Visibility TINYINT NOT NULL;
ALTER TABLE crs_cards CHANGE Texto Txt TEXT NOT NULL;

-------

ALTER TABLE crs_grp_types CHANGE Obligatory Mandatory ENUM('N','Y') NOT NULL;

-----

ALTER TABLE usr_data ADD COLUMN NotifyEventMessage ENUM('N','Y') NOT NULL DEFAULT 'Y' AFTER SecureIcons;
ALTER TABLE usr_data CHANGE COLUMN NotifyEventMessage NotifyEventMessage ENUM('N','Y') NOT NULL DEFAULT 'N';
UPDATE usr_data SET NotifyEventMessage='N';

-----

ALTER TABLE msg_received ADD COLUMN Notified ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Deleted;
ALTER TABLE msg_received ADD INDEX (Notified);

------

ALTER TABLE usr_data ADD COLUMN NotifyEvents INT NOT NULL DEFAULT 0 AFTER NotifyEventMessage;
UPDATE usr_data SET NotifyEvents=8 WHERE NotifyEventMessage='Y';

ALTER TABLE usr_data DROP COLUMN NotifyEventMessage;

-----

ALTER TABLE msg_notices ADD COLUMN Deleted ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Content;

-----

ALTER TABLE msg_notices ADD COLUMN Status ENUM('active','obsolete','deleted') NOT NULL DEFAULT 'active' AFTER Obsolete;
UPDATE msg_notices SET Status='active' WHERE Deleted='N' AND Obsolete='N';
UPDATE msg_notices SET Status='obsolete' WHERE Deleted='N' AND Obsolete='Y';
UPDATE msg_notices SET Status='deleted' WHERE Deleted='Y';

-----

ALTER TABLE msg_notices DROP COLUMN Status;
ALTER TABLE msg_notices ADD COLUMN Status TINYINT NOT NULL DEFAULT 0 AFTER Obsolete;
UPDATE msg_notices SET Status=0 WHERE Deleted='N' AND Obsolete='N';
UPDATE msg_notices SET Status=1 WHERE Deleted='N' AND Obsolete='Y';
UPDATE msg_notices SET Status=2 WHERE Deleted='Y';

-----

ALTER TABLE msg_notices DROP COLUMN Deleted;
ALTER TABLE msg_notices DROP COLUMN Obsolete;
---

SHOW INDEX FROM msg_notices;
DROP INDEX CrsCod_Obsolete ON msg_notices;
SHOW INDEX FROM msg_notices;
CREATE INDEX CrsCod_Status ON msg_notices (CrsCod,Status);
CREATE INDEX Status ON msg_notices (Status);
SHOW INDEX FROM msg_notices;

-----

ALTER TABLE msg_notices ADD COLUMN NumNotif INT NOT NULL DEFAULT 0 AFTER Status;

SELECT COUNT(*),SUM(NumNotif) FROM msg_notices WHERE CrsCod='422' AND Status='0';

-----

SHOW INDEX FROM calls_for_exams;
ALTER TABLE calls_for_exams ADD COLUMN Status TINYINT NOT NULL DEFAULT 0 AFTER CrsCod;
DROP INDEX CrsCod ON calls_for_exams;
CREATE INDEX CrsCod_Status ON calls_for_exams (CrsCod,Status);
CREATE INDEX Status ON calls_for_exams (Status);

----

ALTER TABLE calls_for_exams ADD COLUMN NumNotif INT NOT NULL DEFAULT 0 AFTER Status;

------

ALTER TABLE forum_post ADD COLUMN NumNotif INT NOT NULL DEFAULT 0 AFTER ModifTime;

------

RENAME TABLE grades TO marks;
ALTER TABLE marks CHANGE COLUMN Cabecera Header INT NOT NULL;
ALTER TABLE marks CHANGE COLUMN Pie Foot INT NOT NULL;
SHOW INDEX FROM marks;

-----

ALTER TABLE marks ADD COLUMN NumNotif INT NOT NULL DEFAULT 0 AFTER GrpCod;
ALTER TABLE marks DROP COLUMN NumNotif;

-----

DROP TABLE notif;
CREATE TABLE notif (DegCod INT NOT NULL,CrsCod INT NOT NULL,NotifyEvent TINYINT NOT NULL,NumNotif INT NOT NULL,UNIQUE INDEX (DegCod,CrsCod,NotifyEvent));

----

SELECT SUM(NumNotif) FROM calls_for_exams;
SELECT SUM(NumNotif) FROM msg_notices;
SELECT COUNT(*) FROM msg_received WHERE Notified='Y';
SELECT SUM(NumNotif) FROM forum_post;

UPDATE calls_for_exams SET NumNotif='0';
UPDATE msg_notices SET NumNotif='0';
UPDATE msg_received SET Notified='N';
UPDATE forum_post SET NumNotif='0';
DELETE FROM notif;

-----

SELECT SUM(NumNotif) FROM notif WHERE notif.CrsCod='422' AND NotifyEvent='0';


-----

DESCRIBE IP_prefs;
DESCRIBE calls_for_exams;
DESCRIBE centres;
DESCRIBE chat;
DESCRIBE clicks_without_photo;
DESCRIBE clipboard;
DESCRIBE connected;
DESCRIBE courses;
DESCRIBE crs_card_fields;
DESCRIBE crs_cards;
DESCRIBE crs_common_files;
DESCRIBE crs_grp;
DESCRIBE crs_grp_types;
DESCRIBE crs_grp_usr;
DESCRIBE crs_info_src;
DESCRIBE crs_info_txt;
DESCRIBE crs_usr;
DESCRIBE deg_admin;
DESCRIBE deg_types;
DESCRIBE degrees;
DESCRIBE departments;
DESCRIBE exp_tree_size;
DESCRIBE expanded_folders;
DESCRIBE forum_disabled_post;
DESCRIBE forum_post;
DESCRIBE forum_thr_clip;
DESCRIBE forum_thr_read;
DESCRIBE forum_thread;
DESCRIBE hidden_downloads;
DESCRIBE hidden_params;
DESCRIBE holidays;
DESCRIBE institutions;
DESCRIBE links;
DESCRIBE log;
DESCRIBE log_recent;
DESCRIBE marks;
DESCRIBE messages;
DESCRIBE msg_new;
DESCRIBE msg_notices;
DESCRIBE msg_received;
DESCRIBE notif;
DESCRIBE pending_passwd;
DESCRIBE places;
DESCRIBE sessions;
DESCRIBE sta_degrees;
DESCRIBE timetable_crs;
DESCRIBE timetable_tut;
DESCRIBE tst_answers;
DESCRIBE tst_config;
DESCRIBE tst_question_tags;
DESCRIBE tst_questions;
DESCRIBE tst_status;
DESCRIBE tst_tags;
DESCRIBE usr_data;

-----

ALTER TABLE clipboard CHANGE COLUMN Tipo FileBrowser TINYINT NOT NULL;
ALTER TABLE clipboard CHANGE COLUMN Fecha CopyTime TIMESTAMP;

ALTER TABLE crs_grp CHANGE COLUMN NombreGrupo GrpName VARCHAR(255) NOT NULL;

ALTER TABLE crs_grp_usr CHANGE COLUMN UltAccDescargaGrp LastAccDownloadGrp DATETIME NOT NULL;
ALTER TABLE crs_grp_usr CHANGE COLUMN UltAccComunGrp LastAccCommonGrp DATETIME NOT NULL;
ALTER TABLE crs_grp_usr CHANGE COLUMN UltAccCalifGrp LastAccGradesGrp DATETIME NOT NULL;

ALTER TABLE crs_usr CHANGE COLUMN LastAccDownload LastAccDownloadCrs DATETIME NOT NULL;
ALTER TABLE crs_usr CHANGE COLUMN LastAccGrades LastAccGradesCrs DATETIME NOT NULL;

ALTER TABLE hidden_params CHANGE COLUMN IdSesion IdSession CHAR(43) NOT NULL;
ALTER TABLE hidden_params CHANGE COLUMN Accion Action INT NOT NULL;
ALTER TABLE hidden_params CHANGE COLUMN NombreParam ParamName VARCHAR(255) NOT NULL;
ALTER TABLE hidden_params CHANGE COLUMN ParamValue ParamValue TEXT NOT NULL;

----

ALTER TABLE crs_usr CHANGE COLUMN LastAccGradesCrs LastAccMarksCrs DATETIME NOT NULL;
ALTER TABLE crs_grp_usr CHANGE COLUMN LastAccGradesGrp LastAccMarksGrp DATETIME NOT NULL;

----

ALTER TABLE timetable_crs CHANGE COLUMN Dia Day ENUM('L','M','X','J','V') NOT NULL;
ALTER TABLE timetable_tut CHANGE COLUMN Dia Day ENUM('L','M','X','J','V') NOT NULL;

----

ALTER TABLE timetable_crs CHANGE COLUMN Hora Hour TINYINT NOT NULL;
ALTER TABLE timetable_tut CHANGE COLUMN Hora Hour TINYINT NOT NULL;

-----

ALTER TABLE calls_for_exams CHANGE COLUMN ExamLength Duration TIME NOT NULL;

-----

ALTER TABLE timetable_crs CHANGE COLUMN Duracion Duration TINYINT NOT NULL;
ALTER TABLE timetable_tut CHANGE COLUMN Duracion Duration TINYINT NOT NULL;

----

ALTER TABLE timetable_crs CHANGE COLUMN Lugar Place VARCHAR(255) NOT NULL;
ALTER TABLE timetable_tut CHANGE COLUMN Lugar Place VARCHAR(255) NOT NULL;

----

ALTER TABLE timetable_crs CHANGE COLUMN Tipo ClassType ENUM('libre','teoria','practicas') NOT NULL;
ALTER TABLE timetable_crs CHANGE COLUMN Grupo GroupName VARCHAR(255) NOT NULL;

----

ALTER TABLE calls_for_exams CHANGE COLUMN Indications OtherInformation TEXT NOT NULL;
ALTER TABLE calls_for_exams CHANGE COLUMN OtherInformation OtherInfo TEXT NOT NULL;

-----

ALTER TABLE calls_for_exams CHANGE COLUMN CalCod ExaCod INT NOT NULL AUTO_INCREMENT;

----

RENAME TABLE calls_for_exams TO exam_announcements;
----

ALTER TABLE exam_announcements CHANGE COLUMN CallType ExamSession VARCHAR(255) NOT NULL;
ALTER TABLE exam_announcements CHANGE COLUMN Modality ExamMode TEXT NOT NULL;

----

ALTER TABLE notif CHANGE COLUMN NumNotif NumEvents INT NOT NULL;
ALTER TABLE notif ADD COLUMN NumMails INT NOT NULL AFTER NumEvents;

-----

UPDATE notif SET NumMails=NumEvents;

----

RENAME TABLE notif TO sta_notif;

-----

CREATE TABLE notif (NotifyEvent TINYINT NOT NULL,ToUsrCod INT NOT NULL,FromUsrCod INT NOT NULL,InsCod INT NOT NULL DEFAULT -1,DegCod INT NOT NULL DEFAULT -1,CrsCod INT NOT NULL DEFAULT -1,MsgCod INT NOT NULL DEFAULT -1,ForumType TINYINT NOT NULL,TimeNotif DATETIME NOT NULL,INDEX (ToUsrCod),INDEX(TimeNotif));

----

SELECT NotifyEvent,ToUsrCod,FromUsrCod,InsCod,DegCod,CrsCod,MsgCod,ForumType,TimeNotif FROM notif WHERE ToUsrCod IN (SELECT DISTINCT ToUsrCod FROM notif WHERE UNIX_TIMESTAMP(TimeNotif) < UNIX_TIMESTAMP()-60*60) ORDER BY ToUsrCod,TimeNotif,NotifyEvent;

-----

DROP TABLE ws_keys;
CREATE TABLE ws_keys (WSKey CHAR(43) NOT NULL,UsrCod INT NOT NULL,LastTime DATETIME NOT NULL,UNIQUE INDEX(WSKey),INDEX(UsrCod),INDEX(LastTime));

SELECT COUNT(*) FROM crs_grp_types,crs_grp WHERE crs_grp_types.CrsCod='422' AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod AND crs_grp.GrpCod='7343';

------

CREATE TABLE plugins (PlgCod INT NOT NULL AUTO_INCREMENT,Name VARCHAR(255) NOT NULL,Description TEXT NOT NULL,Logo VARCHAR(16) NOT NULL,URL VARCHAR(255) NOT NULL,UNIQUE INDEX(PlgCod))

ALTER TABLE plugins ADD COLUMN IP CHAR(40) NOT NULL AFTER URL;

-----

SELECT DISTINCT crs_usr.UsrCod,'N' FROM courses,crs_usr,usr_data WHERE courses.DegCod='57' AND courses.CrsCod=crs_usr.CrsCod AND usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Accepted='N' UNION SELECT DISTINCT crs_usr.UsrCod,'Y' FROM courses,crs_usr,usr_data WHERE courses.DegCod='57' AND courses.CrsCod=crs_usr.CrsCod AND usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.UsrCod NOT IN (SELECT DISTINCT crs_usr.UsrCod FROM courses,crs_usr,usr_data WHERE courses.DegCod='57' AND courses.CrsCod=crs_usr.CrsCod AND usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Accepted='N')

SELECT DISTINCT crs_usr.UsrCod,'N' FROM courses,crs_usr,usr_data WHERE courses.DegCod='57' AND courses.CrsCod=crs_usr.CrsCod AND usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Accepted='N';
SELECT DISTINCT crs_usr.UsrCod,'Y' FROM courses,crs_usr,usr_data WHERE courses.DegCod='57' AND courses.CrsCod=crs_usr.CrsCod AND usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.UsrCod NOT IN (SELECT DISTINCT crs_usr.UsrCod FROM courses,crs_usr,usr_data WHERE courses.DegCod='57' AND courses.CrsCod=crs_usr.CrsCod AND usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Accepted='N')

SELECT DISTINCT crs_usr.UsrCod     FROM courses,crs_usr,usr_data WHERE courses.DegCod='57' AND courses.CrsCod=crs_usr.CrsCod AND usr_data.UsrType='2' AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Accepted='N'

------

DROP TABLE prado_sessions;
CREATE TABLE prado_sessions (SWADIdSession CHAR(43) NOT NULL,SWADUsrCod INT NOT NULL,PRADOUsrId CHAR(255) NOT NULL,PRADOIdSession CHAR(255) NOT NULL,PRADOUsrType TINYINT NOT NULL,UNIQUE INDEX(SWADIdSession));

------

CREATE TABLE prado_groups (IdSession CHAR(43) NOT NULL,GrpCod INT NOT NULL AUTO_INCREMENT,DegName VARCHAR(255) NOT NULL,CrsName VARCHAR(255) NOT NULL,GrpName VARCHAR(255) NOT NULL,GrpType VARCHAR(255) NOT NULL,PRIMARY KEY (IdSession),UNIQUE INDEX(GrpCod));
CREATE TABLE prado_students (IdSession CHAR(43) NOT NULL,GrpCod INT NOT NULL AUTO_INCREMENT,DNI CHAR(16) NOT NULL,Surname1 VARCHAR(255) NOT NULL,Surname2 VARCHAR(255) NOT NULL,FirstName VARCHAR(255) NOT NULL,Gender ENUM ('unknown','female','male') NOT NULL DEFAULT 'unknown',E_mail VARCHAR(255) NOT NULL,PRIMARY KEY (IdSession),UNIQUE INDEX(GrpCod));

DROP TABLE prado_groups;
CREATE TABLE prado_groups (GrpCod INT NOT NULL AUTO_INCREMENT,IdSession CHAR(43) NOT NULL,DegName VARCHAR(255) NOT NULL,CrsName VARCHAR(255) NOT NULL,GrpName VARCHAR(255) NOT NULL,GrpType VARCHAR(255) NOT NULL,UNIQUE INDEX (GrpCod),INDEX (IdSession));
DROP TABLE prado_students;
CREATE TABLE prado_students (GrpCod INT NOT NULL AUTO_INCREMENT,DNI CHAR(16) NOT NULL,Surname1 VARCHAR(255) NOT NULL,Surname2 VARCHAR(255) NOT NULL,FirstName VARCHAR(255) NOT NULL,Gender ENUM ('unknown','female','male') NOT NULL DEFAULT 'unknown',E_mail VARCHAR(255) NOT NULL,INDEX(GrpCod));

-------

CREATE TABLE assignments (AsgCod INT NOT NULL AUTO_INCREMENT,CrsCod INT NOT NULL DEFAULT -1,SendWork TINYINT NOT NULL,StartDate DATE NOT NULL,EndDate DATE NOT NULL,Name VARCHAR(255) NOT NULL,UNIQUE INDEX (AsgCod),INDEX(CrsCod));

----

DROP TABLE assignments;
CREATE TABLE assignments (AsgCod INT NOT NULL AUTO_INCREMENT,CrsCod INT NOT NULL DEFAULT -1,SendWork ENUM('N','Y') NOT NULL DEFAULT 'N',StartDate DATE NOT NULL,EndDate DATE NOT NULL,Name VARCHAR(255) NOT NULL,UNIQUE INDEX (AsgCod),INDEX(CrsCod));

ALTER TABLE assignments ADD COLUMN Txt TEXT NOT NULL AFTER Name;

------

ALTER TABLE assignments ADD COLUMN UsrCod INT NOT NULL AFTER CrsCod;

-----

ALTER TABLE assignments ADD COLUMN StartTime DATETIME NOT NULL AFTER StartDate;
ALTER TABLE assignments ADD COLUMN EndTime DATETIME NOT NULL AFTER EndDate;

-----

ALTER TABLE assignments DROP COLUMN StartDate;
ALTER TABLE assignments DROP COLUMN EndDate;

----

ALTER TABLE assignments ADD COLUMN Folder VARCHAR(32) NOT NULL AFTER Txt;
ALTER TABLE assignments ADD COLUMN SendWk ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Txt;
UPDATE assignments SET SendWk=SendWork;
ALTER TABLE assignments DROP COLUMN SendWork;
ALTER TABLE assignments CHANGE COLUMN SendWk SendWork ENUM('N','Y') NOT NULL DEFAULT 'N';

ALTER TABLE assignments ADD COLUMN Texto TEXT NOT NULL AFTER Folder;
UPDATE assignments SET Texto=Txt;
ALTER TABLE assignments DROP COLUMN Txt;
ALTER TABLE assignments CHANGE COLUMN Texto Txt TEXT NOT NULL;

----

RENAME TABLE activities TO assignments;
ALTER TABLE assignments CHANGE COLUMN AtvCod AsgCod INT NOT NULL AUTO_INCREMENT;

-----

ALTER TABLE assignments CHANGE COLUMN Name Title VARCHAR(255) NOT NULL;

-----

CREATE TABLE asg_grp (AsgCod INT NOT NULL,GrpCod INT NOT NULL,INDEX(AsgCod),INDEX(GrpCod));

-----

DROP TABLE asg_grp;
CREATE TABLE asg_grp (AsgCod INT NOT NULL,GrpCod INT NOT NULL,UNIQUE INDEX(AsgCod,GrpCod));

----

SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr WHERE crs_grp_usr.UsrCod='1346' AND asg_grp.GrpCod=crs_grp_usr.GrpCod;

SELECT Folder FROM assignments WHERE CrsCod='421' AND SendWork='Y' AND Folder<>'' AND StartTime<=NOW() AND EndTime>=NOW() AND (AsgCod NOT IN (SELECT AsgCod FROM asg_grp) OR AsgCod IN (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr WHERE crs_grp_usr.UsrCod='1346' AND asg_grp.GrpCod=crs_grp_usr.GrpCod));

-------

SELECT COUNT(*) FROM assignments WHERE AsgCod='24' AND (AsgCod NOT IN (SELECT AsgCod FROM asg_grp) OR AsgCod IN (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr WHERE crs_grp_usr.UsrCod='1346' AND asg_grp.GrpCod=crs_grp_usr.GrpCod));

-----

ALTER TABLE assignments CHANGE COLUMN Folder Folder VARBINARY(32) NOT NULL;

ALTER TABLE assignments DROP COLUMN SendWork;

-----
SELECT COUNT(*) FROM messages,usr_data,crs_usr WHERE messages.CrsCod='421' AND messages.UsrCod=usr_data.UsrCod AND usr_data.UsrType='421' AND crs_usr.CrsCod='%ld' AND messages.UsrCod=crs_usr.UsrCod;

-----

SELECT COUNT(DISTINCT CrsCod),COUNT(DISTINCT UsrCod),MAX(NumLevels),SUM(NumFolders),SUM(NumFiles),SUM(TotalSize) FROM exp_tree_size WHERE TreeType='6' AND CrsCod='421';

-----

RENAME TABLE exp_tree_size TO file_browser_size;

-----

ALTER TABLE expanded_folders CHANGE COLUMN TreeType FileBrowser TINYINT NOT NULL;
ALTER TABLE file_browser_size CHANGE COLUMN TreeType FileBrowser TINYINT NOT NULL;

------

RENAME TABLE msg_notices TO notices;
ALTER TABLE notices CHANGE COLUMN MsgCod NotCod INT NOT NULL AUTO_INCREMENT;

------

ALTER TABLE forum_thread CHANGE COLUMN FirstMsgCod FirstPstCod INT NOT NULL;
ALTER TABLE forum_thread CHANGE COLUMN LastMsgCod LastPstCod INT NOT NULL;


ALTER TABLE forum_post CHANGE COLUMN MsgCod PstCod INT NOT NULL;
ALTER TABLE forum_disabled_post CHANGE COLUMN MsgCod PstCod INT NOT NULL;

RENAME TABLE forum_msg TO forum_post;
RENAME TABLE forum_disabled_msg TO forum_disabled_post;

-----

ALTER TABLE forum_post CHANGE COLUMN PstCod PstCod INT NOT NULL AUTO_INCREMENT;

-----

RENAME TABLE prado_groups TO imported_groups;
RENAME TABLE prado_sessions TO imported_sessions;
RENAME TABLE prado_students TO imported_students;

ALTER TABLE imported_sessions CHANGE COLUMN PRADOUsrId ImportedUsrId CHAR(255) NOT NULL;

-----

ALTER TABLE sessions CHANGE COLUMN IdSession SessionID CHAR(43) NOT NULL;
ALTER TABLE hidden_params CHANGE COLUMN IdSession SessionID CHAR(43) NOT NULL;
ALTER TABLE imported_groups CHANGE COLUMN IdSession SessionID CHAR(43) NOT NULL;
ALTER TABLE tst_status CHANGE COLUMN IdSession SessionID CHAR(43) NOT NULL;

------

ALTER TABLE imported_sessions CHANGE COLUMN SWADIdSession SessionID CHAR(43) NOT NULL;

-------

ALTER TABLE sessions CHANGE COLUMN SessionID SessionId CHAR(43) NOT NULL;
ALTER TABLE hidden_params CHANGE COLUMN SessionID SessionId CHAR(43) NOT NULL;
ALTER TABLE imported_groups CHANGE COLUMN SessionID SessionId CHAR(43) NOT NULL;
ALTER TABLE tst_status CHANGE COLUMN SessionID SessionId CHAR(43) NOT NULL;
ALTER TABLE imported_sessions CHANGE COLUMN SessionID SessionId CHAR(43) NOT NULL;

------

ALTER TABLE imported_sessions CHANGE COLUMN SWADUsrCod UsrCod INT NOT NULL;

-----

ALTER TABLE imported_sessions CHANGE COLUMN PRADOIdSession ImportedSessionId CHAR(255) NOT NULL;

-----

ALTER TABLE imported_sessions CHANGE COLUMN PRADOUsrType ImportedUsrType TINYINT NOT NULL;

-----

UPDATE log SET Accion='Autenticar usuario desde fuera' WHERE Accion='Autenticar usuario desde PRADO';
UPDATE log_recent SET Action='Autenticar usuario desde fuera' WHERE Action='Autenticar usuario desde PRADO';

-----

CREATE TABLE IF NOT EXISTS chat (RoomCode VARCHAR(255) NOT NULL,NumUsrs INT NOT NULL,UNIQUE INDEX(RoomCode));

------

�C�mo cambiar temario.xml por syllabus.xml, y temario.old por syllabus.old?
Desde /var/www/swad/crs:
find . -name "temario.xml" -exec sh -c 'mv "$1" "`dirname $1`/syllabus.xml"' {} {} \; -print
find . -name "temario.old" -exec sh -c 'mv "$1" "`dirname $1`/syllabus.old"' {} {} \; -print

�C�mo cambiar teoria por lect y practicas por prac?
Desde /var/www/swad/crs:
find . -maxdepth 2 -name "teoria"    -exec sh -c 'mv "$1" "`dirname $1`/lect"' {} {} \; -print
find . -maxdepth 2 -name "practicas" -exec sh -c 'mv "$1" "`dirname $1`/prac"' {} {} \; -print

find . -maxdepth 2 -name "lect" -exec sh -c 'mv "$1" "`dirname $1`/lec"' {} {} \; -print
find . -maxdepth 2 -name "prac" -exec sh -c 'mv "$1" "`dirname $1`/pra"' {} {} \; -print

find . -maxdepth 2 -name "conf.txt" -print
find . -maxdepth 2 -name "conf.old" -exec rm {} \;
find . -maxdepth 2 -name "conf.txt" -exec rm {} \;

find . -maxdepth 2 -name "descripcion.old" -exec rm {} \; -print
find . -maxdepth 2 -name "descripcion.xml" -exec rm {} \; -print

------------------

BACKUP TABLE log TO '/var/lib/mysql/swad-copia-log';

ALTER TABLE log CHANGE COLUMN FechaHora ClickTime DATETIME NOT NULL, CHANGE COLUMN TiempoGener TimeToGenerate INT NOT NULL, CHANGE COLUMN TiempoEnvio TimeToSend INT NOT NULL, CHANGE COLUMN Accion Action VARCHAR(32) NOT NULL, CHANGE COLUMN Addr IP CHAR(40) NOT NULL, CHANGE COLUMN Observaciones Comments VARCHAR(255);

-----

RENAME TABLE crs_cards TO crs_records;
RENAME TABLE crs_card_fields TO crs_record_fields;
-----

ALTER TABLE marks CHANGE COLUMN Foot Footer INT NOT NULL;

-----

ALTER TABLE usr_data ADD COLUMN Layout TINYINT NOT NULL DEFAULT 0 AFTER Gender;
ALTER TABLE usr_data ADD INDEX (Layout);

ALTER TABLE IP_prefs ADD COLUMN Layout TINYINT NOT NULL DEFAULT 0 AFTER LastChange;

------

ALTER TABLE crs_info_src CHANGE COLUMN InfoType InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL;
ALTER TABLE crs_info_txt CHANGE COLUMN InfoType InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL;

------

RENAME TABLE messages TO msg_snt;
RENAME TABLE msg_received TO msg_rcv;

-------

CREATE TABLE IF NOT EXISTS msg_content (MsgCod INT NOT NULL,Subject TEXT NOT NULL,Content LONGTEXT NOT NULL,UNIQUE INDEX(MsgCod),FULLTEXT(Subject,Content));

DELETE FROM msg_content;
INSERT INTO msg_content (MsgCod,Subject,Content) SELECT MsgCod,Subject,Content FROM msg_snt;

SELECT msg_snt.MsgCod FROM msg_snt,msg_content WHERE msg_snt.UsrCod='1346' AND msg_snt.Deleted='N' AND msg_snt.MsgCod=msg_content.MsgCod AND MATCH (msg_content.Subject,msg_content.Content) AGAINST ('kkk') ORDER BY msg_snt.MsgCod DESC

ALTER TABLE msg_content CHANGE COLUMN MsgCod MsgCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE msg_snt CHANGE COLUMN MsgCod MsgCod INT NOT NULL;

BACKUP TABLE msg_snt TO '/var/lib/mysql/swad-copia-msg-snt';

ALTER TABLE msg_snt DROP COLUMN Subject;
ALTER TABLE msg_snt DROP COLUMN Content;

SELECT COUNT(*) FROM msg_content WHERE MsgCod IN (SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt WHERE msg_rcv.UsrCod='1346' AND msg_rcv.Deleted='N' AND msg_rcv.Open='N' AND msg_rcv.MsgCod=msg_snt.MsgCod AND msg_snt.CrsCod='421') AND MATCH (Subject,Content) AGAINST ('duda');

----

CREATE TABLE IF NOT EXISTS msg_snt_deleted (MsgCod INT NOT NULL,CrsCod INT NOT NULL DEFAULT -1,UsrCod INT NOT NULL,CreatTime DATETIME NOT NULL,UNIQUE INDEX(MsgCod),INDEX(CrsCod),INDEX(UsrCod),INDEX(CreatTime));
INSERT INTO msg_snt_deleted (MsgCod,CrsCod,UsrCod,CreatTime) SELECT MsgCod,CrsCod,UsrCod,CreatTime FROM msg_snt WHERE MsgCod='762471';

DELETE FROM msg_snt_deleted;
INSERT INTO msg_snt_deleted (MsgCod,CrsCod,UsrCod,CreatTime) SELECT MsgCod,CrsCod,UsrCod,CreatTime FROM msg_snt WHERE Deleted='Y';

ALTER TABLE msg_snt DROP COLUMN Deleted;

------

CREATE TABLE IF NOT EXISTS msg_rcv_deleted (MsgCod INT NOT NULL,UsrCod INT NOT NULL,Notified ENUM('N','Y') NOT NULL DEFAULT 'N',Open ENUM('N','Y') NOT NULL DEFAULT 'N',Replied ENUM('N','Y') NOT NULL DEFAULT 'N',UNIQUE INDEX (UsrCod,MsgCod),INDEX(MsgCod),INDEX(UsrCod),INDEX(Notified));

INSERT INTO msg_rcv_deleted (MsgCod,UsrCod,Notified,Open,Replied) SELECT MsgCod,UsrCod,Notified,Open,Replied FROM msg_rcv WHERE Deleted='Y';

BACKUP TABLE msg_rcv TO '/var/lib/mysql/swad-copia-msg-rcv';

DELETE FROM msg_rcv WHERE Deleted='Y';
ALTER TABLE msg_rcv DROP COLUMN Deleted;

(SELECT msg_rcv.UsrCod,'N',msg_rcv.Open FROM msg_rcv,usr_data WHERE msg_rcv.MsgCod='424504' AND msg_rcv.UsrCod=usr_data.UsrCod) UNION (SELECT msg_rcv_deleted.UsrCod,'Y',msg_rcv_deleted.Open FROM msg_rcv_deleted,usr_data WHERE msg_rcv_deleted.MsgCod='424504' AND msg_rcv_deleted.UsrCod=usr_data.UsrCod) ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName;

-----

ALTER TABLE sessions CHANGE COLUMN LastPageMsgRec LastPageMsgRcv INT NOT NULL DEFAULT 1;

-----

SELECT MsgCod FROM msg_snt_deleted WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_rcv);

CREATE TABLE IF NOT EXISTS msg_content_deleted (MsgCod INT NOT NULL,Subject TEXT NOT NULL,Content LONGTEXT NOT NULL,UNIQUE INDEX(MsgCod),FULLTEXT(Subject,Content));

DELETE FROM msg_content_deleted;
INSERT INTO msg_content_deleted (MsgCod,Subject,Content) SELECT MsgCod,Subject,Content FROM msg_content WHERE MsgCod IN (SELECT MsgCod FROM msg_snt_deleted) AND MsgCod NOT IN (SELECT MsgCod FROM msg_rcv);

-----

SELECT COUNT(*) FROM msg_content WHERE MsgCod IN (SELECT MsgCod FROM msg_snt) OR MsgCod IN (SELECT MsgCod FROM msg_rcv);

SELECT COUNT(*) FROM msg_content WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_content_deleted);

BACKUP TABLE msg_content TO '/var/lib/mysql/swad-copia-msg-content';

DELETE FROM msg_content WHERE MsgCod IN (SELECT MsgCod FROM msg_content_deleted);

------

CREATE TABLE IF NOT EXISTS actions_MFU (UsrCod INT NOT NULL,Action INT NOT NULL,Score INT NOT NULL,LastClick DATETIME NOT NULL,UNIQUE INDEX(UsrCod,Action));

DELETE FROM actions_MFU;

REPLACE INTO actions_MFU (UsrCod,Action,Score,LastClick) VALUES('1346','15',Score+1,NOW());

UPDATE actions_MFU SET Score=MAX(Score-1;'0'),ClickTime=NOW() WHERE UsrCod='%ld' AND Action<>'%ld';

UPDATE actions_MFU SET Score=Score-1 WHERE UsrCod='1346' AND Action<>'15' HAVING LastClick<AVG(LastClick);

UPDATE actions_MFU SET Score=0 WHERE Score<0;

------

SELECT COUNT(*) FROM actions_MFU WHERE UsrCod='1346';

SELECT UsrCod,Action FROM actions_MFU WHERE UsrCod='1346' ORDER BY Score DESC LIMIT (SELECT COUNT(*) FROM actions_MFU WHERE UsrCod='1346');


SET @NumRows=(SELECT COUNT(*) FROM actions_MFU WHERE UsrCod='1346' AND Score<>'0')/2;
PREPARE statement FROM 'UPDATE actions_MFU SET Score=Score-1 WHERE UsrCod=1346 AND Action IN (SELECT * FROM (SELECT Action FROM actions_MFU WHERE UsrCod=1346 AND Score<>0 ORDER BY LastClick LIMIT ?) AS T)';
EXECUTE statement USING @NumRows;


UPDATE actions_MFU SET Score=Score-1                  " WHERE UsrCod='%ld' AND Action<>'%ld' AND Score>'0'                  " AND LastClick<(SELECT * FROM (SELECT AVG(LastClick) FROM actions_MFU WHERE UsrCod='%ld') AS T)

DROP TABLE actions_MFU;
CREATE TABLE IF NOT EXISTS actions_MFU (UsrCod INT NOT NULL,Action INT NOT NULL,Score FLOAT NOT NULL,LastClick DATETIME NOT NULL,UNIQUE INDEX(UsrCod,Action));

------

CREATE TABLE IF NOT EXISTS notices_deleted (NotCod INT NOT NULL,CrsCod INT NOT NULL DEFAULT -1,UsrCod INT NOT NULL,CreatTime DATETIME NOT NULL,Content LONGTEXT NOT NULL,NumNotif INT NOT NULL DEFAULT 0,UNIQUE INDEX(NotCod), INDEX(CrsCod), INDEX(UsrCod), INDEX(CreatTime));

DELETE FROM notices_deleted;
INSERT INTO notices_deleted (NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif) SELECT NotCod,CrsCod,UsrCod,CreatTime,Content,NumNotif FROM notices WHERE Status='2';

SELECT COUNT(*) FROM notices WHERE Status='2';

BACKUP TABLE notices TO '/var/lib/mysql/swad-copia-notices';
DELETE FROM notices WHERE Status='2';

------

CREATE TABLE IF NOT EXISTS mail_domains (MaiCod INT NOT NULL AUTO_INCREMENT,Domain VARCHAR(255) NOT NULL,Info VARCHAR(255) NOT NULL,UNIQUE INDEX(MaiCod),UNIQUE INDEX(Domain));

-------

SELECT SUBSTRING_INDEX(E_mail, '@', -1) AS mail_domain,COUNT(*) AS N FROM usr_data GROUP BY mail_domain ORDER BY N DESC limit 25;

SELECT mail_domains.MaiCod,mail_domains.Domain,mail_domains.Info,COUNT(usr_data.*) FROM mail_domains,usr_data WHERE mail_domains.Domain=SUBSTRING_INDEX(usr_data.E_mail, '@', -1) GROUP BY mail_domains.Domain ORDER BY mail_domains.Info,mail_domains.Domain;

-----


SELECT SUBSTRING_INDEX(E_mail, '@', -1) AS mail_domain,COUNT(*) AS N FROM usr_data GROUP BY mail_domain ORDER BY N DESC limit 25;

------

SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS mail_domain,COUNT(*) AS N FROM usr_data WHERE SUBSTRING_INDEX(E_mail,'@',-1) IN (SELECT Domain FROM mail_domains) GROUP BY mail_domain ORDER BY N DESC;

SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS mail_domain,COUNT(*) AS N FROM usr_data GROUP BY mail_domain ORDER BY N DESC,mail_domain;

SELECT mail_domains.Domain,COUNT(*) AS N FROM usr_data,mail_domains WHERE SUBSTRING_INDEX(usr_data.E_mail,'@',-1) = mail_domains.Domain GROUP BY mail_domains.Domain ORDER BY N DESC;

SELECT Domain,'0' FROM mail_domains WHERE Domain NOT IN (SELECT DISTINCT SUBSTRING_INDEX(E_mail,'@',-1) FROM usr_data);


DROP TABLE IF EXISTS T1,T2;
CREATE TEMPORARY TABLE T1 ENGINE=MEMORY SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS Domain,COUNT(*) as N FROM usr_data GROUP BY Domain;
CREATE TEMPORARY TABLE T2 ENGINE=MEMORY SELECT * FROM T1;
(SELECT mail_domains.MaiCod,mail_domains.Domain AS Domain,mail_domains.Info AS Info,T1.N FROM mail_domains,T1 WHERE mail_domains.Domain=T1.Domain) UNION (SELECT MaiCod,Domain,Info,0 FROM mail_domains WHERE Domain NOT IN (SELECT Domain FROM T2)) ORDER BY N DESC,Info,Domain;
DROP TABLE IF EXISTS T1,T2;

----

ALTER TABLE mail_domains ADD INDEX (Info);

-----

SELECT DISTINCT DNI FROM log WHERE ClickTime>='20081001' AND ClickTime<'20091001' AND UsrType='3';

-----

CREATE TABLE IF NOT EXISTS countries (CtyCod INT NOT NULL,Alpha2 CHAR(2) NOT NULL,Name_de VARCHAR(255) NOT NULL,Name_en VARCHAR(255) NOT NULL,Name_es VARCHAR(255) NOT NULL,Name_fr VARCHAR(255) NOT NULL,Name_it VARCHAR(255) NOT NULL,Name_pt VARCHAR(255) NOT NULL,UNIQUE INDEX(CtyCod), UNIQUE INDEX(Alpha2),INDEX(Name_de),INDEX(Name_en),INDEX(Name_es),INDEX(Name_fr),INDEX(Name_it),INDEX(Name_pt));

-----

ALTER TABLE institutions ADD COLUMN CtyCod INT NOT NULL AFTER InsCod;
ALTER TABLE institutions ADD INDEX (CtyCod);
UPDATE institutions SET CtyCod='724';

------

ALTER TABLE usr_data ADD COLUMN CtyCod INT NOT NULL DEFAULT -1 AFTER PublicPhoto;
ALTER TABLE usr_data ADD INDEX (CtyCod);

------

ALTER TABLE crs_info_src ADD COLUMN MustBeRead ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER InfoSrc;

-------

CREATE TABLE IF NOT EXISTS crs_info_read (UsrCod INT NOT NULL,CrsCod INT NOT NULL,InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL,UNIQUE INDEX(UsrCod,CrsCod,InfoType));


SELECT Action FROM actions_MFU WHERE UsrCod='20511' ORDER BY Score DESC,LastClick DESC;


-------------------

CREATE DATABASE IF NOT EXISTS swad_copia_2010_06_16;
USE swad_copia_2010_06_16;
SOURCE /home/acanas/swad/copia_base_datos/copia_base_datos_swad_2010_06_16.sql;
SELECT usr_data.DNI,usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,crs_record_fields.FieldName,crs_records.Txt FROM usr_data,crs_record_fields,crs_records WHERE crs_records.FieldCod IN (SELECT FieldCod FROM crs_record_fields WHERE CrsCod='2298') AND usr_data.UsrCod=crs_records.UsrCod AND crs_records.FieldCod=crs_record_fields.FieldCod ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName;

---------------

[root@swad swad]# ls -l log*
-rw-rw---- 1 mysql mysql       8892 dic 31  2009 log.frm
-rw-rw---- 1 mysql mysql 7322307800 jul  9 10:00 log.MYD	--->  67,39 bytes por entrada
-rw-rw---- 1 mysql mysql 4978651136 jul  9 10:00 log.MYI 	--->  45,82 bytes por entrada
-rw-rw---- 1 mysql mysql       8892 jun  3  2008 log_recent.frm
-rw-rw---- 1 mysql mysql  179038944 jul  9 10:00 log_recent.MYD ---> 155,85 bytes por entrada
-rw-rw---- 1 mysql mysql  110266368 jul  9 10:00 log_recent.MYI --->  95,98 bytes por entrada
-rw-rw-rw- 1 mysql mysql     176418 may 11  2008 log.txt

mysql> select count(*) from log_recent;
+----------+
| count(*) |
+----------+
|  1148798 |
+----------+
1 row in set (0.00 sec)

mysql> select count(*) from log;
+-----------+
| count(*)  |
+-----------+
| 108654914 |
+-----------+
1 row in set (0.00 sec)

-----

ALTER TABLE log_recent ADD COLUMN ActCod INT NOT NULL DEFAULT -1 FIRST, ADD INDEX (ActCod);
ALTER TABLE log ADD COLUMN ActCod INT NOT NULL DEFAULT -1 FIRST, ADD INDEX (ActCod);

------

CREATE TABLE IF NOT EXISTS actions (ActCod INT NOT NULL DEFAULT -1,Language CHAR(2) NOT NULL DEFAULT 'es',Txt VARCHAR(255) NOT NULL,UNIQUE INDEX (ActCod,Language));
DELETE FROM actions;
INSERT INTO actions (ActCod,Txt) VALUES (0,"Ver descargas"),(1,"Descargar archivo"),(2,"Ver men�"),(3,"Ver mensajes recibidos"),(4,"Ver registro accesos"),(5,"Ver descripci�n"),(6,"Autenticar usuario"),(7,"Ver foro"),(8,"Ver mensajes tema foro"),(9,"Ver enlaces"),(10,"Cerrar sesi�n"),(11,"Ver orla alumnos"),(12,"Administrar descargas"),(13,"Solicitar nueva descarga"),(14,"Enviar archivo descarga"),(15,"Ver evaluaci�n"),(16,"Ver calendario"),(17,"Ver calificaciones"),(18,"Solicitar consulta accesos"),(19,"Solicitar estad�sticas acceso"),(20,"Ver pr�cticas"),(21,"Ver tutor�as"),(22,"Ver fichas profesores"),(23,"Ver malet�n"),(24,"Eliminar descarga"),(25,"Ver horario"),(26,"Solicitar mensaje nuevo"),(27,"Enviar mensaje"),(28,"Ver teor�a"),(29,"Ver test"),(30,"Solicitar env�o foto"),(31,"Cambiar foto"),(32,"Ver bibliograf�a"),(33,"Crear carpeta descarga"),(34,"Solicitar cambio contrase�a"),(35,"Cambiar contrase�a"),(36,"Solicitar cambio apodo"),(37,"Cambiar apodo"),(38,"Solicitar cambio ficha"),(39,"Cambiar ficha"),(40,"Solicitar alta usuario"),(41,"Dar alta usuario"),(42,"Ver lista alumnos"),(43,"Renombrar carpeta descarga"),(44,"Editar teor�a"),(45,"Editar horario asignatura"),(46,"Editar descripci�n"),(47,"Ver HTML acceso directo"),(48,"Cambiar horario tutor�as"),(49,"Eliminar item"),(50,"Enviar respuesta foro"),(51,"Ver salas chat y pizarra"),(52,"Abrir chat y pizarra"),(53,"Cambiar horario asignatura"),(54,"Ver FAQ"),(55,"Eliminar mensaje foro"),(56,"Solicitar cambio otra contrase�a"),(57,"Solicitar baja usuario"),(58,"Dar baja usuario"),(59,"Solicitar aviso nuevo"),(60,"Crear aviso nuevo"),(61,"Solicitar baja total usuario"),(62,"Dar baja total usuario"),(63,"Ver estad�sticas acceso"),(64,"Eliminar mensaje recibido"),(65,"Editar horario tutor�as"),(66,"Solicitar env�o calificaciones"),(67,"Enviar p�gina"),(68,"Editar item"),(69,"Editar evaluaci�n"),(70,"Ver mensajes enviados"),(71,"Imprimir calendario"),(72,"Solicitar eliminaci�n avisos"),(73,"Eliminar aviso"),(74,"Editar pr�cticas"),(75,"Insertar item"),(76,"Editar bibliograf�a"),(77,"Dar alta usuarios (formulario)"),(78,"Solicitar alta usuarios"),(79,"Ver accesos SWAD"),(80,"Aumentar nivel item"),(81,"Disminuir nivel item"),(82,"Cambiar otra contrase�a"),(83,"Ver lista resumida alumnos"),(84,"Ver uso de SWAD"),(85,"Ver convocatorias"),(86,"Enviar descripci�n"),(87,"Dar baja todos alumnos"),(88,"Solicitar baja todos alumnos"),(89,"Ver fichas alumnos"),(90,"Eliminar mensaje enviado"),(91,"Editar convocatoria"),(92,"Solicitar edici�n convocatorias"),(93,"Ver foro profesores"),(94,"Ver mensajes tema foro prof."),(95,"Ver foros"),(96,"Editar enlaces"),(97,"Enviar enlace"),(98,"Evaluar test"),(99,"Enviar calificaciones"),(100,"Solicitar env�o correo"),(101,"Solicitar eliminaci�n registro"),(102,"Eliminar registro accesos"),(103,"Solicitar test"),(104,"Solicitar edici�n de tests"),(105,"Escribir pregunta test"),(106,"Cambiar m�x. alumnos en grupo"),(107,"Solicitar eliminaci�n grupo"),(108,"Solicitar edici�n de grupos"),(109,"Editar FAQ"),(110,"Enviar convocatoria"),(111,"Imprimir fichas alumnos"),(112,"Solicitar consulta accesos asig."),(113,"Solicitar consulta accesos SWAD"),(114,"Enviar tema foro"),(115,"Cambiar usuarios a MySQL"),(116,"Solicitar uni�n a grupos"),(117,"Pasar apodos a BD"),(118,"Cambiar de grupos"),(119,"Ver accesos asignatura"),(120,"Imprimir orla alumnos"),(121,"Renombrar grupo"),(122,"Crear grupo"),(123,"Subir posici�n item"),(124,"Bajar posici�n item"),(125,"Editar test"),(126,"Enviar pregunta test"),(127,"Imprimir fichas profesores"),(128,"Ver mensajes tema foro asig."),(129,"Ver foro asignatura"),(130,"Editar trabajos"),(131,"Editar pregunta test"),(132,"Editar preguntas test"),(133,"Eliminar pregunta test"),(134,"Solicitar preguntas test"),(135,"Enviar respuesta foro usrs."),(136,"Eliminar mensaje foro usrs."),(137,"Ver mensajes tema foro usrs."),(138,"Ver foro usuarios"),(139,"Editar trabajos de asignatura"),(140,"Importar tests");
INSERT INTO actions (ActCod,Txt) VALUES (141,"Solicitar cambio descriptor test"),(142,"Cambiar descriptor test"),(143,"Renombrar descriptor test"),(144,"Enviar respuesta foro asig."),(145,"Eliminar carpeta descarga"),(146,"Dar alta usuarios (archivo)"),(147,"Exportar fichas a base de datos"),(148,"Enviar archivo mis trabajos"),(149,"Solicitar nuevo arch/carp desc."),(150,"Solic. nuevo arch/carp mis trab."),(151,"Editar trabajos de alumno"),(152,"Imprimir horario"),(153,"Enviar archivo a malet�n"),(154,"Solicitar nuevo arch/carp malet."),(155,"Eliminar archivo malet�n"),(156,"Eliminar mensaje foro prof."),(157,"Eliminar mensaje foro asig."),(158,"Enviar tema foro prof."),(159,"Enviar tema foro asig."),(160,"Enviar respuesta foro prof."),(161,"Confirmar alta usuario"),(162,"Solicitar DNI cambio contrase�a"),(163,"Solicitar creaci�n grupo"),(164,"Enviar p�gina de enlaces"),(165,"Solicitar edici�n grupos"),(166,"Borrar grupo"),(167,"Cambiar tipo de grupo"),(168,"Ver mis calificaciones"),(169,"Eliminar archivo mis trabajos"),(170,"Crear carpeta en malet�n"),(171,"Ver centros y departamentos"),(172,"Crear carpeta mis trabajos"),(173,"Eliminar archivo descarga"),(174,"Crear tipo de grupo"),(175,"Eliminar grupo"),(176,"Enviar tema foro usrs."),(177,"Solicitar DNI alta usuario"),(178,"Cambiar de grupo"),(179,"Imprimir convocatoria"),(180,"Convertir horarios"),(181,"Insertar item pr�cticas"),(182,"Enviar enlace a enlaces"),(183,"Eliminar item pr�cticas"),(184,"Enviar p�gina de evaluaci�n"),(185,"Enviar p�gina de bibliograf�a"),(186,"A�adir convocatoria"),(187,"Eliminar convocatoria"),(188,"Eliminar hebra foro asig."),(189,"Solicitar elim. hebra foro asig."),(190,"Solicitar elim. hebra foro prof."),(191,"Solicitar elim. hebra foro usrs."),(192,"Eliminar hebra foro usrs."),(193,"Eliminar hebra foro prof."),(194,"Acci�n desconocida"),(195,"Editar trabajos de usuario"),(196,"Eliminar carpeta malet�n"),(197,"Renombrar carpeta en malet�n"),(198,"Solicitar nuevo arch/carp trab."),(199,"Crear carpeta de trabajos"),(200,"Renombrar carpeta de trabajos"),(201,"Enviar archivo de trabajo"),(202,"Eliminar archivo trabajo"),(203,"Eliminar carpeta trabajos"),(204,"Renombrar carpeta mis trabajos"),(205,"Solic. nuevo arch/carp trab.asg."),(206,"Crear carpeta trabajos asg."),(207,"Enviar archivo trabajos asg."),(208,"Renombrar carpeta trabajos asg."),(209,"Eliminar archivo trabajos asg."),(210,"Eliminar carpeta trabajos asg."),(211,"Editar item teor�a"),(212,"Bajar posici�n item pr�cticas"),(213,"Subir posici�n item pr�cticas"),(214,"Disminuir nivel item pr�cticas"),(215,"Aumentar nivel item pr�cticas"),(216,"Editar item pr�cticas"),(217,"Insertar item teor�a"),(218,"Eliminar item teor�a"),(219,"Enviar p�gina de FAQ"),(220,"Bajar posici�n item teor�a"),(221,"Subir posici�n item teor�a"),(222,"Disminuir nivel item teor�a"),(223,"Aumentar nivel item teor�a"),(224,"Enviar enlace a bibliograf�a"),(225,"Solicitar baja alumnos"),(226,"Dar baja usuarios (formulario)"),(227,"Dar baja usuarios (archivo)"),(228,"Eliminar carpeta mis trabajos"),(229,"Solicitar DNI baja total"),(230,"Confirmar baja total usuario"),(231,"Solicitar DNI baja usuario"),(232,"Confirmar baja usuario"),(233,"Confirmar cambio otra contrase�a"),(234,"Enviar enlace a FAQ"),(235,"Enviar enlace a evaluaci�n"),(236,"Solicitar eliminaci�n tipo grupo"),(237,"Eliminar tipo de grupo"),(238,"Cambiar obligatoriedad grupo"),(239,"Cambiar multiplicidad grupo"),(240,"Ver lista resumida profes. SWAD"),(241,"Ver foro usuarios titulaci�n"),(242,"Ver foro usuarios SWAD"),(243,"Ver foro profesores titulaci�n"),(244,"Ver msjs. tema foro usrs.SWAD"),(245,"Ver foro profesores SWAD"),(246,"Ver msjs. tema foro prof.SWAD"),(247,"Enviar tema foro prof.tit."),(248,"Enviar respuesta foro prof.tit."),(249,"Solic. elim.hebra foro prof.tit."),(250,"Eliminar hebra foro prof.tit."),(251,"Eliminar mensaje foro prof.tit."),(252,"Enviar tema foro usrs.tit."),(253,"Solic. elim.hebra foro usrs.tit."),(254,"Eliminar hebra foro usrs.tit."),(255,"Ver msjs. tema foro usrs.tit.");
INSERT INTO actions (ActCod,Txt) VALUES (256,"Eliminar mensaje foro usrs.tit."),(257,"Ver msjs. tema foro asig."),(258,"Enviar tema foro usrs.SWAD"),(259,"Enviar tema foro prof.SWAD"),(260,"Solic. elim.hebra foro usrs.SWAD"),(261,"Eliminar hebra foro usrs.SWAD"),(262,"Eliminar mensaje foro prof.SWAD"),(263,"Enviar respuesta foro prof.SWAD"),(264,"Enviar respuesta foro usrs.SWAD"),(265,"Solic. elim.hebra foro asig."),(266,"Eliminar mensaje foro usrs.SWAD"),(267,"Administrar zona com�n asg."),(268,"Administrar zona com�n grp."),(269,"Enviar respuesta foro usrs.tit."),(270,"Solicitar nuevo arch/carp calif."),(271,"Crear carpeta calificaciones"),(272,"Eliminar archivo calificaciones"),(273,"Eliminar carpeta calificaciones"),(274,"Enviar archivo calificaciones"),(275,"Renombrar carpeta calificaciones"),(276,"Eliminar �rbol malet�n"),(277,"Eliminar �rbol descarga"),(278,"Eliminar �rbol mis trabajos"),(279,"Eliminar �rbol trabajos asg."),(280,"Eliminar �rbol calificaciones"),(281,"Cambiar filas de cabecera"),(282,"Cambiar filas de pie"),(283,"Cambiar calificaciones"),(284,"Administrar calificaciones"),(285,"Solicitar cambio ficha com�n"),(286,"Solicitar elim. arch. malet�n"),(287,"Solicitar elim. arch. descarga"),(288,"Solicitar elim. arch. mis trab."),(289,"Solicitar elim. arch. trab. asg."),(290,"Solicitar elim. arch. calif."),(291,"Ver msjs. tema foro prof.tit."),(292,"Solicitar edici�n campos fichas"),(293,"Crear campo de fichas"),(294,"Solicitar elimin. campo fichas"),(295,"Eliminar campo de fichas"),(296,"Renombrar campo de fichas"),(297,"Cambiar visibilidad campo"),(298,"Cambiar ficha com�n"),(299,"Solicitar cambio ficha asig."),(300,"Cambiar ficha alumno en asig."),(301,"Cambiar ficha asignatura"),(302,"Cambiar multiplic. tipo grupo"),(303,"Cambiar obligator. tipo grupo"),(304,"Renombrar tipo de grupo"),(305,"Cambiar l�neas campo"),(306,"Solicitar fichas alumnos"),(307,"Ocultar arch/carp descarga"),(308,"Inhabilitar arch/carp descarga"),(309,"Habilitar arch/carp descarga"),(310,"Copiar descarga"),(311,"Copiar de malet�n"),(312,"Copiar de trabajos asg."),(313,"Copiar de calificaciones"),(314,"Copiar de mis trabajos"),(315,"Pegar en malet�n"),(316,"Copiar de descarga"),(317,"Pegar en descarga"),(318,"Pegar en mis trabajos"),(319,"Pegar en trabajos asg."),(320,"Pegar en calificaciones"),(321,"Cerrar grupo"),(322,"Abrir grupo"),(323,"Solic.nuevo arch/carp com�n asg."),(324,"Crear carpeta com�n asg."),(325,"Eliminar carpeta com�n asg."),(326,"Enviar archivo com�n asg."),(327,"Solicitar elim. arch. com�n asg."),(328,"Eliminar archivo com�n asg."),(329,"Renombrar carpeta com�n asg."),(330,"Copiar de zona com�n asg."),(331,"Pegar en zona com�n asg."),(332,"Eliminar �rbol com�n asg."),(333,"Solic.nuevo arch/carp com�n grp."),(334,"Crear carpeta com�n grp."),(335,"Enviar archivo com�n grp."),(336,"Copiar de zona com�n grp."),(337,"Pegar en zona com�n grp."),(338,"Eliminar carpeta com�n grp."),(339,"Eliminar �rbol com�n grp."),(340,"Renombrar carpeta com�n grp."),(341,"Solicitar elim. arch. com�n grp."),(342,"Eliminar archivo com�n grp."),(343,"Ver lista profes. SWAD"),(344,"Pasar mensajes a base de datos"),(345,"Ver foro usuarios asignatura"),(346,"Ver msjs. tema foro usrs.asig."),(347,"Ver msjs. tema foro prof.asig."),(348,"Ver msjs. tema foro usrs.cent."),(349,"Enviar respuesta foro usrs.asig."),(350,"Enviar tema foro usrs.asig."),(351,"Eliminar mensaje foro usrs.asig."),(352,"Solic. elim.hebra foro usrs.asig"),(353,"Eliminar hebra foro usrs.asig."),(354,"Pasar avisos a base de datos"),(355,"Pasar foros a base de datos"),(356,"Pasar mensajes 9 a base de datos"),(357,"Pasar mensajes 0 a base de datos"),(358,"Pasar mensajes 1 a base de datos"),(359,"Pasar mensajes 2 a base de datos"),(360,"Pasar mensajes 3 a base de datos"),(361,"Pasar mensajes 4 a base de datos"),(362,"Pasar mensajes 5 a base de datos"),(363,"Pasar mensajes 6 a base de datos"),(364,"Pasar mensajes 7 a base de datos"),(365,"Pasar mensajes 8 a base de datos"),(366,"Solic. elim.hebra foro prof.SWAD");
INSERT INTO actions (ActCod,Txt) VALUES (367,"Eliminar hebra foro prof.SWAD"),(368,"Cambiar ficha com�n de otro usr."),(369,"Selec. tipo de info. asignatura"),(370,"Selec. tipo de bibliograf�a"),(371,"Editor integrado de pr�cticas"),(372,"Editor integrado de teor�a"),(373,"Solicitar DNI foto usuario"),(374,"Cambiar foto otro usuario"),(375,"Solicitar cambio foto otro usr."),(376,"Editor integrado de bibliograf�a"),(377,"Editor de texto de bibliograf�a"),(378,"Selec. tipo de prog. de teor�a"),(379,"Editor de texto de teor�a"),(380,"Selec. tipo de FAQ"),(381,"Enviar p�gina de teor�a"),(382,"Selec. tipo de prog. de pr�ct."),(383,"Enviar p�gina de pr�cticas"),(384,"Selec. tipo de evaluaci�n"),(385,"Selec. tipo de enlaces"),(386,"Editor integrado de evaluaci�n"),(387,"Editor de texto de evaluaci�n"),(388,"Editor integrado de enlaces"),(389,"Editor de texto de pr�cticas"),(390,"Selec. tipo de descripci�n"),(391,"Pasar descrip.<BR>a base datos"),(392,"Editor de texto de descripci�n"),(393,"Enviar texto de descripci�n"),(394,"Enviar texto de teor�a"),(395,"Editor integrado de descripci�n"),(396,"Enviar texto de pr�cticas"),(397,"Enviar text de evaluaci�n"),(398,"Enviar texto de bibliograf�a"),(399,"Enviar p�gina de descripci�n"),(400,"Editor de texto de enlaces"),(401,"Enviar texto de enlaces"),(402,"Enviar enlace a pr�cticas"),(403,"Enviar enlace a teor�a"),(404,"Editor integrado de FAQ"),(405,"Editor de texto de FAQ"),(406,"Enviar texto de FAQ"),(407,"Enviar enlace a descripci�n"),(408,"Ver horario todas clases"),(409,"Imprimir horario todas clases"),(410,"Expandir carpeta malet�n"),(411,"Contraer carpeta malet�n"),(412,"Expandir carpeta ver calif."),(413,"Contraer carpeta ver calif."),(414,"Expandir carpeta ver descarga"),(415,"Expandir carpeta admin. descarga"),(416,"Expandir carpeta trabajos asg."),(417,"Contraer carpeta admin. calif."),(418,"Expandir carpeta admin. calif."),(419,"Contraer carpeta ver descarga"),(420,"Contraer carpeta admin. descarga"),(421,"Expandir carpeta com�n asg."),(422,"Contraer carpeta com�n asg."),(423,"Expandir carpeta mis trabajos"),(424,"Contraer carpeta trabajos asg."),(425,"Contraer carpeta mis trabajos"),(426,"Contraer carpeta com�n grupo"),(427,"Expandir carpeta com�n grupo"),(428,"Eliminar foto"),(429,"Eliminar foto otro usuario"),(430,"Ver foro profesores centro"),(431,"Ver foro profesores asignatura"),(432,"Solicitar elim. usuarios antiguo"),(433,"Solic. elimin. mensajes enviados"),(434,"Eliminar mensajes enviados"),(435,"Solic. elimin. mensajes recibido"),(436,"Eliminar mensajes recibidos"),(437,"Solicitar edici�n titulaci�n"),(438,"Dar alta usr. sin cambiar datos"),(439,"Dar alta usr. cambiando datos"),(440,"Dar alta usuario nuevo"),(441,"Ver orla profes. SWAD"),(442,"Ver orla profesores"),(443,"Imprimir orla profesores"),(444,"Calcular fotos promedio"),(445,"Ver fotos promedio"),(446,"Imprimir fotos promedio"),(447,"Ver orla de titulaciones"),(448,"Imprimir orla de titulaciones"),(449,"Ver salas pizarra"),(450,"Abrir pizarra virtual"),(451,"Solicitar configuraci�n test"),(452,"Prohibir descriptor test"),(453,"Permitir descriptor test"),(454,"Recibir configuraci�n test"),(455,"Cambiar orden. respuestas test"),(456,"Censurar mensaje foro usrs.asig."),(457,"Permitir mensaje foro usrs.asig."),(458,"Censurar mensaje foro usrs.SWAD"),(459,"Permitir mensaje foro usrs.SWAD"),(460,"Censurar mensaje foro usrs.tit."),(461,"Administrar zona com�n"),(462,"Expandir carpeta ver desc. asg."),(463,"Ver descargas asignatura"),(464,"Habilitar arch/carp desc. asg."),(465,"Inhabilitar arch/carp desc. asg."),(466,"Ver descargas grupo"),(467,"Administrar descargas grupo"),(468,"Solicitar nueva descarga grp."),(469,"Crear carpeta descarga grp."),(470,"Copiar de descarga asg."),(471,"Pegar en descarga grp."),(472,"Copiar de descarga grp."),(473,"Solicitar elim. arch. desc. grp."),(474,"Eliminar archivo descarga grp."),(475,"Administrar descargas asignatura"),(476,"Contraer carpeta ver desc. asg."),(477,"Expandir carpeta admin.desc.asg.");
INSERT INTO actions (ActCod,Txt) VALUES (478,"Pegar en descarga asg."),(479,"Solicitar elim. arch. desc. asg."),(480,"Eliminar archivo descarga asg."),(481,"Solicitar nueva descarga asg."),(482,"Enviar archivo descarga asg."),(483,"Enviar archivo descarga grp."),(484,"Eliminar carpeta descarga grp."),(485,"Eliminar �rbol descarga grp."),(486,"Expandir carpeta admin.desc.grp."),(487,"Contraer carpeta admin.desc.grp."),(488,"Expandir carpeta ver desc. grp."),(489,"Contraer carpeta ver desc. grp."),(490,"Renombrar carpeta descarga grp."),(491,"Crear carpeta descarga asg."),(492,"Inhabilitar arch/carp desc. grp."),(493,"Habilitar arch/carp desc. grp."),(494,"Contraer carpeta admin.desc.asg."),(495,"Habilitar zonas archivos grupo"),(496,"Inhabilitar zonas archivos grupo"),(497,"Eliminar carpeta descarga asg."),(498,"Eliminar �rbol descarga asg."),(499,"Administrar calificaciones asg."),(500,"Administrar calificaciones grp."),(501,"Copiar de calif. asg."),(502,"Pegar en calif. grp."),(503,"Cambiar filas de cabecera asg."),(504,"Cambiar filas de pie asg."),(505,"Solic. nuevo arch/carp calif.asg"),(506,"Crear carpeta calif. asg."),(507,"Pegar en calif. asg."),(508,"Solicitar elim. arch. calif. grp"),(509,"Eliminar archivo calif. grp."),(510,"Cambiar filas de cabecera grp."),(511,"Cambiar filas de pie grp."),(512,"Solic. nuevo arch/carp calif.grp"),(513,"Crear carpeta calif. grp."),(514,"Enviar archivo calif. grp."),(515,"Contraer carpeta admin.calif.asg"),(516,"Enviar archivo calif. asg."),(517,"Expandir carpeta admin.calif.grp"),(518,"Contraer carpeta admin.calif.grp"),(519,"Copiar de calif. grp."),(520,"Eliminar carpeta calif. grp."),(521,"Eliminar �rbol calif. grp."),(522,"Expandir carpeta ver calif. grp"),(523,"Ver mis calificaciones asg."),(524,"Ver mis calificaciones grp."),(525,"Ver calificaciones asignatura"),(526,"Ver calificaciones grupo"),(527,"Contraer carpeta ver calif. asg."),(528,"Expandir carpeta ver calif. asg."),(529,"Renombrar carpeta calif. grp."),(530,"Eliminar carpeta calif. asg."),(531,"Expandir carpeta admin.calif.asg"),(532,"Solicitar elim. arch. calif. asg"),(533,"Eliminar archivo calif. asg."),(534,"Eliminar �rbol calif. asg."),(535,"Renombrar carpeta descarga asg."),(536,"Solicitar edici�n titulaciones"),(537,"Crear tipo de titulaci�n"),(538,"Renombrar tipo de titulaci�n"),(539,"Solicitar eliminaci�n tipo titul"),(540,"Crear titulaci�n"),(541,"Solicitar eliminaci�n titulaci�n"),(542,"Eliminar titulaci�n"),(543,"Renombrar titulaci�n"),(544,"Cambiar tipo de titulaci�n"),(545,"Eliminar tipo titulaci�n"),(546,"Cambiar nombre breve titulaci�n"),(547,"Cambiar nombre completo titulac."),(548,"Cambiar campus de titulaci�n"),(549,"Contraer carpeta ver calif. grp."),(550,"Cambiar primer a�o de titulaci�n"),(551,"Cambiar �ltimo a�o de titulaci�n"),(552,"Cambiar a�o opcional titulaci�n"),(553,"Cambiar logo de titulaci�n"),(554,"Cambiar web de titulaci�n"),(555,"Solicitar edici�n asignaturas"),(556,"Crear asignatura"),(557,"Solicitar acept./rechazo mi alta"),(558,"Aceptar mi alta"),(559,"Rechazar mi alta"),(560,"Eliminar asignatura"),(561,"Cambiar curso de asignatura"),(562,"Cambiar semestre de asignatura"),(563,"Cambiar nombre breve asignatura"),(564,"Cambiar nombre completo asig."),(565,"Mover asignatura a otra titulac."),(566,"Importar titulaciones"),(567,"Importar asignaturas"),(568,"Solicitar cambio resp. secreta"),(569,"Cambiar respuesta secreta"),(570,"Solicitar recuerdo contrase�a"),(571,"Recordar contrase�a"),(572,"Censurar mensaje foro prof.SWAD"),(573,"Solicitar edici�n tipos titul."),(574,"Renombrar carpeta calif. asg."),(575,"Solicitar b�squeda asignaturas"),(576,"Buscar asignaturas"),(577,"Ver lista resumida profesores"),(578,"Ver lista profesores"),(579,"Ver lista resumida administrador"),(580,"Solicitar DNI alta administrador"),(581,"Confirmar alta administrador"),(582,"Solicitar DNI baja administrador"),(583,"Solicitar baja administrador"),(584,"Dar baja administrador"),(585,"Confirmar baja administrador"),(586,"Dar alta administrador");
INSERT INTO actions (ActCod,Txt) VALUES (587,"Ver lista administradores"),(588,"Cambiar tipo de usuario identifi"),(589,"Cambiar tipo de usuario identif."),(590,"Solicitar elim.usuarios antiguos"),(591,"Solic. consulta accesos SWAD"),(592,"Solicitar acept/rechazo mi alta"),(593,"Solic. elim. mensajes recibidos"),(594,"Solic. consulta accesos asig."),(595,"Solicitar elim.arch.calif.asg."),(596,"Solic.nuevo arch/carp calif.asg."),(597,"Solic. nuevo arch/carp malet."),(598,"Confirm. cambio otra contrase�a"),(599,"Enviar resp. foro usrs.asig."),(600,"Solicitar elim.arch.calif.grp."),(601,"Solic.nuevo arch/carp calif.grp."),(602,"Eliminar mens. foro usrs.asig."),(603,"Enviar resp. foro usrs.SWAD"),(604,"Solic. elim. mensajes enviados"),(605,"Expandir carpeta ver calif.grp"),(606,"Enviar resp. foro usrs.tit."),(607,"Expandir carp. admin.calif.asg."),(608,"Eliminar mens. foro usrs.tit."),(609,"Contraer carpeta ver calif.grp."),(610,"Censurar mens. foro usrs.asig."),(611,"Solic.elim.hebra foro usrs.asig"),(612,"Solic. elim. usuarios antiguos"),(613,"Eliminar mens. foro usrs.SWAD"),(614,"Solic.elim.hebra foro usrs.tit."),(615,"Censurar mens. foro usrs.tit."),(616,"Permitir mens. foro usrs.tit."),(617,"Enviar resp. foro prof.tit."),(618,"Censurar mens. foro prof.tit."),(619,"Permitir mens. foro prof.tit."),(620,"Solic.elim.hebra foro prof.tit."),(621,"Contraer carp. admin.calif.asg."),(622,"Enviar resp. foro prof.SWAD"),(623,"Eliminar mens. foro prof.SWAD"),(624,"Permitir mens. foro usrs.asig."),(625,"Censurar mens. foro usrs.SWAD"),(626,"Solic.elim.hebra foro prof.SWAD"),(627,"Solicitar b�squeda asig./prof."),(628,"Buscar asignaturas/profesores"),(629,"Ver mensajes recibidos nuevos"),(630,"Solic.elim.hebra foro usrs.SWAD"),(631,"Expandir carp. admin.calif.grp."),(632,"Permitir mens. foro usrs.SWAD"),(633,"Enviar contrase�a por correo"),(634,"Permitir mens. foro prof.SWAD"),(635,"Censurar mens. foro prof.SWAD"),(636,"Pegar hebra foro usrs.asig."),(637,"Cortar hebra foro usrs.asig."),(638,"Cortar hebra foro usrs.SWAD"),(639,"Pegar hebra foro usrs.SWAD"),(640,"Pegar hebra foro usrs.tit."),(641,"Cortar hebra foro prof.SWAD"),(642,"Ver foro profesores univ."),(643,"Ver msjs. tema foro prof.univ."),(644,"Cortar hebra foro prof.univ."),(645,"Cualquier acci�n"),(646,"Ver foro usuarios univ."),(647,"Pegar hebra foro prof.SWAD"),(648,"Ver msjs. tema foro usrs.univ."),(649,"Cortar hebra foro usrs.univ."),(650,"Pegar hebra foro usrs.univ."),(651,"Enviar resp. foro prof.univ."),(652,"Eliminar mens. foro prof.univ."),(653,"Cortar hebra foro usrs.tit."),(654,"Cambiar idioma"),(655,"Enviar tema foro prof.univ."),(656,"Enviar resp. foro usrs.univ."),(657,"Eliminar mens. foro usrs.univ."),(658,"Enviar tema foro usrs.univ."),(659,"Censurar mens. foro usrs.univ."),(660,"Solic.elim.hebra foro usrs.univ."),(661,"Eliminar hebra foro usrs.univ."),(662,"Permitir mens. foro usrs.univ."),(663,"Ver mensaje recibido"),(664,"Ver mensaje enviado"),(665,"Solicitar env�o contrase�a"),(666,"Ocultar columnas laterales"),(667,"Mostrar columnas laterales"),(668,"Ocultar columna izquierda"),(669,"Ocultar columna derecha"),(670,"Mostrar columna izquierda"),(671,"Mostrar columna derecha"),(672,"Cambiar dise�o"),(673,"Editar preferencias"),(674,"Cambiar columnas"),(675,"Ver departamentos"),(676,"Ver centros"),(677,"Editar departamentos"),(678,"Ver orla o lista de alumnos"),(679,"Ver orla o lista de profesores"),(680,"Eliminar mens. foro prof.tit."),(681,"Editar centros"),(682,"Cambiar nombre breve centro"),(683,"Cambiar web de centro"),(684,"Cambiar nombre completo centro"),(685,"Crear centro"),(686,"Eliminar centro"),(687,"Crear departamento"),(688,"Cambiar nombre breve depar."),(689,"Cambiar nombre completo depar."),(690,"Eliminar departamento"),(691,"Cambiar web de departamento"),(692,"Recibir foto y detectar rostros"),(693,"Detectar rostros mi foto"),(694,"Cambiar mi foto"),(695,"Detectar rostros en foto otro us"),(696,"Ver instituciones"),(697,"Editar instituciones"),(698,"Crear instituci�n");
INSERT INTO actions (ActCod,Txt) VALUES (699,"Cambiar logo de instituci�n"),(700,"Cambiar web de instituci�n"),(701,"Cambiar nombre completo ins."),(702,"Cambiar nombre breve ins."),(703,"Ver lugares"),(704,"Editar lugares"),(705,"Crear lugar"),(706,"Cambiar lugar de titulaci�n"),(707,"Ver d&iacute;as festivos"),(708,"Editar d�as festivos"),(709,"Cambiar fecha d�a festivo"),(710,"Cambiar nombre d�a festivo"),(711,"Crear d�a festivo"),(712,"Cambiar lugar d�a festivo"),(713,"Editar festivo."),(714,"Crear festiv."),(715,"Cambiar tipo festiv."),(716,"Eliminar festiv."),(717,"Cambiar fecha inicio festiv."),(718,"Cambiar fecha fin festiv."),(719,"Cambiar institituci�n de titul."),(720,"Cambiar instituci�n centro"),(721,"Cambiar instituci�n depar."),(722,"Cambiar instituci�n de titul."),(723,"Ver foro profesores general"),(724,"Ver foro profesores instit."),(725,"Ver foro usuarios instit."),(726,"Ver foro usuarios general"),(727,"Ver msjs. tema foro usrs.gral."),(728,"Cortar hebra foro usrs.gral."),(729,"Pegar hebra foro usrs.inst."),(730,"Ver msjs. tema foro usrs.inst."),(731,"Ver msjs. tema foro prof.gral."),(732,"Cortar hebra foro prof.gral."),(733,"Pegar hebra foro prof.inst."),(734,"Pegar hebra foro prof.gral."),(735,"Eliminar mens. foro usrs.gral."),(736,"Enviar tema foro usrs.gral."),(737,"Enviar tema foro usrs.inst."),(738,"Cortar hebra foro usrs.inst."),(739,"Pegar hebra foro usrs.gral."),(740,"Enviar resp. foro usrs.inst."),(741,"Solic.elim.hebra foro usrs.inst."),(742,"Eliminar hebra foro usrs.inst."),(743,"Eliminar mens. foro usrs.inst."),(744,"Solic.elim.hebra foro usrs.gral."),(745,"Eliminar hebra foro usrs.gral."),(746,"Ver msjs. tema foro prof.inst."),(747,"Enviar resp. foro usrs.gral."),(748,"Ver enlaces institucionales"),(749,"Editar enlaces institucionales"),(750,"Crear enlace institucional"),(751,"Cambiar nombre completo enlace i"),(752,"Cambiar web de enlace institucio"),(753,"Cambiar nombre breve enlace ins."),(754,"Enviar tema foro prof.asig."),(755,"Enviar resp. foro prof.asig."),(756,"Eliminar mens. foro prof.asig."),(757,"Solic.elim.hebra foro prof.asig"),(758,"Eliminar hebra foro prof.asig."),(759,"Eliminar instituci�n"),(760,"Cambiar iconos seguros"),(761,"Solicitar ver uso de SWAD"),(762,"Ver avisos"),(763,"Ocultar aviso"),(764,"Mostrar aviso"),(765,"Enviar tema foro prof.gral."),(766,"Cambiar nombre festiv."),(767,"Solicitar estad. asignaturas"),(768,"Ver estad. asignaturas"),(769,"Enviar tema foro prof.inst."),(770,"Enviar resp. foro prof.inst."),(771,"Cortar hebra foro prof.inst."),(772,"Eliminar mens. foro prof.inst."),(773,"Eliminar usuarios antiguos"),(774,"Cambiar privacidad foto"),(775,"Cambiar notificaci�n mensajes"),(776,"Eliminar lugar"),(777,"Listar plugins"),(778,"Editar plugins"),(779,"Crear plugin"),(780,"Cambiar IP de plugin"),(781,"Cambiar logo de plugin"),(782,"Cambiar nombre de plugin"),(783,"Cambiar URL de plugin"),(784,"Ver gu�a docente"),(785,"Editar gu�a docente"),(786,"Editor integrado de gu�a docente"),(787,"Editor de texto de gu�a docente"),(788,"Enviar p�gina de gu�a docente"),(789,"Selec. tipo de gu�a docente"),(790,"Enviar texto de gu�a docente"),(791,"Enviar enlace a gu�a docente"),(792,"Editar mis trabajos"),(793,"Pruebas de PRADO"),(794,"Autenticar usuario desde fuera"),(795,"Solicitar importaci�n de alumnos"),(796,"Ver lista de alumnos oficiales"),(797,"Solicitar alta/baja alumnos"),(798,"Dar alta/baja alumnos (archivo)"),(799,"Dar alta/baja alumnos (formul.)"),(800,"Eliminando ceros de los DNI"),(801,"Ver actividades"),(802,"Editar actividades"),(803,"Crear actividad"),(804,"Cambiar tipo actividad"),(805,"Cambiar fecha fin actividad"),(806,"Eliminar actividad"),(807,"Cambiar fecha inicio actividad"),(808,"Cambiar nombre actividad"),(809,"Cambiar texto actividad"),(810,"Cambiar env�o trabajo actividad"),(811,"Editar actividad"),(812,"Solicitar creaci�n actividad"),(813,"Solicitar elim. actividad"),(814,"Solicitar edici�n actividad"),(815,"Modificar actividad"),(816,"Enviar resp. foro prof.gral.");
INSERT INTO actions (ActCod,Txt) VALUES (817,"Solic. nuevo arch/carp act.asg."),(818,"Crear carpeta activid. asg."),(819,"Expandir carpeta activid. asg."),(820,"Eliminar carpeta activid. asg."),(821,"Pegar en actividades asg."),(822,"Eliminar �rbol activid. asg."),(823,"Renombrar carpeta activid. asg."),(824,"Expandir carpeta mis activid."),(825,"Solic. nuevo arch/carp mis act."),(826,"Crear carpeta mis actividades"),(827,"Eliminar carpeta mis activid."),(828,"Eliminar �rbol mis activid."),(829,"Copiar de mis actividades"),(830,"Pegar en mis actividades"),(831,"Contraer carpeta mis activid."),(832,"Enviar archivo mis actividades"),(833,"Eliminar archivo mis activid."),(834,"Solicitar elim. arch. mis act."),(835,"Contraer carpeta activid. asg."),(836,"Copiar de actividades asg."),(837,"Solicitar elim. arch. ct. asg."),(838,"Eliminar archivo activid. asg."),(839,"Renombrar carpeta mis activid."),(840,"Instalaci�n inicial"),(841,"Cambiar tema (colores)"),(842,"Cambiar a otra asignatura"),(843,"Solicitar inicio sesi�n"),(844,"Ver men� inicial"),(845,"Refrescar mensajes y usuarios"),(846,"Enviar archivo activid. asg."),(847,"Ver info. asignatura"),(848,"Editar info. asignatura"),(849,"Selec. tipo de info. asg."),(850,"Editor de texto de info. asg."),(851,"Enviar texto de info. asg."),(852,"Editor integrado de info. asg."),(853,"Enviar p�gina de info. asg."),(854,"Enviar enlace a info asg."),(855,"Ver dominios de correo"),(856,"Editar dominios de correo"),(857,"Crear dominio de correo"),(858,"Cambiar dominio de correo"),(859,"Cambiar informaci�n dominio corr"),(860,"Eliminar dominio de correo"),(861,"Pasar lista"),(862,"Ver pa�ses"),(863,"Editar pa�ses"),(864,"Crear pa�s"),(865,"Cambiar pa�s de instituci�n"),(866,"Cambiar nombre breve pa�s"),(867,"Solic.elim.discus.foro usrs.asig"),(868,"Eliminar discus. foro usrs.asig."),(869,"Solic.elim.discus.foro prof.asig"),(870,"Cambiar oblig. leer gu�a docente"),(871,"Cambiar oblig. leer prog. de teo"),(872,"Cambiar oblig. leer prog. de pr�"),(873,"Cambiar oblig. leer bibliograf�a"),(874,"Cambiar oblig. leer FAQ"),(875,"Cambiar oblig. leer enlaces"),(876,"Eliminar discus. foro prof.asig."),(877,"Cambiar oblig. leer info. asg."),(878,"Cambiar alumno leido info. asg."),(879,"Cambiar alumno leido FAQ"),(880,"Cambiar alumno leido prog. de te"),(881,"Solic.elim.discus.foro usrs.SWAD"),(882,"Eliminar discus. foro usrs.SWAD"),(883,"Cambiar oblig. leer evaluaci�n"),(884,"Cambiar alumno leido bibliograf�"),(885,"Cambiar alumno leido enlaces"),(886,"Cambiar alumno leido gu�a docent"),(887,"Cambiar alumno leido prog. de pr"),(888,"Cambiar descripci�n de plugin"),(889,"Eliminar plugin"),(890,"Cortar discus. foro usrs.SWAD"),(891,"Pegar discus. foro usrs.asig."),(892,"Ejecutar servicio web"),(893,"Eliminar pa�s"),(894,"Cambiar nombre breve lugar"),(895,"Cambiar nombre completo lugar"),(896,"Cambiar lugar festiv."),(897,"Eliminar enlace institucional"),(898,"Cambiar alumno leido evaluaci�n"),(899,"Solicitar editar trabajos asig."),(900,"Contraer carp. admin.calif.grp."),(901,"Ver foro usuarios centro"),(902,"Ver msjs. tema foro prof.cent."),(903,"Enviar tema foro usrs.cent."),(904,"Enviar tema foro prof.cent."),(905,"Enviar resp. foro usrs.cent."),(906,"Enviar resp. foro prof.cent."),(907,"Solic.elim.discus.foro usrs.tit."),(908,"Solic.elim.discus.foro prof.tit."),(909,"Solic.elim.discus.foro usrs.cent"),(910,"Solic.elim.discus.foro prof.cent"),(911,"Solic.elim.discus.foro usrs.inst"),(912,"Solic.elim.discus.foro prof.inst"),(913,"Solic.elim.discus.foro usrs.gral"),(914,"Solic.elim.discus.foro prof.gral"),(915,"Solic.elim.discus.foro prof.SWAD"),(916,"Eliminar discus. foro usrs.tit."),(917,"Eliminar discus. foro prof.tit."),(918,"Eliminar discus. foro usrs.cent."),(919,"Eliminar discus. foro prof.cent."),(920,"Eliminar discus. foro usrs.inst."),(921,"Eliminar discus. foro prof.inst."),(922,"Eliminar discus. foro usrs.gral."),(923,"Eliminar discus. foro prof.gral."),(924,"Eliminar discus. foro prof.SWAD"),(925,"Censurar mens. foro prof.gral."),(926,"Cortar discus. foro usrs.asig.");
INSERT INTO actions (ActCod,Txt) VALUES (927,"Cortar discus. foro prof.asig."),(928,"Cortar discus. foro usrs.tit."),(929,"Cortar discus. foro prof.tit."),(930,"Cortar discus. foro usrs.cent."),(931,"Cortar discus. foro prof.cent."),(932,"Cortar discus. foro usrs.inst."),(933,"Cortar discus. foro prof.inst."),(934,"Cortar discus. foro usrs.gral."),(935,"Cortar discus. foro prof.gral."),(936,"Cortar discus. foro prof.SWAD"),(937,"Pegar discus. foro prof.asig."),(938,"Pegar discus. foro usrs.tit."),(939,"Pegar discus. foro prof.tit."),(940,"Pegar discus. foro usrs.cent."),(941,"Pegar discus. foro prof.cent."),(942,"Pegar discus. foro usrs.inst."),(943,"Pegar discus. foro prof.inst."),(944,"Pegar discus. foro usrs.gral."),(945,"Pegar discus. foro prof.gral."),(946,"Pegar discus. foro usrs.SWAD"),(947,"Pegar discus. foro prof.SWAD"),(948,"Eliminar mens. foro usrs.cent."),(949,"Eliminar mens. foro prof.cent."),(950,"Eliminar mens. foro prof.gral."),(951,"Permitir mens. foro prof.asig."),(952,"Permitir mens. foro usrs.cent."),(953,"Permitir mens. foro prof.cent."),(954,"Permitir mens. foro usrs.inst."),(955,"Permitir mens. foro prof.inst."),(956,"Permitir mens. foro usrs.gral."),(957,"Permitir mens. foro prof.gral."),(958,"Censurar mens. foro prof.asig."),(959,"Censurar mens. foro usrs.cent."),(960,"Censurar mens. foro prof.cent."),(961,"Censurar mens. foro usrs.inst."),(962,"Censurar mens. foro prof.inst."),(963,"Censurar mens. foro usrs.gral.");

SELECT * FROM log_recent WHERE Action NOT IN (SELECT DISTINCT Txt FROM actions);
UPDATE log_recent,actions SET log_recent.ActCod=actions.ActCod WHERE log_recent.Action=actions.Txt;
UPDATE log_recent,actions SET log_recent.ActCod=actions.ActCod WHERE log_recent.ClickTime>='20100715000000' AND log_recent.Action=actions.Txt;
CREATE INDEX Txt ON actions (Txt);
UPDATE log_recent,actions SET log_recent.ActCod=actions.ActCod WHERE log_recent.Action=actions.Txt;

UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime<=20050101 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20050101 AND log.ClickTime<20060101 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20060101 AND log.ClickTime<20070101 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20070101 AND log.ClickTime<20080101 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20080101 AND log.ClickTime<20080701 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20080701 AND log.ClickTime<20090101 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20090101 AND log.ClickTime<20090701 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20090701 AND log.ClickTime<20100101 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20100101 AND log.ClickTime<20100401 AND log.Action=actions.Txt;
UPDATE log,actions SET log.ActCod=actions.ActCod WHERE log.ClickTime>=20100401 AND log.Action=actions.Txt;

------

SELECT * FROM log_recent WHERE ActCod='-1';
SELECT * FROM log WHERE ActCod='-1';

Tama�os antes del eliminar la columna con el texto de la acci�n:

[root@swad swad]# ls -l log*
-rw-rw---- 1 mysql mysql       8926 jul 15 12:33 log.frm
-rw-rw---- 1 mysql mysql 7820796572 jul 17 02:00 log.MYD		--> 71,57 bytes/entrada
-rw-rw---- 1 mysql mysql 6329058304 jul 17 02:00 log.MYI		--> 57,92 bytes/entrada
-rw-rw---- 1 mysql mysql       8926 jul 15 12:17 log_recent.frm
-rw-rw---- 1 mysql mysql   57154708 jul 17 02:00 log_recent.MYD		--> 91,22 bytes/entrada
-rw-rw---- 1 mysql mysql   44185600 jul 17 02:00 log_recent.MYI		--> 70,52 bytes/entrada

mysql> select count(*) from log_recent;
+----------+
| count(*) |
+----------+
|   626570 |
+----------+
1 row in set (0.00 sec)

mysql> select count(*) from log;
+-----------+
| count(*)  |
+-----------+
| 109271937 |
+-----------+
1 row in set (0.00 sec)

ALTER TABLE log_recent DROP COLUMN Action;
ALTER TABLE log DROP COLUMN Action;

Tama�os antes del eliminar la columna con el texto de la acci�n:

[root@swad swad]# ls -l log*
-rw-rw---- 1 mysql mysql       8892 jul 17 02:11 log.frm
-rw-rw---- 1 mysql mysql 5918754336 jul 17 02:40 log.MYD		--> 54,16 bytes/entrada
-rw-rw---- 1 mysql mysql 5810862080 jul 17 02:40 log.MYI		--> 53,18 bytes/entrada
-rw-rw---- 1 mysql mysql       8892 jul 17 02:04 log_recent.frm
-rw-rw---- 1 mysql mysql   36295732 jul 17 02:40 log_recent.MYD		--> 57,92 bytes/entrada
-rw-rw---- 1 mysql mysql   33438720 jul 17 02:40 log_recent.MYI		--> 53,36 bytes/entrada

SHOW INDEX FROM log;
-----

ALTER TABLE actions ADD COLUMN Obsolete ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER LANGUAGE;

UPDATE actions SET Obsolete='Y' WHERE ActCod IN (1,4,5,7,8,11,13,14,18,19,24,31,33,38,39,40,41,43,46,49,50,55,56,63,66,67,68,72,75,77,78,80,81,83,86,92,93,94,97,99,101,102,112,113,114,115,117,123,124,125,128,129,130,131,134,135,136,137,138,140,141,142,144,145,146,147,149,151,154,156,157,158,159,160,163,165,166,168,171,173,176,178,180,186,188,189,190,191,192,193,195,198,199,200,201,202,203,225,226,227,230,232,233,238,239,240,248,249,250,251,253,254,256,257,260,261,262,263,264,265,266,269,270,271,272,273,274,275,277,280,281,282,283,287,290,307,308,309,310,313,316,317,320,343,344,349,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,390,391,392,393,395,399,407,412,413,414,415,417,418,419,420,432,433,435,437,441,442,445,446,449,450,456,457,458,459,460,505,508,512,515,517,518,522,531,532,539,541,543,548,549,557,566,567,568,569,570,571,572,575,576,577,579,583,588,611,612,614,620,626,630,636,637,638,639,640,641,642,643,644,646,647,648,649,650,651,652,653,655,656,657,658,659,660,661,662,666,667,692,695,708,709,710,711,712,719,728,729,732,733,734,738,739,741,742,744,745,757,758,771,793,795,798,800,802,804,805,807,808,809,810,811);

----

ALTER TABLE log_recent CHANGE COLUMN IP IP CHAR(15) NOT NULL;
ALTER TABLE plugins CHANGE COLUMN IP IP CHAR(15) NOT NULL;
ALTER TABLE IP_prefs CHANGE COLUMN IP IP CHAR(15) NOT NULL;
ALTER TABLE log CHANGE COLUMN IP IP CHAR(15) NOT NULL;

----

ALTER TABLE log_recent ADD COLUMN LogCod INT NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (LogCod);
CREATE TABLE IF NOT EXISTS log_comments (LogCod INT NOT NULL,Comments VARCHAR(255),UNIQUE INDEX (LogCod));
INSERT INTO log_comments (LogCod,Comments) SELECT LogCod,Comments FROM log_recent WHERE Comments<>"";

ALTER TABLE log ADD COLUMN LogCod INT NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (LogCod);
DROP TABLE log_comments;
CREATE TABLE IF NOT EXISTS log_comments (LogCod INT NOT NULL,Comments VARCHAR(255),UNIQUE INDEX (LogCod));
INSERT INTO log_comments (LogCod,Comments) SELECT LogCod,Comments FROM log WHERE Comments<>'';

DELETE FROM log_recent;
ALTER TABLE log_recent DROP COLUMN LogCod;
ALTER TABLE log_recent ADD COLUMN LogCod INT NOT NULL FIRST, ADD PRIMARY KEY (LogCod);
INSERT INTO log_recent (LogCod,ActCod,DegCod,CrsCod,DNI,UsrType,ClickTime,TimeToGenerate,TimeToSend,IP,Comments) SELECT LogCod,ActCod,DegCod,CrsCod,DNI,UsrType,ClickTime,TimeToGenerate,TimeToSend,IP,Comments FROM log WHERE ClickTime>='20100717';

ALTER TABLE log_recent DROP COLUMN Comments;
ALTER TABLE log DROP COLUMN Comments;

ALTER TABLE log_recent CHANGE COLUMN LogCod LogCod INT NOT NULL, ADD PRIMARY KEY (LogCod);

----

ALTER TABLE log_comments CHANGE COLUMN Comments Comments VARCHAR(255) NOT NULL;

DELETE FROM log_recent WHERE LogCod>=109483390 and LogCod<=109483929 and ActCod=843;
DELETE FROM log_recent WHERE LogCod>=109485028 and LogCod<=109485781 and ActCod=843;
DELETE FROM log_recent WHERE LogCod>=109489685 and LogCod<=109491912 and ActCod=843;
DELETE FROM log WHERE LogCod>=109483390 and LogCod<=109483929 and ActCod=843;
DELETE FROM log WHERE LogCod>=109485028 and LogCod<=109485781 and ActCod=843;
DELETE FROM log WHERE LogCod>=109489685 and LogCod<=109491912 and ActCod=843;

------

ALTER TABLE assignments ADD COLUMN Hidden ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER CrsCod;
DROP INDEX CrsCod ON assignments;
CREATE INDEX CrsCod_Hidden ON assignments (CrsCod,Hidden);

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('964','es','N','Ocultar actividad');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('965','es','N','Mostrar actividad');

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('966','es','N','Ver encuestas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('967','es','N','Editar encuestas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('968','es','N','Crear encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('969','es','N','Eliminar encuesta');

----

CREATE TABLE IF NOT EXISTS surveys (SvyCod INT NOT NULL AUTO_INCREMENT,Name VARCHAR(255) NOT NULL,Description TEXT NOT NULL,IP CHAR(15) NOT NULL,UNIQUE INDEX(SvyCod));

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('970','es','N','Cambiar nombre de encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('971','es','N','Cambiar descripci�n de encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('972','es','N','Cambiar IP de encuesta');

------

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('973','es','N','Solicitar creaci�n encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('974','es','N','Solicitar edici�n encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('975','es','N','Modificar encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('976','es','N','Solicitar elim. encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('977','es','N','Ocultar encuesta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('978','es','N','Mostrar encuesta');

---------------

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('979','es','N','Solicitar creaci�n pregunta encuesta');

UPDATE actions SET Obsolete='Y' WHERE ActCod IN (967,970,971,972);
--------------

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('980','es','N','Enviar pregunta encuesta');

----------------

CREATE TABLE IF NOT EXISTS svy_answers (QstCod INT NOT NULL,AnsInd TINYINT NOT NULL,Answer TEXT NOT NULL,INDEX(QstCod));
CREATE TABLE IF NOT EXISTS svy_questions (QstCod INT NOT NULL AUTO_INCREMENT,CrsCod INT NOT NULL DEFAULT -1,EditTime DATETIME NOT NULL,AnsType ENUM ('unique_choice','multiple_choice') NOT NULL,Stem TEXT NOT NULL,UNIQUE INDEX(QstCod),INDEX(CrsCod));

DROP TABLE svy_questions;
DELETE FROM svy_answers;
CREATE TABLE IF NOT EXISTS svy_questions (QstCod INT NOT NULL AUTO_INCREMENT,SvyCod INT NOT NULL,EditTime DATETIME NOT NULL,AnsType ENUM ('unique_choice','multiple_choice') NOT NULL,Stem TEXT NOT NULL,UNIQUE INDEX(QstCod),INDEX(SvyCod));

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('981','es','N','Eliminar pregunta encuesta');

------

ALTER TABLE svy_questions DROP COLUMN EditTime;

----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('982','es','N','Ver una encuesta');

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('983','es','N','Responder encuesta');

CREATE TABLE IF NOT EXISTS svy_usr_answers (QstCod INT NOT NULL,AnsInd INT NOT NULL DEFAULT -1,UsrCod INT NOT NULL,INDEX(QstCod,AnsInd),INDEX(QstCod,UsrCod));
DROP TABLE svy_usr_answers;
CREATE TABLE IF NOT EXISTS svy_usr_answers (QstCod INT NOT NULL,AnsInd INT NOT NULL,NumUsrs INT NOT NULL DEFAULT 0,UNIQUE INDEX(QstCod,AnsInd));

CREATE TABLE IF NOT EXISTS svy_users (SvyCod INT NOT NULL,UsrCod INT NOT NULL,UNIQUE INDEX(SvyCod,UsrCod));

------

ALTER TABLE svy_questions ADD COLUMN QstInd INT NOT NULL AFTER SvyCod;
ALTER TABLE svy_questions CHANGE COLUMN SvyCod SvyCod INT NOT NULL;

UPDATE svy_questions SET QstInd=0 WHERE QstCod=1;
UPDATE svy_questions SET QstInd=1 WHERE QstCod=2;

ALTER TABLE svy_questions CHANGE COLUMN QstInd QstInd INT NOT NULL DEFAULT 0;

SELECT MAX(QstInd) FROM svy_questions WHERE SvyCod='1';

------

ALTER TABLE surveys DROP COLUMN UsrTypes;
ALTER TABLE surveys ADD COLUMN UsrTypes INT NOT NULL DEFAULT 0 AFTER CrsCod;

ALTER TABLE surveys DROP COLUMN UsrTypes;
ALTER TABLE surveys ADD COLUMN UsrTypes INT NOT NULL DEFAULT 0 AFTER Hidden;

-----

SELECT svy_answers.QstCod AS Q,svy_answers.AnsInd AS A FROM svy_questions,svy_answers WHERE svy_questions.SvyCod='6' AND svy_questions.QstCod=svy_answers.QstCod ORDER BY Q,A;

-----

SHOW INDEX FROM svy_answers;
ALTER TABLE svy_answers ADD COLUMN NumUsrs INT NOT NULL DEFAULT 0 AFTER AnsInd;
DROP INDEX QstCod ON svy_answers;
ALTER TABLE svy_answers ADD UNIQUE INDEX (QstCod,AnsInd);

DROP TABLE svy_usr_answers;

------

ALTER TABLE plugins ADD COLUMN AppKey VARCHAR(16) NOT NULL AFTER Logo;
DESCRIBE plugins;

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('986','es','N','Cambiar clave aplicaci�n plugin');

-----

DELETE FROM actions WHERE ActCod='987';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('987','es','N','Seleccionar una de mis asignaturas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('988','es','N','Cambiar a una de mis asignaturas');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('989','es','N','Ver usuarios conectados');

------

ALTER TABLE notif ADD COLUMN Sent ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER TimeNotif;
DESCRIBE notif;

DROP INDEX ToUsrCod ON notif;
ALTER TABLE notif ADD INDEX (ToUsrCod,Sent);

DROP INDEX TimeNotif ON notif;
ALTER TABLE notif ADD INDEX (TimeNotif,Sent);

DESCRIBE notif;

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('990','es','N','Ver notificaciones');

-----

SELECT LOWER(ShortName) FROM institutions;
UPDATE institutions SET ShortName=LOWER(ShortName);

-----

ALTER TABLE notif ADD COLUMN Seen ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Sent;
DESCRIBE notif;

UPDATE notif SET Seen='Y' WHERE ToUsrCod='1346' AND NotifyEvent='3' AND MsgCod='5000';

-----

DROP INDEX ToUsrCod ON notif;
ALTER TABLE notif ADD INDEX (ToUsrCod,Sent,Seen);

DROP INDEX TimeNotif ON notif;
ALTER TABLE notif ADD INDEX (TimeNotif,Sent,Seen);

-----

ALTER TABLE notif ADD INDEX (NotifyEvent);

DROP INDEX ToUsrCod ON notif;
DROP INDEX TimeNotif ON notif;
ALTER TABLE notif ADD INDEX (ToUsrCod);
ALTER TABLE notif ADD INDEX (TimeNotif);

-----

ALTER TABLE notif CHANGE COLUMN MsgCod Cod INT NOT NULL DEFAULT -1;

------

ALTER TABLE notif DROP COLUMN ForumType;

------

ALTER TABLE marks ADD MrkCod INT NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (MrkCod);

SELECT DISTINCT(forum_post.UsrCod) FROM forum_post,usr_data WHERE forum_post.ThrCod = (SELECT ThrCod FROM forum_post WHERE PstCod='514006') AND forum_post.UsrCod<>'%ld' AND forum_post.UsrCod=usr_data.UsrCod AND ((usr_data.NotifyEvents)<>0);

-----

typedef enum
  {
   Ntf_STATUS_BIT_EMAIL = (1 << 0),	// User want to receive notification by email
   Ntf_STATUS_BIT_SENT	= (1 << 1),	// Email has been sent
   Ntf_STATUS_BIT_SEEN  = (1 << 2),	// User has seen the event which caused the notification
  } Ntf_Status_Bits_t;

typedef enum
  {
   Ntf_STATUS_NO_EMAIL,		// --0 !(Status & Ntf_STATUS_BIT_EMAIL)
   Ntf_STATUS_EMAIL_PENDING,	// 001  (Status & Ntf_STATUS_BIT_EMAIL) && !(Status & Ntf_STATUS_BIT_SENT) && !(Status & Ntf_STATUS_BIT_SEEN)
   Ntf_STATUS_EMAIL_CANCELLED,	// 101  (Status & Ntf_STATUS_BIT_EMAIL) && !(Status & Ntf_STATUS_BIT_SENT) &&  (Status & Ntf_STATUS_BIT_SEEN)
   Ntf_STATUS_EMAIL_SENT,	// -11  (Status & Ntf_STATUS_BIT_EMAIL) &&  (Status & Ntf_STATUS_BIT_SENT)
  } Ntf_Status_t;

ALTER TABLE notif ADD COLUMN Status TINYINT NOT NULL DEFAULT 0 AFTER Seen;

UPDATE notif SET Status=1 WHERE Sent='N' AND Seen='N';
UPDATE notif SET Status=5 WHERE Sent='N' AND Seen='Y';
UPDATE notif SET Status=3 WHERE Sent='Y' AND Seen='N';
UPDATE notif SET Status=7 WHERE Sent='Y' AND Seen='Y';

ALTER TABLE notif DROP COLUMN Sent;
ALTER TABLE notif DROP COLUMN Seen;

-----

DROP TABLE msg_new;

-----

ALTER TABLE usr_data ADD COLUMN NumNewNtfs INT NOT NULL AFTER Comments;


-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('991','es','N','Ver notificaciones nuevas');

-----

ALTER TABLE usr_data ADD COLUMN NumUnseenNtfs INT NOT NULL AFTER Comments;

SELECT COUNT(*) FROM notif WHERE ToUsrCod='1346' AND (Status&4)=0;

DROP TABLE IF EXISTS notif_tmp;
CREATE TEMPORARY TABLE notif_tmp (ToUsrCod INT NOT NULL,NumUnseenNtfs INT NOT NULL,UNIQUE INDEX (ToUsrCod)) SELECT ToUsrCod,COUNT(*) AS NumUnseenNtfs FROM notif WHERE (Status&4)=0 GROUP BY ToUsrCod;
UPDATE usr_data,notif_tmp SET usr_data.NumUnseenNtfs=notif_tmp.NumUnseenNtfs WHERE usr_data.UsrCod=notif_tmp.ToUsrCod;

-----

ALTER TABLE usr_data DROP COLUMN NumUnseenNtfs;

-----

ALTER TABLE crs_usr ADD COLUMN Role TINYINT NOT NULL DEFAULT 0 AFTER UsrCod;
UPDATE crs_usr,usr_data SET crs_usr.Role=usr_data.UsrType WHERE crs_usr.UsrCod=usr_data.UsrCod;

------

ALTER TABLE usr_data CHANGE COLUMN UsrType Role TINYINT NOT NULL DEFAULT 0;
ALTER TABLE connected CHANGE COLUMN UsrType Role TINYINT NOT NULL DEFAULT 0;
ALTER TABLE sessions CHANGE COLUMN UsrType Role TINYINT NOT NULL DEFAULT 0;


ALTER TABLE imported_sessions CHANGE COLUMN ImportedUsrType ImportedRole TINYINT NOT NULL DEFAULT 0;

ALTER TABLE surveys CHANGE COLUMN UsrTypes Roles INT NOT NULL DEFAULT 0;

UPDATE crs_usr,usr_data SET crs_usr.Role=usr_data.Role WHERE crs_usr.UsrCod=usr_data.UsrCod;

-------

ALTER TABLE usr_data ADD COLUMN Roles INT NOT NULL DEFAULT 0 AFTER Role;
UPDATE usr_data SET Roles=4 WHERE Role=2;
UPDATE usr_data SET Roles=8 WHERE Role=3;

ALTER TABLE usr_data ADD INDEX (Roles);


A ejecutar justo antes de que la funcionalidad de rol por asignatura se ponga en marcha:
UPDATE usr_data SET Roles=4 WHERE Role=2;
UPDATE usr_data SET Roles=8 WHERE Role=3;
UPDATE crs_usr,usr_data SET crs_usr.Role=usr_data.Role WHERE crs_usr.UsrCod=usr_data.UsrCod;

-------

ALTER TABLE connected CHANGE COLUMN Role RoleInLastCrs TINYINT NOT NULL DEFAULT 0;
ALTER TABLE connected CHANGE COLUMN CrsCod LastCrsCod INT NOT NULL DEFAULT -1;

------

ALTER TABLE usr_data CHANGE COLUMN Gender Sex ENUM ('unknown','female','male') NOT NULL DEFAULT 'unknown';
ALTER TABLE imported_students CHANGE COLUMN Gender Sex ENUM ('unknown','female','male') NOT NULL DEFAULT 'unknown';
ALTER TABLE sta_degrees CHANGE COLUMN Gender Sex ENUM ('unknown','female','male','all') NOT NULL DEFAULT 'all';

-----

ALTER TABLE usr_data DROP COLUMN Role;
OPTIMIZE TABLE usr_data;

-----

ALTER TABLE log_recent CHANGE COLUMN UsrType Role TINYINT NOT NULL;
ALTER TABLE log CHANGE COLUMN UsrType Role TINYINT NOT NULL;

-----

CREATE TABLE teachers_2011_03_08 SELECT * FROM usr_data WHERE (Roles&(1<<3))<>0;

------

ALTER TABLE crs_usr ADD INDEX (Role);
ALTER TABLE crs_usr ADD INDEX (Accepted);

CREATE INDEX CrsCod_Rol ON crs_usr (CrsCod,Role);
CREATE INDEX UsrCod_Rol ON crs_usr (UsrCod,Role);

DROP INDEX CrsCod ON crs_usr;
DROP INDEX UsrCod ON crs_usr;
OPTIMIZE TABLE crs_usr;

ALTER TABLE crs_usr ADD INDEX (CrsCod);
ALTER TABLE crs_usr ADD INDEX (UsrCod);

CREATE INDEX CrsCod_UsrCod_Rol ON crs_usr (CrsCod,UsrCod,Role);

ALTER TABLE usr_data DROP COLUMN Roles;

-----

Averiguar las incoherencias en el n�mero de estudiantes en grupos:

Ver qu� gruposm tienen un n�mero de estudiantes err�neo:
SELECT crs_usr.CrsCod,crs_grp.NumStudents AS A,COUNT(crs_grp_usr.UsrCod) AS B FROM crs_grp_usr,crs_grp,crs_grp_types,crs_usr WHERE crs_grp_usr.GrpCod=crs_grp.GrpCod AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod AND crs_grp_types.CrsCod=crs_usr.CrsCod AND crs_grp_usr.UsrCod=crs_usr.UsrCod AND crs_usr.Role='2' GROUP BY crs_grp_usr.GrpCod HAVING A<>B;

Arreglar el n�mero de estudiantes:
UPDATE crs_grp SET NumStudents='X' WHERE GrpCod='Y';

Contar el n�mero de estudiantes de un grupo:
SELECT COUNT(*) FROM crs_grp_usr,crs_grp,crs_grp_types,crs_usr WHERE crs_grp_usr.GrpCod='5434' AND crs_grp_usr.GrpCod=crs_grp.GrpCod AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod AND crs_grp_types.CrsCod=crs_usr.CrsCod AND crs_grp_usr.UsrCod=crs_usr.UsrCod AND crs_usr.Role='2';

-----

Counting number of students: (slower?)

SELECT COUNT(DISTINCT GrpTypCod) FROM (SELECT crs_grp_types.GrpTypCod AS GrpTypCod,COUNT(*) AS NumStudents,crs_grp.MaxStudents as MaxStudents FROM crs_grp_types,crs_grp,crs_grp_usr,crs_usr WHERE crs_grp_types.CrsCod='422' AND crs_grp_types.Mandatory='Y' AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod AND crs_grp.Open='Y' AND crs_grp_types.CrsCod=crs_usr.CrsCod AND crs_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod=crs_usr.UsrCod AND crs_usr.Role='2' GROUP BY crs_grp.GrpCod HAVING NumStudents<MaxStudents) AS grp_types_open_not_full WHERE GrpTypCod NOT IN (SELECT DISTINCT crs_grp_types.GrpTypCod FROM crs_grp_types,crs_grp,crs_grp_usr WHERE crs_grp_types.CrsCod='422' AND crs_grp_types.Mandatory='Y' AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod AND crs_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod='29759');

SELECT crs_grp_types.GrpTypCod AS GrpTypCod,COUNT(*) AS NumStudents,crs_grp.MaxStudents as MaxStudents FROM crs_grp_types,crs_grp,crs_grp_usr,crs_usr WHERE crs_grp_types.CrsCod='422' AND crs_grp_types.Mandatory='Y' AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod AND crs_grp.Open='Y' AND crs_grp_types.CrsCod=crs_usr.CrsCod AND crs_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod=crs_usr.UsrCod AND crs_usr.Role='2' GROUP BY crs_grp.GrpCod HAVING NumStudents<MaxStudents;

Getting number of students:

SELECT COUNT(DISTINCT crs_grp_types.GrpTypCod) FROM crs_grp_types,crs_grp WHERE crs_grp_types.CrsCod='422' AND crs_grp_types.Mandatory='Y' AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod AND crs_grp.Open='Y' AND crs_grp.NumStudents<crs_grp.MaxStudents AND crs_grp_types.GrpTypCod NOT IN (SELECT DISTINCT crs_grp_types.GrpTypCod FROM crs_grp_types,crs_grp,crs_grp_usr WHERE crs_grp_types.CrsCod='422' AND crs_grp_types.Mandatory='Y' AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod AND crs_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod='29759');


SELECT COUNT(DISTINCT GrpTypCod) FROM (SELECT crs_grp_types.GrpTypCod AS GrpTypCod,COUNT(*) AS NumStudents,crs_grp.MaxStudents as MaxStudents FROM crs_grp_types,crs_grp,crs_grp_usr,crs_usr WHERE crs_grp_types.GrpTypCod='2881' AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod AND crs_grp_types.CrsCod=crs_usr.CrsCod AND crs_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod=crs_usr.UsrCod AND crs_usr.Role='2' GROUP BY crs_grp.GrpCod HAVING NumStudents<MaxStudents) AS available_grp_types;

-----

UPDATE actions SET Obsolete='Y' WHERE ActCod IN ('438');

-----

ALTER TABLE crs_grp DROP COLUMN NumStudents;

-----

SELECT DISTINCT(degrees.DegCod) FROM crs_usr,courses,degrees WHERE crs_usr.UsrCod='%ld' AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod AND degrees.InsCod='%ld' ORDER BY degrees.ShortName


SELECT DISTINCT(degrees.DegCod),MAX(crs_usr.Role) FROM crs_usr,courses,degrees WHERE crs_usr.UsrCod='1346' AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod ORDER BY degrees.ShortName;

SELECT institutions.InsCod,MAX(crs_usr.Role) FROM crs_usr,courses,degrees,institutions WHERE crs_usr.UsrCod='1346' AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod AND degrees.InsCod=institutions.InsCod GROUP BY institutions.InsCod ORDER BY institutions.ShortName

------

SELECT MsgCod FROM msg_content WHERE MsgCod IN (SELECT DISTINCT msg_snt.MsgCod FROM msg_snt,msg_rcv,usr_data WHERE msg_snt.UsrCod='1346' AND msg_snt.MsgCod=msg_rcv.MsgCod AND msg_rcv.UsrCod=usr_data.UsrCod AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '%pepe%' ORDER BY msg_snt.MsgCod DESC) AND MATCH (Subject,Content) AGAINST ('Esto es c�mo tuenti?' IN BOOLEAN MODE);

SELECT MsgCod FROM msg_content WHERE MsgCod IN (SELECT DISTINCT msg_snt.MsgCod FROM msg_snt,msg_rcv,usr_data WHERE msg_snt.UsrCod='1346' AND msg_snt.MsgCod=msg_rcv.MsgCod AND msg_rcv.UsrCod=usr_data.UsrCod AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '%pepe%' ORDER BY msg_snt.MsgCod DESC) AND (Subject LIKE '%Esto es c�mo tuenti?%' OR Content LIKE '%Esto es c�mo tuenti?%');

-----

DROP TABLE IF EXISTS T1;
CREATE TEMPORARY TABLE T1 ENGINE=MEMORY SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS Domain,COUNT(*) as N FROM usr_data GROUP BY Domain;
CREATE TEMPORARY TABLE T2 ENGINE=MEMORY SELECT * FROM T1;
(SELECT mail_domains.MaiCod,mail_domains.Domain AS Domain,mail_domains.Info AS Info,T1.N FROM mail_domains,T1 WHERE mail_domains.Domain=T1.Domain) UNION (SELECT MaiCod,Domain,Info,0 FROM mail_domains WHERE Domain NOT IN (SELECT Domain FROM T2)) ORDER BY N DESC,Info,Domain;
DROP TABLE IF EXISTS T1,T2;

DROP TABLE IF EXISTS T3;
CREATE TEMPORARY TABLE T3 ENGINE=MEMORY SELECT DISTINCT msg_snt.MsgCod FROM msg_snt,msg_rcv,usr_data WHERE msg_snt.UsrCod='1346' AND msg_snt.MsgCod=msg_rcv.MsgCod AND msg_rcv.UsrCod=usr_data.UsrCod AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '%pepe%' ORDER BY msg_snt.MsgCod DESC;
SELECT msg_content.MsgCod FROM msg_content,(SELECT DISTINCT msg_snt.MsgCod FROM msg_snt,msg_rcv,usr_data WHERE msg_snt.UsrCod='1346' AND msg_snt.MsgCod=msg_rcv.MsgCod AND msg_rcv.UsrCod=usr_data.UsrCod AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '%pepe%' ORDER BY msg_snt.MsgCod DESC) AS T2 WHERE message_content.MsgCod=T2.MsgCod AND MATCH (msg_content.Subject,msg_content.Content) AGAINST ('Esto es c�mo tuenti?');
DROP TABLE IF EXISTS T3;

SELECT msg_content.MsgCod FROM msg_content,(SELECT DISTINCT msg_snt.MsgCod FROM msg_snt,msg_rcv,usr_data WHERE msg_snt.UsrCod='1346' AND msg_snt.MsgCod=msg_rcv.MsgCod AND msg_rcv.UsrCod=usr_data.UsrCod AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '%pepe%' ORDER BY msg_snt.MsgCod DESC) AS T2 WHERE msg_content.MsgCod=T2.MsgCod AND MATCH (msg_content.Subject,msg_content.Content) AGAINST ('Esto es c�mo tuenti?');


DROP TABLE IF EXISTS T3;
CREATE TEMPORARY TABLE T3 ENGINE=MEMORY SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt,usr_data WHERE msg_rcv.UsrCod='1346' AND msg_rcv.Open='N' AND msg_rcv.MsgCod=msg_snt.MsgCod AND msg_snt.UsrCod=usr_data.UsrCod AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '%j%';
SELECT MsgCod FROM msg_content WHERE MsgCod IN (SELECT MsgCod FROM T3) AND MATCH (Subject,Content) AGAINST ('Esto es c�mo tuenti?');
DROP TABLE IF EXISTS T3;

SELECT MsgCod FROM msg_content WHERE MsgCod IN (SELECT DISTINCT msg_snt.MsgCod FROM msg_snt,msg_rcv,usr_data WHERE msg_snt.UsrCod='1346' AND msg_snt.MsgCod=msg_rcv.MsgCod AND msg_rcv.UsrCod=usr_data.UsrCod AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '%j%' ORDER BY msg_snt.MsgCod DESC) HAVING MATCH (Subject) AGAINST ('eres un poco como yo');

-----

CREATE TABLE msg_subject SELECT MsgCod,Subject FROM msg_content;
ALTER TABLE msg_subject ADD UNIQUE INDEX (MsgCod);

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('992','es','N','Solicitar cambio idioma');

ALTER TABLE IP_prefs DROP COLUMN Language;


------

ALTER TABLE countries ADD COLUMN Name_pl VARCHAR(255) NOT NULL AFTER Name_it;
CREATE INDEX Name_pl ON countries (Name_pl);
UPDATE countries SET Name_pl=Name_en;

-----

DROP TABLE IF EXISTS swad_students_2010_2011;
CREATE TABLE swad_students_2010_2011 SELECT DISTINCT DNI FROM log WHERE ClickTime>='20100927' AND Role=2 ORDER BY DNI;
CREATE UNIQUE INDEX DNI ON swad_students_2010_2011(DNI);

mysqldump -l -h swad.ugr.es -u swad -p swad swad_students_2010_2011 > swad_students_2010_2011.sql

-----

DROP TABLE IF EXISTS swad_teachers_2010_2011;
CREATE TABLE swad_teachers_2010_2011 SELECT DISTINCT DNI FROM log WHERE ClickTime>='20100927' AND Role=3 ORDER BY DNI;
CREATE UNIQUE INDEX DNI ON swad_teachers_2010_2011(DNI);

mysqldump -l -h swad.ugr.es -u swad -p swad swad_teachers_2010_2011 > swad_teachers_2010_2011.sql

-----

ALTER TABLE tst_question_tags CHANGE COLUMN IndDes TagInd TINYINT NOT NULL;

DROP INDEX CrsCod ON tst_questions;
CREATE INDEX CrsCod_EditTime ON tst_questions (CrsCod,EditTime);

ALTER TABLE tst_tags ADD COLUMN ChangeTime DATETIME NOT NULL AFTER CrsCod;
DROP INDEX CrsCod ON tst_tags;
CREATE INDEX CrsCod_ChangeTime ON tst_tags (CrsCod,ChangeTime);

UPDATE tst_tags SET ChangeTime=NOW();

-----

SELECT QstCod,AnsType,Shuffle,Stem FROM tst_questions WHERE CrsCod='421' AND (UNIX_TIMESTAMP(EditTime)>='1304270888' OR EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='421' AND UNIX_TIMESTAMP(ChangeTime)>='1304270888' AND tst_tags.TagHidden='N')) AND NOT EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='421' AND tst_tags.TagHidden='Y') ORDER BY QstCod;

SELECT QstCod,AnsInd,Answer,Correct FROM tst_answers WHERE QstCod IN (SELECT QstCod FROM tst_questions WHERE CrsCod='421' AND (UNIX_TIMESTAMP(EditTime)>='1304270888' OR EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='421' AND UNIX_TIMESTAMP(ChangeTime)>='1304270888' AND tst_tags.TagHidden='N')) AND NOT EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='421' AND tst_tags.TagHidden='Y')) ORDER BY QstCod,AnsInd;

SELECT QstCod,TagCod,TagInd FROM tst_question_tags WHERE QstCod IN (SELECT QstCod FROM tst_questions WHERE CrsCod='421' AND (UNIX_TIMESTAMP(EditTime)>='1304270888' OR EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='421' AND UNIX_TIMESTAMP(ChangeTime)>='1304270888' AND tst_tags.TagHidden='N')) AND NOT EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='421' AND tst_tags.TagHidden='Y')) ORDER BY QstCod,TagInd;

-----

ALTER TABLE tst_config ADD COLUMN Pluggable ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER CrsCod;

-----

ALTER TABLE usr_data ADD COLUMN LastAccNotif DATETIME NOT NULL AFTER LastTime;
UPDATE usr_data SET LastAccNotif=LastTime;
ALTER TABLE usr_data DROP COLUMN NumNewNtfs;

-----

ALTER TABLE countries ADD COLUMN Name_ca VARCHAR(255) NOT NULL AFTER Alpha2;
CREATE INDEX Name_ca ON countries (Name_ca);
UPDATE countries SET Name_ca=Name_es;

-----


ALTER TABLE usr_data ADD COLUMN NotifyEventsOld INT NOT NULL DEFAULT 0 AFTER NotifyEvents;
UPDATE usr_data SET NotifyEventsOld=NotifyEvents;

ALTER TABLE notif ADD COLUMN NotifyEventOld TINYINT NOT NULL AFTER NotifyEvent;
UPDATE notif SET NotifyEventOld=NotifyEvent;


UPDATE usr_data SET NotifyEvents=(NotifyEventsOld<<2);

UPDATE notif SET NotifyEvent=6 WHERE NotifyEventOld=4;
UPDATE notif SET NotifyEvent=5 WHERE NotifyEventOld=3;
UPDATE notif SET NotifyEvent=4 WHERE NotifyEventOld=2;
UPDATE notif SET NotifyEvent=3 WHERE NotifyEventOld=1;
UPDATE notif SET NotifyEvent=2 WHERE NotifyEventOld=0;


ALTER TABLE sta_notif ADD COLUMN NotifyEventOld TINYINT NOT NULL AFTER NotifyEvent;
UPDATE sta_notif SET NotifyEventOld=NotifyEvent;

UPDATE sta_notif SET NotifyEvent=6 WHERE NotifyEventOld=4;
UPDATE sta_notif SET NotifyEvent=5 WHERE NotifyEventOld=3;
UPDATE sta_notif SET NotifyEvent=4 WHERE NotifyEventOld=2;
UPDATE sta_notif SET NotifyEvent=3 WHERE NotifyEventOld=1;
UPDATE sta_notif SET NotifyEvent=2 WHERE NotifyEventOld=0;

-----

-----

(SELECT crs_usr.UsrCod FROM assignments,crs_usr WHERE assignments.AsgCod='595' AND NOT EXISTS (SELECT * FROM asg_grp WHERE AsgCod='595') AND assignments.CrsCod=crs_usr.CrsCod AND crs_usr.UsrCod<>'1346') UNION (SELECT DISTINCT crs_grp_usr.UsrCod FROM asg_grp,crs_grp_usr WHERE asg_grp.AsgCod='595' AND asg_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod<>'1346');

ALTER TABLE assignments ADD COLUMN NumNotif INT NOT NULL DEFAULT 0 AFTER Hidden;
ALTER TABLE surveys ADD COLUMN NumNotif INT NOT NULL DEFAULT 0 AFTER Hidden;

(SELECT crs_usr.UsrCod FROM surveys,crs_usr WHERE surveys.SvyCod='66' AND NOT EXISTS (SELECT * FROM svy_grp WHERE SvyCod='66') AND surveys.CrsCod=crs_usr.CrsCod AND crs_usr.UsrCod<>'1346' AND (surveys.Roles&(1<<crs_usr.Role))<>0)
                        " UNION " \
(SELECT DISTINCT crs_grp_usr.UsrCod FROM svy_grp,crs_grp_usr,surveys,crs_usr WHERE svy_grp.SvyCod='595' AND svy_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod=crs_usr.UsrCod AND crs_grp_usr.UsrCod<>'1346' AND svy_grp.SvyCod=surveys.SvyCod AND surveys.CrsCod=crs_usr.CrsCod AND (surveys.Roles&(1<<crs_usr.Role))<>0);

-----

ALTER TABLE usr_data CHANGE COLUMN Skype Skype VARCHAR(60) NOT NULL;

ALTER TABLE usr_data ADD COLUMN Twitter VARCHAR(20) NOT NULL AFTER WWW;

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('993','es','N','Ver acciones frecuentes');

------

SELECT COUNT(*),SUM(NumHits),SUM(Score) FROM tst_questions WHERE CrsCod>'0';

SELECT COUNT(tst_config.CrsCod) FROM tst_config,tst_questions WHERE tst_config.pluggable='Y' AND tst_config.CrsCod=tst_questions.CrsCod;

------

Por hacer:

ALTER TABLE crs_grp DROP COLUMN NumStudents;
ALTER TABLE usr_data DROP COLUMN NotifyEventsOld;
ALTER TABLE notif DROP COLUMN NotifyEventOld;
ALTER TABLE sta_notif DROP COLUMN NotifyEventOld;

------

awk '/Table structure for table .timetable_crs./,/Table structure for table .timetable_tut./{print}' swad_2011-07-01_05h00m.Friday.sql > swad_2011-07-01_05h00m.Friday_timetable_crs.sql
Eliminar del nuevo fichero sql todo menos la orden INSERT...
mysql -u swad -p swad < swad_2011-07-01_05h00m.Friday_timetable_crs.sql

------

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('994','es','N','Refrescar �ltimos clics');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('995','es','N','Ver �ltimos clics');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('996','es','N','Hacer p�bl. arch. doc. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('997','es','N','Hacer priv. arch. doc. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('998','es','N','Hacer p�bl. arch. doc. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('999','es','N','Hacer priv. arch. doc. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1000','es','N','Hacer p�bl. arch. com. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1001','es','N','Hacer priv. arch. com. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1002','es','N','Hacer p�bl. arch. com. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1003','es','N','Hacer priv. arch. com. grp.');

-----

DROP TABLE debug;
CREATE TABLE IF NOT EXISTS debug (Txt TEXT NOT NULL);

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1004','es','N','Solicitar edici�n de tipos de grupo');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1005','es','N','Importar preguntas de test');

-----

ALTER TABLE deg_types ADD COLUMN AllowDirectLogIn ENUM('N','Y') NOT NULL DEFAULT 'Y' AFTER DegTypName;

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1006','es','N','Cambiar permiso de identificaci�n directa');

-----

UPDATE msg_rcv SET UsrCod='87625' WHERE UsrCod='94808';

UPDATE msg_snt SET UsrCod='87625' WHERE UsrCod='94808';

------

ALTER TABLE imported_groups ADD COLUMN ExternalCrsCod CHAR(7) NOT NULL AFTER SessionId;

------

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1007','es','N','Solicitar importaci�n preguntas test');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1008','es','N','Importar preguntas test');

-----

BACKUP TABLE tst_questions TO '/var/lib/mysql/swad-copia-tst-questions';
ALTER TABLE tst_questions CHANGE AnsType AnsType ENUM ('T/F','true_false','unique_choice','multiple_choice','text','int','float') NOT NULL;
UPDATE tst_questions SET AnsType='true_false' WHERE AnsType='T/F';
ALTER TABLE tst_questions CHANGE AnsType AnsType ENUM ('true_false','unique_choice','multiple_choice','text','int','float') NOT NULL;

-----
AWK: extraer un fragmento de texto

Con AWK podemos buscar una parte de un texto la cual empiece por una cadena y termine por otra.

cat /etc/passwd | awk '/100/,/111/ { print } '

-----------------------

Restaurar una tabla desde la copia de seguridad:

cd /var/www/backup/daily/swad

bunzip2 swad_2011-10-26_05h00m.Wednesday.sql.bz2

awk '/Table structure for table `assignments`/,/Table structure for table `centres`/{print}' swad_2011-10-26_05h00m.Wednesday.sql > swad_2011-10-26_05h00m.Wednesday_assignments.sql
                                     ^                                          ^
                                tabla a buscar                           tabla siguiente

Eliminar del nuevo fichero sql todo menos la orden INSERT...
mysql -u swad -p swad < swad_2011-10-26_05h00m.Wednesday_assignments.sql

----

awk '/Table structure for table `notices`/,/Table structure for table `notif`/{print}' swad_2011-10-26_05h00m.Wednesday.sql > swad_2011-10-26_05h00m.Wednesday_notices.sql

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1009','es','N','Ver asignaturas');


REPLACE INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1009','es','N','Ver asignaturas de titulaci�n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1010','es','N','Ir a asignatura en titulaci�n');

------

DROP TABLE actions_MFU;
CREATE TABLE IF NOT EXISTS actions_MFU (UsrCod INT NOT NULL,ActCod INT NOT NULL,Score FLOAT NOT NULL,LastClick DATETIME NOT NULL,UNIQUE INDEX(UsrCod,ActCod));

-----

SELECT Score FROM actions_MFU WHERE UsrCod='1346' AND ActCod='15';

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1011','es','N','Ver titulaciones');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1012','es','N','Ir a titulaci�n de un tipo');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1013','es','N','Ver tipos de titulaci�n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1014','es','N','Ir a tipo de titulaci�n');

-----

ALTER TABLE courses ADD COLUMN ExternalCrsCod CHAR(7) NOT NULL AFTER Semester;
ALTER TABLE courses ADD COLUMN InsCrsCod CHAR(7) NOT NULL AFTER Semester;
ALTER TABLE courses DROP COLUMN ExternalCrsCod;

-----

DROP TABLE log_ws;
CREATE TABLE IF NOT EXISTS log_ws (LogCod INT NOT NULL,FunCod INT NOT NULL,AppCod INT NOT NULL,UNIQUE INDEX(LogCod),INDEX(FunCod),INDEX(AppCod));
DROP TABLE log_ws;
CREATE TABLE IF NOT EXISTS log_ws (LogCod INT NOT NULL,PlgCod INT NOT NULL,FunCod INT NOT NULL,UNIQUE INDEX(LogCod),INDEX(PlgCod),INDEX(FunCod));

ALTER TABLE ws_keys ADD COLUMN PlgCod INT NOT NULL AFTER UsrCod;

-----
mysql> DESCRIBE tst_config;
+---------------------+--------------------------------------------------------------+------+-----+---------+-------+
| Field               | Type                                                         | Null | Key | Default | Extra |
+---------------------+--------------------------------------------------------------+------+-----+---------+-------+
| CrsCod              | int(11)                                                      | NO   | PRI | -1      |       |
| Pluggable           | enum('N','Y')                                                | NO   |     | N       |       |
| Min                 | int(11)                                                      | NO   |     | NULL    |       |
| Def                 | int(11)                                                      | NO   |     | NULL    |       |
| Max                 | int(11)                                                      | NO   |     | NULL    |       |
| MinTimeNxtTstPerQst | int(11)                                                      | NO   |     | 0       |       |
| Feedback            | enum('nothing','total_result','each_result','each_good_bad') | NO   |     | NULL    |       |
+---------------------+--------------------------------------------------------------+------+-----+---------+-------+
7 rows in set (0.00 sec)


ALTER TABLE tst_config CHANGE Pluggable Pluggable ENUM ('unknown','Y','N') DEFAULT 'unknown' NOT NULL;

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1015','es','N','Bloquear remitente');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1016','es','N','Desbloquear remitente');

-----

CREATE TABLE IF NOT EXISTS msg_banned (FromUsrCod INT NOT NULL,ToUsrCod INT NOT NULL,UNIQUE INDEX(FromUsrCod,ToUsrCod));

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1017','es','N','Ver usuarios bloqueados');

----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1018','es','N','Quitar remitente de bloqueados');

SELECT msg_banned.FromUsrCod FROM msg_banned,usr_data WHERE msg_banned.ToUsrCod='1346' AND msg_banned.FromUsrCod=usr_data.UsrCod ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName;

-----

ALTER TABLE tst_tags DROP COLUMN ChangeTime;

BACKUP TABLE tst_tags TO '/home/acanas/';
BACKUP TABLE tst_questions TO '/home/acanas/';
BACKUP TABLE tst_question_tags TO '/home/acanas/';

DROP TABLE IF EXISTS tst_question_tags_tmp;
CREATE TEMPORARY TABLE tst_question_tags_tmp ENGINE=MEMORY SELECT QstCod FROM tst_question_tags WHERE TagCod='1912';
DELETE FROM tst_question_tags WHERE TagCod='1913' AND QstCod IN (SELECT QstCod FROM tst_question_tags_tmp);
UPDATE tst_question_tags SET TagCod='1912' WHERE TagCod='1913' AND QstCod NOT IN (SELECT QstCod FROM tst_question_tags_tmp);
DROP TABLE IF EXISTS tst_question_tags_tmp;
DELETE FROM tst_tags WHERE TagCod='1913';

----

SELECT UsrCod,Acc FROM (SELECT DISTINCT crs_usr.UsrCod AS UsrCod,'N' AS Acc FROM courses,crs_usr WHERE courses.DegCod='356' AND courses.CrsCod=crs_usr.CrsCod AND crs_usr.Role='2' AND crs_usr.Accepted='N' UNION SELECT DISTINCT crs_usr.UsrCod AS UsrCod,'Y' AS Acc FROM courses,crs_usr WHERE courses.DegCod='356' AND courses.CrsCod=crs_usr.CrsCod AND crs_usr.Role='2' AND crs_usr.UsrCod NOT IN (SELECT DISTINCT crs_usr.UsrCod FROM courses,crs_usr WHERE courses.DegCod='356' AND courses.CrsCod=crs_usr.CrsCod AND crs_usr.Role='2' AND crs_usr.Accepted='N')) AS list_stds,usr_data WHERE list_stds.UsrCod=usr_data.UsrCod ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.DNI;

-----------------------

Restaurar una tabla desde la copia de seguridad:

cd /var/www/backup/daily/swad

bunzip2 swad_2011-12-06_05h00m.Tuesday.sql.bz2

awk '/Table structure for table `crs_grp_usr`/,/Table structure for table `crs_info_read`/{print}' swad_2011-12-06_05h00m.Tuesday.sql > swad_2011-12-06_05h00m.Tuesday_crs_grp_usr.sql
                                     ^                                          ^
                                tabla a buscar                           tabla siguiente

En el nuevo fichero sql cambiar crs_grp_usr por crs_grp_usr_borrame
mysql -u swad -p swad < swad_2011-12-06_05h00m.Tuesday_crs_grp_usr.sql

DELETE FROM crs_grp_usr WHERE GrpCod IN (SELECT GrpCod FROM crs_grp WHERE GrpTypCod IN (SELECT GrpTypCod FROM crs_grp_types WHERE CrsCod='1032'));
INSERT INTO crs_grp_usr (GrpCod,UsrCod,LastAccDownloadGrp,LastAccCommonGrp,LastAccMarksGrp) SELECT * FROM crs_grp_usr_borrame WHERE GrpCod IN (SELECT GrpCod FROM crs_grp WHERE GrpTypCod IN (SELECT GrpTypCod FROM crs_grp_types WHERE CrsCod='1032'));

-----

SELECT QstCod,AnsType,Shuffle,Stem FROM tst_questions WHERE CrsCod='5107' AND (UNIX_TIMESTAMP(EditTime)>='1300000000' OR EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='5107' AND UNIX_TIMESTAMP(ChangeTime)>='1300000000' AND tst_tags.TagHidden='N')) AND NOT EXISTS (SELECT * FROM tst_question_tags,tst_tags WHERE tst_question_tags.QstCod=tst_questions.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='5107' AND tst_tags.TagHidden='Y') ORDER BY QstCod;

ALTER TABLE tst_tags ADD COLUMN ChangeTime DATETIME NOT NULL AFTER CrsCod;
DROP INDEX CrsCod_2 ON tst_tags;
DROP INDEX CrsCod_ChangeTime ON tst_tags;
CREATE INDEX CrsCod_ChangeTime ON tst_tags (CrsCod,ChangeTime);

UPDATE tst_tags SET ChangeTime=NOW();

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1019','es','N','Ocultar mensaje recibido');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1020','es','N','Ocultar mensaje enviado');


-----

DROP INDEX DstDNI_MsgCod ON msg_rcv;
DROP INDEX UsrCod ON msg_rcv;
DROP INDEX UsrCod_2 ON msg_rcv_deleted;
DROP INDEX CreatTime ON msg_snt;
DROP INDEX CreatTime ON msg_snt_deleted;

-----

SELECT MsgCod FROM msg_content WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt) AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv);

INSERT IGNORE INTO msg_content_deleted (MsgCod,Subject,Content) SELECT MsgCod,Subject,Content FROM msg_content WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt) AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv);
DELETE FROM msg_content WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt) AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv);

-----

ALTER TABLE usr_data DROP COLUMN NumNewMsgs;

-----

SELECT MsgCod FROM msg_rcv WHERE UsrCod='86738' ORDER BY MsgCod DESC;

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1021','es','N','Ver mi c�digo QR');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1022','es','N','Imprimir mi c�digo QR');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1023','es','N','Configurar asignatura');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1024','es','N','Cambiar m�todo autenticaci�n asignatura');

UPDATE actions SET Txt='Cambiar m�todo autenticaci�n tipo titulaci�n' WHERE ActCod='1006';

ALTER TABLE courses ADD COLUMN AllowDirectLogIn ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER InsCrsCod;

-----

DELETE LOW_PRIORITY FROM log WHERE ClickTime>='20120202004800' AND ClickTime<='20120202011700' AND (ActCod='194' OR ActCod='843');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1025','es','N','Cambiar c�d. institucional asignatura');
UPDATE actions SET Txt='Cambiar configuraci�n asignatura' WHERE ActCod='1024';
UPDATE actions SET Txt='Ver documentos' WHERE ActCod='0';
UPDATE actions SET Txt='Administrar documentos' WHERE ActCod='12';
UPDATE actions SET Txt='Ver documentos asignatura' WHERE ActCod='463';
UPDATE actions SET Txt='Ver documentos grupo' WHERE ActCod='466';
UPDATE actions SET Txt='Administrar documentos grupo' WHERE ActCod='467';
UPDATE actions SET Txt='Administrar documentos asignatura' WHERE ActCod='475';

-----

SELECT Twitter FROM usr_data WHERE Twitter<>'' AND Twitter NOT LIKE '@%';
SELECT Twitter FROM usr_data WHERE Twitter<>'' AND Twitter LIKE '-%';
SELECT CONCAT('@',Twitter) FROM usr_data WHERE Twitter<>'' AND Twitter NOT LIKE '@%';

UPDATE usr_data SET Twitter='' WHERE Twitter<>'' AND Twitter LIKE '-%';
UPDATE usr_data SET Twitter=CONCAT('@',Twitter) WHERE Twitter<>'' AND Twitter NOT LIKE '@%';

------

UPDATE usr_data SET Nickname=CONCAT('@',Nickname) WHERE Nickname<>'' AND Nickname NOT LIKE '@%';

-----

SELECT Twitter,SUBSTRING(Twitter,2) FROM usr_data WHERE Twitter<>'';
ALTER TABLE usr_data ADD COLUMN Twitter2 VARCHAR(15) NOT NULL AFTER Twitter;

UPDATE usr_data SET Twitter2=SUBSTRING(Twitter,2) WHERE Twitter<>'';
ALTER TABLE usr_data DROP COLUMN Twitter;
ALTER TABLE usr_data CHANGE Twitter2 Twitter VARCHAR(15) NOT NULL;

-----

DROP TABLE debug;
CREATE TABLE IF NOT EXISTS debug (DebugTime DATETIME NOT NULL,Txt TEXT NOT NULL);

-----

mysqldump -l -h swad.ugr.es -u swad -p swad actions > actions.sql

-----

UPDATE usr_data SET EncryptedUsrCod='lD1biEj4bHXIGH5YUqyLNcZCCYXWV5IMtsQjsuImmM6' WHERE DNI='24243619';

------

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1026','es','N','Ir a instituci�n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1027','es','N','Ir a pa�s');

-----

ALTER TABLE deg_types ADD COLUMN InsCod INT NOT NULL AFTER DegTypCod;
ALTER TABLE deg_types ADD INDEX (InsCod);
UPDATE deg_types SET InsCod=(SELECT InsCod FROM institutions WHERE ShortName='ugr.es');
ALTER TABLE sessions ADD COLUMN InsCod INT NOT NULL DEFAULT -1 AFTER Role;
ALTER TABLE sessions ADD COLUMN CtyCod INT NOT NULL DEFAULT -1 AFTER Role;

-----

CREATE TABLE IF NOT EXISTS IP_last ( IP CHAR(15) NOT NULL, LastClick DATETIME NOT NULL, PRIMARY KEY(IP), INDEX(LastClick));

-----

UPDATE actions SET Txt='Ver configuraci�n asignatura' WHERE ActCod='1023';

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1028','es','N','Imprimir configuraci�n asignatura');

-----

UPDATE actions SET Txt='Seleccionar titulaci�n/asignatura' WHERE ActCod='842';
UPDATE actions SET Txt='Cambiar a otra asignatura' WHERE ActCod='1010';

------

ALTER TABLE countries ADD COLUMN Name_gn VARCHAR(255) NOT NULL AFTER Name_fr;
CREATE INDEX Name_gn ON countries (Name_gn);
UPDATE countries SET Name_gn=Name_es;

-----

SELECT courses.CrsCod,courses.FullName,crs_usr.Role FROM crs_usr,courses WHERE crs_usr.UsrCod='1346' AND crs_usr.CrsCod=courses.CrsCod ORDER BY courses.FullName;

-----

SELECT COUNT(*) FROM file_public WHERE CrsCod='1' AND GrpCod='-1' AND FileBrowser='3' AND Path LIKE 'descarga/%';

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1029','es','N','Ver metadatos arch. doc. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1030','es','N','Ver metadatos arch. doc. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1031','es','N','Ver metadatos arch. com. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1032','es','N','Ver metadatos arch. com. grp.');

-----

UPDATE actions SET Txt='Ver metadatos arch. doc. asg.' WHERE ActCod='996';
UPDATE actions SET Txt='Ver metadatos arch. doc. grp.' WHERE ActCod='998';
UPDATE actions SET Txt='Ver metadatos arch. com. asg.' WHERE ActCod='1000';
UPDATE actions SET Txt='Ver metadatos arch. com. grp.' WHERE ActCod='1002';

UPDATE actions SET Obsolete='Y' WHERE ActCod='997';
UPDATE actions SET Obsolete='Y' WHERE ActCod='999';
UPDATE actions SET Obsolete='Y' WHERE ActCod='1001';
UPDATE actions SET Obsolete='Y' WHERE ActCod='1003';

-----

RENAME TABLE crs_common_files TO files;

-----

ALTER TABLE files ADD COLUMN FileBrowser TINYINT NOT NULL AFTER GrpCod;
UPDATE files SET FileBrowser='4' WHERE GrpCod<='0';
UPDATE files SET FileBrowser='5' WHERE GrpCod>'0';
DROP INDEX CrsCod ON files;
CREATE INDEX CrsCod_GrpCod_FileBrowser ON files (CrsCod,GrpCod,FileBrowser);

-----

ALTER TABLE files ADD COLUMN WorksUsrCod INT NOT NULL DEFAULT -1 AFTER GrpCod;
DROP INDEX CrsCod_GrpCod_FileBrowser ON files;
CREATE INDEX CrsCod_GrpCod_WorksUsrCod_FileBrowser ON files (CrsCod,GrpCod,WorksUsrCod,FileBrowser);
ALTER TABLE files ADD COLUMN Public ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Path;

-----

DROP TABLE file_license;
ALTER TABLE files ADD COLUMN License TINYINT NOT NULL DEFAULT 0 AFTER Public;

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1033','es','N','Ver datos arch. doc. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1034','es','N','Ver datos arch. doc. grp.');

-----

ALTER TABLE files CHANGE GrpCod GrpCod INT NOT NULL DEFAULT -1;
ALTER TABLE files CHANGE WorksUsrCod ZoneUsrCod INT NOT NULL DEFAULT -1;
ALTER TABLE files CHANGE UsrCod PublisherUsrCod INT NOT NULL;
DROP INDEX CrsCod_GrpCod_WorksUsrCod_FileBrowser ON files;
CREATE INDEX CrsCod_GrpCod_ZoneUsrCod_FileBrowser ON files (CrsCod,GrpCod,ZoneUsrCod,FileBrowser);
UPDATE files SET CrsCod='-1',GrpCod='-1',ZoneUsrCod=PublisherUsrCod WHERE FileBrowser='9';

-----

mysql> DESCRIBE surveys;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| SvyCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| DegCod    | int(11)       | NO   | MUL | -1      |                |
| CrsCod    | int(11)       | NO   |     | -1      |                |
| Hidden    | enum('N','Y') | NO   |     | N       |                |
| NumNotif  | int(11)       | NO   |     | 0       |                |
| Roles     | int(11)       | NO   |     | 0       |                |
| UsrCod    | int(11)       | NO   |     | NULL    |                |
| StartTime | datetime      | NO   |     | NULL    |                |
| EndTime   | datetime      | NO   |     | NULL    |                |
| Title     | varchar(255)  | NO   |     | NULL    |                |
| Txt       | text          | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
11 rows in set (0.00 sec)


mysql> DESCRIBE svy_questions;
+---------+-----------------------------------------+------+-----+---------+----------------+
| Field   | Type                                    | Null | Key | Default | Extra          |
+---------+-----------------------------------------+------+-----+---------+----------------+
| QstCod  | int(11)                                 | NO   | PRI | NULL    | auto_increment |
| SvyCod  | int(11)                                 | NO   | MUL | NULL    |                |
| QstInd  | int(11)                                 | NO   |     | 0       |                |
| AnsType | enum('unique_choice','multiple_choice') | NO   |     | NULL    |                |
| Stem    | text                                    | NO   |     | NULL    |                |
+---------+-----------------------------------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)

DESCRIBE svy_answers;
+---------+------------+------+-----+---------+-------+
| Field   | Type       | Null | Key | Default | Extra |
+---------+------------+------+-----+---------+-------+
| QstCod  | int(11)    | NO   | PRI | NULL    |       |
| AnsInd  | tinyint(4) | NO   | PRI | NULL    |       |
| NumUsrs | int(11)    | NO   |     | 0       |       |
| Answer  | text       | NO   |     | NULL    |       |
+---------+------------+------+-----+---------+-------+
4 rows in set (0.01 sec)

mysql> DESCRIBE svy_users;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| SvyCod | int(11) | NO   | PRI | NULL    |       |
| UsrCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)


INSERT INTO surveys (SvyCod,DegCod,CrsCod,Hidden,NumNotif,Roles,UsrCod,StartTime,EndTime,Title,Txt) SELECT * FROM surveys_borrame WHERE SvyCod IN ('112');
INSERT INTO svy_questions (QstCod,SvyCod,QstInd,AnsType,Stem) SELECT * FROM svy_questions_borrame WHERE SvyCod IN ('112');
INSERT INTO svy_answers (QstCod,AnsInd,NumUsrs,Answer) SELECT * FROM svy_answers_borrame WHERE QstCod IN (SELECT QstCod FROM svy_questions_borrame WHERE SvyCod IN ('112'));
INSERT INTO svy_users (SvyCod,UsrCod) SELECT * FROM svy_users_borrame WHERE SvyCod IN ('112');

-----

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1035','es','N','Ver datos arch. admin.calif.asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1036','es','N','Cambiar datos arch. admin.calif.asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1037','es','N','Ver datos arch. admin.calif.grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1038','es','N','Cambiar datos arch. admin.calif.grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1039','es','N','Ver datos arch. mis actividades');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1040','es','N','Cambiar datos arch. mis actividades');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1041','es','N','Ver datos arch. mis trabajos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1042','es','N','Cambiar datos arch. mis trabajos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1043','es','N','Ver datos arch. activ. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1044','es','N','Cambiar datos arch. activ. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1045','es','N','Ver datos arch. trab. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1046','es','N','Cambiar datos arch. trab. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1047','es','N','Ver datos arch. malet�n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1048','es','N','Cambiar datos arch. malet�n');

-----

ALTER TABLE files ADD COLUMN FilCod INT NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (FilCod);
CREATE TABLE IF NOT EXISTS file_view (FilCod INT NOT NULL,UsrCod INT NOT NULL,NumViews INT NOT NULL DEFAULT 0,UNIQUE INDEX(FilCod,UsrCod));


SELECT SUM(NumViews) FROM file_view WHERE FilCod='380';



--------------------------- A partir de aquí hay un control exhaustivo de cambios ------------------------------



----- 2012-06-28
Las líneas siguientes ya hechas en 200.10.229.90, swad.ugr.es, www.una.py/swad, localhost
Falta por hacer en openswad.org

ALTER TABLE degrees ADD COLUMN CtrCod INT NOT NULL AFTER DegCod, ADD KEY (CtrCod);
ALTER TABLE sessions ADD COLUMN CtrCod INT NOT NULL DEFAULT -1 AFTER InsCod;
UPDATE degrees SET CtrCod=(SELECT CtrCod FROM centres ORDER BY CtrCod LIMIT 1) WHERE CtrCod<='0';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1049','es','N','Cambiar centro de titul.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1050','es','N','Ir a un centro');
UPDATE actions SET Obsolete='Y' WHERE ActCod='1014';

----- 2012-07-03
Las líneas siguientes ya hechas en 200.10.229.90, swad.ugr.es, www.una.py/swad, localhost
Falta por hacer en openswad.org

UPDATE actions SET Obsolete='Y' WHERE ActCod='722';

----- 2012-07-03
Las líneas siguientes ya hechas en 200.10.229.90, swad.ugr.es, www.una.py/swad, localhost
Falta por hacer en openswad.org

ALTER TABLE degrees DROP COLUMN InsCod;
ALTER TABLE deg_types DROP COLUMN InsCod;

----- 2012-07-05, swad11.71
Las líneas siguientes ya hechas en 200.10.229.90, swad.ugr.es, www.una.py/swad, localhost
Falta por hacer openswad.org

ALTER TABLE sessions DROP COLUMN DegTypCod;

----- 2012-07-08, swad11.72
Las líneas siguientes ya hechas en www.una.py/swad, localhost, swad.ugr.es
Falta por hacer openswad.org, 200.10.229.90

ALTER TABLE notif ADD COLUMN CtrCod INT NOT NULL DEFAULT -1 AFTER InsCod;
ALTER TABLE centres ADD COLUMN Logo VARCHAR(16) NOT NULL AFTER FullName;
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1051','es','N','Cambiar logo de centro');

----- 2012-09-13, swad11.75.7

ALTER TABLE courses DROP COLUMN Status;
ALTER TABLE courses DROP COLUMN RequesterUsrCod;

ALTER TABLE courses ADD COLUMN Status TINYINT NOT NULL DEFAULT 0 AFTER AllowDirectLogIn;
ALTER TABLE courses ADD COLUMN RequesterUsrCod INT NOT NULL DEFAULT -1 AFTER Status;
CREATE INDEX Status ON courses (Status);

----- 2012-09-14, swad11.75.8

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1052','es','Y','Solicitar asignaturas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1053','es','N','Solicitar asignatura');

----- 2012-09-15, swad11.75.10

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1054','es','Y','Solicitar inscripci�n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1055','es','Y','Cambiar estado asignatura');

----- 2012-09-16, swad11.76.1

UPDATE actions SET Txt='Formulario solicitud inscripci�n' WHERE ActCod='1054';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1056','es','Y','Solicitar inscripci�n');

----- 2012-09-17, swad11.76.2

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1057','es','Y','Listar inscripciones pendientes');

----- 2012-09-17, swad11.76.3

CREATE TABLE IF NOT EXISTS crs_usr_requests (CrsCod INT NOT NULL DEFAULT -1,UsrCod INT NOT NULL,Role TINYINT NOT NULL DEFAULT 0,RequestTime DATETIME NOT NULL,UNIQUE INDEX(CrsCod,UsrCod),INDEX(UsrCod));

----- 2012-09-20, swad11.78.1

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1058','es','Y','Rechazar solicitud inscripci�n');

----- 2012-09-23, swad11.79

UPDATE actions SET Txt='Preguntar si rechazar solicitud inscripci�n' WHERE ActCod='1058';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1059','es','Y','Rechazar solicitud inscripci�n');

----- 

ALTER DATABASE swad DEFAULT CHARACTER SET latin1 DEFAULT COLLATE latin1_bin;

----- 2012-09-24, swad12.0

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1060','es','Y','Titul. con asignat. pendientes');

SELECT degrees.DegCod,COUNT(*) FROM degrees,courses WHERE degrees.DegCod=courses.DegCod AND (courses.Status & 1)<>0 GROUP BY degrees.DegCod ORDER BY degrees.ShortName;
SELECT courses.DegCod,COUNT(*) FROM courses,degrees WHERE (courses.Status & 1)<>0 AND courses.DegCod=degrees.DegCod GROUP BY courses.DegCod ORDER BY degrees.ShortName;

SELECT CrsCod,UsrCod,Role FROM crs_usr_requests WHERE ((1<<Role)&3)<>0 ORDER BY RequestTime;

----- 2012-09-30, swad12.1

ALTER TABLE crs_grp_types ADD COLUMN OpenTime DATETIME NOT NULL DEFAULT 0 AFTER Multiple;

----- 2012-09-30, swad12.1

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1061','es','Y','Cambiar fecha apertura tipo grupo');
ALTER TABLE crs_grp_types ADD COLUMN MustBeOpened ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Multiple;
DROP INDEX OpenTime ON crs_grp_types;

----- 2012-10-04, swad12.4

ALTER TABLE tst_questions ADD COLUMN NumHitsNotBlank INT NOT NULL DEFAULT 0 AFTER NumHits;
UPDATE tst_questions SET NumHitsNotBlank=NumHits;

----- 2012-10-04, swad12.6

DELETE FROM file_view WHERE FilCod IN (SELECT FilCod FROM files WHERE CrsCod>0 AND CrsCod NOT IN (SELECT CrsCod FROM courses));
DELETE FROM file_view WHERE FilCod IN (SELECT FilCod FROM files WHERE GrpCod>0 AND GrpCod NOT IN (SELECT GrpCod FROM crs_grp));
DELETE FROM file_view WHERE FilCod IN (SELECT FilCod FROM files WHERE ZoneUsrCod>0 AND ZoneUsrCod NOT IN (SELECT UsrCod FROM usr_data));
DELETE FROM files WHERE CrsCod>0 AND CrsCod NOT IN (SELECT CrsCod FROM courses);
DELETE FROM files WHERE GrpCod>0 AND GrpCod NOT IN (SELECT GrpCod FROM crs_grp);
DELETE FROM files WHERE ZoneUsrCod>0 AND ZoneUsrCod NOT IN (SELECT UsrCod FROM usr_data);

----- 2012-10-04, swad12.7

ALTER TABLE usr_data ADD COLUMN LastCrs INT NOT NULL DEFAULT -1 AFTER NotifyEvents;

----- 2012-11-01, swad12.12.1

DELETE FROM expanded_folders WHERE FileBrowser IN (3,8,11,13);

----- 2012-11-03, swad12.13

UPDATE actions SET Obsolete='N' WHERE ActCod IN (1054,1055,1056,1057,1058,1059,1060,1061);
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1062','es','N','Administrar documentos (opci�n antigua)');

----- 2012-11-04, swad12.14.2

CREATE TABLE IF NOT EXISTS att_events (AttCod INT NOT NULL AUTO_INCREMENT,CrsCod INT NOT NULL DEFAULT -1,Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',UsrCod INT NOT NULL,StartTime DATETIME NOT NULL,EndTime DATETIME NOT NULL,Title VARCHAR(255) NOT NULL,Txt TEXT NOT NULL,UNIQUE INDEX(AttCod),INDEX(CrsCod,Hidden));
CREATE TABLE IF NOT EXISTS att_grp (AttCod INT NOT NULL,GrpCod INT NOT NULL,UNIQUE INDEX(AttCod,GrpCod));
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1063','es','N','Solicitar creaci�n evento asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1064','es','N','Solicitar edici�n evento asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1065','es','N','Crear evento asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1066','es','N','Modificar evento asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1067','es','N','Solicitar elim. evento asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1068','es','N','Eliminar evento asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1069','es','N','Ocultar evento asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1070','es','N','Mostrar evento asistencia');

----- 2012-11-04, swad12.14.3

CREATE TABLE IF NOT EXISTS att_usr (AttCod INT NOT NULL,UsrCod INT NOT NULL,UNIQUE INDEX(AttCod,UsrCod),INDEX(UsrCod));
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1071','es','N','Listar estudiantes asistentes a evento');

----- 2012-11-07, swad12.18.1

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1072','es','N','Registrar estudiantes asistentes a evento');

----- 2012-11-12, swad12.18.5

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1073','es','N','Selec. estud. y eventos listado asistencia');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1074','es','N','Listar asistencia a varios eventos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1075','es','N','Imprimir asistencia a varios eventos');

----- 2012-11-19, swad12.21

CREATE TABLE IF NOT EXISTS nicknames (UsrCod INT NOT NULL,Nickname CHAR(16) COLLATE latin1_general_ci NOT NULL,CreatTime DATETIME NOT NULL,UNIQUE INDEX(UsrCod,Nickname),UNIQUE INDEX(Nickname));
INSERT INTO nicknames (UsrCod,Nickname,CreatTime) SELECT UsrCod,NickName,NOW() FROM usr_data WHERE Nickname<>'';

----- 2012-11-19, swad12.21

ALTER TABLE nicknames CHANGE Nickname Nickname CHAR(16) COLLATE latin1_spanish_ci NOT NULL;

----- 2012-11-24, swad12.23

ALTER TABLE att_events ADD COLUMN CommentVisibility TINYINT NOT NULL DEFAULT 0 AFTER EndTime;
ALTER TABLE att_usr ADD COLUMN Comment VARCHAR(255) NOT NULL AFTER UsrCod;

----- 2012-11-26, swad12.23.2

ALTER TABLE att_usr ADD COLUMN Present ENUM('N','Y') NOT NULL DEFAULT 'Y' AFTER UsrCod;

----- 2012-11-26, swad12.23.3

ALTER TABLE att_usr CHANGE Comment CommentStd VARCHAR(255) NOT NULL;
ALTER TABLE att_usr ADD COLUMN CommentTch VARCHAR(255) NOT NULL AFTER CommentStd;
ALTER TABLE att_events CHANGE CommentVisibility CommentTchVisible ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER EndTime;

----- 2012-11-26, swad12.23.4

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1076','es','N','Registrar estudiante asistente a evento');

----- 2012-11-29, swad12.24.4

ALTER TABLE att_usr CHANGE CommentStd CommentStd TEXT NOT NULL;
ALTER TABLE att_usr CHANGE CommentTch CommentTch TEXT NOT NULL;

----- 2012-12-16, swad12.28

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1077','es','N','Cambiar de idioma tras autenticar');

----- 2012-12-29, swad12.32.1

ALTER TABLE imported_students CHANGE DNI UsrID CHAR(16) NOT NULL;
ALTER TABLE usr_data CHANGE DNI UsrID CHAR(16) NOT NULL;

----- 2012-12-30, swad12.32.2

ALTER TABLE log_recent CHANGE DNI UsrID CHAR(16) NOT NULL;
ALTER TABLE log CHANGE DNI UsrID CHAR(16) NOT NULL;

----- 2013-03-02, swad12.39

ALTER TABLE tst_answers ADD COLUMN Feedback TEXT NOT NULL AFTER Answer;

----- 2013-03-02, swad12.40

ALTER TABLE tst_questions ADD COLUMN Feedback TEXT NOT NULL AFTER Stem;
ALTER TABLE tst_config CHANGE Feedback Feedback ENUM('nothing','total_result','each_result','each_good_bad','full_feedback') NOT NULL;

----- 2013-03-07, swad12.40.1

ALTER TABLE tst_questions CHANGE AnsType AnsType ENUM ('int','float','true_false','unique_choice','multiple_choice','text') NOT NULL;

----- 2013-03-10, swad12.40.5

CREATE TABLE IF NOT EXISTS tst_exams (TstCod INT NOT NULL AUTO_INCREMENT,CrsCod INT NOT NULL,UsrCod INT NOT NULL,TstTime DATETIME NOT NULL,NumQuestions INT NOT NULL DEFAULT 0,NumQuestionsNotBlank INT NOT NULL DEFAULT 0,Score DOUBLE PRECISION NOT NULL DEFAULT 0, ScoreNotBlank DOUBLE PRECISION NOT NULL DEFAULT 0,UNIQUE INDEX(TstCod),INDEX(CrsCod,UsrCod));
CREATE TABLE IF NOT EXISTS tst_exam_questions (TstCod INT NOT NULL,QstCod INT NOT NULL,Score DOUBLE PRECISION NOT NULL DEFAULT 0,INDEX(TstCod,QstCod));
CREATE TABLE IF NOT EXISTS tst_exam_answers (TstCod INT NOT NULL,QstCod INT NOT NULL,AnsInd INT NOT NULL,Answer TEXT NOT NULL,INDEX(TstCod,QstCod));

----- 2013-03-11, swad12.40.5

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1080','es','N','Seleccionar usuarios para result. test');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1081','es','N','Ver resultados de tests de usuarios');

----- 2013-03-14, swad12.42

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1082','es','N','Ver un examen de test ya realizado');

----- 2013-03-14, swad12.42.1

DROP TABLE tst_exams;
CREATE TABLE IF NOT EXISTS tst_exams (TstCod INT NOT NULL AUTO_INCREMENT,CrsCod INT NOT NULL,UsrCod INT NOT NULL,TstTime DATETIME NOT NULL,NumQsts INT NOT NULL DEFAULT 0,NumQstsNotBlank INT NOT NULL DEFAULT 0,Score DOUBLE PRECISION NOT NULL DEFAULT 0, ScoreNotBlank DOUBLE PRECISION NOT NULL DEFAULT 0,UNIQUE INDEX(TstCod),INDEX(CrsCod,UsrCod));

----- 2013-03-15, swad12.43

DROP TABLE tst_exam_questions;
CREATE TABLE IF NOT EXISTS tst_exam_questions (TstCod INT NOT NULL,QstCod INT NOT NULL,QstInd INT NOT NULL,Score DOUBLE PRECISION NOT NULL DEFAULT 0,INDEX(TstCod,QstCod));
DROP TABLE tst_exam_answers;
CREATE TABLE IF NOT EXISTS tst_exam_answers (TstCod INT NOT NULL,QstCod INT NOT NULL,Indexes TEXT NOT NULL,Answers TEXT NOT NULL,INDEX(TstCod,QstCod));

----- 2013-03-17, swad12.46

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1083','es','N','Seleccionar fechas para mis result. test');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1084','es','N','Ver mis resultados de tests');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1085','es','N','Ver un examen de test m&iacute;o ya realizado');

ALTER TABLE tst_exams ADD COLUMN AllowTeachers ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER UsrCod;

----- 2013-03-17, swad12.47.1

ALTER TABLE tst_exam_questions ADD COLUMN Indexes TEXT NOT NULL AFTER Score;
ALTER TABLE tst_exam_questions ADD COLUMN Answers TEXT NOT NULL AFTER Indexes;
UPDATE tst_exam_questions,tst_exam_answers SET tst_exam_questions.Indexes=tst_exam_answers.Indexes,tst_exam_questions.Answers=tst_exam_answers.Answers WHERE tst_exam_questions.TstCod=tst_exam_answers.TstCod AND tst_exam_questions.QstCod=tst_exam_answers.QstCod;
DROP TABLE tst_exam_answers;

----- 2013-03-20, swad12.47.3

ALTER TABLE crs_usr ADD COLUMN LastAccTst DATETIME NOT NULL AFTER NextAccTst;
ALTER TABLE crs_usr ADD COLUMN NumQstsLastTst INT NOT NULL DEFAULT 0 AFTER LastAccTst;
UPDATE crs_usr SET LastAccTst=NOW();
ALTER TABLE crs_usr DROP COLUMN NextAccTst;

----- 2013-03-24, swad12.48

UPDATE notif SET NotifyEvent=53 WHERE NotifyEvent=1;
UPDATE notif SET NotifyEvent=54 WHERE NotifyEvent=2;
UPDATE notif SET NotifyEvent=55 WHERE NotifyEvent=3;
UPDATE notif SET NotifyEvent=58 WHERE NotifyEvent=4;
UPDATE notif SET NotifyEvent=59 WHERE NotifyEvent=5;
UPDATE notif SET NotifyEvent=60 WHERE NotifyEvent=6;
UPDATE notif SET NotifyEvent=61 WHERE NotifyEvent=7;

UPDATE sta_notif SET NotifyEvent=53 WHERE NotifyEvent=1;
UPDATE sta_notif SET NotifyEvent=54 WHERE NotifyEvent=2;
UPDATE sta_notif SET NotifyEvent=55 WHERE NotifyEvent=3;
UPDATE sta_notif SET NotifyEvent=58 WHERE NotifyEvent=4;
UPDATE sta_notif SET NotifyEvent=59 WHERE NotifyEvent=5;
UPDATE sta_notif SET NotifyEvent=60 WHERE NotifyEvent=6;
UPDATE sta_notif SET NotifyEvent=61 WHERE NotifyEvent=7;

UPDATE notif SET NotifyEvent=3 WHERE NotifyEvent=53;
UPDATE notif SET NotifyEvent=4 WHERE NotifyEvent=54;
UPDATE notif SET NotifyEvent=5 WHERE NotifyEvent=55;
UPDATE notif SET NotifyEvent=8 WHERE NotifyEvent=58;
UPDATE notif SET NotifyEvent=9 WHERE NotifyEvent=59;
UPDATE notif SET NotifyEvent=10 WHERE NotifyEvent=60;
UPDATE notif SET NotifyEvent=11 WHERE NotifyEvent=61;

UPDATE sta_notif SET NotifyEvent=3 WHERE NotifyEvent=53;
UPDATE sta_notif SET NotifyEvent=4 WHERE NotifyEvent=54;
UPDATE sta_notif SET NotifyEvent=5 WHERE NotifyEvent=55;
UPDATE sta_notif SET NotifyEvent=8 WHERE NotifyEvent=58;
UPDATE sta_notif SET NotifyEvent=9 WHERE NotifyEvent=59;
UPDATE sta_notif SET NotifyEvent=10 WHERE NotifyEvent=60;
UPDATE sta_notif SET NotifyEvent=11 WHERE NotifyEvent=61;


1110 9 8 7 6 5 4 3 2 1 0
 0 0 0 0 a b c d e f g 0
 a b c d 0 0 e f g 0 0 0

UPDATE usr_data SET NotifyEvents=(((NotifyEvents << 4) & b'111100000000') | ((NotifyEvents << 2) & b'111000')) WHERE NotifyEvents<>0;

----- 2013-03-25, swad12.48.3

ALTER TABLE crs_usr_requests ADD COLUMN ReqCod INT NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (ReqCod);

----- 2013-03-26, swad12.48.5

ALTER TABLE notif ADD INDEX (CrsCod);

----- 2013-03-27, swad12.49.1

ALTER TABLE notif ADD COLUMN GrpCod INT NOT NULL DEFAULT -1 AFTER CrsCod;
ALTER TABLE notif DROP COLUMN GrpCod;

----- 2013-04-01, swad12.50

CREATE TABLE files_backup LIKE files;
INSERT INTO files_backup SELECT * FROM files;
CREATE TABLE marks_backup LIKE marks;
INSERT INTO marks_backup SELECT * FROM marks;
CREATE TABLE notif_backup LIKE notif;
INSERT INTO notif_backup SELECT * FROM notif;

DROP TABLE IF EXISTS files_copy;
CREATE TABLE files_copy LIKE files;
INSERT INTO files_copy SELECT * FROM files;

DROP TABLE IF EXISTS marks_copy;
CREATE TABLE marks_copy LIKE marks;
INSERT INTO marks_copy SELECT * FROM marks;
DELETE FROM marks_copy WHERE (LOWER(Path) NOT LIKE '%.htm' AND LOWER(Path) NOT LIKE '%.html');

INSERT INTO files_copy (CrsCod,GrpCod,ZoneUsrCod,FileBrowser,PublisherUsrCod,Path,Public,License) SELECT CrsCod,GrpCod,'-1','8','-1',Path,'N','1' FROM marks_copy WHERE GrpCod='-1' AND NOT EXISTS (SELECT * FROM files_copy WHERE files_copy.FileBrowser='8' AND marks_copy.CrsCod=files_copy.CrsCod AND marks_copy.GrpCod=files_copy.GrpCod AND marks_copy.Path=files_copy.Path);
INSERT INTO files_copy (CrsCod,GrpCod,ZoneUsrCod,FileBrowser,PublisherUsrCod,Path,Public,License) SELECT CrsCod,GrpCod,'-1','13','-1',Path,'N','1' FROM marks_copy WHERE GrpCod>'0' AND NOT EXISTS (SELECT * FROM files_copy WHERE files_copy.FileBrowser='13' AND marks_copy.CrsCod=files_copy.CrsCod AND marks_copy.GrpCod=files_copy.GrpCod AND marks_copy.Path=files_copy.Path);

DROP TABLE IF EXISTS marks2;
CREATE TABLE IF NOT EXISTS marks2 (FilCod INT NOT NULL,Header INT NOT NULL,Footer INT NOT NULL,UNIQUE INDEX(FilCod));
INSERT INTO marks2 (FilCod,Header,Footer) SELECT files_copy.FilCod,marks_copy.Header,marks_copy.Footer FROM files_copy,marks_copy WHERE (files_copy.FileBrowser='8' AND marks_copy.GrpCod='-1' AND marks_copy.CrsCod=files_copy.CrsCod AND marks_copy.GrpCod=files_copy.GrpCod AND marks_copy.Path=files_copy.Path) OR (files_copy.FileBrowser='13' AND marks_copy.GrpCod>'0' AND marks_copy.CrsCod=files_copy.CrsCod AND marks_copy.GrpCod=files_copy.GrpCod AND marks_copy.Path=files_copy.Path);

DROP TABLE IF EXISTS notif_copy;
CREATE TABLE notif_copy LIKE notif;
INSERT INTO notif_copy SELECT * FROM notif;
UPDATE notif_copy,marks_copy,files_copy SET notif_copy.Cod=files_copy.FilCod WHERE notif_copy.NotifyEvent='5' AND notif_copy.Cod=marks_copy.MrkCod AND marks_copy.GrpCod='-1' AND marks_copy.CrsCod=files_copy.CrsCod AND marks_copy.GrpCod=files_copy.GrpCod AND marks_copy.Path=files_copy.Path AND files_copy.FileBrowser='8';
UPDATE notif_copy,marks_copy,files_copy SET notif_copy.Cod=files_copy.FilCod WHERE notif_copy.NotifyEvent='5' AND notif_copy.Cod=marks_copy.MrkCod AND marks_copy.GrpCod>'0' AND marks_copy.CrsCod=files_copy.CrsCod AND marks_copy.GrpCod=files_copy.GrpCod AND marks_copy.Path=files_copy.Path AND files_copy.FileBrowser='13';
UPDATE notif_copy SET Cod='-1' WHERE NotifyEvent='5' AND Cod NOT IN (SELECT FilCod FROM marks2);

DROP TABLE files;
RENAME TABLE files_copy TO files;

DROP TABLE marks;
DROP TABLE marks_copy;
RENAME TABLE marks2 TO marks;

DROP TABLE notif;
RENAME TABLE notif_copy TO notif;

----- 2013-04-03, swad12.52

ALTER TABLE files ADD COLUMN Hidden ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Path;

DROP TABLE IF EXISTS files_copy;
CREATE TABLE files_copy LIKE files;
INSERT INTO files_copy SELECT * FROM files;

INSERT INTO files_copy (CrsCod,GrpCod,ZoneUsrCod,FileBrowser,PublisherUsrCod,Path,Hidden,Public,License) SELECT CrsCod,GrpCod,'-1','3','-1',Path,'Y','N','1' FROM hidden_downloads WHERE GrpCod='-1' AND NOT EXISTS (SELECT * FROM files_copy WHERE files_copy.FileBrowser='3' AND hidden_downloads.CrsCod=files_copy.CrsCod AND hidden_downloads.GrpCod=files_copy.GrpCod AND hidden_downloads.Path=files_copy.Path);
INSERT INTO files_copy (CrsCod,GrpCod,ZoneUsrCod,FileBrowser,PublisherUsrCod,Path,Hidden,Public,License) SELECT CrsCod,GrpCod,'-1','11','-1',Path,'Y','N','1' FROM hidden_downloads WHERE GrpCod>'0' AND NOT EXISTS (SELECT * FROM files_copy WHERE files_copy.FileBrowser='3' AND hidden_downloads.CrsCod=files_copy.CrsCod AND hidden_downloads.GrpCod=files_copy.GrpCod AND hidden_downloads.Path=files_copy.Path);

UPDATE files_copy,hidden_downloads SET files_copy.Hidden='Y' WHERE files_copy.FileBrowser='3' AND files_copy.CrsCod=hidden_downloads.CrsCod AND hidden_downloads.GrpCod='-1' AND files_copy.Path=hidden_downloads.Path;
UPDATE files_copy,hidden_downloads SET files_copy.Hidden='Y' WHERE files_copy.FileBrowser='11' AND files_copy.CrsCod=hidden_downloads.CrsCod AND hidden_downloads.GrpCod>'0' AND files_copy.GrpCod=hidden_downloads.GrpCod AND files_copy.Path=hidden_downloads.Path;

DROP TABLE files;
RENAME TABLE files_copy TO files;

----- 2013-04-18, swad12.58.3

ALTER TABLE sessions DROP COLUMN SideCols;

DROP TABLE IF EXISTS notif_backup;
DROP TABLE IF EXISTS marks_backup;
DROP TABLE IF EXISTS marks2;

DROP TABLE IF EXISTS usuarios_accesos_tmp;
DROP TABLE IF EXISTS alumnos_accesos_tmp;
DROP TABLE IF EXISTS alumnos_tmp;
DROP TABLE IF EXISTS profesores_accesos_tmp;
DROP TABLE IF EXISTS profesores_tmp;

----- 2013-04-18, swad12.59

ALTER TABLE sessions ADD COLUMN WhatToSearch TINYINT NOT NULL DEFAULT 0 AFTER LastPageMsgSnt;
ALTER TABLE sessions ADD COLUMN SearchString VARCHAR(255) NOT NULL AFTER WhatToSearch;

----- 2013-04-20, swad12.59.1

ALTER TABLE usr_data ADD COLUMN WhatToSearch TINYINT NOT NULL DEFAULT 0 AFTER NotifyEvents;

----- 2013-04-21, swad12.60

CREATE TABLE IF NOT EXISTS usr_last (
	UsrCod INT NOT NULL,
	WhatToSearch TINYINT NOT NULL DEFAULT 0,
	LastCrs INT NOT NULL DEFAULT -1,
	LastTab TINYINT NOT NULL DEFAULT 0,
	LastTime DATETIME NOT NULL DEFAULT 0,
	LastAccNotif DATETIME NOT NULL DEFAULT 0,
	LastAccBriefcase DATETIME NOT NULL DEFAULT 0,
	UNIQUE INDEX(UsrCod),
	INDEX(LastTime)) SELECT UsrCod,WhatToSearch,LastCrs,LastTab,LastTime,LastAccNotif,LastAccBriefcase FROM usr_data;
ALTER TABLE usr_data DROP COLUMN WhatToSearch,DROP COLUMN LastCrs,DROP COLUMN LastTab,DROP COLUMN LastTime,DROP COLUMN LastAccNotif,DROP COLUMN LastAccBriefcase;

----- 2013-04-24, swad12.61.1

CREATE TABLE IF NOT EXISTS birthdays_today (UsrCod INT NOT NULL,Today DATE NOT NULL,UNIQUE INDEX(UsrCod),INDEX(Today));

----- 2013-05-03, swad12.62

UPDATE notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE notif SET NotifyEvent=11 WHERE NotifyEvent=9;
UPDATE sta_notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE sta_notif SET NotifyEvent=11 WHERE NotifyEvent=9;
UPDATE usr_data SET NotifyEvents=((NotifyEvents & b'10111111111') | ((NotifyEvents << 2) & b'100000000000') | ((NotifyEvents << 1) & b'1000000000000')) WHERE NotifyEvents<>0;

----- 2013-05-21, swad12.63

RENAME TABLE nicknames TO usr_nicknames;
CREATE TABLE IF NOT EXISTS usr_emails (UsrCod INT NOT NULL,E_mail VARCHAR(255) COLLATE latin1_general_ci NOT NULL,CreatTime DATETIME NOT NULL,UNIQUE INDEX(UsrCod,E_mail),UNIQUE INDEX(E_mail));
INSERT IGNORE INTO usr_emails (UsrCod,E_mail,CreatTime) SELECT UsrCod,E_mail,NOW() FROM usr_data WHERE E_mail<>'';

----- 2013-05-21, swad12.63.2

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1086','es','N','Ver datos arch. calif. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1087','es','N','Ver datos arch. calif. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1088','es','N','Cambiar mi direcci&oacute;n de correo');

----- 2013-05-21, swad12.63.2

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1089','es','N','Eliminar apodo');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1090','es','N','Eliminar direcci&oacute;n de correo');

----- 2013-05-27, swad12.63.5

ALTER TABLE usr_emails ADD COLUMN Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER CreatTime;

----- 2013-05-28, swad12.63.6

CREATE TABLE IF NOT EXISTS pending_emails (UsrCod INT NOT NULL,E_mail VARCHAR(255) COLLATE latin1_general_ci NOT NULL,MailKey CHAR(43) COLLATE latin1_bin NOT NULL,DateAndTime DATETIME NOT NULL,INDEX(UsrCod),UNIQUE INDEX(MailKey));

----- 2013-06-06, swad12.63.11

ALTER TABLE usr_data DROP COLUMN Nickname;

----- 2013-09-08, swad12.66

ALTER TABLE log_recent ADD COLUMN UsrCod INT NOT NULL DEFAULT -1 AFTER CrsCod, ADD INDEX (UsrCod);
ALTER TABLE log ADD COLUMN UsrCod INT NOT NULL DEFAULT -1 AFTER CrsCod, ADD INDEX (UsrCod);

UPDATE log_recent,usr_data SET log_recent.UsrCod=usr_data.UsrCod WHERE log_recent.UsrCod='-1' AND log_recent.UsrID<>'' AND log_recent.UsrID=usr_data.UsrID;
UPDATE log,usr_data SET log.UsrCod=usr_data.UsrCod WHERE log.UsrCod='-1' AND log.UsrID<>'' AND log.UsrID=usr_data.UsrID;

----- 2013-09-17, swad12.68

ALTER TABLE usr_data ADD COLUMN IconSet CHAR(16) NOT NULL AFTER Theme, ADD INDEX (IconSet);
UPDATE usr_data SET IconSet='nuvola';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1092','es','N','Cambiar conjunto de iconos');
ALTER TABLE IP_prefs ADD COLUMN IconSet CHAR(16) NOT NULL AFTER Theme;
UPDATE IP_prefs SET IconSet='nuvola';

----- 2013-09-20, swad12.69

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1093','es','N','Probar editor de texto');
DELETE FROM actions WHERE ActCod='1093';

----- 2013-09-22, swad12.70

ALTER TABLE crs_info_src ADD COLUMN InfoSrc2 ENUM('none','editor','plain_text','rich_text','page','URL') NOT NULL AFTER InfoSrc;
UPDATE crs_info_src SET InfoSrc2='none' WHERE InfoSrc='none';
UPDATE crs_info_src SET InfoSrc2='editor' WHERE InfoSrc='editor';
UPDATE crs_info_src SET InfoSrc2='plain_text' WHERE InfoSrc='text';
UPDATE crs_info_src SET InfoSrc2='page' WHERE InfoSrc='page';
UPDATE crs_info_src SET InfoSrc2='URL' WHERE InfoSrc='URL';
ALTER TABLE crs_info_src DROP COLUMN InfoSrc;
ALTER TABLE crs_info_src CHANGE COLUMN InfoSrc2 InfoSrc ENUM('none','editor','plain_text','rich_text','page','URL') NOT NULL;

UPDATE actions SET Txt='Editor de texto plano de info. asg.'   WHERE ActCod='850' AND Language='es';
UPDATE actions SET Txt='Editor de texto plano de gu�a docente' WHERE ActCod='787' AND Language='es';
UPDATE actions SET Txt='Editor de texto plano de teor�a'       WHERE ActCod='379' AND Language='es';
UPDATE actions SET Txt='Editor de texto plano de pr�cticas'    WHERE ActCod='389' AND Language='es';
UPDATE actions SET Txt='Editor de texto plano de bibliograf�a' WHERE ActCod='377' AND Language='es';
UPDATE actions SET Txt='Editor de texto plano de FAQ'          WHERE ActCod='405' AND Language='es';
UPDATE actions SET Txt='Editor de texto plano de enlaces'      WHERE ActCod='400' AND Language='es';
UPDATE actions SET Txt='Editor de texto plano de evaluaci�n'   WHERE ActCod='387' AND Language='es';

UPDATE actions SET Txt='Enviar texto plano de info. asg.'   WHERE ActCod='851' AND Language='es';
UPDATE actions SET Txt='Enviar texto plano de gu�a docente' WHERE ActCod='790' AND Language='es';
UPDATE actions SET Txt='Enviar texto plano de teor�a'       WHERE ActCod='394' AND Language='es';
UPDATE actions SET Txt='Enviar texto plano de pr�cticas'    WHERE ActCod='396' AND Language='es';
UPDATE actions SET Txt='Enviar texto plano de bibliograf�a' WHERE ActCod='398' AND Language='es';
UPDATE actions SET Txt='Enviar texto plano de FAQ'          WHERE ActCod='406' AND Language='es';
UPDATE actions SET Txt='Enviar texto plano de enlaces'      WHERE ActCod='401' AND Language='es';
UPDATE actions SET Txt='Enviar texto plano de evaluaci�n'   WHERE ActCod='397' AND Language='es';

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1093','es','N','Editor de texto enriq. de info. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1094','es','N','Editor de texto enriq. de gu�a docente');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1095','es','N','Editor de texto enriq. de teor�a');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1096','es','N','Editor de texto enriq. de pr�cticas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1097','es','N','Editor de texto enriq. de bibliograf�a');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1098','es','N','Editor de texto enriq. de FAQ');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1099','es','N','Editor de texto enriq. de enlaces');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1100','es','N','Editor de texto enriq. de evaluaci�n');

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1101','es','N','Enviar texto enriq. de info. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1102','es','N','Enviar texto enriq. de gu�a docente');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1103','es','N','Enviar texto enriq. de teor�a');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1104','es','N','Enviar texto enriq. de pr�cticas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1105','es','N','Enviar texto enriq. de bibliograf�a');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1106','es','N','Enviar texto enriq. de FAQ');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1107','es','N','Enviar texto enriq. de enlaces');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1108','es','N','Enviar texto enriq. de evaluaci�n');

----- 2013-09-23, swad13.0

ALTER TABLE usr_data CHANGE NotifyEvents EmailNtfEvents INT NOT NULL DEFAULT 0;
ALTER TABLE usr_data ADD COLUMN NotifNtfEvents INT NOT NULL DEFAULT -1 AFTER SecureIcons;

----- 2013-10-03, restaurar copia de seguridad por unos usuarios eliminados de grupos

Restaurar una tabla desde la copia de seguridad:

cd /var/www/backup/daily/swad

bunzip2 swad_2013-10-03_05h00m.Thursday.sql.bz2

awk '/Table structure for table `crs_grp_usr`/,/Table structure for table `crs_info_read`/{print}' swad_2013-10-03_05h00m.Thursday.sql > swad_2013-10-03_05h00m.Thursday_crs_grp_usr.sql
                                     ^                                          ^
                                tabla a buscar                           tabla siguiente

En el nuevo fichero sql cambiar crs_grp_usr por crs_grp_usr_borrame
mysql -u swad -p swad < swad_2013-10-03_05h00m.Thursday_crs_grp_usr.sql

DELETE FROM crs_grp_usr WHERE GrpCod IN (SELECT GrpCod FROM crs_grp WHERE GrpTypCod IN (SELECT GrpTypCod FROM crs_grp_types WHERE CrsCod='3891'));
INSERT INTO crs_grp_usr (GrpCod,UsrCod,LastAccDownloadGrp,LastAccCommonGrp,LastAccMarksGrp) SELECT * FROM crs_grp_usr_borrame WHERE GrpCod IN (SELECT GrpCod FROM crs_grp WHERE GrpTypCod IN (SELECT GrpTypCod FROM crs_grp_types WHERE CrsCod='3891'));

----- 2013-10-07

CREATE TABLE IF NOT EXISTS crs_last (CrsCod INT NOT NULL,LastTime DATETIME NOT NULL DEFAULT 0,UNIQUE INDEX(CrsCod),INDEX(LastTime));
REPLACE INTO crs_last (CrsCod,LastTime) SELECT CrsCod,MAX(ClickTime) FROM log WHERE Role>='2' GROUP BY CrsCod;
DELETE FROM crs_last WHERE CrsCod NOT IN (SELECT CrsCod FROM courses);
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1109','es','N','Solicitar elim. asignaturas antiguas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1110','es','N','Eliminar asignaturas antiguas');

SELECT CrsCod FROM (SELECT CrsCod FROM crs_last WHERE UNIX_TIMESTAMP(LastTime) < UNIX_TIMESTAMP()-20000 UNION SELECT CrsCod FROM courses WHERE CrsCod NOT IN (SELECT CrsCod FROM crs_last)) AS candidate_crss WHERE CrsCod NOT IN (SELECT DISTINCT CrsCod FROM crs_usr);

----- 2013-10-09, swad13.2.2

UPDATE actions SET Obsolete='Y' WHERE ActCod='1021';

----- 2013-10-10, swad13.3

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1111','es','N','Descargar arch. doc. asg.');

----- 2013-10-10, swad13.3.1

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1112','es','N','Descargar arch. doc. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1113','es','N','Descargar arch. doc. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1114','es','N','Descargar arch. doc. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1115','es','N','Descargar arch. com. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1116','es','N','Descargar arch. com. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1117','es','N','Descargar arch. mis actividades');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1118','es','N','Descargar arch. mis trabajos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1119','es','N','Descargar arch. activ. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1120','es','N','Descargar arch. trab. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1121','es','N','Descargar arch. admin.calif.asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1122','es','N','Descargar arch. admin.calif.grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1123','es','N','Descargar arch. malet&iacute;n');

----- 2013-10-13, swad13.4

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1124','es','N','Crear ZIP doc. asg.');

----- 2013-10-14, swad13.4.2

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1125','es','N','Crear ZIP doc. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1126','es','N','Crear ZIP admin.doc.asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1127','es','N','Crear ZIP admin.doc.grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1128','es','N','Crear ZIP compart.asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1129','es','N','Crear ZIP compart.grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1130','es','N','Crear ZIP carpeta mis activid.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1131','es','N','Crear ZIP carpeta mis trabajos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1132','es','N','Crear ZIP carpeta activid. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1133','es','N','Crear ZIP carpeta trabajos asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1134','es','N','Crear ZIP carpeta admin. calif. asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1135','es','N','Crear ZIP carpeta admin. calif. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1136','es','N','Crear ZIP carpeta malet&iacute;n');

----- 2013-10-20, swad13.7

CREATE TABLE IF NOT EXISTS banners (BanCod INT NOT NULL AUTO_INCREMENT,ShortName VARCHAR(32) NOT NULL,FullName VARCHAR(255) NOT NULL,WWW VARCHAR(255) NOT NULL,UNIQUE INDEX(BanCod));

----- 2013-10-20, swad13.7.1

ALTER TABLE banners ADD COLUMN Img VARCHAR(255) NOT NULL AFTER FullName;
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1137','es','N','Ver banners');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1138','es','N','Editar banners');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1139','es','N','Crear banner');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1140','es','N','Eliminar banner');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1141','es','N','Cambiar nombre corto de banner');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1142','es','N','Cambiar nombre largo de banner (');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1143','es','N','Cambiar URL de banner');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1144','es','N','Cambiar imagen de banner');

----- 2013-10-24, swad13.7.3

CREATE TABLE IF NOT EXISTS log_banners (LogCod INT NOT NULL,BanCod INT NOT NULL,UNIQUE INDEX(LogCod),INDEX(BanCod));

----- 2013-10-26, swad13.7.4

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1145','es','N','Clic en un banner');

----- 2013-11-02, swad13.9.1

ALTER TABLE timetable_crs CHANGE COLUMN Day Day ENUM('L','M','X','J','V','S','D') NOT NULL;
ALTER TABLE timetable_tut CHANGE COLUMN Day Day ENUM('L','M','X','J','V','S','D') NOT NULL;

----- 2013-11-02, swad13.9.1

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1146','es','N','Marcar notificaciones como vistas');

----- 2013-11-08, swad13.10.3

UPDATE actions SET Obsolete='Y' WHERE ActCod='373';

----- 2013-11-19, swad13.14

ALTER TABLE notif ADD NtfCod INT NOT NULL AUTO_INCREMENT FIRST, ADD PRIMARY KEY (NtfCod);

----- 2013-11-22, swad13.15

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1147','es','N','Eliminar uno de mis ID');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1148','es','N','Crear un nuevo ID para m&iacute;');
CREATE TABLE IF NOT EXISTS usr_IDs (UsrCod INT NOT NULL,UsrID CHAR(16) NOT NULL,CreatTime DATETIME NOT NULL,UNIQUE INDEX(UsrCod,UsrID),INDEX(UsrID));
INSERT INTO usr_IDs (UsrCod,UsrID,CreatTime) SELECT UsrCod,UsrID,NOW() FROM usr_data WHERE UsrID<>'';

----- 2013-11-23, swad13.15.1

ALTER TABLE usr_data DROP COLUMN UsrID;


----- 2013-12-04, swad13.18.2

Lo siguiente s�lo lo he hecho en swad.ugr.es. No hay que hacerlo en otros servidores.

UPDATE log_recent,usr_IDs SET log_recent.UsrCod=usr_IDs.UsrCod WHERE log_recent.UsrCod='-1' AND log_recent.UsrID<>'' AND log_recent.UsrID COLLATE latin1_spanish_ci=usr_IDs.UsrID;
UPDATE log,usr_IDs SET log.UsrCod=usr_IDs.UsrCod WHERE log.UsrCod='-1' AND log.UsrID<>'' AND log.UsrID COLLATE latin1_spanish_ci=usr_IDs.UsrID;


CREATE TABLE unused_codes (Cod INT NOT NULL, UNIQUE INDEX(Cod));

DROP PROCEDURE IF EXISTS fill_unused_codes;
DELIMITER |
CREATE PROCEDURE fill_unused_codes(CodStart INT, CodEnd INT)
BEGIN
  WHILE CodStart <= CodEnd DO
    INSERT INTO unused_codes (Cod) VALUES (CodStart);
    SET CodStart = CodStart + 1;
  END WHILE;
END;
|
DELIMITER ;
CALL fill_unused_codes(1,160000);

DELETE FROM unused_codes WHERE Cod IN (SELECT UsrCod FROM usr_IDs);
DELETE FROM unused_codes WHERE Cod IN (SELECT DISTINCT UsrCod FROM log WHERE UsrCod>0);

CREATE TABLE deleted_usr_IDs (UsrID CHAR(16) NOT NULL, UNIQUE INDEX(UsrID)) SELECT DISTINCT UsrID FROM log WHERE UsrCod<=0 AND UsrID<>'' AND Role>=1 AND ActCod<>6 AND ActCod<>794 AND UsrID COLLATE latin1_spanish_ci NOT IN (SELECT UsrID FROM usr_IDs);

ALTER TABLE deleted_usr_IDs ADD COLUMN Ind INT NOT NULL AUTO_INCREMENT AFTER UsrID,ADD PRIMARY KEY (Ind);
ALTER TABLE unused_codes ADD COLUMN Ind INT NOT NULL AUTO_INCREMENT AFTER Cod,ADD PRIMARY KEY (Ind);

CREATE TABLE deleted_users (UsrID CHAR(16) NOT NULL,UsrCod INT NOT NULL,UNIQUE INDEX(UsrID),UNIQUE INDEX(UsrCod));
INSERT INTO deleted_users SELECT deleted_usr_IDs.UsrID,unused_codes.Cod FROM deleted_usr_IDs,unused_codes WHERE deleted_usr_IDs.Ind=unused_codes.Ind;

UPDATE log,deleted_users SET log.UsrCod=deleted_users.UsrCod WHERE log.UsrCod='-1' AND log.UsrID<>'' AND log.UsrID COLLATE latin1_spanish_ci=deleted_users.UsrID;

-------


Tablas que hay que eliminar:
DROP TABLE unused_codes;
DROP TABLE deleted_usr_IDs;
DROP TABLE deleted_users;


----- 2013-12-05, swad13.18.3

ALTER TABLE log_recent DROP COLUMN UsrID;
ALTER TABLE log DROP COLUMN UsrID;

Antes del cambio:
-rw-rw---- 1 mysql mysql        8598 oct 25 00:27 log_banners.frm
-rw-rw---- 1 mysql mysql       42111 dic  5 23:48 log_banners.MYD
-rw-rw---- 1 mysql mysql      107520 dic  5 23:48 log_banners.MYI
-rw-rw---- 1 mysql mysql        8602 jul 20  2010 log_comments.frm
-rw-rw---- 1 mysql mysql   103757284 dic  6 00:27 log_comments.MYD
-rw-rw---- 1 mysql mysql    11695104 dic  6 00:27 log_comments.MYI
-rw-rw---- 1 mysql mysql        8920 sep 14 00:52 log.frm
-rw-rw---- 1 mysql mysql 18631671801 dic  6 00:29 log.MYD
-rw-rw---- 1 mysql mysql 21532697600 dic  6 00:29 log.MYI
-rw-rw---- 1 mysql mysql        8920 sep 14 00:48 log_recent.frm
-rw-rw---- 1 mysql mysql   139523244 dic  6 00:29 log_recent.MYD
-rw-rw---- 1 mysql mysql   184194048 dic  6 00:29 log_recent.MYI
-rw-rw---- 1 mysql mysql        8632 nov 17  2011 log_ws.frm
-rw-rw---- 1 mysql mysql    81722134 dic  6 00:29 log_ws.MYD
-rw-rw---- 1 mysql mysql   222124032 dic  6 00:29 log_ws.MYI

Despu�s del cambio:
-rw-rw---- 1 mysql mysql        8598 oct 25 00:27 log_banners.frm
-rw-rw---- 1 mysql mysql       42138 dic  6 01:47 log_banners.MYD
-rw-rw---- 1 mysql mysql      107520 dic  6 01:47 log_banners.MYI
-rw-rw---- 1 mysql mysql        8602 jul 20  2010 log_comments.frm
-rw-rw---- 1 mysql mysql   103757384 dic  6 00:33 log_comments.MYD
-rw-rw---- 1 mysql mysql    11695104 dic  6 00:33 log_comments.MYI
-rw-rw---- 1 mysql mysql        8888 dic  6 02:01 log.frm
-rw-rw---- 1 mysql mysql 14311515005 dic  6 02:28 log.MYD
-rw-rw---- 1 mysql mysql 17929103360 dic  6 03:07 log.MYI
-rw-rw---- 1 mysql mysql        8888 dic  6 02:01 log_recent.frm
-rw-rw---- 1 mysql mysql    79888702 dic  6 02:01 log_recent.MYD
-rw-rw---- 1 mysql mysql   103815168 dic  6 02:01 log_recent.MYI
-rw-rw---- 1 mysql mysql        8632 nov 17  2011 log_ws.frm
-rw-rw---- 1 mysql mysql    81750682 dic  6 02:00 log_ws.MYD
-rw-rw---- 1 mysql mysql   222208000 dic  6 02:00 log_ws.MYI

----- 2013-12-10, swad13.18.7

ALTER TABLE usr_IDs ADD COLUMN Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER CreatTime;
UPDATE usr_IDs SET Confirmed='Y';

----- 2013-12-11, swad13.19

UPDATE usr_last SET LastTab=12 WHERE LastTab=8;
UPDATE usr_last SET LastTab=11 WHERE LastTab=7;
UPDATE usr_last SET LastTab=10 WHERE LastTab=6;
UPDATE usr_last SET LastTab=9 WHERE LastTab=5;
UPDATE usr_last SET LastTab=8 WHERE LastTab=4;
UPDATE usr_last SET LastTab=7 WHERE LastTab=3;
UPDATE usr_last SET LastTab=6 WHERE LastTab=2;

----- 2013-12-12, swad13.20

UPDATE actions SET Obsolete='Y' WHERE ActCod='1023';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1149','es','N','Ver configuraci&oacute;n titulaci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1150','es','N','Imprimir configuraci&oacute;n titulaci&oacute;n');

----- 2013-12-13, swad13.21

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1151','es','N','Ver informaci&oacute;n centro');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1152','es','N','Imprimir informaci&oacute;n centro');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1153','es','N','Ver informaci&oacute;n instituci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1154','es','N','Imprimir informaci&oacute;n instituci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1155','es','N','Ver informaci&oacute;n instituci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1156','es','N','Imprimir informaci&oacute;n instituci&oacute;n');

----- 2013-12-13, swad13.21

ALTER TABLE places ADD COLUMN InsCod INT NOT NULL AFTER PlcCod, ADD INDEX (InsCod);
ALTER TABLE holidays ADD COLUMN InsCod INT NOT NULL AFTER HldCod, ADD INDEX (InsCod);
ALTER TABLE centres ADD COLUMN PlcCod INT NOT NULL DEFAULT -1 AFTER InsCod, ADD INDEX (PlcCod);
DROP TABLE IF EXISTS tmp_centres_places;
CREATE TEMPORARY TABLE tmp_centres_places (InsCod INT NOT NULL,CtrCod INT NOT NULL,PlcCod INT NOT NULL,INDEX(InsCod),INDEX (CtrCod),INDEX(PlcCod)) SELECT DISTINCTROW centres.InsCod,centres.CtrCod,degrees.PlcCod FROM degrees,centres WHERE degrees.CtrCod=centres.CtrCod ORDER BY centres.InsCod,centres.CtrCod,degrees.PlcCod DESC;
UPDATE centres,tmp_centres_places SET centres.PlcCod=tmp_centres_places.PlcCod WHERE centres.CtrCod=tmp_centres_places.CtrCod AND centres.InsCod=tmp_centres_places.InsCod;
DROP TABLE tmp_centres_places;
ALTER TABLE degrees DROP COLUMN PlcCod;
// ugr.es:
UPDATE places SET InsCod='1';
UPDATE holidays SET InsCod='1';
// una.py:
UPDATE places SET InsCod='92';
UPDATE holidays SET InsCod='92';

----- 2013-12-18, swad13.24

ALTER TABLE countries ADD COLUMN MapAttribution VARCHAR(255) NOT NULL AFTER Alpha2;
ALTER TABLE countries ADD COLUMN WWW_ca VARCHAR(255) NOT NULL AFTER Name_pt;
ALTER TABLE countries ADD COLUMN WWW_de VARCHAR(255) NOT NULL AFTER WWW_ca;
ALTER TABLE countries ADD COLUMN WWW_en VARCHAR(255) NOT NULL AFTER WWW_de;
ALTER TABLE countries ADD COLUMN WWW_es VARCHAR(255) NOT NULL AFTER WWW_en;
ALTER TABLE countries ADD COLUMN WWW_fr VARCHAR(255) NOT NULL AFTER WWW_es;
ALTER TABLE countries ADD COLUMN WWW_gn VARCHAR(255) NOT NULL AFTER WWW_fr;
ALTER TABLE countries ADD COLUMN WWW_it VARCHAR(255) NOT NULL AFTER WWW_gn;
ALTER TABLE countries ADD COLUMN WWW_pl VARCHAR(255) NOT NULL AFTER WWW_it;
ALTER TABLE countries ADD COLUMN WWW_pt VARCHAR(255) NOT NULL AFTER WWW_pl;

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1157','es','N','Cambiar web pa&iacute;s');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1158','es','N','Cambiar atribuci&oacute;n mapa pa&iacute;s');

----- 2013-12-20, swad13.25

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1159','es','N','Cambiar atribuci&oacute;n foto centro');

----- 2013-12-21, swad13.25.1

ALTER TABLE centres ADD COLUMN PhotoAttribution VARCHAR(255) NOT NULL AFTER WWW;

----- 2013-12-21, swad13.26

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1160','es','N','Solicitar env&iacute;o de foto del centro');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1161','es','N','Enviar foto del centro');

----- 2013-12-21, swad13.26.1

ALTER TABLE centres CHANGE COLUMN PhotoAttribution PhotoAttribution TEXT NOT NULL;
ALTER TABLE countries CHANGE COLUMN MapAttribution MapAttribution TEXT NOT NULL;

----- 2013-12-29, swad13.28

UPDATE actions SET Obsolete='Y' WHERE ActCod='21';

----- 2013-12-29, swad13.29

UPDATE actions SET Obsolete='Y' WHERE ActCod IN ('57','61','229','231','580','581','582','585');

----- 2013-12-29, swad13.30.2

UPDATE actions SET Obsolete='Y' WHERE ActCod='1004';

----- 2014-01-01, swad13.32

UPDATE actions SET Obsolete='Y' WHERE ActCod='306';

----- 2014-01-20, swad13.40

UPDATE actions SET Obsolete='Y' WHERE ActCod IN ('842','1027','1026','1050','1012','988','1010');

----- 2014-01-23, swad13.41

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1162','es','N','Solicitar la creaci&oacute;n de una cuenta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1163','es','N','Crear una nueva cuenta');

----- 2014-01-26, swad13.41.1

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1164','es','N','Confirmar creaci&oacute;n de nueva cuenta');

----- 2014-01-29, swad13.43.1

DELETE FROM actions WHERE ActCod='1164';
UPDATE actions SET Obsolete='Y' WHERE ActCod='1162';

----- 2014-02-08, swad13.48

5 --> 8
4 --> 5
3 --> 4
2 --> 3
1 --> 2

UPDATE connected SET RoleInLastCrs='8' WHERE RoleInLastCrs='5';
UPDATE connected SET RoleInLastCrs=RoleInLastCrs+1 WHERE RoleInLastCrs>='1' AND RoleInLastCrs<='4';

UPDATE crs_usr SET Role='4' WHERE Role='3';
UPDATE crs_usr SET Role='3' WHERE Role='2';

UPDATE crs_usr_requests SET Role='4' WHERE Role='3';
UPDATE crs_usr_requests SET Role='3' WHERE Role='2';

UPDATE imported_sessions SET ImportedRole='4' WHERE ImportedRole='3';
UPDATE imported_sessions SET ImportedRole='3' WHERE ImportedRole='2';

UPDATE sessions SET Role='8' WHERE Role='5';
UPDATE sessions SET Role=Role+1 WHERE Role>='1' AND Role<='4';

UPDATE surveys SET Roles=Roles*2;

UPDATE log_recent SET Role='8' WHERE Role='5';
UPDATE log_recent SET Role=Role+1 WHERE Role>='1' AND Role<='4';

UPDATE log SET Role='8' WHERE Role='5';
UPDATE log SET Role=Role+1 WHERE Role>='1' AND Role<='4';

----- 2014-02-15, swad13.48

ALTER TABLE usr_data CHANGE Surname1 Surname1 VARCHAR(32) COLLATE latin1_spanish_ci NOT NULL;
ALTER TABLE usr_data CHANGE Surname2 Surname2 VARCHAR(32) COLLATE latin1_spanish_ci NOT NULL;
ALTER TABLE usr_data CHANGE FirstName FirstName VARCHAR(32) COLLATE latin1_spanish_ci NOT NULL;
ALTER TABLE usr_data CHANGE Office Office VARCHAR(127) NOT NULL;
ALTER TABLE usr_data CHANGE LocalAddress LocalAddress VARCHAR(127) NOT NULL;
ALTER TABLE usr_data CHANGE FamilyAddress FamilyAddress VARCHAR(127) NOT NULL;
ALTER TABLE usr_data CHANGE OriginPlace OriginPlace VARCHAR(127) NOT NULL;

ALTER TABLE exam_announcements CHANGE CrsFullName CrsFullName VARCHAR(127) NOT NULL;
ALTER TABLE exam_announcements CHANGE ExamSession ExamSession VARCHAR(127) NOT NULL;

ALTER TABLE mail_domains CHANGE Domain Domain VARCHAR(127) NOT NULL;
ALTER TABLE mail_domains CHANGE Info Info VARCHAR(127) NOT NULL;

ALTER TABLE usr_emails CHANGE E_mail E_mail VARCHAR(127) COLLATE latin1_general_ci NOT NULL;
ALTER TABLE pending_emails CHANGE E_mail E_mail VARCHAR(127) COLLATE latin1_general_ci NOT NULL;

ALTER TABLE imported_students CHANGE Surname1 Surname1 VARCHAR(32) NOT NULL;
ALTER TABLE imported_students CHANGE Surname2 Surname2 VARCHAR(32) NOT NULL;
ALTER TABLE imported_students CHANGE FirstName FirstName VARCHAR(32) NOT NULL;
ALTER TABLE imported_students CHANGE E_mail E_mail VARCHAR(32) NOT NULL;

ALTER TABLE courses CHANGE FullName FullName VARCHAR(127) COLLATE latin1_spanish_ci NOT NULL;
ALTER TABLE degrees CHANGE FullName FullName VARCHAR(127) COLLATE latin1_spanish_ci NOT NULL;
ALTER TABLE centres CHANGE FullName FullName VARCHAR(127) COLLATE latin1_spanish_ci NOT NULL;
ALTER TABLE institutions CHANGE FullName FullName VARCHAR(127) COLLATE latin1_spanish_ci NOT NULL;
ALTER TABLE departments CHANGE FullName FullName VARCHAR(127) NOT NULL;
ALTER TABLE deg_types CHANGE DegTypName DegTypName VARCHAR(32) NOT NULL;
ALTER TABLE imported_groups CHANGE DegName DegName VARCHAR(127) NOT NULL;
ALTER TABLE imported_groups CHANGE CrsName CrsName VARCHAR(127) NOT NULL;
ALTER TABLE links CHANGE FullName FullName VARCHAR(127) NOT NULL;
ALTER TABLE places CHANGE FullName FullName VARCHAR(127) NOT NULL;
ALTER TABLE plugins CHANGE Name Name VARCHAR(127) NOT NULL;
ALTER TABLE timetable_crs CHANGE Place Place VARCHAR(127) NOT NULL;
ALTER TABLE timetable_tut CHANGE Place Place VARCHAR(127) NOT NULL;
ALTER TABLE banners CHANGE FullName FullName VARCHAR(127) NOT NULL;
ALTER TABLE holidays CHANGE Name Name VARCHAR(127) NOT NULL;

----- 2014-03-29, swad13.56

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1164','es','N','Mostrar aviso completo');

----- 2014-03-31, swad13.56.2

DROP INDEX UsrCod ON usr_nicknames;
CREATE UNIQUE INDEX UsrCod ON usr_nicknames (UsrCod,Nickname);
DROP INDEX Nickname ON usr_nicknames;
CREATE UNIQUE INDEX Nickname ON usr_nicknames (Nickname);

----- 2014-04-07, swad13.56.5

ALTER TABLE usr_data ADD COLUMN InsCtyCod INT NOT NULL DEFAULT -1 AFTER CtyCod;
ALTER TABLE usr_data ADD INDEX (InsCtyCod);
UPDATE usr_data,institutions SET usr_data.InsCtyCod=institutions.CtyCod WHERE usr_data.InsCod=institutions.InsCod;

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1165','es','N','Editar instituci&oacute;n, centro, departamento');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1166','es','N','Cambiar pa&iacute;s instituci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1167','es','N','Cambiar instituci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1168','es','N','Cambiar centro');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1169','es','N','Cambiar departamento');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1170','es','N','Cambiar despacho');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1171','es','N','Cambiar tel&eacute;fono despacho');

----- 2014-04-08, swad13.58

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1172','es','N','Editar redes sociales');

----- 2014-04-09, swad13.58.1

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1173','es','N','Cambiar redes sociales');

----- 2014-04-13, swad13.58.4

CREATE TABLE IF NOT EXISTS usr_webs (
	UsrCod INT NOT NULL,
	Web ENUM('www','facebook','flickr','googleplus','linkedin','skype','twitter','youtube') NOT NULL,
	URL VARCHAR(255) NOT NULL,
	UNIQUE INDEX(UsrCod,Web));

----- 2014-04-15, swad13.58.7

DELETE FROM usr_webs WHERE Web='skype';
ALTER TABLE usr_webs CHANGE Web Web ENUM('www','facebook','flickr','googleplus','linkedin','twitter','youtube') NOT NULL;

----- 2014-04-15, swad13.58.8

ALTER TABLE usr_webs CHANGE Web Web ENUM('www','facebook','flickr','googleplus','instagram','linkedin','pinterest','slideshare','twitter','youtube') NOT NULL;

----- 2014-04-15, swad13.59

REPLACE INTO usr_webs SELECT UsrCod,'www',WWW FROM usr_data WHERE WWW LIKE 'http%';
REPLACE INTO usr_webs SELECT UsrCod,'twitter',CONCAT('https://twitter.com/',Twitter) FROM usr_data WHERE Twitter<>'' AND Twitter NOT LIKE 'http%';

----- 2014-04-20, swad13.59.2

ALTER TABLE usr_webs CHANGE Web Web ENUM('www','facebook','flickr','googleplus','instagram','linkedin','pinterest','slideshare','tumblr','twitter','youtube') NOT NULL;

----- 2014-04-21, swad13.61

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1174','es','N','Ver ficha estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1175','es','N','Ver ficha profesor');

----- 2014-04-22, swad13.61.1

ALTER TABLE usr_webs CHANGE Web Web ENUM('www','edmodo','facebook','flickr','googleplus','instagram','linkedin','pinterest','slideshare','tumblr','twitter','youtube') NOT NULL;

----- 2014-04-26, swad13.62.1

ALTER TABLE usr_webs CHANGE Web Web ENUM('www','edmodo','facebook','flickr','googleplus','googlescholar','instagram','linkedin','pinterest','researchgate','slideshare','tumblr','twitter','wikipedia','youtube') NOT NULL;

----- 2014-05-30, swad13.64.9

ALTER TABLE institutions CHANGE Logo Logo VARCHAR(32) NOT NULL;

-----

CREATE TABLE new_institutions LIKE institutions;
ALTER TABLE new_institutions DROP COLUMN InsCod;
GRANT FILE ON *.* TO swad@localhost;
LOAD DATA INFILE '/tmp/new_institutions_2.txt' INTO TABLE new_institutions FIELDS TERMINATED BY '\t';
INSERT INTO institutions (CtyCod,ShortName,FullName,Logo,WWW) SELECT CtyCod,ShortName,FullName,Logo,WWW FROM new_institutions WHERE ShortName NOT IN (SELECT ShortName FROM institutions);

----- 2014-06-06, swad13.67

ALTER TABLE institutions CHANGE COLUMN FullName FullName TEXT NOT NULL;

----- 2014-06-07, swad13.69

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1078','es','N','Ver documentos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1079','es','N','Ver calificaciones');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1186','es','N','Ver orla o lista de invitados');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1187','es','N','Ver fichas invitados');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1188','es','N','Imprimir fichas invitados');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1189','es','N','Ver lista invitados');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1190','es','N','Imprimir orla ionvitados');

----- 2014-06-25, swad13.71

UPDATE actions SET Obsolete='Y' WHERE ActCod IN ('267','268','463','466','467','475','499','500','525','526');

----- 2014-06-29, swad13.72

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1191','es','N','Habilitar arch/carp calif. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1192','es','N','Inhabilitar arch/carp calif. grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1193','es','N','Habilitar arch/carp calif. asig.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1194','es','N','Inhabilitar arch/carp calif. asig.');

----- 2014-07-02, swad13.72

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1195','es','N','Cambiar a ver documentos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1196','es','N','Cambiar a admin. documentos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1197','es','N','Cambiar a admin. archivos compartidos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1198','es','N','Cambiar a ver archivos calificaciones');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1199','es','N','Cambiar a admin. archivos calificaciones');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1078','es','N','Ver documentos asignatura');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1200','es','N','Ver documentos grupo');
UPDATE actions SET Txt='Administrar documentos asignatura' WHERE ActCod='12';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1201','es','N','Administrar documentos grupo');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1202','es','N','Administrar archivos compartidos asignatura');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1203','es','N','Administrar archivos compartidos grupo');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1079','es','N','Ver calificaciones asignatura');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1204','es','N','Ver calificaciones grupo');
UPDATE actions SET Txt='Administrar calificaciones asignatura' WHERE ActCod='284';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1205','es','N','Administrar calificaciones grupo');

----- 2014-07-04, swad13.74

ALTER TABLE degrees ADD COLUMN Status TINYINT NOT NULL DEFAULT 0 AFTER DegTypCod;
ALTER TABLE degrees ADD COLUMN RequesterUsrCod INT NOT NULL DEFAULT -1 AFTER Status;
CREATE INDEX Status ON degrees (Status);

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1206','es','N','Solicitar titulaci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1207','es','N','Cambiar estado titulaci&oacute;n');

----- 2014-07-05, swad13.74.2

UPDATE actions SET Txt='Ver instit., centros, titul. y asig. pendientes' WHERE ActCod='1060';

----- 2014-07-06, swad13.75

ALTER TABLE centres ADD COLUMN Status TINYINT NOT NULL DEFAULT 0 AFTER PlcCod;
ALTER TABLE centres ADD COLUMN RequesterUsrCod INT NOT NULL DEFAULT -1 AFTER Status;
CREATE INDEX Status ON centres (Status);
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1208','es','N','Solicitar centro');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1209','es','N','Cambiar estado centro');

----- 2014-07-08, swad13.76

ALTER TABLE institutions ADD COLUMN Status TINYINT NOT NULL DEFAULT 0 AFTER CtyCod;
ALTER TABLE institutions ADD COLUMN RequesterUsrCod INT NOT NULL DEFAULT -1 AFTER Status;
CREATE INDEX Status ON institutions (Status);
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1210','es','N','Solicitar instituci&oacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1211','es','N','Cambiar estado instituci&oacute;n');

----- 2014-07-21, swad13.80

UPDATE sessions SET WhatToSearch=WhatToSearch+3 WHERE WhatToSearch>0;
UPDATE usr_last SET WhatToSearch=WhatToSearch+3 WHERE WhatToSearch>0;

----- 2014-09-11, swad13.85

ALTER TABLE banners ADD COLUMN Hidden ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER BanCod;
CREATE INDEX Hidden ON banners (Hidden);
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1212','es','N','Mostrar banner');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1213','es','N','Ocultar banner');

---------------------

UPDATE usr_data SET Password='',Surname1='',Surname2='',FirstName='',Sex='unknown',Layout='0',Theme='white',IconSet='awesome',Language='es',WWW='',Twitter='',Skype='',Photo='',PublicPhoto='N',CtyCod='-1',InsCtyCod='-1',InsCod='-1',DptCod='-1',CtrCod='-1',Office='',OfficePhone='',LocalAddress='',LocalPhone='',FamilyAddress='',FamilyPhone='',OriginPlace='',Birthday='0000-00-00',Comments='',SideCols='3',SecureIcons='Y',NotifNtfEvents='-1',EmailNtfEvents='0' WHERE UsrCod='164634';

----- 2014-09-26, swad14.0

UPDATE sessions SET WhatToSearch=WhatToSearch+1 WHERE WhatToSearch>=7;
UPDATE usr_last SET WhatToSearch=WhatToSearch+1 WHERE WhatToSearch>=7;

----- 2014-10-05, swad14.1.2

ALTER TABLE debug ADD COLUMN DbgCod INT NOT NULL AUTO_INCREMENT FIRST



----- 2014-10-07, swad14.4

UPDATE actions SET Txt='Subir archivo doc. asg. (ant.)'         WHERE ActCod='482' AND Language='es';
UPDATE actions SET Txt='Subir archivo doc. grp. (ant.)'         WHERE ActCod='483' AND Language='es';
UPDATE actions SET Txt='Subir archivo com&uacute;n asg. (ant.)' WHERE ActCod='326' AND Language='es';
UPDATE actions SET Txt='Subir archivo com&uacute;n grp. (ant.)' WHERE ActCod='335' AND Language='es';
UPDATE actions SET Txt='Subir archivo mis activid. (ant.)'      WHERE ActCod='832' AND Language='es';
UPDATE actions SET Txt='Subir archivo mis trabajos (ant.)'      WHERE ActCod='148' AND Language='es';
UPDATE actions SET Txt='Subir archivo activid. asg. (ant.)'     WHERE ActCod='846' AND Language='es';
UPDATE actions SET Txt='Subir archivo trabajos asg. (ant.)'     WHERE ActCod='207' AND Language='es';
UPDATE actions SET Txt='Subir archivo calif. asg. (ant.)'       WHERE ActCod='516' AND Language='es';
UPDATE actions SET Txt='Subir archivo calif. grp. (ant.)'       WHERE ActCod='514' AND Language='es';
UPDATE actions SET Txt='Subir archivo a malet&iacute;n (ant.)'  WHERE ActCod='153' AND Language='es';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1214','es','N','Subir archivo doc. asg.'        );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1215','es','N','Subir archivo doc. grp.'        );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1216','es','N','Subir archivo com&uacute;n asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1217','es','N','Subir archivo com&uacute;n grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1218','es','N','Subir archivo mis activid.'     );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1219','es','N','Subir archivo mis trabajos'     );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1220','es','N','Subir archivo activid. asg.'    );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1221','es','N','Subir archivo trabajos asg.'    );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1222','es','N','Subir archivo calif. asg.'      );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1223','es','N','Subir archivo calif. grp.'      );
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1224','es','N','Subir archivo a malet&iacute;n' );

----- 2014-10-20, swad14.8.2

UPDATE tst_questions SET Feedback='' WHERE Feedback='(null)';

----- 2014-10-21, swad14.9

UPDATE actions SET Obsolete='Y' WHERE ActCod='760';

----- 2014-10-26, swad14.10

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1225','es','N','Crear enlace documentos asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1226','es','N','Crear enlace com&uacute;n asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1227','es','N','Crear enlace com&uacute;n grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1228','es','N','Crear enlace mis trabajos');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1229','es','N','Crear enlace trabajos asg.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1230','es','N','Crear enlace en malet&iacute;n');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1231','es','N','Crear enlace documentos grp.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1232','es','N','Crear enlace mis actividades');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1233','es','N','Crear enlace activid. asg.');

----- 2014-10-28, swad14.11.2

ALTER TABLE clipboard ADD COLUMN FileType TINYINT NOT NULL DEFAULT 0 AFTER WorksUsrCod;

----- 2014-10-29, swad14.11.3

ALTER TABLE files ADD COLUMN FileType TINYINT NOT NULL DEFAULT 0 AFTER PublisherUsrCod;

----- 2014-11-06, swad14.15.2

SELECT students.UsrCod,students.Present,students.CommentStd,students.CommentTch FROM (SELECT UsrCod,Present,CommentStd,CommentTch FROM att_usr WHERE AttCod='1' UNION SELECT crs_usr.UsrCod AS UsrCod,'N' AS Present,'' AS CommentStd,'' AS CommentTch FROM att_events,crs_usr WHERE att_events.AttCod='1' AND att_events.CrsCod=crs_usr.CrsCod AND crs_usr.Role='3' AND crs_usr.UsrCod NOT IN (SELECT UsrCod FROM att_usr WHERE AttCod='1')) AS students,usr_data WHERE students.UsrCod=usr_data.UsrCod ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName;
                  
SELECT students.UsrCod,students.Present,students.CommentStd,students.CommentTch FROM (SELECT UsrCod,Present,CommentStd,CommentTch FROM att_usr WHERE AttCod='1' UNION SELECT DISTINCT crs_grp_usr.UsrCod AS UsrCod,'N' AS Present,'' AS CommentStd,'' AS CommentTch FROM att_grp,crs_grp,crs_grp_types,crs_usr,crs_grp_usr WHERE att_grp.AttCod='1' AND att_grp.GrpCod=crs_grp.GrpCod AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod AND crs_grp_types.CrsCod=crs_usr.CrsCod AND crs_usr.Role='3' AND crs_usr.UsrCod=crs_grp_usr.UsrCod AND crs_grp_usr.GrpCod=att_grp.GrpCod AND crs_grp_usr.UsrCod NOT IN (SELECT UsrCod FROM att_usr WHERE AttCod='1')) AS students,usr_data WHERE students.UsrCod=usr_data.UsrCod ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName;
                  
SELECT AttCod,UsrCod,UNIX_TIMESTAMP(StartTime),UNIX_TIMESTAMP(EndTime),CommentTchVisible,Title,Txt FROM att_events WHERE CrsCod='1' AND Hidden='N' ORDER BY StartTime DESC,EndTime DESC,Title DESC;

SELECT AttCod,UsrCod,UNIX_TIMESTAMP(StartTime) AS ST,UNIX_TIMESTAMP(EndTime) AS ET,CommentTchVisible,Title,Txt FROM att_events WHERE CrsCod='4270' AND Hidden='N' ORDER BY ST DESC,ET DESC,Title DESC;

----- 2014-11-19, swad14.24
         
CREATE TABLE IF NOT EXISTS announcements (AnnCod INT NOT NULL AUTO_INCREMENT,Roles INT NOT NULL DEFAULT 0,Content TEXT NOT NULL,UNIQUE INDEX(AnnCod));
CREATE TABLE IF NOT EXISTS ann_view (AnnCod INT NOT NULL,UsrCod INT NOT NULL,UNIQUE INDEX(AnnCod,UsrCod));

----- 2014-11-19, swad14.24.1

ALTER TABLE announcements ADD COLUMN Subject TEXT NOT NULL AFTER Roles;

INSERT INTO announcements (Roles,Subject,Content) VALUES ('8','ENCUESTA AN&Oacute;NIMA','Por favor, dedica unos minutos a rellenar el cuestionario siguiente sobre el uso de la plataforma SWAD:<br />http://www.encuestafacil.com/RespWeb/Qn.aspx?EID=1843919<br /><br />Los resultados se utilizar&aacute;n en una tesis doctoral del Departamento de M&eacute;todos de Investigaci&oacute;n y Diagn&oacute;stico en Educaci&oacute;n de la Universidad de Granada, en la que se est&aacute; estudiando el dise&ntilde;o y uso de plataformas educativas.');
INSERT INTO announcements (Roles,Subject,Content) VALUES ('16','ENCUESTA AN&Oacute;NIMA','Por favor, dedique unos minutos a rellenar el cuestionario siguiente sobre el uso de la plataforma SWAD:<br />http://www.encuestafacil.com/RespWeb/Qn.aspx?EID=1843918<br /><br />Los resultados se utilizar&aacute;n en una tesis doctoral del Departamento de M&eacute;todos de Investigaci&oacute;n y Diagn&oacute;stico en Educaci&oacute;n de la Universidad de Granada, en la que se est&aacute; estudiando el dise&ntilde;o y uso de plataformas educativas.');





SELECT tst_questions.QstCod,tst_questions.AnsType,tst_questions.Shuffle,tst_questions.Stem,tst_questions.Feedback FROM tst_questions,tst_question_tags,tst_tags WHERE tst_questions.CrsCod='5432' AND tst_questions.QstCod NOT IN (SELECT tst_question_tags.QstCod FROM tst_tags,tst_question_tags WHERE tst_tags.CrsCod='5432' AND tst_tags.TagHidden='Y' AND tst_tags.TagCod=tst_question_tags.TagCod) AND tst_questions.QstCod=tst_question_tags.QstCod AND tst_question_tags.TagCod=tst_tags.TagCod AND tst_tags.CrsCod='5432' AND (UNIX_TIMESTAMP(tst_questions.EditTime)>='0' OR UNIX_TIMESTAMP(tst_tags.ChangeTime)>='0') ORDER BY QstCod;

----- 2015-01-01, swad14.51

ALTER TABLE usr_data ADD COLUMN Menu TINYINT NOT NULL DEFAULT 0 AFTER Comments;
ALTER TABLE usr_data ADD INDEX (Menu);
UPDATE usr_data SET Menu=1;
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1243','es','N','Cambiar men&uacute;');
ALTER TABLE IP_prefs ADD COLUMN Menu TINYINT NOT NULL DEFAULT 0 AFTER IconSet;

----- 2015-01-04, swad14.54.1

ALTER TABLE usr_data DROP COLUMN WWW,DROP COLUMN Twitter,DROP COLUMN Skype;


SELECT DegCod,CtrCod,DegTypCod,Status,RequesterUsrCod,ShortName,FullName,FirstYear,LastYear,OptYear,WWW FROM degrees;

----- 2015-01-20, swad14.60

ALTER TABLE clipboard ADD COLUMN InsCod INT NOT NULL DEFAULT -1 AFTER FileBrowser, ADD INDEX (InsCod);
ALTER TABLE clipboard ADD COLUMN CtrCod INT NOT NULL DEFAULT -1 AFTER InsCod, ADD INDEX (CtrCod);
ALTER TABLE clipboard ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER CtrCod, ADD INDEX (DegCod);

----- 2015-01-20, swad14.61

ALTER TABLE files ADD COLUMN InsCod INT NOT NULL DEFAULT -1 AFTER FilCod;
ALTER TABLE files ADD COLUMN CtrCod INT NOT NULL DEFAULT -1 AFTER InsCod;
ALTER TABLE files ADD COLUMN DegCod INT NOT NULL DEFAULT -1 AFTER CtrCod;
DROP INDEX CrsCod ON files;
DROP INDEX CrsCod_GrpCod_FileBrowser ON files;
CREATE INDEX Location ON files (InsCod,CtrCod,DegCod,CrsCod,GrpCod,FileBrowser);

----- 2015-01-21, swad14.63

CREATE TABLE IF NOT EXISTS file_browser_last (UsrCod INT NOT NULL,FileBrowser TINYINT NOT NULL,Cod INT NOT NULL DEFAULT -1,LastClick DATETIME NOT NULL,UNIQUE INDEX(UsrCod,FileBrowser,Cod));

----- 2015-01-22, swad14.63.5

INSERT INTO file_browser_last (UsrCod,FileBrowser,Cod,LastClick) SELECT UsrCod,'9','-1',LastAccBriefcase FROM usr_last WHERE LastAccBriefcase>0;
ALTER TABLE usr_last DROP COLUMN LastAccBriefcase;

----- 2015-01-26, swad14.69.1

CREATE TABLE files_backup LIKE files;
INSERT INTO files_backup SELECT * FROM files;

CREATE TABLE file_browser_size_backup LIKE file_browser_size;
INSERT INTO file_browser_size_backup SELECT * FROM file_browser_size;

CREATE TABLE expanded_folders_backup LIKE expanded_folders;
INSERT INTO expanded_folders_backup SELECT * FROM expanded_folders;

CREATE TABLE clipboard_backup LIKE clipboard;
INSERT INTO clipboard_backup SELECT * FROM clipboard;

----- 2015-01-29, swad14.71

CREATE TABLE IF NOT EXISTS admin (UsrCod INT NOT NULL,Scope ENUM('Sys','Ins','Ctr','Deg') NOT NULL,Cod INT NOT NULL,UNIQUE INDEX(UsrCod,Scope,Cod));
INSERT INTO admin (UsrCod,Scope,Cod) SELECT UsrCod,'Deg',DegCod FROM deg_admin WHERE DegCod>'0';
INSERT INTO admin (UsrCod,Scope,Cod) SELECT UsrCod,'Sys',DegCod FROM deg_admin WHERE DegCod='-2';
DROP TABLE deg_admin;

----- 2015-03-06, swad14.78

ALTER TABLE usr_data ADD COLUMN PhotoVisibility ENUM('user','course','system','world') NOT NULL DEFAULT 'user' AFTER PublicPhoto;
UPDATE usr_data SET PhotoVisibility='user' WHERE PublicPhoto='N';
UPDATE usr_data SET PhotoVisibility='system' WHERE PublicPhoto='Y';

----- 2015-03-09, swad14.82

CREATE TABLE IF NOT EXISTS usr_figures (UsrCod INT NOT NULL,FirstClickTime DATETIME NOT NULL,NumClicks INT NOT NULL DEFAULT 0,PRIMARY KEY(UsrCod));




SELECT * FROM usr_figures WHERE NumClicks>='0' AND FirstClickTime>'0';

SELECT COUNT(*)+1 FROM (SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1) AS NumClicksPerDay FROM (SELECT UsrCod,COUNT(*) AS N FROM msg_snt_deleted GROUP BY UsrCod) UNION (SELECT UsrCod,'0' FROM usr_figures WHERE UsrCod NOT IN (SELECT UsrCod FROM msg_snt_deleted))usr_figures WHERE NumClicks>='0' AND UNIX_TIMESTAMP(FirstClickTime)>'0') AS TableNumClicksPerDay WHERE NumClicksPerDay>(SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1) FROM usr_figures WHERE UsrCod='1' AND NumClicks>='0' AND UNIX_TIMESTAMP(FirstClickTime)>'0');
               

----- 2015-03-15, swad14.92.2

INSERT INTO usr_figures (UsrCod,FirstClickTime,NumClicks,NumFileViews,NumForPst,NumMsgSnt) SELECT UsrCod,0,-1,-1,-1,-1 FROM usr_data WHERE UsrCod>0 AND UsrCod NOT IN (SELECT UsrCod FROM usr_figures);

---------------

Hecho:
UPDATE usr_figures,((SELECT UsrCod,COUNT(*) AS N FROM forum_post WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX GROUP BY UsrCod) UNION (SELECT UsrCod,'0' AS N FROM usr_figures WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX AND UsrCod NOT IN (SELECT UsrCod FROM forum_post))) AS FP SET usr_figures.NumForPst=FP.N WHERE usr_figures.UsrCod>=@USRCODMIN AND usr_figures.UsrCod<=@USRCODMAX AND usr_figures.NumForPst<0 AND usr_figures.UsrCod=FP.UsrCod;
UPDATE usr_figures,((SELECT UsrCod,COUNT(*) AS N FROM log WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX GROUP BY UsrCod) UNION (SELECT UsrCod,'0' AS N FROM usr_figures WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX AND UsrCod NOT IN (SELECT UsrCod FROM log))) AS NC SET usr_figures.NumClicks=NC.N WHERE usr_figures.UsrCod>=@USRCODMIN AND usr_figures.UsrCod<=@USRCODMAX AND usr_figures.NumClicks<0 AND usr_figures.UsrCod=NC.UsrCod;
UPDATE usr_figures,((SELECT UsrCod,COUNT(*) AS N FROM msg_snt WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX GROUP BY UsrCod) UNION (SELECT UsrCod,'0' AS N FROM usr_figures WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX AND UsrCod NOT IN (SELECT UsrCod FROM msg_snt))) AS MS,((SELECT UsrCod,COUNT(*) AS N FROM msg_snt_deleted WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX GROUP BY UsrCod) UNION (SELECT UsrCod,'0' AS N FROM usr_figures WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX AND UsrCod NOT IN (SELECT UsrCod FROM msg_snt_deleted))) AS MSD SET usr_figures.NumMsgSnt=MS.N+MSD.N WHERE usr_figures.UsrCod>=@USRCODMIN AND usr_figures.UsrCod<=@USRCODMAX AND usr_figures.NumMsgSnt<0 AND usr_figures.UsrCod=MS.UsrCod AND usr_figures.UsrCod=MSD.UsrCod;
SET @USRCODMIN=1;
SET @USRCODMAX=500000;
UPDATE usr_figures,((SELECT UsrCod,SUM(NumViews) AS N FROM file_view WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX GROUP BY UsrCod) UNION (SELECT UsrCod,'0' AS N FROM usr_figures WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX AND UsrCod NOT IN (SELECT UsrCod FROM file_view))) AS FV SET usr_figures.NumFileViews=FV.N WHERE usr_figures.UsrCod>=@USRCODMIN AND usr_figures.UsrCod<=@USRCODMAX AND usr_figures.NumFileViews<0 AND usr_figures.UsrCod=FV.UsrCod;

Haciendo:
SET @USRCODMIN=0;
SET @USRCODMAX=1000;
UPDATE usr_figures,((SELECT UsrCod,MIN(ClickTime) AS FCT FROM log WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX GROUP BY UsrCod) UNION (SELECT UsrCod,0 AS FCT FROM usr_figures WHERE UsrCod>=@USRCODMIN AND UsrCod<=@USRCODMAX AND UsrCod NOT IN (SELECT UsrCod FROM log))) AS CT SET usr_figures.FirstClickTime=CT.FCT WHERE usr_figures.UsrCod>=@USRCODMIN AND usr_figures.UsrCod<=@USRCODMAX AND usr_figures.FirstClickTime=0 AND usr_figures.UsrCod=CT.UsrCod;



---------------

CREATE TABLE IF NOT EXISTS usr_follow (FollowerCod INT NOT NULL,FollowedCod NIT NOT NULL,FollowTime DATETIME NOT NULL,UNIQUE INDEX (FollowerCod,FollowedCod),UNIQUE INDEX (FollowedCod,FollowerCod),INDEX (FollowTime));

---------------

CREATE INDEX UsrCod ON file_view (UsrCod);

SELECT FollowedCod,COUNT(FollowerCod) AS N FROM usr_follow GROUP BY FollowedCod ORDER BY N DESC,FollowedCod LIMIT 100;


----- 2015-03-23, swad14.100
/*
UPDATE notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE notif SET NotifyEvent=9 WHERE NotifyEvent=8;

UPDATE sta_notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE sta_notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE sta_notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE sta_notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE sta_notif SET NotifyEvent=9 WHERE NotifyEvent=8;

UPDATE usr_data SET NotifNtfEvents=(((NotifNtfEvents & ~0xFF) << 1) | (NotifNtfEvents & 0xFF) | 0x100);
UPDATE usr_data SET EmailNtfEvent =(((EmailNtfEvent  & ~0xFF) << 1) | (EmailNtfEvent  & 0xFF));
*/
UPDATE usr_data SET NotifNtfEvents=(NotifNtfEvents | 0x2000);


-----------------


CREATE TABLE IF NOT EXISTS usr_banned (
	UsrCod INT NOT NULL,
	UNIQUE INDEX(UsrCod));
	
	
	
SELECT DISTINCTROW tst_questions.QstCod,tst_questions.AnsType,tst_questions.Shuffle,tst_questions.Stem,tst_questions.Feedback FROM courses,tst_questions WHERE courses.DegCod IN ('252') AND courses.CrsCod=tst_questions.CrsCod AND tst_questions.AnsType='unique_choice' AND tst_questions.QstCod NOT IN (SELECT tst_question_tags.QstCod FROM courses,tst_tags,tst_question_tags WHERE courses.DegCod IN ('252') AND courses.CrsCod=tst_tags.CrsCod AND tst_tags.TagHidden='Y' AND tst_tags.TagCod=tst_question_tags.TagCod) ORDER BY RAND(NOW()) LIMIT 1;



SELECT DISTINCTROW tst_questions.QstCod,tst_questions.AnsType,tst_questions.Shuffle, tst_questions.Stem,tst_questions.Feedback,tst_questions.NumHits/tst_questions.Score AS S FROM courses,tst_questions WHERE courses.DegCod IN ('252') AND courses.CrsCod=tst_questions.CrsCod AND tst_questions.AnsType='unique_choice' AND tst_questions.NumHits>'0' AND tst_questions.QstCod NOT IN (SELECT tst_question_tags.QstCod FROM courses,tst_tags,tst_question_tags WHERE courses.DegCod IN ('252') AND courses.CrsCod=tst_tags.CrsCod AND tst_tags.TagHidden='Y' AND tst_tags.TagCod=tst_question_tags.TagCod) HAVING S>='0.0' AND S<='1.0' ORDER BY RAND(NOW()) LIMIT 1;



<form method="post" action="https://openswad.org/en" id="form_16">
	<input type="hidden" name="act" value="1163" />
	<div style="text-align:center;">
		<div class="FRAME10">
			<div class="TIT_TBL_10" style="text-align:center;">
				New on OpenSWAD? Sign up
			</div>
			<div style="text-align:center;">
				<button type="submit" class="BT_SUBMIT BT_CREATE">Create account</button>
			</div>
		</div>
	</div>
</form>

----- 2015-09-09, swad14.128

ALTER TABLE usr_webs CHANGE Web Web ENUM('www','500px','delicious','deviantart','diaspora','edmodo','facebook','flickr','foursquare','github','googleplus','googlescholar','instagram','linkedin','orcid','paperli','pinterest','quitter','researchgate','researcherid','scoopit','slideshare','storify','tumblr','twitter','wikipedia','youtube') NOT NULL;

----- 2015-09-10, swad14.128.1

ALTER TABLE usr_webs CHANGE Web Web ENUM('www','500px','delicious','deviantart','diaspora','edmodo','facebook','flickr','foursquare','github','gnusocial','googleplus','googlescholar','identica','instagram','linkedin','orcid','paperli','pinterest','quitter','researchgate','researcherid','scoopit','slideshare','storify','tumblr','twitter','wikipedia','youtube') NOT NULL;


----- 2015-09-17, swad14.134

UPDATE notif SET NotifyEvent=14 WHERE NotifyEvent=13;
UPDATE notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE notif SET NotifyEvent=9  WHERE NotifyEvent=8;
UPDATE notif SET NotifyEvent=8  WHERE NotifyEvent=7;

UPDATE sta_notif SET NotifyEvent=14 WHERE NotifyEvent=13;
UPDATE sta_notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE sta_notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE sta_notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE sta_notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE sta_notif SET NotifyEvent=9  WHERE NotifyEvent=8;
UPDATE sta_notif SET NotifyEvent=8  WHERE NotifyEvent=7;

UPDATE usr_data SET NotifNtfEvents=(((NotifNtfEvents & ~0x7F) << 1) | (NotifNtfEvents & 0x7F) | 0x80);
UPDATE usr_data SET EmailNtfEvents=(((EmailNtfEvents & ~0x7F) << 1) | (EmailNtfEvents & 0x7F));




   Ntf_EVENT_UNKNOWN			=  0,	// old  0

   /* Course tab */
   Ntf_EVENT_DOCUMENT_FILE		=  1,	// old  1
   Ntf_EVENT_SHARED_FILE		=  2,	// old  2

   /* Assessment tab */
   Ntf_EVENT_ASSIGNMENT			=  3,	// old  3
   Ntf_EVENT_CALL_FOR_EXAM		=  4,	// old  4
   Ntf_EVENT_MARKS_FILE			=  5,	// old  5

   /* Enrollment tab */
   Ntf_EVENT_ENROLLMENT_STUDENT		=  6,	// old  6
   Ntf_EVENT_ENROLLMENT_TEACHER		=  7,
   Ntf_EVENT_ENROLLMENT_REQUEST		=  8,	// old  7

   /* Messages tab */
   Ntf_EVENT_NOTICE			=  9,	// old  8
   Ntf_EVENT_FORUM_POST_COURSE		= 10,	// old  9
   Ntf_EVENT_FORUM_REPLY		= 11,	// old 10
   Ntf_EVENT_MESSAGE			= 12,	// old 11

   /* Statistics tab */
   Ntf_EVENT_SURVEY			= 13,	// old 12

   /* Profile tab */
   Ntf_EVENT_FOLLOWER			= 14,	// old 13




				
ALTER TABLE usr_webs CHANGE Web Web ENUM('www','500px','delicious','deviantart','diaspora','edmodo','facebook','flickr','foursquare','github','gnusocial','googleplus','googlescholar','identica','instagram','linkedin','orcid','paperli','pinterest','quitter','researchgate','researcherid','scoopit','slideshare','storify','tumblr','twitter','wikipedia','youtube') NOT NULL;

UPDATE actions SET Obsolete='Y' WHERE ActCod='177';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1414','es','N','Solicitar ID modif. invitado');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1415','es','N','Solicitar ID modif. estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1416','es','N','Solicitar ID modif. profesor');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1417','es','N','Solicitar ID modif. administrador');
UPDATE actions SET Obsolete='Y' WHERE ActCod='161';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1418','es','N','Confirmar modif. invitado');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1419','es','N','Confirmar modif. estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1420','es','N','Confirmar modif. profesor');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1421','es','N','Confirmar modif. administrador');
UPDATE actions SET Obsolete='Y' WHERE ActCod='439';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1422','es','N','Modificar datos invitado');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1423','es','N','Modificar datos estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1424','es','N','Modificar datos profesor');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1425','es','N','Modificar datos administrador');

UPDATE actions SET Obsolete='Y' WHERE ActCod='797';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1426','es','N','Administrar varios estudiantes');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1427','es','N','Administrar varios profesores');

UPDATE actions SET Obsolete='Y' WHERE ActCod='799';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1428','es','N','Inscribir/eliminar varios estudiantes');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1429','es','N','Inscribir/eliminar varios profesores');

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1430','es','N','Preguntar si eliminar mi cuenta');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1431','es','N','Eliminar mi cuenta');

UPDATE actions SET Obsolete='Y' WHERE ActCod='375';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1432','es','N','Solicitar cambio foto otro usr.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1433','es','N','Solicitar cambio foto estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1434','es','N','Solicitar cambio foto profesor');

UPDATE actions SET Obsolete='Y' WHERE ActCod='695';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1435','es','N','Detectar rostros foto otro usr.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1436','es','N','Detectar rostros foto estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1437','es','N','Detectar rostros foto profesor');
	
UPDATE actions SET Obsolete='Y' WHERE ActCod='374';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1438','es','N','Cambiar foto otro usr.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1439','es','N','Cambiar foto estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1440','es','N','Cambiar foto profesor');

UPDATE actions SET Obsolete='Y' WHERE ActCod='429';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1441','es','N','Eliminar foto otro usr.');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1442','es','N','Eliminar foto estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1443','es','N','Eliminar foto profesor');

UPDATE actions SET Obsolete='Y' WHERE ActCod='440';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1444','es','N','Crear invitado');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1445','es','N','Crear usuario como estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1446','es','N','Crear usuario como profesor');

UPDATE actions SET Obsolete='Y' WHERE ActCod='1239';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1447','es','N','Solicitar edici&oacute;n ID invitado');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1448','es','N','Solicitar edici&oacute;n ID estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1449','es','N','Solicitar edici&oacute;n ID profesor');
UPDATE actions SET Obsolete='Y' WHERE ActCod='1240';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1450','es','N','Eliminar ID invitado');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1451','es','N','Eliminar ID estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1452','es','N','Eliminar ID profesor');
UPDATE actions SET Obsolete='Y' WHERE ActCod='1241';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1453','es','N','Crear ID invitado');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1454','es','N','Crear ID estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1455','es','N','Crear ID profesor');

UPDATE actions SET Obsolete='Y' WHERE ActCod IN ('1417','1421','1425');

UPDATE actions SET Obsolete='Y' WHERE ActCod='592';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1456','es','N','Solicitar acept. inscrip. estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1457','es','N','Solicitar acept. inscrip. profesor');
UPDATE actions SET Obsolete='Y' WHERE ActCod='558';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1458','es','N','Aceptar inscrip. como estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1459','es','N','Aceptar inscrip. como profesor');
UPDATE actions SET Obsolete='Y' WHERE ActCod='559';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1460','es','N','Rechazar inscrip. como estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1461','es','N','Rechazar inscrip. como profesor');
UPDATE notif SET NotifyEvent=14 WHERE NotifyEvent=13;
UPDATE notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE notif SET NotifyEvent=9  WHERE NotifyEvent=8;
UPDATE notif SET NotifyEvent=8  WHERE NotifyEvent=7;
UPDATE sta_notif SET NotifyEvent=14 WHERE NotifyEvent=13;
UPDATE sta_notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE sta_notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE sta_notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE sta_notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE sta_notif SET NotifyEvent=9  WHERE NotifyEvent=8;
UPDATE sta_notif SET NotifyEvent=8  WHERE NotifyEvent=7;
UPDATE usr_data SET NotifNtfEvents=(((NotifNtfEvents & ~0x7F) << 1) | (NotifNtfEvents & 0x7F) | 0x80);
UPDATE usr_data SET EmailNtfEvents=(((EmailNtfEvents & ~0x7F) << 1) | (EmailNtfEvents & 0x7F));

UPDATE actions SET Obsolete='Y' WHERE ActCod='58';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1462','es','N','Eliminar estudiante asignatura');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1463','es','N','Eliminar profesor asignatura');

UPDATE actions SET Obsolete='Y' WHERE ActCod='598';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1464','es','N','Solicitar cambio contrase&ntilde;a otro usuario');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1465','es','N','Solicitar cambio contrase&ntilde;a estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1466','es','N','Solicitar cambio contrase&ntilde;a profesor');

UPDATE actions SET Obsolete='Y' WHERE ActCod='82';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1467','es','N','Cambiar contrase&ntilde;a otro usuario');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1468','es','N','Cambiar contrase&ntilde;a estudiante');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1469','es','N','Cambiar contrase&ntilde;a profesor');
		
----- SWAD 15.12 (2015/10/10) -----

ALTER TABLE log_recent ADD COLUMN CtyCod INT NOT NULL DEFAULT -1 AFTER ActCod,ADD INDEX (CtyCod),ADD COLUMN InsCod INT NOT NULL DEFAULT -1 AFTER CtyCod,ADD INDEX (InsCod),ADD COLUMN CtrCod INT NOT NULL DEFAULT -1 AFTER InsCod,ADD INDEX (CtrCod);
ALTER TABLE log ADD COLUMN CtyCod INT NOT NULL DEFAULT -1 AFTER ActCod,ADD INDEX (CtyCod),ADD COLUMN InsCod INT NOT NULL DEFAULT -1 AFTER CtyCod,ADD INDEX (InsCod),ADD COLUMN CtrCod INT NOT NULL DEFAULT -1 AFTER InsCod,ADD INDEX (CtrCod);

----- SWAD 15.12 (2015/10/11) -----

UPDATE log,degrees SET log.CtrCod=degrees.CtrCod WHERE log.DegCod=degrees.DegCod;
UPDATE log,centres SET log.InsCod=centres.InsCod WHERE log.CtrCod=centres.CtrCod;
UPDATE log,institutions SET log.CtyCod=institutions.CtyCod WHERE log.InsCod=institutions.InsCod;


-------------------------

CREATE TABLE IF NOT EXISTS log_full (LogCod INT NOT NULL AUTO_INCREMENT,ActCod INT NOT NULL DEFAULT -1,CtyCod INT NOT NULL DEFAULT -1,InsCod INT NOT NULL DEFAULT -1,CtrCod INT NOT NULL DEFAULT -1,DegCod INT NOT NULL DEFAULT -1,CrsCod INT NOT NULL DEFAULT -1,UsrCod INT NOT NULL DEFAULT -1,Role TINYINT NOT NULL,ClickTime DATETIME NOT NULL,TimeToGenerate INT NOT NULL,TimeToSend INT NOT NULL,IP CHAR(15) NOT NULL,UNIQUE INDEX(LogCod),INDEX(ActCod),INDEX(CtyCod),INDEX(InsCod),INDEX(CtrCod),INDEX(DegCod),INDEX(CrsCod),INDEX(UsrCod),INDEX(ClickTime,Role));
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20040101' AND ClickTime<'20050101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20050101' AND ClickTime<'20060101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20060101' AND ClickTime<'20070101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20070101' AND ClickTime<'20080101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20080101' AND ClickTime<'20090101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20090101' AND ClickTime<'20100101';

INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100101' AND ClickTime<'20100201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100201' AND ClickTime<'20100301';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100301' AND ClickTime<'20100401';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100401' AND ClickTime<'20100501';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100501' AND ClickTime<'20100601';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100601' AND ClickTime<'20100701';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100701' AND ClickTime<'20100801';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100801' AND ClickTime<'20100901';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20100901' AND ClickTime<'20101001';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20101001' AND ClickTime<'20101101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20101101' AND ClickTime<'20101201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20101201' AND ClickTime<'20110101';

INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110101' AND ClickTime<'20110201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110201' AND ClickTime<'20110301';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110301' AND ClickTime<'20110401';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110401' AND ClickTime<'20110501';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110501' AND ClickTime<'20110601';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110601' AND ClickTime<'20110701';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110701' AND ClickTime<'20110801';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110801' AND ClickTime<'20110901';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20110901' AND ClickTime<'20111001';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20111001' AND ClickTime<'20111101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20111101' AND ClickTime<'20111201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20111201' AND ClickTime<'20120101';

----------

DELETE FROM log WHERE ClickTime<'20050101';
DELETE FROM log WHERE ClickTime<'20060101';
DELETE FROM log WHERE ClickTime<'20060601';
DELETE FROM log WHERE ClickTime<'20060701';
DELETE FROM log WHERE ClickTime<'20080601';
DELETE FROM log WHERE ClickTime<'20090101';
DELETE FROM log WHERE ClickTime<'20090201';
DELETE FROM log WHERE ClickTime<'20090302';
DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20090303';
DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20090310';

CREATE TABLE log_new LIKE log;
// Insertar log muy recientes en una tabla de log nueva, sin repetir
INSERT log_new SELECT * FROM log WHERE ClickTime>='20151013' AND LogCod NOT IN (SELECT LogCod FROM log_new);

DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20090401';
DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20100101';
DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20110101';
DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20120101';

Parar MySQL
sync
Mover temporalmente ficheros log_full* a /var/www para dejar espacio en /var/lib/mysql
Lanzar MySQL
OPTIMIZE TABLE log; para liberar espacio en disco
Parar MySQL
Mover ficheros log_full* de /var/www a /var/lib/mysql
Lanzar MySQL

Continuar las siguientes inserciones desde log a log_full

Se estar� usando log_new en lugar de log, por tanto al final copiar todo lo nuevo que est� en log_new a log o a log_full
INSERT log SELECT * FROM log_new WHERE LogCod NOT IN (SELECT LogCod FROM log);


Revisar:
Prf_GetFirstClickFromLogAndStoreAsUsrFigure
Prf_GetNumClicksAndStoreAsUsrFigure

----------

INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120101' AND ClickTime<'20120201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120201' AND ClickTime<'20120301';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120301' AND ClickTime<'20120401';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120401' AND ClickTime<'20120501';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120501' AND ClickTime<'20120601';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120601' AND ClickTime<'20120701';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120701' AND ClickTime<'20120801';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120801' AND ClickTime<'20120901';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20120901' AND ClickTime<'20121001';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20121001' AND ClickTime<'20121101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20121101' AND ClickTime<'20121201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20121201' AND ClickTime<'20130101';

INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130101' AND ClickTime<'20130201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130201' AND ClickTime<'20130301';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130301' AND ClickTime<'20130401';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130401' AND ClickTime<'20130501';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130501' AND ClickTime<'20130601';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130601' AND ClickTime<'20130701';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130701' AND ClickTime<'20130801';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130801' AND ClickTime<'20130901';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20130901' AND ClickTime<'20131001';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20131001' AND ClickTime<'20131101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20131101' AND ClickTime<'20131201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20131201' AND ClickTime<'20140101';

INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140101' AND ClickTime<'20140201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140201' AND ClickTime<'20140301';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140301' AND ClickTime<'20140401';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140401' AND ClickTime<'20140501';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140501' AND ClickTime<'20140601';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140601' AND ClickTime<'20140701';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140701' AND ClickTime<'20140801';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140801' AND ClickTime<'20140901';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20140901' AND ClickTime<'20141001';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20141001' AND ClickTime<'20141101';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20141101' AND ClickTime<'20141201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20141201' AND ClickTime<'20150101';

DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20130101';
DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20140101';
DELETE LOW_PRIORITY FROM log WHERE ClickTime<'20150101';

Parar MySQL:
/sbin/service mysqld stop
sync

Lanzar MySQL:
/sbin/service mysqld start

Mover temporalmente ficheros log_full* a /var/www para dejar espacio en /var/lib/mysql
cd /var/lib/mysql/swad/
ls -la log_full*
mv log_full* /var/www

Liberar espacio en disco:
OPTIMIZE TABLE log;


Mover ficheros log_full* de /var/www a /var/lib/mysql:
mv /var/www/log_full* /var/lib/mysql/swad/

Continuar las siguientes inserciones desde log a log_full


INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150101' AND ClickTime<'20150201';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150201' AND ClickTime<'20150301';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150301' AND ClickTime<'20150401';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150401' AND ClickTime<'20150501';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150501' AND ClickTime<'20150601';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150601' AND ClickTime<'20150701';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150701' AND ClickTime<'20150801';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150801' AND ClickTime<'20150901';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20150901' AND ClickTime<'20151001';
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log WHERE ClickTime>='20151001' AND ClickTime<'20151013';


UPDATE log_full,degrees      SET log_full.CtrCod=degrees.CtrCod      WHERE log_full.ClickTime>='20040101' AND log_full.ClickTime<'20050101' AND log_full.DegCod=degrees.DegCod;

CREATE TABLE degrees_copy LIKE degrees; 
INSERT degrees_copy SELECT * FROM degrees;

CREATE TABLE centres_copy LIKE centres; 
INSERT centres_copy SELECT * FROM centres;

CREATE TABLE institutions_copy LIKE institutions; 
INSERT institutions_copy SELECT * FROM institutions;

Hecho hasta aqu�

UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050101' AND log_full.ClickTime<'20050201' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050201' AND log_full.ClickTime<'20050301' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050301' AND log_full.ClickTime<'20050401' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050401' AND log_full.ClickTime<'20050501' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050501' AND log_full.ClickTime<'20050601' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050601' AND log_full.ClickTime<'20050701' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050701' AND log_full.ClickTime<'20050801' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050801' AND log_full.ClickTime<'20050901' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20050901' AND log_full.ClickTime<'20051001' AND log_full.DegCod=degrees_copy.DegCod;


DROP PROCEDURE IF EXISTS update_log_full;
DELIMITER |
CREATE PROCEDURE update_log_full()
BEGIN
  DECLARE LogCodStart INT;
  DECLARE LogCodEnd INT;
  SET LogCodStart = 0;
  WHILE LogCodStart <= 400000000 DO
    SET LogCodEnd = LogCodStart + 100000;
    UPDATE log_full,degrees_copy SET log_full.CtrCod=degrees_copy.CtrCod WHERE (log_full.LogCod BETWEEN LogCodStart AND LogCodEnd) AND log_full.DegCod=degrees_copy.DegCod;
    SET LogCodStart = LogCodEnd;
  END WHILE;
END;
|
DELIMITER ;
CALL update_log_full();


DROP PROCEDURE IF EXISTS update_log_full;
DELIMITER |
CREATE PROCEDURE update_log_full()
BEGIN
  DECLARE LogCodStart INT;
  DECLARE LogCodEnd INT;
  SET LogCodStart = 0;
  WHILE LogCodStart <= 400000000 DO
    SET LogCodEnd = LogCodStart + 100000;
    UPDATE log_full,centres_copy SET log_full.InsCod=centres_copy.InsCod WHERE (log_full.LogCod BETWEEN LogCodStart AND LogCodEnd) AND log_full.CtrCod=centres_copy.CtrCod;
    SET LogCodStart = LogCodEnd;
  END WHILE;
END;
|
DELIMITER ;
CALL update_log_full();

DROP PROCEDURE IF EXISTS update_log_full;
DELIMITER |
CREATE PROCEDURE update_log_full()
BEGIN
  DECLARE LogCodStart INT;
  DECLARE LogCodEnd INT;
  SET LogCodStart = 0;
  WHILE LogCodStart <= 400000000 DO
    SET LogCodEnd = LogCodStart + 100000;
    UPDATE log_full,institutions_copy SET log_full.CtyCod=institutions_copy.CtyCod WHERE (log_full.LogCod BETWEEN LogCodStart AND LogCodEnd) AND log_full.InsCod=institutions_copy.InsCod;
    SET LogCodStart = LogCodEnd;
  END WHILE;
END;
|
DELIMITER ;
CALL update_log_full();


Se estar� usando log_new en lugar de log, por tanto al final copiar todo lo nuevo que est� en log_new a log o a log_full
INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log_new WHERE ClickTime>='20151013' AND LogCod NOT IN (SELECT LogCod FROM log_full);

UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20151013' AND log_full.ClickTime<'20151014' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20151014' AND log_full.ClickTime<'20151015' AND log_full.DegCod=degrees_copy.DegCod;

UPDATE log_full,centres_copy      SET log_full.InsCod=centres_copy.InsCod      WHERE log_full.ClickTime>='20151013' AND log_full.ClickTime<'20151014' AND log_full.CtrCod=centres_copy.CtrCod;
UPDATE log_full,centres_copy      SET log_full.InsCod=centres_copy.InsCod      WHERE log_full.ClickTime>='20151014' AND log_full.ClickTime<'20151015' AND log_full.CtrCod=centres_copy.CtrCod;

UPDATE log_full,institutions_copy SET log_full.CtyCod=institutions_copy.CtyCod WHERE log_full.ClickTime>='20151013' AND log_full.ClickTime<'20151014' AND log_full.InsCod=institutions_copy.InsCod;
UPDATE log_full,institutions_copy SET log_full.CtyCod=institutions_copy.CtyCod WHERE log_full.ClickTime>='20151014' AND log_full.ClickTime<'20151015' AND log_full.InsCod=institutions_copy.InsCod;

Hecho hasta aqu�


-----------

Paso final en swad.ugr.es con SWAD parado:

touch /var/www/cgi-bin/swad.lock

INSERT INTO log_full (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP) SELECT LogCod,ActCod, '-1' , '-1' , '-1' ,DegCod,CrsCod,UsrCod,Role,ClickTime,TimeToGenerate,TimeToSend,IP FROM log_new WHERE ClickTime>='20151015' AND LogCod NOT IN (SELECT LogCod FROM log_full);
UPDATE log_full,degrees_copy      SET log_full.CtrCod=degrees_copy.CtrCod      WHERE log_full.ClickTime>='20151015' AND log_full.DegCod=degrees_copy.DegCod;
UPDATE log_full,centres_copy      SET log_full.InsCod=centres_copy.InsCod      WHERE log_full.ClickTime>='20151015' AND log_full.CtrCod=centres_copy.CtrCod;
UPDATE log_full,institutions_copy SET log_full.CtyCod=institutions_copy.CtyCod WHERE log_full.ClickTime>='20151015' AND log_full.InsCod=institutions_copy.InsCod;

Copiar nuevo programa swad

rm /var/www/cgi-bin/swad.lock





OPTIMIZE TABLE IP_last;
OPTIMIZE TABLE IP_prefs;
OPTIMIZE TABLE actions;
OPTIMIZE TABLE actions_MFU;
OPTIMIZE TABLE admin;
OPTIMIZE TABLE ann_seen;
OPTIMIZE TABLE announcements;
OPTIMIZE TABLE asg_grp;
OPTIMIZE TABLE assignments;
OPTIMIZE TABLE att_events;
OPTIMIZE TABLE att_grp;
OPTIMIZE TABLE att_usr;
OPTIMIZE TABLE banners; 
OPTIMIZE TABLE birthdays_today;
OPTIMIZE TABLE centres;
OPTIMIZE TABLE chat;
OPTIMIZE TABLE clicks_without_photo;
OPTIMIZE TABLE clipboard;
OPTIMIZE TABLE connected;
OPTIMIZE TABLE countries;
OPTIMIZE TABLE courses;
OPTIMIZE TABLE crs_grp;
OPTIMIZE TABLE crs_grp_types;
OPTIMIZE TABLE crs_grp_usr;
OPTIMIZE TABLE crs_info_read;
OPTIMIZE TABLE crs_info_src;
OPTIMIZE TABLE crs_info_txt;
OPTIMIZE TABLE crs_last;
OPTIMIZE TABLE crs_record_fields;
OPTIMIZE TABLE crs_records;
OPTIMIZE TABLE crs_usr;
OPTIMIZE TABLE crs_usr_requests;
OPTIMIZE TABLE debug;
OPTIMIZE TABLE deg_types;
OPTIMIZE TABLE degrees;
OPTIMIZE TABLE departments;
OPTIMIZE TABLE exam_announcements;
OPTIMIZE TABLE expanded_folders;
OPTIMIZE TABLE file_browser_last;
OPTIMIZE TABLE file_browser_size;
OPTIMIZE TABLE file_view;
OPTIMIZE TABLE files;
OPTIMIZE TABLE forum_disabled_post;
OPTIMIZE TABLE forum_post;
OPTIMIZE TABLE forum_thr_clip;
OPTIMIZE TABLE forum_thr_read;
OPTIMIZE TABLE forum_thread;
OPTIMIZE TABLE hidden_params;
OPTIMIZE TABLE holidays;
OPTIMIZE TABLE imported_groups;
OPTIMIZE TABLE imported_sessions;
OPTIMIZE TABLE imported_students;
OPTIMIZE TABLE institutions;
OPTIMIZE TABLE links;
OPTIMIZE TABLE mail_domains;
OPTIMIZE TABLE marks_properties;
OPTIMIZE TABLE msg_banned;
OPTIMIZE TABLE msg_rcv;
OPTIMIZE TABLE msg_rcv_deleted;
OPTIMIZE TABLE msg_snt;
OPTIMIZE TABLE msg_snt_deleted;
OPTIMIZE TABLE notices;
OPTIMIZE TABLE notices_deleted;
OPTIMIZE TABLE notif;
OPTIMIZE TABLE pending_emails;
OPTIMIZE TABLE pending_passwd;
OPTIMIZE TABLE places;
OPTIMIZE TABLE plugins;
OPTIMIZE TABLE sessions;
OPTIMIZE TABLE sta_degrees;
OPTIMIZE TABLE sta_notif;
OPTIMIZE TABLE surveys;
OPTIMIZE TABLE svy_answers;
OPTIMIZE TABLE svy_grp;
OPTIMIZE TABLE svy_questions;
OPTIMIZE TABLE svy_users;
OPTIMIZE TABLE timetable_crs;
OPTIMIZE TABLE timetable_tut;
OPTIMIZE TABLE tst_answers;
OPTIMIZE TABLE tst_config;
OPTIMIZE TABLE tst_exam_questions;
OPTIMIZE TABLE tst_exams;
OPTIMIZE TABLE tst_question_tags;
OPTIMIZE TABLE tst_questions;
OPTIMIZE TABLE tst_status;
OPTIMIZE TABLE tst_tags;
OPTIMIZE TABLE usr_IDs;
OPTIMIZE TABLE usr_banned;
OPTIMIZE TABLE usr_data;
OPTIMIZE TABLE usr_emails;
OPTIMIZE TABLE usr_figures;
OPTIMIZE TABLE usr_follow;
OPTIMIZE TABLE usr_last;
OPTIMIZE TABLE usr_nicknames;
OPTIMIZE TABLE usr_webs;
OPTIMIZE TABLE ws_keys;    


-----

SELECT TstCod,AllowTeachers,UNIX_TIMESTAMP(TstTime) AS T,NumQsts,NumQstsNotBlank,Score FROM tst_exams WHERE T>='0' AND T<='2000000000' ORDER BY TstCod;

-----

SELECT * FROM expanded_folders WHERE UNIX_TIMESTAMP() > UNIX_TIMESTAMP(ClickTime)+'1000';
SELECT * FROM expanded_folders WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'1000');

-----
SELECT UsrCod,'Y',Sex FROM usr_data WHERE UsrCod IN (SELECT DISTINCT UsrCod FROM admin WHERE Scope='Sys' UNION SELECT DISTINCT admin.UsrCod FROM admin,institutions WHERE admin.Scope='Ins' AND admin.Cod=institutions.InsCod AND institutions.CtyCod='724') AS admin_codes ORDER BY Surname1,Surname2,FirstName,UsrCod;



-----------------------

CREATE TABLE timetable_crs_backup LIKE timetable_crs;
INSERT INTO timetable_crs_backup SELECT * FROM timetable_crs;

CREATE TABLE timetable_tut_backup LIKE timetable_tut;
INSERT INTO timetable_tut_backup SELECT * FROM timetable_tut;

UPDATE timetable_crs SET Hour=Hour+2;
UPDATE timetable_tut SET Hour=Hour+2;


SELECT COUNT(DISTINCT crs_usr.UsrCod) FROM institutions,centres,degrees,courses,crs_usr WHERE institutions.CtyCod='724' AND institutions.InsCod=centres.InsCod AND centres.CtrCod=degrees.CtrCod AND degrees.DegCod=courses.DegCod AND courses.CrsCod=crs_usr.CrsCod;



----- SWAD 15.77 (2015/12/28) -----

CREATE TABLE IF NOT EXISTS social (SocCod BIGINT NOT NULL AUTO_INCREMENT,SocialEvent TINYINT NOT NULL,UsrCod INT NOT NULL,CtyCod INT NOT NULL DEFAULT -1,InsCod INT NOT NULL DEFAULT -1,CtrCod INT NOT NULL DEFAULT -1,DegCod INT NOT NULL DEFAULT -1,CrsCod INT NOT NULL DEFAULT -1,Cod INT NOT NULL DEFAULT -1,TimeEvent DATETIME NOT NULL,Status TINYINT NOT NULL DEFAULT 0,UNIQUE INDEX(SocCod),INDEX(SocialEvent),INDEX(UsrCod),INDEX(TimeEvent));
ALTER TABLE social DROP COLUMN Status;

----- SWAD 15.82 (2015/12/29) -----

CREATE TABLE IF NOT EXISTS social_post (PstCod INT NOT NULL AUTO_INCREMENT,Content LONGTEXT NOT NULL,UNIQUE INDEX(PstCod),FULLTEXT(Content)) ENGINE = MYISAM;

----- SWAD 15.87 (2015/12/31) -----

DROP TABLE IF EXISTS social_timeline;
CREATE TABLE IF NOT EXISTS social_timeline (SocCod BIGINT NOT NULL AUTO_INCREMENT,AuthorCod INT NOT NULL,PublisherCod INT NOT NULL,NotCod BIGINT NOT NULL,TimePublish DATETIME NOT NULL,UNIQUE INDEX(SocCod),INDEX(AuthorCod),INDEX(PublisherCod),INDEX(NotCod),INDEX(TimePublish));
INSERT INTO social_timeline (AuthorCod,PublisherCod,NotCod,TimePublish) SELECT UsrCod,UsrCod,NotCod,TimeNote FROM social_notes ORDER BY NotCod;
       
                
SELECT MIN(PubCod) FROM social_timeline WHERE PublisherCod IN (SELECT '1' UNION SELECT FollowedCod FROM usr_follow WHERE FollowerCod='1') GROUP BY NotCod;
		  	  
----- SWAD 15.90 (2016/01/02) -----

ALTER TABLE social_notes ADD COLUMN HieCod INT NOT NULL DEFAULT -1 AFTER UsrCod;
UPDATE social_notes SET HieCod=InsCod WHERE NoteType IN ('1','2');
UPDATE social_notes SET HieCod=CtrCod WHERE NoteType IN ('3','4');
UPDATE social_notes SET HieCod=DegCod WHERE NoteType IN ('5','6');
UPDATE social_notes SET HieCod=CrsCod WHERE NoteType IN ('7','8','9','12');
ALTER TABLE social_notes DROP COLUMN CtyCod,DROP COLUMN InsCod,DROP COLUMN CtrCod,DROP COLUMN CrsCod;


CREATE TABLE IF NOT EXISTS social_timeline_new (PubCod BIGINT NOT NULL AUTO_INCREMENT, NotCod BIGINT NOT NULL, PublisherCod INT NOT NULL, AuthorCod INT NOT NULL, TimePublish DATETIME NOT NULL, UNIQUE INDEX(PubCod), UNIQUE INDEX(NotCod,PublisherCod), INDEX(PublisherCod), INDEX(AuthorCod), INDEX(TimePublish)) SELECT PubCod,NotCod,PublisherCod,AuthorCod,TimePublish FROM social_timeline ORDER BY PubCod;

----- SWAD 15.100.5 (2016/01/07) -----

CREATE TABLE IF NOT EXISTS social_comments (ComCod BIGINT NOT NULL AUTO_INCREMENT,UsrCod INT NOT NULL,NotCod INT NOT NULL,TimeComment DATETIME NOT NULL,UNIQUE INDEX(ComCod),INDEX(UsrCod),INDEX(NotCod,TimeComment),INDEX(TimeComment));

CREATE TABLE IF NOT EXISTS social_comments_content (ComCod BIGINT NOT NULL,Content LONGTEXT NOT NULL,UNIQUE INDEX(ComCod),FULLTEXT(Content)) ENGINE = MYISAM;







CREATE TEMPORARY TABLE note_codes (NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod)) ENGINE=MEMORY;

CREATE TEMPORARY TABLE current_timeline (NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod)) ENGINE=MEMORY;

DROP PROCEDURE IF EXISTS get_note_cods;
DELIMITER |
CREATE PROCEDURE get_note_cods()
BEGIN
  SET @i = 0;
  WHILE @i < 20 DO
    SET @NewestNotCod = (SELECT log_borrame.CrsCod AS NewestNotCod FROM log_borrame,usr_data WHERE log_borrame.LogCod<388900000 AND log_borrame.UsrCod=usr_data.UsrCod AND log_borrame.CrsCod NOT IN (SELECT NotCod FROM current_timeline) ORDER BY log_borrame.LogCod DESC LIMIT 1);
    INSERT INTO note_codes (NotCod) VALUES (@NewestNotCod);
    INSERT INTO current_timeline (NotCod) VALUES (@NewestNotCod);
    SET @i = @i + 1;
  END WHILE;
END;
|
DELIMITER ;
CALL get_note_cods();














----- SWAD 15.123 (2016/01/21) -----
 0 ---> unchanged
...
 8 ---> unchanged

 9 ---> new
...
13 ---> new

14 ---> unchanged

   Ntf_EVENT_FORUM_POST_COURSE	= 15,	// Old 10	// New post in forums of my courses
   Ntf_EVENT_FORUM_REPLY	= 16,	// Old 11	// New reply to one of my posts in any forum

   /* Messages tab */
   Ntf_EVENT_NOTICE		= 17,	// Old  9
   Ntf_EVENT_MESSAGE		= 18,	// Old 12

   /* Statistics tab */
   Ntf_EVENT_SURVEY		= 19,	// Old 13
 9 ---> 17
10 ---> 15
11 ---> 16
12 ---> 18
13 ---> 19
   
UPDATE notif SET NotifyEvent=17 WHERE NotifyEvent=9;
UPDATE notif SET NotifyEvent=15 WHERE NotifyEvent=10;
UPDATE notif SET NotifyEvent=16 WHERE NotifyEvent=11;
UPDATE notif SET NotifyEvent=18 WHERE NotifyEvent=12;
UPDATE notif SET NotifyEvent=19 WHERE NotifyEvent=13;

UPDATE sta_notif SET NotifyEvent=17 WHERE NotifyEvent=9;
UPDATE sta_notif SET NotifyEvent=15 WHERE NotifyEvent=10;
UPDATE sta_notif SET NotifyEvent=16 WHERE NotifyEvent=11;
UPDATE sta_notif SET NotifyEvent=18 WHERE NotifyEvent=12;
UPDATE sta_notif SET NotifyEvent=19 WHERE NotifyEvent=13;

 9 ---> 17
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 x  x  x  x | x  x  x  x | x  x  a  x | x  x  x  x | x  x  x  x
          0            0            2            0            0
                                 |
        __________ << 8 _________|
       |
       v
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 x  x  a  x | x  x  x  x | x  x 1/0 x | x  x  x  x | x  x  x  x

10 ---> 15
11 ---> 16
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 x  x  a  x | x  x  x  x | c  b  *  x | x  x  x  x | x  x  x  x
          0            0            C            0            0
                           |  |
           ________________|  |
          |    ____ << 5 _____|
          |   |                       
          v   v
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 x  x  a  c | b  x  x  x | *  *  *  x | x  x  x  x | x  x  x  x

12 ---> 18
13 ---> 19
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 x  x  a  c | b  x  e  d | *  *  *  x | x  x  x  x | x  x  x  x
          0            3            0            0            0
                    |  |
  __________________|  |
 |   _______ << 6 _____|
 |  |                       
 v  v
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 e  d  a  c | b  x  *  * | *  *  *  x | x  x  x  x | x  x  x  x


UPDATE usr_data SET NotifNtfEvents = ((NotifNtfEvents & ~0xF8000) | ((NotifNtfEvents & 0x0200) << 8) | ((NotifNtfEvents & 0x0C00) << 5) | ((NotifNtfEvents & 0x3000) << 6));
UPDATE usr_data SET EmailNtfEvents = ((EmailNtfEvents & ~0xF8000) | ((EmailNtfEvents & 0x0200) << 8) | ((EmailNtfEvents & 0x0C00) << 5) | ((EmailNtfEvents & 0x3000) << 6));
UPDATE usr_data SET NotifNtfEvents = (NotifNtfEvents |  0x3E00);
UPDATE usr_data SET EmailNtfEvents = (EmailNtfEvents & ~0x3E00);



----- SWAD 15.130 (2016/01/26) -----


 0 ---> unchanged
...
 8 ---> unchanged
 
 10 ---> 9
 11 ---> 10
 
 19 ---> 18

UPDATE notif SET NotifyEvent= 9 WHERE NotifyEvent=10;
UPDATE notif SET NotifyEvent=10 WHERE NotifyEvent=11;
UPDATE notif SET NotifyEvent=11 WHERE NotifyEvent=12;
UPDATE notif SET NotifyEvent=12 WHERE NotifyEvent=13;
UPDATE notif SET NotifyEvent=13 WHERE NotifyEvent=14;
UPDATE notif SET NotifyEvent=14 WHERE NotifyEvent=15;
UPDATE notif SET NotifyEvent=15 WHERE NotifyEvent=16;
UPDATE notif SET NotifyEvent=16 WHERE NotifyEvent=17;
UPDATE notif SET NotifyEvent=17 WHERE NotifyEvent=18;
UPDATE notif SET NotifyEvent=18 WHERE NotifyEvent=19;

UPDATE sta_notif SET NotifyEvent= 9 WHERE NotifyEvent=10;
UPDATE sta_notif SET NotifyEvent=10 WHERE NotifyEvent=11;
UPDATE sta_notif SET NotifyEvent=11 WHERE NotifyEvent=12;
UPDATE sta_notif SET NotifyEvent=12 WHERE NotifyEvent=13;
UPDATE sta_notif SET NotifyEvent=13 WHERE NotifyEvent=14;
UPDATE sta_notif SET NotifyEvent=14 WHERE NotifyEvent=15;
UPDATE sta_notif SET NotifyEvent=15 WHERE NotifyEvent=16;
UPDATE sta_notif SET NotifyEvent=16 WHERE NotifyEvent=17;
UPDATE sta_notif SET NotifyEvent=17 WHERE NotifyEvent=18;
UPDATE sta_notif SET NotifyEvent=18 WHERE NotifyEvent=19;

 9 ---> 17
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 a  b  c  d | e  f  g  h | i  j  k  x | x  x  x  x | x  x  x  x
 |        F            F      |     C            0            0
 |__                          |__
    |                            |
    |                            |
    v                            v
19 18 17 16 |15 14 13 12 |11 10  9  8 | 7  6  5  4 | 3  2  1  0
 x  a  b  c | d  e  f  g | h  i  j  x | x  x  x  x | x  x  x  x

UPDATE usr_data SET NotifNtfEvents = ((NotifNtfEvents & ~0x7FE00) | ((NotifNtfEvents & 0xFFC00) >> 1));
UPDATE usr_data SET EmailNtfEvents = ((EmailNtfEvents & ~0xFFE00) | ((EmailNtfEvents & 0xFFC00) >> 1));

---------------


OPTIMIZE TABLE IP_prefs;
OPTIMIZE TABLE actions;
OPTIMIZE TABLE actions_MFU;
OPTIMIZE TABLE admin;
OPTIMIZE TABLE ann_seen;

OPTIMIZE TABLE announcements;
OPTIMIZE TABLE asg_grp;
OPTIMIZE TABLE assignments;
OPTIMIZE TABLE att_events;
OPTIMIZE TABLE att_grp;
OPTIMIZE TABLE att_usr;

OPTIMIZE TABLE banners;
OPTIMIZE TABLE birthdays_today;
OPTIMIZE TABLE centres;
OPTIMIZE TABLE chat;
OPTIMIZE TABLE clicks_without_photo;
OPTIMIZE TABLE clipboard;
OPTIMIZE TABLE connected;

OPTIMIZE TABLE countries;
OPTIMIZE TABLE courses;
OPTIMIZE TABLE crs_grp;
OPTIMIZE TABLE crs_grp_types;
OPTIMIZE TABLE crs_grp_usr;
OPTIMIZE TABLE crs_info_read;
OPTIMIZE TABLE crs_info_src;
OPTIMIZE TABLE crs_info_txt;
OPTIMIZE TABLE crs_last;
OPTIMIZE TABLE crs_record_fields;
OPTIMIZE TABLE crs_records;
OPTIMIZE TABLE crs_usr;
OPTIMIZE TABLE crs_usr_requests;

OPTIMIZE TABLE debug;
OPTIMIZE TABLE deg_types;
OPTIMIZE TABLE degrees;
OPTIMIZE TABLE departments;
OPTIMIZE TABLE exam_announcements;
OPTIMIZE TABLE expanded_folders;

OPTIMIZE TABLE file_browser_last;
OPTIMIZE TABLE file_browser_size;
OPTIMIZE TABLE file_view;
OPTIMIZE TABLE files;


OPTIMIZE TABLE forum_disabled_post;
OPTIMIZE TABLE forum_post;
OPTIMIZE TABLE forum_thr_clip;
OPTIMIZE TABLE forum_thr_read;
OPTIMIZE TABLE forum_thread;


OPTIMIZE TABLE hidden_params;
OPTIMIZE TABLE holidays;
OPTIMIZE TABLE imported_groups;
OPTIMIZE TABLE imported_sessions;
OPTIMIZE TABLE imported_students;
OPTIMIZE TABLE institutions;
OPTIMIZE TABLE links;

OPTIMIZE TABLE mail_domains;
OPTIMIZE TABLE marks_properties;
OPTIMIZE TABLE msg_banned;

OPTIMIZE TABLE msg_content;
OPTIMIZE TABLE msg_content_deleted;
OPTIMIZE TABLE msg_rcv;
OPTIMIZE TABLE msg_rcv_deleted;
OPTIMIZE TABLE msg_snt;
OPTIMIZE TABLE msg_snt_deleted;

| notices              | 
| notices_deleted      | 
| notif                | 
| pending_emails       | 
| pending_passwd       | 
| places               | 
| plugins              | 
| sessions             | 
| social_comments      | 
| social_comments_fav  | 
| social_notes         | 
| social_notes_fav     | 
| social_posts         | 
| social_pubs          | 
| social_timelines     | 
| sta_degrees          | 
| sta_notif            | 
| surveys              | 
| svy_answers          | 
| svy_grp              | 
| svy_questions        | 
| svy_users            | 
| timetable_crs        | 
| timetable_crs_backup | 
| timetable_tut        | 
| timetable_tut_backup | 
| tst_answers          | 
| tst_config           | 
| tst_exam_questions   | 
| tst_exams            | 
| tst_question_tags    | 
| tst_questions        | 
| tst_status           | 
| tst_tags             | 
| usr_IDs              | 
| usr_banned           | 
| usr_data             | 
| usr_emails           | 
| usr_figures          | 
| usr_follow           | 
| usr_last             | 
| usr_nicknames        | 
| usr_webs             | 
| ws_keys              | 
+----------------------+


SELECT COUNT(*) FROM social_notes WHERE NoteType='10';


----- SWAD 15.199 (2016/04/16) -----

SELECT degrees.DegCod,degrees.CtrCod,degrees.DegTypCod,degrees.Status,degrees.RequesterUsrCod,degrees.ShortName,degrees.FullName,degrees.WWW FROM degrees,courses,crs_usr WHERE degrees.DegCod=courses.CrsCod AND courses.CrsCod=crs_usr.CrsCod AND crs_usr.Role='3' ORDER BY degrees.ShortName;

----- SWAD 15.201 (2016/04/21) -----

CREATE TABLE notif_backup LIKE notif;
INSERT INTO notif_backup SELECT * FROM notif;

CREATE TABLE sta_notif_backup LIKE sta_notif;
INSERT INTO sta_notif_backup SELECT * FROM sta_notif;

CREATE TABLE usr_data_backup LIKE usr_data;
INSERT INTO usr_data_backup SELECT * FROM usr_data;

UPDATE notif     SET NotifyEvent=NotifyEvent+1 WHERE NotifyEvent>=2;
UPDATE sta_notif SET NotifyEvent=NotifyEvent+1 WHERE NotifyEvent>=2;
UPDATE usr_data  SET NotifNtfEvents = (((NotifNtfEvents & ~0x3) << 1) | (NotifNtfEvents & 0x3) | 0x04);
UPDATE usr_data  SET EmailNtfEvents = (((EmailNtfEvents & ~0x3) << 1) | (EmailNtfEvents & 0x3));


---------------

SELECT SUM(NumEvents),SUM(NumMails) FROM sta_notif_backup WHERE NotifyEvent='0';


---------------


DELETE FROM sta_notif;
INSERT INTO sta_notif SELECT * FROM sta_notif_backup;

UPDATE sta_notif SET NotifyEvent=19 WHERE NotifyEvent=18;
UPDATE sta_notif SET NotifyEvent=18 WHERE NotifyEvent=17;
UPDATE sta_notif SET NotifyEvent=17 WHERE NotifyEvent=16;
UPDATE sta_notif SET NotifyEvent=16 WHERE NotifyEvent=15;
UPDATE sta_notif SET NotifyEvent=15 WHERE NotifyEvent=14;
UPDATE sta_notif SET NotifyEvent=14 WHERE NotifyEvent=13;
UPDATE sta_notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE sta_notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE sta_notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE sta_notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE sta_notif SET NotifyEvent=9  WHERE NotifyEvent=8;
UPDATE sta_notif SET NotifyEvent=8  WHERE NotifyEvent=7;
UPDATE sta_notif SET NotifyEvent=7  WHERE NotifyEvent=6;
UPDATE sta_notif SET NotifyEvent=6  WHERE NotifyEvent=5;
UPDATE sta_notif SET NotifyEvent=5  WHERE NotifyEvent=4;
UPDATE sta_notif SET NotifyEvent=4  WHERE NotifyEvent=3;
UPDATE sta_notif SET NotifyEvent=3  WHERE NotifyEvent=2;

DELETE FROM notif;
INSERT INTO notif SELECT * FROM notif_backup;

UPDATE notif SET NotifyEvent=19 WHERE NotifyEvent=18;
UPDATE notif SET NotifyEvent=18 WHERE NotifyEvent=17;
UPDATE notif SET NotifyEvent=17 WHERE NotifyEvent=16;
UPDATE notif SET NotifyEvent=16 WHERE NotifyEvent=15;
UPDATE notif SET NotifyEvent=15 WHERE NotifyEvent=14;
UPDATE notif SET NotifyEvent=14 WHERE NotifyEvent=13;
UPDATE notif SET NotifyEvent=13 WHERE NotifyEvent=12;
UPDATE notif SET NotifyEvent=12 WHERE NotifyEvent=11;
UPDATE notif SET NotifyEvent=11 WHERE NotifyEvent=10;
UPDATE notif SET NotifyEvent=10 WHERE NotifyEvent=9;
UPDATE notif SET NotifyEvent=9  WHERE NotifyEvent=8;
UPDATE notif SET NotifyEvent=8  WHERE NotifyEvent=7;
UPDATE notif SET NotifyEvent=7  WHERE NotifyEvent=6;
UPDATE notif SET NotifyEvent=6  WHERE NotifyEvent=5;
UPDATE notif SET NotifyEvent=5  WHERE NotifyEvent=4;
UPDATE notif SET NotifyEvent=4  WHERE NotifyEvent=3;
UPDATE notif SET NotifyEvent=3  WHERE NotifyEvent=2;


----- SWAD 15.225.3 (2016/06/15) -----

CREATE TABLE IF NOT EXISTS usr_duplicated (UsrCod INT NOT NULL,InformerCod INT NOT NULL,InformTime DATETIME NOT NULL,UNIQUE INDEX(UsrCod,InformerCod),INDEX(UsrCod));


SELECT DISTINCT UsrCod FROM (SELECT DISTINCT UsrCod FROM usr_IDs WHERE UsrID IN (SELECT UsrID FROM usr_IDs WHERE UsrCod='1') UNION SELECT UsrCod FROM usr_data WHERE Surname1='' AND Surname2='' AND FirstName='') AS I;













SELECT * FROM file_view,files WHERE files.FileBrowser='9' AND files.Cod='-1' AND files.FilCod=file_view.FilCod;












----- Optimizaci�n de consulta de mensajes -----


SELECT COUNT(*) FROM msg_content WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt) AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv);


SELECT COUNT(*) FROM msg_content LEFT JOIN msg_snt ON (msg_content.MsgCod=msg_snt.MsgCod) WHERE msg_content.MsgCod IS NULL;


------------------------------------------------


SELECT SQL_NO_CACHE YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'Europe/Berlin')) AS Year,COUNT(*) FROM log_full WHERE ClickTime>=FROM_UNIXTIME('0') AND UsrCod='1' GROUP BY Year DESC;
		  
		  
		 
----- SWAD 16.15 (2016/10/06) -----
		 
CREATE TABLE IF NOT EXISTS usr_report (RepCod INT NOT NULL AUTO_INCREMENT,UsrCod INT NOT NULL,ReportTimeUTC DATETIME NOT NULL,UniqueDirL CHAR(2) NOT NULL,UniqueDirR CHAR(41) NOT NULL,Filename VARCHAR(255) NOT NULL,Permalink VARCHAR(255) NOT NULL,UNIQUE INDEX(RepCod),INDEX(UsrCod));
		


--------------------------------------------------
--------------------------------------------------


SELECT CrsCod,COUNT(*) AS N FROM crs_usr LEFT JOIN log_full ON (crs_usr.CrsCod=log_full.CrsCod AND crs_usr.UsrCod=log_full.UsrCod AND crs_usr.Role=log_full.Role) WHERE UsrCod='7' AND Role='3' GROUP BY CrsCod ORDER BY N DESC;









(SELECT crs_usr.UsrCod FROM surveys,crs_usr WHERE surveys.SvyCod='6' AND surveys.SvyCod NOT IN (SELECT SvyCod FROM svy_grp WHERE SvyCod='6') AND surveys.CrsCod=crs_usr.CrsCod AND crs_usr.UsrCod<>'1' AND (surveys.Roles&(1<<crs_usr.Role))<>0)
 UNION 
(SELECT DISTINCT crs_grp_usr.UsrCod FROM svy_grp,crs_grp_usr,surveys,crs_usr WHERE svy_grp.SvyCod='6' AND svy_grp.GrpCod=crs_grp_usr.GrpCod AND crs_grp_usr.UsrCod=crs_usr.UsrCod AND crs_grp_usr.UsrCod<>'1' AND svy_grp.SvyCod=surveys.SvyCod AND surveys.CrsCod=crs_usr.CrsCod AND (surveys.Roles&(1<<crs_usr.Role))<>0);


----- swad-core 16.45 (2016/10/27) -----

ALTER TABLE surveys ADD COLUMN Scope ENUM('Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Sys' AFTER SvyCod;
ALTER TABLE surveys ADD COLUMN Cod INT NOT NULL DEFAULT -1 AFTER Scope;

UPDATE surveys SET Scope='Sys' WHERE DegCod<='0' AND CrsCod<='0';
UPDATE surveys SET Scope='Deg' WHERE DegCod>'0' AND CrsCod<='0';
UPDATE surveys SET Scope='Crs' WHERE CrsCod>'0';

UPDATE surveys SET Cod='-1' WHERE Scope='Sys';
UPDATE surveys SET Cod=DegCod WHERE Scope='Deg';
UPDATE surveys SET Cod=CrsCod WHERE Scope='Crs';

DROP INDEX DegCod ON surveys;
ALTER TABLE surveys ADD PRIMARY KEY(SvyCod);
DROP INDEX SvyCod ON surveys;
ALTER TABLE surveys ADD UNIQUE INDEX(Scope,Cod);

----- Listar dominios usados pero a�n no contemplados en la lista de dominios permitidos -----

SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS mail_domain,COUNT(*) AS N FROM usr_emails WHERE SUBSTRING_INDEX(E_mail,'@',-1) NOT IN (SELECT Domain COLLATE latin1_general_ci FROM mail_domains) GROUP BY mail_domain ORDER BY N DESC;

----- swad-core 16.142 (2017/02/27) -----

CREATE TABLE IF NOT EXISTS log_search (LogCod INT NOT NULL,SearchStr VARCHAR(255) NOT NULL,UNIQUE INDEX(LogCod));

----- TODO: Eliminar columnas sin uso en futuras versiones -----

ALTER TABLE surveys DROP COLUMN DegCod;
ALTER TABLE surveys DROP COLUMN CrsCod;



SELECT LENGTH(Subject) AS Len,COUNT(*) AS N FROM forum_post GROUP BY Len ORDER BY Len DESC;




CREATE TABLE timetable_crs_backup LIKE timetable_crs;
INSERT INTO timetable_crs_backup SELECT * FROM timetable_crs;

CREATE TABLE timetable_tut_backup LIKE timetable_tut;
INSERT INTO timetable_tut_backup SELECT * FROM timetable_tut;




0x01FF
1 1111 1111
1 1110 0111
0x1E7



0x01FE
1 1111 1110
1 1110 0110
0x1E6


0x01FC
1 1111 1100
1 1110 0100
0x1E4



0x0001
0 0000 0001
0 0000 0001
0x001


0x0100
1 0000 0000
1 0000 0000
0x100


0x01E0
1 1110 0000
1 1110 0000
0x1E0

0x0110
1 0001 0000
1 0000 0000
0x100

0x01F4
1 1111 0100
1 1110 0100
0x1E4

0x011C
1 0001 1100
1 0000 0100
0x104

0x0114
1 0001 0100
1 0000 0100
0x104

0x01E4
1 1110 0100
1 1110 0100
0x1E4

0x0104
1 0000 0100
1 0000 0100
0x104

0x0180
1 1000 0000
1 1000 0000
0x180

0x01C0
1 1100 0000
1 1100 0000
0x1C0

0x01F0
1 1111 0000
1 1110 0000
0x1E0


0x01F8
1 1111 1000
1 1110 0000
0x1E0

0x0006
0 0000 0110
0 0000 0110
0x006

------

0x01FF
1 1111 1111
1 1111 1000
0x1F8


--- Insert new rol non-editing teacher ----------------------------------------

0x_1F8
0001 1111 1000
0011 111x 1000
0011 1111 1000
0x3F8


0x_1E7
0001 1110 0111
0011 110x 0111
0011 1100 0111
0x3C7

0x_1E6
0001 1110 0110
0011 110x 0110
0011 1100 0110
0x3C6

0x_1E0
0001 1110 0000
0011 110x 0000
0011 1100 0000
0x3C0

0x_100
0001 0000 0000
0010 000x 0000
0010 0000 0000
0x200

0x_180
0001 1000 0000
0011 000x 0000
0011 0000 0000
0x300

0x_1C0
0001 1100 0000
0011 100x 0000
0011 1000 0000
0x380

0x_118
0001 0001 1000
0010 001x 1000
0010 0011 1000
0x238

0x_110
0001 0001 0000
0010 001x 0000
0010 0010 0000
0x220

0x_008
0000 0000 1000
0000 000x 1000
0000 0000 1000
0x008

0x_108
0001 0000 1000
0010 000x 1000
0010 0000 1000
0x208

0x_1F0
0001 1111 0000
0011 111x 0000
0011 1110 0000
0x3E0

0x_006
0000 0000 0110
0000 000x 0110
0000 0000 0110
0x006

0x_001
0000 0000 0001
0000 000x 0001
0000 0000 0001
0x001

0x_1E4
0001 1110 0100
0011 110x 0100
0011 1100 0100
0x3C4

0x_1E8
0001 1110 1000
0011 110x 1000
0011 1100 1000
0x3C8

0x_104
0001 0000 0100
0010 000x 0100
0010 0000 0100
0x204




DROP PROCEDURE IF EXISTS update_log_full;
DELIMITER |
CREATE PROCEDURE update_log_full()
BEGIN
  DECLARE LogCodStart INT;
  DECLARE LogCodEnd INT;
  SET LogCodStart = 0;
  WHILE LogCodStart <= 450000000 DO
    SET LogCodEnd = LogCodStart + 99999;
    UPDATE log_full SET Role = Role + (Role >= 4) WHERE LogCod BETWEEN LogCodStart AND LogCodEnd;
    SET LogCodStart = LogCodEnd + 1;
  END WHILE;
END;
|
DELIMITER ;
CALL update_log_full();













SELECT COUNT(DISTINCT GrpTypCod) FROM
(
(
SELECT crs_grp_types.GrpTypCod AS GrpTypCod,COUNT(*) AS NumStudents,crs_grp.MaxStudents as MaxStudents
FROM crs_grp_types,crs_grp,crs_usr,crs_grp_usr
WHERE crs_grp_types.GrpTypCod=15
AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod
AND crs_grp.Open='Y'
AND crs_grp_types.CrsCod=crs_usr.CrsCod
AND crs_usr.Role=3
AND crs_grp.GrpCod=crs_grp_usr.GrpCod
AND crs_grp_usr.UsrCod=crs_usr.UsrCod
GROUP BY crs_grp.GrpCod
HAVING NumStudents<MaxStudents
)
UNION
(
SELECT crs_grp_types.GrpTypCod AS GrpTypCod,0 AS NumStudents,crs_grp.MaxStudents as MaxStudents
FROM crs_grp_types,crs_grp
WHERE crs_grp_types.GrpTypCod=15
AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod
AND crs_grp.Open='Y'
AND crs_grp.GrpCod NOT IN
(SELECT crs_grp_usr.GrpCod FROM crs_grp_types,crs_usr,crs_grp_usr
WHERE crs_grp_types.GrpTypCod=15
AND crs_grp_types.CrsCod=crs_usr.CrsCod
AND crs_usr.Role=3
AND crs_usr.UsrCod=crs_grp_usr.UsrCod)
)
) AS available_grp_types;


SELECT crs_grp.GrpCod FROM crs_grp,crs_grp_types WHERE crs_grp_types.CrsCod=21 AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod;


DELETE FROM file_view USING file_view,files WHERE files.FileBrowser IN (11,26,5,13) AND files.Cod IN (SELECT crs_grp.GrpCod FROM crs_grp_types,crs_grp WHERE crs_grp_types.CrsCod=21 AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod) AND files.FilCod=file_view.FilCod;



----- swad-core 17.26 (2017-11-08) -----


mysql> SHOW TABLES;
+----------------------+
| Tables_in_swad       |
+----------------------+
| IP_prefs             |
| actions              |
| actions_MFU          |
| admin                |
| agendas              |
| ann_seen             |
| announcements        |
| asg_grp              |
| assignments          |
| att_events           |
| att_grp              |
| att_usr              |
| banners              |
| birthdays_today      |
| centres              |
| chat                 |
| clicks_without_photo |
| clipboard            |
| connected            |
| countries            |
| courses              |
| crs_grp              |
| crs_grp_types        |
| crs_grp_usr          |
| crs_info_read        |
| crs_info_src         |
| crs_info_txt         |
| crs_last             |
| crs_record_fields    |
| crs_records          |
| crs_usr              |
| crs_usr_requests     |
| debug                |
| deg_admin            |
| deg_types            |
| degrees              |
| departments          |
| exam_announcements   |
| expanded_folders     |
| file_browser_last    |
| file_browser_size    |
| file_view            |
| files                |
| forum_disabled_post  |
| forum_post           |
| forum_thr_clip       |
| forum_thr_read       |
| forum_thread         |
| gam_answers          |
| gam_grp              |
| gam_questions        |
| gam_users            |
| games                |
| hidden_params        |
| holidays             |
| institutions         |
| links                |
| log                  |
| log_banners          |
| log_comments         |
| log_full             |
| log_new              |
| log_recent           |
| log_search           |
| log_ws               |
| mail_domains         |
| marks_properties     |
| msg_banned           |
| msg_content          |
| msg_content_deleted  |
| msg_rcv              |
| msg_rcv_deleted      |
| msg_snt              |
| msg_snt_deleted      |
| notices              |
| notices_deleted      |
| notif                |
| pending_emails       |
| pending_passwd       |
| places               |
| plugins              |
| prj_usr              |
| projects             |
| sessions             |
| social_comments      |
| social_comments_fav  |
| social_notes         |
| social_notes_fav     |
| social_posts         |
| social_pubs          |
| social_timelines     |
| sta_degrees          |
| sta_notif            |
| surveys              |
| svy_answers          |
| svy_grp              |
| svy_questions        |
| svy_users            |
| timetable_crs        |
| timetable_crs_backup |
| timetable_tut        |
| timetable_tut_backup |
| tst_answers          |
| tst_config           |
| tst_exam_questions   |
| tst_exams            |
| tst_question_tags    |
| tst_questions        |
| tst_status           |
| tst_tags             |
| usr_IDs              |
| usr_banned           |
| usr_data             |
| usr_duplicated       |
| usr_emails           |
| usr_figures          |
| usr_follow           |
| usr_last             |
| usr_nicknames        |
| usr_report           |
| usr_webs             |
| ws_keys              |
+----------------------+
122 rows in set (0,00 sec)

SHOW TABLE STATUS WHERE Name = 'IP_prefs';
ALTER TABLE IP_prefs ENGINE=MyISAM;
OPTIMIZE TABLE IP_prefs;

SHOW TABLE STATUS WHERE Name = 'actions';
ALTER TABLE actions ENGINE=MyISAM;
OPTIMIZE TABLE actions;

SHOW TABLE STATUS WHERE Name = 'actions_MFU';
ALTER TABLE actions_MFU ENGINE=MyISAM;
OPTIMIZE TABLE actions_MFU;

SHOW TABLE STATUS WHERE Name = 'admin';
ALTER TABLE admin ENGINE=MyISAM;
OPTIMIZE TABLE admin;

SHOW TABLE STATUS WHERE Name = 'agendas';
ALTER TABLE agendas ENGINE=MyISAM;
OPTIMIZE TABLE agendas;

SHOW TABLE STATUS WHERE Name = 'ann_seen';
ALTER TABLE ann_seen ENGINE=MyISAM;
OPTIMIZE TABLE ann_seen;

SHOW TABLE STATUS WHERE Name = 'announcements';
ALTER TABLE announcements ENGINE=MyISAM;
OPTIMIZE TABLE announcements;

SHOW TABLE STATUS WHERE Name = 'asg_grp';
ALTER TABLE asg_grp ENGINE=MyISAM;
OPTIMIZE TABLE asg_grp;

SHOW TABLE STATUS WHERE Name = 'assignments';
ALTER TABLE assignments ENGINE=MyISAM;
OPTIMIZE TABLE assignments;

SHOW TABLE STATUS WHERE Name = 'att_events';
ALTER TABLE att_events ENGINE=MyISAM;
OPTIMIZE TABLE att_events;

SHOW TABLE STATUS WHERE Name = 'att_grp';
ALTER TABLE att_grp ENGINE=MyISAM;
OPTIMIZE TABLE att_grp;

SHOW TABLE STATUS WHERE Name = 'att_usr';
ALTER TABLE att_usr ENGINE=MyISAM;
OPTIMIZE TABLE att_usr;

SHOW TABLE STATUS WHERE Name = 'banners';
ALTER TABLE banners ENGINE=MyISAM;
OPTIMIZE TABLE banners;

SHOW TABLE STATUS WHERE Name = 'birthdays_today';
ALTER TABLE birthdays_today ENGINE=MyISAM;
OPTIMIZE TABLE birthdays_today;

SHOW TABLE STATUS WHERE Name = 'centres';
ALTER TABLE centres ENGINE=MyISAM;
OPTIMIZE TABLE centres;

SHOW TABLE STATUS WHERE Name = 'chat';
ALTER TABLE chat ENGINE=MyISAM;
OPTIMIZE TABLE chat;

SHOW TABLE STATUS WHERE Name = 'clicks_without_photo';
ALTER TABLE clicks_without_photo ENGINE=MyISAM;
OPTIMIZE TABLE clicks_without_photo;

SHOW TABLE STATUS WHERE Name = 'clipboard';
ALTER TABLE clipboard ENGINE=MyISAM;
OPTIMIZE TABLE clipboard;

SHOW TABLE STATUS WHERE Name = 'connected';
ALTER TABLE connected ENGINE=MyISAM;
OPTIMIZE TABLE connected;

SHOW TABLE STATUS WHERE Name = 'countries';
ALTER TABLE countries ENGINE=MyISAM;
OPTIMIZE TABLE countries;

SHOW TABLE STATUS WHERE Name = 'courses';
ALTER TABLE courses ENGINE=MyISAM;
OPTIMIZE TABLE courses;

SHOW TABLE STATUS WHERE Name = 'crs_grp';
ALTER TABLE crs_grp ENGINE=MyISAM;
OPTIMIZE TABLE crs_grp;

SHOW TABLE STATUS WHERE Name = 'crs_grp_types';
ALTER TABLE crs_grp_types ENGINE=MyISAM;
OPTIMIZE TABLE crs_grp_types;

SHOW TABLE STATUS WHERE Name = 'crs_grp_usr';
ALTER TABLE crs_grp_usr ENGINE=MyISAM;
OPTIMIZE TABLE crs_grp_usr;

SHOW TABLE STATUS WHERE Name = 'crs_info_read';
ALTER TABLE crs_info_read ENGINE=MyISAM;
OPTIMIZE TABLE crs_info_read;

SHOW TABLE STATUS WHERE Name = 'crs_info_src';
ALTER TABLE crs_info_src ENGINE=MyISAM;
OPTIMIZE TABLE crs_info_src;

SHOW TABLE STATUS WHERE Name = 'crs_info_txt';
ALTER TABLE crs_info_txt ENGINE=MyISAM;
OPTIMIZE TABLE crs_info_txt;

SHOW TABLE STATUS WHERE Name = 'crs_last';
ALTER TABLE crs_last ENGINE=MyISAM;
OPTIMIZE TABLE crs_last;

SHOW TABLE STATUS WHERE Name = 'crs_record_fields';
ALTER TABLE crs_record_fields ENGINE=MyISAM;
OPTIMIZE TABLE crs_record_fields;

SHOW TABLE STATUS WHERE Name = 'crs_records';
ALTER TABLE crs_records ENGINE=MyISAM;
OPTIMIZE TABLE crs_records;

UPDATE crs_usr SET LastAccTst=FROM_UNIXTIME(0) WHERE LastAccTst=0;
SHOW TABLE STATUS WHERE Name = 'crs_usr';
ALTER TABLE crs_usr ENGINE=MyISAM;
OPTIMIZE TABLE crs_usr;

SHOW TABLE STATUS WHERE Name = 'crs_usr_requests';
ALTER TABLE crs_usr_requests ENGINE=MyISAM;
OPTIMIZE TABLE crs_usr_requests;

SHOW TABLE STATUS WHERE Name = 'debug';
ALTER TABLE debug ENGINE=MyISAM;
OPTIMIZE TABLE debug;

SHOW TABLE STATUS WHERE Name = 'deg_types';
ALTER TABLE deg_types ENGINE=MyISAM;
OPTIMIZE TABLE deg_types;

SHOW TABLE STATUS WHERE Name = 'degrees';
ALTER TABLE degrees ENGINE=MyISAM;
OPTIMIZE TABLE degrees;

SHOW TABLE STATUS WHERE Name = 'departments';
ALTER TABLE departments ENGINE=MyISAM;
OPTIMIZE TABLE departments;

SHOW TABLE STATUS WHERE Name = 'exam_announcements';
ALTER TABLE exam_announcements ENGINE=MyISAM;
OPTIMIZE TABLE exam_announcements;

SHOW TABLE STATUS WHERE Name = 'expanded_folders';
ALTER TABLE expanded_folders ENGINE=MyISAM;
OPTIMIZE TABLE expanded_folders;

SHOW TABLE STATUS WHERE Name = 'file_browser_last';
ALTER TABLE file_browser_last ENGINE=MyISAM;
OPTIMIZE TABLE file_browser_last;

SHOW TABLE STATUS WHERE Name = 'file_browser_size';
ALTER TABLE file_browser_size ENGINE=MyISAM;
OPTIMIZE TABLE file_browser_size;

SHOW TABLE STATUS WHERE Name = 'file_view';
ALTER TABLE file_view ENGINE=MyISAM;
OPTIMIZE TABLE file_view;

SHOW TABLE STATUS WHERE Name = 'files';
ALTER TABLE files ENGINE=MyISAM;
OPTIMIZE TABLE files;

SHOW TABLE STATUS WHERE Name = 'forum_disabled_post';
ALTER TABLE forum_disabled_post ENGINE=MyISAM;
OPTIMIZE TABLE forum_disabled_post;

SHOW TABLE STATUS WHERE Name = 'forum_post';
ALTER TABLE forum_post ENGINE=MyISAM;
OPTIMIZE TABLE forum_post;

SHOW TABLE STATUS WHERE Name = 'forum_thr_clip';
ALTER TABLE forum_thr_clip ENGINE=MyISAM;
OPTIMIZE TABLE forum_thr_clip;

SHOW TABLE STATUS WHERE Name = 'forum_thr_read';
ALTER TABLE forum_thr_read ENGINE=MyISAM;
OPTIMIZE TABLE forum_thr_read;

SHOW TABLE STATUS WHERE Name = 'forum_thread';
ALTER TABLE forum_thread ENGINE=MyISAM;
OPTIMIZE TABLE forum_thread;

SHOW TABLE STATUS WHERE Name = 'gam_answers';
ALTER TABLE gam_answers ENGINE=MyISAM;
OPTIMIZE TABLE gam_answers;

SHOW TABLE STATUS WHERE Name = 'gam_grp';
ALTER TABLE gam_grp ENGINE=MyISAM;
OPTIMIZE TABLE gam_grp;

SHOW TABLE STATUS WHERE Name = 'gam_questions';
ALTER TABLE gam_questions ENGINE=MyISAM;
OPTIMIZE TABLE gam_questions;

SHOW TABLE STATUS WHERE Name = 'gam_users';
ALTER TABLE gam_users ENGINE=MyISAM;
OPTIMIZE TABLE gam_users;

SHOW TABLE STATUS WHERE Name = 'games';
ALTER TABLE games ENGINE=MyISAM;
OPTIMIZE TABLE games;

SHOW TABLE STATUS WHERE Name = 'hidden_params';
ALTER TABLE hidden_params ENGINE=MyISAM;
OPTIMIZE TABLE hidden_params;

SHOW TABLE STATUS WHERE Name = 'holidays';
ALTER TABLE holidays ENGINE=MyISAM;
OPTIMIZE TABLE holidays;

SHOW TABLE STATUS WHERE Name = 'institutions';
ALTER TABLE institutions ENGINE=MyISAM;
OPTIMIZE TABLE institutions;

SHOW TABLE STATUS WHERE Name = 'links';
ALTER TABLE links ENGINE=MyISAM;
OPTIMIZE TABLE links;

SHOW TABLE STATUS WHERE Name = 'links';
ALTER TABLE links ENGINE=MyISAM;
OPTIMIZE TABLE links;

SHOW TABLE STATUS WHERE Name = 'log_banners';
ALTER TABLE log_banners ENGINE=MyISAM;
OPTIMIZE TABLE log_banners;

SHOW TABLE STATUS WHERE Name = 'log_comments';
ALTER TABLE log_comments ENGINE=MyISAM;
OPTIMIZE TABLE log_comments;

SHOW TABLE STATUS WHERE Name = 'log_full';
ALTER TABLE log_full ENGINE=MyISAM;	# WARNING: This can be very slow
OPTIMIZE TABLE log_full; 		# WARNING: This can be very slow

SHOW TABLE STATUS WHERE Name = 'log_recent';
ALTER TABLE log_recent ENGINE=MyISAM;	# WARNING: This can be very slow
OPTIMIZE TABLE log_recent; 		# WARNING: This can be very slow

SHOW TABLE STATUS WHERE Name = 'log_search';
ALTER TABLE log_search ENGINE=MyISAM;
OPTIMIZE TABLE log_search;

SHOW TABLE STATUS WHERE Name = 'log_ws';
ALTER TABLE log_ws ENGINE=MyISAM;
OPTIMIZE TABLE log_ws;

SHOW TABLE STATUS WHERE Name = 'mail_domains';
ALTER TABLE mail_domains ENGINE=MyISAM;
OPTIMIZE TABLE mail_domains;

SHOW TABLE STATUS WHERE Name = 'marks_properties';
ALTER TABLE marks_properties ENGINE=MyISAM;
OPTIMIZE TABLE marks_properties;

SHOW TABLE STATUS WHERE Name = 'msg_banned';
ALTER TABLE msg_banned ENGINE=MyISAM;
OPTIMIZE TABLE msg_banned;

SHOW TABLE STATUS WHERE Name = 'msg_content';
ALTER TABLE msg_content ENGINE=MyISAM;
OPTIMIZE TABLE msg_content;

SHOW TABLE STATUS WHERE Name = 'msg_content_deleted';
ALTER TABLE msg_content_deleted ENGINE=MyISAM;
OPTIMIZE TABLE msg_content_deleted;

SHOW TABLE STATUS WHERE Name = 'msg_rcv';
ALTER TABLE msg_rcv ENGINE=MyISAM;
OPTIMIZE TABLE msg_rcv;

SHOW TABLE STATUS WHERE Name = 'msg_rcv_deleted';
ALTER TABLE msg_rcv_deleted ENGINE=MyISAM;
OPTIMIZE TABLE msg_rcv_deleted;

SHOW TABLE STATUS WHERE Name = 'msg_snt';
ALTER TABLE msg_snt ENGINE=MyISAM;
OPTIMIZE TABLE msg_snt;

SHOW TABLE STATUS WHERE Name = 'msg_snt_deleted';
ALTER TABLE msg_snt_deleted ENGINE=MyISAM;
OPTIMIZE TABLE msg_snt_deleted;

SHOW TABLE STATUS WHERE Name = 'notices';
ALTER TABLE notices ENGINE=MyISAM;
OPTIMIZE TABLE notices;

SHOW TABLE STATUS WHERE Name = 'notices_deleted';
ALTER TABLE notices_deleted ENGINE=MyISAM;
OPTIMIZE TABLE notices_deleted;

SHOW TABLE STATUS WHERE Name = 'notif';
ALTER TABLE notif ENGINE=MyISAM;
OPTIMIZE TABLE notif;

SHOW TABLE STATUS WHERE Name = 'pending_emails';
ALTER TABLE pending_emails ENGINE=MyISAM;
OPTIMIZE TABLE pending_emails;

SHOW TABLE STATUS WHERE Name = 'pending_passwd';
ALTER TABLE pending_passwd ENGINE=MyISAM;
OPTIMIZE TABLE pending_passwd;

SHOW TABLE STATUS WHERE Name = 'places';
ALTER TABLE places ENGINE=MyISAM;
OPTIMIZE TABLE places;

SHOW TABLE STATUS WHERE Name = 'plugins';
ALTER TABLE plugins ENGINE=MyISAM;
OPTIMIZE TABLE plugins;

SHOW TABLE STATUS WHERE Name = 'prj_usr';
ALTER TABLE prj_usr ENGINE=MyISAM;
OPTIMIZE TABLE prj_usr;

SHOW TABLE STATUS WHERE Name = 'projects';
ALTER TABLE projects ENGINE=MyISAM;
OPTIMIZE TABLE projects;

SHOW TABLE STATUS WHERE Name = 'sessions';
ALTER TABLE sessions ENGINE=MyISAM;
OPTIMIZE TABLE sessions;

SHOW TABLE STATUS WHERE Name = 'social_comments';
ALTER TABLE social_comments ENGINE=MyISAM;
OPTIMIZE TABLE social_comments;

SHOW TABLE STATUS WHERE Name = 'social_comments_fav';
ALTER TABLE social_comments_fav ENGINE=MyISAM;
OPTIMIZE TABLE social_comments_fav;

SHOW TABLE STATUS WHERE Name = 'social_notes';
ALTER TABLE social_notes ENGINE=MyISAM;
OPTIMIZE TABLE social_notes;

SHOW TABLE STATUS WHERE Name = 'social_notes_fav';
ALTER TABLE social_notes_fav ENGINE=MyISAM;
OPTIMIZE TABLE social_notes_fav;

SHOW TABLE STATUS WHERE Name = 'social_posts';
ALTER TABLE social_posts ENGINE=MyISAM;
OPTIMIZE TABLE social_posts;

SHOW TABLE STATUS WHERE Name = 'social_pubs';
ALTER TABLE social_pubs ENGINE=MyISAM;
OPTIMIZE TABLE social_pubs;

SHOW TABLE STATUS WHERE Name = 'social_timelines';
ALTER TABLE social_timelines ENGINE=MyISAM;
OPTIMIZE TABLE social_timelines;

SHOW TABLE STATUS WHERE Name = 'sta_degrees';
ALTER TABLE sta_degrees ENGINE=MyISAM;
OPTIMIZE TABLE sta_degrees;

SHOW TABLE STATUS WHERE Name = 'sta_notif';
ALTER TABLE sta_notif ENGINE=MyISAM;
OPTIMIZE TABLE sta_notif;

SHOW TABLE STATUS WHERE Name = 'surveys';
ALTER TABLE surveys ENGINE=MyISAM;
OPTIMIZE TABLE surveys;

SHOW TABLE STATUS WHERE Name = 'svy_answers';
ALTER TABLE svy_answers ENGINE=MyISAM;
OPTIMIZE TABLE svy_answers;

SHOW TABLE STATUS WHERE Name = 'svy_grp';
ALTER TABLE svy_grp ENGINE=MyISAM;
OPTIMIZE TABLE svy_grp;

SHOW TABLE STATUS WHERE Name = 'svy_questions';
ALTER TABLE svy_questions ENGINE=MyISAM;
OPTIMIZE TABLE svy_questions;

SHOW TABLE STATUS WHERE Name = 'svy_users';
ALTER TABLE svy_users ENGINE=MyISAM;
OPTIMIZE TABLE svy_users;

SHOW TABLE STATUS WHERE Name = 'timetable_crs';
ALTER TABLE timetable_crs ENGINE=MyISAM;
OPTIMIZE TABLE timetable_crs;

SHOW TABLE STATUS WHERE Name = 'timetable_tut';
ALTER TABLE timetable_tut ENGINE=MyISAM;
OPTIMIZE TABLE timetable_tut;

SHOW TABLE STATUS WHERE Name = 'tst_answers';
ALTER TABLE tst_answers ENGINE=MyISAM;
OPTIMIZE TABLE tst_answers;

SHOW TABLE STATUS WHERE Name = 'tst_config';
ALTER TABLE tst_config ENGINE=MyISAM;
OPTIMIZE TABLE tst_config;

SHOW TABLE STATUS WHERE Name = 'tst_exam_questions';
ALTER TABLE tst_exam_questions ENGINE=MyISAM;
OPTIMIZE TABLE tst_exam_questions;

SHOW TABLE STATUS WHERE Name = 'tst_exams';
ALTER TABLE tst_exams ENGINE=MyISAM;
OPTIMIZE TABLE tst_exams;

SHOW TABLE STATUS WHERE Name = 'tst_question_tags';
ALTER TABLE tst_question_tags ENGINE=MyISAM;
OPTIMIZE TABLE tst_question_tags;

SHOW TABLE STATUS WHERE Name = 'tst_questions';
ALTER TABLE tst_questions ENGINE=MyISAM;
OPTIMIZE TABLE tst_questions;

SHOW TABLE STATUS WHERE Name = 'tst_status';
ALTER TABLE tst_status ENGINE=MyISAM;
OPTIMIZE TABLE tst_status;

SHOW TABLE STATUS WHERE Name = 'tst_tags';
ALTER TABLE tst_tags ENGINE=MyISAM;
OPTIMIZE TABLE tst_tags;

SHOW TABLE STATUS WHERE Name = 'usr_IDs';
ALTER TABLE usr_IDs ENGINE=MyISAM;
OPTIMIZE TABLE usr_IDs;

SHOW TABLE STATUS WHERE Name = 'usr_banned';
ALTER TABLE usr_banned ENGINE=MyISAM;
OPTIMIZE TABLE usr_banned;

SHOW TABLE STATUS WHERE Name = 'usr_data';
ALTER TABLE usr_data ENGINE=MyISAM;
OPTIMIZE TABLE usr_data;

SHOW TABLE STATUS WHERE Name = 'usr_duplicated';
ALTER TABLE usr_duplicated ENGINE=MyISAM;
OPTIMIZE TABLE usr_duplicated;

SHOW TABLE STATUS WHERE Name = 'usr_emails';
ALTER TABLE usr_emails ENGINE=MyISAM;
OPTIMIZE TABLE usr_emails;

SHOW TABLE STATUS WHERE Name = 'usr_figures';
ALTER TABLE usr_figures ENGINE=MyISAM;
OPTIMIZE TABLE usr_figures;

SHOW TABLE STATUS WHERE Name = 'usr_follow';
ALTER TABLE usr_follow ENGINE=MyISAM;
OPTIMIZE TABLE usr_follow;

SHOW TABLE STATUS WHERE Name = 'usr_last';
ALTER TABLE usr_last ENGINE=MyISAM;
OPTIMIZE TABLE usr_last;

SHOW TABLE STATUS WHERE Name = 'usr_nicknames';
ALTER TABLE usr_nicknames ENGINE=MyISAM;
OPTIMIZE TABLE usr_nicknames;

SHOW TABLE STATUS WHERE Name = 'usr_report';
ALTER TABLE usr_report ENGINE=MyISAM;
OPTIMIZE TABLE usr_report;

SHOW TABLE STATUS WHERE Name = 'usr_webs';
ALTER TABLE usr_webs ENGINE=MyISAM;
OPTIMIZE TABLE usr_webs;

SHOW TABLE STATUS WHERE Name = 'ws_keys';
ALTER TABLE ws_keys ENGINE=MyISAM;
OPTIMIZE TABLE ws_keys;


SELECT Weekday,TIME_TO_SEC(StartTime) AS S,TIME_TO_SEC(Duration) AS D,Place,ClassType,GrpCod FROM timetable_crs WHERE CrsCod=19 ORDER BY Weekday,S,ClassType,GrpCod,Place,D DESC;


CREATE TABLE IF NOT EXISTS firewall (ClickTime DATETIME NOT NULL,IP CHAR(15) NOT NULL,INDEX(ClickTime),INDEX(IP));



SELECT UsrCod,FavCod FROM
(
(SELECT UsrCod,FavCod FROM social_notes_fav WHERE NotCod=278 ORDER BY FavCod)
 UNION
(SELECT 1,(SELECT FavCod FROM social_notes_fav WHERE NotCod=278 AND UsrCod=1) AS FavCod)
ORDER BY FavCod LIMIT 7
) AS favers;
 
SELECT UsrCod FROM social_notes_fav WHERE NotCod=278 AND UsrCod<>7 AND UsrCod<>1 ORDER BY FavCod LIMIT 7;

SELECT CAST(~0 AS UNSIGNED);


SELECT last_logs.LogCod,last_logs.ActCod,last_logs.Dif,last_logs.Role,last_logs.CtyCod,last_logs.InsCod,last_logs.CtrCod,last_logs.DegCod,actions.Txt FROM (SELECT LogCod,ActCod,UNIX_TIMESTAMP()-UNIX_TIMESTAMP(ClickTime) AS Dif,Role,CtyCod,InsCod,CtrCod,DegCod FROM log_recent ORDER BY LogCod DESC LIMIT 20) AS last_logs LEFT JOIN actions ON last_logs.ActCod=actions.ActCod WHERE actions.Language='es';


ALTER TABLE forum_post CHANGE COLUMN MediaType MediaType ENUM('none','jpg','gif') NOT NULL DEFAULT 'none';
ALTER TABLE msg_content CHANGE COLUMN MediaType MediaType ENUM('none','jpg','gif') NOT NULL DEFAULT 'none';
ALTER TABLE msg_content_deleted CHANGE COLUMN MediaType MediaType ENUM('none','jpg','gif') NOT NULL DEFAULT 'none';
ALTER TABLE social_comments CHANGE COLUMN MediaType MediaType ENUM('none','jpg','gif') NOT NULL DEFAULT 'none';
ALTER TABLE social_posts CHANGE COLUMN MediaType MediaType ENUM('none','jpg','gif') NOT NULL DEFAULT 'none';
ALTER TABLE tst_answers CHANGE COLUMN MediaType MediaType ENUM('none','jpg','gif') NOT NULL DEFAULT 'none';
ALTER TABLE tst_questions CHANGE COLUMN MediaType MediaType ENUM('none','jpg','gif') NOT NULL DEFAULT 'none';

UPDATE forum_post SET MediaType='none' WHERE MediaName='';
UPDATE msg_content SET MediaType='none' WHERE MediaName='';
UPDATE msg_content_deleted SET MediaType='none' WHERE MediaName='';
UPDATE social_comments SET MediaType='none' WHERE MediaName='';
UPDATE social_posts SET MediaType='none' WHERE MediaName='';
UPDATE tst_answers SET MediaType='none' WHERE MediaName='';
UPDATE tst_questions SET MediaType='none' WHERE MediaName='';

UPDATE forum_post SET MediaType='jpg' WHERE MediaType<>'gif' AND MediaName<>'';
UPDATE msg_content SET MediaType='jpg' WHERE MediaType<>'gif' AND MediaName<>'';
UPDATE msg_content_deleted SET MediaType='jpg' WHERE MediaType<>'gif' AND MediaName<>'';
UPDATE social_comments SET MediaType='jpg' WHERE MediaType<>'gif' AND MediaName<>'';
UPDATE social_posts SET MediaType='jpg' WHERE MediaType<>'gif' AND MediaName<>'';
UPDATE tst_answers SET MediaType='jpg' WHERE MediaType<>'gif' AND MediaName<>'';
UPDATE tst_questions SET MediaType='jpg' WHERE MediaType<>'gif' AND MediaName<>'';

------------



UPDATE usr_figures 
SET NumSocPub = T.NumSocPub
FROM (
    SELECT PublisherCod,COUNT(*) AS NumSocPub FROM social_pubs GROUP BY PublisherCod) AS T
WHERE 
    usr_figures.UsrCod = T.PublisherCod;


------------



UPDATE usr_figures,(SELECT PublisherCod,COUNT(*) AS NumSocPub FROM social_pubs GROUP BY PublisherCod) AS publishers SET usr_figures.NumSocPub=publishers.NumSocPub WHERE usr_figures.NumSocPub<0 AND usr_figures.UsrCod=publishers.PublisherCod;
UPDATE usr_figures SET NumSocPub=0 WHERE NumSocPub<0;

UPDATE usr_figures,(SELECT UsrCod,COUNT(*) AS NumForPst FROM forum_post GROUP BY UsrCod) AS posters SET usr_figures.NumForPst=posters.NumForPst WHERE usr_figures.NumForPst<0 AND usr_figures.UsrCod=posters.UsrCod;
UPDATE usr_figures SET NumForPst=0 WHERE NumForPst<0;

UPDATE usr_figures,(SELECT UsrCod,SUM(NMS) AS NumMsgSnt FROM (SELECT UsrCod,COUNT(*) AS NMS FROM msg_snt GROUP BY UsrCod UNION SELECT Usrcod,COUNT(*) AS NMS FROM msg_snt_deleted GROUP BY UsrCod) AS MS GROUP BY UsrCod) AS senders SET usr_figures.NumMsgSnt=senders.NumMsgSnt WHERE usr_figures.NumMsgSnt<0 AND usr_figures.UsrCod=senders.UsrCod;
UPDATE usr_figures SET NumMsgSnt=0 WHERE NumMsgSnt<0;

UPDATE usr_figures,(SELECT UsrCod,SUM(NumViews) AS NumFileViews FROM file_view WHERE UsrCod>0 GROUP BY UsrCod) AS viewers SET usr_figures.NumFileViews=viewers.NumFileViews WHERE usr_figures.NumFileViews<0 AND usr_figures.UsrCod=viewers.UsrCod;
UPDATE usr_figures SET NumFileViews=0 WHERE NumFileViews<0;





--------------------


SELECT degrees.ShortName,main_degree.MaxRole
 FROM degrees,
 (SELECT courses.DegCod AS DegCod,MAX(crs_usr.Role) AS MaxRole,COUNT(*) AS N
 FROM crs_usr,courses
 WHERE crs_usr.UsrCod=1
 AND crs_usr.CrsCod=courses.CrsCod
 GROUP BY courses.DegCod
 ORDER BY N DESC LIMIT 1)
 AS main_degree
 WHERE degrees.DegCod=main_degree.DegCod;
			     
			     
ALTER TABLE usr_data CHANGE COLUMN BasicProfileVisibility BaPrfVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown';
ALTER TABLE usr_data DROP COLUMN ExtendedProfileVisibility;
			     
			     
			     
			     
 SELECT DISTINCT UsrCod FROM
 (
 (SELECT DISTINCT UsrCod FROM
 (
 (
 SELECT DISTINCT usr_follow.FollowedCod AS UsrCod
 FROM usr_follow,
 (SELECT FollowedCod FROM usr_follow
 WHERE FollowerCod=1346) AS my_followed,
 usr_data
 WHERE usr_follow.FollowerCod=my_followed.FollowedCod
 AND usr_follow.FollowedCod<>1346
 AND usr_follow.FollowedCod=usr_data.UsrCod
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('system','world')
 AND usr_data.Photo<>''
 )
 UNION 
 (
 SELECT DISTINCT crs_usr.UsrCod
 FROM crs_usr,
 (SELECT CrsCod FROM crs_usr
 WHERE UsrCod=1346) AS my_crs,
 usr_data
 WHERE crs_usr.CrsCod=my_crs.CrsCod
 AND crs_usr.UsrCod<>1346
 AND crs_usr.UsrCod=usr_data.UsrCod
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('course','system','world')
 AND usr_data.Photo<>''
 )
 UNION 
 (
 SELECT DISTINCT crs_usr.UsrCod
 FROM crs_usr,
 (SELECT CrsCod,Role FROM crs_usr
 WHERE UsrCod=1346) AS my_crs_role,
 usr_data
 WHERE crs_usr.CrsCod=my_crs_role.CrsCod
 AND crs_usr.Role<>my_crs_role.Role
 AND crs_usr.UsrCod=usr_data.UsrCod
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('user','course','system','world')
 AND usr_data.Photo<>''
 )
 ) AS LikelyKnownUsrsToFollow
 WHERE UsrCod NOT IN
 (SELECT FollowedCod FROM usr_follow
 WHERE FollowerCod=1346)
 ORDER BY RAND() LIMIT 6
 )
 UNION 
 (
 SELECT usr_data.UsrCod FROM usr_data,
(SELECT ROUND(RAND() * (SELECT MAX(UsrCod) FROM usr_data)) AS UsrCod) AS random_usr
 WHERE usr_data.UsrCod<>1346
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('system','world')
 AND usr_data.Photo<>''
 AND usr_data.UsrCod NOT IN
 (SELECT FollowedCod FROM usr_follow
 WHERE FollowerCod=1346)
 AND usr_data.UsrCod>=random_usr.UsrCod
 LIMIT 3
 )
 ) AS UsrsToFollow
 ORDER BY RAND() LIMIT 3;






 SELECT usr_data.UsrCod FROM usr_data,
(SELECT ROUND(RAND() * (SELECT MAX(UsrCod) FROM usr_data)) AS UsrCod) AS random_usr
 WHERE usr_data.UsrCod<>1346
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('system','world')
 AND usr_data.Photo<>''
 AND usr_data.UsrCod NOT IN
 (SELECT FollowedCod FROM usr_follow
 WHERE FollowerCod=1346)
 AND usr_data.UsrCod>=random_usr.UsrCod
 LIMIT 3;












			     
 SELECT DISTINCT UsrCod FROM
 (
 (SELECT DISTINCT UsrCod FROM
 (
 (
 SELECT DISTINCT usr_follow.FollowedCod AS UsrCod
 FROM usr_follow,
 (SELECT FollowedCod FROM usr_follow
 WHERE FollowerCod=1346) AS my_followed,
 usr_data
 WHERE usr_follow.FollowerCod=my_followed.FollowedCod
 AND usr_follow.FollowedCod<>1346
 AND usr_follow.FollowedCod=usr_data.UsrCod
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('system','world')
 AND usr_data.Photo<>''
 )
 UNION 
 (
 SELECT DISTINCT crs_usr.UsrCod
 FROM crs_usr,
 (SELECT CrsCod FROM crs_usr
 WHERE UsrCod=1346) AS my_crs,
 usr_data
 WHERE crs_usr.CrsCod=my_crs.CrsCod
 AND crs_usr.UsrCod<>1346
 AND crs_usr.UsrCod=usr_data.UsrCod
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('course','system','world')
 AND usr_data.Photo<>''
 )
 UNION 
 (
 SELECT DISTINCT crs_usr.UsrCod
 FROM crs_usr,
 (SELECT CrsCod,Role FROM crs_usr
 WHERE UsrCod=1346) AS my_crs_role,
 usr_data
 WHERE crs_usr.CrsCod=my_crs_role.CrsCod
 AND crs_usr.Role<>my_crs_role.Role
 AND crs_usr.UsrCod=usr_data.UsrCod
 AND usr_data.Surname1<>''
 AND usr_data.FirstName<>''
 AND usr_data.PhotoVisibility IN ('user','course','system','world')
 AND usr_data.Photo<>''
 )
 ) AS LikelyKnownUsrsToFollow
 WHERE UsrCod NOT IN
 (SELECT FollowedCod FROM usr_follow
 WHERE FollowerCod=1346)
 ORDER BY RAND() LIMIT 6
 )
 ) AS UsrsToFollow
 ORDER BY RAND() LIMIT 3;
 
 
 
 
 
 
INSERT INTO social_pubs (NotCod,PublisherCod,PubType,TimePublish) VALUES (600,7140,2,NOW());

INSERT IGNORE INTO social_comments_fav (PubCod,UsrCod,TimeFav) VALUES (1294,7140,NOW());
 

SELECT COUNT(*) FROM degrees WHERE CtrCod=2 AND FullName='M�ster Universitario en Electr�nica Industrial' AND DegCod<>-1;

-----------------

SELECT UNIX_TIMESTAMP(MIN(StartTime)),UNIX_TIMESTAMP(MAX(EndTime)) FROM gam_matches WHERE GamCod=1;

----------------------


SELECT MchCod,GamCod,UsrCod,UNIX_TIMESTAMP(StartTime),UNIX_TIMESTAMP(EndTime),Title,QstInd,QstCod,UNIX_TIMESTAMP(QstStartTime),ShowingAnswers,Finished FROM gam_matches WHERE GamCod=7 ORDER BY MchCod;


REPLACE gam_time (MchCod,QstInd,ElapsedTime) VALUES (61,1,ADDTIME(ElapsedTime,SEC_TO_TIME(1)));

INSERT INTO gam_time (MchCod,QstInd) VALUES (61,1,SEC_TO_TIME(1)) ON DUPLICATE KEY UPDATE ElapsedTime=ADDTIME(ElapsedTime,SEC_TO_TIME(1));
  


----------------

SELECT gam_questions.QstCod,
       gam_questions.QstInd,
       mch_indexes.Indexes
 FROM mch_matches,gam_questions,mch_indexes
 WHERE mch_matches.MchCod=69
 AND mch_matches.GamCod=gam_questions.GamCod
 AND mch_matches.MchCod=mch_indexes.MchCod
 AND gam_questions.QstInd=mch_indexes.QstInd
 ORDER BY gam_questions.QstInd;
 
SELECT gam_questions.QstCod,
       gam_questions.QstInd,
       mch_indexes.Indexes
 FROM mch_matches,gam_questions,mch_indexes
 WHERE mch_matches.MchCod=69
 AND mch_matches.GamCod=gam_questions.GamCod
 AND mch_matches.MchCod=mch_indexes.MchCod
 AND gam_questions.QstInd=mch_indexes.QstInd
 ORDER BY gam_questions.QstInd;
 
 
 SELECT Correct FROM tst_answers WHERE QstCod=1787 ORDER BY AnsInd;

 -------------------------------------------------------------------------------
 
 
 
 
 
swad 18.128 ----> swad 19.25

Ficheros:
---------

sudo cp icon/list.svg /var/www/html/swad/icon/

sudo cp icon/step-backward.svg /var/www/html/swad/icon/
sudo cp icon/step-forward.svg /var/www/html/swad/icon/

sudo cp -a fontawesome /var/www/html/swad/



Base de datos:
--------------

UPDATE actions SET Txt='Solicitar la creaci&oacute;n de una cuenta (1/2)' WHERE ActCod='36' AND Language='es';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1787','es','N','Solicitar la creaci&oacute;n de una cuenta (2/2)');

ALTER TABLE games DROP COLUMN StartTime,DROP COLUMN EndTime;

UPDATE actions SET Obsolete='Y' WHERE ActCod=1779;

DROP TABLE IF EXISTS gam_answers;
CREATE TABLE IF NOT EXISTS gam_answers (MchCod INT NOT NULL,UsrCod INT NOT NULL,QstInd INT NOT NULL,AnsInd TINYINT NOT NULL,UNIQUE INDEX(MchCod,UsrCod,QstInd));

DROP TABLE IF EXISTS gam_players;
CREATE TABLE IF NOT EXISTS gam_players (MchCod INT NOT NULL,UsrCod INT NOT NULL,TS TIMESTAMP,UNIQUE INDEX(MchCod,UsrCod));
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1788','es','N','Refrescar partida (como profesor)');

UPDATE actions SET Txt='Mostrar pregunta siguiente en partida (como profesor)' WHERE ActCod='1672' AND Language='es';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1789','es','N','Mostrar pregunta actual en partida (como profesor)');

CREATE TABLE IF NOT EXISTS gam_mch_being_played (MchCod INT NOT NULL,TS TIMESTAMP,UNIQUE INDEX(MchCod));

DROP TABLE IF EXISTS gam_users;
ALTER TABLE games ENGINE=MyISAM;
ALTER TABLE gam_answers ENGINE=MyISAM;
ALTER TABLE gam_grp ENGINE=MyISAM;
ALTER TABLE gam_matches ENGINE=MyISAM;
ALTER TABLE gam_mch_being_played ENGINE=MyISAM;
ALTER TABLE gam_players ENGINE=MyISAM;
ALTER TABLE gam_questions ENGINE=MyISAM;

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1790','es','N','Mostrar pregunta anterior en partida (como profesor)');

ALTER TABLE gam_matches DROP COLUMN Finished;

CREATE TABLE IF NOT EXISTS gam_time (MchCod INT NOT NULL,QstInd INT NOT NULL,ElapsedTime TIME NOT NULL DEFAULT 0,UNIQUE INDEX(MchCod,QstInd));

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1791','es','N','Pausar partida (como profesor)');

UPDATE actions SET Txt='Comenzar/reanudar partida (como profesor)' WHERE ActCod='1789' AND Language='es';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1792','es','N','Mostrar solo enunciado en partida (como profesor)');

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1793','es','N','Mostrar enunciado y respuestas en partida (como profesor)');

ALTER TABLE gam_matches ADD COLUMN ShowResults ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER ShowingAnswers;
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1794','es','N','Cambiar presentaci&oacute;n de resultados en partida (como profesor)');

ALTER TABLE gam_matches DROP COLUMN ShowingAnswers,DROP COLUMN ShowResults;
ALTER TABLE gam_matches ADD COLUMN ShowResults ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER QstStartTime;
ALTER TABLE gam_matches ADD COLUMN Showing ENUM('stem','answers','results') NOT NULL DEFAULT 'stem' AFTER ShowResults;

ALTER TABLE gam_matches DROP COLUMN ShowResults,DROP COLUMN Showing;
ALTER TABLE gam_matches ADD COLUMN Showing ENUM('wording','answers','request','results') NOT NULL DEFAULT 'wording' AFTER QstStartTime;
UPDATE actions SET Obsolete='Y' WHERE ActCod IN (1792,1793,1794);

ALTER TABLE gam_matches DROP COLUMN Showing;
ALTER TABLE gam_matches ADD COLUMN ShowResults ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER QstStartTime;
ALTER TABLE gam_matches ADD COLUMN Showing ENUM('wording','answers','results') NOT NULL DEFAULT 'wording' AFTER ShowResults;
DELETE FROM actions WHERE ActCod='1794';
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1794','es','N','Cambiar presentaci&oacute;n de resultados en partida (como profesor)');
UPDATE actions SET Obsolete='Y' WHERE ActCod=1795;

ALTER TABLE gam_matches CHANGE COLUMN ShowResults ShowResults ENUM('N','Y') NOT NULL DEFAULT 'Y';

ALTER TABLE gam_matches DROP COLUMN Showing;
ALTER TABLE gam_matches ADD COLUMN Showing ENUM('stem','answers','results') NOT NULL DEFAULT 'stem' AFTER ShowResults;

RENAME TABLE gam_answers TO mch_answers;
RENAME TABLE gam_grp TO mch_groups;
RENAME TABLE gam_matches TO mch_matches;
RENAME TABLE gam_players TO mch_players;
RENAME TABLE gam_mch_being_played TO mch_playing;
RENAME TABLE gam_time TO mch_times;
RENAME TABLE games TO gam_games;

ALTER TABLE mch_matches DROP COLUMN Showing;
ALTER TABLE mch_matches ADD COLUMN Showing ENUM('nothing','stem','answers','results') NOT NULL DEFAULT 'nothing' AFTER ShowResults;

ALTER TABLE mch_matches DROP COLUMN QstStartTime;

UPDATE actions SET Obsolete='Y' WHERE ActCod IN (1658,1659);

INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1795','es','N','Seleccionar fechas para mis result. partidas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1796','es','N','Ver mis resultados de partidas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1797','es','N','Ver una partida m&iacute;a ya realizada');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1798','es','N','Seleccionar usuarios para result. partidas');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1799','es','N','Ver resultados de partidas de usuarios');
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1800','es','N','Ver una partida ya realizada');

CREATE TABLE IF NOT EXISTS mch_results (MchResCod INT NOT NULL AUTO_INCREMENT,MchCod INT NOT NULL,UsrCod INT NOT NULL,NumQsts INT NOT NULL DEFAULT 0,NumQstsNotBlank INT NOT NULL DEFAULT 0,Score DOUBLE PRECISION NOT NULL DEFAULT 0,UNIQUE INDEX(MchResCod),UNIQUE INDEX(MchCod,UsrCod));

DROP TABLE IF EXISTS mch_results;
CREATE TABLE IF NOT EXISTS mch_results (MchCod INT NOT NULL,UsrCod INT NOT NULL,NumQsts INT NOT NULL DEFAULT 0,NumQstsNotBlank INT NOT NULL DEFAULT 0,Score DOUBLE PRECISION NOT NULL DEFAULT 0,UNIQUE INDEX(MchCod,UsrCod));

DROP TABLE IF EXISTS mch_results;
CREATE TABLE IF NOT EXISTS mch_results (MchCod INT NOT NULL,UsrCod INT NOT NULL,StartTime DATETIME NOT NULL,EndTime DATETIME NOT NULL,NumQsts INT NOT NULL DEFAULT 0,NumQstsNotBlank INT NOT NULL DEFAULT 0,Score DOUBLE PRECISION NOT NULL DEFAULT 0,UNIQUE INDEX(MchCod,UsrCod));

CREATE TABLE IF NOT EXISTS mch_indexes (MchCod INT NOT NULL,QstInd INT NOT NULL,Indexes TEXT NOT NULL,UNIQUE INDEX(MchCod,QstInd));

ALTER TABLE mch_answers ADD COLUMN NumOpt TINYINT NOT NULL AFTER QstInd;

ALTER TABLE mch_matches ADD COLUMN VisibleResult ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Title;

ALTER TABLE mch_matches DROP COLUMN VisibleResult,DROP COLUMN ShowResults;
ALTER TABLE mch_matches ADD COLUMN ShowQstResults ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER Showing;
ALTER TABLE mch_matches ADD COLUMN ShowUsrResults ENUM('N','Y') NOT NULL DEFAULT 'N' AFTER ShowQstResults;
INSERT INTO actions (ActCod,Language,Obsolete,Txt) VALUES ('1801','es','N','Cambiar visibilidad de resultados de una partida');
UPDATE actions SET Obsolete='Y' WHERE ActCod=1786;

ALTER TABLE hidden_params ENGINE=MyISAM;
ALTER TABLE mch_indexes ENGINE=MyISAM;
ALTER TABLE mch_results ENGINE=MyISAM;
ALTER TABLE mch_times ENGINE=MyISAM;
ALTER TABLE timetable_tut ENGINE=MyISAM;
SELECT TABLE_NAME,ENGINE FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'swad';

UPDATE actions SET Txt='Solicitar eliminaci&oacute;n partida' WHERE ActCod='1783' AND Language='es';
UPDATE actions SET Txt='Eliminar partida' WHERE ActCod='1784' AND Language='es';
UPDATE actions SET Txt='Solicitar nueva partida' WHERE ActCod='1670' AND Language='es';
UPDATE actions SET Txt='Crear nueva partida' WHERE ActCod='1671' AND Language='es';
UPDATE actions SET Txt='Reanudar partida' WHERE ActCod='1785' AND Language='es';
UPDATE actions SET Txt='Ir hacia atr&aacute;s en partida' WHERE ActCod='1790' AND Language='es';
UPDATE actions SET Txt='Jugar partida' WHERE ActCod='1789' AND Language='es';
UPDATE actions SET Txt='Pausar partida' WHERE ActCod='1791' AND Language='es';
UPDATE actions SET Txt='Ir hacia delante en partida' WHERE ActCod='1672' AND Language='es';
UPDATE actions SET Txt='Cambiar presentaci&oacute;n de resultados de pregunta en partida' WHERE ActCod='1794' AND Language='es';
UPDATE actions SET Txt='Refrescar partida (como profesor)' WHERE ActCod='1788' AND Language='es';
UPDATE actions SET Txt='Unirse a partida' WHERE ActCod='1780' AND Language='es';
UPDATE actions SET Txt='Responder pregunta en partida' WHERE ActCod='1651' AND Language='es';
UPDATE actions SET Txt='Refrescar partida (como estudiante)' WHERE ActCod='1782' AND Language='es';


// Borrado opcional:

DELETE FROM gam_games;
DELETE FROM gam_questions;

DELETE FROM mch_answers;
DELETE FROM mch_groups;
DELETE FROM mch_indexes;
DELETE FROM mch_matches;
DELETE FROM mch_players;
DELETE FROM mch_playing;
DELETE FROM mch_results;
DELETE FROM mch_times;








----------------------





SELECT projects.PrjCod,COUNT(prj_usr.UsrCod) AS NumStds FROM projects LEFT JOIN prj_usr ON projects.PrjCod=prj_usr.UsrCod WHERE projects.CrsCod=19 AND prj_usr.RoleInProject=3 GROUP BY projects.PrjCod ORDER BY projects.Title;









SELECT COUNT(*) FROM tst_answers,gam_questions WHERE gam_questions.GamCod=6 AND gam_questions.QstCod=tst_answers.QstCod;


SELECT MIN(QstInd) FROM gam_questions WHERE GamCod=47 AND QstInd>5;



 
 
 
 
SELECT NotifyEvent,FromUsrCod,InsCod,CtrCod,DegCod,CrsCod,Cod,UNIX_TIMESTAMP(TimeNotif),Status FROM notif WHERE ToUsrCod=1346 ORDER BY TimeNotif DESC;


SELECT AVG(Latitude),AVG(Longitude),MAX(MAX(Latitude)-MIN(Latitude),MAX(Longitude)-MIN(Longitude)) FROM centres WHERE InsCod=1 AND Latitude<>0 AND Longitude<>0;



SELECT COUNT(*) FROM centres WHERE InsCod=1 AND Latitude<>0 AND Longitude<>0;
SELECT EXISTS (SELECT * FROM centres WHERE InsCod=1 AND Latitude<>0 AND Longitude<>0);



-----------------------

Restaurar una tabla desde la copia de seguridad:

awk '/Table structure for table `log_full`/,/Table structure for table `log_recent`/{print}' swad_backup_20200322.sql > swad_backup_20200322_log_full.sql
                                     ^                                          ^
                                tabla a buscar                           tabla siguiente

En el nuevo fichero sql cambiar crs_grp_usr por crs_grp_usr_borrame

sed -i 's/log_full/log_full_new/g' swad_backup_20200322_log_full.sql

mysql -u swad -p swad < swad_backup_20200322_log_full.sql

Importante para que REPAIR TABLE vaya r�pido:
https://dev.mysql.com/doc/refman/5.7/en/repair-table-optimization.html

SET SESSION myisam_sort_buffer_size = 256*1024*1024;
SET SESSION read_buffer_size = 64*1024*1024;
SET GLOBAL myisam_max_sort_file_size = 100*1024*1024*1024;
SET GLOBAL repair_cache.key_buffer_size = 128*1024*1024;
CACHE INDEX log_full_new IN repair_cache;
LOAD INDEX INTO CACHE log_full_new;
REPAIR TABLE log_full_new;

RENAME TABLE log_full TO log_full_old;
RENAME TABLE log_full_new TO log_full;

-------------------------------







SELECT LogCod,ActCod,UNIX_TIMESTAMP()-UNIX_TIMESTAMP(ClickTime),Role,CtyCod,InsCod,CtrCod,DegCod FROM log_recent ORDER BY LogCod DESC LIMIT 20;








-----------------


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
    
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) < 2005; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2005; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2006; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2007; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2008; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2009; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2010; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2011; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2012; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2013; Hecho

INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140101' AND ClickTime<'20140201'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140201' AND ClickTime<'20140301'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140301' AND ClickTime<'20140401'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140401' AND ClickTime<'20140501'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140501' AND ClickTime<'20140601'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140601' AND ClickTime<'20140701'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140701' AND ClickTime<'20140801'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140801' AND ClickTime<'20140901'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20140901' AND ClickTime<'20141001'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20141001' AND ClickTime<'20141101'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20141101' AND ClickTime<'20141201'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20141201' AND ClickTime<'20150101'; Hecho

INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150101' AND ClickTime<'20150201'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150201' AND ClickTime<'20150301'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150301' AND ClickTime<'20150401'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150401' AND ClickTime<'20150501'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150501' AND ClickTime<'20150601'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150601' AND ClickTime<'20150701'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150701' AND ClickTime<'20150801'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150801' AND ClickTime<'20150901'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20150901' AND ClickTime<'20151001'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20151001' AND ClickTime<'20151101'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20151101' AND ClickTime<'20151201'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20151201' AND ClickTime<'20160101'; Hecho

INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160101' AND ClickTime<'20160201'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160201' AND ClickTime<'20160301'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160301' AND ClickTime<'20160401'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160401' AND ClickTime<'20160501'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160501' AND ClickTime<'20160601'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160601' AND ClickTime<'20160701'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160701' AND ClickTime<'20160801'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160801' AND ClickTime<'20160901'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20160901' AND ClickTime<'20161001'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20161001' AND ClickTime<'20161101'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20161101' AND ClickTime<'20161201'; Hecho
INSERT INTO log SELECT * FROM log_full WHERE ClickTime>='20161201' AND ClickTime<'20170101'; Hecho

INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2017; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2018; Hecho
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2019; Hecho
# hasta aqu� ya hecho

# Actualizar los �ltimos antes de dar el cambiazo:
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) = 2020; 
INSERT INTO log SELECT * FROM log_full WHERE YEAR(ClickTime) > 2020;

Si se quieren eliminar todos los datos de una partici�n:
ALTER TABLE log TRUNCATE PARTITION p2014;



------------------------------------


// Cambios realizados el 14 de abril de 2020
// Ahora la pareja ExaCod,QstCod deben ser �nica en la tabla tst_exam_questions.
// Detect� los siguientes ex�menes que ten�an preguntas repetidas, y los elimin� de la tabla tst_exam_questions pero no de tst_exams;

SELECT DISTINCT (ExaCod) FROM (SELECT ExaCod,COUNT(*) AS N FROM tst_exam_questions GROUP BY ExaCod,QstCod HAVING N>1) AS E;

SELECT * FROM tst_exam_questions WHERE ExaCod IN (138946,149939,153732,155072,158845,160028,163616,163617,165036,176633,177040,177140,177221,180469,182532,189061,189169,189179,189207,189245,189273,200335,200336,200337,203975,205562,206021,207224,215085,218272,218294,218300,218317,218665,218851,218869,218880,218955,219045,219074,219082,219086,219089,219092,219095,219097,219104,219107,219189,252548,252549,252550,259574,259825);
 
DELETE FROM tst_exam_questions WHERE ExaCod IN (138946,149939,153732,155072,158845,160028,163616,163617,165036,176633,177040,177140,177221,180469,182532,189061,189169,189179,189207,189245,189273,200335,200336,200337,203975,205562,206021,207224,215085,218272,218294,218300,218317,218665,218851,218869,218880,218955,219045,219074,219082,219086,219089,219092,219095,219097,219104,219107,219189,252548,252549,252550,259574,259825);

SELECT ExaCod,CrsCod FROM tst_exams WHERE ExaCod IN (138946,149939,153732,155072,158845,160028,163616,163617,165036,176633,177040,177140,177221,180469,182532,189061,189169,189179,189207,189245,189273,200335,200336,200337,203975,205562,206021,207224,215085,218272,218294,218300,218317,218665,218851,218869,218880,218955,219045,219074,219082,219086,219089,219092,219095,219097,219104,219107,219189,252548,252549,252550,259574,259825);

SELECT * FROM tst_exams WHERE ExaCod IN (138946,149939,153732,155072,158845,160028,163616,163617,165036,176633,177040,177140,177221,180469,182532,189061,189169,189179,189207,189245,189273,200335,200336,200337,203975,205562,206021,207224,215085,218272,218294,218300,218317,218665,218851,218869,218880,218955,219045,219074,219082,219086,219089,219092,219095,219097,219104,219107,219189,252548,252549,252550,259574,259825);

-----------------------------------------


SELECT my_courses.CrsCod,COUNT(*) AS N FROM (SELECT UsrCod,CrsCod,Role FROM crs_usr WHERE UsrCod=1346 AND Role=5) AS my_courses LEFT JOIN log ON (my_courses.UsrCod=log.UsrCod AND my_courses.CrsCod=log.CrsCod AND my_courses.Role=log.Role) GROUP BY my_courses.CrsCod ORDER BY N DESC,my_courses.CrsCod DESC;

SELECT my_courses.CrsCod,COUNT(*) AS N FROM (SELECT CrsCod,UsrCod,Role FROM crs_usr WHERE UsrCod=1346 AND Role=5) AS my_courses LEFT JOIN log ON (my_courses.CrsCod=log.CrsCod AND my_courses.UsrCod=log.UsrCod AND my_courses.Role=log.Role) GROUP BY my_courses.CrsCod ORDER BY N DESC,my_courses.CrsCod DESC;
 
SELECT my_courses.CrsCod,COUNT(*) AS N FROM (SELECT CrsCod FROM crs_usr WHERE UsrCod=1346 AND Role=5) AS my_courses LEFT JOIN log ON (my_courses.CrsCod=log.CrsCod) WHERE log.UsrCod=1346 AND log.Role=5 GROUP BY my_courses.CrsCod ORDER BY N DESC,my_courses.CrsCod DESC;
 
 
 
--------------------

SELECT COUNT(*) FROM usr_data WHERE InsCod=25037;
UPDATE usr_data SET InsCod=1130 WHERE InsCod=25037;


ALTER TABLE rooms CHANGE COLUMN BldCod BldCod INT NOT NULL DEFAULT -1;
 
 
--------------------

INSERT INTO room_MAC (RooCod,MAC) VALUES (1,0xf07f0667d5ff);
INSERT INTO room_MAC (RooCod,MAC) VALUES (2,0x6886a731695f);
INSERT INTO room_MAC (RooCod,MAC) VALUES (108,0xccd53989961f);
 
mysql> SELECT RooCod,HEX(MAC) FROM room_MAC;
+--------+--------------+
| RooCod | HEX(MAC)     |
+--------+--------------+
|      1 | F07F0667D5FF |
|      2 | 6886A731695F |
|    108 | CCD53989961F |
+--------+--------------+
3 rows in set (0.00 sec)


SELECT institutions.InsCod,institutions.ShortName,institutions.FullName,centres.CtrCod,centres.ShortName,centres.FullName,buildings.BldCod,buildings.ShortName,buildings.FullName,rooms.Floor,rooms.RooCod,rooms.ShortName,rooms.FullName FROM room_MAC,rooms,buildings,centres,institutions WHERE room_MAC.MAC=0xCCD53989961F AND room_MAC.RooCod=rooms.RooCod AND rooms.BldCod=buildings.BldCod AND buildings.CtrCod=centres.CtrCod AND centres.InsCod=institutions.InsCod ORDER BY institutions.FullName,centres.FullName,buildings.FullName,rooms.Floor,rooms.FullName;




-------------------------------

SELECT * FROM
(SELECT PrnCod,Score AS S FROM exa_prints ORDER BY PrnCod) AS ep
LEFT JOIN
(SELECT PrnCod,SUM(Score) AS S FROM exa_print_questions GROUP BY PrnCod ORDER BY PrnCod) AS epq
ON ep.PrnCod=epq.PrnCod
WHERE ABS(ep.S-epq.S)<0.001;

----------------------------------


SELECT COUNT(*) FROM exa_log_user_agent WHERE LogCod=(SELECT MAX(LogCod) FROM exa_log_user_agent WHERE PrnCod=10) AND UserAgent='Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0';



----------------------------------


INSERT INTO room_check_in (UsrCod,RooCod,CheckInTime) SELECT 1,RooCod,NOW() FROM rooms WHERE RooCod=1;



----------------------------------------


INSERT INTO room_check_in (UsrCod,RooCod,CheckInTime) VALUES (1,1,NOW());

SELECT institutions.InsCod,institutions.ShortName,institutions.FullName,centres.CtrCod,centres.ShortName,centres.FullName,buildings.BldCod,buildings.ShortName,buildings.FullName,rooms.Floor,rooms.RooCod,rooms.ShortName,rooms.FullName,UNIX_TIMESTAMP(room_check_in.CheckInTime) FROM room_check_in,rooms,buildings,centres,institutions WHERE room_check_in.UsrCod=1 AND room_check_in.CheckInTime=(SELECT MAX(CheckInTime) FROM room_check_in.UsrCod=1) AND room_check_in.RooCod=rooms.RooCod AND rooms.BldCod=buildings.BldCod AND buildings.CtrCod=centres.CtrCod AND centres.InsCod=institutions.InsCod;

SELECT COUNT(*) FROM (SELECT DISTINCT degrees.CtrCod FROM crs_usr,courses,degrees WHERE crs_usr.UsrCod=1346 AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod) AS centres1,(SELECT DISTINCT degrees.CtrCod FROM crs_usr,courses,degrees WHERE crs_usr.UsrCod=1331 AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod) AS centres2 WHERE centres1.CtrCod=centres2.CtrCod;


----------------------------------------



# Cambios de la versi�n 20.30 a la 20.66.2:

DROP TABLE IF EXISTS crs_usr_backup_delete_me;
DROP TABLE IF EXISTS crs_usr_old_backup_delete_me;
DROP TABLE IF EXISTS log_full;
RENAME TABLE tl_comments TO tml_comments;
RENAME TABLE tl_comments_fav TO tml_comments_fav;
RENAME TABLE tl_notes TO tml_notes;
RENAME TABLE tl_notes_fav TO tml_notes_fav;
RENAME TABLE tl_posts TO tml_posts;
RENAME TABLE tl_pubs TO tml_pubs;
RENAME TABLE tl_timelines TO tml_timelines;
RENAME TABLE surveys TO svy_surveys;
RENAME TABLE admin TO usr_admins;
RENAME TABLE degrees TO deg_degrees;
RENAME TABLE courses TO crs_courses;
RENAME TABLE centres TO ctr_centers;
RENAME TABLE institutions TO ins_instits;
RENAME TABLE countries TO cty_countrs;
RENAME TABLE actions TO act_actions;
RENAME TABLE actions_MFU TO act_MFU;
DROP TABLE IF EXISTS act_actions;
RENAME TABLE agendas TO agd_agendas;
RENAME TABLE announcements TO ann_announcements;
RENAME TABLE assignments TO asg_assignments;
RENAME TABLE banners TO ban_banners;
RENAME TABLE birthdays_today TO usr_birthdays_today;
RENAME TABLE buildings TO bld_buildings;
RENAME TABLE chat TO cht_rooms;
RENAME TABLE clicks_without_photo TO pho_clicks_without_photo;
RENAME TABLE clipboard TO brw_clipboards;
RENAME TABLE connected TO usr_connected;
RENAME TABLE departments TO dpt_departments;
RENAME TABLE exam_announcements TO cfe_calls_for_exams;
RENAME TABLE expanded_folders TO brw_expanded_folders;
RENAME TABLE figures TO fig_figures;
RENAME TABLE file_browser_last TO brw_last;
RENAME TABLE file_browser_size TO brw_sizes;
RENAME TABLE files TO brw_files;
RENAME TABLE file_cache TO brw_file_caches;
RENAME TABLE file_view TO brw_file_views;
RENAME TABLE firewall_banned TO fir_banned;
RENAME TABLE firewall_log TO fir_log;
RENAME TABLE forum_disabled_post TO for_disabled_posts;
RENAME TABLE forum_post TO for_posts;
RENAME TABLE forum_thread TO for_threads;
RENAME TABLE forum_thr_clip TO for_clipboards;
RENAME TABLE forum_thr_read TO for_read;
RENAME TABLE for_disabled_posts TO for_disabled;
RENAME TABLE hidden_params TO ses_params;
RENAME TABLE sessions TO ses_sessions;
RENAME TABLE holidays TO hld_holidays;
RENAME TABLE notif TO ntf_notifications;
RENAME TABLE mail_domains TO ntf_mail_domains;
RENAME TABLE projects TO prj_projects;
RENAME TABLE media TO med_media;
RENAME TABLE rooms TO roo_rooms;
RENAME TABLE room_MAC TO roo_MACs;
RENAME TABLE room_check_in TO roo_check_in;
RENAME TABLE notices TO not_notices;
RENAME TABLE notices_deleted TO not_deleted;
RENAME TABLE links TO lnk_links;
RENAME TABLE pending_emails TO usr_pending_emails;
RENAME TABLE pending_passwd TO usr_pending_passwd;
RENAME TABLE timetable_crs TO tmt_courses;
RENAME TABLE timetable_tut TO tmt_tutoring;
RENAME TABLE places TO plc_places;
RENAME TABLE plugins TO plg_plugins;
RENAME TABLE marks_properties TO mrk_marks;
RENAME TABLE ws_keys TO API_keys;
RENAME TABLE debug TO dbg_debug;
RENAME TABLE asg_grp TO asg_groups;
RENAME TABLE att_grp TO att_groups;
RENAME TABLE svy_grp TO svy_groups;
RENAME TABLE att_usr TO att_users;
RENAME TABLE crs_usr TO crs_users;
DROP TABLE IF EXISTS crs_usr_copy;
RENAME TABLE crs_grp TO grp_groups;
RENAME TABLE crs_grp_types TO grp_types;
RENAME TABLE crs_grp_usr TO grp_users;
RENAME TABLE brw_expanded_folders TO brw_expanded;
RENAME TABLE brw_file_caches TO brw_caches;
RENAME TABLE brw_file_views TO brw_views;
RENAME TABLE IP_prefs TO set_ip_settings;
RENAME TABLE API_keys TO api_keys;
RENAME TABLE cfe_calls_for_exams TO cfe_exams;
RENAME TABLE crs_usr_requests TO crs_requests;
RENAME TABLE crs_usr_last TO crs_user_settings;
RENAME TABLE exa_log_session TO exa_log_sessions;
RENAME TABLE exa_log_user_agent TO exa_log_user_agents;
RENAME TABLE log_ws TO log_api;
RENAME TABLE prj_usr TO prj_users;
RENAME TABLE sta_notif TO sta_notifications;
RENAME TABLE usr_report TO usr_reports;
RENAME TABLE act_MFU TO act_frequent;
RENAME TABLE roo_MACs TO roo_macs;
RENAME TABLE usr_IDs TO usr_ids;
RENAME TABLE pho_clicks_without_photo TO usr_clicks_without_photo;

---------------------------


SELECT gam_questions.QstCod,gam_questions.QstInd,tst_questions.AnsType,tst_questions.Shuffle FROM gam_questions,tst_questions WHERE gam_questions.GamCod=7 AND gam_questions.QstCod=tst_questions.QstCod ORDER BY gam_questions.QstInd;

---------------------------

SELECT COUNT(tst_answers.AnsInd) AS N FROM tst_answers,gam_questions WHERE gam_questions.GamCod=8 AND gam_questions.QstCod=tst_answers.QstCod GROUP BY tst_answers.QstCod;

---------------------------

SELECT MAX(PubCod) AS NewestPubCod
  FROM tml_pubs
 GROUP BY NotCod
 ORDER BY NewestPubCod DESC
 LIMIT 10;
 
SELECT PubCod
FROM tml_pubs
WHERE NotCod NOT IN (SELECT NotCod FROM tml_tmp_just_retrieved_notes)
ORDER BY PubCod DESC
LIMIT 1;


CREATE TEMPORARY TABLE tml_tmp_just_retrieved_notes (NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod)) ENGINE=MEMORY;
CREATE TEMPORARY TABLE tml_tmp_visible_timeline (NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod)) ENGINE=MEMORY SELECT NotCod FROM tml_timelines WHERE SessionId='%s',Gbl.Session.Id);	      
		   
		   
CREATE TEMPORARY TABLE fol_tmp_me_and_followed (UsrCod INT NOT NULL,UNIQUE INDEX(UsrCod)) ENGINE=MEMORY SELECT 1 AS UsrCod UNION SELECT FollowedCod AS UsrCod FROM usr_follow WHERE FollowerCod=1;

CREATE TEMPORARY TABLE fol_tmp_me_and_followed_2 (UsrCod INT NOT NULL,UNIQUE INDEX(UsrCod)) ENGINE=MEMORY SELECT 1 UNION SELECT FollowedCod FROM usr_follow WHERE FollowerCod=1;


SELECT tml_pubs.PubCod,
       tml_pubs.NotCod,
       tml_pubs.PublisherCod,
       tml_pubs.PubType
  FROM tml_pubs,
       fol_tmp_me_and_followed
 WHERE tml_pubs.PublisherCod=fol_tmp_me_and_followed.UsrCod
   AND tml_pubs.NotCod NOT IN
       (SELECT NotCod
          FROM tml_tmp_just_retrieved_notes)
 ORDER BY PubCod DESC
 LIMIT 1;
 /*
 		   "SELECT tml_pubs.PubCod,"		// row[0]
			  "tml_pubs.NotCod,"		// row[1]
			  "tml_pubs.PublisherCod,"	// row[2]
			  "tml_pubs.PubType"		// row[3]
		    " FROM tml_pubs%s"
		   " WHERE tml_pubs.PublisherCod=fol_tmp_me_and_followed.UsrCod"
		       AND tml_pubs.PubCod>Bottom AND "
		       AND tml_pubs.PubCod<Top AND "
		   " ORDER BY tml_pubs.PubCod DESC"
		   " LIMIT 1",
		   SubQueries->Publishers.Table,
		   SubQueries->RangeBottom,
		   SubQueries->RangeTop,
		   SubQueries->Publishers.SubQuery,
		   SubQueries->AlreadyExists);
*/

SELECT MAX(PstCod) AS NewestPstCod FROM for_posts GROUP BY ThrCod ORDER BY NewestPstCod DESC LIMIT 10;
 
SELECT PstCod FROM for_posts ORDER BY PstCod DESC LIMIT 1;


-------------------------------------------------------------------------------
*******************************************************************************

2021-11-25: Error en juegos

mch_results.Score se ha guardado err�neamente en los juegos realizados aproximadamente entre estas fechas:
mch_result.StartTime>=20211124210000 AND mch_result.StartTime<=20211125160000

S�lo los que han contestado alguna pregunta:
mch_result.NumQstsNotBlank>0

La funci�n:
void MchPrn_ComputeScoreAndUpdateMyMatchPrintInDB (long MchCod)
calcula Score y lo guarda en la base de datos.
Llama a:
   Mch_GetMatchQuestionsFromDB (&Print);
   MchPrn_ComputeScore (&Print);
      TstPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],&Question);
         TstPrn_GetCorrectAndComputeChoAnsScore (struct Qst_PrintedQuestion *PrintedQuestion,
				                 struct Qst_Question *Question);
	    Qst_GetCorrectChoAnswerFromDB (Question);		// <----- Aqu� estaba el fallo
	          SELECT Correct
		    FROM tst_answers
		   WHERE QstCod=%ld	// Question code
		   ORDER BY AnsInd;
            TstPrn_ComputeChoAnsScore (PrintedQuestion,Question);
      Print->Score += Print->PrintedQuestions[NumQst].Score;
   MchPrn_UpdateMatchPrintInDB (&Print);




ALTER TABLE set_ip_settings ADD COLUMN PhotoShape TINYINT NOT NULL DEFAULT 0 AFTER SideCols;
UPDATE set_ip_settings SET PhotoShape=3;
ALTER TABLE usr_data ADD COLUMN PhotoShape TINYINT NOT NULL DEFAULT 0 AFTER SideCols,ADD INDEX (PhotoShape);
UPDATE usr_data SET PhotoShape=3;


-----------------

INSERT INTO prg_resources (ItmCod,RscInd,Hidden,Title) VALUES (14,0,'N','Recurso 1');
INSERT INTO prg_resources (ItmCod,RscInd,Hidden,Title) VALUES (14,1,'N','Recurso 2');
INSERT INTO prg_resources (ItmCod,RscInd,Hidden,Title) VALUES (14,2,'N','Recurso 3');


-----------------

DELETE FROM usr_clipboards WHERE UsrCod IN (SELECT UsrCod FROM (SELECT UsrCod,MIN(CopyTime) AS OldestCopyTime FROM usr_clipboards GROUP BY UsrCod HAVING OldestCopyTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-1500)) AS usr_users_with_old_clipboards);


DELETE LOW_PRIORITY FROM usr_clipboards WHERE UsrCod IN (SELECT UsrCod FROM (SELECT DISTINCT UsrCod FROM usr_clipboards WHERE CopyTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-15)) AS usr_old_clipboards);


-----------------



2024-12-03:



RENAME TABLE prg_expanded TO tre_expanded;
ALTER TABLE tre_expanded DROP INDEX UsrCod;
ALTER TABLE tre_expanded DROP INDEX ItmCod;
ALTER TABLE tre_expanded DROP INDEX ClickTime;
ALTER TABLE tre_expanded CHANGE COLUMN ItmCod NodCod INT NOT NULL;
ALTER TABLE tre_expanded ADD UNIQUE INDEX(UsrCod,NodCod);
ALTER TABLE tre_expanded ADD INDEX(NodCod);
ALTER TABLE tre_expanded ADD INDEX(ClickTime);

RENAME TABLE prg_items TO tre_nodes;
ALTER TABLE tre_nodes DROP INDEX CrsCod;
ALTER TABLE tre_nodes CHANGE COLUMN ItmCod NodCod INT NOT NULL AUTO_INCREMENT;
ALTER TABLE tre_nodes CHANGE COLUMN ItmInd NodInd INT NOT NULL DEFAULT 0;
ALTER TABLE tre_nodes ADD UNIQUE INDEX(NodCod);
ALTER TABLE tre_nodes ADD UNIQUE INDEX(CrsCod,NodInd);
ALTER TABLE tre_nodes DROP INDEX ItmCod;
ALTER TABLE tre_nodes DROP INDEX PrgIteCod;

ALTER TABLE prg_resources DROP INDEX ItmCod;
ALTER TABLE prg_resources CHANGE COLUMN ItmCod NodCod INT NOT NULL DEFAULT -1;
ALTER TABLE prg_resources ADD UNIQUE INDEX(NodCod,RscInd);

ALTER TABLE tre_nodes ADD COLUMN Type ENUM('prg','gui','lec','pra','bib','faq','lnk','ass') NOT NULL DEFAULT 'prg' AFTER CrsCod;
ALTER TABLE tre_nodes DROP INDEX CrsCod;
ALTER TABLE tre_nodes ADD UNIQUE INDEX(CrsCod,Type,NodInd);
UPDATE tre_nodes SET Type='prg' WHERE Type NOT IN ('gui','lec','pra','bib','faq','lnk','ass');
ALTER TABLE tre_nodes CHANGE COLUMN StartTime StartTime DATETIME NOT NULL DEFAULT '1970-01-01 01:00:00';
ALTER TABLE tre_nodes CHANGE COLUMN EndTime EndTime DATETIME NOT NULL DEFAULT '1970-01-01 01:00:00';
ALTER TABLE tre_nodes CHANGE COLUMN Type Type ENUM('unk','inf','prg','gui','lec','pra','bib','faq','lnk','ass') NOT NULL DEFAULT 'unk';

CREATE TABLE IF NOT EXISTS faq_questions (QaACod INT NOT NULL AUTO_INCREMENT,NodCod INT NOT NULL DEFAULT -1,QaAInd INT NOT NULL DEFAULT 0,Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',Question VARCHAR(4095) NOT NULL,Answer TEXT NOT NULL,UNIQUE INDEX(QaACod),UNIQUE INDEX(NodCod,QaAInd));
ALTER TABLE faq_questions ENGINE=MyISAM;
ALTER TABLE faq_questions CHANGE COLUMN Question Question VARCHAR(2047) NOT NULL;

CREATE TABLE IF NOT EXISTS crs_links (LnkCod INT NOT NULL AUTO_INCREMENT,NodCod INT NOT NULL DEFAULT -1,LnkInd INT NOT NULL DEFAULT 0,Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',Title VARCHAR(2047) NOT NULL,Description VARCHAR(2047) NOT NULL,WWW VARCHAR(255) NOT NULL,UNIQUE INDEX(LnkCod),UNIQUE INDEX(NodCod,LnkInd));
ALTER TABLE crs_links ENGINE=MyISAM;

CREATE TABLE IF NOT EXISTS crs_bibliography (BibCod INT NOT NULL AUTO_INCREMENT,NodCod INT NOT NULL DEFAULT -1,BibInd INT NOT NULL DEFAULT 0,Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',Authors VARCHAR(2047) NOT NULL,Title VARCHAR(2047) NOT NULL,Source VARCHAR(2047) NOT NULL,Publisher VARCHAR(2047) NOT NULL,Date VARCHAR(2047) NOT NULL,Id VARCHAR(2047) NOT NULL,URL VARCHAR(255) NOT NULL,UNIQUE INDEX(BibCod),UNIQUE INDEX(NodCod,BibInd));
ALTER TABLE crs_bibliography ENGINE=MyISAM;

ALTER TABLE prg_resources CHANGE COLUMN RscInd ItmInd INT NOT NULL DEFAULT 0;
ALTER TABLE crs_bibliography CHANGE COLUMN BibInd ItmInd INT NOT NULL DEFAULT 0;
ALTER TABLE faq_questions CHANGE COLUMN QaAInd ItmInd INT NOT NULL DEFAULT 0;
ALTER TABLE crs_links CHANGE COLUMN LnkInd ItmInd INT NOT NULL DEFAULT 0;

ALTER TABLE prg_resources CHANGE COLUMN RscCod ItmCod INT NOT NULL DEFAULT 0;
ALTER TABLE crs_bibliography CHANGE COLUMN BibCod ItmCod INT NOT NULL DEFAULT 0;
ALTER TABLE faq_questions CHANGE COLUMN QaACod ItmCod INT NOT NULL DEFAULT 0;
ALTER TABLE crs_links CHANGE COLUMN LnkCod ItmCod INT NOT NULL DEFAULT 0;









































 
