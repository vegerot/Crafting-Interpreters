package lox

class RuntimeError(public val failedAtToken: Token, message: String) : RuntimeException(message) {}
