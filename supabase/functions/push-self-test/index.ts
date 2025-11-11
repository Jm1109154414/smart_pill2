import { serve } from "https://deno.land/std@0.168.0/http/server.ts";
import { createClient } from "https://esm.sh/@supabase/supabase-js@2";
import webpush from "https://esm.sh/web-push@3.6.7";

const corsHeaders = {
  'Access-Control-Allow-Origin': '*',
  'Access-Control-Allow-Headers': 'authorization, x-client-info, apikey, content-type',
};

serve(async (req) => {
  if (req.method === 'OPTIONS') {
    return new Response(null, { headers: corsHeaders });
  }

  try {
    const supabaseUrl = Deno.env.get('SUPABASE_URL')!;
    const supabaseServiceKey = Deno.env.get('SUPABASE_SERVICE_ROLE_KEY')!;
    const supabase = createClient(supabaseUrl, supabaseServiceKey);

    // Authenticate user
    const authHeader = req.headers.get('Authorization');
    if (!authHeader) {
      return new Response(
        JSON.stringify({ error: 'missing_auth' }),
        { status: 401, headers: { ...corsHeaders, 'Content-Type': 'application/json' } }
      );
    }

    const { data: { user }, error: authError } = await supabase.auth.getUser(
      authHeader.replace('Bearer ', '')
    );

    if (authError || !user) {
      return new Response(
        JSON.stringify({ error: 'invalid_auth' }),
        { status: 401, headers: { ...corsHeaders, 'Content-Type': 'application/json' } }
      );
    }

    // Parse body
    const body = await req.json().catch(() => ({}));
    const title = body.title || 'Prueba de notificaciones';
    const message = body.body || 'Si ves esto, tu VAPID y Service Worker están correctos.';

    // Get VAPID keys
    const vapidPublic = Deno.env.get('VAPID_PUBLIC');
    const vapidPrivate = Deno.env.get('VAPID_PRIVATE');

    if (!vapidPublic || !vapidPrivate) {
      return new Response(
        JSON.stringify({ error: 'vapid_not_configured' }),
        { status: 500, headers: { ...corsHeaders, 'Content-Type': 'application/json' } }
      );
    }

    webpush.setVapidDetails(
      'mailto:noreply@pillmate.app',
      vapidPublic,
      vapidPrivate
    );

    // Get user's push subscriptions
    const { data: subs, error: subsError } = await supabase
      .from('push_subscriptions')
      .select('*')
      .eq('user_id', user.id);

    if (subsError || !subs || subs.length === 0) {
      return new Response(
        JSON.stringify({ error: 'no_subscriptions', sent: 0, removed: 0 }),
        { status: 200, headers: { ...corsHeaders, 'Content-Type': 'application/json' } }
      );
    }

    const payload = JSON.stringify({
      title,
      body: message,
      icon: '/icon-192.png',
      badge: '/badge.png',
    });

    let sent = 0;
    let removed = 0;

    for (const sub of subs) {
      const pushSub = {
        endpoint: sub.endpoint,
        keys: {
          auth: sub.auth,
          p256dh: sub.p256dh,
        },
      };

      try {
        await webpush.sendNotification(pushSub, payload);
        sent++;
      } catch (err: any) {
        console.error('Push send error:', err);
        const statusCode = err?.statusCode || 0;
        if (statusCode === 404 || statusCode === 410) {
          await supabase
            .from('push_subscriptions')
            .delete()
            .eq('id', sub.id);
          removed++;
        }
      }
    }

    return new Response(
      JSON.stringify({ sent, removed }),
      {
        headers: { ...corsHeaders, 'Content-Type': 'application/json' },
      }
    );
  } catch (error: any) {
    console.error('Self-test error:', error);
    return new Response(
      JSON.stringify({ error: error.message }),
      {
        status: 500,
        headers: { ...corsHeaders, 'Content-Type': 'application/json' },
      }
    );
  }
});
