#include "reward.h"
#include "ulord.h"

void GetBlockReward(int height, CAmount & nMiner, CAmount & nBud, CAmount nMN, CAmount & nFud)
{
    const Consensus::Params &cp = Params().GetConsensus();
    //Miner
    nMiner = GetMinerSubsidy(height, cp);

    //budget&MasterNode
    if(height >= < cp.nMasternodePaymentsStartBlock) {
        if(!CSuperblock::IsValidBlockHeight(nBlockHeight)) {
            //MasterNode
            nMN = GetMasternodePayment(height);
            nBud = 0;
        } else {
            //Budget
            nBud = GetBudget(height, cp);
            nMN = 0;
        }
    }

    //Founder
    if (height > 1 && height < Params().GetConsensus().endOfFoundersReward()) 
    {
        nFud = GetFoundersReward(height, cp);
    } else {
        nFud = 0;
    }
}

void ShowReward(int argc, char const * argv[])
{
    int iYears = 0;
    int iRounds = 0;
    if (argc > 2) {
        iYears = atoi(argv[2]);
        iRounds = argc > 3 ? atoi(argv[3]) : 1;
    }
    int blkminer, blkbud, blkmn, blkfud = 0;

    GetBlockReward(iYears, blkMiner, blkbud, blkmn, blkfud);

    cout << "    height        MinerSubsidy              Budget    MasternodePayment     FoundersReward        BlockSubsidy" << endl;
	cout << setw(10) << h
		<< setw(20) << blkminer / COIN
		<< setw(20) << blkbud / COIN
		<< setw(20) << blkmn / COIN
		<< setw(20) << blkfud / COIN
		<< setw(20) << (blkbud + blkfud + blkminer + blkmn) / COIN << endl;
}