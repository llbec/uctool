#include "keytool.h"
#include "privsend.h"
#include "utils.h"
#include "ulord.h"

using namespace std;

void NewKey(int argc, char const * argv[])
{
    try {
        CKeyTool key(true);
        printf("Private Key: %s\nPublic Key: %s\nAddress : %s\n",
                key.GetKeyString().c_str(),
                key.GetPubKeyString().c_str(),
                key.GetAddress().c_str());
    }
    catch(int) {
        printf("Create New Key failed!\n");
        return;
    }
    return;
}

void GenKey(int argc, char const * argv[])
{
    try {
        CKeyTool key(false);
        printf("Masternode Key: %s\nPublic Key: %s\n",
                key.GetKeyString().c_str(),
                key.GetPubKeyString().c_str());
    }
    catch(int) {
        printf("Create new masternode Key failed!\n");
        return;
    }
    return;
}

void ShowkeyHelp()
{
    cout << "Command \"keyshow\" example :" << endl << endl
        << "keyshow stringPrivkey" << endl
		<< "keyshow \"L1kF5amYLK6JZuuWyHTEk7dArcTu5nucFrGC9bUxmHezd8fdY183\"" << endl;
}
void Showkey(int argc, char const * argv[])
{
    if(argc < 3) {
        ShowkeyHelp();
        return;
    }
    string strpriv = argv[2];
    try {
        CKeyTool key(strpriv);
        printf("Private Key: %s\nPublic Key: %s\nAddress : %s\n",
                key.GetKeyString().c_str(),
                key.GetPubKeyString().c_str(),
                key.GetAddress().c_str());
    }
    catch(int) {
        printf("String(%s) is not a valid private key!\n", strpriv.c_str());
        return;
    }
    return;
}

