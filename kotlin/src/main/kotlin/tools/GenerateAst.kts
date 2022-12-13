package tools

import java.io.File

fun createAstClass(name: String, types: List<String>): String {
    return """
sealed class $name {
${
        types.map { t ->
            val (className, fieldsString) = t.trim().split(':').map { it.trim() }
            val fields =
                fieldsString.split(',').map { it.trim() }.flatMap { it.split(' ').map { it.trim() }.zipWithNext() }

            createExpressionType(name, className, fields)
        }.joinToString("\n")
    }
}
 """.trimIndent()
}

fun createExpressionType(baseName: String, typeName: String, nonterminals: List<Pair<String, String>>): String {
    return "data class $typeName(${
        nonterminals.map{(type, name)-> "val $name: $type"}.joinToString(", ")
    }): $baseName()"
}

fun writeExprFile(outputDir: String, name: String, types: List<String>) {
    val header = "package lox"
    val body = createAstClass(name, types)

    val path = "$outputDir/$name.kt"

    File(path).writeText("""
        $header
        $body
    """.trimIndent())


}


fun main() {
    writeExprFile("src/main/kotlin/lox", "Expr", listOf(
            "Binary   : Expr left, Token operator, Expr right",
        "Grouping : Expr expression",
        "Literal  : Any value",
        "Unary    : Token operator, Expr right"
    ) )
}

main()
