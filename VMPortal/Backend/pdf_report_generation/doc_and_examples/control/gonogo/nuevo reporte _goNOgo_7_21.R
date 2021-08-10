
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


#### NOTEBOOK ########


# -Index_Error_in_Task_with_Interference
# -Index_Error_in_Facilitated_Task
# 
# pasan a ser:
# 
# -Inhibitory_alterations_in_Task_with_Interference
# -Inhibitory_alterations_in_Facilitated_Task
# 
# 
# Medias y SE de controles para usar en graficos sacada, gazing y fixation duration:
# 
#  type mean_saccade SEsaccade mean_gazing SEgazing mean_duration SEduration
# 1    G     2.916667 0.1311396    525.2137 6.434115      121.5165   3.146649
# 2    R     3.761308 0.1486889    563.5136 6.109360      120.7572   2.652945



setwd("C:/Users/gerar/Dropbox/Machine learning/Go_no GO_gazepoint")  #HP


#en azul va el ojo izquierdo
#en rojo va el ojo derecho

rm(list=ls())  #Remove workspace


#0: Roja a la izquierda
#1: Verde a la izquierda
#2: Roja a la derecha
#3: Verde a la derecha.

#1) suj: Indentificador sujeto/experimento etc
#2) idtrial: El nombre del trial
#3) target_hit: Esta columna vale 1 si la fijación se encuentra dentro del target que corresponde para el trial
#4) dur: duración, en ms, de la fijación.
#5) ojoDI: El 1 es el ojo activo (el que hay que procesar). Si se usaron los dos, marca el derecho y el 0 el izquierdo.
## NO USAR #6) latSac: Latencia sacádica. Se calcula el tiempo desde el comienzo del trial hasta 
#que se encuentra el primer PUNTO (no fijación, sino data individual) fuera de un área de tolerancia del centro
#de la pantalla. 
#7) amp_sacada: Amplitud sacádica: Distancia,normalizada, en píxeles entre dos fijaciones
#8) resp_time: Esto lo calculé como el tiempo desde el inicio del trial hasta que se encuentra la primer FIJACIÖN 
#(no punto) dentro del target correcto. Si esto no sucede, verás N/A. El tiempo de la fijación es el promedio entre
#el tiempo del punto inicial y final de la fijación. 





b<-read.csv("gonogo_2_2020_11_09_15_05.csv") #gerardo


b$idtrial<-as.factor(b$idtrial)
b$target_hit<-as.factor(b$target_hit)
b$dur<-as.numeric(b$dur)
b$ojoDI<-as.factor(b$ojoDI)
b$latSac<-as.numeric(b$latSac)
b$amp_sacada<-as.numeric(b$amp_sacada)
b$resp_time<-as.numeric(b$resp_time)
b$pupil_size<-as.numeric(b$pupil_size)
b$num_trial_fix<-as.factor(b$num_trial_fix)
b$gazing<-as.numeric(b$gazing)
b$num_center_fix<-as.factor(b$num_center_fix)
b$total_study_time<-as.numeric(b$total_study_time)
b$arrow_type<-as.factor(b$arrow_type)
b$trial_type<-as.factor(b$trial_type)

dim(b)
#b<-b[,3:17]
head(b)
str(b)
table(b$num_center_fix)
table(b$trial_type)

names(b) <- c("suj","trial_id", "target_hit","dur", "eye", "latsac", "ao","respT","pupil","trial_fix","gazing","num_center_fix","TotalTime","type","trial_type")



#b<-c


table(b$eye)
table(b$trial_id)
#eye = 0 IZQUIERDO
idx<-which(b$eye=="1") #En Go NO.GO uso el ojo izquiero porque es el primero en llegar al target  10/11/2020.
length(idx)
b<-b[-idx,]

idx<-which(b$dur<50) 
length(idx)
#b<-b[-idx,]
b$trial_id<-as.numeric(b$trial_id)
idx<-which(b$trial_id <= 9) #trabajamos con el ojo DERECHO, como en lectura.
length(idx)
b<-b[-idx,]

table(b$suj)

#View(b)
table(b$type)
table(b$type,b$target_hit)


b$trial_id<-as.factor(b$trial_id)
# -2  -1   0   1   2
# -2 = fijaci+on entre flecha y target Incorrecto
# -1 = fijación en trial incorrecto
# 0 =  fijación sobre la flecha
# 1 = fijacion en target Correcto
# 2 = fijacion entre flecha y target Correcto

Number_of_fixations<-nrow(b)
Number_of_fixations

