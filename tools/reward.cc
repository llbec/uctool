#include "reward.h"
#include "main.h"
#include "ulord.h"
#include "utils.h"

#include <iomanip>

using namespace std;

void GetBlockReward(int height, CAmount & nMiner, CAmount & nBud, CAmount & nMN, CAmount & nFud)
{
    const Consensus::Params &cp = Params().GetConsensus();
    //Miner
    nMiner = GetMinerSubsidy(height, cp);

    //budget&MasterNode
    if(height >= cp.nMasternodePaymentsStartBlock) {
        if(height >= cp.nSuperblockStartBlock &&
            ((height % cp.nSuperblockCycle) == 0)) {
            //Budget
            nBud = GetBudget(height, cp);
            nMN = 0;
        } else {
	    //MasterNode
            nMN = GetMasternodePayment(height);
            nBud = 0;
	}
    } else {
	nBud = 0;
	nMN = 0;
    }

    //Founder
    if (height > 1 && height < Params().GetConsensus().endOfFoundersReward()) 
    {
        nFud = GetFoundersReward(height, cp);
    } else {
        nFud = 0;
    }
}

string ShowAmount(const CAmount & n)
{
    CAmount nInt = n / COIN;
    CAmount nDecimal = n - (nInt*COIN);
    return Strings::Format("%ld.%ld", nInt, nDecimal);
}

void ShowReward(int argc, char const * argv[])
{
    int iYears = 0;
    int iRounds = 0;
    if (argc > 2) {
        iYears = atoi(argv[2]);
        iRounds = argc > 3 ? atoi(argv[3]) : 1;
    }
    CAmount blkminer, blkbud, blkmn, blkfud = 0;

    GetBlockReward(iYears, blkminer, blkbud, blkmn, blkfud);

    cout << "    height        MinerSubsidy              Budget    MasternodePayment     FoundersReward        BlockSubsidy" << endl;
	cout << setw(10) << iYears
		<< setw(20) << ShowAmount(blkminer)
		<< setw(20) << ShowAmount(blkbud)
		<< setw(20) << ShowAmount(blkmn)
		<< setw(20) << ShowAmount(blkfud)
		<< setw(20) << ShowAmount(blkbud + blkfud + blkminer + blkmn) << endl;
}
