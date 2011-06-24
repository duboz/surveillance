source("phitsa-surveillance-functions.r")
#Général
duration=333
epidemic<-function(rate = 0.1, expe_num =1, genNetw=TRUE){

#État initial
infected_villages<-c("650504_01","650905_03","650117_09")

#Paramètres
distance_infectieuse <- 4500 #Distance d'infection (En mètres donc)
infper = 9
restockper = 7
securedper = 21
controlDelay = 3 #math dit trois/ 4 jours
controlRadius = 4000 #Distance en m.
p = 0.01 #proba de déclaration

#Données
villages <- read.table("../data/villages-data.csv", sep=";", header=TRUE)

#Génération du réseau
if (genNetw) {
phitsa_netw<-create_phitsa_graph(distance_infectieuse)
}
#Génération du planning de visite de villages pour la surveillance xRay
villages<-generate_xRay_plan(villages, vague=1, vague_start=90)
villages<-generate_xRay_plan(villages, vague=2, vague_start=213)

#formatage de l'état initial
initState<-matrix(0,length(phitsa_netw[1,]),1)
if (length(which(villages$VIL_CODE %in% infected_villages)) == 0) {
	print("ERROR: initial outbreak not found..\n \n \n")
}
initState[which(villages$VIL_CODE %in% infected_villages),1]<-1

#simulation
res= controled_disease(phitsa_netw,initState, 
rate, duration,infper, securedper, restockper, probaDeclaration=p, controlDelay
= controlDelay, controlRadius = controlRadius, nodes_positions =
c(rbind(villages$XX,villages$YY)), first_wave_sched = villages$V13,
second_wave_sched = villages$V14)

#Code pour récupérer les données:
data<-read.table("../data/date_code_village_phitsa.csv", sep =";", header=T)
#CODE POUR RECUPERER LA LISTE SIMULEE:
A= read.table("dsc-phitsa_surv_repport.dat", sep="\t")
#Puis:
#for (i in 1)
#report_semainei_total=eval(parse(text=paste("c(",A[i,2],")",sep="")))
#report_semainei_passive=test=eval(parse(text=paste("c(",A[i,3],")",sep="")))
#c1
x11()
nb_outb_per_week<-matrix(0,max(data$Semaine)+1,3)
for (i in 1:max(data$Semaine))
{nb_outb_per_week[i,1]<-i
nb_outb_per_week[i,2]<- length(which(data$Semaine==i))
simu_report=eval(parse(text=paste("c(",A[i,2],")",sep="")))
nb_outb_per_week[i,3]<-length(simu_report)
}
plot(nb_outb_per_week[,1], nb_outb_per_week[,2], ylim =
c(0,max(nb_outb_per_week)), main = paste("expe: ", expe_num))
lines(nb_outb_per_week[,1], nb_outb_per_week[,2])
points(nb_outb_per_week[,1], nb_outb_per_week[,3], col = 4)
lines(nb_outb_per_week[,1], nb_outb_per_week[,3], col = 4)
return(res)
}
