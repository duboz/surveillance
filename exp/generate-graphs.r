# return sociomatrix

sociomatrix_as_random_graph <- function(nb_nodes, proba=0.1){
   m <- matrix(rbinom(nb_nodes*nb_nodes, 1, proba), nr = nb_nodes, nc = nb_nodes)
   for ( i in 1:nb_nodes){
       for ( j in i:nb_nodes){
           if (i==j)
               m[i,j] = 0
           else
       if (m[i,j] > 0)
                   m[j,i] = 1
       else
           m[j,i] = 0
       }
   }
   return(m)
}

sociomatrix_as_regular_lattice <- function(x_lattice , y_lattice){

   nb_node = x_lattice * y_lattice

   mat = matrix(0,nr=nb_node,nc=nb_node)

   x_lat = 0
   for ( i in 1:nb_node ){
       #x_lat, y_lat position du noeud dans la grille
       modulo = i %% y_lattice
       if(modulo == 1)
           x_lat = x_lat + 1
       if(modulo == 0)
           modulo = y_lattice
       y_lat = modulo
                  for (x in (x_lat-1):(x_lat+1)){
           for (y in (y_lat-1):(y_lat+1)){
               if(x > 0 && y > 0 && x <= x_lattice && y <= y_lattice){
                   j = (x-1) * y_lattice + y
                   if (i != j){
                       mat[i,j] = 1
                       mat[j,i] = 1
                   }
               }
           }
       }
   }

   return(mat)
}


sociomatrix_as_rewired_lattice <- function(x_lattice , y_lattice, p = 1){

   mat <- sociomatrix_as_regular_lattice(x_lattice , y_lattice)
      for(i in 1:(x_lattice*y_lattice)){
       for(j in 1:(x_lattice*y_lattice)){
           if(runif(1) < p && mat[i,j] > 0){
               indice = as.integer(runif(1, 1, x_lattice*y_lattice))
                           if(mat[i,indice] < 1){                                       mat[i,j]      = 0
                   mat[j,i]      = 0
                   mat[i,indice] = 1
                   mat[indice,i] = 1
                               }
           }
       }
   }

   return(mat)   
}

sociomatrix_as_scale_free <- function(nb_node, m, proba=0.3, init_size=10){

   tmp <- matrix(rbinom(init_size*init_size, 1, proba), nr = init_size, nc = init_size)
   net <- matrix(0, nr = nb_node, nc = nb_node)
   
      for (i in 1:init_size)
       tmp[i,i] <- 0

      for (i in 1:init_size ){
       for(j in i:init_size){
           net[i,j] <- tmp[i,j]
           net[j,i] <- tmp[i,j]
       }
   }
	for (i in 1:init_size) {
	if (sum(net[i,])==0){
		jo<-as.integer(runif(1)*init_size+1)
		print("jo")
		print(jo)
		net[i,jo]<-1
		net[jo,1]<-1
		}
	}
print(net[1:init_size,1:init_size])
print(sum(net[init_size:nb_node,init_size:nb_node]))

   net[1,2] <- 1
   net[2,1] <- 1


   for (i in (init_size+1):nb_node ){
       k_vector <- vector("numeric", length = i-1)
       K = sum(net[,])
              for (j in 1:(i-1)){
           k_vector[j] <- 0               
	   k <- sum(net[j,])
           p <- k / K
           k_vector[j] <- p
       }
       for(z in 2:(i-1))
           k_vector[z] <- k_vector[z] + k_vector[z-1]


       for(z in 1:m){                       
	   j <- 1
           p <- runif(1)
           while(j <= (i-1) && p > k_vector[j])
               j <- j + 1
           net[i,j] <- 1
           net[j,i] <- 1
       }
   }
   return(net)
}

