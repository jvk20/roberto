#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>


// CONFIGURACIÓN DE COMUNICACIÓN - Communication Setup
const int BT_RX_PIN = 10;          // Pin RX del Bluetooth (conectar a TX del HC-06)
const int BT_TX_PIN = 11;          // Pin TX del Bluetooth (conectar a RX del HC-06)
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);


// CONFIGURACIÓN DE PINES - Pin Configuration


// Pines del Puente H (L298N) - H-Bridge Pins
const int RIGHT_MOTOR_PIN1 = 7;    // Control dirección motor derecho A
const int RIGHT_MOTOR_PIN2 = 6;    // Control dirección motor derecho B
const int LEFT_MOTOR_PIN1 = 8;     // Control dirección motor izquierdo A
const int LEFT_MOTOR_PIN2 = 9;     // Control dirección motor izquierdo B

// Pines de accesorios - Accessories Pins
const int BUZZER_PIN = 2;          // Pin del buzzer/bocina
const int HEADLIGHT_PIN = 3;       // Pin LED luces delanteras
const int TAILLIGHT_PIN = 4;       // Pin LED luces traseras

// Pines para modo autonomo - Autonomous mode pins
const int SERVO_PIN = 5;           // Servo para sensor ultrasonido
const int ULTRASONIC_TRIGGER = 12; // Trigger del sensor HC-SR04
const int ULTRASONIC_ECHO = 13;    // Echo del sensor HC-SR04


// OBJETOS Y VARIABLES GLOBALES - Global Objects
Servo radarServo;                  // Servo para el "radar"


// CONFIGURACIÓN DE VELOCIDAD - Speed Settings
int currentSpeed = 255;            // Velocidad actual (0-255)
const int SPEED_LEVEL_1 = 65;      // 25% velocidad
const int SPEED_LEVEL_2 = 130;     // 50% velocidad
const int SPEED_LEVEL_3 = 195;     // 75% velocidad
const int SPEED_LEVEL_4 = 255;     // 100% velocidad


// CONFIGURACIÓN MODO AUTÓNOMO - Autonomous Config
// Distancias de seguridad (en centímetros)
const int DISTANCE_DANGER = 15;    // Zona crítica - parar inmediatamente
const int DISTANCE_WARNING = 30;   // Zona de advertencia - buscar alternativa
const int DISTANCE_SAFE = 50;      // Zona segura - continuar

// Ángulos de escaneo del servo
const int ANGLE_LEFT = 150;        // Mirar a la izquierda
const int ANGLE_CENTER = 90;       // Mirar al frente
const int ANGLE_RIGHT = 30;        // Mirar a la derecha

// Variables para almacenar mediciones
int distanceLeft = 0;
int distanceCenter = 0;
int distanceRight = 0;


// Variable tipos de funcionamiento roberto
enum SystemMode {
    MODE_MANUAL,
    MODE_AUTONOMOUS
};

SystemMode currentMode = MODE_MANUAL;  // Iniciar en modo manual
bool debugMode = true;                 // Mensajes de depuración
unsigned long lastScanTime = 0;        // Timestamp del último escaneo
const int SCAN_INTERVAL = 2000;        // Escanear cada 2 segundos en modo auto

void setup() {
    // Inicializar Serial USB (para pruebas)
    Serial.begin(9600);
    
    // Inicializar SoftwareSerial para HC-06
    BTSerial.begin(9600);
    
    // Configurar pines del puente H como salidas
    pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
    pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
    pinMode(LEFT_MOTOR_PIN1, OUTPUT);
    pinMode(LEFT_MOTOR_PIN2, OUTPUT);
    
    // Configurar pines de accesorios como salidas
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(HEADLIGHT_PIN, OUTPUT);
    pinMode(TAILLIGHT_PIN, OUTPUT);
    
    // Configurar pines del ultrasonido
    pinMode(ULTRASONIC_TRIGGER, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);
    
    // Inicializar servo en posición central
    radarServo.attach(SERVO_PIN);
    radarServo.write(ANGLE_CENTER);
    delay(500);
    
    // Estado inicial: todo apagado
    stopMotors();
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(HEADLIGHT_PIN, LOW);
    digitalWrite(TAILLIGHT_PIN, LOW);   

}

