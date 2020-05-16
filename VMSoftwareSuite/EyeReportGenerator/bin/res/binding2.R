library(memisc)
library(MASS)
library(lme4)
library(grid)
library(ggplot2)
library(plyr)
library(scales)
library(reshape2)
library(reshape)
library(gtable)
library(gdata)
library(data.table)


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

BC<-read.csv(args[1]) 

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






r<-rbind(BC,UC)
r$gaze<-as.numeric(r$gaze)

rm(BC)
rm(UC)




r$score<-as.numeric(r$score)




head(r)

dim(r)

idx<-which(r$dur <50)
length(idx)
r<-r[-idx,]

r$sacc_ampl<-as.numeric(r$sacc_ampl)
r$gaze<-as.numeric(r$gaze)
r$nf<-as.numeric(r$nf)
r$timeline<-as.numeric(r$timeline)
r$pupila<-as.numeric(r$pupila)
r$score<-as.numeric(r$score)


r$trial_id<-as.numeric(r$trial_id)
idx<-which(r$trial_id==0)
length(idx)
r<-r[-idx,] 


str(r)
#describe(b)
levels(r$trial_id)
r$type<-as.factor(r$type)
r$is_trial<-as.factor(r$is_trial)
r$eye<-as.factor(r$eye)


#eye = 0 IZQUIERDO
idx<-which(r$eye=="0") #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
r<-r[-idx,]


str(r)
idx<-which(r$timeline >4000)
length(idx)
#as<-as[-idx,]
idx<-which(r$timeline<"0")
length(idx)
#as3<-as3[-idx,]

dim(r)

as<-r

table(as$Condition)

as$Condition<-as.factor(as$Condition)


rm(r)






as_1<-as


#as$Condition1[as$Condition == "CONTROL"]<- "1"

head(as_1)  
dim(as_1)



as_1$age<-as.factor(as_1$age)

str(as_1)

table(as_1$trial_id)
table(as_1$age)

idx<-which(as_1$type=="UC")
length(idx)
as_1<-as_1[-idx,]

idx<-which(as_1$is_trial=="1")
length(idx)
as_1<-as_1[-idx,]
table(as_1$type)
str(as_1)
#as_1$gaze<-log(as_1$gaze)



as_1$score<-(as_1$score*100)/32


head(as_1)
tail(as_1)

table(as_1$Condition)




#
#install.packages("healthcareai")
library(healthcareai)
#packageVersion("healthcareai")


m1 <- load_models("binding2_model.RDS")

head(as_1)
a4 <- ddply(as_1, .(Condition,id), summarise,  mdur = mean(dur),msd = sd(dur),  mao = mean(sacc_ampl),
            mpupila = mean(pupila),sdpupila = sd(pupila), mgaze = mean(gaze)
            , mnf = mean(nf), sdnf = sd(nf),mscore = mean(score),sdao = sd(sacc_ampl),sdgaze = sd(gaze))


str(a4)
head(a4)
# a4$mao<-round(a4$mao,digits=2)
# a4$mpupila<-round(a4$mpupila,digits=2)
# a4$mgaze<-round(a4$mgaze,digits=2)
# a4$mnf<-round(a4$mnf,digits=2)
# a4$mscore<-round(a4$mscore,digits=2)








a2<-a4



# #######################################################
a2$Deterioro <-ifelse(a2$Condition== "CONTROL", "NO", "YES")

a2$Deterioro<-as.factor(a2$Deterioro)

#a2$mgaze<-log(a2$mgaze)
#a2$mgaze<-round(a2$mgaze,digits=2)

#library(keras)
library(dplyr)


head(a2)
tail(a2)
dim(a2)
#View(a2[,])
# ..$ id     : Factor w/ 68 levels "","test protocol_0002",..: 
#   ..$ mao    : num(0) 
# ..$ mpupila: num(0) 
# ..$ mnf    : num(0) 
# ..$ sdnf   : num(0) 
# ..$ mscore : num(0) 
# ..$ sdao   : num(0) 
a31 <- a2[,c(2,14)]

a41 <- (a2[,c(3,5,6,7,8,10,11,12,13)])


a5<-cbind(a31,a41)



m3<-predict(m1, a5, outcome_groups = 2)

m3

binding_output <- paste0(               "bc_predicted_deterioration = ",m3$predicted_Deterioro[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_deterioration = ",m3$predicted_Deterioro[2], ";\n")
binding_output <- paste0(binding_output,"bc_predicted_group = ",m3$predicted_group[1], ";\n")
binding_output <- paste0(binding_output,"uc_predicted_group = ",m3$predicted_group[2], ";\n")

fileConn<-file(args[3])
writeLines(binding_output, fileConn)
close(fileConn)
