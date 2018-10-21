-- MySQL dump 10.14  Distrib 5.5.56-MariaDB, for Linux (x86_64)
--
-- Host: viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com    Database: viewmind_develop
-- ------------------------------------------------------
-- Server version	5.6.40-log

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
-- Current Database: `viewmind_develop`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `viewmind_develop` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `viewmind_develop`;

--
-- Table structure for table `tDoctors`
--

DROP TABLE IF EXISTS `tDoctors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tDoctors` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `uid` varchar(255) DEFAULT NULL,
  `firstname` varchar(255) DEFAULT NULL,
  `lastname` varchar(255) DEFAULT NULL,
  `idtype` varchar(255) DEFAULT NULL,
  `countryid` varchar(2) DEFAULT NULL,
  `state` varchar(255) DEFAULT NULL,
  `city` varchar(255) DEFAULT NULL,
  `medicalinstitution` varchar(255) DEFAULT NULL,
  `telephone` varchar(30) DEFAULT NULL,
  `email` varchar(255) DEFAULT NULL,
  `address` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=51 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tDoctors`
--

LOCK TABLES `tDoctors` WRITE;
/*!40000 ALTER TABLE `tDoctors` DISABLE KEYS */;
INSERT INTO `tDoctors` VALUES (1,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(2,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(3,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(4,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(5,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(6,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(7,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(8,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(9,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(10,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(11,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(12,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(13,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(14,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(15,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(16,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(17,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(18,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(19,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(20,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(21,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(22,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(23,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(24,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(25,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(26,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(27,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(28,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(29,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(30,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(31,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(32,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(33,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(34,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(35,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(36,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(37,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(38,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(39,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(40,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis Mama','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(41,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 415'),(42,'AR30123697','Juan','Doctor','DNI','AR','Catamarca','','Alguna','','',''),(43,'AR30123697','Juan','Doctor','DNI','AR','Catamarca','Pindonga','Alguna','','',''),(44,'BJvm00000000','Juan','Juan','DNI','BJ','Alguno','','','','',''),(45,'AR25134823','Gerardo','Fernandez','DNI','AR','','','CONICET','','',''),(46,'AR26264556','Marcela','schumacher','DNI','AR','buenos aires','Bahia Blanca','','','',''),(47,'AR21483418','Gustavo','Echeverria','DNI','AR','Buenos Aires','Bahia Blanca','Axis Neurociencias','','','San Martin 326'),(48,'AR23130517','Gustavo ','Sgrilli','DNI','AR','Buenos Aires','Bahia Blanca','Axis Naurociencias','','','San Martin 326'),(49,'AR24136069','David','Orozco','DNI','AR','Buenos Aires','Bahia Blanca','Axis Neurociencias','','','San martin 326'),(50,'AR26325486','Juan Jose','Rayer','DNI','AR','Buenos Aires','Bahia Blanca','Axis Neurociencias','','','San Martin 326');
/*!40000 ALTER TABLE `tDoctors` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tEyeResults`
--

DROP TABLE IF EXISTS `tEyeResults`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tEyeResults` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `patientid` varchar(255) DEFAULT NULL,
  `doctorid` varchar(255) DEFAULT NULL,
  `study_id` varchar(255) DEFAULT NULL,
  `study_date` datetime DEFAULT NULL,
  `rdTotalFixL` int(11) DEFAULT NULL,
  `rdFStepFixL` int(11) DEFAULT NULL,
  `rdMStepFixL` int(11) DEFAULT NULL,
  `rdSStepFixL` int(11) DEFAULT NULL,
  `rdTotalFixR` int(11) DEFAULT NULL,
  `rdFStepFixR` int(11) DEFAULT NULL,
  `rdMStepFixR` int(11) DEFAULT NULL,
  `rdSStepFixR` int(11) DEFAULT NULL,
  `bcAvgPupilR` float DEFAULT NULL,
  `ucAvgPupilR` float DEFAULT NULL,
  `bcAvgPupilL` float DEFAULT NULL,
  `ucAvgPupilL` float DEFAULT NULL,
  `bcGazeResR` float DEFAULT NULL,
  `ucGazeResR` float DEFAULT NULL,
  `bcGazeResL` float DEFAULT NULL,
  `ucGazeResL` float DEFAULT NULL,
  `bcCorrectAns` tinyint(4) DEFAULT NULL,
  `ucCorrentAns` tinyint(4) DEFAULT NULL,
  `bcWrongAns` tinyint(4) DEFAULT NULL,
  `ucWrongAns` tinyint(4) DEFAULT NULL,
  `bcTestCorrectAns` tinyint(4) DEFAULT NULL,
  `ucTestCorrectAns` tinyint(4) DEFAULT NULL,
  `bcTestWrongAns` tinyint(4) DEFAULT NULL,
  `ucTestWrongAns` tinyint(4) DEFAULT NULL,
  `fdHitR` int(11) DEFAULT NULL,
  `fdHitL` int(11) DEFAULT NULL,
  `fdHitTotal` int(11) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=30 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tEyeResults`
