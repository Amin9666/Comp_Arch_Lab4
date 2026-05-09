#ifndef PREDICTOR_H
#define PREDICTOR_H

/*
  GShare Branch Predictor
  -----------------------
  Budget: 14-bit GHR XOR 14-bit PC -> 2^14 = 16384 entries, each a 2-bit
  saturating counter. Total memory: 16384 * 2 + 14 = 32778 bits ~ (32K+14) bits.

  The Global History Register (GHR) holds the outcomes of the last 14 branches.
  The table index is computed as: (GHR ^ pc) & 0x3FFF.

  States: 0=StronglyNotTaken, 1=WeaklyNotTaken, 2=WeaklyTaken, 3=StronglyTaken
  Prediction: taken if counter >= 2, else not taken.
  GHR and all BHT entries initialized to 0.
*/

#define INDEX_BITS   14
#define BHT_ENTRIES  (1 << INDEX_BITS)   /* 16384 entries */
#define INDEX_MASK   (BHT_ENTRIES - 1)
#define GHR_MASK     INDEX_MASK           /* 14-bit GHR mask */

extern unsigned char bht[BHT_ENTRIES]; /* 2-bit saturating counters */
extern unsigned int  ghr;              /* Global History Register (14 bits) */

void init_predictor ();
bool make_prediction (unsigned int pc);
void train_predictor (unsigned int pc, bool outcome);

#endif
