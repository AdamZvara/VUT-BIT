using System;
using System.Linq;
using System.Linq.Expressions;
using System.Threading.Tasks;
using AutoMapper;
using AutoMapper.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.DAL.UnitOfWork;
    public class Repository<TEntity> : IRepository<TEntity> where TEntity : class, IEntity
    {
        private readonly DbSet<TEntity> _dbSet;
        private readonly IModel _model;

        public Repository(DbContext dbContext)
        {
            _dbSet = dbContext.Set<TEntity>();
            _model = dbContext.Model;
        }

        public IQueryable<TEntity> Get() => _dbSet;

        public async Task<TEntity> InsertOrUpdateAsync<TModel>(
            TModel model,
            IMapper mapper,
            CancellationToken cancellationToken = default) where TModel : class
        {
            await _dbSet.PreLoadChangeTracker(mapper.Map<TEntity>(model).Id, _model, cancellationToken);

            return await _dbSet.Persist(mapper).InsertOrUpdateAsync(model, cancellationToken);
        }

        public void Delete(Guid IEntity) => _dbSet.Remove(_dbSet.Single(i => i.Id == IEntity));
    }

