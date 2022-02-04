#include "smkhe/public_key.h"

PublicKey::PublicKey(Parameters params, vector<Polynomial<uint64_t>> polyA, vector<Polynomial<uint64_t>> polyB)
        : params(params), polyA(polyA), polyB(polyB) {}

Polynomial<uint64_t> PublicKey::getA(int level) {
    return polyA[level];
}

Polynomial<uint64_t> PublicKey::getB(int level) {
    return polyB[level];
}
