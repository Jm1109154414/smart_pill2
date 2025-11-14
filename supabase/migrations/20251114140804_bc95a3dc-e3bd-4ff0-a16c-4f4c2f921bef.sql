-- Añadir campo servo_angle_deg a compartments para controlar ángulo del servomotor
ALTER TABLE public.compartments 
ADD COLUMN servo_angle_deg smallint CHECK (servo_angle_deg >= 0 AND servo_angle_deg <= 180);