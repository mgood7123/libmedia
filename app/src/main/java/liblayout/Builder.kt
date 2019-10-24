package liblayout

import android.app.Activity
import android.content.Context
import android.os.Looper
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.view.ViewTreeObserver
import android.widget.AbsoluteLayout
import android.widget.FrameLayout
import java.lang.IllegalArgumentException

/**
 * constructs a new [View] that fits the specified **activity**
 *
 * a Builder does not contain any [rows][row] when created to avoid confusing between the user adding a [row]
 * thinking the Builder contains no [rows][row], and then ending up with 1 extra [row], and vice versa (in which
 * case [column] will throw [ArrayIndexOutOfBoundsException])
 *
 */
fun Builder(activity: Activity): Builder {
    val root = activity.window.decorView.findViewById<FrameLayout>(android.R.id.content)
    val builder = Builder(null, null).also {
        it.threaded = true
    }
    builder.activity = activity
    builder.activityFrameLayout = root
    return builder
}

/**
 * constructs a new [View] that fits the specified **activity**
 *
 * a Builder does not contain any [rows][row] when created to avoid confusing between the user adding a [row]
 * thinking the Builder contains no [rows][row], and then ending up with 1 extra [row], and vice versa (in which
 * case [column] will throw [ArrayIndexOutOfBoundsException])
 *
 */
fun Builder(UI: UiThread, activity: Activity): Builder {
    val root = activity.window.decorView.findViewById<FrameLayout>(android.R.id.content)
    var height: Int? = null
    var width: Int? = null
    root.viewTreeObserver.addOnGlobalLayoutListener(object :
        ViewTreeObserver.OnGlobalLayoutListener {
        override fun onGlobalLayout() {
            root.viewTreeObserver.removeOnGlobalLayoutListener(this)
            height = root.height
            width = root.width
        }
    })
    val builder = Builder(height, width).also {
        it.threaded = true
    }
    Thread {
        while (height == null && width == null) {
            Thread.sleep(16)
        }
        builder.maxHeight = height
        builder.maxWidth = width
        builder.activity = activity
        builder.activityFrameLayout = root
        builder.UI = UI
    }.start()
    return builder
}

/**
 * constructs a new [View] that fits the specified **view**
 *
 * a Builder does not contain any [rows][row] when created to avoid confusing between the user adding a [row]
 * thinking the Builder contains no [rows][row], and then ending up with 1 extra [row], and vice versa (in which
 * case [column] will throw [ArrayIndexOutOfBoundsException])
 *
 */
fun Builder(UI: UiThread, view: View): Builder {
    val builder = Builder(null, null).also {
        it.threaded = true
        it.view = view
        it.UI = UI
    }
    return builder
}

/**
 * constructs a new [View] that fits the specified **view**
 *
 * a Builder does not contain any [rows][row] when created to avoid confusing between the user adding a [row]
 * thinking the Builder contains no [rows][row], and then ending up with 1 extra [row], and vice versa (in which
 * case [column] will throw [ArrayIndexOutOfBoundsException])
 *
 */
fun Builder(UI: UiThread, view: View, useParent: Boolean): Builder {
    val builder = Builder(null, null).also {
        it.threaded = true
        it.view = view
        it.UI = UI
        it.useParent = useParent
    }
    return builder
}

/**
 * constructs a new [View] that fits the specified **maxHeight** and **maxWidth**, in pixels
 *
 * a Builder does not contain any [rows][row] when created to avoid confusing between the user adding a [row]
 * thinking the Builder contains no [rows][row], and then ending up with 1 extra [row], and vice versa (in which
 * case [column] will throw [ArrayIndexOutOfBoundsException])
 *
 */
class Builder(var maxHeight: Int?, var maxWidth: Int?) {
    var threaded = false
    var activity: Activity? = null
    var activityFrameLayout: FrameLayout? = null
    var view: View? = null
    var UI: UiThread? = null
    var useParent = true

    private val MutableList<() -> Unit>.executeAll: Unit
        get() {
            while (this.isNotEmpty()) {
                Log.e("NU", "executing from queue")
                this.first()()
                this.removeAt(0)
                Log.e("NU", "executed")
            }
        }
    private val queue: MutableList<() -> Unit> = mutableListOf()

