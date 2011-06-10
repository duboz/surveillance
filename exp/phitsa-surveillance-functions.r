library(rvle)

#Fonction de simulation AVEC contrôle..
controled_disease<-function(graph, infectedNodes, transmissionRate, 
                      duration, infPeriode, recovPeriode, probaDeclaration =
0.2, constPeriods = FALSE, control = TRUE){
dir<-getwd()
  f = rvle.open("phitsa-disease-surveillance-control-R.vpz", pkg="surveillance")
  setwd(dir)
  rvle.setDuration(f,duration)
	rvle.setSeed(f,runif(1)*1000000)
 	rvle.setBooleanCondition(f,"control","disabled", !control)
 	rvle.setRealCondition(f,"control","controlDelay", 1)
 	rvle.setRealCondition(f,"passive_surv","probabilityDeclaration",
probaDeclaration)
 	rvle.setRealCondition(f,"disease","infectiousPeriod", infPeriode)
	rvle.setRealCondition(f,"disease","securedPeriod", recovPeriode)
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

##INITIALISATION

#Génération du graphe:
#graph=sociomatrix_as_random_graph(nb,proba=0.1) 
#graph<-sociomatrix_as_regular_lattice(10,10) #(x*y = nb)
#graph<-sociomatrix_as_rewired_lattice(x,y,proba) #(x*y = nb)
#graph<-sociomatrix_as_scale_free(nb,1)
source("generate-graphs.r")

x=100
g<-sociomatrix_as_rewired_lattice(10,10, p = 0.01) #(x*x = nb)
longueDist<-1
for (i in 1:longueDist){
    g[runif(1)*x,runif(1)*x]<-1
}
initState<-matrix(0,x,1)
initState[1,1]<-1
rate=0.3
duration=500
infper=5
recovper=5

##SIMULATION
test_uncontroled_disease<-function () 
{
#conditions initiales d'infection (exemple):

res<-uncontroled_disease(g,initState, rate, duration,infper, recovper)
infectious = c()
for (t in 0:duration)
  {
	infectious =
		c(infectious, length(which(res[[3]][t,2:length(initState)
		+1]==2)))}
  plot(res[[3]][,1], infectious)
  return(res)
}

test_controled_disease<-function () 
{
res<-controled_disease(g,initState, rate, duration,infper, recovper)
infectious = c()
for (t in 0:duration)
  {
	infectious =
		c(infectious, length(which(res[[3]][t,2:length(initState)
		+1]==2)))}
  plot(res[[3]][,1], infectious)
  lines(res[[3]][,1], infectious)
  return(res)
}


##Analyse


##Fonctions de visualisation
library(sna)
show_epidemic<-function(r,graph, coordo = FALSE){
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
gplot(graph,coord=coordo,vertex.col=r[[3]][1,2:(nbNodes+1)],arrowhead.cex = 0.1)
gplot(graph,coord=coordo,vertex.col=colors,arrowhead.cex = 0.1)
for (i in c(1:length(r[[3]][,1]))){
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


