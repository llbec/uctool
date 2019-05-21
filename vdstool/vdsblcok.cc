#include "vdsblock.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "crypto/common.h"

uint256 CVdsBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}