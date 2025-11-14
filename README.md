# PillMate - Smart Pillbox System

Sistema inteligente de pastillero con ESP32 y PWA para gestión de medicamentos con servomotor y notificaciones Web Push.

## 🏗️ Arquitectura

### Stack Tecnológico
- **Frontend**: React 18 + Vite + TypeScript + Tailwind CSS + shadcn/ui
- **Backend**: Supabase (Auth, PostgreSQL, Storage) + Edge Functions (Deno)
- **Hardware**: ESP32 + Servomotor + LED + Buzzer
- **Comunicación**: HTTP/HTTPS exclusivamente (no MQTT)

### Características Principales
- ✅ Registro y autenticación de usuarios
- ✅ Gestión de dispositivos (ESP32)
- ✅ Configuración de 3 compartimentos por dispositivo
- ✅ Control de servomotor con ángulos configurables (0-180°)
- ✅ Programación de horarios con días, ventanas y alarmas
- ✅ Notificaciones Web Push en tiempo real
- ✅ Dashboard con métricas de adherencia
- ✅ Generación de reportes PDF (semanales/mensuales)
- ✅ Sistema de comandos (snooze, apply_config, reboot)
- ✅ PWA instalable con Service Worker

## 📋 Requisitos Previos

### Para el Frontend (PWA)
```bash
Node.js >= 18
npm o bun
```

### Para el Backend
- Cuenta de Supabase (o Lovable Cloud)
- Claves VAPID para Web Push

### Para el ESP32
```bash
Arduino IDE o PlatformIO
Librería HTTPClient
WiFi configurado
Servomotor conectado
```

## 🚀 Instalación y Configuración

### 1. Clonar el Repositorio
```bash
git clone <repository-url>
cd pillmate
npm install
```

### 2. Configurar Variables de Entorno

**IMPORTANTE:** El archivo `.env` NO se versiona (protegido por `.gitignore`). 

Crear archivo `.env` local basado en `.env.example`:

```bash
# Supabase (configuradas automáticamente por Lovable Cloud)
VITE_SUPABASE_URL=https://tu-proyecto.supabase.co
VITE_SUPABASE_ANON_KEY=tu-anon-key
VITE_SUPABASE_PROJECT_ID=tu-project-id

# VAPID Keys - Cliente (generar con: npx web-push generate-vapid-keys)
VITE_VAPID_PUBLIC=tu-vapid-public-key
```

### 3. Generar y Configurar Claves VAPID
```bash
npx web-push generate-vapid-keys
```

**Configuración de claves VAPID:**

#### Frontend (Variables Públicas)
En Lovable: Settings → Environment → Frontend/Public variables
```
VITE_VAPID_PUBLIC = <TU_PUBLIC_KEY>
```

#### Servidor (Secrets - Edge Functions)
En Lovable: Settings → Environment → Server/Secrets
```
VAPID_PUBLIC = <TU_PUBLIC_KEY>
VAPID_PRIVATE = <TU_PRIVATE_KEY>
```

**CRÍTICO:** 
- La clave `VAPID_PRIVATE` NUNCA debe exponerse al cliente
- Solo se configura en secretos del servidor
- El cliente solo necesita `VITE_VAPID_PUBLIC`

### 4. Crear Bucket de Storage
El bucket `reports` se crea automáticamente con la migración. Si necesitas crearlo manualmente:

```sql
INSERT INTO storage.buckets (id, name, public, file_size_limit, allowed_mime_types)
VALUES ('reports', 'reports', false, 26214400, ARRAY['application/pdf']);
```

### 5. Edge Functions
Las funciones se despliegan automáticamente. Verificar en Supabase Dashboard:
- `devices-register`, `devices-config`
- `events-dose`, `doses-query`
- `reports-generate`
- `push-subscribe`, `push-send`, `alarm-start`
- `commands-create`, `commands-poll`, `commands-ack`

### 6. Ejecutar Localmente
```bash
npm run dev
```

Abrir http://localhost:5173

## 🔌 Contrato HTTP para Agente ESP32

Base URL: `https://tu-proyecto.supabase.co/functions/v1`

### Autenticación
El ESP32 debe autenticarse usando su Serial (dirección MAC) y un Secret (PSK almacenado en NVS o código).

Headers requeridos para endpoints de dispositivo:
```
X-Device-Serial: <MAC-address-del-ESP32>
X-Device-Secret: <PSK-del-dispositivo>
```

