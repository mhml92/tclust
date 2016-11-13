library(ggplot2)
library(stringr)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/data/results/test_run_2016-11-12--16-09-21/")
data <- read.table(file("costs.log"), header=TRUE, sep="\t")
options(scipen=999)

fpt_data = data[which(data$fpt == "True"),]
fpt_data_cpp <- fpt_data[which(fpt_data$program == "cpp"),]
fpt_data_cpp$program <- "cpp_fpt"
fpt_data_java <-fpt_data[which(fpt_data$program == "java"),]
fpt_data_java$program <- "java_fpt"

fpt_data <- rbind(fpt_data_cpp,fpt_data_java)

force_data = data[which(data$fpt == "False"),]
force_data_cpp <- force_data[which(force_data$program == "cpp"),]
force_data_cpp$program <- "cpp_force"
force_data_java <-force_data[which(force_data$program == "java"),]
force_data_java$program <- "java_force"

data <- rbind(
  fpt_data,
  force_data_cpp,
  force_data_java
  )

#data_cpp <- data[which(grepl("cpp", data$program)),]
#data_java <- data[which(grepl("java", data$program)),]
#data <- data_cpp

files = unique(data$filename)
for(f in files){
  plot <- ggplot(data = data[which(data$filename==f),],aes(x=threshold,y=cost,colour = program)) +
    geom_line(alpha = 0.75) +
    geom_point(alpha = 0.5) +
    #scale_y_log10() +
    ggtitle(f)
  print(plot)
}
