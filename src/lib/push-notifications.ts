import { supabase } from "@/integrations/supabase/client";

const VAPID_PUBLIC_KEY = 'BPxPzQ8kZxRQ0yGKQvZxF8P9mJLHgN2wYvR5K3nB1uT4oC6sV7dE9jW2xA3yL8mN5pK4qR6sT7uV8wX9yZ0aB1c'; // Placeholder - needs to be generated

export async function registerPushNotifications(): Promise<boolean> {
  try {
    // Check if service worker and push notifications are supported
    if (!('serviceWorker' in navigator) || !('PushManager' in window)) {
      console.error('Push notifications are not supported');
      return false;
    }

    // Request notification permission
    const permission = await Notification.requestPermission();
    if (permission !== 'granted') {
      console.log('Notification permission denied');
      return false;
    }

    // Register service worker
    const registration = await navigator.serviceWorker.register('/sw.js');
    await navigator.serviceWorker.ready;

    // Subscribe to push notifications
    const subscription = await registration.pushManager.subscribe({
      userVisibleOnly: true,
      applicationServerKey: urlBase64ToUint8Array(VAPID_PUBLIC_KEY),
    });

    // Get device info
    const deviceInfo = {
      userAgent: navigator.userAgent,
      platform: navigator.platform,
      language: navigator.language,
    };

    // Send subscription to backend
    const { error } = await supabase.functions.invoke('push-subscribe', {
      body: {
        subscription: subscription.toJSON(),
        deviceInfo,
      },
    });

    if (error) {
      console.error('Failed to save subscription:', error);
      return false;
    }

    console.log('Push notifications registered successfully');
    return true;
  } catch (error) {
    console.error('Error registering push notifications:', error);
    return false;
  }
}

export async function unregisterPushNotifications(): Promise<boolean> {
  try {
    if (!('serviceWorker' in navigator)) {
      return false;
    }

    const registration = await navigator.serviceWorker.ready;
    const subscription = await registration.pushManager.getSubscription();

    if (subscription) {
      await subscription.unsubscribe();
      console.log('Push notifications unregistered');
      return true;
    }

    return false;
  } catch (error) {
    console.error('Error unregistering push notifications:', error);
    return false;
  }
}

export async function isPushNotificationEnabled(): Promise<boolean> {
  try {
    if (!('serviceWorker' in navigator) || !('PushManager' in window)) {
      return false;
    }

    const permission = Notification.permission;
    if (permission !== 'granted') {
      return false;
    }

    const registration = await navigator.serviceWorker.ready;
    const subscription = await registration.pushManager.getSubscription();

    return !!subscription;
  } catch (error) {
    console.error('Error checking push notification status:', error);
    return false;
  }
}

function urlBase64ToUint8Array(base64String: string): Uint8Array {
  const padding = '='.repeat((4 - (base64String.length % 4)) % 4);
  const base64 = (base64String + padding)
    .replace(/\-/g, '+')
    .replace(/_/g, '/');

  const rawData = window.atob(base64);
  const outputArray = new Uint8Array(rawData.length);

  for (let i = 0; i < rawData.length; ++i) {
    outputArray[i] = rawData.charCodeAt(i);
  }
  return outputArray;
}
