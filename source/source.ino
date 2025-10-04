#include <Arduino.h>


// PINES DE MOTORES 
const int RIGHT_MOTOR_PIN1 = 7;    // Control dirección motor derecho A
const int RIGHT_MOTOR_PIN2 = 6;    // Control dirección motor derecho B
const int LEFT_MOTOR_PIN1 = 8;     // Control dirección motor izquierdo A
const int LEFT_MOTOR_PIN2 = 9;     // Control dirección motor izquierdo B


void setup() {
    // Inicializar Serial para pruebas
    Serial.begin(9600);
    
    // Configurar pines de motores como salidas
    pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
    pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
    pinMode(LEFT_MOTOR_PIN1, OUTPUT);
    pinMode(LEFT_MOTOR_PIN2, OUTPUT);
    
    // Asegurar que todo empiece apagado
    stopAll();
    
    // Mensaje de inicio
    Serial.println("========================================");
    Serial.println("    DIAGNÓSTICO DE MOTORES 2WD");
    Serial.println("========================================");
    Serial.println("Comandos de prueba:");
    Serial.println("  1 = Probar SOLO motor DERECHO adelante");
    Serial.println("  2 = Probar SOLO motor DERECHO atrás");
    Serial.println("  3 = Probar SOLO motor IZQUIERDO adelante");
    Serial.println("  4 = Probar SOLO motor IZQUIERDO atrás");
    Serial.println("  5 = Probar AMBOS motores adelante");
    Serial.println("  6 = Probar AMBOS motores atrás");
    Serial.println("  7 = Test automático secuencial (3 seg c/u)");
    Serial.println("  0 = DETENER todo");
    Serial.println("========================================");
    Serial.println("Listo para recibir comandos...\n");
}

// ============================================
// BUCLE PRINCIPAL
// ============================================
void loop() {
    if (Serial.available() > 0) {
        char command = Serial.read();
        
        // Limpiar el buffer serial
        while(Serial.available() > 0) {
            Serial.read();
        }
        
        processTestCommand(command);
    }
}


// PROCESAMIENTO DE COMANDOS DE PRUEBA
void processTestCommand(char command) {
    Serial.println("----------------------------------------");
    
    switch(command) {
        case '1':
            Serial.println("TEST: Motor DERECHO - ADELANTE");
            testRightMotorForward();
            break;
            
        case '2':
            Serial.println("TEST: Motor DERECHO - ATRÁS");
            testRightMotorBackward();
            break;
            
        case '3':
            Serial.println("TEST: Motor IZQUIERDO - ADELANTE");
            testLeftMotorForward();
            break;
            
        case '4':
            Serial.println("TEST: Motor IZQUIERDO - ATRÁS");
            testLeftMotorBackward();
            break;
            
        case '5':
            Serial.println("TEST: AMBOS motores - ADELANTE");
            testBothMotorsForward();
            break;
            
        case '6':
            Serial.println("TEST: AMBOS motores - ATRÁS");
            testBothMotorsBackward();
            break;
            
        case '7':
            runAutomaticTest();
            break;
            
        case '0':
            Serial.println("DETENIENDO todos los motores");
            stopAll();
            break;
            
        default:
            Serial.println("Comando no reconocido. Use 1-7 o 0");
            break;
    }
    
    // Mostrar estado actual de los pines
    printPinStatus();
}


// FUNCIONES DE PRUEBA INDIVIDUAL
void testRightMotorForward() {
    stopAll();  // Primero parar motores
    digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    Serial.println("Motor derecho girando ADELANTE");
}

void testRightMotorBackward() {
    stopAll();
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
    Serial.println("Motor derecho girando ATRÁS");
}

void testLeftMotorForward() {
    stopAll();
    digitalWrite(LEFT_MOTOR_PIN1, HIGH);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
    Serial.println("Motor izquierdo girando ADELANTE");
}

void testLeftMotorBackward() {
    stopAll();
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, HIGH);
    Serial.println("Motor izquierdo girando ATRÁS");
}

void testBothMotorsForward() {
    digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, HIGH);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
    Serial.println("AMBOS motores girando ADELANTE");
}

void testBothMotorsBackward() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, HIGH);
    Serial.println("AMBOS motores girando ATRÁS");
}

void stopAll() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
}


// TEST AUTOMATICO SECUENCIAL
void runAutomaticTest() {
    Serial.println("INICIANDO TEST AUTOMÁTICO (3 seg cada prueba)");
    Serial.println("========================================");
    
    // Test 1: Motor derecho adelante
    Serial.println("\n[1/8] Motor DERECHO - ADELANTE (3 seg)");
    testRightMotorForward();
    delay(3000);
    
    // Test 2: Motor derecho atrás
    Serial.println("\n[2/8] Motor DERECHO - ATRÁS (3 seg)");
    testRightMotorBackward();
    delay(3000);
    
    // Pausa
    Serial.println("\n[3/8] PAUSA (2 seg)");
    stopAll();
    delay(2000);
    
    // Test 3: Motor izquierdo adelante
    Serial.println("\n[4/8] Motor IZQUIERDO - ADELANTE (3 seg)");
    testLeftMotorForward();
    delay(3000);
    
    // Test 4: Motor izquierdo atrás
    Serial.println("\n[5/8] Motor IZQUIERDO - ATRÁS (3 seg)");
    testLeftMotorBackward();
    delay(3000);
    
    // Pausa
    Serial.println("\n[6/8] PAUSA (2 seg)");
    stopAll();
    delay(2000);
    
    // Test 5: Ambos adelante
    Serial.println("\n[7/8] AMBOS motores - ADELANTE (3 seg)");
    testBothMotorsForward();
    delay(3000);
    
    // Test 6: Ambos atrás
    Serial.println("\n[8/8] AMBOS motores - ATRÁS (3 seg)");
    testBothMotorsBackward();
    delay(3000);
    
    // Finalizar
    stopAll();
    Serial.println("\n========================================");
    Serial.println("TEST AUTOMÁTICO COMPLETADO");
    Serial.println("========================================\n");
}

// ============================================
// FUNCIÓN DE DIAGNÓSTICO - Mostrar estado de pines
// ============================================
void printPinStatus() {
    Serial.println("\nEstado actual de los pines:");
    Serial.print("  Motor DERECHO:  Pin1(");
    Serial.print(RIGHT_MOTOR_PIN1);
    Serial.print(")=");
    Serial.print(digitalRead(RIGHT_MOTOR_PIN1) ? "HIGH" : "LOW");
    Serial.print("  Pin2(");
    Serial.print(RIGHT_MOTOR_PIN2);
    Serial.print(")=");
    Serial.println(digitalRead(RIGHT_MOTOR_PIN2) ? "HIGH" : "LOW");
    
    Serial.print("  Motor IZQUIERDO: Pin1(");
    Serial.print(LEFT_MOTOR_PIN1);
    Serial.print(")=");
    Serial.print(digitalRead(LEFT_MOTOR_PIN1) ? "HIGH" : "LOW");
    Serial.print("  Pin2(");
    Serial.print(LEFT_MOTOR_PIN2);
    Serial.print(")=");
    Serial.println(digitalRead(LEFT_MOTOR_PIN2) ? "HIGH" : "LOW");
    Serial.println("----------------------------------------\n");
}