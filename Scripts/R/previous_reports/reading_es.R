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


parseq_fast<-function(a)
{
  la = length(a)
  dim(a) <- c(la,1)
  diff <- a[2:la,]-a[1:la-1,]
  tmp <- which(diff!=0)
  b <- cbind(c(1,tmp+1), c(tmp, la))
  b <- cbind(b, b[,2]-b[,1]+1)
}


args = commandArgs(trailingOnly=TRUE)
if (length(args) != 2) {
   stop("Reading Script requires 2 and only 2 argurments", call.=FALSE)
}

#setwd("./res")


sss<-read.csv(args[1])  #yo_09_05_2020
dim(sss)

bs3<-sss
#head(bs3)
#table(bs3$age)
#as.data.frame(table(bs3$display_id))

bs3<-bs3[,c(1:18)]

bs3$Condition<-"NA"
bs3$Condition<-as.factor(bs3$Condition)

head(bs3)

names(bs3)<-c("id_","age","suj","id","sn","trial","fixn","screenpos","wn","let","dur","nw","eye", "pupila","blink","ao","gaze","nf","Condition")
table(bs3$id)

str(bs3)
summary(bs3)
bs3$Condition<-as.numeric(bs3$Condition)

############################
bs3$Condition<-"CONTROL"



a<-(bs3)


rm(bs3)


library(healthcareai)


#ofile <- ("PACIENTE_NN_R.rda")   #### INTRODUCR NOMBRE DEL PACIENTE!!!!!!!!!

table(a$eye)
idx<-which(a$eye=="0")
#a<-a[-idx,]

dim(a)
head(a)

str(a)
a$trial<-as.numeric(a$trial)
a$dur<-as.numeric(a$dur)
a$gaze<-as.numeric(a$gaze)
a$nw<-as.numeric(a$nw)
#a<-a[,2:12]
#a<-a[,2:17]
a$let<-round(a$let)

tail(a)



#------------------------------------
# changes in the fixation sequences
#------------------------------------
#idx<-which(is.nan(a$let) & a$wn<=0)

a$wn<-round(a$wn)

idx<-which(a$wn>a$nw)
a<-a[-idx,]

dim(a)

# 
# # 1   1  1  1  2  2  2    3    3
# # NA  0 -9  0  1  0 -9    1    0
# # NA NA  9 -9 -1  1  9   -10   1
# 
str(a)
a$id<-as.numeric(a$id)
a$sn<-as.numeric(a$sn)
a$eye<-as.numeric(a$eye)
pid<-a$id*1000+a$sn*100+a$eye
sid<-c(NA,diff(pid))
length(pid)
dim(a)
sid[is.nan(a$let)]<--99
for (i in 1:11){
  sid1<-c(NA,diff(sid))
  sid[sid1==99]<--99
}

a$let[sid==-99]<-NaN
#check
tail(a)
head(cbind(a$let,sid,sid1))
head(a)

#  delete first 10 practice sentences
head(a)
a<- a[a$sn>10,]
head(a);dim(a)


# delete wn==-1 or 0 (words right to the last word)
idx<-which(a$wn<=0)
# a<- a[-idx,]
# head(a);dim(a)




#------------------------------------


aL<-a[a$eye==1,c("id_","id","sn","nw","wn","let","dur","pupila","gaze","nf","ao","Condition")]
aR<-a[a$eye==1,c("id_","id","sn","nw","wn","let","dur","pupila","gaze","nf","ao","Condition")]
#aL<-a[a$eye==0,c("id","sn","nw","wn","let","dur", "Condition")]
#aR<-a[a$eye==1,c("id","sn","nw","wn","let","dur", "Condition")]
dim(aR)
rm(a)


# ----------------------------------------------------------------------------------------
for (ijk in 1:2) {
  if (ijk==1) {a <- aL}  # Left or right eye
  if (ijk==2) {a <- aR  # Left or right eye      
  }
}  

# --- Matrix a ---------------------------------------------------------------------------
# glueing together fixations on the same letter
asnl<-a$id*1000000000+a$sn*10000+a$wn*100+a$let
sid<-parseq_fast(asnl)
asnl[is.nan(asnl)]<--1

a_fsl<-a[sid[,1],]
a<-a_fsl;rm(a_fsl)

dim(a)

head(a)

# 1) id 
# 2) sent, 
# 3) n of words, 
# 4) word number, 
# 5) letter, 
# 6) duration

