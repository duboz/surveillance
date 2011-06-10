library(rvle)

#Fonction de simulation AVEC contrôle..
controled_disease<-function(graph, infectedNodes, transmissionRate, 
                      duration, infPeriode, securedPeriode, restockPeriode,
controlDelay, probaDeclaration =
0.2, constPeriods = TRUE, control = TRUE){
dir<-getwd()
  f = rvle.open("phitsa-disease-surveillance-control-R.vpz", pkg="surveillance")
  setwd(dir)
  rvle.setDuration(f,duration)
	rvle.setSeed(f,runif(1)*1000000)
 	rvle.setBooleanCondition(f,"control","disabled", !control)
 	rvle.setRealCondition(f,"control","controlDelay", controlDelay)
 	rvle.setRealCondition(f,"passive_surv","probabilityDeclaration",
probaDeclaration)
 	rvle.setRealCondition(f,"disease","infectiousPeriod", infPeriode)
	rvle.setRealCondition(f,"disease","securedPeriod", securedPeriode)
	rvle.setRealCondition(f,"disease","restockingPeriod", restockPeriode)
	rvle.setBooleanCondition(f,"disease","constPeriods", constPeriods)
	rvle.setRealCondition(f,"transmission","rate", transmissionRate)
	classes<- ""
  for (node in infectedNodes) {
  if (node == 1) classes <- paste(classes, "InfectedVertex ", sep="")
  else if (node == 0) classes <- paste(classes, "Vertex ", sep="")
  else print("ERROR!! Mauvais vecteur d'infection")
  }
	rvle.setStringCondition(f,"cond_graph","graphInfo_classes", classes)	
	if (length(infectedNodes)^2 != length(graph)) print("ERROR: mauvais nombre de noeuds")
	write.table(graph, "rvle-running-exp-graph.dat", sep =" ", row.names =F,
col.names = F)
	string_graph<-read.table("rvle-running-exp-graph.dat", header = FALSE, sep=";")
	#adj_matrix<-as.character(string_graph)
	adj_matrix<-""
	for (i in 1:length(graph[1,])) {
	adj_matrix<-paste(adj_matrix,string_graph[i,1],sep=" ")
	}
	#for (j in 1:length(graph[,1])) {
     #if (graph[i,j] == 0) adj_matrix=paste(adj_matrix, "0 ", sep="")
     #else if (graph[i,j] == 1) adj_matrix=paste(adj_matrix, "1 ", sep="")
     #else print("ERR mauvais formatage du graph")
    #}
  #}
	rvle.setBooleanCondition(f,"cond_graph","R_INIT", TRUE)
	rvle.setStringCondition(f,"cond_graph","graphInfo_adjacency_matrix", adj_matrix)
  rvle.setIntegerCondition(f,"cond_graph","graphInfo_number", length(infectedNodes))
 rvle.save(f,"rvle-running-exp.vpz")
  result = rvle.runMatrix(f)#[[3]]
return (result)
}

##Fonctions de visualisation
library(sna)
show_epidemic<-function(r,graph, coordo = FALSE, date = 1){
if (coordo == FALSE) {
par<-list(niter=1000)
coordo=gplot.layout.fruchtermanreingold(graph,par)}
cols=heat.colors(4)
nbNodes<-length(graph[1,])
colors<-matrix(0,1,nbNodes)
infection<-r[[3]][1,2:(nbNodes+1)]
for (i in 1:nbNodes){
colors[1,i]<-cols[(4-infection[i])]
}
#gplot(graph,coord=coordo,vertex.col=r[[3]][1,2:(nbNodes+1)],arrowhead.cex = 0.1)
#gplot(graph,coord=coordo,vertex.col=colors,arrowhead.cex = 0.1)
for (i in c(date:length(r[[3]][,1]))){
infection<-r[[3]][i,2:(nbNodes+1)]
colors<-matrix(0,1,nbNodes)
for (j in 1:nbNodes){
colors[1,j]<-cols[4-infection[j]]
}
scan()
gplot(graph,coord=coordo,vertex.col=colors,arrowhead.cex = 0.1,new=TRUE,
main=paste("Epidemic at time",i))
}
}

#Fonction pour phitsanuloc

#Ploter le graphe (nécessite la librairie SNA)
plot_nice_netw<-function(phitsa_netw){
library(sna)
coordinates <- villages$XX
coordinates <- cbind(coordinates, villages$YY)
grandes_villes <- c("") #Pour afficher les noms des grandes villes
vill_label <- matrix("",1044,1)
for (i in 1:1044)
{#if (villages$VIL_CODE[i] %in% grandes_villes) 
	vill_label[i] <- as.character(villages$VIL_CODE[i])
	 #print(villages$VIL_ENG[i])
	}

gplot(phitsa_netw, gmode ="graph", coord=coordinates, displayLabel =TRUE,
label = vill_label, boxed.labels=FALSE, label.bg=TRUE,
label.cex=1, vertex.cex = 0.4, vertex.col="yellow")
}

create_phitsa_graph <-function(inf_distance) {
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
return(inf_graph)
}


