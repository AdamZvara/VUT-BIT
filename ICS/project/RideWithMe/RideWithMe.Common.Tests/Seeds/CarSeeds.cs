using System;
using System.Globalization;
using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Enums;
using RideWithMe.DAL.Entities;

namespace RideWithMe.Common.Tests.Seeds;

public static class CarSeeds
{
    public static readonly CarEntity EmptyCarEntity = new(
        Id: default,
        RegistrationDate: default,
        ImageUrl: default,
        OwnerId: default,
        CarTypeId: default,
        BrandId: default,
        Seats: default
    );

    public static readonly CarEntity CarEntity = new(
        Id: Guid.Parse("9A6F3CF1-B512-4660-854D-D62703D57A10"),
        RegistrationDate: DateTime.ParseExact(
            "18/08/2018", "dd/MM/yyyy", CultureInfo.InvariantCulture),
        ImageUrl: null,
        OwnerId: Guid.Parse("3D90E905-8C83-471A-9E71-CC13182B49CC"),
        CarTypeId: CarTypeId.Sedan,
        BrandId: CarBrandId.Audi,
        Seats: 3
    )
    {
        Owner = UserSeeds.DriverEntity
    };

    // DAL tests
    public static readonly CarEntity CarEntityWithoutCollections = CarEntity with { Owner = null, Rides = Array.Empty<RideEntity>() };
    public static readonly CarEntity CarEntityUpdate = CarEntity with { Id = Guid.Parse("DB456F9E-8E33-437D-B8F1-455A04FE0913"), Owner = null, Rides = Array.Empty<RideEntity>() };
    public static readonly CarEntity CarEntityDelete = CarEntity with { Id = Guid.Parse("D642E3F6-B52D-41F5-A947-A032D4560492"), Owner = null, Rides = Array.Empty<RideEntity>() };
    // BL tests
    public static readonly CarEntity CarEntityToDelete = CarEntity with { Id = Guid.Parse("D01D8CBD-A9B0-41DF-84FF-5C9B92FAE901"), Owner = null, Rides = Array.Empty<RideEntity>() };
    public static readonly CarEntity CarEntityToUpdate = CarEntity with { Id = Guid.Parse("DD94C96A-785F-4DD7-8AD2-987E136EEC95"), Owner = null, Rides = Array.Empty<RideEntity>() };
    public static readonly CarEntity CarWithNoOwner = CarEntity with
    {
        Id = Guid.Parse("07B4B018-06D5-42A8-B86D-7354B0050FA9"),
        Owner = null,
        OwnerId = Guid.Parse("83130753-8E00-483A-A16A-2B53A5B6BE34"),
        Rides = Array.Empty<RideEntity>()
    };    
    public static readonly CarEntity CarWithLegitOwner = CarEntity with
    {
        Id = Guid.Parse("07B4B018-06D5-42A8-B86D-7354B0050FA9"),
        Owner = null,
        OwnerId = Guid.Parse("83130753-8E00-483A-A16A-2B53A5B6BE34"),
        Rides = Array.Empty<RideEntity>()
    };

    // Add Owner and Rides entities connected to address
    static CarSeeds()
    {
        CarEntity.Rides.Add(RideSeeds.RideEntity);
    }

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<CarEntity>().HasData(
            CarEntityWithoutCollections,
            CarEntityUpdate,
            CarEntityDelete, 
            CarEntityToDelete,
            CarWithNoOwner
        );
    }

    private static CarEntity DeleteNavigationProps(CarEntity e)
    {
        return e with
        {
            Owner = null, Rides = Array.Empty<RideEntity>() 
        };
    }
}