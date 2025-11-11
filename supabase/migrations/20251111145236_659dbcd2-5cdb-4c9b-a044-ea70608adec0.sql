-- Create reports bucket in storage
INSERT INTO storage.buckets (id, name, public, file_size_limit, allowed_mime_types)
VALUES (
  'reports',
  'reports',
  false,
  26214400, -- 25MB
  ARRAY['application/pdf']
)
ON CONFLICT (id) DO NOTHING;

-- Enable RLS for reports bucket
CREATE POLICY "Users can read their own reports"
ON storage.objects FOR SELECT
USING (
  bucket_id = 'reports' AND
  auth.uid()::text = (storage.foldername(name))[1]
);

CREATE POLICY "Service role can insert reports"
ON storage.objects FOR INSERT
WITH CHECK (bucket_id = 'reports');

CREATE POLICY "Service role can update reports"
ON storage.objects FOR UPDATE
USING (bucket_id = 'reports');