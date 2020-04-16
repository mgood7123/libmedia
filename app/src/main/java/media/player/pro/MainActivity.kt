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
        System.loadLibrary("AudioEngine")
        println("android.os.Environment.getExternalStorageDirectory().getPath() = ${android.os.Environment.getExternalStorageDirectory().getPath()}")
        println("getFilesDir().getPath() = ${getFilesDir().getPath()}")
        media = Media(this)
        media!!.init()
            .loadMediaAssetAsFile("00001313_48000.raw")
            .loop(true)
        val build = Builder(this)
        build
            .row().height(5)
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
            .row().height(5)
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
            .row().height(60)
            .column {
                media!!.WaveformView(
                    context = this,
                    height = build.currentColumn!!.sizeFromTop,
                    width = build.currentColumn!!.sizeFromLeft,
                    media = media!!
                )
            }
            .row().height(1).column {
                UpdatingTextView(this).also {
                    it.addOnFirstDrawAction {
                        it.text = "underruns: 0"
                    }
                    it.addOnDrawAction {
                        it.text = "underruns: ${media!!.Oboe_underrunCount()}"
                    }
                }
            }
            .row().height(1).column {
                UpdatingTextView(this).also {
                    it.addOnFirstDrawAction {
                        it.text = "frames per burst: 0"
                    }
                    it.addOnDrawAction {
                        it.text = "frames per burst: ${media!!.Oboe_framesPerBurst()}"
                    }
                }
            }
            .row().height(1).column {
                UpdatingTextView(this).also {
                    it.addOnFirstDrawAction {
                        it.text = "buffer size: 0"
                    }
                    it.addOnDrawAction {
                        it.text = "buffer size: ${media!!.Oboe_bufferSize()}"
                    }
                }
            }
            .row().height(1).column {
                UpdatingTextView(this).also {
                    it.addOnFirstDrawAction {
                        it.text = "buffer capacity: 0"
                    }
                    it.addOnDrawAction {
                        it.text = "buffer capacity: ${media!!.Oboe_bufferCapacity()}"
                    }
                }
            }
            .row().height(1).column {
                UpdatingTextView(this).also {
                    it.addOnFirstDrawAction {
                        it.text = "frame bursts in buffer: 0"
                    }
                    it.addOnDrawAction {
                        it.text = "frame bursts in buffer: ${
                        media!!.Oboe_bufferCapacity() / media!!.Oboe_framesPerBurst()
                        }"
                    }
                }
            }
            .row().height(5)
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
                    when {
                        media!!.isPlaying -> it.text = "playing"
                        media!!.isPaused -> it.text = "paused"
                        media!!.isStopped -> it.text = "stopped"
                    }
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
        media!!.destroy()
        super.onDestroy()
    }
}