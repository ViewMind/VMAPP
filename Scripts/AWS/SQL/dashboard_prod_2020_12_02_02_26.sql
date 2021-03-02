-- MySQL dump 10.14  Distrib 5.5.68-MariaDB, for Linux (x86_64)
--
-- Host: viewmind-dashboard.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com    Database: viewmind_dashboard
-- ------------------------------------------------------
-- Server version	5.7.26-log

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
-- Current Database: `viewmind_dashboard`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `viewmind_dashboard` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `viewmind_dashboard`;

--
-- Table structure for table `tComplaints`
--

DROP TABLE IF EXISTS `tComplaints`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tComplaints` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `arrival_date` date DEFAULT NULL,
  `institution` int(11) DEFAULT NULL,
  `product_sn` varchar(255) DEFAULT NULL,
  `complaint` int(11) DEFAULT NULL,
  `complaint_description` varchar(255) DEFAULT NULL,
  `inmmediate_action` int(11) DEFAULT NULL,
  `action_date` date DEFAULT NULL,
  `received_by` varchar(255) DEFAULT NULL,
  `internal_analysis` varchar(255) DEFAULT NULL,
  `action_description` varchar(255) DEFAULT NULL,
  `user_in_charge` varchar(255) DEFAULT NULL,
  `complaint_ID` varchar(255) DEFAULT NULL,
  `recall_ID` varchar(255) DEFAULT NULL,
  `notification_ID` varchar(255) DEFAULT NULL,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tComplaints`
--

