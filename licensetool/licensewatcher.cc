#ifdef MYSQL_ENABLE
#include "licensewatcher.h"
#include "ulord.h"
using namespace std;

CLicenseWatcher::CLicenseWatcher() :
licversion_(GetArg("-licversion",1)),
licPeriodLimit_(GetArg("-periodunit",86400)*GetArg("-periodlimit",30)),
needUpdatePeriod_(GetArg("-periodunit",86400)*GetArg("-needupdate",3)),
runInterval_(GetArg("-runinterval",60)),
tablename_(GetArg("-dbtable","udevforums_major_node_bind")),
db_()
{
    if(!InitWatcherKey()) throw -1;
}

bool CLicenseWatcher::InitWatcherKey()
{
    char cKeyVersion[20];
    int keyVersion = 1;
    std::string strWatcherKey;
    
    while(true)
    {
        memset(cKeyVersion, 0, sizeof(cKeyVersion));
        sprintf(cKeyVersion, "-privkey%d", keyVersion);
        strWatcherKey = GetArg(std::string(cKeyVersion), "");
        if(strWatcherKey.empty()) {
            break;
        }

        try {
            CKeyExtension privkey(1,strWatcherKey);
            mapWatcherkey_.insert(pair_int_key_t(keyVersion, privkey.GetKey()));
            keyVersion_ = keyVersion;
            LOG(INFO) << "Load ucenter key <" << keyVersion << ":" << strWatcherKey << ">";
            keyVersion++;
        } catch(int) {
            printf("Invalid ulord center private key in the configuration! %s\n", strWatcherKey.c_str());
            return false;
        }
    }
    if(mapWatcherkey_.size() == 0) {
        printf("You must specify a Ulord Center privkey in the configuration.! example privkey1=123qwe\n");
        return false;
    }
    return true;
}

void CLicenseWatcher::SelectNeedUpdateMNData(std::vector<CMNode> & vecnode)
{
    int64_t tnow = GetTime();
    int64_t tlimit = tnow + needUpdatePeriod_;
    MySQLResult res;
    string sql = Strings::Format("SELECT trade_txid, trade_vout_no, special_code, status, validdate, node_period FROM %s WHERE node_period > %ld AND validdate < %ld AND status = 1",
                                    tablename_.c_str(), tnow, tlimit);

    vector<std::string> vecFilter;
    vecFilter.push_back("status=1");
    vecFilter.push_back(Strings::Format("validdate<%ld", tlimit));
    vecFilter.push_back(Strings::Format("node_period>%ld", tnow));
    db_.SelectMNode(vecFilter, vecnode);
}

bool CLicenseWatcher::SignMNLicense(CMNode & mn)
{
    vector<unsigned char> vchSig;
    int64_t tnow = GetTime();
    int64_t tlimit = tnow + needUpdatePeriod_;
    
    if(mn._licperiod >= mn._nodeperiod || mn._licperiod > tlimit || mn._status != 1) {
        return false;
    }

    try {
        if(!mn._pubkey.IsValid()) {
            CKeyExtension key(mn._privkey);
            mn._pubkey = key.GetPubKey();
        }
    } catch (int) {
        LOG(INFO) << "sign license failed, invalid privkey = " << mn._privkey << "masternode<" << mn._txid << ":" << mn._voutid << ">";
        return false;
    }

    mn._licperiod = tnow + licPeriodLimit_;
    if(mn._licperiod > mn._nodeperiod)
        mn._licperiod = mn._nodeperiod;
    
    mn._licversion = keyVersion_;

    if(!mapWatcherkey_[keyVersion_].SignCompact(mn.GetLicenseWord(), vchSig)){
        LOG(INFO) << "sign license failed, privkey = " << HexStr(mapWatcherkey_[keyVersion_]) << "masternode<" << mn._txid << ":" << mn._voutid << ">";
        return false;
    }
    mn._licence = EncodeBase64(&vchSig[0], vchSig.size());
    if(!mn.VerifyLicense()) {
        LOG(INFO) << "verify license failed, licence = " << mn._licence << "masternode<" << mn._txid << ":" << mn._voutid << ">";
        return false;
    }
    return true;
}

void CLicenseWatcher::UpdateDB(std::vector<CMNode> & vecnode)
{
    for(auto & mn : vecnode) {
        if(SignMNLicense(mn)) {
            if(!db_.UpdateLicense(mn))
                LOG(INFO) << "UpdateMNData failed, masternode<" << mn._txid << ":" << mn._voutid << ">";
            else
                LOG(INFO) << "UpdateMNData success, masternode<" << mn._txid << ":" << mn._voutid << "> license@<" << mn._licperiod << ":" << mn._licversion << ">";
        }
    }
}

void CLicenseWatcher::ClearDB()
{
    db_.ClearLicenses();
}

void CLicenseWatcher::Run()
{
    vector<CMNode> vecnode;
    printf("Runner inerval is %ld s, update time=%ld s, license period=%ld s\n", runInterval_, needUpdatePeriod_, licPeriodLimit_);
    while(true) {
        vecnode.clear();
        SelectNeedUpdateMNData(vecnode);
        UpdateDB(vecnode);
        MilliSleep(runInterval_*1000);
    }
}
#endif // MYSQL_ENABLE
