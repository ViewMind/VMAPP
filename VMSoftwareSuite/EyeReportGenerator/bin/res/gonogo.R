
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
rm(list=ls())  #Remove workspace

setwd("./res")

args = commandArgs(trailingOnly=TRUE)
if (length(args) != 2) {
   stop("GONOGO Script requires 2 and only 2 argurments", call.=FALSE)
}

b<-read.csv(args[1]);

#en azul va el ojo izquierdo
#en rojo va el ojo derecho

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
b<-b[,1:15]
head(b)
str(b)
table(b$num_center_fix)
table(b$trial_type)

names(b) <- c("suj","trial_id", "target_hit","dur", "eye", "latsac", "ao","respT","pupil","trial_fix","gazing","num_center_fix","TotalTime","type","trial_type")


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


Number_of_fixations<-nrow(b)
Number_of_fixations

head(b)
Speed_Processing<-unique((b$TotalTime)/1000)/60
Speed_Processing


TFG<-nrow(b[b$type=="G",])
Facilitation<-TFG  #Total # Fixation Green
TFR<-nrow(b[b$type=="R",])
Interference<-TFR  #Total # Fixation Red

idx<-which(b$target_hit=="0")
b_0<-b[idx,]  #Me quedo con aquellos fijaciones sobre la flecha

Decision_making_time<-aggregate(b_0$gazing,by=list(b_0$type),mean)

Decision_making_time_Facilitate<-Decision_making_time[1,2]
Decision_making_time_Facilitate
Decision_making_time_Interference<-Decision_making_time[2,2]
Decision_making_time_Interference
#aggregate(b_0$gazing,by=list(b_0$type),sd)

Percentage_Inhibition_processes_Facilitation<-table(b$target_hit[b$type=="G"]) / nrow(b[b$type=="G",])
Percentage_Inhibition_processes_Facilitation<-Percentage_Inhibition_processes_Facilitation[1]
Percentage_Inhibition_processes_Facilitation
Percentage_Inhibition_processes_Interference<-table(b$target_hit[b$type=="R"]) / nrow(b[b$type=="R",])
Percentage_Inhibition_processes_Interference<-Percentage_Inhibition_processes_Interference[1]
Percentage_Inhibition_processes_Interference

#mayor impacto por deterioro está en:
# TT, gazing, target_Hit = -1.

Speed_Processing

Facilitation

Interference


Percentage_Inhibition_processes_Interference

Percentage_Inhibition_processes_Facilitation

Decision_making_time_Facilitate

Decision_making_time_Interference


results<-c(Speed_Processing,Facilitation,Interference,Decision_making_time_Facilitate,Decision_making_time_Interference,
           Percentage_Inhibition_processes_Facilitation,Percentage_Inhibition_processes_Interference)

colnames=c("Speed_Processing","Facilitation","Interference","Decision_making_time_Facilitate","Decision_making_time_Interference",
           "Percentage_Inhibition_processes_Facilitation","Percentage_Inhibition_processes_Interference")

# rownames = c()
# results = matrix( 
#      c(Speed_Processing,Facilitation,Interference,Decision_making_time_Facilitate,Decision_making_time_Interference,
#         Percentage_Inhibition_processes_Facilitation,Percentage_Inhibition_processes_Interference), 
#       
#      ncol=7,dimnames = list(rownames,colnames)) 



#write.table(results, args[2]) #acá setea el directorio de tu maquina


gonogo_output <- paste0(               "gonogo_speed_processing = ",           Speed_Processing, ";\n")
gonogo_output <- paste0(gonogo_output, "gonogo_facilitation = ",               Facilitation, ";\n")
gonogo_output <- paste0(gonogo_output, "gonogo_interference = ",               Interference, ";\n")
gonogo_output <- paste0(gonogo_output, "gonogo_dmt_facilitate = ",             Decision_making_time_Facilitate, ";\n")
gonogo_output <- paste0(gonogo_output, "gonogo_dmt_interference = ",           Decision_making_time_Interference, ";\n")
gonogo_output <- paste0(gonogo_output, "gonogo_pip_facilitate = ",             Percentage_Inhibition_processes_Facilitation, ";\n")
gonogo_output <- paste0(gonogo_output, "gonogo_pip_interference = ",           Percentage_Inhibition_processes_Interference, ";\n")
# nbackrt_output <- paste0(nbackrt_output,"number_of_fixations_ret = ",            length(Number_of_fixations_RET), ";\n")
# nbackrt_output <- paste0(nbackrt_output,"inhibitory_problems_percent = ",        prueba[3], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"correct_target_hit_percent = ",         prueba[4], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"correct_complete_sequence_percent = ",  prueba[5], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"mean_response_time = ",                 prueba[6], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"mean_saccade_amplitude = ",             prueba[7], ";\n")


# nbackrt_output <- paste0(               "number_of_fixations_enc = ",            length(Number_of_fixations_ENC), ";\n")
# nbackrt_output <- paste0(nbackrt_output,"number_of_fixations_ret = ",            length(Number_of_fixations_RET), ";\n")
# nbackrt_output <- paste0(nbackrt_output,"inhibitory_problems_percent = ",        prueba[3], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"correct_target_hit_percent = ",         prueba[4], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"correct_complete_sequence_percent = ",  prueba[5], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"mean_response_time = ",                 prueba[6], ";\n")
# nbackrt_output <- paste0(nbackrt_output,"mean_saccade_amplitude = ",             prueba[7], ";\n")
# 
fileConn<-file(args[2])
writeLines(gonogo_output, fileConn)
close(fileConn) 


