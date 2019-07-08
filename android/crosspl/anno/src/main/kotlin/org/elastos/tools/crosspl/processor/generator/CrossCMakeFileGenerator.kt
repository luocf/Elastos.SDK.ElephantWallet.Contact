package org.elastos.tools.crosspl.processor.generator

import org.elastos.tools.crosspl.processor.CrossTmplUtils
import org.elastos.tools.crosspl.processor.Log
import java.io.File

class CrossCMakeFileGenerator {
    companion object {
        fun Generate(crossplDir: File, headerFileList: List<File>, sourceFileList: List<File>): Boolean {
            val cmakefileFile = File(crossplDir, "crosspl.cmake")

            Log.w("Generate: ${cmakefileFile.absolutePath}")
            var content =
                CrossTmplUtils.ReadTmplContent(CrossCMakeListsTmpl)

            var headerDirList = mutableListOf<String>()
            var headerContent = ""
            headerFileList.forEach {
                headerContent += "${CrossTmplUtils.TabSpace}\"${it.absolutePath}\"\n"

                if(! headerDirList.contains(it.parent)) {
                    headerDirList.add(it.parent)
                }
            }
            var headerDirContent = ""
            headerDirList.forEach {
                headerDirContent += "${CrossTmplUtils.TabSpace}\"${it}\"\n"
            }
            var sourceContent = ""
            sourceFileList.forEach {
                sourceContent += "${CrossTmplUtils.TabSpace}\"${it.absolutePath}\"\n"
            }

            content = content
                .replace(TmplKeyProxyHeaders, headerContent)
                .replace(TmplKeyProxySources, sourceContent)
                .replace(TmplKeyProxyHeaderDir, headerDirContent)

            CrossTmplUtils.WriteContent(cmakefileFile, content)
            return true
        }

        private const val CrossCMakeListsTmpl = "/crosspl.cmake.tmpl"

        private const val TmplKeyProxyHeaders = "%CrossProxyHeaders%"
        private const val TmplKeyProxySources = "%CrossProxySources%"
        private const val TmplKeyProxyHeaderDir = "%CrossProxyHeaderDir%"
    }
}
