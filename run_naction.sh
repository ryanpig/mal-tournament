#!/bin/sh
# compiling
make -j8
total_strategies=10
# for n in 2 3 4 5 6 7 8 9 10
# run 
actions=4
# for n in 2 3 4 5 6 7 8 9 10
for n in 9  
do 
  # . clean.sh
  echo $(date) >> report_nplayer_naction.txt
  start=`date +%s`
  time ./mal -q true -p $n -v $total_strategies -a $actions -m false
  cp Result/result2.db Result_nplayer/result_${n}p${actions}a.db
  cp log_file/program.log log_file/program_a4_$n
  cp error_report.txt log_file/error_report_a4_$n
  end=`date +%s`
  echo "finish ${n} player tournament" >> report_nplayer_naction.txt
  runtime=$((end-start))
  echo $runtime >> report_nplayer_naction.txt
done

