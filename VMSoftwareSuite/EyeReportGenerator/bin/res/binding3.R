library(healthcareai)

library(memisc)
library(MASS)
library(lme4)
library(grid)
library(ggplot2)
library(plyr)
library(scales)
library(reshape2)
library(reshape)

library(gdata)

rm(list=ls())  #Remove workspace

#### NOTEBOOK ########
args = commandArgs(trailingOnly=TRUE)

if (length(args) != 3) {
   print(args[1])
   print(args[2])
   print(args[3])
   print(args[4])
   stop(paste0("Binding Script requires 3 and only 3 argurments. Found: ",length(args)), call.=FALSE)
}

setwd("./res")

BC_s<-read.csv(args[1]) 

head(BC)
tail(BC)

dim(BC)
BC<-BC[,c(1:15,28,29)]
names(BC) <- c("id","age","subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")

table(BC$is_trial)
head(BC)


UC<-read.csv(args[2])

dim(UC)
head(UC)
UC<-UC[,c(1:15,28,29)]

names(UC) <- c("id","age","subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")
str(UC)

table(BC$subj_id)
table(UC$subj_id)

BC$type<-NA
BC$type<-"BC"


BC$Condition<-"CONTROL"



UC$type<-NA
UC$type<-"UC"

UC$Condition<-"CONTROL"






bs<-rbind(BC,UC)
str(bs)
bs$gaze<-as.numeric(bs$gaze)
#bs$gaze<-log(bs$gaze)
head(bs)
#bs$gaze<-exp(bs$gaze)

rm(BC)
rm(UC)




bs$score<-as.numeric(bs$score)



bs$dur<-as.numeric(bs$dur)
dim(bs)
idx<-which(bs$dur <50)
length(idx)
bs<-bs[-idx,]
# idx<-which(b$dur >=700)
# length(idx)
# b<-b[-idx,]


bs$trial_id<-as.numeric(bs$trial_id)
idx<-which(bs$trial_id==0)
length(idx)
bs<-bs[-idx,] 

bs$type<-as.factor(bs$type)
bs$is_trial<-as.factor(bs$is_trial)


bs$is_trial_1[bs$is_trial == "0"] <- "ENCODING"
bs$is_trial_1[bs$is_trial == "1"] <- "RETRIEVAL"


levels(bs$response)[levels(bs$response)==""] <- "same"



levels(bs$response)[levels(bs$response)=="S"] <- "same"



levels(bs$response)[levels(bs$response)=="S"] <- "same"
levels(bs$response)[levels(bs$response)=="D"] <- "different"

dim(bs)

table(bs$is_trial)

table(bs$eye)
#eye = 0 IZQUIERDO
idx<-which(bs$eye=="0") #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
bs<-bs[-idx,]


idx<-which(bs$timeline >4000)
length(idx)
#bs<-bs[-idx,]
idx<-which(bs$timeline<0)
length(idx)
#as3<-as3[-idx,]

bs$is_trial_1<-as.factor(bs$is_trial_1)

bs$gaze<-as.numeric(bs$gaze)
bs$Condition<-as.factor(bs$Condition)
bs$score<-as.numeric(bs$score)

as_1<-bs



as_1$nf<-as.numeric(as_1$nf)


str(as_1)

table(as_1$trial_id)

idx<-which(as_1$type=="UC")
length(idx)
as_1<-as_1[-idx,]

idx<-which(as_1$is_trial=="1")
length(idx)
as_1<-as_1[-idx,]

as_1$score<-(as_1$score*100)/32

as_1$Condition1[as_1$Condition=="CONTROL"]<-"CONTROL"


a4 <- ddply(as_1, .(Condition1,id), summarise,  mdur = mean(dur),msd = sd(dur),  mao = mean(sacc_ampl),
            mpupila = mean(pupila),sdpupila = sd(pupila), mgaze = mean(gaze)
            , mnf = mean(nf), sdnf = sd(nf),mscore = mean(score),sdao = sd(sacc_ampl),sdgaze = sd(gaze))




a4$Condition1<-as.factor(a4$Condition1)



a2<-a4


# #######################################################
a2$Deterioro <-ifelse(a2$Condition1== "CONTROL", "NO", "SI")

a2$Deterioro<-as.factor(a2$Deterioro)

#library(keras)
library(dplyr)


head(a2)
tail(a2)
dim(a2)

a31 <- a2[,c(2,14)]

a41 <- (a2[,c(3,7,8,10,11,12,13)])

a5<-cbind(a31,a41)

m1 <- load_models("binding3_model.RDS")

m4<-predict(m1, a5, outcome_groups = 2)

m4

binding_output <- paste0(               "bc_predicted_deterioration = ",m4$predicted_Deterioro[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_deterioration = ",m4$predicted_Deterioro[2], ";\n")
binding_output <- paste0(binding_output,"bc_predicted_group = ",m4$predicted_group[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_group = ",m4$predicted_group[2], ";\n")

fileConn<-file(args[3])
writeLines(binding_output, fileConn)
close(fileConn)

