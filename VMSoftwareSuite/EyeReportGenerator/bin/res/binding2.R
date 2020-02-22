
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
   stop(paste0("Binding Script requires 3 and only 3 argurments. Found: ",length(args)), call.=FALSE)
   print(args[1])
   print(args[2])
   print(args[3])
   print(args[4])   
}

setwd("./res")

BC_AXIS<-read.csv(args[1]) 

head(BC_AXIS)
dim(BC_AXIS)
BC_AXIS<-BC_AXIS[,c(1:13,26,27)]
names(BC_AXIS) <- c("subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")

BC<-BC_AXIS

UC_AXIS<-read.csv(args[2])

dim(UC_AXIS)

UC_AXIS<-UC_AXIS[,c(1:13,26,27)]

names(UC_AXIS) <- c("subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")
UC<-UC_AXIS
head(UC_AXIS)


BC$type<-NA
BC$type<-"BC"


BC$Condition<-"AD"



UC$type<-NA
UC$type<-"UC"
head(UC)


UC$Condition<-"AD"



bs<-rbind(BC,UC)
rm(BC)
rm(UC)


bs$dur<-as.numeric(bs$dur)
dim(bs)
idx<-which(bs$dur <50)
length(idx)
#bs<-bs[-idx,]
#


bs$trial_id<-as.numeric(bs$trial_id)
idx<-which(bs$trial_id==0)
length(idx)
bs<-bs[-idx,] 

bs$type<-as.factor(bs$type)
bs$is_trial<-as.factor(bs$is_trial)



#eye = 0 IZQUIERDO
idx<-which(bs$eye=="0") #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
bs<-bs[-idx,]






#b$pupila<-b$pupila/423




idx<-which(bs$is_trial=="1")
length(idx)
bs<-bs[-idx,]



idx<-which(bs$type=="UC")
length(idx)
bs<-bs[-idx,]
###########################################################################################




head(bs)

as<-bs


head(as)
tail(as)





a3<-as

tail(a3)




rm(as)
#rm(b1s)
rm(bs)
rm(idx)
rm(BC_AXIS)
rm(UC_AXIS)

###################################################################################


#
#install.packages("healthcareai")
library(healthcareai)
#packageVersion("healthcareai")


m1 <- load_models("binding2_model.RDS")

###############################################################################################
a4 <- ddply(a3, .(Condition,subj_id), summarise,  mao = mean(sacc_ampl),
            mpupila = mean(pupila), mgaze = mean(gaze)
            , mnf = mean(nf),mscore = mean(score))

#a4$mgaze<-log(a4$mgaze)

a4$mao<-round(a4$mao,digits=1)
a4$mpupila<-round(a4$mpupila,digits=1)
a4$mgaze<-round(a4$mgaze,digits=1)
a4$mnf<-round(a4$mnf,digits=1)


a4$Deterioro <-ifelse(a4$Condition== "CONTROL", "NO", "SI")

a4$Deterioro<-as.factor(a4$Deterioro)

head(a4)
dim(a4)
a311 <- a4[,c(2,8)]

a411 <- (a4[,c(3,4,5,6,7)])



a6<-cbind(a311,a411)

str(a6)
head(a6)

m3<-predict(m1, a6, outcome_groups = 5)

m3

binding_output <- paste0(               "bc_predicted_deterioration = ",m3$predicted_Deterioro[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_deterioration = ",m3$predicted_Deterioro[2], ";\n")
binding_output <- paste0(binding_output,"bc_predicted_group = ",m3$predicted_group[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_group = ",m3$predicted_group[2], ";\n")

fileConn<-file(args[3])
writeLines(binding_output, fileConn)
close(fileConn)
