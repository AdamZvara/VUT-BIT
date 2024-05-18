using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Linq.Expressions;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.EntityFrameworkCore.Query.SqlExpressions;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Enums;
using RideWithMe.App.Extensions;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Views;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;
using RideWithMe.DAL.Entities;

namespace RideWithMe.App.ViewModels.MainViewVMs;

public class RideListViewModel : ViewModelBase, IRideListViewModel
{
    private readonly RideFacade _rideFacade;
    private readonly IMediator _mediator;

    public RideListViewModel(
        RideFacade rideFacade,
        ICreateRideViewModel createRideViewModel,
        IFilterRidesViewModel filterViewModel,
        IMediator mediator)
    {
        _rideFacade = rideFacade;
        _mediator = mediator;

        CreateRideViewModel = createRideViewModel;
        FilterViewModel = filterViewModel;

        RideSelectedCommand = new RelayCommand<RideListModel>(RideSelected);
        NewRideCommand = new RelayCommand(OpenCreateRideView);

        _mediator.Register<UpdateMessage<RideWrapper>>(OnRideUpdated);
        _mediator.Register<RefreshMessage<RideWrapper>>(OnAppRefresh);
        _mediator.Register<UnselectMessage<RideWrapper>>(UnselectRide);
        
    }

    private void UnselectRide(UnselectMessage<RideWrapper> _)
    {
        SelectedRideIndex = -1;
    }

    public ObservableCollection<RideListModel> Rides { get; } = new();

    private ICreateRideViewModel CreateRideViewModel { get; }
    private IFilterRidesViewModel FilterViewModel { get; }

    public ICommand RideSelectedCommand { get; }
    public ICommand NewRideCommand { get; }

    public int SelectedRideIndex { get; set; } = -1;

    private void RideSelected(RideListModel? ride)
    {
        if (ride == null) return;
        _mediator.Send(new SelectedMessage<RideWrapper> { Id = ride?.Id });
    }

    

    private void OpenCreateRideView()
    {
        CreateRideViewModel.LoadAsync(Guid.Empty);
        _mediator.Send(new OpenCreateRideDetailMessage<RideWrapper>());
    }

    private async void OnRideUpdated(UpdateMessage<RideWrapper> _)
    {
        await LoadAsync();
    }
    private async void OnAppRefresh(RefreshMessage<RideWrapper> _)
    {
        await LoadAsync();
    }

    public async Task LoadAsync()
    {
        Rides.Clear();
        Expression<Func<RideEntity, bool>>? filterExpression = null;
        IEnumerable<RideListModel> rides;

        foreach (var filter in FilterViewModel.FilterModels)
        {
            if (filter.Checked)
                filterExpression = filter.Name switch
                {
                    FilterOption.Driver => _rideFacade.GetDriverFilter(filter.Value),
                    FilterOption.Date => _rideFacade.GetDateFilter(filter.Value),
                    FilterOption.StartLocation => _rideFacade.GetStartLocationFilter(filter.Value),
                    FilterOption.EndLocation => _rideFacade.GetEndLocationFilter(filter.Value),
                    _ => filterExpression
                };
        }

        if (filterExpression != null)
            rides = await _rideFacade.GetFilteredAsync(filterExpression);
        else
            rides = await _rideFacade.GetAllAsync();

        Rides.AddRange(rides);
    }


}