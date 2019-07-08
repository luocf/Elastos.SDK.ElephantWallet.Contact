package org.elastos.tools.crosspl.processor

import javax.annotation.processing.AbstractProcessor
import javax.annotation.processing.ProcessingEnvironment
import javax.annotation.processing.RoundEnvironment
import javax.lang.model.element.TypeElement
import javax.tools.Diagnostic
import javax.tools.Diagnostic.Kind.*

object Log {
    fun v(msg: String) {
        println(OTHER, msg, null)
    }

    fun d(msg: String) {
        println(NOTE, msg, null)
    }

    fun i(msg: String) {
        println(NOTE, msg, null)
    }

    fun w(msg: String) {
        println(WARNING, msg, null)
    }

    fun w(msg: String, tr: Throwable) {
        println(WARNING, msg, tr)
    }

    fun w(tr: Throwable) {
        println(WARNING, "", tr)
    }

    fun e(msg: String) {
        println(ERROR, msg, null)
    }

    fun e(msg: String, tr: Throwable) {
        println(ERROR, msg, tr)
    }

    fun setEnv(env: ProcessingEnvironment) {
        procEnv = env
    }

    private fun println(priority: Diagnostic.Kind, msg: String, tr: Throwable?) {
        var trmsg = ""
        if(tr != null) {
            trmsg = tr.toString()
        }
        procEnv.messager.printMessage(priority, "${msg} ${trmsg}")
    }

    private lateinit var procEnv: ProcessingEnvironment
}