a$dur1 <- c(NA, a$dur[-(nrow(a))])
a$dur2 <- c(a$dur[-(1)], NA)
a$dur3 <- c(NA, NA, a$dur[-((nrow(a)-1):nrow(a))])
a$dur4 <- c(a$dur[-(1:2)], NA, NA)

s <- parseq_fast(a$id*1000+a$sn)
a[s[,1], c("dur1", "dur3")] <- NA	# for 1st word in sentence
a[s[,1]+1, c("dur3")]       <- NA	# for 2nd word in sentence
a[s[,2], c("dur2", "dur4")] <- NA	# for last word in sentence
a[s[,2]-1, c("dur4")]       <- NA	# for last-1 word in sentence

#str(a) 
# ----------------------------------------------------------------------------------------
head(a)


# --- Remove sentences with max 2 fixated words ------------------------------------------

sid <- a[,"id"]*1000 + a[,"sn"]

t0<-proc.time()
six <- parseq_fast(sid)
t1<-proc.time()
print(t1-t0)
# ----------------------------------------------------------------------------------------

# # --- outgoing amplitude and relative fixation position ---------------------------------- -->
#a$ao 	<- c(diff(a$wb+a$let), NA) -->
pq 	<- parseq_fast(a$id*1000 + a$sn) 
a$ao[pq[,2]]<-NA

# a$dir <- sign(a$ao)          # Strip off sign and store in a$ao -->

a$ao  <- round(a$ao)           # Absolute amplitude in a$ao


a$o 	<- a$let/(1/a$l+1)   
# ----------------------------------------------------------------------------------------
head(a)
dim(a)

# --- add lag & successor information ----------------------------------------------------
# (fixation level)
#a[,c("wn1.x", "let1", "l1.x", "ao1", "o1","dir1")] <-
#  rbind(NA, a[-nrow(a),c("wn", "let", "l", "ao", "o","dir")])
# a[pq[,1],c("wn1.x", "let1", "l1.x", "ao1", "o1","dir1")] <- NA

a[,c("wn1.x", "let1", "ao1", "o1")] <-
  rbind(NA, a[-nrow(a),c("wn", "let", "ao", "o")])
a[pq[,1],c("wn1.x", "let1", "ao1", "o1")] <- NA


#a[,c("wn2.x", "let2", "l2.x", "ao2", "o2","dir2")] <-
#  rbind(a[-1,c("wn", "let", "l", "ao", "o","dir")],NA)
#a[pq[,2],c("wn2.x", "let2", "l2.x", "ao2", "o2","dir2")] <- NA

a[,c("wn2.x", "let2", "ao2", "o2")] <-
  rbind(a[-1,c("wn", "let", "ao", "o")],NA)
a[pq[,2],c("wn2.x", "let2", "ao2", "o2")] <- NA


#a[,c("wn3.x", "let3", "l3.x", "ao3", "o3","dir3")] <-
#  rbind(NA, NA, a[-(c(nrow(a)-1,nrow(a))),c("wn", "let", "l", "ao", "o","dir")])
#a[c(pq[,1],pq[,1]+1), c("wn3.x", "let3", "l3.x", "ao3", "o3","dir3")] <- NA

a[,c("wn3.x", "let3", "ao3", "o3")] <-
  rbind(NA, NA, a[-(c(nrow(a)-1,nrow(a))),c("wn", "let", "ao", "o")])
a[c(pq[,1],pq[,1]+1), c("wn3.x", "let3", "ao3", "o3")] <- NA

#a[,c("wn4.x", "let4", "l4.x", "ao4", "o4","dir4")] <-
#  rbind(a[-c(1,2),c("wn", "let", "l", "ao", "o","dir")],NA,NA)
#a[c(pq[,2],pq[,2]-1),c("wn4.x", "let4", "l4.x", "ao4", "o4","dir4")] <- NA

a[,c("wn4.x", "let4", "ao4", "o4")] <-
  rbind(a[-c(1,2),c("wn", "let", "ao", "o")],NA,NA)
a[c(pq[,2],pq[,2]-1),c("wn4.x", "let4", "ao4", "o4")] <- NA

#str(a)
# ----------------------------------------------------------------------------------------

#View(a4)

mean(a$dur)



dim(a)
#a[100:150,1:12]



MINDUR <- 51   #para versión corrección de de j laubrock

MAXDUR <- 1750  # para versión corrección de de j laubrock

