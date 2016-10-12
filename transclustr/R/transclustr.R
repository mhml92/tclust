tclust_params <- function(){
   list(
      normalization = "RELATIVE",
      use_custom_fallback = FALSE,
      sim_fallback        = 0.0,
      use_default_interval= TRUE,

      th_min              = 0.0,
      th_max              = 100,
      th_step             = 1.0,

      p                   = 1.0,
      f_att               = 100.0,
      f_rep               = 100.0,
      R                   = 100,
      dim                 = 3,
      start_t             = 100,

      d_init              = 0.01,
      d_maximal           = 5.0,
      s_init              = 0.01,
      f_s                 = 0.01,

      fpt_time_limit      = 20,
      fpt_max_cost        = 5000,
      fpt_step_size       = 10,

      disable_force       = FALSE,
      disable_fpt         = FALSE,

      seed                = 42
   )
}
#'tclust
#'@examples
#'library(transclustr)
#'library(ggplot2)
#'library(plyr)
#'
#'SD <- 1.3
#'CLUSTER_SIZE <- 200
#'a <- cbind(rnorm(CLUSTER_SIZE, mean = 10, sd = SD), rnorm(n  =CLUSTER_SIZE,  mean = 10,sd = SD))
#'b <- cbind(rnorm(CLUSTER_SIZE, mean = 0,  sd = SD),  rnorm(n =CLUSTER_SIZE, mean = 10,sd = SD))
#'c <- cbind(rnorm(CLUSTER_SIZE, mean = 10, sd = SD), rnorm(n  =CLUSTER_SIZE,  mean = 0, sd = SD))
#'d <- cbind(rnorm(CLUSTER_SIZE, mean = 0,  sd = SD),  rnorm(n =CLUSTER_SIZE, mean = 0, sd = SD))
#'e <- cbind(rnorm(CLUSTER_SIZE, mean = 5,  sd = SD),  rnorm(n =CLUSTER_SIZE, mean = 5, sd = SD))
#'#
#'set <- rbind(a,b,c,d, e)
#'df_set <- as.data.frame(set)
#'names(df_set) <- c("x","y")
#'ggplot(data=df_set,aes(x=x,y=y)) +geom_point()
#'
#'
#'dist_set <- dist(set)
#'max(dist_set)
#'
#'for(norm in c("RELATIVE","ABSOLUTE")){
#'   tclust_res<- tclust(dist_set, disable_fpt = TRUE, d_maximal = 5,dim = 3,R = 100,threshold = 17,normalization = norm)
#'
#'   set_res <- as.data.frame(cbind(set,tclust_res$clusters[[1]]))
#'   names(set_res) <- c("x","y","cluster")
#'
#'   find_hull <- function(df) df[chull(df$x, df$y), ]
#'   hulls <- ddply(set_res, "cluster", find_hull)
#'
#'   length(unique(tclust_res$clusters[[1]]))
#'   print(ggplot(data=as.data.frame(set_res),aes(x=x,y=y,colour=factor(cluster),fill=factor(cluster))) +
#'            geom_point() +
#'            theme(legend.position = "none") +
#'            geom_polygon(data = hulls,alpha = 0.25) +
#'            ggtitle(paste("Normalization: ",norm,", Cost: ",tclust_res$costs[[1]]))
#'   )
#'}
#'@export
tclust <- function(
   dist_obj = NULL,
   filename = NULL,
   simmatrix = NULL,
   threshold = NULL,
   convert_dissimilarity_to_similarity = TRUE,
   file_type = "SIMPLE",
   normalization = "RELATIVE",

   use_custom_fallback = FALSE,
   sim_fallback        = 0.0,

   use_default_interval = TRUE,
   th_min              = 0.0,
   th_max              = 100,
   th_step             = 1.0,

   p                   = 1.0,
   f_att               = 100.0,
   f_rep               = 100.0,
   R                   = 100,
   dim                 = 3,
   start_t             = 100,

   d_init              = 0.01,
   d_maximal           = 5.0,
   s_init              = 0.01,
   f_s                 = 0.01,

   fpt_time_limit      = 20,
   fpt_max_cost        = 5000,
   fpt_step_size       = 10,
   disable_force       = FALSE,
   disable_fpt         = FALSE,

   seed                = 42
)
{
   params <- tclust_params()
   params$normalization         = normalization
   params$use_custom_fallback    = use_custom_fallback
   params$sim_fallback           = sim_fallback
   params$use_default_interval   = use_default_interval
   params$th_min                 = th_min
   params$th_max                 = th_max
   params$th_step                = th_step
   params$p                      = p
   params$f_att                  = f_att
   params$f_rep                  = f_rep
   params$R                      = R
   params$dim                    = dim
   params$start_t                = start_t
   params$d_init                 = d_init
   params$d_maximal              = d_maximal
   params$s_init                 = s_init
   params$f_s                    = f_s
   params$fpt_time_limit         = fpt_time_limit
   params$fpt_max_cost           = fpt_max_cost
   params$fpt_step_size          = fpt_step_size
   params$disable_force          = disable_force
   params$disable_fpt            = disable_fpt
   params$seed                   = seed

   # if all input arguments are NULL
   if(is.null(filename) && is.null(simmatrix) && is.null(dist_obj)){
      stop("Either a file, similarity matrix or dist object needed")
   }
   if(!is.null(threshold)){
      params$use_default_interval = FALSE
      params$th_min <- threshold
      params$th_max <- threshold
      params$th_step <- 100
   }

   if(!is.null(filename)){
      ##########################################################################
      # FILE
      ##########################################################################
      if(!file.exists(filename)){
         stop("could not find file")
      }
      # file path must be absolute
      filename <- tools::file_path_as_absolute(filename)

      res <- cppTransClustFile(
         filename,
         file_type,
         params
      )
   }else if(!is.null(dist_obj)){
      ##########################################################################
      # DIST OBJECT
      ##########################################################################

      # convert dissimilarity to similarity
      if(convert_dissimilarity_to_similarity){
         dist_obj <- max(dist_obj) - dist_obj
         #d <- max(as.vector(dist_obj))-as.vector(dist_obj)
      }

      res <- cppTransClustDist(
         as.vector(dist_obj),
         as.numeric(attr(dist_obj,"Size")),
         params
      )

   }else if(!is.null(simmatrix)){
      ##########################################################################
      # SIMILARITY MATRIX
      ##########################################################################
      dist_obj <- as.dist(simmatrix)
      # convert dissimilarity to similarity
      if(convert_dissimilarity_to_similarity){
         d <- max(as.vector(dist_obj))-as.vector(dist_obj)
      }else{
         d <- as.vector(dist_obj)
      }
      res <- cppTransClustDist(
         d,
         as.numeric(attr(dist_obj,"Size")),
         params
      )
   }
   res
}

