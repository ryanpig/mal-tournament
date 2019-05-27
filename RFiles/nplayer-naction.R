library("RSQLite")
sysPath = "~/CppProj/mal-tournament/"
flag_export_tables = FALSE
flag_draw_execution_time = FALSE
flag_draw_all = TRUE
flag_draw_group = TRUE
flag_plotline = FALSE
flag_kstest = TRUE

players <- c(2:10)
actions <- c(2:4)

all <- array(0.0, c(length(players), 10, length(actions)))
mal <- c("FP", "NoRegret")
bandit <- c("QL", "Softmax", "NGreedy", "EGreedy", "Satisficing", "UCB1", "EXP3")
totaldata <- numeric(0)

# for instance v.s. running time
instances <- matrix(0.0, nrow = length(players), ncol = 6)
#colnames(instances) <- c("instances", "executionTime")
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
    #dbname = paste(sysPath, paste(paste(paste("Result_nplayer_0413/result_",n ,sep=""), "p", sep=""), a, sep=""), "a.db", sep="")
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

#basic heatmap
if(flag_draw_all){
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
}

# Grouping
if(flag_draw_group){
  x <- list(
    title = "MAL/Bandit algorithms in 2,3,4 actions"
  )
  y <- list(
    title = "Players"
  )
  title_str <- "MAL/Bandit algorithms v.s. n-players"
  colnames(totaldata) <- c("MAL_2A","Bandit_2A", "MAL_3A","Bandit_3A", "MAL_4A","Bandit_4A" )
  plot_ly(x=colnames(totaldata), y=rownames(totaldata), z =totaldata, type = "heatmap") %>%  layout(title = title_str, xaxis=x, yaxis=y)
}

# Descriptive statistics 
#library("mosaic")#favstats
#favstats(all ~ type_p0,data=final)
#favstats(data$MAL_algorithms ~ MAL_algorithms,data=data[, "MAL_algorithms"])

# Running time v.s. instances
if(flag_draw_execution_time){
  ins = instances[,c(1:3)]      # the eruption durations 
  exe = instances[,c(4:6)]         # the waiting interval 
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
}

# export to latex table
if(flag_export_tables){
  library(xtable)
  path <- paste(sysPath, "/tables/groupdata.tex", sep="")
  print(xtable(totaldata, type = "latex"), file = path)
  
  path <- paste(sysPath, "/tables/totaldata2a.tex", sep="")
  tmp <- data[,c(1:10)] 
  colnames(tmp) <- algorithms
  print(xtable(tmp , type = "latex"), file = path)
  
  path <- paste(sysPath, "/tables/totaldata3a.tex", sep="")
  tmp <- data[,c(11:20)]
  colnames(tmp) <- algorithms
  print(xtable(tmp, type = "latex"), file = path)
  
  path <- paste(sysPath, "/tables/totaldata4a.tex", sep="")
  tmp <- data[,c(21:30)]
  colnames(tmp) <- algorithms
  print(xtable(tmp, type = "latex"), file = path)
}

