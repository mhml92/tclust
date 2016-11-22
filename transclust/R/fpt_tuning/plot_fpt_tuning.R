library(ggplot2)

# working directory
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/R/fpt_tuning/")

# read data
df <- read.csv2(
  "fpt_test.txt",
  sep = ",",
  header = T,
  dec = ".")

# use nonscientific notation
options("scipen"=999)

################################################################################
# Split input data in FORCE and FPT and add a columns indicating alg
################################################################################

# FORCE dataframe
force <- cbind(
  type = "force",
  cc_size = df$cc_size,
  conflicting_edges = df$num_conflicting_edges,
  time = df$force_time/(1000000000),
  cost = df$force_cost,
  relative_cost = df$force_relative_cost,
  completeness = ((((df$cc_size*df$cc_size)-df$cc_size)/2)-df$num_conflicting_edges)/(((df$cc_size*df$cc_size)-df$cc_size)/2)
  )

# FPT dataframe
fpt <- cbind(
  type = "fpt",
  cc_size = df$cc_size,
  conflicting_edges = df$num_conflicting_edges,
  time = df$fpt_time/(1000000000),
  cost = df$fpt_cost,
  relative_cost = df$fpt_relative_cost,
  completeness = ((((df$cc_size*df$cc_size)-df$cc_size)/2)-df$num_conflicting_edges)/(((df$cc_size*df$cc_size)-df$cc_size)/2)
  )

# combine result 
df_res <- as.data.frame(rbind(force,fpt),stringsAsFactors = F)

# fuck R up the ass untill it understands that numeric values are numeric
df_res[,c(2:7)] <- sapply(df_res[,c(2:7)],as.numeric)

## CC_SIZE VS TIME
ggplot(
  data = df_res,
  aes(x = cc_size,y=time,colour = factor(type))
  ) + 
  geom_point(alpha = 0.25)+  
  geom_hline(aes(yintercept=1))+
  geom_vline((aes(xintercept=200)))+
  scale_y_log10()+
  scale_x_log10() + 
  geom_smooth(alpha = 0.25)+
  ggtitle("Connected Components size vs. time")

## CONFLICTING_EDGES VS TIME
ggplot(
  data = df_res[which(df_res$conflicting_edges <=50000),],
  aes(x = conflicting_edges,y=time)
  ) + 
  #geom_boxplot()+
  geom_point(alpha = 0.25)+  
  scale_y_log10()+
  geom_hline(aes(yintercept=1))+
  geom_vline((aes(xintercept=5000)))+
  scale_x_log10() + 
  geom_smooth()+
  ggtitle("Conflicting edges vs. time")

## CONFLICTING_EDGES VS CC_SIZE
ggplot(
  data = df_res,
  aes(y = conflicting_edges,x=cc_size,colour = factor(type))
  ) + 
  #geom_boxplot()+
  geom_point(alpha = 0.1)+  
  scale_y_log10()+
  geom_hline(aes(yintercept=5000))+
  geom_vline((aes(xintercept=200)))+
  scale_x_log10() + 
  geom_smooth()+
  ggtitle("cc_size vs conflict_edges")

## TIME VS COMPLETENESS
ggplot(
  data = df_res,
  aes(y = time,x=completeness,colour = factor(type))
  ) + 
  #geom_boxplot()+
  geom_point(alpha = 0.1)+  
  scale_y_log10()+
  #geom_hline(aes(yintercept=5000))+
  #geom_vline((aes(xintercept=200)))+
  #scale_x_log10() + 
  geom_smooth()+
  ggtitle("time vs completeness")


## COMPLETENESS vs CC_SIZE
ggplot(
  data = df_res,
  aes(y = cc_size,x=completeness)
  ) + 
  #geom_boxplot()+
  geom_point(alpha = 0.1)+  
  scale_y_log10()+
  #geom_hline(aes(yintercept=5000))+
  #geom_vline((aes(xintercept=200)))+
  #scale_x_log10() + 
  geom_smooth()+
  ggtitle("completeness vs cc_size")
