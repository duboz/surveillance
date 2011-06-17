source("phitsa-surveillance-functions.r")
#Général
duration=60

#État initial
infected_villages<-c("650408_01")

#Paramètres
distance_infectieuse <- 4500 #Distance d'infection (En mètres donc)
rate = 0.2
infper = 7
restockper = 7
securedper = 7
controlDelay = 1
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

#CODE POUR RECUPERER LA LISTE:

#A= read.table("dsc-phitsa_surv_repport.dat", sep="\t")
#Puis:
#report_semainei_total=test=eval(parse(text=paste("c(",A[i,2],")",sep="")))
#report_semainei_passive=test=eval(parse(text=paste("c(",A[i,3],")",sep="")))

