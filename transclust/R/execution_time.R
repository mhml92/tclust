library(ggplot2)
library(stringr)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/data/results/large_java_cpp_test_2016-11-12--20-03-20/")
data <- read.table(file("execution_duration.log"), header=TRUE, sep="\t")
options(scipen=999)

ggplot(data=data[which(data$using_fpt == "False"),],aes(x=similarity_file,y=time))+
  geom_boxplot(aes(color=program)) +
  scale_y_log10() + 
  theme(axis.text.x = element_text(angle = 15, hjust = 1))

ggplot(data=data[(which(data$using_fpt == "False") && which(data$similarity_file == "actino.sim")),],aes(x=similarity_file,y=time))+
  geom_point() + 
  #geom_boxplot(aes(color=program)) +
  scale_y_log10() + 
  theme(axis.text.x = element_text(angle = 15, hjust = 1))