    private var addToQueue = true
    private var invokedFromBuild = false

    private var index = 0

    class Column() {
        var isView = true
        var viewfun: (() -> View)? = null
        var view: View? = null
        var sizeFromTop = 0
        var sizeFromLeft = 0
        var distanceFromLeft = 0
        var distanceFromTop = 0
        var cellId: Int = 0
    }

    class Row() {
        val column = mutableListOf<Column>()
        var customHeight = false
        var percentage = 0.0
    }

    private val row = mutableListOf<Row>()


    var currentRow: Row? = null;
    var currentColumn: Column? = null;

    /**
     * appends a new column to the specified row index
     *
     * this version takes a function instead of a direct [View]
     *
     * this is useful if you intend to use [Builder.currentColumn] or [Builder.currentRow],
     * or the same [View] is intended to be used in multiple [columns][column], allowing you to avoid
     * rebuilding the [View] for each [column] manually, eliminating duplicate code
     *
     * since a [View] cannot have more than one [parent][View.getParent], it needs to be reconstructed for each a.view
     * that it wants to be attached to
     *
     * throws [ArrayIndexOutOfBoundsException] if the row is
     * [empty][MutableList.isEmpty] or [rowIndex] is greater than or equal to
     * [row.size][MutableList.size]
     *
     * @param rowIndex the row number
     * @param view a function that returns a [View]
     */
    @Throws(ArrayIndexOutOfBoundsException::class)
    private fun column_(rowIndex: Int, view: () -> View): Builder {
        if (!threaded || !addToQueue) {
            if (row.isEmpty() && row.size >= rowIndex) throw ArrayIndexOutOfBoundsException()
            else {
                currentColumn = Column()
                currentColumn!!.viewfun = view
                currentColumn!!.isView = false
                val c = currentColumn!!
                c.sizeFromTop = maxHeight!!
                c.sizeFromLeft = maxWidth!!
                c.cellId = index++
                row[rowIndex].column.add(c)
            }
        } else {
            Log.e("NU", "added column to queue")
            queue.add {
                if (row.isEmpty() && row.size >= rowIndex)
                    throw ArrayIndexOutOfBoundsException()
                else {
                    currentColumn = Column()
                    currentColumn!!.viewfun = view
                    currentColumn!!.isView = false
                    val c = currentColumn!!
                    c.sizeFromTop = maxHeight!!
                    c.sizeFromLeft = maxWidth!!
                    c.cellId = index++
                    row[rowIndex].column.add(c)
                }
            }
        }
        return this
    }

    /**
     * appends a new column to the specified row index
     * 
     * [Builder.currentColumn] and [Builder.currentRow] CANNOT be used as parameters to a custom [View]'s function 
     * parameters: 
     * 
     * ```
     * val build = Builder(activity)
     * build.column(
     *     1, 
     *     myCustomView(
     *         // build.currentColumn WILL return null
     *         height = build.currentColumn!!.sizeFromTop,
     *         // build.currentColumn WILL return null
     *         width = build.currentColumn!!.sizeFromLeft
     *     )
     * ).build()
     * ```
     *
     * throws [ArrayIndexOutOfBoundsException] if the row is
     * [empty][MutableList.isEmpty] or [rowIndex] is greater than or equal to
     * [row.size][MutableList.size]
     *
     * @param rowIndex the row number
     * @param view the [a.view][View] to attach to this column
     */
    @Throws(ArrayIndexOutOfBoundsException::class)
    fun column(rowIndex: Int, view: View): Builder {
        if (!threaded || !addToQueue) {
            if (row.isEmpty() && row.size >= rowIndex) throw ArrayIndexOutOfBoundsException()
            else {
                currentColumn = Column()
                val c = currentColumn!!
                c.sizeFromTop = maxHeight!!
                c.sizeFromLeft = maxWidth!!
                currentColumn!!.view = view
                c.cellId = index++
                row[rowIndex].column.add(c)
            }
        } else {
            Log.e("NU", "added column to queue")
            queue.add {
                if (row.isEmpty() && row.size >= rowIndex)
                    throw ArrayIndexOutOfBoundsException()
                else {
                    currentColumn = Column()
                    val c = currentColumn!!
                    c.sizeFromTop = maxHeight!!
                    c.sizeFromLeft = maxWidth!!
                    currentColumn!!.view = view
                    c.cellId = index++
                    row[rowIndex].column.add(c)
                }
            }
        }
        return this
    }

