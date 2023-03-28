package lox

class Parser(private val tokens: List<Token>) {
    private var current: Int = 0

    fun parse(): List<Stmt> {
        val statements: MutableList<Stmt> = mutableListOf()
        while (!isAtEnd()) {
            val decl = declaration()
            if (decl != null) statements.add(decl)
        }
        return statements
        // this doesn't work right without statements
        /*assert(current == tokens.size) {
            "invalid expression ${tokens.drop(current)}"
        }*/
    }

    private fun declaration(): Stmt? {
        return try {
            if (ifMatchConsume(TokenType.VAR)) varDeclaration() else statement()
        } catch (error: ParseError) {
            synchronize()
            null
        }
    }

    private fun varDeclaration(): Stmt {
        val name: Token = assertNextCharAndConsume(TokenType.IDENTIFIER, "Expected a variable name")

        val initializer: Expr? = if (ifMatchConsume(TokenType.EQUAL)) expression() else null

        assertNextCharAndConsume(TokenType.SEMICOLON, "Expected ';' after variable declaration")
        return Stmt.Var(name, initializer)
    }

    private fun statement(): Stmt {
        return if (ifMatchConsume(TokenType.PRINT)) printStatement()
        else if (ifMatchConsume(TokenType.LEFT_BRACE)) Stmt.Block(block())
        else expressionStatement()
    }

    private fun printStatement(): Stmt.Print {
        val value = expression()
        assertNextCharAndConsume(TokenType.SEMICOLON, "Expect ';' after value")
        return Stmt.Print(value)
    }

    private fun expressionStatement(): Stmt.Expression {
        val expr: Expr = expression()
        assertNextCharAndConsume(TokenType.SEMICOLON, "Expect ';' after expression")
        return Stmt.Expression(expr)
    }

    private fun block(): List<Stmt> {
        val statements: MutableList<Stmt> = mutableListOf()

        while (!isNextTokenOfType(TokenType.RIGHT_BRACE)) {
            val stmt: Stmt? = declaration()
            // stmt will be null if there is a parsing error that we recover from
            if (stmt != null) statements.add(stmt)
        }
        assertNextCharAndConsume(TokenType.RIGHT_BRACE, "Expect '}' after block.")
        return statements
    }

    /** ONLY CALL FROM TESTS */
    public fun parseSingleExpression(): Expr? {
        if (tokens.isEmpty()) return null
        val firstExpression = expression()
        return firstExpression
    }

    private fun expression() = assignment()

    private fun assignment(): Expr {
        val expr: Expr = equality()

        return if (ifMatchConsume(TokenType.EQUAL)) {
            val equals: Token = previous()
            val value: Expr = assignment()

            if (expr is Expr.Variable) {
                val name: Token = expr.name
                Expr.Assign(name, value)
            } else throw error(equals, "can only assign to l-values")
        } else expr
    }

    private fun equality(): Expr {
        val expr: Expr = comparison()
        return if (ifMatchConsume(TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL)) {
            val operator: Token = previous()
            val right: Expr = comparison()
            Expr.Binary(expr, operator, right)
        } else expr
    }

    private fun comparison(): Expr {
        val left = term()
        if (isAtEnd()) return left
        return if (
            ifMatchConsume(
                TokenType.LESS,
                TokenType.LESS_EQUAL,
                TokenType.GREATER,
                TokenType.GREATER_EQUAL,
            )
        ) {
            val operator = previous()
            val right = term()
            Expr.Binary(left, operator, right)
        } else left
    }

    private fun term(): Expr {
        val left = factor()
        if (isAtEnd()) return left
        return when (peek().type) {
            TokenType.PLUS,
            TokenType.MINUS -> {
                val operator = consume()
                val right = factor()
                Expr.Binary(left, operator, right)
            }
            else -> left
        }
    }

    private fun factor(): Expr {
        val left = unary()
        if (isAtEnd()) return left
        return when (peek().type) {
            TokenType.STAR -> {
                val operator = peek()
                advance()
                val right = unary()
                Expr.Binary(left, operator, right)
            }
            TokenType.SLASH -> {
                val operator = peek()
                advance()
                val right = unary()
                Expr.Binary(left, operator, right)
            }
            else -> left
        }
    }

    private fun unary(): Expr {
        return when (peek().type) {
            TokenType.BANG -> {
                advance()
                Expr.Unary(previous(), unary())
            }
            TokenType.MINUS -> {
                advance()
                Expr.Unary(previous(), unary())
            }
            else -> primary()
        }
    }

    private fun primary(): Expr {
        return if (ifMatchConsume(TokenType.TRUE)) Expr.Literal(previous())
        else if (ifMatchConsume(TokenType.FALSE)) Expr.Literal(previous())
        else if (ifMatchConsume(TokenType.NIL)) Expr.Literal(previous())
        else if (ifMatchConsume(TokenType.NUMBER)) Expr.Literal(previous())
        else if (ifMatchConsume(TokenType.STRING)) Expr.Literal(previous())
        else if (ifMatchConsume(TokenType.IDENTIFIER)) Expr.Variable(previous())
        else if (ifMatchConsume(TokenType.LEFT_PAREN)) {
            val expr = expression()
            assertNextCharAndConsume(TokenType.RIGHT_PAREN, "Expect ')' after expression")
            Expr.Grouping(expr)
        } else if (peek().type === TokenType.EOF) {
            assert(current == tokens.size - 1) { "unexpected EOF" }
            val expr = Expr.Literal(peek())
            ++current
            expr
        } else {
            assert(false) { "invalid expression at line ${peek().line}" }
            // unreachable on purpose but needed to get typechecking to work
            Expr.Literal(Token(TokenType.INVALID, "😵🔫", "fuck", -1))
        }
    }

    private fun ifMatchConsume(vararg types: TokenType): Boolean {
        for (type in types) {
            if (isNextTokenOfType(type)) {
                advance()
                return true
            }
        }
        return false
    }

    private fun assertNextCharAndConsume(type: TokenType, errorMessage: String): Token {
        if (isNextTokenOfType(type)) return advance() else throw error(peek(), errorMessage)
    }

    private fun consume(): Token {
        return advance()
    }

    private fun isNextTokenOfType(type: TokenType): Boolean {
        return if (!isAtEnd()) peek().type == type else false
    }

    private fun advance(): Token {
        if (!isAtEnd()) ++current
        return previous()
    }

    private fun isAtEnd(): Boolean {
        return current >= tokens.size || peek().type == TokenType.EOF
    }

    private fun peek(): Token {
        return tokens[current]
    }

    private fun previous(): Token {
        return tokens[current - 1]
    }

    private fun error(token: Token, message: String): ParseError {
        Lox.error(token, message)
        return ParseError()
    }

    private fun synchronize() {
        var prev = advance()

        while (!isAtEnd()) {
            if (prev.type == TokenType.SEMICOLON) return

            when (peek().type) {
                TokenType.CLASS,
                TokenType.FUN,
                TokenType.VAR,
                TokenType.FOR,
                TokenType.IF,
                TokenType.WHILE,
                TokenType.PRINT,
                TokenType.RETURN -> return
                else -> {}
            }
            prev = advance()
        }
    }

    companion object {
        class ParseError : RuntimeException()
    }
}