--

LOCK TABLES `tEyeResults` WRITE;
/*!40000 ALTER TABLE `tEyeResults` DISABLE KEYS */;
INSERT INTO `tEyeResults` VALUES (1,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-09-24 12:30:41',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(2,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 12:31:16',0,0,0,0,0,0,0,0,13.7033,12.8019,14.8025,13.8957,NULL,NULL,NULL,NULL,31,29,1,3,9,0,1,10,NULL,NULL,NULL),(3,'AR15896547','AR20356459','rdv1_bc2sv2_uc2sv2','2018-09-24 12:31:53',0,0,0,0,0,0,0,0,3.39331,3.2891,3.11052,3.04411,NULL,NULL,NULL,NULL,16,26,16,6,2,8,8,2,NULL,NULL,NULL),(4,'AR15896547','AR20356459','rdv1_bc2sv2_uc2sv2','2018-09-24 12:32:27',0,0,0,0,0,0,0,0,3.39331,3.2891,3.11052,3.04411,NULL,NULL,NULL,NULL,16,26,16,6,2,8,8,2,NULL,NULL,NULL),(5,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 12:33:02',111,99,12,90,111,99,12,90,3.51751,3.47412,2.89546,2.88567,NULL,NULL,NULL,NULL,16,16,16,16,5,5,5,5,NULL,NULL,NULL),(6,'AR15896547','AR20356459','rdv1_bc2sv2_uc2sv2','2018-09-24 12:43:43',111,99,12,90,111,99,12,90,3.39331,3.2891,3.11052,3.04411,NULL,NULL,NULL,NULL,16,26,16,6,2,8,8,2,NULL,NULL,NULL),(7,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 12:44:18',111,99,12,90,111,99,12,90,13.7033,12.8019,14.8025,13.8957,NULL,NULL,NULL,NULL,31,29,1,3,9,0,1,10,NULL,NULL,NULL),(8,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 12:44:53',0,0,0,0,0,0,0,0,13.7033,12.8019,14.8025,13.8957,NULL,NULL,NULL,NULL,31,29,1,3,9,0,1,10,NULL,NULL,NULL),(9,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 12:48:24',0,0,0,0,0,0,0,0,13.7033,12.8019,14.8025,13.8957,NULL,NULL,NULL,NULL,31,29,1,3,9,0,1,10,NULL,NULL,NULL),(10,'AR15896547','AR20356459','rdv1_bc2sv2_uc2sv2','2018-09-24 12:48:59',0,0,0,0,0,0,0,0,3.39331,3.2891,3.11052,3.04411,NULL,NULL,NULL,NULL,16,26,16,6,2,8,8,2,NULL,NULL,NULL),(11,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 12:49:33',111,99,12,90,111,99,12,90,3.51751,3.47412,2.89546,2.88567,NULL,NULL,NULL,NULL,16,16,16,16,5,5,5,5,NULL,NULL,NULL),(12,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-09-24 12:50:07',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(13,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-09-24 12:50:41',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(14,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 13:13:57',111,99,12,90,111,99,12,90,3.51751,3.47412,2.89546,2.88567,NULL,NULL,NULL,NULL,16,16,16,16,5,5,5,5,NULL,NULL,NULL),(15,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-09-24 13:14:31',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(16,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-09-24 13:15:05',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(17,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-24 13:15:40',0,0,0,0,0,0,0,0,13.7033,12.8019,14.8025,13.8957,NULL,NULL,NULL,NULL,31,29,1,3,9,0,1,10,NULL,NULL,NULL),(18,'AR15896547','AR20356459','rdv1_bc2sv2_uc2sv2','2018-09-24 13:16:14',0,0,0,0,0,0,0,0,3.39331,3.2891,3.11052,3.04411,NULL,NULL,NULL,NULL,16,26,16,6,2,8,8,2,NULL,NULL,NULL),(19,'AU55145dasdf','AR20356459','rdv1','2018-09-24 14:55:07',111,99,12,90,111,99,12,90,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(20,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-09-24 14:59:08',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(21,'AR15896547','AR20356459','bc2sv2_uc2sv2','2018-09-24 14:59:42',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,3.39331,3.2891,3.11052,3.04411,NULL,NULL,NULL,NULL,16,26,16,6,2,8,8,2,NULL,NULL,NULL),(22,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-25 22:31:55',111,99,12,90,111,99,12,90,3.51751,3.47412,2.89546,2.88567,NULL,NULL,NULL,NULL,16,16,16,16,5,5,5,5,NULL,NULL,NULL),(23,'AR15896547','AR20356459','bc2sv2_uc2sv2','2018-09-30 12:53:54',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,3.39331,3.2891,3.11052,3.04411,NULL,NULL,NULL,NULL,16,26,16,6,2,8,8,2,NULL,NULL,NULL),(24,'AR15896547','AR20356459','bc2lv1_uc2lv1','2018-09-30 12:54:29',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,13.7033,12.8019,14.8025,13.8957,NULL,NULL,NULL,NULL,31,29,1,3,9,0,1,10,NULL,NULL,NULL),(25,'AR15896547','AR20356459','rdv1','2018-09-30 12:55:01',0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(26,'AR15896547','AR20356459','rdv1_bc2lv1_uc2lv1','2018-09-30 12:55:36',111,99,12,90,111,99,12,90,3.51751,3.47412,2.89546,2.88567,NULL,NULL,NULL,NULL,16,16,16,16,5,5,5,5,NULL,NULL,NULL),(27,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-09-30 12:56:09',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(28,'AR15896547','AR20356459','rdv1_bc2lv2_uc2lv2','2018-10-03 18:30:15',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,0,0,0,0,2,2,1,1,NULL,NULL,NULL),(29,'AR25619753','AR26264556','rdv1_bc2lv2_uc2lv2','2018-10-16 12:30:09',447,386,61,136,447,386,61,136,3.09385,3.01255,0.151329,0.50028,NULL,NULL,NULL,NULL,31,31,1,1,7,9,3,1,NULL,NULL,NULL);
/*!40000 ALTER TABLE `tEyeResults` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tPatientOptionalData`
--

DROP TABLE IF EXISTS `tPatientOptionalData`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tPatientOptionalData` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `patientid` varchar(255) DEFAULT NULL,
  `date_insertion` datetime DEFAULT NULL,
  `state` varchar(255) DEFAULT NULL,
  `city` varchar(255) DEFAULT NULL,
  `mainactivity` varchar(20) DEFAULT NULL,
  `highestdegree` varchar(20) DEFAULT NULL,
  `physicalactivity` varchar(5) DEFAULT NULL,
  `nofspokenlang` tinyint(4) DEFAULT NULL,
  `agreesharedata` tinyint(4) DEFAULT NULL,
  `familyhistory` varchar(255) DEFAULT NULL,
  `patienthistory` varchar(255) DEFAULT NULL,
  `remedies` varchar(255) DEFAULT NULL,
  `diagnostic` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=46 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tPatientOptionalData`
--

LOCK TABLES `tPatientOptionalData` WRITE;
/*!40000 ALTER TABLE `tPatientOptionalData` DISABLE KEYS */;
INSERT INTO `tPatientOptionalData` VALUES (1,'AR15896547','2018-09-19 20:38:54','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(2,'AR559714155','2018-09-19 20:38:54','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(3,'AU55145dasdf','2018-09-19 20:38:54','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(4,'AR15896547','2018-09-19 20:44:01','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(5,'AR559714155','2018-09-19 20:44:01','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(6,'AU55145dasdf','2018-09-19 20:44:01','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(7,'AR15896547','2018-09-19 20:44:57','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(8,'AR559714155','2018-09-19 20:44:57','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(9,'AU55145dasdf','2018-09-19 20:44:57','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(10,'AR15896547','2018-09-19 20:48:26','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(11,'AR559714155','2018-09-19 20:48:26','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(12,'AU55145dasdf','2018-09-19 20:48:26','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(13,'AR15896547','2018-09-19 20:50:09','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(14,'AR559714155','2018-09-19 20:50:09','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(15,'AU55145dasdf','2018-09-19 20:50:09','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(16,'AR15896547','2018-09-19 20:51:22','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(17,'AR559714155','2018-09-19 20:51:22','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(18,'AU55145dasdf','2018-09-19 20:51:22','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(19,'AR15896547','2018-09-19 20:52:44','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(20,'AR559714155','2018-09-19 20:52:44','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(21,'AU55145dasdf','2018-09-19 20:52:44','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(22,'AR15896547','2018-09-19 20:54:30','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(23,'AR559714155','2018-09-19 20:54:30','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(24,'AU55145dasdf','2018-09-19 20:54:30','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(25,'AR15896547','2018-09-19 20:57:35','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(26,'AR559714155','2018-09-19 20:57:35','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(27,'AU55145dasdf','2018-09-19 20:57:35','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(28,'AR15896547','2018-09-19 20:58:38','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(29,'AR559714155','2018-09-19 20:58:38','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(30,'AU55145dasdf','2018-09-19 20:58:38','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(31,'AR15896547','2018-09-19 21:02:47','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(32,'AR559714155','2018-09-19 21:02:47','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(33,'AU55145dasdf','2018-09-19 21:02:47','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(34,'AR15896547','2018-09-19 21:26:32','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(35,'AR559714155','2018-09-19 21:26:32','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(36,'AU55145dasdf','2018-09-19 21:26:32','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(37,'AR15896547','2018-09-19 21:27:34','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(38,'AR559714155','2018-09-19 21:27:34','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(39,'AU55145dasdf','2018-09-19 21:27:34','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(40,'AR330125589','2018-09-20 23:06:32','Alguna','De algun lado',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(41,'AR330125589','2018-09-20 23:06:57','Alguna','De algun lado de por ahi',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(42,'AR665894123','2018-09-30 13:13:24','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(43,'AR559714155','2018-10-03 18:35:45','Pindonga','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(44,'AR25619753','2018-10-16 12:08:17','buenos aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(45,'SQ8','2018-10-17 19:26:59','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `tPatientOptionalData` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tPatientReqData`
--

DROP TABLE IF EXISTS `tPatientReqData`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tPatientReqData` (
  `keyid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `uid` varchar(255) DEFAULT NULL,
  `doctorid` varchar(255) DEFAULT NULL,
  `firstname` varchar(255) DEFAULT NULL,
  `lastname` varchar(255) DEFAULT NULL,
  `idtype` varchar(255) DEFAULT NULL,
  `sex` char(1) DEFAULT NULL,
  `birthcountry` varchar(2) DEFAULT NULL,
  `birthdate` date DEFAULT NULL,
  PRIMARY KEY (`keyid`)
) ENGINE=InnoDB AUTO_INCREMENT=46 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tPatientReqData`
--

LOCK TABLES `tPatientReqData` WRITE;
/*!40000 ALTER TABLE `tPatientReqData` DISABLE KEYS */;
INSERT INTO `tPatientReqData` VALUES (1,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(2,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(3,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(4,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(5,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(6,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(7,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(8,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(9,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(10,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(11,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(12,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(13,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(14,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(15,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(16,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(17,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(18,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(19,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(20,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(21,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(22,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(23,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(24,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(25,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(26,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(27,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(28,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(29,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(30,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(31,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(32,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(33,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(34,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(35,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(36,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(37,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(38,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(39,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(40,'AR330125589','AR30123697','Señor','Paciente','DNI','M','AR','1974-06-05'),(41,'AR330125589','AR30123697','Señor','Paciente','DNI','M','AR','1974-06-05'),(42,'AR665894123','AR30913307','Pablo','Gutierrez','DNI','M','AR','1968-06-05'),(43,'AR559714155','AR20356459','Pedrito','HeMan','Passport','M','AR','1963-04-06'),(44,'AR25619753','AR26264556','Bruno','Dalmau','DNI','M','AR','1977-01-21'),(45,'SQ8','BJvm00000000','ssfdgsdf','nsfgsdfgsdfg','DNI','M','SQ','1985-04-01');
/*!40000 ALTER TABLE `tPatientReqData` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-10-17 19:56:00
