#ifndef __UNIGD_DEBUG_PRINT_H__
#define __UNIGD_DEBUG_PRINT_H__

#ifndef UNIGD_DEBUG
#define debug_print(...)
#define debug_println(...)
#else
#define debug_print(fmt, ...) \
  Rprintf("%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#define debug_println(fmt, ...) \
  Rprintf("%s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#endif

#endif /* __UNIGD_DEBUG_PRINT_H__ */