    /**
     * appends a new [column][column_] to the specified row index
     *
     * this version takes a function instead of a direct [View]
     *
     * this is useful if you intend to use [Builder.currentColumn] or [Builder.currentRow],
     * or the same [View] is intended to be used in multiple [columns][column], allowing you to avoid
     * rebuilding the [View] for each [column] manually, eliminating duplicate code
     *
     * since a [View] cannot have more than one [parent][View.getParent], it needs to be reconstructed for each a.view
     * that it wants to be attached to
     *
     * throws [ArrayIndexOutOfBoundsException] if the row is
     * [empty][MutableList.isEmpty] or [rowIndex] is greater than or equal to
     * [row.size][MutableList.size]
     *
     * @param rowIndex the row number
     * @param view a function that returns a [View]
     */
    fun column(rowIndex: Int, view: () -> View): Builder = column_(rowIndex, view)

    /**
     * appends a new [column] to the last added [row]
     *
     * [Builder.currentColumn] and [Builder.currentRow] CANNOT be used as parameters to a custom [View]'s function
     * parameters:
     *
     * ```
     * val build = Builder(activity)
     * build.column(
     *     1,
     *     myCustomView(
     *         // build.currentColumn WILL return null
     *         height = build.currentColumn!!.sizeFromTop,
     *         // build.currentColumn WILL return null
     *         width = build.currentColumn!!.sizeFromLeft
     *     )
     * ).build()
     * ```
     *
     * throws [ArrayIndexOutOfBoundsException] if the row is [empty][MutableList.isEmpty]
     *
     * @param view the [a.view][View] to attach to this column
     */
    @Throws(ArrayIndexOutOfBoundsException::class)
    fun column(view: View) = column(row.lastIndex, view)

    /**
     * appends a new [column][column_] to the specified row index
     *
     * this version takes a function instead of a direct [View]
     *
     * this is useful if you intend to use [Builder.currentColumn] or [Builder.currentRow],
     * or the same [View] is intended to be used in multiple [columns][column], allowing you to avoid
     * rebuilding the [View] for each [column] manually, eliminating duplicate code
     *
     * since a [View] cannot have more than one [parent][View.getParent], it needs to be reconstructed for each a.view
     * that it wants to be attached to
     *
     * throws [ArrayIndexOutOfBoundsException] if the row is [empty][MutableList.isEmpty]
     *
     * @param view a function that returns a [View]
     */
    fun column(view: () -> View) = column_(row.lastIndex, view)

    /**
     * adds a new row to the [Builder]
     *
     */
    fun row(): Builder {
        currentRow = Row()
        row.add(currentRow!!)
        return this
    }

    /**
     * adds a new row to the [Builder]
     *
     * this version takes a function instead of a direct [View]
     *
     * this is useful if you intend to use [Builder.currentColumn] or [Builder.currentRow],
     * or the same [View] is intended to be used in multiple [columns][column], allowing you to avoid
     * rebuilding the [View] for each [column] manually, eliminating duplicate code
     *
     * since a [View] cannot have more than one [parent][View.getParent], it needs to be reconstructed for each a.view
     * that it wants to be attached to
     *
     */
    fun row(columns: Int, view: () -> View): Builder {
        if (!threaded) {
            val i = row.size
            row()
            repeat(columns) {
                column(i, view)
            }
        } else {
            Log.e("NU", "added column to queue")
            queue.add {
                val i = row.size
                row()
                repeat(columns) {
                    if (invokedFromBuild) addToQueue = false
                    column(i, view)
                    if (invokedFromBuild) addToQueue = true
                }
            }
        }
        return this
    }

    fun height(percentage: Int): Builder {
        if (percentage < 0 || percentage > 100) throw IllegalArgumentException(
            "value must be between 0 and 100"
        )
        if (!threaded) {
            row[row.lastIndex].customHeight = true
            row[row.lastIndex].percentage = percentage.toDouble()
        } else queue.add() {
            row[row.lastIndex].customHeight = true
            row[row.lastIndex].percentage = percentage.toDouble()
        }
        return this
    }

