library("RSQLite")
# connect to the sqlite file
con <- dbConnect(drv=RSQLite::SQLite(), dbname="~/CppProj/mal-tournament/Result/result2.db")
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
  avg_DF <- total[] / total_iteration
  new_DF <- tmp[, c("gametype", "type_p0", "type_p1")]
  final <- cbind(new_DF,avg_DF)

## Drawing
  #png("boxplot.png", 640,480)
  boxplot(avg_DF ~ type_p0, data=final, las=2)
  
  #png("beanplot.png", 640, 480)
  library("ggplot2")
  ggplot(final, aes(x=type_p0, y=avg_DF))+geom_boxplot()+geom_violin(fill='lightblue', alpha=0.5)+geom_jitter(position = position_jitter(width = .1))

## descriptive statistics
  library("mosaic")#favstats
  favstats(avg_DF ~ type_p0,data=final)

## finish 
  dbDisconnect(con)
  #dev.off()