MAXAMP <- 24



#######################
firstpass<-function(a){
  #############################
  ffw<-a[1]; #furthest fixated word (entspricht cur)
  r=0; # regression (entspricht jump, aber umgekehrt gepolt)
  fpf<-c()
  #fpf<-as.matrix(fpf)
  #fpf<-as.data.frame(fpf)
  for (i in 1:length(a)){
    if (a[i]==ffw & r==0){
      fpf[i]=1
      next
    }
    if (a[i]>ffw){
      r=0
      ffw=a[i]
      fpf[i]=1
      next
    }
    # if ((a[i]<ffw) | (a[i]==ffw & r!=0)){
    r=1
    fpf[i]=0
    # }
    
  }
  return(fpf) 
}







aL<-a

dim(a)
head(a)
str(a)

aL$Condition<-as.factor(aL$Condition)


head(aL)
aL$sn<-as.numeric(aL$sn)
aL$id<-as.numeric(aL$id)

sid<-aL$id*1000+aL$sn  
six<-parseq_fast(sid)           # Start and n of fix for each sentence


firstpL_h=c()                  # Firstpass fixations = 1 (left eye)
for (i in 1:nrow(six))
{
  ix=six[i,1]
  iy=six[i,2]
  fpf=firstpass(aL$wn[ix:iy]) # One sentence at a time
  firstpL_h<-c(firstpL_h,fpf)
  ix=ix+iy
}
firstpL<-which(firstpL_h==1)  # Keep indices for firstpass fix
length(firstpL)
length(firstpL)/nrow(aL)
dim(aL)







morepL<-which(firstpL_h==0)  # indices for second and more pass 
length(morepL)
length(morepL)/nrow(aL)



firstpL_h=c()                 # Firstpass fixations = 1 (right eye)
for (i in 1:nrow(six))
{
  ix=six[i,1]
  iy=six[i,2]
  fpf=firstpass(aL$wn[ix:iy])  # One sentence at a time
  firstpL_h=c(firstpL_h, fpf)
  ix=ix+iy
}
firstpL<-which(firstpL_h==1)
length(firstpL)


morepL<-which(firstpL_h==0)


firstpB <- intersect(firstpL, firstpL)  #Intercept rigth and left eye fixations
first<-a[firstpB,]
#first[1:33,c(1,2,3,4,5,6,16,20,35,41)]


morepB  <- intersect(morepL, morepL)

# Index for regression (word n: origin, word n+1: goal)  -->
regL  <- which(aL$wn>aL$wn1.x & aL$wn>aL$wn2.x) 
regR  <- which(aL$wn>aL$wn1.x & aL$wn>aL$wn2.x) 
regBv <- intersect(regL,regR)  

# Index for first and last fixations of sentences -->
frstfix=six[,1]
lastfix=six[,2] 



regL  <- which(aL$wn>aL$wn1.x & aL$wn>aL$wn2.x) 

length(regL) 
length(regL)/nrow(aL) 

regR  <- which(aL$wn>aL$wn1.x & aL$wn>aL$wn2.x) 


regBv <- intersect(regL,regR)  


xL <- which(aL$wn>aL$wn1.x & aL$wn<aL$wn2.x)


xL1st <- intersect(firstpL,xL) 
length(xL1st) 
#xxx<-a[xL1st,] 



a1<-aL
dim(a1)
nrow(a1)
a2<-aL[firstpL, ]
first<-length(firstpL)/nrow(aL)
first

#dim(a2)

a3<-aL[xL1st, ]       #singlefix
dim(a3)
single<-length(xL1st)/nrow(aL)
single

a4<-aL[morepL, ]
dim(a4)
multi<-length(morepL)/nrow(aL)
multi

#############################
#################################################
####################################################################################




aL$id<-as.factor(aL$id)
aL$sn<-as.factor(aL$sn)
aL$wn<-as.numeric(aL$wn)
aL$dur<-as.numeric(aL$dur)
aL$gaze<-as.numeric(aL$gaze)
aL$nf<-as.numeric(aL$nf)
aL$ao<-as.numeric(aL$ao)
head(aL)


a10 <- ddply(aL, .(Condition,id), summarise, mdur = mean(dur),sddur=sd(dur), mao = mean(ao),
             sdao=sd(ao), mwn = mean(wn),sdwn=sd(wn), mgaze = mean(gaze),sdgaze=sd(gaze))

