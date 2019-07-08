package org.elastos.tools.crosspl.processor

import javax.lang.model.element.ExecutableElement
import javax.lang.model.element.Modifier
import javax.lang.model.type.ExecutableType

class CrossMethodInfo {
    companion object {
        fun Parse(methodElement: ExecutableElement, forceStatic: Boolean = false): CrossMethodInfo {
            val methodInfo = CrossMethodInfo()
            methodInfo.methodName = methodElement.simpleName.toString()

            val methodType = methodElement.asType() as ExecutableType;
            methodType.parameterTypes.forEach {
                val type = CrossVariableType.Parse(it)
                if(type == null) {
                    val msg = "Unsupport param type `${it}` for method ${methodElement}"
                    Log.e(msg)
                    throw CrossPLException(msg)
                }

                methodInfo.paramsType.add(type)
            }

            val type = CrossVariableType.Parse(methodElement.returnType)
            if(type == null) {
                val msg = "Unsupport return type `${methodElement.returnType}` for method ${methodElement}"
                Log.e(msg)
                throw CrossPLException(msg)
            }
            methodInfo.returnType = type

            methodInfo.isStatic = forceStatic
            if(methodElement.modifiers.contains(Modifier.STATIC)) {
                methodInfo.isStatic = true
            }
            if(methodElement.modifiers.contains(Modifier.NATIVE)) {
                methodInfo.isNative = true
            }

            return methodInfo
        }

        private const val TmplKeyArguments = "%Arguments%"
        private const val TmplKeyFuncBody = "%FunctionBody%"
    }

    override fun toString(): String {
        return  "MethodInfo{methodName=${methodName}," +
                " params=${paramsType}, returnType=${returnType}," +
                " static=${isStatic}, native=${isNative}}"
    }

    fun makeProxyDeclare(): String {
        return when {
            isNative -> makeNativeFunctionDeclare(null)
            else     -> makePlatformFunctionDeclare(null)
        }
    }

    fun makeProxySource(cppClassName: String, javaClassPath: String): String {
        val funcDeclare = when {
            isNative -> makeNativeFunctionDeclare(cppClassName)
            else     -> makePlatformFunctionDeclare(cppClassName)
        }
        val emptyFunc = "$funcDeclare\n{\n$TmplKeyFuncBody\n}\n"

        val funcBody = when {
            isNative -> makeNativeFunctionBody(cppClassName)
            else     -> makePlatformFunctionBody(cppClassName, javaClassPath)
        }

        val content = emptyFunc.replace(TmplKeyFuncBody, funcBody)

        return content
    }

    lateinit var methodName: String
    var paramsType = mutableListOf<CrossVariableType>()
    lateinit var returnType: CrossVariableType
    var isStatic = false
    var isNative = false

    private fun makeNativeFunctionDeclare(cppClassName: String?): String {
        var className = (if(cppClassName != null) "${cppClassName}::" else "")
        val returnType = returnType.toJniString(false)
        var content = "$returnType $className$methodName($TmplKeyArguments)"

        var arguments = "JNIEnv* jenv"
        arguments += when {
            isStatic -> ", jclass jtype"
            else     -> ", jobject jobj"
        }
        for(idx in paramsType.indices) {
            val type = paramsType[idx].toJniString()
            arguments += ", $type jvar$idx"
        }
        content = content.replace(TmplKeyArguments, arguments)

        return content
    }

    private fun makePlatformFunctionDeclare(cppClassName: String?): String {
        var className = (if(cppClassName != null) "$cppClassName::" else "")
        var retTypeStr = returnType.toCppString(false).removeSuffix("*")
        retTypeStr =
            when {
                returnType == CrossVariableType.CROSSBASE ->  "$retTypeStr*"
                ! returnType.isPrimitiveType()            ->  "std::shared_ptr<$retTypeStr>"
                else -> retTypeStr
            }
        var content = "$retTypeStr $className$methodName($TmplKeyArguments)"

        var arguments = "int64_t platformHandle"
        for(idx in paramsType.indices) {
            val type = paramsType[idx].toCppString(true)
            arguments += ", $type var$idx"
        }
        content = content.replace(TmplKeyArguments, arguments)

        return content
    }

