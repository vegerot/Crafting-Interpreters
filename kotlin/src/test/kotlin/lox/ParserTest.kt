package lox

import kotlin.test.Ignore
import kotlin.test.Test
import kotlin.test.assertEquals

class ParserTest {
    @Test
    @Ignore
    fun kitchenSink() {
        // true or false and
        // (1-1)
        // != nil or "string" == "stringy"
        // and 2*2 == 4
        // and 4/2 = 2
        // or 3 + 6 = 9
        // and 6 < 9
        // or 69>42
        val input: List<Token> =
            listOf(
                Tokens.True,
                Tokens.Or,
                Tokens.False,
                Tokens.And,
                Tokens.LeftParen,
                Tokens.Number(1),
                Tokens.Minus,
                Tokens.Number(1),
                Tokens.RightParen,
                Tokens.NotEqual,
                Tokens.Nil,
                Tokens.Or,
                Tokens.String("string"),
                Tokens.EqualEqual,
                Tokens.String("stringy"),
                Tokens.And,
                Tokens.Number(2),
                Tokens.Star,
                Tokens.Number(2),
                Tokens.EqualEqual,
                Tokens.Number(4),
                Tokens.And,
                Tokens.Number(4),
                Tokens.Slash,
                Tokens.Number(2),
                Tokens.Equal,
                Tokens.Number(2),
                Tokens.Or,
                Tokens.Number(3),
                Tokens.Plus,
                Tokens.Number(6),
                Tokens.Equal,
                Tokens.Number(9),
                Tokens.And,
                Tokens.Number(6),
                Tokens.Less,
                Tokens.Number(9),
                Tokens.Or,
                Tokens.Number(69),
                Tokens.Greater,
                Tokens.Number(4)
            )
        val got = Parser(input).parse()
        println(got)
    }

    @Test
    fun testLol() {
        val want: Expr? = null
        val got: List<Stmt> = Parser(listOf()).parse()
        assertEquals(expressionToExpressionStatements(want), got)
    }

    @Test
    fun testParseLiteralEOF() {
        val got = Parser(listOf(Tokens.EOF())).parse()
        val want = Expr.Literal(Tokens.EOF())
        assertEquals(expressionToExpressionStatements(want), got)
    }

    @Test
    fun testParseLiteralNumber() {
        val got = Parser(listOf(Tokens.Number(123, 2))).parse()
        val want = Expr.Literal(Tokens.Number(123, 2))
        assertEquals(expressionToExpressionStatements(want), got)
    }

    @Test
    fun testParens() {
        val got = Parser(listOf(Tokens.LeftParen, Tokens.Number(7), Tokens.RightParen)).parse()
        val want = Expr.Grouping(Expr.Literal(Tokens.Number(7)))
        assertEquals(expressionToExpressionStatements(want), got)
    }

    @Test
    fun testSimple() {
        val tokensToExpr =
            mapOf(
                listOf(Tokens.True) to Expr.Literal(Tokens.True),
                listOf(Tokens.False) to Expr.Literal(Tokens.False),
                listOf(Tokens.Nil) to Expr.Literal(Tokens.Nil),
                listOf(Tokens.String("hello, world!")) to
                    Expr.Literal(Tokens.String("hello, world!"))
            )
        tokensToExpr.map { (tokens, want) ->
            val got = Parser(tokens).parse()
            assertEquals(expressionToExpressionStatements(want), got)
        }
    }

    @Test
    fun testUnary() {
        val tokensToExpr =
            mapOf(
                listOf(Tokens.Minus, Tokens.Number(1)) to
                    Expr.Unary(Tokens.Minus, Expr.Literal(Tokens.Number(1))),
                listOf(Tokens.Bang, Tokens.True) to
                    Expr.Unary(Tokens.Bang, Expr.Literal(Tokens.True)),
            )
        tokensToExpr.map { (tokens, want) ->
            val got = Parser(tokens).parse()
            assertEquals(expressionToExpressionStatements(want), got)
        }
    }

    @Test
    fun testFactor() {
        val tokensToExpr =
            mapOf(
                listOf(Tokens.Number(1), Tokens.Star, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Star,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.Slash, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Slash,
                        Expr.Literal(Tokens.Number(2))
                    ),
            )
        tokensToExpr.map { (tokens, want) ->
            val got = Parser(tokens).parse()
            assertEquals(expressionToExpressionStatements(want), got)
        }
    }

    @Test
    fun testTerm() {
        val tokensToExpr =
            mapOf(
                listOf(Tokens.Number(1), Tokens.Minus, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Minus,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.Plus, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Plus,
                        Expr.Literal(Tokens.Number(2))
                    ),
            )
        tokensToExpr.map { (tokens, want) ->
            val got = Parser(tokens).parse()
            assertEquals(expressionToExpressionStatements(want), got)
        }
    }

    @Test
    fun testComparison() {
        val tokensToExpr =
            mapOf(
                listOf(Tokens.Number(1), Tokens.Less, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Less,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.Greater, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Greater,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.LessEqual, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.LessEqual,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.GreaterEqual, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.GreaterEqual,
                        Expr.Literal(Tokens.Number(2))
                    ),
            )
        tokensToExpr.map { (tokens, want) ->
            val got = Parser(tokens).parse()
            assertEquals(expressionToExpressionStatements(want), got)
        }
    }

    @Test
    fun testEquality() {
        val tokensToExpr =
            mapOf(
                listOf(Tokens.Number(1), Tokens.Less, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Less,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.Greater, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.Greater,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.LessEqual, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.LessEqual,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.GreaterEqual, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.GreaterEqual,
                        Expr.Literal(Tokens.Number(2))
                    ),
                listOf(Tokens.Number(1), Tokens.EqualEqual, Tokens.Number(2)) to
                    Expr.Binary(
                        Expr.Literal(Tokens.Number(1)),
                        Tokens.EqualEqual,
                        Expr.Literal(Tokens.Number(2))
                    ),
            )
        tokensToExpr.map { (tokens, want) ->
            val got = Parser(tokens).parse()
            assertEquals(expressionToExpressionStatements(want), got)
        }
    }

    companion object {
        fun expressionToExpressionStatements(expr: Expr?): List<Stmt.Expression> {
            return when (expr) {
                null -> listOf()
                else -> listOf(Stmt.Expression(expr))
            }
        }
    }
}
