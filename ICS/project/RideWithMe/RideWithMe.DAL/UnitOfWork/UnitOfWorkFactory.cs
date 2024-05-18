using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL;

namespace RideWithMe.DAL.UnitOfWork;

public class UnitOfWorkFactory : IUnitOfWorkFactory
{
    private readonly IDbContextFactory<RideWithMeDbContext> _dbContextFactory;

    public UnitOfWorkFactory(IDbContextFactory<RideWithMeDbContext> dbContextFactory)
    {
        _dbContextFactory = dbContextFactory;
    }
    public IUnitOfWork Create() => new UnitOfWork(_dbContextFactory.CreateDbContext());
}