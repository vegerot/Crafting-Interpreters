package lox

import kotlin.contracts.contract

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

        val operator = expr.operator

        return when (operator.type) {
            // arithmetic
            TokenType.MINUS -> {
                loxRequireValueType<Double>(left, expr.operator)
                loxRequireValueType<Double>(right, expr.operator)
                left - right
            }
            TokenType.PLUS -> {
                if (left is Double && right is Double) left + right
                else if (left is String && right is String) left + right
                else throw RuntimeError(operator, "Operands must be two numbers or two strings")
            }
            TokenType.STAR -> {
                loxRequireValuesAreNumbers(expr.operator, left, right)
                left * right
            }
            TokenType.SLASH -> {
                loxRequireValuesAreNumbers(operator, left, right)
                left / right
            }

            // comparison
            TokenType.GREATER -> {
                loxRequireValuesAreNumbers(operator, left, right)
                left > right
            }
            TokenType.LESS -> {
                loxRequireValuesAreNumbers(operator, left, right)

                left < right
            }
            TokenType.GREATER_EQUAL -> {
                loxRequireValuesAreNumbers(operator, left, right)

                left >= right
            }
            TokenType.LESS_EQUAL -> {
                loxRequireValuesAreNumbers(operator, left, right)
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
                loxRequireValueIsNumber(right, expr.operator)
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

    @OptIn(kotlin.contracts.ExperimentalContracts::class)
    private inline fun loxRequireValuesAreNumbers(token: Token, firstValue: Any, secondValue: Any) {
        contract { returns() implies (firstValue is Double && secondValue is Double) }
        loxRequireValueType<Double>(firstValue, token)
        loxRequireValueType<Double>(secondValue, token)
    }

    @OptIn(kotlin.contracts.ExperimentalContracts::class)
    private inline fun loxRequireValueIsNumber(value: Any, token: Token) {
        contract { returns() implies (value is Double) }
        return loxRequireValueType<Double>(value, token)
    }

    @OptIn(kotlin.contracts.ExperimentalContracts::class)
    private inline fun <reified T> loxRequireValueType(value: Any, token: Token) {
        contract { returns() implies (value is T) }
        try {
            if (value !is T) {
                val message = "Failed requirement."
                throw IllegalArgumentException(message.toString())
            }
        } catch (ex: Exception) {
            throw RuntimeError(token, "fuck")
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
