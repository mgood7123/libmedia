package libclient_service;

public class InfiniteLoop {
    /**
     * @param sleepTimeInNanoseconds time in nanoseconds
     * @throws IllegalArgumentException if the param {@code sleepTimeInNanoseconds} is greater than {@code 999}
     */
    public void setSleepTimeInNanoseconds(int sleepTimeInNanoseconds) throws IllegalArgumentException {
        if (sleepTimeInNanoseconds > 999) throw new IllegalArgumentException(
            "nanoseconds must not be greater than 999"
        );
        this.sleepTimeInNanoseconds = sleepTimeInNanoseconds;
    }

    /**
     * @param sleepTimeInMicroseconds time in microseconds
     * @throws IllegalArgumentException if the param {@code sleepTimeInMicroseconds} is greater than {@code 999}
     */
    public void setSleepTimeInMicroseconds(int sleepTimeInMicroseconds) throws IllegalArgumentException {
        if (sleepTimeInMicroseconds > 999) throw new IllegalArgumentException(
            "microseconds must not be greater than 999"
        );
        // a nanosecond is      xxx999
        // a microsecond is     999xxx
        if (sleepTimeInNanoseconds <= 999) {
            // if nanoseconds is under 1 microsecond, just add val * 1000
            sleepTimeInNanoseconds += (sleepTimeInMicroseconds * 1000);
        } else {
            // if nanosecond is greater than 1 microsecond, extract nanoseconds and add val * 1000

            // obtain first 3 digits from nanoseconds : xxxyyy -> xxx
            final int nanosecondsDividedByOneThousand = sleepTimeInNanoseconds / 1000;

            // multiply first 3 digits by a thousand: xxx -> xxx000
            final int nanosecondsMultipliedByOneThousand = nanosecondsDividedByOneThousand * 1000;

            // remove first three digits from nanoseconds
            final int extractedNanoseconds = sleepTimeInNanoseconds - nanosecondsMultipliedByOneThousand;
            // 0 * 1000 would just be 0000 so avoid computation
            if (sleepTimeInMicroseconds == 0) sleepTimeInNanoseconds = extractedNanoseconds;
            else sleepTimeInNanoseconds = extractedNanoseconds + (sleepTimeInMicroseconds * 1000);
        }
    }

    /**
     * @param sleepTimeInMilliseconds time in milliseconds
     * @throws IllegalArgumentException if the param {@code sleepTimeInMilliseconds} is greater than {@code 999}
     */
    public void setSleepTimeInMilliseconds(int sleepTimeInMilliseconds) {
        if (sleepTimeInMilliseconds > 999) throw new IllegalArgumentException(
            "milliseconds must not be greater than 999"
        );
        this.sleepTimeInMilliseconds = sleepTimeInMilliseconds;
    }

    private int sleepTimeInNanoseconds = 0;
    private int sleepTimeInMilliseconds = 0;

    /**
     * @param condition a condition to check for, this can be set to any valid CODE block that can
     *                 be put into a function such as the following:
     *                  <pre> {@code boolean check_condition() {
     *      // CODE
     *  }
     *  // ...
     *      while ( check_condition() ) {
     *          // do stuff
     *      }
     *                  }</pre>
     *                  for example:
     * <pre>{@code boolean a = true;
     * boolean b = true;
     * Object c = null;
     *
     * loop(
     *     () -> a ? true : (b ? false : (c != null))
     * ); } </pre>
     * or
     * <pre>{@code boolean a = true;
     * boolean b = true;
     * Object c = null;
     *
     * loop(
     *     () -> {
     *         if (a) return true;
     *         if (b) return false;
     *         return c != null;
     *     }
     * );
     * } </pre>
     */
    public void loop(RunnableReturn<Boolean> condition) {
        while(condition.run()) {
            try {
                Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
            } catch (InterruptedException e) {
                throw new AssertionError("Thread Interrupted", e);
            }
        }
    }

    /**
     * @param runnable a {@link RunnableThrowableException} object, this runnable is capable of
     *                throwing exceptions, this is executed in a
     * <pre>{@code while (true)}</pre> loop:
     *                <pre>{@code loopMayThrowException(
     *     () -> {
     *         // CODE HERE
     *     }
     * )
     * //    while(true) {
     * //        runnable.run();
     * //        try {
     * //            Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
     * //        } catch (InterruptedException e) {
     * //            throw new AssertionError("Thread Interrupted", e);
     * //        }
     * //    }
     * }</pre>
     * @throws Exception
     */
    public void loopMayThrowException(RunnableThrowableException runnable) throws Exception {
        while(true) {
            runnable.run();
            try {
                Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
            } catch (InterruptedException e) {
                throw new AssertionError("Thread Interrupted", e);
            }
        }
    }

