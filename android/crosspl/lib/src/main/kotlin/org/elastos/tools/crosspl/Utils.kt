package org.elastos.tools.crosspl

import java.lang.Exception

class Utils private constructor() {
    companion object {
        const val TAG : String = "crosspl"
    }

    open class CrossPLException(message: String?) : Exception(message)
}
