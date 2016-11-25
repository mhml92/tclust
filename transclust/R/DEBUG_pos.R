library(ggplot2)
library(stringr)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/R/")
#data <- read.table(file("stdin"), header=FALSE, sep=",")
data <- read.table(file("jtc_gavin_pos/hsphere.txt"), header=FALSE, sep=",")
names(data) <- c('r','x','y','z')
iter <- unique(data$r)
options(scipen=999)
for(i in iter){ 
  #if(i == 0){
    plot <- ggplot(data = data[which(data$r == i),],aes(x = x, y = y)) + 
      geom_point(alpha = 0.5) +
      coord_fixed() +
      theme(legend.position="none")
  png(
    filename = paste("java_pos_out_hsphere/",str_pad(i,4,pad="0"),".png",sep = ""),
    width = 1024,
    height = 1024
    )
 plot(plot)
  dev.off()
}
#edge_weight <- -0.30004
#f_rep       <- 100
#log_d       <- 2.96937
#num_nodes   <- 135
##force:        -0.0748482
#
#(edge_weight*f_rep)/(log_d*num_nodes)
#plot(
#  (edge_weight*f_rep)/(log_d*num_nodes)
#)
#
#