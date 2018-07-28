#ifdef MYSQL_ENABLE
#include "dbwatcher.h"
#include "utils.h"

#include "util.h"
#include "privsend.h"

using namespace std;

DBWatcher::DBWatcher(const CKey priv, const MysqlConnectInfo & ptrDBInfo) :
watcherKey_(priv),
licPeriodLimit_(86400*GetArg("-periodlimit",30)),
needUpdatePeriod_(86400*GetArg("-needupdate",3)),
runInterval_(GetArg("-runinterval",60000)),
db_(ptrDBInfo)
{
    tablename_ = GetArg("-dbtable","udevforums_major_node_bind");
}

bool DBWatcher::IsDBOnline()
{
    for (size_t nPings = 0; nPings < 3; nPings++) {
        if (db_.ping())
            return true;
    }
    LOG(INFO) << "DBWatcher db is offline!";
    return false;
}

void DBWatcher::SelectMNData(std::vector<CMstNodeData> & vecnode)
{
    MySQLResult res;
    std::string sql = Strings::Format("SELECT * FROM %s", tablename_.c_str());
    if(!IsDBOnline())
        return;
    db_.query(sql, res);
    LOG(INFO) << "DBWatcher::SelectMNData:Read DB and select <" << res.numRows() << " masternodes";
    if(res.numRows() == 0)
        return;

    char **row = res.nextRow();
    CMstNodeData mstnode;
    while(row != nullptr) {
        if(row[TB_TXID] == NULL || row[TB_VOUTID] == NULL || row[TB_PRIVKEY] == NULL)
            continue;
        mstnode._txid       = row[TB_TXID];
        mstnode._voutid     = atoi(row[TB_VOUTID]);
        mstnode._privkey    = row[TB_PRIVKEY];
        mstnode._status     = atoi(row[TB_STATUS]);
        mstnode._validflag  = atoi(row[TB_VALIDFLAG]);
        mstnode._licperiod  = atoi(row[TB_LICPERIOD]);
        if(row[TB_LICENSE] != NULL)
            mstnode._licence    = row[TB_LICENSE];
        if(row[TB_PERIOD] != NULL)
            mstnode._nodeperiod = atoi(row[TB_PERIOD]);
        else
            mstnode._nodeperiod = 0;

        CKey mnpriv;
        if(!privSendSigner.GetKeysFromSecret(mstnode._privkey, mnpriv, mstnode._pubkey)) {
            LOG(INFO) << "DBWatcher::SelectMNData:masternode <" <<mstnode._txid << ":" <<mstnode._voutid << "> private key string " << mstnode._privkey << " is invalid!";
            row = res.nextRow();
            continue;
        }
        vecnode.push_back(mstnode);
        //printf("DBReadMNInfo: get masternode <%s:%d>\n", mstnode._txid.c_str(), mstnode._voutid);

        row = res.nextRow();
    }
    return;
}

bool DBWatcher::UpdateMNData(const CMstNodeData & mn)
{
    if(!IsDBOnline())
        return false;
    string sql = Strings::Format("UPDATE %s SET validdate = %ld, certificate = '%s' WHERE trade_txid='%s' AND trade_vout_no=%d",
                                    tablename_.c_str(),
                                    mn._licperiod,
                                    mn._licence.c_str(),
                                    mn._txid.c_str(),
                                    mn._voutid);

    return db_.execute(sql);
}

bool DBWatcher::ClearMNData(const CMstNodeData & mn)
{
    if(!IsDBOnline())
        return false;
    string sql = Strings::Format("UPDATE %s SET validflag = 0, validdate = 0, certificate = NULL WHERE trade_txid='%s' AND trade_vout_no=%d",
                                    tablename_.c_str(),
                                    mn._txid.c_str(),
                                    mn._voutid);

    return db_.execute(sql);
}

