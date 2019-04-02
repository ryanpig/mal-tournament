
for i in `seq 2 10`
do
 mv result_${i}p.db result_${i}p2a.db
 echo "mv ${i}"
done
