package libmedia.waveform.view.utils

import java.util.Arrays

object SamplingUtils {
    fun getExtremes(data: ShortArray, sampleSize: Int): Array<ShortArray> {
        val newData = mutableListOf<ShortArray>()
        val groupSize = data.size / sampleSize

        for (i in 0 until sampleSize) {
            val group = Arrays.copyOfRange(data, i * groupSize,
                    Math.min((i + 1) * groupSize, data.size))

            // Fin min & max values
            var min = java.lang.Short.MAX_VALUE
            var max = java.lang.Short.MIN_VALUE
            for (a in group) {
                min = Math.min(min.toInt(), a.toInt()).toShort()
                max = Math.max(max.toInt(), a.toInt()).toShort()
            }
            newData.add(shortArrayOf(max, min))
        }

        return newData.toTypedArray()
    }
}
