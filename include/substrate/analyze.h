#ifndef __SUBSTRATE__ANALYZE_H
#define __SUBSTRATE__ANALYZE_H

#include <osl/osl.h>

#include "substrate/statement_profile.h"

struct osl_scop * substrate_analyze(struct osl_scop * scop);

#endif
