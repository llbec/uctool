#ifdef MYSQL_ENABLE
#include "dbtool.h"
#include "ulordDB.h"

#include "ulord.h"

//#include "util.h"
//#include "masternodeman.h"

using namespace std;

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
        if(!mn.VerifyLicense()) {
            cout << "license verify failed!" << endl;
        }
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
    
    int icount = 0;
    for(auto mn : vecRet)
    {
        string sRet;
        if(!mn.Check(sRet, db._needUpdatePeriod)) {
            cout << sRet << endl;
        } else icount++;
    }
    cout << "Read " << vecRet.size() << " entries, " << icount << " entries check ok!" << endl;
    return;
}

void DBGetAll(int argc, char const * argv[])
{
    CUlordDb::map_col_val_t mapCheck;
    vector<CMNode> vecRet;
    CUlordDb db;
    mapCheck.clear();
    if(!db.SelectMNode(mapCheck, vecRet)) {
        cout << "Read DB failed!" << endl;
        return;
    }
    
    int icount = 0;
    for(auto mn : vecRet)
    {
        string sRet;
        if(mn.Check(sRet, db._needUpdatePeriod)) {
            cout << mn.ToString() << endl;
        } else icount++;
    }
    cout << "Read " << vecRet.size() << " entries, " << icount << " entries check ok!" << endl;
    return;
}

void SelectNodeHelp()
{
    cout << "Command \"dbcheck\" example :" << endl << endl
        << "dbselect filter ..." << endl
		<< "dbselect \"trade_txid=2122660463ab2c041a8b8ab406aa314e76f2b4bf88dec75ce7b17af0c8bc2887\""<< endl
        << "major_node_nickname trade_txid ip_address special_code status cert_version validdate node_period" << endl;
}
void DBSelect(int argc, char const * argv[])
{
    if(argc < 3) {
        SelectNodeHelp();
        return;
    }
    vector<string> vecFilter;
    for(int i = 2; i < argc; i++)
        vecFilter.push_back(argv[i]);
    vector<CMNode> vecRet;
    CUlordDb db;
    if(!db.SelectMNode(vecFilter, vecRet)) {
        cout << "Read DB failed!" << endl;
        return;
    }
    for(auto mn : vecRet)
    {
        cout << mn.ToString() << endl;
    }
}
#endif // MYSQL_ENABLE
