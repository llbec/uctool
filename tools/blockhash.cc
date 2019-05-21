#include "primitives/block.h"

void ShowBlockHash(int argc, char const * argv[])
{
    CBlockHeader block;
    block.nVersion = 536870912;
    block.hashPrevBlock = uint256S("431679762bf2e0a39c394d3a87f3aa1a91ee68e0e02eff7467199f168d1630fd");
    block.hashMerkleRoot = uint256S("e7c7ff5c98240449fd776a89cc0060e8737bdb200870201f478f6e261741e6ee");
    block.nTime = 1558408176;
    block.nBits = std::stoi("1c197221",nullptr,16);
    block.nNonce = uint256S("012f7b3d000000000000000000000000000000000000000000000000d6ed0060");

    printf("%s\n", block.ToString().c_str());
    return;
}