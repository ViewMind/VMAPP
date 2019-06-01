

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

BC_AXIS<-read.csv("binding_bc_2_l_2_2019_04_01_20_34.csv")
#ofile <- ("BC_AXIS.rda")  
#save(BC_AXIS, file=ofile)
#load("BC_AXIS.rda")
head(BC_AXIS)
tail(BC_AXIS)
dim(BC_AXIS)
BC_AXIS<-BC_AXIS[,c(1:13,23)]
names(BC_AXIS) <- c("subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","score")
table(BC_AXIS$is_trial)
head(BC_AXIS)

BC<-BC_AXIS

UC_AXIS<-read.csv("binding_uc_2_l_2_2019_04_01_20_27.csv")
#ofile <- ("UC_AXIS.rda")  
#save(UC_AXIS, file=ofile)
#load("UC_AXIS.rda")
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










#b$pupila<-b$pupila/423




idx<-which(bs$is_trial=="1")
length(idx)
#b<-b[-idx,]


###########################################################################################

head(bs)

as<-bs


head(as)
tail(as)




 
a3<-as



#ofile<-("BINDING_for_deeplearning_TRIAL_AXIS_DCL.rda")
#save(a3, file=ofile)



rm(as)
rm(b1s)
rm(bs)
rm(idx)
rm(BC_AXIS)
rm(UC_AXIS)

###################################################################################

# load("BINDING_for_deeplearning_TRIAL_AXIS_DCL.rda")
# 
# a1<-axis
# rm(axis)
# 
# 
# load("BINDING_for_deeplearning_TRIAL_STRATHCLYDE-2.rda")  #LOSS 0.63, ACC 0.68
# 
# as$Condition<-"CONTROL"
# 
# str(as)
# as$Condition<-as.factor(as$Condition)
# str(a1)
# 
# 
# 
# 
# aggregate(a1$score,by=list(a1$type,a1$size),mean)
# aggregate(a1$score,by=list(a1$type,a1$size),sd)
# 
# str(a1)
# # #as$gaze<-log10(as$gaze)
# # t.test(a1$gaze ~ a1$size)
# # t.test(a1$pupila ~ a1$size)
# # t.test(a1$nf ~ a1$size)
# # t.test(a1$sacc_ampl ~ a1$size)
# # t.test(a1$score ~ a1$size)
# 
# 
# 
# 
# table(as$gaze)
# 
# idx<-which(as$gaze>6941)
# length(idx)
# # idx<-which(as$gaze>6941)
# # length(idx)
# #as<-as[-idx,]
# 
# dim(as)
# 
# 
# head(a1)
# table(a1$id)
# table(a1$subj_id)
# 
# #a1<-a
# head(a1)
# tail(a1)
# 
# str(a1)
# table(a1$id,a1$type)
# 
# table(a1$gaze)
# 
# 
# 
# 
# 
# 
# 
# 
# 
# idx<-which(as$size=="S")
# length(idx)
# as<-as[-idx,]
# as<-as[,c(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18)]
# 
# 
# a1<-a1[,c(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17)]
# head(as)
# head(a1)
# 
# 
# 
# a1s1<-rbind(a1,as)
# rm(a1)
# 
# a1<-a1s1
# 
# 
# summary(a1)
# a1$gaze<-log10(a1$gaze)
# 
# table(a1$gaze)
# 
# 
# table(a1$is_trial)
# idx<-which(a1$is_trial=="1")
# length(idx)
# a1<-a1[-idx,]
# 
# idx<-which(a1$gaze>2000)
# length(idx)
# 
# 
# table(a1$is_trial)
# # 
# 
# # 
# 
# library(data.table)
# 
# head(a1)
# tail(a1)
# str(a1)
# a1$eye<-as.factor(a1$eye)
# table(a1$eye)
# 
# 
# str(a1)
# 
# table(a1$trial_id)
# 
# table(a1$type)
# idx<-which(a1$type== "UC")
# length(idx)
# # a1<-a1[-idx,]
# summary(a1)
# 
# 
# table(a1$Condition)
# idx<-which(a1$Condition=="DEPRESSION")
# a1<-a1[-idx,]
# 
# table(a1$subj_id)
# 
# 
# idx<-which(a1$subj_id=="2019_03_28_00_18_12" | a1$subj_id=="2019_03_28_00_20_35")
# length(idx)
# a1<-a1[-idx,]
# 
# a2 <- ddply(a1, .(Condition,subj_id,type), summarise,  mao = mean(sacc_ampl),
#             mpupila = mean(pupila), mgaze = mean(gaze)
#             , mnf = mean(nf), mscore = mean(score))
# 
# 
# View(a2[1:160,])
# str(a2)
# 
# 
# 
# 
# summary(a2)
# 
# 
# #https://cran.r-project.org/web/packages/healthcareai/healthcareai.pdf


#
#install.packages("healthcareai")
library(healthcareai)
#packageVersion("healthcareai")

# 
# #######################################################
# a2$Deterioro <-ifelse(a2$Condition== "CONTROL", "NO", "SI")
# 
# a2$Deterioro<-as.factor(a2$Deterioro)
# 
# head(a2)
# dim(a2)
# a31 <- a2[,c(2,3,9)]
# #a31 <- a2[,c(2,3,7)]
# 
# #a31 <- a2[,c(2,3,4,13)]
# 
# a41 <- scale(a2[,4:8],center=TRUE)
# #a41 <- scale(a2[,4:7],center=TRUE)
# 
# 
# a5<-cbind(a31,a41)
# 
# summary(a5)
# str(a5)
# 
# m1 <- machine_learn(a5, subj_id, outcome = Deterioro)
# 
# #plot(m1)
# interpret(m1)
# 
# summary(m1)
# 
# 
# evaluate(m1, all_models = TRUE)
# 
# m2 <- predict(m1, outcome_groups = 5)
# 
# evaluate(m2)
# View(m2[1:180,])
# 
# #write.table(m2, "C:/Users/usuario/Dropbox/Machine learning/BINDING_gazepoint/m2_sin_SCORE-ni_nf.txt") 
# 
# get_thresholds(m2, optimize = "cost", cost_fn = 5) %>%
#   filter(optimal)
# 
# get_thresholds(m2, optimize = "acc") %>%
#   plot()
# 
# get_thresholds(m1) %>%plot()
# 
# class_preds <- predict(m1,  outcome_groups = 5)
# table(actual = class_preds$Deterioro, predicted = class_preds$predicted_group)
# 
# 
# class_preds [1:20,]
# class_preds[20:40,]
# 
# get_cutoffs(class_preds)
# 
# plot(class_preds)
# 
# 
# 
# # is greater than the threshold
# 
# class_predictions <-predict(m1) %>%
#   mutate(predicted_class_Deterioro = case_when
#          (predicted_Deterioro > optimal_threshold ~ "Y",
#            predicted_Deterioro <= optimal_threshold ~ "N"))
# 
# #m2 <- predict(m1, newdata= a2[1:40,])
# m2
# evaluate(m2)
# plot(m2)
# m2[1:20,]
# m2[25:40,]
# 
# 
# get_variable_importance(m1) %>%
#   plot()
# 
# explore(m1) %>% 
#   plot()
# 
# #save_models(m1, file = "my_models.RDS")

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

m3

write.table(m3, "binding_output.txt") 