    fun height(percentage: Double): Builder {
        if (percentage < 0.0 || percentage > 100.0) throw IllegalArgumentException(
            "value must be between 0 and 100"
        )
        if (!threaded) {
            row[row.lastIndex].customHeight = true
            row[row.lastIndex].percentage = percentage
        } else queue.add() {
            row[row.lastIndex].customHeight = true
            row[row.lastIndex].percentage = percentage
        }
        return this
    }

    private fun resize() {
        when {
            row.any { it.customHeight } -> when {
                row.sumByDouble { it.percentage } > 100.0 -> throw ArithmeticException()
                !row.all { it.customHeight } -> {
                    data class perc(var hp: Boolean, var value: Double)

                    val known = mutableListOf<perc>()
                    val unknown = mutableListOf<perc>()
                    val x2 = mutableListOf<perc>()
                    row.forEach {
                        if (it.customHeight) known.add(perc(it.customHeight, it.percentage))
                        else unknown.add(perc(it.customHeight, it.percentage))
                        x2.add(perc(it.customHeight, it.percentage))
                    }
                    val s = known.sumByDouble { it.value }
                    val r = 100.0 - s
                    val v = r / unknown.size
                    row.forEach {
                        if (!it.customHeight) {
                            it.customHeight = true
                            it.percentage = v
                        }
                    }
                    if (row.sumByDouble { it.percentage } > 100.0) throw ArithmeticException()
                    val s2 = row.sumByDouble { it.percentage }
                    Log.e("TAG", "sum = $s")
                    Log.e("TAG", "100 - sum = $r")
                    Log.e("TAG", "result / number of unknowns = $v")
                    Log.e("TAG", "new sum = $s2")
                }
            }
        }

        val p = row.any { it.customHeight }
        val offset = maxHeight!! / row.size
        Log.e("TAG", "offset = $offset")
        val maxHeight = offset * row.size
        Log.e("TAG", "maxHeight = $maxHeight")
        var nextRowOffset = 0
        row.forEach { ROW ->
            var nextOffset = 0
            if (p) {
                Log.e("TAG", "ROW.percentage = ${ROW.percentage}")
            }
            ROW.column.forEach { COLUMN ->
                COLUMN.sizeFromLeft = COLUMN.sizeFromLeft / ROW.column.size
                COLUMN.distanceFromLeft = nextOffset

                nextOffset += COLUMN.sizeFromLeft

                Log.e("TAG", "prev COLUMN.sizeFromTop = ${COLUMN.sizeFromTop}")
                COLUMN.sizeFromTop = if (p)
                    ((ROW.percentage / 100) * COLUMN.sizeFromTop).toInt()
                else
                    COLUMN.sizeFromTop / row.size
                Log.e("TAG", "next COLUMN.sizeFromTop = ${COLUMN.sizeFromTop}")
                Log.e("TAG", "prev COLUMN.distanceFromTop = ${COLUMN.distanceFromTop}")
                COLUMN.distanceFromTop = nextRowOffset
                Log.e("TAG", "next COLUMN.distanceFromTop = ${COLUMN.distanceFromTop}")
            }
            Log.e("TAG", "prev nextRowOffset = $nextRowOffset")
            nextRowOffset += ROW.column[0].sizeFromTop
            Log.e("TAG", "next nextRowOffset = $nextRowOffset")
        }
    }

    /**
     * builds the [View] that has been constructed by [row] or [column] and attach's it to [view]
     *
     * [View] must be an [AbsoluteLayout]
     */
    @Throws(Exception::class)
    fun build(absoluteLayout: AbsoluteLayout) {
        if (!threaded) {
            resize()
            row.forEach {
                it.column.forEach {
                    absoluteLayout.addView(
                        it.view, AbsoluteLayout.LayoutParams(
                            it.sizeFromLeft,
                            it.sizeFromTop,
                            it.distanceFromLeft,
                            it.distanceFromTop
                        )
                    )
                }
            }
        } else throw Exception("executing this when threading makes no sense")
    }