a10$mdur<-round(a10$mdur)
a10$mao<-round(a10$mao)
a10$mwn<-round(a10$mwn)
a10$mgaze<-round(a10$mgaze)

DF <- data.frame(table(aL$id))
names(DF)<-c("id","TOTAL")
head(DF)
dim(DF)

a10$TOTAL <- DF[match(paste(a10$id),paste(DF$id)),"TOTAL"]



DF <- data.frame(table(a2$id))
names(DF)<-c("id","TOTAL")
head(DF)
dim(DF)

a2<- ddply(a2, .(Condition,id), summarise, mdur = mean(dur),sddur=sd(dur), mao = mean(ao),sdao=sd(ao), mwn = mean(wn),sdwn=sd(wn))

a10$FPF <- DF[match(paste(a2$id),paste(DF$id)),"TOTAL"]


DF <- data.frame(table(a3$id))
names(DF)<-c("id","TOTAL")
head(DF)
dim(DF)

a3<- ddply(a3, .(Condition,id), summarise, mdur = mean(dur),sddur=sd(dur), mao = mean(ao),sdao=sd(ao), mwn = mean(wn),sdwn=sd(wn))

a10$SF <- DF[match(paste(a3$id),paste(DF$id)),"TOTAL"]


DF <- data.frame(table(a4$id))
names(DF)<-c("id","TOTAL")
head(DF)
dim(DF)

a4<- ddply(a4, .(Condition,id), summarise, mdur = mean(dur),sddur=sd(dur), mao = mean(ao),sdao=sd(ao), mwn = mean(wn),sdwn=sd(wn))

a10$MF <- DF[match(paste(a4$id),paste(DF$id)),"TOTAL"]



a10$FPF_<-a10$FPF/a10$TOTAL*100
# #a1$FPF_ <- data.frame((a1$FPF_))
# #a1$FPF_ <- melt(a1$FPF_) 
# 
a10$SF_<-a10$SF/a10$TOTAL*100
# #a1$SF_ <- data.frame((a1$SF_))
# #a1$SF_ <- melt(a1$SF_) 
# 
a10$MF_<-a10$MF/a10$TOTAL*100
# #a1$MF_ <- data.frame((a1$MF_))
# #a1$MF_ <- melt(a1$MF_) 

head(a10)





a10$mdur<-log(a10$mdur)
a10$mgaze<-log(a10$mgaze)
a10$sddur<-log(a10$sddur)
a10$sdgaze<-log(a10$sdgaze)

a10$Deterioro <-ifelse(a10$Condition== "CONTROL", "NO", "SI")

a10$Deterioro<-as.factor(a10$Deterioro)

str(a10)
a10$id<-as.factor(a10$id)
a10$mwn<-as.numeric(a10$mwn)
a10$mdur<-as.numeric(a10$mdur)
a10$mgaze<-as.numeric(a10$mgaze)
a10$mao<-as.numeric(a10$mao)

a10$TOTAL<-as.numeric(a10$TOTAL)
a10$FPF<-as.numeric(a10$FPF)
a10$SF<-as.numeric(a10$SF)
a10$MF<-as.numeric(a10$MF)

head(a10)
dim(a10)




models_reading <- load_models("reading_es_model.RDS")

#models_reading <- load_models("my_models_reading_ESPAÑOL.RDS")


#####################################################################################
################################################################################################




###############################################################################################

#Condition id     mdur     sddur      mao     sdao      mwn     sdwn    mgaze   sdgaze TOTAL FPF SF  MF

head(a10)
str(a10)
head(a10)

reading_output <- paste0(               "attentional_processes = ", a10$TOTAL, ";\n")
reading_output <- paste0(reading_output,"executive_proceseses = ",  a10$MF_, ";\n")
reading_output <- paste0(reading_output,"working_memory = ",        a10$FPF_, ";\n")
reading_output <- paste0(reading_output,"retrieval_memory = ",      a10$SF_, ";\n")

a3111 <- a10[,c(2,18)]

str(a10)
a4111 <- (a10[,c(3,4,7:10,15:17)])

a60<-cbind(a3111,a4111)

head(a60)
m3<-predict(models_reading, a60, outcome_groups = 5)

m3

reading_output <- paste0(reading_output,"reading_predicted_deterioration = ",m3$predicted_Deterioro, ";\n")

fileConn<-file(args[2])
writeLines(reading_output, fileConn)
close(fileConn)     
 


