package media.player.pro

import alpine.term.TerminalController
import android.os.Bundle
import android.view.ContextMenu
import android.view.ContextMenu.ContextMenuInfo
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.slide_out_terminal.*
import liblayout.Builder
import liblayout.UiThread
import libmedia.Media

class MainActivity : AppCompatActivity() {

    var media: Media? = null
    var terminalController: TerminalController? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // If this method is called more than once with the same library name
        // the second and subsequent calls are ignored.
        // set log view

        // set log view
        setContentView(R.layout.slide_out_terminal)

        // obtain log view instance

        // obtain log view instance
        terminalController = TerminalController()
        terminalController!!.onCreate(this, terminal_view)

        when(terminalController!!.terminalContainer.visibility) {
            View.INVISIBLE -> toggle_terminal.setText(R.string.Show_LogTerminal)
            else -> toggle_terminal.setText(R.string.Hide_LogTerminal)
        }

        toggle_terminal.setOnClickListener {
            terminalController!!.onClick(toggle_terminal)
        }

        println("android.os.Environment.getExternalStorageDirectory().getPath() = ${android.os.Environment.getExternalStorageDirectory().getPath()}")
        println("getFilesDir().getPath() = ${getFilesDir().getPath()}")
        media = Media(this)
        media!!.init()
            .loadMediaAssetAsFile("00001313_44100.raw")
            .loop(true)

        Builder(this, mainView)
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
                    height = it.sizeFromTop,
                    width = it.sizeFromLeft,
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

    override fun onStart() {
        super.onStart()
        terminalController!!.onStart()
    }

    override fun onResume() {
        super.onResume()
        media!!.foreground()
    }

    override fun onPause() {
        super.onPause()
        media!!.background()
    }

    override fun onStop() {
        super.onStop()
        terminalController!!.onStop()
    }

    override fun onDestroy() {
        media!!.destroy()
        terminalController!!.onDestroy()
        super.onDestroy()
    }

    override fun onCreateContextMenu(
        menu: ContextMenu?,
        v: View?,
        menuInfo: ContextMenuInfo?
    ) {
        terminalController!!.onCreateContextMenu(menu, v, menuInfo)
    }

    override fun onContextItemSelected(item: MenuItem?): Boolean {
        return terminalController!!.onContextItemSelected(item) || super.onContextItemSelected(item)
    }

    override fun onBackPressed() {
        if (!terminalController!!.onBackPressed()) super.onBackPressed()
    }
}