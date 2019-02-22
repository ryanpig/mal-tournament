library("RSQLite")
# connect to the sqlite file
con <- dbConnect(drv=RSQLite::SQLite(), dbname="~/CppProj/thesis_proj/Result/result_iter_10000.db")
tables <- dbListTables(con)

## exclude sqlite_sequence (contains table information)
tables <- tables[tables != "sqlite_sequence"]

lDataFrames <- vector("list", length=length(tables))

## create a data.frame for each table
for (i in seq(along=tables)) {
  DF <- dbGetQuery(conn=con, statement=paste("SELECT * FROM '", tables[[i]], "'", sep=""))
}

odd_index = seq(1, nrow(DF), 2)
even_index = seq(2, nrow(DF), 2)
tmp1 <- DF[odd_index, ]
tmp2 <- DF[even_index, ]
avg_DF <- (tmp1[, "payoff_p0"] + tmp2[, "payoff_p1"]) / 2
new_DF <- tmp1[, c("gametype", "type_p0", "type_p1")]
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

#basic heatmap
png("heatmap_no_normal_algorithms.png", 640,640)
plot_ly(x=colnames(data), y=rownames(data), z = data, type = "heatmap")


# with normalization (right) (apply(,2,): by columns
png("heatmap_normal_algorithms.png", 640,640)
data=apply(data, 2, function(x){x/mean(x)})
plot_ly(x=colnames(data), y=rownames(data), z = data, type = "heatmap")




## 
library("mosaic")#favstats
favstats(avg_DF ~ type_p0,data=final)

## finish 
dbDisconnect(con)
dev.off()
