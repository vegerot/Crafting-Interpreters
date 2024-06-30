package lox

import java.io.ByteArrayOutputStream
import java.io.PrintStream
import kotlin.test.*

class InterpreterTest {
    @Test
    fun lol() {
        val want = "7"
        val got =
            Interpreter()
                .interpretExpression(Parser(Scanner("7").scanTokens()).parseSingleExpression()!!)

        assertEquals(want, got)
    }

    @Test
    fun grouping() {
        val want = "69"
        val got =
            Interpreter()
                .interpretExpression(
                    Parser(Scanner("(69.0)").scanTokens()).parseSingleExpression()!!
                )

        assertEquals(want, got)
    }

    @Test
    fun unary() {
        val want = "-42"
        val got = parseAndInterpretSingleExpression("-42")
        assertEquals(want, got)
    }

    @Test
    fun binary() {
        val want = "4"
        val got = parseAndInterpretSingleExpression("2 *2")

        assertEquals(want, got)
    }

    @Test
    fun testNegatingNonNumberErrors() {
        val want = RuntimeError(Tokens.Minus(1), "Operand must be a number")

        // TODO: create `assertFailsWith`-like thing but takes an `Exception` as a parameter instead
        // of a `String`
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("-\"hello\"")
        }
    }

    @Test
    fun testNonNumberErrors() {
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("2>\"\"")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("2>=\"\"")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("\"a\"<3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("\"a\"<=3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("\"a\"-3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("\"a\"/3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("78 / \"a\"")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("78 * \"m\"")
        }
    }

    @Test
    fun testSubtractNonNumberError() {
        val want = RuntimeError(Tokens.Minus(1), "Operand must be a number")

        // TODO: create `assertFailsWith`-like thing but takes an `Exception` as a parameter instead
        // of a `String`
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretSingleExpression("\"hello\" - 2")
        }
    }

    @Test
    @Ignore
    fun testAddNonNumbersError() {
        // TODO: Write tests when we have variables
        assertFailsWith<RuntimeError>("Operands must be numbers") {
            // TODO: Add custom parsing errors
            parseAndInterpretSingleExpression("\"hello\" + 2")
        }
        assertFailsWith<RuntimeError>("Operands must be numbers") {
            parseAndInterpretSingleExpression("69 + \"hello\"")
        }
        assertFailsWith<RuntimeError>("Operands must be numbers") {
            parseAndInterpretSingleExpression("69 + \"hello\" + \"bye\" + 2")
        }
    }

    @Test
    fun testPrintExpression() {
        val buffer = ByteArrayOutputStream()
        val originalStdout = System.out
        try {

            System.setOut(PrintStream(buffer))

            parseAndInterpretCode("print \"lox is a language\";")

            val want = "lox is a language\n"
            assertEquals(want, buffer.toString())
        } finally {
            System.setOut(originalStdout)
        }
    }

    @Test
    fun testInterpret() {
        val codeToWant: Map<String, String> =
            mapOf(
                // literals
                "7" to "7",
                "\"lox\"" to "lox",
                "(69.0)" to "69",
                // unary
                "-42" to "-42",
                "!true" to "false",
                "!7" to "false",
                "!!\"hello\"" to "true",
                "!\"\"" to "false",
                // binary arithmetic
                "34 + 35" to "69",
                "3*6.5" to "19.5",
                "69 - 27" to "42",
                "120 /16" to "7.5",
                "\"he\" + \"llo\"" to "hello",
                // binary comparison
                "7 > 2" to "true",
                "9 < 3" to "false",
                "2 >= 2" to "true",
                "3 <= 2" to "false",
                // equality
                "1 == 2" to "false",
                "42 == 42" to "true",
                "2 != 2" to "false",
                "\"hi\" == \"hello\"" to "false",
                "\"hi\" != \"hello\"" to "true",
                "\"hi\" == \"hi\"" to "true",
                "\"hi\" != \"hi\"" to "false",
            )

        for ((code, want) in codeToWant) {
            try {
                val got = parseAndInterpretSingleExpression(code)
                if (got != want) {
                    assert(false) {
                        "expected parseAndInterpretSingleExpression($code) to be $want but got $got"
                    }
                } else {
                    assert(true)
                }
            } catch (e: Exception) {
                System.err.println(
                    "error thrown when evaluating assertEquals(parseAndInterpretSingleExpression($code), $want)"
                )
                throw e
            }
        }
    }

    fun parseAndInterpretSingleExpression(code: String): String {
        return Interpreter()
            .interpretExpression(Parser(Scanner(code).scanTokens()).parseSingleExpression()!!)
    }
    fun parseAndInterpretCode(code: String): Unit {
        Interpreter().interpret(Parser(Scanner(code).scanTokens()).parse())
    }
}
