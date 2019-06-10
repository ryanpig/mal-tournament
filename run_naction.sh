#!/bin/sh
# compiling
make -j8
total_strategies=10
# for n in 2 3 4 5 6 7 8 9 10
# run 
outfolder="Result_nplayer_0413"
# actions=2
# # for n in 2 3 4 5 6 7 8 9 10
# for n in 8  
# do 
#   # . clean.sh
#   echo $(date) >> report_nplayer_naction.txt
#   start=`date +%s`
#   time ./mal -q true -p $n -v $total_strategies -a $actions -m false
#   cp Result/result2.db ${outfolder}/result_${n}p${actions}a.db
#   cp log_file/program.log log_file/program_a${actions}_$n
#   cp error_report.txt log_file/error_report_a${actions}_$n
#   end=`date +%s`
#   echo "finish ${n} player tournament" >> report_nplayer_naction.txt
#   runtime=$((end-start))
#   echo $runtime >> report_nplayer_naction.txt
# done

# actions=3
# # for n in 2 3 4 5 6 7 8 9 10
# for n in 8
# do 
#   # . clean.sh
#   echo $(date) >> report_nplayer_naction.txt
#   start=`date +%s`
#   time ./mal -q true -p $n -v $total_strategies -a $actions -m false
#   cp Result/result2.db ${outfolder}/result_${n}p${actions}a.db
#   cp log_file/program.log log_file/program_a${actions}_$n
#   cp error_report.txt log_file/error_report_a${actions}_$n
#   end=`date +%s`
#   echo "finish ${n} player tournament" >> report_nplayer_naction.txt
#   runtime=$((end-start))
#   echo $runtime >> report_nplayer_naction.txt
# done

actions=4
# for n in 2 3 4 5 6 7 8 
for n in 4 
do 
  # . clean.sh
  echo $(date) >> report_nplayer_naction.txt
  start=`date +%s`
  time ./mal -q true -p $n -v $total_strategies -a $actions -m false
  cp Result/result2.db ${outfolder}/result_${n}p${actions}a.db
  cp log_file/program.log log_file/program_a${actions}_$n
  cp error_report.txt log_file/error_report_a${actions}_$n
  end=`date +%s`
  echo "finish ${n} player tournament" >> report_nplayer_naction.txt
  runtime=$((end-start))
  echo $runtime >> report_nplayer_naction.txt
done
