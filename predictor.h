#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "utils.h"
#include "tracer.h"
#include <bitset>
#include <stdint.h>

class PREDICTOR{
 private:
  bitset<59> ghr;
  UINT32 s;
  UINT32 table[512][60];
 public:
  PREDICTOR(void);
  bool    GetPrediction(UINT32 PC);
  void    UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget);
  void    TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget);
  UINT32 HashPC(UINT32 PC);
};
#endif
