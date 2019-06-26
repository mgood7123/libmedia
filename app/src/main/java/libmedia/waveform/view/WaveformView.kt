package libmedia.waveform.view

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.Path
import android.graphics.Picture
import android.graphics.Rect
import android.os.Build
import android.text.TextPaint
import android.util.AttributeSet
import android.view.View
import libmedia.waveform.view.utils.AudioUtils
import libmedia.waveform.view.utils.SamplingUtils
import libmedia.waveform.view.utils.TextUtils

import java.lang.Short

import java.util.LinkedList

/**
 * TODO: document your custom view class.
 */
class WaveformView : View {

    private var mTextPaint: TextPaint? = null
    private var mStrokePaint: Paint? = null
    private var mFillPaint: Paint? = null
    private var mMarkerPaint: Paint? = null
    var mMode: Int = MODE_PLAYBACK
    val strokeThickness = 1f
    val mStrokeColor = -500010
    val mFillColor = -500009
    val mMarkerColor = -500014
    val mTextColor = -500071

    // Used in draw
    private var brightness: Int = 0
    private var drawRect: Rect? = null

    private var width1: Int = 0
    private var height1: Int = 0
    private var overrideWH = false
    private var xStep: Float = 0.toFloat()
    private var centerY: Float = 0.toFloat()
    var audioLength: Int = 0
        private set
    private var mMarkerPosition: Int = 0
    var sampleRate: Int = 0
        set(sampleRate) {
            field = sampleRate
            calculateAudioLength()
        }
    var channels: Int = 0
        set(channels) {
            field = channels
            calculateAudioLength()
        }
    var samples: ShortArray? = null
        set(samples) {
            field = samples
            calculateAudioLength()
            onSamplesChanged()
        }
    private var mHistoricalData: LinkedList<FloatArray>? = null
    private var mCachedWaveform: Picture? = null
    private var mCachedWaveformBitmap: Bitmap? = null
    private val colorDelta = 255 / (HISTORY_SIZE + 1)
    private var showTextAxis = true

    var mode: Int
        get() = mMode
        set(mMode) {
            var mMode = mMode
            mMode = mMode
        }

    var markerPosition: Int
        get() = mMarkerPosition
        set(markerPosition) {
            mMarkerPosition = markerPosition
            postInvalidate()
        }

    constructor(context: Context, height: Int, width: Int) :
            super(context) {
        height1 = height
        width1 = width
        overrideWH = true
        init(context, null, 0)
    }

    constructor(context: Context, height: Int, width: Int, attrs: AttributeSet) :
            super(context, attrs) {
        height1 = height
        width1 = width
        overrideWH = true
        init(context, attrs, 0)
    }

    constructor(context: Context, height: Int, width: Int, attrs: AttributeSet, defStyle: Int) :
            super(context, attrs, defStyle) {
        height1 = height
        width1 = width
        overrideWH = true
        init(context, attrs, defStyle)
    }

    constructor(context: Context) : super(context) {
        init(context, null, 0)
    }

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {
        init(context, attrs, 0)
    }

    constructor(context: Context, attrs: AttributeSet, defStyle: Int) : super(context, attrs, defStyle) {
        init(context, attrs, defStyle)
    }

    private fun init(context: Context, attrs: AttributeSet?, defStyle: Int) {
        // Load attributes

        mTextPaint = TextPaint()
        mTextPaint!!.flags = Paint.ANTI_ALIAS_FLAG
        mTextPaint!!.textAlign = Paint.Align.CENTER
        mTextPaint!!.color = mTextColor
        mTextPaint!!.textSize = TextUtils.getFontSize(getContext(), android.R.attr.textAppearanceSmall)

        mStrokePaint = Paint()
        mStrokePaint!!.color = mStrokeColor
        mStrokePaint!!.style = Paint.Style.STROKE
        mStrokePaint!!.strokeWidth = strokeThickness
        mStrokePaint!!.isAntiAlias = true

        mFillPaint = Paint()
        mFillPaint!!.style = Paint.Style.FILL
        mFillPaint!!.isAntiAlias = true
        mFillPaint!!.color = mFillColor

        mMarkerPaint = Paint()
        mMarkerPaint!!.style = Paint.Style.STROKE
        mMarkerPaint!!.strokeWidth = 0f
        mMarkerPaint!!.isAntiAlias = true
        mMarkerPaint!!.color = mMarkerColor
    }

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        super.onSizeChanged(w, h, oldw, oldh)

        if (!overrideWH) {
            width1 = measuredWidth
            height1 = measuredHeight
        }
        xStep = width1 / (audioLength * 1.0f)
        centerY = height1 / 2f
        drawRect = Rect(0, 0, width1, height1)

