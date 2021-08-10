library(memisc)
library(MASS)
library(lme4)
library(grid)
library(ggplot2)
library(plyr)
library(dplyr)
library(jsonlite)
library(scales)
library(reshape2)
library(reshape)
library(gtable)
library(gdata)
library(data.table)


rm(list=ls())  #Remove workspace

args = commandArgs(trailingOnly=TRUE)

## Argument 1: BC CSV File
## Argument 2: UC CSV File
## Argument 3: Model file location
## Argument 4: Output File. 

if (length(args) != 4) {
   stop(paste0("Binding Script requires 4 and only 4 argurments. Found: ",length(args)), call.=FALSE)
   print(args[1])  # BC
   print(args[2])  # UC
   print(args[3])  # MODEL FILE
   print(args[4])  # OUTPUT_FILE 
}

bc_file<-args[1];
uc_file<-args[2];
model_file<-args[3];
output_file<-args[4];

BC<-read.csv(bc_file)

head(BC)
tail(BC)

dim(BC)
#BC<-BC[,c(1:13,26,27)]   #para ANT
#names(BC) <- c("id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")

BC<-BC[,c(1:15,28,29)]
names(BC) <- c("id","age","subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")

table(BC$is_trial)
head(BC)


#UC<-read.csv("binding_uc_2_l_2_2019_03_22_09_39.csv")

UC<-read.csv(uc_file)

#UC<-read.csv("binding_uc_2_l_2_2019_12_02_14_47.csv")  #ANT
#UC<-read.csv("binding_uc_2_l_2_2020_08_04_09_52.csv")  #AXIS
#UC<-read.csv("130MDbinding_uc_2_l_2_2020_08_04_09_52.csv")  #AXIS

dim(UC)
head(UC)

#UC<-UC[,c(1:13,26,27)]
#names(UC) <- c("id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")

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

r = filter(r, eye != "0" )


#eye = 0 IZQUIERDO
idx<-which(r$eye=="0") #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
#r<-r[-idx,]


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




head(as)

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


m1 <- load_models(model_file)

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

PRESERVACION_PARAHIPOCAMPAL<-m3   # PARAHIPOCAMPAL FUNCTIONING == AI
# Intervalo de Referencia =  67 - 99.   Verde 67-99.  Amarillo 55 - 66.   Rojo 10 - 54



head(as)
tail(as)

as$score<-(as$score*100)/32

a41 <- ddply(as, .(Condition,id,type), summarise,  mdur = mean(dur),msd = sd(dur),  mao = mean(sacc_ampl),
             mpupila = mean(pupila),sdpupila = sd(pupila), mgaze = mean(gaze)
             , mnf = mean(nf), sdnf = sd(nf),mscore = mean(score),sdao = sd(sacc_ampl),sdgaze = sd(gaze))

unique(as$score,as$type)
#which(as)
head(a41)

BUSQUEDA_VISUAL<-a41$mao[a41$type=="BC"] #<- VISUAL SEARCH == SUBCORTICAL -> BASAL GANGLIA, SUPERIOR COLICULLUS.
# Intervalo de Referencia =  1 - 3.5.   Verde 1 - 3.5.  Amarillo 3.6 - 7.   Rojo mayor a 7.1

DURACION_MIRADA<-a41$mgaze[a41$type=="BC"] #  GAZE  == CORTICAL  ->lateral parietal cortex, the medial prefrontal cortex (mPFC) and the anterior superior temporal sulcus (aSTS) 
# Intervalo de Referencia =  800 - 1900.   Verde 800 - 1900.  Amarillo 799 - 400.   Rojo 399 - 1



Integrative_memory_performance<-a41$mscore[a41$type=="BC"]  #Integrative memory performance  == score BC  -> corteza enthorinal, corteza perirhinal
# Intervalo de Referencia =  68 - 100.   Verde 68 - 100.  Amarillo 59 - 67.   Rojo 40 - 58


Asociative_memory_performance<-a41$mscore[a41$type=="UC"]  #Asociative memory performance  == score UC -> Hipocampo
# Intervalo de Referencia =  67 - 100.   Verde 67 - 100.  Amarillo 66 - 58.   Rojo 40 - 57

idx<-which(as$is_trial=="0")
length(idx)
as_REC<-as[-idx,]

