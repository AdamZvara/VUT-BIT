using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Globalization;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using RideWithMe.App.Settings;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Options;
using RideWithMe.App.Extensions;
using RideWithMe.App.Services;
using RideWithMe.App.Services.MessageDialog;
using RideWithMe.App.ViewModels;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.ViewModels.MainViewVMs;
using RideWithMe.App.Views;
using RideWithMe.BL;
using RideWithMe.DAL;
using RideWithMe.DAL.Factories;

namespace RideWithMe.App
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private readonly IHost _host;

        public App()
        {
            // Create generic host which will set app configuration and services (singletons and factories)
            _host = Host.CreateDefaultBuilder()
                .ConfigureAppConfiguration(ConfigureAppConfiguration)
                .ConfigureServices((context, services) => { ConfigureServices(context.Configuration, services); })
                .Build();
        }
        private static void ConfigureAppConfiguration(HostBuilderContext context, IConfigurationBuilder builder)
        {
            // Store configuration in AppSettings.json file
            builder.AddJsonFile(@"AppSettings.json", false, false);
        }

        private static void ConfigureServices(IConfiguration configuration,
            IServiceCollection services)
        {
            services.AddBLServices();

            // Load stored AppSettings.json file into DALSettings object
            services.Configure<DALSettings>(configuration.GetSection("RideWithMe:DAL"));

            // Prepare DbContext for the app
            services.AddSingleton<IDbContextFactory<RideWithMeDbContext>>(provider =>
            {
                var dalSettings = provider.GetRequiredService<IOptions<DALSettings>>().Value;
                return new SqlServerDbContextFactory(dalSettings.ConnectionString!, dalSettings.SkipMigrationAndSeedDemoData);
            });

            services.AddSingleton<MainWindow>();

            // Register services
            services.AddSingleton<IMessageDialogService, MessageDialogService>();
            services.AddSingleton<IMediator, Mediator>();
            services.AddSingleton<ILoggedInUser, LoggedInUser>();
            
            services.AddSingleton<MainViewModel>();

            // User selection and registration views
            services.AddSingleton<IUserListViewModel, UserListViewModel>();
            services.AddSingleton<IUserDetailViewModel, UserDetailViewModel>();

            // RideWithMeViewModel and its views
            services.AddSingleton<IRideWithMeViewModel, RideWithMeViewModel>();
            services.AddSingleton<IFilterRidesViewModel, FilterRidesViewModel>();
            services.AddSingleton<IRideListViewModel, RideListViewModel>();
            services.AddSingleton<IRideDetailViewModel, RideDetailViewModel>();
            services.AddSingleton<IUserMenuViewModel, UserMenuViewModel>();
            services.AddSingleton<ICarDetailViewModel, CarDetailViewModel>();
            services.AddSingleton<ICarListViewModel, CarListViewModel>();
            services.AddSingleton<IEditCarViewModel, EditCarViewModel>();

            services.AddFactory<ICreateRideViewModel, CreateRideViewModel>();
            services.AddFactory<IAddressListViewModel, AddressListViewModel>();
            services.AddFactory<IUserDetailViewModel, UserDetailViewModel>();
            services.AddFactory<IAddressDetailViewModel, AddressDetailViewModel>();
        }

        protected override async void OnStartup(StartupEventArgs e)
        {
            // First code to be executed when application starts
            await _host.StartAsync();

            var dbContextFactory = _host.Services.GetRequiredService<IDbContextFactory<RideWithMeDbContext>>();

            var dalSettings = _host.Services.GetRequiredService<IOptions<DALSettings>>().Value;

            // Setup database
            await using (var dbx = await dbContextFactory.CreateDbContextAsync())
            {
                // If we want to skip migrations, seed demo data contained in DAL/Seeds
                if (dalSettings.SkipMigrationAndSeedDemoData)
                {
                    // Delete previously created database
                    await dbx.Database.EnsureDeletedAsync();
                    // Create new database with seeded demo data
                    await dbx.Database.EnsureCreatedAsync();
                }
                else
                {
                    await dbx.Database.MigrateAsync();
                }
            }

            var mainWindow = _host.Services.GetRequiredService<MainWindow>();
            // Blocking call that will run the actual application
            mainWindow.Show();

            base.OnStartup(e);
        }

        protected override async void OnExit(ExitEventArgs e)
        {
            using (_host)
            {
                await _host.StopAsync(TimeSpan.FromSeconds(5));
            }

            base.OnExit(e);
        }
    }
}
