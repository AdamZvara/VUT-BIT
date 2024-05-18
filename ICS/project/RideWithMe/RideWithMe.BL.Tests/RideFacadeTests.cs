using RideWithMe.BL.Tests;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;
using RideWithMe.Common.Tests.Seeds;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Enums;
using RideWithMe.Common.Tests;
using Xunit;
using Xunit.Abstractions;

namespace RideWithMe.BL.Tests
{
    public sealed class RideFacadeTests : CRUDFacadeTestsBase
    {
        private readonly RideFacade _rideFacadeSUT;
        public RideFacadeTests(ITestOutputHelper output) : base(output)
        {
            _rideFacadeSUT = new RideFacade(UnitOfWorkFactory, Mapper);
        }

        [Fact]
        public async Task SaveAsync_NewRide_DoesNotThrow()
        {
            var model = new RideDetailModel
            (
                StartTime: DateTime.Now.AddDays(3),
                EndTime: DateTime.Now.AddDays(3),
                DriverId: UserSeeds.DriverEntity.Id,
                CarId: CarSeeds.CarEntity.Id,
                StartLocationId: AddressSeeds.AddressEntity.Id,
                EndLocationId: AddressSeeds.AddressEntity.Id
            );

            var _ = await _rideFacadeSUT.SaveAsync(model);
        }

        [Fact]
        public async Task Get_SeededRide()
        {
            var ride = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntity.Id);
            Assert.NotNull(ride);
        }

