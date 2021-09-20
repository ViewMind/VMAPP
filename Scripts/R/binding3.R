library(healthcareai)
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
library(gdata)

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
BC<-BC[,c(1:15,28,29)]
names(BC) <- c("id","age","subj_id", "trial_id", "is_trial","trial_name","trial_type", "response", "dur", "eye",  "blinks", "sacc_ampl",  "pupila","gaze","nf","timeline","score")

table(BC$is_trial)
head(BC)


UC<-read.csv(uc_file)

#UC<-read.csv("binding_uc_3_l_2_2020_06_30_11_07.csv") #primer sujeto protocolo islandia

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
str(bs)
bs = filter(bs, eye != "0" )

idx<-which(bs$eye=="0") #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
#bs<-bs[-idx,]


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




#m1 <- load_models("my_models_doble_CIEGO_04_5_20-3.RDS")
m1 <- load_models(model_file)

m4<-predict(m1, a5, outcome_groups = 2)

m4


PRESERVACION_PARAHIPOCAMPAL<-m4   # PARAHIPOCAMPAL FUNCTIONING == AI
# Intervalo de Referencia =  64 - 99.   Verde 64-99.  Amarillo 45 - 63.   Rojo 1 - 43







a41 <- ddply(as_1, .(Condition,id,type), summarise,  mdur = mean(dur),msd = sd(dur),  mao = mean(sacc_ampl),
             mpupila = mean(pupila),sdpupila = sd(pupila), mgaze = mean(gaze)
             , mnf = mean(nf), sdnf = sd(nf),mscore = mean(score),sdao = sd(sacc_ampl),sdgaze = sd(gaze))

#head(a41)
#print("a41==========================================")
#a41

# BUSQUEDA_VISUAL<-a41$mao[a41$type=="BC"] #<- VISUAL SEARCH == SUBCORTICAL -> BASAL GANGLIA, SUPERIOR COLICULLUS.
BUSQUEDA_VISUAL<-a41[1,6]
# Intervalo de Referencia =  1 - 3.5.   Verde 1 - 3.5.  Amarillo 3.6 - 7.   Rojo mayor a 7.1

#DURACION_MIRADA<-a41$mgaze[a41$type=="BC"] #  GAZE  == CORTICAL  ->lateral parietal cortex, the medial prefrontal cortex (mPFC) and the anterior superior temporal sulcus (aSTS) 
DURACION_MIRADA<-a41[1,9] #  GAZE  == CORTICAL  ->lateral parietal cortex, the medial prefrontal cortex (mPFC) and the anterior superior temporal sulcus (aSTS) 
# print("Duracion Mirada")
# DURACION_MIRADA
# Intervalo de Referencia =  800 - 2000.   Verde 800 - 2000.  Amarillo 799 - 400.   Rojo 399 - 1



# Integrative_memory_performance<-a41$mscore[a41$type=="BC"]  #Integrative memory performance  == score BC  -> corteza enthorinal, corteza perirhinal
Integrative_memory_performance<-a41[1,12]
# Intervalo de Referencia =  63 - 100.   Verde 63 - 100.  Amarillo 59 - 62.   Rojo 40 - 58

bs$score<-(bs$score*100)/32

Asociative_memory_performance<-bs$score[bs$type=="UC"]  #Asociative memory performance  == score UC -> Hipocampo
#Asociative_memory_performance<-a41[3,13]
Asociative_memory_performance<-unique(Asociative_memory_performance)
# Intervalo de Referencia =  67 - 100.   Verde 67 - 100.  Amarillo 66 - 58.   Rojo 40 - 57

idx<-which(bs$is_trial=="0")
length(idx)
as_REC<-bs[-idx,]
idx<-which(as_REC$type=="UC")
length(idx)
as_REC<-as_REC[-idx,]


a42MEAN_REC <- ddply(as_REC, .(Condition), summarise,   mnf = mean(nf))



executive_Functions <- a42MEAN_REC$mnf #executive Functions == Number of fixations EN LA RECOGNITION
# Intervalo de Referencia =  1 - 16.   Verde 1 - 16.  Amarillo 17 - 21.   Rojo 21 o m�s

print("preservacion");
PRESERVACION_PARAHIPOCAMPAL
print("busqueda visual");
BUSQUEDA_VISUAL
print("duracion");
DURACION_MIRADA
print("int mem perf");
Integrative_memory_performance
print("assoc mem perf");
Asociative_memory_performance
print("exec func");
executive_Functions


T3 <- ddply(bs, .(Condition,type,is_trial_1), summarise,  mean_saccade = mean(sacc_ampl),SEsaccade = sd(sacc_ampl)/sqrt(length(sacc_ampl))
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


# pepe <- aggregate(bs$sacc_ampl,by=list(bs$Condition,bs$type,bs$is_trial_1),mean)
# pepe
# print("Prueba 1")
# pepe[1,1]
#print("Prueba 2")
#T3[1,4]



