package libmedia

import android.app.Activity
import android.content.Context
import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.media.AudioManager
import android.util.AttributeSet
import android.util.Log
import android.view.View
import androidx.constraintlayout.widget.ConstraintLayout
import com.example.libperm.PermissionManager
import java.io.IOException


@Suppress("unused")
class Media(private val activity: Activity) {

    fun `init`(): Media {
        val myAudioMgr = activity.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        val sampleRateStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)
        val defaultSampleRate = Integer.parseInt(sampleRateStr)
        val framesPerBurstStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)
        val defaultFramesPerBurst = Integer.parseInt(framesPerBurstStr)
        // library is loaded at application startup
        Oboe_Init(defaultSampleRate, defaultFramesPerBurst)
        return this
    }

    var MEDIAbackground: Boolean = false

    fun foreground(): Media
    {
        MEDIAbackground = false
        return this
    }

    fun background(): Media {
        MEDIAbackground = true
        return this
    }

    fun destroy(): Media {
        Oboe_Cleanup()
        return this
    }

    fun loadMediaRes(data: Int): Media {
        // Files under res/raw are not zipped, just copied into the APK.
        // Get the offset and length to know where our file is located.
        val fd = activity.resources.openRawResourceFd(data)
        val fileOffset = fd.startOffset.toInt()
        val fileLength = fd.length.toInt()
        try {
            fd.parcelFileDescriptor.close()
        } catch (e: IOException) {
            Log.e("PlayerExample", "Close error.")
        }
        val path = activity.packageResourcePath         // get path to APK package
//        if (type == Type().Superpowered) Superpowered_OpenFileRes(path, fileOffset, fileLength)         // open audio file from APK
        return this
    }

    fun loadMediaPath(path: String): Media {
        val p = PermissionManager(activity).Permissions("android.permission.READ_EXTERNAL_STORAGE")
        p.requestAllRemaining()
        while (!p.checkAll()) Thread.sleep(1)
//        if (type == Type().Superpowered) Superpowered_OpenFilePath(path)                            // open audio file from storage
//        Oboe_LoadTrackFromAssets(activity.assets)
        return this
    }

    fun loadMediaAsset(asset: String): Media {
        Oboe_LoadTrackFromAssets(asset, activity.assets)
        return this
    }

    fun play(): Media {
//        if (type == Type().Superpowered) {
//            if (!Superpowered_IsPlaying()) Superpowered_Play()
//        }
        Oboe_Play()
        return this
    }

    fun pause(): Media {
//        if (type == Type().Superpowered) {
//            if (Superpowered_IsPlaying()) Superpowered_Pause()
//        }
        Oboe_Pause()
        return this
    }

    fun stop(): Media {
//        if (type == Type().Superpowered) Superpowered_Stop()
        Oboe_Stop()
        return this
    }

    fun loop(value: Boolean): Media {
//        if (type == Type().Superpowered) Superpowered_Loop(value)
        Oboe_Loop(value)
        return this
    }

    private data class loop(var name: String, var start: Double, var end: Double, var timing: Int);
    private val looper: MutableList<loop> = mutableListOf()
    private var currentLooper: loop? = null
    private var stopLooper = true

    class LooperTiming() {
        val nanoseconds = 1
        val microseconds = 2
        val milliseconds = 3
        val seconds = 4
        val minutes = 5
        val hours = 6
    }

    private fun looperStart() {
//        if (type == Type().Superpowered) {
//            Superpowered_HasLooper(true)
//            Superpowered_Looper(currentLooper!!.start, currentLooper!!.end)
//        } {
            Oboe_Looper(currentLooper!!.start, currentLooper!!.end, currentLooper!!.timing)
//        }
    }

    private fun looperStop() {
//        if (type == Type().Superpowered) Superpowered_HasLooper(false)
        currentLooper = null
    }

    fun addLooper(name: String, start: Int, end: Int, timing: Int): Media = addLooper(
        name,
        start.toDouble(),
        end.toDouble(),
        timing
    )

    fun addLooper(name: String, start: Double, end: Double, timing: Int): Media {
        looper.add(loop(name, start, end, timing))
        return this
    }

    fun setLooper(name: String?): Media {
        if (name == null) looperStop()
        else {
            val l = looper.find { it.name == name }
            if (l != null) {
                currentLooper = l
                looperStart()
            }
            else Log.e("Media", "Looper not found: $name")
        }
        return this
    }

    fun currentFrame(width: Int): Int = Oboe_CurrentFrame(width);

    fun WaveformView(context: Context, height: Int, width: Int): ConstraintLayout =
        internal().WaveformView_(context, height, width)

    fun WaveformView(context: Context, height: Int, width: Int, media: Media): ConstraintLayout =
        internal().WaveformView_(context, height, width, media)


    val samples: ShortArray get() {
        val a = mutableListOf<Short>()
        // this WILL read incur a buffer overrun by twice its size
        // | [] [] [] [] | -> | [] [] [] [] | [] [] [] [] |
        // |    DATA     | -> |    DATA     | RANDOM DATA |
        // a.size == Oboe_SampleCount()*2
        for (i in 0 until ((Oboe_SampleCount()*2)-1)) a.add(Oboe_SampleIndex(i))
        return a.toTypedArray().toShortArray()
    }
    val sampleRate get() = Oboe_SampleRate()
    val channelCount get() = Oboe_ChannelCount()

    // Functions implemented in the native library.

    // Oboe
    private external fun Oboe_Init(sampleRate: Int, framesPerBurst: Int)
    private external fun Oboe_LoadTrackFromAssets(asset: String, assetManager: AssetManager)
    private external fun Oboe_Play()
    private external fun Oboe_Pause()
    private external fun Oboe_Stop()
    private external fun Oboe_Loop(value: Boolean)
    private external fun Oboe_Looper(start: Double, end: Double, timing: Int)
    private external fun Oboe_CurrentFrame(width: Int): Int
    private external fun Oboe_Cleanup()
    private external fun Oboe_SampleIndex(index: Long): Short
    private external fun Oboe_SampleCount(): Long
    private external fun Oboe_SampleRate(): Int
    private external fun Oboe_ChannelCount(): Int

    inner class WaveformViewOptions__ {
        var drawLines: Boolean = true
        var highlightSilence: Boolean = false
        var stretchToScreenHeight: Boolean = true;
        var stretchToScreenWidth: Boolean = true;
    }
    val WaveformViewOptions = WaveformViewOptions__()

    private inner class internal {
        internal inner class WaveformView_ : ConstraintLayout {

            private var media: Media? = null
            private var height_ = 0;
            private var width_ = 0;

            constructor(context: Context, height: Int, width: Int) :
                    super(context) {
                height_ = height
                width_ = width
                initView()
            }
            constructor(context: Context, height: Int, width: Int, media: Media) :
                    super(context) {
                height_ = height
                width_ = width
                this.media = media
                initView()
            }



            constructor(context: Context, height: Int, width: Int, attrs: AttributeSet) :
                    super(context, attrs) {
                height_ = height
                width_ = width
                initView()
            }
            constructor(context: Context, height: Int, width: Int, media: Media, attrs: AttributeSet) :
                    super(context, attrs) {
                height_ = height
                width_ = width
                this.media = media
                initView()
            }



            constructor(context: Context, height: Int, width: Int, attrs: AttributeSet, defStyleAttr: Int) :
                    super(context, attrs, defStyleAttr) {
                height_ = height
                width_ = width
                initView()
            }
            constructor(context: Context, height: Int, width: Int, media: Media, attrs: AttributeSet, defStyleAttr: Int) :
                    super(context, attrs, defStyleAttr) {
                height_ = height
                width_ = width
                this.media = media
                initView()
            }

            fun initView() {
                addView(
                    WaveformView__(context, width_, height_)
                )
                if (media != null) addView(
                    MyView__(context, height_).also {
                        Thread {
                            var currentFrame = 0
                            while (true) {
                                val previousFrame = currentFrame
                                currentFrame = currentFrame(width_)
                                if (currentFrame != previousFrame) {
                                    activity.runOnUiThread {
                                        it.left = currentFrame
                                    }
                                }
                            }
                        }.start()
                    }
                )
            }
        }

        private inner class WaveformView__(context: Context, width_: Int, height_: Int) : View(context) {

            private val mBitmap: Bitmap
            private val mStartTime: Long

            private external fun renderWaveform(
                bitmap: Bitmap,
                time_ms: Long,
                waveformViewOptions: WaveformViewOptions__
            )

            init {
                mBitmap = Bitmap.createBitmap(width_, height_, Bitmap.Config.RGB_565)
                mStartTime = System.currentTimeMillis()
            }

            override fun onDraw(canvas: Canvas) {
                renderWaveform(mBitmap, System.currentTimeMillis() - mStartTime, WaveformViewOptions)
                canvas.drawBitmap(mBitmap, 0f, 0f, null)
//                canvas.drawLine()
                // force a redraw, with a different time-based pattern.
                invalidate()
            }
        }

        internal inner class MyView__(context: Context, val height_: Int) : View(context) {

            internal var paint: Paint? = null

            init {
                paint = Paint()
                paint!!.color = Color.GREEN
                paint!!.strokeWidth = 2f
                paint!!.style = Paint.Style.STROKE
            }

            override fun onDraw(canvas: Canvas) {
                // TODO Auto-generated method stub
                super.onDraw(canvas)
                val offset: Float = 0f
                canvas.drawRect(offset, 0f, offset, height_.toFloat(), paint!!)
            }

        }
    }
}