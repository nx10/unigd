#ifndef __UNIGD_R_THREAD_H__
#define __UNIGD_R_THREAD_H__

#include <future>
#include <thread>
#include <type_traits>  // for std::invoke_result/result_of

#include "async_utils.h"

namespace unigd
{
namespace async
{
void ipc_open();
void ipc_close();

void r_thread_impl(function_wrapper&& f);

template <typename FunctionType>
#if defined(__cplusplus) && __cplusplus >= 201703L
std::future<typename std::invoke_result_t<FunctionType>> r_thread(FunctionType f)
#else
std::future<typename std::result_of<FunctionType()>::type> r_thread(FunctionType f)
#endif
{
#if defined(__cplusplus) && __cplusplus >= 201703L
  typedef typename std::invoke_result_t<FunctionType> result_type;
#else
  typedef typename std::result_of<FunctionType()>::type result_type;
#endif
  std::packaged_task<result_type()> task(std::move(f));
  std::future<result_type> res(task.get_future());
  r_thread_impl(std::move(task));
  return res;
}

}  // namespace async
}  // namespace unigd

#endif /* __UNIGD_R_THREAD_H__ */
