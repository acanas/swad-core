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
-- Table structure for table `mail_domains`
--

DROP TABLE IF EXISTS `mail_domains`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mail_domains` (
  `MaiCod` int(11) NOT NULL auto_increment,
  `Domain` varchar(127) NOT NULL,
  `Info` varchar(127) NOT NULL,
  UNIQUE KEY `MaiCod` (`MaiCod`),
  UNIQUE KEY `Domain` (`Domain`),
  KEY `Info` (`Info`)
) ENGINE=MyISAM AUTO_INCREMENT=117 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mail_domains`
--

LOCK TABLES `mail_domains` WRITE;
/*!40000 ALTER TABLE `mail_domains` DISABLE KEYS */;
INSERT INTO `mail_domains` VALUES (1,'ugr.es','Universidad de Granada: PDI y PAS'),(2,'correo.ugr.es','Universidad de Granada: alumnos'),(3,'juntadeandalucia.es','Junta de Andalucía: correo corporativo'),(4,'atc.ugr.es','Universidad de Granada: Dpto. Arquitectura y Tecnología de Computadores'),(5,'decsai.ugr.es','Universidad de Granada: Dpto. Ciencias de la Computación e Inteligencia Artificial'),(6,'usc.es','Universidade de Santiago de Compostela'),(7,'dipgra.es','Diputación de Granada'),(8,'alumni.uv.es','Universitat de València: alumnos'),(9,'eulainmaculada.com','Escuela Universitaria Diocesana de Magisterio La Inmaculada'),(10,'estudiants.urv.cat','Universitat Rovira i Virgili: estudiantes'),(11,'ual.es','Universidad de Almería'),(12,'unizar.es','Universidad de Zaragoza'),(13,'uma.es','Universidad de Málaga'),(14,'ujaen.es','Universidad de Jaén'),(15,'ehu.es','Euskal Herriko Unibertsitateko'),(16,'udelmar.cl','Universidad del Mar (Chile)'),(17,'student.kuleuven.be','Katholieke Universiteit Leuven (Belgique): studenten'),(18,'unican.es','Universidad de Cantabria'),(19,'us.es','Universidad de Sevilla'),(20,'bucknell.edu','Bucknell University (USA)'),(21,'ulb.ac.be','Université libre de Bruxelles (Belgique)'),(22,'studenti.unitn.it','Università degli Studi di Trento (Italia): studenti'),(23,'uni-potsdam.de','Universität Potsdam (Deutschland)'),(24,'uco.es','Universidad de Córdoba'),(25,'le.ac.uk','University of Leicester (UK)'),(26,'sms.ed.ac.uk','University of Edinburgh (UK): students'),(27,'umail.ucsb.edu','University of California, Santa Barbara (USA)'),(28,'bham.ac.uk','University of Birmingham (UK)'),(29,'insa-rennes.fr','Institut National des Sciences Appliquées de Rennes (France)'),(30,'urv.cat','Universitat Rovira i Virgili'),(31,'uibk.ac.at','Universität Innsbruck (Österreich)'),(32,'uam.es','Universidad Autónoma de Madrid'),(33,'ucsd.edu','University of California, San Diego (USA)'),(34,'reduc.edu.cu','Universidad de Camagüey (Cuba)'),(35,'uca.es','Universidad de Cádiz'),(36,'ciccp.es','Colegio de Ingenieros de Caminos, Canales y Puertos'),(37,'upo.es','Universidad Pablo de Olavide'),(38,'iaa.es','Instituto de Astrofísica de Andalucía CSIC'),(39,'utpl.edu.ec','Universidad Técnica Particular de Loja (Ecuador)'),(40,'sergas.es','Consellería de Sanidade'),(41,'una.ac.cr','Universidad Nacional (Costa Rica)'),(42,'jyu.fi','Jyväskylän yliopisto (Finland)'),(43,'alum.uca.es','Universidad de Cádiz: alumnos'),(44,'etu.unige.ch','Université de Genève (Suisse)'),(45,'geneura.ugr.es','Universidad de Granada: GeNeura'),(46,'student.uibk.ac.at','Universität Innsbruck (Österreich): studenten'),(47,'etsii.ull.es','Universidad de La Laguna: E. T. S. I. Informática'),(48,'uconn.edu','University of Connecticut (USA)'),(49,'student.ulg.ac.be','Université de Liège (Belgique): étudiants'),(50,'uned.ac.cr','Universidad Estatal a Distancia (Costa Rica)'),(51,'ucdavis.edu','University of California, Davis (USA)'),(52,'alumni.unav.es','Universidad de Navarra: alumnos'),(53,'estudiante.uam.es','Universidad Autónoma de Madrid: estudiantes'),(54,'usal.es','Universidad de Salamanca'),(55,'ugent.be','Universiteit Gent (Belgique)'),(56,'du.edu','University of Denver (USA)'),(57,'um.es','Universidad de Murcia'),(58,'yale.edu','Yale University (USA)'),(59,'tulane.edu','Tulane University (USA)'),(60,'sbg.ac.at','Universität Salzburg (Österreich)'),(61,'ucsc.edu','University of California, Santa Cruz (USA)'),(62,'fiv.upv.es','Universitat Politècnica de València: E. T. S. d&#39;Enginyeria Informàtica'),(63,'alum.us.es','Universidad de Sevilla: alumnos'),(64,'uclm.es','Universidad de Castilla-La Mancha'),(65,'uea.ac.uk','University of East Anglia (UK)'),(66,'student.leedsmet.ac.uk','Leeds Metropolitan University (UK): students'),(67,'et.mde.es','Ministerio de Defensa: Ejército de Tierra'),(68,'epfl.ch','Ecole Polytechnique Federale de Lausanne (Suisse)'),(69,'bris.ac.uk','Bristol University (UK)'),(70,'icagr.es','Ilustre Colegio de Abogados de Granada'),(71,'eez.csic.es','Estación Experimental del Zaidín CSIC'),(72,'sheffield.ac.uk','University of Sheffield (UK)'),(73,'umich.edu','University of Michigan (USA)'),(74,'oc.mde.es','Mnisterio de Defensa'),(75,'studio.unibo.it','Università di Bologna (Italia)'),(76,'student.unife.it','Università di Ferrara (Italia): studenti'),(77,'uni-bonn.de','Universität Bonn (Deutschland)'),(78,'correo.cop.es','Col·legi Oficial de Psicòlegs de la Comunitat Valenciana'),(79,'mcu.es','Ministerio de Cultura'),(80,'map.es','Ministerio de Política Territorial'),(81,'ilstu.edu','Illinois State University (USA)'),(82,'coitt.es','Colegio Oficial de Ingenieros Técnicos de Telecomunicación'),(83,'ncl.ac.uk','Newcastle University (UK)'),(84,'unavarra.es','Universidad Pública de Navarra'),(85,'unh.edu','University of New Hampshire (USA)'),(86,'unex.es','Universidad de Extremadura'),(87,'tu-cottbus.de','Brandenburgische Technische Universität Cottbus (Deutschland)'),(88,'uni-ulm.de','Universität Ulm (Deutschland)'),(89,'nottingham.ac.uk','University of Nottingham (UK)'),(90,'humv.es','Hospital Universitario Marqués de Valdecilla (Santander)'),(91,'uv.es','Universitat de València'),(92,'univ-lyon2.fr','Université Lyon 2 (France)'),(93,'uni-konstanz.de','Universität Konstanz (Deutschland)'),(94,'warwick.ac.uk','University of Warwick (UK)'),(95,'ditec.ugr.es','Universidad de Granada: Grupo de Investigación DiTEC'),(96,'u.washington.edu','University of Washington (USA)'),(97,'umh.es','Universidad Miguel Hernández de Elche'),(98,'sussex.ac.uk','University of Sussex (UK)'),(99,'cam.upv.es','Universitat Politècnica de València: E. T. S. de Caminos, Canales y Puertos'),(100,'tgss.seg-social.es','Tesorería General de la Seguridad Social'),(101,'uoregon.edu','University of Oregon (USA)'),(102,'urjc.es','Universidad Rey Juan Carlos'),(103,'alu.uclm.es','Universidad de Castilla-La Mancha: alumnos'),(104,'inss.seg-social.es','Instituto Nacional de la Seguridad Social'),(105,'pmdgranada.org','Patronato Municipal de Deportes - Ayuntamiento de Granada'),(106,'csusm.edu','California State University, San Marcos (USA)'),(107,'fcjs.unl.edu.ar','Universidad Nacional del Litoral (Argentina): Facultad de Ciencias Jurídicas y Sociales'),(108,'alu.ua.es','Universitat d&#39;Alacant: alumnos'),(109,'stud.nhh.no','Norwegian School of Economics and Business Administration (Norway)'),(110,'ucla.edu','University of California, Los Angeles (USA)'),(111,'aberdeen.ac.uk','University of Aberdeen (UK)'),(112,'nuig.ie','National University of Ireland, Galway (Ireland)'),(113,'etu.univ-tours.fr','Université Francois Rabelais, Tours (France): étudiants'),(115,'andaluciajunta.es','Junta de Andalucía: correo andaluz'),(116,'everis.com','Consultora Everis');
/*!40000 ALTER TABLE `mail_domains` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-07-18 19:54:50
