//
// Created by Andru Stefanescu on 31.01.2022.
//

#include "smkhe/public_key.h"

PublicKey::PublicKey(Parameters params) : params(params), polyA(params.getRingDegree()), polyB(params.getRingDegree()) {}
