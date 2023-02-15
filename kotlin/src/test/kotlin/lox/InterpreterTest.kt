package lox

import kotlin.test.Test
import kotlin.test.assertEquals

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
    fun testInterpret() {
        val codeToWant: Map<String, String> =
            mapOf(
                "7" to "7.0",
                "(69.0)" to "69.0",
                "-42" to "-42.0",
                "!true" to "false",
                "!7" to "false",
                "!!\"hello\"" to "true",
                "!\"\"" to "false",
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
