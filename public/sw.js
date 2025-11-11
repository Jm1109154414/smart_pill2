/* eslint-disable no-restricted-globals */

self.addEventListener('install', (event) => {
  console.log('Service Worker installing...');
  self.skipWaiting();
});

self.addEventListener('activate', (event) => {
  console.log('Service Worker activating...');
  event.waitUntil(self.clients.claim());
});

self.addEventListener('push', (event) => {
  console.log('Push notification received:', event);

  let data = {};
  try {
    data = event.data ? event.data.json() : {};
  } catch (e) {
    console.error('Failed to parse push data:', e);
  }

  const title = data.title || 'PillMate';
  const options = {
    body: data.body || 'Tienes una notificación',
    icon: data.icon || '/icons/icon-192.png',
    badge: data.badge || '/icons/badge-72.png',
    data: data.data || {},
    actions: [
      { action: 'open', title: 'Abrir' },
      { action: 'snooze', title: 'Posponer 5 min' },
    ],
    requireInteraction: true,
  };

  event.waitUntil(
    self.registration.showNotification(title, options)
  );
});

self.addEventListener('notificationclick', (event) => {
  console.log('Notification clicked:', event.action);
  
  event.notification.close();

  if (event.action === 'snooze') {
    // Send snooze command to backend
    event.waitUntil(
      (async () => {
        try {
          const deviceId = event.notification.data?.deviceId;
          if (!deviceId) {
            console.error('No deviceId in notification data');
            return;
          }

          const supabaseUrl = 'https://cnbjuqvppulnfdxscesr.supabase.co';
          const anonKey = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImNuYmp1cXZwcHVsbmZkeHNjZXNyIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NjI4NjcyNzMsImV4cCI6MjA3ODQ0MzI3M30.B8zOhEccljPGoSiobkyngnw8TI07MHpcA2krgQ3s-L0';

          const response = await fetch(`${supabaseUrl}/functions/v1/commands-create`, {
            method: 'POST',
            headers: {
              'Authorization': `Bearer ${anonKey}`,
              'Content-Type': 'application/json',
            },
            body: JSON.stringify({
              deviceId,
              type: 'snooze',
              payload: { minutes: 5 },
            }),
          });

          if (!response.ok) {
            console.error('Failed to create snooze command:', await response.text());
          } else {
            console.log('Snooze command created successfully');
          }
        } catch (error) {
          console.error('Error creating snooze command:', error);
        }
      })()
    );
  } else {
    // Default action: open app
    const urlToOpen = event.notification.data?.route || '/dashboard';
    
    event.waitUntil(
      self.clients.matchAll({ type: 'window', includeUncontrolled: true })
        .then((clientList) => {
          // Check if there's already a window open
          for (const client of clientList) {
            if (client.url.includes(urlToOpen) && 'focus' in client) {
              return client.focus();
            }
          }
          // If not, open a new window
          if (self.clients.openWindow) {
            return self.clients.openWindow(urlToOpen);
          }
        })
    );
  }
});