head(b)
Processing_Speed<-unique((b$TotalTime)/1000)/60
Processing_Speed

Processing_Speed_Interference<-sum((b$gazing)[b$type=="R"])/1000
Processing_Speed_Interference  #tiempo en segundos que tarda el sujeto en hacer todos los No Go.

Processing_Speed_Facilitated<-sum((b$gazing)[b$type=="G"])/1000
Processing_Speed_Facilitated  #tiempo en segundos que tarda el sujeto en hacer todos los No Go.


TFG<-nrow(b[b$type=="G",])
Facilitation<-TFG  #Total # Fixation Green
TFR<-nrow(b[b$type=="R",])
Interference<-TFR  #Total # Fixation Red

idx<-which(b$target_hit=="0")
b_0<-b[idx,]  #Me quedo con aquellos fijaciones sobre la flecha

Decision_making_time<-aggregate(b_0$gazing,by=list(b_0$type),mean)

Reaction_time_Facilitated_Task<-Decision_making_time[1,2]
Reaction_time_Facilitated_Task
Reaction_time_in_Task_with_Interference<-Decision_making_time[2,2]
Reaction_time_in_Task_with_Interference
#aggregate(b_0$gazing,by=list(b_0$type),sd)

b[1:10,1:12]

str(b)
table(b$target_hit[b$type=="R"])
table(b$target_hit[b$type=="G"])

table(b$type)
table(b$target_hit,b$type)

idx<-which(b$target_hit=="-1")
ERROR<-b[idx,]

Inhibitory_alterations_in_Facilitated_Task<-table(ERROR$target_hit[ERROR$type=="G"])/ (24) *100
Inhibitory_alterations_in_Facilitated_Task<-Inhibitory_alterations_in_Facilitated_Task[1]
Inhibitory_alterations_in_Facilitated_Task 

Inhibitory_alterations_in_Task_with_Interference<-table(ERROR$target_hit[ERROR$type=="R"])/ (27) *100
Inhibitory_alterations_in_Task_with_Interference<-Inhibitory_alterations_in_Task_with_Interference[1]
Inhibitory_alterations_in_Task_with_Interference

str(b)
head(b)
#idx<-which(b$type=="R")
#DMF<-b[-idx,]
#idx<-which(DMF$target_hit=="0")
#DMF<-DMF[idx,]
#Decision making facilitated <- sum fix dur sobre flecha (target_hit "0")
#Decision_making_Facilitated<-mean(sum(DMF$dur))
# aggregate((DMF$dur),by=list(DMF$target_hit=="0"),mean)

#mayor impacto por deterioro está en:
# TT, gazing, target_Hit = -1.



Executive_Functions<-Number_of_fixations



#Facilitation

#Interference


Executive_Functions

Processing_Speed

Processing_Speed_Interference  

Processing_Speed_Facilitated


Inhibitory_alterations_in_Task_with_Interference

Inhibitory_alterations_in_Facilitated_Task

Reaction_time_Facilitated_Task

Reaction_time_in_Task_with_Interference

results<-c(Processing_Speed,Processing_Speed_Facilitated  
           ,Processing_Speed_Interference  
           ,Reaction_time_Facilitated_Task
           ,Reaction_time_in_Task_with_Interference,
           Inhibitory_alterations_in_Facilitated_Task,Inhibitory_alterations_in_Task_with_Interference,Executive_Functions)

colnames=c("Processing_Speed","Processing_Speed_Interference","Processing_Speed_Facilitated","Reaction_time_Facilitated_Task
           ","Reaction_time_in_Task_with_Interference",
           "Inhibitory_alterations_in_Facilitated_Task","Inhibitory_alterations_in_Task_with_Interference","Executive_Functions")

rownames = c()
results = matrix( 
  c(Processing_Speed,Processing_Speed_Interference  
    ,Processing_Speed_Facilitated  
    ,Reaction_time_Facilitated_Task
    ,Reaction_time_in_Task_with_Interference,
    Inhibitory_alterations_in_Facilitated_Task,Inhibitory_alterations_in_Task_with_Interference,Executive_Functions), 
  
  ncol=8,dimnames = list(rownames,colnames)) 


go_no_go<-as.table(results) 


#Fixation_Duration_time<-aggregate(b_0$dur,by=list(b_0$type == "R"),mean)

#Fixation_Duration_time<-aggregate(b$gazing,by=list(b$type,b$trial_id),mean)
#names(Fixation_Duration_time)<-c("Type","Trial Identification","Time to Complete Each Trial")




#binocular disparity TENGO QUE INCLUIR!

