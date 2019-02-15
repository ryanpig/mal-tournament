#/bin/sh
cmd="Rscript statistical_analysis.R"
if [ -f boxplot.png ]; then
  echo "plot file boxplot.png exist!"
  read -p "Do you wish to run this program?" yn

  case $yn in
  [Yy]* ) echo ${cmd} | bash; break;;
  [Nn]* ) echo "exit"; break;;
  esac
else
 echo ${cmd} | bash 
fi
