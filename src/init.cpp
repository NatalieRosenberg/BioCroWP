// WARNING: This file was included in this package by the BioCro skeleton module
// library and should not be manually edited.

// This code in this file is necessary to register dynamic symbols for use in R.
// For more information, please see the following sources:
//
// - Section 5.4 (Registering native routines) of the "Writing R Extensions" guide:
//   https://cran.r-project.org/doc/manuals/r-release/R-exts.html#Registering-native-routines
//
// - The brief, high-level introduction in R News (volume 1/3, September 2001,
//   pages 20–23): https://www.r-project.org/doc/Rnews/Rnews_2001-3.pdf
//
// - The source code for the `stats` R package, especially `init.c`:
//   https://github.com/wch/r-source/blob/trunk/src/library/stats/src/

#include <stdlib.h>            // for NULL
#include <R_ext/Rdynload.h>    // for R_CallMethodDef, R_registerRoutines, R_forceSymbols
#include <R_ext/Visibility.h>  // for attribute_visible

#include "R_module_library.h"
#include "R_skeleton_version.h"
#include "R_framework_version.h"

extern "C" {
static const R_CallMethodDef callMethods[] = {
    {"R_get_all_modules",        (DL_FUNC) &R_get_all_modules,        0},
    {"R_get_all_quantities",     (DL_FUNC) &R_get_all_quantities,     0},
    {"R_module_creators",        (DL_FUNC) &R_module_creators,        1},
    {"R_skeleton_version",       (DL_FUNC) &R_skeleton_version,       0},
    {"R_framework_version",      (DL_FUNC) &R_framework_version,      0},
    {NULL,                       NULL,                                0}
};

// The name of this function must be `R_init_NAME`, where `NAME` must exactly
// match the name of the R package.
void attribute_visible R_init_BioCroWP(DllInfo* info)
{
    // We only use .Call in our R code, so we only need to register call methods
    R_registerRoutines(info, NULL, callMethods, NULL, NULL);

    // Don't allow searching; i.e., only allow registered symbols to be called
    // from R code
    R_useDynamicSymbols(info, FALSE);

    // Only allow R objects (not character strings) when using .Call from R
    R_forceSymbols(info, TRUE);
}
}