**Configuración del ESP32:**
- **Serial**: Usar la dirección MAC del ESP32 (ej: `ESP32-AABBCCDDEE`)
- **Secret**: Pre-Shared Key (PSK) almacenado en NVS o hardcoded
- El Secret se hashea con SHA-256 en el servidor para validación

### Endpoints Principales

#### 1. Obtener Configuración
```http
GET /devices-config
Headers:
  X-Device-Serial: ESP32-AABBCCDDEE
  X-Device-Secret: mi-psk-seguro
```

**Respuesta:**
```json
{
  "timezone": "America/Mexico_City",
  "deviceId": "uuid",
  "compartments": [
    {
      "id": "uuid",
      "idx": 1,
      "title": "Aspirina",
      "active": true,
      "servo_angle_deg": 0
    },
    {
      "id": "uuid",
      "idx": 2,
      "title": "Ibuprofeno",
      "active": true,
      "servo_angle_deg": 90
    },
    {
      "id": "uuid",
      "idx": 3,
      "title": "Paracetamol",
      "active": true,
      "servo_angle_deg": 180
    }
  ],
  "schedules": [
    {
      "id": "uuid",
      "compartment_id": "uuid",
      "time_of_day": "08:00",
      "days_of_week": 127,
      "window_minutes": 10,
      "enable_led": true,
      "enable_buzzer": true
    }
  ]
}
```

#### 2. Reportar Evento de Dosis
```http
POST /events-dose
Content-Type: application/json

{
  "serial": "ESP32-AABBCCDDEE",
  "secret": "mi-psk-seguro",
  "compartmentId": "uuid",
  "scheduledAt": "2025-01-11T08:00:00Z",
  "status": "taken",
  "actualAt": "2025-01-11T08:02:30Z",
  "deltaWeightG": null
}
```

**Nota:** `deltaWeightG` puede ser `null` ya que el ESP32 no usa sensor de peso.

#### 3. Iniciar Alarma (Envía Push)
```http
POST /alarm-start
Content-Type: application/json

{
  "serial": "ESP32-AABBCCDDEE",
  "secret": "mi-psk-seguro",
  "compartmentId": "uuid",
  "scheduledAt": "2025-01-11T08:00:00Z",
  "title": "Aspirina"
}
```

#### 4. Consultar Comandos (Polling)
```http
GET /commands-poll?since=2025-01-11T08:00:00Z
Headers:
  X-Device-Serial: ESP32-AABBCCDDEE
  X-Device-Secret: mi-psk-seguro
```

#### 5. Confirmar Comando
```http
POST /commands-ack
Content-Type: application/json

{
  "serial": "ESP32-AABBCCDDEE",
  "secret": "mi-psk-seguro",
  "commandId": "uuid",
  "status": "done"
}
```

### Flujo de Trabajo del ESP32

1. **Startup:** 
   - Conectar WiFi
   - Sincronizar tiempo via NTP
   - Obtener configuración (`/devices-config`)
   - Programar alarmas basadas en schedules

2. **Loop Principal:**
   - Polling de comandos cada 60s (`/commands-poll`)
   - Ejecutar comandos (snooze, apply_config, reboot)

3. **Alarma:**
   - Activar LED/Buzzer según configuración
   - Mover servomotor al ángulo del compartimento
   - POST a `/alarm-start` (envía notificación push al usuario)
   - Esperar confirmación del usuario (botón físico o comando)
   - POST a `/events-dose` con status="taken" o "missed"

4. **Comandos:**
   - `snooze`: Posponer alarma 5 minutos
   - `apply_config`: Recargar configuración
   - `reboot`: Reiniciar dispositivo

## 🔔 Notificaciones Web Push

### Configuración
1. Navegar a `/notifications`
2. Hacer clic en "Activar notificaciones"
3. Aceptar permisos del navegador

### Acciones de Notificación
- **Abrir**: Abre `/dashboard`
- **Posponer 5 min**: Crea comando `snooze` y abre ruta especial

## 📊 Reportes PDF

1. Navegar a `/reports?deviceId=<id>`
2. Seleccionar tipo (Semanal/Mensual)
3. Hacer clic en "Generar PDF"
4. El PDF se genera y descarga con URL firmada

**Contenido:**
- Métricas de adherencia (% taken, tardías, omitidas)
- Tabla completa de eventos del período

