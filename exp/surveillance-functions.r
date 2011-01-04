library(rvle)
library(sna)

dir<-getwd()

#Génération du graphe:
#graph=sociomatrix_as_random_graph(nb,proba=0.1) 
#graph<-sociomatrix_as_regular_lattice(10,10) #(x*y = nb)
#graph<-sociomatrix_as_rewired_lattice(x,y,proba) #(x*y = nb)
#graph<-sociomatrix_as_scale_free(nb,1)

#conditions initiales d'infection:
initalize_infection<-function(nbInfected,nb)
{
initStates<-matrix("Vertex",1,nb)
for(i in 1:nbInfected){
initStates[1,as.integer(runif(1)*length(initStates) +1)]<- "InfectedVertex"
}
return(initStates) 
}


#SIMULATION
simulate<-function(sampleSize,duration,replicats){
dir<-getwd()
diff=c()
pcent=sampleSize*100
main=paste("RVLE-surveillance-output(sample ",pcent," pct)",sep="") 
    x11(title=main)
    layout(matrix(1:replicats,min(replicats,10)/2,2))
f = rvle.open("init-R-surveillance.vpz", "surveillance")
	rvle.setDuration(f,duration)
 	rvle.setRealCondition(f,"xRay_1","echProp", sampleSize)
    for (r in 1:replicats){
        result = rvle.runMatrix(f)
        rvle.setSeed(f,runif(1)*1000000)
        plot(result[[1]][,1],result[[1]][,2],type="l",
        xlab="time",ylab="prevalence",main=paste("replicat:",r,sep=" "),ylim=c(0,1))
        points(result[[2]][,1],result[[2]][,4], col="red")
        lines(result[[2]][,1],result[[2]][,4], col="red")
        diff<-c(diff, mean(abs(result[[2]][,4]-result[[2]][,2])))
    }
    print(paste("mean difference is: ",mean(diff)))
setwd(dir)
return (result)
}

