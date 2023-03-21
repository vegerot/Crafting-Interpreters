package lox

class AstPrinter : Expr.Visitor<String> {
    fun print(expr: Expr): String {
        return expr.accept(this)
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

    override fun visitUnaryExpr(expr: Expr.Unary): String {
        return "(${expr.operator.lexeme} ${expr.right.accept(this)})"
    }

    override fun visitVariableExpr(expr: Expr.Variable): String {
        return "var(${expr.name.lexeme})"
    }
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
