package tools

import kotlin.test.Test
import kotlin.test.assertEquals

internal class GenerateAstTest {
    val script = GenerateAst(arrayOf("--no-write"))

    @Test
    fun testCreateBinaryExpr() {
        val want =
            """
            sealed class Expr {
            interface Visitor<R> {
            fun visitBinaryExpr(expr: Binary): R
            }

            abstract fun<R> accept(visitor: Visitor<R>): R
            data class Binary(val left: Expr, val operator: Token, val right: Expr): Expr() {
                override fun<R> accept(visitor: Visitor<R>): R {
                    return visitor.visitBinaryExpr(this)
                    }
                }
            }
        """
                .trimIndent()

        val got =
            script.createAstClass("Expr", listOf("Binary : Expr left, Token operator, Expr right"))
        // todo: assert code compiles
        // todo^2: test compiled code

        assertEquals(want, got)
    }

    @Test
    fun testCreateExpr() {
        val want =
            """
            sealed class B {
                    interface Visitor<R> {
                    fun visitFooB(b: Foo): R
            fun visitBarB(b: Bar): R
                    }

            abstract fun<R> accept(visitor: Visitor<R>): R
            data class Foo(val f: F, val s: S, val t: T): B() {
                override fun<R> accept(visitor: Visitor<R>): R {
                    return visitor.visitFooB(this)
                    }
                }
            data class Bar(val o: O, val t: T, val th: Th): B() {
                override fun<R> accept(visitor: Visitor<R>): R {
                    return visitor.visitBarB(this)
                    }
                }
            }
        """
                .trimIndent()

        val got = script.createAstClass("B", listOf("Foo : F f, S s, T t", "Bar : O o, T t, Th th"))

        assertEquals(want, got)
    }
}
