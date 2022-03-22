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

// Types

#include <unigd_api/client.h> 
#include <memory>


// Exports

namespace unigd {

    static inline int ugd_test_fun() {
        typedef int (*FUN_TYPE)(); 
        static FUN_TYPE fun = NULL;
        if (fun == NULL) {
            fun = (FUN_TYPE) R_GetCCallable("unigd", "ugd_test_fun_");
        }
        return fun();
    }

    static inline bool ugd_attach_client(int devnum, const std::shared_ptr<unigd::graphics_client> &t_client) {
        try {
            typedef bool (*FUN_TYPE)(int devnum, const std::shared_ptr<unigd::graphics_client> &); 
            static FUN_TYPE fun = NULL;
            if (fun == NULL) {
                fun = (FUN_TYPE) R_GetCCallable("unigd", "ugd_attach_client_");
            }
            return fun(devnum, t_client);
        } catch (const std::exception& e) {
            REprintf("Attach error: %s\n",e.what());
            return false;
        } catch (...) {
            return false;
        }
    }

}

#endif // UNIGD_EXTERN_API_H