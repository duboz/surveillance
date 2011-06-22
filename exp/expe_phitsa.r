source("phitsa-surveillance-functions.r")
#Général
duration=60

#État initial
infected_villages<-c("650504_1","650905_3","650117_9")

#Paramètres
distance_infectieuse <- 4500 #Distance d'infection (En mètres donc)
rate = 0.2
infper = 7
restockper = 7
securedper = 7
controlDelay = 1 #math dit trois/ 4 jours
controlRadius = 4000 #Distance en m.
p = 0.01 #proba de déclaration

#Données
villages <- read.table("../data/villages-data.csv", sep=";", header=TRUE)

#Génération du réseau
phitsa_netw<-create_phitsa_graph(distance_infectieuse)

#formatage de l'état initial
initState<-matrix(0,length(phitsa_netw[1,]),1)
if (length(which(villages$VIL_CODE %in% infected_villages)) == 0) {
	print("ERROR: initial outbreak not found..\n \n \n")
}
initState[which(villages$VIL_CODE %in% infected_villages),1]<-1

#simulation
res= controled_disease(phitsa_netw,initState, 
rate, duration,infper, securedper, restockper, probaDeclaration=p, controlDelay
= controlDelay, controlRadius = controlRadius, nodes_positions = c(rbind(villages$XX,villages$YY)))

#Code pour récupérer les données:
data<-read.table("../data/date_code_village_phitsa.csv", sep =";", header=T)
#CODE POUR RECUPERER LA LISTE SIMULEE:
A= read.table("dsc-phitsa_surv_repport.dat", sep="\t")
#Puis:
#for (i in 1)
#report_semainei_total=eval(parse(text=paste("c(",A[i,2],")",sep="")))
#report_semainei_passive=test=eval(parse(text=paste("c(",A[i,3],")",sep="")))
#c1
nb_outb_per_week<-matrix(0,max(data$Semaine)+1,3)
for (i in 1:max(data$Semaine))
{nb_outb_per_week[i,1]<-i
nb_outb_per_week[i,2]<- length(which(data$Semaine==i))
simu_report=eval(parse(text=paste("c(",A[i,2],")",sep="")))
nb_outb_per_week[i,3]<-length(simu_report)
}
plot(nb_outb_per_week[,1], nb_outb_per_week[,2])
lines(nb_outb_per_week[,1], nb_outb_per_week[,2])
points(nb_outb_per_week[,1], nb_outb_per_week[,3], col = 4)
lines(nb_outb_per_week[,1], nb_outb_per_week[,3], col = 4)


for (i in 1:60){
png(filename=paste("phitsa",i,".png",sep=""))
show_epidemic(res, phitsa_netw, cbind(villages$XX, villages$YY), date =i,
vertex=c(eval(parse(text=paste("c(",A[abs(i/7),2],")",sep="")))))
dev.off(2)
print(paste("image",i))
}
