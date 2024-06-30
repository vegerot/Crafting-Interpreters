package lox

import java.io.File
import java.lang.Exception
import java.nio.charset.Charset
import kotlin.system.exitProcess

class Lox {
    companion object {
        private val interpreter: Interpreter = Interpreter()

        fun runFile(file: String) {
            val bytes = File(file).readBytes()
            val string = bytes.toString(charset = Charset.defaultCharset())
            runCode(string)

            if (hadError) exitProcess(65)
            if (hadRuntimeError) exitProcess(70)
        }

        fun runPrompt() {
            while (true) {
                print("> ")
                val line: String =
                    try {
                        readln()
                    } catch (e: RuntimeException) {
                        println(e)
                        null
                    } // `readln` doesn't return a more specific error??
                     ?: break
                runCode(line)
                hadError = false
            }
        }

        fun runCode(source: String) {
            val scanner = Scanner(source)
            val tokens: List<Token> = scanner.scanTokens()
            val parser = Parser(tokens)
            val statements: List<Stmt> =
                try {
                    parser.parse()!!
                } catch (error: Exception) {
                    // All the logging is done before this exception is thrown
                    // TODO: make `parse()` not throw an error
                    return
                }
            if (hadError) return

            try {
                interpreter.interpret(statements)
            } catch (error: RuntimeError) {
                runtimeError(error)
            }
        }

        private var hadError: Boolean = false

        fun error(line: Int, where: String?, message: String) {
            reportError(line, where ?: "", message)
            this.hadError = true
        }

        fun reportError(line: Int, where: String, message: String) {
            System.err.println("[line $line] Error $where: $message")
        }

        fun error(token: Token, message: String) {
            return if (token.type == TokenType.EOF) {
                reportError(token.line, "at end", message)
            } else {
                reportError(token.line, "at '${token.lexeme}'", message)
            }
        }

        private var hadRuntimeError: Boolean = false

        fun runtimeError(error: RuntimeError) {
            System.err.println("${error.message}\n[line ${error.failedAtToken.line}]")
            this.hadRuntimeError = true
        }
    }
}

fun main(args: Array<String>) {
    if (args.size > 2) {
        println("Usage: jlox [script]")
        exitProcess(64)
    } else if (args.size == 2) {
        if (args[0] != "--fmt") {
            println("Usage: jlox [script]")
            exitProcess(64)
        } else {
            println("TODO:")
        }
    } else if (args.size == 1) {
        Lox.runFile(args[0])
    } else {
        Lox.runPrompt()
    }
}

typealias LoxValue = Any
