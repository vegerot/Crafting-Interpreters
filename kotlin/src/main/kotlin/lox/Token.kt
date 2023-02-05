package lox

data class Token(
    val type: TokenType,
    val lexeme: String? = null,
    val literal: Any? = null,
    val line: Int
)
