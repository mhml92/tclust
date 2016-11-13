library(ggplot2)
library(stringr)
data <- read.table(file("stdin"), header=TRUE, sep="\t")
data <- cbind(data,files = paste(data$file,"vs.",data$gs_file))
plot <- ggplot(data,aes(x=threshold,y=cost, colour = factor(file))) + 
   theme(legend.position="bottom",legend.direction="vertical")+
   geom_line() + geom_point(size=.5) + scale_y_log10()
plot
