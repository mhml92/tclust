library(ggplot2)
#data <- read.table(file("stdin"), header=FALSE, sep="\t")
data <- read.table(file("~/out.txt"), header=FALSE, sep="\t")
names(data) <- c('cc_id','threshold','r','x','y')
options(scipen=999)
iter <- unique(data$cc_id)
for(i in iter){
   subdata <- data[which(data$cc_id == i),]
   plot <- ggplot(subdata,aes(x = x, y = y, alpha = r/100)) + 
      geom_point() +
      coord_fixed() + 
		ggtitle(paste("cc id:",i,"threshold:",subdata$threshold,sep=" "))
   plot(plot)
}
