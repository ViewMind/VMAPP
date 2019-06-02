library(memisc)
library(MASS)
library(lme4)
library(grid)
library(ggplot2)
library(plyr)
library(scales)
library(reshape2)
library(reshape)
library(healthcareai)
library(gdata)

rm(list=ls())  #Remove workspace

args = commandArgs(trailingOnly=TRUE)

if (length(args) != 3) {
   stop("Binding Script requires 3 and only 3 argurments", call.=FALSE)
}

setwd("./res")

#### NOTEBOOK ########

BC_AXIS<-read.csv(args[1])
head(BC_AXIS)
tail(BC_AXIS)
dim(BC_AXIS)
BC_AXIS<-BC_AXIS[,c(1:13,23)]
names(BC_AXIS) <- c("subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","score")
table(BC_AXIS$is_trial)
head(BC_AXIS)

BC<-BC_AXIS

UC_AXIS<-read.csv(args[2])
dim(UC_AXIS)
UC_AXIS<-UC_AXIS[,c(1:13,23)]

names(UC_AXIS) <- c("subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","score")
str(UC_AXIS)
UC<-UC_AXIS

table(BC$subj_id)
table(UC$subj_id)

BC$type<-NA
BC$type<-"BC"


BC$Condition<-"DCL"



UC$type<-NA
UC$type<-"UC"
head(UC)


UC$Condition<-"DCL"



bs<-rbind(BC,UC)
rm(BC)
rm(UC)


bs$dur<-as.numeric(bs$dur)
dim(bs)
idx<-which(bs$dur <50)
length(idx)
bs<-bs[-idx,]
#


bs$trial_id<-as.numeric(bs$trial_id)
idx<-which(bs$trial_id==0)
length(idx)
bs<-bs[-idx,] 

levels(bs$trial_id)
bs$type<-as.factor(bs$type)
bs$is_trial<-as.factor(bs$is_trial)



#eye = 0 IZQUIERDO
idx<-which(bs$eye=="0") #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
bs<-bs[-idx,]


idx<-which(bs$is_trial=="0")
length(idx)
b1s<-bs[-idx,]

table(bs$subj_id)

table(b1s$response)
table(b1s$type)
str(b1s)
table(b1s$trial_type)

idx<-which(bs$is_trial=="1")
length(idx)
#b<-b[-idx,]


###########################################################################################

head(bs)

as<-bs


head(as)
tail(as)
 
a3<-as


rm(as)
rm(b1s)
rm(bs)
rm(idx)
rm(BC_AXIS)
rm(UC_AXIS)

###################################################################################

m1 <- load_models("my_models.RDS")

###############################################################################################
a4 <- ddply(a3, .(Condition,subj_id,type), summarise,  mao = mean(sacc_ampl),
            mpupila = mean(pupila), mgaze = mean(gaze)
            , mnf = mean(nf),mscore = mean(score))

a4$Deterioro <-ifelse(a4$Condition== "CONTROL", "NO", "SI")

a4$Deterioro<-as.factor(a4$Deterioro)

head(a4)
a311 <- a4[,c(2,3,9)]

#a31 <- a2[,c(2,3,4,13)]

#a41 <- scale(a2[,4:7],center=TRUE)
a411 <- scale(a4[,4:8])

#a41 <- scale(a2[,5:12])

#a4 <- scale(a2[,3:6])

a6<-cbind(a311,a411)


m3<-predict(m1, a6, outcome_groups = 5)


binding_output <- paste0(               "bc_predicted_deterioration = ",m3$predicted_Deterioro[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_deterioration = ",m3$predicted_Deterioro[2], ";\n")
binding_output <- paste0(binding_output,"bc_predicted_group = ",m3$predicted_group[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_group = ",m3$predicted_group[2], ";\n")

fileConn<-file(args[3])
writeLines(binding_output, fileConn)
close(fileConn)
