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
        val got = Parser(input).parseSingleExpression()
        println(got)
    }

    @Test
    fun testLol() {
        val want: Expr? = null
        val got = Parser(listOf()).parseSingleExpression()
        assertEquals(expressionToExpressionStatements(want), got)
    }

    @Test
    fun testParseLiteralEOF() {
        val got = Parser(listOf(Tokens.EOF())).parseSingleExpression()
        val want = Expr.Literal(Tokens.EOF())
        assertEquals(expressionToExpressionStatements(want), got)
    }

    @Test
    fun testParseLiteralNumber() {
        val got = Parser(listOf(Tokens.Number(123, 2), Tokens.Semi)).parseSingleExpression()
        val want = Expr.Literal(Tokens.Number(123, 2))
        assertEquals(expressionToExpressionStatements(want), got)
    }

    @Test
    fun testParens() {
        val got =
            Parser(listOf(Tokens.LeftParen, Tokens.Number(7), Tokens.RightParen, Tokens.Semi))
                .parseSingleExpression()
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
            val got = Parser(tokens + Tokens.Semi).parseSingleExpression()
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
            val got = Parser(tokens).parseSingleExpression()
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
            val got = Parser(tokens).parseSingleExpression()
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
            val got = Parser(tokens).parseSingleExpression()
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
            val got = Parser(tokens).parseSingleExpression()
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
            val got = Parser(tokens).parseSingleExpression()
            assertEquals(expressionToExpressionStatements(want), got)
        }
    }

    @Test
    fun testMultipleStatements() {
        val got = Parser(Scanner("123;\n456;").scanTokens()).parse()
        val want: List<Stmt> =
            listOf(
                Stmt.Expression(Expr.Literal(Tokens.Number(123, 0))),
                Stmt.Expression(Expr.Literal(Tokens.Number(456, 1)))
            )
        assertEquals(want, got)
    }

    @Test
    fun testPrintStatement() {
        val got = scanAndParseCode("print \"hello, world!\";")
        val want = listOf(Stmt.Print(Expr.Literal(Tokens.String("hello, world!"))))
        assertEquals(want, got)
    }

    @Test
    fun testForStatement() {
        val got = scanAndParseCode("for (var i = 0; i<10; i = i + 1) print i;")

        val i = Tokens.Identifier("i")
        val initializer = Stmt.Var(i, Expr.Literal(Tokens.Number(0)))

        val condition = Expr.Binary(Expr.Variable(i), Tokens.Less, Expr.Literal(Tokens.Number(10)))
        val printStatement = Stmt.Print(Expr.Variable(i))
        val increment =
            Stmt.Expression(
                Expr.Assign(
                    i,
                    Expr.Binary(Expr.Variable(i), Tokens.Plus(0), Expr.Literal(Tokens.Number(1, 0)))
                )
            )
        val whileStatement = Stmt.While(condition, Stmt.Block(listOf(printStatement, increment)))
        val want = listOf(Stmt.Block(listOf(initializer, whileStatement)))

        assertEquals(
            want.map { AstPrinter().printStatement(it) },
            got.map { AstPrinter().printStatement(it) }
        )
    }

    companion object {
        fun expressionToExpressionStatements(expr: Expr?): Expr? {
            return expr
        }
        fun scanAndParseCode(code: String): List<Stmt> {
            return Parser(Scanner(code).scanTokens()).parse()
        }
    }
}
