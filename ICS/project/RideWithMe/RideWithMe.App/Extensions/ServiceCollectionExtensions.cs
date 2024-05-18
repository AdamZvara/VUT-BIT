using System;
using Microsoft.Extensions.DependencyInjection;
using RideWithMe.App.Factories;

namespace RideWithMe.App.Extensions
{
    public static class ServiceCollectionExtensions
    {
        public static void AddFactory<TService, TImplementation>(this IServiceCollection services)
            where TService : class
            where TImplementation : class, TService
        {
            services.AddTransient<TService, TImplementation>();

            services.AddSingleton<Func<TService>>(x => x.GetRequiredService<TService>);

            services.AddSingleton<IFactory<TService>, Factory<TService>>();
        }
    }
}