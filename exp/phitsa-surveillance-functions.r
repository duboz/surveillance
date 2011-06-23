library(rvle)

#Fonction de simulation AVEC contrôle..
controled_disease<-function(graph, infectedNodes, transmissionRate, 
                      duration, infPeriode, securedPeriode, restockPeriode,
controlDelay, probaDeclaration =
0.2, constPeriods = TRUE, control = TRUE, controlRadius = 0, nodes_positions =
runif(length(infectedNodes)),first_wave_sched =
runif(1)*duration*matrix(1,length(infectedNodes),1),
second_wave_sched = runif(1)*duration*matrix(1,length(infectedNodes),1)){
dir<-getwd()
  f = rvle.open("phitsa-disease-surveillance-control-R.vpz", pkg="surveillance")
  setwd(dir)
  rvle.setDuration(f,duration)
	rvle.setSeed(f,runif(1)*1000000)
 	rvle.setBooleanCondition(f,"control","disabled", !control)
 	rvle.setRealCondition(f,"control","controlDelay", controlDelay)
	if (controlRadius > 0)
		rvle.setRealCondition(f, "control", "controlRadius",
controlRadius)
	rvle.setTupleCondition(f,"cond_graph","nodes_positions", nodes_positions)	
	rvle.setTupleCondition(f,"xRay_surv","first_wave", first_wave_sched)	
	rvle.setTupleCondition(f,"xRay_surv","second_wave", second_wave_sched)	
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
show_epidemic<-function(r,graph, coordo = FALSE, date = 1, vertex =c()){
if (coordo == FALSE) {
par<-list(niter=1000)
coordo=gplot.layout.fruchtermanreingold(graph,par)}
cols=heat.colors(5)
nbNodes<-length(graph[1,])
#for (i in c(date:length(r[[1]][,1]))){
i=date
infection<-r[[1]][i,2:(nbNodes+1)]
colors<-matrix(0,1,nbNodes)
for (j in 1:nbNodes){
colors[1,j]<-cols[5-infection[j]]
if (infection[j] == 4) {colors[1,j]<-4}
#else {colors[1,j]<-0}
#if (paste("vertex-",j-1,sep="") %in% vertex)
if (j %in% vertex)
colors[1,j]<-3
}
gplot(graph,coord=coordo,vertex.col=colors,arrowhead.cex = 0.1,new=TRUE,
main=paste("Epidemic at time",i))
#scan()
#}
}

plot_prev<-function(res,duration){
times<-res[[1]][,1]
SIRSec<-matrix(0,duration+1,4)
for (i in 1:duration+1){
SIRSec[i,1]<-length(which(res[[1]][i,] == 1))
SIRSec[i,2]<-length(which(res[[1]][i,] == 2))
SIRSec[i,3]<-length(which(res[[1]][i,] == 3))
SIRSec[i,4]<-length(which(res[[1]][i,] == 4))
}
plot(times, SIRSec[,1], ylim=c(0,1044), type = "l",col="blue")
lines(times, SIRSec[,2], ylim=c(0,1044), type = "l",col="red")
lines(times, SIRSec[,3], ylim=c(0,1044), type = "l",col="green")
lines(times, SIRSec[,4], ylim=c(0,1044), type = "l",col="black")
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

generate_xRay_plan<-function(data,vague=1, vague_start=1, vague_duration=30)
{
#Première vague

nb_vill_per_day<-abs(length(data$VIL_CODE)/vague_duration)
nb_village<-length(data$VIL_CODE)
#vaguei<-matrix(NA,length(data$VIL_CODE),1)
plan<-data
today<-vague_start
nb_vill_today<-0
random_districts<-sample(levels(as.factor(data$VIL_LTD_DI)),replace=FALSE)
no_sub_district<-1
#tant qu'il reste des villages à inspecter
for (district in random_districts) {

#Tant qu'il reste des villages à inspecter dans le district..
#while (length(which(is.na(plan$vague1[plan$VIL_LTD_DI == district]))) != 0) {
random_sub_distr<-sample(levels(as.factor(data$VIL_LTS_SU[plan$VIL_LTD_DI == district])),replace=FALSE)
for (sub_d in random_sub_distr){
for (vill in sample(which(data$VIL_LTS_SU ==sub_d), replace = FALSE)){
plan[vill,12+vague]<-today
if (nb_vill_today < nb_vill_per_day){
nb_vill_today <- nb_vill_today + 1
} else {
nb_vill_today <- 0
today <- today + 1
}
}
}
}
return(plan)
}

movie_all<-function(){
for (i in 1:60){
png(filename=paste("phitsa",i,".png",sep=""))
show_epidemic(res, phitsa_netw, cbind(villages$XX, villages$YY), date =i,
vertex=c(eval(parse(text=paste("c(",A[abs(i/7),2],")",sep="")))))
dev.off(2)
print(paste("image",i))
}
}

movie_xRay<-function(data){
for (i in min(data$vague1):max(data$vague1)){
png(filename=paste("xRay",i,".png",sep=""))
show_epidemic(res, phitsa_netw, cbind(villages$XX, villages$YY), date =3,
vertex=c(which(data$vague1 == i)))
dev.off(2)
print(paste("image",i))
}
}
