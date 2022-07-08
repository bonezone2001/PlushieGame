using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace UI.HUD
{
    class CountdownConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            int countdown = (int)value;
            if (countdown > 0)
                return countdown.ToString();
            return "START";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}