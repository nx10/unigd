#include "unigd_external.h"
#include <R_ext/Rdynload.h>

int ugd_c_test_fun_()
{
    return 7;
}

namespace unigd {
  void unigd_register_exports()
  {
    R_RegisterCCallable("unigd", "ugd_test_fun", (DL_FUNC)ugd_c_test_fun_);
  }
}