LOCK TABLES `tComplaints` WRITE;
/*!40000 ALTER TABLE `tComplaints` DISABLE KEYS */;
/*!40000 ALTER TABLE `tComplaints` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tEvaluations`
--

DROP TABLE IF EXISTS `tEvaluations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tEvaluations` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `institution` int(11) DEFAULT NULL,
  `evaluations` int(11) DEFAULT NULL,
  PRIMARY KEY (`keyid`),
  UNIQUE KEY `institution` (`institution`,`evaluations`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tEvaluations`
--

LOCK TABLES `tEvaluations` WRITE;
/*!40000 ALTER TABLE `tEvaluations` DISABLE KEYS */;
INSERT INTO `tEvaluations` VALUES (4,0,-1),(2,989302458,-1),(1,1083582946,-1),(3,1105849094,-1),(5,1145868706,-1),(6,1242673082,-1),(7,1369462188,-1);
/*!40000 ALTER TABLE `tEvaluations` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tInstitutions`
--

DROP TABLE IF EXISTS `tInstitutions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tInstitutions` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `uid` int(11) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `firstname` varchar(255) DEFAULT NULL,
  `lastname` varchar(255) DEFAULT NULL,
  `address` varchar(255) DEFAULT NULL,
  `phone` varchar(255) DEFAULT NULL,
  `email` varchar(255) DEFAULT NULL,
  `enabled` tinyint(4) DEFAULT NULL,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tInstitutions`
--

LOCK TABLES `tInstitutions` WRITE;
/*!40000 ALTER TABLE `tInstitutions` DISABLE KEYS */;
INSERT INTO `tInstitutions` VALUES (1,1145868706,'Axis - Bahia Blanca','David Alejandro','Orozco','San Martin 752, BahiÂ­a Blanca, Buenos Aires','+5492914023913','ddaaorozco@gmail.com',1,'2020-05-10 18:14:47','ariela'),(2,1242673082,'Strathclyde','Mario','Parra','50 George St ,Graham Hills Building, Universyty of Strathclyde, Glagow, Scotland, UK. ZipCode G1 1QE','+447854483968','mario.parra-rodriguez@strath.ac.uk',1,'2020-05-10 18:14:47','ariela'),(3,740292426,'Sanatorio de los Arcos','Maria Alejandra','Amengual','Av. Juan B Justo 909, Sanatorio de los Arcos, Ciudad Autonoma de Buenos Aires, Argentina','47784613','MariaAlejandra.Amengual@swissmedical.com.ar',0,'2020-05-10 18:14:47','ariela'),(4,1369462188,'ViewMind','Matias','','','','',1,'2020-05-10 18:14:47','ariela'),(5,0,'Test Institution','Ariel','Arelovich','','','aarelovich@gmail.com',1,'2020-05-10 18:14:47','ariela'),(6,1771608688,'Fundacion Fleni','Salvador','Guinjoan','Montaneses 2325, Ciudad Autonoma de Buenos Aires','+541157773200 (INT 2514)','sguinjoan@fleni.org.ar',0,'2020-05-10 18:14:47','ariela'),(7,1105849094,'Fundacion Universidad de Antioquia','Luis guillermo','Mendez Barrera','Sede de Investigacion Universitaria SIU Calle 62 No. 52 - 59','+5742196664','luis.mendez48@gmail.com',1,'2020-05-10 18:14:47','ariela'),(8,989302458,'Novartis','Jelena','Curcic','NIBR Building, Novartis, Basel, Switzaerland','+ 41 79 500 36 48','jelena.curcic@novartis.com',1,'2020-05-10 18:14:47','ariela'),(9,1083582946,'Universidad Complutense de Madrid','Sara Fernandez','Guinea','','','',1,'2020-05-10 18:14:47','ariela'),(10,1083582946,'Universidad Complutense de Madrid','Sara Fernandez','Guinea','','','',1,'2020-05-14 14:11:51','ariela'),(11,989302458,'Novartis','Jelena','Curcic','NIBR Building, Novartis, Basel, Switzaerland','+ 41 79 500 36 48','jelena.curcic@novartis.com',1,'2020-05-14 14:12:02','ariela'),(12,1105849094,'Fundacion Universidad de Antioquia','Luis guillermo','Mendez Barrera','Sede de Investigacion Universitaria SIU Calle 62 No. 52 - 59','+5742196664','luis.mendez48@gmail.com',1,'2020-05-14 14:12:12','ariela'),(13,0,'Test Institution','Ariel','Arelovich','','','aarelovich@gmail.com',1,'2020-05-14 14:12:18','ariela'),(14,1145868706,'Axis - Bahia Blanca','David Alejandro','Orozco','San Martin 752, BahiÂ­a Blanca, Buenos Aires','+5492914023913','ddaaorozco@gmail.com',1,'2020-05-14 14:12:25','ariela'),(15,1242673082,'Strathclyde','Mario','Parra','50 George St ,Graham Hills Building, Universyty of Strathclyde, Glagow, Scotland, UK. ZipCode G1 1QE','+447854483968','mario.parra-rodriguez@strath.ac.uk',1,'2020-05-14 14:12:34','ariela'),(16,1369462188,'ViewMind','Matias','','','','',1,'2020-05-14 14:13:02','ariela');
/*!40000 ALTER TABLE `tInstitutions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tMonthCounters`
--

DROP TABLE IF EXISTS `tMonthCounters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tMonthCounters` (
  `mmyyyy` date DEFAULT NULL,
  `counter` int(11) DEFAULT NULL,
  `keyid` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tMonthCounters`
--

LOCK TABLES `tMonthCounters` WRITE;
/*!40000 ALTER TABLE `tMonthCounters` DISABLE KEYS */;
INSERT INTO `tMonthCounters` VALUES ('2019-02-24',2,1),('2019-03-01',2,2),('2019-04-23',0,3),('2019-12-27',0,4),('2020-01-08',0,5),('2020-02-21',0,6),('2020-03-18',0,7),('2020-04-03',1,8);
/*!40000 ALTER TABLE `tMonthCounters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tProduction`
--

DROP TABLE IF EXISTS `tProduction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tProduction` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `product_sn` varchar(255) DEFAULT NULL,
  `linked_institution` int(11) DEFAULT NULL,
  `production_date` date DEFAULT NULL,
  `product` int(11) DEFAULT NULL,
  `software_version` varchar(255) DEFAULT NULL,
  `et_software_version` int(11) DEFAULT NULL,
  `pc_configuration` tinyint(4) DEFAULT NULL,
  `pc_configuration_rejection` int(11) DEFAULT NULL,
  `et_install` tinyint(4) DEFAULT NULL,
  `et_install_rejection` int(11) DEFAULT NULL,
  `manual_version` int(11) DEFAULT NULL,
  `final_control` tinyint(4) DEFAULT NULL,
  `final_control_rejection` int(11) DEFAULT NULL,
  `labelling` tinyint(4) DEFAULT NULL,
  `packaging` tinyint(4) DEFAULT NULL,
  `final_release` tinyint(4) DEFAULT NULL,
  `observations` varchar(255) DEFAULT NULL,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  `vm_soft_install` tinyint(4) DEFAULT NULL,
  `vm_soft_install_rejection` int(11) DEFAULT NULL,
  `vm_soft_install_mod` timestamp NULL DEFAULT NULL,
  `vm_soft_install_user` varchar(255) DEFAULT NULL,
  `pc_configuration_mod` timestamp NULL DEFAULT NULL,
  `pc_configuration_user` varchar(255) DEFAULT NULL,
  `et_install_mod` timestamp NULL DEFAULT NULL,
  `et_install_user` varchar(255) DEFAULT NULL,
  `eyeexp_install_mod` timestamp NULL DEFAULT NULL,
  `eyeexp_install_user` varchar(255) DEFAULT NULL,
  `final_control_mod` timestamp NULL DEFAULT NULL,
  `final_control_user` varchar(255) DEFAULT NULL,
  `labelling_mod` timestamp NULL DEFAULT NULL,
  `labelling_user` varchar(255) DEFAULT NULL,
  `packaging_mod` timestamp NULL DEFAULT NULL,
  `packaging_user` varchar(255) DEFAULT NULL,
  `final_release_mod` timestamp NULL DEFAULT NULL,
  `final_release_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=25 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tProduction`
--

LOCK TABLES `tProduction` WRITE;
/*!40000 ALTER TABLE `tProduction` DISABLE KEYS */;
INSERT INTO `tProduction` VALUES (1,'3-20200514-141614',NULL,'2020-05-14',3,'',0,0,0,0,0,0,0,0,0,0,1,'','2020-05-14 14:16:14','ariela',1,NULL,'2020-05-14 14:16:14','ariela','2020-05-14 14:16:14','ariela','2020-05-14 14:16:14','ariela','2020-05-14 14:16:14','ariela','2020-05-14 14:16:14','ariela','2020-05-14 14:16:14','ariela','2020-05-14 14:16:14','ariela','2020-05-14 14:16:14','ariela'),(2,'3-20200514-141614',0,'2020-05-14',3,'',0,0,0,0,0,0,0,0,0,0,1,'','2020-05-14 14:16:29','ariela',1,NULL,'2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela'),(3,'4-20200515-195914',NULL,'2020-02-21',4,'13.0.5',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 19:59:14','mshulz',1,NULL,'2020-05-15 19:59:14','mshulz','2020-05-15 19:59:14','mshulz','2020-05-15 19:59:14','mshulz','2020-05-15 19:59:14','mshulz','2020-05-15 19:59:14','mshulz','2020-05-15 19:59:14','mshulz','2020-05-15 19:59:14','mshulz','2020-05-15 19:59:14','mshulz'),(4,'4-20200515-200040',NULL,'2020-01-08',4,'13.0.0',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:00:40','mshulz',1,NULL,'2020-05-15 20:00:40','mshulz','2020-05-15 20:00:40','mshulz','2020-05-15 20:00:40','mshulz','2020-05-15 20:00:40','mshulz','2020-05-15 20:00:40','mshulz','2020-05-15 20:00:40','mshulz','2020-05-15 20:00:40','mshulz','2020-05-15 20:00:40','mshulz'),(5,'4-20200515-200142',NULL,'2019-12-27',4,'13.0.0',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:01:42','mshulz',1,NULL,'2020-05-15 20:01:42','mshulz','2020-05-15 20:01:42','mshulz','2020-05-15 20:01:42','mshulz','2020-05-15 20:01:42','mshulz','2020-05-15 20:01:42','mshulz','2020-05-15 20:01:42','mshulz','2020-05-15 20:01:42','mshulz','2020-05-15 20:01:42','mshulz'),(6,'5-20200515-200524',NULL,'2019-09-25',5,'',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:05:24','mshulz',1,NULL,'2020-05-15 20:05:24','mshulz','2020-05-15 20:05:24','mshulz','2020-05-15 20:05:24','mshulz','2020-05-15 20:05:24','mshulz','2020-05-15 20:05:24','mshulz','2020-05-15 20:05:24','mshulz','2020-05-15 20:05:24','mshulz','2020-05-15 20:05:24','mshulz'),(7,'5-20200515-200848',NULL,'2018-08-09',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:08:48','mshulz',1,NULL,'2020-05-15 20:08:48','mshulz','2020-05-15 20:08:48','mshulz','2020-05-15 20:08:48','mshulz','2020-05-15 20:08:48','mshulz','2020-05-15 20:08:48','mshulz','2020-05-15 20:08:48','mshulz','2020-05-15 20:08:48','mshulz','2020-05-15 20:08:48','mshulz'),(8,'5-20200515-200948',NULL,'2019-02-25',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:09:48','mshulz',1,NULL,'2020-05-15 20:09:48','mshulz','2020-05-15 20:09:48','mshulz','2020-05-15 20:09:48','mshulz','2020-05-15 20:09:48','mshulz','2020-05-15 20:09:48','mshulz','2020-05-15 20:09:48','mshulz','2020-05-15 20:09:48','mshulz','2020-05-15 20:09:48','mshulz'),(9,'5-20200515-201054',NULL,'2019-02-25',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:10:54','mshulz',1,NULL,'2020-05-15 20:10:54','mshulz','2020-05-15 20:10:54','mshulz','2020-05-15 20:10:54','mshulz','2020-05-15 20:10:54','mshulz','2020-05-15 20:10:54','mshulz','2020-05-15 20:10:54','mshulz','2020-05-15 20:10:54','mshulz','2020-05-15 20:10:54','mshulz'),(10,'5-20200515-201202',NULL,'2019-03-01',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:12:02','mshulz',1,NULL,'2020-05-15 20:12:02','mshulz','2020-05-15 20:12:02','mshulz','2020-05-15 20:12:02','mshulz','2020-05-15 20:12:02','mshulz','2020-05-15 20:12:02','mshulz','2020-05-15 20:12:02','mshulz','2020-05-15 20:12:02','mshulz','2020-05-15 20:12:02','mshulz'),(11,'4-20200515-195914',1369462188,'2020-02-21',4,'13.0.5',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:12:59','mshulz',1,NULL,'2020-05-15 20:12:59','mshulz','2020-05-15 20:12:59','mshulz','2020-05-15 20:12:59','mshulz','2020-05-15 20:12:59','mshulz','2020-05-15 20:12:59','mshulz','2020-05-15 20:12:59','mshulz','2020-05-15 20:12:59','mshulz','2020-05-15 20:12:59','mshulz'),(12,'4-20200515-200040',1145868706,'2020-01-08',4,'13.0.0',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:13:29','mshulz',1,NULL,'2020-05-15 20:13:29','mshulz','2020-05-15 20:13:29','mshulz','2020-05-15 20:13:29','mshulz','2020-05-15 20:13:29','mshulz','2020-05-15 20:13:29','mshulz','2020-05-15 20:13:29','mshulz','2020-05-15 20:13:29','mshulz','2020-05-15 20:13:29','mshulz'),(13,'4-20200515-200142',989302458,'2019-12-27',4,'13.0.0',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:13:41','mshulz',1,NULL,'2020-05-15 20:13:41','mshulz','2020-05-15 20:13:41','mshulz','2020-05-15 20:13:41','mshulz','2020-05-15 20:13:41','mshulz','2020-05-15 20:13:41','mshulz','2020-05-15 20:13:41','mshulz','2020-05-15 20:13:41','mshulz','2020-05-15 20:13:41','mshulz'),(14,'5-20200515-200524',1369462188,'2019-09-25',5,'',0,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:14:13','mshulz',1,NULL,'2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz'),(15,'5-20200515-200848',1145868706,'2018-08-09',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:14:37','mshulz',1,NULL,'2020-05-15 20:14:37','mshulz','2020-05-15 20:14:37','mshulz','2020-05-15 20:14:37','mshulz','2020-05-15 20:14:37','mshulz','2020-05-15 20:14:37','mshulz','2020-05-15 20:14:37','mshulz','2020-05-15 20:14:37','mshulz','2020-05-15 20:14:37','mshulz'),(16,'5-20200515-200948',1105849094,'2019-02-25',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:14:55','mshulz',1,NULL,'2020-05-15 20:14:55','mshulz','2020-05-15 20:14:55','mshulz','2020-05-15 20:14:55','mshulz','2020-05-15 20:14:55','mshulz','2020-05-15 20:14:55','mshulz','2020-05-15 20:14:55','mshulz','2020-05-15 20:14:55','mshulz','2020-05-15 20:14:55','mshulz'),(17,'5-20200515-201054',1083582946,'2019-02-25',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:15:05','mshulz',1,NULL,'2020-05-15 20:15:05','mshulz','2020-05-15 20:15:05','mshulz','2020-05-15 20:15:05','mshulz','2020-05-15 20:15:05','mshulz','2020-05-15 20:15:05','mshulz','2020-05-15 20:15:05','mshulz','2020-05-15 20:15:05','mshulz','2020-05-15 20:15:05','mshulz'),(18,'5-20200515-201202',989302458,'2019-03-01',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-15 20:15:16','mshulz',1,NULL,'2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz'),(19,'5-20200515-201202',NULL,'2019-03-01',5,'8.1.5-GP',6,1,0,0,0,0,1,0,1,1,1,'','2020-05-30 19:57:22','ariela',1,0,'2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-30 19:57:22','ariela',NULL,NULL,'2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz'),(20,'5-20200515-201202',NULL,'2019-03-01',5,'8.1.5-GP',6,1,0,1,0,0,1,0,1,1,1,'','2020-05-30 19:57:37','ariela',1,0,'2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-30 19:57:37','ariela',NULL,NULL,'2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz','2020-05-15 20:15:16','mshulz'),(21,'3-20200514-141614',NULL,'2020-05-14',3,'',0,1,0,1,0,0,1,0,0,0,1,'','2020-06-01 19:15:09','ariela',1,0,'2020-05-14 14:16:29','ariela','2020-06-01 19:15:09','ariela','2020-06-01 19:15:09','ariela',NULL,NULL,'2020-06-01 19:15:09','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela'),(22,'3-20200514-141614',0,'2020-05-14',3,'',0,1,0,1,0,0,1,0,0,0,1,'','2020-06-21 13:07:13','ariela',1,0,'2020-05-14 14:16:29','ariela','2020-06-01 19:15:09','ariela','2020-06-01 19:15:09','ariela',NULL,NULL,'2020-06-01 19:15:09','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela','2020-05-14 14:16:29','ariela'),(23,'5-20200515-200524',NULL,'2019-09-25',5,'10',0,1,0,1,0,0,1,0,1,1,1,'','2020-10-07 18:55:42','laura.vega',1,0,'2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz',NULL,NULL,'2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz'),(24,'5-20200515-200524',NULL,'2019-09-25',5,'',0,1,0,1,0,0,1,0,1,1,1,'','2020-10-07 18:56:02','laura.vega',1,0,'2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz',NULL,NULL,'2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz','2020-05-15 20:14:13','mshulz');
/*!40000 ALTER TABLE `tProduction` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tPurchases`
--

DROP TABLE IF EXISTS `tPurchases`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tPurchases` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `product_type` int(11) DEFAULT NULL,
  `quantity` int(11) DEFAULT NULL,
  `brand` varchar(255) DEFAULT NULL,
  `lot` varchar(255) DEFAULT NULL,
  `model` varchar(255) DEFAULT NULL,
  `serial_number` varchar(255) DEFAULT NULL,
  `supplier` varchar(255) DEFAULT NULL,
  `entrance_date` date DEFAULT NULL,
  `invoice_number` varchar(255) DEFAULT NULL,
  `admin_accepted` tinyint(4) DEFAULT NULL,
  `admin_rejection_reason` int(11) DEFAULT NULL,
  `technical_accepted` tinyint(4) DEFAULT NULL,
  `technical_rejection_reason` int(11) DEFAULT NULL,
  `controled_by` varchar(255) DEFAULT NULL,
  `control_date` date DEFAULT NULL,
  `purchase_id` int(11) DEFAULT NULL,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tPurchases`
--

LOCK TABLES `tPurchases` WRITE;
/*!40000 ALTER TABLE `tPurchases` DISABLE KEYS */;
INSERT INTO `tPurchases` VALUES (1,1,1,'Test EyeTracker','','','','Test Supplier','2020-05-14','',1,0,1,0,'ariela','2020-05-14',1589465713,'2020-05-14 14:15:13','ariela'),(2,2,1,'Test Laptop','','','','Test Supplier','2020-05-14','',1,0,1,0,'ariela','2020-05-14',1589465737,'2020-05-14 14:15:37','ariela'),(3,1,1,'HTC','','HTC VIVE Pro Eye','FA96JAX00078','HTC','2020-02-21','',1,0,1,0,'mshulz','2020-02-21',1589560580,'2020-05-15 16:36:20','mshulz'),(4,1,1,'HTC','','HTC VIVE Pro Eye','','HTC','2020-01-08','77H02705-00M',1,0,1,0,'mshulz','2020-01-08',1589560980,'2020-05-15 16:43:00','mshulz'),(5,1,1,'HTC','','HTC VIVE Pro Eye','77H02705-00M-1','HTC','2020-01-08','',1,0,1,0,'mshulz','2020-01-08',1589560980,'2020-05-15 16:43:30','mshulz'),(6,1,1,'HTC','','HTC VIVE Pro Eye','77H02705-00M-2','HTC','2019-12-27','',1,0,1,0,'mshulz','2019-12-27',1589561065,'2020-05-15 16:44:25','mshulz'),(7,1,1,'SMI','','RED 250','','SMI','2019-09-25','',1,0,1,0,'mshulz','2019-09-25',1589561114,'2020-05-15 16:45:14','mshulz'),(8,1,1,'GazePoint','','GP3HD','GP3HD-18276270','GazePoint','2018-08-09','',1,0,1,0,'mshulz','2018-09-01',1589561209,'2020-05-15 16:46:49','mshulz'),(9,1,1,'GazePoint','','GP3HD','GP3HD-17496465','GazePoint','2019-02-01','',1,0,1,0,'mshulz','2019-02-25',1589561281,'2020-05-15 16:48:01','mshulz'),(10,1,1,'GazePoint','','GP3HD','GP3HD-18276280','GazePoint','2019-02-25','',1,0,1,0,'mshulz','2019-02-25',1589561356,'2020-05-15 16:49:16','mshulz'),(11,1,1,'GazePoint','','GP3HD','GP3HD-18318816','GazePoint','2019-03-01','',1,0,1,0,'mshulz','2019-03-04',1589561392,'2020-05-15 16:49:52','mshulz'),(12,2,1,'HP','','HP 15-bs022la','CND7510L15','HP','2019-02-25','',1,0,1,0,'mshulz','2019-02-25',1589564018,'2020-05-15 17:33:38','mshulz'),(13,2,1,'HP','','HP 15-bs022la','CND7338LYY','HP','2019-02-01','',1,0,1,0,'mshulz','2019-02-01',1589564059,'2020-05-15 17:34:19','mshulz'),(14,2,1,'HP','','HP I5 - bsO22ls','CND7510KXQ','HP','2018-08-09','',1,0,1,0,'mshulz','2018-08-09',1589564101,'2020-05-15 17:35:01','mshulz'),(15,2,1,'Lenovo','','Lenovo V310-15ISK','LROAXR11','Lenovo','2019-03-01','',1,0,1,0,'mshulz','2019-03-01',1589564154,'2020-05-15 17:35:54','mshulz'),(16,2,1,'MSI','','MSI GF62 8RE','K1809N0105729','MSI','2019-12-27','',1,0,1,0,'mshulz','2019-12-27',1589564199,'2020-05-15 17:36:39','mshulz'),(17,2,1,'MSI','','MSI GL73 9RCX','K1908N0010802','MSI','2020-01-08','',1,0,1,0,'mshulz','2020-01-08',1589564251,'2020-05-15 17:37:31','mshulz'),(18,2,1,'Legion','','81SY','PF1Y846H','Legion','2020-02-21','',1,0,1,0,'mshulz','2020-02-21',1589564583,'2020-05-15 17:43:03','mshulz');
/*!40000 ALTER TABLE `tPurchases` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tStockMP`
--

DROP TABLE IF EXISTS `tStockMP`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tStockMP` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `purchase_id` int(11) DEFAULT NULL,
  `inuse_product` varchar(255) DEFAULT NULL,
  `disabled` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tStockMP`
--

LOCK TABLES `tStockMP` WRITE;
/*!40000 ALTER TABLE `tStockMP` DISABLE KEYS */;
INSERT INTO `tStockMP` VALUES (1,1589465713,'3-20200514-141614',0),(2,1589465737,'3-20200514-141614',0),(3,1589560580,'4-20200515-195914',0),(4,1589560980,'4-20200515-200040',0),(5,1589561065,'4-20200515-200142',0),(6,1589561114,'5-20200515-200524',0),(7,1589561209,'5-20200515-200848',0),(8,1589561281,'5-20200515-200948',0),(9,1589561356,'5-20200515-201054',0),(10,1589561392,'5-20200515-201202',0),(11,1589564018,'5-20200515-201054',0),(12,1589564059,'5-20200515-200948',0),(13,1589564101,'5-20200515-200848',0),(14,1589564154,'5-20200515-201202',0),(15,1589564199,'4-20200515-200142',0),(16,1589564251,'4-20200515-200040',0),(17,1589564583,'4-20200515-195914',0);
/*!40000 ALTER TABLE `tStockMP` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tStrings`
--

DROP TABLE IF EXISTS `tStrings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tStrings` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `table_column` varchar(255) DEFAULT NULL,
  `string` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tStrings`
--

LOCK TABLES `tStrings` WRITE;
/*!40000 ALTER TABLE `tStrings` DISABLE KEYS */;
INSERT INTO `tStrings` VALUES (1,'tPurchases.product_type','Eye Tracker'),(2,'tPurchases.product_type','Laptops'),(3,'tProduction.product','Test Product'),(4,'tProduction.product','EyeExplorer - VR'),(5,'tProduction.product','Eye Explorer'),(6,'tProduction.et_software_version','GazePoint Control x64 4.3.11'),(7,'tTrainedUsers.profesion','Director TÃ©cnico'),(8,'tTrainedUsers.profesion','Director Tecnico');
/*!40000 ALTER TABLE `tStrings` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tSuppliers`
--

DROP TABLE IF EXISTS `tSuppliers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tSuppliers` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `supplier` varchar(255) DEFAULT NULL,
  `country` varchar(255) DEFAULT NULL,
  `supplier_id` varchar(255) DEFAULT NULL,
  `contact_name` varchar(255) DEFAULT NULL,
  `phone_number` varchar(255) DEFAULT NULL,
  `email` varchar(255) DEFAULT NULL,
  `enabled` tinyint(4) DEFAULT NULL,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tSuppliers`
--

LOCK TABLES `tSuppliers` WRITE;
/*!40000 ALTER TABLE `tSuppliers` DISABLE KEYS */;
INSERT INTO `tSuppliers` VALUES (1,'Test Supplier','','','','','',0,'2020-05-14 14:13:46','ariela'),(2,'Test Supplier','','','','','',1,'2020-05-14 14:14:13','ariela'),(3,'GazePoint','Canada','GazePoint','Johnny','','johnny@gazept.com',1,'2020-05-15 16:15:35','mshulz'),(4,'HTC','USA','HTC','HTC Support','','eu_support@htc.com',1,'2020-05-15 16:17:06','mshulz'),(5,'SMI','','SMI','Jeff','','sales@singyip.net',1,'2020-05-15 16:19:03','mshulz'),(6,'Pupil Labs','Germany','Pupil Labs','Swati','','info@pupil-labs.com',0,'2020-05-15 16:21:00','mshulz'),(7,'Lenovo','USA','Lenovo','','','',0,'2020-05-15 17:31:26','mshulz'),(8,'HP','USA','HP','','','',1,'2020-05-15 17:31:40','mshulz'),(9,'Lenovo','USA','Lenovo','','','',1,'2020-05-15 17:31:55','mshulz'),(10,'MSI','USA','MSI','','','',1,'2020-05-15 17:32:13','mshulz'),(11,'Legion','USA','Legion','','','',1,'2020-05-15 17:40:08','mshulz');
/*!40000 ALTER TABLE `tSuppliers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tTechnicalAssistance`
--

DROP TABLE IF EXISTS `tTechnicalAssistance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tTechnicalAssistance` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `product_sn` varchar(255) DEFAULT NULL,
  `arrival_date` date DEFAULT NULL,
  `received_by` varchar(255) DEFAULT NULL,
  `institution` int(11) DEFAULT NULL,
  `problem_found` int(11) DEFAULT NULL,
  `cause` int(11) DEFAULT NULL,
  `solution` varchar(255) DEFAULT NULL,
  `resolution_date` date DEFAULT NULL,
  `user_in_charge` varchar(255) DEFAULT NULL,
  `assist_uid` varchar(255) DEFAULT NULL,
  `extra_details` varchar(255) DEFAULT NULL,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tTechnicalAssistance`
--

LOCK TABLES `tTechnicalAssistance` WRITE;
/*!40000 ALTER TABLE `tTechnicalAssistance` DISABLE KEYS */;
/*!40000 ALTER TABLE `tTechnicalAssistance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tTrainedUsers`
--

DROP TABLE IF EXISTS `tTrainedUsers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tTrainedUsers` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `trained_user_id` int(11) DEFAULT NULL,
  `profesion` varchar(255) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `email` varchar(255) DEFAULT NULL,
  `is_trained` tinyint(4) DEFAULT NULL,
  `birth_date` date DEFAULT NULL,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tTrainedUsers`
--

LOCK TABLES `tTrainedUsers` WRITE;
/*!40000 ALTER TABLE `tTrainedUsers` DISABLE KEYS */;
INSERT INTO `tTrainedUsers` VALUES (1,0,'8','Matias Shulz','matias.shulz@viewmind.ai',0,'1987-05-14','2020-10-14 18:19:50','laura.vega'),(2,0,'8','Matias Shulz','matias.shulz@viewmind.ai',1,'1987-05-14','2020-10-14 18:20:10','laura.vega');
/*!40000 ALTER TABLE `tTrainedUsers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tUsers`
--

DROP TABLE IF EXISTS `tUsers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tUsers` (
  `name` varchar(255) DEFAULT NULL,
  `lastname` varchar(255) DEFAULT NULL,
  `passwd` varchar(255) DEFAULT NULL,
  `username` varchar(255) DEFAULT NULL,
  `email` varchar(255) DEFAULT NULL,
  `permissions` int(11) DEFAULT NULL,
  `keyid` int(11) NOT NULL AUTO_INCREMENT,
  `mod_last` timestamp NULL DEFAULT NULL,
  `mod_user` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tUsers`
--

LOCK TABLES `tUsers` WRITE;
/*!40000 ALTER TABLE `tUsers` DISABLE KEYS */;
INSERT INTO `tUsers` VALUES ('Ariel Eduardo','Arelovich','$2y$10$RTQaeGgmBw4XUcoN2thGoex6RcsuKINZ5R6TDttiPgrrfTX1L8mBe','ariela','aarelovich@gmail.com',2,3,'2020-05-14 14:02:06','ariela'),('Matias','Shulz','$2y$10$lK/v/oRJgdTdTGduC.wtkOo7GXVH0pGdp2ppF8Gzlhcfbn4LR1UGi','mshulz','matias.shulz@viewmind.com.ar',1,4,'2020-05-14 14:02:06','ariela'),('Laura','Vega','$2y$10$e081hAm0CzGI12y2UZwPKOc1mkgtKTJ3raNm2A8Kk97/ELyYinzoO','laura.vega','lvega@axxionconsultora.com.ar',1,5,'2020-05-14 14:02:06','ariela'),('Gerardo','Fernandez','$2y$10$.NqS9U/P4qRvnrcIvF1/BuCnH9e7kriA78gekaakUm8pUZ0VOLkRy','gfernandez','gerardofernandez480@gmail.com',1,6,'2020-05-14 14:02:06','ariela'),('David','Orozco','$2y$10$IfNLb9ncjLM5WndEcfewVOBO1zrWQdCMnN9wcF.AsGqQyd0AwUuHS','dorozco','ddaaorozco@gmail.com',1,7,'2020-05-14 14:02:06','ariela'),('Mario','Parra','$2y$10$fCObQNUXbd2zky1CFnojWeMJ0ZAo0oyIQOz9nr04GAhL1VYCPaT7G','mario.parra','marioparra05@gmail.com',1,8,'2020-05-14 14:02:06','ariela'),('Gustavo','Abdala','$2y$10$edl1DDXvpsYMrLnX0jFkqucMFo8r478bZ251B.9tZQK.0NXnjyeB2','gustavo.abdala','gustavo.abdala@axxionconsultora.com',1,10,'2020-05-14 14:02:06','ariela'),('Mark','Edwards','$2y$10$UZPG5VlVPd/9wgDJfWrJxOEAs6Qler7zc4rFkROu3B.1yaqnmowwi','mark.edwards','mark.edwards@viewmind.ai',1,11,'2020-05-14 14:02:06','ariela'),('Matias','Shulz','$2y$10$lK/v/oRJgdTdTGduC.wtkOo7GXVH0pGdp2ppF8Gzlhcfbn4LR1UGi','mshulz','matias.shulz@viewmind.com.ar',2,12,'2020-05-14 14:11:08','ariela'),('Tomas','Presman','$2y$10$ZyBoikCrGVZ/sKsi3KkkZO35A1L8dhlXeK1lVWS8gcIDKOVYx2zXC','tpresman','tpresman@axxionconsultora.com.ar',0,13,'2020-05-26 20:15:15','mshulz'),('Tomas','Presman','$2y$10$w9J3DvyKk98oEwXki4T0HeSYB.BODBnks7qx3cz5nFObNUK0PEBVK','tpresman','tpresman@axxionconsultora.com.ar',2,14,'2020-05-26 20:15:30','mshulz'),('Gustavo','Abdala','$2y$10$edl1DDXvpsYMrLnX0jFkqucMFo8r478bZ251B.9tZQK.0NXnjyeB2','gustavo.abdala','gustavo.abdala@axxionconsultora.com',2,15,'2020-06-01 18:04:18','mshulz'),('Laura','Vega','$2y$10$e081hAm0CzGI12y2UZwPKOc1mkgtKTJ3raNm2A8Kk97/ELyYinzoO','laura.vega','lvega@axxionconsultora.com.ar',2,16,'2020-06-01 18:04:26','mshulz'),('Gerardo','Fernandez','$2y$10$.NqS9U/P4qRvnrcIvF1/BuCnH9e7kriA78gekaakUm8pUZ0VOLkRy','gfernandez','gerardofernandez480@gmail.com',2,17,'2020-06-30 16:20:27','mshulz'),('Gerardo','Fernandez','$2y$10$MVj6lnJUKBMZVkQpgrVwuOuV1YJeeL1RrmlV961wTe9rhunQsXBbm','gfernandez','gerardofernandez480@gmail.com',2,18,'2020-06-30 16:22:01','mshulz'),('Gustavo','Abdala','$2y$10$NA0PEOfmG0Q30vo5XaMOP./pLCSVZc7djQW6WE50qFHBbhOi9EZPC','gustavo.abdala','gustavo.abdala@axxionconsultora.com',2,19,'2020-10-13 19:40:30','laura.vega');
/*!40000 ALTER TABLE `tUsers` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2020-12-02  5:27:18
