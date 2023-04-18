package lox

interface LoxCallable {
  fun call(interpreter: Interpreter, arguments: List<LoxValue>): LoxValue
}