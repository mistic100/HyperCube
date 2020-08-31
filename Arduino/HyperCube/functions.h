void nblendPaletteTowardPaletteReverse( CRGBPalette16& current, CRGBPalette16& target, uint8_t maxChanges) {
    uint8_t* p1;
    uint8_t* p2;
    uint8_t  changes = 0;

    p1 = (uint8_t*)current.entries;
    p2 = (uint8_t*)target.entries;

    const uint8_t totalChannels = sizeof(CRGBPalette16);
    for( uint8_t i = totalChannels - 1; i >= 0; i--) {
        // if the values are equal, no changes are needed
        if( p1[i] == p2[i] ) { continue; }

        // if the current value is less than the target, increase it by one
        if( p1[i] < p2[i] ) { p1[i]++; changes++; }

        // if the current value is greater than the target,
        // increase it by one (or two if it's still greater).
        if( p1[i] > p2[i] ) {
            p1[i]--; changes++;
            if( p1[i] > p2[i] ) { p1[i]--; }
        }

        // if we've hit the maximum number of changes, exit
        if( changes >= maxChanges) { break; }
    }
}