        if (mHistoricalData != null) {
            mHistoricalData!!.clear()
        }
        if (mMode == MODE_PLAYBACK) {
            createPlaybackWaveform()
        }
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        val temp = mHistoricalData
        if (mMode == MODE_RECORDING && temp != null) {
            brightness = colorDelta
            for (p in temp) {
                mStrokePaint!!.alpha = brightness
                canvas.drawLines(p, mStrokePaint!!)
                brightness += colorDelta
            }
        } else if (mMode == MODE_PLAYBACK) {
            if (mCachedWaveform != null) {
                canvas.drawPicture(mCachedWaveform!!)
            } else if (mCachedWaveformBitmap != null) {
                canvas.drawBitmap(mCachedWaveformBitmap!!, null, drawRect!!, null)
            }
            if (mMarkerPosition > -1 && mMarkerPosition < audioLength)
                canvas.drawLine(xStep * mMarkerPosition, 0f, xStep * mMarkerPosition, height1.toFloat(), mMarkerPaint!!)
        }
    }

    fun showTextAxis(): Boolean {
        return showTextAxis
    }

    fun setShowTextAxis(showTextAxis: Boolean) {
        this.showTextAxis = showTextAxis
    }

    private fun calculateAudioLength() {
        if (samples == null || sampleRate == 0 || channels == 0)
            return

        audioLength = AudioUtils.calculateAudioLength(samples!!.size, sampleRate, channels)
    }

    private fun onSamplesChanged() {
        if (mMode == MODE_RECORDING) {
            if (mHistoricalData == null)
                mHistoricalData = LinkedList()
            val temp = LinkedList(mHistoricalData!!)

            // For efficiency, we are reusing the array of points.
            val waveformPoints: FloatArray
            if (temp.size == HISTORY_SIZE) {
                waveformPoints = temp.removeFirst()
            } else {
                waveformPoints = FloatArray(width1 * 4)
            }

            drawRecordingWaveform(samples, waveformPoints)
            temp.addLast(waveformPoints)
            mHistoricalData = temp
            postInvalidate()
        } else if (mMode == MODE_PLAYBACK) {
            mMarkerPosition = -1
            xStep = width1 / (audioLength * 1.0f)
            createPlaybackWaveform()
        }
    }

    internal fun drawRecordingWaveform(buffer: ShortArray?, waveformPoints: FloatArray) {
        var lastX = -1f
        var lastY = -1f
        var pointIndex = 0
        val max = Short.MAX_VALUE.toFloat()

        // For efficiency, we don't draw all of the samples in the buffer, but only the ones
        // that align with pixel boundaries.
        for (x in 0 until width1) {
            val index = (x * 1.0f / width1 * buffer!!.size).toInt()
            val sample = buffer[index]
            val y = centerY - sample / max * centerY

            if (lastX != -1f) {
                waveformPoints[pointIndex++] = lastX
                waveformPoints[pointIndex++] = lastY
                waveformPoints[pointIndex++] = x.toFloat()
                waveformPoints[pointIndex++] = y
            }

            lastX = x.toFloat()
            lastY = y
        }
    }

    internal fun drawPlaybackWaveform(width: Int, height: Int, buffer: ShortArray): Path {
        val waveformPath = Path()
        val centerY = height / 2f
        val max = Short.MAX_VALUE.toFloat()

        val extremes = SamplingUtils.getExtremes(buffer, width)


        waveformPath.moveTo(0f, centerY)

        // draw maximums
        for (x in 0 until width) {
            val sample = extremes[x][0]
            val y = centerY - sample / max * centerY
            waveformPath.lineTo(x.toFloat(), y)
        }

        // draw minimums
        for (x in width - 1 downTo 0) {
            val sample = extremes[x][1]
            val y = centerY - sample / max * centerY
            waveformPath.lineTo(x.toFloat(), y)
        }

        waveformPath.close()

        return waveformPath
    }

    private fun createPlaybackWaveform() {
        if (width1 <= 0 || height1 <= 0 || samples == null)
            return

        val cacheCanvas: Canvas
        if (Build.VERSION.SDK_INT >= 23 && isHardwareAccelerated) {
            mCachedWaveform = Picture()
            cacheCanvas = mCachedWaveform!!.beginRecording(width1, height1)
        } else {
            mCachedWaveformBitmap = Bitmap.createBitmap(width1, height1, Bitmap.Config.ARGB_8888)
            cacheCanvas = Canvas(mCachedWaveformBitmap!!)
        }

        val mWaveform = drawPlaybackWaveform(width1, height1, samples!!)
        cacheCanvas.drawPath(mWaveform, mFillPaint!!)
        cacheCanvas.drawPath(mWaveform, mStrokePaint!!)
        drawAxis(cacheCanvas, width1)

        if (mCachedWaveform != null)
            mCachedWaveform!!.endRecording()
    }

    private fun drawAxis(canvas: Canvas, width: Int) {
        if (!showTextAxis) return
        val seconds = audioLength / 1000
        val xStep = width / (audioLength / 1000f)
        val textHeight = mTextPaint!!.textSize
        val textWidth = mTextPaint!!.measureText("10.00")
        var secondStep = (textWidth * seconds.toFloat() * 2f).toInt() / width
        secondStep = Math.max(secondStep, 1)
        var i = 0f
        while (i <= seconds) {
            canvas.drawText(String.format("%.2f", i), i * xStep, textHeight, mTextPaint!!)
            i += secondStep.toFloat()
        }
    }

    companion object {
        val MODE_RECORDING = 1
        val MODE_PLAYBACK = 2

        private val HISTORY_SIZE = 6
    }
}
