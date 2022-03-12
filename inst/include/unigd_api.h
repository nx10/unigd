#ifndef UNIGD_EXTERN_API_H
#define UNIGD_EXTERN_API_H


// Setup R includes

#ifndef R_NO_REMAP
#define R_NO_REMAP
#endif
#ifndef STRICT_R_HEADERS
#define STRICT_R_HEADERS
#endif
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


// Exports

static inline int ugd_test_fun() {
    typedef int (*BLUB)(); 
    static BLUB fun = NULL;
    if (fun == NULL) {
        fun = (BLUB) R_GetCCallable("unigd", "ugd_test_fun_");
    }
    return fun();
}


#endif // UNIGD_EXTERN_API_H