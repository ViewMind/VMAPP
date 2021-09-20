
library(memisc)
library(MASS)
library(grid)
library(ggplot2)
library(plyr)
library(scales)
library(reshape2)
library(reshape)

library(gdata)


#### NOTEBOOK ########
rm(list=ls())  #Remove workspace
#setwd("./res")

args = commandArgs(trailingOnly=TRUE)
if (length(args) != 2) {
   stop("NBACK RT Script requires 2 and only 2 argurments", call.=FALSE)
}

b<-read.csv(args[1]);

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
correct_complete_sequence_Percent # cantidad de secuencias completas. Working memory and executive memory performance
mean_Response_Time  # Velocidad para hacer tareas y desarrollo de estrategia de búsqueda eficiente,
mean_saccade_amplitude # amplitud de la sacada. Funcionamiento subcortical

#mean_latencia_sacadica 


prueba<-cbind(length(Number_of_fixations_ENC),length(Number_of_fixations_RET),Inhibitory_problems_Percent,
              correct_target_hit_Percent,correct_complete_sequence_Percent,mean_Response_Time,
              mean_saccade_amplitude)
prueba


nbackrt_output <- paste0(               "number_of_fixations_enc = ",            length(Number_of_fixations_ENC), ";\n")
nbackrt_output <- paste0(nbackrt_output,"number_of_fixations_ret = ",            length(Number_of_fixations_RET), ";\n")
nbackrt_output <- paste0(nbackrt_output,"inhibitory_problems_percent = ",        prueba[3], ";\n")
nbackrt_output <- paste0(nbackrt_output,"correct_target_hit_percent = ",         prueba[4], ";\n")
nbackrt_output <- paste0(nbackrt_output,"correct_complete_sequence_percent = ",  prueba[5], ";\n")
nbackrt_output <- paste0(nbackrt_output,"mean_response_time = ",                 prueba[6], ";\n")
nbackrt_output <- paste0(nbackrt_output,"mean_saccade_amplitude = ",             prueba[7], ";\n")

fileConn<-file(args[2])
writeLines(nbackrt_output, fileConn)
close(fileConn) 



