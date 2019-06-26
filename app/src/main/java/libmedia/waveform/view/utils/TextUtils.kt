package libmedia.waveform.view.utils

import android.content.Context
import android.content.res.TypedArray
import android.util.TypedValue

object TextUtils {
    fun getFontSize(ctx: Context, textAppearance: Int): Float {
        val typedValue = TypedValue()
        ctx.theme.resolveAttribute(textAppearance, typedValue, true)
        val textSizeAttr = intArrayOf(android.R.attr.textSize)
        val arr = ctx.obtainStyledAttributes(typedValue.data, textSizeAttr)
        val fontSize = arr.getDimensionPixelSize(0, -1).toFloat()
        arr.recycle()
        return fontSize
    }
}