    private fun makeNativeFunctionBody(cppClassName: String): String {
        var prefixContent = ""
        var suffixContent = ""
        for(idx in paramsType.indices) {
            var type = paramsType[idx]
            val isPrimitiveType = type.isPrimitiveType()
            prefixContent += when {
                isPrimitiveType                     -> "${CrossTmplUtils.TabSpace}${type.toCppString()} var$idx = jvar$idx;\n"
                type == CrossVariableType.CROSSBASE -> "${CrossTmplUtils.TabSpace}auto var$idx = CrossPLUtils::SafeCastCrossObject<$cppClassName>(jenv, jvar$idx);\n"
                else                                -> "${CrossTmplUtils.TabSpace}auto var$idx = CrossPLUtils::SafeCast$type(jenv, jvar$idx);\n"
            }

            if(type == CrossVariableType.STRINGBUFFER
            || type == CrossVariableType.BYTEBUFFER) {
                suffixContent += "${CrossTmplUtils.TabSpace}CrossPLUtils::SafeCopy${type}ToJava(jenv, jvar$idx, var$idx.get());\n"
            }
        }
        prefixContent += "\n"
        suffixContent += "\n"

        var funcContent: String
        if(! isStatic) {
            prefixContent += "${CrossTmplUtils.TabSpace}auto obj = CrossPLUtils::SafeCastCrossObject<::$cppClassName>(jenv, jobj);\n"
            funcContent = "obj->"
        } else {
            funcContent = "::$cppClassName::"
        }

        var argusContent = ""
        for(idx in paramsType.indices) {
            var type = paramsType[idx]
            val isPrimitiveType = type.isPrimitiveType()
            argusContent += when {
                isPrimitiveType                  -> "var$idx, "
                type == CrossVariableType.STRING -> "var$idx.get(), "
                else                             -> "var$idx.get(), "
            }
        }
        argusContent = argusContent.removeSuffix(", ")

        var retContent = ""
        if(returnType != CrossVariableType.VOID) {
            val cppType = returnType.toCppString(false)
            retContent = "$cppType ret = "

            val isPrimitiveType = returnType.isPrimitiveType()
            when {
                isPrimitiveType -> {
                    suffixContent += "${CrossTmplUtils.TabSpace}${returnType.toJniString()} jret = ret;\n"
                    suffixContent += "${CrossTmplUtils.TabSpace}return jret;"
                }
                returnType == CrossVariableType.STRING -> {
                    suffixContent += "${CrossTmplUtils.TabSpace}auto jret = CrossPLUtils::SafeCast$returnType(jenv, ret);\n"
                    suffixContent += "${CrossTmplUtils.TabSpace}return jret.get();"
                }
                returnType == CrossVariableType.CROSSBASE -> {
                    suffixContent += "${CrossTmplUtils.TabSpace}auto jret = CrossPLUtils::SafeCastCrossObject<$cppClassName>(jenv, ret);\n"
                    suffixContent += "${CrossTmplUtils.TabSpace}return jret.get();"
                }
                else -> {
                    suffixContent += "${CrossTmplUtils.TabSpace}auto jret = CrossPLUtils::SafeCast$returnType(jenv, &ret);\n"
                    suffixContent += "${CrossTmplUtils.TabSpace}return jret.get();"
                }
            }
        }
        var content = "$prefixContent"
        content += "${CrossTmplUtils.TabSpace}$retContent$funcContent$methodName($argusContent);\n\n"
        content += "$suffixContent"

        return content
    }

