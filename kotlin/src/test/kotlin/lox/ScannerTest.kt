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
        val want: List<Token> = listOf(Token(TokenType.LEFT_PAREN, null, null, 0), Token(TokenType.EOF, null, null, 0))
        val got = Scanner("(").scanTokens()
        assertEquals(want, got)
    }

    @Test
    fun testTwoTokens() {
        val want: List<Token> = listOf(
            Token(TokenType.LEFT_PAREN, null, null, 0),
            Token(TokenType.RIGHT_PAREN, null, null, 0),
            Token(TokenType.EOF, null, null, 0)
        )
        val got = Scanner("()").scanTokens()
        assertEquals(want, got)
    }

    @Test
    fun kitchenSink() {
        val sourceToTokens = mapOf(
            "" to listOf(), "//" to listOf(), "// aoeuaoeu aoeu" to listOf(),
            """
            // this is a comment
            (
            """.trimIndent() to listOf(Token(TokenType.LEFT_PAREN, null, null, 1)),
            "{} // bye" to listOf(
                Token(TokenType.LEFT_BRACE, null, null, 0),
                Token(TokenType.RIGHT_BRACE, null, null, 0),
            ),
            "{}" to listOf(
                Token(TokenType.LEFT_BRACE, null, null, 0),
                Token(TokenType.RIGHT_BRACE, null, null, 0),
            ),
            """
            (
            )
            """.trimIndent() to listOf(
                Token(TokenType.LEFT_PAREN, null, null, 0),
                Token(TokenType.RIGHT_PAREN, null, null, 1),
            ),
            "\"hi\"" to listOf(
                Token(TokenType.STRING, "\"hi\"", "hi", 0)
            ),
            "\"\"" to listOf(
                Token(TokenType.STRING, "\"\"", "", 0),
            ),
            """
            "su
            p"
            """.trimIndent() to listOf(
                Token(TokenType.STRING, "\"su\np\"", "su\np", 0),
            ),
            "1" to listOf(
                Token(TokenType.NUMBER, "1", 1.0, 0),
            ),
            "42.69" to listOf(
                Token(TokenType.NUMBER, "42.69", 42.69, 0),
            ),
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
        val want = listOf(
            Token(TokenType.LEFT_PAREN, null, null, 1),
            Token(TokenType.LEFT_PAREN, null, null, 1),
            Token(TokenType.RIGHT_PAREN, null, null, 1),
            Token(TokenType.RIGHT_PAREN, null, null, 1),
            Token(TokenType.LEFT_BRACE, null, null, 1),
            Token(TokenType.RIGHT_BRACE, null, null, 1),
            Token(TokenType.BANG, null, null, 2),
            Token(TokenType.STAR, null, null, 2),
            Token(TokenType.PLUS, null, null, 2),
            Token(TokenType.MINUS, null, null, 2),
            Token(TokenType.SLASH, null, null, 2),
            Token(TokenType.EQUAL, null, null, 2),
            Token(TokenType.LESS, null, null, 2),
            Token(TokenType.GREATER, null, null, 2),
            Token(TokenType.LESS_EQUAL, null, null, 2),
            Token(TokenType.EQUAL_EQUAL, null, null, 2),
            Token(TokenType.EOF, null, null, 2)
        )
        val source = """
        // this is a comment
        (( )){} // grouping stuff
        !*+-/=<> <= == // operators
        """.trimIndent()
        val got = Scanner(source).scanTokens()
        assertEquals(want, got)
    }
}
