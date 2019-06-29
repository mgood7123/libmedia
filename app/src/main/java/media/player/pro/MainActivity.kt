package media.player.pro

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import liblayout.Builder
import libmedia.Media

class MainActivity : AppCompatActivity() {
    var media: Media? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        System.loadLibrary("PlayerOboe")
        media = Media(this)
        media!!.init()
            .loadMediaAsset("FUNKY_HOUSE.raw")
            .loop(true)
//            .play()
        val build = Builder(this)
        build.row(1) {
//            libmedia.waveform.view.WaveformView(this).also {
//                it.channels = 2
//                it.sampleRate = 48000
//                it.samples = media!!.samples
//                Thread {
//                    var currentFrame = 0
//                    while (true) {
//                        val previousFrame = currentFrame
//                        currentFrame = media!!.currentFrame(build.currentColumn!!.sizeFromLeft)
//                        if (currentFrame != previousFrame) {
//                            runOnUiThread {
//                                it.markerPosition = currentFrame
//                            }
//                        }
//                    }
//                }.start()
//            }
//        }.row(1) {
            media!!.WaveformView(
                context = this,
                height = build.currentColumn!!.sizeFromTop,
                width = build.currentColumn!!.sizeFromLeft
//                media = media!!
            )
        }.build()
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
