#ifdef MYSQL_ENABLE
#include "dbtool.h"
//#include "dbclass.h"
#include "ulordDB.h"

#include "ulord.h"

//#include "util.h"
//#include "masternodeman.h"

using namespace std;

/*Cmndata::Cmndata():
_txid(""),
_voutid(0),
_privkey(""),
_name(""),
_ipaddr(""),
_status(0),
_licperiod(0),
_licence(""),
_nodeperiod(0)
{}

Cmndata & Cmndata::operator=(Cmndata& b)
{
    _txid       = b._txid;
    _voutid     = b._voutid;
    _privkey    = b._privkey;
    _name       = b._name;
    _ipaddr     = b._ipaddr;
    _status     = b._status;
    _licperiod  = b._licperiod;
    _licence    = b._licence;
    _nodeperiod = b._nodeperiod;
    return * this;
}

bool Cmndata::IsNull()
{
    if(_txid == "NULL" || _privkey == "NULL" || _ipaddr == "NULL" || _licence == "NULL" ||
        _voutid == -1 || _status == -1 || _licperiod == -1 || _nodeperiod == -1)
        return false;
    return true;
}

bool Cmndata::Check(std::string& strRet, int64_t needUpdatePeriod)
{
    bool bRet = true;
    int64_t tnow = GetTime();
    strRet = Strings::Format("MasterNode %s@%ld<%s-%d>,privkey<%s>,ipaddress<%s>,status<%d-%ld>,license<%s  || %ld>",
                                _name.c_str(), tnow, _txid.c_str(), _voutid, _privkey.c_str(), _ipaddr.c_str(), _status, _nodeperiod, _licence.c_str(), _licperiod);
    if(_status != 1)
        return true;
    try {
        CKeyExtension key(_privkey);
    }
    catch (int) {
        Strings::Append(strRet, ", privkey is invalid");
        bRet = false;
    }
    if(_nodeperiod + needUpdatePeriod < tnow) {
        bRet = false;
        Strings::Append(strRet, ", node period is expired");
    }
    return bRet && IsNull();
}

CDbHandler::CDbHandler() :
db_(MysqlConnectInfo(GetArg("-dbhost", "127.0.0.1"), atoi(GetArg("-dbport", "3306")), GetArg("-dbuser", "root"),GetArg("-dbpwd", "123456"),GetArg("-dbname", "mysql"))),
tablename_(GetArg("-dbtable","udevforums_major_node_bind")),
_needUpdatePeriod(GetArg("-periodunit",86400)*GetArg("-needupdate",3))
{
}

bool CDbHandler::IsDBOnline()
{
    try
    {
        for (size_t nPings = 0; nPings < 3; nPings++)
        {
            if (db_.ping())
                return true;
        }
    }
    catch (const exception& ex)
    {
        LOG(ERROR) << "DB Ping exception: " << ex.what();
        return false;
    }
    LOG(INFO) << "db is offline!";
    return false;
}

bool CDbHandler::SelectData(const map_col_val_t& mapWhere, std::vector<Cmndata>& vecResult)
{
    bool b1st = true;
    MySQLResult res;
    std::string sql = Strings::Format("SELECT trade_txid, trade_vout_no, special_code, major_node_nickname, ip_address, status, validdate, certificate, node_period FROM %s",
                                    tablename_.c_str());
    for(auto i : mapWhere)
    {
        if (b1st == true) {
            b1st = false;
            Strings::Append(sql, " WHERE %s = '%s'", i.first.c_str(), i.second.c_str());
        } else {
            Strings::Append(sql, " And %s = '%s'", i.first.c_str(), i.second.c_str());
        }
    }

    if(!IsDBOnline())
        return false;
    try {
        db_.query(sql, res);
    }
    catch (const exception& ex) {
        cout << "sql word <" << sql << ">. query db exception " << ex.what() << endl;
        return false;
    }

    vecResult.clear();
	if(res.numRows() == 0) {
		cout << "sql word <" << sql << ">, select 0 entry" << endl;
		return false;
	}
    char **row = res.nextRow();
    while(row != nullptr)
    {
        Cmndata mstnode;
        int i = 0;
        row[i] != NULL ? mstnode._txid = row[i] : mstnode._txid = "NULL";
        row[++i] != NULL ? mstnode._voutid = atoi(row[i]) : mstnode._voutid = -1;
        row[++i] != NULL ? mstnode._privkey = row[i] : mstnode._privkey = "NULL";
        row[++i] != NULL ? mstnode._name = row[i] : mstnode._name = "NULL";
        row[++i] != NULL ? mstnode._ipaddr = row[i] : mstnode._ipaddr = "NULL";
        row[++i] != NULL ? mstnode._status = atoi(row[i]) : mstnode._status = -1;
        row[++i] != NULL ? mstnode._licperiod = atoi(row[i]) : mstnode._licperiod = -1;
        row[++i] != NULL ? mstnode._licence = row[i] : mstnode._licence = "NULL";
        row[++i] != NULL ? mstnode._nodeperiod = atoi(row[i]) : mstnode._nodeperiod = -1;
        vecResult.push_back(mstnode);
        row = res.nextRow();
    }
    return true;
}*/

void CheckNodeHelp()
{
    cout << "Command \"dbcheck\" example :" << endl << endl
        << "dbcheck column value ..." << endl
		<< "dbcheck \"trade_txid\" \"2122660463ab2c041a8b8ab406aa314e76f2b4bf88dec75ce7b17af0c8bc2887\""<< endl
        << "major_node_nickname trade_txid ip_address special_code status cert_version" << endl;
}
void DBCheckNode(int argc, char const * argv[])
{
    if(argc < 4) {
        CheckNodeHelp();
        return;
    }
    int i = 2;
    CUlordDb::map_col_val_t mapCheck;
    vector<CMNode> vecRet;
    CUlordDb db;
    while(i+1 < argc)
    {
        mapCheck.insert(make_pair(argv[i], argv[i + 1]));
        i += 2;
    }
    if(!db.SelectMNode(mapCheck, vecRet)) {
        cout << "Read DB failed!" << endl;
        return;
    }
    cout << "Read " << vecRet.size() << " entries:" << endl;
    for(auto mn : vecRet)
    {
        string sRet;
        mn.Check(sRet, db._needUpdatePeriod);
        cout << sRet << endl;
    }
    return;
}

void DBCheckAll(int argc, char const * argv[])
{
    CUlordDb::map_col_val_t mapCheck;
    vector<CMNode> vecRet;
    CUlordDb db;
    mapCheck.clear();
    if(!db.SelectMNode(mapCheck, vecRet)) {
        cout << "Read DB failed!" << endl;
        return;
    }
    cout << "Read " << vecRet.size() << " entries, check failed:" << endl;
    for(auto mn : vecRet)
    {
        string sRet;
        if(!mn.Check(sRet, db._needUpdatePeriod)) {
            cout << sRet << endl;
        }
    }
    return;
}
#endif // MYSQL_ENABLE
