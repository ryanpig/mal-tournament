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

# spearman's test
hugeDF <- subset(hugeDF, hugeDF$payoff_p0 >=0.1)
df_act2 <- subset(hugeDF, hugeDF$actions==2)
df <- df_act2[sample(nrow(df_act2), 5000),]
corr <- cor.test(x=df$players, y=df$payoff_p0, method = 'spearman')

plot(df$players, df$payoff_p0, type = 'l', main = 'Monotonically Decreasing', frame.plot = FALSE)
library(ggplot2)
ggplot(df, aes(x=players, y=payoff_p0)) + 
  geom_point(color='#2980B9', size = 4) + 
  geom_smooth(method=lm, se=FALSE, fullrange=TRUE, color='#2C3E50')

## spearman's example
x <- 1:100
x2 <- 1:5
y <- (1/4) * x^2 # Monotonically Increasing Function
y2 <- exp(-x2) # Montonically Decreasing
y3 <- sin(x / 5) # Not Monotonic

par(mfrow=c(1,3))
plot(x2, y2, type = 'l', main = 'Monotonically Decreasing', xlab="x", ylab="y", frame.plot = FALSE, 
     cex.main=2.5, cex.lab=2, cex.axis=1)
plot(x, y, type = 'l', main = 'Monotonically Increasing', frame.plot = FALSE, 
     cex.main=2.5, cex.lab=2, cex.axis=1)
plot(x, y3, type = 'l', main = 'Non-monotonic', xlab="x", ylab="y", frame.plot = FALSE,
     cex.main=2.5, cex.lab=2, cex.axis=1)

corr1 <-  cor.test(x2, y2, method = 'spearman', alternative = "greater") # Null
corr2 <-  cor.test(x, y, method = 'spearman', alternative = "greater") # Alternative
corr3 <-  cor.test(x, y3, method = 'spearman', alternative = "greater") # Null
d = cbind
