package lox

class Parser(private val tokens: List<Token>) {
    private var current: Int = 0

    fun parse(): Expr? {
        if (tokens.isEmpty()) return null
        val parsed = expression()
        // this doesn't work right without statements
        /*assert(current == tokens.size) {
            "invalid expression ${tokens.drop(current)}"
        }*/
        return parsed
    }

    private fun expression() = equality()

    private fun equality(): Expr {
        val expr: Expr = comparison()
        return if (match(TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL)) {
            val operator: Token = previous()
            val right: Expr = comparison()
            Expr.Binary(expr, operator, right)
        } else expr
    }

    private fun comparison(): Expr {
        val left = term()
        if (isAtEnd()) return left
        return if (
            match(
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
        return if (match(TokenType.TRUE)) Expr.Literal(previous())
        else if (match(TokenType.FALSE)) Expr.Literal(previous())
        else if (match(TokenType.NIL)) Expr.Literal(previous())
        else if (match(TokenType.NUMBER)) Expr.Literal(previous())
        else if (match(TokenType.STRING)) Expr.Literal(previous())
        else if (match(TokenType.LEFT_PAREN)) {
            val expr = expression()
            consumeUntil(TokenType.RIGHT_PAREN, "Expect ')' after expression")
            Expr.Grouping(expr)
        } else if (peek().type === TokenType.EOF) {
            assert(current == tokens.size - 1) { "unexpected EOF" }
            val expr = Expr.Literal(peek())
            ++current
            expr
        } else {
            assert(false) { "invalid expression" }
            // unreachable on purpose but needed to get typechecking to work
            Expr.Literal(Token(TokenType.INVALID, "😵🔫", "fuck", -1))
        }
    }

    private fun match(vararg types: TokenType): Boolean {
        for (type in types) {
            if (isNextTokenOfType(type)) {
                advance()
                return true
            }
        }
        return false
    }

    private fun consumeUntil(type: TokenType, errorMessage: String): Token {
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

    companion object {
        class ParseError : RuntimeException()
    }
}