if(flag_plotline)
{
  lends <- c("MAL","Bandit")
  diff <- totaldata[,2] - totaldata[,1]
  matplot(players, totaldata[,c(1:2)], type="b", 
          xlab="the number of players", ylab="average accumulated payoffs",sub="MAL v.s. Bandin in 2 actions", 
          lty=1, pch=15, col=1:2)

  
  d = data.frame(x =players,
                 n = totaldata[,c(1)],
                 n2 = totaldata[,c(2)],
                 diff = totaldata[,1] - totaldata[,2])
  with(d, plot(players, n, type="b", col="red3", 
               ylab="average payoffs",
               ylim=c(60,80)))
  par(new = T)
  with(d, plot(players, n2, type="b", col="blue3", pch=20,
               ylab=NA,
               ylim=c(60,80)))
  par(new = T)
  with(d, plot(x, diff, pch=16, axes=F, xlab=NA, ylab=NA, cex=1.2, type="l"))
  axis(side = 4)
  mtext(side = 4, line = 2, "Diff. payoffs between MAL and Bandit")
  legend("topright",
         legend=c("MAL in 2 actions","Bandit in 2 actions","Difference between MAL and Bandit."),
         lty=c(1,1,1), pch=c(1, 20, NA), col=c("red3", "blue3", "black"))
  
  ## 3 actions
  d = data.frame(x =players,
                 n = totaldata[,c(3)],
                 n2 = totaldata[,c(4)],
                 diff = totaldata[,3] - totaldata[,4])
  with(d, plot(players, n, type="b", col="red3", 
               ylab="average payoffs",
               ylim=c(50,80)))
  par(new = T)
  with(d, plot(players, n2, type="b", col="blue3", pch=20,
               ylab=NA,
               ylim=c(50,80)))
  par(new = T)
  with(d, plot(x, diff, pch=16, axes=F, xlab=NA, ylab=NA, cex=1.2, type="l", ylim=c(-1,3)))
  axis(side = 4)
  mtext(side = 4, line = 2, "Diff. payoffs between MAL and Bandit")
  legend("topright",
         legend=c("MAL in 3 actions","Bandit in 3 actions","Difference between MAL and Bandit."),
         lty=c(1,1,1), pch=c(1, 20, NA), col=c("red3", "blue3", "black"))
  
  
  # 4 actions
  
  d = data.frame(x =players[1:7],
                 n = totaldata[c(1:7), 5],
                 n2 = totaldata[c(1:7), 6],
                 diff = totaldata[c(1:7), 5]- totaldata[c(1:7), 6])
  with(d, plot(x, n, type="b", col="red3", 
               ylab="average payoffs",
               ylim=c(50,80)))
  par(new = T)
  with(d, plot(x, n2, type="b", col="blue3", pch=20,
               ylab=NA,
               ylim=c(50,80)))
  par(new = T)
  with(d, plot(x, diff, pch=16, axes=F, xlab=NA, ylab=NA, cex=1.2, type="l"))
  axis(side = 4)
  mtext(side = 4, line = 2, "Diff. payoffs between MAL and Bandit")
  legend("topright",
         legend=c("MAL in 4 actions","Bandit in 4 actions","Difference between MAL and Bandit."),
         lty=c(1,1,1), pch=c(1, 20, NA), col=c("red3", "blue3", "black"))
  
  
  
}

if(flag_kstest)
{
  # choose alternative hypothesis
  alt_hypo = c("two.sided", "less", "greater")
  #alt_hypo = alt_hypo[1] # two.sided
   alt_hypo = alt_hypo[2] # less
  
  # start KS-tests for 2-action, 3-action, 4-action between MAL and MAB.
  tot <- numeric(0)
  res <- ks.test(totaldata[,1], totaldata[,2], alternative = alt_hypo)
  res <- res[1:2]
  tot <- cbind(tot, res)
  
  res <- ks.test(totaldata[,3], totaldata[,4], alternative = alt_hypo)
  res <- res[1:2]
  tot <- cbind(tot, res)
  
  res <- ks.test(totaldata[1:7,5], totaldata[1:7,6], alternative = alt_hypo)
  res <- res[1:2]
  tot <- cbind(tot, res)
  
  colnames(tot) <- c ("2-action", "3-action", "4-action")

  library(xtable)
  #path <- paste(sysPath, "/tables/kstest-groupdata-two.tex", sep="")
  path <- paste(sysPath, "/tables/kstest-groupdata-less.tex", sep="")
  print(xtable(tot, type = "latex"), file = path)

  ## ggplot2 for ks test
  # simulate two distributions - your data goes here!
  library(ggplot2)
  #sample1 <- rnorm(10000, 10, 5)
  #sample2 <- rnorm(10000, 1, 5)
  sample1 <- totaldata[,1] 
  sample2 <- totaldata[,2]
  group <- c(rep("sample1", length(sample1)), rep("sample2", length(sample2)))
  dat <- data.frame(KSD = c(sample1,sample2), group = group)
  # create ECDF of data
  cdf1 <- ecdf(sample1) 
  cdf2 <- ecdf(sample2) 
  # find min and max statistics to draw line between points of greatest distance
  minMax <- seq(min(sample1, sample2), max(sample1, sample2), length.out=length(sample1)) 
  x0 <- minMax[which( abs(cdf1(minMax) - cdf2(minMax)) == max(abs(cdf1(minMax) - cdf2(minMax))) )] 
  y0 <- cdf1(x0) 
  y1 <- cdf2(x0) 
  ## plot
  ggplot(dat, aes(x = KSD, group = group, color = group))+
    stat_ecdf(size=1) +
    theme_bw(base_size = 28) +
    theme(legend.position ="top") +
    xlab("Sample") +
    ylab("ECDF") +
    #geom_line(size=1) +
    geom_segment(aes(x = x0[1], y = y0[1], xend = x0[1], yend = y1[1]),
                 linetype = "dashed", color = "red") +
    geom_point(aes(x = x0[1] , y= y0[1]), color="red", size=8) +
    geom_point(aes(x = x0[1] , y= y1[1]), color="red", size=8) +
    ggtitle("K-S Test: Sample 1 / Sample 2") +
    theme(legend.title=element_blank())
  
}

