package org.elastos.tools.crosspl.processor

import java.io.File
import java.util.Scanner

class CrossTmplUtils {
    companion object {
        fun ReadTmplContent(tmplName: String): String {
            val istream = CrossClassAnnoProcessor::class.java.getResourceAsStream(tmplName)
            val scanner = Scanner(istream)
            val template = scanner.useDelimiter("\\A").next()

            return template
        }

        fun WriteContent(file: File, content: String) {
            file.apply {
                this.parentFile.mkdirs()
                writeText(content)
            }
        }

        const val TabSpace = "  "
    }
}
