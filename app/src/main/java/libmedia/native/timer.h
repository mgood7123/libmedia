//
// Created by Mac on 16/5/20.
//

#ifndef MEDIA_PLAYER_PRO_TIMER_H
#define MEDIA_PLAYER_PRO_TIMER_H

//Author: mgood7123 (Matthew James Good) http://github.com/mgood7123

#include <stdio.h>

#include <time.h>
#include <stdbool.h>
#include <chrono>

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


#define clock__declare__print_timing_information_function(function_name) void function_name(const char * executed_function, clock__declare(start), clock__declare(end))

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