        [Fact]
        public async Task NavProps_All_NotNull()
        {
            var ride = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntity.Id);
            Assert.NotNull(ride!.EndLocation);
            Assert.NotNull(ride!.StartLocation);     
            Assert.NotNull(ride!.Driver);
            Assert.NotNull(ride!.Car);
            Assert.NotNull(ride!.Passengers);
        }
        [Fact]
        public async Task Get_InsertedRide()
        {
            // Arrange
            var newRide = new RideDetailModel
            (
                StartTime: DateTime.Now.AddDays(3),
                EndTime: DateTime.Now.AddDays(3),
                DriverId: UserSeeds.DriverEntity.Id,
                CarId: CarSeeds.CarEntity.Id,
                StartLocationId: AddressSeeds.AddressEntity.Id,
                EndLocationId: AddressSeeds.AddressEntity.Id
            )
            {
                Id = Guid.Parse("A9D5A2D4-6418-4759-9D87-F09F94C6909D")
            };
            // Act
            await _rideFacadeSUT.SaveAsync(newRide);
            // Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var rideFromDb = dbxAssert.Rides.FirstOrDefault(ride => ride.Id == newRide.Id);
            Assert.NotNull(rideFromDb);
            Assert.Equal(newRide.Id, rideFromDb!.Id);
        }
        [Fact]
        public async Task GetAll_GetSingle_RideEntity()
        {
            var allRides = await _rideFacadeSUT.GetAllAsync();
            var oneRide = allRides.Single(i => i.Id == RideSeeds.RideEntity.Id);

            DeepAssert.Equal(Mapper.Map<RideListModel>(RideSeeds.RideEntity), oneRide);
        }
        [Fact]
        public async Task GetById_RideEntity()
        {
            var rideModelDb = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntity.Id);

            var seededRideModel = Mapper.Map<RideDetailModel>(RideSeeds.RideEntity);
            DeepAssert.Equal(seededRideModel, rideModelDb);
        }

        [Fact]
        public async Task GetById_IsNull_NonExistentRide()
        {
            var nonExistingRide = await _rideFacadeSUT.GetAsync(RideSeeds.EmptyRideEntity.Id);

            Assert.Null(nonExistingRide);
        }

        [Fact]
        public async Task GetById_IsNotNull_ExistentRide()
        {
            var legitRide = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntity.Id);

            Assert.NotNull(legitRide);
        }

        [Fact]
        public async Task DeleteById_RideEntityToDelete()
        {
            var rideExist = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntityToDelete.Id);
            Assert.NotNull(rideExist);

            await _rideFacadeSUT.DeleteAsync(rideExist!);

            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var deletedRide = await dbxAssert.Rides.AnyAsync(x => x.Id == RideSeeds.RideEntityToDelete.Id);
            Assert.False(deletedRide);
        }

        [Fact]
        public async Task InsertOrUpdate_RideUpdated_RideEntityToUpdate()
        {
            //Arrange
            var oldRide = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntityToUpdate.Id);
            var newStartTime = DateTime.ParseExact(
                "18/08/2016", "dd/MM/yyyy", CultureInfo.InvariantCulture);
            var newEndTime = DateTime.ParseExact(
                "18/08/2015", "dd/MM/yyyy", CultureInfo.InvariantCulture);
            var updatedRide = oldRide! with
            {
                StartTime = newStartTime,
                EndTime = newEndTime,
            };
            //Act
            updatedRide = await _rideFacadeSUT.SaveAsync(updatedRide);

            //Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var rideFromDb = await dbxAssert.Rides.SingleAsync(i => i.Id == oldRide.Id);
            Assert.Equal(rideFromDb.StartTime, newStartTime );
            Assert.Equal(rideFromDb.EndTime, newEndTime );
        }

        [Fact]
        public async Task GetCount_ChangedAfterAdded()
        {
            // Arrange
            var newRide = new RideDetailModel
            (
                StartTime: DateTime.Now.AddDays(3),
                EndTime: DateTime.Now.AddDays(3),
                DriverId: UserSeeds.DriverEntity.Id,
                CarId: CarSeeds.CarEntity.Id,
                StartLocationId: AddressSeeds.AddressEntity.Id,
                EndLocationId: AddressSeeds.AddressEntity.Id
            )
            {
                Id = Guid.Parse("F2078381-F3DB-4A92-8FD4-6C798F0B6B56")
            };
            int oldCount = await _rideFacadeSUT.GetCount();
            // Act
            await _rideFacadeSUT.SaveAsync(newRide);
            // Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            int newCount = dbxAssert.Rides.Count();
            Assert.Equal(oldCount + 1, newCount);
        }
        [Fact]
        public async Task RemovePassengerFromRide_Remove_First()
        {
            // Arrange
            var ride = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntityWithPassenger.Id);
            Assert.NotNull(ride);
            var oldCount = ride!.Passengers.Count();
            // Act
            await _rideFacadeSUT.RemovePassengerFromRide(ride, ride.Passengers.First());
            // Assert
            var newCount = ride!.Passengers.Count();
            Assert.Equal(oldCount - 1, newCount);
        }
        [Fact]
        public async Task RemovePassengerFromRide_Remove_Passenger1()
        {
            // Arrange
            var ride = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntityWithPassenger.Id);
            var passenger = Mapper.Map<UserModel>(UserSeeds.PassengerEntity);            
            Assert.True(ride!.Passengers?.Any(x => x.Id == UserSeeds.PassengerEntity.Id));

            // Act
            await _rideFacadeSUT.RemovePassengerFromRide(ride!, passenger);
            // Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var rideFromDb = await dbxAssert.Rides.SingleAsync(x => x.Id == RideSeeds.RideEntityWithPassenger.Id);
            Assert.False(rideFromDb!.RidePassengers?.Any(x => x.PassengerId == UserSeeds.PassengerEntity.Id));
        }
        [Fact]
        public async Task AddPassengerToRide_Add()
        {
            // Arrange
            var ride = await _rideFacadeSUT.GetAsync(RideSeeds.RideEntityWithPassenger.Id);
            Assert.NotNull(ride);
            var passenger = Mapper.Map<UserModel>(UserSeeds.PassengerEntityToAdd2);
            var oldCount = ride!.Passengers.Count();
            // Act
            await _rideFacadeSUT.AddPassengerToRide(ride, passenger);
            // Assert
            var newCount = ride!.Passengers.Count();
            Assert.Equal(oldCount + 1, newCount);
        }

        
    }
}
