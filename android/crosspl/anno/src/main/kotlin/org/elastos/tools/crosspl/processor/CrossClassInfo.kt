package org.elastos.tools.crosspl.processor

import com.sun.tools.javac.code.Symbol
import org.elastos.tools.crosspl.annotation.CrossInterface
import java.util.*
import javax.lang.model.element.Element
import javax.lang.model.element.ElementKind
import javax.lang.model.element.ExecutableElement
import javax.lang.model.element.TypeElement

class CrossClassInfo {
    companion object {
        fun Parse(classElement: Element): CrossClassInfo? {
            if(! isExtendsCrossBaseClass(classElement)) {
                return null
            }

            if(classElement::class != Symbol.ClassSymbol::class) {
                throw CrossPLException("@CrossClass can ONLY use for class")
            }

            val classSymbol = classElement as Symbol.ClassSymbol

            val classInfo = CrossClassInfo()

            classInfo.javaInfo.className = classSymbol.fullname.toString()
            classInfo.javaInfo.classPath = classSymbol.flatname.toString().replace(".", "/")

//            classInfo.cppInfo.namespace = "crosspl"
            val packageName = classSymbol.packge().toString()
            val className = classInfo.javaInfo.className.removePrefix("$packageName.")
            classInfo.cppInfo.className = className.replace(".", "_")

            var isStaticClass = false
            classSymbol.enclosedElements.forEach { // for kotlin static class
                if(it.getKind() == ElementKind.FIELD
                && it.toString() == "INSTANCE") {
                    isStaticClass = true
                }
            }

            var companionElement: Element? = null
            classSymbol.enclosedElements.forEach {
                if(it.getKind() == ElementKind.METHOD
                && it.getAnnotation(CrossInterface::class.java) != null) {
                    val methodInfo = CrossMethodInfo.Parse(it as ExecutableElement, isStaticClass)
                    classInfo.methodInfo.add(methodInfo);
                } else if (it.getKind() == ElementKind.CLASS
                && it.toString() == classElement.toString() + ".Companion") { // for kotlin companion object methods
                    companionElement = it
                }
            }
            if(companionElement != null) {
                classInfo.javaInfo.isKotlinCode = true
                companionElement!!.enclosedElements.forEach {
                    if(it.kind == ElementKind.METHOD
                    && it.getAnnotation(CrossInterface::class.java) != null) {
                        val methodInfo = CrossMethodInfo.Parse(it as ExecutableElement, true)
                        classInfo.methodInfo.add(methodInfo);
                    }
                }
            }

            Log.d("ClassInfo: ${classInfo}")
            return classInfo
        }

        private fun isExtendsCrossBaseClass(element: Element): Boolean {
//            val typeUtils = procEnv.typeUtils
//            val crossBaseType = procEnv.elementUtils.getTypeElement(CrossBaseClass).asType()
            val typeElement = findEnclosingTypeElement(element)
            if(typeElement.toString() == CrossBaseClass
            || typeElement.superclass.toString() == CrossBaseClass) {
                return true
            }

            Log.e("${element} MUSTBE extends from ${CrossBaseClass}")
            return false
        }

        private fun findEnclosingTypeElement(element: Element): TypeElement {
            var e = element
            while(e !is TypeElement) {
                e = e.enclosingElement;
            }

            return e
        }

        private const val CrossBaseClass = "org.elastos.tools.crosspl.CrossBase"

        private const val TmplKeyClassName = "%ClassName%"
        private const val TmplKeyPlatformFunction = "%PlatformFunction%"
        private const val TmplKeyNativeFunction = "%NativeFunction%"

        private const val TmplKeyJniJavaClass = "%JniJavaClass%"
        private const val TmplKeyJniNativeMethods = "%JniNativeMethods%"
        private const val TmplKeyKotlinStaticNativeMethods = "%KotlinStaticNativeMethods%"
    }

    class CppInfo {
//        lateinit var namespace: String
        lateinit var className: String

        override fun toString(): String {
//            return  "CppInfo{namespace=$namespace," +
//                    " className=$className}"
            return  "CppInfo{className=$className}"
        }
    }

    class JavaInfo {
        lateinit var className: String
        lateinit var classPath: String
        var isKotlinCode: Boolean = false

        override fun toString(): String {
            return "JavaInfo{className=$className," +
                    " classPath=$classPath," +
                    " isKotlinCode=$isKotlinCode}"
        }
    }

    override fun toString(): String {
        return  "ClassInfo{cppInfo=$cppInfo," +
                " javaInfo=$javaInfo," +
                " methodInfo=${methodInfo}}"
    }

    var cppInfo = CppInfo()
    var javaInfo = JavaInfo()
    var methodInfo = mutableListOf<CrossMethodInfo>()

    fun makeProxyDeclare(tmpl: String): String {
        var nativeFuncList = ""
        var platformFuncList = ""
        methodInfo.forEach {
            val functionDeclare = it.makeProxyDeclare()
            when {
                it.isNative -> nativeFuncList += "${CrossTmplUtils.TabSpace}static $functionDeclare;\n"
                else        -> platformFuncList += "${CrossTmplUtils.TabSpace}static $functionDeclare;\n"
            }
        }

        val content = tmpl
            .replace(TmplKeyClassName, cppInfo.className)
            .replace(TmplKeyPlatformFunction, platformFuncList)
            .replace(TmplKeyNativeFunction, nativeFuncList)

        return content
    }

    fun makeProxySource(tmpl: String): String {
        var nativeFuncList = ""
        var platformFuncList = ""
        var jniNativeMethodList = ""
        var kotlinStaticNativeMethodList = ""
        methodInfo.forEach {
            val funcSource = it.makeProxySource(cppInfo.className, javaInfo.classPath)
            if(it.isNative) {
                nativeFuncList += funcSource

                val methodContent = makeJniNativeMethod(it)
                if(javaInfo.isKotlinCode && it.isStatic) {
                    kotlinStaticNativeMethodList += "${CrossTmplUtils.TabSpace}${CrossTmplUtils.TabSpace}$methodContent,\n"
                } else {
                    jniNativeMethodList += "${CrossTmplUtils.TabSpace}${CrossTmplUtils.TabSpace}$methodContent,\n"
                }
            } else {
                platformFuncList += funcSource
            }
        }

        val content = tmpl
            .replace(TmplKeyClassName, cppInfo.className)
            .replace(TmplKeyPlatformFunction, platformFuncList)
            .replace(TmplKeyNativeFunction, nativeFuncList)
            .replace(TmplKeyJniNativeMethods, jniNativeMethodList)
            .replace(TmplKeyKotlinStaticNativeMethods, kotlinStaticNativeMethodList)
            .replace(TmplKeyJniJavaClass, javaInfo.className.replace(".", "/"))

        return content
    }

    private fun makeJniNativeMethod(methodInfo: CrossMethodInfo): String {
        var funcType = "("
        methodInfo.paramsType.forEach {
            funcType += it.toJniSigChar()
        }
        funcType += ")"
        funcType += methodInfo.returnType.toJniSigChar()

        val methodContent = "{\"${methodInfo.methodName}\", \"$funcType\", (void*)${methodInfo.methodName}}"

        return methodContent
    }
}
