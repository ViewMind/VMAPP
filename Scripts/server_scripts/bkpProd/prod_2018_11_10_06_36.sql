-- MySQL dump 10.14  Distrib 5.5.60-MariaDB, for Linux (x86_64)
--
-- Host: viewminddb.cdqlb2rkfdvi.us-east-2.rds.amazonaws.com    Database: viewmind_data
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
-- Current Database: `viewmind_data`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `viewmind_data` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `viewmind_data`;

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
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tDoctors`
--

LOCK TABLES `tDoctors` WRITE;
/*!40000 ALTER TABLE `tDoctors` DISABLE KEYS */;
INSERT INTO `tDoctors` VALUES (1,'AR24136069','David','Orozco','DNI','AR','Buenos Aires','Bahia Blanca','Axis Neurociencias','','','San Martin 752'),(2,'AR26264556','David','Orozco','DNI','AR','Buenos Aires','Bahia Blanca','Axis Neurociencias','291154411087','schumachermarcela@gmail.com','San Martin 752'),(3,'AR21483418','Gustavo ','Echevarria','DNI','AR','Buenos Aires','Bahia Blanca','Axis Neurociencias','','','San Martin 752'),(4,'AR23130517','Gustavo','Sgrilli','DNI','AR','Buenos Aires','Bahia Blanca','Axis Neurociencias','','','San Martin 752'),(5,'AR20356459','David','Orozco','DNI','AR','Buenos Aires','Bahía Blanca','Axis','+3652147895633','david.orozco@viewmind.com.ar','Calle Mitre 456'),(6,'AR30913307','Ariel','Arelovich','DNI','AR','Buenos Aires','Bahia Blanca','UnS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(7,'DK30913307','Ariel','Arelovich','Identity Card','DK','Buenos Aires','Bahia Blanca','UNS','+549291154221229','aarelovich@gmail.com','Bravard 417'),(17,'SQvm00000000','Test','Doctor','DNI','SQ','','','','','',''),(19,'SQvm00000000','Test','Doctor','DNI','SQ','','Sometime','Somewhere','','','');
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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tEyeResults`
--

