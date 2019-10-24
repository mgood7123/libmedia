package media.player.pro

import android.content.Context
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class NativeView(val context: Context) {
    val TAG = "EglSample"
    external fun nativeOnStart()
    external fun nativeOnResume()
    external fun nativeOnPause()
    external fun nativeOnStop()
    external fun nativeSetSurface(surface: Surface?)
    val surfaceView: View?
    val surfaceHolderCallback: SurfaceHolderCallback?
    init {
        System.loadLibrary("nativeegl")
        surfaceHolderCallback = SurfaceHolderCallback()
        surfaceView = View(surfaceHolderCallback, context)
    }

    inner class View: SurfaceView {
        constructor(callback: SurfaceHolder.Callback, context: Context) : super(context) {
            holder.addCallback(callback)
        }

        constructor(callback: SurfaceHolder.Callback, context: Context, attrs: AttributeSet) : super(context, attrs) {
            holder.addCallback(callback)
        }

        constructor(callback: SurfaceHolder.Callback, context: Context, attrs: AttributeSet, defStyle: Int) : super(context, attrs, defStyle) {
            holder.addCallback(callback)
        }

        constructor(callback: SurfaceHolder.Callback, context: Context, attrs: AttributeSet, defStyle: Int, defStyleRes: Int) : super(context, attrs, defStyle, defStyleRes) {
            holder.addCallback(callback)
        }
    }

    inner class SurfaceHolderCallback: SurfaceHolder.Callback {
        override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {
            nativeSetSurface(holder.surface)
        }

        override fun surfaceCreated(holder: SurfaceHolder) {}

        override fun surfaceDestroyed(holder: SurfaceHolder) {
            nativeSetSurface(null)
        }
    }
}
