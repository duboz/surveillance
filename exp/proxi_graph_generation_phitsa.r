#Paramètres
inf_distance <- 4000 #Distance d'in fection (En mètres donc)

#Données
villages <- read.table("villages-data.csv", sep=";", header=TRUE)

#Génération du graphe par proximité
inf_graph <- matrix(0, 1044, 1044)
dist_matrix <- matrix(0, 1044, 1044)
for (i in 1:1044) {
for (j in 1:1044){
dist <- sqrt((villages$XX[i] - villages$XX[j])^2 +
	(villages$YY[i] - villages$YY[j])^2)
dist_matrix[i,j]<-dist
if (dist < inf_distance) {inf_graph[i,j]=1}
}}

#Connectage du réseau (ie. faire en sorte qu'il n'y ait plus qu'un seul
#compostant)

node_components <- component.dist(inf_graph)$membership
nb_components <- max(component.dist(inf_graph)$membership)
while (nb_components > 1) {
	node_components <- component.dist(inf_graph)$membership
	nb_components <- max(component.dist(inf_graph)$membership)
	print(paste("Moui c'est un peu long.. Il reste: ",nb_components,
"composants"))
	comp_nodes <- which(node_components == 1)
	other_nodes <- which(node_components != 1)
	mindist <- 1000000
	node_i<-0
	node_j<-0
	for (i in comp_nodes) {
	for (j in other_nodes) {
	  distij <- dist_matrix[i,j] 
	  if (distij < mindist) {
	    mindist <- distij
	    node_i <- i
	    node_j <- j
  	  }
	}}
	inf_graph[node_i,node_j] <-1
	inf_graph[node_j,node_i] <-1
}


#Ploter le graphe (nécessite la librairie SNA)
library(sna)
coordinates <- villages$XX
coordinates <- cbind(coordinates, villages$YY)
grandes_villes <- c("650119_01","650902_03") #Pour afficher les noms des grandes villes
vill_label <- matrix("",1044,1)
for (i in 1:1044)
{if (villages$VIL_CODE[i] %in% grandes_villes) 
	{vill_label[i] <- as.character(villages$VIL_ENG[i])
	 print(villages$VIL_ENG[i])}
	}

gplot(inf_graph, gmode ="graph", coord=coordinates, 
label = vill_label, boxed.labels=FALSE, label.bg=TRUE,
label.cex=1, vertex.cex = 0.4, vertex.col="yellow")
