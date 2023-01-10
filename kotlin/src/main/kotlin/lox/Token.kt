package lox

data class Token(val type: TokenType, val lexeme: String? = null, val literal: Any? = null, val line: Int? = null) {
    companion object {
        fun False(line: Int? = null) = Token(TokenType.FALSE, null, null, line)

        fun Number(value: Number, line: Int? = null) = Token(TokenType.NUMBER, null, value, line)

        fun Star(line: Int? = null) = Token(TokenType.STAR, null, null, line)

        fun Minus(line: Int? = null) = Token(TokenType.MINUS, null, null, line)

        fun EOF(line: Int? = null) = Token(TokenType.EOF, null, null, line)
    }
}
