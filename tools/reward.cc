#include "reward.h"
#include "main.h"
#include "ulord.h"
#include "utils.h"

#include <iomanip>

using namespace std;

#define YEARBLOCKS = 210240

void GetBlockReward(int height, CAmount & nMiner, CAmount & nBud, CAmount & nMN, CAmount & nFud)
{
    const Consensus::Params &cp = Params().GetConsensus();

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
    } else if(height == 1) {
        nFud = GetMinerSubsidy(height, cp);
    } else {
        nFud = 0;
    }

    //Miner
    if (height == 1) {
        nMiner == 0
    } else {
        nMiner = GetMinerSubsidy(height, cp);
    }
}

string ShowAmount(const CAmount & n)
{
    CAmount nInt = n / COIN;
    CAmount nDecimal = n - (nInt*COIN);
    return Strings::Format("%ld.%ld", nInt, nDecimal);
}

void showBlock(const int & height, const CAmount & nMiner, const CAmount & nBud, const CAmount & nMN, const CAmount & nFud)
{
    cout << setw(10) << height
		<< setw(20) << ShowAmount(nMiner)
		<< setw(20) << ShowAmount(nBud)
		<< setw(20) << ShowAmount(nMN)
		<< setw(20) << ShowAmount(nFud)
		<< setw(20) << ShowAmount(nBud + nFud + nMiner + nMN) << endl;
}

void showYears(const int & nyear, const CAmount & nMiner, const CAmount & nBud, const CAmount & nMN, const CAmount & nFud)
{}

void ShowReward(int argc, char const * argv[])
{
    /*int iYears = 0;
    int iRounds = 0;*/
    int h = 0;
    /*if (argc > 2) {
        iYears = atoi(argv[2]);
        iRounds = argc > 3 ? atoi(argv[3]) : 1;
    }*/
    CAmount blkminer, blkbud, blkmn, blkfud, amtSum = 0;

    cout << "    height        MinerSubsidy              Budget    MasternodePayment     FoundersReward        BlockSubsidy" << endl;

    /*auto isQuit = []() {
        if(iYears == 0 && iRounds == 0) {
            if(amtSum >= 100000000000000000)
                return true;
        } else {
            if(h >= (CAmount)(iYears*iRounds*YEARBLOCKS))
                return true;
        }
        return false;
    }*/

    CAmount tmpminer, tmpbud, tmpmn, tmpfud = 0;
    CAmount summiner, sumbud, summn, sumfud = 0;
    //CAmount rminer, rbud, rmn, rfud = 0;
    /*if(iYears == 0 && iRounds == 0) {
        //for(; amtSum < 100000000000000000; h++)
        iYears = 4;
    } else {
    }*/
    while(true)
    {
        GetBlockReward(h, blkminer, blkbud, blkmn, blkfud);
        if(blkminer == 0 && blkbud == 0 && blkmn == 0 && blkfud == 0) {
            showBlock(h, blkminer, blkbud, blkmn, blkfud);
            showBlock(h, summiner, sumbud, summn, sumfud);
            return;
        }
        if(tmpminer != blkminer && tmpbud != blkbud && tmpmn != blkmn, tmpfud != blkfud) {
            showBlock(h, blkminer, blkbud, blkmn, blkfud);
            btmpminer = blkminer;
            tmpbud = blkbud;
            tmpmn = blkmn;
            tmpfud = blkfud;
        }
        summiner += blkminer;
        sumbud += blkbud;
        summn += blkmn;
        sumfud += blkfud;
        amtSum += (blkminer+blkbud+blkmn+blkfud);
    }
}
