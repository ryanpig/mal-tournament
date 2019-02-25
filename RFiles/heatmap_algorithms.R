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
avg_DF <- total[] / total_iteration * 100
new_DF <- tmp[, c("gametype", "type_p0", "type_p1")]
final <- cbind(new_DF,avg_DF)

# games v.s. algorithms
gametypes <- unique(final$gametype)
algorithms <- unique(final$type_p0)
##combined_final <- data_frame("gametype", "actor", "avg")

combined_final <- matrix(0.0, nrow = length(algorithms), ncol = length(algorithms))
colnames(combined_final) <- c(algorithms)
rownames(combined_final) <- c(algorithms)

for(i in seq(length(algorithms))){
  for(j in seq(length(algorithms))){
    tmp3 <-subset(final, final$type_p0== algorithms[i]& final$type_p1 == algorithms[j])
    combined_final[i,j] = mean(tmp3$avg_DF)
  }
}

#heatmap
# plotly library
library(plotly)

# Data: mtcars:
head(combined_final)
data=as.matrix(combined_final)

# Basic heatmap
plot_ly(x=colnames(data), y=rownames(data), z = data, type = "heatmap")


## 
library("mosaic")#favstats
favstats(avg_DF ~ type_p0,data=final)

## finish 
dbDisconnect(con)

