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
    CMstNodeData * GetData() { return (CMstNodeData *)this; }
    std::string ToString() {
        return Strings::Format("MasterNode %s<%s-%d>,privkey<%s>,ipaddress<%s>,status<%d>,license< %s >@[%ld,%ld]@%d",
                                _name.c_str(),
                                _txid.c_str(),
                                _voutid,
                                _privkey.c_str(),
                                _ipaddr.c_str(),
                                _status,
                                _licence.c_str(),
                                _licperiod,
                                _nodeperiod,
                                _licversion);
    }
};

class CMNCoin
{
public:
    std::string _txid;
    unsigned int _voutid;
    CMNCoin() {}
    CMNCoin(std::string txid, int voutid):_txid(txid),_voutid(voutid) {}
    friend bool operator==(const CMNCoin& a, const CMNCoin& b) { return a._txid==b._txid && a._voutid==b._voutid; }
    friend bool operator!=(const CMNCoin& a, const CMNCoin& b) { return a._txid!=b._txid || a._voutid!=b._voutid; }
    friend bool operator<(const CMNCoin& a, const CMNCoin& b) { return a._txid==b._txid ? a._voutid<b._voutid : a._txid<b._txid; }
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
    bool SelectMNode(const std::vector<std::string>& vecfilter, std::vector<CMNode>& vecRet);
    bool UpdateLicenses(const std::vector<CMNode>& vecMNode);
    bool IsDBOnline(MySQLConnection* ptrDB);
};

#endif // MYSQL_ENABLE

#endif // ULORD_DB_H
