source("surveillance-functions.r")

x=100
g<-sociomatrix_as_rewired_lattice(10,10, p = 0.01) #(x*x = nb)
longueDist<-1
for (i in 1:longueDist){
    g[runif(1)*x,runif(1)*x]<-1
}
initState<-matrix(0,x,1)
initState[1,1]<-1
rate=0.3
duration=200
infper=5
recovper=5
#x11(title=paste("rate=",rate,"infper=",infper, "recovper=", recovper, "delai d'intervention= 1"))
layout(matrix(1:6,3,2))
for (p in (0:5)/10) {
plot(0,0, xlim=c(0,duration/10), ylim= c(0,1), 
main=paste("Proba SHH!=",p, " (5 simulations)"), xlab="Presentation time",ylab="Laugh prevalence" )
for (i in 1:5){
res= controled_disease(g,initState, 
rate, duration,infper, recovper, probaDeclaration=p)
infectious = c()
for (t in 0:duration)
  {
	infectious =
		c(infectious, length(which(res[[3]][t,2:length(initState)
		+1]==2)))}
  lines((res[[3]][,1])/10, infectious/100)
}
}

