library(ggplot2)
library(stringr)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/data/results/")
data <- read.table(file("java_vs_cpp_execution_time_coryne_server.txt"), header=TRUE, sep="\t")
options(scipen=999)
datasets <- unique(data$similarity_file)
for(file in datasets){
  subdata <- data[which(data$similarity_file == file),]
  plot <- ggplot(data=subdata[which(subdata$using_fpt == "False"),],aes(x=threshold,y=time))+
    geom_point(aes(color=program))+
    geom_line(aes(color=program))+
    #geom_boxplot(aes(color=program)) +
    #scale_y_log10() + 
    ggtitle(file)
  print(plot)
}
#ggplot(data=data[which(data$using_fpt == "False"),],aes(x=similarity_file,y=time))+
#  geom_boxplot(aes(color=program)) +
#  scale_y_log10() + 
#  theme(axis.text.x = element_text(angle = 15, hjust = 1))
#
#ggplot(data=data[(which(data$using_fpt == "False") && which(data$similarity_file == "actino.sim")),],aes(x=similarity_file,y=time))+
#  geom_point() + 
#  #geom_boxplot(aes(color=program)) +
#  scale_y_log10() + 
#  theme(axis.text.x = element_text(angle = 15, hjust = 1))
