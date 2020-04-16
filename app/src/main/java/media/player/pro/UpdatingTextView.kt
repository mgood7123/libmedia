package media.player.pro

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet
import android.util.Log
import androidx.appcompat.widget.AppCompatTextView

class UpdatingTextView: AppCompatTextView {
    constructor(context: Context?) : super(context)
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(
        context,
        attrs,
        defStyleAttr
    )

    private val initialDrawQueue: MutableList<(view: UpdatingTextView) -> Unit> = mutableListOf()
    private var hasInitialDrawQueue = false
    private var initialDrawQueueHasBeenExecuted = false
    private val drawQueue: MutableList<(view: UpdatingTextView) -> Unit> = mutableListOf()

    fun addOnFirstDrawAction(action: (view: UpdatingTextView) -> Unit) {
        initialDrawQueue.add(action)
        hasInitialDrawQueue = true
    }

    fun addOnDrawAction(action: (view: UpdatingTextView) -> Unit) {
        drawQueue.add(action)
    }

    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas)
        if (hasInitialDrawQueue && !initialDrawQueueHasBeenExecuted) {
            initialDrawQueue.forEach { it(this) }
            initialDrawQueueHasBeenExecuted = true
        }
        else drawQueue.forEach { it(this) }
    }

}