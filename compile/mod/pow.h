template<typename Int, typename Exp>
Int pow(Int i, Exp e) {
    Int result = 1;
    while(e) {
        if(e % 2 == 1) {
            result *= i;
        }
        i *= i;
        e /= 2;
    }
    return result;
}
