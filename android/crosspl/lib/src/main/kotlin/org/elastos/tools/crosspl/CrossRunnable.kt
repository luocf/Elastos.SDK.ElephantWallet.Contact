package org.elastos.tools.crosspl

import org.elastos.tools.crosspl.annotation.CrossClass
import org.elastos.tools.crosspl.annotation.CrossInterface

@CrossClass
private class CrossRunnable(nativeHandle: Long) : CrossBase(nativeHandle), Runnable {

    @CrossInterface
    external override fun run()
}
