package lox

class AstPrinter : Expr.Visitor<String>, Stmt.Visitor<String> {
    fun print(expr: Expr): String {
        return expr.accept(this)
    }
    fun printStatement(stmt: Stmt): String {
        return stmt.accept(this)
    }

    override fun visitBlockStmt(stmt: Stmt.Block): String {
        return "(block ${stmt.statements.map{it.accept(this)}} )"
    }

    override fun visitExpressionStmt(stmt: Stmt.Expression): String {
        return "(; ${stmt.expression.accept(this)})"
    }

    override fun visitIfStmt(stmt: Stmt.If): String {
        return if (stmt.elseBranch == null) {
            parenthesize2("if", listOf(stmt.condition, stmt.thenBranch))
        } else parenthesize2("if-else", listOf(stmt.condition, stmt.thenBranch, stmt.elseBranch))
    }

    override fun visitCallExpr(expr: Expr.Call): String {
        val functionName = expr.callee.accept(this)
        val arguments = expr.arguments.joinToString(", ") { it.accept(this) }

        return "${functionName}(${arguments})"
    }

    private fun parenthesize2(name: String, parts: List<Any>): String {
        fun transform(string: String, parts: List<*>): String {
            var str = string
            for (part in parts) {
                str += " "
                str +=
                    when (part) {
                        is Expr -> {
                            part.accept(this)
                        }
                        is Stmt -> {
                            part.accept(this)
                        }
                        is Token -> {
                            part.lexeme
                        }
                        is List<*> -> {
                            transform(str, part)
                        }
                        else -> {
                            part
                        }
                    }
            }
            return str
        }

        return "(${name} ${transform("", parts)})"
    }

    override fun visitPrintStmt(stmt: Stmt.Print): String {
        return "(print ${stmt.expression.accept(this)})"
    }

    override fun visitVarStmt(stmt: Stmt.Var): String {
        return if (stmt.initializer == null) {
            parenthesize2("var", listOf(stmt.name))
        } else parenthesize2("var", listOf(stmt.name, "=", stmt.initializer))
    }

    override fun visitWhileStmt(stmt: Stmt.While): String {
        return parenthesize2("while", listOf(stmt.condition, stmt.body))
    }

    override fun visitBinaryExpr(expr: Expr.Binary): String {
        // (+ 1 2)
        return "(${expr.operator.lexeme} ${expr.left.accept(this)} ${expr.right.accept(this)})"
    }

    override fun visitGroupingExpr(expr: Expr.Grouping): String {
        return "(${expr.expression.accept(this)})"
    }

    override fun visitLiteralExpr(expr: Expr.Literal): String {
        return "${expr.value.literal ?: expr.value.lexeme}"
    }

    override fun visitLogicalExpr(expr: Expr.Logical): String {
        return "(${expr.operator.lexeme} ${expr.left.accept(this)} ${expr.right.accept(this)})"
    }

    override fun visitUnaryExpr(expr: Expr.Unary): String {
        return "(${expr.operator.lexeme} ${expr.right.accept(this)})"
    }

    override fun visitVariableExpr(expr: Expr.Variable): String {
        return "var(${expr.name.lexeme})"
    }

    override fun visitAssignExpr(expr: Expr.Assign): String =
        "var ${expr.name.lexeme} = ${expr.value}"
}

fun main(args: Array<String>) {
    val expression =
        Expr.Binary(
            Expr.Unary(
                Token(TokenType.MINUS, null, null, 1),
                Expr.Literal(Token(TokenType.FALSE, null, null, 1))
            ),
            Token(TokenType.STAR, null, null, 1),
            Expr.Grouping(Expr.Literal(Token(TokenType.NUMBER, null, 45.67, 1)))
        )

    println(AstPrinter().print(expression))
}