void loop() {
    // Verificar comandos desde Bluetooth (HC-06)
    if (BTSerial.available() > 0) {
        char command = BTSerial.read();
        
        if (debugMode) {
            Serial.print("[BT→CMD]: '");
            Serial.print(command);
            Serial.print("' (ASCII: ");
            Serial.print((int)command);
            Serial.println(")");
        }
        
        processCommand(command);
    }
    
    // Verificar comandos desde USB (para pruebas)
    if (Serial.available() > 0) {
        char command = Serial.read();
        
        BTSerial.write(command);
        
        if (debugMode) {
            Serial.print("[USB→CMD]: '");
            Serial.print(command);
            Serial.println("'");
        }
        
        processCommand(command);
    }
    
    // Ejecutar logica si esta en modo automatico
    if (currentMode == MODE_AUTONOMOUS) {
        runAutonomousMode();
    }
}

// ============================================
// MODO AUTÓNOMO - Autonomous Mode
// ============================================
void runAutonomousMode() {
    // Verificar si es tiempo de escanear
    unsigned long currentTime = millis();
    
    if (currentTime - lastScanTime >= SCAN_INTERVAL) {
        lastScanTime = currentTime;
        
        // Realizar escaneo completo
        performRadarScan();
        
        // Tomar decision basada en escaneo
        executeAutonomousDecision();
    }
}

void performRadarScan() {
    if (debugMode) {
        Serial.println("\n[AUTO] Iniciando escaneo radar...");
    }
    
    // 1. Escanear IZQUIERDA
    radarServo.write(ANGLE_LEFT);
    delay(300); // Tiempo de estabilización
    distanceLeft = measureDistance();
    
    // 2. Escanear CENTRO
    radarServo.write(ANGLE_CENTER);
    delay(300);
    distanceCenter = measureDistance();
    
    // 3. Escanear DERECHA
    radarServo.write(ANGLE_RIGHT);
    delay(300);
    distanceRight = measureDistance();
    
    // Volver al centro
    radarServo.write(ANGLE_CENTER);
    
    if (debugMode) {
        Serial.print("[AUTO] Distancias -> L:");
        Serial.print(distanceLeft);
        Serial.print("cm C:");
        Serial.print(distanceCenter);
        Serial.print("cm R:");
        Serial.print(distanceRight);
        Serial.println("cm");
    }
}

int measureDistance() {
    // Limpiar el trigger
    digitalWrite(ULTRASONIC_TRIGGER, LOW);
    delayMicroseconds(2);
    
    // Enviar pulso de 10us (microsegundos)
    digitalWrite(ULTRASONIC_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIGGER, LOW);
    
    // Leer el tiempo de respuesta (timeout 30 milisegundos)
    long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000);
    
    // Calcular distancia en cm; 0.0343 = La velocidad del sonido en el aire, expresada en centimetros por microsegundo (cm/µs)
    int distance = duration * 0.0343 / 2;
    
    // Validar lectura
    if (distance == 0 || distance > 400) {
        return 999; // Sin obstaculo detectado
    }
    
    return distance;
}

