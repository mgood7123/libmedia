package media.player.pro

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import liblayout.Builder
import libmedia.Media
import java.io.IOException
import java.nio.ByteBuffer
import java.nio.ByteOrder
import org.apache.commons.io.IOUtils

class MainActivity : AppCompatActivity() {
    private val audioSample: ShortArray
        @Throws(IOException::class)
        get() {
            val `is` = resources.openRawResource(R.raw.fh)
            val data: ByteArray
            try {
                data = IOUtils.toByteArray(`is`)
            } finally {
                `is`?.close()
            }

            val sb = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer()
            val samples = ShortArray(sb.limit())
            sb.get(samples)
            return samples
        }
    var media: Media? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        System.loadLibrary("PlayerOboe")
        media = Media(this)
        media!!.init()
//            .loadMediaPath("/sdcard/RAW_FILE/mp3/00001240.mp3")
//            .loadMediaPath("/sdcard/Music/SuperpoweredPlayer Demo.mp3")
//            .loadMediaPath("/sdcard/Download/11ms.wav")
            .loadMediaAsset("FUNKY_HOUSE.raw")
//            .loadMediaAsset("CLAP.raw")
            .loop(true)
            // test loopers
//            .addLooper("1/4", 0, 250, media!!.LooperTiming().milliseconds)
//            .addLooper("1/2", 500, 1000, media!!.LooperTiming().milliseconds)
//            .addLooper("1/1", 0, 1, media!!.LooperTiming().seconds)
//            .setLooper("1/2")
            .play()
        val samples1 = audioSample
        val samples2 = media!!.samples
        val build = Builder(this)
        build.row(1) {
                libmedia.waveform.view.WaveformView(this).also {
                    it.channels = 2
                    it.sampleRate = 48000
                    it.samples = samples1
                    Thread {
                        var currentFrame = 0
                        while (true) {
                            val previousFrame = currentFrame
                            currentFrame = media!!.currentFrame()
                            if (currentFrame != previousFrame) {
                                runOnUiThread {
                                    it.markerPosition = currentFrame
                                }
                            }
                        }
                    }.start()
                }
            }
            .row(1) {
                libmedia.waveform.view.WaveformView(this).also {
                    it.channels = 2
                    it.sampleRate = 48000
                    it.samples = samples2
                    Thread {
                        var currentFrame = 0
                        while (true) {
                            val previousFrame = currentFrame
                            currentFrame = media!!.currentFrame()
                            if (currentFrame != previousFrame) {
                                runOnUiThread {
                                    it.markerPosition = currentFrame
                                }
                            }
                        }
                    }.start()
                }
            }
            .build()
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
