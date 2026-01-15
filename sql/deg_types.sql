-- MySQL dump 10.13  Distrib 5.1.73, for redhat-linux-gnu (x86_64)
--
-- Host: localhost    Database: swad
-- ------------------------------------------------------
-- Server version	5.1.73-log

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
  `DegTypCod` int(11) NOT NULL AUTO_INCREMENT,
  `DegTypName` varchar(511) NOT NULL,
  UNIQUE KEY `DegTypCod` (`DegTypCod`)
) ENGINE=MyISAM AUTO_INCREMENT=56 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `deg_types`
--

LOCK TABLES `deg_types` WRITE;
/*!40000 ALTER TABLE `deg_types` DISABLE KEYS */;
INSERT INTO `deg_types` VALUES (1,'Diplomatura'),(2,'Doctorate / Doctorado'),(3,'Ingeniería / Arquitectura'),(4,'Research / Investigación'),(5,'Licenciatura'),(6,'Magisterio / Profesorado'),(8,'Project / Proyecto'),(9,'Other / Otro'),(17,'Expert / Experto'),(20,'Practicum Educación'),(21,'Degree / Grado'),(22,'Centre / Centro'),(23,'Course 0 / Curso 0'),(33,'Master / Máster / Maestría'),(48,'Course / Curso'),(49,'Educación Primaria'),(50,'Educación Secundaria'),(51,'Posgrado'),(52,'Pregrado'),(53,'Técnico Universitario'),(54,'Formación Profesional'),(55,'-');
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

-- Dump completed on 2026-01-13 12:54:10
