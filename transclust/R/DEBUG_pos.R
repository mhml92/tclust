library(ggplot2)
library(stringr)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/R/")
#data <- read.table(file("stdin"), header=FALSE, sep=",")
data <- read.table(file("../build/release/out.txt"), header=FALSE, sep=",")
names(data) <- c('r','x','y')
iter <- unique(data$r)
options(scipen=999)
for(i in iter){ 
  #if(i == 0){
    plot <- ggplot(data = data[which(data$r == i),],aes(x = x, y = y)) + 
      geom_point(aes(alpha=0.75,shape="circ")) +
      coord_fixed() +
      coord_equal()+
      theme(legend.position="none"
           # ,
           # axis.title.x=element_blank(),
           # axis.text.x=element_blank(),
           # axis.ticks.x=element_blank(), 
           # axis.ticks.y=element_blank(), 
           # axis.title.y=element_blank(),
           # axis.text.y=element_blank(),
           # axis.ticks.y=element_blank() 
            )
  pdf(
    file = paste("2d_thesis/",str_pad(i,4,pad="0"),".pdf",sep = ""),
    width = 3,
    height = 3
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