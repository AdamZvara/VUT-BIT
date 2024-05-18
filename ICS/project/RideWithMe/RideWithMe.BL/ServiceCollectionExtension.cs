using AutoMapper;
using AutoMapper.EquivalencyExpression;
using RideWithMe.BL.Facades;
using RideWithMe.DAL;
using RideWithMe.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Internal;
using Microsoft.Extensions.DependencyInjection;

namespace RideWithMe.BL;

public static class ServiceCollectionExtension
{
    public static IServiceCollection AddBLServices(this IServiceCollection services)
    {
        services.AddSingleton<IUnitOfWorkFactory, UnitOfWorkFactory>();
        services.AddSingleton<CarFacade>();
        services.AddSingleton<RideFacade>();
        services.AddSingleton<UserFacade>();
        services.AddSingleton<AddressFacade>();

        services.AddAutoMapper((serviceProvider, cfg) =>
        {
            cfg.AddCollectionMappers();

            var dbContextFactory = serviceProvider.GetRequiredService<IDbContextFactory<RideWithMeDbContext>>();
            using var dbContext = dbContextFactory.CreateDbContext();
            cfg.UseEntityFrameworkCoreModel<RideWithMeDbContext>(dbContext.Model);
        }, typeof(BusinessLogic).Assembly);
        return services;
    }
}