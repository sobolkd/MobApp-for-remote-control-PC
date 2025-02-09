using Microsoft.Maui.Controls;
using Microsoft.Maui.Graphics;

namespace RemoteGod.Controls
{
    public class JoystickView : GraphicsView
    {
        public event EventHandler<(double X, double Y)>? Moved;

        private readonly JoystickDrawable _drawable;
        private double _radius = 100;
        private double _thumbX = 0, _thumbY = 0;

        public JoystickView()
        {
            _drawable = new JoystickDrawable(this);
            Drawable = _drawable;
            WidthRequest = HeightRequest = _radius * 2;
            BackgroundColor = Colors.Transparent;

            var panGesture = new PanGestureRecognizer();
            panGesture.PanUpdated += OnPanUpdated;
            GestureRecognizers.Add(panGesture);
        }

        private void OnPanUpdated(object sender, PanUpdatedEventArgs e)
        {
            if (e.StatusType == GestureStatus.Running)
            {
                _thumbX = Math.Clamp(e.TotalX, -_radius, _radius);
                _thumbY = Math.Clamp(e.TotalY, -_radius, _radius);
                Moved?.Invoke(this, (_thumbX / _radius, _thumbY / _radius));
                Invalidate();
            }
            else if (e.StatusType == GestureStatus.Completed)
            {
                _thumbX = _thumbY = 0;
                Moved?.Invoke(this, (0, 0));
                Invalidate();
            }
        }

        private class JoystickDrawable : IDrawable
        {
            private readonly JoystickView _joystick;

            public JoystickDrawable(JoystickView joystick) => _joystick = joystick;

            public void Draw(ICanvas canvas, RectF dirtyRect)
            {
                double centerX = dirtyRect.Center.X;
                double centerY = dirtyRect.Center.Y;

                canvas.FillColor = Colors.DarkGray;
                canvas.FillCircle((float)centerX, (float)centerY, (float)_joystick._radius);

                canvas.FillColor = Colors.Blue;
                canvas.FillCircle((float)(centerX + _joystick._thumbX),
                                  (float)(centerY + _joystick._thumbY),
                                  (float)_joystick._radius / 4);
            }
        }
    }
}