void executeAutonomousDecision() {
    // CASO 1: PELIGRO INMEDIATO al frente
    if (distanceCenter < DISTANCE_DANGER) {
        if (debugMode) {
            Serial.println("[AUTO] PELIGRO! Obstaculo critico");
        }
        
        stopMotors();
        delay(300);
        
        // Retroceder un poco
        moveBackward();
        delay(500);
        
        // Girar hacia el lado con mas espacio
        if (distanceLeft > distanceRight) {
            turnLeft();
            if (debugMode) Serial.println("[AUTO] Girando izquierda fuerte");
        } else {
            turnRight();
            if (debugMode) Serial.println("[AUTO] Girando derecha fuerte");
        }
        delay(800);
        stopMotors();
        
        return;
    }
    
    // CASO 2: ADVERTENCIA al frente
    if (distanceCenter < DISTANCE_WARNING) {
        if (debugMode) {
            Serial.println("[AUTO] Advertencia - Buscando ruta");
        }
        
        // Buscar mejor direccion
        if (distanceLeft > DISTANCE_SAFE) {
            moveForwardLeft();
            if (debugMode) Serial.println("[AUTO] Avanzando por izquierda");
            delay(600);
        } else if (distanceRight > DISTANCE_SAFE) {
            moveForwardRight();
            if (debugMode) Serial.println("[AUTO] Avanzando por derecha");
            delay(600);
        } else {
            // Ambos lados bloqueados - retroceder
            moveBackward();
            if (debugMode) Serial.println("[AUTO] Retrocediendo");
            delay(500);
            stopMotors();
        }
        
        return;
    }
    
    // CASO 3: Camino despejado
    if (distanceCenter > DISTANCE_SAFE) {
        // Evaluar si hay mas espacio a un lado
        if (distanceLeft > distanceCenter + 20) {
            moveForwardLeft();
            if (debugMode) Serial.println("[AUTO] Avanzando suave izquierda");
        } else if (distanceRight > distanceCenter + 20) {
            moveForwardRight();
            if (debugMode) Serial.println("[AUTO] Avanzando suave derecha");
        } else {
            moveForward();
            if (debugMode) Serial.println("[AUTO] Avanzando recto");
        }
        delay(800);
        
        return;
    }
    
    // CASO 4: Situacion de precaución
    moveForward();
    if (debugMode) Serial.println("[AUTO] Avance con precaucion");
    delay(400);
}

// PROCESAMIENTO DE COMANDOS - Command Processing
void processCommand(char command) {
    switch(command) {
        // ===== CONTROL DE MODO =====
        case 'Z':   // Activar modo AUTÓNOMO
            activateAutonomousMode();
            break;
            
        case '1':   // Activar modo MANUAL
            activateManualMode();
            break;
            
        // ===== MOVIMIENTOS (solo en modo MANUAL) =====
        case 'F':   // Avanzar - Forward
            if (currentMode == MODE_MANUAL) {
                moveForward();
                sendFeedback("Avanzando");
            }
            break;
            
        case 'B':   // Retroceder - Backward
            if (currentMode == MODE_MANUAL) {
                moveBackward();
                sendFeedback("Retrocediendo");
            }
            break;
            
        case 'L':   // Girar izquierda - Turn Left
            if (currentMode == MODE_MANUAL) {
                turnLeft();
                sendFeedback("Girando izquierda");
            }
            break;
            
        case 'R':   // Girar derecha - Turn Right
            if (currentMode == MODE_MANUAL) {
                turnRight();
                sendFeedback("Girando derecha");
            }
            break;
            
        case 'G':   // Adelante-Izquierda - Forward-Left
            if (currentMode == MODE_MANUAL) {
                moveForwardLeft();
                sendFeedback("Adelante-Izquierda");
            }
            break;
            
        case 'H':   // Adelante-Derecha - Forward-Right
            if (currentMode == MODE_MANUAL) {
                moveForwardRight();
                sendFeedback("Adelante-Derecha");
            }
            break;
            
        case 'I':   // Atrás-Izquierda - Backward-Left
            if (currentMode == MODE_MANUAL) {
                moveBackwardLeft();
                sendFeedback("Atras-Izquierda");
            }
            break;
            
        case 'J':   // Atrás-Derecha - Backward-Right
            if (currentMode == MODE_MANUAL) {
                moveBackwardRight();
                sendFeedback("Atras-Derecha");
            }
            break;
            
        case 'S':   // Detener - Stop
            stopMotors();
            sendFeedback("Detenido");
            break;
            
        // ===== ACCESORIOS (funcionan en ambos modos) =====
        case 'Y':   // Bocina - Horn
            activateHorn();
            sendFeedback("Bocina");
            break;
            
        case 'U':   // Encender faros - Headlights ON
            digitalWrite(HEADLIGHT_PIN, HIGH);
            sendFeedback("Faros ON");
            break;
            
        case 'u':   // Apagar faros - Headlights OFF
            digitalWrite(HEADLIGHT_PIN, LOW);
            sendFeedback("Faros OFF");
            break;
            
        case 'V':   // Encender luz trasera - Taillight ON
            digitalWrite(TAILLIGHT_PIN, HIGH);
            sendFeedback("Luz trasera ON");
            break;
            
        case 'v':   // Apagar luz trasera - Taillight OFF
            digitalWrite(TAILLIGHT_PIN, LOW);
            sendFeedback("Luz trasera OFF");
            break;
            
        case 'W':   // Todas las luces ON
            digitalWrite(HEADLIGHT_PIN, HIGH);
            digitalWrite(TAILLIGHT_PIN, HIGH);
            sendFeedback("Luces ON");
            break;
            
        case 'w':   // Todas las luces OFF
            digitalWrite(HEADLIGHT_PIN, LOW);
            digitalWrite(TAILLIGHT_PIN, LOW);
            sendFeedback("Luces OFF");
            break;
            
        case 'X':   // Luces intermitentes
            blinkTaillight();
            break;
            
        // ===== COMANDOS ESPECIALES =====
        case 'D':   // Toggle debug mode
            debugMode = !debugMode;
            sendFeedback(debugMode ? "Debug ON" : "Debug OFF");
            break;
            
        case 'T':   // Test manual de escaneo (solo en manual)
            if (currentMode == MODE_MANUAL) {
                performRadarScan();
                displayScanResults();
            }
            break;
            
        default:
            if (debugMode) {
                Serial.print("[WARNING] Comando desconocido: ");
                Serial.println(command);
            }
            break;
    }
}


