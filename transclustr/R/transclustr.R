tclust_params <- function(){
   list(
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
#'@export
tclust <- function(
   dist_obj = NULL,
   filename = NULL,
   simmatrix = NULL,
   threshold = NULL,
   convert_dissimilarity_to_similarity = TRUE,
   file_type = "SIMPLE",

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

