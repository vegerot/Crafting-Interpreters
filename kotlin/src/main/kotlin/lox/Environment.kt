package lox

class Environment() {
    private val values: MutableMap<String, LoxValue?> = mutableMapOf()

    fun define(name: String, value: LoxValue?) {
        values[name] = value
    }

    fun get(name: Token): LoxValue? {
        return values.getOrElse(name.lexeme!!) {
            throw RuntimeError(name, "Undefined variable '${name.lexeme}'.")
        }
    }

    fun assign(name: Token, value: LoxValue) {
        if (values.containsKey(name.lexeme)) {
            values[name.lexeme!!] = value
            return
        }
        throw RuntimeError(name, "Undefined variable '${name.lexeme}'")
    }
}
