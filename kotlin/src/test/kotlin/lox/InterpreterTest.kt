package lox

import kotlin.test.Ignore
import kotlin.test.Test
import kotlin.test.assertEquals

class InterpreterTest {
    @Test
    fun foo() {
        val want = "7.0"
        val got = Interpreter().interpret(Parser(Scanner("7").scanTokens()).parse()!!)

        assertEquals(want, got)
    }
}
