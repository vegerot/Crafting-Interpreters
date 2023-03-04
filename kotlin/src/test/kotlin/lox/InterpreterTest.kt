package lox

import kotlin.test.*

class InterpreterTest {
    @Test
    fun lol() {
        val want = "7.0"
        val got = Interpreter().interpret(Parser(Scanner("7").scanTokens()).parse()!!)

        assertEquals(want, got)
    }

    @Test
    fun grouping() {
        val want = "69.0"
        val got = Interpreter().interpret(Parser(Scanner("(69.0)").scanTokens()).parse()!!)

        assertEquals(want, got)
    }

    @Test
    fun unary() {
        val want = "-42.0"
        val got = parseAndInterpretCode("-42")

        assertEquals(want, got)
    }

    @Test
    fun binary() {
        val want = "4.0"
        val got = parseAndInterpretCode("2 *2")

        assertEquals(want, got)
    }

    @Test
    fun testNegatingNonNumberErrors() {
        val want = RuntimeError(Tokens.Minus(1), "Operand must be a number")

        // TODO: create `assertFailsWith`-like thing but takes an `Exception` as a parameter instead
        // of a `String`
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("-\"hello\"")
        }
    }

    @Test
    fun testNonNumberErrors() {
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("2>\"\"")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("2>=\"\"")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("\"a\"<3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("\"a\"<=3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("\"a\"-3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("\"a\"/3")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("78 / \"a\"")
        }
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("78 * \"m\"")
        }
    }

    @Test
    fun testSubtractNonNumberError() {
        val want = RuntimeError(Tokens.Minus(1), "Operand must be a number")

        // TODO: create `assertFailsWith`-like thing but takes an `Exception` as a parameter instead
        // of a `String`
        assertFailsWith<RuntimeError>("Operand must be a number") {
            parseAndInterpretCode("\"hello\" - 2")
        }
    }

    @Test
    @Ignore
    fun testAddNonNumbersError() {
        // TODO: Write tests when we have variables
        assertFailsWith<RuntimeError>("Operands must be numbers") {
            // TODO: Add custom parsing errors
            parseAndInterpretCode("\"hello\" + 2")
        }
        assertFailsWith<RuntimeError>("Operands must be numbers") {
            parseAndInterpretCode("69 + \"hello\"")
        }
        assertFailsWith<RuntimeError>("Operands must be numbers") {
            parseAndInterpretCode("69 + \"hello\" + \"bye\" + 2")
        }
    }

    @Test
    fun testInterpret() {
        val codeToWant: Map<String, String> =
            mapOf(
                // literals
                "7" to "7.0",
                "\"lox\"" to "lox",
                "(69.0)" to "69.0",
                // unary
                "-42" to "-42.0",
                "!true" to "false",
                "!7" to "false",
                "!!\"hello\"" to "true",
                "!\"\"" to "false",
                // binary arithmetic
                "34 + 35" to "69.0",
                "3*6.5" to "19.5",
                "69 - 27" to "42.0",
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
                val got = parseAndInterpretCode(code)
                if (got != want) {
                    assert(false) {
                        "expected parseAndInterpretCode($code) to be $want but got $got"
                    }
                } else {
                    assert(true)
                }
            } catch (e: Exception) {
                System.err.println(
                    "error thrown when evaluating assertEquals(parseAndInterpretCode($code), $want)"
                )
                throw e
            }
        }
    }

    fun parseAndInterpretCode(code: String): String {
        return Interpreter().interpret(Parser(Scanner(code).scanTokens()).parse()!!)
    }
}
