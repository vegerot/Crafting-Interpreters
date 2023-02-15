package lox

class Interpreter : Expr.Visitor<Any> {

    override fun visitLiteralExpr(expr: Expr.Literal): Any {
        return expr.value.literal!!
    }

    override fun visitBinaryExpr(expr: Expr.Binary): Any {
        return expr
    }

    override fun visitGroupingExpr(expr: Expr.Grouping): Any {
        return evaluate(expr.expression)
    }

    override fun visitUnaryExpr(expr: Expr.Unary): Any {
        return expr
    }

    fun interpret(expression: Expr?): String {
        val value: Any? = evaluate(expression)
        return stringify(value)
    }

    private fun stringify(evald: Any?): String {
        return evald.toString()
    }

    fun evaluate(expr: Expr?): Any? {
        if (expr == null) return null
        return expr.accept(this)
    }
}
