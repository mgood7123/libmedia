//
// Created by Mac on 16/5/20.
//

#ifndef MEDIA_PLAYER_PRO_TIMER_H
#define MEDIA_PLAYER_PRO_TIMER_H

//Author: mgood7123 (Matthew James Good) http://github.com/mgood7123

#include <stdio.h>
#include <stdarg.h>

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)

/* C99-style: anonymous argument referenced by __VA_ARGS__, empty arg not OK */

# define N_ARGS(...) N_ARGS_HELPER1(__VA_ARGS__, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
# define N_ARGS_HELPER1(...) N_ARGS_HELPER2(__VA_ARGS__)
# define N_ARGS_HELPER2(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, n, ...) n

#elif defined(__GNUC__)

/* GCC-style: named argument, empty arg is OK */

# define N_ARGS(args...) N_ARGS_HELPER1(args, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
# define N_ARGS_HELPER1(args...) N_ARGS_HELPER2(args)
# define N_ARGS_HELPER2(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, n, x...) n

#else

#error variadic macros for your compiler here

#endif


#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
/* C99-style: anonymous argument referenced by __VA_ARGS__, empty arg not OK */
# define foo(...) foo_helper(N_ARGS(__VA_ARGS__), __VA_ARGS__)
# define clock__total(...) clock__total_h(N_ARGS(__VA_ARGS__), __VA_ARGS__)
#elif defined(__GNUC__)
/* GCC-style: named argument, empty arg is OK */
# define foo(args...) foo_helper(N_ARGS(args), args)
# define clock__total(args...) clock_total__h(N_ARGS(args), args)
#endif

static inline void foo_helper(unsigned int n_args, ...)
{
    unsigned int i, arg;
    va_list ap;
    va_start(ap, n_args);
    printf("%u argument(s):\n", n_args);
    for (i = 0; i < n_args; i++) {
        arg = va_arg(ap, unsigned int);
        printf("  %u\n", arg);
    }
    va_end(ap);
}


#include <time.h>
#include <stdbool.h>
#include <chrono>

bool has_time = false;

#define clock__get(clock) builtin_clock_##clock
#define clock__timepoint std::chrono::steady_clock::time_point
#define clock__declare(clock) clock__timepoint clock__get(clock)
#define clock__declare__pair(clock_start, clock_end) std::chrono::steady_clock::time_point clock__get(clock_start), clock__get(clock_end)
#define clock__set(clock) clock__get(clock) = std::chrono::steady_clock::now()
#define clock__declare__and__set(clock) clock__declare(clock); clock__set(clock)
#define clock__shorten(x) #x, builtin_clock_##x##_start, builtin_clock_##x##_end
#define clock__calculate__nanoseconds(clock_start, clock_end) std::chrono::duration_cast<std::chrono::nanoseconds>(clock__get(clock_end) - clock__get(clock_start))
#define clock__calculate__microseconds(clock_start, clock_end) std::chrono::duration_cast<std::chrono::microseconds>(clock__calculate__nanoseconds(clock_start, clock_end))
#define clock__calculate__milliseconds(clock_start, clock_end) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration_cast<std::chrono::microseconds>(clock__calculate__nanoseconds(clock_start, clock_end)))
#define clock__calculate__seconds(clock_start, clock_end) std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration_cast<std::chrono::microseconds>(clock__calculate__nanoseconds(clock_start, clock_end))))


#define clock__declare__print_timing_information_function(function_name) \
void function_name(const char * executed_function, clock__declare(start), clock__declare(end))

/**
 * func: block of code to time and execute
 *
 * print_timing_information_function: block of code to print timing information
 *
 * one usually invokes this as
 *
 * <pre> {@code clock__declare__print_timing_information_function(print_time) {
    AudioTime x = AudioTime();
    x.calculateNanoseconds(clock__calculate__nanoseconds(start, end).count());
    printf("\n\n%s took %lu minutes, %lu seconds, %lu milliseconds, %lu microseconds, and "%lu nanoseconds"
    " to execute\n\n",
        executed_function, x.minutes, x.seconds, x.milliseconds, x.microseconds, x.nanoseconds);
};
// ...
        clock__time__code__block(function(argument), print_time); </pre>
 */
#define clock__time__code__block(func, print_timing_information_function) { \
    clock__declare__pair(start, end); \
	clock__set(start); \
	func; \
	clock__set(end); \
	print_timing_information_function(#func, clock__get(start), clock__get(end)); \
}

#endif //MEDIA_PLAYER_PRO_TIMER_H
