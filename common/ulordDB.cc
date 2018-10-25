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

void CMNode::SetKey(const std::string& strKey)
{
    _privkey = strKey;
    try {
        CKeyExtension key(_privkey);
        _pubkey = key.GetPubKey();
    }
    catch (int) {
        LOG(INFO) << "CMNode::SetKey:failed @ " << ToString();
    }
}

bool CMNode::Check(std::string& strRet, int64_t needUpdatePeriod)
{
    bool bRet = true;
    int64_t tnow = GetTime();
    strRet = ToString();
    if(_status != 1)
        return true;
    try {
        CKeyExtension key(_privkey);
    }
    catch (int) {
        Strings::Append(strRet, ", privkey is invalid");
        bRet = false;
    }
    if(_nodeperiod < tnow) {
        bRet = false;
        Strings::Append(strRet, ", node period is expired");
    }
    if(_licperiod < tnow + needUpdatePeriod) {
        bRet = false;
        Strings::Append(strRet, ", license need update");
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
        if(!IsDBOnline(&db)) {
            LOG(INFO) << "CUlordDb::SelectMNode:db ping failed!";
            return false;
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
        row[++i] != NULL ? mstnode.SetKey(row[i]) : mstnode.SetKey("NULL");
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

bool CUlordDb::SelectMNode(const std::vector<std::string>& vecfilter, std::vector<CMNode>& vecRet)
{
    bool b1st = true;
    MySQLResult res;
    std::string sql = Strings::Format("SELECT trade_txid, trade_vout_no, special_code, major_node_nickname, ip_address, status, validdate, certificate, cert_version, node_period FROM %s",
                                    tablename_.c_str());

    for(auto i : vecfilter)
    {
        if (b1st == true) {
            b1st = false;
            Strings::Append(sql, " WHERE %s", i.c_str());
        } else {
            Strings::Append(sql, " And %s", i.c_str());
        }
    }

    try {
        MySQLConnection db(dbinfo_);
        if(!IsDBOnline(&db)) {
            LOG(INFO) << "CUlordDb::SelectMNode:db ping failed!";
            return false;
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
        row[++i] != NULL ? mstnode.SetKey(row[i]) : mstnode.SetKey("NULL");
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
        if(!IsDBOnline(&db)) {
            LOG(INFO) << "CUlordDb::UpdateLicenses:db ping failed!";
            return false;
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

bool CUlordDb::UpdateLicense(const CMNode& mn)
{
    try {
        MySQLConnection db(dbinfo_);
        if(!IsDBOnline(&db)) {
            LOG(INFO) << "CUlordDb::UpdateLicenses:db ping failed!";
            return false;
        }

        string sql = Strings::Format("UPDATE %s SET validdate = %ld, certificate = '%s', cert_version = %d WHERE trade_txid='%s' AND trade_vout_no=%d",
                                    tablename_.c_str(),
                                    mn._licperiod,
                                    mn._licence.c_str(),
                                    mn._licversion,
                                    mn._txid.c_str(),
                                    mn._voutid);

        return db.execute(sql);
    }
    catch (const std::exception& ex) {
        LOG(ERROR) << "CUlordDb::UpdateLicenses:db exception " << ex.what();
        return false;
    }
}

bool CUlordDb::ClearLicenses()
{
    try {
        MySQLConnection db(dbinfo_);
        if(!IsDBOnline(&db)) {
            LOG(INFO) << "CUlordDb::UpdateLicenses:db ping failed!";
            return false;
        }

        string sql = Strings::Format("UPDATE %s SET validdate = 0, certificate = NULL, cert_version = 1", tablename_.c_str());

        return db.execute(sql);
    }
    catch (const std::exception& ex) {
        LOG(ERROR) << "CUlordDb::UpdateLicenses:db exception " << ex.what();
        return false;
    }
}

bool CUlordDb::IsDBOnline(MySQLConnection* ptrDB)
{
    bool bRet = true;
    try {
        if(ptrDB == NULL) {
            LOG(ERROR) << "CUlordDb::IsDBOnline:ptr is NULL";
            return false;
        }
        for (size_t i = 0; i < 3; i++)
        {
            if (ptrDB->ping())
                break;
            i == 2 ? bRet = false : sleep(3);
	    }
    } catch (const std::exception& ex) {
        LOG(ERROR) << "CUlordDb::IsDBOnline:ping exception " << ex.what();
        return false;
    }
    return bRet;
}
#endif // MYSQL_ENABLE
