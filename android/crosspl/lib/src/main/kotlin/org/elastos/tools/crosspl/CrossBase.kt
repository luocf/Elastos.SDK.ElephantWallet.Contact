package org.elastos.tools.crosspl

import android.util.Log
import org.elastos.tools.crosspl.annotation.CrossClass
import org.elastos.tools.crosspl.annotation.CrossInterface
import kotlin.reflect.KClass
import kotlin.reflect.full.primaryConstructor

@CrossClass
open class CrossBase
    protected constructor(private var nativeHandle: Long = 0) {

    init {
        if(nativeHandle == 0L) {
            nativeHandle = CreateNativeObject(this.javaClass.name)
        }
        Log.i(Utils.TAG, "construct " + toString())
        if(nativeHandle == 0L) {
            throw Utils.CrossPLException("Failed to create native object.")
        }
    }

    protected fun finalize() {
        DestroyNativeObject(this.javaClass.name, nativeHandle)
        Log.i(Utils.TAG, "deconstruct " + toString())
    }

    @CrossInterface
    override fun toString(): String {
        return "${this.javaClass.name}{nativeHandle=${nativeHandle}}"
    }

//    private var nativeHandle: Long = 0
    private companion object {
        init {
            System.loadLibrary("crosspl")
        }

        @CrossInterface
        private fun CreatePlatformObject(className: String, nativeHandle: Long): CrossBase {
            val clazz = Class.forName(className).kotlin as KClass<out CrossBase>
            val platformObject = clazz.primaryConstructor!!.call(nativeHandle)
            return platformObject
        }

        @CrossInterface
        private fun DestroyPlatformObject(className: String, platformObject: CrossBase) {
            platformObject.finalize()
        }

        @CrossInterface
        private external fun CreateNativeObject(className: String): Long

        @CrossInterface
        private external fun DestroyNativeObject(className: String, nativeHandle: Long)
    }


}
