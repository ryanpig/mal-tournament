library("RSQLite")
sysPath = "~/CppProj/mal-tournament/"

flag_kstest = FALSE

players <- c(2:10)
actions <- c(2:4)

all <- array(0.0, c(length(players), 10, length(actions)))
mal <- c("FP", "NoRegret")
bandit <- c("QL", "Softmax", "NGreedy", "EGreedy", "Satisficing", "UCB1", "EXP3")
totaldata <- numeric(0)
hugeDF <- numeric(0)

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
    
    hugeDF <- rbind(hugeDF, DF)
    ## form a huge database
    
    
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

  ## Grouping
  data=as.matrix(combined_players)
  MAL_algorithms <- rowSums(data[, mal]) / length(mal)
  Bandit_algorithms <- rowSums(data[, bandit]) / length(bandit)
  data <- cbind(MAL_algorithms, Bandit_algorithms)
  totaldata <- cbind(totaldata, data)
  
  all[,,a-1] = combined_players
}

### subset data, action=2, player=2, MAL & Bandit
Pattern_bandit = paste(bandit, collapse="|")
Pattern_mal = paste(mal, collapse="|")
num_act = 2
num_pla = 2
# choose alternative hypothesis
alt_hypo = c("two.sided", "less", "greater")
alt_hypo = alt_hypo[1] # two.sided

tot_all <- numeric(0)
for(num_act in actions)
{
  tot <- numeric(0)
  for(num_pla in players)
  {
    if(num_act == 4 & (num_pla == 9 || num_pla == 10))
    { 
      #res[1:2] <- c("N/A", "N/A")
      tot <- rbind(tot, res)
      next
    }
    data_b <- subset(hugeDF, hugeDF$actions == num_act & hugeDF$players == num_pla & grepl(Pattern_bandit, hugeDF$type_p0))
    data_m <- subset(hugeDF, hugeDF$actions == num_act & hugeDF$players == num_pla & grepl(Pattern_mal, hugeDF$type_p0))
    sample1 <- data_m$payoff_p0
    sample2 <- data_b$payoff_p0
    
    # start KS-tests for 2-action, 3-action, 4-action between MAL and MAB.
    res <- ks.test(sample1, sample2, alternative = alt_hypo)
    res <- res[1:2]
    tot <- rbind(tot, res)
  }
  tot_all <- cbind(tot_all, tot)
}  
rownames(tot_all) <- c(2:10)

## Save as a latex table
library(xtable)
path <- paste(sysPath, "/tables/kstest-all-raw-data.tex", sep="")
print(xtable(tot_all, digits = -3,  type = "latex"), file = path)



# colnames(tot) <- c ("2-action", "3-action", "4-action")

  library(xtable)
  #path <- paste(sysPath, "/tables/kstest-groupdata-two.tex", sep="")
  print(xtable(tot, type = "latex"), file = path)
  
  ## ggplot2 for ks test
  # simulate two distributions - your data goes here!
  library(ggplot2)
  #sample1 <- data_m$payoff_p0
  #sample2 <- data_b$payoff_p0
  sample1 <- rnorm(1000, 0.8, 0.03)
  sample2 <- rnorm(1000, 0.77, 0.04)
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
    xlab("Normalized average rewards") +
    ylab("ECDF") +
    #geom_line(size=1) +
    geom_segment(aes(x = x0[1], y = y0[1], xend = x0[1], yend = y1[1]),
                 linetype = "dashed", color = "red") +
    geom_point(aes(x = x0[1] , y= y0[1]), color="red", size=8) +
    geom_point(aes(x = x0[1] , y= y1[1]), color="red", size=8) +
    ggtitle("K-S Test: Sample 1 / Sample 2") +
    theme(legend.title=element_blank())



