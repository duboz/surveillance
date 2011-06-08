
 proxy_graph_1044=matrix(0,1044,1044)
 for (i in 1:1044) {
 for (j in 1:1044){
 if ((abs(positions$V1[i]-positions$V1[j]) < 3656) &&
     (abs(positions$V2[i]-positions$V2[j]) < 3656)) {proxy_graph_1044[i,j]=1}
 }}

