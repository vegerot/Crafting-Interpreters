package lox

class Interpreter : Expr.Visitor<Any> {

    override fun visitLiteralExpr(expr: Expr.Literal): Any {
        return when (expr.value.type) {
            TokenType.NUMBER -> expr.value.literal!!
            TokenType.FALSE -> false
            TokenType.TRUE -> true
            TokenType.STRING -> expr.value.lexeme!!
            else -> {
                assert(false) { "invalid literal expression: ${expr.value}" }
            }
        }
    }

    override fun visitBinaryExpr(expr: Expr.Binary): Any {
        return expr
    }

    override fun visitGroupingExpr(expr: Expr.Grouping): Any {
        return evaluate(expr.expression)
    }

    override fun visitUnaryExpr(expr: Expr.Unary): Any {
        val right = evaluate(expr.right)

        return when (expr.operator.type) {
            TokenType.MINUS -> {
                require(right is Double)
                -right
            }
            TokenType.BANG -> {
                fun isTruthy(t: Any): Boolean {
                    return when (t) {
                        is Boolean -> t
                        // my types are totally wrong.  damn
                        null -> false
                        else -> true
                    }
                }
                !isTruthy(right)
            }
            // idea: somehow encode "unary" types in the type system so we can exhaustively
            // pattern-match without `else`
            else -> assert(false) { "unknown UNARY token" }
        }
    }

    fun interpret(expression: Expr?): String {
        if (expression == null) return "null"
        val value: Any = evaluate(expression)
        return stringify(value)
    }

    private fun stringify(evald: Any?): String {
        return evald.toString()
    }

    private fun evaluate(expr: Expr): Any {
        return expr.accept(this)
    }
}
