#include "reward.h"
#include "main.h"
#include "ulord.h"
#include "utils.h"

#include <iomanip>

using namespace std;

#define YEARBLOCKS 210240

bool IsSuperBlock(int height)
{
    const Consensus::Params &cp = Params().GetConsensus();
    if(height >= cp.nMasternodePaymentsStartBlock) {
        if(height >= cp.nSuperblockStartBlock &&
            ((height % cp.nSuperblockCycle) == 0))
            return true;
    }
    return false;
}

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
        nMiner = 0;
    } else {
        nMiner = GetMinerSubsidy(height, cp);
    }
}

string ShowAmount(const CAmount & n)
{
    CAmount nInt = n / COIN;
    CAmount nDecimal = n - (nInt*COIN);
    return Strings::Format("%ld.%08ld", nInt, nDecimal);
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

void showYears(const int & n, const int & nyear, const CAmount & nMiner, const CAmount & nBud, const CAmount & nMN, const CAmount & nFud)
{
    if (n >= 0) {
        cout << setw(2) << n+1 << " x " << nyear << "    " << setw(9) << (n*nyear*YEARBLOCKS) << "--" << setw(9)  << ((n+1)*nyear*YEARBLOCKS - 1);
    } else {
        cout << "total" << setw(5) << nyear << setw(9) << 0 << "--" << setw(9)  << (nyear*YEARBLOCKS - 1);
    }
    cout << setw(20) << ShowAmount(nMiner)
            << setw(20) << ShowAmount(nBud)
            << setw(20) << ShowAmount(nMN)
            << setw(20) << ShowAmount(nFud)
            << setw(20) << ShowAmount(nBud + nFud + nMiner + nMN) << endl;
}

void ShowReward(int argc, char const * argv[])
{
    int h = 0;

    CAmount blkminer = 0, blkbud = 0, blkmn = 0, blkfud = 0, amtSum = 0;

    cout << "    height        MinerSubsidy              Budget    MasternodePayment     FoundersReward        BlockSubsidy" << endl;

    CAmount tmpminer = 0, tmpbud = 0, tmpmn = 0, tmpfud = 0;
    CAmount summiner = 0, sumbud = 0, summn = 0, sumfud = 0;

    while(true)
    {
        GetBlockReward(h, blkminer, blkbud, blkmn, blkfud);
        if(blkminer == 0 && blkbud == 0 && blkmn == 0 && blkfud == 0 && IsSuperBlock(h)) {
            showBlock(h, blkminer, blkbud, blkmn, blkfud);
            showBlock(h%(YEARBLOCKS-1)==0?h/YEARBLOCKS:(h/YEARBLOCKS+1), summiner, sumbud, summn, sumfud);
            return;
        }
        if(tmpminer != blkminer || tmpbud != blkbud || tmpmn != blkmn || tmpfud != blkfud) {
            showBlock(h, blkminer, blkbud, blkmn, blkfud);
            tmpminer = blkminer;
            tmpbud = blkbud;
            tmpmn = blkmn;
            tmpfud = blkfud;
        }
        summiner += blkminer;
        sumbud += blkbud;
        summn += blkmn;
        sumfud += blkfud;
        amtSum += (blkminer+blkbud+blkmn+blkfud);
	h++;
    }
}

void ShowRewardStatus(int argc, char const * argv[])
{
    int iYears = 4;
    int iRounds = 0;
    int h = 0;
    if (argc > 2) {
        iYears = atoi(argv[2]);
        iRounds = argc > 3 ? atoi(argv[3]) : 0;
    }

    cout << endl << "The rewards in every "<< iYears << " years status :" <<endl
		<< "year             height range         MinerSubsidy         Budget        MasternodePayment      FoundersReward       BlockSubsidy" <<endl;
    
    /*auto isQuit = []() {
        if(iRounds != 0) {
            if(h >= (CAmount)(iYears*iRounds*YEARBLOCKS))
                return true;
        } else {
            if (h*iYears >= 190)
                return true;
        }
        return false;
    }*/

    CAmount blkminer = 0, blkbud = 0, blkmn = 0, blkfud = 0;
    CAmount summiner = 0, sumbud = 0, summn = 0, sumfud = 0;
    CAmount rminer = 0, rbud = 0, rmn = 0, rfud = 0;
    bool bRunner = true;
    while(bRunner){
        for(int i = h*iYears*YEARBLOCKS; i < (h+1)*iYears*YEARBLOCKS; i++) {
            GetBlockReward(i, blkminer, blkbud, blkmn, blkfud);
            if(blkminer == 0 && blkbud == 0 && blkmn == 0 && blkfud == 0 && IsSuperBlock(i)) {
                bRunner = false;
                break;
            }
            summiner += blkminer;
            sumbud += blkbud;
            summn += blkmn;
            sumfud += blkfud;
            //amtSum += (blkminer+blkbud+blkmn+blkfud);
            rminer += blkminer;
            rbud += blkbud;
            rmn += blkmn;
            rfud += blkfud;
        }
        showYears(h, iYears, rminer, rbud, rmn, rfud);
        h++;
        rminer = 0;
        rbud = 0;
        rmn = 0;
        rfud = 0;
        if(iRounds > 0 && h >= iRounds) {
            bRunner = false;
        }
        if(!bRunner) {
            int flagtotal = -1;
            int nyears = h * iYears;
            showYears(flagtotal, nyears, summiner, sumbud, summn, sumfud);
        }
    }
}
