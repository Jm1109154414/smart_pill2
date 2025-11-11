# PillMate - Smart Pillbox System

Sistema inteligente de pastillero con Raspberry Pi 3 y PWA para gestión de medicamentos, confirmación por peso y notificaciones Web Push.

## 🏗️ Arquitectura

### Stack Tecnológico
- **Frontend**: React 18 + Vite + TypeScript + Tailwind CSS + shadcn/ui
- **Backend**: Supabase (Auth, PostgreSQL, Storage) + Edge Functions (Deno)
- **Hardware**: Raspberry Pi 3 + Sensor HX711 + LED + Buzzer
- **Comunicación**: HTTP/HTTPS exclusivamente (no MQTT)

### Características Principales
- ✅ Registro y autenticación de usuarios
- ✅ Gestión de dispositivos (Raspberry Pi)
- ✅ Configuración de 5 compartimentos por dispositivo
- ✅ Programación de horarios con días, ventanas y alarmas
- ✅ Notificaciones Web Push en tiempo real
- ✅ Confirmación automática de tomas por caída de peso
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

### Para la Raspberry Pi
```bash
Python 3.9+
GPIO configurado
Sensor HX711 conectado
```

## 🚀 Instalación y Configuración

### 1. Clonar el Repositorio
```bash
git clone <repository-url>
cd pillmate
npm install
```

### 2. Configurar Variables de Entorno
Crear archivo `.env` basado en `.env.example`:

```bash
# Supabase
VITE_SUPABASE_URL=https://tu-proyecto.supabase.co
VITE_SUPABASE_ANON_KEY=tu-anon-key

# VAPID Keys (generar con: npx web-push generate-vapid-keys)
VITE_VAPID_PUBLIC_KEY=tu-vapid-public-key
```

### 3. Generar Claves VAPID
```bash
npx web-push generate-vapid-keys
```

Copiar las claves generadas:
- `VAPID_PUBLIC_KEY` → Configurar como `VITE_VAPID_PUBLIC_KEY` en `.env` local
- `VAPID_PRIVATE_KEY` → Configurar solo en Supabase Secrets (Cloud → Settings → Secrets)
- `VAPID_PUBLIC` → También en Supabase Secrets

**IMPORTANTE:** Configurar ambas claves en Supabase Secrets con estos nombres exactos:
- `VAPID_PUBLIC_KEY` (o `VAPID_PUBLIC`)
- `VAPID_PRIVATE_KEY` (o `VAPID_PRIVATE`)

### 4. Crear Bucket de Storage
El bucket `reports` se crea automáticamente con la migración. Si necesitas crearlo manualmente:

```sql
INSERT INTO storage.buckets (id, name, public, file_size_limit, allowed_mime_types)
VALUES ('reports', 'reports', false, 26214400, ARRAY['application/pdf']);
```

### 5. Edge Functions
Las funciones se despliegan automáticamente. Verificar en Supabase Dashboard:
- `devices-register`, `devices-config`
- `events-dose`, `weights-bulk`, `doses-query`
- `reports-generate`
- `push-subscribe`, `push-send`, `alarm-start`
- `commands-create`, `commands-poll`, `commands-ack`

### 6. Ejecutar Localmente
```bash
npm run dev
```

Abrir http://localhost:5173

## 🔌 Contrato HTTP para Raspberry Pi

Base URL: `https://tu-proyecto.supabase.co/functions/v1`

### Autenticación
Headers requeridos para endpoints de dispositivo:
```
X-Device-Serial: <serial-del-dispositivo>
X-Device-Secret: <secret-del-dispositivo>
```

### Endpoints Principales

#### 1. Obtener Configuración
```http
GET /devices-config
Headers:
  X-Device-Serial: RPI-12345
  X-Device-Secret: mi-secret-seguro
```

**Respuesta:**
```json
{
  "timezone": "America/Mexico_City",
  "deviceId": "uuid",
  "compartments": [...],
  "schedules": [...]
}
```

#### 2. Reportar Evento de Dosis
```http
POST /events-dose
Content-Type: application/json

{
  "serial": "RPI-12345",
  "secret": "mi-secret-seguro",
  "compartmentId": "uuid",
  "scheduledAt": "2025-01-11T08:00:00Z",
  "status": "taken",
  "actualAt": "2025-01-11T08:02:30Z",
  "deltaWeightG": 0.48
}
```

#### 3. Enviar Lecturas de Peso (Batch)
```http
POST /weights-bulk
Content-Type: application/json

{
  "serial": "RPI-12345",
  "secret": "mi-secret-seguro",
  "readings": [
    {
      "measuredAt": "2025-01-11T08:00:00Z",
      "weightG": 125.43,
      "raw": { "adc": 12345 }
    }
  ]
}
```

#### 4. Iniciar Alarma (Envía Push)
```http
POST /alarm-start
Content-Type: application/json

{
  "serial": "RPI-12345",
  "secret": "mi-secret-seguro",
  "compartmentId": "uuid",
  "scheduledAt": "2025-01-11T08:00:00Z",
  "title": "Aspirina"
}
```

#### 5. Consultar Comandos (Polling)
```http
GET /commands-poll?since=2025-01-11T08:00:00Z
Headers:
  X-Device-Serial: RPI-12345
  X-Device-Secret: mi-secret-seguro
```

#### 6. Confirmar Comando
```http
POST /commands-ack
Content-Type: application/json

{
  "serial": "RPI-12345",
  "secret": "mi-secret-seguro",
  "commandId": "uuid",
  "status": "done"
}
```

### Flujo de Trabajo de la Raspberry Pi

1. **Startup:** Sincronizar NTP, obtener configuración, programar alarmas
2. **Loop:** Polling de comandos cada 60s, monitorear peso
3. **Alarma:** Activar LED/Buzzer, POST a `alarm-start` (envía push)
4. **Toma:** Detectar caída de peso, POST a `events-dose`
5. **Comandos:** Procesar snooze, apply_config, reboot

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

## 🐛 Troubleshooting

### Notificaciones no llegan
- Verificar permisos del navegador
- Comprobar que Service Worker está activo
- Revisar logs de `push-send`

### Error de bucket reports
Si ves error `reports_bucket_missing`:
```sql
INSERT INTO storage.buckets (id, name, public) 
VALUES ('reports', 'reports', false);
```

## 📚 Recursos

- [Documentación Supabase](https://supabase.com/docs)
- [Web Push Protocol](https://developers.google.com/web/fundamentals/push-notifications)
- [Raspberry Pi GPIO](https://www.raspberrypi.com/documentation/)

## 📝 Licencia

MIT License

---

**Nota de Seguridad:** Este proyecto requiere que habilites la protección de contraseñas filtradas en Supabase Auth para cumplir con las mejores prácticas de seguridad. Ve a Dashboard → Authentication → Providers → Email → Password Protection.
