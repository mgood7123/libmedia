package liblayout

import android.content.Context

/**
 * honestly i thought this would be alot more complex
 *
 * turns out this only needs a handler, thread, and Android's [android.app.Activity.runOnUiThread] method
 *
 * @sample api28.android.app.Activity.runOnUiThread
 * @sample UiThread
 *
 * @param context substitute for activity.applicationContext where activity is not available
 */
class UiThread(val context: Context) {
    private val mHandler = android.os.Handler()
    private val mUiThread: Thread = Thread.currentThread()

    /**
     * DATA array for applications to pass data to
     *
     * pass any data you need here
     */
    val DATA: MutableList<Any?> = mutableListOf()

    // threads seem to have no relation to the UI thread's internals
    // so dont implement it

    // start of android.app.Activity.runOnUiThread

    /**
     * Runs the specified action on the UI thread. If the current thread is the UI
     * thread, then the action is executed immediately. If the current thread is
     * not the UI thread, the action is posted to the event queue of the UI thread.
     *
     * @param action the action to run on the UI thread
     */
    fun runOnUiThread(action: Runnable) {
        if (Thread.currentThread() !== mUiThread) {
            mHandler.post(action)
        } else {
            action.run()
        }
    }

    // end of android.app.Activity.runOnUiThread

    fun runOnUiThread(action: () -> Unit) {
        runOnUiThread(Runnable(action))
    }
}