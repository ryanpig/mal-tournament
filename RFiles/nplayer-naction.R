library("RSQLite")
sysPath = "~/CppProj/mal-tournament/"

players <- c(2:10)
actions <- c(2:4)

all <- array(0.0, c(length(players), 10, length(actions)))
mal <- c("FP", "NoRegret")
bandit <- c("QL", "Softmax", "NGreedy", "EGreedy", "Satisficing", "UCB1", "EXP3")
totaldata <- numeric(0)

# for instance v.s. running time
instances <- matrix(0.0, nrow = length(players), ncol = 6)
colnames(instances) <- c("instances", "executionTime")
instances[,4] = c(62,77,90,105,139, 197,253,323,405) # 2 actions
instances[,5] = c(58,72,93,128,176,241,331,473,653) # 3 actions
instances[,6] = c(59,75,105,152,224,348,532,532,532) # 4 actions

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
    instances[n-1,a-1] = nrow(DF)
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

## Running time v.s. instances
library(plotly)
ins = instances[,c(1:3)]      # the eruption durations 
exe = instances[,c(4:6)]         # the waiting interval 
# plot(ins, exe, type="p", xlab="Instances", ylab="Execution Time", main=title_str) # %>%  layout(title = title_str)
# abline(lm(ins ~ exe)) 

# layout
  x <- list(
    title = "Total game instances"
  )
  y <- list(
    title = "Execution time"
  )
  title_str <- "Instances v.s. Execution time in n-player, m-action"
data2 <- data.frame(x, exe[,1], exe[,2], exe[,3])
plot_ly(data2, x = ~ins[,1], y = ~exe[,1], name = 'n-player, 2-action ', type = 'scatter', mode = 'lines+markers') %>%
  add_trace(x = ~ins[,2], y = ~exe[,2], name = 'n-player, 3-action', mode = 'lines+markers') %>%
  add_trace(x = ~ins[,3], y = ~exe[,3], name = 'n-player, 4-action', mode = 'lines+markers') %>%  layout(title = title_str, xaxis=x, yaxis=y)

# export to latex table
library(xtable)
path <- paste(sysPath, "/tables/totaldata.tex", sep="")
print(xtable(totaldata, type = "latex"), file = path)