// FUNCIONES DE CAMBIO DE MODO - Mode Switching
void activateAutonomousMode() {
    if (currentMode == MODE_AUTONOMOUS) {
        sendFeedback("Ya en modo AUTO");
        return;
    }
    
    currentMode = MODE_AUTONOMOUS;
    lastScanTime = 0; // Forzar escaneo inmediato
    
    // Encender luces para indicar modo autónomo
    digitalWrite(HEADLIGHT_PIN, HIGH);
    digitalWrite(TAILLIGHT_PIN, HIGH);
    
    // Patron de sonido
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    
    Serial.println("\n========================================");
    Serial.println("   MODO AUTONOMO ACTIVADO");
    Serial.println("   Comando 'M' para volver a manual");
    Serial.println("========================================\n");
    
    BTSerial.println("MODO AUTO");
}

void activateManualMode() {
    if (currentMode == MODE_MANUAL) {
        sendFeedback("Ya en modo MANUAL");
        return;
    }
    
    // Detener motores y centrar servo
    stopMotors();
    radarServo.write(ANGLE_CENTER);
    
    currentMode = MODE_MANUAL;
    
    // Apagar luces
    digitalWrite(HEADLIGHT_PIN, LOW);
    digitalWrite(TAILLIGHT_PIN, LOW);
    
    // Patron de sonido
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    
    Serial.println("\n========================================");
    Serial.println("   MODO MANUAL ACTIVADO");
    Serial.println("   Comando 'A' para modo autonomo");
    Serial.println("========================================\n");
    
    BTSerial.println("MODO MANUAL");
}


// FUNCIONES DE MOVIMIENTO - Movement Functions
void moveForward() {
    digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, HIGH);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
}

void moveBackward() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, HIGH);
}

void turnLeft() {
    digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, HIGH);
}

void turnRight() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
    digitalWrite(LEFT_MOTOR_PIN1, HIGH);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
}

void moveForwardLeft() {
    digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
}

void moveForwardRight() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, HIGH);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
}

void moveBackwardLeft() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
}

void moveBackwardRight() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, HIGH);
}

void stopMotors() {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
}


// FUNCIONES DE ACCESORIOS - Tools Functions
void activateHorn() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(80);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
}

void blinkTaillight() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(TAILLIGHT_PIN, HIGH);
        delay(500);
        digitalWrite(TAILLIGHT_PIN, LOW);
        delay(500);
    }
}


// FUNCIONES AUXILIARES - Helper Functions

void sendFeedback(const char* message) {
    if (debugMode) {
        Serial.print(">>> ");
        Serial.println(message);
    }
}

void displayScanResults() {
    Serial.println("\n=== RESULTADOS DE ESCANEO ===");
    Serial.print("Izquierda: ");
    Serial.print(distanceLeft);
    Serial.println(" cm");
    Serial.print("Centro:    ");
    Serial.print(distanceCenter);
    Serial.println(" cm");
    Serial.print("Derecha:   ");
    Serial.print(distanceRight);
    Serial.println(" cm");
    Serial.println("============================\n");
}