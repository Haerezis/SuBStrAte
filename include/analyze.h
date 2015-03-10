#ifndef __SUBSTRATE__ANALYZE_H
#define __SUBSTRATE__ANALYZE_H

#include <osl/osl.h>

#include "statement_profile.h"

struct substrate_scop_profile substrate_analyze(struct osl_scop * scop);

#endif
