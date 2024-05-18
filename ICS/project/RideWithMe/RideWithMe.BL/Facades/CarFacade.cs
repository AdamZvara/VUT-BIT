using System.Linq;
using AutoMapper;
using Microsoft.EntityFrameworkCore;
using RideWithMe.BL.Models;
using RideWithMe.DAL.UnitOfWork;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Facades;

public class CarFacade : CRUDFacade<CarEntity, CarListModel, CarDetailModel>
{
    public CarFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper) : base(unitOfWorkFactory, mapper)
    {
    }

    public async Task ChangeCarOwner(CarDetailModel car, UserModel newOwner)
        => await ChangeCarOwner(car, newOwner.Id);
    public async Task ChangeCarOwner(CarDetailModel car, Guid ownerId)
    {
        await using var uow = _unitOfWorkFactory.Create();

        if(!uow.GetRepository<UserEntity>().Get().Any(x => x.Id == ownerId))
            return;

        car.OwnerId = ownerId;
        await SaveAsync(car);
        await uow.CommitAsync();
    }

    public async Task<IEnumerable<CarListModel>> GetAllOwnerAsync(Guid ownerId)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<CarEntity>()
            .Get()
            .Where(car => car.OwnerId == ownerId);
        return await _mapper.ProjectTo<CarListModel>(query).ToArrayAsync().ConfigureAwait(false);
    }
}