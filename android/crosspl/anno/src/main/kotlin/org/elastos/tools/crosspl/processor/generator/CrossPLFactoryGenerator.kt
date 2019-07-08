package org.elastos.tools.crosspl.processor.generator

import org.elastos.tools.crosspl.processor.CrossClassInfo
import org.elastos.tools.crosspl.processor.CrossTmplUtils
import org.elastos.tools.crosspl.processor.Log
import java.io.File

class CrossPLFactoryGenerator {
    companion object {
        fun Generate(crossplDir: File, classInfoList: List<CrossClassInfo>, headerFileList: List<File>): Boolean {
            val headerFile = GetHeaderFile(crossplDir)
            val sourceFile = GetSourceFile(crossplDir)

            var ret = GenerateHeader(headerFile)
            if(! ret) {
                return ret
            }

            ret = GenerateSource(sourceFile, classInfoList, headerFileList)
            if(! ret) {
                return ret
            }

            return true
        }

        fun GetSourceFile(crossplDir: File): File {
            return File(crossplDir, "CrossPLFactory.cpp")
        }

        fun GetHeaderFile(crossplDir: File): File {
            return File(crossplDir, "CrossPLFactory.hpp")
        }

        private fun GenerateHeader(headerFile: File): Boolean {
            Log.w("Generate: ${headerFile.absolutePath}")
            val content = CrossTmplUtils.ReadTmplContent(CrossPLHeaderTmpl)

            CrossTmplUtils.WriteContent(headerFile, content)
            return true
        }

        private fun GenerateSource(sourceFile: File, classInfoList: List<CrossClassInfo>, headerFileList: List<File>): Boolean {
            Log.w("Generate: ${sourceFile.absolutePath}")
            var content = CrossTmplUtils.ReadTmplContent(CrossPLSourceTmpl)

            var includeProxyList = ""
            headerFileList.forEach {
                includeProxyList += "#include <${it.name}>\n"
            }
            classInfoList.forEach {
                includeProxyList += "#include <${it.cppInfo.className}.hpp>\n"
            }
            content = content.replace(TmplKeyIncludeProxyHpp, includeProxyList)

            var regNativeMethodsList = ""
            classInfoList.forEach {
                regNativeMethodsList += "${CrossTmplUtils.TabSpace}ret |= proxy::${it.cppInfo.className}::RegisterNativeMethods(jenv.get());\n"
            }
            content = content
                .replace(TmplKeyRegisterNativeMethods, regNativeMethodsList)

            var createCppObjectList = ""
            var destroyCppObjectList = ""
            var getJavaClassPathList = ""
            classInfoList.forEach {
                createCppObjectList += "${CrossTmplUtils.TabSpace}if(std::strcmp(javaClassName, \"${it.javaInfo.className}\") == 0) {\n"
                createCppObjectList += "${CrossTmplUtils.TabSpace}${CrossTmplUtils.TabSpace}ptr = new ${it.cppInfo.className}();\n"
                createCppObjectList += "${CrossTmplUtils.TabSpace}}\n"

                destroyCppObjectList += "${CrossTmplUtils.TabSpace}if(std::strcmp(javaClassName, \"${it.javaInfo.className}\") == 0) {\n"
                destroyCppObjectList += "${CrossTmplUtils.TabSpace}${CrossTmplUtils.TabSpace}delete reinterpret_cast<${it.cppInfo.className}*>(cppHandle);\n"
                destroyCppObjectList += "${CrossTmplUtils.TabSpace}${CrossTmplUtils.TabSpace}return 0;\n"
                destroyCppObjectList += "${CrossTmplUtils.TabSpace}}\n"

                getJavaClassPathList += "${CrossTmplUtils.TabSpace}if(EndsWith(cppClassName, \"${it.cppInfo.className}\") == 0) {\n"
                getJavaClassPathList += "${CrossTmplUtils.TabSpace}${CrossTmplUtils.TabSpace}return \"${it.javaInfo.classPath}\";\n"
                getJavaClassPathList += "${CrossTmplUtils.TabSpace}}\n"
            }
            content = content
                .replace(TmplKeyCreateCppObject, createCppObjectList)
                .replace(TmplKeyDestroyCppObject, destroyCppObjectList)
                .replace(TmplKeyGetJavaClassPath, getJavaClassPathList)

            content = content
                .replace(TmplKeyJniOnLoad, "JNI_OnLoad")

            CrossTmplUtils.WriteContent(sourceFile, content)
            return true
        }

        private const val CrossPLHeaderTmpl = "/CrossPLFactory.hpp.tmpl"
        private const val CrossPLSourceTmpl = "/CrossPLFactory.cpp.tmpl"

        private const val TmplKeyIncludeProxyHpp = "%IncludeProxyHpp%"
        private const val TmplKeyRegisterNativeMethods = "%RegisterNativeMethods%"
        private const val TmplKeyCreateCppObject = "%CreateCppObject%"
        private const val TmplKeyDestroyCppObject = "%DestroyCppObject%"
        private const val TmplKeyGetJavaClassPath = "%GetJavaClassPath%"
        private const val TmplKeyJniOnLoad = "%JniOnLoad%"
    }
}
