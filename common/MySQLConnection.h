#ifndef UTCENTER_MYSQL_CONNECTION_H_
#define UTCENTER_MYSQL_CONNECTION_H_

#ifdef MYSQL_ENABLE

#include "utils.h"

using std::string;
using std::vector;

extern "C" struct st_mysql;
typedef struct st_mysql MYSQL;
extern "C" struct st_mysql_res;
typedef struct st_mysql_res MYSQL_RES;

/**
 * Simple wrapper for MYSQL_RES
 * auto free
 */
struct MySQLResult {
  struct st_mysql_res * result;
  MySQLResult();
  MySQLResult(MYSQL_RES * result);
  void reset(MYSQL_RES * result);
  ~MySQLResult();
  uint64 numRows();
  uint32 fields();
  char ** nextRow();
};

class MysqlConnectInfo {
public:
  string  host_;
  int32_t port_;
  string  username_;
  string  password_;
  string  dbName_;

  MysqlConnectInfo(const string &host, int32_t port, const string &userName,
                   const string &password, const string &dbName):
  host_(host), port_(port), username_(userName), password_(password), dbName_(dbName)
  {
  }

  MysqlConnectInfo(const MysqlConnectInfo &r) {
    host_     = r.host_;
    port_     = r.port_;
    username_ = r.username_;
    password_ = r.password_;
    dbName_   = r.dbName_;
  }

  MysqlConnectInfo& operator=(const MysqlConnectInfo &r) {
    host_     = r.host_;
    port_     = r.port_;
    username_ = r.username_;
    password_ = r.password_;
    dbName_   = r.dbName_;
    return *this;
  }
};

/**
 * Simple wrapper for MYSQL connection
 * open/close auto managed by class
 * with support for auto-reconnect
 * @see test/TestMySQLConnection.cc for demo usage
 */
class MySQLConnection {
protected:
  string host_;
  int32_t port_;
  string username_;
  string password_;
  string dbName_;

  struct st_mysql * conn;

public:
  MySQLConnection(const MysqlConnectInfo &connectInfo);
  ~MySQLConnection();

  bool open();
  void close();
  bool ping();

  bool execute(const char * sql);
  bool execute(const string &sql) {
    return execute(sql.c_str());
  }

  bool query(const char * sql, MySQLResult & result);
  bool query(const string & sql, MySQLResult & result) {
    return query(sql.c_str(), result);
  }

  // return -1 on failure
  int64_t update(const char * sql);
  int64_t update(const string & sql) {
    return update(sql.c_str());
  }
  uint64 affectedRows();
  uint64 getInsertId();

  string getVariable(const char *name);
};

bool multiInsert(MySQLConnection &db, const string &table,
                 const string &fields, const vector<string> &values);

#endif // MYSQL_ENABLE

#endif // UTCENTER_MYSQL_CONNECTION_H_