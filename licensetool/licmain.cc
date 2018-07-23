#ifdef MYSQL_ENABLE

#include "utils.h"
#include "MySQLConnection.h"
#include "util.h"
#include "masternode.h"

using namespace std;

int main(int argc, char const *argv[])
{
    SetFilePath("licensetool.conf");
	LoadConfigFile(mapConfigs, mapMultiConfigs);

    //ScanTable(GetArg("-sqluser", "root"), GetArg("-sqlpwd", "123456"), GetArg("-sqlthread", 1));
    MysqlConnectInfo *poolDBInfo = nullptr;
    {
        int32_t poolDBPort = GetArg("-dbport", 3306);
        poolDBInfo = new MysqlConnectInfo(GetArg("-dbhost", "127.0.0.1"), poolDBPort,
                                            GetArg("-dbuser", "root"),
                                            GetArg("-dbpwd", "123456"),
                                            GetArg("-dbname", "udevforums_major_node_bind"));
    }

    string sql;
    sql = Strings::Format("select *from udevforums_major_node_bind");
    MySQLResult res;

    MySQLConnection db(poolDBInfo);
    for (size_t i = 0; i < 3; i++) {
      if (db.ping())
        break;
    }

    db.query(sql, res);
    printf("read db %d rows\n", res.numRows());

    return 0;
}

#endif // MYSQL_ENABLE