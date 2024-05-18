using System.Globalization;
using System.Linq;
using System.Linq.Expressions;
using System.Security.Cryptography.X509Certificates;
using AutoMapper;
using Microsoft.EntityFrameworkCore;
using RideWithMe.BL.Models;
using RideWithMe.Common.Enums;
using RideWithMe.DAL;
using RideWithMe.DAL.UnitOfWork;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Facades;

public class RideFacade : CRUDFacade<RideEntity, RideListModel, RideDetailModel>
{
    public RideFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper) : base(unitOfWorkFactory, mapper)
    {
    }
    public async Task AddPassengerToRide(RideDetailModel ride,  UserModel passenger)
    {
        await using var uow = _unitOfWorkFactory.Create();

        if (!uow.GetRepository<UserEntity>().Get().Any(x => x.Id == passenger.Id) &&
            !uow.GetRepository<RideEntity>().Get().Any(x => x.Id == ride.Id))
            throw new InvalidOperationException("Passenger or ride doesn't exist");

        ride.Passengers.Add(passenger);
        await SaveAsync(ride);
        await uow.CommitAsync();
    }

    public async Task RemovePassengerFromRide(RideDetailModel ride, UserModel passenger)
        => await RemovePassengerFromRide(ride, passenger.Id);
    public async Task RemovePassengerFromRide(RideDetailModel ride, Guid passengerId)
    {
        await using var uow = _unitOfWorkFactory.Create();

        if (!uow.GetRepository<RideEntity>().Get().Any(x => x.Id == ride.Id))
        {
            throw new InvalidOperationException("Ride doesn't exist.");
        }


        var passengerToDelete = ride.Passengers?.FirstOrDefault(x => x.Id == passengerId);
        if (passengerToDelete == null) return;

        ride.Passengers?.Remove(passengerToDelete);
        await SaveAsync(ride);
        await uow.CommitAsync();
    }

    public async Task<IEnumerable<RideListModel>> GetFilteredAsync(Expression<Func<RideEntity, bool>> filter)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<RideEntity>()
            .Get()
            .Where(filter);
        return await _mapper.ProjectTo<RideListModel>(query).ToArrayAsync().ConfigureAwait(false);
    }

    public Expression<Func<RideEntity, bool>> GetDriverFilter(string name)
    {
        return cl1 => cl1.Driver!.FirstName == name;
    }

    public Expression<Func<RideEntity, bool>> GetDateFilter(string date)
    {
        DateTime actualDate;
        if (DateTime.TryParse(date, CultureInfo.InvariantCulture, DateTimeStyles.None, out actualDate))
            return cl1 => cl1.StartTime.Date == actualDate.Date;
        return cl1 => false;
    }

    public Expression<Func<RideEntity, bool>> GetStartLocationFilter(string city)
    {
        return cl1 => cl1.StartLocation!.City == city;
    }

    public Expression<Func<RideEntity, bool>> GetEndLocationFilter(string city)
    {
        return cl1 => cl1.EndLocation!.City == city;
    }
}