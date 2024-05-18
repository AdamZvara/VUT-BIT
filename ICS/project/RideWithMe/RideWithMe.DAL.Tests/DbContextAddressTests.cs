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

public class DbContextAddressTests : DbContextTestsBase
{
    public DbContextAddressTests(ITestOutputHelper output) : base(output)
    {
        
    }

    [Fact]
    public async Task GetById_Address()
    {
        var entity = await RideWithMeDbContextSUT.Addresses
            .SingleAsync(i => i.Id == AddressSeeds.AddressEntity.Id);

        DeepAssert.Equal(AddressSeeds.AddressEntityWithoutCollections, entity);
    }

    [Fact]
    public async Task GetById_AddressIncludeCollections()
    {
        var entity = await RideWithMeDbContextSUT.Addresses
            .Include(i=>i.StartOfRides)
            .Include(i=>i.EndOfRides)
            .SingleAsync(i => i.Id == AddressSeeds.AddressEntity.Id);
        
        Assert.Contains(RideSeeds.RideEntity.Id, entity.StartOfRides.ToList().Select(x => x.Id));
        Assert.Contains(RideSeeds.RideEntity.Id, entity.EndOfRides.ToList().Select(x => x.Id));
    }

    [Fact]
    public async Task AddNew_Address_WithoutCollections()
    {
        // Arrange
        var entity = AddressSeeds.EmptyAddressEntity with
        {
            State = "Slovakia",
            City = "Bratislava",
            Street = "Masarykova",
            StartOfRides = Array.Empty<RideEntity>(),
            EndOfRides = Array.Empty<RideEntity>()
        };

        // Act
        RideWithMeDbContextSUT.Addresses.Add(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Addresses
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
        Assert.NotEqual(entity.State, AddressSeeds.AddressEntity.State);
    }

    [Fact]
    public async Task DeleteById_Address()
    {
        // Arrange
        var baseEntity = AddressSeeds.AddressEntityDelete;

        // Act
        RideWithMeDbContextSUT.Addresses.Remove(
            RideWithMeDbContextSUT.Addresses.Single(i => i.Id == baseEntity.Id));
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        Assert.False(await RideWithMeDbContextSUT.Addresses.AnyAsync(i => i.Id == baseEntity.Id));
    }

    [Fact]
    public async Task Delete_Address()
    {
        // Arrange
        var baseEntity = AddressSeeds.AddressEntityDelete;

        // Act
        RideWithMeDbContextSUT.Addresses.Remove(baseEntity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        Assert.False(await RideWithMeDbContextSUT.Addresses.AnyAsync(i => i.Id == baseEntity.Id));
    }

    [Fact]
    public async Task Update_Address()
    {
        // Arrange
        var entity = AddressSeeds.AddressEntityUpdate with
        {
            State = AddressSeeds.AddressEntity.State + "Updated"
        };

        // Act
        RideWithMeDbContextSUT.Update(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        // Assert
        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.Addresses
            .SingleAsync(i => i.Id == entity.Id);

        DeepAssert.Equal(entity, actualEntity);
    }
}