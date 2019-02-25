library("RSQLite")
sysPath = "~/CppProj/mal-tournament/"

# connect to the sqlite file
con <- dbConnect(drv=RSQLite::SQLite(), dbname=paste(sysPath, "Result/result2.db", sep=""))
tables <- dbListTables(con)

## exclude sqlite_sequence (contains table information)
tables <- tables[tables != "sqlite_sequence"]

lDataFrames <- vector("list", length=length(tables))

## create a data.frame for each table
for (i in seq(along=tables)) {
  DF <- dbGetQuery(conn=con, statement=paste("SELECT * FROM '", tables[[i]], "'", sep=""))
}
## Average different round payoffs
  total_iteration <- length(unique(DF$round))
## create 0 value data
  total <- subset(DF, DF$round == 0)[, "payoff_p0"]
  total[] = 0
## get average of payoffs
  for(i in seq(total_iteration)){
    tmp <- subset(DF, DF$round == i - 1)
    total <- total + tmp[, paste("payoff_p", i-1,sep="")]
  }
  Average_Payoff <- total[] / total_iteration
  new_DF <- tmp[, c("gametype", "type_p0", "type_p1")]
  final <- cbind(new_DF,Average_Payoff)
  colnames(final)[2] <- "Main_Algorithms"

## Drawing
  png(paste(sysPath,"img/boxplot.png", sep=""), 640,480)
  boxplot(Average_Payoff ~ Main_Algorithms, data=final, las=2,  xlab="Main Algorithms", ylab="Average Payoffs")
  dev.off()
  
  png(paste(sysPath,"img/beanplot.png", sep=""), 640,480)
  library("ggplot2")
  ggplot(final, aes(x=Main_Algorithms, y=Average_Payoff))+geom_boxplot()+geom_violin(fill='lightblue', alpha=0.5)+geom_jitter(position = position_jitter(width = .1))
  dev.off()
  
## descriptive statistics
  library("mosaic")#favstats
  favstats(Average_Payoff ~ Main_Algorithms,data=final)

## finish 
  dbDisconnect(con)
  