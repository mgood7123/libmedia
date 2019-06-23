package liblayout.view

import android.content.Context
import android.graphics.Typeface
import android.util.AttributeSet
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.LinearLayout
import android.widget.TextView

/**
 * a Button with a custom view, this defaults to [TextView]
 */
@Suppress("unused")
class Button : LinearLayout {

    init {
        isClickable = true
        isFocusable = true
    }

    constructor(context: Context) : super(context) {
        currentView = TextView(context)
        this.background = android.widget.Button(context).background
    }

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {
        currentView = TextView(context)
        this.background = android.widget.Button(context).background
    }

    constructor(context: Context, view: View) : super(context) {
        currentView = view
        this.background = android.widget.Button(context).background
    }

    constructor(context: Context, view: View, attrs: AttributeSet) : super(context, attrs) {
        currentView = view
        this.background = android.widget.Button(context).background
    }

    constructor(
        context: Context,
        view: Any,
        ViewRetrievalMethod: (view: Any) -> View,
        TextViewRetrievalMethod: (view: Any) -> TextView
    ) : super(context) {
        viewRetrievalMethod = ViewRetrievalMethod
        textViewRetrievalMethod = TextViewRetrievalMethod
        currentView = view
        this.background = android.widget.Button(context).background
    }

    constructor(
        context: Context,
        view: Any,
        ViewRetrievalMethod: (view: Any) -> View,
        TextViewRetrievalMethod: (view: Any) -> TextView,
        attrs: AttributeSet
    ) : super(context, attrs) {
        viewRetrievalMethod = ViewRetrievalMethod
        textViewRetrievalMethod = TextViewRetrievalMethod
        currentView = view
        this.background = android.widget.Button(context).background
    }

    /**
     * the method used to retrieve a desired [view][View] if the [TextView] is nested, possibly allows dynamic [Views]
     */
    var viewRetrievalMethod: (view: Any) -> View = { view ->
        view as View
    }

    /**
     * the method used to retrieve a desired [TextView], possibly allows dynamic [TextViews][TextView]
     */
    var textViewRetrievalMethod: (view: Any) -> TextView = { view ->
        view as TextView
    }

    /**
     * the method used to [assign text][TextView.setText] to the [text view][TextView]
     */
    var textAssignmentMethod: (text: CharSequence) -> Unit = { text ->
        textViewRetrievalMethod(currentView).text = text
    }

    /**
     * the method used to [retrieve text][TextView.getText] to the [text view][TextView]
     */
    var textRetrievalMethod: () -> CharSequence = {
        textViewRetrievalMethod(currentView).text
    }

    /**
     * this method is called when the [currentView] is changed
     */
    var onViewChangeMethod: () -> Unit = {
        var l = textViewRetrievalMethod(currentView).layoutParams as ViewGroup.LayoutParams
        l = LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT)
        textViewRetrievalMethod(currentView).gravity = Gravity.CENTER
        textViewRetrievalMethod(currentView).setTypeface(Typeface.MONOSPACE, Typeface.BOLD)
    }

    /**
     *
     */
    var currentView: Any
        set(view: Any) {
            field = view
            if (this.childCount == 1) this.removeViewAt(0)
            this.addView(viewRetrievalMethod(field))
            onViewChangeMethod()
        }

    /**
     *
     */
    var text: CharSequence
        set(text: CharSequence) = textAssignmentMethod(text)
        get() = textRetrievalMethod()
}