a42MEAN_REC <- ddply(as_REC, .(Condition), summarise,  mdur = mean(dur),msd = sd(dur),  mao = mean(sacc_ampl),sdao = sd(sacc_ampl),
                     mpupila = mean(pupila),sdpupila = sd(pupila), mgaze = mean(gaze),sdgaze = sd(gaze)
                     , mnf = mean(nf), sdnf = sd(nf),mscore = mean(score),sdscore = sd(score))



executive_Functions <- a42MEAN_REC$mnf #executive Functions == Number of fixations EN LA RECOGNITION
# Intervalo de Referencia =  1 - 9.   Verde 1 - 9.  Amarillo 10 - 12.   Rojo 13 o más

PRESERVACION_PARAHIPOCAMPAL  #0.994
BUSQUEDA_VISUAL #2.67
DURACION_MIRADA  #1882
Integrative_memory_performance  #100
Asociative_memory_performance  #96
executive_Functions #8.6

#prueba<-c(PRESERVACION_PARAHIPOCAMPAL,BUSQUEDA_VISUAL,DURACION_MIRADA,Integrative_memory_performance,Asociative_memory_performance,executive_Functions)
#write.table(m3, "C:/Users/gerar/Dropbox/Machine learning/BINDING_gazepoint/PRUEBA_COLO/MachineLearning_3T2T_Reading_16_05_2020/sujeto2T.txt") #acá setea el directorio de tu maquina



as$is_trial_1[as$is_trial == "0"] <- "ENCODING"
as$is_trial_1[as$is_trial == "1"] <- "RETRIEVAL"


T3 <- ddply(as, .(Condition,type,is_trial_1), summarise,  mean_saccade = mean(sacc_ampl),SEsaccade = sd(sacc_ampl)/sqrt(length(sacc_ampl))
            , mean_gazing= mean(gaze),SEgazing= sd(gaze)/sqrt(length(gaze)), mean_duration = mean(dur),SEduration = sd(dur)/sqrt(length(dur))
            , mnf = mean(nf), SEnf = sd(nf)/sqrt(length(nf)))


T3

write_json(x = list(
      visual_search         = unbox(BUSQUEDA_VISUAL),
      gaze_dur              = unbox(DURACION_MIRADA),
      int_mem_perf          = unbox(Integrative_memory_performance),
      assoc_mem_perf        = unbox(Asociative_memory_performance),
      exec_func             = unbox(executive_Functions),
      parahipocampal_region = unbox(PRESERVACION_PARAHIPOCAMPAL$predicted_Deterioro[1]*100),
      bc                    = list(
          gazing = list (
             encoding = c(T3[1,6],T3[1,7]),
             retrieval = c(T3[2,6],T3[2,7])
          ),
          saccade = list (
             encoding = c(T3[1,4],T3[1,5]),
             retrieval = c(T3[2,4],T3[2,5])
          ),
          duration = list (
             encoding = c(T3[1,8],T3[1,9]),
             retrieval = c(T3[2,8],T3[2,9])
          ),
          num_fixations = list (
             encoding = c(T3[1,10],T3[1,11]),
             retrieval = c(T3[2,10],T3[2,11])
          )          
      ),
      uc                    = list(
          gazing = list (
             encoding = c(T3[3,6],T3[3,7]),
             retrieval = c(T3[4,6],T3[4,7])
          ),
          saccade = list (
             encoding = c(T3[3,4],T3[3,5]),
             retrieval = c(T3[4,4],T3[4,5])
          ),
          duration = list (
             encoding = c(T3[3,8],T3[3,9]),
             retrieval = c(T3[4,8],T3[4,9])
          ),
          num_fixations = list (
             encoding = c(T3[3,10],T3[3,11]),
             retrieval = c(T3[4,10],T3[4,11])
          )          
      )      
      
),pretty = T, path = output_file)
#),pretty = T, path = "test.json")

#Si "Integrative_memory_performance" esta en rojo y "Asociative_memory_performance" en verde o amarillo, 
#y "PRESERVACION PARAHIPOCAMPAL" en rojo y "executive_Functions" en rojo, entonces hay probabilidad de Alzheimer.

#Si "Integrative_memory_performance" esta en rojo y "Asociative_memory_performance" en rojo, 
#y "PRESERVACION PARAHIPOCAMPAL" en rojo, entonces hay alteración cognitiva y cerebral generalizada.

#Si "Integrative_memory_performance" esta en verde o amarillo  y "Asociative_memory_performance" en verde o amarillo, 
#y "PRESERVACION PARAHIPOCAMPAL" en verde o amarillo, entonces hay desempeño dentro de los valores normales para edad de la persona.