## 🔒 Seguridad

- **RLS**: Políticas estrictas que aíslan recursos por usuario
- **Secrets**: Hasheados con SHA-256 (no texto plano)
- **Rate Limiting**: En login y funciones públicas
- **Bucket privado**: Reports solo accesibles por URL firmada

## 📱 PWA

- Instalable en móviles y escritorio
- Service Worker registrado para push
- Funciona offline (cache estático)
- Manifest con iconos

## 🧪 Pruebas Rápidas

### Verificar Configuración
1. Visita `/dev/config-check` para ver el estado completo del sistema
2. Confirma que todos los checks estén en verde ✅
3. Si hay errores, sigue las instrucciones en pantalla

### Activar Notificaciones
1. Ve a `/notifications` o usa el botón en `/dev/config-check`
2. Haz clic en "Activar notificaciones"
3. Acepta los permisos del navegador
4. Usa "Notificación de prueba (self)" para verificar

### Registrar Dispositivo
1. Ve a `/device`
2. Completa el formulario con `serial`, `secret`, `name` y `timezone`
3. Guarda el dispositivo

### Probar ESP32 (con curl)
Desde cualquier terminal, reemplaza los placeholders:
```bash
# Activar alarma (envía push al usuario)
curl -X POST https://tu-proyecto.supabase.co/functions/v1/alarm-start \
  -H "Content-Type: application/json" \
  -d '{
    "serial": "ESP32-AABBCCDDEE",
    "secret": "TU-PSK",
    "compartmentId": "uuid-compartment",
    "scheduledAt": "2025-01-15T08:00:00Z",
    "title": "Aspirina"
  }'

# Reportar toma de pastilla
curl -X POST https://tu-proyecto.supabase.co/functions/v1/events-dose \
  -H "Content-Type: application/json" \
  -d '{
    "serial": "ESP32-AABBCCDDEE",
    "secret": "TU-PSK",
    "compartmentId": "uuid-compartment",
    "scheduledAt": "2025-01-15T08:00:00Z",
    "status": "taken",
    "actualAt": "2025-01-15T08:02:30Z",
    "deltaWeightG": null
  }'
```

### Generar Reporte
1. Ve a `/reports`
2. Selecciona tipo (Semanal/Mensual)
3. Haz clic en "Generar PDF"
4. Descarga el reporte generado

## 🐛 Troubleshooting

### Verificador de Configuración
Visita `/dev/config-check` para diagnosticar problemas de configuración:
- Estado de `VITE_VAPID_PUBLIC`
- Service Worker y PushManager disponibles
- Permisos de notificación
- Claves VAPID del servidor (`VAPID_PUBLIC`/`VAPID_PRIVATE`)
- Bucket `reports` creado
- Conteo de suscripciones y dispositivos

### Notificaciones no llegan
1. Verificar que `VITE_VAPID_PUBLIC` está configurada (ver `/dev/config-check`)
2. Confirmar que `VAPID_PUBLIC` y `VAPID_PRIVATE` están en Server Secrets
3. Verificar permisos del navegador (deben ser "granted")
4. Comprobar que Service Worker está activo
5. Revisar logs de `push-send` en Edge Functions
6. Usar botón "Notificación de prueba (self)" en `/dev/config-check`

### Error de claves VAPID
Si ves `missing_vapid_keys`:
- **Cliente:** Configurar `VITE_VAPID_PUBLIC` en Frontend/Public variables
- **Servidor:** Configurar `VAPID_PUBLIC` y `VAPID_PRIVATE` en Server/Secrets
- Generar con: `npx web-push generate-vapid-keys`
- **CRÍTICO:** La `VAPID_PRIVATE` NUNCA debe exponerse al cliente

### Error de bucket reports
Si ves error `reports_bucket_missing`:
```sql
INSERT INTO storage.buckets (id, name, public) 
VALUES ('reports', 'reports', false);
```

## 📚 Recursos

- [Documentación Supabase](https://supabase.com/docs)
- [Web Push Protocol](https://developers.google.com/web/fundamentals/push-notifications)
- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

## 📝 Licencia

MIT License

---

**Nota de Seguridad:** Este proyecto requiere que habilites la protección de contraseñas filtradas en Supabase Auth para cumplir con las mejores prácticas de seguridad. Ve a Dashboard → Authentication → Providers → Email → Password Protection.
