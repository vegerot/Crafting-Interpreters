import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    kotlin("jvm") version "1.7.21"
    application
    id("com.ncorti.ktfmt.gradle") version "0.11.0"
}

group = "org.example"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation(kotlin("test"))
    implementation("org.jetbrains.kotlin:kotlin-script-runtime")
}

tasks.test {
    useJUnitPlatform()
    this.testLogging {
        this.showStandardStreams = true
    }
}

tasks.withType<KotlinCompile> {
    kotlinOptions.jvmTarget = "17"
}

application {
    mainClass.set("lox.LoxKt")
}

tasks.getByName("run", JavaExec::class) {
    standardInput = System.`in`
}

ktfmt {kotlinLangStyle()}