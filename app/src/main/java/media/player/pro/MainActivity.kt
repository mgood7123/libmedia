package media.player.pro

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
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
                media!!.WaveformView(
                    this,
                    build.currentColumn!!.sizeFromTop,
                    build.currentColumn!!.sizeFromLeft,
                    media!!
                )
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
//            .addLooper("1/2", 500, 1000, Media.LooperTiming().milliseconds)
//            .addLooper("1/1", 0, 1, Media.LooperTiming().seconds)
//            .setLooper("1/2")
            .play()

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