#aggregate(b_0$gazing,by=list(b_0$type),sd)




#write.table(results, "C:/Users/gerar/Dropbox/Machine learning/Go_no GO_gazepoint/results_go_no_go_23_7_21.txt") #acá setea el directorio de tu maquina

#VINCULAR VARIABLES CON AREAS DEL CEREBRO

#INCLUIR ALGORITMO INTELIGENTE
#################################################################################################3
#############################################
Processing_Speed  #
# Intervalo de Referencia =  Verde 1 - 1.88.  Amarillo 1.89 - 1.95.   Rojo mayor a 1.96

Processing_Speed_Interference  
# Intervalo de Referencia =  Verde 1 - 83.  Amarillo 84 - 90.   Rojo mayor a 91

Processing_Speed_Facilitated
# Intervalo de Referencia =  Verde 1 - 75.  Amarillo 75 - 85.   Rojo mayor a 86

#Index_Error_in_Task_with_Interference
Inhibitory_alterations_in_Task_with_Interference

# Intervalo de Referencia =  Verde 0 - 0.31.  Amarillo 0.32 - 0.40.   Rojo mayor a 0.41

#Index_Error_in_Facilitated_Task
Inhibitory_alterations_in_Facilitated_Task

# Intervalo de Referencia =  Verde 0 - 0.15.  Amarillo 0.16 - 0.25.   Rojo mayor a 0.26

Reaction_time_Facilitated_Task
# Intervalo de Referencia =  Verde 1 - 784.  Amarillo 785 - 873.   Rojo mayor a 874

Reaction_time_in_Task_with_Interference
# Intervalo de Referencia =  Verde 1 - 829.  Amarillo 830 - 946.   Rojo mayor a 947

Executive_Functions
# Intervalo de Referencia =  Verde 1 - 320.  Amarillo 320 - 420.   Rojo mayor a 421


Processing_Speed  # 1.69
Processing_Speed_Interference # 77.76 
Processing_Speed_Facilitated  # 64.68
Inhibitory_alterations_in_Task_with_Interference # 22.22
Inhibitory_alterations_in_Facilitated_Task       # 0
Reaction_time_Facilitated_Task        # 471 
Inhibitory_alterations_in_Task_with_Interference # 509
Executive_Functions  # 292

#sacada  =  Visual scanning while looking for targets
#gazing  =  time to extract the whole picture of a trial
#fixation duration  = time for extracting relevant information in just once fixation
#nf  = number of fixations required for extracting visual information.

#######
######  GO NO GO Y CEREBRO   https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4808505/  29/7/2021


#Processing_Speed   #Frontal Lobe
#Processing_Speed_Interference #prefrontal cortex, caudate nucleus 
#Processing_Speed_Facilitated  #prefrontal cortex
#Inhibitory_alterations_in_Task_with_Interference # caudal inferior frontal gyrus (cIFG)
#Index_Error_in_Facilitated_Task       # right inferior frontal gyrus (rIFG)
#Reaction_time_Facilitated_Task        # the medial prefrontal cortex (mPFC), subthalamic nucleus
#Reaction_time_in_Task_with_Interference # the medial prefrontal cortex (mPFC), anterior and posterior cingulate cortex
#Executive_Functions  #  Igual que en Binding



######################################################################################################################
head(b)
tail(b)
str(b)
b$trial_fix<-as.numeric(b$trial_fix)
GoNOgo <- ddply(b, .(type), summarise,  mean_saccade = mean(ao),SEsaccade = sd(ao)/sqrt(length(ao))
                , mean_gazing= mean(gazing),SEgazing= sd(gazing)/sqrt(length(gazing)), 
                mean_duration = mean(dur),SEduration = sd(dur)/sqrt(length(dur))
)

GoNOgo

#sacada  =  Visual scanning while looking for targets
#gazing  =  time to extract the whole picture of a trial
#fixation duration  = time for extracting relevant information in just once fixation


### ##################
### 
### Medias y SE de controles para usar en graficos  
### 
###   type mean_saccade SEsaccade mean_gazing SEgazing mean_duration SEduration
### 1    G     2.916667 0.1311396    525.2137 6.434115      121.5165   3.146649
### 2    R     3.761308 0.1486889    563.5136 6.109360      120.7572   2.652945
###   
### ##################
### 
###   sacada  =  Visual scanning while looking for targets
###     gazing  =  time to extract the whole picture of a trial
###     fixation duration  = time for extracting relevant information in just once fixation
### 
###     
###     
###     T3
###     
###     toJSON(x = T3, dataframe = 'rows', pretty = T)
    
    