LOCK TABLES `tEyeResults` WRITE;
/*!40000 ALTER TABLE `tEyeResults` DISABLE KEYS */;
INSERT INTO `tEyeResults` VALUES (1,'AR4611804','AR24136069','rdv_1_bcv_1_ucv_1','2018-08-28 12:56:28',514,358,156,98,514,358,156,98,2.81841,2.83417,2.74356,2.71587,NULL,NULL,NULL,NULL,12,16,20,16,6,6,4,4,NULL,NULL,NULL),(2,'AR1432860','AR24136069','rdv_1_bcv_1_ucv_1','2018-08-28 13:57:38',262,210,52,139,262,210,52,139,2.68195,2.61526,2.51761,2.5351,NULL,NULL,NULL,NULL,22,21,10,11,3,6,7,4,NULL,NULL,NULL),(3,'AR26264556','AR24136069','rdv_1_bc2lv1_uc2lv1','2018-08-30 11:20:42',111,99,12,90,111,99,12,90,3.51751,3.47412,2.89546,2.88567,NULL,NULL,NULL,NULL,16,16,16,16,5,5,5,5,NULL,NULL,NULL),(4,'AR6071052','AR24136069','rdv_1_bc2lv1_uc2lv1','2018-08-30 12:36:33',577,485,92,107,577,485,92,107,2.90052,2.89809,2.76243,2.74435,NULL,NULL,NULL,NULL,14,24,18,8,5,6,5,4,NULL,NULL,NULL),(5,'AR18473833','AR24136069','rdv_1_bc2lv1_uc2lv1','2018-08-30 13:07:39',226,198,28,162,226,198,28,162,3.76024,3.74159,3.79753,3.7566,NULL,NULL,NULL,NULL,25,31,7,1,5,9,5,1,NULL,NULL,NULL),(6,'AR16250744','AR24136069','rdv_1_bc2lv1_uc2lv1','2018-08-30 16:15:12',438,284,154,97,438,284,154,97,4.51598,4.57849,4.03682,4.1367,NULL,NULL,NULL,NULL,16,25,16,7,5,9,5,1,NULL,NULL,NULL),(7,'CO14533965','AR24136069','rdv1_bc2lv2_uc2lv2','2018-09-04 12:16:02',199,199,0,191,199,199,0,191,3.40928,3.3063,3.45149,3.32498,NULL,NULL,NULL,NULL,26,28,6,4,4,9,6,1,NULL,NULL,NULL),(8,'AR10388141','AR23130517','rdv1_bc2lv2_uc2lv2','2018-09-04 12:45:10',277,241,36,171,277,241,36,171,1.77526,1.76862,1.63302,1.63959,NULL,NULL,NULL,NULL,21,29,11,3,4,8,6,2,NULL,NULL,NULL),(9,'AR4383865','AR21483418','rdv1_bc2lv2_uc2lv2','2018-09-04 14:43:30',0,0,0,0,0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL,12,4,20,28,4,1,6,9,NULL,NULL,NULL),(10,'AR16739228','AR24136069','rdv1_bc2lv2_uc2lv2','2018-09-04 15:02:21',132,127,5,118,132,127,5,118,3.16058,2.99519,3.20198,3.11545,NULL,NULL,NULL,NULL,28,31,4,1,5,10,5,0,NULL,NULL,NULL),(11,'AR1439746','AR24136069','rdv1_bc2lv2_uc2lv2','2018-09-04 15:24:57',0,0,0,0,0,0,0,0,3.84375,0,4.16523,0,NULL,NULL,NULL,NULL,15,14,17,18,2,3,8,7,NULL,NULL,NULL),(12,'AR14453504','AR24136069','rdv1_bc2lv2_uc2lv2','2018-09-04 15:50:10',259,223,36,166,259,223,36,166,2.57976,2.49796,2.8704,2.75887,NULL,NULL,NULL,NULL,24,29,8,3,4,8,6,2,NULL,NULL,NULL),(13,'AR5457584','AR24136069','bc2lv2_uc2lv2','2018-09-27 12:45:03',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,2.01707,1.99229,2.02863,1.9883,NULL,NULL,NULL,NULL,19,22,13,10,4,6,6,4,NULL,NULL,NULL),(14,'AR5457584','AR24136069','rdv1_bc2lv2_uc2lv2','2018-09-27 12:59:54',639,323,316,85,0,0,0,0,2.01707,1.99229,2.02863,1.9883,NULL,NULL,NULL,NULL,19,22,13,10,4,6,6,4,NULL,NULL,NULL),(15,'AR5479636','AR21483418','bc2lv2_uc2lv2','2018-09-27 13:29:59',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,3.22224,0,2.4673,0,NULL,NULL,NULL,NULL,17,15,15,17,4,8,6,2,NULL,NULL,NULL),(16,'AR5474724','AR21483418','rdv1_bc2lv2_uc2lv2','2018-09-27 13:57:48',0,0,0,0,0,0,0,0,2.81764,2.87623,2.57561,2.67712,NULL,NULL,NULL,NULL,18,24,14,8,4,6,6,4,NULL,NULL,NULL),(17,'AR5460059','AR21483418','bc2lv2_uc2lv2','2018-09-27 14:35:14',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,1.8774,0,2.20244,0,NULL,NULL,NULL,NULL,14,24,18,8,4,5,6,5,NULL,NULL,NULL),(18,'AR17594491','AR24136069','rdv1_bc2lv2_uc2lv2','2018-10-02 12:30:56',0,0,0,0,259,178,81,143,0,0,0,0,NULL,NULL,NULL,NULL,25,19,7,13,6,8,4,2,NULL,NULL,NULL),(19,'AR8188622','AR26264556','rdv1_bc2lv2_uc2lv2','2018-10-02 13:05:38',209,207,2,174,0,0,0,0,0,0,3.45452,3.37476,NULL,NULL,NULL,NULL,22,29,10,3,4,6,6,4,NULL,NULL,NULL),(20,'SQ000000','SQvm00000000','rdv1_bc2lv1_uc2lv1','2018-10-06 11:02:54',40,38,2,36,40,38,2,36,3.57482,3.5543,2.97182,2.93884,NULL,NULL,NULL,NULL,16,16,16,16,5,5,5,5,NULL,NULL,NULL);
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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tPatientOptionalData`
--

LOCK TABLES `tPatientOptionalData` WRITE;
/*!40000 ALTER TABLE `tPatientOptionalData` DISABLE KEYS */;
INSERT INTO `tPatientOptionalData` VALUES (1,'AR1432860','2018-08-28 13:17:34','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(2,'AR6071052','2018-08-30 12:20:06','BS AS','BAHIA BLANCA',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(3,'AR18473833','2018-08-30 12:47:54','BS AS','BAHIA BLANCA',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(4,'AR16250744','2018-08-30 15:49:25','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(5,'CO14533965','2018-09-04 11:58:33','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(6,'AR10388141','2018-09-04 12:26:00','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(7,'AR10388141','2018-09-04 12:28:30','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(8,'AR1439746','2018-09-04 14:06:56','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(9,'AR4383865','2018-09-04 14:12:14','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(10,'AR16739228','2018-09-04 14:48:31','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(11,'AR14453504','2018-09-04 15:30:58','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(12,'AR15896547','2018-09-17 20:03:25','Buenos Aires','Bahía Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(13,'AU55145dasdf','2018-09-17 20:03:25','Alguna','Que se yo',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(14,'AR5457584','2018-09-27 12:24:52','Bs As','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(15,'AR5479636','2018-09-27 13:03:49','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(16,'AR5460059','2018-09-27 14:15:15','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(17,'AR17594491','2018-10-02 12:13:48','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(18,'AR8188622','2018-10-02 12:48:15','Buenos Aires','Bahia Blanca',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(19,'SQ000000','2018-10-06 10:41:54','','',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(20,'SQ000000','2018-10-06 10:42:36','','Somewhere',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
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
) ENGINE=InnoDB AUTO_INCREMENT=25 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tPatientReqData`
--

