#ifndef UNIGD_EXTERNAL_API_H
#define UNIGD_EXTERNAL_API_H

#ifndef R_NO_REMAP
#define R_NO_REMAP
#endif

#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include <stdlib.h>
#include <stdint.h>

#include <unigd_api_v1.h>

static inline int unigd_api_v1_create(unigd_api_v1 **api)
{
    static int(*ptr_api_v1_create)(unigd_api_v1 **) = NULL;
    if (ptr_api_v1_create == NULL) {
        ptr_api_v1_create = (int(*)(unigd_api_v1 **)) R_GetCCallable("unigd", "api_v1_create");
    }
    return ptr_api_v1_create(api);
}

static inline int unigd_api_v1_destroy(unigd_api_v1 *api)
{
    static int(*ptr_api_v1_destroy)(unigd_api_v1 *) = NULL;
    if (ptr_api_v1_destroy == NULL) {
        ptr_api_v1_destroy = (int(*)(unigd_api_v1 *)) R_GetCCallable("unigd", "api_v1_destroy");
    }
    return ptr_api_v1_destroy(api);
}

#endif // UNIGD_EXTERNAL_API_H
