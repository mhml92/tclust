library(ggplot2)
df <- read.csv2("~/Dropbox/Datalogi/Speciale/repo/transclust/build/release/gm_cost.tsv", header = F, sep = "\t",dec = ".")
colnames(df) <- c("group","count","dist","cost")
ggplot(data = df,aes(x = dist,y=cost,group = factor(group),colour = factor(group))) + 
  geom_line(alpha=0.2)+
  scale_y_log10() + 
  theme(legend.position="none")
