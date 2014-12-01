-- MySQL dump 10.11
--
-- Host: localhost    Database: swad
-- ------------------------------------------------------
-- Server version	5.0.95-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `deg_types`
--

DROP TABLE IF EXISTS `deg_types`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `deg_types` (
  `DegTypCod` int(11) NOT NULL auto_increment,
  `DegTypName` varchar(32) NOT NULL,
  `AllowDirectLogIn` enum('N','Y') NOT NULL default 'Y',
  UNIQUE KEY `DegTypCod` (`DegTypCod`)
) ENGINE=MyISAM AUTO_INCREMENT=48 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `deg_types`
--

LOCK TABLES `deg_types` WRITE;
/*!40000 ALTER TABLE `deg_types` DISABLE KEYS */;
INSERT INTO `deg_types` VALUES (1,'Diplomaturas (UGR)','Y'),(2,'Doctorados','Y'),(3,'Ingenier./Arquitec. (UGR)','Y'),(4,'Investigación','Y'),(5,'Licenciaturas (UGR)','Y'),(6,'Magisterio (UGR)','Y'),(8,'Proyectos','Y'),(9,'Otros','Y'),(17,'Expertos','Y'),(20,'Practicum Edu.','Y'),(21,'Grados (UGR)','Y'),(22,'Centros','Y'),(23,'Cursos 0','Y'),(24,'Proy. Inno. Docente','Y'),(25,'Pruebas','Y'),(29,'Diplomaturas (otros)','Y'),(30,'Magisterio (otros)','Y'),(32,'Másteres Propios','Y'),(33,'Másteres Oficiales (UGR)','Y'),(34,'Másteres Interuniv.','Y'),(35,'Grados (otros)','Y'),(47,'Másteres (UCM)','Y'),(40,'Otros (UAL)','Y'),(36,'Grados (TEOL)','Y'),(37,'Licenciaturas (TEOL)','Y'),(38,'Diplomaturas (TEOL)','Y'),(39,'Másteres (TEOL)','Y'),(41,'Diplomaturas (UAL)','Y'),(42,'Licenciaturas (UAL)','Y'),(43,'Másteres (UAL)','Y'),(44,'Maestrías (UAGRM)','Y'),(45,'Diplomaturas (EHU)','Y'),(46,'Research (liv.ac.uk)','Y');
/*!40000 ALTER TABLE `deg_types` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-07-18 19:54:15
