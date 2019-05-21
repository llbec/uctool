#include "vdstool.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
//#include "crypto/common.h"

uint256 CVdsBlockHeader::GetHash() const
{
//	return SerializeHash(*this);
	uint256 hash;

	CryptoHello(this, (unsigned char *)&hash);
//	view_data_u8("PoW 3", (unsigned char *)&hash, OUTPUT_LEN); 
//	std::cout<<"gethex() ="<<hash.GetHex()<<std::endl;
//	std::cout<<"tostring ="<<hash.ToString()<<std::endl; 
	return hash;	
}

std::string CVdsBlockHeader::ToString() const                                                                                                                                                                                                                                   
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, hashClaimTrie=%s, nTime=%u, nBits=%08x, nNonce=%s)\n",
		GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        hashClaimTrie.ToString(),
        nTime, nBits, nNonce.ToString());

    return s.str();
}


void ShowVdsHash(int argc, char const * argv[])
{
    CVdsBlockHeader block;
    block.nVersion = 536870912;
    block.hashPrevBlock = uint256S("431679762bf2e0a39c394d3a87f3aa1a91ee68e0e02eff7467199f168d1630fd");
    block.hashMerkleRoot = uint256S("e7c7ff5c98240449fd776a89cc0060e8737bdb200870201f478f6e261741e6ee");
    block.nTime = 1558408176;
    block.nBits = std::stoi("1c197221",nullptr,16);
    block.nNonce = uint256S("012f7b3d000000000000000000000000000000000000000000000000d6ed0060");

    printf("%s\n", block.ToString().c_str());
    return 0;
}