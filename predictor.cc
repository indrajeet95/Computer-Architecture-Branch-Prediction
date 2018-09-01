#include "predictor.h"
#include <bitset>
#include <stdint.h>

/////////////// STORAGE BUDGET JUSTIFICATION ////////////////
// Total storage budget: 32KB
// Total bits in history register:  59 bits
// Total number of weights per perceptron:  59 + 1 = 60
// Total bits to store weight: 8
// Total number of perceptrons: 512
// Threshold = 1.93*h + 14 = 1.93*59 + 14 = 127.87 ~ 128
// Total perceptron table size = 512 * 60 * 8 = 245760 bits
// Total perceptron steps table size = 512 * 8 = 4096 bits
// Total Size = perceptron table size + history register size
//       + perceptron steps table = 245760 + 4096 + 59 = 249915 bits
/////////////////////////////////////////////////////////////

PREDICTOR::PREDICTOR(void){
  s = 0;
  ghr = bitset<59>();
  for(UINT32 i=0; i < 512; i++){
    for(UINT32 j=0; j < 59; j++){
	  table[i][j] = 0;                 // initialize all values to zero
    }
  }
}

bool   PREDICTOR::GetPrediction(UINT32 PC){
  UINT32 index = HashPC(PC);
  INT32 p = 0;
  p += table[index][0];
  for(UINT32 i=1; i < 60; i++){
    if(ghr[i - 1] == 1){            //if taken
	  p += table[index][i];          // add weight
    }
    else{
	  p -= table[index][i];          // subtract weight
    }
  }
  s = abs(p);
 if(p >= 0){
    return TAKEN;                 //greater than zero predtict taken
  }
  else{
    return NOT_TAKEN;           //lesser than zero predict not taken
  }
}

void  PREDICTOR::UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget){
  UINT32 index = HashPC(PC);
  if(resolveDir != predDir || s <= 127){ //update if lesser than Threshold
    if(resolveDir == TAKEN){
	  int32_t updateVal = table[index][0] + 1;
	  if (updateVal > 128){
		  table[index][0] = 128;
	  }
	  else{
		 table[index][0]++;
	  }
	}
    else{
	  int32_t updateVal = table[index][0] - 1;
	  if (updateVal < -128){
		  table[index][0] = -128;
	  }
	  else{
		  table[index][0]--;
	  }
	}
	for(UINT32 i = 1; i < 60; i++){
       if((resolveDir == TAKEN && ghr[i - 1] == 1) || (resolveDir == NOT_TAKEN && ghr[i - 1] == 0)){
		 int32_t updateVal = table[index][i] + 1;
	     if (updateVal > 127){
		   table[index][i] = 127;
	     }
	      else{
			  table[index][i]++;
		  }
       }
	   else{
	     int32_t updateVal = table[index][i] - 1;
	     if (updateVal < 127 * (-1)){
		   table[index][i] = 127 * (-1);
	     }
		 else{
			 table[index][i]--;
		 }
	   }
	}
  }
  ghr = (ghr << 1);
  if(resolveDir == TAKEN){
    ghr.set(0, 1);
  }
  else{
	ghr.set(0, 0);
  }
}

void    PREDICTOR::TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget){

  // This function is called for instructions which are not
  // conditional branches, just in case someone decides to design
  // a predictor that uses information from such instructions.
  // We expect most contestants to leave this function untouched.

  return;
}

UINT32 PREDICTOR::HashPC(UINT32 PC){
	UINT32 PCend = PC % 512;
	UINT32 ghrend = ((UINT32)ghr.to_ulong()) % 512;
	return PCend ^ ghrend;
}
