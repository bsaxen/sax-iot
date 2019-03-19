# =============================================
# File: mysqlLib.py
# Author: Benny Saxen
# Date: 2019-03-19
# Description: IoT python library
# =============================================
import MySQLdb

# =============================================
def lib_mysqlInsert(c1,cr,xTable,xPar,xValue):
# =============================================
    db = MySQLdb.connect(host=c1.dbhost,user=c1.dbuser,db=c1.dbname)
    cursor = db.cursor()
    if cr == 1:
        sql = "CREATE TABLE IF NOT EXISTS " + xTable + " (id int(11) NOT NULL AUTO_INCREMENT,value float,ts timestamp, PRIMARY KEY (id))"
        cursor.execute(sql)
    sql = "INSERT INTO "+ xTable + " (`id`, " + xPar + ", `ts`) VALUES (NULL," + str(xValue) + ", CURRENT_TIMESTAMP)"
    cursor.execute(sql)
    db.commit()
    db.close()
