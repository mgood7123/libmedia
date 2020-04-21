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
            .row().height(70)
            .column {
                media!!.WaveformView(
                    context = this,
                    height = build.currentColumn!!.sizeFromTop,
                    width = build.currentColumn!!.sizeFromLeft,
                    media = media!!
                )
            }
            .row().height(10).column {
                UpdatingTextView(this).also {
                    it.addOnFirstDrawAction {
                        it.text = "Time:                      0\n" +
                                "current frame :                0\n" +
                                "Audio Timing NANO :            0\n" +
                                "Audio Timing MICRO:            0\n" +
                                "Audio Timing MILLI:            0\n" +
                                "Audio Timing Format NANO :     0\n" +
                                "Audio Timing Format MICRO:     0\n" +
                                "Audio Timing Format MILLI:     0\n" +
                                "Audio Timing Chrono NANO :     0\n" +
                                "Audio Timing Chrono MICRO:     0\n" +
                                "Audio Timing Chrono MILLI:     0\n" +
                                "underruns:                     0\n" +
                                "buffer size:                   0\n" +
                                "buffer capacity:               0\n" +
                                "frame bursts in buffer:        0\n" +
                                "frames per burst:              0"
                    }
                    it.addOnDrawAction {
                        it.text = "Time:                      ${media!!.Oboe_getCurrentTime()}\n" +
                                "current frame :                ${media!!.Oboe_getCurrentFrame()}\n" +
                                "Audio Timing NANO :            ${media!!.Oboe_getAudioTimingNANO()}\n" +
                                "Audio Timing MICRO:            ${media!!.Oboe_getAudioTimingMICRO()}\n" +
                                "Audio Timing MILLI:            ${media!!.Oboe_getAudioTimingMILLI()}\n" +
                                "Audio Timing Format NANO :     ${media!!.Oboe_getAudioTimingFormatNANO()}\n" +
                                "Audio Timing Format MICRO:     ${media!!.Oboe_getAudioTimingFormatMICRO()}\n" +
                                "Audio Timing Format MILLI:     ${media!!.Oboe_getAudioTimingFormatMILLI()}\n" +
                                "Audio Timing Chrono NANO :     ${media!!.Oboe_getAudioTimingChronoNANO()}\n" +
                                "Audio Timing Chrono MICRO:     ${media!!.Oboe_getAudioTimingChronoMICRO()}\n" +
                                "Audio Timing Chrono MILLI:     ${media!!.Oboe_getAudioTimingChronoMILLI()}\n" +
                                "underruns:                     ${media!!.Oboe_underrunCount()}\n" +
                                "buffer size:                   ${media!!.Oboe_bufferSize()}\n" +
                                "buffer capacity:               ${media!!.Oboe_bufferCapacity()}\n" +
                                "frame bursts in buffer:        ${
                                media!!.Oboe_bufferCapacity() / media!!.Oboe_framesPerBurst()
                                }\n" +
                                "frames per burst:              ${media!!.Oboe_framesPerBurst()}"
                    }
                }
            }
            .row().height(10)
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