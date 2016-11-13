library(ggplot2)
library(stringr)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/data/results/test_run_2016-11-11--09-54-17/")
data <- read.table(file("execution_duration.log"), header=TRUE, sep="\t")
options(scipen=999)

ggplot(data=data[which(data$using_fpt == "False"),],aes(x=similarity_file,y=time))+
  geom_boxplot(aes(color=program)) +
  scale_y_log10() + 
  theme(axis.text.x = element_text(angle = 15, hjust = 1))