    fun build() {
        queue.add {
            resize()
            if (activity != null) {
                val absoluteLayout = AbsoluteLayout(activity!!.applicationContext)
                row.forEach {
                    it.column.forEach {
                        absoluteLayout.addView(
                            if (it.isView) it.view else it.viewfun!!(),
                            AbsoluteLayout.LayoutParams(
                                it.sizeFromLeft,
                                it.sizeFromTop,
                                it.distanceFromLeft,
                                it.distanceFromTop
                            )
                        )
                    }
                }
                activity!!.runOnUiThread {
                    activity!!.setContentView(absoluteLayout)
                }
            } else if (view != null) {
                val absoluteLayout = AbsoluteLayout(UI!!.context)
                Log.e("NU", "row.size = ${row.size}")
                row.forEach {
                    Log.e("NU", "it.column.size = ${it.column.size}")
                    it.column.forEach {
                        Log.e("NU", "it.sizeFromLeft = ${it.sizeFromLeft}")
                        Log.e("NU", "it.sizeFromTop = ${it.sizeFromTop}")
                        Log.e("NU", "it.distanceFromLeft = ${it.distanceFromLeft}")
                        Log.e("NU", "it.distanceFromTop = ${it.distanceFromTop}")
                        absoluteLayout.addView(
                            it.view, AbsoluteLayout.LayoutParams(
                                it.sizeFromLeft,
                                it.sizeFromTop,
                                it.distanceFromLeft,
                                it.distanceFromTop
                            )
                        )
                    }
                }
                UI!!.runOnUiThread {
                    Log.e("runOnUiThread", "adding $absoluteLayout to $view")
                    (view!! as ViewGroup).addView(absoluteLayout)
                    Log.e("runOnUiThread", "added $absoluteLayout to $view")
                }
            } else throw NullPointerException()
        }
        Thread {
            Looper.prepare()
            while (activity != null && view != null) {
                Log.w(
                    "AV",
                    "activity or view is null: activity is $activity and view is $view"
                )
                Thread.sleep(16)
            }
            if (view != null) {
                Log.e("P", "view is not null")
                if (useParent) {
                    while (view!!.parent == null) {
                        Log.w("P", "view!!.parent is null")
                        Thread.sleep(16)
                    }
                    Log.e("P", "parent not null")
                    while (
                        (view!!.parent as View).height == 0 && (view!!.parent as View).width == 0
                    ) {
                        Log.w(
                            "P",
                            "(view!!.parent as view).height is 0 and (view!!.parent as " +
                                    "View).width is 0"
                        )
                        Thread.sleep(16)
                    }
                    maxHeight = (view!!.parent as View).height
                    maxWidth = (view!!.parent as View).width
                    Log.e(
                        "M",
                        "(view!!.parent as View).height = ${(view!!.parent as View).height}"
                    )
                    Log.e(
                        "M",
                        "(view!!.parent as View).width = ${(view!!.parent as View).width}"
                    )
                } else {
                    while (view!!.height == 0 && view!!.width == 0) {
                        Log.w(
                            "P",
                            "view!!.height is 0 and view!!.width is 0"
                        )
                        Thread.sleep(16)
                    }
                    maxHeight = view!!.height
                    maxWidth = view!!.width
                    Log.e(
                        "M",
                        "view!!.height = ${view!!.height}"
                    )
                    Log.e(
                        "M",
                        "view!!.width = ${view!!.width}"
                    )
                }
            } else if (activity != null) {
                Log.e("P", "activity is not null")
                while (
                    activityFrameLayout!!.height == 0 && activityFrameLayout!!.width == 0
                ) {
                    Log.w(
                        "P",
                        "activityFrameLayout!!.height is 0 and activityFrameLayout!!.width is 0"
                    )
                    Thread.sleep(16)
                }
                maxHeight = activityFrameLayout!!.height
                maxWidth = activityFrameLayout!!.width
                Log.e(
                    "M",
                    "activityFrameLayout!!.height = ${activityFrameLayout!!.height}"
                )
                Log.e(
                    "M",
                    "activityFrameLayout!!.width = ${activityFrameLayout!!.width}"
                )
            }
            Thread.sleep(16)
            invokedFromBuild = true
            queue.executeAll
        }.start()
    }

