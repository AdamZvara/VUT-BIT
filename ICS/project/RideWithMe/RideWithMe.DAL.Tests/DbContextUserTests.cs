using System;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Tests;
using RideWithMe.Common.Tests.Seeds;
using RideWithMe.DAL.Entities;
using Xunit;
using Xunit.Abstractions;

namespace RideWithMe.DAL.Tests;

public class DbContextUserTests : DbContextTestsBase
{
    public DbContextUserTests(ITestOutputHelper output) : base(output)
    {
        
    }

    [Fact]
    public async Task GetById_User_WithoutCollections()
    {
        var entity = await RideWithMeDbContextSUT.Users
            .SingleAsync(i => i.Id == UserSeeds.DriverEntity.Id);

        DeepAssert.Equal(UserSeeds.DriverEntityWithoutCollections, entity);
    }

    [Fact]
    public async Task GetById_Driver_IncludeCollections()
    {
        var entity = await RideWithMeDbContextSUT.Users
            .Include(i=>i.Cars)
            .Include(i=>i.DriverRides)
            .SingleAsync(i => i.Id == UserSeeds.DriverEntity.Id);

        Assert.Contains(CarSeeds.CarEntity.Id, entity.Cars.ToList().Select(x => x.Id));
        Assert.Contains(RideSeeds.RideEntity.Id, entity.DriverRides.ToList().Select(x => x.Id));

    }

    [Fact]
    public async Task GetById_Passenger_IncludeCollections()
    {
        var entity = await RideWithMeDbContextSUT.Users
            .Include(i => i.RidePassengers)
            .SingleAsync(i => i.Id == UserSeeds.PassengerEntity.Id);

        Assert.Contains(RidePassengersSeeds.RidePassengers.Id, entity.RidePassengers.ToList().Select(x => x.Id));
    }

    [Fact]
    public async Task GetById_MissingUser()
    {
        await Assert.ThrowsAsync<InvalidOperationException>(() => RideWithMeDbContextSUT.Users
            .SingleAsync(i => i.Id == UserSeeds.EmptyUserEntity.Id));
    }

    [Fact]
    public async Task AddNew_User_WithoutCollections()
    {
        // Arrange
        var entity = UserSeeds.EmptyUserEntity with
        {
            FirstName = "Janka",
            LastName = "Stastna",
            Cars = Array.Empty<CarEntity>()
        };

        // Act
        RideWithMeDbContextSUT.Users.Add(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Users
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
        Assert.NotEqual(entity.FirstName, UserSeeds.DriverEntity.FirstName);
    }

    [Fact]
    public async Task Delete_User()
    {
        // Arrange
        var baseEntity = UserSeeds.UserEntityDelete;

        // Act
        RideWithMeDbContextSUT.Users.Remove(baseEntity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        Assert.False(await RideWithMeDbContextSUT.Users.AnyAsync(i => i.Id == baseEntity.Id));
    }

    [Fact]
    public async Task DeleteById_User()
    {
        // Arrange
        var baseEntity = UserSeeds.UserEntityDelete;

        // Act
        RideWithMeDbContextSUT.Users.Remove(
            RideWithMeDbContextSUT.Users.Single(i=>i.Id == baseEntity.Id));
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        Assert.False(await RideWithMeDbContextSUT.Users.AnyAsync(i => i.Id == baseEntity.Id));
    }

    [Fact]
    public async Task Update_User()
    {
        // Arrange
        var entity = UserSeeds.UserEntityUpdate with
        {
            FirstName = UserSeeds.DriverEntityWithoutCollections.FirstName + "Updated",
            LastName = UserSeeds.DriverEntityWithoutCollections.LastName + "Updated"
        };

        // Act
        RideWithMeDbContextSUT.Update(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Users
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
    }
}