bool DBWatcher::SignMNLicense(CMstNodeData & mn)
{
    vector<unsigned char> vchSig;
    int64_t tnow = GetTime();
    int64_t tlimit = tnow + needUpdatePeriod_;
    
    if(mn._licperiod >= mn._nodeperiod || mn._licperiod > tlimit || mn._status != 1) {
        return false;
    }

    mn._licperiod = tnow + licPeriodLimit_;
    if(mn._licperiod > mn._nodeperiod)
        mn._licperiod = mn._nodeperiod;

    if(!watcherKey_.SignCompact(mn.GetLicenseWord(), vchSig)){
        LOG(INFO) << "sign license failed, privkey = " << HexStr(watcherKey_) << "masternode<" << mn._txid << ":" << mn._voutid << ">";
        return false;
    }
    mn._licence = EncodeBase64(&vchSig[0], vchSig.size());
    if(!mn.VerifyLicense()) {
        LOG(INFO) << "verify license failed, licence = " << mn._licence << "masternode<" << mn._txid << ":" << mn._voutid << ">";
        return false;
    }
    return true;
}

void DBWatcher::UpdateDB(std::vector<CMstNodeData> & vecnode)
{
    for(auto & mn : vecnode) {
        if(SignMNLicense(mn)) {
            if(!UpdateMNData(mn))
                LOG(INFO) << "UpdateMNData failed, masternode<" << mn._txid << ":" << mn._voutid << ">";
        }
    }
}

void DBWatcher::SelectNeedUpdateMNData(std::vector<CMstNodeData> & vecnode)
{
    int64_t tnow = GetTime();
    int64_t tlimit = tnow + needUpdatePeriod_;
    MySQLResult res;
    string sql = Strings::Format("SELECT trade_txid, trade_vout_no, special_code, status, validdate, node_period FROM %s WHERE node_period > %ld AND validdate < %ld AND status = 1",
                                    tablename_.c_str(), tnow, tlimit);

    if(!IsDBOnline())
        return;
    db_.query(sql, res);
    LOG(INFO) << "DBWatcher::SelectNeedUpdateMNData:Read DB and select " << res.numRows() << " masternodes";
    if(res.numRows() == 0)
        return;

    char **row = res.nextRow();
    CMstNodeData mstnode;
    while(row != nullptr) {
        if(row[0] == NULL) {
            LOG(INFO) << "DBWatcher::SelectNeedUpdateMNData:Skip without txid";
            row = res.nextRow();
            continue;
        }
        mstnode._txid = row[0];
        if(row[1] == NULL) {
            LOG(INFO) << "DBWatcher::SelectNeedUpdateMNData:Skip without voutid, txid " << mstnode._txid;
            row = res.nextRow();
            continue;
        }
        mstnode._voutid     = atoi(row[1]);
        if(row[2] != NULL)
            mstnode._privkey = row[2];
        mstnode._status     = atoi(row[3]);
        mstnode._licperiod  = atoi(row[4]);
        if(row[5] != NULL)
            mstnode._nodeperiod = atoi(row[5]);
        else
            mstnode._nodeperiod = 0;

        CKey mnpriv;
        if(!privSendSigner.GetKeysFromSecret(mstnode._privkey, mnpriv, mstnode._pubkey)) {
            LOG(INFO) << "DBWatcher::SelectNeedUpdateMNData:masternode <" <<mstnode._txid << ":" <<mstnode._voutid << "> private key string " << mstnode._privkey << " is invalid!";
            row = res.nextRow();
            continue;
        }
        vecnode.push_back(mstnode);
        //printf("DBReadMNInfo: get masternode <%s:%d>\n", mstnode._txid.c_str(), mstnode._voutid);

        row = res.nextRow();
    }
}

void DBWatcher::Runner()
{
    vector<CMstNodeData> vecnode;
    printf("Runner inerval is %ld ms, update time=%ld s, license period=%ld s\n", runInterval_, needUpdatePeriod_, licPeriodLimit_);
    while(true) {
        vecnode.clear();
        SelectNeedUpdateMNData(vecnode);
        UpdateDB(vecnode);
        MilliSleep(runInterval_);
    }
}
#endif // MYSQL_ENABLE
