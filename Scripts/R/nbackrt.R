
library(memisc)
library(MASS)
library(grid)
library(ggplot2)
library(plyr)
library(scales)
library(reshape2)
library(reshape)
library(jsonlite)
library(gdata)


#### NOTEBOOK ########


rm(list=ls())  #Remove workspace

args = commandArgs(trailingOnly=TRUE)

## Argument 1: CSV File
## Argument 2: Output File. 

if (length(args) != 2) {
   stop(paste0("NBackRT Script requires 2 and only 2 arguments. Found: ",length(args)), call.=FALSE)
   print(args[1])  # CSV
   print(args[2])  # output
}

csv_file<-args[1];
output_file<-args[2];

b<-read.csv(csv_file)

str(b)
head(b)
b$ojoDI<-as.factor(b$ojoDI)
b$idtrial<-as.numeric(b$idtrial)
b$imgnum<-as.factor(b$imgnum)
b$target_hit<-as.factor(b$target_hit)
b$trial_seq<-as.factor(b$trial_seq)
b$seq_comp<-as.factor(b$seq_comp)
b$dur<-as.numeric(b$dur)
b$latSac<-as.numeric(b$latSac)
b$amp_sacada<-as.numeric(b$amp_sacada)
#b$resp_time<-as.numeric(b$resp_time)

dim(b)
head(b)
names(b) <- c("suj","trial_id", "imagenN","target","target_hit","nback","trial_seq","seq_comp","dur", "eye", "latsac", "ao","respT")

b$imagenN<-as.factor(b$imagenN)




table(b$resp_time)
tail(b)

table(b$eye)
table(b$trial_id)
#eye = 0 IZQUIERDO
idx<-which(b$eye=="1") #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
b<-b[idx,]

idx<-which(b$dur<50) 
length(idx)
#b<-b[-idx,]

idx<-which(b$trial_id <= 9) #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
b<-b[-idx,]

#View(b)

# "O" sea en la "5" el tipo tiene que mirar a donde estaba la "3" en "6" tiene que mirar a donde estaba el "2"
# y en "7" a donde estaba el "1"

#table(b$latsac,b$imagenN)
table(b$imagenN,b$target_hit)

str(b)
levels(b$imagenN)
table(b$imagenN)
table(b$target)
table(b$eye)
table(b$target_hit)
table(b$seq_comp)
b$trial_id<-as.factor(b$trial_id)

head(b)
tail(b)
b$respT<-as.numeric(as.character(b$respT))


Number_of_fixations_RET<-which(b$imagenN=="4")
length(Number_of_fixations_RET)

Number_of_fixations_ENC<-which(b$imagenN!="4")
length(Number_of_fixations_ENC)
pruebaENC<-b[Number_of_fixations_ENC,]
pruebaENC$target
table(pruebaENC$target,pruebaENC$imagenN)
b$trial_id<-as.factor(b$trial_id)

b$target<-as.numeric(b$target)
a4 <- ddply(b, .(suj,trial_id,imagenN,target,target_hit,seq_comp), summarise,
            mlatsac = mean(latsac),mrespT = mean(respT),mao = mean(ao),Nfixations=length(suj))

#                        mlatsac = mean(latsac),mrespT = mean(respT),mao = mean(ao),Nfixations=length(suj))



a4$Nfixations<-as.numeric(a4$Nfixations)



###################################
#calculo problemas inhibitorios
idx<-which(a4$imagenN=="4")
a4_ENC<-a4[-idx,]   #Me quedo con ENCODING
#View(a4_ENC)
table((a4_ENC$target_hit))
table((a4_ENC$target))

b$imagenN<-as.numeric(b$imagenN)


a4_ENC$target<-as.numeric(a4_ENC$target)
idx<-which(a4_ENC$target < 6)

a4_ENC_INH<-a4_ENC[idx,]  #Me quedo con aquellos targets donde NO DEBERIA haber fijado en el ENCODING (fijaciones sobre punto rojo)
#table((a4_ENC$target))

###########################
str(a4_ENC)
############################################

a4_ENC$target<-as.numeric(a4_ENC$target)
Inhibitory_problems_Percent<-(nrow(a4_ENC_INH)*100)/nrow(a4_ENC)
Inhibitory_problems_Percent  #Porcentaje de veces que fijaron en punto rojo durante el ENCODING

table(b$imagenN)
idx<-which(b$imagenN=="1"|b$imagenN=="2"|b$imagenN=="3")
a4_RET<-b[-idx,] #me quedo con decodificación o RETRIEVAL






#a4_RET$respT<-as.numeric(as.character(a4_RET$respT))
summary(a4_RET)

mean_saccade_amplitude<-mean(a4_RET$ao)   #

mean_saccade_amplitude

idx<-which(a4_RET$respT!="NA")
length(idx)
a4_RET<-a4_RET[idx,]
mean_Response_Time<-mean(a4_RET$respT)
mean_Response_Time


