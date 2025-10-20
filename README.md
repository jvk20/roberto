<img src="docs/logo_roberto.png" alt="Logo del proyecto" width="110" align="right">

# Vehículo Robótico 2WD con Control Bluetooth y Navegación Autónoma
## Johan Alejandro Van Kesteren González -Décimo Grado | Colegio Gimnasio Inglés 

> Proyecto educativo para la **Feria de STEAM 2025**. Un robot 2WD controlado desde el celular por **Bluetooth**, con luces, bocina y **modo autónomo** basado en **sensor ultrasónico** sobre un **servomotor** para explorar el entorno y evitar obstáculos.

**Poster del proyecto:** [docs/poster.pdf](docs/poster.pdf)

---

## 📹 Demo
- **Video demostrativo (próximamente):** https://youtu.be/XXXXXXXXXX 
---

## 🧠 Resumen

Se construyó un vehículo 2WD con Arduino que se controla por Bluetooth desde un celular y permite moverse, encender luces y accionar bocina. Cuenta con un modo autónomo básico: un sensor ultrasónico (HC‑SR04) montado en un servomotor (SG90) “barre” el entorno, mide distancias a izquierda–centro–derecha y **toma decisiones** (avanzar, girar, retroceder) para esquivar obstáculos.  
El proyecto integra **ciencia, tecnología, ingeniería y matemáticas (STEAM)** y conecta con la pasión por los autos. Sirve como base para aplicaciones reales, por ejemplo **robots de entrega de “último kilómetro”** en campus, hospitales u oficinas.

---

## 🚗 Características

- Control manual vía **Bluetooth (HC‑06)** desde una app móvil o un terminal serie.
- Accesorios: **faros delanteros**, **luz trasera**, **bocina**.
- **Modo autónomo** con barrido de 180° y lógica de evasión.
---

## 🛠️ Lista de materiales

- **Control:** Arduino **UNO** R3  
- **Comunicación:** Módulo Bluetooth **HC‑06**
- **Sensores / Actuadores:**  
  - Sensor ultrasónico **HC‑SR04**  
  - Servomotor **SG90**  
  - 2 × Motores **TT** + ruedas  
  - Puente H **L298N**  
  - **Buzzer** (activo 5 V)  
  - LEDs (faros y luz trasera) + resistencias 220 Ω
- **Alimentación:** 2 × baterías **18650** (7.4 V nominal) + portapilas y switch
- Varios: jumpers, protoboard o base, tornillería, bridas.
---

## 🔌 Conexiones (Wiring)

### Bluetooth HC‑06 ↔ Arduino (SoftwareSerial)
- **HC‑06 TXD → Arduino D10**  *(RX de SoftwareSerial)*  
- **HC‑06 RXD ← Arduino D11**  *(TX de SoftwareSerial — usa divisor resistivo 1 k/2 k si tu HC‑06 no es 5 V tolerant)*  
- VCC 5 V, GND

### L298N ↔ Motores / Arduino
- **Motor derecho:** OUT1/OUT2  
- **Motor izquierdo:** OUT3/OUT4  
- **IN1..IN4 → Arduino D7, D6, D8, D9**  
  - D7 = RIGHT_MOTOR_PIN1  
  - D6 = RIGHT_MOTOR_PIN2  
  - D8 = LEFT_MOTOR_PIN1  
  - D9 = LEFT_MOTOR_PIN2  
- ENA/ENB: con **jumper** para velocidad fija (o a futuro PWM)
- +12 V del pack 2×18650 → **+12V L298N**  
- GND batería → **GND L298N** y **GND Arduino**

### Sensores y accesorios
- **Servo (SG90):** señal **D5**, +5 V, GND  
- **HC‑SR04:** TRIG **D12**, ECHO **D13**, +5 V, GND  
- **Buzzer:** **D2** a buzzer activo 5 V (si es pasivo, usar transistor)  
- **Faros (LEDs + 220 Ω):** **D3**  
- **Luz trasera (LED + 220 Ω):** **D4**

---

## 🧾 Mapa de conexiones

| Nombre en código | Pin Arduino |
|---|---|
| `BT_RX_PIN` (HC‑06 TX) | D10 |
| `BT_TX_PIN` (HC‑06 RX) | D11 |
| `RIGHT_MOTOR_PIN1` | D7 |
| `RIGHT_MOTOR_PIN2` | D6 |
| `LEFT_MOTOR_PIN1` | D8 |
| `LEFT_MOTOR_PIN2` | D9 |
| `BUZZER_PIN` | D2 |
| `HEADLIGHT_PIN` | D3 |
| `TAILLIGHT_PIN` | D4 |
| `SERVO_PIN` | D5 |
| `ULTRASONIC_TRIGGER` | D12 |
| `ULTRASONIC_ECHO` | D13 |

---
## 📱 Uso

### Emparejar Bluetooth
1. Encender el robot. El HC‑06 aparece como `HC-06` (PIN **1234** o **0000**).
2. Desde el teléfono abrir:  App tipo **BT Car Controller** que envia **comandos** tipo carácter.
3. Conectar al módulo y enviar comandos.

### Comandos

| Comando | Acción |
|---|---|
| **Z** | Activar **modo autónomo** |
| **1** | Activar **modo manual** |
| **F** | Avanzar |
| **B** | Retroceder |
| **L** | Girar izquierda |
| **R** | Girar derecha |
| **G** | Adelante‑Izquierda |
| **H** | Adelante‑Derecha |
| **I** | Atrás‑Izquierda |
| **J** | Atrás‑Derecha |
| **S** | **Stop** |
| **Y** | Bocina |
| **U** / **u** | Faros ON / OFF |
| **V** / **v** | Luz trasera ON / OFF |
| **W** / **w** | Todas las luces ON / OFF |
| **X** | Luz trasera intermitente |
| **D** | Alterna **modo debug** por Serial |
| **T** | Escaneo ultrasónico manual (solo en manual) |

---

## 🤖 Modo autónomo (cómo funciona)

- **Barrido 180°** con el servo: **izquierda (150°) – centro (90°) – derecha (30°)**.  
- Medición por **HC‑SR04** y decisión cada **2 s** (`SCAN_INTERVAL`).  
- Umbrales por defecto (cm):  
  - `DISTANCE_DANGER = 15` → **parar + retroceder** y girar hacia el lado más libre.  
  - `DISTANCE_WARNING = 30` → **buscar alternativa** (avanzar con sesgo a izquierda/derecha).  
  - `DISTANCE_SAFE = 50` → **avanzar** (recto o con leve corrección).
---

## 📚 Bibliografía

- Arduino. (2025). *Arduino Documentation*.  
- Dejan. (2022, 18 feb.). *Ultrasonic sensor HC‑SR04 and Arduino – Complete guide*. HowToMechatronics.  
- Fosmire, M., & Radcliffe, S. (2024). *Scientific posters: A learner's guide*. Ohio State University Libraries.  
- Margolis, M. (2012). *Make an Arduino‑controlled robot*. O’Reilly.  
- MarketsandMarkets. (2025). *Delivery robots market size, share, trends and growth, 2025–2030*.  
- Paul, S., Biswas, S., Sengupta, A., Basu, B., & Basu, S. (2019). *Arduino based, Bluetooth controlled RC car*. IJCSC, 11(1), 7–13.  
- Precedence Research. (2024, 16 ago.). *Delivery robots market size and forecast 2025–2034*.