LOCK TABLES `tPatientReqData` WRITE;
/*!40000 ALTER TABLE `tPatientReqData` DISABLE KEYS */;
INSERT INTO `tPatientReqData` VALUES (1,'AR1432860','AR24136069','Isabel Margarita','Altolaguirre','DNI','F','AR','1932-05-07'),(2,'AR6071052','AR24136069','Monica','Graff','DNI','F','AR','1950-12-25'),(3,'AR18473833','AR24136069','ALICIA','BARGA','DNI','F','AR','1967-05-17'),(4,'AR16250744','AR24136069','Stella Maris','Haag','DNI','F','AR','1963-06-06'),(5,'CO14533965','AR24136069','Nancy Margarita','Cecci','DNI','F','CO','1961-05-02'),(6,'AR10388141','AR24136069','Ruben Mario','Alvarez','DNI','M','AR','1952-09-17'),(8,'AR1439746','AR24136069','Ana','Cseh','DNI','F','AR','1933-08-04'),(9,'AR4383865','AR21483418','Nilda Esther','Streitenberger','DNI','F','AR','1939-05-13'),(10,'AR16739228','AR24136069','Beatriz ','Rando','DNI','F','AR','1964-02-06'),(11,'AR14453504','AR24136069','Silvia','Gomez Llanos','DNI','F','AR','1960-10-07'),(12,'AR15896547','AR20356459','Raul','Ramirez','DNI','M','AR','1987-09-14'),(13,'AU55145dasdf','AR20356459','Juana','De Los Palotes','Identity Card','F','AU','1932-05-14'),(18,'AR5457584','AR24136069','Heber Jesus','Mendez','DNI','M','AR','1933-12-24'),(19,'AR5479636','AR21483418','Juan Manuel','Julian','DNI','M','AR','1939-03-30'),(20,'AR5460059','AR21483418','Mariano','Romero','DNI','M','AR','1934-07-18'),(21,'AR17594491','AR24136069','Laura','Sanchez Bajo','DNI','F','AR','1965-07-06'),(22,'AR8188622','AR26264556','Eduardo','Linhardo','DNI','M','AR','1950-10-26'),(23,'SQ000000','SQvm00000000','Fictional','Patient','DNI','F','SQ','1985-01-01'),(24,'SQ000000','SQvm00000000','Fictional','Patient','DNI','F','SQ','1985-01-01');
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

-- Dump completed on 2018-11-10  9:36:22
