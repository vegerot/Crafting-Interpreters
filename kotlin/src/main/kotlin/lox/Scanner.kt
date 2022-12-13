package lox

class Scanner(val source: String) {
    private var tokens: MutableList<Token> = ArrayList()

    /**
     * offset to first character in lexeme
     */
    private var start: Int = 0

    /**
     * offset to current character in lexeme
     */
    private var current: Int = start

    /**
     * line number where [current] is
     */
    private var line: Int = 0

    fun scanTokens(): List<Token> {
        while (!isAtEnd()) {
            start = current
            scanToken()
        }
        tokens.add(Token(TokenType.EOF, null, null, line))
        return tokens
    }

    private fun isAtEnd(): Boolean {
        return current >= source.length
    }

    private fun scanToken() {
        val c: Char = advance()
        val nextTokenType: TokenType = when (c) {
            '(' -> TokenType.LEFT_PAREN
            ')' -> TokenType.RIGHT_PAREN
            '{' -> TokenType.LEFT_BRACE
            '}' -> TokenType.RIGHT_BRACE
            ',' -> TokenType.COMMA
            '.' -> TokenType.DOT
            '-' -> TokenType.MINUS
            '+' -> TokenType.PLUS
            ';' -> TokenType.SEMICOLON
            '*' -> TokenType.STAR
            '=' -> if (isNextToken('=')) TokenType.EQUAL_EQUAL else TokenType.EQUAL
            '!' -> if (isNextToken('=')) TokenType.BANG_EQUAL else TokenType.BANG
            '<' -> if (isNextToken('=')) TokenType.LESS_EQUAL else TokenType.LESS
            '>' -> if (isNextToken('=')) TokenType.GREATER_EQUAL else TokenType.GREATER
            '/' -> if (isNextToken('/')) {
                consumeUntil('\n')
                TokenType.SKIP
            } else {
                TokenType.SLASH
            }

            ' ' -> TokenType.SKIP
            '\r' -> TokenType.SKIP
            '\t' -> TokenType.SKIP
            '\n' -> {
                ++line
                TokenType.SKIP
            }

            '"' -> TokenType.STRING
            in validDigits -> TokenType.NUMBER
            in validStartingIdentifierChars -> TokenType.IDENTIFIER
            else -> TokenType.INVALID

        }
        if (nextTokenType == TokenType.SKIP) return
        if (nextTokenType == TokenType.INVALID) {
            Lox.error(line, null, "unexpected character: $c")
        }
        val nextToken: Token = when (nextTokenType) {
            TokenType.STRING -> consumeUntilEOSandReturnString()
            TokenType.NUMBER -> consumeNumber()
            TokenType.IDENTIFIER -> consumeUntilEOI()
            else -> makeToken(nextTokenType)
        }
        addToken(nextToken)
    }

    private val validDigits = '0'..'9'
    private val validStartingIdentifierChars = 'A'..'z'
    private val validIdentifierChars = validDigits + validStartingIdentifierChars
    private val keywordToTokenType: Map<String, TokenType> = mapOf(
        "and" to TokenType.AND,
        "class" to TokenType.CLASS,
        "else" to TokenType.ELSE,
        "false" to TokenType.FALSE,
        "for" to TokenType.FOR,
        "fun" to TokenType.FUN,
        "if" to TokenType.IF,
        "nil" to TokenType.NIL,
        "or" to TokenType.OR,
        "print" to TokenType.PRINT,
        "return" to TokenType.RETURN,
        "super" to TokenType.SUPER,
        "this" to TokenType.THIS,
        "true" to TokenType.TRUE,
        "var" to TokenType.VAR,
        "while" to TokenType.WHILE
    )

    private fun consumeNumber(): Token {
        while (peek() in validDigits)
            advance()

        if (peek() == '.') {
            advance() // consume the `.`
            while (peek() in validDigits) {
                advance()
            }
        }
        val substring = source.substring(start, current)
        val number = substring.toDoubleOrNull()
        if (number == null) {
            Lox.reportError(line, "", "invalid number literal")
        }
        return Token(type = TokenType.NUMBER, lexeme = substring, literal = number, line)
    }

    private fun consumeUntilEOSandReturnString(): Token {
        while (source.getOrNull(current + 1) != null && source.getOrNull(current) != '"' && !isAtEnd()) {
            advance()
        }
        if (isAtEnd()) {
            Lox.error(line, where = null, "unterminated string")
            //return
        }
        val stringContents = run {
            val length = current - (start + 1)
            if (length == 0) "" else source.substring(start + 1, current)
        }
        
        val stringIncludingQuotes = source.substring(start, current + 1)
        advance()
        return Token(TokenType.STRING, stringIncludingQuotes, stringContents, line)
    }

    private fun consumeUntilEOI(): Token {
        while (peek() in validIdentifierChars) {
            advance()
        }

        val tokenName = source.substring(start, current)
        val tokenType = keywordToTokenType.getOrDefault(tokenName, TokenType.IDENTIFIER)
        val lexeme = if (tokenType == TokenType.IDENTIFIER) tokenName else null

        return Token(tokenType, lexeme, null, line)
    }

    private fun consumeUntil(char: Char): Char? {
        while (peekNext() != char && !isAtEnd()) {
            advance()
        }
        return if (isAtEnd()) null else advance()
    }

    private fun isNextToken(char: Char): Boolean =
        if (peek() == char) {
            ++current
            true
        } else {
            false
        }


    private fun peek(): Char? {
        return source.getOrNull(current)
    }

    private fun peekNext(): Char? = source.getOrNull(current + 1)

    fun isDigit(c: Char): Boolean {
        return c in validDigits
    }

    private fun advance(): Char {
        return source[current++]
    }

    private fun makeToken(type: TokenType): Token =
        Token(type, lexeme = null, literal = null, line)

    private fun addToken(token: Token) {
        tokens.add(token)
    }

}