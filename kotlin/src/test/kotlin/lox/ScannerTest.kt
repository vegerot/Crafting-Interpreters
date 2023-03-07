package lox

import kotlin.test.Test
import kotlin.test.assertEquals

internal class ScannerTest {
    @Test
    fun testLastTokenIsEOF() {
        val want: List<Token> = listOf(Token(TokenType.EOF, null, null, 0))
        val got = Scanner("").scanTokens()
        assertEquals(want, got)
    }

    @Test
    fun testSingleToken() {
        val want: List<Token> =
            listOf(Token(TokenType.LEFT_PAREN, "(", null, 0), Token(TokenType.EOF, null, null, 0))
        val got = Scanner("(").scanTokens()
        assertEquals(want, got)
    }

    @Test
    fun testTwoTokens() {
        val want: List<Token> =
            listOf(
                Token(TokenType.LEFT_PAREN, "(", null, 0),
                Token(TokenType.RIGHT_PAREN, ")", null, 0),
                Token(TokenType.EOF, null, null, 0)
            )
        val got = Scanner("()").scanTokens()
        assertEquals(want, got)
    }

    @Test
    fun kitchenSink() {
        val sourceToTokens =
            mapOf(
                "" to listOf(),
                "//" to listOf(),
                "// aoeuaoeu aoeu" to listOf(),
                """
            // this is a comment
            (
            """
                    .trimIndent() to listOf(Token(TokenType.LEFT_PAREN, "(", null, 1)),
                "{} // bye" to
                    listOf(
                        Token(TokenType.LEFT_BRACE, "{", null, 0),
                        Token(TokenType.RIGHT_BRACE, "}", null, 0),
                    ),
                "{}" to
                    listOf(
                        Token(TokenType.LEFT_BRACE, "{", null, 0),
                        Token(TokenType.RIGHT_BRACE, "}", null, 0),
                    ),
                """
            (
            )
            """
                    .trimIndent() to
                    listOf(
                        Token(TokenType.LEFT_PAREN, "(", null, 0),
                        Token(TokenType.RIGHT_PAREN, ")", null, 1),
                    ),
                "\"hi\"" to listOf(Token(TokenType.STRING, "\"hi\"", "hi", 0)),
                "\"\"" to
                    listOf(
                        Token(TokenType.STRING, "\"\"", "", 0),
                    ),
                """
            "su
            p"
            """
                    .trimIndent() to
                    listOf(
                        Token(TokenType.STRING, "\"su\np\"", "su\np", 0),
                    ),
                "1" to
                    listOf(
                        Token(TokenType.NUMBER, "1", 1.0, 0),
                    ),
                "42.69" to
                    listOf(
                        Token(TokenType.NUMBER, "42.69", 42.69, 0),
                    ),
                "hello" to listOf(Token(TokenType.IDENTIFIER, "hello", null, 0)),
                "hello+1" to
                    listOf(
                        Token(TokenType.IDENTIFIER, "hello", null, 0),
                        Token(TokenType.PLUS, "+", null, 0),
                        Token(TokenType.NUMBER, "1", 1.0, 0)
                    ),
                "true" to listOf(Token(TokenType.TRUE, "true", null, 0)),
                "true and false" to
                    listOf(
                        Token(TokenType.TRUE, "true", null, 0),
                        Token(TokenType.AND, "and", null, 0),
                        Token(TokenType.FALSE, "false", null, 0)
                    ),
                """
            var a =
            17;
            """
                    .trimIndent() to
                    listOf(
                        Token(TokenType.VAR, "var", null, 0),
                        Token(TokenType.IDENTIFIER, "a", null, 0),
                        Token(TokenType.EQUAL, "=", null, 0),
                        Token(TokenType.NUMBER, "17", 17.0, 1),
                        Token(TokenType.SEMICOLON, ";", null, 1),
                    )
            )
        sourceToTokens.entries.forEach { kv ->
            val (source, want) = kv
            val got = Scanner(source).scanTokens()
            assertEquals(want, got.dropLast(1))
            assertEquals(TokenType.EOF, got.last().type)
        }
    }

    @Test
    fun symbols() {
        val want =
            listOf(
                Token(TokenType.LEFT_PAREN, "(", null, 1),
                Token(TokenType.LEFT_PAREN, "(", null, 1),
                Token(TokenType.RIGHT_PAREN, ")", null, 1),
                Token(TokenType.RIGHT_PAREN, ")", null, 1),
                Token(TokenType.LEFT_BRACE, "{", null, 1),
                Token(TokenType.RIGHT_BRACE, "}", null, 1),
                Token(TokenType.BANG, "!", null, 2),
                Token(TokenType.STAR, "*", null, 2),
                Token(TokenType.PLUS, "+", null, 2),
                Token(TokenType.MINUS, "-", null, 2),
                Token(TokenType.SLASH, "/", null, 2),
                Token(TokenType.EQUAL, "=", null, 2),
                Token(TokenType.LESS, "<", null, 2),
                Token(TokenType.GREATER, ">", null, 2),
                Token(TokenType.LESS_EQUAL, "<=", null, 2),
                Token(TokenType.EQUAL_EQUAL, "==", null, 2),
                Token(TokenType.EOF, null, null, 2)
            )
        val source =
            """
        // this is a comment
        (( )){} // grouping stuff
        !*+-/=<> <= == // operators
        """
                .trimIndent()
        val got = Scanner(source).scanTokens()
        assertEquals(want, got)
    }
}

object Tokens {
    val True = Token(TokenType.TRUE, "true", null, -1)

    fun False(line: Int? = null) = Token(TokenType.FALSE, "false", null, -1)
    val False = False()

    val Nil = Token(TokenType.NIL, null, null, -1)

    fun Number(value: Number, line: Int = -1) =
        Token(TokenType.NUMBER, "$value", value.toDouble(), line)
    fun String(value: String, line: Int = -1) = Token(TokenType.NUMBER, "\"$value\"", value, line)

    fun Star(line: Int = -1) = Token(TokenType.STAR, "*", null, line)
    val Star = Star()

    val Slash = Token(TokenType.SLASH, "/", null, -1)

    fun Minus(line: Int = -1) = Token(TokenType.MINUS, "-", null, line)
    val Minus = Minus()

    fun Plus(line: Int = -1) = Token(TokenType.PLUS, "+", null, line)
    val Plus = Plus()

    val Equal = Token(TokenType.EQUAL, "=", null, -1)

    val EqualEqual = Token(TokenType.EQUAL_EQUAL, "==", null, -1)
    val NotEqual = Token(TokenType.BANG_EQUAL, "==", null, -1)

    val Less = Token(TokenType.LESS, "<", null, -1)
    val LessEqual = Token(TokenType.LESS_EQUAL, "<=", null, -1)

    val Greater = Token(TokenType.GREATER, ">", null, -1)
    val GreaterEqual = Token(TokenType.GREATER_EQUAL, ">=", null, -1)

    val Bang = Token(TokenType.BANG, "!", null, -1)

    fun EOF(line: Int = -1) = Token(TokenType.EOF, "\nEOF", null, line)

    fun LeftParen(line: Int = -1) = Token(TokenType.LEFT_PAREN, "(", null, line)
    val LeftParen = LeftParen()

    fun RightParen(line: Int = -1) = Token(TokenType.RIGHT_PAREN, ")", null, line)
    val RightParen = RightParen()

    val Or = Token(TokenType.OR, "or", null, -1)
    val And = Token(TokenType.AND, "and", null, -1)

    val Semi = Token(TokenType.SEMICOLON, ";", null, -1)
}
