using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace RideWithMe.App.Converters;

public class StringToIntConverter : IValueConverter
{
    public object Convert(object? value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is int intValue)
        {
            if (intValue == 0)
                return "";
        }

        return value.ToString();
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
        if (value is string stringValue)
        {
            if (string.IsNullOrEmpty(stringValue))
                return 0;
            
            if(Int32.TryParse(stringValue, out int xxx))
                return xxx;
            return 0;
        }
        return 0;
    }
}