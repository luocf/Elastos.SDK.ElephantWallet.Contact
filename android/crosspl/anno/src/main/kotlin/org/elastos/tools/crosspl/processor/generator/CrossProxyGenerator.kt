package org.elastos.tools.crosspl.processor.generator

import org.elastos.tools.crosspl.processor.*
import java.io.File

class CrossProxyGenerator {
    companion object {
        fun Generate(crossProxyDir: File, classInfo: CrossClassInfo): Boolean {
            val proxyHeaderFile = GetHeaderFile(crossProxyDir, classInfo)
            val proxySourceFile = GetSourceFile(crossProxyDir, classInfo)

            var ret = GenerateHeader(proxyHeaderFile, classInfo)
            if(! ret) {
                return ret
            }

            ret = GenerateSource(proxySourceFile, classInfo)
            if(! ret) {
                return ret
            }

            return true
        }

        fun GetSourceFile(crossProxyDir: File, classInfo: CrossClassInfo): File {
            return File(crossProxyDir, classInfo.cppInfo.className + ".proxy.cpp")
        }

        fun GetHeaderFile(crossProxyDir: File, classInfo: CrossClassInfo): File {
            return File(crossProxyDir, classInfo.cppInfo.className + ".proxy.hpp")
        }

        private fun GenerateHeader(proxyFile: File, classInfo: CrossClassInfo): Boolean {
            Log.w("Generate: ${proxyFile.absolutePath}")

            val tmpl = CrossTmplUtils.ReadTmplContent(CrossClassProxyHeaderTmpl)
            val content = classInfo.makeProxyDeclare(tmpl)
            CrossTmplUtils.WriteContent(proxyFile, content)

            return true
        }

        private fun GenerateSource(proxyFile: File, classInfo: CrossClassInfo): Boolean {
            Log.w("Generate: ${proxyFile.absolutePath}")

            val tmpl = CrossTmplUtils.ReadTmplContent(CrossClassProxySourceTmpl)
            val content = classInfo.makeProxySource(tmpl)
            CrossTmplUtils.WriteContent(proxyFile, content)

            return true
        }


        private const val CrossClassProxyHeaderTmpl = "/CrossPLClass.proxy.hpp.tmpl"
        private const val CrossClassProxySourceTmpl = "/CrossPLClass.proxy.cpp.tmpl"
    }
}
