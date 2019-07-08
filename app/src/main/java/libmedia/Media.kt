package libmedia

import android.app.Activity
import android.content.Context
import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.media.AudioAttributes
import android.media.AudioFocusRequest
import android.media.AudioManager
import android.util.AttributeSet
import android.util.Log
import android.view.View
import android.widget.Toast
import androidx.constraintlayout.widget.ConstraintLayout
import com.example.libperm.PermissionManager
import java.io.IOException


@Suppress("unused")
class Media(private val activity: Activity) {
    lateinit var currentTemporyMediaFilesDirectory: String
    lateinit var audioManager: AudioManager
    
    private inner class FocusManager {
        private var mAudioFocusChangeListener: AudioFocusChangeListenerImpl? = null
        var hasFocus: Boolean = false
        private var focusChanged: Boolean = false
        private val TAG = "FocusManager"

        fun request(): Boolean {
            if (hasFocus) return true
            mAudioFocusChangeListener = AudioFocusChangeListenerImpl()
            val result = audioManager.requestAudioFocus(
                mAudioFocusChangeListener,
                AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN
            )

            when (result) {
                AudioManager.AUDIOFOCUS_REQUEST_GRANTED -> hasFocus = true
                AudioManager.AUDIOFOCUS_REQUEST_FAILED -> hasFocus = false
            }

            val message = "Focus request " + if (hasFocus) "granted" else "failed"
            Toast.makeText(activity, message, Toast.LENGTH_LONG).show()
            Log.i(TAG, message)
            return hasFocus
        }

        fun release(): Boolean {
            if (!hasFocus) return true
            val result = audioManager.abandonAudioFocus(mAudioFocusChangeListener)
            when (result) {
                AudioManager.AUDIOFOCUS_REQUEST_GRANTED -> hasFocus = false
                AudioManager.AUDIOFOCUS_REQUEST_FAILED -> hasFocus = true
            }
            val message = "Abandon focus request " + if (!hasFocus) "granted" else "failed"
            Toast.makeText(activity, message, Toast.LENGTH_LONG).show()
            Log.i(TAG, message)
            return !hasFocus
        }

        private inner class AudioFocusChangeListenerImpl : AudioManager.OnAudioFocusChangeListener {

            override fun onAudioFocusChange(focusChange: Int) {
                focusChanged = true
                Log.i(TAG, "Focus changed")

                when (focusChange) {
                    AudioManager.AUDIOFOCUS_GAIN -> {
                        Log.i(TAG, "AUDIOFOCUS_GAIN")
                        Toast.makeText(activity, "Focus GAINED", Toast.LENGTH_LONG).show()
                        play()
                    }
                    AudioManager.AUDIOFOCUS_LOSS -> {
                        Log.i(TAG, "AUDIOFOCUS_LOSS")
                        Toast.makeText(activity, "Focus LOST", Toast.LENGTH_LONG).show()
                        pause()
                    }
                    AudioManager.AUDIOFOCUS_LOSS_TRANSIENT -> {
                        Log.i(TAG, "AUDIOFOCUS_LOSS_TRANSIENT")
                        Toast.makeText(activity, "Focus LOST TRANSIENT", Toast.LENGTH_LONG).show()
                    }
                    AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK -> {
                        Log.i(TAG, "AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK")
                        Toast.makeText(activity, "Focus LOST TRANSIENT CAN DUCK", Toast.LENGTH_LONG).show()
                    }
                }
            }
        }
    }
    
    private val focusManager = FocusManager()

    fun `init`(): Media {
        setTemporyMediaFilesDirectory(activity.filesDir.absolutePath)
        audioManager = activity.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        val sampleRateStr = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)
        val defaultSampleRate = Integer.parseInt(sampleRateStr)
        val framesPerBurstStr = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)
        val defaultFramesPerBurst = Integer.parseInt(framesPerBurstStr)
        // library is loaded at application startup
        Oboe_Init(defaultSampleRate, defaultFramesPerBurst)
        return this
    }

    fun requestAudioFocus(): Media {
        focusManager.request()
        return this
    }

    fun releaseAudioFocus(): Media {
        focusManager.release()
        return this
    }

    fun setTemporyMediaFilesDirectory(dir: String): Media {
        currentTemporyMediaFilesDirectory = dir
        Log.e("Media", "setting Temporary Files Directory to $dir")
        Oboe_SetTempDir(currentTemporyMediaFilesDirectory)
        return this
    }

    var isInBackground: Boolean = false

    fun foreground(): Media
    {
        isInBackground = false
        return this
    }

    fun background(): Media {
        isInBackground = true
        return this
    }

    fun destroy(): Media {
        focusManager.release()
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
        return this
    }

    fun loadMediaPath(path: String): Media {
//        val p = PermissionManager(activity).Permissions("android.permission.READ_EXTERNAL_STORAGE")
//        p.requestAllRemaining()
//        while (!p.checkAll()) Thread.sleep(1)
        Oboe_LoadTrackFromPath(path)
        return this
    }

    fun loadMediaAsset(asset: String): Media {
        Oboe_LoadTrackFromAssets(asset, activity.assets)
        return this
    }
    
    class ListnerCallback {
        var play: () -> Unit = {}
        var pause: () -> Unit = {}
        var stop: () -> Unit = {}
    }
    
    val Listner = ListnerCallback()

    var isPlaying = false
    var isPaused = false
    var isStopped = false
    var isLooping = false

    fun togglePlayback(): Media {
        when {
            isStopped || isPaused -> play()
            isPlaying -> play()
            else -> Log.e("LibMedia", "Unknown Playback State")
        }
        return this
    }

    fun play(): Media {
        focusManager.request()
        Oboe_Play()
        isPlaying = true
        isPaused = false
        isStopped = false
        Listner.play()
        return this
    }

    fun pause(): Media {
        focusManager.release()
        Oboe_Pause()
        isPlaying = false
        isPaused = true
        isStopped = false
        Listner.pause()
        return this
    }

    fun stop(): Media {
        focusManager.release()
        Oboe_Stop()
        isPlaying = false
        isPaused = false
        isStopped = true
        Listner.stop()
        return this
    }

    fun loop(value: Boolean): Media {
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
        Oboe_Looper(currentLooper!!.start, currentLooper!!.end, currentLooper!!.timing)
        isLooping = true
    }

    private fun looperStop() {
        currentLooper = null
        isLooping = false
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

    private external fun Oboe_Init(sampleRate: Int, framesPerBurst: Int)
    private external fun Oboe_SetTempDir(dir: String)
    private external fun Oboe_LoadTrackFromAssets(asset: String, assetManager: AssetManager)
    private external fun Oboe_LoadTrackFromPath(path: String)
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
                super.onDraw(canvas)
                val offset: Float = 0f
                canvas.drawRect(offset, 0f, offset, height_.toFloat(), paint!!)
            }

        }
    }
}