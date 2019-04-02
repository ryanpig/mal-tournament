library("RSQLite")
sysPath = "~/CppProj/mal-tournament/"

# connect to the sqlite file
con <- dbConnect(drv=RSQLite::SQLite(), dbname=paste(sysPath, "Result/result2.db", sep=""))
# con <- dbConnect(drv=RSQLite::SQLite(), dbname=paste(sysPath, "Result_nplayer/result_2p.db", sep=""))

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
  total <- total + tmp[, paste("payoff_p", 0,sep="")]
}
avg_DF <- total[] / total_iteration * 100
new_DF <- tmp[, c("gametype", "type_p0", "type_p1")]
final <- cbind(new_DF,avg_DF)

# games v.s. algorithms
gametypes <- unique(final$gametype)
algorithms <- unique(final$type_p0)
##combined_final <- data_frame("gametype", "actor", "avg")

combined_final <- matrix(0.0, nrow = length(gametypes), ncol = length(algorithms))
colnames(combined_final) <- c(algorithms)
rownames(combined_final) <- c(gametypes)

for(i in seq(length(gametypes))){
  for(j in seq(length(algorithms))){
    tmp3 <-subset(final, final$gametype== gametypes[i]& final$type_p0 == algorithms[j])
    combined_final[i,j] = mean(tmp3$avg_DF)
  }
}

#heatmap
# plotly library
library(plotly)

# Data: mtcars:
head(combined_final)
data=as.matrix(combined_final)

#basic heatmap
  # layout
  x <- list(
    title = "Algorithms in 2-player, 2-action games"
  )
  y <- list(
    title = "Game types"
  )
  title_str <- "Performance of algorithms in 2-player, 2-action games"
png(paste(sysPath,"img/heatmap_game_algorithms.png", sep=""), 640,640)
plot_ly(x=colnames(data), y=rownames(data), z = data, type = "heatmap")  %>% layout(title = title_str, xaxis=x, yaxis=y)
dev.off()

# Grouping
  # layout
  x <- list(
    title = "MAL/Bandit Algorithms in 2-player, 2-action games"
  )
  y <- list(
    title = "Game types"
  )
  title_str <- "Performance of MAL/Bandit algorithms in 2-player, 2-action games"

mal <- c("FP", "NoRegret")
bandit <- c("QL", "Softmax", "NGreedy", "EGreedy", "Satisficing", "UCB1", "EXP3")
MAL_algorithms <- rowSums(data[, mal]) / length(mal)
Bandit_algorithms <- rowSums(data[, bandit]) / length(bandit)
data = cbind(MAL_algorithms, Bandit_algorithms)
png(paste(sysPath,"img/heatmap_game_algorithms_by_group.png", sep=""), 640,640)
plot_ly(x=colnames(data), y=rownames(data), z = data, type = "heatmap")  %>% layout(title = title_str, xaxis=x, yaxis=y)
dev.off()


## 
library("mosaic")#favstats
favstats(avg_DF ~ type_p0,data=final)
favstats(data$MAL_algorithms ~ MAL_algorithms,data=data[, "MAL_algorithms"])


## finish 
dbDisconnect(con)
c <- vector("list", length=length(gametypes))
for(i in seq(length(gametypes))){
  tmp <- nrow(subset(final, gametype == gametypes[i]))
  c[i] = tmp
}


         
       

