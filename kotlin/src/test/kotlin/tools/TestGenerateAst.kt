package tools

import kotlin.test.Test
import kotlin.test.assertEquals

internal class GenerateAstTest {
    val script = GenerateAst(arrayOf())

    @Test
    fun testCreateBinaryExpr() {
        val want = """
            sealed class Expr {
            data class Binary(val left: Expr, val operator: Token, val right: Expr): Expr()
            }
        """.trimIndent()

        val got = script.createAstClass(
            "Expr", listOf(
                "Binary : Expr left, Token operator, Expr right"
            )
        )
        // todo: assert code compiles
        // todo^2: test compiled code

        assertEquals(want, got)
    }

    @Test
    fun testCreateExpr() {
        val want = """
            sealed class B {
            data class Foo(val f: F, val s: S, val t: T): B()
            data class Bar(val o: O, val t: T, val th: Th): B()
            }
        """.trimIndent()

        val got = script.createAstClass(
            "B", listOf(
                "Foo : F f, S s, T t",
                "Bar : O o, T t, Th th"
            )
        )

        assertEquals(want, got)
    }
}