    private fun makePlatformFunctionBody(cppClassName: String, javaClassPath: String): String {
        var prefixContent = ""
        var suffixContent = ""

        prefixContent += "${CrossTmplUtils.TabSpace}auto jenv = CrossPLUtils::SafeGetEnv();\n"
        prefixContent += "${CrossTmplUtils.TabSpace}auto jobj = reinterpret_cast<jobject>(platformHandle);\n"
        for(idx in paramsType.indices) {
            var type = paramsType[idx]
            val isPrimitiveType = type.isPrimitiveType()
            prefixContent += when {
                isPrimitiveType                     -> "${CrossTmplUtils.TabSpace}${type.toJniString()} jvar$idx = var$idx;\n"
                type == CrossVariableType.CROSSBASE -> "${CrossTmplUtils.TabSpace}auto jvar$idx = CrossPLUtils::SafeCastCrossObject<${type.toCppString()}>(jenv.get(), var$idx);\n"
                else                                -> "${CrossTmplUtils.TabSpace}auto jvar$idx = CrossPLUtils::SafeCast$type(jenv.get(), var$idx);\n"
            }

            if(type == CrossVariableType.STRINGBUFFER
            || type == CrossVariableType.BYTEBUFFER) {
                suffixContent += "${CrossTmplUtils.TabSpace}CrossPLUtils::SafeCopy${type}ToCpp(jenv.get(), const_cast<${type.toCppString()}*>(var$idx), jvar$idx.get());\n"
            }
        }
        prefixContent += "\n"
        suffixContent += "\n"

        prefixContent += "${CrossTmplUtils.TabSpace}auto jclazz = CrossPLUtils::FindJavaClass(jenv.get(), \"$javaClassPath\");\n"

        var jniSigContent = "("
        for(idx in paramsType.indices) {
            var type = paramsType[idx]
            jniSigContent += "${type.toJniSigChar()}"
        }
        jniSigContent += ")${returnType.toJniSigChar()}"
        prefixContent += "${CrossTmplUtils.TabSpace}auto jmethod$methodName = jenv->GetMethodID(jclazz, \"$methodName\", \"$jniSigContent\");\n"

        var funcContent = "jenv->Call"
        funcContent +=
            when {
                isStatic -> "Static"
                else     -> ""
            }
        funcContent +=
            when {
                returnType.isPrimitiveType() -> returnType.toString()
                else                         -> "Object"
            }

        val objContent =
            when {
                isStatic -> "jclazz"
                else     -> "jobj"
            }
        funcContent += "Method"

        var argusContent = ""
        for(idx in paramsType.indices) {
            var type = paramsType[idx]
            val isPrimitiveType = type.isPrimitiveType()
            argusContent += when {
                isPrimitiveType                     -> ", jvar$idx"
                type == CrossVariableType.CROSSBASE -> ", jvar$idx"
                else                                -> ", jvar$idx.get()"
            }
        }

        var retContent = ""
        if(returnType != CrossVariableType.VOID) {
            val jniType = returnType.toJniString(false)
            retContent = "$jniType jret = ($jniType)"

            val isPrimitiveType = returnType.isPrimitiveType()
            when {
                isPrimitiveType -> {
                    suffixContent += "${CrossTmplUtils.TabSpace}${returnType.toJniString()} ret = jret;\n"
                }
                returnType == CrossVariableType.CROSSBASE -> {
                    suffixContent += "${CrossTmplUtils.TabSpace}auto ret = CrossPLUtils::SafeCastCrossObject<${returnType.toCppString()}>(jenv.get(), jret);\n"
                }
                else -> {
                    suffixContent += "${CrossTmplUtils.TabSpace}auto ret = CrossPLUtils::SafeCast$returnType(jenv.get(), jret);\n"
                }
            }
            suffixContent += "${CrossTmplUtils.TabSpace}return ret;"
        }
        var content = "$prefixContent"
        content += "${CrossTmplUtils.TabSpace}$retContent$funcContent($objContent, jmethod$methodName $argusContent);\n\n"
        content += "$suffixContent"

        return content
    }
}
