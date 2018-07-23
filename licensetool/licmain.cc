#include "utils.h"
#include "connection_pool.h"
#include "util.h"
#include "masternode.h"

using namespace std;

bool ReadAllNodeToNet(sql::Connection * con, std::vector<CMstNodeData>& vecnode )
{
    char sqlBuf[500] = {""};

    sprintf(sqlBuf ,"select *from udevforums_major_node_bind ");

    sql::Statement * stmt = NULL;
    stmt = con->createStatement();
    if(stmt == NULL)
    {
        //BOOST_LOG_TRIVIAL(info)<<"sql query   query ulord failed  ";
        //cout << "query master node nodeaddr "   << endl;
        return false;
    }
    stmt->execute("SET CHARSET GB2312");
    stmt->execute("USE mysql");
    CMstNodeData mstnode;
    sql::ResultSet * resultSet =  stmt->executeQuery(sqlBuf);
    int i = 0;
    while (resultSet->next()) {
        mstnode._hostname  = resultSet->getString(5);
        mstnode._txid      = resultSet->getString(6);
        mstnode._voutid    = resultSet->getInt(7);
        mstnode._hostip    = resultSet->getString(8);
        mstnode._validflag = resultSet->getInt(11);
        mstnode._licperiod = resultSet->getInt(12);
        mstnode._licence   = resultSet->getInt(13);
        vecnode.push_back(mstnode);

        /*uint256 txid;
        txid.SetHex(mstnode._txid);
        COutPoint mnVin = COutPoint(txid, mstnode._voutid);
        AddMasterNodeMemory(mnVin, mstnode._validflag);*/
        cout<<"master tx  "<< mstnode._txid << " vouid " << mstnode._voutid <<" hostname " << mstnode._hostname << " hostip  "<< mstnode._hostip <<endl;
        i++;
    }

    cout<<"total "<<i<<" record"<<endl;
    delete stmt;
    delete resultSet;
    return true;
}

void ScanTable(string dbuser, string dbpwd, int nThread)
{
    vector<CMstNodeData> vecnode;
    ConnPool* pPool = ConnPool::GetInstance(dbuser, dbpass, nThread);
    if(pPool != NULL)
    {
        sql::Connection * pConn = pPool->GetConnection();
        if(pConn==NULL)
        {
            printf("ERROR: sql connection failed !\n");
            return;
        }
        ReadAllNodeToNet(pConn, vecnode);
        pPool->ReleaseConnection(pConn);
    }
    
}

int main(int argc, char const *argv[])
{
    SetFilePath("licensetool.conf");
	LoadConfigFile(mapConfigs, mapMultiConfigs);

    ScanTable(GetArg("-sqluser", "root"), GetArg("-sqlpwd", "123456"), atoi(GetArg("-sqlthread", "1")));

    return 0;
}

