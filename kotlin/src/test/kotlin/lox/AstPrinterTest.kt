package lox

import kotlin.test.Test
import kotlin.test.assertEquals

internal class AstPrinterTest {
    @Test
    fun testPrinter() {
        val want = "(* (- 123) (45.67))"

        /**
         * -123*(45.67)
         * TODO: use string instead of manual tree
         */
        val expression = Expr.Binary(
            Expr.Unary(
                Token(TokenType.MINUS, "-", null, 1),
                Expr.Literal(123)),
            Token(TokenType.STAR, "*", null, 1),
            Expr.Grouping(
                Expr.Literal(45.67))
        )

        val got = AstPrinter().print(expression)
        assertEquals(want, got)
    }
}