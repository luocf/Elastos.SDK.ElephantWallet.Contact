package org.elastos.tools.crosspl.processor.generator

import org.elastos.tools.crosspl.processor.CrossTmplUtils
import org.elastos.tools.crosspl.processor.Log
import java.io.File

class CrossPLUtilsGenerator {
    companion object {
        fun Generate(crossplDir: File): Boolean {
            val headerFile = GetHeaderFile(crossplDir)
            val sourceFile = GetSourceFile(crossplDir)

            var ret = GenerateFile(CrossPLUtilsHeaderTmpl, headerFile)
            if(! ret) {
                return ret
            }

            ret = GenerateFile(CrossPLUtilsSourceTmpl, sourceFile)
            if(! ret) {
                return ret
            }

            var spanFile = GetFile(crossplDir, ExperimentalSpanTmpl)
            ret = GenerateFile(ExperimentalSpanTmpl, spanFile)
            if(! ret) {
                return ret
            }

            return true
        }

        fun GetSourceFile(crossplDir: File): File {
            return GetFile(crossplDir, CrossPLUtilsSourceTmpl)
        }

        fun GetHeaderFile(crossplDir: File): File {
            return GetFile(crossplDir, CrossPLUtilsHeaderTmpl)
        }

        private fun GenerateFile(from: String, to: File): Boolean {
            Log.w("Generate: ${to.absolutePath}")
            val content = CrossTmplUtils.ReadTmplContent(from)

            CrossTmplUtils.WriteContent(to, content)
            return true
        }

        fun GetFile(crossplDir: File, fileName: String): File {
            return File(crossplDir, fileName)
        }

        private const val CrossPLUtilsHeaderTmpl = "/CrossPLUtils.hpp"
        private const val CrossPLUtilsSourceTmpl = "/CrossPLUtils.cpp"
        private const val ExperimentalSpanTmpl = "/experimental-span.hpp"
    }
}