    /**
     * @param condition a condition to check for, this can be set to any valid CODE block that can
     *                 be put into a function such as the following:
     *                  <pre> {@code boolean check_condition() {
     *      // CODE
     *  }
     *  // ...
     *      while ( check_condition() ) {
     *          // do stuff
     *      }
     *                  }</pre>
     *                  for example:
     * <pre>{@code boolean a = true;
     * boolean b = true;
     * Object c = null;
     *
     * loopMayThrowException(
     *     () -> a ? true : (b ? false : (c != null)),
     *     runnableThrowableExceptionObject
     * ); } </pre>
     * or
     * <pre>{@code boolean a = true;
     * boolean b = true;
     * Object c = null;
     *
     * loopMayThrowException(
     *     () -> {
     *         if (a) return true;
     *         if (b) return false;
     *         return c != null;
     *     },
     *     runnableThrowableExceptionObject
     * );
     * } </pre>
     * @param runnable a {@link RunnableThrowableException} object, this runnable is capable of
     *                throwing exceptions, this is executed in a
     * <pre>{@code while (
     *     condition.run()
     * )}</pre> loop:
     *                <pre>{@code loopMayThrowException(
     *     () -> true,
     *     () -> {
     *         // CODE HERE
     *     }
     * )
     * //    while(condition.run()) {
     * //        runnable.run();
     * //        try {
     * //            Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
     * //        } catch (InterruptedException e) {
     * //            throw new AssertionError("Thread Interrupted", e);
     * //        }
     * //    }
     * }</pre>
     * @throws Exception
     */
    public void loopMayThrowException(RunnableReturn<Boolean> condition, RunnableThrowableException runnable) throws Exception {
        while(condition.run()) {
            runnable.run();
            try {
                Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
            } catch (InterruptedException e) {
                throw new AssertionError("Thread Interrupted", e);
            }
        }
    }

    /**
     * @param runnable a {@link Runnable} object, this is executed in a
     * <pre>{@code while (true)}</pre> loop:
     *                <pre>{@code loop(
     *     () -> true,
     *     new Runnable() {
     *         @Override
     *         public void run() {
     *             // CODE HERE
     *         }
     *     }
     * )
     * //    while(true) {
     * //        runnable.run();
     * //        try {
     * //            Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
     * //        } catch (InterruptedException e) {
     * //            throw new AssertionError("Thread Interrupted", e);
     * //        }
     * //    }
     * }</pre>
     */
    public void loop(Runnable runnable) {
        while(true) {
            runnable.run();
            try {
                Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
            } catch (InterruptedException e) {
                throw new AssertionError("Thread Interrupted", e);
            }
        }
    }

    /**
     * @param condition a condition to check for, this can be set to any valid CODE block that can
     *                 be put into a function such as the following:
     *                  <pre> {@code boolean check_condition() {
     *      // CODE
     *  }
     *  // ...
     *      while ( check_condition() ) {
     *          // do stuff
     *      }
     *                  }</pre>
     *                  for example:
     * <pre>{@code boolean a = true;
     * boolean b = true;
     * Object c = null;
     *
     * loop(
     *     () -> a ? true : (b ? false : (c != null)),
     *     runnableObject
     * ); } </pre>
     * or
     * <pre>{@code boolean a = true;
     * boolean b = true;
     * Object c = null;
     *
     * loop(
     *     () -> {
     *         if (a) return true;
     *         if (b) return false;
     *         return c != null;
     *     },
     *     runnableObject
     * );
     * } </pre>
     * @param runnable a {@link Runnable} object, this is executed in a
     * <pre>{@code while (
     *     condition.run()
     * )}</pre> loop:
     *                <pre>{@code loop(
     *     () -> true,
     *     new Runnable() {
     *         @Override
     *         public void run() {
     *             // CODE HERE
     *         }
     *     }
     * )
     * //    while(condition.run()) {
     * //        runnable.run();
     * //        try {
     * //            Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
     * //        } catch (InterruptedException e) {
     * //            throw new AssertionError("Thread Interrupted", e);
     * //        }
     * //    }
     * }</pre>
     */
    public void loop(RunnableReturn<Boolean> condition, Runnable runnable) {
        while(condition.run()) {
            runnable.run();
            try {
                Thread.sleep(sleepTimeInMilliseconds, sleepTimeInNanoseconds);
            } catch (InterruptedException e) {
                throw new AssertionError("Thread Interrupted", e);
            }
        }
    }
}
