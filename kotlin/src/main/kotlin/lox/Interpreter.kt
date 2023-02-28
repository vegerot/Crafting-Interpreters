package lox

class Interpreter : Expr.Visitor<Any> {

    override fun visitLiteralExpr(expr: Expr.Literal): Any {
        return when (expr.value.type) {
            TokenType.NUMBER -> expr.value.literal!!
            TokenType.FALSE -> false
            TokenType.TRUE -> true
            TokenType.STRING -> expr.value.literal!!
            else -> {
                assert(false) { "invalid literal expression: ${expr.value}" }
            }
        }
    }

    override fun visitBinaryExpr(expr: Expr.Binary): Any {
        val left = evaluate(expr.left)
        val right = evaluate(expr.right)

        return when (expr.operator.type) {
            // arithmetic
            TokenType.MINUS -> {
                require(left is Double)
                require(right is Double)
                left - right
            }
            TokenType.PLUS -> {
                if (left is Double && right is Double) left + right
                else if (left is String && right is String) left + right
                else
                    assert(false) {
                        "for binary + operations, left and right must be either both numbers or both strings"
                    }
            }
            TokenType.STAR -> {
                require(left is Double)
                require(right is Double)
                left * right
            }
            TokenType.SLASH -> {
                require(left is Double)
                require(right is Double)
                left / right
            }

            // comparison
            TokenType.GREATER -> {
                require(left is Double)
                require(right is Double)
                left > right
            }
            TokenType.LESS -> {
                require(left is Double)
                require(right is Double)
                left < right
            }
            TokenType.GREATER_EQUAL -> {
                require(left is Double)
                require(right is Double)
                left >= right
            }
            TokenType.LESS_EQUAL -> {
                require(left is Double)
                require(right is Double)
                left <= right
            }

            // equality
            TokenType.EQUAL_EQUAL -> {
                left == right
            }
            TokenType.BANG_EQUAL -> {
                left != right
            }
            else -> assert(false) { "unknown binary operator ${expr.operator.type}" }
        }
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
