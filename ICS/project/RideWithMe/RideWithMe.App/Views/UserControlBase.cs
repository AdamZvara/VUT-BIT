﻿using System.Threading.Tasks;
using RideWithMe.App.ViewModels;
using System.Windows;
using System.Windows.Controls;

namespace RideWithMe.App.Views
{
    public abstract class UserControlBase : UserControl
    {
        protected UserControlBase()
        {
            Loaded += OnLoaded;
        }

        private async void OnLoaded(object sender, RoutedEventArgs e)
        {
            if (DataContext is IListViewModel viewModel)
            {
                await viewModel.LoadAsync();
            }
        }
    }
}