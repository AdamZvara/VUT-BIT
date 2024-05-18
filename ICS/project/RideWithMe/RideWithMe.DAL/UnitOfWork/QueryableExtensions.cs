using System;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using RideWithMe.DAL;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata;
using RideWithMe.DAL.Entities;

namespace RideWithMe.DAL.UnitOfWork;

public static class QueryableExtensions
{
    public static async Task PreLoadChangeTracker<TEntity>(this IQueryable<TEntity> dbSet, Guid IEntity, IModel model, CancellationToken cancellationToken) where TEntity : class, IEntity
        => await dbSet
            .IncludeFirstLevelNavigationProperties(model)
            .Where(e => e.Id == IEntity)
            .FirstOrDefaultAsync(cancellationToken)
            .ConfigureAwait(false);

    public static IQueryable<TEntity> IncludeFirstLevelNavigationProperties<TEntity>(this IQueryable<TEntity> query, Microsoft.EntityFrameworkCore.Metadata.IModel model) where TEntity : class
    {
        var navigationProperties = model.FindEntityType(typeof(TEntity))?.GetNavigations();
        if (navigationProperties == null)
            return query;

        foreach (var navigationProperty in navigationProperties)
        {
            query = query.Include(navigationProperty.Name);
        }

        return query;
    }
}