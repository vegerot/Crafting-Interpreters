package lox

class Environment(private val enclosing: Environment? = null) {
    private val values: MutableMap<String, LoxValue?> = mutableMapOf()

    fun define(name: String, value: LoxValue?) {
        values[name] = value
    }

    fun get(name: Token): LoxValue? {
        return values.getOrElse(name.lexeme!!) {
            if (enclosing != null) return enclosing.get(name)
            throw RuntimeError(name, "Undefined variable '${name.lexeme}'.")
        }
    }

    fun assign(name: Token, value: LoxValue) {
        if (values.containsKey(name.lexeme)) {
            values[name.lexeme!!] = value
            return
        } else if (enclosing != null) return enclosing.assign(name, value)
        throw RuntimeError(name, "Undefined variable '${name.lexeme}'")
    }
}
