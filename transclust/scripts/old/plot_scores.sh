# BROWN
#{ 
#   python3 cluster_score.py -R -H ../data/brown/cpp_transclust_result.txt   --trans_gs  ../data/brown/transclust_result.txt ; 
#   python3 cluster_score.py -R    ../data/brown/cpp_transclust_result.txt   --col_gs    ../data/brown/sfld_brown_et_al_amidohydrolases_families_gold_standard.txt ; 
#   python3 cluster_score.py -R    ../data/brown/transclust_result.txt       --col_gs    ../data/brown/sfld_brown_et_al_amidohydrolases_families_gold_standard.txt ;
#} | Rscript ../R/plot_cluster_score.R

# BROWN rand_index
{ 
   #python3 cluster_score.py -H --rand_index ../data/brown/cpp_transclust_result.txt   --trans_gs  ../data/brown/transclust_result.txt ; 
   #python3 cluster_score.py    --rand_index ../data/brown/cpp_transclust_result.txt   --col_gs    ../data/brown/sfld_brown_et_al_amidohydrolases_families_gold_standard.txt ; 
   python3 cluster_score.py  -H  --rand_index ../data/brown/cpp_transclust_fpt_and_force_result.txt --col_gs    ../data/brown/sfld_brown_et_al_amidohydrolases_families_gold_standard.txt ;
   python3 cluster_score.py   --rand_index ../data/brown/cpp_transclust_force.txt --col_gs    ../data/brown/sfld_brown_et_al_amidohydrolases_families_gold_standard.txt ;
   #python3 cluster_score.py    --rand_index ../data/brown/transclust_result.txt       --col_gs    ../data/brown/sfld_brown_et_al_amidohydrolases_families_gold_standard.txt ;
   python3 cluster_score.py  	 --rand_index ../data/brown/cpp_transclust_fpt_and_force_result.txt   --trans_gs ../data/brown/cpp_transclust_force.txt ; 
   #python3 cluster_score.py  	 --rand_index ../data/brown/cpp_transclust_force.txt   --trans_gs  ../data/brown/transclust_result.txt ; 
} | Rscript ../R/plot_cluster_score.R

# KARATE rand_index
#{ 
#   python3 cluster_score.py  -H --rand_index   ../data/karateclub/cpp_transclust_result.txt   --trans_gs  ../data/karateclub/transclust_result.txt ; 
#   python3 cluster_score.py  --rand_index      ../data/karateclub/cpp_transclust_result.txt   --col_gs    ../data/karateclub/Zachary_karate_club_gold_standard.txt ; 
#   python3 cluster_score.py  --rand_index      ../data/karateclub/transclust_result.txt       --col_gs    ../data/karateclub/Zachary_karate_club_gold_standard.txt ;
#} | Rscript ../R/plot_cluster_score.R
