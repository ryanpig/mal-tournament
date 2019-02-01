library("RSQLite")
# connect to the sqlite file
con <- dbConnect(drv=RSQLite::SQLite(), dbname="~/CppProj/thesis_proj/result2.db")
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
new_DF <- tmp1[, c("type_p0", "type_p1")]
final <- cbind(new_DF,avg_DF)
boxplot(avg_DF ~ type_p0, data=final, las=2)

## finish 
dbDisconnect(con)
