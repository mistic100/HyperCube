void nblendPaletteTowardPaletteReverse( CRGBPalette16& current, CRGBPalette16& target, uint8_t maxChanges) {
    uint8_t* p1;
    uint8_t* p2;
    uint8_t  changes = 0;

    p1 = (uint8_t*)current.entries;
    p2 = (uint8_t*)target.entries;

    const uint8_t totalChannels = sizeof(CRGBPalette16);
    for( uint8_t i = totalChannels; i > 0; i--) {
        // if the values are equal, no changes are needed
        if( p1[i-1] == p2[i-1] ) { continue; }

        // if the current value is less than the target, increase it by one
        if( p1[i-1] < p2[i-1] ) { p1[i-1]++; changes++; }

        // if the current value is greater than the target,
        // increase it by one (or two if it's still greater).
        if( p1[i-1] > p2[i-1] ) {
            p1[i-1]--; changes++;
            if( p1[i-1] > p2[i-1] ) { p1[i-1]--; }
        }

        // if we've hit the maximum number of changes, exit
        if( changes >= maxChanges) { break; }
    }
}

uint8_t lerp(float value, uint8_t from, uint8_t to){
  return ((1 - value) * from) + (value * to);
}

float invLerp(uint8_t value, uint8_t from, uint8_t to){
  return 1.0 * (value - from) / (to - from);
}

uint8_t changeScale(uint8_t value, uint8_t from1, uint8_t from2, uint8_t to1, uint8_t to2) {
  if (from1 > from2) {
    return lerp(1 - invLerp(value, from2, from1), to1, to2);
  } else {
    return lerp(invLerp(value, from1, from2), to1, to2);
  }
}
