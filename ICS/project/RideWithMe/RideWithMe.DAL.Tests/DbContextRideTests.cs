using System;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Tests;
using RideWithMe.Common.Tests.Seeds;
using Xunit;
using Xunit.Abstractions;


namespace RideWithMe.DAL.Tests;

public class DbContextRideTests : DbContextTestsBase
{
    public DbContextRideTests(ITestOutputHelper output) : base(output)
    {
        
    }

    [Fact]
    public async Task GetById_Ride_WithoutCollections()
    {
        var entity = await RideWithMeDbContextSUT.Rides
            .SingleAsync(i => i.Id == RideSeeds.RideEntity.Id);

        DeepAssert.Equal(RideSeeds.RideEntityWithoutCollections, entity);
    }

    [Fact]
    public async Task GetById_Ride_WithCollections()
    {
        var entity = await RideWithMeDbContextSUT.Rides
            .Include(i=>i.Car)
            .Include(i=>i.Driver)
            .Include(i=>i.EndLocation)
            .Include(i=>i.StartLocation)
            .Include(i=>i.RidePassengers)
            .SingleAsync(i => i.Id == RideSeeds.RideEntity.Id);


        Assert.Equal(CarSeeds.CarEntity.Id, entity.Car?.Id);
        Assert.Equal(UserSeeds.DriverEntity.Id, entity.Driver?.Id);
        Assert.Equal(AddressSeeds.AddressEntity.Id, entity.EndLocation?.Id);
        Assert.Equal(AddressSeeds.AddressEntity.Id, entity.StartLocation?.Id);
        Assert.Contains(RidePassengersSeeds.RidePassengers.Id, entity.RidePassengers.ToList().Select(i=>i.Id));
    }

    [Fact]
    public async Task AddNew_Ride_WithoutCollections()
    {
        // Arrange
        var entity = RideSeeds.EmptyRideEntity with
        {
            StartLocationId = AddressSeeds.AddressEntity.Id,
            EndLocationId = AddressSeeds.AddressEntity.Id,
            CarId = CarSeeds.CarEntity.Id,
            DriverId = UserSeeds.DriverEntity.Id
        };

        // Act
        RideWithMeDbContextSUT.Rides.Add(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Rides
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
        Assert.NotEqual(entity.StartTime, RideSeeds.RideEntity.StartTime);
    }

    [Fact]
    public async Task AddNew_Ride_WithAddresses()
    {
        // Arrange
        var entity = RideSeeds.EmptyRideEntity with
        {
            CarId = CarSeeds.CarEntity.Id,
            DriverId = UserSeeds.DriverEntity.Id,
            StartLocation = AddressSeeds.EmptyAddressEntity with
            {
                State = "Slovakia",
                City = "Presov",
                Street = "Random Street 1"
            },
            EndLocation = AddressSeeds.EmptyAddressEntity with
            {
                State = "Hungary",
                City = "Budapest",
                Street = "Random Street 2"
            }
        };

        // Act
        RideWithMeDbContextSUT.Rides.Add(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Rides
            .Include(i=>i.StartLocation)
            .Include(i=>i.EndLocation)
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
    }

    [Fact]
    public async Task Delete_Ride()
    {
        // Arrange
        var baseEntity = RideSeeds.RideEntityDelete;

        // Act
        RideWithMeDbContextSUT.Rides.Remove(baseEntity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        Assert.False(await RideWithMeDbContextSUT.Users.AnyAsync(i => i.Id == baseEntity.Id));
    }

    [Fact]
    public async Task Update_Ride()
    {
        // Arrange
        var entity = RideSeeds.RideEntityUpdate with
        {
            StartTime = DateTime.MaxValue,
            EndTime = DateTime.MaxValue
        };

        // Act
        RideWithMeDbContextSUT.Update(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Rides
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
    }
}