    /**
     * builds the [View] that has been constructed by [row] or [column] and returns it
     *
     */
    @Throws(Exception::class)
    fun buildReturn(context: Context): AbsoluteLayout {
        if (!threaded) {
            resize()
            val absoluteLayout = AbsoluteLayout(context)
            row.forEach {
                it.column.forEach {
                    absoluteLayout.addView(
                        it.view, AbsoluteLayout.LayoutParams(
                            it.sizeFromLeft,
                            it.sizeFromTop,
                            it.distanceFromLeft,
                            it.distanceFromTop
                        )
                    )
                }
            }
            return absoluteLayout
        } else throw Exception("executing this when threading makes no sense")
    }

    /**
     * cells to grid converter
     */
    class ConvertCellsToValidGrid() {

        /**
         * the **Rows** class holds this rows current columns and leftover columns
         */
        inner class Rows {
            var columns = 0
            var leftover = 0
        }

        private fun convertCellsToValidGrid(cells: Int, modulator: Int = 2): MutableList<Rows>? {

            if (cells == 0) return null
            else if (cells == 1) return mutableListOf<Rows>().also {
                it.add(Rows().also {
                    it.columns = 1
                })
            }

            // create an array of rows
            val row = mutableListOf<Rows>()
            // add one row
            row.add(Rows())
            // set the current row index to zero (0)
            var currentRow = 0

            val GRID = true
            // TODO: add orientation conversions for landscape grid (1x2) and portrait grid (2x1), default is landscape
            if (GRID) {
                // equivalent to the C for loop: for (int i = 1; i < cells; i++)
                repeat(cells) {

                    val leftoverIsNotInitializedToOne = row[currentRow].leftover != 0

                    val remainderOfLeftoverDividedByModulatorIsEqualToZero =
                        (row[currentRow].leftover % modulator) == 0

                    // if leftover is initialized to 1 it will offset the entire grid by 1 producing incorrect layouts
                    // due to the number of cells being off by 1, make sure this is not true
                    if (leftoverIsNotInitializedToOne) {
                        // if the remainder of dividing the leftover by the modulator is equal to zero (0)
                        if (remainderOfLeftoverDividedByModulatorIsEqualToZero) {
                            // wrap leftover into next row
                            row[currentRow].columns = row[currentRow].leftover
                            row[currentRow].leftover = 0
                            row.add(Rows())
                            currentRow++
                            row[currentRow].leftover = 1
                        }
                    }
                    // if neither of the above conditions are true
                    if (!leftoverIsNotInitializedToOne || !remainderOfLeftoverDividedByModulatorIsEqualToZero) {
                        row[currentRow].leftover++
                    }
                }

                // after the counting we can still end up with leftovers, turn these into columns
                row[currentRow].columns = row[currentRow].leftover
                row[currentRow].leftover = 0

                Log.e("convertCellsToValidGrid", "requested: $cells")
                // check if the grid can be compacted, for example: 5x2 to 3x3
                val sqrt = if (row[0].columns == 0) Math.sqrt(cells.toDouble())
                else Math.sqrt(((currentRow + 1) * row[0].columns).toDouble())
                val rounded = Math.floor(sqrt).toInt()
                Log.e("convertCellsToValidGrid", "requested: $cells : sqrt             :  $sqrt")
                Log.e("convertCellsToValidGrid", "requested: $cells : rounded          :  $rounded")
                Log.e("convertCellsToValidGrid", "requested: $cells : total rows       :  ${currentRow + 1}")
                // if rounded is not equal to modulator and rounded is not equal to 1
                if (rounded != modulator && rounded != 1) {
                    // the grid can be compacted
                    Log.e("convertCellsToValidGrid", "requested: $cells : converting to ${rounded}x$rounded")
                    return convertCellsToValidGrid(cells, rounded)
                } else {
                    // the grid cannot be compacted
                    // print the grid's contents in case anything happens to be incorrect
                    for ((index, it) in row.withIndex()) {
                        Log.e("convertCellsToValidGrid", "requested: $cells : row $index : columns  :  ${it.columns}")
                    }
                    return row
                }
            }
            return null
        }

        /**
         * converts the specified number of **cells** into a grid layout template
         */
        fun convertCellsToValidGrid(cells: Int) = convertCellsToValidGrid(cells, 2)
    }
}