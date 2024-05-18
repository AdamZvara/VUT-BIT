using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using RideWithMe.App.Enums;

namespace RideWithMe.App.Views.MainViewViews
{
    /// <summary>
    /// Interakční logika pro AddressListView.xaml
    /// </summary>
    public partial class AddressListView 
    {
        public AddressListView()
        {
            InitializeComponent();
        }


        public string AddressTypeDescription
        {
            get { return (string)GetValue(AddressTypeDescriptionProperty); }
            set { SetValue(AddressTypeDescriptionProperty, value); }
        }

        public static readonly DependencyProperty AddressTypeDescriptionProperty = DependencyProperty.Register(
            "AddressTypeDescription", typeof(string), typeof(AddressListView), new PropertyMetadata(default));
    }

}