# spearman's correlation
library(ggplot2)
ggplot(totaldata, aes(x=players, y=totaldata[,1])) + 
  geom_point(color='#2980B9', size = 4) + 
  geom_smooth(method=lm, se=FALSE, fullrange=TRUE, color='#2C3E50')

plot(players, totaldata[,1], main="players v.s. average rewards", 
     xlab="The number of players", ylab="Average rewards", type="l")

## the number of players(+) => the average rewards (-)

tot_all <- numeric(0)
for(i in c(1:6))
{
  if (i == 5 || i == 6) {
    corr <- cor.test(x=c(1:7), y=totaldata[1:7,i], method = 'spearman', alternative = "less")
  } else {
    corr <- cor.test(x=players, y=totaldata[,i], method = 'spearman', alternative = "less")
  }
  tot_all <- rbind(tmp2, corr[c(1,3,4)])
  
}
tot_all <- tot_all[1:6, ]
## Save as a latex table
library(xtable)
path <- paste(sysPath, "/tables/spearman-player-payoffs.tex", sep="")
print(xtable(tot_all, digits = -3,  type = "latex"), file = path)

####-----------------------
## the number of actions(+) => the average rewards (-)
tmp5 <- colSums(totaldata) / nrow(totaldata)

tot_all <- numeric(0)
corr1 <- cor.test(x=actions, y=tmp5[c(1,3,5)], method = 'spearman', alternative = "less", exact= TRUE)
corr2 <- cor.test(x=actions, y=tmp5[c(2,4,6)], method = 'spearman', alternative = "less", exact= TRUE)

tot_all <- rbind(tot_all, corr1[c(1,3,4)])
tot_all <- rbind(tot_all, corr2[c(1,3,4)])

## Save as a latex table
library(xtable)
path <- paste(sysPath, "/tables/spearman-action-payoffs.tex", sep="")
print(xtable(tot_all, digits = -3,  type = "latex"), file = path)

####-----------------------
## the number of players(+) => difference between MAL and MAB (-)
diff <- numeric(0)
pr = c(1:7)
diff <- cbind(diff, totaldata[pr,1] - totaldata[pr,2])
diff <- cbind(diff, totaldata[pr,3] - totaldata[pr,4])
diff <- cbind(diff, totaldata[pr,5] - totaldata[pr,6])
corr <- cor.test(x=pr, y=diff[,1], method = 'spearman', alternative = "less")

tot_all <- numeric(0)
alt = "less"
for(i in c(1:3))
{
  if(i == 3)
    corr <- cor.test(x=c(1:7), y=diff[c(1:7),i], method = 'spearman', alternative = alt)
  else
    corr <- cor.test(x=pr, y=diff[,i], method = 'spearman', alternative = alt)
  tot_all <- rbind(tot_all, corr[c(1,3,4)])
}
alt = "greater"
for(i in c(1:3))
{
  if(i == 3)
    corr <- cor.test(x=c(1:7), y=diff[c(1:7),i], method = 'spearman', alternative = alt)
  else
    corr <- cor.test(x=pr, y=diff[,i], method = 'spearman', alternative = alt)
  tot_all <- rbind(tot_all, corr[c(1,3,4)])
}


