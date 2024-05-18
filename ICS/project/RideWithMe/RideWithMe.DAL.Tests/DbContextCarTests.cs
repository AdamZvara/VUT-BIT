using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Enums;
using RideWithMe.Common.Tests;
using RideWithMe.Common.Tests.Seeds;
using Xunit;
using Xunit.Abstractions;

namespace RideWithMe.DAL.Tests;

public class DbContextCarTests : DbContextTestsBase
{
    public DbContextCarTests(ITestOutputHelper output) : base(output)
    {

    }

    [Fact]
    public async Task GetById_Car_WithoutCollections()
    {
        var entity = await RideWithMeDbContextSUT.Cars
            .SingleAsync(i => i.Id == CarSeeds.CarEntity.Id);

        DeepAssert.Equal(CarSeeds.CarEntityWithoutCollections, entity);
    }

    [Fact]
    public async Task GetById_Car_IncludeCollections()
    {
        var entity = await RideWithMeDbContextSUT.Cars
            .Include(i=>i.Owner)
            .Include(i=>i.Rides)
            .SingleAsync(i => i.Id == CarSeeds.CarEntity.Id);

        Assert.Equal(UserSeeds.DriverEntity.Id, entity.Owner?.Id);
        Assert.Contains(RideSeeds.RideEntity.Id, entity.Rides.ToList().Select(i=>i.Id));
    }

    [Fact]
    public async Task AddNew_Car_WithoutCollections()
    {
        // Arrange
        var entity = CarSeeds.EmptyCarEntity with
        {
            CarTypeId = CarTypeId.Cabriolet,
            BrandId = CarBrandId.AstonMartin,
            OwnerId = UserSeeds.DriverEntity.Id
        };

        // Act
        RideWithMeDbContextSUT.Cars.Add(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Cars
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
        Assert.NotEqual(entity.CarTypeId, CarSeeds.CarEntity.CarTypeId);
    }

    [Fact]
    public async Task Delete_Car()
    {
        // Arrange
        var baseEntity = CarSeeds.CarEntityDelete;

        // Act
        RideWithMeDbContextSUT.Cars.Remove(baseEntity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        Assert.False(await RideWithMeDbContextSUT.Users.AnyAsync(i => i.Id == baseEntity.Id));
    }

    [Fact]
    public async Task DeleteById_Car()
    {
        // Arrange
        var baseEntity = CarSeeds.CarEntityDelete;

        // Act
        RideWithMeDbContextSUT.Cars.Remove(
            RideWithMeDbContextSUT.Cars.Single(i => i.Id == baseEntity.Id));
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        Assert.False(await RideWithMeDbContextSUT.Users.AnyAsync(i => i.Id == baseEntity.Id));
    }

    [Fact]
    public async Task Update_Car()
    {
        // Arrange
        var entity = CarSeeds.CarEntityUpdate with
        {
            CarTypeId = CarTypeId.Convertible
        };

        // Act
        RideWithMeDbContextSUT.Update(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Cars
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
    }
}