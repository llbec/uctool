#ifdef MYSQL_ENABLE

#include "utils.h"
#include "MySQLConnection.h"
#include "util.h"
#include "masternode.h"
#include <glog/logging.h>

using namespace std;

int main(int argc, char const *argv[])
{
    SetFilePath("licensetool.conf");
	LoadConfigFile(mapConfigs, mapMultiConfigs);

	// Initialize Google's logging library.	
	google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = GetLogPath();
	// Log messages at a level >= this flag are automatically sent to
 	// stderr in addition to log files.
 	FLAGS_stderrthreshold = 3;    // 3: FATAL
 	FLAGS_max_log_size    = 100;  // max log file size 100 MB
 	FLAGS_logbuflevel     = -1;   // don't buffer logs
 	FLAGS_stop_logging_if_full_disk = true;

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

    MySQLConnection db(*poolDBInfo);
    for (size_t i = 0; i < 3; i++) {
      if (db.ping())
        break;
    }

    db.query(sql, res);
    printf("read db %ld rows\n", res.numRows());

    return 0;
}

#endif // MYSQL_ENABLE
