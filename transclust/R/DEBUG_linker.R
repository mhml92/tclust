library(ggplot2)
library(stringr)
#setwd("~/Dropbox/Datalogi/Speciale/code/firstDraft/R")
#filename <- "ktest.txt"
#data <- read.table(filename, header=FALSE, sep="\t")
data <- read.table(file("stdin"), header=FALSE, sep="\t")
names(data) <- c('cluster','x','y','threshold','id')
options(scipen=999)
th <- unique(data$threshold)
for(i in th){
   subdata <- data[which(data$threshold == i),]
   uniqueCC <- unique(subdata$id)
   clusters <- 0
   for(cc in uniqueCC){
      clusters <- clusters + length(unique(subdata[which(subdata$id == cc),]$cluster))
   }
   plot <- ggplot(subdata,aes(x = x, y = y,label = cluster)) +
      geom_label(aes(fill = factor(id)), colour = "white", fontface = "bold") + 
      #coord_fixed() +
      #scale_y_log10()+
      #geom_point(aes(y = m,x = factor(r),color=factor(threshold))) +
      ggtitle(paste0("Num obj: ",nrow(subdata),", Clusters: ",clusters,", Threshold: ",i))+
      theme(
         legend.position="none"
      )
   #png(paste0("brown_png/",str_pad(i,4,c("left"),pad="0"),".png"),width = 1024,height = 768)
   plot(plot)
   #dev.off()
}