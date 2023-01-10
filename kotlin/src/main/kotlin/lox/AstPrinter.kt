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
        return "${expr.value}"
    }

    override fun visitUnaryExpr(expr: Expr.Unary): String {
        return "(${expr.operator.lexeme} ${expr.right.accept(this)})"
    }
}

fun main(args: Array<String>) {
    val expression =
        Expr.Binary(
            Expr.Unary(Token(TokenType.MINUS, "-", null, 1), Expr.Literal(123)),
            Token(TokenType.STAR, "*", null, 1),
            Expr.Grouping(Expr.Literal(45.67))
        )

    println(AstPrinter().print(expression))
}
