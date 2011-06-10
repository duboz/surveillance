source("phitsa-surveillance-functions.r")
#Général
duration=180

#État initial
infected_villages<-c("650205_10")

#Paramètres
distance_infectieuse <- 3500 #Distance d'infection (En mètres donc)
rate = 0.3
infper = 5
recovper = 5
p = 0.1 #proba de déclaration

#Données
villages <- read.table("../data/villages-data.csv", sep=";", header=TRUE)

#Génération du réseau
phitsa_netw<-create_phitsa_graph(distance_infectieuse)

#formatage de l'état initial
initState<-matrix(0,length(phitsa_netw[1,]),1)
initState[which(villages$VIL_CODE %in% infected_villages),1]<-1

#simulation
res= controled_disease(phitsa_netw,initState, 
rate, duration,infper, recovper, probaDeclaration=p)

#Ploter le graphe (nécessite la librairie SNA)
library(sna)
coordinates <- villages$XX
coordinates <- cbind(coordinates, villages$YY)
grandes_villes <- c("") #Pour afficher les noms des grandes villes
vill_label <- matrix("",1044,1)
for (i in 1:1044)
{if (villages$VIL_CODE[i] %in% grandes_villes) 
	{vill_label[i] <- as.character(villages$VIL_ENG[i])
	 print(villages$VIL_ENG[i])}
	}

gplot(phitsa_netw, gmode ="graph", coord=coordinates, 
label = vill_label, boxed.labels=FALSE, label.bg=TRUE,
label.cex=1, vertex.cex = 0.4, vertex.col="yellow")
