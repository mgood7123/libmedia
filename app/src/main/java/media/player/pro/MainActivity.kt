package media.player.pro

import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import liblayout.Builder
import libmedia.Media
import java.io.File

class MainActivity : AppCompatActivity() {
    var media: Media? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        System.loadLibrary("PlayerOboe")
        media = Media(this)
        media!!.init()
//            .loadMediaAsset("00001313Upsampled.raw")
            .loadMediaPath("/sdcard/ReSampler/00001313.wav")
            .loop(true)
            .play()
        val build = Builder(this)
        build
            .row().height(20)
            .column {
                Button(this).also {
                    it.text = "waveform draw Lines ${media!!.WaveformViewOptions.drawLines}"
                    it.setOnClickListener { _ ->
                        media!!.WaveformViewOptions.drawLines = !media!!.WaveformViewOptions.drawLines
                        it.text = "waveform draw Lines ${media!!.WaveformViewOptions.drawLines}"
                    }
                }
            }
            .column {
                Button(this).also {
                    it.text = "waveform highlight Silence ${media!!.WaveformViewOptions.highlightSilence}"
                    it.setOnClickListener { _ ->
                        media!!.WaveformViewOptions.highlightSilence = !media!!.WaveformViewOptions.highlightSilence
                        it.text = "waveform highlight Silence ${media!!.WaveformViewOptions.highlightSilence}"
                    }
                }
            }
            .row().height(20)
            .column {
                Button(this).also {
                    it.text = "waveform stretch to screen height ${media!!.WaveformViewOptions.stretchToScreenHeight}"
                    it.setOnClickListener { _ ->
                        media!!.WaveformViewOptions.stretchToScreenHeight = !media!!.WaveformViewOptions.stretchToScreenHeight
                        it.text = "waveform stretch to screen height ${media!!.WaveformViewOptions.stretchToScreenHeight}"
                    }
                }
            }
            .column {
                Button(this).also {
                    it.text = "waveform stretch to screen width ${media!!.WaveformViewOptions.stretchToScreenWidth}"
                    it.setOnClickListener { _ ->
                        media!!.WaveformViewOptions.stretchToScreenWidth = !media!!.WaveformViewOptions.stretchToScreenWidth
                        it.text = "waveform stretch to screen width ${media!!.WaveformViewOptions.stretchToScreenWidth}"
                    }
                }
            }
            .row().height(40)
            .column {
                media!!.WaveformView(
                    context = this,
                    height = build.currentColumn!!.sizeFromTop,
                    width = build.currentColumn!!.sizeFromLeft,
                    media = media!!
                )
            }
            .row().height(20)
            .column {
                Button(this).also {
                    media!!.Listner.play = {
                        it.text = "playing"
                    }
                    media!!.Listner.pause = {
                        it.text = "paused"
                    }
                    media!!.Listner.stop = {
                        it.text = "stopped"
                    }
                    it.setOnClickListener {
                        if (media!!.isPlaying) media!!.pause()
                        else media!!.play()
                    }
                    Thread {
                        while (true) {
                            when {
                                media!!.isPlaying -> it.text = "playing"
                                media!!.isPaused -> it.text = "paused"
                                media!!.isStopped -> it.text = "stopped"
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
