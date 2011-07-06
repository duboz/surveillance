coord_sp<-SpatialPoints(coord)
summary(coord_sp)
library(spatial)
plot(coord_sp)
library(spatstat)
replay=Kest(as(coord_sp, "ppp")
plot(replay)
data<-read.table("../data/date_code_village_phitsa.csv", sep =";", header=T)
A= read.table("dsc-phitsa_surv_repport.dat", sep="\t")
simu_report=as.numeric(strsplit(paste(A[17,2],"",sep=""),",")[[1]])
real_report=which(villages$VIL_CODE %in% data$code_village[data$Semaine==16])
plot(coord_sp[simu_report,])
x11()
plot(coord_sp[real_report,])
kest_simu=Kest(as(coord_sp[simu_report,], "ppp"))
kest_real=Kest(as(coord_sp[real_report,], "ppp"))
plot(kest_real)
x11()
plot(kest_simu)
