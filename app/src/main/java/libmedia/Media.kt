package libmedia

import android.app.Activity
import android.content.Context
import android.content.res.AssetManager
import android.media.AudioManager
import android.util.Log
import com.example.libperm.PermissionManager
import java.io.IOException


@Suppress("unused")
class Media(private val activity: Activity) {

    var buffersize = 0
    var samplerate = 0
    var playing = false

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

    fun foreground(): Media {
//        if (type == Type().Superpowered) Superpowered_onForeground()
        return this
    }

    fun background(): Media {
//        if (type == Type().Superpowered) Superpowered_onBackground()
        return this
    }

    fun destroy(): Media {
//        if (type == Type().Superpowered) Superpowered_Cleanup()
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

    fun currentFrame(): Int = Oboe_CurrentFrame();

    // Functions implemented in the native library.

    // Oboe
    private external fun Oboe_Init(sampleRate: Int, framesPerBurst: Int)
    private external fun Oboe_LoadTrackFromAssets(asset: String, assetManager: AssetManager)
    private external fun Oboe_Play()
    private external fun Oboe_Pause()
    private external fun Oboe_Stop()
    private external fun Oboe_Loop(value: Boolean)
    private external fun Oboe_Looper(start: Double, end: Double, timing: Int)
    private external fun Oboe_CurrentFrame(): Int
    private external fun Oboe_Cleanup()
}