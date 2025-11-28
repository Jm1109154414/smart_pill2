/* PillMate ESP32 - Archivo de Configuración
 * 
 * IMPORTANTE: NO subas este archivo a repositorios públicos
 * Contiene tus credenciales secretas
 */

#ifndef CONFIG_H
#define CONFIG_H

// Credenciales WiFi
#define WIFI_SSID "iPhone de Lucas"
#define WIFI_PASSWORD "123456789"

// URL de tu proyecto Supabase
#define SUPABASE_URL "https://lhrwgcekkogybemtykts.supabase.co"

// Credenciales del dispositivo (obtenidas al registrar el pastillero en la PWA)
#define DEVICE_SERIAL "ESP32-1CC34AACCD98"  // Ejemplo: "ESP32-TEST-001"
#define DEVICE_SECRET "4bc64296f3878ae9e606132d5ea459fab3d8a5525e5e876f0974e9701b0a20b8"  // Ejemplo: "testpass123"

#endif
