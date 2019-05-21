#include "vdsblock.h"

#include "hash.h"
#include "tinyformat.h"
#include <util/strencodings.h>
#include "crypto/common.h"

uint256 CVdsBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}
