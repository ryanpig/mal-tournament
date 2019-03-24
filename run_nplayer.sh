#!/bin/sh
# compiling
make -j8
total_strategies=10
# for n in 2 3 4 5 6 7 8 9 10
# run 
for n in 2 3 4 5 6 7 8 9 10
# for n in 2 
do 
  . clean.sh
  echo $(date) >> report_nplayer.txt
  start=`date +%s`
  time ./mal -q true -p $n -v $total_strategies 
  cp Result/result2.db Result_nplayer/result_${n}p.db
  cp log_file/program.log log_file/program_$n
  cp error_report.txt log_file/error_report_$n
  end=`date +%s`
  echo "finish ${n} player tournament" >> report_nplayer.txt
  runtime=$((end-start))
  echo $runtime >> report_nplayer.txt
done

