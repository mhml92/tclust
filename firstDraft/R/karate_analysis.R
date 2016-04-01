setwd("~/Dropbox/Datalogi/Speciale/code/firstDraft/R")
library(ggplot2)

brown <- read.table(file("../data/brown/sfld_brown_et_al_amidohydrolases_protein_similarities_for_beh.txt"),header = FALSE)
karate <- read.table(file("../data/karateclub/Zachary_karate_club_similarities.txt"),header = FALSE)
names(karate) <- c("o1","o2","val")
                     
objects <- unique(karate$o1)
max_sim <- numeric()
for(obj in objects){
   o <- karate[which(karate$o1 == obj | karate$o2 == obj),]
   max_sim <- c(max_sim,max(o$val))   
}
print(min(max_sim))
max_sim <- sort(max_sim)
qplot(max_sim)+geom_bar()

obj <- objects[1]
o <- karate[which(karate$o1 == obj | karate$o2 == obj),]
o <- o[order(o$val),]
p <- ggplot(data=o,aes(y = val, x=1:nrow(o)))+ geom_line()
objects <- objects[2:length(objects)]
for(obj in objects){
   o <- karate[which(karate$o1 == obj | karate$o2 == obj),]
   o <- o[order(o$val),]
   p <- p + geom_line(data= o,aes(y = val,x=1:nrow(o)))   
}



