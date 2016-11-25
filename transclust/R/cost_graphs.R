library(ggplot2)
library(stringr)
library(gridExtra)
setwd("~/Dropbox/Datalogi/Speciale/repo/transclust/data/results/")

parse_data <- function(data, name,include_fpt = F){
  programs <- unique(data$program)
  fpt <- unique(data$fpt)
  if(!include_fpt){
    fpt <- c("False")
  }
  for(program in programs){
    for(using_fpt in fpt){
      res <- data[which(data$program == program),]
      res <- data[which(res$fpt == using_fpt),]
      if(using_fpt == "True"){
        res$program <- paste(name,program,"wFPT",sep="_")
      }else{
        res$program <- paste(name,program,sep="_")
      }
      if(exists("result")){
        result <- rbind(result,res)
      }else{
        result <- res
      }
    }
  }
  result
}

# Read cost logs
## JAVA BASELINE COST
#java_tc <- read.table(file("2016-11-19--18-09-31__java_tc2/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
#java_tc_new <- read.table(file("2016-11-20--16-43-32__java_2.0.1/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
#
## bad normalization 0:1 and no check for fallback-threshold is smallest value
#cpp_bad_norm <- read.table(file("2016-11-21--10-46-20__cpp_bad_norm/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
cpp_normal_force <- read.table(file("2016-11-21--10-59-22__cpp_normal_force/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
#cpp_no_force <- read.table(file("2016-11-21--13-38-17__cpp_no_force/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
#cpp_no_force_small_gml <- read.table(file("2016-11-21--13-49-18__cpp_no_force_gml_small/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
#cpp_no_force_small_gml_vsmall_gml <- read.table(file("2016-11-21--14-00-38__cpp_no_force_gml_small_diff_seed_vsmall_gml/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
#cpp_no_force_small_gml_diff_seed <- read.table(file("2016-11-21--13-56-02__cpp_no_force_gml_small_diff_seed/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
java2_1_vs_cpp <- read.table(file("2016-11-22--13-25-25__jtc2.1_vs_cpp/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
cpp_alt_force <- read.table(file("2016-11-22--15-33-58__cpp_alt_force/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
cpp_better_force <- read.table(file("2016-11-23--10-29-40__better_force/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
java_fixed <- read.table(file("2016-11-24--14-14-28__test_run/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]
cpp_square_dist <- read.table(file("2016-11-25--10-41-05__square_dist/costs.log"), header=TRUE, sep="\t",strip.white = T)[,1:6]


data <- rbind(
  #parse_data(cpp_bad_norm,"cpp_bad_norm"),
  #parse_data(cpp_no_force_small_gml_vsmall_gml,"no_force_vsmall_gml"),
  parse_data(cpp_normal_force,"normal_force"),
  parse_data(java_fixed,"java_tc"),
  #parse_data(java2_1_vs_cpp[which(java2_1_vs_cpp$program == "cpp"),],"cpp"),
  #parse_data(java2_1_vs_cpp[which(java2_1_vs_cpp$program == "java"),],"java"),
  parse_data(cpp_square_dist,"square_dist")
  #parse_data(java_tc_new,"java_tc_new"),
)

files = unique(data$filename)
options("scipen"=100, "digits"=4)
#pdf("tc_comparison.pdf")
for(f in files){
  plot <- ggplot(data = data[which(data$filename==f),],aes(x=threshold,y=cost,colour = program)) +
    geom_line(alpha = 0.5) +
    geom_point(alpha = 0.5) +
    #scale_y_log10() +
    ggtitle(paste0(f,sep=" ")) 
  print(plot)
}
#dev.off()
