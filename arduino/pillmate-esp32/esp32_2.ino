/*
 * Smart Pill ESP32 Client
 *
 * Versión extendida: 3 compartimentos, servo, LDRs y ultrasonido
 * Usa los mismos endpoints de Supabase que el pillmate-esp32 original.
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <ESP32Servo.h>
#include "config.h"

// ================== ENDPOINTS ==================

const String BASE_URL              = SUPABASE_URL;
const String CONFIG_ENDPOINT       = BASE_URL + "/functions/v1/devices-config";
const String DOSE_EVENT_ENDPOINT   = BASE_URL + "/functions/v1/events-dose";
const String ALARM_START_ENDPOINT  = BASE_URL + "/functions/v1/alarm-start";
const String COMMANDS_POLL_ENDPOINT= BASE_URL + "/functions/v1/commands-poll";
const String COMMANDS_ACK_ENDPOINT = BASE_URL + "/functions/v1/commands-ack";
// Opcional: solo si creas esa función en Supabase
// const String TELEMETRY_ENDPOINT    = BASE_URL + "/functions/v1/telemetry";

// ================== HARDWARE ==================

// Ajusta estos pines a tu placa
const int LED_PINS[3] = {2, 4, 16};
const int LDR_PINS[3] = {32, 33, 34};
const int BUZZER_PIN  = 15;
const int TRIG_PIN    = 5;
const int ECHO_PIN    = 10;   // ojo con este pin, verifica que sea usable en tu ESP32
const int SERVO_PIN   = 12;

Servo servoMotor;

const int SERVO_CLOSED_ANGLE = 0;
const int SER// ...existing code...
-      <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
-        {compartments.filter(c => c.idx <= 3).map((comp) => (
+      <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
+        {compartments.map((comp) => (
           <Card key={comp.id}>
             <CardHeader>
               <CardTitle>Compartimento {comp.idx}</CardTitle>
               <CardDescription>
                 <div className="flex items-center justify-between mt-2">
                   <span>Activo</span>
                   <Switch
                     checked={comp.active}
                     onCheckedChange={(checked) =>
                       updateCompartment(comp.idx, "active", checked)
                     }
                   />
                 </div>
               </CardDescription>
             </CardHeader>
             <CardContent className="space-y-4">
               <div className="space-y-2">
                 <Label htmlFor={`title-${comp.idx}`}>Título</Label>
                 <Input
                   id={`title-${comp.idx}`}
                   value={comp.title}
                   onChange={(e) =>
                     updateCompartment(comp.idx, "title", e.target.value)
                   }
                   placeholder="ej: Aspirina"
                 />
               </div>
               <div className="space-y-2">
                 <Label htmlFor={`angle-${comp.idx}`}>Ángulo Servo (0-180°)</Label>
-                <Input
-                  id={`angle-${comp.idx}`}
-                  type="number"
-                  min="0"
-                  max="180"
-                  value={comp.servo_angle_deg ?? (comp.idx === 1 ? 0 : comp.idx === 2 ? 90 : 180)}
-                  onChange={(e) =>
-                    updateCompartment(
-                      comp.idx,
-                      "servo_angle_deg",
-                      e.target.value ? parseInt(e.target.value, 10) : null
-                    )
-                  }
-                  placeholder={`${comp.idx === 1 ? '0' : comp.idx === 2 ? '90' : '180'}`}
-                />
+                <Input
+                  id={`angle-${comp.idx}`}
+                  type="number"
+                  min={0}
+                  max={180}
+                  // si hay valor numérico lo mostramos, si no dejamos cadena vacía para permitir edición libre
+                  value={comp.servo_angle_deg != null ? String(comp.servo_angle_deg) : ""}
+                  onChange={(e) =>
+                    updateCompartment(
+                      comp.idx,
+                      "servo_angle_deg",
+                      e.target.value !== "" ? parseInt(e.target.value, 10) : null
+                    )
+                  }
+                  placeholder="ej: 120"
+                />
               </div>
               <div className="space-y-2">
                 <Label htmlFor={`weight-${comp.idx}`}>Peso esperado (g)</Label>
                 <Input
                   id={`weight-${comp.idx}`}
                   type="number"
                   step="0.01"
                   value={comp.expected_pill_weight_g || ""}
                   onChange={(e) =>
                     updateCompartment(
                       comp.idx,
                       "expected_pill_weight_g",
                       e.target.value ? parseFloat(e.target.value) : null
                     )
                   }
                   placeholder="0.50"
                 />
               </div>
             </CardContent>
           </Card>
         ))}
       </div>
// ...existing code...// ...existing code...
-      <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
-        {compartments.filter(c => c.idx <= 3).map((comp) => (
+      <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
+        {compartments.map((comp) => (
           <Card key={comp.id}>
             <CardHeader>
               <CardTitle>Compartimento {comp.idx}</CardTitle>
               <CardDescription>
                 <div className="flex items-center justify-between mt-2">
                   <span>Activo</span>
                   <Switch
                     checked={comp.active}
                     onCheckedChange={(checked) =>
                       updateCompartment(comp.idx, "active", checked)
                     }
                   />
                 </div>
               </CardDescription>
             </CardHeader>
             <CardContent className="space-y-4">
               <div className="space-y-2">
                 <Label htmlFor={`title-${comp.idx}`}>Título</Label>
                 <Input
                   id={`title-${comp.idx}`}
                   value={comp.title}
                   onChange={(e) =>
                     updateCompartment(comp.idx, "title", e.target.value)
                   }
                   placeholder="ej: Aspirina"
                 />
               </div>
               <div className="space-y-2">
                 <Label htmlFor={`angle-${comp.idx}`}>Ángulo Servo (0-180°)</Label>
-                <Input
-                  id={`angle-${comp.idx}`}
-                  type="number"
-                  min="0"
-                  max="180"
-                  value={comp.servo_angle_deg ?? (comp.idx === 1 ? 0 : comp.idx === 2 ? 90 : 180)}
-                  onChange={(e) =>
-                    updateCompartment(
-                      comp.idx,
-                      "servo_angle_deg",
-                      e.target.value ? parseInt(e.target.value, 10) : null
-                    )
-                  }
-                  placeholder={`${comp.idx === 1 ? '0' : comp.idx === 2 ? '90' : '180'}`}
-                />
+                <Input
+                  id={`angle-${comp.idx}`}
+                  type="number"
+                  min={0}
+                  max={180}
+                  // si hay valor numérico lo mostramos, si no dejamos cadena vacía para permitir edición libre
+                  value={comp.servo_angle_deg != null ? String(comp.servo_angle_deg) : ""}
+                  onChange={(e) =>
+                    updateCompartment(
+                      comp.idx,
+                      "servo_angle_deg",
+                      e.target.value !== "" ? parseInt(e.target.value, 10) : null
+                    )
+                  }
+                  placeholder="ej: 120"
+                />
               </div>
               <div className="space-y-2">
                 <Label htmlFor={`weight-${comp.idx}`}>Peso esperado (g)</Label>
                 <Input
                   id={`weight-${comp.idx}`}
                   type="number"
                   step="0.01"
                   value={comp.expected_pill_weight_g || ""}
                   onChange={(e) =>
                     updateCompartment(
                       comp.idx,
                       "expected_pill_weight_g",
                       e.target.value ? parseFloat(e.target.value) : null
                     )
                   }
                   placeholder="0.50"
                 />
               </div>
             </CardContent>
           </Card>
         ))}
       </div>
// ...existing code...VO_COMP_ANGLE[3] = {60, 120, 180}; // un ángulo por compartimento

// ================== ESTRUCTURAS ==================

struct Compartment {
  String id;
  String title;
  int idx;      // 1,2,3 en BD
  bool active;
};

struct Schedule {
  String id;
  String compartmentId;
  String timeOfDay;      // "HH:MM"
  int daysOfWeek;        // 127 = todos los días
  int windowMinutes;     // ventana para considerar la toma
  bool enableBuzzer;
  bool enableLed;
};

Compartment compartments[3];
Schedule schedules[9]; // máximo 3 horarios por compartimento

int compartmentCount = 0;
int scheduleCount = 0;

String deviceId = "";
String timezone = "";

// Para no disparar el mismo horario muchas veces
bool   scheduleActive[9] = {false};
unsigned long scheduleTriggeredAt[9] = {0};

// ================== TIMERS ==================

unsigned long lastConfigFetch = 0;
unsigned long lastCommandPoll = 0;
// unsigned long lastTelemetry   = 0; // opcional

const unsigned long CONFIG_INTERVAL_MS = 300000UL; // 5 min
const unsigned long POLL_INTERVAL_MS   = 30000UL;  // 30 s
// const unsigned long TELEMETRY_INTERVAL_MS = 300000UL; // 5 min

// ================== UMBRALES ==================

const int   LDR_THRESHOLD          = 300;   // diferencia ADC para considerar "se abrió"
const float ULTRASONIC_THRESHOLD   = 1.5f;  // cm de cambio para considerar que se metió la mano
const int   SENSOR_SAMPLES         = 5;

// ================== PROTOTIPOS ==================

void connectWiFi();
void fetchDeviceConfig();
void pollCommands();
bool processCommand(const String &type, JsonObject payload);
void acknowledgeCommand(const String &commandId, const String &status, const String &detail = "");

void checkSchedules();
int  findCompartmentIndex(const String &compartmentId);

void openCompartment(int compIndex);
void closeCompartment();
void activateLocalAlarm(int compIndex);
void deactivateLocalAlarm(int compIndex);

bool detectTaken(int compIndex, int windowSeconds, String &why);
float measureDistance();

void reportDoseEvent(const String &compartmentId, const String &scheduledAt,
                     const String &status, const String &scheduleId = "");
void triggerAlarmBackend(const String &compartmentId, const String &scheduledAt);

// opcional
// void sendTelemetry(int compIndex, int ldrValue, float distance);

String getCurrentTimestamp();

// ================== SETUP ==================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Smart Pill ESP32 Client ===");
  Serial.println("Iniciando...");

  // Pines
  for (int i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  servoMotor.attach(SERVO_PIN);
  closeCompartment();

  // WiFi + NTP
  connectWiFi();

  configTime(-6 * 3600, 0, "pool.ntp.org"); // UTC-6 México
  Serial.print("Sincronizando NTP");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nNTP OK");

  // Config inicial desde Supabase
  fetchDeviceConfig();

  Serial.println("Sistema listo!");
}

// ================== LOOP ==================

void loop() {
  unsigned long now = millis();

  if (now - lastCommandPoll > POLL_INTERVAL_MS) {
    lastCommandPoll = now;
    pollCommands();
  }

  if (now - lastConfigFetch > CONFIG_INTERVAL_MS) {
    lastConfigFetch = now;
    fetchDeviceConfig();
  }

  // Si quieres telemetría periódica genérica, la podrías activar aquí.
  // if (now - lastTelemetry > TELEMETRY_INTERVAL_MS) { ... }

  checkSchedules();

  delay(500);
}

// ================== WIFI ==================

void connectWiFi() {
  Serial.printf("Conectando a WiFi: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 60) {
    delay(300);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nERROR: No se pudo conectar a WiFi");
  }
}

// ================== CONFIG DESDE SUPABASE ==================

void fetchDeviceConfig() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, reintentando...");
    connectWiFi();
    if (WiFi.status() != WL_CONNECTED) return;
  }

  HTTPClient http;
  http.begin(CONFIG_ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-device-serial", DEVICE_SERIAL);
  http.addHeader("x-device-secret", DEVICE_SECRET);

  Serial.println("Solicitando configuración al backend...");
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();

    DynamicJsonDocument doc(8192);
    DeserializationError err = deserializeJson(doc, payload);

    if (!err) {
      deviceId = doc["deviceId"].as<String>();
      timezone = doc["timezone"].as<String>();

      // Compartments
      JsonArray comps = doc["compartments"];
      compartmentCount = 0;
      for (JsonObject comp : comps) {
        if (compartmentCount < 3) {
          compartments[compartmentCount].id    = comp["id"].as<String>();
          compartments[compartmentCount].title = comp["title"].as<String>();
          compartments[compartmentCount].idx   = comp["idx"];  // normalmente 1..3
          compartments[compartmentCount].active= comp["active"];
          compartmentCount++;
        }
      }

      // Schedules
      JsonArray scheds = doc["schedules"];
      scheduleCount = 0;
      for (JsonObject sched : scheds) {
        if (scheduleCount < 9) {
          schedules[scheduleCount].id            = sched["id"].as<String>();
          schedules[scheduleCount].compartmentId = sched["compartment_id"].as<String>();
          schedules[scheduleCount].timeOfDay     = sched["time_of_day"].as<String>();
          schedules[scheduleCount].daysOfWeek    = sched["days_of_week"];
          schedules[scheduleCount].windowMinutes = sched["window_minutes"];
          schedules[scheduleCount].enableBuzzer  = sched["enable_buzzer"];
          schedules[scheduleCount].enableLed     = sched["enable_led"];
          scheduleCount++;
        }
      }

      for (int i = 0; i < scheduleCount; i++) {
        scheduleActive[i] = false;
        scheduleTriggeredAt[i] = 0;
      }

      Serial.println("✓ Configuración actualizada");
      Serial.printf("  Device: %s  TZ: %s\n", deviceId.c_str(), timezone.c_str());
      Serial.printf("  Compartimentos: %d  Horarios: %d\n", compartmentCount, scheduleCount);
    } else {
      Serial.println("✗ Error parseando JSON de configuración");
    }
  } else {
    Serial.printf("✗ Error obteniendo configuración: %d\n", httpCode);
    if (httpCode > 0) {
      Serial.println(http.getString());
    }
  }

  http.end();
}

// ================== POLL DE COMANDOS ==================

void pollCommands() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(COMMANDS_POLL_ENDPOINT);
  http.addHeader("x-device-serial", DEVICE_SERIAL);
  http.addHeader("x-device-secret", DEVICE_SECRET);

  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(4096);
    DeserializationError err = deserializeJson(doc, payload);

    if (!err) {
      JsonArray commands = doc["commands"];
      if (commands.size() > 0) {
        Serial.printf("📥 %d comandos recibidos\n", commands.size());
        for (JsonObject cmd : commands) {
          String commandId = cmd["id"].as<String>();
          String type      = cmd["type"].as<String>();
          JsonObject p     = cmd["payload"];

          Serial.printf("  Comando: %s (ID: %s)\n", type.c_str(), commandId.c_str());

          bool ok = processCommand(type, p);
          acknowledgeCommand(commandId, ok ? "done" : "error",
                             ok ? "" : "Error procesando comando");
        }
      }
    }
  } else if (httpCode != 404) {
    Serial.printf("✗ Error polling comandos: %d\n", httpCode);
  }

  http.end();
}

bool processCommand(const String &type, JsonObject payload) {
  if (type == "snooze") {
    int minutes = payload["minutes"];
    Serial.printf("  → Snooze %d minutos (no implementado aún)\n", minutes);
    return true;
  } else if (type == "apply_config") {
    Serial.println("  → apply_config: refetch configuración");
    fetchDeviceConfig();
    return true;
  } else if (type == "reboot") {
    Serial.println("  → reboot");
    delay(500);
    ESP.restart();
    return true;
  } else {
    Serial.printf("  → Comando desconocido: %s\n", type.c_str());
    return false;
  }
}

void acknowledgeCommand(const String &commandId, const String &status, const String &detail) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(COMMANDS_ACK_ENDPOINT);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(256);
  doc["serial"]    = DEVICE_SERIAL;
  doc["secret"]    = DEVICE_SECRET;
  doc["commandId"] = commandId;
  doc["status"]    = status;
  if (detail.length() > 0) {
    doc["detail"] = detail;
  }

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  int httpCode = http.POST(jsonPayload);
  if (httpCode == 200) {
    Serial.printf("  ✓ ACK enviado (%s)\n", status.c_str());
  } else {
    Serial.printf("  ✗ Error enviando ACK: %d\n", httpCode);
  }

  http.end();
}

// ================== HORARIOS ==================

void checkSchedules() {
  time_t nowRaw;
  time(&nowRaw);
  struct tm nowTm;
  localtime_r(&nowRaw, &nowTm);

  int curHour   = nowTm.tm_hour;
  int curMinute = nowTm.tm_min;
  int weekday   = nowTm.tm_wday; // 0=domingo ... 6=sábado

  for (int i = 0; i < scheduleCount; i++) {
    Schedule &sch = schedules[i];

    // Filtrar día de la semana según bits de daysOfWeek si quieres ser estricto.
    // Por simplicidad, asume todos los días (o 127).

    String tod = sch.timeOfDay;
    int colon = tod.indexOf(':');
    if (colon <= 0) continue;

    int sh = tod.substring(0, colon).toInt();
    int sm = tod.substring(colon + 1).toInt();

    int nowMin   = curHour * 60 + curMinute;
    int schedMin = sh * 60 + sm;
    int diffMin  = nowMin - schedMin;

    if (diffMin >= 0 && diffMin <= sch.windowMinutes) {
      // Estamos dentro de la ventana
      if (!scheduleActive[i]) {
        int compIndex = findCompartmentIndex(sch.compartmentId);
        if (compIndex >= 0 && compIndex < 3 && compartments[compIndex].active) {
          Serial.printf("→ Disparando horario %s para comp %d (diff %d min)\n",
                        sch.id.c_str(), compIndex, diffMin);

          scheduleActive[i] = true;
          scheduleTriggeredAt[i] = millis();

          // 1) Avisar backend (notificación push)
          triggerAlarmBackend(sch.compartmentId, sch.timeOfDay);

          // 2) Alarma local
          if (sch.enableLed)    activateLocalAlarm(compIndex);
          if (sch.enableBuzzer) tone(BUZZER_PIN, 1000);

          // 3) Abrir compartimento
          openCompartment(compIndex);

          // 4) Monitorear sensores durante la ventana
          String why = "";
          bool taken = detectTaken(compIndex, sch.windowMinutes * 60, why);

          // 5) Apagar alarma, cerrar
          noTone(BUZZER_PIN);
          deactivateLocalAlarm(compIndex);
          closeCompartment();

          if (taken) {
            Serial.printf("✓ Dosis tomada (horario %s) reason=%s\n", sch.id.c_str(), why.c_str());
            reportDoseEvent(sch.compartmentId, sch.timeOfDay, "taken", sch.id);
          } else {
            Serial.printf("✗ Dosis NO detectada (horario %s) -> missed\n", sch.id.c_str());
            reportDoseEvent(sch.compartmentId, sch.timeOfDay, "missed", sch.id);
          }
        }
      }
    } else {
      // Fuera de ventana: reseteamos flag después de un tiempo
      if (scheduleActive[i]) {
        unsigned long elapsed = millis() - scheduleTriggeredAt[i];
        unsigned long allowedMs = (unsigned long)sch.windowMinutes * 60UL * 1000UL + 5000UL;
        if (elapsed > allowedMs) {
          scheduleActive[i] = false;
          scheduleTriggeredAt[i] = 0;
        }
      }
    }
  }
}

int findCompartmentIndex(const String &compartmentId) {
  for (int i = 0; i < compartmentCount && i < 3; i++) {
    if (compartments[i].id == compartmentId) {
      // idx en BD podría ser 1..3, lo mapeamos a 0..2 si quieres
      return i; 
    }
  }
  return -1;
}

// ================== SERVO + ALARMA LOCAL ==================

void openCompartment(int compIndex) {
  if (compIndex < 0 || compIndex > 2) return;
  int angle = SERVO_COMP_ANGLE[compIndex];
  servoMotor.write(angle);
  delay(600);
}

void closeCompartment() {
  servoMotor.write(SERVO_CLOSED_ANGLE);
  delay(300);
}

void activateLocalAlarm(int compIndex) {
  if (compIndex < 0 || compIndex > 2) return;
  digitalWrite(LED_PINS[compIndex], HIGH);
}

void deactivateLocalAlarm(int compIndex) {
  if (compIndex < 0 || compIndex > 2) return;
  digitalWrite(LED_PINS[compIndex], LOW);
}

// ================== DETECCIÓN DE TOMA ==================

bool detectTaken(int compIndex, int windowSeconds, String &why) {
  unsigned long start = millis();
  unsigned long timeoutMs = (unsigned long)windowSeconds * 1000UL;

  // Baseline LDR
  int baseLdr = 0;
  for (int i = 0; i < SENSOR_SAMPLES; i++) {
    baseLdr += analogRead(LDR_PINS[compIndex]);
    delay(25);
  }
  baseLdr /= SENSOR_SAMPLES;

  float baseDist = measureDistance();
  delay(200);

  Serial.printf("Baseline comp %d -> LDR=%d dist=%.2fcm\n", compIndex, baseLdr, baseDist);

  while (millis() - start < timeoutMs) {
    int ldr = analogRead(LDR_PINS[compIndex]);
    float dist = measureDistance();

    // LDR: más luz de golpe => se abrió
    if ((ldr - baseLdr) > LDR_THRESHOLD) {
      why = "ldr_open";
      return true;
    }

    // Ultrasonido: cambio grande en distancia => mano/pastilla
    if (baseDist > 0.1f && (dist - baseDist) > ULTRASONIC_THRESHOLD) {
      why = "ultrasonic_change";
      return true;
    }

    delay(300);
  }

  return false;
}

// ================== ULTRASONIDO ==================

float measureDistance() {
  long duration;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1.0f;

  float distance = (duration * 0.0343f) / 2.0f;
  return distance;
}

// ================== REPORTES AL BACKEND ==================

void reportDoseEvent(const String &compartmentId, const String &scheduledAt,
                     const String &status, const String &scheduleId) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado: no se puede reportar evento");
    return;
  }

  HTTPClient http;
  http.begin(DOSE_EVENT_ENDPOINT);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(512);
  doc["serial"]       = DEVICE_SERIAL;
  doc["secret"]       = DEVICE_SECRET;
  doc["compartmentId"]= compartmentId;
  doc["scheduledAt"]  = scheduledAt;
  doc["status"]       = status;   // "taken" | "missed"
  doc["source"]       = "device";
  doc["actualAt"]     = getCurrentTimestamp();
  if (scheduleId.length() > 0) {
    doc["scheduleId"] = scheduleId;
  }

  String payload;
  serializeJson(doc, payload);

  int httpCode = http.POST(payload);
  if (httpCode == 200) {
    Serial.println("✓ Evento de dosis reportado");
  } else {
    Serial.printf("✗ Error reportando evento: %d\n", httpCode);
  }

  http.end();
}

void triggerAlarmBackend(const String &compartmentId, const String &scheduledAt) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado - no puedo notificar alarma");
    return;
  }

  HTTPClient http;
  http.begin(ALARM_START_ENDPOINT);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(256);
  doc["serial"]      = DEVICE_SERIAL;
  doc["secret"]      = DEVICE_SECRET;
  doc["compartmentId"]= compartmentId;
  doc["scheduledAt"] = scheduledAt;

  String payload;
  serializeJson(doc, payload);

  int httpCode = http.POST(payload);
  if (httpCode == 200) {
    Serial.println("✓ Alarma iniciada en backend (notificación push)");
  } else {
    Serial.printf("✗ Error al iniciar alarma: %d\n", httpCode);
  }

  http.end();
}

// ================== TIMESTAMP ==================

String getCurrentTimestamp() {
  time_t now;
  time(&now);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  char buffer[32];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
  return String(buffer);
}
