library(ggplot2)
data <- read.table(file("stdin"), header=FALSE, sep="\t")
names(data) <- c('threshold','r','x','y')
options(scipen=999)
iter <- unique(data$r)
for(i in iter){
   subdata <- data[which(data$r == i),]
   plot <- ggplot(subdata,aes(x = x, y = y, color = factor(threshold))) + 
      geom_point() +
      coord_fixed()
   plot(plot)
}