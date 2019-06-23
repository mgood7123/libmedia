package media.player.pro

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.os.Bundle
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.ProgressBar
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import com.jraska.console.Console
import liblayout.Builder
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
        media = Media(this)
        val build = Builder(this)
        build
            .row(1) {
                WaveformView(
                    context = this,
                    width = build.currentColumn!!.sizeFromLeft,
                    height = build.currentColumn!!.sizeFromTop
                )
            }
            .row(1) {
                ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal).also {
                    it.max = 1440
                    it.min = 0
                    Thread() {
                        var previousFrame = 0
                        var currentFrame = 0
                        while(true) {
                            previousFrame = currentFrame
                            currentFrame = media!!.currentFrame()
                            if (currentFrame != previousFrame) {
                                it.progress = currentFrame
                                Log.i("PLAYHEAD", "currentFrame is $currentFrame, previousFrame is $previousFrame")
                            }
                        }
                    }.start()
                }
            }
            .build()
            media!!.init()
//            .loadMediaPath("/sdcard/RAW_FILE/mp3/00001240.mp3")
//            .loadMediaPath("/sdcard/Music/SuperpoweredPlayer Demo.mp3")
//            .loadMediaPath("/sdcard/Download/11ms.wav")
            .loadMediaAsset("FUNKY_HOUSE.raw")
//            .loadMediaAsset("CLAP.raw")
            .loop(true)
            // test loopers
//            .addLooper("1/4", 0, 250, Media.LooperTiming().milliseconds)
//            .addLooper("1/2", 60, 300, Media.LooperTiming().milliseconds)
//            .addLooper("1/1", 0, 1, Media.LooperTiming().seconds)
//            .setLooper("1/2")
            .play()

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

// Custom view for rendering waveform.
//
// Note: suppressing lint warning for ViewConstructor since it is
//       manually set from the activity and not used in any layout.
@SuppressLint("ViewConstructor")
internal class WaveformView(context: Context, width: Int, height: Int) : View(context) {

    private val mBitmap: Bitmap
    private val mStartTime: Long

    // implementend by libwaveform.so
    private external fun renderWaveform(bitmap: Bitmap, time_ms: Long)

    init {
        mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565)
        mStartTime = System.currentTimeMillis()
    }

    override fun onDraw(canvas: Canvas) {
        renderWaveform(mBitmap, System.currentTimeMillis() - mStartTime)
        canvas.drawBitmap(mBitmap, 0f, 0f, null)
        // force a redraw, with a different time-based pattern.
        invalidate()
    }
}
