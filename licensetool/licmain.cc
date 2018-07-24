#ifdef MYSQL_ENABLE

#include "utils.h"
#include "MySQLConnection.h"
#include "util.h"
#include "masternode.h"

using namespace std;

void DBReadMNInfo(MySQLConnection & db, std::vector<CMstNodeData>& vecnode)
{
    string sql;
    sql = Strings::Format("select *from %s", GetArg("-dbtable","udevforums_major_node_bind"));
    MySQLResult res;

    db.query(sql, res);
    printf("DBReadMNInfo: read db %ld rows\n", res.numRows());
    char **row = res.nextRow();
    CMstNodeData mstnode;

    while(row != nullptr)
    {
        mstnode._txid      = row[6];
        mstnode._voutid    = atoi(row[7]);
        mstnode._status    = row[10];
        mstnode._validflag = atoi(row[11]);
        mstnode._licperiod = atoi(row[12]);
        mstnode._licence   = atoi(row[13]);
        mstnode._nodeperiod= atoi(row[19]);
        vecnode.push_back(mstnode);
        printf("DBReadMNInfo: get masternode <%s:%d>", mstnode._txid.c_str(), mstnode._voutid);

        row = res.nextRow();
    }
    return;
}

void DBUpdateMNInfo(MySQLConnection & db, std::vector<CMstNodeData>& vecnode)
{}

void ThreadCheckDB()
{
    static bool fOneThread;
    if(fOneThread) return;
    fOneThread = true;

    // Make this thread recognisable
    //RenameThread("ucenter-checkdb");

    MysqlConnectInfo *ptrDBInfo = nullptr;
    {
        int32_t poolDBPort = GetArg("-dbport", 3306);
        ptrDBInfo = new MysqlConnectInfo(GetArg("-dbhost", "127.0.0.1"),
                                            poolDBPort,
                                            GetArg("-dbuser", "root"),
                                            GetArg("-dbpwd", "123456"),
                                            GetArg("-dbname", "mysql"));
    }

    string sql;
    sql = Strings::Format("select *from udevforums_major_node_bind");
    MySQLResult res;

    MySQLConnection db(*ptrDBInfo);
    vector<CMstNodeData> vecnode;

    int ncheckinterval = GetArg("-dbcheckinterval",60000);
    size_t nPings = 0;

    while(true)
    {
        MilliSleep(ncheckinterval);

        for (nPings = 0; nPings < 3; i++) {
            if (db.ping())
                break;
        }

        if(nPings >= 3) {
            LOG(ERROR) << "DB ping failed, can not read mysql";
            return;
        }

        vecnode.clear();
        DBReadMNInfo(db, vecnode);
    }
}

void DBTest()
{
    MysqlConnectInfo *ptrDBInfo = nullptr;
    {
        int32_t poolDBPort = GetArg("-dbport", 3306);
        ptrDBInfo = new MysqlConnectInfo(GetArg("-dbhost", "127.0.0.1"), poolDBPort,
                                            GetArg("-dbuser", "root"),
                                            GetArg("-dbpwd", "123456"),
                                            GetArg("-dbname", "mysql"));
    }

    vector<CMstNodeData> vecnode;
    MySQLConnection db(*ptrDBInfo);
    for (size_t i = 0; i < 3; i++) {
      if (db.ping())
        break;
    }
    DBReadMNInfo(db, vecnode);
}

int main(int argc, char const *argv[])
{
    /*init*/
    SetFilePath("licensetool.conf");
	LoadConfigFile(mapArgs, mapMultiArgs);
    InitLog(argv);

    if(argc > 1)
    {
        if("test" == argv[1])
        {
            DBTest();
            return 0;
        }
    }

    return 0;
}

#endif // MYSQL_ENABLE
