//---------------------------------------------------------------------------

#ifndef uModelModeH
#define uModelModeH

#include "uCommon.h"

void init_model_array(void);
int model_scheme(void);
void reset_charges(void);
int is_cycle_exist(int target_component, int out_wire);

//---------------------------------------------------------------------------
#endif
