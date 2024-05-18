﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using AutoMapper;
using AutoMapper.EntityFrameworkCore;
using RideWithMe.BL.Models;
using RideWithMe.DAL;
//using RideWithMe.DAL.Entities;
using RideWithMe.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Facades;

public class CRUDFacade<TEntity, TListModel, TDetailModel>
        where TEntity : class, IEntity
        where TListModel : IModel
        where TDetailModel : class, IModel
{
    protected readonly IMapper _mapper;
    protected readonly IUnitOfWorkFactory _unitOfWorkFactory;

    protected CRUDFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper)
    {
        _unitOfWorkFactory = unitOfWorkFactory;
        _mapper = mapper;
    }

    public async Task DeleteAsync(TDetailModel model) => await this.DeleteAsync(model.Id);

    public async Task DeleteAsync(Guid id)
    {
        await using var uow = _unitOfWorkFactory.Create();
        uow.GetRepository<TEntity>().Delete(id);
        await uow.CommitAsync().ConfigureAwait(false);
    }

    public async Task<TDetailModel?> GetAsync(Guid id)
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<TEntity>()
            .Get()
            .Where(e => e.Id == id);
        return await _mapper.ProjectTo<TDetailModel>(query).SingleOrDefaultAsync().ConfigureAwait(false);
    }

    public async Task<IEnumerable<TListModel>> GetAllAsync()
    {
        await using var uow = _unitOfWorkFactory.Create();
        var query = uow
            .GetRepository<TEntity>()
            .Get();
        return await _mapper.ProjectTo<TListModel>(query).ToArrayAsync().ConfigureAwait(false);
    }

    public async Task<TDetailModel> SaveAsync(TDetailModel model)
    {
        await using var uow = _unitOfWorkFactory.Create();

        var entity = await uow
            .GetRepository<TEntity>()
            .InsertOrUpdateAsync(model, _mapper)
            .ConfigureAwait(false);
        await uow.CommitAsync();

        return (await GetAsync(entity.Id).ConfigureAwait(false))!;
    }

    public async Task<int> GetCount()
    {
        await using var uow = _unitOfWorkFactory.Create();

        var count = uow
            .GetRepository<TEntity>()
            .Get()
            .Count();

        return count;
    }

}