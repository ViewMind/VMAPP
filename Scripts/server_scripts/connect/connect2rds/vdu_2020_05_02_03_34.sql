-- MySQL dump 10.16  Distrib 10.2.10-MariaDB, for Linux (x86_64)
--
-- Host: localhost    Database: viewmind_dashboard_users
-- ------------------------------------------------------
-- Server version	10.2.10-MariaDB

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
-- Current Database: `viewmind_dashboard_users`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `viewmind_dashboard_users` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `viewmind_dashboard_users`;

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
  PRIMARY KEY (`keyid`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tUsers`
--

LOCK TABLES `tUsers` WRITE;
/*!40000 ALTER TABLE `tUsers` DISABLE KEYS */;
INSERT INTO `tUsers` VALUES ('Ariel Eduardo','Arelovich','$2y$10$RTQaeGgmBw4XUcoN2thGoex6RcsuKINZ5R6TDttiPgrrfTX1L8mBe','ariela','aarelovich@gmail.com',1,3),('Matias','Shulz','$2y$10$lK/v/oRJgdTdTGduC.wtkOo7GXVH0pGdp2ppF8Gzlhcfbn4LR1UGi','mshulz','matias.shulz@viewmind.com.ar',1,4),('Laura','Vega','$2y$10$e081hAm0CzGI12y2UZwPKOc1mkgtKTJ3raNm2A8Kk97/ELyYinzoO','laura.vega','lvega@axxionconsultora.com.ar',1,5),('Gerardo','Fernandez','$2y$10$.NqS9U/P4qRvnrcIvF1/BuCnH9e7kriA78gekaakUm8pUZ0VOLkRy','gfernandez','gerardofernandez480@gmail.com',1,6),('David','Orozco','$2y$10$IfNLb9ncjLM5WndEcfewVOBO1zrWQdCMnN9wcF.AsGqQyd0AwUuHS','dorozco','ddaaorozco@gmail.com',1,7),('Mario','Parra','$2y$10$fCObQNUXbd2zky1CFnojWeMJ0ZAo0oyIQOz9nr04GAhL1VYCPaT7G','mario.parra','marioparra05@gmail.com',1,8),('Gustavo','Abdala','$2y$10$edl1DDXvpsYMrLnX0jFkqucMFo8r478bZ251B.9tZQK.0NXnjyeB2','gustavo.abdala','gustavo.abdala@axxionconsultora.com',1,10),('Mark','Edwards','$2y$10$UZPG5VlVPd/9wgDJfWrJxOEAs6Qler7zc4rFkROu3B.1yaqnmowwi','mark.edwards','mark.edwards@viewmind.ai',1,11);
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

-- Dump completed on 2020-05-02  6:34:31
