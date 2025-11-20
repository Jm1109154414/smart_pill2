import { supabase } from "@/integrations/supabase/client";

export type PushSub = PushSubscription | null;

function urlBase64ToUint8Array(base64String: string): Uint8Array {
  const padding = '='.repeat((4 - (base64String.length % 4)) % 4);
  const base64 = (base64String + padding).replace(/-/g, '+').replace(/_/g, '/');
  const rawData = typeof atob !== 'undefined' ? atob(base64) : Buffer.from(base64, 'base64').toString('binary');
  const outputArray = new Uint8Array(rawData.length);
  for (let i = 0; i < rawData.length; ++i) outputArray[i] = rawData.charCodeAt(i);
  return outputArray;
}

export async function registerPush(): Promise<PushSub> {
  console.log('[registerPush] Iniciando registro de push...');
  const publicKey = import.meta.env.VITE_VAPID_PUBLIC as string;
  if (!publicKey) {
    console.error('[registerPush] VAPID_PUBLIC no está definida');
    throw new Error('VAPID pública ausente: define VITE_VAPID_PUBLIC');
  }
  console.log('[registerPush] VAPID_PUBLIC encontrada:', publicKey.substring(0, 20) + '...');

  if (!('serviceWorker' in navigator) || !('PushManager' in window)) {
    console.error('[registerPush] Service Worker o PushManager no soportados');
    throw new Error('Tu navegador no soporta notificaciones push');
  }
  console.log('[registerPush] Service Worker y PushManager soportados');

  // Register the Service Worker (use the returned registration directly)
  console.log('[registerPush] Registrando Service Worker...');
  const reg = await navigator.serviceWorker.register('/sw.js');
  console.log('[registerPush] Service Worker registrado:', reg);

  console.log('[registerPush] Solicitando permisos de notificación...');
  const perm = await Notification.requestPermission();
  console.log('[registerPush] Permiso obtenido:', perm);
  
  if (perm === 'denied') {
    throw new Error('Permisos de notificación denegados. Ve a la configuración del navegador para permitir notificaciones para este sitio.');
  }
  if (perm !== 'granted') {
    throw new Error('Permisos de notificación no concedidos');
  }

  const appServerKey = urlBase64ToUint8Array(publicKey);
  console.log('[registerPush] VAPID key convertida a Uint8Array');

  // Try to reuse existing subscription, otherwise (re)subscribe
  let sub = await reg.pushManager.getSubscription();
  console.log('[registerPush] Suscripción existente:', sub ? 'Sí' : 'No');
  
  if (!sub) {
    try {
      console.log('[registerPush] Creando nueva suscripción...');
      sub = await reg.pushManager.subscribe({
        userVisibleOnly: true,
        applicationServerKey: appServerKey.buffer as ArrayBuffer,
      });
      console.log('[registerPush] Suscripción creada:', sub);
    } catch (err) {
      console.error('[registerPush] Error al suscribirse:', err);
      // If there's a conflicting old subscription (e.g., VAPID key changed), unsubscribe and retry once
      const existing = await reg.pushManager.getSubscription();
      if (existing) {
        console.log('[registerPush] Intentando desuscribir y re-suscribir...');
        try { await existing.unsubscribe(); } catch (_) {}
      }
      sub = await reg.pushManager.subscribe({
        userVisibleOnly: true,
        applicationServerKey: appServerKey.buffer as ArrayBuffer,
      });
      console.log('[registerPush] Suscripción creada después de retry:', sub);
    }
  }

  // Save to backend and surface errors if any
  console.log('[registerPush] Guardando suscripción en backend...');
  const { error } = await supabase.functions.invoke('push-subscribe', {
    body: { subscription: sub.toJSON() },
  });
  if (error) {
    console.error('[registerPush] Error al guardar en backend:', error);
    throw error;
  }
  console.log('[registerPush] Suscripción guardada exitosamente en backend');

  return sub;
}

export async function unregisterPush(): Promise<boolean> {
  if (!('serviceWorker' in navigator)) return false;

  const reg = await navigator.serviceWorker.ready;
  const sub = await reg.pushManager.getSubscription();

  if (sub) {
    await sub.unsubscribe();
    return true;
  }

  return false;
}

export async function isPushEnabled(): Promise<boolean> {
  if (!('serviceWorker' in navigator) || !('PushManager' in window)) return false;
  if (Notification.permission !== 'granted') return false;

  const reg = await navigator.serviceWorker.ready;
  const sub = await reg.pushManager.getSubscription();

  return !!sub;
}
