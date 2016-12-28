library(ggplot2)
library(stringr)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/scripts/")
data <- read.table(file("coryne_time_test_cpp_java.txt"), header=TRUE, sep="\t")
options(scipen=999)
datasets <- unique(data$similarity_file)
datasets <- "actino.sim"
for(file in datasets){
  subdata <- data[which(data$similarity_file == file),]
  
  plot <- ggplot(data=subdata[which(subdata$using_fpt == "False"),],aes(x=threshold,y=time))+
    geom_point(aes(color=program))+
    geom_line(aes(color=program))+
    #geom_boxplot(aes(color=program)) +
    #scale_y_log10() + 
    theme(legend.position = "bottom")+
    labs(title = paste("In memory run time Actino",sep = " "),x="Threshold",y="Time (Seconds)") 
  pdf(
    file = paste("actino_in_ram",".pdf",sep = ""),
    width = 8,
    height = 4
  )
  print(plot)
  dev.off()
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
