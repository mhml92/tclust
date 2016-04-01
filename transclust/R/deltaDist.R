setwd("~/Dropbox/Datalogi/Speciale/code/firstDraft/R")
library(ggplot2)
library(stringr)
#
#files <- list.files(path="data", pattern="*.txt", full.names=T, recursive=FALSE)
#lapply(files, function(filename) {
#})

filename <- "data/test_delta_karate.txt"
data <- read.table(filename, header=TRUE, sep="\t")
names(data) <- c('threshold','r','dist')
options(scipen=999)
ggplot(data,aes(x = factor(r), y = as.numeric(dist), color = factor(threshold))) +
   geom_boxplot(outlier.shape = NA) +
   #scale_y_log10()+
   #geom_point(aes(y = m,x = factor(r),color=factor(threshold))) +
   ggtitle(filename)



filename <- "pos_de.txt"
data <- read.table(filename, header=FALSE, sep="\t")
names(data) <- c('threshold','r','x','y')
options(scipen=999)
iter <- unique(data$r)
for(i in iter){
   subdata <- data[which(data$r == i),]
   plot <- ggplot(subdata,aes(x = x, y = y, color = factor(threshold))) + geom_point()
   plot(plot)
}

filename <- "pos_de.txt"
data <- read.table(filename, header=TRUE, sep="\t")
names(data) <- c('cluster','x','y','threshold')
options(scipen=999)
   plot <- ggplot(data,aes(x = x, y = y,color = factor(cluster),shape = factor(cluster))) +
      theme(legend.position = "bottom") +
      geom_point() +
      coord_fixed(ratio = 1) +
      scale_shape_manual(values = 0:length(unique(data$cluster))) + 
      #scale_y_log10()+
      #geom_point(aes(y = m,x = factor(r),color=factor(threshold))) +
      ggtitle("link")
   #png(paste0("brown_png/",str_pad(i,4,c("left"),pad="0"),".png"),width = 1024,height = 768)
   plot(plot)
   #dev.off()
