source("phitsa-surveillance-functions.r")
#Général
duration=60

#État initial
infected_villages<-c("650408_01")

#Paramètres
distance_infectieuse <- 4500 #Distance d'infection (En mètres donc)
rate = 0.8
infper = 7
restockper = 7
securedper = 7
controlDelay = 2
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
= controlDelay)
