#ifdef MYSQL_ENABLE
#include "ulordDB.h"
#include "ulord.h"
#include "util.h"

bool CMNode::IsNull()
{
    if(_txid == "NULL" || _privkey == "NULL" || _ipaddr == "NULL" || _licence == "NULL" ||
       _status == -1 || _licperiod == -1 || _nodeperiod == -1 || _licversion == 0)
        return false;
    return true;
}

bool CMNode::Check(std::string& strRet, int64_t needUpdatePeriod)
{
    bool bRet = true;
    int64_t tnow = GetTime();
    strRet = Strings::Format("MasterNode %s@%ld<%s-%d>,privkey<%s>,ipaddress<%s>,status<%d-%ld>,license<%s>@%ld@%d",
                                _name.c_str(), tnow, _txid.c_str(), _voutid, _privkey.c_str(), _ipaddr.c_str(), _status, _nodeperiod, _licence.c_str(), _licperiod, _licversion);
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

CUlordDb::CUlordDb() :
tablename_(GetArg("-dbtable","udevforums_major_node_bind")),
dbinfo_(MysqlConnectInfo(GetArg("-dbhost", "127.0.0.1"), atoi(GetArg("-dbport", "3306")), GetArg("-dbuser", "root"),GetArg("-dbpwd", "123456"),GetArg("-dbname", "mysql"))),
_needUpdatePeriod(GetArg("-periodunit",86400)*GetArg("-needupdate",3))
{}

bool CUlordDb::SelectMNode(const map_col_val_t& mapfilter, std::vector<CMNode>& vecRet)
{
    bool b1st = true;
    MySQLResult res;
    std::string sql = Strings::Format("SELECT trade_txid, trade_vout_no, special_code, major_node_nickname, ip_address, status, validdate, certificate, cert_version, node_period FROM %s",
                                    tablename_.c_str());

    for(auto i : mapfilter)
    {
        if (b1st == true) {
            b1st = false;
            Strings::Append(sql, " WHERE %s = '%s'", i.first.c_str(), i.second.c_str());
        } else {
            Strings::Append(sql, " And %s = '%s'", i.first.c_str(), i.second.c_str());
        }
    }

    try {
        MySQLConnection db(dbinfo_);
        for (size_t i = 0; i < 3; i++)
        {
            if (db.ping())
                break;
            else {
                if(i == 2) ([]{ LOG(INFO) << "CUlordDb::SelectDb:db ping failed!"; return false; });
		else sleep(3);
	    }
        }
        if(db.query(sql, res) == false) {
            LOG(INFO) << "CUlordDb::SelectDb:query failed!";
            return false;
        }
    }
    catch (const std::exception& ex) {
        LOG(ERROR) << "CUlordDb::SelectDb:query exception " << ex.what();
        return false;
    }
    vecRet.clear();
	if(res.numRows() == 0) return true;
    char **row = res.nextRow();
    while(row != nullptr)
    {
        CMNode mstnode;
        int i = 0;
        row[i] != NULL ? mstnode._txid = row[i] : mstnode._txid = "NULL";
        row[++i] != NULL ? mstnode._voutid = atoi(row[i]) : mstnode._voutid = 0;
        row[++i] != NULL ? mstnode._privkey = row[i] : mstnode._privkey = "NULL";
        row[++i] != NULL ? mstnode._name = row[i] : mstnode._name = "NULL";
        row[++i] != NULL ? mstnode._ipaddr = row[i] : mstnode._ipaddr = "NULL";
        row[++i] != NULL ? mstnode._status = atoi(row[i]) : mstnode._status = -1;
        row[++i] != NULL ? mstnode._licperiod = atoi(row[i]) : mstnode._licperiod = -1;
        row[++i] != NULL ? mstnode._licence = row[i] : mstnode._licence = "NULL";
        row[++i] != NULL ? mstnode._licversion = atoi(row[i]) : mstnode._licversion = 0;
        row[++i] != NULL ? mstnode._nodeperiod = atoi(row[i]) : mstnode._nodeperiod = -1;
        vecRet.push_back(mstnode);
        row = res.nextRow();
    }
    return true;
}

bool CUlordDb::UpdateLicenses(const std::vector<CMNode>& vecMNode)
{
    try {
        MySQLConnection db(dbinfo_);
        for (size_t i = 0; i < 3; i++)
        {
            if (db.ping())
                break;
            else {
                if(i == 2) []{ LOG(INFO) << "CUlordDb::UpdateLicenses:db ping failed!"; return false; };
	        else sleep(3);
            }
        }
        for(auto mn : vecMNode)
        {
            string sql = Strings::Format("UPDATE %s SET validdate = %ld, certificate = '%s', cert_version = %d WHERE trade_txid='%s' AND trade_vout_no=%d",
                                        tablename_.c_str(),
                                        mn._licperiod,
                                        mn._licence.c_str(),
                                        mn._licversion,
                                        mn._txid.c_str(),
                                        mn._voutid);
            if(!db.execute(sql)) LOG(INFO) << "CUlordDb::UpdateLicenses:db execute failed @ node<" << mn._txid << " : " << mn._voutid << ">";
        }
        return true;
    }
    catch (const std::exception& ex) {
        LOG(ERROR) << "CUlordDb::UpdateLicenses:db exception " << ex.what();
        return false;
    }
}
#endif // MYSQL_ENABLE
