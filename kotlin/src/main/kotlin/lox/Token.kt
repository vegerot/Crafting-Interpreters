package lox

data class Token(
    val type: TokenType,
    val lexeme: String? = null,
    val literal: LoxValue? = null,
    val line: Int
)
