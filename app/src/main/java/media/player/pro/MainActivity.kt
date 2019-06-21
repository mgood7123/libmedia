package media.player.pro

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Point
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import libmedia.Media

class MainActivity : AppCompatActivity() {
    companion object {

        init {
            System.loadLibrary("PlayerOboe")
        }
    }

    var media: Media? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val display = windowManager.defaultDisplay
        val displaySize = Point()
        display.getSize(displaySize)
        setContentView(PlasmaView(this, displaySize.x, displaySize.y/2))
        media = Media(this)
            .init()
//            .loadMediaPath("/sdcard/RAW_FILE/mp3/00001240.mp3")
//            .loadMediaPath("/sdcard/Music/SuperpoweredPlayer Demo.mp3")
            .loadMediaPath("/sdcard/Download/11ms.wav")
            .loop(true)
            // test loopers
            .addLooper("1/4", 0, 250, Media.LooperTiming().milliseconds)
            .addLooper("1/2", 60, 300, Media.LooperTiming().milliseconds)
            .addLooper("1/1", 0, 1, Media.LooperTiming().seconds)
            .setLooper("1/2")
//            .play()

//        Thread {
//            while(true) {
//                Thread.sleep(5000)
//                media!!.stop()
//                media!!.play()
//            }
//        }.start()
    }

    public override fun onPause() {
        super.onPause()
        media!!.background()
    }

    public override fun onResume() {
        super.onResume()
        media!!.foreground()
    }

    override fun onDestroy() {
        super.onDestroy()
        media!!.destroy()
    }

}

// Custom view for rendering plasma.
//
// Note: suppressing lint warning for ViewConstructor since it is
//       manually set from the activity and not used in any layout.
@SuppressLint("ViewConstructor")
internal class PlasmaView(context: Context, width: Int, height: Int) : View(context) {
    private val mBitmap: Bitmap
    private val mStartTime: Long

    // implementend by libplasma.so
    private external fun renderPlasma(bitmap: Bitmap, time_ms: Long)

    init {
        mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565)
        mStartTime = System.currentTimeMillis()
    }

    override fun onDraw(canvas: Canvas) {
        renderPlasma(mBitmap, System.currentTimeMillis() - mStartTime)
        canvas.drawBitmap(mBitmap, 0f, 0f, null)
        // force a redraw, with a different time-based pattern.
        invalidate()
    }
}
