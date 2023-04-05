package lox

import kotlin.contracts.contract

class Interpreter : Expr.Visitor<LoxValue>, Stmt.Visitor<Unit> {
    private var environment: Environment = Environment()

    fun interpret(statements: List<Stmt>): Unit {
        try {
            for (statement in statements) {
                execute(statement)
            }
        } catch (error: RuntimeError) {
            Lox.runtimeError(error)
        }
    }

    private fun evaluate(expr: Expr): LoxValue {
        return expr.accept(this)
    }

    private fun execute(stmt: Stmt) {
        stmt.accept(this)
    }

    private fun executeBlock(statements: List<Stmt>, environment: Environment) {
        val previousEnvironment: Environment = this.environment
        try {
            this.environment = environment
            for (statement in statements) {
                execute(statement)
            }
        } finally {
            this.environment = previousEnvironment
        }
    }

    override fun visitBlockStmt(stmt: Stmt.Block) {
        executeBlock(stmt.statements, Environment(environment))
    }

    override fun visitExpressionStmt(stmt: Stmt.Expression) {
        evaluate(stmt.expression)
    }

    override fun visitIfStmt(stmt: Stmt.If) {
        if (isTruthy(evaluate(stmt.condition))) execute(stmt.thenBranch)
        else if (stmt.elseBranch != null) execute(stmt.elseBranch)
    }

    override fun visitPrintStmt(stmt: Stmt.Print) {
        val value = evaluate(stmt.expression)
        println(stringify(value))
    }

    override fun visitVarStmt(stmt: Stmt.Var) {
        val value =
            if (stmt.initializer != null) {
                evaluate(stmt.initializer)
            } else null

        environment.define(stmt.name.lexeme!!, value)
    }

    override fun visitWhileStmt(stmt: Stmt.While) {
        while (isTruthy(evaluate(stmt.condition))) {
            execute(stmt.body)
        }
    }

    override fun visitAssignExpr(expr: Expr.Assign): LoxValue {
        val value = evaluate(expr.value)
        environment.assign(expr.name, value)
        return value
    }

    override fun visitBinaryExpr(expr: Expr.Binary): LoxValue {
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

    override fun visitGroupingExpr(expr: Expr.Grouping): LoxValue {
        return evaluate(expr.expression)
    }

    override fun visitLiteralExpr(expr: Expr.Literal): LoxValue {
        return when (expr.value.type) {
            TokenType.NUMBER -> expr.value.literal!!
            TokenType.FALSE -> false
            TokenType.TRUE -> true
            TokenType.STRING -> expr.value.literal!!
            // TODO: support nil
            // TokenType.NIL -> null!!
            else -> {
                assert(false) { "invalid literal expression: ${expr.value}" }
            }
        }
    }

    override fun visitLogicalExpr(expr: Expr.Logical): LoxValue {
        val left: LoxValue = evaluate(expr.left)

        return when (expr.operator.type) {
            TokenType.OR -> {
                if (isTruthy(left)) left else evaluate(expr.right)
            }
            TokenType.AND -> {
                if (!isTruthy(left)) left else evaluate(expr.right)
            }
            else -> {
                assert(false) { "invalid logical operator: ${expr.operator.type}" }
            }
        }
    }

    override fun visitUnaryExpr(expr: Expr.Unary): LoxValue {
        val right = evaluate(expr.right)

        return when (expr.operator.type) {
            TokenType.MINUS -> {
                loxRequireValueIsNumber(right, expr.operator)
                -right
            }
            TokenType.BANG -> {
                !isTruthy(right)
            }
            // idea: somehow encode "unary" types in the type system so we can exhaustively
            // pattern-match without `else`
            else -> assert(false) { "unknown UNARY token" }
        }
    }

    override fun visitVariableExpr(expr: Expr.Variable): LoxValue {
        return environment.get(expr.name)!!
    }

    /* UTILS */

    @OptIn(kotlin.contracts.ExperimentalContracts::class)
    private fun loxRequireValuesAreNumbers(
        token: Token,
        firstValue: LoxValue,
        secondValue: LoxValue
    ) {
        contract { returns() implies (firstValue is Double && secondValue is Double) }
        loxRequireValueType<Double>(firstValue, token)
        loxRequireValueType<Double>(secondValue, token)
    }

    @OptIn(kotlin.contracts.ExperimentalContracts::class)
    private fun loxRequireValueIsNumber(value: LoxValue, token: Token) {
        contract { returns() implies (value is Double) }
        return loxRequireValueType<Double>(value, token)
    }

    @OptIn(kotlin.contracts.ExperimentalContracts::class)
    private inline fun <reified T> loxRequireValueType(value: LoxValue, token: Token) {
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

    private fun isTruthy(t: LoxValue): Boolean {
        return when (t) {
            is Boolean -> t
            // my types are totally wrong.  damn
            null -> false
            else -> true
        }
    }

    private fun stringify(evald: LoxValue?): String {
        return when (evald) {
            is Double -> {
                val str = evald.toString()
                if (str.endsWith(".0")) str.dropLast(2) else str
            }
            else -> evald.toString()
        }
    }

    /** ONLY RUN FROM TESTS */
    fun interpretExpression(expr: Expr?): String {
        if (expr == null) return "null"
        val value: LoxValue = evaluate(expr)
        return stringify(value)
    }
}
