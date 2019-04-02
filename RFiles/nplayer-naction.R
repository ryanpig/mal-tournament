library("RSQLite")
sysPath = "~/CppProj/mal-tournament/"

players <- c(2:10)
actions <- c(2:4)

all <- array(0.0, c(length(players), 10, length(actions)))
mal <- c("FP", "NoRegret")
bandit <- c("QL", "Softmax", "NGreedy", "EGreedy", "Satisficing", "UCB1", "EXP3")
totaldata <- numeric(0)
# connect to the sqlite file
for(a in actions)
{
  combined_players <- matrix(0.0, nrow = length(players), ncol = 10)
  rownames(combined_players) <- players
  for(n in players)
  {
    dbname = paste(sysPath, paste(paste(paste("Result_nplayer_all/result_",n ,sep=""), "p", sep=""), a, sep=""), "a.db", sep="")
    con <- dbConnect(drv=RSQLite::SQLite(), dbname=dbname)
    tables <- dbListTables(con)
    tables <- tables[tables != "sqlite_sequence"]
    lDataFrames <- vector("list", length=length(tables))
    for (i in seq(along=tables)) {
      DF <- dbGetQuery(conn=con, statement=paste("SELECT * FROM '", tables[[i]], "'", sep=""))
    }
    #instances[n-1,1] = nrow(DF)
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
    combined_players[n - 1, ] <- colMeans(combined_final, na.rm=TRUE)  
    colnames(combined_players) <- c(algorithms)
    dbDisconnect(con)
  }
  
  #if(a == 4)
  #{
  #  combined_players[c(8:9),] = 100
  #}

  ## Grouping
  data=as.matrix(combined_players)
  MAL_algorithms <- rowSums(data[, mal]) / length(mal)
  Bandit_algorithms <- rowSums(data[, bandit]) / length(bandit)
  data <- cbind(MAL_algorithms, Bandit_algorithms)
  totaldata <- cbind(totaldata, data)

  all[,,a-1] = combined_players
}

#heatmap
# plotly library
library(plotly)

# Data: mtcars:
#head(combined_players)
#data=as.matrix(combined_players)

#basic heatmap
## Drawing
  x <- list(
    title = "Algorithms in 2,3,4 actions"
    )
  y <- list(
    title = "Players"
  )
  data = cbind(cbind(as.matrix(all[,,1]),as.matrix(all[,,2])), as.matrix(all[,,3])) 
  addActionStr<-function(n)
  {
    n <- paste(n, a, sep="")
  }
  tmp_col <- numeric(0)
  for(a in actions)
  {
    tmp <- lapply(algorithms, addActionStr)
    tmp_col <- c(tmp_col, tmp)
  }
  colnames(data) <- tmp_col
  
  rownames(data) <- c(2:10)
  title_str <- paste(paste('Algorithms v.s. n-playes - ', "2,3,4", sep=""), "-actions", sep="")
  plot_ly(x=colnames(data), y=rownames(data), z = data, type = "heatmap") %>% layout(title = title_str, xaxis=x, yaxis=y)
  

# Grouping
x <- list(
  title = "MAL/Bandit algorithms in 2,3,4 actions"
)
y <- list(
  title = "Players"
)
title_str <- "MAL/Bandit algorithms v.s. n-players"
colnames(totaldata) <- c("MAL_2A","Bandit_2A", "MAL_3A","Bandit_3A", "MAL_4A","Bandit_4A" )
plot_ly(x=colnames(totaldata), y=rownames(totaldata), z =totaldata, type = "heatmap") %>%  layout(title = title_str, xaxis=x, yaxis=y)


## 
library("mosaic")#favstats
favstats(avg_DF ~ type_p0,data=final)
favstats(data$MAL_algorithms ~ MAL_algorithms,data=data[, "MAL_algorithms"])

##
#ins = instances[,1]      # the eruption durations 
#exe = instances[,2]         # the waiting interval 
#plot(ins, exe, type="b", xlab="Instances", ylab="Execution Time")
#abline(lm(ins ~ exe)) 






