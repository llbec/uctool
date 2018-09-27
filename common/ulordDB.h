#ifndef ULORD_DB_H
#define ULORD_DB_H

#ifdef MYSQL_ENABLE
#include "utils.h"
#include "MySQLConnection.h"

#include "masternodeman.h"

class CMNode : public CMstNodeData {
public:
    std::string  _name;
    std::string  _ipaddr;
    bool IsNull();
    bool Check(std::string& strRet, int64_t needUpdatePeriod);
};

class CUlordDb {
private:
    const std::string tablename_;
    MysqlConnectInfo dbinfo_;
public:
    const int64_t _needUpdatePeriod;
    typedef std::map <std::string, std::string> map_col_val_t;
    typedef std::pair <std::string, std::string> pair_col_val_t;
    typedef typename map_col_val_t::iterator map_col_val_it;
    typedef typename map_col_val_t::const_iterator map_col_val_cit;
    CUlordDb();
    bool SelectMNode(const map_col_val_t& mapfilter, std::vector<CMNode>& vecRet);
    bool UpdateLicenses(const std::vector<CMNode>& vecMNode);
};

#endif // MYSQL_ENABLE

#endif // ULORD_DB_H
