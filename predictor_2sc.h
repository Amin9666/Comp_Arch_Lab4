#ifndef PREDICTOR_H
#define PREDICTOR_H

/*
  2-bit Saturating Counter Branch Predictor
  ------------------------------------------
  Budget: 14-bit PC index -> 2^14 = 16384 entries, each a 2-bit saturating counter.
  Total memory: 16384 * 2 = 32768 bits = 32K bits.

  States: 0=StronglyNotTaken, 1=WeaklyNotTaken, 2=WeaklyTaken, 3=StronglyTaken
  Prediction: taken if counter >= 2, else not taken.
  All entries initialized to 0 (StronglyNotTaken).
*/

#define INDEX_BITS 14
#define BHT_ENTRIES (1 << INDEX_BITS)   /* 16384 entries */
#define INDEX_MASK  (BHT_ENTRIES - 1)

extern unsigned char bht[BHT_ENTRIES]; /* 2-bit saturating counters */

void init_predictor ();
bool make_prediction (unsigned int pc);
void train_predictor (unsigned int pc, bool outcome);

#endif
