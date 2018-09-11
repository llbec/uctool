#ifndef UTCENTER_DBCLASS_H
#define UTCENTER_DBCLASS_H
#include "utils.h"
#include "MySQLConnection.h"

#ifdef MYSQL_ENABLE
class Cmndata{
public:
    std::string  _txid;
    int          _voutid;
    std::string  _privkey;
    std::string  _name;
    std::string  _ipaddr;
    int          _status;
    int64_t      _licperiod;
    std::string  _licence;
    int64_t      _nodeperiod;
    Cmndata();
    Cmndata & operator=(Cmndata& b);
    bool IsNull();
    bool Check(std::string& strRet);
};

class CDbHandler {
private:
	MySQLConnection db_;
    std::string tablename_;
public:
    typedef std::map <std::string, std::string> map_col_val_t;
    typedef std::pair <std::string, std::string> pair_col_val_t;
    typedef typename map_col_val_t::iterator map_col_val_it;
    typedef typename map_col_val_t::const_iterator map_col_val_cit;
    CDbHandler();
	bool IsDBOnline();
    bool SelectData(const map_col_val_t& mapWhere, std::vector<Cmndata>& vecResult);
};
#endif // MYSQL_ENABLE
#endif // UTCENTER_DBCLASS_H
