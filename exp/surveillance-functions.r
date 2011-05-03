library(rvle)

#Fonction de simulation
VLEsimulate<-function(graph, infectedNodes, transmissionRate, 
                      duration, infPeriode, recovPeriode){
dir<-getwd()
  f = rvle.open("init-R-surveillance_1.vpz", pkg="surveillance")
  setwd(dir)
  rvle.setDuration(f,duration)
	rvle.setSeed(f,runif(1)*1000000)
 	rvle.setRealCondition(f,"susceptible","infectiousPeriod", infPeriode)
	rvle.setRealCondition(f,"infected","infectiousPeriod", infPeriode)
	rvle.setRealCondition(f,"susceptible","securedPeriod", recovPeriode)
	rvle.setRealCondition(f,"infected","securedPeriod", recovPeriode)
	rvle.setRealCondition(f,"transmission","rate", transmissionRate)
	classes<- ""
  for (node in infectedNodes) {
  if (node == 1) classes <- paste(classes, "InfectedVertex ", sep="")
  else if (node == 0) classes <- paste(classes, "Vertex ", sep="")
  else print("ERROR!! Mauvais vecteur d'infection")
  }
	rvle.setStringCondition(f,"cond_graph","graphInfo_classes", classes)	
	if (length(infectedNodes)^2 != length(graph)) print("ERROR: mauvais nombre de noeuds")
	adj_matrix<-""
	for (i in 1:length(graph[1,])) {
	 for (j in 1:length(graph[,1])) {
     if (graph[i,j] == 0) adj_matrix=paste(adj_matrix, "0 ", sep="")
     else if (graph[i,j] == 1) adj_matrix=paste(adj_matrix, "1 ", sep="")
     else print("ERR mauvais formatage du graph")
    }
  }
	rvle.setStringCondition(f,"cond_graph","graphInfo_adjacency_matrix", adj_matrix)
  rvle.setIntegerCondition(f,"cond_graph","graphInfo_number", length(infectedNodes))	
  result = rvle.runMatrix(f)#[[3]]
  infectious=c()
  for (t in 0:duration)
  {infectious =c(infectious, length(which(result[[1]][t,2:length(infectedNodes)+1]==2)))}
  plot(result[[1]][,1], infectious)
return (result)
}

##INITIALISATION

#Génération du graphe:
#graph=sociomatrix_as_random_graph(nb,proba=0.1) 
#graph<-sociomatrix_as_regular_lattice(10,10) #(x*y = nb)
#graph<-sociomatrix_as_rewired_lattice(x,y,proba) #(x*y = nb)
#graph<-sociomatrix_as_scale_free(nb,1)
source("generate-graphs.r")


#conditions initiales d'infection (exemple):
x=2000
g<-sociomatrix_as_rewired_lattice(20,100) #(x*x = nb)
longueDist<-1
for (i in 1:longueDist){
    g[runif(1)*x,runif(1)*x]<-1
}
initState<-matrix(0,x,1)
initState[1,1]<-1
rate=0.1
duration=200
infper=30
recovper=30

##SIMULATION
res<-VLEsimulate(g,initState, rate, duration,infper, recovper)

##Analyse


##Fonctions de visualisation
library(sna)
show_epidemic<-function(r,graph){
par<-list(niter=1000)
coordo=gplot.layout.fruchtermanreingold(graph,par)
cols=heat.colors(4)
nbNodes<-length(graph[1,])
colors<-matrix(0,1,nbNodes)
infection<-r[[1]][1,2:(nbNodes+1)]
for (i in 1:nbNodes){
colors[1,i]<-cols[(4-infection[i])]
}
gplot(graph,coord=coordo,vertex.col=r[[1]][1,2:(nbNodes+1)],arrowhead.cex = 0.1)
gplot(graph,coord=coordo,vertex.col=colors,arrowhead.cex = 0.1)
for (i in c(1:length(r[[1]][,1]))){
infection<-r[[1]][i,2:(nbNodes+1)]
colors<-matrix(0,1,nbNodes)
for (j in 1:nbNodes){
colors[1,j]<-cols[4-infection[j]]
}
scan()
gplot(graph,coord=coordo,vertex.col=colors,arrowhead.cex = 0.1,new=TRUE,
main=paste("Epidemic at time",i))
}
}