#n_back_Percent<-table(a4_RET$nback)*100/86


idx<-which(a4_RET$target_hit=="0") 
length(idx)
a4_RET_TH<-a4_RET[-idx,]

mean_SAC<-mean(a4_RET$ao)
mean_SAC


table(a4$suj)

#correct_target_hit_Percent<-(nrow(a4_RET_TH)*100)/nrow(a4_RET) 
correct_target_hit_Percent<-((nrow(a4_RET_TH))*100)/261 # cantidad de trials para responder (87 trial x 3 imagenes)
correct_target_hit_Percent  #Cantidad de sujetos analizados

idx<-which(a4_RET$seq_comp=="1") 
length(idx)
a4_RET_1<-a4_RET[idx,]

correct_complete_sequence_Percent<-(nrow(a4_RET_1)*100)/87 
correct_complete_sequence_Percent




#incorrect targer_hit me dará "Working memory error %"
#diferencia entre nback 0, 1 y 2 (o 5, 6, 7) me dará "working memory effect". No debería haber diferencia.


nrow(b)
length(Number_of_fixations_ENC) #Number_of_fixations_encoding  #cantidad de fijaciones que hace para levantar info en "encoding" .
length(Number_of_fixations_RET) #Number_of_fixations_retrieval  #cantidad de fijaciones que hace para levantar info  en "retrieval".

Inhibitory_problems_Percent  #cantidad de veces que miró el punto rojo y no debía hacerlo; problemas inhibitorios
correct_target_hit_Percent # cantidad de veces que miró el lugar donde estaban los targets correctos. Working memory error 
correct_complete_sequence_Percent # cantidad de secuencias completas. Working Memory Capacity
mean_Response_Time  # Velocidad para hacer tareas y desarrollo de estrategia de búsqueda eficiente,
mean_saccade_amplitude # amplitud de la sacada. Funcionamiento subcortical

Visual_Working_Memory<-mean_Response_Time  #tiempo que tarda en hacer cada trial

prueba<-cbind(length(Number_of_fixations_ENC),length(Number_of_fixations_RET),Inhibitory_problems_Percent,
              correct_target_hit_Percent,correct_complete_sequence_Percent,mean_Response_Time,
              mean_saccade_amplitude)
prueba
#write.table(prueba, "C:/Users/gerar/Dropbox/N_back_RT/para colo/prueba_n-back_RT.txt") #acá setea el directorio de tu maquina

VISUAL_EXPLORATION_EFFICIENCY <- mean_saccade_amplitude #basal ganglia (BG) and the superior colliculus (SC)
VISUAL_EXPLORATION_EFFICIENCY
#2.9|4|6
#4 - 6 verde, 2.9 - 4 amarillo, 6 o mas azul, 2.9 o menos rojo


Visual_Working_Memory <- mean_Response_Time   # medial prefrontal cortex (mPFC)
#1600|2400|2700
#1600 - 2400 verde, 2400 - 2700 amarillo, 1600 o menos azul, 2700 o mas rojo

Correct_Responses<-correct_target_hit_Percent  #Prefrontal Cortex, Anterior Cingulate Cortex (ACC)
Correct_Responses
#50|60|81
#60 - 81 verde, 50 - 60 amarillo, 81 o mas azul, 50 o menos rojo

Working_Memory <- correct_complete_sequence_Percent # Frontal and Parietal Regions
Working_Memory
#45|58|76
#58 - 76 verde, 45 - 58 amarillo, 76 o mas azul, 45 o menos rojo

INHIBITORY_PROCESSES  <- Inhibitory_problems_Percent#caudal inferior frontal gyrus (cIFG)
INHIBITORY_PROCESSES
#3|18|45
#3 - 18 verde, 18 - 45 amarillo, 3 o menos azul, 45 o mas rojo

DECODING_EFFICIENCY  <- length(Number_of_fixations_RET)# Lateral Geniculate Nucleus (lGN)
DECODING_EFFICIENCY
#500|650|990
#500 - 650 verde, 650 - 990 amarillo, 500 o menos azul, 990 o mas rojo

ENCODING_EFFICIENCY  <-length(Number_of_fixations_ENC)#  Lateral Posterior (LP) nucleus
ENCODING_EFFICIENCY 
#260|300|430
#260 - 300 verde, 300 - 430 amarillo, 260 o menos azul, 430 o mas rojo

write_json(x = list(
      visual_exploration_efficiency  = unbox(VISUAL_EXPLORATION_EFFICIENCY),
      visual_working_memory          = unbox(Visual_Working_Memory),
      correct_responses              = unbox(Correct_Responses),
      working_memory                 = unbox(Working_Memory),
      inhibitory_processes           = unbox(INHIBITORY_PROCESSES),
      decoding_efficiency            = unbox(DECODING_EFFICIENCY),
      enconding_efficiency           = unbox(ENCODING_EFFICIENCY)
),pretty = T, path = output_file)


