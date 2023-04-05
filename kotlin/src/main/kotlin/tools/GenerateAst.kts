package tools

import java.io.File

fun createAstClass(name: String, types: List<String>): String {
    return """
sealed class $name {
${createVisitorInterface(name, types)}
abstract fun<R> accept(visitor: Visitor<R>): R
${
        types.map { t ->
            val (className, fieldsString) = t.trim().split(':').map { it.trim() }
            val fields =
                fieldsString.split(',').map { it.trim() }.flatMap { it.split(' ').map { it.trim() }.zipWithNext() }

            createExpressionType(name, className, fields)
        }.joinToString("\n")
    }
}
 """
        .trimIndent()
}

fun createVisitorInterface(baseName: String, types: List<String>): String {
    return """
        interface Visitor<R> {
        ${types.map{type->
            val (typeName, _) = type.trim().split(':').map { it.trim() }
            "fun visit${typeName}${baseName}(${baseName.lowercase()}: $typeName): R"
        }.joinToString("\n")
        }
        }

    """
        .trimIndent()
}

fun createExpressionType(
    baseName: String,
    typeName: String,
    nonterminals: List<Pair<String, String>>
): String {
    return """
    data class $typeName(${
        nonterminals.map{(type, name)-> "val $name: $type"}.joinToString(", ")
        }): $baseName() {
        override fun<R> accept(visitor: Visitor<R>): R {
            return visitor.visit${typeName}${baseName}(this)
            }
        }
    """
        .trimIndent()
}

fun writeAstFile(outputDir: String, name: String, types: List<String>) {
    val header = "package lox"
    val body = createAstClass(name, types)

    val path = "$outputDir/$name.kt"

    File(path)
        .writeText(
            """
        $header
        $body
    """
                .trimIndent()
        )
}

/** run like `kotlinc -script .\src\main\kotlin\tools\GenerateAst.kts` */
fun main() {
    writeAstFile(
        "src/main/kotlin/lox",
        "Expr",
        listOf(
            "Assign   : Token name, Expr value",
            "Binary   : Expr left, Token operator, Expr right",
            "Grouping : Expr expression",
            "Literal  : Token value",
            "Logical  : Expr left, Token operator, Expr right",
            "Unary    : Token operator, Expr right",
            "Variable : Token name",
        )
    )

    writeAstFile(
        "src/main/kotlin/lox",
        "Stmt",
        listOf(
            "Block      : List<Stmt> statements",
            "Expression : Expr expression",
            "If         : Expr condition, Stmt thenBranch, Stmt? elseBranch",
            "Print      : Expr expression",
            "Var        : Token name, Expr? initializer ",
            "While      : Expr condition, Stmt body"
        )
    )
}

// don't generate ast during tests
if (!args.contains("--no-write")) main()
