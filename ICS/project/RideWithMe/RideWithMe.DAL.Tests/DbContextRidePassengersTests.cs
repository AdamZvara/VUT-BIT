using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Tests;
using RideWithMe.Common.Tests.Seeds;
using Xunit;
using Xunit.Abstractions;

namespace RideWithMe.DAL.Tests;

public class DbContextRidePassengersTests : DbContextTestsBase
{
    public DbContextRidePassengersTests(ITestOutputHelper output) : base(output)
    {

    }

    [Fact]
    public async Task GetById_RidePassengers_WithoutCollections()
    {
        var entity = await RideWithMeDbContextSUT.RidePassengers
            .SingleAsync(i => i.Id == RidePassengersSeeds.RidePassengers.Id);

        DeepAssert.Equal(RidePassengersSeeds.RidePassengers with {Passenger = null, Ride = null}, entity);
    }

    [Fact]
    public async Task GetById_RidePassengers_WithCollections()
    {
        var entity = await RideWithMeDbContextSUT.RidePassengers
            .Include(i=>i.Passenger)
            .Include(i=>i.Ride)
            .SingleAsync(i => i.Id == RidePassengersSeeds.RidePassengers.Id);

        Assert.Equal(UserSeeds.PassengerEntity.Id, entity.Passenger?.Id);
        Assert.Equal(RideSeeds.RideEntity.Id, entity.Ride?.Id);
    }

    [Fact]
    public async Task AddNew_RidePassenger_WithoutCollections()
    {
        var entity = RidePassengersSeeds.EmptyRidePassengers with
        {
            RideId = RideSeeds.RideEntity.Id,
            PassengerId = UserSeeds.DriverEntity.Id
        };

        RideWithMeDbContextSUT.RidePassengers.Add(entity);
        await RideWithMeDbContextSUT.SaveChangesAsync();

        await using var dbx = await DbContextFactory.CreateDbContextAsync();
        var actualEntity = await dbx.RidePassengers
            .SingleAsync(i => i.RideId == entity.RideId && i.PassengerId == entity.PassengerId);

        DeepAssert.Equal(entity, actualEntity);
    }

}
 
