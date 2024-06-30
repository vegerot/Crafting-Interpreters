package lox

import kotlin.test.Test
import kotlin.test.assertEquals
import lox.ParserTest.Companion.scanAndParseCode

internal class AstPrinterTest {
    @Test
    fun testPrinter() {
        val want = "(* (- 123) (45.67))"

        /** -123*(45.67) TODO: use string instead of manual tree */
        val expression =
            Expr.Binary(
                Expr.Unary(
                    Token(TokenType.MINUS, "-", null, 1),
                    Expr.Literal(Token(TokenType.NUMBER, null, 123, 1))
                ),
                Token(TokenType.STAR, "*", null, 1),
                Expr.Grouping(Expr.Literal(Token(TokenType.NUMBER, null, 45.67, 1)))
            )

        val got = AstPrinter().print(expression)
        assertEquals(want, got)
    }

    @Test
    fun testPrinter2() {
        val code = scanAndParseCode("for (var i = 0; i<10; i = i + 1) print i;")

        val got = code.map { AstPrinter().printStatement(it) }

        val want =
            listOf(
                "(block [(var  i = 0.0), (while  (< var(i) 10.0) (block [(print var(i)), (; var i = Binary(left=Variable(name=Token(type=IDENTIFIER, lexeme=i, literal=null, line=0)), operator=Token(type=PLUS, lexeme=+, literal=null, line=0), right=Literal(value=Token(type=NUMBER, lexeme=1, literal=1.0, line=0))))] ))] )"
            )
        assertEquals(want, got)
    }
}
